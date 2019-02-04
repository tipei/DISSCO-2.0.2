/*******************************************************************************
 *
 *  File Name     : IEvent.h
 *  Date created  : Feb. 2 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains the declaration of IEvent class and
 *                  its related subclasses.
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

#ifndef IEVENT_H
#define IEVENT_H


#include "LASSIE.h"




class ProjectViewController;
/*! \brief The event types including the types defined in CMOD
  *    (Top - Bottom) and types for LASSIE
  *
  *****************************************************************************/


class EventBottomModifier{ //implemented as a linked list for the convenience
public:

  EventBottomModifier();
  EventBottomModifier(EventBottomModifier* _original);
  ~EventBottomModifier();

  ModifierType getModifierType();
  int getModifierTypeInt();
  void setModifierType(ModifierType _type);

  int getApplyHowFlag();
  void setApplyHowFlag(int _flag);

  std::string  getProbability();
  void  setProbability(std::string _string);

  std::string  getAmpValue();
  void  setAmpValue(std::string _string);

  std::string  getRateValue();
  void  setRateValue(std::string _string);

  std::string  getWidth();
  void  setWidth(std::string _string);

  std::string  getGroupName();
  void  setGroupName(std::string _string);



  EventBottomModifier* next;

  std::string getSaveToDiskString();
  std::string getSaveLASSIEMetaDataString();
  std::string getXMLString();


private:
  ModifierType type;
  int applyHowFlag;
  std::string probability;
  std::string ampValue;
  std::string rateValue;
  std::string width;
  std::string groupName;





};

class IEvent; //forward declaration
class EventDiscretePackage{

  public:
    IEvent* event;

    // this one and eventName is used to store info to in order to link
    EventType eventType;
    std::string eventName;
    std::string weight;
    std::string attackEnv;
    std::string attackEnvScale;
    std::string durationEnv;
    std::string durationEnvScale;
    EventDiscretePackage(IEvent* _event){
     event = _event;

     //this is used to store file name of the event for linking after open
     eventName = "";
     weight = "";
     attackEnv = "";
     attackEnvScale = "";
     durationEnv = "";
     durationEnvScale = "";
    }
		EventDiscretePackage(EventDiscretePackage* _originalPackage);
    EventDiscretePackage( FileValue* _thisPackageFileValue);
    EventDiscretePackage(DOMElement* _thisPackageElement);
    std::string getLASSIEMetadataString();
    std::string getXMLString();
    bool link(ProjectViewController* _projectView, IEvent* _thisEvent);

    ~EventDiscretePackage(){}
};



class EventLayer {

  public:
    EventLayer(IEvent* _thisEvent);
    EventLayer(IEvent* _thisEvent, EventLayer* _originalLayer);
    EventLayer(FileValue* _thisLayerFileValue,IEvent* _thisEvent);
    EventLayer(DOMElement* _thisLayerElement, IEvent* _thisEvent);
    ~EventLayer();
    EventDiscretePackage* addChild(IEvent* _child);
    bool removeChild(EventDiscretePackage* _child);
    void showContents();
    void deleteLayer();

    double getChildrenWeightSum();
    int size();
    std::string outputChildrenNameString();
    std::string getLASSIEMetaDataString();
    std::string getXMLString();
    std::list<EventDiscretePackage*> children;
    void link (ProjectViewController* _projectView, IEvent* _thisEvent);

    //Warning!
    //again it's bad putting this as public.
    //make eventattribute easier to build layer box.
    //should revise it later

//private:

    std::string byLayer;
    IEvent* thisIEvent;

    std::string getByLayer();

};



class SpectrumPartial {
public:
  SpectrumPartial* prev;
  SpectrumPartial* next;
  std::string envString;

  SpectrumPartial(){
    prev = NULL;
    next = NULL;
    envString = "";

  }
  ~SpectrumPartial(){};

};


/*! \brief The class representing each event file in CMOD
 *
 ******************************************************************************/
class IEvent {

public:

  std::list<EventLayer*> layers;
  //although is a really bad practice putting field here, but for easy
  // accessing all layers in the event, i keep it this way for now


  /*! \brief Constructor of IEvent Object
   *
   ****************************************************************************/
  IEvent();

  //when open project

  IEvent(IEvent* _original, string _newName);
  IEvent(DOMElement* _domElement);

  ~IEvent();

  /*! \brief set the name of the event
   *
   *  @param _eventName name of the event
   ****************************************************************************/
  void setEventName(std::string _eventName);

  /*! \brief get the name of the event
   *
   *  @return name of the event
   ****************************************************************************/
  std::string getEventName();

  /*! \brief set the type of the event
   *
   *  @param _type type of the event
   ****************************************************************************/
  void setEventType(EventType _type);

  /*! \brief get the type of the event
   *
   *  @return type of the event
   ****************************************************************************/
  EventType getEventType();

  /*! \brief get the string representing the type of the event
   *
   *  @return string of the event type
   ****************************************************************************/
  std::string getEventTypeString();
  std::string getEventFolderName();

  /*! \brief set maxChildDur of the event
   *
   *  @param _maxChildDur maxChildDur
   ****************************************************************************/
  void setMaxChildDur(std::string _maxChildDur);

  /*! \brief get maxChildDur of the event
   *
   *  @return maxChildDur
   ****************************************************************************/
  std::string getMaxChildDur();

  /*! \brief set unitsPerSecond of the event
   *
   *  @param _unitsPerSecond unitsPerSecond
   ****************************************************************************/
  void setUnitsPerSecond(std::string _unitsPerSecond);


  /*! \brief get unitsPerSecond of the event
   *
   *  @return unitsPerSecond
   ****************************************************************************/
  std::string getUnitsPerSecond();



  /*! \brief get Spa of the event
   *
   *  @return Spa
   ****************************************************************************/
  std::string getSpa();



  /*! \brief set Spa of the event
   *
   *  @param _spa
   ****************************************************************************/
  void setSpa(std::string _spa);






  /*! \brief get Rev of the event
   *
   *  @return Rev
   ****************************************************************************/
  std::string getRev();

  /*! \brief set Rev of the event
   *
   *  @param _rev
   ****************************************************************************/
  void setRev(std::string _rev);



    /*! \brief get Fil of the event
   *
   *  @return Fil
   ****************************************************************************/
  std::string getFil();

  /*! \brief set Fil of the event
   *
   *  @param _fil
   ****************************************************************************/
  void setFil(std::string _fil);


  //setting and getting measurement of the event
  std::string getMea();

  void setMea(std::string _mea);
  //after open project and constructing all the event, has to link them properly
  void link(ProjectViewController* _projectView);



  //time signature
  std::string getTimeSignatureEntry1();
  void setTimeSignatureEntry1(std::string _timeSignatureEntry1);
  std::string getTimeSignatureEntry2();
  void setTimeSignatureEntry2(std::string _timeSignatureEntry2);



  //tempo
  int getTempoMethodFlag(); //0 = as note value, 1 = as fraction
  void setTempoMethodFlag(int _flag);
  TempoPrefix getTempoPrefix();
  void setTempoPrefix(TempoPrefix _prefix);
  TempoNoteValue getTempoNoteValue();
  void setTempoNoteValue(TempoNoteValue _noteValue);

  std::string getTempoFractionEntry1();
  void setTempoFractionEntry1(std::string _entry1);
  std::string getTempoFractionEntry2();
  void setTempoFractionEntry2(std::string _entry2);
  std::string getTempoValueEntry();
  void setTempoValueEntry(std::string _entry);




  void setChangedButNotSaved(bool value);
  void addParent(IEvent* _parent);
  bool removeParent(IEvent* _event);
  void deleteLayer (EventLayer* _deleteLayer);



  bool haveString(string _string);


  void makeSuperColliderCode();
  string getXMLString();
  string getXMLTHMLB();
  string getXMLSound();
  string getXMLEnv();
  string getXMLSiv();
  string getXMLSpa();
  string getXMLPat();
  string getXMLRev();
  string getXMLFil();
  string getXMLNote();
  string getXMLMea();
  static string getFunctionString(DOMElement* _thisFunctionElement);




  /*! \brief set flagNumChildren of the event
   *
   *  @param _flag The flag indicating how to specify numChildren.
   *               Fixed = 0, Density = 1, By Layer =2
   ****************************************************************************/
  void setFlagNumChildren(int _flag);

  /*! \brief get flagNumChildren of the event
   *
   *  @return The flag indicating how to specify numChildren.
   *          0 = Fixed, 1 = Density, 2 = By Layer
   ****************************************************************************/
  int getFlagNumChildren();

  /*! \brief set numChildrenEntry1 of the event
   *
   *  @param _entry1 The first entry of numChildren
   ****************************************************************************/
  void setNumChildrenEntry1(std::string _entry1);

  /*! \brief get numChildrenEntry1 of the event
   *
   *  @return numChildrenEntry1
   ****************************************************************************/
  std::string getNumChildrenEntry1();

  /*! \brief set numChildrenEntry2 of the event
   *
   *  @param _entry2 The second entry of numChildren
   ****************************************************************************/
  void setNumChildrenEntry2(std::string _entry2);

  /*! \brief get numChildrenEntry2 of the event
   *
   *  @return numChildrenEntry2
   ****************************************************************************/
  std::string getNumChildrenEntry2();

  /*! \brief set numChildrenEntry3 of the event
   *
   *  @param _entry3 The third entry of numChildren
   ****************************************************************************/
  void setNumChildrenEntry3(std::string _entry3);

  /*! \brief get numChildrenEntry3 of the event
   *
   *  @return numChildrenEntry3
   ****************************************************************************/
  std::string getNumChildrenEntry3();


  void setFlagChildEventDef(int _flag);
  int  getFlagChildEventDef();

  void setFlagChildEventDefStartType(int _flag);
  int  getFlagChildEventDefStartType();

  void setFlagChildEventDefDurationType(int _flag);
  int  getFlagChildEventDefDurationType();


  void        setChildEventDefEntry1(std::string _string);
  std::string getChildEventDefEntry1();

  void        setChildEventDefEntry2(std::string _string);
  std::string getChildEventDefEntry2();

  void        setChildEventDefEntry3(std::string _string);
  std::string getChildEventDefEntry3();


  void        setChildEventDefAttackSieve(std::string _string);
  std::string getChildEventDefAttackSieve();

  void        setChildEventDefDurationSieve(std::string _string);
  std::string getChildEventDefDurationSieve();

  //manipulate modifiers
  EventBottomModifier* getModifiers();
  EventBottomModifier* addModifier();
  void removeModifier( EventBottomModifier* _modifier);

  void setEventOrderInPalette(int _number);
  int getEventOrderInPalette();


  /*! \brief add an object to the event as a child
   *
   *  @param _newChild the object to be added to the event
   ****************************************************************************/
  void addChildOrObject(IEvent* _newChild);


  /*! \brief remove an object from the event
   *
   *  @param _childToBeRemoved the object to be removed from the event
   ****************************************************************************/
  void removeChildOrObject(IEvent* _childToBeRemoved);


  /*! \brief set the field "modifiedButNotSaved" to true
   *
   ****************************************************************************/
  void setModifiedButNotSaved();


  /*! \brief save the object onto the disk
   *
   *  @param _pathOfProject the string of the path so the file can be
   *   written according to the path
   ****************************************************************************/
  void saveToDisk(std::string _pathOfProject);
  void saveAsToDisk(std::string _pathOfProject); //save everything
  void saveToDiskHelper(std::string _pathOfProject, bool _forced);

  int getNumberOfLayers();


  void showAllChildren();




  /*! \brief the class holds extra infomation for special objects
   *(objects other than Top to Low
   ****************************************************************************/
  class EventExtraInfo{

  public:
    //implemented in SoundExtraInfo
    virtual void setNumPartials(std::string _numPartials){}
    virtual std::string getNumPartials(){return "";}
    virtual bool haveString(string _string){return false;}

    virtual void setDeviation(std::string _deviation){}
    virtual std::string getDeviation(){return "";}
    virtual std::string getSpectrumMetaData(){ return "";}
    virtual std::string getSpectrumXMLString(){ return "";}
    virtual std::string getSoundSpectrumEnvelopesString(){return "";}

    virtual SpectrumPartial* getSpectrumPartials(){return NULL;}
    virtual SpectrumPartial* addPartial(){}
    virtual bool deletePartial(SpectrumPartial* _partial){}


    // Bottom extra info
    virtual int  getFrequencyFlag(){return -1;}
    virtual void  setFrequencyFlag( int _flag){}
    virtual int  getFrequencyContinuumFlag(){return -1;}
    virtual void  setFrequencyContinuumFlag(int _flag){}
    virtual std::string  getFrequencyEntry1(){return "";}
    virtual void  setFrequencyEntry1(std::string _string){}
    virtual std::string  getFrequencyEntry2(){return "";}
    virtual void  setFrequencyEntry2(std::string _string){}
    virtual std::string  getLoudness(){return "";}
    virtual void  setLoudness(std::string _string){}
    virtual std::string  getSpatialization(){return "";}
    virtual void  setSpatialization(std::string _string){}
    virtual std::string  getReverb(){return "";}
    virtual void  setReverb(std::string _string){}
    virtual std::string  getFilter(){return "";}
    virtual void  setFilter(std::string _string){}
    virtual EventBottomModifier*  getModifiers(){return NULL;}
    virtual EventBottomModifier* addModifier(){return NULL;}
    virtual void  removeModifier(EventBottomModifier* _modifier){}
    virtual std::string getEnvelopeBuilder(){return "";}
    virtual void setEnvelopeBuilder(std::string _string){}
    virtual std::string getPatternBuilder(){return "";}
    virtual void setPatternBuilder(std::string _string){}
    virtual std::string getSieveBuilder(){return "";}
    virtual void setSieveBuilder(std::string _string){}
    virtual std::string getReverbBuilder(){return "";}
    virtual void setReverbBuilder(std::string _string){}
    virtual std::string getFilterBuilder(){return "";}
    virtual void setFilterBuilder(std::string _string){}
    virtual std::string getSpatializationBuilder(){return "";}
    virtual void setSpatializationBuilder(std::string _string){}
    virtual void setChildTypeFlag(int _type){};
    virtual int getChildTypeFlag(){ return -1;}
    virtual std::string getMeasurement(){return"";}
    virtual void setMeasurement(std::string _string){}
    virtual std::string getMeasureBuilder(){return "";}
    virtual void setMeasureBuilder(std::string _string){}
    //note extra info
    virtual std::list<std::string> getNoteModifiers()
      {std::list<std::string> dummy;return dummy;}
    virtual void clearNoteModifiers(){}
    virtual void addNoteModifiers(std::string _modifier){}




    // EnvelopeExtraInfo
  };


  EventExtraInfo* getEventExtraInfo();

  /*! \brief the class holds extra information carried
   *
   ****************************************************************************/
     // BottomEvent
  class BottomEventExtraInfo : public EventExtraInfo {
  public:
    BottomEventExtraInfo();
    BottomEventExtraInfo(BottomEventExtraInfo* _original);
    BottomEventExtraInfo(int _childTypeFlag); //called when parsing files.
    BottomEventExtraInfo(int _childTypeFlag, DOMElement* _thisElement);
    ~BottomEventExtraInfo();
    int getFrequencyFlag(); // 0 = Well_tempered, 1 = Fundamental, 2 = Continuum
    void setFrequencyFlag(int _flag);
    int getFrequencyContinuumFlag();// 0 = hertz, 1 = power of two
    void setFrequencyContinuumFlag(int _flag);

    std::string  getFrequencyEntry1();
    void  setFrequencyEntry1(std::string _string);
    std::string  getFrequencyEntry2();
    void  setFrequencyEntry2(std::string _string);
    std::string  getLoudness();
    void  setLoudness(std::string _string);
    std::string  getSpatialization();
    void  setSpatialization(std::string _string);
    std::string  getReverb();
    void  setReverb(std::string _string);
    std::string  getFilter();
    void  setFilter(std::string _string);
    EventBottomModifier* getModifiers();
    EventBottomModifier* addModifier();
    void removeModifier(EventBottomModifier* _modifier);
    void setChildTypeFlag(int _type);
    int getChildTypeFlag();

    EventBottomModifier* modifiers;
    bool haveString(string _string);
    static EventBottomModifier* buildModifiersFromDOMElement(DOMElement* _thisModifierElement);

  private:
    int frequencyFlag; // 0 = Well_tempered, 1 = Fundamental, 2 = Continuum
    int frequencyContinuumFlag; //0 = hertz, 1 =] power of two
    int childTypeFlag; // 0 = sound, 1 = note, 2 = visual
    std::string frequencyEntry1;
    std::string frequencyEntry2;
    std::string loudness;
    std::string spatialization;
    std::string reverb;
    std::string filter;



  };

     // Envelope
  class EnvelopeExtraInfo : public EventExtraInfo {
  public:
  	EnvelopeExtraInfo(EnvelopeExtraInfo* _original){
  	  envelopeBuilder = _original->envelopeBuilder;}
    EnvelopeExtraInfo(){envelopeBuilder = "";}
    ~EnvelopeExtraInfo(){}
    std::string getEnvelopeBuilder();
    void setEnvelopeBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string envelopeBuilder;

  };

     // Pattern
  class PatternExtraInfo : public EventExtraInfo {
  public:
  	PatternExtraInfo(PatternExtraInfo* _original){
  	  patternBuilder = _original->patternBuilder;}
    PatternExtraInfo(){patternBuilder = "";}
    ~PatternExtraInfo(){}
    std::string getPatternBuilder();
    void setPatternBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string patternBuilder;

  };

     // Spatialization
  class SpatializationExtraInfo : public EventExtraInfo {
  public:
  	SpatializationExtraInfo(SpatializationExtraInfo* _original){
  	  spatializationBuilder = _original->spatializationBuilder;}
    SpatializationExtraInfo(){spatializationBuilder = "";}
    ~SpatializationExtraInfo(){}
    std::string getSpatializationBuilder();
    void setSpatializationBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string spatializationBuilder;
  };

     // Sieve
  class SieveExtraInfo : public EventExtraInfo {
  public:
  	SieveExtraInfo(SieveExtraInfo* _original){
  	  sieveBuilder = _original->sieveBuilder;}
    SieveExtraInfo(){sieveBuilder = "";}
    ~SieveExtraInfo(){}
    std::string getSieveBuilder();
    void setSieveBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string sieveBuilder;
  };

    // Reverb
  class ReverbExtraInfo : public EventExtraInfo {
  public:
  	ReverbExtraInfo(ReverbExtraInfo* _original);
    ReverbExtraInfo(){reverbBuilder = "";}
    ~ReverbExtraInfo(){}
    std::string getReverbBuilder();
    void setReverbBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string reverbBuilder;
  };


    // Filter
  class FilterExtraInfo : public EventExtraInfo {
  public:
  	FilterExtraInfo(FilterExtraInfo* _original);
    FilterExtraInfo(){filterBuilder = "";}
    ~FilterExtraInfo(){}
    std::string getFilterBuilder();
    void setFilterBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string filterBuilder;
  };

    // Measurement
  class MeasureExtraInfo : public EventExtraInfo {
  public:
    MeasureExtraInfo(MeasureExtraInfo* _original);
    MeasureExtraInfo(){measureBuilder = "";}
    ~MeasureExtraInfo(){}
    std::string getMeasureBuilder();
    void setMeasureBuilder(std::string _string);
    bool haveString(string _string);
  private:
    std::string measureBuilder;
  };

     // Sound
  class SoundExtraInfo: public EventExtraInfo {

  public:
    SoundExtraInfo();
    SoundExtraInfo(SoundExtraInfo* _original);
    ~SoundExtraInfo();

    //void        setNumPartials(std::string _numPartials);
    std::string getNumPartials();

    void        setDeviation(std::string _deviation);
    std::string getDeviation();
    std::string getSoundSpectrumEnvelopesString();

    //void        setSpectrum(std::string _spectrum);
    std::string getSpectrumMetaData();
    std::string getSpectrumXMLString();
    SpectrumPartial* getSpectrumPartials();
    SpectrumPartial* addPartial();
    bool deletePartial(SpectrumPartial* _partial);
    bool haveString(string _string);
  private:

    int numPartials;
    std::string deviation;
    SpectrumPartial* spectrumPartials;
  };

  class NoteExtraInfo: public EventExtraInfo {

  public:
    NoteExtraInfo();
    NoteExtraInfo(NoteExtraInfo* _original);
    ~NoteExtraInfo();
    std::list<std::string> getNoteModifiers();
    void addNoteModifiers(std::string _modifier);
    void clearNoteModifiers();
    bool haveString(string _string);

  private:
    std::list<std::string> modifiers;
  };












  EventLayer* addLayer();
private:
  EventType eventType;
  std::string oldEventName;
  int eventOrderInPalette;
  std::string eventName;

  std::string maxChildDur;

  std::string unitsPerSecond; //is the old EDUPerBeat.
  std::string timeSignatureEntry1;
  std::string timeSignatureEntry2;

  int tempoMethodFlag; //0 = as note value, 1 = as fraction
  TempoPrefix tempoPrefix;
  TempoNoteValue tempoNoteValue;
  std::string tempoFractionEntry1;
  std::string tempoFractionEntry2;
  std::string tempoValueEntry;



  std::string numChildrenEntry1;
  std::string numChildrenEntry2;
  std::string numChildrenEntry3;

  std::string childEventDefEntry1;
  std::string childEventDefEntry2;
  std::string childEventDefEntry3;
  std::string childEventDefAttackSieve;
  std::string childEventDefDurationSieve;

  std::string filter;
  std::string reverb;
  std::string spatialization;
  std::string measurement;
  EventBottomModifier* modifiers;

   // 0 = fixed, 1 = density, 2 = By layer
  int flagNumChildren;

   // 0 = continuum, 1 = sweep, 2 = discrete
  int flagChildEventDef;

   // 0 = percentage, 1 = units, 2 = seconds
  int flagChildEventDefStartType;

   // 0 = percentage, 1 = units, 2 = seconds
  int flagChildEventDefDurationType;





  //void IEventParseFile(std::string _fileName);
  void parseNonEvent();
  void buildNonEventFromDOM(DOMElement* _element);


  //EventExtraInfo* openExtraInfo(EventFactory* _event ,EventType _eventType);


  bool changedButNotSaved;

  std::list<IEvent*> sounds;
  std::list<IEvent*> envelopes;
  std::list<IEvent*> reverbs;
  std::list<IEvent*> spatializations;
  std::list<IEvent*> patterns;
  std::list<IEvent*> sieves;
  std::list<IEvent*> parents;

  EventExtraInfo* extraInfo;
};

#endif //IEVENT_H
