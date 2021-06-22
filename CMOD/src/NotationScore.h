#ifndef NOTATION_SCORE_H
#define NOTATION_SCORE_H

#include "Libraries.h"

#include "Note.h" // TODO - remove
#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"

typedef vector<vector<Note*>*> Layer;

/**
 * A class representing a notated score for output using 
 * Lilypond (or something else in the future).
 * 
 * Original notation module written by Haorong Sun
**/
class NotationScore {

public:
  /**
   * Construct a notation score with the provided tempo
   * 
   * @param tempo The tempo of the score
  **/
  NotationScore(Tempo& tempo);

  // TODO - implement copy constructor
  // TODO - implement move constructor
  // TODO - implement assignment operator
  // TODO - implement destructor

  /**
   * Insert a note into this score.
   * 
   * @param n A pointer to the note to insert
  **/
  void InsertNote(Note* n);
  
  /**
   * Build the text representation of this score.
  **/
  void BuildScore();

  /**
   * Output the text representation of this score.
   * 
   * @param out_stream The stream to which the text will be appended
   * @returns The appended stream
  **/
  ostream& operator>>(ostream& out_stream) const;

private:
  size_t CalculateTupletLimit();

  void ConstructTupletNames();

  /**
   * If the remainder given by the note's end time modulo the edu's per beat
   * is not divisible by any element of valid_dividers_, find the smallest 
   * difference between a multiple of the valid dividers and the remainder. 
   * Then, shorten the note's duration by this difference to make the note's
   * duration expressible.
   * 
   * @param n A pointer to the note whose duration to examine and modify
  **/
  void EnsureNoteExpressible(Note* n);

  /**
   * Resize the score to the specified size.
   * 
   * @param new_size The desired size of the score
  **/
  void ResizeScore(size_t new_size);

  string time_signature_; // NOTE - only need this for outputting the actual score
  int beat_edus_;
  int bar_edus_;
  int unit_note_; // the note that represents one beat

  vector<int> valid_dividers_;

  vector<string> tuplet_types_;
  int tuplet_limit_; // the greatest allowed tuplet type (exclusive)

  Layer notation_score_;
  stringstream score_;

  //a integer variable to indicate the previous tuplet type in notation loop
  static int pre_tuplet = 0;
  static string loudness_prev = "";
};

#endif