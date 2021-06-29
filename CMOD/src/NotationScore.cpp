#include "NotationScore.h"

NotationScore::NotationScore(Tempo& tempo) {
  time_signature_ = tempo.getTimeSignature();
  beat_edus_ = stoi(tempo.getEDUPerTimeSignatureBeat().toPrettyString());
  bar_edus_ = stoi(tempo.getEDUPerBar().toPrettyString());
  unit_note_ = tempo.getTimeSignatureBeat().Den(); // the note that represents one beat

  tuplet_limit_ = CalculateTupletLimit();

  ConstructTupletNames();
}

NotationScore::~NotationScore() {
  for (vector<Note*>* bar : score_){
      // Note pointers are not deleted because they are owned by the Bottom event
      // and thus deleted in the Bottom class.
      bar->clear;
      delete bar;
  }
  score_.clear();
  score_flat_.clear();
}

void NotationScore::InsertNote(Note* n) {
  is_built_ = false;

  EnsureNoteExpressible(n);

  if (n->end_t <= n->start_t ) {
	  return; // Discard invalid note
  }

  int bar_num = n->start_t / bar_edus_;

  if (bar_num >= score_.size()) {
    ResizeScore(bar_num);
  }

  int end = n->end_t / bar_edus_;
  // if note is completely inside the bar, insert it
  if(end <= bar_num) {
    vector<Note*>::iterator iter;
    for (iter = score_[bar_num]->begin(); 
         iter != score_[bar_num]->end(); 
         iter++) {
      Note* cur = *iter;
      if (cur->start_t > n->start_t) {
        score_[bar_num]->insert(iter, n);
        return;
      }
    }
    score_[bar_num]->insert(iter, n);
  } else { // split note exceeding barline
    Note* second = new Note(*n);
    second->start_t = (bar_num+1) * bar_edus_;
    n->end_t = (bar_num + 1) * bar_edus_;
    n->split = 1;

    vector<Note*>::iterator iter;
    for (iter = score_[bar_num]->begin(); 
         iter != score_[bar_num]->end(); 
         iter++) {
      Note* cur = *iter;
      if (cur->start_t > n->start_t) {
        score_[bar_num]->insert(iter, n);
        InsertNote(second);
        return;
      }
    }
    
    score_[bar_num]->insert(iter, n);
    InsertNote(second);
  }
}

void NotationScore::Build() {
  if (!is_built_) {
    score_flat_.clear();

    AddBars();
    AddRestsAndFlatten();
    Notate();
  }
}

ostream& operator<<(ostream& output_stream, 
                    NotationScore& notation_score) {
  if (!notation_score.is_built_) {
    notation_score.Build();
  }

  output_stream << "\\new Voice \\with {" << endl;
  output_stream << "\\remove \"Note_heads_engraver\"" << endl;
  output_stream << "\\consists \"Completion_heads_engraver\"" << endl;
  output_stream << "\\remove \"Rest_engraver\"" << endl;
  output_stream << "\\consists \"Completion_rest_engraver\"" << endl;
  output_stream << "}" << endl;

  output_stream << "{" << endl;
  output_stream << "\\time " << timesignature << endl;

  for (Note* note : notation_score.score_flat_) {
    output_stream << note->type_out;
  }
  
  /* output one last thing and close file stream*/
  output_stream << "\\bar \"|.\"" << endl;
  output_stream << "}" << endl;
}

size_t NotationScore::CalculateTupletLimit() {
  size_t tuplet_num = 1;

  while (beat_edus_ % tuplet_num == 0) {
    valid_dividers_.push_back(beat_edus_ / tuplet_num);
    tuplet_num++;
  }

  return tuplet_num;
}

void NotationScore::ConstructTupletNames() {
  for(size_t i = 0; i < tuplet_limit_; i++) {
    int l = check_lower(i);
    string t = "\\tuplet ";
    t += std::to_string(i) + "/" + std::to_string(l) + "{ ";
    tuplet_types_.push_back(t);
  }
}

void NotationScore::EnsureNoteExpressible(Note* n) {
  int dur = n->end_t % beat_edus_;
	int before = n->end_t;
  int min_diff = beat_edus_;
  bool note_needs_chop = true;

  for (int i = 0; i < valid_dividers_.size(); i++) {
    if (dur % valid_dividers_[i] == 0) {
      note_needs_chop = false;
      break;
    } else {
      if (min_diff > dur % valid_dividers_[i]) {
        min_diff = dur % valid_dividers_[i];
      }
    }
  }

  if (note_needs_chop) {
    n->end_t -= min_diff;
  }
}

void NotationScore::ResizeScore(size_t new_size) {
  for(size_t bar_idx = score_.size(); bar_idx <= new_size; ++bar_idx) {
    vector<Note*>* bar = new vector<Note*>();
    Note* n = new Note();
    n->start_t = bar_edus_ * bar_idx;
    n->end_t = bar_edus_ * bar_idx;
    n->type_out = " ";
    bar->push_back(n);
    score_.push_back(bar);
  }
}

void NotationScore::AddBars() {
  vector<vector<Note*>*>::iterator it;
  int bar_idx = 1;
  for (vector<Note*>* bar : score_){
      Note* n = new Note();
      n->start_t = bar_edus_ * bar_idx;
      n->end_t = bar_edus_ * bar_idx;
      n->type_out = "\\bar\"|\" \n";
      bar->push_back(n);
      ++bar_idx;
  }
}

void NotationScore::AddRestsAndFlatten() {
  for(size_t i = 0; i < score_.size(); i++) {
    vector<Note*>::iterator it;
    Note* prev = *(score_[i]->begin());
    score_flat_.push_back(prev);
    Note* cur;

    for (it = score_[i]->begin() + 1; it != score_[i]->end(); it++) {
      cur = *it;
      int gap = cur->start_t - prev->end_t;

      // a rest will be placed only if gap is more than half of the
      // smallest non-zero valid duration
      if(gap > (beat_edus_ / (tuplet_limit_ - 1) / 2)) {
        Note* rest = new Note();
        rest->start_t = prev->end_t;
        rest->end_t = cur->start_t;
        rest->pitch_out = "r";
        score_flat_.push_back(rest);
      } else {
        if(it+1 == score_[i]->end()){
          prev->end_t = cur->start_t; // Can't shorten current note because end of bar
        } else {
          cur->start_t = prev->end_t;
        }
      }
      score_flat_.push_back(cur);
      prev = cur;
    }
  }
}

void NotationScore::Notate() {
  vector<Note*>::iterator it;
  int prev_tuplet = 0; // the previous tuplet type
  int tuplet_dur = 0; // the current tuplet duration in edus
  for (it = all_notes.begin(); it!=all_notes.end(); it++) {
    Note* cur = *it;

    // adjust note duration according to the tuplet type
    if(tuplet_dur > 0) {
      int duration = cur -> end_t - cur -> start_t;
      int dur_remainder = (duration) % beatEDUs;
      int dur_beats = (duration) / beatEDUs;
      int desire_type = prev_tuplet;
      int cur_type = DetermineTuplet(dur_remainder);
      int excess_tuplet_type = DetermineTuplet(duration - tuplet_dur);

      // True if the note exceeds the current tuplet duration
      // but the excess is expressible in another tuplet
      bool note_is_valid = (duration > tuplet_dur) && (excess_tuplet_type != -1);

      // if the current note's duration cannot be fitted in the tuplet
      // find the closest value and change the end time of the note.
      // Also change the start time of the next note
      if((desire_type != cur_type) && !note_is_valid) {
        int t = beatEDUs / desire_type;
        double a = (double)dur_remainder / (double)t;
        int best_fit = (int) round(a) * t;
        cur->end_t = cur->start_t + dur_beats * beatEDUs + best_fit;

        Note* next = *(it+1);
        if(next == NULL){
          continue;
        }

        next->start_t = cur->start_t + dur_beats * beatEDUs + best_fit;
      }
    }

    // force the closing of the tuplet before the bar line (not necessary if
    // the code is correct)
    if(cur -> type_out == "\\bar\"|\" \n" || cur -> type_out == " "){
      if(tuplet_dur > 0){
        cur -> type_out = "} " + cur -> type_out;
        tuplet_dur = 0;
      }
      continue;
    }

    // pass the note to notate function
    tuplet_dur = NotateCurrentNote(&prev_tuplet, tuplet_dur);
  }
}

int NotationScore::NotateCurrentNote(Note* current_note, 
                                     int* prev_tuplet, 
                                     int tuplet_dur) {
  int dur = current_note->end_t - current_note->start_t;
  if(dur == 0){
    return tuplet_dur;
  }

  int remaining_dur;
  if (tuplet_dur > 0) {
    remaining_dur = FullCurrentTupletDur(current_note, *prev_tuplet, tuplet_dur);
    if (remaining_dur < 0) { // Note fits inside the current tuplet
      return -remaining_dur; // Tuplet partially filled
    }
    dur = remaining_dur;
    tuplet_dur = 0; // Tuplet now filled
  }

  remaining_dur = FillCompleteBeats(current_note, remaining_dur);

  if (remaining_dur > 0) {
    tuplet_dur = CreateTupletWithRests(current_note, prev_tuplet, remaining_dur);
  }

  return tuplet_dur;
}

int NotationScore::FillCurrentTupletDur(Note* current_note, 
                                        int prev_tuplet, 
                                        int tuplet_dur) {
  int dur = current_note->end_t - current_note->start_t;
  // if the current duration is less than the duration of the tuplet,
  // the entire duration will be inserted in the tuplet and the tuplet will
  // be completed by the next sound or silence.
  if(dur < tuplet_dur){
    NoteInTuplet(current_note, prev_tuplet, dur);
    return dur - tuplet_dur;
  }
  // even if the previous tuplet is an eighth note or sixteenth note,
  // it is still necessary to split part of the current note.
  if(prev_tuplet == 2 || prev_tuplet == 4){
    int unit = tuplet_dur / (beatEDUs/prev_tuplet);
    if(unit == 3) {
      string s = to_string(unit_note_ * 2);
      current_note->type_out += current_note->pitch_out + s + ".";
    } else {
      string s = to_string(unit_note_ * prev_tuplet / unit);
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

int NotationScore::FillCompleteBeats(Note* current_note, int remaining_dur) {
  int remainder = remaining_dur % beatEDUs;
  int mainDur = remaining_dur / beatEDUs;

  while (mainDur > 0) {
    int power_of_2 = DiscreteLog2(unit_note_);
    while(power_of_2 >= 0) {
      int beats = power(2, power_of_2);
      if(mainDur >= beats) {
        current_note->type_out += current_note->pitch_out + to_string(unit_note_ / beats);
        mainDur -= beats;
        if(mainDur >= beats / 2 && beats >= 2){
          current_note->type_out += ".";
          mainDur -= beats / 2;
        }
        break;
      }
      power_of_2--;
    }

    current_note->loudness_mark();

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

int NotationScore::CreateTupletWithRests(Note* current_note, 
                                         int* prev_tuplet,
                                         int remaining_dur) {
  int tuplet_dur = 0;
  
  int tuplet_type = DetermineTuplet(remaining_dur);
  if(tuplet_type == 2 || tuplet_type == 4) {
    if(remaining_dur / (beatEDUs / tuplet_type) == 3) {
      string s = to_string(unit_note_ * 2);
      current_note->type_out += current_note->pitch_out + s + ". ";
    } else {
      string s = to_string(unit_note_ * tuplet_type);
      current_note->type_out += current_note->pitch_out + s + " ";
    }
    tuplet_dur = beatEDUs - remaining_dur;
  } else if(tuplet_type != -1) {
    current_note->type_out += tuplet_types_[tuplet_type];
    NoteInTuplet(current_note, tuplet_type, remaining_dur);
    tuplet_dur = beatEDUs - remaining_dur;
  }

  *prev_tuplet = tuplet_type;
  return tuplet_dur;
}

void NotationScore::NoteInTuplet(Note* current_note, int tuplet_type, int duration) {
  string pitch = current_note->pitch_out;
  bool first = true;

  int beat = duration / (beatEDUs / tuplet_type); // working in tuplet beats
  int unit_in_tuplet = unit_note_ * CalculateNearestPow2(tuplet_type);
  int power_of_2 = DiscreteLog2(unit_in_tuplet);
  while (beat > 0){

    int power_of_2 = DiscreteLog2(unit_in_tuplet);
    while(power_of_2 >= 0){
      int beats = power(2, power_of_2);
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

    if ((beat > 0) && (current_note->pitch_out != "r")){
      current_note->type_out += "~ ";
    } else {
      current_note->type_out += " ";
    }

    if(first == true){
      current_note->modifiers_mark();
      current_note->loudness_mark();
      first = false;
    }
  }
}

int NotationScore::DiscreteLog2(int num) {
  if (num % 2 != 0) {
    return -1;
  }

  int pow = 0;
  int power_of_2 = 1;
  while (power_of_2 < num && pow < std::numeric_limits<int>::digits) {
    power_of_2 *= 2;
    if (power_of_2 == num) {
      return pow;
    }
    ++pow;
  }

  return -1;
}

int NotationScore::CalculateNearestPow2(int num) {
  int multiple_of_2 = 2;

  while (multiple_of_2 <= num) {
    multiple_of_2 *= 2;
  }

  return multiple_of_2 / 2;
}

int NotationScore::Power(int base, int pow) {
  int output = 1;
  while (pow != 0) {
    output *= base;
    --pow;
  }

  return output;
}