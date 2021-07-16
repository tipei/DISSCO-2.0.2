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

enum class NoteType {
  kNote,
  kRest,
  kBarline,
  kTimeSignature,
  kUnknown
};

/**
 *  Constructor
 *
 **/
class Note {
  friend class Section;
  friend class Bottom;

    		//Rhythm//

    //The timespan of the note.
    TimeSpan ts;

    // The ancestor event that carries the reference tempo for this Note.
    const Event* rootExactAncestor;

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

    std::vector<std::string> modifiers_out;

    NoteType type = NoteType::kUnknown;

  public:
    /**
     *  Simple constructor
     **/
    Note();

    /**
     *  Constructor with timespan and root exact ancestor
     **/
    Note(TimeSpan ts, const Event* root_exact_ancestor);

    /**
     *  Copy constructor
     **/
    Note(const Note& other);

    /**
     *  Comparison operator (to sort in a list)
     *  \param rhs the object to compare to (right hand side)
     **/
    bool operator< (const Note& rhs);

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

    /**
     * Get the text associated with this Note.
     * 
     * \return the text associated with this Note.
    **/
    const string& GetText() const;

};

#endif /* NOTE_H */
