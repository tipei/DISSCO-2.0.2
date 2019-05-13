/*******************************************************************************
 *
 *  File Name     : IEvent.cpp
 *  Date created  : Feb. 2 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains the implementation of IEvent class and
 *                  its related subclasses.
 *
 *
 *
 *==============================================================================
 *
 *  This file is part of LASSIE.
 *  2010 Ming-ching Chiu, Sever Tipei
 *
 *
 *  LASSIE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LASSIE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with LASSIE.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "IEvent.h"
#include "ProjectViewController.h"
#include "FunctionGenerator.h"

#include "../../CMOD/src/parser/lex.yy.c"





//#include <stdlib.h>

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};


IEvent::IEvent(){
  eventName = "New IEvent";
  oldEventName = "";
  eventOrderInPalette = -1;

  maxChildDur = "";

  unitsPerSecond = "6";
  timeSignatureEntry1 = "4";
  timeSignatureEntry2 = "4";

  tempoMethodFlag = 0; //0 = as note value, 1 = as fraction
  tempoPrefix = tempoPrefixNone;
  tempoNoteValue = tempoNoteValueQuarter;
  tempoFractionEntry1 = "1";
  tempoFractionEntry2 = "1";
  tempoValueEntry = "60";


  flagNumChildren    = 0; // 0 = fixed, 1 = density
  numChildrenEntry1  = "";
  numChildrenEntry2  = "";
  numChildrenEntry3  = "";
  changedButNotSaved = false;

  childEventDefEntry1 = "";
  childEventDefEntry2 = "";
  childEventDefEntry3 = "";

  flagChildEventDef             = 0;
  flagChildEventDefStartType    = 2;
  flagChildEventDefDurationType = 2;

  childEventDefAttackSieve = "";
  childEventDefDurationSieve = "";

  filter = "";
  reverb = "";
  spatialization = "";
  measurement = "";

  modifiers = NULL;
  extraInfo = NULL;


  EventLayer* newLayer = new EventLayer(this);
  layers.push_back(newLayer);
}


IEvent::~IEvent(){
  if(extraInfo!=NULL){
    delete extraInfo;
  }
  //delete layers here
}


IEvent::SoundExtraInfo::SoundExtraInfo(){
  spectrumPartials = new SpectrumPartial();
  deviation = "";
  numPartials =1;
  spectrumGenBuilder = "";
}


IEvent::SoundExtraInfo::~SoundExtraInfo(){
}


void IEvent::setEventName(std::string _eventName){
  eventName = _eventName;
  changedButNotSaved = true;
}

void IEvent::setEventType(EventType _type){
  eventType = _type;
  changedButNotSaved = true;
  if (extraInfo!= NULL){
    delete extraInfo;
  }
  if(_type == eventSound){
    //EventExtraInfo* SoundEventExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::SoundExtraInfo();
  }
  else if(_type == eventBottom){
    //EventExtraInfo* BottomEventExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::BottomEventExtraInfo();
    string firstChar = eventName.substr(0,1);
    if (firstChar =="s"){
    	extraInfo->setChildTypeFlag(0);
    }
    else if (firstChar =="n"){
    	extraInfo->setChildTypeFlag(1);
    }
    else {
    	extraInfo->setChildTypeFlag(2);
   	}



  }
  else if(_type == eventEnv){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::EnvelopeExtraInfo();
  }
  else if(_type == eventSiv){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::SieveExtraInfo();
  }
  else if(_type == eventSpa){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::SpatializationExtraInfo();
  }
  else if(_type == eventPat){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::PatternExtraInfo();
  }
  else if(_type == eventRev){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::ReverbExtraInfo();
  }
    else if(_type == eventFil){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::FilterExtraInfo();
  }
    else if(_type == eventMea){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::MeasureExtraInfo();
  }
  else if(_type == eventNote){
    //EventExtraInfo* EnvelopeExtraInfo;
    extraInfo = (IEvent::EventExtraInfo*) new IEvent::NoteExtraInfo();
  }

}


std::string IEvent::getEventName(){
  return eventName;
}

EventType IEvent::getEventType(){
  return eventType;
}


std::string IEvent::getEventTypeString(){

  switch(eventType){
    case 0:
      return "Top";
    case 1:
      return "High";
    case 2:
      return "Mid";
    case 3:
      return "Low";
    case 4:
      return "Bottom";
    case 5:
      return "Spectrum";
    case 6:
      //return "Env.";
      return "Envelope";
    case 7:
      return "Sieve";
    case 8:
      //return "Spat.";
      return "Spatialization";
    case 9:
      //return "Pat.";
      return "Pattern";
    case 10:
      return "Reverb";
      //return "Rev.";
    case 11:
      return "Folder";
    case 12:
      return "Note";
    case 13:
      return "Filter";
    case 14:
      return "Measurement";
  }
}


void IEvent::setMaxChildDur(std::string _maxChildDur){
  maxChildDur = _maxChildDur;
  changedButNotSaved = true;
}


std::string IEvent::getMaxChildDur(){
  return maxChildDur;
}


void IEvent::setUnitsPerSecond(std::string _unitsPerSecond){
  unitsPerSecond = _unitsPerSecond;
  changedButNotSaved = true;
}


std::string IEvent::getUnitsPerSecond(){
  return unitsPerSecond;
}




void IEvent::setSpa(std::string _spa){
  spatialization = _spa;
  changedButNotSaved = true;
}


std::string IEvent::getSpa(){
  return spatialization;
}


void IEvent::setRev(std::string _rev){
  reverb = _rev;
  changedButNotSaved = true;
}


std::string IEvent::getRev(){
  return reverb;
}


void IEvent::setFil(std::string _fil){
  filter = _fil;
  changedButNotSaved = true;
}


std::string IEvent::getFil(){
  return filter;
}

void IEvent::setMea(std::string _mea){
  measurement = _mea;
  changedButNotSaved = true;
}

std::string IEvent::getMea(){
  return measurement;
}









void IEvent::setTimeSignatureEntry1(std::string _timeSignatureEntry1){
  timeSignatureEntry1 = _timeSignatureEntry1;
  changedButNotSaved = true;
}


std::string IEvent::getTimeSignatureEntry1(){
  return timeSignatureEntry1;
}


void IEvent::setTimeSignatureEntry2(std::string _timeSignatureEntry2){
  timeSignatureEntry2 = _timeSignatureEntry2;
  changedButNotSaved = true;
}



std::string IEvent::getTimeSignatureEntry2(){
  return timeSignatureEntry2;
}








int IEvent::getTempoMethodFlag(){
  return tempoMethodFlag;
}

void IEvent::setTempoMethodFlag(int _flag){

  tempoMethodFlag = _flag;
  changedButNotSaved = true;
}
TempoPrefix IEvent::getTempoPrefix(){
  return tempoPrefix;
}

void IEvent::setTempoPrefix(TempoPrefix _prefix){

  tempoPrefix = _prefix;
  changedButNotSaved = true;
}

TempoNoteValue IEvent::getTempoNoteValue(){
  return tempoNoteValue;
}

void IEvent::setTempoNoteValue(TempoNoteValue _noteValue){

  tempoNoteValue = _noteValue;
  changedButNotSaved = true;
}


std::string IEvent::getTempoFractionEntry1(){
  return tempoFractionEntry1;
}
void IEvent::setTempoFractionEntry1(std::string _entry1){
  tempoFractionEntry1 = _entry1;
  changedButNotSaved = true;
}
std::string IEvent::getTempoFractionEntry2(){
  return tempoFractionEntry2;
}
void IEvent::setTempoFractionEntry2(std::string _entry2){
  tempoFractionEntry2 = _entry2;
  changedButNotSaved = true;
}

std::string IEvent::getTempoValueEntry(){
  return tempoValueEntry;
}
void IEvent::setTempoValueEntry(std::string _entry){
  tempoValueEntry =_entry;
  changedButNotSaved = true;
}




void IEvent::setFlagNumChildren(int _flag){
  flagNumChildren = _flag;
  changedButNotSaved = true;
}


int IEvent::getFlagNumChildren(){
  return flagNumChildren;
}


void IEvent::setNumChildrenEntry1(std::string _string){
  numChildrenEntry1 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getNumChildrenEntry1(){
  return numChildrenEntry1;
}


void IEvent::setNumChildrenEntry2(std::string _string){
  numChildrenEntry2 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getNumChildrenEntry2(){
  return numChildrenEntry2;
}


void IEvent::setNumChildrenEntry3(std::string _string){
  numChildrenEntry3 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getNumChildrenEntry3(){
  return numChildrenEntry3;
}


void IEvent::setChildEventDefEntry1(std::string _string){
  childEventDefEntry1 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getChildEventDefEntry1(){
  return childEventDefEntry1;
}


void IEvent::setChildEventDefEntry2(std::string _string){
  childEventDefEntry2 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getChildEventDefEntry2(){
  return childEventDefEntry2;
}


void IEvent::setChildEventDefEntry3(std::string _string){
  childEventDefEntry3 = _string;
  changedButNotSaved = true;
}


std::string IEvent::getChildEventDefEntry3(){
  return childEventDefEntry3;
}


void IEvent::setFlagChildEventDef(int _flag){
  // 0 = continuum, 1 = sweep, 2 = descrete
  flagChildEventDef = _flag;
}


int IEvent::getFlagChildEventDef(){
  return flagChildEventDef;
}


void IEvent::setFlagChildEventDefStartType(int _flag){

  // 0 = percentage, 1 = units, 2 = second.   -1 if childeventdef is discrete
  flagChildEventDefStartType = _flag;
}

int IEvent::getFlagChildEventDefStartType(){
  return flagChildEventDefStartType;
}



void IEvent::setFlagChildEventDefDurationType(int _flag){

  // 0 = percentage, 1 = units, 2 = second.   -1 if childeventdef is discrete
  flagChildEventDefDurationType = _flag;
}


int IEvent::getFlagChildEventDefDurationType(){
  return flagChildEventDefDurationType;
}


void IEvent::setChildEventDefAttackSieve(std::string _string){
  childEventDefAttackSieve = _string;

}


std::string IEvent::getChildEventDefAttackSieve(){
  return childEventDefAttackSieve;
}


void IEvent::setChildEventDefDurationSieve(std::string _string){
  childEventDefDurationSieve = _string;
}


std::string IEvent::getChildEventDefDurationSieve(){
  return childEventDefDurationSieve;
}


//deprecated. replaced by eventlayer::addchild
void IEvent::addChildOrObject(IEvent* _newChild){

}


void IEvent::removeChildOrObject(IEvent* _childToBeRemoved){
  //TODO: implement this function
}


void IEvent::setModifiedButNotSaved(){
  changedButNotSaved = true;
}

void IEvent::saveToDisk(std::string _pathOfProject){
	//saveToDiskHelper(_pathOfProject, false);
}




int IEvent::getNumberOfLayers(){
  return layers.size();
}


EventLayer* IEvent::addLayer(){
  EventLayer* newLayer = new EventLayer(this);
  layers.push_back(newLayer);
  return newLayer;

}






void IEvent::showAllChildren(){
  std::list<EventLayer*>::iterator i = layers.begin();

  for (i ; i != layers.end(); ++i){
    std::cout << "===========================================";
    std::cout << std::endl;
    (*i)->showContents();
  }
}


std::string EventLayer::getByLayer(){
  return byLayer;
}




EventLayer::EventLayer(IEvent* _thisEvent){
  thisIEvent = _thisEvent;
  byLayer     = "";
}


EventLayer::~EventLayer(){
  std::list<EventDiscretePackage*>::iterator childIter = children.begin();
  for (childIter; childIter!= children.end(); childIter++){
    (*childIter)->event->removeParent(thisIEvent);
    delete (*childIter);
  }
  children.clear();
}

void IEvent::setChangedButNotSaved(bool value){
  changedButNotSaved = value;

}

EventDiscretePackage* EventLayer::addChild(IEvent* _child){
  thisIEvent->setChangedButNotSaved(true);
  EventDiscretePackage* newdyad = new EventDiscretePackage(_child);
  children.push_back(newdyad);

  _child->addParent(thisIEvent);

  return newdyad;
}



void EventLayer::showContents(){
  std::cout << "EventLayer begins showing contents" << std::endl;
  std::list<EventDiscretePackage*>::iterator i = children.begin();

  for (i; i != children.end(); ++i){
    std::cout << "!" << (*i)->event->getEventName() << std::endl;
  }

  std::cout << "EventLayer stop showing Contents" << std::endl;
}



int EventLayer::size(){
  return children.size();
}


std::string EventLayer::outputChildrenNameString(){
  if(children.size() == 0) return "";

  std::string temp = "                 <\n                    ";
  std::list<EventDiscretePackage*>::iterator i = children.begin();

  while(i != children.end()){
    IEvent* thisEvent = (*i)->event;

    if( thisEvent->getEventType() == eventTop){
      temp = temp + "\"T/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventHigh){
      temp = temp + "\"H/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventMid){
      temp = temp + "\"M/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventLow){
      temp = temp + "\"L/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventBottom){
      temp = temp + "\"B/" + thisEvent->getEventName() + "\"";

    }else if( thisEvent->getEventType() == eventSound){
      temp = temp + "\"S/" + thisEvent->getEventName() + "\"";
    }else if (thisEvent->getEventType() == eventNote){
    	temp = temp + "\"N/" + thisEvent->getEventName() + "\"";
   	}


    i++;
    if(i == children.end()){
      break;
    }
    temp = temp + ",\n                    ";
  }

  temp = temp + "\n                 >";
  return temp;
}


IEvent::EventExtraInfo* IEvent::getEventExtraInfo(){
  return extraInfo;
}


void IEvent::SoundExtraInfo::setDeviation(std::string _deviation){
  deviation = _deviation;
}


std::string IEvent::SoundExtraInfo::getDeviation(){
  return deviation;
}

void IEvent::SoundExtraInfo::setSpectrumGenBuilder(std::string _string){
  spectrumGenBuilder = _string;
}


std::string IEvent::SoundExtraInfo::getSpectrumGenBuilder(){
  return spectrumGenBuilder;
}

//void IEvent::SoundExtraInfo::setNumPartials(std::string _numPartial){
  //numPartials = _numPartial;
//}


std::string IEvent::SoundExtraInfo::getNumPartials(){
  char charbuffer[20];
  sprintf( charbuffer, "%d", numPartials);
  return string(charbuffer);
}


//void IEvent::SoundExtraInfo::setSpectrum(std::string _spectrum){
  //spectrum = _spectrum;
//}


SpectrumPartial* IEvent::SoundExtraInfo::getSpectrumPartials(){
  return spectrumPartials;
}

SpectrumPartial* IEvent::SoundExtraInfo::addPartial(){

  SpectrumPartial* newPartial = new SpectrumPartial();
  SpectrumPartial* end = spectrumPartials;
  numPartials ++;

  if (spectrumPartials ==NULL){
    spectrumPartials = newPartial;
    return newPartial;
  }
  else {
    while (end->next!= NULL){
      end = end->next;
    }
    end->next = newPartial;
    newPartial->prev = end;
    return newPartial;
  }
}



bool IEvent::SoundExtraInfo::deletePartial(SpectrumPartial* _partial){
  if (numPartials ==1 ){
    return false;
  }
  if (spectrumPartials == _partial){
    spectrumPartials = spectrumPartials ->next;
    if (spectrumPartials != NULL){
      spectrumPartials->prev = NULL;
    }
  }

  else {
    SpectrumPartial* search = spectrumPartials;
    while (search != _partial){
      search = search->next;
    }

    search->prev->next = search->next;
    if (search->next != NULL){
      search->next->prev = search->prev;
    }
  }
  numPartials --;
  delete _partial;
  return true;


}




double EventLayer::getChildrenWeightSum(){
  double sum = 0;

  std::list<EventDiscretePackage*>::iterator i = children.begin();
  for (i; i!= children.end(); i++){

     sum += atof((*i)->weight.c_str());

  }

  return sum;

}



IEvent::BottomEventExtraInfo::BottomEventExtraInfo(){
  frequencyFlag = 0;
  frequencyContinuumFlag = 0;
  frequencyEntry1 = "";
  frequencyEntry2 = "";
  loudness = "";
  spatialization = "";
  reverb = "";
  modifiers = NULL;
  childTypeFlag =-1;

}
IEvent::BottomEventExtraInfo::~BottomEventExtraInfo(){
}

int IEvent::BottomEventExtraInfo::getFrequencyFlag(){
  return frequencyFlag;// 0 = Well_tempered, 1 = Fundamental, 2 = Continuum
}

void IEvent::BottomEventExtraInfo::setFrequencyFlag(int _flag){
  frequencyFlag = _flag;
}
int IEvent::BottomEventExtraInfo::getFrequencyContinuumFlag(){
  return frequencyContinuumFlag;// 0 = hertz, 1 = power of two
}

void IEvent::BottomEventExtraInfo::setFrequencyContinuumFlag(int _flag){
  frequencyContinuumFlag = _flag;
}

std::string  IEvent::BottomEventExtraInfo::getFrequencyEntry1(){
  return frequencyEntry1;
}

void  IEvent::BottomEventExtraInfo::setFrequencyEntry1(std::string _string){
  frequencyEntry1 = _string;
}
std::string  IEvent::BottomEventExtraInfo::getFrequencyEntry2(){
  return frequencyEntry2;
}

void  IEvent::BottomEventExtraInfo::setFrequencyEntry2(std::string _string){
  frequencyEntry2 = _string;
}
std::string  IEvent::BottomEventExtraInfo::getLoudness(){
  return loudness;
}
void  IEvent::BottomEventExtraInfo::setLoudness(std::string _string){
  loudness = _string;
}
std::string  IEvent::BottomEventExtraInfo::getSpatialization(){
  return spatialization;
}
void  IEvent::BottomEventExtraInfo::setSpatialization(std::string _string){
  spatialization = _string;
}
std::string  IEvent::BottomEventExtraInfo::getReverb(){
  return reverb;
}
void  IEvent::BottomEventExtraInfo::setReverb(std::string _string){
  reverb = _string;
}

std::string  IEvent::BottomEventExtraInfo::getFilter(){
  return filter;
}
void  IEvent::BottomEventExtraInfo::setFilter(std::string _string){
  filter = _string;
}

void IEvent::BottomEventExtraInfo::setChildTypeFlag(int _type){
	childTypeFlag = _type;
};

int IEvent::BottomEventExtraInfo::getChildTypeFlag(){
	return childTypeFlag;
}


EventBottomModifier* IEvent::BottomEventExtraInfo::getModifiers(){
  return modifiers;
}


EventBottomModifier* IEvent::BottomEventExtraInfo::addModifier(){
  EventBottomModifier* newModifier = new EventBottomModifier();
  if (modifiers == NULL){
    modifiers = newModifier;
    return modifiers;
  } //special case


  EventBottomModifier* tail = modifiers;
  while (tail->next!= NULL){
    tail = tail->next;
  }
  tail->next = newModifier;
  return newModifier;

}

void IEvent::BottomEventExtraInfo::removeModifier(
  EventBottomModifier* _modifier){
  if (modifiers == _modifier){ //if modifier to be removed is the head of list
    if (modifiers->next == NULL){ //the only modifier
      delete modifiers;
      modifiers = NULL;
      return;
    }
    else { //somethingelse in the back
      EventBottomModifier* temp = modifiers->next;
      delete modifiers;
      modifiers = temp;
      return;
    }
  }//end handling head of list

  else {
    EventBottomModifier* temp = modifiers->next;
    EventBottomModifier* temp2 = modifiers;
    while (temp != _modifier){
      temp2 = temp;
      temp = temp->next;
    }

    temp2->next = temp->next;
    delete temp;
    return;
  }
}

EventBottomModifier::EventBottomModifier(){
  type = modifierTremolo;
  applyHowFlag = 0;
  probability = "";
  ampValue = "";
  rateValue = "";
  width = "";
  groupName = "";
  next = NULL;
}

EventBottomModifier::~EventBottomModifier(){
}

ModifierType EventBottomModifier::getModifierType(){
  return type;
}
void EventBottomModifier::setModifierType(ModifierType _type){
  type = _type;
}

int EventBottomModifier::getApplyHowFlag(){
  return applyHowFlag;
}
void EventBottomModifier::setApplyHowFlag(int _flag){
  applyHowFlag = _flag;
}

std::string  EventBottomModifier::getProbability(){
  return probability;
}
void  EventBottomModifier::setProbability(std::string _string){
  probability = _string;
}
std::string  EventBottomModifier::getAmpValue(){
  return ampValue;
}
void  EventBottomModifier::setAmpValue(std::string _string){
  ampValue = _string;
}

std::string  EventBottomModifier::getRateValue(){
  return rateValue;
}
void  EventBottomModifier::setRateValue(std::string _string){
  rateValue = _string;
}

std::string  EventBottomModifier::getWidth(){
  return width;
}
void  EventBottomModifier::setWidth(std::string _string){
  width = _string;
}

std::string  EventBottomModifier::getGroupName(){
  return groupName;
}
void  EventBottomModifier::setGroupName(std::string _string){
  groupName = _string;
}


int EventBottomModifier::getModifierTypeInt(){
  if (type ==   modifierTremolo){
    return 0;
  }
  else if (type ==  modifierVibrato){
    return 1;
  }
  else if (type == modifierGlissando){
    return 2;
  }
  else if (type ==  modifierBend){
    return 3;
  }
  else if (type ==  modifierDetune){
    return 4;
  }
  else if (type ==  modifierAmptrans){
    return 5;
  }
  else if (type ==  modifierFreqtrans){
    return 6;
  }
  else {
    return 7;
  }


}



std::string EventBottomModifier::getSaveToDiskString(){
  std::string stringbuffer = "";
  if (type == modifierTremolo){
    stringbuffer ="              <\n"
      "                \"TREMOLO\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +",\n" +
      "                "+ rateValue +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" +
      groupName + "\">\n              >"));

  }
  else if (type == modifierVibrato){
    stringbuffer ="              <\n"
      "                \"VIBRATO\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +",\n" +
      "                "+ rateValue +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" + groupName +
      "\">\n              >"));

  }
  else if (type == modifierGlissando){
    stringbuffer ="              <\n"
      "                \"GLISSANDO\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" + groupName +
      "\">\n              >"));
  }
  else if (type == modifierBend){
    stringbuffer ="              <\n"
      "                \"BEND\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" +
      groupName + "\">\n              >"));

  }
  else if (type == modifierDetune){
    stringbuffer ="              <\n"
      "                \"DETUNE\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" +
      groupName + "\">\n              >"));

  }
  else if (type == modifierAmptrans){
    stringbuffer ="              <\n"
      "                \"AMPTRANS\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +",\n" +
      "                "+ rateValue +",\n" +
      "                "+ width +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" +
      groupName + "\">\n              >"));


  }
  else if (type == modifierFreqtrans){
    stringbuffer ="              <\n"
      "                \"FREQTRANS\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +",\n" +
      "                "+ rateValue +",\n" +
      "                "+ width +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" +
      groupName + "\">\n              >"));



  }
  else{// type == modifierWave_type
    stringbuffer ="              <\n"
      "                \"WAVE_TYPE\",\n"
      "                "+ probability + ",\n"
      "                "+ ((applyHowFlag ==0)?"\"SOUND\",\n":"\"PARTIAL\",\n") +
      "                "+ ampValue +
      ((groupName ==""||groupName =="")?"\n              >":(
      ",\n                <\"MUT_EX\", \"" +
      groupName + "\">\n              >"));

  }
  return stringbuffer;

}






std::string EventBottomModifier::getSaveLASSIEMetaDataString(){

  char temp1[10];
  char temp2[10];

  sprintf(temp1, "%d",(int) type);
  sprintf(temp2, "%d", applyHowFlag);

  std::string stringbuffer = "<" + string(temp1) + "," + string(temp2)
    + ",`" + probability
    + "`,`" + ampValue
    + "`,`" + rateValue
    + "`,`" + width;


  if (groupName != "" && groupName != ""){
    stringbuffer = stringbuffer +  "`,`" + groupName + "`>";
  }
  else {
    stringbuffer = stringbuffer + "`>";
  }
  return stringbuffer;

}

std::string EventBottomModifier::getXMLString(){

  char temp1[10];
  char temp2[10];

  sprintf(temp1, "%d",(int) type);
  sprintf(temp2, "%d", applyHowFlag);

  std::string stringbuffer =
    "            <Modifier>\n"
    "              <Type>" + string(temp1) + "</Type>\n"
    "              <ApplyHow>" + string(temp2) + "</ApplyHow>\n"
    "              <Probability>" + probability + "</Probability>\n"
    "              <Amplitude>" + ampValue + "</Amplitude>\n"
    "              <Rate>" + rateValue +"</Rate>\n"
    "              <Width>"+ width + "</Width>\n"
    "              <GroupName>" + groupName + "</GroupName>\n"
    "            </Modifier>\n";
  return stringbuffer;

}



void IEvent::EnvelopeExtraInfo::setEnvelopeBuilder(std::string _string){
  envelopeBuilder = _string;
}

std::string IEvent::EnvelopeExtraInfo::getEnvelopeBuilder(){
  return envelopeBuilder;
}


void IEvent::SpatializationExtraInfo::setSpatializationBuilder(
  std::string _string){
  spatializationBuilder = _string;
}

std::string IEvent::SpatializationExtraInfo::getSpatializationBuilder(){
  return spatializationBuilder;
}


void IEvent::ReverbExtraInfo::setReverbBuilder(std::string _string){
  reverbBuilder = _string;
}

std::string IEvent::ReverbExtraInfo::getReverbBuilder(){
  return reverbBuilder;
}

void IEvent::FilterExtraInfo::setFilterBuilder(std::string _string){
  filterBuilder = _string;
}

std::string IEvent::FilterExtraInfo::getFilterBuilder(){
  return filterBuilder;
}

void IEvent::MeasureExtraInfo::setMeasureBuilder(std::string _string){
  measureBuilder = _string;
}

std::string IEvent::MeasureExtraInfo::getMeasureBuilder(){
  return measureBuilder;
}

void IEvent::SieveExtraInfo::setSieveBuilder(std::string _string){
  sieveBuilder = _string;
}

std::string IEvent::SieveExtraInfo::getSieveBuilder(){
  return sieveBuilder;
}

void IEvent::PatternExtraInfo::setPatternBuilder(std::string _string){
  patternBuilder = _string;
}

std::string IEvent::PatternExtraInfo::getPatternBuilder(){
  return patternBuilder;
}



IEvent::BottomEventExtraInfo::BottomEventExtraInfo(int _childTypeFlag){

	childTypeFlag = _childTypeFlag;

  extern map<const char*, FileValue*, ltstr> file_data;
  FileValue* value;


  value = file_data["LASSIEBOTTOMfrequencyFlag"];
  frequencyFlag = (value == NULL)? 0: value->getInt();

  //0 = hertz, 1 =] power of two
  value = file_data["LASSIEBOTTOMfrequencyContinuumFlag"];
  frequencyContinuumFlag = (value == NULL)? 0: value->getInt();

  value = file_data["LASSIEBOTTOMfrequencyEntry1"];
  frequencyEntry1 = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMfrequencyEntry2"];
  frequencyEntry2  = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMloudness"];
  loudness  = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMspatialization"];
  spatialization  = (value == NULL)? "": value->getString();

  value = file_data["LASSIEBOTTOMreverb"];
  reverb  = (value == NULL)? "": value->getString();


  //read modifiers
  modifiers = NULL;


  value = file_data["LASSIEBOTTOMmodifiers"];

  if (value != NULL ){
    std::list<FileValue> modifierList = value->getList();



    std::list<FileValue>::iterator modifierListIter =modifierList.begin();


    EventBottomModifier* currentModifier = NULL;
    EventBottomModifier* previousModifier = NULL;
    while (modifierListIter!= modifierList.end()){
      currentModifier = new EventBottomModifier();
      std::list<FileValue> thisModifierList = modifierListIter->getList();
      std::list<FileValue>::iterator modifierIter =thisModifierList.begin();


      currentModifier->setModifierType((ModifierType) modifierIter->getInt());

      modifierIter++;
      currentModifier->setApplyHowFlag(modifierIter->getInt());

      modifierIter++;
      currentModifier->setProbability(modifierIter->getString());

      modifierIter++;
      currentModifier->setAmpValue(modifierIter->getString());

      modifierIter++;
      currentModifier->setRateValue(modifierIter->getString());

      modifierIter++;
      currentModifier->setWidth(modifierIter->getString());

      modifierIter++;

      if (modifierIter != thisModifierList.end()){
        currentModifier->setGroupName(modifierIter->getString());
      }

      if (previousModifier ==NULL){
        modifiers = currentModifier;
        previousModifier = currentModifier;

      }
      else{
        previousModifier->next = currentModifier;
        previousModifier = currentModifier;
      }

      modifierListIter++;

    }//end while

  } //end if


}





EventLayer::EventLayer(FileValue* _thisLayerFileValue,IEvent* _thisEvent){
  thisIEvent = _thisEvent;


  std::list<FileValue> fileValueList = _thisLayerFileValue->getList();

  std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();

  //the first value in the list is teh name
  byLayer = fileValueListIter->getString();

  fileValueListIter ++;
  std::list<FileValue> discretePackages = fileValueListIter->getList();

  std::list<FileValue>::iterator discretePackagesIter = discretePackages.begin();
  if (discretePackagesIter->getInt() == 666){//see dummy
    return;
  }

  for (discretePackagesIter;
    discretePackagesIter!= discretePackages.end();
    discretePackagesIter++){
    children.push_back(new EventDiscretePackage (&(*discretePackagesIter)))  ;
  }


}


EventDiscretePackage::EventDiscretePackage( FileValue* _thisPackageFileValue){
  event = NULL;

  std::list<FileValue> stringList = _thisPackageFileValue->getList();
  std::list<FileValue>::iterator i = stringList.begin();

  eventName = i->getString();
  i ++;
  eventType = (EventType) i->getInt();
  i ++;
  weight = i->getString();
  i ++;
  attackEnv = i->getString();
  i ++;
  attackEnvScale = i->getString();
  i ++;
  durationEnv = i->getString();
  i ++;
  durationEnvScale = i->getString();
}

std::string EventLayer::getLASSIEMetaDataString(){
  std::string a = "<`"+byLayer+ "`,<";

  std::list<EventDiscretePackage*>::iterator discretePackagesIter =
    children.begin();

  while (discretePackagesIter != children.end()){
    a = a + (*discretePackagesIter)->getLASSIEMetadataString();

    discretePackagesIter++;
    if (discretePackagesIter != children.end()){
      a = a + ",";
    }
  }

  if (children.size() ==0){
    a = a + "666"; //dummy
  }
  a = a + ">>";

  return a;
}



std::string EventLayer::getXMLString(){

  std::list<EventDiscretePackage*>::iterator discretePackagesIter =
    children.begin();

  string packageBuffer = "";

  while (discretePackagesIter != children.end()){
    packageBuffer = packageBuffer + (*discretePackagesIter)->getXMLString();
    discretePackagesIter++;
  }

  string stringBuffer =
    "        <Layer>\n";
  stringBuffer = stringBuffer +
    "          <ByLayer>" + byLayer + "</ByLayer>\n"
    "          <DiscretePackages>\n" + packageBuffer +
    "          </DiscretePackages>\n"
    "        </Layer>\n";

  return stringBuffer;

}



std::string EventDiscretePackage::getLASSIEMetadataString(){

  char charBuffer[5];
  sprintf (charBuffer, "%d", (int) event->getEventType());

  std::string a = "<`" + event->getEventName() + "`,"
              + string(charBuffer) +",`" //this is eventType
              + weight + "`,`"
              + attackEnv + "`,`"
              + attackEnvScale + "`,`"
              + durationEnv + "`,`"
              + durationEnvScale + "`>" ;

  return a;

}


std::string EventDiscretePackage::getXMLString(){
  //string a = "";

  char charBuffer[5];
  sprintf (charBuffer, "%d", (int) event->getEventType());

  std::string a =
    "            <Package>\n"
    "              <EventName>" + event->getEventName() + "</EventName>\n"
    "              <EventType>" + string(charBuffer) +"</EventType>\n" //this is eventType
    "              <Weight>" + weight + "</Weight>\n"
    "              <AttackEnvelope>"+ attackEnv + "</AttackEnvelope>\n"
    "              <AttackEnvelopeScale>"+ attackEnvScale + "</AttackEnvelopeScale>\n"
    "              <DurationEnvelope>"+ durationEnv + "</DurationEnvelope>\n"
    "              <DurationEnvelopeScale>"+ durationEnvScale + "</DurationEnvelopeScale>\n"
    "            </Package>\n";

  return a;

}


void IEvent::link(ProjectViewController* _projectView){
  std::list<EventLayer*>::iterator i = layers.begin();
  for (i ; i != layers.end(); i ++){
    (*i)->link(_projectView, this); //link each layer

  }





}


void EventLayer::link(ProjectViewController* _projectView, IEvent* _thisEvent){
  std::list<EventDiscretePackage*>::iterator i = children.begin();
  for (i ; i != children.end(); i ++){
    if (!(*i)->link(_projectView, _thisEvent)){ //link each layer
      cout<<"Linking with parent failed, removing "<< (*i)->eventName<<" from"
      " its parent."<<endl;
      children.remove(*i--);
      //delete *i;

    }




  }

}

bool EventDiscretePackage::link(
  ProjectViewController* _projectView, IEvent* _thisEvent){
    event = _projectView->getEventByTypeAndName(eventType,eventName);

    if (event==NULL){
      cout<<"Warning! event: "<<eventName<<" is not found, It is included in "<<
        _thisEvent->getEventName()<<" , but the file is not presented."<<endl;
    //cout<<"_thisEvent is: "<< _thisEvent->getEventName()<<", eventName is: "<<eventName<<endl;
      return false;
    }
    event->addParent(_thisEvent);
    return true;
}









void IEvent::parseNonEvent(){
  extern map<const char*, FileValue*, ltstr> file_data;


  FileValue* value;

  value = file_data["LASSIEeventName"];
  if (value!= NULL){
    std::list<FileValue> nameAndOrderList = value->getList();
    if (nameAndOrderList.size()==0){ //old file format
      eventOrderInPalette = /* rand() */ -1; // maintainer: why rand???
    }

    else {
      std::list<FileValue>::iterator iter = nameAndOrderList.begin();

      iter++;
      value = &(*iter);
      eventOrderInPalette = value->getInt();
    }
  }
  else {

     //old file format
    eventOrderInPalette = /* rand() */ -1;
  }

  if (eventType == eventSound){
    extraInfo = (EventExtraInfo*) new SoundExtraInfo();
    value = file_data["LASSIESOUNDnumPartials"];
    //extraInfo-> setNumPartials((value == NULL)? "": value->getString());

    value = file_data["LASSIESOUNDdeviation"];
    extraInfo-> setDeviation((value == NULL)? "": value->getString());

    value = file_data["LASSIESOUNDspectrum"];

    std::list<FileValue> fileValueList = value->getList();
    std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();
    SpectrumPartial* thisPartial = NULL;
    if (fileValueList.size()!= 0){
    	thisPartial = extraInfo->getSpectrumPartials();
      thisPartial->envString = fileValueListIter->getString();
      fileValueListIter++;
    }

    for (fileValueListIter;
      fileValueListIter!= fileValueList.end();
      fileValueListIter++){
      thisPartial = extraInfo->addPartial();
      thisPartial->envString = fileValueListIter->getString();
    }

    //extraInfo->setSpectrum((value == NULL)? "": value->getString());
  }
  else if (eventType == eventEnv){
    extraInfo = (EventExtraInfo*) new EnvelopeExtraInfo();
    value = file_data["LASSIEENV"];
    extraInfo->setEnvelopeBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventSiv){
    extraInfo = (EventExtraInfo*) new SieveExtraInfo();
    value = file_data["LASSIESIV"];
    extraInfo->setSieveBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventSpa){
    extraInfo = (EventExtraInfo*) new SpatializationExtraInfo();
    value = file_data["LASSIESPA"];
    extraInfo->setSpatializationBuilder(
      (value == NULL)? "": value->getString());

  }
  else if (eventType == eventPat){
    extraInfo = (EventExtraInfo*) new PatternExtraInfo();
    value = file_data["LASSIEPAT"];
    extraInfo->setPatternBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventRev){
    extraInfo = (EventExtraInfo*) new ReverbExtraInfo();
    value = file_data["LASSIEREV"];
    extraInfo->setReverbBuilder((value == NULL)? "": value->getString());

  }
  else if (eventType == eventNote){
    extraInfo = (EventExtraInfo*) new NoteExtraInfo();
    value = file_data["noteModifiers"];
    if (value){
      std::list<FileValue> fileValueList = value->getList();
      std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();

      for (fileValueListIter;
        fileValueListIter!= fileValueList.end();
        fileValueListIter++){
        extraInfo->addNoteModifiers(fileValueListIter->getString());
      }
    }
  }

}



void IEvent::addParent(IEvent* _event){
  parents.push_back(_event);
}


bool IEvent::removeParent(IEvent* _event){
  bool success = false;
  std::list<IEvent*>::iterator iter =  parents.begin();

  while (iter != parents.end()){
    if (*iter == _event){
      success = true;
      break;
    }
    iter++;
  } //end while

  if (success){
    parents.erase(iter);
  }

  return success;

}



bool EventLayer::removeChild(EventDiscretePackage* _child){
  bool success = false;
  std::list<EventDiscretePackage*>::iterator iter =  children.begin();

  while (iter != children.end()){
    if (*iter == _child){
      success = true;
      break;
    }
    iter++;
  } //end while

  if (success){
    children.erase(iter);
  }

  delete _child;
  return success;


}




void EventLayer::deleteLayer(){

  if (thisIEvent!= NULL){
    thisIEvent->deleteLayer (this);
  }

}

void IEvent::deleteLayer (EventLayer* _deleteLayer){

  std::list<EventLayer*>::iterator i = layers.begin();

  while (*i != _deleteLayer){
    i++;
  }

  layers.erase(i);

  delete (*i);

}



IEvent::NoteExtraInfo::NoteExtraInfo(){
}

IEvent::NoteExtraInfo::~NoteExtraInfo(){
}

std::list<std::string> IEvent::NoteExtraInfo::getNoteModifiers(){
  return modifiers;
}

void IEvent::NoteExtraInfo::addNoteModifiers(std::string _modifier){
  modifiers.push_back(_modifier);
}


void IEvent::NoteExtraInfo::clearNoteModifiers(){
  modifiers.clear();
}


IEvent::IEvent(IEvent* _original, string _newName){
	eventName = _newName;
  oldEventName = "";
  eventOrderInPalette = -1;
	eventType = _original->eventType;
  maxChildDur = _original->maxChildDur;

  unitsPerSecond = _original->unitsPerSecond;
  timeSignatureEntry1 = _original->timeSignatureEntry1;
  timeSignatureEntry2 = _original->timeSignatureEntry2;

  //0 = as note value, 1 = as fraction
  tempoMethodFlag = _original->tempoMethodFlag;
  tempoPrefix = _original-> tempoPrefix;
  tempoNoteValue = _original-> tempoNoteValue;
  tempoFractionEntry1 = _original->tempoFractionEntry1;
  tempoFractionEntry2 = _original->tempoFractionEntry2;
  tempoValueEntry = _original->tempoValueEntry;

  // 0 = fixed, 1 = density
  flagNumChildren    = _original->flagNumChildren;
  numChildrenEntry1  = _original->numChildrenEntry1;
  numChildrenEntry2  = _original->numChildrenEntry2;
  numChildrenEntry3  = _original->numChildrenEntry3;
  changedButNotSaved = true;

  childEventDefEntry1 = _original->childEventDefEntry1;
  childEventDefEntry2 = _original->childEventDefEntry2;
  childEventDefEntry3 = _original->childEventDefEntry3;

  flagChildEventDef             = _original->flagChildEventDef;
  flagChildEventDefStartType    = _original->flagChildEventDefStartType;
  flagChildEventDefDurationType = _original->flagChildEventDefDurationType;


  childEventDefAttackSieve = _original->childEventDefAttackSieve;
  childEventDefDurationSieve = _original->childEventDefDurationSieve;

  filter = _original->filter;
  reverb = _original->reverb;
  spatialization = _original->spatialization;
  measurement = _original->measurement;

  if (eventType ==eventBottom){
  	extraInfo = (EventExtraInfo*)
  	  new BottomEventExtraInfo((BottomEventExtraInfo*) _original->extraInfo);
  	  modifiers = NULL;

  }
  else if (eventType== eventRev){
  	extraInfo = (EventExtraInfo*)
  	  new ReverbExtraInfo((ReverbExtraInfo*) _original->extraInfo);
  }
  else if (eventType== eventFil){
  	extraInfo = (EventExtraInfo*)
  	  new FilterExtraInfo((FilterExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventMea){
    extraInfo = (EventExtraInfo*)
      new MeasureExtraInfo((MeasureExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventSiv){
  	extraInfo = (EventExtraInfo*)
  	  new SieveExtraInfo((SieveExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventEnv){
  	extraInfo = (EventExtraInfo*)
  	  new EnvelopeExtraInfo((EnvelopeExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventSpa){
  	extraInfo = (EventExtraInfo*)
  	  new SpatializationExtraInfo(
  	    (SpatializationExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventPat){
  	extraInfo = (EventExtraInfo*)
  	  new PatternExtraInfo((PatternExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventSound){
  	extraInfo = (EventExtraInfo*)
  	  new SoundExtraInfo((SoundExtraInfo*) _original->extraInfo);
  }
  else if (eventType == eventNote){
  	extraInfo = (EventExtraInfo*)
  	  new NoteExtraInfo((NoteExtraInfo*) _original->extraInfo);
  }

  //layers go here

  list<EventLayer*>::iterator iter = _original->layers.begin();

  while (iter!= _original->layers.end()){

  	EventLayer* newLayer = new EventLayer(this, *iter);

  	layers.push_back(newLayer);
  	iter++;
  }


  //modifiers
  // modifiers of bottom is already handled by its extrainfo
  if (eventType ==eventTop||
      eventType ==eventHigh||
      eventType == eventMid||
      eventType == eventLow){

    if (_original->modifiers == NULL){
    	modifiers = NULL;

    }
    else{
  	modifiers = new EventBottomModifier(_original->modifiers);

  	EventBottomModifier* currentOriginalModifier = _original->modifiers;
  	EventBottomModifier* prevModifier = NULL;
  	EventBottomModifier* currentModifier = modifiers;

  	while (currentOriginalModifier->next!= NULL){
  		currentOriginalModifier = currentOriginalModifier->next;
  		prevModifier= currentModifier;
  		currentModifier = new EventBottomModifier(currentOriginalModifier);
  		prevModifier->next = currentModifier;

  	}
    }
  }



}




IEvent::ReverbExtraInfo::ReverbExtraInfo(ReverbExtraInfo* _original){
	reverbBuilder = _original->reverbBuilder;
}
IEvent::FilterExtraInfo::FilterExtraInfo(FilterExtraInfo* _original){
	filterBuilder = _original->filterBuilder;
}

IEvent::MeasureExtraInfo::MeasureExtraInfo(MeasureExtraInfo* _original){
  measureBuilder = _original->measureBuilder;
}

IEvent::NoteExtraInfo::NoteExtraInfo(NoteExtraInfo* _original){
	 modifiers = _original->modifiers;
}




IEvent::SoundExtraInfo::SoundExtraInfo(SoundExtraInfo* _original){

    numPartials = _original->numPartials;
    deviation = _original->deviation;
    spectrumGenBuilder = _original->spectrumGenBuilder;

    SpectrumPartial* currentOriginalPartial = _original->spectrumPartials;
    SpectrumPartial* currentPartial = NULL;
    SpectrumPartial* previousPartial = NULL;
    spectrumPartials = new SpectrumPartial();
    currentPartial = spectrumPartials;
    spectrumPartials->envString = currentOriginalPartial->envString;

    while (currentOriginalPartial->next != NULL){
    	currentOriginalPartial = currentOriginalPartial->next;

    	previousPartial = currentPartial;
    	currentPartial = new SpectrumPartial();
    	currentPartial->envString = currentOriginalPartial->envString;
    	currentPartial->prev = previousPartial;
    	previousPartial->next = currentPartial;


    }


}


IEvent::BottomEventExtraInfo::BottomEventExtraInfo(
  BottomEventExtraInfo* _original){

  // 0 = Well_tempered, 1 = Fundamental, 2 = Continuum
	frequencyFlag = _original->frequencyFlag;
	//0 = hertz, 1 =] power of two
  frequencyContinuumFlag = _original->frequencyContinuumFlag;
  // 0 = sound, 1 = note, 2 = visual
  childTypeFlag = _original->childTypeFlag;
  frequencyEntry1 = _original->frequencyEntry1;
  frequencyEntry2 = _original->frequencyEntry2;
  loudness = _original->loudness;
  spatialization = _original->spatialization;
  reverb = _original->reverb;


  if (_original->modifiers == NULL){
  	modifiers = NULL;

  }
  else{

  	modifiers = new EventBottomModifier(_original->modifiers);

  	EventBottomModifier* currentOriginalModifier = _original->modifiers;
  	EventBottomModifier* prevModifier = NULL;
  	EventBottomModifier* currentModifier = modifiers;

  	while (currentOriginalModifier->next!= NULL){
  		currentOriginalModifier = currentOriginalModifier->next;
  		prevModifier= currentModifier;
  		currentModifier = new EventBottomModifier(currentOriginalModifier);
  		prevModifier->next = currentModifier;

  	}
  }
}


EventBottomModifier::EventBottomModifier(EventBottomModifier* _original){
	next=NULL;
  type = _original->type;
  applyHowFlag = _original->applyHowFlag;
  probability = _original->probability;
  ampValue = _original->ampValue;
  rateValue = _original->rateValue;
  width = _original->width;
  groupName = _original->groupName;

}




EventLayer::EventLayer(IEvent* _thisEvent, EventLayer* _originalLayer){

	byLayer = _originalLayer->byLayer;
 	thisIEvent = _thisEvent;

  list<EventDiscretePackage*>::iterator iter = _originalLayer->children.begin();
  while (iter != _originalLayer->children.end()){

    EventDiscretePackage* newPackage = new EventDiscretePackage(*iter);
    children.push_back(newPackage);

    iter++;

   }

}

EventDiscretePackage::EventDiscretePackage(
  EventDiscretePackage* _originalPackage){
  event = _originalPackage->event;

  // this one and eventName is used to store info to in order to link
  eventType  = _originalPackage->eventType;
  eventName = _originalPackage->eventName;
  weight = _originalPackage->weight;
  attackEnv = _originalPackage->attackEnv;
  attackEnvScale = _originalPackage->attackEnvScale;
  durationEnv = _originalPackage->durationEnv;
  durationEnvScale = _originalPackage->durationEnvScale;
}

void IEvent::setEventOrderInPalette(int _number){
  eventOrderInPalette = _number;
  changedButNotSaved = true;
}

int IEvent::getEventOrderInPalette(){
  return eventOrderInPalette;
}

string IEvent::getEventFolderName(){
    switch(eventType){
    case 0:
      return "Top";
    case 1:
      return "High";
    case 2:
      return "Mid";
    case 3:
      return "Low";
    case 4:
      return "Bottom";
    case 5:
      return "Spectrum";
    case 6:
      return "Envelope";
    case 7:
      return "Sieve";
    case 8:
      return "Spatialization";
    case 9:
      return "Pattern";
    case 10:
      return "Reverb";
    case 11:
      return "Folder";
    case 12:
      return "Note";
    case 13:
      return "Filter";
    case 14:
      return "Measurement";
  }
}


bool IEvent::haveString(string _string){
  size_t position = maxChildDur.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = timeSignatureEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = timeSignatureEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = unitsPerSecond.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = tempoValueEntry.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = tempoFractionEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = tempoFractionEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = numChildrenEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = numChildrenEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = numChildrenEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }

  std::string thisLayer;
  std::list<EventLayer*>::iterator i = layers.begin();
  bool firstNumberExisted = false;
  while(i != layers.end()){
    thisLayer = (*i)->outputChildrenNameString();
    position = thisLayer.find (_string, 0);
    if (position != -1){
       return true;
    }
    i++;
  }

  position = childEventDefEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = childEventDefEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = childEventDefEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = numChildrenEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = numChildrenEntry3.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = childEventDefAttackSieve.find (_string, 0);
  if (position != -1){
    return true;
  }

    position = childEventDefDurationSieve.find (_string, 0);
  if (position != -1){
    return true;
  }

  if (extraInfo!= NULL){
    //return extraInfo->haveString(_string);   this line crashes the program when removing bottom event, so oh well
  }
  return false;

}


bool IEvent::EnvelopeExtraInfo::haveString(string _string){
  size_t position = envelopeBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}

bool IEvent::PatternExtraInfo::haveString(string _string){
  size_t position = patternBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}
bool IEvent::SpatializationExtraInfo::haveString(string _string){
  size_t position = spatializationBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}
bool IEvent::SieveExtraInfo::haveString(string _string){
  size_t position = sieveBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}
bool IEvent::ReverbExtraInfo::haveString(string _string){
  size_t position = reverbBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}

bool IEvent::FilterExtraInfo::haveString(string _string){
  size_t position = filterBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}

bool IEvent::MeasureExtraInfo::haveString(string _string){
  size_t position = measureBuilder.find (_string, 0);
  if (position != -1){
    return true;
  }
  return false;
}

bool IEvent::NoteExtraInfo::haveString(string _string){
  return false;

}


bool IEvent::SoundExtraInfo::haveString(string _string){
  size_t position = deviation.find (_string, 0);
  if (position != -1){
    return true;
  }

  position = spectrumGenBuilder.find(_string, 0);
  if (position != -1){
    return true;
  }

  position = getSoundSpectrumEnvelopesString().find (_string, 0);
  if (position != -1){
    return true;
  }

  return false;

}




bool IEvent::BottomEventExtraInfo::haveString(string _string){
  size_t position = frequencyEntry1.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = frequencyEntry2.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = loudness.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = spatialization.find (_string, 0);
  if (position != -1){
    return true;
  }
  position = reverb.find (_string, 0);
  if (position != -1){
    return true;
  }



  EventBottomModifier* mod = getModifiers();
  if (mod!= NULL){

    string stringbuffer = "";


    while (mod != NULL){
      stringbuffer = stringbuffer + mod->getSaveToDiskString();

      if (mod->next!=NULL){
        stringbuffer = stringbuffer + ",\n";
      }
      mod = mod->next;
    }
    position = stringbuffer.find (_string, 0);
    if (position != -1){
      return true;
    }
  }
  return false;
}



void IEvent::makeSuperColliderCode(){
  /*TODO::
    1. collect the string of all the necessary global settings (such as envelope library, etc)
       so we probably want to go to EnvelopeLibraryWindow and make a function like:
       string outputSCEnvelopeCode(){
          //go through every envelope entry

       }
       and in EnvelopeLibraryEntry class, make a function to actually generate the SC code.

    2. attach the string of the setting of this event to the global settings, and then output it to a file
       (maybe take a look of IEvent::saveAsTHMLB()  )



  */
  cout<<"under construction, see line 3696 of IEvent.cpp"<<endl;
}


string IEvent::getXMLString(){
switch(eventType){
    case 0:
      return getXMLTHMLB();
      break;
    case 1:
      return getXMLTHMLB();
      break;
    case 2:
      return getXMLTHMLB();
      break;
    case 3:
      return getXMLTHMLB();
      break;
    case 4:
      return getXMLTHMLB();
      break;
    case 5:
      return getXMLSound();
      break;
    case 6:
      return getXMLEnv();
      break;
    case 7:
      return getXMLSiv();
      break;
    case 8:
      return getXMLSpa();
      break;
    case 9:
      return getXMLPat();
      break;
    case 10:
      return getXMLRev();
      break;
    case 11: //no need to save folder
      break;
    case 12:
      return getXMLNote();
      break;
    case 13:
      return getXMLFil();
      break;
    case 14:
      return getXMLMea();
      break;


  }

}


string IEvent::getXMLTHMLB(){

  char typeBuffer[10];
  sprintf(typeBuffer, "%d", eventType);
  char charBuffer[10];

  sprintf (charBuffer," %d", eventOrderInPalette);

  char tempoMethodFlagChar[10];
  sprintf(tempoMethodFlagChar, "%d", tempoMethodFlag);

  char tempoPrefixChar[10];
  sprintf(tempoPrefixChar, "%d", tempoPrefix);

  char tempoNoteValueChar[10];
  sprintf(tempoNoteValueChar, "%d", tempoNoteValue);


  char flagNumChildrenChar[10];
  sprintf(flagNumChildrenChar, "%d", flagNumChildren);


  char flagChildEventDefChar[10];
  sprintf(flagChildEventDefChar, "%d", flagChildEventDef);

  char flagChildEventDefStartTypeChar[10];
  sprintf(flagChildEventDefStartTypeChar, "%d", flagChildEventDefStartType);

  char flagChildEventDefDuratonTypeChar[10];
  sprintf(flagChildEventDefDuratonTypeChar, "%d",flagChildEventDefDurationType);


  string layerbuffer = "";

  std::list<EventLayer*>::iterator layersIter = layers.begin();

  while(layersIter != layers.end()){
    layerbuffer = layerbuffer + (*layersIter)->getXMLString();
    layersIter++;
  }




  string stringbuffer = "    <Event orderInPalette='" + string(charBuffer)+"'>\n"
    "      <EventType>" + string(typeBuffer) + "</EventType>\n"
    "      <Name>" + eventName + "</Name>\n"
    "      <MaxChildDuration>" + maxChildDur + "</MaxChildDuration>\n"
    "      <EDUPerBeat>" + unitsPerSecond + "</EDUPerBeat>\n"
    "      <TimeSignature>\n"
    "        <Entry1>"+ timeSignatureEntry1 + "</Entry1>\n"
    "        <Entry2>"+ timeSignatureEntry2 + "</Entry2>\n"
    "      </TimeSignature>\n"
    "      <Tempo>\n"
    "        <MethodFlag>" + tempoMethodFlagChar + "</MethodFlag>\n"
    "        <Prefix>" + tempoPrefixChar + "</Prefix>\n"
    "        <NoteValue>" + tempoNoteValueChar + "</NoteValue>\n"
    "        <FractionEntry1>" + tempoFractionEntry1 + "</FractionEntry1>\n"
    "        <FractionEntry2>" + tempoFractionEntry2 + "</FractionEntry2>\n"
    "        <ValueEntry>" + tempoValueEntry + "</ValueEntry>\n"
    "      </Tempo>\n"
    "      <NumberOfChildren>\n"
    "        <MethodFlag>"+flagNumChildrenChar +"</MethodFlag>\n"
    "        <Entry1>" + numChildrenEntry1 + "</Entry1>\n"
    "        <Entry2>" + numChildrenEntry2 + "</Entry2>\n"
    "        <Entry3>" + numChildrenEntry3 + "</Entry3>\n"
    "      </NumberOfChildren>\n"
    "      <ChildEventDefinition>\n"
    "        <Entry1>"+ childEventDefEntry1 + "</Entry1>\n"
    "        <Entry2>"+ childEventDefEntry2 + "</Entry2>\n"
    "        <Entry3>"+ childEventDefEntry3 + "</Entry3>\n"
    "        <AttackSieve>"+ childEventDefAttackSieve + "</AttackSieve>\n"
    "        <DurationSieve>" + childEventDefDurationSieve + "</DurationSieve>\n"
    "        <DefinitionFlag>" + flagChildEventDefChar + "</DefinitionFlag>\n"
    "        <StartTypeFlag>" + flagChildEventDefStartTypeChar + "</StartTypeFlag>\n"
    "        <DurationTypeFlag>" + flagChildEventDefDuratonTypeChar + "</DurationTypeFlag>\n"
    "      </ChildEventDefinition>\n"
    "      <Layers>\n" + layerbuffer + "      </Layers>\n"
    "      <Spatialization>" + spatialization + "</Spatialization>\n"
    "      <Reverb>" + reverb + "</Reverb>\n"
    "      <Filter>" + filter + "</Filter>\n" ;


    if (eventType == eventBottom){



      char frequencyFlagChar[10];
      sprintf(frequencyFlagChar, "%d", extraInfo->getFrequencyFlag());
      char frequencyContinuumFlagChar[10];
      sprintf(frequencyContinuumFlagChar, "%d", extraInfo->getFrequencyContinuumFlag());

      string modifiersbuffer = "        <Modifiers>\n";
      EventBottomModifier* mod = extraInfo->getModifiers();

      while (mod != NULL){
        modifiersbuffer = modifiersbuffer + mod->getXMLString();
        mod = mod->next;
      }
      modifiersbuffer = modifiersbuffer +"        </Modifiers>\n";

      string bottomBuffer = "";
      bottomBuffer = bottomBuffer +
        "      <ExtraInfo>\n"
        "        <FrequencyInfo>\n"
        "          <FrequencyFlag>" + frequencyFlagChar + "</FrequencyFlag>\n"
        "          <FrequencyContinuumFlag>" + frequencyContinuumFlagChar + "</FrequencyContinuumFlag>\n"
        "          <FrequencyEntry1>" + extraInfo->getFrequencyEntry1() + "</FrequencyEntry1>\n"
        "          <FrequencyEntry2>" + extraInfo->getFrequencyEntry2() + "</FrequencyEntry2>\n"
        "        </FrequencyInfo>\n"
        "        <Loudness>" + extraInfo->getLoudness() + "</Loudness>\n"
        "        <Spatialization>" + extraInfo->getSpatialization() + "</Spatialization>\n"
        "        <Reverb>" + extraInfo->getReverb() + "</Reverb>\n"
        "        <Filter>" + extraInfo->getFilter() + "</Filter>\n"
          + modifiersbuffer +
        "      </ExtraInfo>\n";
      stringbuffer = stringbuffer + bottomBuffer;

    }

    else {// modifiers

      string modifiersbuffer = "      <Modifiers>\n";
      EventBottomModifier* mod = modifiers;
      while (mod != NULL){
        modifiersbuffer = modifiersbuffer + mod->getXMLString();
        mod = mod->next;
      }
      modifiersbuffer = modifiersbuffer +"      </Modifiers>\n";
      stringbuffer = stringbuffer + modifiersbuffer;
    }




  stringbuffer = stringbuffer + "    </Event>\n";
  return stringbuffer;

}


string IEvent::getXMLSound(){

  char orderInPaletteChar[10];
  sprintf (orderInPaletteChar," %d", eventOrderInPalette);
  string buffer =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>5</EventType>\n" //5 is sound
    "      <Name>" + eventName + "</Name>\n"
    "      <NumberOfPartials>" + extraInfo->getNumPartials() + "</NumberOfPartials>\n"
    "      <Deviation>" + extraInfo->getDeviation() + "</Deviation>\n"
    "      <GenerateSpectrum>" + extraInfo->getSpectrumGenBuilder() + "</GenerateSpectrum>\n"
    "      <Spectrum>\n" + extraInfo->getSpectrumXMLString() +
    "      </Spectrum>\n"
    "    </Event>\n";


  return buffer;
}


string IEvent::getXMLEnv(){

  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>6</EventType>\n" //6 is envelope
    "      <Name>" + eventName + "</Name>\n"
    "      <EnvelopeBuilder>" + extraInfo->getEnvelopeBuilder() + "</EnvelopeBuilder>\n"
    "    </Event>\n";

  return buffer;
}



string IEvent::getXMLSiv(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>7</EventType>\n" //7 is siv
    "      <Name>" + eventName + "</Name>\n"
    "      <SieveBuilder>" + extraInfo->getSieveBuilder() + "</SieveBuilder>\n"
    "    </Event>\n";

  return buffer;

}





string IEvent::getXMLSpa(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>8</EventType>\n" //8 is spa
    "      <Name>" + eventName + "</Name>\n"
    "      <SpatializationBuilder>" + extraInfo->getSpatializationBuilder() + "</SpatializationBuilder>\n"
    "    </Event>\n";

  return buffer;

}
string IEvent::getXMLPat(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>9</EventType>\n" // is pattern
    "      <Name>" + eventName + "</Name>\n"
    "      <PatternBuilder>" + extraInfo->getPatternBuilder() + "</PatternBuilder>\n"
    "    </Event>\n";

  return buffer;

}
string IEvent::getXMLRev(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>10</EventType>\n" //10 is Reverb
    "      <Name>" + eventName + "</Name>\n"
    "      <ReverbBuilder>" + extraInfo->getReverbBuilder() + "</ReverbBuilder>\n"
    "    </Event>\n";

  return buffer;

}

string IEvent::getXMLFil(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>13</EventType>\n" //13 is filter
    "      <Name>" + eventName + "</Name>\n"
    "      <FilterBuilder>" + extraInfo->getFilterBuilder() + "</FilterBuilder>\n"
    "    </Event>\n";

  return buffer;

}


string IEvent::getXMLMea(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>14</EventType>\n"
    "      <Name>" + eventName + "</Name>\n"
    "      <MeasureBuilder>" + extraInfo->getMeasureBuilder() + "</MeasureBuilder>\n"
    "    </Event>\n";

  return buffer;

}

string IEvent::getXMLNote(){
  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  string modifiersBuffer = "      <Modifiers>\n";

  std::list<std::string> modifiers = extraInfo->getNoteModifiers();

  std::list<std::string>::iterator iter = modifiers.begin();

  while (iter!= modifiers.end()){
    modifiersBuffer = modifiersBuffer + "        <Modifier>"+*iter + "</Modifier>\n";
    iter ++;
  }

  modifiersBuffer = modifiersBuffer + "      </Modifiers>\n";



  string buffer =
    "    <Event orderInPalette='" + string(charBuffer) +"'>\n"
    "      <EventType>12</EventType>\n" //12 is Note
    "      <Name>" + eventName + "</Name>\n" + modifiersBuffer +
    "    </Event>\n";

    return buffer;


}





void IEvent::buildNonEventFromDOM(DOMElement* _element){
  DOMCharacterData* textData;
  char* charBuffer;
  if (eventType == eventSound){
    extraInfo = (EventExtraInfo*) new SoundExtraInfo();
    extraInfo->setNumPartials(getFunctionString(_element));


    DOMElement* deviationElement = _element->getNextElementSibling();
    extraInfo->setDeviation(getFunctionString(deviationElement));

    DOMElement* spectrumGenElement = deviationElement->getNextElementSibling();
    std::string next_fun = getFunctionString(spectrumGenElement);

    //default is spectrum is not present
    DOMElement* thisPartialElement = spectrumGenElement->getFirstElementChild();

    if (next_fun == ""){
      //1.spectrum is present but empty
      if (spectrumGenElement->getNextElementSibling()!=NULL){
        thisPartialElement = spectrumGenElement->getNextElementSibling()->getFirstElementChild();
      }
      //2. spectrum is not present, do nothing
    }
    else if (int(next_fun[11]) == 83 && int(next_fun[12]) == 112) {
      //if spectrum is present and filled
      extraInfo->setSpectrumGenBuilder(getFunctionString(spectrumGenElement));
      thisPartialElement = spectrumGenElement->getNextElementSibling()->getFirstElementChild();
    }


    SpectrumPartial* thisPartial = NULL;
    if (thisPartialElement){
    	thisPartial = extraInfo->getSpectrumPartials();
      thisPartial->envString = getFunctionString(thisPartialElement);
      thisPartialElement = thisPartialElement->getNextElementSibling();
    }

    while( thisPartialElement){
      thisPartial = extraInfo->addPartial();
      thisPartial->envString = getFunctionString(thisPartialElement);
      thisPartialElement = thisPartialElement->getNextElementSibling();
    }

  }

  else if (eventType == eventEnv){
    extraInfo = (EventExtraInfo*) new EnvelopeExtraInfo();
    extraInfo->setEnvelopeBuilder(getFunctionString(_element));
  }

  else if (eventType == eventSiv){
    extraInfo = (EventExtraInfo*) new SieveExtraInfo();
    extraInfo->setSieveBuilder(getFunctionString(_element));
  }

  else if (eventType == eventSpa){
    extraInfo = (EventExtraInfo*) new SpatializationExtraInfo();
    extraInfo->setSpatializationBuilder(getFunctionString(_element));
  }

  else if (eventType == eventPat){
    extraInfo = (EventExtraInfo*) new PatternExtraInfo();
    extraInfo->setPatternBuilder(getFunctionString(_element));
  }
  else if (eventType == eventRev){
    extraInfo = (EventExtraInfo*) new ReverbExtraInfo();
    extraInfo->setReverbBuilder(getFunctionString(_element));
  }


  else if (eventType == eventFil){
    extraInfo = (EventExtraInfo*) new FilterExtraInfo();
    extraInfo->setFilterBuilder(getFunctionString(_element));
  }

  else if (eventType == eventMea){
    extraInfo = (EventExtraInfo*) new MeasureExtraInfo();
    extraInfo->setMeasureBuilder(getFunctionString(_element));
  }

  else if (eventType == eventNote){
    extraInfo = (EventExtraInfo*) new NoteExtraInfo();
    DOMElement* thisModifier = _element->getFirstElementChild();

    while (thisModifier){
      extraInfo->addNoteModifiers(getFunctionString(thisModifier));
      thisModifier = thisModifier -> getNextElementSibling();
    }
  }
}

IEvent::IEvent(DOMElement* _domElement){

  XMLCh* orderInPaletteXMLCh = XMLString::transcode ("orderInPalette");
  string orderInPalette = XMLString::transcode(_domElement->getAttribute(orderInPaletteXMLCh));
  eventOrderInPalette = atoi(orderInPalette.c_str());


  DOMElement* eventTypeElement = _domElement->getFirstElementChild();
  DOMCharacterData* textData = ( DOMCharacterData*) eventTypeElement->getFirstChild();
  char* charBuffer = XMLString::transcode(textData->getData());
  eventType = (EventType) atoi (charBuffer);
  XMLString::release(&charBuffer);

  DOMElement* eventNameElement = eventTypeElement->getNextElementSibling();
  textData = ( DOMCharacterData*) eventNameElement->getFirstChild();
  charBuffer= XMLString::transcode(textData->getData());
  eventName = charBuffer;
  XMLString::release(&charBuffer);

  if (eventType >= 5){
    buildNonEventFromDOM( eventNameElement->getNextElementSibling());
    return;
  }

  //maxChildDur
  DOMElement* thisElement = eventNameElement->getNextElementSibling();
  maxChildDur = getFunctionString(thisElement);

  //EDUPerBeat
  thisElement = thisElement->getNextElementSibling();
  unitsPerSecond = getFunctionString(thisElement);


  // TimeSignature
  thisElement = thisElement->getNextElementSibling();
  DOMElement* secondLevelElement = thisElement->getFirstElementChild();
  timeSignatureEntry1 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  timeSignatureEntry2 = getFunctionString(secondLevelElement);


  //Tempo

  thisElement = thisElement->getNextElementSibling();
  secondLevelElement = thisElement->getFirstElementChild();
  charBuffer = (char*)getFunctionString(secondLevelElement).c_str();
  tempoMethodFlag = atoi(charBuffer);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  charBuffer= XMLString::transcode(textData->getData());
  charBuffer =  (char*)getFunctionString(secondLevelElement).c_str();

  tempoPrefix = (TempoPrefix) atoi(charBuffer);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  charBuffer =  (char*)getFunctionString(secondLevelElement).c_str();
  tempoNoteValue = (TempoNoteValue)atoi(charBuffer);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  tempoFractionEntry1 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  tempoFractionEntry2 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  tempoValueEntry = getFunctionString(secondLevelElement);



  //NumberOfChildren

  thisElement = thisElement->getNextElementSibling();
  secondLevelElement = thisElement->getFirstElementChild();
  charBuffer =  (char*)getFunctionString(secondLevelElement).c_str();
  flagNumChildren = atoi(charBuffer);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  numChildrenEntry1 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  numChildrenEntry2 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  numChildrenEntry3 = getFunctionString(secondLevelElement);

  //childEventDef

  thisElement = thisElement->getNextElementSibling();
  secondLevelElement = thisElement->getFirstElementChild();
  childEventDefEntry1 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  childEventDefEntry2 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  childEventDefEntry3 = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  childEventDefAttackSieve = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  childEventDefDurationSieve = getFunctionString(secondLevelElement);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  charBuffer =  (char*)getFunctionString(secondLevelElement).c_str();
  flagChildEventDef = atoi(charBuffer);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  charBuffer =  (char*)getFunctionString(secondLevelElement).c_str();
  flagChildEventDefStartType = atoi(charBuffer);

  secondLevelElement = secondLevelElement->getNextElementSibling();
  charBuffer =  (char*)getFunctionString(secondLevelElement).c_str();
  flagChildEventDefDurationType = atoi( charBuffer);


  //layers
  thisElement = thisElement->getNextElementSibling();
  secondLevelElement = thisElement->getFirstElementChild();
  while (secondLevelElement){
    layers.push_back (new EventLayer(secondLevelElement, this));
    secondLevelElement = secondLevelElement->getNextElementSibling();
  }

  //environment

  if (thisElement->getNextElementSibling() != NULL &&thisElement->getNextElementSibling()->getNextElementSibling() != NULL){
    thisElement = thisElement->getNextElementSibling();
    spatialization = getFunctionString(thisElement);
    thisElement = thisElement->getNextElementSibling();
    reverb = getFunctionString(thisElement);
    thisElement = thisElement->getNextElementSibling();
    filter = getFunctionString(thisElement);
  }


  thisElement=thisElement->getNextElementSibling(); //now it's either bottom or modifiers or empty (missing modifiers tag)

  if (eventType == eventBottom){


    string firstChar = eventName.substr(0,1);
    int childTypeFlag = -1;
    if (firstChar =="s"){
    	childTypeFlag = 0;
    }
    else if (firstChar =="n"){
    	childTypeFlag = 1;
    }
    else {
    	childTypeFlag = 2;
   	}
    extraInfo = (EventExtraInfo*) new BottomEventExtraInfo(childTypeFlag, thisElement); //line 2789
    modifiers = NULL;

  }
  else { //if not bottom, the following element is modifiers
    if (thisElement!= NULL){

      modifiers = BottomEventExtraInfo::buildModifiersFromDOMElement(thisElement->getFirstElementChild());
    }
    else {
      modifiers = NULL;
    }
  }



}


IEvent::BottomEventExtraInfo::BottomEventExtraInfo(int _childTypeFlag, DOMElement* _thisElement){
  childTypeFlag = _childTypeFlag;

  DOMElement* thisElement = _thisElement->getFirstElementChild()->getFirstElementChild();

  char* charBuffer;
  charBuffer =  (char*)getFunctionString(thisElement).c_str();
  frequencyFlag = atoi(charBuffer);



  thisElement = thisElement->getNextElementSibling();
  charBuffer =  (char*)getFunctionString(thisElement).c_str();
  frequencyContinuumFlag = atoi( charBuffer);

  thisElement = thisElement->getNextElementSibling();
  frequencyEntry1 = getFunctionString(thisElement);


  thisElement = thisElement->getNextElementSibling();
  frequencyEntry2 = getFunctionString(thisElement);



  thisElement = _thisElement->getFirstElementChild()->getNextElementSibling();
  //thisElement = thisElement->getNextElementSibling();
  loudness = getFunctionString(thisElement);

  thisElement = thisElement->getNextElementSibling();
  spatialization = getFunctionString(thisElement);;

  thisElement = thisElement->getNextElementSibling();
  reverb = getFunctionString(thisElement);

  thisElement = thisElement->getNextElementSibling();
  DOMElement* temp = thisElement->getNextElementSibling();

  if (temp != NULL){ // in case there is no <Filter></Filter>
    filter = getFunctionString(thisElement);

    modifiers = buildModifiersFromDOMElement(thisElement->getNextElementSibling()->getFirstElementChild());
  }
  else {
    filter = "";
    modifiers = buildModifiersFromDOMElement(thisElement->getFirstElementChild());
  }

}


EventBottomModifier* IEvent::BottomEventExtraInfo::buildModifiersFromDOMElement(DOMElement* _thisModifierElement){
  if (_thisModifierElement == NULL){
    return NULL;
  }

  EventBottomModifier* currentModifier = new EventBottomModifier();


  DOMElement* thisElement = _thisModifierElement->getFirstElementChild();
  char* charBuffer;
  charBuffer =  (char*)getFunctionString(thisElement).c_str();
  currentModifier->setModifierType((ModifierType) atoi(charBuffer));

  thisElement = thisElement->getNextElementSibling();
  charBuffer =  (char*)getFunctionString(thisElement).c_str();
  currentModifier->setApplyHowFlag( atoi( charBuffer));

  thisElement = thisElement->getNextElementSibling();
  currentModifier->setProbability(getFunctionString(thisElement));





  thisElement = thisElement->getNextElementSibling();
  currentModifier->setAmpValue(getFunctionString(thisElement));


  thisElement = thisElement->getNextElementSibling();
  currentModifier->setRateValue(getFunctionString(thisElement));


  thisElement = thisElement->getNextElementSibling();
  currentModifier->setWidth(getFunctionString(thisElement));


  thisElement = thisElement->getNextElementSibling();
  currentModifier->setGroupName(getFunctionString(thisElement));

  currentModifier->next = buildModifiersFromDOMElement(_thisModifierElement->getNextElementSibling());
  return currentModifier;

}









EventDiscretePackage::EventDiscretePackage(DOMElement* _thisPackageElement){

  DOMElement* thisElement = _thisPackageElement->getFirstElementChild();
  char* charBuffer;
  DOMCharacterData* textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    eventName = charBuffer;
    XMLString::release(&charBuffer);
  } else eventName = "";

  thisElement = thisElement->getNextElementSibling();
  textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    eventType = (EventType) atoi( charBuffer);
    XMLString::release(&charBuffer);
  } else eventType = (EventType)0;

  thisElement = thisElement->getNextElementSibling();
  textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    weight = charBuffer;
    XMLString::release(&charBuffer);
  } else weight = "";

    thisElement = thisElement->getNextElementSibling();
  textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    attackEnv = charBuffer;
    XMLString::release(&charBuffer);
  } else attackEnv = "";

    thisElement = thisElement->getNextElementSibling();
  textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    attackEnvScale = charBuffer;
    XMLString::release(&charBuffer);
  } else attackEnvScale = "";

    thisElement = thisElement->getNextElementSibling();
  textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    durationEnv = charBuffer;
    XMLString::release(&charBuffer);
  } else durationEnv = "";

    thisElement = thisElement->getNextElementSibling();
  textData = ( DOMCharacterData*) thisElement->getFirstChild();
  if (textData){
    charBuffer = XMLString::transcode(textData->getData());
    durationEnvScale = charBuffer;
    XMLString::release(&charBuffer);
  } else durationEnvScale = "";

}

EventLayer::EventLayer(DOMElement* _thisLayerElement, IEvent* _thisEvent){
  thisIEvent = _thisEvent;


  //byLayer
  DOMElement* thisElement = _thisLayerElement->getFirstElementChild();
  byLayer = IEvent::getFunctionString(thisElement);


  //discrete packages
  thisElement = thisElement->getNextElementSibling()->getFirstElementChild();
  while(thisElement){
    children.push_back(new EventDiscretePackage(thisElement));
    thisElement = thisElement->getNextElementSibling();
  }

}



std::string IEvent::getFunctionString(DOMElement* _thisFunctionElement){

  char* charBuffer;
  DOMCharacterData* textData;
  string returnString;
  DOMNode* child = _thisFunctionElement->getFirstChild();
  if (child ==NULL){ //not containing any child, return string


    return "";
  }

  XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
  DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
  XMLCh* bla = theSerializer->writeToString (_thisFunctionElement);
  char* toTranscode = XMLString::transcode(bla);
  returnString = string (toTranscode);
  XMLString::release(&toTranscode);
  XMLString::release(&bla);

  int tagLength = (int) XMLString::stringLen(_thisFunctionElement->getTagName());

  delete theSerializer;
  returnString = returnString.substr(tagLength+2, returnString.size() - tagLength * 2 - 5);
  return returnString;
}



std::string IEvent::SoundExtraInfo::getSpectrumMetaData(){
  std::string returnString = "";
  SpectrumPartial* thisPartial = spectrumPartials;

  while (thisPartial!= NULL){
    std::string tempstring = thisPartial->envString;
    if (tempstring == ""){
      tempstring = "";
    }



    returnString = returnString +"`"+ tempstring + "`";
    thisPartial = thisPartial->next;
    if (thisPartial!= NULL){
      returnString = returnString + ",";
    }
  }

  return returnString;

}


std::string IEvent::SoundExtraInfo::getSpectrumXMLString(){
  std::string returnString = "";
  SpectrumPartial* thisPartial = spectrumPartials;

  while (thisPartial!= NULL){
    std::string tempstring = thisPartial->envString;
    returnString = returnString +"        <Partial>"+ tempstring + "</Partial>\n";
    thisPartial = thisPartial->next;
  }

  return returnString;

}






std::string IEvent::SoundExtraInfo::getSoundSpectrumEnvelopesString(){
  std::string returnString = "";
  SpectrumPartial* thisPartial = spectrumPartials;

  while (thisPartial!= NULL){
    returnString = returnString + thisPartial->envString;
    thisPartial = thisPartial->next;
    if (thisPartial!= NULL){
      returnString = returnString + ",";
    }
  }

  return returnString;
}







//modifiers for event (same functions as the bottom version)
EventBottomModifier* IEvent::getModifiers(){
  return modifiers;
}



EventBottomModifier* IEvent::addModifier(){
  EventBottomModifier* newModifier = new EventBottomModifier();
  if (modifiers == NULL){
    modifiers = newModifier;
    return modifiers;
  } //special case


  EventBottomModifier* tail = modifiers;
  while (tail->next!= NULL){
    tail = tail->next;
  }
  tail->next = newModifier;
  return newModifier;

}

void IEvent::removeModifier(
  EventBottomModifier* _modifier){
  if (modifiers == _modifier){ //if modifier to be removed is the head of list
    if (modifiers->next == NULL){ //the only modifier
      delete modifiers;
      modifiers = NULL;
      return;
    }
    else { //somethingelse in the back
      EventBottomModifier* temp = modifiers->next;
      delete modifiers;
      modifiers = temp;
      return;
    }
  }//end handling head of list

  else {
    EventBottomModifier* temp = modifiers->next;
    EventBottomModifier* temp2 = modifiers;
    while (temp != _modifier){
      temp2 = temp;
      temp = temp->next;
    }

    temp2->next = temp->next;
    delete temp;
    return;
  }
}
