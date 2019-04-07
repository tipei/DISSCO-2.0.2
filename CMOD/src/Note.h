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

//----------------------------------------------------------------------------//
//
//  Note.h
//
//----------------------------------------------------------------------------//

#ifndef NOTE_H
#define NOTE_H

#include "Libraries.h"

#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"
#include "tables.h"

static int sBeat;
static int eBeat;

//static Sieve attackSieve;
//static Sieve durSieve;

/**
 *  Constructor
 *
 **/
class Note {

// static ofstream notaFile;

    		//Rhythm//

    //The timespan of the note.
    TimeSpan ts;

    //The parent tempo.
    Tempo tempo;

    		//Pitch//

    //Absolute numeric value of the pitch
    int pitchNum;

    //The octave the pitch is in
    int octaveNum;

    //The number of the pitches within the octave
    int octavePitch;

    //The string name of this pitch
    std::string pitchName;

    //Dynamic number
    int loudnessNum;

    //Dynamic marking (i.e. "ff")
    std::string loudnessMark;

    //Modifiers
    std::vector<std::string> modifiers; //string names of the modifiers

  public:

    /* variables for output notes */
    string pitch_out;
    string type_out;
    string loudness_out;

    int start_t; //start time
    int end_t; //end time

    int tuplet;
    string tuplet_name;
    int split;
    int diff, first_half_dur, last_half_dur, tuplet_1, tuplet_2;

    std::vector<std::string> modifiers_out;


    //Simple constructor
    Note();

    //Constructor with timespan and tempo
    Note(TimeSpan ts, Tempo tempo);

    //Copy constructor
    Note(const Note& other);

    /**
     *  Comparison operator (to sort in a list)
     *  \param rhs the object to compare to (right hand side)
     **/
    bool operator < (const Note& rhs);

    /**
     * Destructor
     **/
    ~Note();

//----------------------------------------------------------------------------//

    /**
      *  Assigns the pitch of a note
      *  \param absPitchNum Pitch on the well-tempered scale, starting with 0=C0
      *  \param pitchNames The names of the pitches (C, C#, D, Eb, ...)
      *  \note: vector<string> pitchnames not available yet
     **/
    void setPitchWellTempered(int pitchNum);

    /**
     *  Assigns the pitch of a note
     *  \param freqHz The frequency of the note in Hz
     *  \return int pitchNum, while float wellTemperedPitch could
     *    aaccommodate 1/4 tones.
     *
     *  \note: this assumes an equal tempered scale of 12 semitones
     **/
    int HertzToPitch(float freqHz);

    /**
     *  Assigns the loudness of a note
     *  \param dynamicNum The index into the noteDynamicMark array
     *  \param dynamicNames The names of the dynamics ( mf, f, ppp ...)
     *  \note: not in use; vector<string> dynamicNames not available yet
     **/
    void setLoudnessMark(int dynamicNum, std::vector<std::string> dynamicNames);

    /**
     *  Assigns the loudness of a note
     *  \param sones the loudness of the note in Sones (0-256)
     *  \note this translates sones into "ff", "mf" , "pp", etc.
     **/
    void setLoudnessSones(float sones);

    /**
      *  Assigns any modifiers to the sound: expressive mark attached to notes:
      *   accent, espressivo, marcato, portato, staccatissimo, staccato, tenuto,
      *   prall, prallup, pralldown, upprall, downprall, prallprall, lineprall,
      *   prall mordent mordent upmordent downmordent trill turn reverseturn
      *   shortfermata, fermata, longfermata, verylongfermata, upbow, downbow,
      *   flageolet, open, halfopen, lheel, rheel, ltoe, rtoe, snappizzicato,
      *    stopped, segno, coda, varcoda (LyliPond markings)
      *  \param modNums
      *  \param modNames
     **/
    void setModifiers(std::vector<std::string> modNames);

 //   bool is_attach_mark(string mod_name);

    /**
     *  Gets the start time and duration of a sound,
uses validateSound
       and inserts each sound in a vector (insert_soundsOnly)
     *   \param startEDU, stime in EDUs
     *   \param durationEDU, duration of the sound in EDUs
    **/
    void collectSounds( string startEDU, string durationEDU);

  /**
    * When a note's duration is longer than barEDU, this function splits
    *  the middle part of the note (exclude the starting bar and ending bar)
    *: \param int bar_diff - difference between starting bar and ending bar
    *  \output: string contains notation of middle part of the note
    **/
    string split_bars(int bar_diff);

    /**
     *      not in use ?
     *
     * Converts duration (in EDU) to a string representing the type of the
     * note(s)
     * \param dur - duration of the note
     * \output: string
     **/
    string convert_dur_to_type(int dur);

    /**
     * Main function that set up all variables for output score
       \param none
       \output: none
     **/
    int notate(int in_tuple, int tuple_end);
    int notate_rest(int in_tuple, int tuple_end);

  /**
    * Some duration need two notes to notate, such notation is marked
    * starting with an '!'. This function read this mark and split it into
    * two notes.
    * \param s - string that holds the duration info. s will be modified
    * \param pitch_out - output pitch
    * \output: NONE
    **/
   void translate(string & s, string pitch_out);


   /**
    * Inserts the notes after processing including rests into a vector
    *  and keep the vector in time increasing order
    *  \input: Note * n - pointer to a note
    **/
  static void insert_soundsAndRests(Note * n);

  /**
  * Insertsindividual note into a vector and keeps the vector in time
  *  increasing order (for sounds only. This function is similar to prev
  *  one but inserts notes into another vector.
  *  \param Note * n - pointer to a note
  **/
  static void insert_soundsOnly(Note * n);


  /**
   * Checks if the gap between two notes(dur of a rest sign) is valid. If not,
   *  the beginning time of the second note will be modified to a valid value.
   *  Then each note is converted to a corresponding notation
   *								-shenyi
   **/
  static void addRests();

  /**
   *
   *  \param &time1  previous end time
   *  \param &time2  current start time
   *  \param loud    previous loudness
   **/
  static void validate_rests(int &time1, int & time2, string loud);

  /**
   * Checks if start time and end time are valid, if not, change them
   *  to the closest valid values.
   *  \param int &stime - (reference) start time to be verified
   *  \param int &endTime - (reference) endTime to be verified
   **/
   void validateSound(int &stime, int &endTime);

  /**
   *
   **/
   void validateSoundsAndRests ();

  /**
   * Creates a rest
   *  \parm stime
   *  \parm endTime
   *  \parm loud
   **/
  static Note * createRest (int stime, int endTime, string loud, int & next_st);

  /**
   * Checks if a value (stime, endTime, duration) matches a valid time
   *  point or duration (available in standard Western notation)
   *  \param value
   **/
  int checkValue(int & value);

  /**
   * Checks and adjusts if necessary a duration so that it can be
   *  expressed in standard Western notation.
   *  \param stime
   *  \param endtime
   **/
   void checkDuration (int & stime, int & endTime);

  /**
   * Finds the closest valid time to an existing invalid time
   *  \param remain
   *  \param dur
   *  \param beatNum
   **/
   int closestVal(int remain, int dur, int beatNum);

  /**
    *      not in use
    **/
    string split_internal(int dur);

  /**
   *            not in use
   **/
   bool need_split(int dur, int beatEDUs);

   /**
    *           not in use
    **/
    string split_head(int x);

   /**
    *           not in use
    **/
    string split_tail();

   /**
    *           not in use
    **/
    void split_within_bar();

   /**
    *           not in use
    **/
    void split_across_bar(int bar_diff);

    /**
     *          not in use
     **/
    string new_convert_dur_to_type(int dur);


    void verify_rests(int &time1, int &time2, string loud);




};

//extern string convert_dur_to_type(int dur);

extern int beatEDUs;

extern string timesignature;

//a vector holds the pointers to all the notes after processing
extern vector<Note*> soundsAndRests;

//a vector holds the pointers to all the sounds (no rests)
extern vector<Note*> soundsOnly;

//helper functions
extern int str_to_int(string s);

/**
 * Checks if input rational number is a power of 2
  \param temp - a rational number
  \outpu: true or false
**/
bool is_valid(Rational<int> temp);

 /*
  extern string restsign_dur_to_type(int start_time, int end_time, int dur,
   int tuplet, int tuplet_s_t);
  */


#endif /* NOTE_H */
