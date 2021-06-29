#ifndef NOTATION_SCORE_H
#define NOTATION_SCORE_H

#include "Libraries.h"

#include "Note.h" // TODO - remove
#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"

typedef vector<vector<Note*>*> Layer;

// TODO - add lilypond enums

/**
 * A class representing a notated score for output using 
 * Lilypond (or something else in the future).
 * 
 * Original notation module written by Haorong Sun
**/
class NotationScore {

public:
  /**
   * Construct a notation score with the provided tempo.
   * 
   * @param tempo The tempo of the score
  **/
  NotationScore(const Tempo& tempo);

  /**
   * Destruct this notation score.
  **/
  ~NotationScore();

  /**
   * Set this NotationScore's tempo.
   * // TODO - allow a score to have multiple tempos
  **/
  void SetTempo(const Tempo& tempo);

  /**
   * Insert a note into this score.
   * 
   * @param n A pointer to the note to insert
  **/
  void InsertNote(Note* n);
  
  /**
   * Build the text representation of this score by adding bars,
   * rests, and adjusting durations.
  **/
  void Build();

  /**
   * Output the text representation of a score.
   * 
   * @param out_stream The stream to which the text will be appended
   * @param notation_score The score whose text representation to output
   * @returns The modified stream
  **/
  friend ostream& operator<<(ostream& output_stream, 
                             NotationScore& notation_score);

private:
  size_t CalculateTupletLimit();

  /** 
   * Determine what type of tuplet will fit the input duration. Eg. if 
   * a triplet fits the input duration return 3
   * 
   * @param dur The duration to evaluate
   * @return The fitting tuplet; -1 if input invalid
  **/
  int DetermineTuplet(int dur);

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

  /**
   * Add bars to this score.
  **/
  void AddBars();

  /**
   * Add rests between notes and flatten the score to score_flat_. 
   * Rests are not processed and may have invalid (inexpressible) 
   * durations.
  **/
  void AddRestsAndFlatten();

  /**
   * Run the notation loop for the score. 
  **/
  void Notate();

  /**
   * Notate the current note's duration given the previous tuplet type and the current
   * tuplet duration. This involves filling the current tuplet duration, filling the
   * subsequent complete beats if the note extends past the tuplet, and then using
   * rests to fill the remaining duration.
   * 
   * @param current_note The current note whose duration to notate
   * @param prev_tuplet The type of the previous tuplet
   * @param tuplet_dur The duration of the current tuplet to fill
   * @return The remaining duration to carry over in the notation loop
  **/
  int NotateCurrentNote(Note* current_note, int* prev_tuplet, int tuplet_dur);

  /**
   * Fill the current tuplet duration using the current note's duration.
   * 
   * @param current_note The current note whose duration to notate
   * @param prev_tuplet The type of the previous tuplet
   * @param tuplet_dur The duration of the current tuplet to fill
   * @return The remaining note duration
  **/
  int FillCurrentTupletDur(Note* current_note, int prev_tuplet, int tuplet_dur);
  
  /**
   * Fill the complete beats of the remaining current note's duration.
   * 
   * @param current_note The current note whose duration to notate
   * @param remaining_dur The remaining duration of the current note
   * @return The final leftover duration to be notated with rests
  **/
  int FillCompleteBeats(Note* current_note, int remaining_dur);

  /**
   * Create a tuplet with rests for the current note's final remaining duration.
   * 
   * @param current_note The current note whose duration to notate
   * @param prev_tuplet The type of the previous tuplet to set
   * @param remaining_dur The current note's leftover duration to be notated with rests
   * @return The tuplet duration to carry over in the notation loop
  **/
  int CreateTupletWithRests(Note* current_note, int* prev_tuplet, int remaining_dur);

  /**
   * Notate a note (or rest) in a tuplet.
   * 
   * @param current_note A pointer to the note to notate
   * @param tuplet_type The type of the tuplet in which to notate the note
   * @param duration The duration of the note to notate
  **/
  void NoteInTuplet(Note* current_note, int tuplet_type, int duration);

  /**
   * Add a loudness mark to the current note.
   * 
   * @param current_note The note to which to add a loudness mark
  **/
  void LoudnessMark(Note* current_note);

  /**
   * Add a modifier to the current note.
   * 
   * @param current_note The note to which to add a modifier
  **/
  void ModifiersMark(Note* current_note);

  /**
   * Calculate log2(num).
   * 
   * @param num The number to evaluate
   * @return The exponent of 2 which gives num; -1 if 
   * num is not a power of 2
   **/
  static int DiscreteLog2(int num);

  /**
   * Find the nearest power of 2 lower than the given number.
   * E.g. if num = 7, return 4; if num = 9, return 8.
   * 
   * @param num The number to evaluate
   * @return The nearest power of 2 lower than num
  **/
  static int CalculateNearestPow2(int num);

  /**
   * Calculate base^pow
   * 
   * @param base The integer to exponentiate
   * @param pow The exponential power
   * @return base^pow
  **/
  static int Power(int base, int pow);

  string time_signature_; // NOTE - only need this for outputting the actual score
  int beat_edus_;
  int bar_edus_;
  int unit_note_; // the note that represents one beat

  vector<int> valid_dividers_;

  vector<string> tuplet_types_;
  int tuplet_limit_; // the greatest allowed tuplet type (exclusive)

  Layer score_;
  vector<Note*> score_flat_;
  bool is_built_ = false;

  static string prev_loudness = ""; // the previous loudness mark in notation loop
};

#endif