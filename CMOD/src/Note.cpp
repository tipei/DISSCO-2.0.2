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
//   Note.cpp
//
//----------------------------------------------------------------------------//

#include "Note.h"
#include "Event.h"
#include "Output.h"
#include "Rational.h"
#include "tables.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

//----------------------------------------------------------------------------//

Note::Note(TimeSpan ts, const Event* root_exact_ancestor) : ts(ts), rootExactAncestor(root_exact_ancestor),
  pitchNum(0), octaveNum(0), octavePitch(0), loudnessNum(0), type(NoteType::kUnknown) {
}

//----------------------------------------------------------------------------//

Note::Note() : type(NoteType::kUnknown) {
}

//----------------------------------------------------------------------------//

Note::Note(const Note& other) {
  start_t = other.start_t;
  end_t = other.end_t;
  pitch_out = other.pitch_out;
  ts = other.ts;
  rootExactAncestor = other.rootExactAncestor;
  pitchNum = other.pitchNum;
  octaveNum = other.octaveNum;
  octavePitch = other.octavePitch;
  pitchName = other.pitchName;
  loudnessNum = other.loudnessNum;
  loudnessMark = other.loudnessMark;
  loudness_out = other.loudness_out;
  modifiers = other.modifiers;
  type = other.type;
}

//----------------------------------------------------------------------------//

bool Note::operator < (const Note& rhs) {
  //Sort notes by their *global* start time.
  return (ts.start < rhs.ts.start);
}

//----------------------------------------------------------------------------//

void Note::setStartTime(int start_time) {
  start_t = start_time;
}

//----------------------------------------------------------------------------//

void Note::setEndTime(int end_time) {
  end_t = end_time;
}

//----------------------------------------------------------------------------//

void Note::setPitchWellTempered(int pitchNum) {
  octaveNum = pitchNum / 12;

  octavePitch = pitchNum % 12;
  pitchName = pitchNames[octavePitch];


  pitch_out = OutNames[octavePitch];
  string signs[8] = {",,,",",,",",","","'","''","'''","''''"};
  string sign = signs[octaveNum];
  pitch_out = pitch_out + sign;


  Output::addProperty("Pitch Number", pitchNum, "semitones");
  Output::addProperty("Pitch Name", pitchName);
  Output::addProperty("Octave Number", octaveNum);
  Output::addProperty("Pitch In Octave", octavePitch);
}

//----------------------------------------------------------------------------//

int Note::HertzToPitch(float freqHz) {

  int pitchNum;

  if ( freqHz >= CEILING || freqHz <= MINFREQ) {
    cerr << "WARNING: frequency out of range" << endl;
  }

  pitchNum = rint(12 * log2(freqHz / C0));

  return pitchNum;
}

//----------------------------------------------------------------------------//

void Note::setLoudnessMark(int dynamicNum, vector<string> dynamicNames) {
  loudnessNum = dynamicNum;
  loudnessMark = dynamicNames[loudnessNum];  // int dur[10] = {2, 4, 6, 8, 10, 16, 18, 32, 64, 80};
  // int note = 23;
  // int unit_note = 8;
  // string pitch = "c";
  // string output = "";
  // while(note > 0){
  //   int power = check_pow(unit_note);
  //   while(power >= 0){
  //     int beats = po(2, power);
  //     if(note >= beats){
  //       output += pitch + int_to_str(unit_note/beats);
  //       note -= beats;
  //       if(note >= beats/2){
  //         output += ".";
  //         note -= beats/2;
  //       }
  //       output += " ";
  //       break;
  //     }
  //     power--;
  //   }
  // }
  // cout << output << endl;
  // return 0;
  Output::addProperty("Dynamic", loudnessMark);
  Output::addProperty("Dynamic Level", loudnessNum);
}

//----------------------------------------------------------------------------//

void Note::setLoudnessSones(float sones) {
  loudnessNum = -1;
  // cout << " sones: " << sones << endl;
  if(sones < 0 || sones > 256) {
    cerr << "Note received invalid value for sones!" << endl;
    exit(1);
  } else if(sones <= 4) {
    loudnessMark = "ppp";
  } else if(sones <= 8) {
    loudnessMark = "pp";
  } else if(sones <= 16) {
    loudnessMark = "p";
  } else if(sones <= 32) {
    loudnessMark = "mp";
  } else if(sones <= 45) {
    loudnessMark = "mf";
  } else if(sones <= 64) {
    loudnessMark = "f";
  } else if(sones <= 128) {
    loudnessMark = "ff";
  } else if (sones <= 256) {
    loudnessMark = "fff";
  }

  // for particel
  Output::addProperty("Loudness", loudnessMark);
  loudness_out = char(92) + loudnessMark;

}


//----------------------------------------------------------------------------//

bool is_attach_mark(string mod_name){
    for (int i=0; i< 39; i++)
        if (mod_name == modifiers[i])
            return true;
    return false;
}

// bool is_attach_mark(string mod_name){
//     for (int i=0; i< 40; i++)
//         if (mod_name == modifiers[i])
//             return true;
//     return false;
// }

//----------------------------------------------------------------------------//

void Note::setModifiers(vector<string> modNames) {
    for(int i = 0; i < modNames.size(); i++) {
    if (is_attach_mark(modNames[i])){
    string temp = "\\" + modNames[i];
    modifiers_out.push_back(temp);
    }
  }
}

const string& Note::GetText() const {
  return type_out;
}
