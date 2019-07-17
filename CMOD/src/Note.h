/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)


   Update:
   This class is not yet implemented in the XML version of CMOD.

                                            --Ming-ching Chiu May 06 2013



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
     *   Spells note attributes: start time, duration in fractions equivalent
     *   to Traditional notation note values and marking bar lines.
     *   Adds pitch, dynamics and playing techniques.
     *   \param
    **/
    void notateDurations( string aName, string startEDU, string durationEDU);



    /**
     * Main function that set up all variables for output score
       \param none
       \output: none
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
    *            not in use ?
    *
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

  void loudness_and_modifiers();

  /**
   * Checks if the gap between two notes(dur of a rest sign) is valid. If not,
   *  the beginning time of the second note will be modified to a valid value.
   *  Then each note is converted to a corresponding notation
   *								-shenyi
   **/
  static void make_valid();

  /**
   *
   *  \param &time1  previous end time
   *  \param &time2  current start time
   *  \param loud    previous loudness
   **/
  static void adjust_notes ();

  /**
   * Checks if time is valid, if it is not, change it to closest
   *  valid value
   *  \input: int &time - (reference) time to be verified
   **/
   void verify_valid(int &stime, int &endTime);

   //// ---- functions added by haorong at June 12 ---- ////

    void note_in_tuplet(int tup_type, int dur);

    /**
     * insert new note in to all_notes_bar in order.
     **/
    static void insert_note(Note* n);

    /**
      * close every vector in all_notes_bar with bar;
      **/
    static void add_bars();

    static void add_rests();

    static void free_all_notes();

    static void construct_tuplet_names(int uplimit);
    // ----------------------------------------------- ////


};

//extern string convert_dur_to_type(int dur);

extern int beatEDUs;

extern string timesignature;

//a vector holds the pointers to all the notes after processing
extern vector<Note*> all_notes;

//a vector holds the pointers to all the original notes (currently not in use)
//extern vector<Note*> all_notes_orig;

//a 2D vector holds the notes divided by bars(added by haorong at June 12)
extern vector<vector<Note*>*> all_notes_bar;



//helper functions
extern int str_to_int(string s);
extern string int_to_str(int n);
extern int power(int base, int p);
extern int check_pow(int dur);


/**
 * Checks if input rational number is a power of 2
  \param temp - a rational number
  \outpu: true or false
**/
bool is_valid(Rational<int> temp);

// print all the elements in all_notes vector
void print_all_notes();

// determine which type of tuplet will fit the input duration
// eg: if it belongs to a triplet return value will be 3
// -1 is return for invalid input
int determine_tuplet(int dur);
int check_lower(int value);

// generate the string for notes inside the tuplet
// sample output: r4~ r16
// string note_in_tuplet(int tup_type, int dur, string pitch);
// string note_in_tuplet(int tup_type, Note* n);

#endif /* NOTE_H */
