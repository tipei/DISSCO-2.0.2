/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)
Modified by Ming-ching Chiu 2013

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
//  Event.h
//
//    This class defines a generic event.  It is used to manage all the
//    common things between types of events (top, high, low, bottom, etc.).
//
//----------------------------------------------------------------------------//

#ifndef EVENT_H
#define EVENT_H

#include "Libraries.h"
#include "Define.h"
#include "Matrix.h"
#include "Note.h"
#include "Rational.h"
#include "Tempo.h"
#include "TimeSpan.h"
#include "Utilities.h"
#include "Patter.h"


/**
* This class is to store the created patterns and its XML representation as key
**/
class PatternPair{

protected:
  string key;
  Patter* pattern;
public:
  PatternPair(string _key, Patter* _pat):key(_key),pattern(_pat){}
  ~PatternPair();
  string getKey(){return key;}
  Patter* getPattern (){return pattern;}

};


/**
* this class is used for bottom to construct its children
**/
class SoundAndNoteWrapper{
public:
  Tempo tempo;
  TimeSpan ts;
  string name;
  int type;
  // this is the element of the spectrum if the bottom event is a sound
  // or of the note
  DOMElement* element;

  SoundAndNoteWrapper(DOMElement* _element,
                      TimeSpan _ts,
		      string _name,
                      int _type,
                      Tempo _tempo ):
    ts(_ts),
    type(_type),
    name(_name),
    element(_element),
    tempo (_tempo){}

};


/**
*
**/
class Event {
public:
    //---------------------------- Information -------------------------------//

    //Name of the event
    string name;

    //Type of the event
    int type;

    //Timespan for the event
    TimeSpan ts;

    //Tempo of the event
    Tempo tempo;

    //stores the pattern needed for the Event and its XML representation
    std::vector<PatternPair*> patternStorage;

    //Utilities needs these
    DOMElement* AttackSieveElement;
    DOMElement* DurationSieveElement;

    DOMElement* spatializationElement;
    DOMElement* reverberationElement;
    DOMElement* filterElement;
    DOMElement* modifiersElement;

    //This Element is created by the Event, not the parser. It is passed to the
    // children and needs to be deleted once the event is done.
    DOMElement* modifiersIncludingAncestorsElement;

    // Storage for temporary parsers. For evaluating objects, XML parsers are
    // sometimes created by the utilities object. The event has the ownership
    // of these temporary parsers and is responsible to clean up.
    vector<XercesDOMParser*> temporaryXMLParsers;

protected:
    //------------------------------ Children --------------------------------//

    //File value to the event definitions of children


    //Children of the event
    vector<Event*> childEvents;

    //this is only used for bottom to stored its children
    vector<SoundAndNoteWrapper*> childSoundsAndNotes;
    //------------------------------ Building --------------------------------//

    //Child Event Def
    DOMElement* childEventDefElement;

    //Maximum allowed duration for a child
    float maxChildDur;

    //Percentage of child start time divided by total parent duration
    double checkPoint;

    //Previous start time
    TimeSpan tsPrevious;

    //Previous child duration
    double previousChildDuration;

    // Whether to align note durations to a sieve. True for Bottoms that contain Notes.
    bool sieveAligned;
    //-------------------------- Layers and Types ----------------------------//

    //Names of the layers
    vector< vector<string> > layerVect;
    vector<DOMElement*> layerElements;


    //Names of the children by type.
    vector<string> typeVect;
    vector<DOMElement*> childTypeElements; //discretepackage
    //Number of children to create (all layers)
    int numChildren;

    //Number of children to create for each layer
    vector<int> layerNumChildren;

    //Number of children yet to create (initially equal to layerNumChildren)
    vector<int> layerRemainingChildren;

    //Density of each layer.
    vector<float> layerDensity;

    //--------------------------- Restart Control ----------------------------//

    /*Putative child events are stored here. The children are not added
    immediately because the build process can fail and need to be restarted in
    the case of buildDiscrete.*/
    vector<Event*> temporaryChildEvents;

    //Number of restarts remaining.
    int restartsRemaining;
    static const int restartsNormallyAllowed = 6;
    static const int restartsAllowedWithFewerChildren = 10;

    ///Restarts the build process if necessary (for buildDiscrete).
    void tryToRestart(void);

    //---------------------------- Static Values -----------------------------//

    //Stores value for the getter to the static function CURRENT_CHILD_NUM
    int currChildNum;

    //Stores value for the getter to the static function CURRENT_CHILD_TYPE
    int childType;

    //--------------------------- Discrete Matrix ----------------------------//

    Matrix* matrix;

    //-------------------------- Used by build... ----------------------------//

    /*This stores the intermediate child timespan before it has been implemented
    an actual Event.*/
    TimeSpan tsChild;

    Utilities* utilities;
    DOMElement* childStartTimeElement ;
    DOMElement* childTypeElement;
    DOMElement* childDurationElement;
    DOMElement* methodFlagElement;
    DOMElement* childStartTypeFlag;
    DOMElement* childDurationTypeFlag;

    // This thing sorta works, but killing a thread waiting for cin causes
    // memory leak..   -- Ming-ching May 06, 2013
    pthread_t discreteWaitForInputIfFailedThread;
    string discreteFailedResponse;


  public:


    /**
    * This is the constructor for new CMOD model
    *
    **/

    Event(DOMElement* _element,
          TimeSpan _timeSpan,
          int _type,
          Tempo _tempo,
          Utilities* _utilities,
          DOMElement* _ancestorSpa,
          DOMElement* _ancestorRev,
          DOMElement* _ancestorFil,
          DOMElement* _ancestorModifiers);


	/**
     * NEW FUNCTION 2_23_16 NEEDS TESTING
	 * Finds and prints (right now) all leaf events
	 * TO BE USED ONLY AFTER THE BUILD HAS COMPLETED.
	 */
    void findLeafChildren(vector<Event*> &leafChildren);

    /**
    *   buildChildren. Builds sub-events from parsed information and
    *   information already set for this event.
    *   Contains a loop whthin which objects are created one at a time.
    *   Each object has at least three parameters: start time, duration,
    *   and type.  They are selected using one of the following methods:
    *     CONTINUUM for continuous probability, non-sequential order
    *     DISCRETE  for discrete values, using a Matrix object, non-sequential
    *     SWEEP for reading values from a file provided by the user,
    *   sequential order
    *
    *  This function is overloaded in Bottom
    **/

    virtual void buildChildren();

    /**
    * Added by Rishabh. Experimental.
    **/

    virtual void modifyChildren();

    /**
    *	Event destructor.
    **/
    virtual ~Event();


    //------------- Used by FileValue static functions  ------------//
    /**
     *	Returns the events name
     **/
    string getEventName() {return name;};

    /**
    *	Returns the number of the child this event is currently building
    **/
    int getCurrentChild() {return currChildNum;};

    /**
     *	Returns layer of the child this event is currently building
     **/
    int getCurrentLayer();

    /**
    *   Returns the current child type
    **/
    int getCurrentChildType() {return childType;};

    /**
    *   Returns the previous child duration
    **/

    double getPreviousChildDuration(){ return previousChildDuration;}

    /**
    *  Returns the number of current partial -- will call to bottom in most cases
    **/
    virtual int getCurrPartialNum() {return 0;};

    /**
    *  Return this events duration in EDU
    **/
    int getAvailableEDU();

    /**
    *  Returns check point of the event
    *  \return check point of the event as a double
    **/
    double getCheckPoint() {return checkPoint;};


    /**
    * Checks the event to see if it was built successfully.
    **/
    void checkEvent(bool buildResult);

    /**
    *   Outputs information about the current subEvent.
    **/
    virtual void outputProperties();

    /**
    * Adds pointers to any notes in this Event (or any children) to a vector
    *
    * \param noteVect a reference to a vector of notes
    **/
    virtual list<Note> getNotes();

    /**
    * gives the ownership of the temporary XML parser to the event.
    **/
    void addTemporaryXMLParser(XercesDOMParser* _parser);

    /**
    * gives the ownership of the pattern to the event.
    **/
    void addPattern(std::string _string, Patter* _pat);

    /**
    * todo: incomplete function
    **/
    void setDiscreteFailedResponse(string _input)
      { discreteFailedResponse = _input;}

  //------------- Private helper functions  ------------//
  protected:

    /**
    *  Method for determining the EDU-wise exactness of the duration. Returns
    *  "Yes", "No", or "Almost". "Almost" occurs when the EDU duration is
    *  within one-percent of the actual duration. This occurs when floating-
    *  point math approximates the result, but does not exactly produce it.
    *  "Yes" can only occur when the result is exactly divisible. "No" occurs
    *  for all other case (i.e., when there are 3.2 EDU for the total duration).
    **/
    string getEDUDurationExactness(void);

    /**
    *  Method for assigning float values for stimeSec and duration using
    *  continuos (stochastic) distributions and int value for type -
    *  actually, a discrete value..
    *
    *   Follows aq slightly different procedure than Sweep and Discrete.  Why?
    *  \param iter FileValues to pass in for new objects
    **/
    bool buildContinuum();

    /**
     *  Method for assigning stimeSec and durSec values in sequential order -
     *  "sweeping" from left to right or beginning to end of the event.
     *  For stime and duration two different methods are used, one for integer
     *  values the other for float values.  Type being a discrete value, the
     *  integer values method is used for it.
     *  \param iter FileValues to pass in for new objects
     **/
    bool buildSweep();

    /**
    *   Wrapper for assigning values for stimeMatrix, type and durMatrix
    *   using a matrix.  Calls ObjCoordinates, Adjustments, and TimeConvert.
    *   \param iter FileValues to pass in for new objects
    **/
    virtual bool buildDiscrete();

    /**
    *  Converts "SECONDS" to "sec.", "PERCENTAGE" to "%", etc.
    **/
    string unitTypeToUnits(string type);

    /**
    *  helper functions
    **/
    string getTempoStringFromDOMElement(DOMElement* _element);
    string getTimeSignatureStringFromDOMElement(DOMElement* _element);

    void buildMatrix(bool discrete);
};
#endif
