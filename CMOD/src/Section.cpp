#include "Section.h"

string Section::prev_loudness;

Section::Section(TimeSignature time_signature) : 
    time_signature_(time_signature),
    is_built_(false),
    is_edu_limit_(true),
    cap_(0) {
  prev_loudness = "";
  section_ = vector<vector<Note*>*>(0);
  section_flat_ = list<Note*>();
  remaining_edus_ = 0;
}

Section::Section(const Section& other) {
  time_signature_ = other.time_signature_;
  section_ = other.section_;
  section_flat_ = other.section_flat_;
  is_built_ = other.is_built_;
  remaining_edus_ = other.remaining_edus_;
  is_edu_limit_ = other.is_edu_limit_;
  cap_ = other.cap_;
}

/* For upgrading to c++11
Section::Section(Section&& source) {
  time_signature_ = std::move(source.time_signature_);
  section_ = std::move(source.section_);
  section_flat_ = std::move(source.section_flat_);
  is_built_ = source.is_built_; source.is_built_ = false;
  remaining_edus_ = source.remaining_edus_; source.remaining_edus_ = 0;
  is_edu_limit_ = source.is_edu_limit_; source.is_edu_limit_ = false;
  cap_ = source.cap_; source.cap_ = 0;
}
*/

Section& Section::operator=(const Section& other) {
  time_signature_ = other.time_signature_;
  section_ = other.section_;
  section_flat_ = other.section_flat_;
  is_built_ = other.is_built_;
  remaining_edus_ = other.remaining_edus_;
  is_edu_limit_ = other.is_edu_limit_;
  cap_ = other.cap_;
  return *this;
}

/* For upgrading to c++11
Section& Section::operator=(Section&& source) {
  time_signature_ = std::move(source.time_signature_);
  section_ = std::move(source.section_);
  section_flat_ = std::move(source.section_flat_);
  is_built_ = source.is_built_; source.is_built_ = false;
  remaining_edus_ = source.remaining_edus_; source.remaining_edus_ = 0;
  is_edu_limit_ = source.is_edu_limit_; source.is_edu_limit_ = false;
  cap_ = source.cap_; source.cap_ = 0;
  return *this;
}
*/

Section::~Section() {
/*
There's a discussion on memory management to be had here. Currently, notes are dynamically
allocated outside and inside of the Section class. The deallocation is handled right here.
This leads to unnecessary complexity in the Section class and it also necessitates allocating
the end cap (which is also a Section) dynamically on the heap to manually manage its lifetime
so notes don't get deleted before their use is over.
*/
  for (vector<vector<Note*>*>::iterator iter = section_.begin();
       iter != section_.end();
       ++iter) {
      vector<Note*>*& bar = *iter;
      bar->clear();
      delete bar;
  }
  section_.clear();

  for (list<Note*>::iterator iter = section_flat_.begin();
       iter != section_flat_.end();
       ++iter) {
    Note*& note = *iter;
    delete note;
  }
  section_flat_.clear();

  if (cap_) {
    delete cap_;
  }
}

bool Section::InsertNote(Note* n) {
  if (n->rootExactAncestor != time_signature_.tempo_.getRootExactAncestor()) {
    return false; // Note does not belong to this section.
  }

  n->type_out = "";
  n->type = NoteType::kNote;
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
    second->start_t = (bar_num + 1) * time_signature_.bar_edus_;
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
  remaining_edus_ = edus;
}

float Section::GetStartTimeGlobal() const {
  return time_signature_.start_time_global_;
}

int Section::CalculateEDUsFromSecondsInTempo(float seconds) {
  return time_signature_.tempo_.convertSecondsToEDUs(seconds);
}

void Section::Build(bool notate_time_signature) {
  if (!is_built_) {
    if (is_edu_limit_ && remaining_edus_ == 0) {
      cerr << "Section cannot be built without exact edu allotment" << endl;
      exit(1);
    }

    section_flat_.clear();

    if (notate_time_signature) {
      Note* time_signature = new Note();
      time_signature->start_t = 0;
      time_signature->end_t = 0;
      time_signature->type_out = "\\time " + time_signature_.time_signature_ + '\n';
      time_signature->type = NoteType::kTimeSignature;
      section_flat_.push_back(time_signature);
    }

    Note* first_barline = new Note();
    first_barline->start_t=0;
    first_barline->end_t=0;
    first_barline->type = NoteType::kBarline;
    section_flat_.push_back(first_barline);

    AddBars();
    AddRestsAndFlatten();
    Notate();
    if (is_edu_limit_) {
      CapEnding();
    }

    is_built_ = true;
  }
}

const list<Note*>& Section::GetSectionFlat() {
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

void Section::ResizeSection(int new_size) {
  for(int bar_idx = section_.size(); bar_idx <= new_size; ++bar_idx) {
    vector<Note*>* bar = new vector<Note*>();
    Note* n = new Note();
    n->start_t = time_signature_.bar_edus_ * bar_idx;
    n->end_t = time_signature_.bar_edus_ * bar_idx;
    n->type_out = " ";
    n->type = NoteType::kUnknown;
    bar->push_back(n);
    section_.push_back(bar);
  }
}

void Section::AddBars() {
  int bar_idx = 1;
  for (vector<vector<Note*>*>::iterator iter = section_.begin();
       iter != section_.end();
       ++iter) {
      vector<Note*>*& bar = *iter;
      Note* n = new Note();
      n->start_t = time_signature_.bar_edus_ * bar_idx;
      n->end_t = time_signature_.bar_edus_ * bar_idx;
      n->type_out = "\\bar\"|\" \n";
      n->type = NoteType::kBarline;
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
        rest->type = NoteType::kRest;
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
  list<Note*>::iterator it = section_flat_.begin();
  list<Note*>::iterator next_it = ++section_flat_.begin();
  int prev_tuplet = 0; // the previous tuplet type
  int tuplet_dur = 0; // the current tuplet duration in edus
  for (; it != section_flat_.end(); ++it, ++next_it) {
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
      if ((desire_type != cur_type) && !note_is_valid) {
        int t = time_signature_.beat_edus_ / desire_type;
        double a = (double)dur_remainder / (double)t;
        int best_fit = (int) round(a) * t;

        cur->end_t = cur->start_t + dur_beats * time_signature_.beat_edus_ + best_fit;

        Note* next_note = *(next_it);
        if (next_note == 0 || next_it == section_flat_.end()) {
          continue;
        }

        next_note->start_t = cur->start_t + dur_beats * time_signature_.beat_edus_ + best_fit;
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

void Section::CapEnding() {
  int cur_bar_edus = 0;
  list<Note*> last_bar = PopLastBarNotes();
  
  if (!last_bar.empty()) {
    cur_bar_edus = last_bar.back()->end_t - last_bar.front()->start_t;
    cur_bar_edus %= time_signature_.bar_edus_; // want the last __incomplete__ bar
  }

  int used_edus_ = last_bar.back()->end_t; // edus start at 0, so this gives used time
  remaining_edus_ -= used_edus_; // 'rest' edus
  int total_edus_to_use = remaining_edus_ + cur_bar_edus;

  if (remaining_edus_ < 0) {
    cerr << "Sections overlap" << endl;
    exit(1);
  } else if (remaining_edus_ == 0 && cur_bar_edus == 0) {
    return; // Sections align perfectly!
  } else {
    int pow_2 = 0;
    int min_err = INT_MAX;
    int ts_num, ts_den;
    while (time_signature_.beat_edus_ % TimeSignature::Power(2, pow_2) == 0) {
      int tmp_beat_edus = time_signature_.beat_edus_ / TimeSignature::Power(2, pow_2);
      if (total_edus_to_use % tmp_beat_edus == 0) {
        ts_num = total_edus_to_use / tmp_beat_edus;
        ts_den = time_signature_.unit_note_ * TimeSignature::Power(2, pow_2);
        min_err = 0;
        break; // Overhanging time forms a dyadic time signature
      } else {
        // Form a dyadic time signature by adding sound or rest with the least error
        int num_beats = (total_edus_to_use / tmp_beat_edus) + 1; // TODO - do this better
        int err = tmp_beat_edus * num_beats - total_edus_to_use;
        if (err < min_err) {
          ts_num = num_beats; // Add time
          ts_den = time_signature_.unit_note_ * TimeSignature::Power(2, pow_2);
          min_err = err;
        }
      }
      ++pow_2;
    }

    int beat_divisor = TimeSignature::Power(2, pow_2);
    Tempo new_tempo(time_signature_.tempo_);

    new_tempo.setEDUPerTimeSignatureBeat(time_signature_.beat_edus_ / beat_divisor);
    new_tempo.setTimeSignature(Note::int_to_str(ts_num) + "/" + Note::int_to_str(ts_den));
    // IMPORTANT - Tempo rate (i.e. 1/4=60bpm) is left the same
    // IMPORTANT - Tempo start time left unchanged because the calculation is not necessary

    cap_ = new Section(TimeSignature(new_tempo));
    cap_->SetDurationEDUS(-1);

    int offset = last_bar.front()->start_t;
    while (!last_bar.empty()) {
      last_bar.front()->start_t -= offset; // make notes in cap start from 0
      last_bar.front()->end_t -= offset;

      if (!cap_->InsertNote(last_bar.front())) {
        cerr << "Note could not be inserted into end cap. " <<
                "This should not happen under any circumstance." << endl;
        exit(1);
      }
      last_bar.pop_front();
    }

    if (min_err != 0 && remaining_edus_ == 0) { // No extra time and leftover sound does not fill time signature
      Note* extra_space = new Note(*last_bar.back()); // TODO - what if we get a tie over the last bar
      extra_space->start_t = last_bar.back()->start_t;
      extra_space->end_t = extra_space->start_t + min_err;
      extra_space->split = 1;
      cap_->InsertNote(extra_space);
    }

    if (min_err != 0) {
      cout << Note::int_to_str(new_tempo.calculateSecondsFromEDUs(min_err))
           << " seconds added to stitch sections." << endl;
    }

    // Only notate time signature if different
    cap_->time_signature_ == time_signature_ ? cap_->Build(false) : cap_->Build(true);

    list<Note*> final_bar = cap_->PopFirstBar();
    while (!final_bar.empty()) {
      section_flat_.push_back(final_bar.front());
      final_bar.pop_front();
    }
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
  if (prev_tuplet == 2 || prev_tuplet == 4) {
    int unit = tuplet_dur / (time_signature_.beat_edus_ / prev_tuplet);
    if(unit == 3) {
      string s = Note::int_to_str(time_signature_.unit_note_ * 2);
      current_note->type_out += current_note->pitch_out + s + ".";
    } else {
      string s = Note::int_to_str(time_signature_.unit_note_ * prev_tuplet / unit);
      current_note->type_out += current_note->pitch_out + s;
    }
    if ((dur > tuplet_dur || current_note->split == 1) && 
       (current_note->pitch_out != "r")) {
      current_note->type_out += "~ ";
    } else {
      current_note->type_out += " ";
    }

  } else {
    // if the current sound completes the tuplet use the LilyPond symbol and
    // close the tuplet
    NoteInTuplet(current_note, prev_tuplet, tuplet_dur);
    if ((dur > tuplet_dur || current_note->split == 1) && (current_note->pitch_out != "r")) {
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
        current_note->type_out += current_note->pitch_out + Note::int_to_str(time_signature_.unit_note_ / beats);
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
      string s = Note::int_to_str(time_signature_.unit_note_ * 2);
      current_note->type_out += current_note->pitch_out + s + ". ";
    } else {
      string s = Note::int_to_str(time_signature_.unit_note_ * tuplet_type);
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
        current_note->type_out += current_note->pitch_out + Note::int_to_str(unit_in_tuplet / beats);
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

list<Note*> Section::PopFirstBar() {
  list<Note*> bar;

  list<Note*>::iterator note_iter = section_flat_.begin();
  int num_items_in_bar = 0;
  bool first_barline_seen = false;
  while (note_iter != section_flat_.end() &&
         !first_barline_seen ||
         (*note_iter)->type != NoteType::kBarline) {
    bar.push_back(*note_iter);

    if ((*note_iter)->type == NoteType::kBarline) 
      first_barline_seen = true;

    ++note_iter; ++num_items_in_bar;
  }

  if (!first_barline_seen) {
    cerr << "Could not locate first bar in section" << endl;
    exit(1);
  }

  // Remove the first bar from section_flat_
  while (num_items_in_bar > 0) {
    section_flat_.pop_front();
    --num_items_in_bar;
  }

  return bar;
}

list<Note*> Section::PopLastBarNotes() {
  PrintAllNotesFlat("Popping last bar");
  list<Note*> last_bar(0);

  Note* last_barline = 0;
  list<Note*>::iterator note_iter = section_flat_.begin();
  list<Note*>::iterator next = ++section_flat_.begin();
  int num_items_in_bar = 0;
  for (; note_iter != section_flat_.end(); ++note_iter, ++next) {
    Note* note = *note_iter;

    if (note->type == NoteType::kNote) {
      last_bar.push_back(note);
    }

    if (next != section_flat_.end() && 
        note->type == NoteType::kBarline) {
      num_items_in_bar = 0;
      last_barline = note;
      last_bar.clear();
    }

    ++num_items_in_bar;
  }

  if (last_barline == 0) {
    cerr << "Could not locate last bar for stitching" << endl;
    exit(1);
  }

  // Remove the last bar from section_flat_
  while (num_items_in_bar > 0) {
    if (section_flat_.back()->type != NoteType::kNote) {
      delete section_flat_.back(); // not returned so they have to be deleted here
    }
    section_flat_.pop_back();
    --num_items_in_bar;
  }

  PrintAllNotesFlat("Popped last bar");

  return last_bar;
}