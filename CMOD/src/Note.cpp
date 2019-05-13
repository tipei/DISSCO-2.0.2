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
ofstream * outputFile;
//extern ofstream * outputFile;
ofstream * outscore;
string outstring;
vector<Note*> all_notes;
vector<Note*> all_notes_orig;

//----------------------------------------------------------------------------//

Note::Note(TimeSpan ts, Tempo tempo) : ts(ts), tempo(tempo),
  pitchNum(0), octaveNum(0), octavePitch(0), loudnessNum(0) {

//if ( &outputFile == " ") {
//  outputFile = new std::ofstream;
//  outputFile->open( "score.ly", ios::app);
//}
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
  ts = other.ts;
  tempo = other.tempo;
  pitchNum = other.pitchNum;
  octaveNum = other.octaveNum;
  octavePitch = other.octavePitch;
  pitchName = other.pitchName;
  loudnessNum = other.loudnessNum;
  loudnessMark = other.loudnessMark;
  modifiers = other.modifiers;
}

//----------------------------------------------------------------------------//

bool Note::operator < (const Note& rhs) {
  //Sort notes by their *global* start time.
  return (ts.start < rhs.ts.start);
}

//----------------------------------------------------------------------------//

void Note::setPitchWellTempered(int pitchNum) {

//octaveNum = pitchNum / pitchNames.size();
//octavePitch = pitchNum % pitchNames.size();
//pitchName = pitchNames[octavePitch];
  octaveNum = pitchNum / 12;
//*notaFile << "Octave Number " << octaveNum << endl;
  octavePitch = pitchNum % 12;
  pitchName = pitchNames[octavePitch];


  pitch_out = OutNames[octavePitch];
  string signs[8] = {",,,",",,",",","","'","''","'''","''''"};
  string sign = signs[octaveNum];
  pitch_out = pitch_out + sign;

/*  -----------old code ---------------------
  if (octavePitch == 0) {
    pitchName = "C";
  } else if (octavePitch == 1) {
    pitchName = "C#";
  } else if (octavePitch == 2) {
    pitchName = "D";
  } else if (octavePitch == 3) {
    pitchName = "Eb";
  } else if (octavePitch == 4) {
    pitchName = "E";
  } else if (octavePitch == 5) {
    pitchName = "F";
  } else if (octavePitch == 6) {
    pitchName = "F#";
  } else if (octavePitch == 7) {
    pitchName = "G";
  } else if (octavePitch == 8) {
    pitchName = "G#";
  } else if (octavePitch == 9) {
    pitchName = "A";
  } else if (octavePitch == 10) {
    pitchName = "Bb";
  } else if (octavePitch == 11) {
    pitchName = "B";
  } else {
    cerr << "Invalid pitchNum or pitchName !" << endl;
  }
*/

  Output::addProperty("Pitch Number", pitchNum, "semitones");
  Output::addProperty("Pitch Name", pitchName);
  Output::addProperty("Octave Number", octaveNum);
  Output::addProperty("Pitch In Octave", octavePitch);

  //*outputFile<< setw(5) <<"Pitch " << pitchNum << setw(5) << octaveNum << setw(3)
    //   << pitchName;


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
  loudnessMark = dynamicNames[loudnessNum];
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

void Note::setModifiers(vector<string> modNames)
{
  for(int i = 0; i < modNames.size(); i++) {
//  cout << "Modifier" << i << "  " << modNames[i] << endl;
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

string Note::new_convert_dur_to_type(int dur){
  /* convert duration to type of notes */
  int temp;
  string buffer;
  Rational<int> ratio(dur, beatEDUs);
  tuplet = 0;
  Rational<int> denominator(timesignature[2] - '0', 1);
//cout << "		inside Note::new_convert" << endl;
  // regular notes
  if (is_valid(ratio)){
    Rational<int> type = denominator / ratio;
  return type.toPrettyString();
  }
  //dotted notes
  if (dur == (6 * beatEDUs))
  return "1.";
  if ((ratio.Num() == 3) && (ratio.Den()%2 == 0)) {
  temp = ratio.Den() * 2;
  Rational<int> x(temp);
  buffer = x.toPrettyString() + ".";
  return buffer;
  }

  // generate list of valid durations
  vector<int> durations;
  int accumulator = 1;
  while ((beatEDUs / accumulator) * accumulator) {
  durations.push_back(beatEDUs / accumulator);
  accumulator *= 2;
  }


  for (int i = 0; i < durations.size(); i++) {

  tuplet = 1;
  // getting tuplet name
  Rational<int> flipped = (ratio.Den());
  int flip_n = atoi(flipped.toPrettyString().c_str());
  int flip_d = atoi(flipped.toPrettyString().c_str()) - 1;
  Rational<int> tuplet(flip_n, flip_d);
  tuplet_name = tuplet.toPrettyString();
  if (durations[i] - dur < 0) {
    int single = beatEDUs * (timesignature[2] - '0') / flip_n;
    if ((dur % 20) != 0 && (durations[i] * 1.5 - dur) > 0 && ((durations[i] * 1.5 - durations[i - 1]) < 0)) {
      Rational<int> r(durations[i], beatEDUs * (timesignature[2] - '0'));
      Rational<int> final_r(r.Den(), 1);
      if (final_r.toPrettyString() == "240") return "16";
      return final_r.toPrettyString() + ".";
    }
    else if ((dur % 20) != 0 && single * 1.25 == dur) {
      Rational<int> r(beatEDUs, beatEDUs * (timesignature[2] - '0'));
      Rational<int> final_r(r.Den(), 1);
      return final_r.toPrettyString() + "~ bes''16";
    }
   // regular notes
      Rational<int> r(durations[i - 1], beatEDUs * (timesignature[2] - '0'));
      Rational<int> final_r(r.Den(), 1);
      return final_r.toPrettyString();
  }
  }

  return denominator.toPrettyString();
}


//---------------------------------------------------------------

string Note::convert_dur_to_type(int dur){
  /* convert duration to type of notes */
  int temp;
  string buffer;
  Rational<int> ratio(dur, beatEDUs);
  tuplet = 0;

  cout << "		inside Note::convert_dur_to_type" << endl;
  // regular notes
  if (is_valid(ratio)){
    Rational<int> four(4);
    Rational<int> type = four / ratio;
  return type.toPrettyString();
  }
  //dotted notes
  if (ratio == 6)
  return "1.";
  if ((ratio.Num() == 3) && (ratio.Den()%2 == 0)) {
  temp = ratio.Den() * 2;
  Rational<int> x(temp);
  buffer = x.toPrettyString() + ".";
  return buffer;
  }
  //other irregular notes
  for (int i=0; i<8; i++){
//	cout << dur << endl;
  Rational<int> x(ratios[i]);
  if (x == ratio){
    tuplet = 1;
    tuplet_name = tuplet_names[i];
    string type1 = types[i];
    if (type1[0] == '!')
      translate(type1, pitch_out);
cout <<"Note::convert_dur_to_type - dur=" << dur << " type: " << type1 << endl;
    return type1;
  }
  }
  return "4";

}


//------------?? this function is currently not used ?? ----------------------//
string convert(int dur){
  Rational<int> ratio(dur, beatEDUs);
  if (is_valid(ratio)){
    Rational<int> four(4);
    Rational<int> type = four / ratio;
  return "r" + type.toPrettyString();
  }
  for (int i=0; i<8; i++){
  Rational<int> x(rest_ratios[i]);
  if (x == ratio){
    return rest_signs[i];
  }
  }
  return "";
}


//----------------- this function is currently not used ---------------------//
string restsign_dur_to_type(int start_time, int end_time, int dur, int tuplet, int tuplet_s_t){
  int div = 0;
  int temp;
  int t_time = start_time;
  string buffer = "";
//  cout << "rest duration1: " << dur << endl;

  int sBeat = start_time / beatEDUs;
  int eBeat = end_time / beatEDUs;
  if (eBeat > sBeat){
  int dur1 = beatEDUs - (start_time % beatEDUs);
  buffer = buffer + convert(dur1) + " ";
  dur = dur - dur1;
  }
  while (dur > beatEDUs){
  buffer = buffer + "r4 ";
  dur -= beatEDUs;
  }
//  cout << "rest duration2: " << dur << endl;

  buffer = buffer + convert(dur) + " ";
  return buffer;
}


//-----------------------------------------------------------

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
  //cout << "Note::sort_notes - final: cur->start_t=" << cur->start_t << endl;

  return;
}


//---------------------------------------------------------------------------//

void Note::sort_notes_orig(Note * n){
  // check if the vector is empty
  if (all_notes_orig.empty()){
  all_notes_orig.push_back(n);
  return;
  }
  Note* cur;
  // find the position inside the vector
  vector<Note*>::iterator it;
  for (it = all_notes_orig.begin(); it!=all_notes_orig.end(); it++){
      cur = *it;
      if (cur->start_t > n->start_t){
/*
  cout << "Note::sort_notes_orig - cur->start_t=" << cur->start_t << " > "
       << "n->start_t=" << n->start_t << endl;
  cout <<"		nothing" << endl;
*/
    all_notes_orig.insert(it, n);
    return;
  }
  }
  // insert the note
  all_notes_orig.insert(it, n);
/*
  cout << "Note::sort_notes_orig - final: cur->start_t=" << cur->start_t
       << endl;
*/
  return;
}

//-------------- this function is currently not used ------------------------//
bool need_split(int dur, int beatEDUs){
  Rational<int> x(dur, beatEDUs);
  if (is_valid(x)) return false;
  return true;
}


//---------------- this function is currently not used ----------------------//
string Note::split_internal(int dur){
  string ret;
  ret = pitch_out + convert_dur_to_type(dur);
  return ret;

}

//---------------------------------------------------------------------------//

string Note::split_bars(int bar_diff){
  // calculate time signature
  Rational<int> r(barEDUs, beatEDUs);
  string temp, ret;
  ret = "";
  if (r == 4)
  temp = pitch_out + "1";
  if (r == 3)
  temp = pitch_out + "2~ " + pitch_out + "4";
  if (r == 5)
  temp = pitch_out + "1~ " + pitch_out + "4";
  while (bar_diff > 2){
  ret = ret + temp + "~ ";
  bar_diff --;
  }
  ret = ret + temp;
  return ret;
}


//------------------ this function is currently not used --------------------//
string Note::split_head(int x){ //parameter is to indicate if "~" is needed
  string s, t;
  s = "";
  Rational<int> r(beatEDUs-sDiv, beatEDUs);
  t = convert_dur_to_type(beatEDUs-sDiv);
  s = pitch_out + t;
  if (x == 1) s = s + "~";
  s = s + " ";
  if (tuplet == 1)
  s = s + " } " ;
  return s;
}


//-------------------- this function is currently not used ------------------//
string Note::split_tail(){
  string s, t;
  s = "";
  Rational<int> r(eDiv, beatEDUs);
  if (eDiv == 0)
  return "DNE";
  else {
  t = convert_dur_to_type(eDiv);
  if (tuplet == 1)
     s = "\\tuplet " + tuplet_name + " { " + pitch_out + t;
  else s = pitch_out + t;
  }
 // cout << s << endl;
  return s;
}


//--------------------- this function is currently not used -----------------//
void Note::split_within_bar(){
  string temp;
  int tie = 0;
  int x = 0;
  if (end_t > sBeat + beatEDUs) x = 1;
  int dur = eBeat - (sBeat + beatEDUs);
  if (dur >= beatEDUs) x = 1;
  if (eDiv > 0) x = 1;
  type_out = split_head(x);
  if (dur >= beatEDUs){
     temp = split_internal(dur);
     tie = 1;
  }
  else temp = " ";
  type_out = type_out + temp;
  if (split_tail()!="DNE"){
     if (tie == 1) type_out = type_out + "~";
     type_out = type_out + split_tail() + " ";
  }
//  cout <<"split within bar " << type_out << " " << endl;

}


//------------------ this function is currently not used --------------------//
void Note::split_across_bar(int bar_diff){
  string temp;
  int x = 0;
  int tie = 0;
  if (end_t > sBeat + beatEDUs) x = 1;
  type_out = split_head(x);
  int dur = (sBar + barEDUs) - (sBeat + beatEDUs);

  if (dur >= beatEDUs){
     temp = split_internal(dur);
     tie = 1;
  }
  else temp = " ";

  type_out = type_out + temp;
  if (bar_diff > 1){
     if (tie == 1) type_out = type_out + "~ ";
     type_out = type_out + split_bars(bar_diff);
     tie = 1;
  }
  dur = eBeat - eBar;

  if (dur >= beatEDUs){
     if (tie == 1) temp = "~ ";
     temp = temp + split_internal(dur);
     tie = 1;
  }
  else temp = " ";

  type_out = type_out + temp;
  if (split_tail()!="DNE"){
     if (tie == 1) type_out = type_out + "~ ";
     type_out = type_out + split_tail() + " ";
  }
//  cout << "split across bar " << type_out << endl;
}

//---------------------------------------------------------------------------//

void Note::verify_rests(int &time1, int & time2, string loud){
  int i;
  int flag = 1;
  int diff = time2 - time1;		//rest duration

//if (diff <= 0) return;			//if there is no rest, return
  if (diff == 0) {
    cout << "Output::verify_rests 1 - diff=" << diff << endl;
  //int sever; cin >> sever;
    return;
  } else if ( diff < 0) {
    cout << "Output::verify_rests 2 - diff=" << diff << endl;
   //nt sever; cin >> sever;
  }

  int remainder1 = time1 % beatEDUs;	//EDUs after beat; beginning of rest
  int beatNum = time1 / beatEDUs;
  int remainder2 = time2 % beatEDUs;  //EDUs after the beat end of rest
  Rational<int> rest(remainder1, beatEDUs);
  int denominator = rest.Den();		//tuplet type
  int factor = beatEDUs / denominator;	//EDUs in one tuplet unit
/*
  cout << "Output::verify_rests - time1=" << time1 << " time2=" << time2
  << " diff=" << diff << " beatEDUs=" << beatEDUs << endl;
  cout << " factor=" << factor << " denominator=" << denominator << " beatNum="
       << beatNum << " remainde1=" << remainder1 << " remainder2="
       << remainder2 << endl;
// int sever; cin >> sever;
*/
  //if the rest is longer than one beat or rest is across more than one beat
  if (remainder1 + diff > beatEDUs) flag = 0;

  for (i = 0; i<13; i++)
  if (valid_time[i] == diff)
     flag = 0;			//rest has a valid value

  //modifying the start time if rest is less than one beat
  if (flag == 1){			//rest has an invalid value
    cout << "		modifying start time ........" << endl;

    int temp = factor;

    //     TO DO
    //finding the closest time2 possible
    // check if there is a smaller subdivision available (eg. 10 instead of 20)

    while (temp < remainder2)
  temp += factor;
    if (temp - factor > remainder1)
  temp -= factor;
    time2 = beatNum * beatEDUs + temp;
   cout << "            time2=" << time2 << endl;
  }

  //if the rest is not 0
  if (diff > 0){
   Note * t = new Note();
   t->pitch_out = "r";
   t->start_t = time1;
   t->end_t = time2;
   t->loudness_out = loud;
     t->notate();
   //cout << "notate in verify rests" << endl;
  }

  return;
}


//---------------------------------------------------------------------------//

void Note::make_valid(){
  vector<Note*>::iterator it;
  Note * prev = *(all_notes_orig.begin());
  Note * cur;
  //prev->notate();

  for (it = all_notes_orig.begin()+1; it!=all_notes_orig.end(); it++){
        cur = *it;
  sBeat = cur->start_t / beatEDUs;
  eBeat = cur->end_t / beatEDUs;
/*
        cout << " " << endl;
  cout << "Output::make_validcp2: " << prev->end_t << " "
             << cur->start_t << endl;
*/
  verify_rests(prev->end_t, cur->start_t, prev->loudness_out);
/*
  cout << "   after verify_rests - cp3: " << prev->end_t << " "
             << cur->start_t << endl;
*/
  //cur->notate();
  prev = cur;
  }

  for (it = all_notes_orig.begin(); it!=all_notes_orig.end(); it++){
  cur = *it;
  cur->notate();
  //    cout << "  notate in make_valid" << endl;
  }

  return;
}


//---------------------------------------------------------------------------//

void Note::verify_valid(int &stime, int &endTime) {

  int closest_time = valid_time[0];             //closest valid_time
  int remainder[2];	//EDUs after beat (0: stime; 1: endTime)
  remainder[0] = stime % beatEDUs;
  remainder[1] = endTime % beatEDUs;

  int distance[2];	//distance to closest valid_time (0: start; 1: end)
  distance[0] = abs(valid_time[0]-remainder[0]);
  distance[1] = abs(valid_time[0]-remainder[1]);

  int beatNum[2];
  beatNum[0] = stime/beatEDUs;			//stime beat number
  beatNum[1] = endTime/beatEDUs;		//endTime beat number

  // check both stime and endTime
  for (int j=0; j<2; j++) {
/*
  cout << "Note::verify_valid - stime=" << stime << " distance[" << j <<"]="
       << distance[j] << " remainder[" << j << "]=" << remainder[j]
       << " endTime=" << endTime << endl;
*/
    //go through the list of valid EDU numbers
    for (int i=0; i<13; i++) {

      //find the appropriate closest_time
      if (abs(valid_time[i]-remainder[j] < distance[j]) && distance[j] !=0) {
  distance[j] = abs(valid_time[i]-remainder[j]);
//cout << "	distance[" << j << "]=" << distance[j] << endl;
        closest_time = valid_time[i];
      }
    }
     //cout << "closest_time=" << closest_time << " j=" << j  << endl;

    //Simpler case: stime
    if (j == 0) {
      stime = closest_time + beatNum[0] * beatEDUs;
      //cout << "	result stime=" << stime << endl;
    } else if (j == 1) {				//endTime case
      endTime = closest_time + beatNum[1] * beatEDUs;
      //cout << "	result endTime=" << endTime << endl;

      //if endTime is within the same beat as stime
      if (beatNum[0] == beatNum[1] + 1) {
  int length = endTime - stime;
        //cout << "@ length=" << length << endl;

        //find a duration that can be notated
        for (int i=0; i<13; i++) {

          if (valid_time[i] >= length)  {
   cout << "		valid_time[" << i << "]=" << valid_time[i]
        << " length=" << length << endl;

            while (length <= beatEDUs) {
              length += 1;

              if (length == valid_time[i]) {
                endTime = stime + valid_time[i];
                break;
              }
            }
          }
        }
        cout << "FINAL endTime=" << endTime  << " stime=" << stime << endl;
  int sever;
        if (endTime < stime) cin >> sever;
      }
    }
  }
}


//---------------------------------------------------------------------------//

void Note::notate(){
  // Barlines, beats, subdivisions
  int numSbar = start_t / barEDUs;		//start barline num
  sBar = numSbar * barEDUs ;			//start barline EDUs
  int numSbeat = (start_t - sBar) / beatEDUs;	//start beat num
  sBeat = start_t / beatEDUs * beatEDUs;	//start beat EDUs
  sDiv = start_t - sBeat;			//start subdivisions EDUs *(40)

                                               //beginning subdiv of dur
  int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - start_t) % beatEDUs;
  int numEbar = end_t / barEDUs;		//end barline num
  eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == end_t) eBar += 1;
  int numEbeat = (end_t - eBar) / beatEDUs;	//end beat num
  eBeat = end_t / beatEDUs * beatEDUs;		//end beat EDUs
  eDiv = end_t - eBeat;				//end subdivisions

  int dur = end_t - start_t;
  int durBeats = ((end_t - eDiv) - (start_t + sDiv)) / beatEDUs;
  int internal_s_time = sBeat + beatEDUs; //beat after start time
  int internal_dur = eBeat - sBeat;
  int bar_diff = numEbar - numSbar;

/*
  cout << "Note::notate - start_t=" << start_t << " dur=" << dur
       << " end_t=" << end_t << endl;
  int sever;
  if(dur < 0) cin >> sever;
*/

  //spilt long durations
  Note * n = new Note(*this);
  n->split = 0;
  n->tuplet = 0;
  n->type_out = "";
  n->start_t = sBeat+beatEDUs;
  n->end_t = eBeat;
  n->pitch_out = pitch_out;
  n->loudness_out = loudness_out;
  int diff = (eBeat-sBeat) / beatEDUs;
  //cout << "diff is " << diff << endl;
  int d = diff;
  int t = sBeat + beatEDUs;
//cout << "Note::notate " << n->pitchName << endl;
  while (d > 2){
  if (d > 5 && (barEDUs - t % barEDUs >= beatEDUs * 4)){
    n->type_out += "1~ ";
    n->type_out +=pitch_out;
    d = d-4;
    t = t + beatEDUs*4;
    continue;
  }
  if (d > 3 && (barEDUs - t % barEDUs >= beatEDUs * 2)){
    n->type_out += "2~ ";
    n->type_out +=pitch_out;
    d = d-2;
    t = t + beatEDUs*2;
    continue;
  }
  n->type_out += "4~ ";
  n->type_out +=pitch_out;
  t = t + beatEDUs;
  d--;
  }
  if (d==2 && eDiv!=0) n->type_out += "4~ ";
  if (d==2 && eDiv == 0) n->type_out += "4 ";
  if (d==1 && eDiv!=0) n->type_out += "1";
  if (d==1 && eDiv == 0) n->type_out += "1";

/*
  if (d==1 && eDiv!=0) n->type_out += "~ ";
  if (d==1 && eDiv == 0) n->type_out += " ";
*/
  if (diff > 1) {
//	cout << "note(long): " << (diff-1) * beatEDUs << " " << n->start_t << " " << n->end_t << endl;
  sort_notes(n);
  }


  //split head and tail
  Note * shalf;

  if ((diff == 0) || (diff == 1 && eDiv==0)){
     type_out = new_convert_dur_to_type(dur); //////////// changed //////////////
// cout << "dur" << dur;
// cout << "type_out" << type_out;
     split = 0;
  }
  else {
  shalf = new Note(*this);
  shalf->pitch_out = pitch_out;
  shalf->loudness_out;
  shalf->start_t = eBeat;
  shalf->end_t = end_t;
  shalf->split = 0;
  end_t = sBeat + beatEDUs;
  split = 1;
        //cout<< "firstpart";
  type_out = new_convert_dur_to_type(beatEDUs-sDiv); //////////// changed ////////////
// cout << "dur" << dur;
// cout << "type_out" << type_out;
  //cout<< "lastpart";
  shalf->type_out = shalf->new_convert_dur_to_type(eDiv); /////////// changed ////////////
// cout << "shalf dur" << eDiv;
// cout << "shalf type_out" << shalf->type_out;
    if (eDiv != 0) sort_notes(shalf);
  }
  sort_notes(this);
//cout << "	Note::notate - the END" << endl;
}


//---------------------------------------------------------------------------//
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

void Note::notateDurations( string aName, string startEDU, string durationEDU)
{
  int stime, dur, endTime, bar, beat;

  // Get needed parameters
  string attributeName = aName;
  std::stringstream ss(startEDU);
  ss >> stime;

  std::stringstream ss1(durationEDU);
  ss1 >> dur;
  endTime = stime + dur;

  string barEDU_str = tempo.getEDUPerBar().toPrettyString();
  string beatEDU_str = tempo.getEDUPerTimeSignatureBeat().toPrettyString();

  timesignature = tempo.getTimeSignature();

  barEDUs = str_to_int( barEDU_str);
  beatEDUs = str_to_int( beatEDU_str);

/*
  cout << "Note::notateDurations1 - beatEDUs=" << beatEDUs << " stime="
       << startEDU << "  dur=" << durationEDU <<" endtime=" << endTime << endl;
*/

  //check if start time and end time are valid
  verify_valid(stime, endTime);
  start_t = stime;
  end_t = endTime;
/*
  cout << "Note::notateDurations2 - start_t=" << start_t << " end_t="
  << end_t << " duration=" << end_t-start_t << endl;  cout<< endl;
*/
  int sever; if (dur <=0) cin >> sever;

  //sort before processing
  sort_notes_orig(this);
  //sort_notes(this);
  return;
}

/*==============================================================
//REST PART IS NOT EXECUTED

  // Barlines, beats, subdivisions
  int numSbar = stime / barEDUs;		//start barline num
  sBar = numSbar * barEDUs ;			//start barline EDUs
  int numSbeat = (stime - sBar) / beatEDUs;	//start beat num
  sBeat = stime / beatEDUs * beatEDUs;		//start beat EDUs
  sDiv = stime - sBeat;				//start subdivisions EDUs *(40)

                                               //beginning subdiv of dur
  int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - stime) % beatEDUs;
  int numEbar = endTime / barEDUs;		//end barline num
  eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == endTime) eBar += 1;
  int numEbeat = (endTime - eBar) / beatEDUs;	//end beat num
  eBeat = endTime / beatEDUs * beatEDUs;	//end beat EDUs
  eDiv = endTime - eBeat;			//end subdivisions

  int validSub[13] = {0, 10, 12, 15, 20, 24, 30, 36, 40, 45, 48, 50, 60};
  int internal_s_time = sBeat + beatEDUs; //beat after start time
  int internal_dur = eBeat - sBeat;
  int bar_diff = numEbar - numSbar;


  int split_flag = 0; //flag for split, 1 for yes, 0 for no
  if (bar_diff>0) split_flag = 1;
  if (need_split(dur, beatEDUs)) split_flag = 1;

  if (split_flag == 0) {
     type_out = convert_dur_to_type(dur);
     split = 0;
     sort_notes(this);
  cout << "not split " << type_out << endl;
     return;
  }

  split = 1;
  if (bar_diff == 0){
     split_within_bar();
     sort_notes(this);
     return;
  }

  split_across_bar(bar_diff);
  sort_notes(this);
  return;


  cout << "notateDurations - dur=" << dur << " start_t=" << start_t
       << " end_t=" << end_t << endl;


  Note * n = new Note(*this);
  n->split = 0;
  n->tuplet = 0;
  n->type_out = "";
  n->start_t = sBeat+beatEDUs;
  n->end_t = eBeat;
  n->pitch_out = pitch_out;
  n->loudness_out = loudness_out;
  int diff = (eBeat-sBeat) / beatEDUs;
  // cout << "diff is " << diff << endl;
  int d = diff;
  int t = sBeat + beatEDUs;
//cout << "n: " << n->pitch_out << endl;
  while (d > 2){
  if (d > 5 && (barEDUs - t % barEDUs >= beatEDUs * 4)){
    n->type_out += "1~ ";
    n->type_out +=pitch_out;
    d = d-4;
    t = t + beatEDUs*4;
    continue;
}
  if (d > 3 && (barEDUs - t % barEDUs >= beatEDUs * 2)){
    n->type_out += "2~ ";
    n->type_out +=pitch_out;
    d = d-2;
    t = t + beatEDUs*2;
    continue;
  }
  n->type_out += "4~ ";
  n->type_out +=pitch_out;
  t = t + beatEDUs;
  d--;
  }
  if (d==2 && eDiv!=0) n->type_out += "4~ ";
  if (d==2 && eDiv == 0) n->type_out += "4 ";
  if (d==1 && eDiv!=0) n->type_out += "~ ";
  if (d==1 && eDiv == 0) n->type_out += " ";
  if (diff > 1) {
  // cout << "note(long): " << (diff-1) * beatEDUs << " " << n->start_t << " " << n->end_t << endl;
  sort_notes(n);
  }



  Note * shalf;

  if ((diff == 0) || (diff == 1 && eDiv==0)){
     type_out = new_convert_dur_to_type(dur);  //////////// changed ////////////////
// cout << "dur" << dur;
// cout << "type_out" << type_out;
     split = 0;
  }
  else {
  shalf = new Note(*this);
  shalf->pitch_out = pitch_out;
  shalf->loudness_out;
  shalf->start_t = eBeat;
  shalf->end_t = end_t;
  shalf->split = 0;
  end_t = sBeat + beatEDUs;
  split = 1;
        // cout<< "firstpart";
  type_out = new_convert_dur_to_type(beatEDUs-sDiv); ////////// changed ////////////
// cout << "dur" << dur;
// cout << "type_out" << type_out;
  // cout<< "lastpart";
  shalf->type_out = shalf->new_convert_dur_to_type(eDiv); /////// changed /////////
// cout << "shalf dur" << eDiv;
// cout << "type_out" << shalf->type_out;
    if (eDiv != 0) sort_notes(shalf);
  }
  sort_notes(this);

  if (outstring!="")
  while (!modifiers.empty()){
    outstring = outstring + char(92) + modifiers.back();
    modifiers.pop_back();
  }

  // Duration
  spellingBee.push_back(numSbar);
  spellingBee.push_back(numSbeat);
  spellingBee.push_back(sDiv);

  spellingBee.push_back(eBegDiv);		//3
  spellingBee.push_back(numEbeat);
  spellingBee.push_back(eDiv);
  spellingBee.push_back(numEbar);

// Print -  OLD CODE for textfile output ---  not in use --------------------//


  *outputFile << "=========================== " << attributeName;
  *outputFile << " ==========================" << endl;
  *outputFile << "  stime=" << stime << " dur=" << dur << " endTime=" << endTime
       << endl;
  *outputFile << "   barEDUs=" << barEDUs << " beatEDUs=" << beatEDUs <<  endl;

  *outputFile << "sBar=" << sBar << " numSbar=" << numSbar << endl;
  *outputFile << " sBeat=" << sBeat << " numSbeat=" << numSbeat << " sDiv="
       << sDiv << endl;
  *outputFile << "eBar=" << eBar << " numEbar=" << numEbar << endl;
  *outputFile << " eBeat=" << eBeat << " numEbeat=" << numEbeat << " eDiv="
       << eDiv << endl;

  *outputFile << " " << endl;
  *outputFile << "Bar " << spellingBee.at(0) << " [+"
              << spellingBee.at(1) << "+" << spellingBee.at(2) << "/"
              << beatEDUs << "]" << setw(5) << spellingBee.at(3) << "/"
              << beatEDUs << setw(8) << "Beat "<< spellingBee.at(4)
              << setw(5)<< spellingBee.at(5) << "/" << beatEDUs << setw(13)
              << "Bar " << spellingBee.at(6) << endl;
*/
