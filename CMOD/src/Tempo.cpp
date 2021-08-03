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
//   Tempo.cpp
//
//----------------------------------------------------------------------------//

#include "Tempo.h"

//----------------------------------------------------------------------------//

void Tempo::setBackwardsCompatibleTempo(void) {
  tempoBeatsPerMinute = 60;
  tempoBeat = Ratio(1,4);
  timeSignatureBeat = Ratio(1,4);
  timeSignatureBeatsPerBar = 5;
  EDUPerTimeSignatureBeat = 1;
  tempoStartTime = 0;
}

//----------------------------------------------------------------------------//

Tempo::Tempo() {
  //Default tempo should just be the backwards-compatible one.
  setBackwardsCompatibleTempo();
}

//----------------------------------------------------------------------------//

Tempo::Tempo(const Tempo& other) {
  tempoBeatsPerMinute = other.tempoBeatsPerMinute;
  tempoBeat = other.tempoBeat;
  timeSignatureBeat = other.timeSignatureBeat;
  timeSignatureBeatsPerBar = other.timeSignatureBeatsPerBar;
  EDUPerTimeSignatureBeat = other.EDUPerTimeSignatureBeat;
  tempoStartTime = other.tempoStartTime;
  rootExactAncestor = other.rootExactAncestor;
}

//----------------------------------------------------------------------------//

bool Tempo::isTempoSameAs(Tempo& other) {
  return tempoBeatsPerMinute == other.tempoBeatsPerMinute &&
    tempoBeat == other.tempoBeat &&
    timeSignatureBeat == other.timeSignatureBeat &&
    timeSignatureBeatsPerBar == other.timeSignatureBeatsPerBar &&
    EDUPerTimeSignatureBeat == other.EDUPerTimeSignatureBeat &&
    fabs(tempoStartTime - other.tempoStartTime) < 0.0001; // Changed by andreworals
}

//----------------------------------------------------------------------------//

float Tempo::getStartTime(void) {return tempoStartTime;}

//----------------------------------------------------------------------------//

void Tempo::setStartTime(float newStartTime) {tempoStartTime = newStartTime;}

//----------------------------------------------------------------------------//

void Tempo::setTempo(string newTempo) {
  int length = (int)newTempo.length();
  string firstHalf, secondHalf;
  bool firstHalfIsNumber = false;
  bool inSecondHalf = false;
  bool countDigits = false;
  bool encounteredSecondHalfSlash = false;
  int digitCount = 0;

  for(int i = 0; i < length; i++) {
    char c = newTempo[i];
    if(c == ' ')
      continue;
    else if(c == '=')
      inSecondHalf = true;
    else if(!inSecondHalf) {
      if((c >= '0' && c <= '9') || c == '/')
        firstHalfIsNumber = true;
      firstHalf.append(1, c);
    }
    else {
      if(c == '.')
        countDigits = true;
      else if(c >= '0' && c <= '9') {
        secondHalf.append(1, c);
        if(countDigits)
          digitCount++;
      }
      else if(c == '/' && !countDigits && !encounteredSecondHalfSlash) {
        encounteredSecondHalfSlash = true;
        secondHalf.append("/");
      }
    }
  }
  
  //A really obscure way to add /10 /100 /1000 etc. depending on digitCount
  if(countDigits) {
    secondHalf.append("/1");
    secondHalf.append(digitCount, '0');
  }
  tempoBeatsPerMinute = secondHalf;
  
  if(firstHalfIsNumber) {
    tempoBeat = firstHalf;
    return;
  }
  
  if(firstHalf.find("thirt") != string::npos)
    tempoBeat = Ratio(1,32);
  else if(firstHalf.find("six") != string::npos)
    tempoBeat = Ratio(1,16);
  else if(firstHalf.find("eig") != string::npos)
    tempoBeat = Ratio(1,8);
  else if(firstHalf.find("quar") != string::npos)
{   tempoBeat = Ratio(1,4);
}
  else if(firstHalf.find("hal") != string::npos)
    tempoBeat = Ratio(1,2);
  else if(firstHalf.find("who") != string::npos)
    tempoBeat = Ratio(1,1);
    
  if(firstHalf.find("doub") != string::npos)
    tempoBeat *= Ratio(7,4);
  else if(firstHalf.find("dot") != string::npos)
    tempoBeat *= Ratio(3,2);
    
  if(firstHalf.find("tripl") != string::npos)
    tempoBeat *= Ratio(2,3);
  else if(firstHalf.find("quin") != string::npos)
    tempoBeat *= Ratio(4,5);
  else if(firstHalf.find("sext") != string::npos)
    tempoBeat *= Ratio(5,6); 
  else if(firstHalf.find("sept") != string::npos)
    tempoBeat *= Ratio(6,7);
}

//----------------------------------------------------------------------------//

void Tempo::setTimeSignature(string newTimeSignature) {
  int length = (int)newTimeSignature.length();
  string firstHalf, secondHalf;
  bool inSecondHalf = false;
  for(int i = 0; i < length; i++) {
    char c = newTimeSignature[i];
    if(c == ' ')
      continue;
    else if(c == '/')
      inSecondHalf = true;
    else if(c >= '0' && c <= '9') {
      if(!inSecondHalf) {
        firstHalf.append(1, c);
      }
      else {
        secondHalf.append(1, c);
      }
    }
  }
  timeSignatureBeatsPerBar = Ratio(firstHalf);
  timeSignatureBeat = Ratio(1) / Ratio(secondHalf);
}

//----------------------------------------------------------------------------//


void Tempo::setEDUPerTimeSignatureBeat(Ratio newEDUPerTimeSignatureBeat) {
  EDUPerTimeSignatureBeat = newEDUPerTimeSignatureBeat;
}
//in order to clean up the error message in Valgrind.
//void Tempo::setEDUPerTimeSignatureBeat(string newEDUPerTimeSignatureBeat) {
//  EDUPerTimeSignatureBeat = Ratio(newEDUPerTimeSignatureBeat);
//}

//----------------------------------------------------------------------------//

void Tempo::setRootExactAncestor(const Event* root_exact_ancestor) {
  rootExactAncestor = root_exact_ancestor;
}

//----------------------------------------------------------------------------//

const Event* Tempo::getRootExactAncestor() const {
  return rootExactAncestor;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTimeSignatureBeatsPerBar(void) {
  return timeSignatureBeatsPerBar;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTimeSignatureBeat(void) {
  return timeSignatureBeat;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTempoBeatsPerMinute(void) {
  return tempoBeatsPerMinute;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTempoBeat(void) {
  return tempoBeat;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTempoBeatsPerBar(void) {
  return timeSignatureBeatsPerBar * (timeSignatureBeat / tempoBeat);
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTimeSignatureBeatsPerMinute(void) {
  return tempoBeatsPerMinute * (tempoBeat / timeSignatureBeat);
}

//----------------------------------------------------------------------------//

string Tempo::getTimeSignature(void)
{
  int n = timeSignatureBeatsPerBar.Num();
  int d = timeSignatureBeat.Den();
  stringstream oss; oss << n << "/" << d;
  return oss.str();
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTempoBeatDurationInSeconds(void) {
  return Ratio(60) / tempoBeatsPerMinute;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getTimeSignatureBeatDurationInSeconds(void) {
  return Ratio(60) / (tempoBeatsPerMinute * (tempoBeat / timeSignatureBeat));
}

//----------------------------------------------------------------------------//

Ratio Tempo::getEDUPerTimeSignatureBeat(void) {
//cout << "  	Ratio Tempo::getEDUPerTimeSignatureBeat - 
// EDUPerTimeSignatureBeat=" << EDUPerTimeSignatureBeat.toPrettyString() << endl;
  return EDUPerTimeSignatureBeat;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getEDUPerBar(void) {
  return EDUPerTimeSignatureBeat * timeSignatureBeatsPerBar;
}

//----------------------------------------------------------------------------//

Ratio Tempo::getEDUPerTempoBeat(void) {
  return EDUPerTimeSignatureBeat * (tempoBeat / timeSignatureBeat);
}

//----------------------------------------------------------------------------//

Ratio Tempo::getEDUPerSecond(void) {
/*
  cout << "Ratio Tempo::getEDUPerSecond - EDUPerTimeSignatureBeat="
       << EDUPerTimeSignatureBeat.toPrettyString() << " TimeSignatureBeatsPerMinute="
       << getTimeSignatureBeatsPerMinute().toPrettyString() << endl;
  cout << "    	   string:" << getEDUPerTimeSignatureBeat().toPrettyString() << endl;
*/
  return EDUPerTimeSignatureBeat *
    getTimeSignatureBeatsPerMinute() / Ratio(60);
}

//----------------------------------------------------------------------------//

Ratio Tempo::getEDUDurationInSeconds(void) {
  return getTimeSignatureBeatDurationInSeconds() / EDUPerTimeSignatureBeat;
}

//----------------------------------------------------------------------------//

float Tempo::calculateSecondsFromEDUs(int edus) {
  return (float(edus) * getTimeSignatureBeatDurationInSeconds().To<float>()) / EDUPerTimeSignatureBeat.To<int>();
}

//----------------------------------------------------------------------------//

int Tempo::convertSecondsToEDUs(float seconds) {
  float beats_per_second = tempoBeatsPerMinute.To<float>() / 60.0f;
  float beats = seconds * beats_per_second;
  return static_cast<int>(lround(beats * EDUPerTimeSignatureBeat.To<float>()));
}
