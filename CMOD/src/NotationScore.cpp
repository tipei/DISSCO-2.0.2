#include "NotationScore.h"

NotationScore::NotationScore() : 
    is_built_(false),
    score_title_("Score") {}

NotationScore::NotationScore(const string& score_title) :
    is_built_(false),
    score_title_(score_title) {}

void NotationScore::RegisterTempo(Tempo& tempo) {
  // Find insertion point by comparing the global start in __seconds__
  Section section_to_insert = Section(TimeSignature(tempo));
  vector<Section>::iterator section_iter = score_.begin();
  while (section_iter != score_.end() && 
         section_to_insert > *section_iter) {
    ++section_iter;
  }

  if (score_.empty() || *section_iter != section_to_insert) {
    score_.insert(section_iter, section_to_insert);
  }

  // Discard section; tempo does not start a new section
}

void NotationScore::InsertNote(Note* n) {
  if (score_.empty()) {
    cerr << "Cannot add note to score without any sections!" << endl;
    exit(1);
  }

  vector<Section>::iterator section_iter = score_.begin();
  while (section_iter != score_.end() && !(*section_iter).InsertNote(n)) ++section_iter;

  if (section_iter == score_.end()) {
    cerr << "Note does not belong to any section in the score!" << endl;
    exit(1);
  }
}

void NotationScore::Build() {
  if (!is_built_) {
    // Since all tempos are registered, calculate their start times 
    // in terms of the previous tempo's EDU's
    vector<Section>::iterator iter = score_.begin();
    vector<Section>::iterator next = score_.begin() + 1;
    int last_start_time_edu = 0;

    while (next != score_.end()) {
      float dur_seconds = next->GetStartTimeGlobal() - iter->GetStartTimeGlobal();
      iter->SetDurationEDUS(iter->CalculateEDUsFromSecondsInTempo(dur_seconds));
      iter->Build(true);
      ++iter; ++next;
    }
    iter->SetDurationEDUS(-1);
    iter->Build(true);

    is_built_ = true;
  }
}

ostream& operator<<(ostream& output_stream,
                    NotationScore& notation_score) {
  if (!notation_score.is_built_) {
    notation_score.Build();
  }

  output_stream << "\\header {\n  title=\"" << notation_score.score_title_ 
                << "\"\ncomposer=\"DISSCO\"\n}" << endl;
  output_stream << "\\new Voice \\with {" << endl;
  output_stream << "\\remove \"Note_heads_engraver\"" << endl;
  output_stream << "\\consists \"Completion_heads_engraver\"" << endl;
  output_stream << "\\remove \"Rest_engraver\"" << endl;
  output_stream << "\\consists \"Completion_rest_engraver\"" << endl;
  output_stream << "}" << endl;

  output_stream << "{" << endl;

  for (vector<Section>::iterator iter = notation_score.score_.begin();
       iter != notation_score.score_.end();
       ++iter) {
    iter->PrintAllNotesFlat("Final output");
    list<Note*> section_flat = iter->GetSectionFlat();

    for (list<Note*>::iterator iter_iter = section_flat.begin();
         iter_iter != section_flat.end();
         ++iter_iter) {
      Note* cur_note = *iter_iter;
      output_stream << cur_note->GetText();
    }
    
    output_stream << '\n';
  }
  
  output_stream << "\\bar \"|.\"" << endl;
  output_stream << "}" << endl;

  return output_stream;
}
