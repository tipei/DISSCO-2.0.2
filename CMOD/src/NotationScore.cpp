#include "NotationScore.h"

NotationScore::NotationScore(Tempo& tempo) {
  time_signature_ = tempo.getTimeSignature();
  beat_edus_ = stoi(tempo.getEDUPerTimeSignatureBeat().toPrettyString());
  bar_edus_ = stoi(tempo.getEDUPerBar().toPrettyString());
  unit_note_ = tempo.getTimeSignatureBeat().Den(); // the note that represents one beat

  tuplet_limit_ = CalculateTupletLimit();

  ConstructTupletNames();
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

    // TODO - AddBars();
    // TODO - AddRests();
    // TODO - 
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
      Note* n = new Note(); // NOTE - the bars are added as __notes__
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
        Note* rest = new Note(); // NOTE - now a note is a rest too!
        rest->start_t = prev->end_t;
        rest->end_t = cur->start_t;
        rest->pitch_out = "r";
        score_flat_.push_back(rest);
      } else {
        if(it+1 == score_[i]->end()){
          prev->end_t = cur->start_t; // NOTE - Can't shorten current note because end of bar
        } else {
          cur->start_t = prev->end_t;
        }
      }
      score_flat_.push_back(cur);
      prev = cur;
    }
  }
}