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
//   Bottom.cpp
//
//----------------------------------------------------------------------------//

#include "Bottom.h"
#include "Random.h"
#include "Output.h"
static int test=0;

//----------------------------------------------------------------------------//

Bottom::Bottom(DOMElement* _element,
               TimeSpan _timeSpan,
               int _type,
               Tempo _tempo,
               Utilities* _utilities,
               DOMElement* _ancestorSpa,
               DOMElement* _ancestorRev,
               DOMElement* _ancestorFil,
               DOMElement* _ancestorModifiers):
  Event(_element, _timeSpan,_type, _tempo, _utilities, NULL,NULL,NULL,NULL),
  ancestorModifiersElement(_ancestorModifiers){


  XMLCh* extraInfoString = XMLString::transcode("ExtraInfo");
  DOMNodeList* extraInfoList = _element->getElementsByTagName(extraInfoString);
  DOMElement* extraInfo = (DOMElement*) extraInfoList->item(0);
  XMLString::release(&extraInfoString);

  /*
  <ExtraInfo>
    <FrequencyInfo>
      <FrequencyFlag>0</FrequencyFlag>
      <FrequencyContinuumFlag>0</FrequencyContinuumFlag>
      <FrequencyEntry1>3</FrequencyEntry1>
      <FrequencyEntry2/>
    </FrequencyInfo>
    <Loudness>4</Loudness>
    <Spatialization>5</Spatialization>
    <Reverb>6</Reverb>
    <Filter>f</Filter>
    <Modifiers>
    </Modifiers>
  </ExtraInfo>
  */

  frequencyElement = extraInfo->GFEC();
  loudnessElement = frequencyElement->GNES();
  if (_ancestorSpa != NULL){
    spatializationElement = _ancestorSpa;
  }
  else {
    spatializationElement = loudnessElement->GNES();
  }

  if (_ancestorRev != NULL){
    reverberationElement = _ancestorRev;
  }
  else {
    reverberationElement = loudnessElement->GNES()->GNES();
  }

  if (_ancestorFil != NULL){
    filterElement = _ancestorFil;
  }
  else {
    filterElement = loudnessElement->GNES()->GNES()->GNES();
  }


  modifiersElement = loudnessElement->GNES()->GNES()->GNES()->GNES();

}


//----------------------------------------------------------------------------//

Bottom::~Bottom() {
  //do nothing
}

//----------------------------------------------------------------------------//

void Bottom::buildChildren(){

  if (utilities->getOutputParticel()){
    //Begin this sub-level in the output and write out its properties.
    Output::beginSubLevel(name);
    outputProperties();
  }
  //Build the event's children.

  string method = XMLTC(methodFlagElement);

  //Set the number of possible restarts (for buildDiscrete)
  restartsRemaining = restartsNormallyAllowed;

  //Make sure that the temporary child events array is clear.
  if(temporaryChildEvents.size() > 0) {
    cerr << "WARNING: temporaryChildEvents should not contain data." << endl;
    cerr << "There may be a bug in the code. Please report." << endl;
    exit(1);
  }

  //Create the child events.
  for (currChildNum = 0; currChildNum < numChildren; currChildNum++) {
    if (method == "0") //continuum
      checkEvent(buildContinuum());
    else if (method == "1"){ //sweep

//      bool buildSweepSuccess = buildSweep();
//      cout<<endl;  // Ming-ching: I don't know why the program crash without this line... very odd..
//      checkEvent(buildSweepSuccess);

      //the three lines above is simply:
      checkEvent(buildSweep());

    }
    else if (method == "2") {  //discrete
      checkEvent(buildDiscrete());
    }
    else {
      cerr << "Unknown build method: " << method << endl << "Aborting." << endl;
      exit(1);
    }
  }

  //Using the temporary events that were created, construct the actual children.
  //The code below is different from buildchildren in Event class.
  for (int i = 0; i < childSoundsAndNotes.size(); i++) {
    SoundAndNoteWrapper* thisChild = childSoundsAndNotes[i];
    //Increment the static current child number.
    currChildNum = i;
    constructChild(thisChild);
    delete thisChild;
  }

/*
  //Clear the temporary event list.
  childSoundsAndNotes.clear();
*/

  if (utilities->getOutputParticel()){
  //End this output sublevel.
    Output::addProperty("Updated Tempo Start Time", tempo.getStartTime());
    Output::endSubLevel();
  }
}

//---------------------------------------------------------------------------//

void Bottom::modifyChildren(){            //Incomplete Override

  //Randomly modify elements

   //const short unsigned num = 15 ;
   //const short unsigned* loudness_val = &num;

   //cout<<computeLoudness();

   //loudnessElement->setNodeValue(loudness_val);

   //cout<<*loudnessElement->getNodeValue(); //Checking if update is successful

}

//---------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

void Bottom::constructChild(SoundAndNoteWrapper* _soundNoteWrapper) {
  //Just to get the checkpoint. Not used any other time.
  checkPoint = (_soundNoteWrapper->ts.start - ts.start) / ts.duration;
  cout << "CHECK POINT: " << checkPoint << endl;
  if (name.substr(0,1) == "s"){
    // buildNote(_soundNoteWrapper);
    buildSound(_soundNoteWrapper);
    return;
  }
  else if (name.substr(0,1) == "n"){
    buildNote(_soundNoteWrapper);
    return;
  }
  // else if (name.substr(0,2) == "ns" || name.substr(0,2) == "sn"){
  //   buildSound(_soundNoteWrapper);
  //   buildNote(_soundNoteWrapper);
  //   return;
  // }
}

//----------------------------------------------------------------------------//

void Bottom::buildSound(SoundAndNoteWrapper* _soundNoteWrapper) {
  //Create a new sound object.
  Sound* newSound = new Sound();

  if (utilities->getOutputParticel()){
    //Output sound related properties.
    Output::beginSubLevel("Sound");
    Output::addProperty("Name", _soundNoteWrapper->name);
    Output::addProperty("Type", _soundNoteWrapper->type);
    Output::addProperty("Start Time", _soundNoteWrapper->ts.start, "sec.");
      Output::addProperty("End Time", _soundNoteWrapper-> ts.start +
        _soundNoteWrapper->ts.duration, "sec.");
      Output::addProperty("Duration",_soundNoteWrapper-> ts.duration, "sec.");
  }

  //Set the start time and duration from the timespan.
  newSound->setParam(START_TIME, _soundNoteWrapper->ts.start);
  newSound->setParam(DURATION, _soundNoteWrapper->ts.duration);

  //Set the frequency.
  float baseFrequency = computeBaseFreq();
  if (utilities->getOutputParticel())Output::addProperty("Base Frequency", baseFrequency, "Hz");

  //Set the loudness.
  float loudSones = computeLoudness();
  newSound->setParam(LOUDNESS, loudSones);
  if (utilities->getOutputParticel())Output::addProperty("Loudness", loudSones, "sones");

  int numPartials = computeNumPartials( baseFrequency ,_soundNoteWrapper->element );

 //Element for GenSpectrum
  DOMElement* specElement = _soundNoteWrapper->element->GFEC()
    ->GNES()
    ->GNES()
    ->GNES()
    ->GNES();

  //if there is no spec element,go to partial routine;else, go to gen_spectrum
  if (specElement->GFEC() != NULL){
    DOMElement* partialEnvElement = (DOMElement*)utilities->evaluateObject(XMLTC(specElement), (void*)this,eventSpec);
    DOMElement* distanceElement = partialEnvElement->GNES();

    Envelope* waveShape = (Envelope*) utilities->evaluateObject(XMLTC(partialEnvElement),(void*)this, eventEnv );
    float distance = utilities->evaluate(XMLTC(distanceElement),(void*)this);

    //instead of reading partials, generate spectrum envelope and add to sound
    generatePartials(newSound, baseFrequency, loudSones, distance, waveShape);
    delete waveShape;
  }
  else{
      //Set the number of partials.
      if (utilities->getOutputParticel())Output::beginSubLevel("Partials");
      if (utilities->getOutputParticel())
    	Output::addProperty("Deviation",
    		computeDeviation(_soundNoteWrapper->element), "normalized");

      for (int i = 0; i < numPartials; i++) {
        currPartialNum = i; //added by ming 20130425

        //Create the next partial object.
        Partial partial;

        //Set the partial number of the partial based on the current index.
        partial.setParam(PARTIAL_NUM, i);

        //Compute the deviation for partials above the fundamental.
        double deviation = 0;

        if(i != 0)
          deviation = computeDeviation(_soundNoteWrapper->element );

        //Set the frequencies for each partial.
        float actualFrequency = setPartialFreq(
          partial, deviation, baseFrequency, i);

        //Report the actual frequency.
        stringstream ss; if(i != 0) ss << "Partial " << i; else ss << "Fundamental";
        if (utilities->getOutputParticel())
    	Output::addProperty(ss.str(), actualFrequency, "Hz");
        //Set the spectrum for this partial.
        setPartialSpectrum(partial, i, _soundNoteWrapper->element);

        //Add the partial to the sound.
        newSound->add(partial);

      }
   }

  if (utilities->getOutputParticel())Output::endSubLevel();

  //Apply the modifiers to the sound.
  applyModifiers(newSound, numPartials);

  //Apply the spatialization to the sound.
  applySpatialization(newSound, numPartials);

  //apply the filter to the sound
  applyFilter(newSound);

  //Apply the reverberation to the sound.
  applyReverberation(newSound);

  if (utilities->getOutputParticel())Output::endSubLevel();

  // at this point the ownership of the sound is given to the utilities.
  // utiliise will then transfer it to the Score object.
  utilities->addSound(newSound);

}

//----------------------------------------------------------------------------//

void Bottom::buildNote(SoundAndNoteWrapper* _soundNoteWrapper) {
  //Create the note.
//Note* newNote = new Note();		//sever, make similar to newSound
  Note* newNote = new Note(tsChild, tempo);
  if (utilities->getOutputParticel()){
  //Output note-related properties.
    Output::beginSubLevel("Note");
    Output::addProperty("Name", _soundNoteWrapper->name);
    Output::addProperty("Type", _soundNoteWrapper->type);
    Output::addProperty("Start Time", _soundNoteWrapper->ts.start, "sec.");
    Output::addProperty("End Time", _soundNoteWrapper-> ts.start +
	                      _soundNoteWrapper->ts.duration, "sec.");
    Output::addProperty("Duration",_soundNoteWrapper-> ts.duration, "sec.");
    // cout << "Bottom::buildNote: " << _soundNoteWrapper-> ts.duration << endl;
    Output::addProperty("Tempo Start Time",
	                      _soundNoteWrapper->tempo.getStartTime(), "sec.");
    Output::addProperty("EDU Start Time",
	                      _soundNoteWrapper->ts.startEDU.toPrettyString(), "EDU");
    Output::addProperty("EDU Start Time Absolute", 
                        _soundNoteWrapper->ts.startEDUAbsolute, "EDU");
    Output::addProperty("EDU Duration",
	                      _soundNoteWrapper->ts.durationEDU.toPrettyString(), "EDU");
  }

//set loudness
  float loudfloat = computeLoudness();
  newNote->setLoudnessSones(loudfloat);

//set modifiers
  vector<string> noteMods = applyNoteModifiers(_soundNoteWrapper->element);
    newNote->setModifiers(noteMods);

  //Set the pitch.
  float baseFrequency = computeBaseFreq();
  //cout << "base frequency" << baseFrequency << endl;

  int absPitchNum;

  if(wellTempPitch <= 0) { 		//if frequency is in Hertz
    absPitchNum = newNote->HertzToPitch(baseFrequency);
  } else {
    absPitchNum = wellTempPitch;
  }
  newNote->setPitchWellTempered(absPitchNum);

  // Set notation start, start absolute, and end times in edus
  // NOTE - these parameters are currently overridden in notateDurations(), however it will make sense in the future to set them here
  newNote->start_t = _soundNoteWrapper->ts.startEDUAbsolute;
  newNote->start_t_absolute = _soundNoteWrapper->ts.startEDUAbsolute;
  newNote->end_t = _soundNoteWrapper->ts.startEDUAbsolute + stoi(_soundNoteWrapper->ts.durationEDU.toPrettyString());

  //Bars and durations
  newNote->notateDurations( (string)_soundNoteWrapper->name,
 			    _soundNoteWrapper->ts.startEDU.toPrettyString(),
			    _soundNoteWrapper->ts.durationEDU.toPrettyString()); // TODO - this is where we need fixes
  if (utilities->getOutputParticel()){
      Output::endSubLevel();
  }
  // cout << "note's data: " << newNote -> pitch_out << " start: " << newNote -> start_t << " end: " << newNote -> end_t << endl;

  childNotes.push_back(newNote);
}


//----------------------------------------------------------------------------//

list<Note> Bottom::getNotes() { // FIXME - this can be removed
  list<Note> result;
  for(int i = 0; i < childNotes.size(); i++)
    result.push_back(*childNotes[i]);
  return result;
}

//----------------------------------------------------------------------------//

float Bottom::computeBaseFreq() {

  float baseFreqResult;
  DOMElement* freqFlagElement = frequencyElement->GFEC();
  DOMElement* continuumFlagElement = freqFlagElement->GNES();
  DOMElement* valueElement = continuumFlagElement->GNES();
  DOMElement* valueElement2 = valueElement->GNES();
  if (utilities->evaluate(XMLTC(freqFlagElement),(void*) this)==2) {//contiruum
    /* 2nd arg is a string (HERTZ or POW2) */

    if (utilities->evaluate(XMLTC(continuumFlagElement), NULL)==0) { //Hertz
      float expVal = 0;
      for(int i = 0; i < 10; i++){
        expVal += utilities->evaluate(XMLTC(valueElement), (void*)this);
      }
      expVal /= 10;
      baseFreqResult = utilities->evaluate(XMLTC(valueElement), (void*)this);
      float diff = baseFreqResult - expVal;
      baseFreqResult -= 0.4 * diff;
      /* 3rd arg is a float (baseFreq in Hz) */
    }
    else  {//power of 2
      /* 3rd arg is a float (power of 2) */
      float expVal = 0;
      for(int i = 0; i < 10; i++){
        float step = utilities->evaluate(XMLTC(valueElement), (void*)this);
        float range = log10(CEILING / MINFREQ) / log10(2.); // change log base
        expVal += pow(2, step * range) * MINFREQ;  // equal chance for all 8vs
      }
      expVal /= 10;
      float step = utilities->evaluate(XMLTC(valueElement), (void*)this);
      float range = log10(CEILING / MINFREQ) / log10(2.); // change log base
      baseFreqResult = pow(2, step * range) * MINFREQ;  // equal chance for all 8vs

      float diff = baseFreqResult - expVal;
      baseFreqResult -= 0.4 * diff;
    }

  } else if (utilities->evaluate(XMLTC(freqFlagElement), (void*) this)==0) { //equal tempered
    /* 2nd arg is an int */
    float expVal = 0;
    for(int i = 0; i < 10; i++){
      wellTempPitch = utilities->evaluate(XMLTC(valueElement), (void*)this);
//cout << " Bottom - wellTempPitch=" << wellTempPitch << endl;
      expVal += C0 * pow(WELL_TEMP_INCR, wellTempPitch);
    }

    expVal /= 10;
    wellTempPitch = utilities->evaluate(XMLTC(valueElement), (void*)this);
    // cout << " Bottom - wellTempPitch=" << wellTempPitch << endl;
    baseFreqResult = C0 * pow(WELL_TEMP_INCR, wellTempPitch);

    float diff = baseFreqResult - expVal;
    baseFreqResult -= 0.4 * diff;

  } else  {// fundamental
    /* 2nd arg is (float)fundamental_freq, 3rd arg is (int)overtone_num */
    // float expVal = 0;
    // for(int i = 0; i < 10; i++){
    //   float fund_freq = utilities->evaluate(XMLTC(valueElement), (void*)this);
    //   int overtone_step = utilities->evaluate(XMLTC(valueElement2), (void*)this);
    //   expVal += fund_freq * overtone_step;
    // }
    // expVal /= 10;
    float fund_freq = utilities->evaluate(XMLTC(valueElement), (void*)this);
    int overtone_step = utilities->evaluate(XMLTC(valueElement2), (void*)this);
    baseFreqResult = fund_freq * overtone_step;

    // float diff = baseFreqResult - expVal;
    // baseFreqResult -= 0.4 * diff;
  }
  return baseFreqResult;
}

//----------------------------------------------------------------------------//

float Bottom::computeLoudness() {
  float expVal = 0;
  for(int i = 0; i < 10; i++){
      expVal += utilities->evaluate(XMLTC(loudnessElement), (void*)this);
  }
  // expVal /= 10;
  float loudval = utilities->evaluate(XMLTC(loudnessElement), (void*)this);
  // float diff = loudval - expVal;
  // loudval -= 0.4 * diff;
  // cout << "bottom loudness: " << loudval << endl;
  // cout << "bottom expval: " << expVal << endl;
  return loudval;
}

//----------------------------------------------------------------------------//

int Bottom::computeNumPartials(float baseFreq, DOMElement* _spectrum) {

  DOMElement* numPartialElement = _spectrum->GFEC()
    ->GNES()
    ->GNES();
  int numPartsResult = utilities->evaluate(XMLTC(numPartialElement), (void*) this);

  // Decrease numPartials until p < CEILING
  // (CEILING is a global def from define.h)
  while(numPartsResult * baseFreq > CEILING) {
    numPartsResult--;
  }

  if(numPartsResult <= 0) {
    cerr << "Error: Bottom::computeNumPartials got 0, baseFrequency="
         << baseFreq << endl;
    exit(1);
  }

  return numPartsResult;
}

//----------------------------------------------------------------------------//

float Bottom::computeDeviation( DOMElement* _spectrum) {
  DOMElement* devElement = _spectrum->GFEC()
    ->GNES()
    ->GNES()
    ->GNES();
  return utilities->evaluate(XMLTC(devElement), (void*)this);
}

//----------------------------------------------------------------------------//

float Bottom::setPartialFreq(Partial& part, float deviation, float baseFreq, int partNum) {

  // assign frequency to each partial
  float pDev = deviation * (Random::Rand() - 0.5) * 2;
  float pFreq = baseFreq * ((partNum + 1) + pDev);

  // if pFreq is out of range then set it to the closer of the max or min value
  if(pFreq < MINFREQ) {
    pFreq = MINFREQ;
  } else if(pFreq > CEILING) {
    pFreq = CEILING;
  }

  part.setParam(FREQUENCY, pFreq);
  return pFreq;
}

//----------------------------------------------------------------------------//

void Bottom::setPartialSpectrum(Partial& part, int partNum, DOMElement* _element) {

  DOMElement* partialEnvElement = _element->GFEC()
    ->GNES()
    ->GNES()
    ->GNES()
    ->GNES()
    ->GNES()
    ->GFEC();

    int counter = partNum;
    while(counter != 0){
      partialEnvElement=partialEnvElement->GNES();
      counter--;
    }
    Envelope* waveShape = (Envelope*) utilities->evaluateObject(XMLTC(partialEnvElement),(void*)this, eventEnv );
    part.setParam(WAVE_SHAPE, *waveShape );
    delete waveShape;
}

//----------------------------------------------------------------------------//

void Bottom::applySpatialization(Sound* s, int numPartials) {

  DOMElement* SPAElement = (DOMElement*) utilities->evaluateObject("", (void*) this, eventSpa); //this call will return a spa function, just in case users use "select" here. The string here is just a dummy since the callee will find the right spa element  within "this".


//  <Fun>
//    <Name>SPA</Name>
//    <Method>STEREO</Method>
//    <Apply>SOUND</Apply>
//    <Channels>  bla bla bla ... </Channels>
//  </Fun>
  DOMElement* methodElement = SPAElement->GFEC()->GNES();
  string method = XMLTC(methodElement);

  DOMElement* applyHowElement = methodElement->GNES();
  string apply = XMLTC(applyHowElement);

  DOMElement* channelsElement = applyHowElement->GNES();

  if (method.compare("STEREO")==0) {
    //will be a list of envs, of length 1 if applyhow == SOUND, or
    //  else if applyhow == PARTIAL, it will be numpartials in length
    spatializationStereo(s, channelsElement, apply, numPartials);

  } else if (method.compare("MULTI_PAN")==0) {
    //will be a list of lists of envs ... number of items in
    // outerlist = NumChannels, items in inner lists = NumPartials

    spatializationMultiPan(s, channelsElement, apply, numPartials);

  } else if (method.compare("POLAR")==0) {
    //will be 2 lists of envelopes, both NumPartials in length

    spatializationPolar(s, channelsElement, apply, numPartials);

  } else {
    cout << "spat_method = " << method << endl;
    cout << "SOUND_SPATIALIZATION has invalid method!  Use STEREO, MULTI_PAN, or POLAR" << endl;
    exit(1);
  }

}

//----------------------------------------------------------------------------//

void Bottom::spatializationStereo(Sound *s,
                                  DOMElement* _channels,
                                  string applyHow,
                                  int numParts) {
//  <Channels>
//    <Partials>
//      <P><Fun><Name>EnvLib</Name><Env>1</Env><Scale>1.0</Scale></Fun></P>
//    </Partials>
//  </Channels>

  DOMElement* envelopeElement = _channels->GFEC()->GFEC();
  Envelope* panning = (Envelope*) utilities->evaluateObject(XMLTC(envelopeElement),(void*)this, eventEnv );

  Pan stereoPan(*panning); // cavis +1
  s->setSpatializer(stereoPan);
  delete panning;

  if (applyHow == "PARTIAL") {
    //this isn't implemented in LASS yet, so we can't do it here
    cerr << "Sorry, applying spatialization by PARTIAL not supported yet" << endl;

  }
  else if (applyHow != "SOUND"){
    cerr << "Error: " << applyHow << " is an invalid way to apply spatialization! "
         << "Use SOUND or PARTIAL" << endl;

  }

}

//----------------------------------------------------------------------------//

void Bottom::spatializationMultiPan(Sound *s,
                                    DOMElement* _channels,
                                    string applyHow,
                                    int numParts) {


//  <Channels>
//    <Partials>
//      <P><Fun><Name>EnvLib</Name><Env>2</Env><Scale>1.0</Scale></Fun></P>
//    </Partials>
//    <Partials>
//      <P><Fun><Name>EnvLib</Name><Env>2</Env><Scale>1.0</Scale></Fun></P>
//    </Partials>
//    <Partials>
//      <P><Fun><Name>EnvLib</Name><Env>2</Env><Scale>1.0</Scale></Fun></P>
//    </Partials>
//  </Channels>

  vector<Envelope*> mult;
  Envelope* env;
  DOMElement* partials = _channels->GFEC();
  while (partials!=NULL){
    env = (Envelope*)utilities->evaluateObject(XMLTC(partials->GFEC()), (void*)this,eventEnv);
    mult.push_back(env);
    partials = partials->GNES();
  }

  MultiPan multipan(mult.size(), mult);
  s->setSpatializer(multipan);

  for (int i = 0; i < mult.size(); i ++){
    delete mult[i];
  }

  if (applyHow == "PARTIAL") {
    //this isn't implemented in LASS yet, so we can't do it here
    cerr << "Sorry, applying spatialization by PARTIAL not supported yet" << endl;

  }
  else if (applyHow != "SOUND"){
    cerr << "Error: " << applyHow << " is an invalid way to apply spatialization! "
         << "Use SOUND or PARTIAL" << endl;

  }
}

//----------------------------------------------------------------------------//

void Bottom::spatializationPolar(Sound *s,
                                 DOMElement* _channels,
                                 string applyHow,
                                 int numParts) {
//<Channels>
//  <Partials>         This element is actually the Theta
//    <P><Fun><Name>EnvLib</Name><Env>2</Env><Scale>1.0</Scale></Fun></P>
//  </Partials>
//  <Partials>         This element is the Radius
//    <P><Fun><Name>EnvLib</Name><Env>2</Env><Scale>1.0</Scale></Fun></P>
//  </Partials>
//</Channels>

  Envelope* thetaEnv;
  Envelope* radiusEnv;
  DOMElement* thetaElement = _channels->GFEC()->GFEC();
  DOMElement* radiusElement = _channels->GFEC()->GNES()->GFEC();

  thetaEnv = (Envelope*)utilities->evaluateObject(XMLTC(thetaElement), (void*)this,eventEnv);
  radiusEnv = (Envelope*)utilities->evaluateObject(XMLTC(radiusElement), (void*)this,eventEnv);

  MultiPan multipan(utilities->getNumberOfChannels());
  float time, theta, radius;

    // take 100 samples of the envelopes and apply them to the sound
    // (this should be enough to catch the important parts of the env)
    int numPolarSamples = 100;
    //cout << "TIME    THETA   RADIUS" << endl;
    for (int i = 0; i <= numPolarSamples; i++) {
      time = (float)i / numPolarSamples;
      theta = PI * thetaEnv->getScaledValueNew(time, 1.0);
      radius = radiusEnv->getScaledValueNew(time, 1.0);

      multipan.addEntryLocation(time, theta, radius);
      //if (i % 5 == 0) {
      //  cout << setw(6) << time << "  ";
      //  cout << setw(6) << theta << " ";
      //  cout << setw(6) << radius << "  " << endl;
      //}
    }

    multipan.doneAddEntryLocation();

    s->setSpatializer(multipan);
  delete thetaEnv;
  delete radiusEnv;

}

//----------------------------------------------------------------------------//

void Bottom::applyFilter(Sound* s){
  DOMElement* filterElement = (DOMElement*) utilities->evaluateObject("", (void*) this, eventFil);
  if (filterElement == NULL) return; //no filter

//  <Fun>
//    <Name>MakeFilter</Name>
//    <Type>HPF</Type>
//    <Frequency>2000</Frequency>
//    <BandWidth>4.5</BandWidth>
//    <dBGain/>
//  </Fun>
  DOMElement* it = filterElement->GFEC()->GNES();
  string type = XMLTC(it);
  it = it->GNES();
  double frequency = utilities->evaluate(XMLTC(it), (void*)this);
  it = it->GNES();
  double bandWidth = utilities->evaluate(XMLTC(it), (void*)this);
  it = it->GNES();
  double gain = utilities->evaluate(XMLTC(it), (void*)this);

  int typeInt;
  if (type =="LPF") typeInt = 0;
  else if (type == "HPF") typeInt =1;
  else if (type == "BPF") typeInt =2;
  else if (type == "NF") typeInt =3;
  else if (type == "PBEQF") typeInt =4;
  else if (type == "LSF") typeInt =5;
  else if (type == "HSF") typeInt =6;
  else {
    cout<<"Filter Type not recognized."<<endl;
    return;
  }

  /**
   *Usage:(From Mert Bay's BiQuadFilter.cpp)
  **/
  /**
   *	BiQuadFilter(int type, m_sample_type dbGain,  m_sample_type freq,  m_sample_type srate,   m_sample_type bandwidth);
   * Where arguments are
   * 1)Filter type: 0-6:
	*0-Low Pass Filter,
	*1-High Pass Filter
	*2-Band Pass Filter
	*3-Notch Filter
	*4-Peaking Band EQ filter
	*5-Low Shelf Filter
	*6-High Shelf Filter
   * 2) dbGain: Filters gain (dB) for peaking and shelving filters only
   * 3) Cutoff Frequency (hz)
   * 4) Sampling Rate (samples/sc)
   * 5) Bandwidth  (in octaves)
   *
  **/

  BiQuadFilter *filterObj= new BiQuadFilter(
        typeInt,
        gain,
        frequency,
        utilities->getSamplingRate(),
        bandWidth);

  s->use_filter(filterObj);
}


//----------------------------------------------------------------------------//

void Bottom::applyReverberation(Sound* s) {

  DOMElement* reverbElement =
          (DOMElement*) utilities->evaluateObject("", (void*) this, eventRev);

//this call will return a rev function, just in case users use "select" here.
//The string here is just a dummy since the callee will find the right rev
//element  within "this".

  string rev_method =  XMLTC(reverbElement->GFEC());

  if (rev_method.compare("REV_Simple") == 0) {

    float roomSize =
         utilities->evaluate(XMLTC(reverbElement->GFEC()->GNES()),(void*)this);
    Reverb* reverbObj = new Reverb(roomSize, SAMPLING_RATE);
    s->use_reverb(reverbObj);

  }
  else if (rev_method.compare("REV_Medium") == 0) {

//    <Fun>
//      <Name>REV_Medium</Name>
//      <Percent>
//        <Fun><Name>EnvLib</Name><Env>1</Env><Scale>1.0</Scale></Fun>
//      </Percent>
//      <Spread>  0.5</Spread>
//      <AllPass>0.5</AllPass>
//      <Delay>0.5</Delay>
//    </Fun>
    DOMElement* argument = reverbElement->GFEC()->GNES();

    //second input is percent reverb envelope
    Envelope* percent_rev =
	 (Envelope*) utilities->evaluateObject(XMLTC(argument), this, eventEnv);
    argument = argument->GNES();

    //3 floats:  hi/low spread, gain all pass, delay
    float hi_low_spread = utilities->evaluate(XMLTC(argument),this);
    argument = argument->GNES();
    float gain_all_pass = utilities->evaluate(XMLTC(argument),this);
    argument = argument->GNES();
    float delay = utilities->evaluate(XMLTC(argument),this);


    Reverb* reverbObj = new Reverb(percent_rev, hi_low_spread, gain_all_pass,
      delay, SAMPLING_RATE);
    s->use_reverb(reverbObj);
    delete percent_rev;

  }
  else if (rev_method.compare("REV_Advanced") == 0) {
//	<Fun>
//	  <Name>REV_Advanced</Name>
//	  <Percent>
//	    <Fun><Name>EnvLib</Name><Env>1</Env><Scale>1.0</Scale></Fun>
//	  </Percent>
//	  <CombGainList>0.46, 0.48, 0.50, 0.52, 0.53, 0.55</CombGainList>
//	  <LPGainList>0.05, 0.06, 0.07, 0.05, 0.04, 0.02</LPGainList>
//	  <AllPass></AllPass>
//	  <Delay></Delay>
//	</Fun>
    DOMElement* argument = reverbElement->GFEC()->GNES();

    //second input is percent reverb envelope
    Envelope* percent_rev =
         (Envelope*) utilities->evaluateObject(XMLTC(argument), this, eventEnv);
    argument = argument->GNES();

//    //list of EXACTLY 6 comb gain filters
    vector<std::string> stringListC =
			utilities->listElementToStringVector(argument);
    if (stringListC.size() != 6) {
      cerr << "Error: reverb comb gain list must contain 6 items!" << endl;
      exit(1);
    }
    vector<float> comb_gain_list;

    for (int i = 0; i < stringListC.size(); i ++){
      float num = (float) utilities->evaluate(stringListC[i], this);
      comb_gain_list.push_back(num);
    }

    argument = argument->GNES();

    //list of EXACTLY 6 lp gain filters
    vector<std::string> stringListG =
              		utilities->listElementToStringVector(argument);
    if (stringListG.size() != 6) {
      cerr << "Error: reverb lp gain list must contain 6 items!" << endl;
      exit(1);
    }
    vector<float> lp_gain_list;

    for (int i = 0; i < stringListG.size(); i ++){
      float num = (float) utilities ->evaluate(stringListG[i], this);
      lp_gain_list.push_back(num);
    }
    argument = argument->GNES();

    //2 floats:  gain all pass, delay
    float gain_all_pass = utilities->evaluate(XMLTC(argument),this);

    argument = argument->GNES();
    float delay = utilities->evaluate(XMLTC(argument),this);

    Reverb* reverbObj = new Reverb(percent_rev,
			           &comb_gain_list[0],
				   &lp_gain_list[0],
				   gain_all_pass, delay, SAMPLING_RATE);
    s->use_reverb(reverbObj);
  } else {
    cerr << "Invalid method/syntax in reverb!" << endl;
    cerr << "   filename=" << name << endl;
    exit(1);
 }

}

//-----------------------------------------------------------------------------/

void Bottom::applyModifiers(Sound *s, int numPartials) {
  vector<Modifier> modNoDep;  //mods without dependencies
  map<string, vector<Modifier> > modMutEx; // map mutex group names to the mods


  DOMElement* modifiersIncludingAncestorsElement = (DOMElement*) modifiersElement->cloneNode(true);


  if (ancestorModifiersElement != NULL){

      DOMElement* ancestorModifierIter = ancestorModifiersElement->GFEC();
      while(ancestorModifierIter !=NULL){
        DOMElement* cloneModifier = (DOMElement*) ancestorModifierIter->cloneNode(true);
        modifiersIncludingAncestorsElement->appendChild((DOMNode*)cloneModifier);
        ancestorModifierIter = ancestorModifierIter->GNES();
      }

  }// end handling ancestorModifiers

  //cout<<"Bottom-"<<name<<": Modifiers after merge:"<<XMLTC(modifiersIncludingAncestorsElement)<<endl<<endl<<"============="<<endl;



  DOMElement* modifierElement = modifiersIncludingAncestorsElement->GFEC();
  //cout<<"modifierElement: "<<XMLTC(modifierElement)<<endl;
  while (modifierElement!=NULL) {

//    <Modifier>
//      <Type>7</Type>
//      <ApplyHow>0</ApplyHow>
//      <Probability><Fun><Name>EnvLib</Name><Env>1</Env><Scale>1.0</Scale></Fun></Probability>
//      <Amplitude><Fun><Name>EnvLib</Name><Env>2</Env><Scale>1.0</Scale></Fun></Amplitude>
//      <Rate></Rate>
//      <Width></Width>
//      <GroupName></GroupName>
//    </Modifier>

    DOMElement* arg = modifierElement->GFEC();
    string modType;
    switch ((int)utilities->evaluate(XMLTC(arg), this)){
      case 0: modType = "TREMOLO"; break;
      case 1: modType = "VIBRATO"; break;
      case 2: modType = "GLISSANDO"; break;
      case 3: modType = "BEND"; break;
      case 4: modType = "DETUNE"; break;
      case 5: modType = "AMPTRANS"; break;
      case 6: modType = "FREQTRANS"; break;
      case 7: modType = "WAVE_TYPE"; break;
    }
    //cout<<"Mod Type: "<<modType<<endl;
    arg = arg->GNES();
    string applyHow = ((int)utilities->evaluate(XMLTC(arg), this)==0)?"SOUND":"PARTIAL";

    arg = arg->GNES();

    Envelope* probEnv = (Envelope*)utilities->evaluateObject(XMLTC(arg), this, eventEnv);
    DOMElement* ampElement = arg->GNES();
    DOMElement* rateElement = ampElement->GNES();
    DOMElement* widthElement = rateElement->GNES();

    string ampStr = XMLTC(ampElement);
    string rateStr = XMLTC(rateElement);
    string widthStr = XMLTC(widthElement);

    Modifier newMod(modType, probEnv, applyHow);

    if (applyHow == "SOUND") {

      if (ampStr!=""){
        Envelope* env =  (Envelope*)utilities->evaluateObject(ampStr, this, eventEnv );
        newMod.addValueEnv(env);
        delete env;
      }

      if (rateStr!=""){
        Envelope* env =  (Envelope*)utilities->evaluateObject(rateStr, this, eventEnv );
        newMod.addValueEnv(env);
        delete env;
      }

      if (widthStr!=""){
        Envelope* env =  (Envelope*)utilities->evaluateObject(widthStr, this, eventEnv );
        newMod.addValueEnv(env);
        delete env;
      }
    }
    else if (applyHow == "PARTIAL") {

      for (int i = 0; i <numPartials; i ++){ // make envelopes for all the partials

        if (ampStr!=""){
          Envelope* env =  (Envelope*)utilities->evaluateObject(ampStr, this, eventEnv );
          newMod.addValueEnv(env);
          delete env;
        }


        if (rateStr!=""){
          Envelope* env =  (Envelope*)utilities->evaluateObject(rateStr, this, eventEnv );
          newMod.addValueEnv(env);
          delete env;
        }


        if (widthStr!=""){
          Envelope* env =  (Envelope*)utilities->evaluateObject(widthStr, this, eventEnv );
          newMod.addValueEnv(env);
          delete env;
        }
      }
    }


    arg = widthElement->GNES();//group name (MUT_EX)
    string mutExGroup = XMLTC(arg);
    if (mutExGroup == "") {
      // not MUT_EX
      modNoDep.push_back(newMod);
    } else {
      // mutually exclusive
      modMutEx[mutExGroup].push_back(newMod);
    }

    delete probEnv;
    modifierElement = modifierElement->GNES(); // go to the next MOD in the list
  } // end of the main while loop


  // go through the non-exclusive mods
  for (int i = 0; i < modNoDep.size(); i++) {

    if (modNoDep[i].willOccur(checkPoint)) {

      modNoDep[i].applyModifier(s, numPartials);

    }
  }


  // go through the exclusive mods
  map<string, vector<Modifier> >::iterator iter = modMutEx.begin();
  while (iter != modMutEx.end()) {
    vector<Modifier> modGroup = (*iter).second;

    //go through the group, and apply 1 at most
    bool appliedMod = false;
    for (int i = 0; i < modGroup.size() && !appliedMod; i++) {
      if (modGroup[i].willOccur(checkPoint)) {
        modGroup[i].applyModifier(s, numPartials);
        appliedMod = true;
      }
    }
    iter ++;
  }
  //delete modifiersIncludingAncestorsElement;
}

//----------------------------------------------------------------------------//

vector<string> Bottom::applyNoteModifiersOld() {
  vector<string> modNames;

  vector<Modifier> modNoDep;  //mods without dependencies
  map<string, vector<Modifier> > modMutEx; // map mutex group names to the mods
  cout << "Bottom::applyNoteModifiers begin" << endl;


  DOMElement* modifiersIncludingAncestorsElement =
			(DOMElement*) modifiersElement->cloneNode(true);

  // ancestorModifiers
  if (ancestorModifiersElement != NULL){
      DOMElement* ancestorModifierIter = ancestorModifiersElement->GFEC();

      while(ancestorModifierIter !=NULL){
        DOMElement* cloneModifier =
			(DOMElement*) ancestorModifierIter->cloneNode(true);
        modifiersIncludingAncestorsElement->appendChild((DOMNode*)cloneModifier);
        ancestorModifierIter = ancestorModifierIter->GNES();
      }

  }	// end handling ancestorModifiers

  cout << "Bottom-"<<name<<": Modifiers after merge:"
       << XMLTC(modifiersIncludingAncestorsElement) << endl << endl
       << "=============" << endl;


/* needs to be rewrite to remove filevalue
  list<FileValue>* modList = modifiersFV->getListPtr(this);
  list<FileValue>::iterator modIter = modList->begin();
*/

  DOMElement* modifierElement = modifiersIncludingAncestorsElement->GFEC();
/*
  DOMElement* modifierElement = modifiersElement->GFEC();

    cout<<"modifierElement: "<<XMLTC(modifierElement)<<endl;
   int sever;  cin >> sever;
*/

  while (modifierElement != NULL) {

    DOMElement* arg = modifierElement->GFEC();
    string modType;
    switch ((int)utilities->evaluate(XMLTC(arg), this)){
      case 0: modType = "TREMOLO"; break;
      case 1: modType = "VIBRATO"; break;
      case 2: modType = "GLISSANDO"; break;
      case 3: modType = "BEND"; break;
      case 4: modType = "DETUNE"; break;
    }
    //cout<<"Mod Type: "<<modType<<endl;

    arg = arg->GNES();
    string applyHow =
           ((int)utilities->evaluate(XMLTC(arg), this) == 0)? "SOUND":"PARTIAL";

    arg = arg->GNES();
    Envelope* probEnv =
	   (Envelope*)utilities->evaluateObject(XMLTC(arg), this, eventEnv);

    DOMElement* ampElement = arg->GNES();
    DOMElement* rateElement = ampElement->GNES();

    string ampStr = XMLTC(ampElement);
    string rateStr = XMLTC(rateElement);
    cout << "Bottom::applyNoteModifiers - rateStr: " << rateStr << endl;

    Modifier newMod(modType, probEnv, applyHow);

/* needs to be rewrite to remove filevalue
  while (modIter != modList->end()) {
    // create the modifier and add it to the proper group
    list<FileValue>::iterator currMod = modIter->getListPtr(this)->begin();
    // 1st arg is string, 2nd is env
    string modType = currMod->getString(this);
    currMod++;
    Envelope* probEnv = currMod->getEnvelope(this);
    currMod++;

    Modifier newMod(modType, probEnv, "SOUND");
*/

/* needs to be rewrite to remove filevalue
    string mutExGroup = "";

    while (mutExGroup == "" && currMod != modIter->getListPtr(this)->end()) {
      FileValue mutExFV = currMod->getListPtr(this)->back();
      if (mutExFV.getReturnType() == FVAL_STRING) {
        mutExGroup = mutExFV.getString(this);
      } else {
        cerr << "Bottom::applyModifiers error: invalid syntax for MUT_EX group!" << endl;
        exit(1);
      }
*/

    arg = rateElement->GNES();//group name (MUT_EX)
    string mutExGroup = XMLTC(arg);

    if (applyHow == "SOUND") {

      if (ampStr!=""){
        Envelope* env =
		 (Envelope*)utilities->evaluateObject(ampStr, this, eventEnv );
        newMod.addValueEnv(env);
        delete env;
      }

      if (rateStr!=""){
        Envelope* env =
		 (Envelope*)utilities->evaluateObject(rateStr, this, eventEnv );
        newMod.addValueEnv(env);
        delete env;
      }
    }
    else if (applyHow == "PARTIAL") {
      cout << "ERROR: Note does not use PARTIAL" << endl;
    }

    if (mutExGroup == "") {
      // not MUT_EX
      modNoDep.push_back(newMod);
    } else {
      // mutually exclusive
      modMutEx[mutExGroup].push_back(newMod);
    }
/*    arg = widthElement->GNES();//group name (MUT_EX)
    string mutExGroup = XMLTC(arg);

    modIter++; // go to the next MOD in the list
  }
*/
    delete probEnv;
    modifierElement = modifierElement->GNES(); // go to the next MOD in the list
  } // end of the main while loop


  // go through the non-exclusive mods
  for (int i = 0; i < modNoDep.size(); i++) {
    if (modNoDep[i].willOccur(checkPoint)) {
      modNames.push_back( modNoDep[i].getModName() );
    }
  }

  // go through the exclusive mods
  map<string, vector<Modifier> >::iterator iter = modMutEx.begin();
  if (iter != modMutEx.end()) {
    vector<Modifier> modGroup = (*iter).second;

    //go through the group, and apply 1 at most
    bool appliedMod = false;
    for (int i = 0; i < modGroup.size() && !appliedMod; i++) {
      if (modGroup[i].willOccur(checkPoint)) {
        modNames.push_back( modGroup[i].getModName() );
        appliedMod = true;
      }
    }
    iter++;
  }

  //delete modifiersIncludingAncestorsElement;

  return modNames;
}


//----------------------------------------------------------------------------//

vector<string> Bottom::applyNoteModifiers( DOMElement* _playingMethods) {

  vector<string> modNames;

  DOMElement* techniqueElement = _playingMethods->GFEC()->GNES()->GNES();

//  string name0 = XMLTC(techniqueElement);
//  cout << "name0: " << name0 << endl;

  DOMElement* currentTechnique = techniqueElement->GFEC();

  do {
    string name = XMLTC(currentTechnique);
    modNames.push_back(name);
    currentTechnique = currentTechnique->GNES();
  } while ( currentTechnique != NULL);

  return modNames;
}

//----------------------------------------------------------------------------//

void Bottom::generatePartials(Sound* newsound, float frequency, float loudness, float distance, Envelope* waveShape){
  float strength = loudness*distance/256*2;   //strength is normalized between 0 and 2

  if ((frequency < 233) || frequency > 932){
    cout << "Error in genratePartials: frequency out of range" << endl;
  }
  //calculate scale for Partials
  double scaleTable[3][3][20] = {
    { //Bb3,p,m,f     233 hz
      {0.886, 0.014, 1, 0.072, 0.367, 0.193, 0.183, 0.036, 0.058, 0.010, 0.028,
        0.021, 0.011, 0.016, 0.003, 0.003, 0.002, 0.002, 0.002, 0.002},
      {0.756, 0.014, 1, 0.037, 0.521, 0.217, 0.234, 0.123, 0.306, 0.066, 0.142,
      0.040, 0.053, 0.044, 0.053, 0.039, 0.033, 0.043, 0.004, 0.012},
      {0.832, 0.019, 1.000, 0.043, 0.481, 0.226, 0.145, 0.119, 0.384, 0.091,
        0.182, 0.048, 0.080, 0.094, 0.077, 0.053, 0.044, 0.057, 0.009, 0.021}
      },
    {//Bb4,p,m,f     466 hz
      {1.000, 0.115, 0.834, 0.079, 0.233, 0.031, 0.027, 0.015, 0.005, 0.004,
      0.008, 0.002, 0.001, 0.0017, 0.001, 0.001, 0.001, 0.001, 0.0006, 0.001},
      {0.938, 0.072, 1.000, 0.318, 0.096, 0.096, 0.036, 0.109, 0.023, 0.010,
      0.007, 0.007, 0.002, 0.010, 0.002, 0.0015, 0.002, 0.002, 0.002, 0.0015},
      {0.939, 0.036, 1.000, 0.252, 0.220, 0.078, 0.057, 0.045, 0.027, 0.016,
      0.013, 0.007, 0.003, 0.008, 0.004, 0.001, 0.002, 0.001, 0.0022, 0.0018}
    },
    {//Bb5,p,m,f     932 hz
      {1.000, 0.269, 0.227, 0.040, 0.004, 0.006, 0.003, 0.0007, 0.002, 0.0007,
      0.0007, 0.003, 0.0007, 0.001, 0.0014, 0.0005, 0.0007, 0.0005, 0.0007, 0.0005},
      {1.00, 0.507, 0.132, 0.160, 0.030, 0.002, 0.001, 0.0008, 0.0016, 0.003, 0.002,
      0.001, 0.001, 0.001, 0.0005, 0.0005, 0.001, 0.0005, 0.001, 0.00064},
      {1.00, 0.354, 0.198, 0.102, 0.054, 0.012, 0.005, 0.002, 0.005, 0.003, 0.003,
      0.002, 0.001, 0.002, 0.004, 0.0015, 0.002, 0.002, 0.003, 0.0013}
    }
  };
  //Set the number of partials.
  int numPartials = 20;
  //For each partial, create and add to sound.
  for (int i = 0; i < numPartials; i++) {

    //Create the next partial object.
    Partial partial;

    //Set the partial number of the partial based on the current index.
    partial.setParam(PARTIAL_NUM, i);

    //Set the frequencies for each partial.
    float actualFrequency = setPartialFreq(
      partial, 0, frequency, i);

    //Report the actual frequency.
    stringstream ss; if(i != 0) ss << "Partial " << i; else ss << "Fundamental";
    if (utilities->getOutputParticel())
  Output::addProperty(ss.str(), actualFrequency, "Hz");
    //Set the spectrum for this partial.
    //interpolate the scale from scale table
    double scale;
    if (frequency <= 466){
      if (strength <= 1){
        double scale_strength1 = calculateFreqPartial(233,scaleTable[0][0][i],466,scaleTable[1][0][i],frequency);
        double scale_strength2 = calculateFreqPartial(233,scaleTable[0][1][i],466,scaleTable[1][1][i],frequency);
        scale = strength * scale_strength1 + (1 - strength) * scale_strength2;
      }
      else{
        double scale_strength1 = calculateFreqPartial(233,scaleTable[0][1][i],466,scaleTable[1][1][i],frequency);
        double scale_strength2 = calculateFreqPartial(233,scaleTable[0][2][i],466,scaleTable[1][2][i],frequency);
        scale = (2 - strength) * scale_strength1 + (strength - 1) * scale_strength2;
      }
    }
    else{
      if (strength <= 1){
        double scale_strength1 = calculateFreqPartial(466,scaleTable[1][0][i],932,scaleTable[2][0][i],frequency);
        double scale_strength2 = calculateFreqPartial(466,scaleTable[1][1][i],932,scaleTable[2][1][i],frequency);
        scale = strength * scale_strength1 + (1 - strength) * scale_strength2;
      }
      else{
        double scale_strength1 = calculateFreqPartial(466,scaleTable[1][1][i],932,scaleTable[2][1][i],frequency);
        double scale_strength2 = calculateFreqPartial(466,scaleTable[1][2][i],932,scaleTable[2][2][i],frequency);
        scale = (2 - strength) * scale_strength1 + (strength - 1) * scale_strength2;
      }
    }

    partial.setParam(WAVE_SHAPE, *waveShape );
    //Add the partial to the sound.
    newsound->add(partial);
  }
}

//----------------------------------------------------------------------------//

double Bottom::calculateFreqPartial(double x1, double y1, double x2, double y2, double x){
  //return the amplitude (ratio) of the generated partial
  //the curve between two points (x1,y1) and (x2,y2) is modeled by y=a*2^(b*(x-x1))
  //assuming y2>y1
  if ((x > x2) || (x < x1)){
    cout << "Bottom::error in calculateFreqPartials" << endl;
    return 0;
  }
  double a, b, y;
  if (y2 >= y1){
    a = y1;
    b = (log2(y2 / y1)) / (x2 - x1);
    y = a * pow(2, b * (x-x1) );
  }
  else{
    //the downward curve is supposed to be symmetric (with respect to the
    //horizontal line y1) to the above case. Model the curve as if y2>y1 and
    //take the symmetric result
    double y2_temp = y1 + (y1 - y2);
    a = y1;
    b = (log2(y2_temp / y1)) / (x2 - x1);
    double y_temp = a * pow(2, b * (x-x1) );
    y = y1 - (y_temp - y1);
  }
  return y;
}
