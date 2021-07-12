#include "Section.h"

string Section::prev_loudness;

Section::Section(TimeSignature time_signature) : 
    time_signature_(time_signature) {
  prev_loudness = "";
}

Section::~Section() {
  for (vector<Note*>* bar : section_){
      for (Note* note : *bar) {
        delete note;
      }
      bar->clear();
      delete bar;
  }
  section_.clear();
  section_flat_.clear();
}

bool Section::InsertNote(Note* n) {
  cout << "foreign address: " << n->rootExactAncestor << endl;
  cout << "local address: " << time_signature_.tempo_.getRootExactAncestor() << endl;
  if (n->rootExactAncestor != time_signature_.tempo_.getRootExactAncestor()) {
    return false; // Note does not belong to this section.
  }

  cout << "Note start (edus): " << n->start_t << endl;
  cout << "Note end (edus): " << n->end_t << endl;

  n->type_out = "";
  is_built_ = false;

  EnsureNoteExpressible(n);

  if (n->end_t <= n->start_t ) {
	  return true; // Discard invalid note
  }

  int bar_num = n->start_t / time_signature_.bar_edus_;

  if (bar_num >= section_.size()) {
    ResizeSection(bar_num);
  }

  int end = n->end_t / time_signature_.bar_edus_;
  // if note is completely inside the bar, insert it
  if(end <= bar_num) {
    vector<Note*>::iterator iter;
    for (iter = section_[bar_num]->begin(); 
         iter != section_[bar_num]->end(); 
         iter++) {
      Note* cur = *iter;
      if (cur->start_t > n->start_t) {
        section_[bar_num]->insert(iter, n);
        return true;
      }
    }
    section_[bar_num]->insert(iter, n);
  } else { // split note exceeding barline
    Note* second = new Note(*n);
    second->start_t = (bar_num+1) * time_signature_.bar_edus_;
    n->end_t = (bar_num + 1) * time_signature_.bar_edus_;
    n->split = 1;

    vector<Note*>::iterator iter;
    for (iter = section_[bar_num]->begin();
         iter != section_[bar_num]->end();
         iter++) {
      Note* cur = *iter;
      if (cur->start_t > n->start_t) {
        section_[bar_num]->insert(iter, n);
        InsertNote(second);
        return true;
      }
    }
    
    section_[bar_num]->insert(iter, n);
    InsertNote(second);
  }

  return true;
}

void Section::SetDurationEDUS(int edus) {
  if (edus == -1) {
    is_edu_limit_ = false;
  }
  time_signature_.duration_edus_ = edus;
  remaining_edus_ = edus;
}

float Section::GetStartTimeGlobal() const {
  return time_signature_.start_time_global_;
}

int Section::CalculateEDUsFromSecondsInTempo(float seconds) {
  return time_signature_.tempo_.convertSecondsToEDUs(seconds);
}

void Section::Build() {
  if (!is_built_) {
    if (is_edu_limit_ && remaining_edus_ == 0) {
      cerr << "Section cannot be built without exact edu allotment" << endl;
      exit(1);
    }

    PrintAllNotesBar(); // TODO - remove

    section_flat_.clear();

    // Notate the time signature
    Note* time_signature = new Note();
    time_signature->start_t = 0;
    time_signature->end_t = 0;
    time_signature->type_out = "\\time " + time_signature_.time_signature_ + '\n';
    section_flat_.push_back(time_signature);

    AddBars();
    AddRestsAndFlatten();
    Notate();

    PrintAllNotesFlat(); // TODO - remove

    is_built_ = true;
  }
}

vector<Note*>& Section::GetSectionFlat() {
  if (is_built_) {
    return section_flat_;
  }

  cerr << "Cannot get flattened unbuilt section!" << endl;
  exit(1);
}

bool Section::operator<(const Section& other) const {
  return time_signature_.start_time_global_ < other.time_signature_.start_time_global_;
}

bool Section::operator>(const Section& other) const {
  return time_signature_.start_time_global_ > other.time_signature_.start_time_global_;
}
  
bool Section::operator==(const Section& other) const {
  return time_signature_ == other.time_signature_;
}

bool Section::operator!=(const Section& other) const {
  return !operator==(other);
}

void Section::EnsureNoteExpressible(Note* n) {
  int dur = n->end_t % time_signature_.beat_edus_;
	int before = n->end_t;
  int min_diff = time_signature_.beat_edus_;
  bool note_needs_chop = true;

  for (int i = 0; i < time_signature_.valid_dividers_.size(); i++) {
    if (dur % time_signature_.valid_dividers_[i] == 0) {
      note_needs_chop = false;
      break;
    } else {
      if (min_diff > dur % time_signature_.valid_dividers_[i]) {
        min_diff = dur % time_signature_.valid_dividers_[i];
      }
    }
  }

  if (note_needs_chop) {
    n->end_t -= min_diff;
  }
}

void Section::ResizeSection(size_t new_size) {
  for(size_t bar_idx = section_.size(); bar_idx <= new_size; ++bar_idx) {
    vector<Note*>* bar = new vector<Note*>();
    Note* n = new Note();
    n->start_t = time_signature_.bar_edus_ * bar_idx;
    n->end_t = time_signature_.bar_edus_ * bar_idx;
    n->type_out = " ";
    bar->push_back(n);
    section_.push_back(bar);
  }
}

void Section::AddBars() {
  vector<vector<Note*>*>::iterator it;
  int bar_idx = 1;
  for (vector<Note*>* bar : section_){
      Note* n = new Note();
      n->start_t = time_signature_.bar_edus_ * bar_idx;
      n->end_t = time_signature_.bar_edus_ * bar_idx;
      n->type_out = "\\bar\"|\" \n";
      bar->push_back(n);
      ++bar_idx;
  }
}

void Section::AddRestsAndFlatten() {
  for(size_t i = 0; i < section_.size(); ++i) {
    vector<Note*>::iterator it;
    Note* prev = *(section_[i]->begin());
    section_flat_.push_back(prev);
    Note* cur;

    for (it = section_[i]->begin() + 1; it != section_[i]->end(); it++) {
      cur = *it;
      int gap = cur->start_t - prev->end_t;

      // a rest will be placed only if gap is more than half of the
      // smallest non-zero valid duration
      if(gap > (time_signature_.beat_edus_ / (time_signature_.tuplet_limit_ - 1) / 2)) {
        Note* rest = new Note();
        rest->start_t = prev->end_t;
        rest->end_t = cur->start_t;
        rest->pitch_out = "r";
        section_flat_.push_back(rest);
      } else {
        if(it+1 == section_[i]->end()){
          prev->end_t = cur->start_t; // Can't shorten current note because end of bar
        } else {
          cur->start_t = prev->end_t;
        }
      }
      section_flat_.push_back(cur);
      prev = cur;
    }
  }
}

void Section::Notate() {
  vector<Note*>::iterator it;
  int prev_tuplet = 0; // the previous tuplet type
  int tuplet_dur = 0; // the current tuplet duration in edus
  for (it = section_flat_.begin(); it!=section_flat_.end(); it++) {
    Note* cur = *it;

    // adjust note duration according to the tuplet type
    if(tuplet_dur > 0) {
      int duration = cur->end_t - cur->start_t;
      int dur_remainder = (duration) % time_signature_.beat_edus_;
      int dur_beats = (duration) / time_signature_.beat_edus_;
      int desire_type = prev_tuplet;
      int cur_type = time_signature_.DetermineTuplet(dur_remainder);
      int excess_tuplet_type = time_signature_.DetermineTuplet(duration - tuplet_dur);

      // True if the note exceeds the current tuplet duration
      // but the excess is expressible in another tuplet
      bool note_is_valid = (duration > tuplet_dur) && (excess_tuplet_type != -1);

      // if the current note's duration cannot be fitted in the tuplet
      // find the closest value and change the end time of the note.
      // Also change the start time of the next note
      if((desire_type != cur_type) && !note_is_valid) {
        int t = time_signature_.beat_edus_ / desire_type;
        double a = (double)dur_remainder / (double)t;
        int best_fit = (int) round(a) * t;
        cur->end_t = cur->start_t + dur_beats * time_signature_.beat_edus_ + best_fit;

        Note* next = *(it+1);
        if(next == nullptr){
          continue;
        }

        next->start_t = cur->start_t + dur_beats * time_signature_.beat_edus_ + best_fit;
      }
    }

    // force the closing of the tuplet before the bar line (not necessary if
    // the code is correct)
    if(cur->type_out == "\\bar\"|\" \n" || cur->type_out == " "){
      if(tuplet_dur > 0){
        cur->type_out = "} " + cur->type_out;
        tuplet_dur = 0;
      }
      continue;
    }

    tuplet_dur = NotateCurrentNote(cur, &prev_tuplet, tuplet_dur);
  }
}

int Section::NotateCurrentNote(Note* current_note, 
                                     int* prev_tuplet, 
                                     int tuplet_dur) {
  int dur = current_note->end_t - current_note->start_t;
  if (dur == 0) {
    return tuplet_dur;
  }

  int remaining_dur = dur;
  if (tuplet_dur > 0) {
    remaining_dur = FillCurrentTupletDur(current_note, *prev_tuplet, tuplet_dur);
    if (remaining_dur < 0) { // Note fits inside the current tuplet
      return -remaining_dur; // Tuplet partially filled
    }
    tuplet_dur = 0; // Tuplet now filled
  }

  remaining_dur = FillCompleteBeats(current_note, remaining_dur);

  if (remaining_dur > 0) {
    tuplet_dur = CreateTupletWithRests(current_note, prev_tuplet, remaining_dur);
  }

  return tuplet_dur;
}

int Section::FillCurrentTupletDur(Note* current_note, 
                                        int prev_tuplet, 
                                        int tuplet_dur) {
  int dur = current_note->end_t - current_note->start_t;
  // if the current duration is less than the duration of the tuplet,
  // the entire duration will be inserted in the tuplet and the tuplet will
  // be completed by the next sound or silence.
  if (dur < tuplet_dur) {
    NoteInTuplet(current_note, prev_tuplet, dur);
    return dur - tuplet_dur;
  }
  // even if the previous tuplet is an eighth note or sixteenth note,
  // it is still necessary to split part of the current note.
  if(prev_tuplet == 2 || prev_tuplet == 4) {
    int unit = tuplet_dur / (time_signature_.beat_edus_ / prev_tuplet);
    if(unit == 3) {
      string s = to_string(time_signature_.unit_note_ * 2);
      current_note->type_out += current_note->pitch_out + s + ".";
    } else {
      string s = to_string(time_signature_.unit_note_ * prev_tuplet / unit);
      current_note->type_out += current_note->pitch_out + s;
    }
    if((dur > tuplet_dur || current_note->split == 1) && 
       (current_note->pitch_out != "r")) {
      current_note->type_out += "~ ";
    } else {
      current_note->type_out += " ";
    }

  } else {
    // if the current sound completes the tuplet use the LilyPond symbol and
    // close the tuplet
    NoteInTuplet(current_note, prev_tuplet, tuplet_dur);
    if((dur > tuplet_dur || current_note->split == 1) && (current_note->pitch_out != "r")) {
      current_note->type_out += "~ ";
    } else {
      // current_note->loudness_and_modifiers();
    }
    current_note->type_out += "} ";
  }

  return dur - tuplet_dur;
}

int Section::FillCompleteBeats(Note* current_note, int remaining_dur) {
  int remainder = remaining_dur % time_signature_.beat_edus_;
  int mainDur = remaining_dur / time_signature_.beat_edus_;

  while (mainDur > 0) {
    int power_of_2 = TimeSignature::DiscreteLog2(time_signature_.unit_note_);
    while (power_of_2 >= 0) {
      int beats = TimeSignature::Power(2, power_of_2);
      if (mainDur >= beats) {
        current_note->type_out += current_note->pitch_out + to_string(time_signature_.unit_note_ / beats);
        mainDur -= beats;
        if (mainDur >= beats / 2 && beats >= 2){
          current_note->type_out += ".";
          mainDur -= beats / 2;
        }
        break;
      }
      power_of_2--;
    }

    LoudnessMark(current_note);

    if (mainDur > 0 || remainder > 0) {
      if (current_note->pitch_out != "r") {
        current_note->type_out += "~ ";
      }
    } else {
      if (current_note->split == 1) {
        if (current_note->pitch_out != "r") {
          current_note->type_out += "~ ";
        }
        current_note->split = 0;
      }
      current_note->type_out += " ";
    }
  }

  return remainder;
}

int Section::CreateTupletWithRests(Note* current_note, 
                                         int* prev_tuplet,
                                         int remaining_dur) {
  int tuplet_dur = 0;
  
  int tuplet_type = time_signature_.DetermineTuplet(remaining_dur);
  if (tuplet_type == 2 || tuplet_type == 4) {
    if (remaining_dur / (time_signature_.beat_edus_ / tuplet_type) == 3) {
      string s = to_string(time_signature_.unit_note_ * 2);
      current_note->type_out += current_note->pitch_out + s + ". ";
    } else {
      string s = to_string(time_signature_.unit_note_ * tuplet_type);
      current_note->type_out += current_note->pitch_out + s + " ";
    }
    tuplet_dur = time_signature_.beat_edus_ - remaining_dur;
  } else if (tuplet_type != -1) {
    current_note->type_out += time_signature_.tuplet_types_[tuplet_type];
    NoteInTuplet(current_note, tuplet_type, remaining_dur);
    tuplet_dur = time_signature_.beat_edus_ - remaining_dur;
  }

  *prev_tuplet = tuplet_type;
  return tuplet_dur;
}

void Section::NoteInTuplet(Note* current_note, int tuplet_type, int duration) {
  string pitch = current_note->pitch_out;
  bool first = true;

  int beat = duration / (time_signature_.beat_edus_ / tuplet_type); // working in tuplet beats
  int unit_in_tuplet = time_signature_.unit_note_ * TimeSignature::CalculateNearestPow2(tuplet_type);
  int power_of_2 = TimeSignature::DiscreteLog2(unit_in_tuplet);
  while (beat > 0){

    int power_of_2 = TimeSignature::DiscreteLog2(unit_in_tuplet);
    while(power_of_2 >= 0){
      int beats = TimeSignature::Power(2, power_of_2);
      if(beat >= beats){
        current_note->type_out += current_note->pitch_out + to_string(unit_in_tuplet / beats);
        beat -= beats;
        if(beat >= beats/2 && beats >= 2){
          current_note->type_out += ".";
          beat -= beats/2;
        }
        break;
      }
      power_of_2--;
    }

    if ((beat > 0) && (current_note->pitch_out != "r")) {
      current_note->type_out += "~ ";
    } else {
      current_note->type_out += " ";
    }

    if(first == true){
      ModifiersMark(current_note);
      LoudnessMark(current_note);
      first = false;
    }
  }
}

void Section::LoudnessMark(Note* current_note) {
  if (current_note->loudness_out != prev_loudness && 
      current_note->pitch_out != "r") {
  	current_note->type_out += current_note->loudness_out + " ";
  	prev_loudness = current_note->loudness_out;
  }
}

void Section::ModifiersMark(Note* current_note) {
  while (!current_note->modifiers_out.empty()){
    current_note->type_out += current_note->modifiers_out.back() + " ";
    current_note->modifiers_out.pop_back();
  }
}