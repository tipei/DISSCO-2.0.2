#ifndef NotationScore
#define NotationScore

#include "Libraries.h"

#include "Note.h" // TODO - remove
#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"

typedef vector<vector<Note*>*> Layer;

/**
 * A class representing a notated score for output using 
 * Lilypond (or something else in the future).
**/
class NotationScore {

public:
  /**
   * Construct a notation score with the provided tempo
   * 
   * @param tempo The tempo of the score
  **/
  NotationScore(Tempo& tempo);

  void insert_note(Note* note);

private:
  string time_signature_; // NOTE - only need this for outputting the actual score
  int beat_edus_;
  int bar_edus_;
  int unit_note_; // the note that represents one beat

  vector<int> valid_dividers_;

  vector<string> tuplet_types_;
  int tuplet_limit_; // the greatest allowed tuplet type (exclusive)

  Layer notation_score_;

  //a integer variable to indicate the previous tuplet type in notation loop
  static int pre_tuplet = 0;
  static string loudness_prev = "";

  size_t CalculateTupletLimit();

  void ConstructTupletNames();

};

#endif