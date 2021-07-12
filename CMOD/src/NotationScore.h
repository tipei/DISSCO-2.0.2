/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef NOTATION_SCORE_H
#define NOTATION_SCORE_H

#include "Libraries.h"

#include "Note.h"
#include "Section.h"
#include "TimeSignature.h"
#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"

class Section;

/**
 * A class representing a notated score for output using Lilypond.
 * 
 * Original notation module written by Haorong Sun
**/
class NotationScore {

public:
  /**
   * Construct a notation score.
  **/
  NotationScore();

  /**
   * Construct a notation score with the provided title.
   * 
   * @param score_title The title of this score
  **/
  NotationScore(const string& score_title);

  /**
   * Insert a Tempo into this score.
   * 
   * @param tempo The tempo to insert
  **/
  void RegisterTempo(Tempo& tempo);

  /**
   * Insert a Note into this score.
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
  void PrintScore() const {
    size_t section_idx = 0;
    cout << endl << endl;
    cout << "SCORE: " << endl;
    for (const Section& section : score_) {
      cout << "SECTION " << section_idx << endl;
      section.PrintAllNotesBar();
      ++section_idx;
    }
    cout << endl << endl;
  }

  string score_title_;

  vector<Section> score_;
  bool is_built_ = false;
};

#endif