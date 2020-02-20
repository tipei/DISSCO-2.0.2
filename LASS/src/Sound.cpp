/*
LASS (additive sound synthesis library)
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
//	Sound.cpp
//
//----------------------------------------------------------------------------//

#ifndef __SOUND_CPP
#define __SOUND_CPP

//----------------------------------------------------------------------------//
#include "Sound.h"
#include "Score.h"
#include "Loudness.h"

//----------------------------------------------------------------------------//
Sound::Sound()
{
    setParam(DURATION, 1.0);
    setParam(START_TIME, 0.0);
    setParam(LOUDNESS, 44100.0);
    setParam(LOUDNESS_RATE, 44100.0);

    setParam(DETUNE_SPREAD, 0.0);
    setParam(DETUNE_DIRECTION, 0.0);
    setParam(DETUNE_VELOCITY, -0.5);
    setParam(DETUNE_FUNDAMENTAL, -1.0);
    filterObj = NULL;
    reverbObj = NULL;
    spatializer_ = new Spatializer();
}
  
//----------------------------------------------------------------------------//
Sound::Sound(int numPartials, m_value_type baseFreq)
{
    
    spatializer_ = new Spatializer();
    if (numPartials < 1)
    {
        cerr << "ERROR: Sound: Creation with less than 1 partial." << endl;
        return;
    }
    
    // generate each subsequent partial
    for (int i=0; i<numPartials; i++)
    {
        Partial p;
        p.setParam(RELATIVE_AMPLITUDE, (1/pow(2.71828,i)));

        // INCREMENT FREQUENCY MULTIPLIER FOR GLISSANDO
        p.setParam(FREQUENCY, baseFreq * (i+1));
        p.setParam(PARTIAL_NUM, i);
        add(p);
    }

    setParam(DURATION, 1.0);
    setParam(START_TIME, 0.0);
    setParam(LOUDNESS, 44100.0);
    setParam(LOUDNESS_RATE, 44100.0);

    setParam(DETUNE_SPREAD, 0.0);
    setParam(DETUNE_DIRECTION, 0.0);
    setParam(DETUNE_VELOCITY, -0.5);
    setParam(DETUNE_FUNDAMENTAL, -1.0);

    reverbObj = NULL;

}



//----------------------------------------------------------------------------//
void Sound::setPartialParam(PartialStaticParam p, m_value_type v)
{
    Iterator<Partial> it = iterator();
    while(it.hasNext())
        it.next().setParam(p,v);
}

//----------------------------------------------------------------------------//
void Sound::setPartialParam(PartialDynamicParam p, DynamicVariable& v)
{
    Iterator<Partial> it = iterator();
    while(it.hasNext())
        it.next().setParam(p,v);
}

//----------------------------------------------------------------------------//
void Sound::setPartialParam(PartialDynamicParam p, m_value_type v)
{
    Iterator<Partial> it = iterator();
    int i = 1;

    while(it.hasNext())
    {
        m_value_type mod_val = v;

        // FOR GLISSANDO
        if( p == FREQUENCY )
	    mod_val *= i;

        it.next().setParam(p,mod_val);
        i++;  
    }
}


//----------------------------------------------------------------------------//
/*
void Sound::getPartialParamEnv(PartialDynamicParam p)
{
    Iterator<Partial> it = iterator();
    int i = 1;

    while(it.hasNext())
    {
        m_value_type mod_val = v;

        // FOR GLISSANDO
        if( p == FREQUENCY )
	    mod_val *= i;

        it.next().getParamEnv(p);
        i++;  
    }
}
*/
//----------------------------------------------------------------------------//
MultiTrack* Sound::render(
    int numChannels,
    m_rate_type samplingRate)
    // Remember, the default value for samplingRate (as defined in the
    //  .h file is DEFAULT_SAMPLING_RATE
{
    //------------------
    // calculate loudness for this sound.
    //------------------

    // negative loudness values signify that
    // loudness is not to be calculated for this sound.
    if (getParam(LOUDNESS) >= 0)
    {
        cout << "\t Calculating Loudness..." << endl;
        //m_rate_type loudnessRate = m_rate_type(getParam(LOUDNESS_RATE));
        //Loudness::calculate(*this, loudnessRate);
        Loudness::calculate(*this);
    }

    //------------------
    // do the detuning setup 
    //------------------
    if (size() != 0)
    {
        cout << "\t Creating Envelopes..." << endl;
        Iterator<Partial> iter = iterator();

	// create the detuning envelope for this partial
	ExponentialInterpolator detuning_env;
	setup_detuning_env(&detuning_env);
	if(getParam(DETUNE_FUNDAMENTAL) > 0.0)
          iter.next().setParam(DETUNING_ENV,detuning_env);
        else //NOTE: this else was not here before (Andrew)
          iter.next();
        
        while(iter.hasNext())
        {
	    // create the detuning envelope for this partial
	    ExponentialInterpolator detuning_env;
	    setup_detuning_env(&detuning_env);
            iter.next().setParam(DETUNING_ENV,detuning_env);
        }
    }
    

    //------------------
    // render each partial, and composite into a single track.
    //------------------

    cout << "\t Rendering..." << endl;

    /*
     * duration gets tricky when you've got partials and sounds, either or both
     * of which can have different reverb applied.  'duration' refers to the
     * 'dry' duration of this sound.  This is analogous to the length of time an
     * instrument is buzzing a noise.  This is the duration of time we are
     * generating sine waves in the partial.  After 'duration', each partial may
     * or may not have reverb applied to it, and the overall sound may have
     * reverb (potentially on top -- meaning, in addition to -- the partials'
     * reverb.
     *
     * in contrast, getTotalDuration() returns the total audible length of time
     * during which this sound, its partials, and any reverb will produce noise.
     */
    m_time_type duration = getParam(DURATION);

    // compute # of samples, taking into account space for reverb die-out
    m_sample_count_type sampleCount;
    sampleCount = (m_sample_count_type) ((m_time_type)getTotalDuration() * (m_time_type)samplingRate);

    Track* composite;

    if (size() == 0)
    {
        // there are no partials
        // create a new empty track:
        composite = new Track(sampleCount, samplingRate);
        // should we zero this memory out?
    }
    else
    {
        Iterator<Partial> iter = iterator();
        composite = iter.next().render(sampleCount, duration, samplingRate);

        Track* tempTrack;
        while(iter.hasNext())
        {
            tempTrack = iter.next().render(sampleCount, duration, samplingRate);
            composite->composite(*tempTrack);
            delete tempTrack;
        }
    }
  
  
  
  
  // do the filter
  if (filterObj != NULL){
    cout << "\t Applying Filter..." << endl;
    Track &filteredTrack = filterObj->do_filter_Track(*composite);
		delete composite;
		composite = &filteredTrack;
  }
    
  
	// do the reverb
	if(reverbObj != NULL)
	{
		cout << "\t Applying Reverb..." << endl;
		Track &reverbedTrack = reverbObj->do_reverb_Track(*composite);
		delete composite;

		//------------------
		// spatialize the sound into a MultiTrack object
		//------------------

		cout << "\t Spatializing..." << endl;
		MultiTrack* mt = spatializer_->spatialize(reverbedTrack, numChannels);

		// delete the temporary track object that held the unspatialized reverbed sound
		delete &reverbedTrack;

		return mt;
	}
	else
	{
		//------------------
		// spatialize the sound into a MultiTrack object
		//------------------

		cout << "\t Spatializing..." << endl;
		MultiTrack* mt = spatializer_->spatialize(*composite, numChannels);
		delete composite;
		return mt;
	}
}

//----------------------------------------------------------------------------//
void Sound::setSpatializer(Spatializer& s)
{
    delete spatializer_;
    spatializer_ = s.clone();
}

//----------------------------------------------------------------------------//
void Sound::use_reverb(Reverb *newReverbObj)
{
  if (reverbObj != NULL){
    delete reverbObj;
  }
	reverbObj = newReverbObj;
}


void Sound::use_filter(Filter *newFilterObj){
  if (filterObj!= NULL){
    delete filterObj;
  }
  filterObj = newFilterObj;
}


//----------------------------------------------------------------------------//
void Sound::setup_detuning_env(ExponentialInterpolator *detuning_env)
{
	float x[3], y[3], spread, vel;

	// determine the shape of the envelope
	vel = getParam(DETUNE_VELOCITY);
	x[0] = 0.0;
	y[0] = 1.0;

	x[1] = (((vel*0.95)+1.0)/2.0);
	y[1] = x[1]; 
	
	x[2] = 1.0;
	y[2] = 0.0;

	// scale by the height spread of the envelope
	spread = (float)random() / (float)RAND_MAX * getParam(DETUNE_SPREAD) * 2.0;
	spread = spread - getParam(DETUNE_SPREAD);

	y[0] *= spread;
	y[1] *= spread;
	x[1] *= spread;
	y[2] *= spread;

	// then offset to normalize the whole thing at 1.0
	y[0] += 1.0;
	y[1] += 1.0;
	y[2] += 1.0;

	if(getParam(DETUNE_DIRECTION) < 0.0) // divergence (detuning)
	{
		detuning_env->addEntry(x[2], y[2]);
		detuning_env->addEntry(x[1], y[1]);
		detuning_env->addEntry(x[0], y[0]);
	}
	else // convergence (tuning)
	{
		detuning_env->addEntry(x[0], y[0]);
		detuning_env->addEntry(x[1], y[1]);
		detuning_env->addEntry(x[2], y[2]);
	}
}

//----------------------------------------------------------------------------//
m_time_type Sound::getTotalDuration(void)
{
	m_time_type curDuration, maxDuration;
	Iterator<Partial> iter = iterator();

	maxDuration = getParam(DURATION);
	while(iter.hasNext())
	{
		curDuration = iter.next().getTotalDuration(getParam(DURATION));
		if(curDuration > maxDuration)
			maxDuration = curDuration;
	}

	maxDuration += ( (reverbObj != NULL) ? reverbObj->getDecay() : 0.0);

	return maxDuration;
}

//----------------------------------------------------------------------------//
void Sound::xml_print( ofstream& xmlOutput, list<Reverb*>& revObjs, list<DynamicVariable*>& dynObjs )
{
	xmlOutput << "\t<sound>" << endl;

	// Output reverb ID and update reverb collection if necessary
	xmlOutput << "\t\t<reverb_ptr id=\"" << (long)reverbObj << "\" />" << endl;
	list<Reverb*>::const_iterator revit;
	
	for( revit=revObjs.begin(); revit != revObjs.end(); revit++ )
	{
		if( (*revit) == reverbObj )
			break;
	}
	if( revit == revObjs.end() ){
		revObjs.push_back( reverbObj );
	}

	// Static Parameters
	xmlOutput << "\t\t<duration value=\"" << getParam(DURATION) << "\" />" << endl;	
	xmlOutput << "\t\t<start_time value=\"" << getParam(START_TIME) << "\" />" << endl;
	xmlOutput << "\t\t<loudness value=\"" << getParam(LOUDNESS) << "\" />" << endl;
	xmlOutput << "\t\t<loudness_rate value=\"" << getParam(LOUDNESS_RATE) << "\" />" << endl;
	xmlOutput << "\t\t<detune_spread value=\"" << getParam(DETUNE_SPREAD) << "\" />" << endl;
	xmlOutput << "\t\t<detune_direction value=\"" << getParam(DETUNE_DIRECTION) << "\" />" << endl;
	xmlOutput << "\t\t<detune_velocity value=\"" << getParam(DETUNE_VELOCITY) << "\" />" << endl;
	xmlOutput << "\t\t<detune_fundamental value=\"" << getParam(DETUNE_FUNDAMENTAL) << "\" />" << endl;

	//Dynamic Parameters
	spatializer_->xml_print( xmlOutput );
	
	// Output XML for partials
	Iterator<Partial> it = iterator();
	while(it.hasNext())
    {
		it.next().xml_print( xmlOutput, revObjs, dynObjs );
	}

	xmlOutput << "\t</sound>" << endl;
}

//----------------------------------------------------------------------------//
void Sound::xml_read(XmlReader::xmltag* soundtag, DISSCO_HASHMAP<long, Reverb *>* reverbHash, DISSCO_HASHMAP<long, DynamicVariable *>* dvHash)
{
	if(strcmp("sound",soundtag->name))
	{
		printf("Not a sound tag?!  This is a %s tag!\n", soundtag->name);
		return;
	}

	char *value;
	
	if((value=soundtag->findChildParamValue("reverb_ptr","id")) != 0)
		if(Reverb* temp = (*reverbHash)[atoi(value)])
			use_reverb(temp);
	if((value = soundtag->findChildParamValue("duration","value")) != 0)
		setParam(DURATION, atof(value));
	if((value = soundtag->findChildParamValue("start_time","value")) != 0)
		setParam(START_TIME, atof(value));
	if((value = soundtag->findChildParamValue("loudness","value")) != 0)
		setParam(LOUDNESS, atof(value));
	if((value = soundtag->findChildParamValue("loudness_rate","value")) != 0)
		setParam(LOUDNESS_RATE, atof(value));
	if((value = soundtag->findChildParamValue("detune_spread","value")) != 0)
		setParam(DETUNE_SPREAD, atof(value));
	if((value = soundtag->findChildParamValue("detune_direction","value")) != 0)
		setParam(DETUNE_DIRECTION, atof(value));
	if((value = soundtag->findChildParamValue("detune_velocity","value")) != 0)
		setParam(DETUNE_VELOCITY, atof(value));
	if((value = soundtag->findChildParamValue("detune_fundamental","value")) != 0)
		setParam(DETUNE_FUNDAMENTAL, atof(value));
	
	XmlReader::xmltag *partialtag;

	while((partialtag=soundtag->children->find("partial")) != 0)
	{
		Partial p;
		p.xml_read(partialtag, reverbHash, dvHash);
		add(p);
	}
}

//----------------------------------------------------------------------------//
Sound::~Sound(){
  if(reverbObj)delete reverbObj;
  if(filterObj)delete filterObj;
  if(spatializer_) delete spatializer_;
  
}


//----------------------------------------------------------------------------//
#endif //__SOUND_H
