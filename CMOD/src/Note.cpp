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
ofstream * outscore;
string outstring;
vector<Note*> soundsAndRests;
vector<Note*> soundsOnly;

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

  //Output::addProperty("Loudness", loudnessMark);
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

//check if is power of 2
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
  if (pitch_out == "r")
    ret = ret + " " + pitch_out + t;
  else
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
  //cout << "		inside Note::new_convert_dur_to_type" << endl;

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
//in use
string Note::convert_dur_to_type(int dur){
  /* convert duration to type of notes */
  int temp;
  string buffer;
  Rational<int> ratio(dur, beatEDUs);
  cout << ratio.toPrettyString() << endl;
  tuplet = 0;

  // regular notes
  if (is_valid(ratio)){
    Rational<int> four(4);
    Rational<int> type = four / ratio;
  return type.toPrettyString();
  }
  //dotted notes
  if (ratio == 6)
  return "1.";
  if ((ratio.Num() == 3) && (ratio.Den()%2 == 0) && (is_valid(ratio.Den()))) {
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
//cout <<"Note::convert_dur_to_type - dur=" << dur << " type: " << type1 << endl;
    return type1;
  }
  }
  if (dur <= 15)
    return "16";
  else if (dur <= 30)
    return "8";
  else return "4";

}


//-----------------------------------------------------------

void Note::insert_soundsAndRests(Note * n){
/*  commented out the printing vector part
if (!soundsAndRests.empty()) {
cout << "----------------------" << endl;
cout << "In insert_soundsAndRests -- begin printing vector: " << endl;
	vector<Note*>::iterator it2;

	Note * c;

	for (it2 = soundsAndRests.begin(); it2 != soundsAndRests.end(); it2++) {
	  c = *it2;
	  cout << "note: " << c->start_t <<"  " <<  c->end_t << " " << c->pitch_out << c->type_out << endl;

	}


cout << "end printing vector" << endl;
cout << "---------------------" << endl;
}
*/

  // check if the vector is empty
  if (soundsAndRests.empty()){
  soundsAndRests.push_back(n);
  return;
  }
  Note* cur;
  // find the position inside the vector
  vector<Note*>::iterator it;
  for (it = soundsAndRests.begin(); it!=soundsAndRests.end(); it++){
      cur = *it;

      if (cur->start_t > n->start_t){
    soundsAndRests.insert(it, n);
    return;
  }
  }
  // insert the note
  soundsAndRests.insert(it, n);

  return;
  //cout << "Note::sort_notes - final: cur->start_t=" << cur->start_t << endl;
}


//---------------------------------------------------------------------------//

void Note::insert_soundsOnly(Note * n){

  // check if the vector is empty
  if (soundsOnly.empty()){
    soundsOnly.push_back(n);
//cout << "insert_soundsOnly empty vector: " << n->start_t << endl;
    return;
  }

  Note* cur;

  // find the position inside the vector
  vector<Note*>::iterator it;
  for (it = soundsOnly.begin(); it!=soundsOnly.end(); it++){
    cur = *it;
//cout << "insert_soundsOnly: " << cur->start_t << endl;
    if (cur->start_t > n->start_t){
      soundsOnly.insert(it, n);
//cout << "INSERTED: " << n->start_t << endl;

      return;
    }
  }

  soundsOnly.insert(it, n);
//cout << "insert_soundsOnly ending: " << n->start_t << endl;
//cout << "------------------------------------" << endl;
  return;
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
/*
  while (bar_diff > 2){
  ret = ret + temp + "~ ";
  bar_diff --;
  }
*/
  ret = ret + temp;
  return ret;
}


//---------------------------------------------------------------------------//

void Note::validate_rests(int &time1, int & time2, string loud){
  int i;
  int flag = 1;				//valid=0, invalid=1 rest value
  int diff = time2 - time1;		//rest duration

  if (diff <= 0) {
    if (diff < 0) {
     cout << "!!!! Note::validate_rests - diff=" << diff << " time1="
          << time1 << " time2=" << time2 << endl;
    int sever; cin >> sever;
    }
     return;		//if there is no rest, return
  }

  int remainder1 = time1 % beatEDUs;	//EDUs after beat; beginning of rest
  int beatNum = time1 / beatEDUs;
  int remainder2 = time2 % beatEDUs;  	//EDUs after the beat end of rest
  Rational<int> rest(remainder1, beatEDUs);
  int denominator = rest.Den();		//tuplet type
  int factor = beatEDUs / denominator;	//EDUs in the tuplet unit

  //if the rest is longer than one beat or rest is across more than one beat
  if (remainder1 + diff > beatEDUs) flag = 0;

  for (i = 0; i<13; i++)
  if (valid_time[i] == diff)
     flag = 0;			//rest has a valid value

  //modifying the start time if rest is less than one beat
  if (flag == 1){			//rest has an invalid value
    cout << "		modifying start time ........" << endl;
    int temp = factor;
    cout << "validate_rests - time1=" << time1 << " time2=" << time2
         << " diff=" << diff << endl;

    while (temp < remainder2)
      temp += factor;
    if (temp - factor > remainder1)
      temp -= factor;
    time2 = beatNum * beatEDUs + temp;
    cout << "            new time2=" << time2 << endl;
/*
check if valid_times less than beatEDUs-reminder1 plus reminder1 are
valid(_times); then compare them with reminder2 and choose the closest to it.

  int keep[13]

  for (i = 0; i<13; i++) {
    check = valid_time[i];
    while (check <= beatEDUs - reminder1) {
      for (j = 0; j<13; j++) {
        if (reminder1 + check == valid_time[i]) {
          keep[j] = check;
        }
      }
    }
  }
*/
  }

  //if the rest is not 0
  if (diff > 0){
   Note * t = new Note();
   t->pitch_out = "r";
   t->start_t = time1;
   t->end_t = time2;
   t->loudness_out = loud;
//   cout << "	 validate rests" << endl;
//     t->notate();
   //cout << "notate in validate rests" << endl;
  }

  return;
}


//---------------------------------------------------------------------------//

void Note::addRests(){
  vector<Note*>::iterator it;
  Note * prev = *(soundsOnly.begin());
  Note * cur;

  for (it = soundsOnly.begin()+1; it!=soundsOnly.end(); it++){
    cur = *it;
    validate_rests(prev->end_t, cur->start_t, prev->loudness_out);
    prev = cur;
  }

  prev=0;	//.........

  //    cout << "  notate in addRests" << endl;
//  cout << "  addRests" << endl;
  for (it = soundsOnly.begin(); it!=soundsOnly.end(); it++){
    cur = *it;

//:::::::::::
  int pre = 0;
  int dif= pre - cur->start_t;

        if (dif > 0) {

//  cout << "   after validate_rests - cur->start_t=" << cur->start_t
//       << " cur->end_t=" << cur->end_t << " pre=" << pre
//            << " dif=" << dif << endl;
  pre = cur->end_t;
         }
//::::::::::::::
//    cur->notate();
  }

  return;
}

//---------------------------------------------------------------------------//

void Note::verify_rests(int &time1, int & time2, string loud){
  int i;
  int flag = 1;
  int diff = time2 - time1;		//rest duration

//if (diff <= 0) return;			//if there is no rest, return
  if (diff == 0) {
//    cout << "Output::verify_rests 1 - diff=" << diff << endl;
  //int sever; cin >> sever;
    return;
  } else if ( diff < 0) {
//    cout << "Output::verify_rests 2 - diff=" << diff << endl;
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
//    cout << "		modifying start time ........" << endl;

    int temp = factor;

    //     TO DO
    //finding the closest time2 possible
    // check if there is a smaller subdivision available (eg. 10 instead of 20)

    while (temp < remainder2)
  temp += factor;
    if (temp - factor > remainder1)
  temp -= factor;
    time2 = beatNum * beatEDUs + temp;
//   cout << "            time2=" << time2 << endl;
  }

  //if the rest is not 0
  if (diff > 0){
   Note * t = new Note();
   t->pitch_out = "r";
   t->start_t = time1;
   t->end_t = time2;
   t->loudness_out = loud;
//     t->notate();
   //cout << "notate in verify rests" << endl;
  }

  return;
}


//---------------------------------------------------------------------------//

void Note::validateSound(int &stime, int &endTime) {

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

    //go through the list of valid EDU numbers
    for (int i=0; i<13; i++) {

      //find the appropriate closest_time
      if (abs(valid_time[i]-remainder[j] < distance[j]) && distance[j] !=0) {
  distance[j] = abs(valid_time[i]-remainder[j]);
        closest_time = valid_time[i];
      }
    }

    //Simpler case: stime
    if (j == 0) {
      stime = closest_time + beatNum[0] * beatEDUs;
    } else if (j == 1) {				//endTime case
      endTime = closest_time + beatNum[1] * beatEDUs;

      if (beatNum[0] == beatNum[1] + 1) {
        int length = endTime - stime;

        //find a duration that can be notated
        for (int i=0; i<13; i++) {

          if (valid_time[i] >= length)  {
            while (length <= beatEDUs) {
              length += 1;

              if (length == valid_time[i]) {
                endTime = stime + valid_time[i];
                break;
              }
            }
          }
        }
 int sever;
        if (endTime < stime) cin >> sever;
      }
    }
  }
}


//---------------------------------------------------------------------------//

int Note::notate(int in_tuple, int tuple_end){
  // Barlines, beats, subdivisions
  int numSbar = start_t / barEDUs;		//start barline num
  sBar = numSbar * barEDUs ;			//start barline EDUs
  int numSbeat = (start_t - sBar) / beatEDUs;	//start beat num
  sBeat = start_t / beatEDUs * beatEDUs;	//start beat EDUs
  sDiv = start_t - sBeat;			//start subdivisions EDUs

                                               //beginning subdiv of dur
  //int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - start_t) % beatEDUs;
  int numEbar = end_t / barEDUs;		//end barline num
  eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == end_t) eBar += 1;
  int numEbeat = (end_t - eBar) / beatEDUs;	//end beat num
  eBeat = end_t / beatEDUs * beatEDUs;		//end beat EDUs
  eDiv = end_t - eBeat;				//end subdivisions

  int dur = end_t - start_t;
  int bar_diff = numEbar - numSbar;
  //int durBeats = ((end_t - eDiv) - (start_t + sDiv)) / beatEDUs;
  //int internal_s_time = sBeat + beatEDUs; //beat after start time
  //int internal_dur = eBeat - sBeat;


  cout << "Note::notate - start_t=" << start_t << " dur=" << dur
       << " end_t=" << end_t << endl;
  cout << in_tuple <<endl;

  //spilt long durations
  Note * n = new Note(*this);
  n->split = 0;
  n->tuplet = in_tuple;
  n->type_out = "";
  n->start_t = sBeat+beatEDUs;
  n->end_t = eBeat;
  n->pitch_out = pitch_out;
  n->loudness_out = loudness_out;
  int diff = (eBeat-sBeat) / beatEDUs; // number of beats to tie
  int d = diff; // counter variable
  int t = sBeat + beatEDUs; // marker for most recently notated beat
cout << "diff is: " << diff << endl;

 Rational<int> sig(timesignature[0], timesignature[2]); // time signature (simplified) as a Rational

 /*
  * simplified time signature:
  * 4/4 -> 1/1
  * 3/4 -> 3/4
  * 6/8 -> 3/4
  * etc...
  */

 char orig_den = timesignature[2] - '0'; // get time signature's denominator value. since time signature is stored as a number, we do (- '0') to convert it into a character that can be used for printing and output.
 Rational<int> den(sig.Den(), 1); // denominator of simplified time signature, used for whole notes
 Rational<int> den2(sig.Den() * 2, 1); // 2 * denominator of simplified time signature, used for half notes

/* ************************************commented out by shenyi 09-30-2018
 while (d > 2) {
   if (d > (timesignature[0] + 1) && (barEDUs - t % barEDUs >= beatEDUs * 4)) { // not sure why (barEDUs - t ...) is used. taken from previous version
      if (timesignature[0] % 2 == 0) { // for regular note lengths
	n->type_out += den.toPrettyString() + "~";
	n->type_out += pitch_out;
	t = t + beatEDUs * (timesignature[0]); // update most recently notated
	d -= (timesignature[0] + 1); // update number of beats left to tie
      } else { // irregular note lengths; probably needs to be fixed
	       // notates one beat at a time; change to multiple?
	int nn = 0;
	while (nn < timesignature[0]) {
	  Note * n = new Note(*this);
          n->split = 0;
          n->tuplet = 0;
          n->type_out = orig_den + "~" + pitch_out;
          n->start_t = sBeat+beatEDUs;
          n->end_t = eBeat;
          n->pitch_out = pitch_out;
          n->loudness_out = loudness_out;
	  nn++;
	  d -= 1;
	}
	t += beatEDUs * timesignature[0];
      }
   }
   if (d > ((timesignature[0]) / 2 + 1) && (barEDUs - t % barEDUs >= beatEDUs * 2)) { // not sure why (barEDUs - t ... is used. taken from previous version
     n->type_out += den2.toPrettyString() + "~";
     n->type_out += pitch_out;
     d -= 2;
   }
   n->type_out += orig_den + "~";
   n->type_out += pitch_out;
   d--;

 }

  if (d==2 && eDiv!=0) n->type_out += orig_den + "~ ";
  if (d==2 && eDiv == 0) n->type_out += orig_den + " ";
  if (d==1) n->type_out += den.toPrettyString();

  if (d==1 && eDiv!=0) n->type_out += "1";
  if (d==1 && eDiv == 0) n->type_out += "1";


  if (d==1 && eDiv!=0) n->type_out += "~ ";
  if (d==1 && eDiv == 0) n->type_out += " ";
**************************************************************** */
// the code to replace the commented part
int flag_connect = 0;

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
//    cout << "note(long): " << (diff-1) * beatEDUs << " " << n->start_t << " " << n->end_t << endl;
    insert_soundsAndRests(n);
}
/////////////////////////////////////////////////////////////////////////////////////

  //split head and tail
  Note * shalf = NULL;
  int return_time = 0;  //value of end time of current tuplet, 0 if not in tuplet

 // case 1: no need to split
  if ((diff == 0) || (diff == 1 && eDiv==0)){
    type_out = convert_dur_to_type(dur);
        if (in_tuple == 1)
          tuplet = 1;
        if (in_tuple == 0 && tuplet == 1)
          return_time = start_t + 60;
//     type_out = new_convert_dur_to_type(dur); //////////// changed //////////////
    if (diff > 1) {
      this -> type_out += "~";
      flag_connect = 1;
    }
     split = 0;
  }
  else {   //case 2: need to split
  shalf = new Note(*this);
  shalf->pitch_out = pitch_out;
  shalf->loudness_out = loudness_out;
  shalf->start_t = eBeat;
  shalf->end_t = end_t;
  shalf->split = 0;
  end_t = sBeat + beatEDUs;
  split = 1;
  type_out = convert_dur_to_type(beatEDUs-sDiv);
  if (in_tuple == 1){
      tuplet = 1;
      shalf->tuplet = 1;
    }
  if (diff > 1 && flag_connect == 0 && pitch_out!="r") {
    this -> type_out += "~";
    flag_connect = 1;
  }
//  type_out = new_convert_dur_to_type(beatEDUs-sDiv); //////////// changed ////////////
  shalf->type_out = shalf->convert_dur_to_type(eDiv);
  if (in_tuple == 1 && shalf->end_t <= tuple_end )
    shalf->tuplet = 1;
  if (diff <= 1 && tuplet == 1)
    shalf->tuplet = 1;
//  shalf->type_out = shalf->new_convert_dur_to_type(eDiv); /////////// changed ////////////
  if ((eDiv != 0) && (shalf->end_t - shalf->start_t >= 10)) {
    insert_soundsAndRests(shalf);
    if (flag_connect == 0) this -> type_out += "~";
  }
 }
  insert_soundsAndRests(this);
  if ((in_tuple == 0) && (tuplet == 1))
    return start_t + 60;
  else if ((shalf!= NULL) && (shalf->tuplet == 1) && (in_tuple == 0))
    return shalf->start_t + beatEDUs;
  else return return_time;
}

//---------------------------------------------------------------------------//
//similar functionality to notate(), but for rest signs
int Note::notate_rest(int in_tuple, int tuple_end){
  // Barlines, beats, subdivisions
  int numSbar = start_t / barEDUs;		//start barline num
  sBar = numSbar * barEDUs ;			//start barline EDUs
  int numSbeat = (start_t - sBar) / beatEDUs;	//start beat num
  sBeat = start_t / beatEDUs * beatEDUs;	//start beat EDUs
  sDiv = start_t - sBeat;			//start subdivisions EDUs

                                               //beginning subdiv of dur
  //int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - start_t) % beatEDUs;
  int numEbar = end_t / barEDUs;		//end barline num
  eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == end_t) eBar += 1;
  int numEbeat = (end_t - eBar) / beatEDUs;	//end beat num
  eBeat = end_t / beatEDUs * beatEDUs;		//end beat EDUs
  eDiv = end_t - eBeat;				//end subdivisions

  int dur = end_t - start_t;
  int bar_diff = numEbar - numSbar;


  cout << "Note::notate_rest - start_t=" << start_t << " dur=" << dur
       << " end_t=" << end_t << endl;
  cout << in_tuple << endl;
//  int sever; //cin >> sever;
//  if(dur < 0) cin >> sever;
  int return_time = 0;

  Note * n = new Note(*this);
  n->split = 0;
  n->tuplet = 0;
  n->type_out = "";
  n->start_t = sBeat+beatEDUs;
  n->end_t = eBeat;
  n->pitch_out = "r";
  n->loudness_out = loudness_out;
  int diff = (eBeat-sBeat) / beatEDUs; // number of beats to tie
  int d = diff; // counter variable
  int t = sBeat + beatEDUs; // marker for most recently notated beat

 Rational<int> sig(timesignature[0], timesignature[2]); // time signature (simplified) as a Rational

 char orig_den = timesignature[2] - '0'; // get time signature's denominator value. since time signature is stored as a number, we do (- '0') to convert it into a character that can be used for printing and output.
 Rational<int> den(sig.Den(), 1); // denominator of simplified time signature, used for whole notes
 Rational<int> den2(sig.Den() * 2, 1); // 2 * denominator of simplified time signature, used for half notes


int flag_connect = 0;

while (d > 2){
    if (d > 5 && (barEDUs - t % barEDUs >= beatEDUs * 4)){
      n->type_out += "1 ";
      n->type_out +="r";
      d = d-4;
      t = t + beatEDUs*4;
      continue;
    }
    if (d > 3 && (barEDUs - t % barEDUs >= beatEDUs * 2)){
      n->type_out += "2 ";
      n->type_out +="r";
      d = d-2;
      t = t + beatEDUs*2;
      continue;
    }
    n->type_out += "4 ";
    n->type_out +="r";
    t = t + beatEDUs;
    d--;
  }
if (d==2 && eDiv!=0) n->type_out += "4 ";
if (d==2 && eDiv == 0) n->type_out += "4 ";
if (d==1 && eDiv!=0) n->type_out += " ";
if (d==1 && eDiv == 0) n->type_out += " ";
if (diff > 1) {
//    cout << "note(long): " << (diff-1) * beatEDUs << " " << n->start_t << " " << n->end_t << endl;
    insert_soundsAndRests(n);
}

  //split head and tail
  Note * shalf;

  if ((diff == 0) || (diff == 1 && eDiv==0)){
    type_out = convert_dur_to_type(dur);
    if (in_tuple == 1)
      tuplet = 1;
    if (in_tuple == 0 && tuplet == 1)
      return_time = start_t + 60;
     split = 0;
  }
  else {
  shalf = new Note(*this);
  shalf->pitch_out = "r";
  shalf->loudness_out;
  shalf->start_t = eBeat;
  shalf->end_t = end_t;
  shalf->split = 0;
  end_t = sBeat + beatEDUs;
  split = 1;
  type_out = convert_dur_to_type(beatEDUs-sDiv);
  if (in_tuple == 1) tuplet = 1;
  if (diff > 1 && flag_connect == 0 && pitch_out!="r") {
    this -> type_out += " ";
    flag_connect = 1;
    }
//  type_out = new_convert_dur_to_type(beatEDUs-sDiv); //////////// changed ////////////
  shalf->type_out = shalf->convert_dur_to_type(eDiv);
      if (in_tuple == 1 && shalf->end_t <= tuple_end )
        shalf->tuplet = 1;
      if (diff <= 1 && tuplet == 1)
        shalf->tuplet = 1;
//  shalf->type_out = shalf->new_convert_dur_to_type(eDiv); /////////// changed ////////////
  if ((eDiv != 0) && (shalf->end_t - shalf->start_t >= 10)) {
    insert_soundsAndRests(shalf);
    if (flag_connect == 0) this -> type_out += " ";
  }
 }
  insert_soundsAndRests(this);
  if ((in_tuple == 0) && (tuplet == 1)){

    return start_t + beatEDUs;
    }
  if (shalf!= NULL)
    if ((shalf->tuplet == 1) && (in_tuple == 0))
    return shalf->start_t + beatEDUs;
  else return return_time;
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

void Note::collectSounds( string startEDU, string durationEDU)
{
  int stime, dur, endTime, bar, beat;
  int sever;

  // Get needed parameters
  std::stringstream ss(startEDU);
  ss >> stime;
  std::stringstream ss1(durationEDU);
  ss1 >> dur;
  endTime = stime + dur;
  string beatEDU_str = tempo.getEDUPerTimeSignatureBeat().toPrettyString();
  beatEDUs = str_to_int( beatEDU_str);		//extern

		//are the following 6 lines needed ?
  string barEDU_str = tempo.getEDUPerBar().toPrettyString();
//string beatEDU_str = tempo.getEDUPerTimeSignatureBeat().toPrettyString();
  barEDUs = str_to_int( barEDU_str);
//beatEDUs = str_to_int( beatEDU_str);		//extern

  timesignature = tempo.getTimeSignature();

  //check if start time and end time are valid
  //validateSound(stime, endTime);
  start_t = stime;
  end_t = endTime;
  //insert before processing
//  cout << "In collectSounds: start_t:" << start_t << "  end_t" << end_t << endl;
  //notate();
  insert_soundsOnly(this);
  return;
}

//---------------------------------------------------------------------------//
/* -------------------moved to output.cpp ----------------

void Note::validateSoundsAndRests () {

  vector<Note*>::iterator it;
  Note * cur;
  Note * next;
  int flag = 0;
  if (cur->start_t != 0)
	cur->createRest( 0, cur->start_t, cur->loudness_out);


  // iterate through the vector of all notes pointers
  for (it = soundsOnly.begin(); it!=soundsOnly.end(); it++){
    cur = *it;
    insert_soundsAndRests(cur);

    if (cur == NULL) {
      cout << "cur is null" << endl;
      break;
    }
    cur = *it;
if ((it+1) != soundsOnly.end())
    next = *(it+1);
cout << "cur->start_t: " << cur->start_t << endl;
cout << "cur->end_t: " << cur->end_t << endl;

if (next == NULL)
cout << " at end " << endl;
else {
cout << "	next->start_t=" << next->start_t << " next->end_t="
     << next->end_t << endl;
}
      //check stime
      flag = checkValue(cur->start_t);
cout << "	stime		flag=" << flag << endl;

    if (flag == 0) {			//stime is not valid
cout << " later" << endl;
    }


    //check duration
    checkDuration(cur->start_t, cur->end_t);


if (next != NULL) {

cout << "not null" << endl;
cout << "  cur->start_t=" << cur->start_t << " cur->end_t=" << cur->end_t
     <<	" next->start_t=" << next->start_t << " next->end_t" << next->end_t
     << endl;
    if ( next->start_t < cur->end_t) next->start_t = cur->end_t;
cout << "	     cur->end_t=" << cur->end_t << "  next->start_t="
     << next->start_t  << endl;

    int gap = next->start_t - cur->end_t;
    if (gap < 0) {
      cout << "	ERROR" << endl;
      cout << "Note::validateSoundsAndRests - gap=" << gap << endl;
      int sever; cin >> sever;
    } else if (gap > 0) {
      flag = checkValue(gap);
cout << "	gap=" << gap << "		flag=" << flag << endl;
int sever; cin >> sever;
      cur->createRest (cur->end_t, next->start_t, cur->loudness_out);
      if (flag == 0) {

      }
    }

}


  }

cout << "----------------------" << endl;
cout << "begin printing vector: " << endl;
	vector<Note*>::iterator it2;
	Note * c;

	for (it2 = soundsOnly.begin(); it2 != soundsOnly.end(); it2++) {
	  c = *it2;
    //c->notate();
	}


}

*/
//---------------------------------------------------------------------------//

Note * Note::createRest(int stime, int endTime, string loud, int & next_st){
  Note * t = new Note();
  t->start_t = stime;
  t->end_t = endTime;
  t->checkDuration(t->start_t, t->end_t);
  if ( next_st < t->end_t)
    next_st = t->end_t;
  t->pitch_out = "r";
  t->loudness_out == loud;
//  cout << "endTime: " << endTime << "stime: " << stime << endl;



  //insert_soundsAndRests(t); removed, inserion is done within notate_rest()

  return t;
}


//--------------------------------------------------------------------------//
// 1 for valid
// 0 for invalid
int Note::checkValue (int & value) {
  int flag = 0;			//invalid

  int distance = value % beatEDUs;

  for (int i = 0; i<13; i++) {
    if (distance == valid_time[i]) flag = 1;
  }

  return flag;
}


//---------------------------------------------------------------------------//

void Note::checkDuration(int & stime, int & endTime) {
  int dur = endTime - stime;
  if (dur == 0) return;
  int flag = 0;
//cout << "checkedValue stime: " << stime << "  endtime: " << endTime << endl;

//if both duration and endtime are valid, then its valid
if (checkValue(dur) == 1 && checkValue(endTime) == 1) return;
//if duration is over different beats and endtime is valid, then its also valid
if (stime / 60 != endTime/60 && checkValue(endTime)==1) return;
//cout << "checkedValue stime: " << stime << "  endtime: " << endTime << endl;
  int remainToStart= stime % beatEDUs;    //EDUs between last beat and stime
  int remainToEnd = endTime % beatEDUs;    //EDUs between last beat and endTime

  //if the rest is longer than one beat
  /*******************what is this part for ************************/
  if (remainToStart + dur >= beatEDUs) {
    flag = checkValue(endTime);
    if (flag == 0) {

      //cout << "Note::checkDuration - invalid endTime dur > beatEDUs" << endl;
      endTime = closestVal(0, endTime%beatEDUs, endTime/beatEDUs);
      }
  }
  else if (remainToStart + dur < beatEDUs) {    	//less than one beat
    endTime = closestVal(remainToStart, dur, stime/beatEDUs);
  }
//  cout << "Note::checkDuration - new endTime=" << endTime << endl;
  start_t = stime; end_t = endTime;
}


//---------------------------------------------------------------------------//

int Note::closestVal(int remain, int dur, int beatNum) {
//dur = 30
  int close = beatEDUs - remain;//48
  int tempEnd;
  int keep = 0;
  int flag = 0;
  for (int i = 1; i<13; i++) {
    tempEnd = remain + valid_time[i];
    flag = checkValue(tempEnd);

    //if temporary end time is vaid and within the beat
    if (flag == 1 && tempEnd <= beatEDUs) {

      if (abs(valid_time[i] - dur) < close) {
        close = abs(valid_time[i] - dur); //close contains the difference between original duration and current duration
        keep = tempEnd;
      }
    }
  }
  return keep + beatEDUs * beatNum;
}
