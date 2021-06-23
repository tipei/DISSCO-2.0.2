#include "NotationScore.h"

NotationScore::NotationScore(Tempo& tempo) {
  time_signature_ = tempo.getTimeSignature();
  beat_edus_ = stoi(tempo.getEDUPerTimeSignatureBeat().toPrettyString());
  bar_edus_ = stoi(tempo.getEDUPerBar().toPrettyString());
  unit_note_ = tempo.getTimeSignatureBeat().Den(); // the note that represents one beat

  tuplet_limit_ = CalculateTupletLimit();

  ConstructTupletNames();
}

NotationScore::InsertNote(Note* n) {
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

NotationScore::Build() {
  if (!is_built_) {
    score_flat_.clear();

    // TODO - AddBars();
    // TODO - AddRests();
    // TODO - 
  }
}

ostream& operator<<(ostream& output_stream, 
                    const NotationScore& notation_score) {
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

NotationScore::CalculateTupletLimit() {
  size_t tuplet_num = 1;

  while (beat_edus_ % tuplet_num == 0) {
    valid_dividers_.push_back(beat_edus_ / tuplet_num);
    tuplet_num++;
  }

  return tuplet_num;
}

NotationScore::ConstructTupletNames() {
  for(size_t i = 0; i < tuplet_limit_; i++) {
    int l = check_lower(i);
    string t = "\\tuplet ";
    t += stoi(i) + "/" + stoi(l) + "{ ";
    tuplet_types_.push_back(t);
  }
}

NotationScore::EnsureNoteExpressible(Note* n) {
  int dur = n->end_t % beat_edus_;
	int before = n->end_t;
  int min_diff = beat_edus_;
  bool note_needs_chop = true;

  for (int i = 0; i < valid_dividers_.size(); i++) {
    if (dur % valid_divider[i] == 0) {
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

NotationScore::ResizeScore(size_t new_size) {
  for(size_t bar_idx = score_.size(); i <= new_size; i++) {
    vector<Note*>* bar = new vector<Note*>();
    Note* n = new Note();
    n->start_t = bar_edus_ * bar_idx;
    n->end_t = bar_edus_ * bar_idx;
    n->type_out = " ";
    bar->push_back(n);
    score_.push_back(bar);
  }
}