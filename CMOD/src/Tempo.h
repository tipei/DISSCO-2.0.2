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
//  Tempo.h
//
//----------------------------------------------------------------------------//

#ifndef TEMPO_H
#define TEMPO_H

//CMOD includes
#include "Libraries.h"
#include "Define.h"
#include "Rational.h"

class Event; // Forward declaration for root exact ancestor

//----------------------------------------------------------------------------//
class Tempo {
  Ratio tempoBeatsPerMinute;
  Ratio tempoBeat;
  Ratio timeSignatureBeat;
  Ratio timeSignatureBeatsPerBar;
  Ratio EDUPerTimeSignatureBeat;
  float tempoStartTime;

  const Event* rootExactAncestor; // The root ancestor event in which this tempo was created.
  
  ///When unitsPerSecond was used, it assumed this tempo (5/4 at quarter=60).
  void setBackwardsCompatibleTempo(void);
  
  public:
  
  ///Constructor initializes a backwards-compatible tempo of 5/4 quarter=60.
  Tempo();
  
  ///Explicit copy constructor just to be sure (eventually this should go away).
  Tempo(const Tempo& other);
  
  /**Two tempos are identical if they have the same tempo and time signature and
  tempo start time. Note that EDU per beat does not matter because the two
  tempos are still easily conflated with rational numbers.*/
  bool isTempoSameAs(Tempo& other);
  
  float getStartTime(void);
  
  void setStartTime(float newStartTime);
  
  /*Sets the beats-per-minute using the following format:
  
  <beat-name>|<beat-ratio> = <beats-per-minute>
  
  Examples:
  tempo = 'quarter = 130'
  tempo = '1/4 = 130'
  tempo = 'dotted quarter = 130'
  tempo = '3/8 = 130'

  Tuple equivalents:
  triplet half = 1/3
  triplet quarter = 1/6
  triplet eighth = 1/12
  
  Also accepts decimal for the BPM: "quarter=60.5"  --> 1/4 = 121/2
  */
  void setTempo(string newTempo);
  
  void setTimeSignature(string newTimeSignature);

  // in order to remove the errer in valgrind...
  //void setEDUPerTimeSignatureBeat(string newEDUPerTimeSignatureBeat);
  void setEDUPerTimeSignatureBeat(Ratio newEDUPerTimeSignatureBeat);

  void setRootExactAncestor(const Event* root_exact_ancestor);

  const Event* getRootExactAncestor() const;
  
  Ratio getTimeSignatureBeatsPerBar(void);
  
  Ratio getTimeSignatureBeat(void);
  Ratio getTempoBeatsPerMinute(void);
  
  Ratio getTempoBeat(void);
  
  Ratio getTempoBeatsPerBar(void);
  
  Ratio getTimeSignatureBeatsPerMinute(void);
  
  string getTimeSignature(void);
  
  Ratio getTempoBeatDurationInSeconds(void);
  
  Ratio getTimeSignatureBeatDurationInSeconds(void) ;
  
  Ratio getEDUPerTimeSignatureBeat(void);
    
  Ratio getEDUPerBar(void);
  
  Ratio getEDUPerTempoBeat(void);
  
  Ratio getEDUPerSecond(void);
  
  Ratio getEDUDurationInSeconds(void);

  /**
   * Using this Tempo's parameters, convert the given time in seconds
   * to the nearest time in EDUs.
   * 
   * @param seconds the time in seconds
   * @return the nearest time in EDUs
  **/
  int convertSecondsToEDUs(float seconds);
};
#endif /* TEMPO_H */

