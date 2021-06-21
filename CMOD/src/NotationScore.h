#ifndef NotationScore
#define NotationScore

#include "Libraries.h"

#include "Note.h" // TODO - remove
#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"

typedef vector<vector<Note*>*> Section;

/**
 * Stores metadata pertaining to sections of the Score
**/
struct SectionMetadata {
  string time_signature;
  int beat_edus;
  int bar_edus;
  int unit_note; // the note that represents one beat

  vector<int> valid_dividers;

  vector<string> tuplet_types;
  int tuplet_limit; // the greatest allowed tuplet type (exclusive)
};

/**
 * A class representing a notated score for output using 
 * Lilypond (or something else in the future).
**/
class NotationScore {

public:
  


private:
  list<pair<SectionMetadata, Section>> _notation_score;

  //a integer variable to indicate the previous tuplet type in notation loop
  static int pre_tuplet = 0;
  static string loudness_prev = "";
};

#endif