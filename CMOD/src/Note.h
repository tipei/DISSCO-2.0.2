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

    int start_t;
    int start_t_absolute;
    int end_t;

    int tuplet;
    string tuplet_name;
    int split;
    int diff, first_half_dur, last_half_dur, tuplet_1, tuplet_2;

    std::vector<std::string> modifiers_out;


    /**
     *  Simple constructor
     **/
    Note();

    /**
     *  Constructor with timespan and tempo
     **/
    Note(TimeSpan ts, Tempo tempo);

    /**
     *  Copy constructor
     **/
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
     * pitchNames: The names of the pitches (C, C#, D, Eb, ...) starting w C0=0
     * octavePitch number of the octave
     *  \param int pitchNum
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
     *   Spells note attributes: start time, duration in fractions equivalent
     *   to Traditional notation note values and marking bar lines.
     *   Adds pitch, dynamics and playing techniques.
     *   \param string aName
     *   \param atring startEDU
     *   \param string durationEDU
    **/
    void notateDurations( string aName, string startEDU, string durationEDU);


    /**
     * Main function that set up all variables for output score
     * \param: tuplet_dur
     * \output: none
     **/
    int notate(int tuplet);


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
    * Sorts the note into a vector and keep the vector in time
    *  increasing order
    *  \input: Note * n - pointer to a note
    **/
  static void sort_notes(Note * n);

  /**
   * Sorts individual note into a vector and keeps the vector in time
   *  increasing order (for original notes). This function is similar to prev
   *  one but sorts notes into another vector.
   *  \param Note * n - pointer to a note
   **/
  static void sort_notes_orig(Note * n);

  /**
   *  Prepares the loudness and modifiers notation
   **/
  void loudness_mark();
  void modifiers_mark();

  /**
   *  Processes the raw notes and gets them ready for LilyPond by calling 
   *   add_bars, add_rests, adjust_notes (and print_all_notes)
   *
   **/
  static void make_valid();

  /**
   *  Adjusts the durations modifying the way a sound is split among tuplets 
   *   if neccessary and gets them ready according to LilyPond sintax.
   **/
  static void adjust_notes ();

  /**
   *		obsolete
   *
   * Checks if time is valid, if it is not, change it to closest
   *  valid value
   *  \input: int &time - (reference) time to be verified
   **/
   void verify_valid(int &stime, int &endTime);



   //// ---- functions added by haorong at June 12 ---- ////

    /**
     *  Notates a note inside a tuplet
     *  \param int tup_type
     *  \param int dur
     **/
    void note_in_tuplet(int tup_type, int dur);

    /**
     * Inserst a new note into the all_notes_bar vector in order.
     * Determines the position of the note and splits it across bar lines
     * \param Note* n
     **/
    static void insert_note(Note* n);

    /**
     * Adds bars in the vector all_notes_bar
     **/
    static void add_bars();

    /**
     * Adds rests between notes.  The rest is ont processed yet and it could
     *  have an invalid duration
     **/
    static void add_rests();

    /**
     * Deletes all notes
     **/
    static void free_all_notes();

    /**
     * Initiate the tuplet_types according to the tuplet_limit
     * \param int uplimit
     **/
    static void construct_tuplet_names(int uplimit);

    // ----------------------------------------------- ////

};

  //extern string convert_dur_to_type(int dur);

  extern int beatEDUs;

  extern string timesignature;

  //a vector holds the pointers to all the notes after processing
  extern vector<Note*> all_notes;

  //a 2D vector holds the notes divided by bars(added by haorong at June 12)
  extern vector<vector<Note*>*> all_notes_bar;


  //		Helper functions

  /**
   * Converts a string to an integer
   *  eg: "123" ---> 123
   *  \paam: string s
   **/
  extern int str_to_int(string s);

  /**
   * Convers an integer to a string
   *  eg: 123 ---> "123"
   * \param: int n
   **/
  extern string int_to_str(int n);

  /**
   * Calculates base^p
   * \patam: int base
   * \param: int p
   **/
  extern int power(int base, int p);

  /*
   * Calculates log2(dur) returning -1 if dur is not a power of 2
   * \param: int dur
   **/
  extern int check_pow(int dur);


  /**
   * Checks if input rational number is a power of 2
   *  \param temp - a rational number
   *  \outpu: true or false
   **/
  bool is_valid(Rational<int> temp);

  /*
   * Prints all the elements in all_notes vector
   **/
  void print_all_notes();

  /** 
   * Determins which type of tuplet will fit the input duration
   *  eg: if it belongs to a triplet return value will be 3
   *  -1 is return for invalid input
  **/
  int determine_tuplet(int dur);

  /**
   * Finds the closest power of 2 which is less that value
   *  eg: if value == 7, return 4, if value == 9 return 8
   * \param: int value
   **/
  int check_lower(int value);

  // generate the string for notes inside the tuplet
  // sample output: r4~ r16
  // string note_in_tuplet(int tup_type, int dur, string pitch);
  // string note_in_tuplet(int tup_type, Note* n);

#endif /* NOTE_H */
