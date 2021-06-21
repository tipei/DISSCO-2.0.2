#include "NotationScore.h"

NotationScore::NotationScore(Tempo& tempo) {
  time_signature_ = tempo.getTimeSignature();
  beat_edus_ = stoi(tempo.getEDUPerTimeSignatureBeat().toPrettyString());
  bar_edus_ = stoi(tempo.getEDUPerBar().toPrettyString());
  unit_note_ = tempo.getTimeSignatureBeat().Den() // the note that represents one beat

  valid_dividers_;

  tuplet_types_;
  tuplet_limit_ = CalculateTupletLimit();

  ConstructTupletNames();
}

NotationScore::CalculateTupletLimit() {
  size_t i = 1;

  while (beat_edus_ % i == 0) {
    valid_divider.push_back(beatEDUs/i);
    i++;
  }

  return i;
}

NotationScore::ConstructTupletNames() {
  for(size_t i = 0; i < tuplet_limit_; i++) {
    int l = check_lower(i);
    string t = "\\tuplet ";
    t += stoi(i) + "/" + stoi(l) + "{ ";
    tuplet_types_.push_back(t);
  }
}