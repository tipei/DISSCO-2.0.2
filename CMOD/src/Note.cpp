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

string timesignature;
int beatEDUs;
static int sDiv;
static int eDiv;
static int sBar;
static int eBar;
static int barEDUs;
//a integer variable to indicate the previous tuplet type in notation loop
static int pre_tuplet = 0;
static string loudness_prev = "";
ofstream * outputFile;
ofstream * outscore; // not sure is in use
string outstring;
vector<Note*> all_notes;
// vector<Note*> all_notes_orig;

// 2D vector storing notes, every row is a single bar.
vector<vector<Note*>*> all_notes_bar;
// string array for tuplet names
// string tuplet_types[8] = {"invalid", "invalid", "invalid", "\\tuplet 3/2{","invalid", "\\tuplet 5/4 {", "\\tuplet 6/4 {", "\\tuplet 7/4 {"};//added by haorong
vector<string> tuplet_types;
// the note which represent one beat
static int unit_note;

// mark the highest tuplet type we will have (exclusive) eg: if we want up to 6-tuplet, tuplet_limit will be 7
static int tuplet_limit;
//----------------------------------------------------------------------------//

Note::Note(TimeSpan ts, Tempo tempo) : ts(ts), tempo(tempo),
  pitchNum(0), octaveNum(0), octavePitch(0), loudnessNum(0) {
}

//----------------------------------------------------------------------------//

Note::Note() {
}

//----------------------------------------------------------------------------//

Note:: ~Note() {
  delete outputFile;
  delete outscore;
}

//----------------------------------------------------------------------------//

Note::Note(const Note& other) {
  start_t = other.start_t;
  end_t = other.end_t;
  pitch_out = other.pitch_out;
  ts = other.ts;
  tempo = other.tempo;
  pitchNum = other.pitchNum;
  octaveNum = other.octaveNum;
  octavePitch = other.octavePitch;
  pitchName = other.pitchName;
  loudnessNum = other.loudnessNum;
  loudnessMark = other.loudnessMark;
  loudness_out = other.loudness_out;
  modifiers = other.modifiers;
}

//----------------------------------------------------------------------------//

bool Note::operator < (const Note& rhs) {
  //Sort notes by their *global* start time.
  return (ts.start < rhs.ts.start);
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

//cout << "Note::setLoudnessSones - loudnessMark=" << loudnessMark << endl;
  Output::addProperty("Loudness", loudnessMark);
  loudness_out = char(92) + loudnessMark;

}


//----------------------------------------------------------------------------//

bool is_attach_mark(string mod_name){
    for (int i=0; i< 40; i++)
        if (mod_name == modifiers[i])
            return true;
    return false;
}


//----------------------------------------------------------------------------//

void Note::setModifiers(vector<string> modNames) {
    for(int i = 0; i < modNames.size(); i++) {
    if (is_attach_mark(modNames[i])){
    string temp = "\\" + modNames[i];
    modifiers_out.push_back(temp);
    }
  }
}



//-------------------------------------------------------------

bool is_valid(Rational<int> temp){

  for (int i=0; i<10; i++){
  Rational<int> x(pow_of_2[i]);
  if (x == temp)
    return true;

  }
  return false;
}


//--------------------------------------------------------------

void Note::translate(string & s, string pitch_out){
  int i = 1;
  string ret = "";
  //get first half
  while (s[i]!=' '){
  ret = ret + s[i];
  i++;
  }
  i++;
  //get second half
  string t = s.substr(i);
  ret = ret + "~ " + pitch_out + t;
  s = ret;
}


//---------------------------------------------------------------------------//

void Note::sort_notes(Note * n){
  // check if the vector is empty
  if (all_notes.empty()){
      all_notes.push_back(n);
      return;
  }
  Note* cur;
  // find the position inside the vector
  vector<Note*>::iterator it;
  for (it = all_notes.begin(); it!=all_notes.end(); it++){
      cur = *it;
  //cout << "Note::sort_notes - cur->start_t=" << cur->start_t << " > "
  //   << "n->start_t=" << n->start_t << endl;
      if (cur->start_t > n->start_t){
        all_notes.insert(it, n);
        return;
      }
  }
  // insert the note
  all_notes.insert(it, n);


  return;
}


//---------------------------------------------------------------------------//

void Note::sort_notes_orig(Note * n){
  insert_note(n);
}

//---------------------------------------------------------------------------//

void Note::loudness_and_modifiers(){

  while (!this -> modifiers_out.empty()){
    this -> type_out += this -> modifiers_out.back() + " ";
    this -> modifiers_out.pop_back();
  }

  if (this -> loudness_out != loudness_prev && this -> pitch_out != "r"){
    //cout<< "pitch: "<< this->pitch_out << " current: " << this -> loudness_out << " previous: " << loudness_prev << endl;
  	this -> type_out += this -> loudness_out + " ";
  	loudness_prev = this -> loudness_out;
  }
}


//---------------------------------------------------------------------------//
// tuplet will be an integer range from 0 to beatEDUs, indicates the amount of sound needed to fill the previous tuplet.
// the function divide into 3 parts:
// 1. complete the previous beat (if not finished)
// 2. notate the notes in complete beats during this sound
// 3. notate the beginning of the uncomplete beat for this sound

// eg: if a sound start from 24 end at 135
//    prev op    part 1        part 2      part 3     next op
//  |_______|------------|----------------|------|_______________|
//  0       24           60               120    135            180
int Note::notate(int tuplet_dur){
  // the first part of notate function is to satisfy the demand from the previous tuplet.
  int dur = this -> end_t - this -> start_t;
  if(dur == 0){
    return tuplet_dur;
  }
  if(tuplet_dur > 0){
    // when the current note does not have enough duration for the tuplet,
    // the whole note will be fit in the tuplet, the return the remaining part of the tuplet
    if(dur < tuplet_dur){
      // string s1 = note_in_tuplet(pre_tuplet, dur, this->pitch_out);
      // this -> type_out += s1;
      // this -> loudness_and_modifiers();
      this -> note_in_tuplet(pre_tuplet, dur);
      return tuplet_dur - dur;
    }
    // even if the previous tuplet is an eighth note or sixteenth note,
    // it is still necessary to split part of the current note.
    if(pre_tuplet == 2 || pre_tuplet == 4){
      int unit = tuplet_dur / (beatEDUs/pre_tuplet);
      if(unit == 3){
        string s = int_to_str(unit_note * 2);
        this -> type_out += this -> pitch_out + s + ".";
      } else {
        string s = int_to_str(unit_note * pre_tuplet / unit);
        this -> type_out += this -> pitch_out + s;
      }
      if(dur > tuplet_dur){
        this -> type_out += "~ ";
      } else {
        this -> type_out += " ";
      }

    } else {
      // if the cur note has enough duration for the tuplet,
      // write in the lilypond and close the tuplet
      // this -> type_out += note_in_tuplet(pre_tuplet, tuplet_dur, this->pitch_out);
      this -> note_in_tuplet(pre_tuplet, tuplet_dur);
      if(dur > tuplet_dur){
        this -> type_out += "~ ";
      } else {
        // this -> loudness_and_modifiers();
      }
      this -> type_out += "} ";
    }
    dur -= tuplet_dur;
    tuplet_dur = 0;
  }

  int remaind = dur % beatEDUs;
  int mainDur = dur / beatEDUs;// * beatEDUs;
  // the second part is to notate the part during complete beats
  while (mainDur > 0){

    int p = check_pow(unit_note);
    while(p >= 0){
      int beats = power(2, p);
      if(mainDur >= beats){
        this -> type_out += this -> pitch_out + int_to_str(unit_note/beats);
        mainDur -= beats;
        if(mainDur >= beats/2 && beats >= 2){
          this -> type_out += ".";
          mainDur -= beats/2;
        }
        break;
      }
      p--;
    }

    if( mainDur > 0 ||remaind > 0){
      this -> type_out += "~ ";
    } else {
      if(this -> split == 1){
        this -> type_out += "~";
        this -> split = 0;
      }
      this -> type_out += " ";
      // this -> loudness_and_modifiers();
    }
  }

  // the third part is constructing tuplet to fit the rest part of the note
  if(remaind > 0){
    //TODO add tuplet in note type_out
    int tuplet_type = determine_tuplet(remaind);
    if(tuplet_type == 2 || tuplet_type == 4){
        if(remaind / (beatEDUs/tuplet_type) == 3){
          string s = int_to_str(unit_note * 2);
          this -> type_out += this -> pitch_out + s + ". ";
        } else {
          string s = int_to_str(unit_note * tuplet_type);
          this -> type_out += this -> pitch_out + s + " ";
        }
        tuplet_dur = beatEDUs - remaind;

    } else if(tuplet_type != -1){
        // string s1 = tuplet_types[tuplet_type];
        // string s2 = note_in_tuplet(tuplet_type, remaind, this->pitch_out);
        // this -> type_out += s1 + s2;
        this -> type_out += tuplet_types[tuplet_type];
        this -> note_in_tuplet(tuplet_type, remaind);
        tuplet_dur = beatEDUs - remaind;
    }
    pre_tuplet = tuplet_type;
    // this -> loudness_and_modifiers();

  }

  // if(this -> split == 1){
  //   this -> type_out += "~ ";
  //   this -> split = 0;
  // }

  return tuplet_dur;
}

//---------------------------------------------------------------------------//

// this is the function to rearrange the notes and have them ready for lilypond syntex
// added by Haorong
void Note::adjust_notes(){

  vector<Note*>::iterator it;
  pre_tuplet = 0;
  int tuplet = 0; // variable which indicates if a tuplet is needed
  for (it = all_notes.begin(); it!=all_notes.end(); it++){
      Note* cur = *it;

      // this part is to adjust note duration according to the tuplet type
      if(tuplet > 0){
        int duration = cur -> end_t - cur -> start_t;
        int dur_remainder = (duration) % beatEDUs;
        int dur_beats = (duration) / beatEDUs;
        int desire_type = pre_tuplet;//determine_tuplet(tuplet);
        int cur_type = determine_tuplet(dur_remainder);
        // the boolean flag is checking if the note have valid duration after split to the previous tuplet
        int temp = determine_tuplet(duration - tuplet);
        bool flag = (duration > tuplet) && (temp != -1);
        // if the current note's duration can not be fit in the request tuplet
        // find the closest type and change the end time of the note
        // also have to change the start time of the next note
        if((desire_type != cur_type) && !flag){
          int t = beatEDUs / desire_type;
          double a = (double)dur_remainder / (double)t;
          int best_fit = (int) round(a) * t;
          cur -> end_t = cur -> start_t + dur_beats * beatEDUs + best_fit;
          Note* next = *(it+1);

          // cout << "original : "<< next -> start_t << " current: " << cur -> end_t << endl;
          next -> start_t = cur -> start_t + dur_beats * beatEDUs + best_fit;
        }
      }
      // this is the part to force close the tuplet before the bar line
      // should not be useful if the code is correct
      if(cur -> type_out == "\\bar\"|\" \n" || cur -> type_out == " "){
        if(tuplet > 0){
          cur -> type_out = "} " + cur -> type_out;
          tuplet = 0;
        }
        continue;
      }

      // pass the note to notate function
      tuplet = cur -> notate(tuplet);
  }
  pre_tuplet = 0;
}


//---------------------------------------------------------------------------//
// this is the function to process the raw notes. have them ready for lilypond
// edited by Haorong
void Note::make_valid(){

  // adding bar lines
  add_bars();
  // adding rests
  add_rests();
  // adjusting the duration of notes and notate them
  adjust_notes();
  // print_all_notes();


  return;
}


//---------------------------------------------------------------------------//

// this function is to check if the input sound has valid start time and end time
void Note::verify_valid(int &stime, int &endTime) {
    // int start_time = stime % beatEDUs;
    // int end_time = endTime % beatEDUs;
    // // cout << "start time: " << start_time << " end time: " << end_time << endl;
    // // find the distance between start time and the closest valid time spot
    // int min_diff = valid_time[0] - start_time;
    // for(int i = 0; i < 19; i++){
    //   int diff = valid_time[i] - start_time;
    //   if(diff == 0){
    //     min_diff = 0;
    //     break;
    //   }
    //   if(abs(diff) < abs(min_diff)){
    //     min_diff = diff;
    //   }
    // }
    // stime += min_diff;
    //
    // // find the distance between end time and the closest valid time spot
    // min_diff = valid_time[0] - end_time;
    // for(int i = 0; i < 19; i++){
    //   int diff = valid_time[i] - end_time;
    //   if(diff == 0){
    //     min_diff = 0;
    //     break;
    //   }
    //   if(abs(diff) < abs(min_diff)){
    //     min_diff = diff;
    //   }
    // }
    // endTime += min_diff;
}


//---------------------------------------------------------------------------//


// this function is used to determine the type of the tuplet we need
// added by Haorong
int determine_tuplet(int dur){
  for(int i = 2; i < tuplet_limit; i++){
    if(dur % (beatEDUs/i) == 0){
      return i;
    }
  }
  // invalid tuplet
  return -1;
}


// this function is used to find the closest power of 2 which is less that value
// eg: if value == 7, return 4, if value == 9 return 8
int check_lower(int value){
  int two = 1;
  while(two <= value){
    two *= 2;
  }
  return two/2;
}
//---------------------------------------------------------------------------//

// helper function which convert the string to integer
// eg: "123" ---> 123
int str_to_int( string s)
{
  int temp = 0;
  for (int i=0; i< s.length(); i++){
        int x = int(s[i] - 48);
        temp = temp*10 + x;
  }
  return temp;
}

//----------------------------------------------------------------------------//

// helper function which convert the integer to string
// eg: 123 ---> "123"
string int_to_str(int n){
  stringstream ss;
  ss << n;
  return ss.str();
}

// helper functions which calculate base^p
int power(int base, int p){
  int output = 1;
  for(int i = 0; i < p; i++){
    output *= base;
  }
  return output;
}
//----------------------------------------------------------------------------//

// helper function which calculates log2(dur)
// returning -1 is dur is not a power of 2
int check_pow(int dur){
  if(dur % 2 != 0){
    return -1;
  }
  int check = 1;
  int count = 0;
  while(check < dur){
    check *= 2;
    count++;
    if(check == dur){
      return count;
    }
  }
  return -1;
}
//----------------------------------------------------------------------------//

// helper function which initiate the tuplet_types according to the tuplet_limit
void Note::construct_tuplet_names(int uplimit){
  for(int i = 0; i < uplimit; i++){
    int l = check_lower(i);
    string t = "\\tuplet ";
    t += int_to_str(i) + "/" + int_to_str(l) + "{ ";
    tuplet_types.push_back(t);
  }
}
//----------------------------------------------------------------------------//

void Note::notateDurations( string aName, string startEDU, string durationEDU)
{
  int stime, dur, endTime, bar, beat;
cout << "Note::notateDuration: " << aName << " " << durationEDU  << endl;
  // Get needed parameters
  string attributeName = aName;
  std::stringstream ss(startEDU);
  ss >> stime;

  std::stringstream ss1(durationEDU);
  ss1 >> dur;
  endTime = stime + dur;

  string barEDU_str = tempo.getEDUPerBar().toPrettyString();
  string beatEDU_str = tempo.getEDUPerTimeSignatureBeat().toPrettyString();
  unit_note = tempo.getTimeSignatureBeat().Den();

  timesignature = tempo.getTimeSignature();

  barEDUs = str_to_int( barEDU_str);
  beatEDUs = str_to_int( beatEDU_str);

  int i = 1;
  while (beatEDUs % i == 0){
    i++;
  }
  tuplet_limit = i;
  construct_tuplet_names(tuplet_limit);
  // check if start time and end time are valid
  //verify_valid(stime, endTime);
  //cout <<  " Note: start " << stime << " end " << endTime << endl;
  start_t = stime;
  end_t = endTime;

  //sort before processing
  sort_notes_orig(this);

  return;
}

//----------------------------------------------------------------------------//
// this function is used to insert Note into the vector all_notes_bar,
// it will determine the position of the Note, and split Note if it goes across the bar
void Note::insert_note(Note* n){
  // cout << "note's data: " << n -> pitch_out << " " << n -> start_t << " " << n -> end_t << endl;
  if (n -> end_t <= n -> start_t){
    // discard 0 duration sound
    return;
  }
  // int barLength = barEDUs;
  int barNum = n -> start_t / barEDUs;
  size_t vecSize = all_notes_bar.size();
  // if the insert note's position exceed the bound of the current vector
  // creat more room until we can fit in the note
  if(barNum >= vecSize){
    for(size_t i = vecSize; i <= barNum; i++){
      vector<Note*>* v = new vector<Note*>();
      Note* n = new Note();
      n -> start_t = barEDUs * i;
      n -> end_t = barEDUs * i;
      n -> type_out = " ";
      v -> push_back(n);
      all_notes_bar.push_back(v);
    }
  }
  int end = n->end_t / barEDUs;
  // if the inserted note is completely inside the bar, insert the note in the correct position
  if(end <= barNum){
    vector<Note*>::iterator it;
    for (it = all_notes_bar[barNum]->begin(); it!=all_notes_bar[barNum]->end(); it++){
        Note* cur = *it;
        if (cur->start_t > n->start_t){
          all_notes_bar[barNum]->insert(it, n);
          return;
      }
    }
    all_notes_bar[barNum]->insert(it, n);
  } else {

    // spliting the long note which is crossing the bar
    Note* second = new Note(*n);
    second -> start_t = (barNum+1) * barEDUs;
    n -> end_t = (barNum + 1) * barEDUs;
    n -> split = 1;

    vector<Note*>::iterator it;
    for (it = all_notes_bar[barNum]->begin(); it!=all_notes_bar[barNum]->end(); it++){
        Note* cur = *it;
        if (cur->start_t > n->start_t){
          all_notes_bar[barNum]->insert(it, n);
          insert_note(second);
          return;
      }
    }
    all_notes_bar[barNum]->insert(it, n);
    insert_note(second);
  }
  return;
}

// this the function used for adding bars in the vector all_notes_bar
void Note::add_bars(){
  cout <<  "adding bars!"  << endl;
  vector<vector<Note*>*>::iterator it;
  int i = 1;
  for (it = all_notes_bar.begin(); it!=all_notes_bar.end(); it++){
      vector<Note*>* cur = *it;
      Note* n = new Note();
      n -> start_t = barEDUs * i;
      n -> end_t = barEDUs * i;
      n -> type_out = "\\bar\"|\" \n";
      //create(barEDUs * i, "\\bar\"|\" \n");
      cur -> push_back(n);
      i++;
  }
}

// this function is used for adding rests between notes,
// the rest is unprocessed so it could have invalid duration
void Note::add_rests(){
  cout << "add rests!" << endl;
  size_t vecSize = all_notes_bar.size();

  for(size_t i = 0; i < vecSize; i++){
    vector<Note*>::iterator it;
    Note * prev = *(all_notes_bar[i]->begin());
    sort_notes(prev);
    Note * cur;

    for (it = all_notes_bar[i]->begin()+1; it!=all_notes_bar[i]->end(); it++){
        cur = *it;
        int gap = cur -> start_t - prev -> end_t;
        // a rest will be placed only if gap is more than half of the smallest non-zero valid duration
        if(gap > (beatEDUs / (tuplet_limit - 1) / 2)){
            Note* n = new Note();
            n -> start_t = prev -> end_t;
            n -> end_t = cur -> start_t;
            n -> pitch_out = "r";
            sort_notes(n);
        } else {

            if(it+1 == all_notes_bar[i]->end()){
              prev -> end_t = cur -> start_t;
            } else {
              cur -> start_t = prev -> end_t;
            }
        }
        sort_notes(cur);
        prev = cur;
    }
  }
}

void Note::free_all_notes(){

  vector<vector<Note*>*>::iterator it2;
  for (it2 = all_notes_bar.begin(); it2!=all_notes_bar.end(); it2++){
      vector<Note*>* cur = *it2;
      cur -> clear();
      delete cur;
  }
  all_notes.clear();
}

// helper function to print vector all_notes
void print_all_notes(){ //helper function added by Haorong
  vector<Note*>::iterator it;
  for (it = all_notes.begin(); it!=all_notes.end(); it++){
    Note* cur = *it;
    if (cur -> pitch_out == "r111" || cur -> type_out == "\\bar\"|\" \n" || cur -> type_out == " "){
      continue;
    }
/*
      cout << " pitch: " << cur -> pitch_out << " start time: " << cur -> start_t
        << " end_time: "<< cur -> end_t << " dur: " << cur -> end_t - cur -> start_t << endl;
*/
  }
}


// this is the part to notate inside the tuplet
// added by Haorong
void Note::note_in_tuplet(int tup_type, int dur){
  // int dur = this -> end_t - this -> start_t;
  string pitch = this -> pitch_out;
  int first = 1;
  // string output = "";

  int beat = dur / (beatEDUs / tup_type);
  int unit_in_tuplet = unit_note * check_lower(tup_type);
  int p = check_pow(unit_in_tuplet);
  while (beat > 0){

    int p = check_pow(unit_in_tuplet);
    while(p >= 0){
      int beats = power(2, p);
      if(beat >= beats){
        this -> type_out += this -> pitch_out + int_to_str(unit_in_tuplet/beats);
        beat -= beats;
        if(beat >= beats/2 && beats >= 2){
          this -> type_out += ".";
          beat -= beats/2;
        }
        break;
      }
      p--;
    }
    if (beat > 0){
      this -> type_out += "~ ";
    } else {
      this -> type_out += " ";
    }
    if(first == 1){
      this -> loudness_and_modifiers();
      first = 0;
    }
  }

}
