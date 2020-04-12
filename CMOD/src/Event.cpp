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
//  Event.cpp
//
//----------------------------------------------------------------------------//

#include "Event.h"
#include "Output.h"
#include "Sieve.h"
#include "Random.h"
#include "Bottom.h"

//----------------------------------------------------------------------------//
//Checked

Event::Event(DOMElement* _element,
             TimeSpan _timeSpan,
             int _type,
             Tempo _tempo,
             Utilities* _utilities,
             DOMElement* _ancestorSpa,
             DOMElement* _ancestorRev,
             DOMElement* _ancestorFil,
             DOMElement* _ancestorModifiers):
  maxChildDur(0), checkPoint(0),
  numChildren(0),
  restartsRemaining(0),
  currChildNum(0), childType(0),
  matrix(0),
  type(_type),
  previousChildDuration(0),
  discreteFailedResponse(""),
  utilities( _utilities),
  modifiersIncludingAncestorsElement(NULL),
  sieveAligned(false){

  //Initialize parameters
  DOMElement* thisEventElement = _element->GFEC();
  string typeString =XMLTC(thisEventElement);
  type = atoi(typeString.c_str());

  thisEventElement = thisEventElement->GNES();
  name = XMLTC(thisEventElement);
  ts = _timeSpan;
  tempo = _tempo;

  thisEventElement = thisEventElement->GNES();
  maxChildDur = (float)utilities->evaluate(XMLTC(thisEventElement), (void*)this);

  thisEventElement = thisEventElement->GNES();
  int newEDUPerBeat = (int) utilities->evaluate(XMLTC(thisEventElement),(void*)this);
  Ratio k(newEDUPerBeat,1);
  Tempo fvTempo;
  fvTempo.setEDUPerTimeSignatureBeat(k);

  thisEventElement = thisEventElement->GNES();
  fvTempo.setTimeSignature(getTimeSignatureStringFromDOMElement(thisEventElement));

  thisEventElement = thisEventElement->GNES();
  fvTempo.setTempo(getTempoStringFromDOMElement(thisEventElement));

  fvTempo.getTempoBeat();

  fvTempo.setStartTime(tempo.getStartTime());


  if(tempo.getStartTime() == 0){
    tempo = fvTempo;
  }
  else if(!tempo.isTempoSameAs(fvTempo)) { //Warn if different tempi
    cout << endl << "WARNING: the tempo of this exact event differs from" << endl
      << "that of its exact parent." << endl;
    cout << "Parent: " << endl;
    cout << "  " << tempo.getTempoBeatsPerMinute() << endl;
    cout << "  " << tempo.getTempoBeat() << endl;
    cout << "  " << tempo.getTimeSignatureBeat() << endl;
    cout << "  " << tempo.getTimeSignatureBeatsPerBar() << endl;
    cout << "  " << tempo.getEDUPerTimeSignatureBeat() << endl;
    cout << "  " << tempo.getStartTime() << endl;
    cout << "File-Value Tempo: " << endl;
    cout << "  " << fvTempo.getTempoBeatsPerMinute() << endl;
    cout << "  " << fvTempo.getTempoBeat() << endl;
    cout << "  " << fvTempo.getTimeSignatureBeat() << endl;
    cout << "  " << fvTempo.getTimeSignatureBeatsPerBar() << endl;
    cout << "  " << fvTempo.getEDUPerTimeSignatureBeat() << endl;
    cout << "  " << fvTempo.getStartTime() << endl;
  }

  //This part initializes childnum and childnames

  DOMElement* numChildrenElement = thisEventElement->GNES();

  //store the pointer to be used in buildChildren()
  childEventDefElement = numChildrenElement->GNES();
  childStartTimeElement = childEventDefElement->GFEC();
  childTypeElement = childStartTimeElement->GNES();
  childDurationElement = childTypeElement->GNES();
  AttackSieveElement = childDurationElement->GNES();
  DurationSieveElement = AttackSieveElement->GNES();
  methodFlagElement = DurationSieveElement->GNES();
  childStartTypeFlag = methodFlagElement->GNES();
  childDurationTypeFlag = childStartTypeFlag->GNES();

  //layers, initialize child names
  thisEventElement = childEventDefElement->GNES();
  DOMElement* layerElement = thisEventElement->GFEC();
  while (layerElement){

    layerElements.push_back(layerElement);
    DOMElement* childPackage = layerElement->GFEC()->GNES()->GFEC();

    while(childPackage){
      childTypeElements.push_back(childPackage);
      childPackage = childPackage->GNES();
    }
    layerElement = layerElement->GNES();
  }


  DOMElement* flagElement = numChildrenElement->GFEC();
  if (XMLTC(flagElement) =="0"){ // Continuum
    DOMElement* entry1Element = flagElement->GNES();
    if (XMLTC(entry1Element)==""){
      numChildren = childTypeElements.size();
    }
    else {
      numChildren =(int) utilities->evaluate(XMLTC(entry1Element), (void*)this);
    }
  }
  else if (XMLTC(flagElement) == "1"){ // Densitiy
    DOMElement* densityElement = numChildrenElement->GFEC()->GNES();
    DOMElement* areaElement = densityElement->GNES();
    DOMElement* underOneElement = areaElement->GNES();
    double density = utilities->evaluate( XMLTC(densityElement),(void*)this);
    double area = utilities->evaluate( XMLTC(areaElement),(void*)this);
//  cout << "areaElement=" << areaElement << endl;
    double underOne = utilities->evaluate( XMLTC(underOneElement),(void*)this);
    double soundsPsec = pow(2, density * area - underOne); //this can't be right..
//  cout<<"density:"<< density<<", area:"<<area<<", underOne:"<<underOne<<endl;

    //not sure which version is the correct one. ask sever
    numChildren = (int)(soundsPsec * ts.duration + underOne/area);
//  cout << "     numChildren=" << numChildren << endl;
    //numChildren = (int)(soundsPsec * layerElements * ts.duration + 0.5);

  }
  else {// by layer
  numChildren = 0;
    for (int i = 0; i < layerElements.size(); i ++){
      numChildren +=utilities->evaluate(XMLTC(layerElements[i]->GFEC()),(void*)this);
    }
  }

  if (type <=3){ //top, high, mid, low

    thisEventElement = thisEventElement->GNES();
    if (_ancestorSpa != NULL) {
      spatializationElement = _ancestorSpa;
    }
    else if (Utilities::removeSpaces(XMLTC(thisEventElement)) ==""){
        spatializationElement = NULL;
    }
    else {
      spatializationElement = thisEventElement;
    }

    thisEventElement = thisEventElement->GNES();
    if (_ancestorRev != NULL) {
      reverberationElement = _ancestorRev;
    }
    else if (Utilities::removeSpaces(XMLTC(thisEventElement)) ==""){
        reverberationElement = NULL;
    }
    else {
      reverberationElement = thisEventElement;
    }

    thisEventElement = thisEventElement->GNES();
    if (_ancestorFil != NULL) {
      filterElement = _ancestorFil;
    }
    else if (Utilities::removeSpaces(XMLTC(thisEventElement)) ==""){
        filterElement = NULL;
    }
    else {
      filterElement = thisEventElement;
    }


    thisEventElement = thisEventElement->GNES();
    modifiersElement = thisEventElement;
    modifiersIncludingAncestorsElement = (DOMElement*) modifiersElement->cloneNode(true);

    if (_ancestorModifiers != NULL){

      DOMElement* ancestorModifierIter = _ancestorModifiers->GFEC();
      while(ancestorModifierIter !=NULL){
        DOMElement* cloneModifier = (DOMElement*) ancestorModifierIter->cloneNode(true);
        modifiersIncludingAncestorsElement->appendChild((DOMNode*)cloneModifier);
        ancestorModifierIter = ancestorModifierIter->GNES();
      }

    }// end handling _ancestorModifiers

   //cout<<"Event:"<<name<<":\nModifiers after merge:"<<XMLTC(modifiersIncludingAncestorsElement)<<endl<<endl<<"============="<<endl;
  } // end event type = 0, 1, 2, 3

}


//----------------------------------------------------------------------------//

string Event::getTempoStringFromDOMElement(DOMElement* _element){

  /*
  <Tempo>
        <MethodFlag>0</MethodFlag>
        <Prefix>0</Prefix>
        <NoteValue>2</NoteValue>
        <FractionEntry1></FractionEntry1>
        <FractionEntry2></FractionEntry2>
        <ValueEntry>60</ValueEntry>
      </Tempo>
  */
  DOMElement* thisElement = _element->GFEC();
  string stringbuffer = "";
  string methodFlag = XMLTC(thisElement); //it's either 0 or 1

  thisElement = thisElement->GNES();
  string prefix =  XMLTC(thisElement);

  thisElement = thisElement->GNES();
  string noteValue = XMLTC(thisElement);

  thisElement = thisElement->GNES();
  double fractionEntry1 = utilities->evaluate(XMLTC(thisElement),(void*)this);

  thisElement = thisElement->GNES();
  double fractionEntry2 = utilities->evaluate(XMLTC(thisElement),(void*)this);

  thisElement = thisElement->GNES();
  double valueEntry = utilities->evaluate(XMLTC(thisElement),(void*)this);


  if (prefix == "1"){
      stringbuffer = stringbuffer + "dotted ";
    }
    else if (prefix == "2" ){
      stringbuffer = stringbuffer + "double dotted ";
    }
    else if (prefix =="3"){
      stringbuffer = stringbuffer + "triple ";
    }
    else if (prefix =="4"){
      stringbuffer = stringbuffer + "quintuple ";
    }
    else if (prefix =="5"){
      stringbuffer = stringbuffer + "sextuple ";
    }
    else if (prefix =="6"){
      stringbuffer = stringbuffer + "septuple ";
    }

    if (noteValue == "0"){
      stringbuffer = stringbuffer + "whole = ";
    }
    else if (noteValue == "1"){
      stringbuffer = stringbuffer + "half = ";
    }
    else if (noteValue == "2"){
      stringbuffer = stringbuffer + "quarter = ";
    }
    else if (noteValue == "3"){
      stringbuffer = stringbuffer + "eighth = ";
    }
    else if (noteValue == "4"){
      stringbuffer = stringbuffer + "sixteenth = ";
    }
    else if (noteValue == "5"){
      stringbuffer = stringbuffer + "thirtysecond = ";
    }

  if (methodFlag == "0") {// tempo as note value

    char tempobuffer[20];
    sprintf(tempobuffer, "%f", valueEntry);
    stringbuffer = stringbuffer + string(tempobuffer);
  }

  else { // tempo as fraction
    //"entry1" notes in "value" seconds
    //entry : value = actual number : 60
    //entry1 * 60 / value = actual number


    double entry1 = fractionEntry1 * 60;
    double den = valueEntry;

    char tempobuffer [20];
    sprintf(tempobuffer, "%f", entry1);
    string numString = string(tempobuffer);

    sprintf (tempobuffer,"%f", den);
    string denString = string(tempobuffer);

    string ratioNumber = numString + "/" + denString;
    Ratio ratio = Ratio(ratioNumber);

    sprintf(tempobuffer, "%d", ratio.Num());

    if (ratio.Den() ==1){
     	stringbuffer = stringbuffer + string(tempobuffer) ;
    }
    else{
    	stringbuffer = stringbuffer + string(tempobuffer) + "/";
    	sprintf(tempobuffer, "%d", ratio.Den());
    	stringbuffer = stringbuffer + string(tempobuffer);
    }
  }
  return stringbuffer;
}


//----------------------------------------------------------------------------//

string Event::getTimeSignatureStringFromDOMElement(DOMElement* _element){
/*
<TimeSignature>
        <Entry1>4</Entry1>
        <Entry2>4</Entry2>
      </TimeSignature>
  */

  DOMElement* thisElement = _element->GFEC();
  int entry1 = utilities->evaluate(XMLTC(thisElement),(void*)this);

  char charbuffer[20];
  sprintf(charbuffer, "%d", entry1);
  string stringbuffer =  string(charbuffer);

  thisElement = thisElement->GNES();
  int entry2 = utilities->evaluate(XMLTC(thisElement),(void*)this);
  sprintf(charbuffer, "%d", entry2);
  string returnString = stringbuffer + "/"+ string(charbuffer);

  return returnString;
}


//----------------------------------------------------------------------------//

void Event::buildChildren(){
  if (utilities->getOutputParticel()){
  //Begin this sub-level in the output and write out its properties.
    Output::beginSubLevel(name);
    outputProperties();
  }

  string eventclass;
  switch (type){
    case 0: eventclass = "T         : ";break;
    case 1: eventclass = "--H       : ";break;
    case 2: eventclass = "----M     : ";break;
    case 3: eventclass = "------L   : ";break;
  }
  //Build the event's children.

  //Create the event definition iterator.
        /*
        <Entry1>1</Entry1>
        <Entry2>2</Entry2>
        <Entry3>3</Entry3>
        <AttackSieve>4</AttackSieve>
        <DurationSieve>5</DurationSieve>
        <DefinitionFlag>1</DefinitionFlag>
        <StartTypeFlag>2</StartTypeFlag>
        <DurationTypeFlag>2</DurationTypeFlag>
        */
  string method = XMLTC(methodFlagElement);


  //Set the number of possible restarts (for buildDiscrete)
  restartsRemaining = restartsNormallyAllowed;

  //Make sure that the temporary child events array is clear.
  if(temporaryChildEvents.size() > 0) {
    cerr << "WARNING: temporaryChildEvents should not contain data." << endl;
    cerr << "There may be a bug in the code. Please report." << endl;
    exit(1);
  }

  /*  old code. --Ming-ching May 06, 2013
  //Make sure the childType indexes correctly.
  if (childType >= typeVect.size() || typeVect[childType] == "") {
    cerr << "There is a mismatch between childType and typeVect." << endl;
    exit(1);
  }
  */

  //Create the child events.
  for (currChildNum = 0; currChildNum < numChildren; currChildNum++) {
    if (method == "0")
      checkEvent(buildContinuum());
    else if (method == "1")
      checkEvent(buildSweep());

    else if (method == "2")
      checkEvent(buildDiscrete());
    else {
      cerr << "Unknown build method: " << method << endl << "Aborting." << endl;
      exit(1);
    }
  }

  //Using the temporary events that were created, construct the actual children.
  for (int i = 0; i < temporaryChildEvents.size(); i++) {

    //Increment the static current child number.
    currChildNum = i;

    //build child.
    Event *e = temporaryChildEvents[currChildNum];
    childEvents.push_back(e);
  }

  //Clear the temporary event list.
  temporaryChildEvents.clear();

  //For each child that was created, build its children.
  for(int i = 0; i < childEvents.size(); i++){
    //overload in bottom
	/*
		RISKY METHOD 1

		If we are to stop the generation here
		call new method childEvents[i]->buildEmptyChildren()
			in place of buildChildren,
		then

		!!! REFACTOR THIS INTO A SINGLE CALLABLE METHOD !!!
		if (soundSynthesis){
    		MultiTrack* renderedScore = utilities->doneCMOD();
			!!! REPLACE GET NEXT SOUND FILE WITH A NEW NAMING METHOD !!!
			string soundFilename = getNextSoundFile();
    		//Write to file.
    		AuWriter::write(*renderedScore, soundFilename);
    		delete renderedScore;
  		}
	*/
    childEvents[i]->buildChildren();
    //TODO: DELETE ELSEWHERE. Cleanup should probably be done after execution.
		delete childEvents[i];
  }

  if (utilities->getOutputParticel()){
  //End this output sublevel.
    Output::addProperty("Updated Tempo Start Time", tempo.getStartTime());
    Output::endSubLevel();
  }
}

//---------------------------------------------------------------------------//

void Event::modifyChildren(){            //Incomplete

  //Randomly modify reverb and spatial elements

  //cout<<childEvents.size();

  for(int i = 0; i < childEvents.size(); i++){

     childEvents[i]->modifyChildren();
  }

}

//---------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void Event::findLeafChildren(vector<Event*> & leafChildren){
	if(childEvents.size() == 0){
		cout << "FOUND A LEAF: " << name << endl;
		leafChildren.push_back(this);
	}
	else{
		cout << "numchildren: " << childEvents.size()
                     << "----Non-Leaf----: " << name << endl;
	}
	for(int i = 0; i < childEvents.size(); i++){
		cout << "Before a dereference in findLeafChildren in " << name << endl;
		if(childEvents[i]!=NULL)
			childEvents[i]->findLeafChildren(leafChildren);
		cout << "Finished a dereference line. " << endl;
	}
}


//-----------------------------------------------------------------------------

bool Event::buildContinuum() {
  string startType = XMLTC(childStartTypeFlag);
  string durType = XMLTC(childDurationTypeFlag);

  // Whether we should align notes to sieves
  bool align = (   sieveAligned
                && Utilities::isSieveFunction(childStartTimeElement)
                && Utilities::isSieveFunction(childDurationElement)
                && startType == "1"
                && durType == "1");

  if (currChildNum == 0) {
    checkPoint = 0;
  }
  else {
    checkPoint = Random::Rand();
  }

  // get the start time
  float rawChildStartTime = 0.0;
  float rawChildDuration = 0.0;
  if (align) {
    if (matrix == NULL) buildMatrix(false);
    MatPoint childPt = matrix->chooseContinuum();

    rawChildStartTime = childPt.stime;
    tsChild.startEDU = childPt.stime;
    tsChild.start = childPt.stime * tempo.getEDUDurationInSeconds().To<float>();

    rawChildDuration = childPt.dur;
    tsChild.durationEDU = childPt.dur;
    tsChild.duration = childPt.dur * tempo.getEDUDurationInSeconds().To<float>();
  } else {
    rawChildStartTime = utilities->evaluate(XMLTC(childStartTimeElement),(void*)this);
    // how to process start time: EDU, SECONDS or PERCENTAGE
    if (startType == "1" ) { //"EDU"
      tsChild.start = rawChildStartTime *
        tempo.getEDUDurationInSeconds().To<float>();
      tsChild.startEDU = Ratio((int)rawChildStartTime, 1);
    } else if (startType == "2") { //second
      tsChild.start = rawChildStartTime; // no conversion needed
      tsChild.startEDU = Ratio(0, 0);  // floating point is not exact: NaN
    } else if (startType == "0") { //fraction
      tsChild.start = rawChildStartTime * ts.duration; // convert to seconds
      tsChild.startEDU = Ratio(0, 0);  // floating point is not exact: NaN
    } else {
      cerr << "Event::buildContinuum -- invalid or missing start type!" << endl;
      cerr << "      startType = " << startType << endl;
      cerr << "      in file " << name << endl;
      exit(1);
    }

    // get the duration
    rawChildDuration = utilities->evaluate(XMLTC(childDurationElement),(void*)this);

    // assign previousChild Duration here so that the next child can use it
    previousChildDuration = rawChildDuration;

    // pre-quantize the duration in case "EDU" is used
    int rawChildDurationInt = (int)rawChildDuration;
    int maxChildDurInt = (int)maxChildDur;
    if(rawChildDurationInt > maxChildDurInt)
        rawChildDurationInt = maxChildDurInt;

    // how to process duration: EDU, SECONDS or PERCENTAGE
    if (durType == "1") {//EDU
      tsChild.durationEDU = Ratio(rawChildDurationInt, 1);
      tsChild.duration = // convert to seconds
        (float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();
    } else if (durType == "2") {//seconds
      tsChild.duration = rawChildDuration;
      if(tsChild.duration > maxChildDur)
        tsChild.duration = maxChildDur; // enforce limit
      tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
    } else if (durType == "0") {//fraction
      tsChild.duration = rawChildDuration * ts.duration; // convert to seconds
      if(tsChild.duration > maxChildDur)
        tsChild.duration = maxChildDur; // enforce limit
      tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
    } else {
      cerr << "Event::buildContinuum -- invalid or missing duration type!" << endl;
      cerr << "      durtype = " << durType << endl;
      cerr << "      in file " << name << endl;
      exit(1);
    }
  }

  // get the type
  childType = utilities->evaluate(XMLTC(childTypeElement),(void*)this);
  string childName = XMLTC(childTypeElements[childType]->GFEC());

  checkPoint = (double)tsChild.start / ts.duration;

  if (utilities->getOutputParticel()){
  //Output parameters in the different units available.
    Output::beginSubLevel("Continuum");
    Output::addProperty("Name", childName);
    Output::beginSubLevel("Parameters");
      Output::addProperty("Start", rawChildStartTime, unitTypeToUnits(startType));
      Output::addProperty("Duration", rawChildDuration, unitTypeToUnits(durType));
      if(unitTypeToUnits(startType) == "EDU")
        Output::addProperty("Max Duration", maxChildDur, "EDU");
      else
        Output::addProperty("Max Duration", maxChildDur, "sec.");
    Output::endSubLevel();
    Output::beginSubLevel("Seconds");
      Output::addProperty("Start", tsChild.start, "sec.");
      Output::addProperty("Duration", tsChild.duration, "sec.");
    Output::endSubLevel();
    Output::beginSubLevel("EDU");
      Output::addProperty("Start", tsChild.startEDU.toPrettyString(), "EDU");
      Output::addProperty("Duration", tsChild.durationEDU.toPrettyString(), "EDU");
    Output::endSubLevel();
    Output::addProperty("Checkpoint", checkPoint, "of parent");
    Output::endSubLevel();
  }
  return true; //success!
}


//----------------------------------------------------------------------------//

bool Event::buildSweep() {
  string startType = XMLTC(childStartTypeFlag);
  string durType = XMLTC(childDurationTypeFlag);

  // Whether we should align notes to sieves
  bool align = (   sieveAligned
                && Utilities::isSieveFunction(childStartTimeElement)
                && Utilities::isSieveFunction(childDurationElement)
                && startType == "1"
                && durType == "1");

  // find start time and dur of last child
  if (currChildNum == 0) {
    tsPrevious.start = 0;
    tsPrevious.startEDU = 0;
  }

  // Set checkpoint to the endpoint of the last event
  checkPoint = tsPrevious.start / ts.duration;

  if (checkPoint > 1) {
    cerr << "Event::Sweep -- Error1: tsChild.start outside range of "
        << "parent duration." << endl;
    cerr << "      childStime=" << tsChild.start << ", parentDur="
        << ts.duration << endl;
    cerr << "      in file: " << name << ", childNum="
        << currChildNum << endl;
    cerr << "currChildNum=" << currChildNum << " tsPrevious.start="
        << tsPrevious.start << " checkPoint=" << checkPoint << endl;
    exit(1);
  }

  // get the start time
  float rawChildStartTime = 0.0;
  float rawChildDuration = 0.0;
  if (align) {
    if (matrix == NULL) buildMatrix(false);
    MatPoint childPt = matrix->chooseSweep(numChildren - currChildNum - 1);

    rawChildStartTime = childPt.stime;
    tsChild.startEDU = childPt.stime;
    tsChild.start = childPt.stime * tempo.getEDUDurationInSeconds().To<float>();

    rawChildDuration = childPt.dur;
    tsChild.durationEDU = childPt.dur;
    tsChild.duration = childPt.dur * tempo.getEDUDurationInSeconds().To<float>();
  } else {
    // get the start time
    rawChildStartTime =
      utilities->evaluate(XMLTC(childStartTimeElement),(void*)this);

    if (currChildNum == 0) {
      rawChildStartTime = 0;
      tsChild.start = 0;
      tsChild.startEDU = 0;
      tsPrevious.start = 0;
    }

cout << "Event::buildSweep - rawChildStartTime=" << rawChildStartTime << endl;

    if (startType == "1" ) {		//EDU
      tsChild.start = rawChildStartTime *
        tempo.getEDUDurationInSeconds().To<float>() + tsPrevious.start;
cout << "	tsPrevious.start=" << tsPrevious.start<< " tsChild.start="
     << tsChild.start<< endl;
      tsChild.startEDU = Ratio((int)rawChildStartTime, 1) + tsPrevious.startEDU;
    } else if (startType == "2") {	//seconds
      tsChild.start = rawChildStartTime + tsPrevious.start; // no conversion needed
      tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
    } else if (startType == "0") {	//fraction
      tsChild.start = rawChildStartTime * ts.duration + tsPrevious.start; // convert to seconds
      tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
    }

    if (tsChild.start < tsPrevious.start) {
      tsChild.start = tsPrevious.start;
      tsChild.startEDU = tsPrevious.startEDU;
    }

    if (currChildNum == 0) {
      tsChild.start = 0;
      tsChild.startEDU = 0;
    }

    // get the duration
cout << "Event::buildSweep - duration" << endl;
    rawChildDuration = utilities->evaluate(XMLTC(childDurationElement),(void*)this);
cout << "Event.cpp -buildSweep1 - rawChildDuration=" << rawChildDuration << endl;
int sever; cin>> sever;

    //assign previousChild Duration here so that the next child can use it
    previousChildDuration = rawChildDuration;

    // pre-quantize the duration in case "EDU" is used
    int rawChildDurationInt = (int)rawChildDuration;
    int maxChildDurInt = (int)maxChildDur;
    if(rawChildDurationInt > maxChildDurInt)
        rawChildDurationInt = maxChildDurInt;

    if (durType == "1") {
      tsChild.durationEDU = Ratio(rawChildDurationInt, 1);
cout << "Event.cpp -buildSweep2  - rawChildDurationInt=" << rawChildDurationInt << endl;
int sever; cin>> sever;
      tsChild.duration = 					// convert to seconds
        (float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();
    } else if (durType == "2") {
      tsChild.duration = rawChildDuration;
      if(tsChild.duration > maxChildDur)
        tsChild.duration = maxChildDur; // enforce limit
      tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
    } else if (durType == "0") {
      tsChild.duration = rawChildDuration * ts.duration; // convert to seconds
      if(tsChild.duration > maxChildDur)
        tsChild.duration = maxChildDur; // enforce limit
      tsChild.durationEDU = Ratio(0, 0); // floating point is not exact: NaN
    }
  }

  // set checkpoint to the start of this child event
  checkPoint = tsChild.start / ts.duration;
cout << "Event::Sweep3 - checkPoint=" << checkPoint << endl;

  if (checkPoint > 1) {
    cerr << "Event::Sweep -- Error2: tsChild.start outside range of "
        << "parent duration." << endl;
    cerr << "      childStime=" << tsChild.start << ", parentDur="
        << ts.duration << endl;
    cerr << "      in file: " << name << ", childNum="
        << currChildNum << endl;
    exit(1);
  }

  // get the type
  childType = utilities->evaluate(XMLTC(childTypeElement),(void*)this);
  string childName = XMLTC(childTypeElements[childType]->GFEC());

  tsPrevious.start = tsChild.start; //+ tsChild.duration;
  tsPrevious.startEDU = tsChild.startEDU; //+ tsChild.durationEDU;
  if (utilities->getOutputParticel()){
    //Output parameters in the different units available.
    Output::beginSubLevel("Sweep");
    Output::addProperty("Name", childName);
    Output::beginSubLevel("Parameters");
      Output::addProperty("Start", rawChildStartTime, unitTypeToUnits(startType));
      Output::addProperty("Duration", rawChildDuration, unitTypeToUnits(durType));
      if(unitTypeToUnits(startType) == "EDU")
        Output::addProperty("Max Duration", maxChildDur, "EDU");
      else
        Output::addProperty("Max Duration", maxChildDur, "sec.");
    Output::endSubLevel();
    Output::beginSubLevel("Seconds");
      Output::addProperty("Start", tsChild.start, "sec.");
      Output::addProperty("Duration", tsChild.duration, "sec.");
      Output::addProperty("Previous", tsPrevious.start, "sec.");
    Output::endSubLevel();
    Output::beginSubLevel("EDU");
      Output::addProperty("Start", tsChild.startEDU, "EDU");
      Output::addProperty("Duration", tsChild.durationEDU, "EDU");
      Output::addProperty("Previous", tsPrevious.startEDU, "EDU");
    Output::endSubLevel();
    Output::addProperty("Checkpoint", checkPoint, "of parent");
    Output::endSubLevel();
  }
  return true; // success!
}


//----------------------------------------------------------------------------//

void Event::addTemporaryXMLParser(XercesDOMParser* _parser){
  temporaryXMLParsers.push_back( _parser);
}

PatternPair::~PatternPair(){
  if (pattern)delete pattern;
}

void Event::addPattern(std::string _string, Patter* _pat){
  PatternPair* n = new PatternPair(_string, _pat);
  patternStorage.push_back(n);
}


//---------------------------------------------------------------------------//

Event::~Event() {

  //if ( modifiersIncludingAncestorsElement!=NULL)
  //  			delete modifiersIncludingAncestorsElement;
//turns out that clone dom node is not created in heap
  for (int i = 0; i < temporaryXMLParsers.size(); i++){
    delete temporaryXMLParsers[i];
  }

  vector<PatternPair*>::iterator iter = patternStorage.begin();

  for (; iter != patternStorage.end(); iter ++){
    delete *iter;
  }

  if ( matrix != NULL) delete matrix;
}


//----------------------------------------------------------------------------//
//Checked

void* waitForDiscreteResponse(void* _event){
  Event* event = (Event*) _event;
  string response = "";
  cin >>response;
	string * retval = new string(response);

  event->setDiscreteFailedResponse(response);
  return (void *) retval;
}


//----------------------------------------------------------------------------//
//Checked

void Event::tryToRestart(void) {

  //Decrement restarts, or if there are none left, ask for fewer children.
  if(restartsRemaining > 0) {
    restartsRemaining--;
    cout << "Failed to build child " << currChildNum << " of " << numChildren
      << " in file " << name << ". There are " << restartsRemaining
      << " tries remaining." << endl;
//  int sever; cin >> sever;
  } else {
    //Ask for permission to build with less children.
    cout << "Event::tryToRestart - currChildNum=" << currChildNum
         << " numChildren=" << numChildren << endl;
    cerr << "No tries remain. Try building with one less child? (Y/n)" << endl;

    bool inputAccepted = false;
    string answer = "";
    while (!inputAccepted){
      discreteFailedResponse = "";
      pthread_create(&discreteWaitForInputIfFailedThread, NULL, waitForDiscreteResponse, (void*) this);
			void * myfail;
      pthread_join(discreteWaitForInputIfFailedThread, &myfail);
			string thisfail = *((string *)myfail);
      /*int counter = 30;
      while (counter != 0){
        if (discreteFailedResponse !=""){
          break;
        }
        cout<<" Seconds before default action: "<< counter<<"\r"<< flush;
        sleep(1);
        counter --;
      }*/

      //warning! memory leak here! There is a problem killing thread waiting for cin. need to figure this out.
      // --Ming-ching May 06, 2013)
      //pthread_cancel(discreteWaitForInputIfFailedThread);

      //answer = (counter ==0)? "y" : discreteFailedResponse;
			answer = thisfail;
			delete ((string*)myfail);
      if (answer == "y" || answer == "Y" || answer =="n" || answer == "N") inputAccepted = true;
      else {
        cout<<"Please enter 'y' or 'n'."<<endl;
      }

    }
    if (answer == "n"|| answer == "N") {
			exit(1);
    }
    //Build with one less child.
    numChildren--; cerr << "Changed numChildren to " << numChildren << endl;

    //Reset the restart count.
    restartsRemaining = restartsAllowedWithFewerChildren;
  }

  //Start over by clearing the event arrays and resetting the for-loop index.
	// NOTE: SHOULD BE -1
  currChildNum = -1;
  for (int i = 0; i < childEvents.size(); i++)
    delete temporaryChildEvents[i];
  temporaryChildEvents.clear();

  //Clear the temporary event list.
  childSoundsAndNotes.clear();			//sever 5/29/2016

  patternStorage.clear();
}


//----------------------------------------------------------------------------//
//Checked

void Event::checkEvent(bool buildResult) {

  //If the build failed, restart if necessary.
  if (!buildResult) {
    tryToRestart();
    return;
  }

  /*Up to now the child start time is an *offset*, that is, it has no context
  yet within the piece. The following section uses the start time/tempo rules to
  determine the correct exact and inexact start times, in some cases leading to
  a new tempo.*/

  /*Inexact start time is global. That is, it *always* refers to the position in
  time relative to the beginning of the piece. Thus the child start time is
  merely the child offset added of the parent start time.*/
  tsChild.start += ts.start;

  /*The next part of the code deals with exactness issues since inexact and
  exact events may be nested inside each other.*/

  /*The following graphic attempts to show the many possibilities of nested
  exact and inexact offsets.

  i = inexact start offset
  e = exact start offset
  T = tempo start time

  0=============================================================================
  |            EVENT 1-----------------------------------------------------. . .
  |            |           EVENT 2-----------------------------------------. . .
  |            |           |          EVENT 3------------------------------. . .
  |            |           |          |         EVENT 4--------------------. . .
  |            |           |          |         |         EVENT 5----------. . .
  |            |           |          |         |         |       EVENT 6--. . .
  |            |           |          |         |         |       |
  |      +     i1    +     e2    +    e3   +    i4   +    i5  +   e6
  |            |           |          |         |         |       |
  .            .           .          .         .         .       .
  .            .           .          .         .         .       .
  .            .           .          .         .         .       .
  |    TEMPO   |   START   |  TIMES   |         |         |       |
               \\                                         \\
               T1         (T1)       (T1)                 T5     (T5)

  Inexact Start Times (~ means the exact value is truncated to floating point):
  Event 1 = i1
  Event 2 = i1 + ~e2
  Event 3 = i1 + ~e2 + ~e3
  Event 4 = i1 + ~e2 + ~e3 + i4
  Event 5 = i1 + ~e2 + ~e3 + i4 + i5
  Event 6 = i1 + ~e2 + ~e3 + i4 + i5 + ~e6

  Exact Start Times:
  Event 1 = (not applicable)
  Event 2 = T1 + e2
  Event 3 = T1 + (e2 + e3)
  Event 4 = (not applicable)
  Event 5 = (not applicable)
  Event 6 = T5 + e6

  Note that Event 4 ignores tempo information altogether since its child is
  inexact.

  Possible combinations:
  1) Parent inexact, child inexact (Events 4-5)
  Since both are inexact, nothing further is to be done. They will both only
  have global inexact time offsets.*/
  if(!ts.startEDU.isDeterminate() && !tsChild.startEDU.isDeterminate()) {
    //Nothing to do here.
  }

  /*2) Parent exact, child inexact (Events 3-4)
  Since the child is inexact, nothing further is to be done. The child will
  simply have a global inexact time offset. The parent will already have
  calculated its tempo start time.*/
  if(ts.startEDU.isDeterminate() && !tsChild.startEDU.isDeterminate()) {
    //Nothing to do here.
  }

  /*3) Parent exact, child exact (Events 2-3)
  Since the both are exact, the child inherits the tempo of the parent. Its
  exact offset is calculated by adding the exact parent start time offset.

  Important Note:
  If the child attempts to override the parent tempo, it will be ignored and the
  above calculation. This is to prevent implicitly nested tempos, which are
  better handled explicitly at the moment. For example it would be very
  difficult to properly render "4/4 for 3 1/4 beats, then change to 5/8 for
  3 beats as a child tempo." If this nesting were allowed, it would be very
  ambiguous as to how to return back to 4/4. Even if the 5/8 were to trigger a
  new tempo start time, in the score this would be misleading making it appear
  that the two sections were not rhythmically related, even though they
  inherently are by virtue of them both being exact.*/
  if(ts.startEDU.isDeterminate() && tsChild.startEDU.isDeterminate()) {
    tsChild.startEDU += ts.startEDU;
    /*We need to force child to have the same tempo, so that weird things do not
    happen. This is done below by explictly setting the tempo of the child. This
    will in turn be honored by initDiscreteInfo which will not override the
    given parent tempo. Note in order for this to be done, the tempo is passed
    to buildChildEvents, to constructChild, to EventFactory::Build, and finally
    to initDiscreteInfo.*/
  }

  /*4) Parent inexact, child exact (Events 1-2, 5-6)
  In this case, since the parent did not have an exact offset from the
  grandparent, the exact child needs a new reference point. This triggers the
  creation of a new tempo start time *for the parent*. Since the child is
  offset an exact amount from the parent, the parent is the new tempo reference.

  This could easily be the source of confusion: when a parent offset is inexact,
  and a child offset is exact, it is the parent which takes on the new tempo.
  Note that this implies that the child's siblings will refer to the same new
  tempo start time.*/
  if(!ts.startEDU.isDeterminate() && tsChild.startEDU.isDeterminate()) {
    /*The offset is the new start time, so nothing needs to be done to
    tsChild.startEDU. Instead we need to trigger a new tempo start for the
    parent. If this is the second exact child of a parent, then it will merely
    set the start time to the same thing.*/
    tempo.setStartTime(ts.start);
    //We need to force child to have the same tempo. See statement for 3).
  }

  //Make sure the childType indexes correctly.
  if (childType >= childTypeElements.size() ) {
    cerr << "There is a mismatch between childType and typeVect." << endl;
    exit(1);
  }

  //Create new event.
  DOMElement* discretePackage = childTypeElements[childType];
  EventType childEventType = (EventType) utilities->evaluate(XMLTC(discretePackage->GFEC()->GNES()),(void*)this);

  string childEventName = XMLTC(discretePackage->GFEC());
  DOMElement* childElement = utilities->getEventElement(childEventType, childEventName);

  Event* e;
  if (childEventType == eventBottom){
    e = (Event*)   new Bottom(childElement, tsChild, childType, tempo, utilities,
                spatializationElement, reverberationElement, filterElement, modifiersIncludingAncestorsElement);


    if(tsChild.startEDU.isDeterminate()){
    //cout<<"Child start EDU is determinate."<<endl;
      e->tempo = tempo;
    }
  }

  else if (childEventType == eventSound || childEventType == eventNote){
    childSoundsAndNotes.push_back(new SoundAndNoteWrapper
		  (childElement,tsChild, childEventName, childType, tempo));
  }
/*
  else if (childEventType == eventNote) {
    childSoundsAndNotes.push_back(new SoundAndNoteWrapper
				    (childElement,tsChild, childType, tempo));
  }
*/
  else {
    e = new Event( childElement, tsChild, childType, tempo, utilities,
                  spatializationElement, reverberationElement, filterElement,
		    modifiersIncludingAncestorsElement);
    if(tsChild.startEDU.isDeterminate()){
      //cout<<"Child start EDU is determinate."<<endl;
      e->tempo = tempo;
    }
  }

  //Add the event to the temporary event list.
  temporaryChildEvents.push_back(e);

}


//----------------------------------------------------------------------------//
//Checked

void Event::outputProperties() {
  Output::addProperty("Type", type);
  Output::addProperty("Start Time", ts.start, "sec.");
  Output::addProperty("Duration", ts.duration, "sec.");
  Output::addProperty("Tempo Start Time", tempo.getStartTime());
  Output::addProperty("Tempo",
    tempo.getTempoBeatsPerMinute().toPrettyString(), "BPM");
  Output::addProperty("Tempo Beat", tempo.getTempoBeat().toPrettyString(), "of whole note");
  Output::addProperty("Time Signature", tempo.getTimeSignature());
  Output::addProperty("Divisions",
    tempo.getEDUPerTimeSignatureBeat().toPrettyString(), "EDU/beat");
  Output::addProperty("Available EDU", getAvailableEDU());
  Output::addProperty("Available EDU is Exact", getEDUDurationExactness());
  Output::addProperty("EDU Duration", tempo.getEDUDurationInSeconds().toPrettyString(), "sec.");
}


//----------------------------------------------------------------------------//
//Checked

list<Note> Event::getNotes() {
  list<Note> result;
  for (int i = 0; i < childEvents.size(); i++) {
    list<Note> append = childEvents[i]->getNotes();
    list<Note>::iterator iter = append.begin();
    while (iter != append.end()) {
      result.push_back(*iter);
      iter++;
    }
  }
  return result;
}


//----------------------------------------------------------------------------//
//Checked

int Event::getCurrentLayer() {
  int countInLayer = 0;
  for(int i = 0; i < layerVect.size(); i++) {
    countInLayer += layerVect[i].size();
    if(childType < countInLayer)
      return i;
  }
  cerr << "Unable to get layer number in file " << name << endl; exit(1);
}


//----------------------------------------------------------------------------//
//Checked

int Event::getAvailableEDU()
{
  //Return exact duration if it is already apparent.
  if(ts.startEDU.isDeterminate() && ts.startEDU != Ratio(0, 1))
    return ts.startEDU.To<int>();

  //The duration is not exact.
  int myDurationInt = (int)ts.duration;
  Ratio EDUs;
  float durationScalar;
  if(ts.duration == (float)myDurationInt)
  {
    //Since duration is an integer, it may still be possible to have exact EDUs.
    EDUs = tempo.getEDUPerSecond() * Ratio(myDurationInt, 1);
    if(EDUs.Den() == 0)//This shouldn't happen.
      return 0;
    else if(EDUs.Den() != 0 && EDUs.Den() != 1) //We have exact EDUs
      return EDUs.To<int>();
    else //Implied EDUs.Den() == 1
      durationScalar = 1;
  }
  else
  {
    EDUs = tempo.getEDUPerSecond();
    if(EDUs.Den() == 0)
      return 0; //This shouldn't happen.
    else //Implied EDUs.Den() != 0
      durationScalar = ts.duration;
  }

  //The duration is not exact, so the available EDUs must be quantized.
  float approximateEDUs = EDUs.To<float>() * durationScalar;
  int quantizedEDUs = (int)(approximateEDUs + 0.001f);
  if(abs((float)quantizedEDUs - approximateEDUs) > 0.001f) {
    cout << "WARNING: quantizing AVAILABLE_EDU from ";
    cout << approximateEDUs << " to " << quantizedEDUs << endl;
  }
  return quantizedEDUs;
};


//----------------------------------------------------------------------------//
//Checked

string Event::getEDUDurationExactness(void) {
  float actualEDUDuration =
    (Ratio(getAvailableEDU(), 1) * tempo.getEDUDurationInSeconds()).To<float>();

  if(actualEDUDuration == ts.duration)
    return "Yes";
  else if(fabs(actualEDUDuration / ts.duration - 1.0f) < 0.01f)
    return "Almost";
  else
    return "No";
}


//----------------------------------------------------------------------------//
//Checked

string Event::unitTypeToUnits(string type) {
  if(type == "UNITS" || type == "EDU")
    return "EDU";
  else if(type == "SECONDS")
    return "sec.";
  else if(type == "PERCENTAGE")
    return "normalized";
  else
    return "";
}


//----------------------------------------------------------------------------//

bool Event::buildDiscrete() {

  if (currChildNum == 0) {
    checkPoint = 0;
  }
  MatPoint childPt;

  if (matrix == NULL) buildMatrix(true);

  // get something out of the matrix
  childPt  = matrix->chooseDiscrete(numChildren - currChildNum - 1);

  // check to see if we ran out of space in matrix --- (type=-1 is a flag)
  if (childPt.type == -1) {
    delete matrix; // delete the matrix so it gets recreated on a retry
    matrix = NULL;
    return false; // failure!
  }

  int stimeEDU = childPt.stime;
  int durEDU = childPt.dur;
  childType = childPt.type;
  string childName = XMLTC(childTypeElements[childType]->GFEC());

  if(durEDU > (int)maxChildDur)
    durEDU = maxChildDur;
  tsChild.startEDU = stimeEDU;
  //cout << "Event::buildDiscrete - stimeEDU=" << stimeEDU << endl;
  tsChild.durationEDU = durEDU;
  //cout << "Event::buildDiscrete - durEDU=" << durEDU << endl;

  tsChild.start = (float)stimeEDU *
    tempo.getEDUDurationInSeconds().To<float>();
  tsChild.duration = (float)durEDU *
    tempo.getEDUDurationInSeconds().To<float>();

  // using edu
  checkPoint = (double)tsChild.start / ts.duration;

  if (utilities->getOutputParticel()){
  //Output parameters in the different units available.
    Output::beginSubLevel("Discrete");
    Output::addProperty("Name", childName);
    Output::beginSubLevel("Parameters");
      Output::addProperty("Max Duration", maxChildDur, "EDU");
    Output::endSubLevel();
    Output::beginSubLevel("Seconds");
      Output::addProperty("Start", tsChild.start, "sec.");
      Output::addProperty("Duration", tsChild.duration, "sec.");
    Output::endSubLevel();
    Output::beginSubLevel("EDU");
      Output::addProperty("Start", tsChild.startEDU, "EDU");
      Output::addProperty("Duration", tsChild.durationEDU, "EDU");
    Output::endSubLevel();
    Output::endSubLevel();
  }

  //Return success.

  return true;
}


//---------------------------------------------------------------------------//

void Event::buildMatrix(bool discrete) {
  // first time called --- create the matrix!
  Sieve* attackSiv;
  Sieve* durSiv;
  vector<double> typeProbs;
  vector<Envelope*> attackEnvs;
  vector<Envelope*> durEnvs;
  vector<int> numTypesInLayers;

  if (discrete) {
    attackSiv = (Sieve*) utilities->evaluateObject(
                  XMLTC(AttackSieveElement),
                  (void*) this, eventSiv);

    durSiv = (Sieve*) utilities->evaluateObject(
                  XMLTC(DurationSieveElement),
                  (void*) this, eventSiv);
  } else {
    attackSiv = utilities->evaluateSieve(XMLTC(childStartTimeElement), (void*) this);
    durSiv = utilities->evaluateSieve(XMLTC(childDurationElement), (void*) this);
  }

  double weightSum = 0;
  for (int i = 0; i < childTypeElements.size(); i ++){
    double prob = utilities->evaluate(XMLTC(childTypeElements[i]->GFEC()->GNES()->GNES()), (void*) this);
    typeProbs.push_back(prob);
    weightSum += prob;
  }

  for (int i = 0; i < typeProbs.size(); i ++){
    typeProbs[i] = typeProbs[i] / weightSum;
  }


  if (discrete) {
    for (int i = 0; i < childTypeElements.size(); i ++){
      // attack env
      DOMElement* elementIter = childTypeElements[i]->GFEC()->GNES()->GNES()->GNES();

      string attackEnvString = XMLTC(elementIter);
      elementIter = elementIter ->GNES();
      string attackEnvScaleString = XMLTC(elementIter);

      string attackFunctionString =
              "<Fun><Name>EnvLib</Name><Env>" +
              attackEnvString +
              "</Env><Scale>" +
              attackEnvScaleString +
              "</Scale></Fun>";

      elementIter = elementIter ->GNES();
      string durationEnvString = XMLTC(elementIter);

      elementIter = elementIter ->GNES();
      string durationEnvScaleString = XMLTC(elementIter);

      string durationFunctionString =
              "<Fun><Name>EnvLib</Name><Env>" +
              durationEnvString +
              "</Env><Scale>" +
              durationEnvScaleString +
              "</Scale></Fun>";

      attackEnvs.push_back((Envelope*)
        utilities->evaluateObject(attackFunctionString, this, eventEnv));

      durEnvs.push_back((Envelope*)
        utilities->evaluateObject(durationFunctionString, this, eventEnv));
    }
  }
  for (int i = 0; i < layerElements.size(); i ++){
    int numOfDiscretePackages = 0;
    DOMElement* elementIter = layerElements[i]->GFEC()->GNES()->GFEC();
    while (elementIter!= NULL){
      numOfDiscretePackages++;
      elementIter = elementIter ->GNES();
    }
    numTypesInLayers.push_back (numOfDiscretePackages);
  }

  int parentEDUs = str_to_int(tempo.getEDUPerSecond().toPrettyString()) * ts.duration;

  matrix = new Matrix(childTypeElements.size(), attackSiv->GetNumItems(),
       durSiv->GetNumItems(),  numTypesInLayers, parentEDUs, tempo, sieveAligned);

  if (discrete) {
    matrix->setAttacks(attackSiv, attackEnvs);
    matrix->setDurations(durSiv, parentEDUs, durEnvs);
  } else {
    matrix->setAttacks(attackSiv);
    matrix->setDurations(durSiv, parentEDUs);
  }
  matrix->setTypeProbs(typeProbs);

  delete attackSiv;
  delete durSiv;

  for (int i = 0 ; i < attackEnvs.size(); i++){
    delete attackEnvs[i];
  }
  attackEnvs.clear();
  for (int i = 0 ; i< durEnvs.size(); i++){
    delete durEnvs[i];
  }
  durEnvs.clear();


}
