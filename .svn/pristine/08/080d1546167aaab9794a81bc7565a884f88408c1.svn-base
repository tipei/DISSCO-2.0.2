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
//	Partial.cpp
//
//----------------------------------------------------------------------------//

#ifndef __PARTIAL_CPP
#define __PARTIAL_CPP

//----------------------------------------------------------------------------//
#include "Partial.h"
#include "SoundSample.h"
#include "Constant.h"
#include "Track.h"
#include "Iterator.h"

//----------------------------------------------------------------------------//
Partial::Partial()
{
    // set some default parameters:
    setParam(WAVE_TYPE, 0);
    setParam(RELATIVE_AMPLITUDE, 1.0);
    setParam(PARTIAL_NUM, 1.0);
    setParam(WAVE_SHAPE, 1.0);
    setParam(FREQUENCY, 440.0);
    setParam(LOUDNESS_SCALAR, 1.0);
    //setParam(GLISSANDO_ENV, 1.0); // (no gliss)
    setParam(FREQ_ENV, 1.0);  // nothing applied to change frequency
    //setParam(DETUNING_ENV, 1.0); // (no detuning)
    setParam(AMPTRANS_AMP_ENV, 0 ); //no transients
    setParam(AMPTRANS_RATE_ENV, 0 );
    setParam(FREQTRANS_AMP_ENV, 0 );
    setParam(FREQTRANS_RATE_ENV, 0 );
    setParam(AMPTRANS_WIDTH, 1103);
    setParam(FREQTRANS_WIDTH, 1103);

    reverbObj = NULL;
}

//----------------------------------------------------------------------------//
Track* Partial::render(m_sample_count_type sampleCount,
                       m_time_type duration,
                       m_rate_type samplingRate)
{
    // calculate the number of samples needed:
    m_sample_count_type numSamplesToRender = (m_sample_count_type) (duration * (m_time_type)samplingRate);
    m_sample_count_type numSamplesTotal;
    if(reverbObj != NULL)
        numSamplesTotal = (m_sample_count_type) ((duration + reverbObj->getDecay()) * (m_time_type)samplingRate);
    else
        numSamplesTotal = (m_sample_count_type) (duration * (m_time_type)samplingRate);
    // create two SoundSample objects to write to:
    // one for the actuial sound:
    //    SoundSample* waveSample = new SoundSample(numSamplesTotal, samplingRate);
    SoundSample* waveSample = new SoundSample(sampleCount, samplingRate);
    // and one to store amplitude data:
    //    SoundSample* ampSample = new SoundSample(numSamplesTotal, samplingRate);
    SoundSample* ampSample = new SoundSample(sampleCount, samplingRate);

    // tell each dynamic variable what the DURATION will be:
    getParam(FREQUENCY).setDuration(duration);
    getParam(WAVE_SHAPE).setDuration(duration);
    getParam(TREMOLO_AMP).setDuration(duration);
    getParam(TREMOLO_RATE).setDuration(duration);
    getParam(VIBRATO_AMP).setDuration(duration);
    getParam(VIBRATO_RATE).setDuration(duration);
    getParam(PHASE).setDuration(duration);
    getParam(LOUDNESS_SCALAR).setDuration(duration);
    //getParam(FREQUENCY_DEVIATION).setDuration(duration);
    //getParam(GLISSANDO_ENV).setDuration(duration);
    getParam(FREQ_ENV).setDuration(duration);
    //getParam(DETUNING_ENV).setDuration(duration);
    getParam(AMPTRANS_WIDTH).setDuration(duration);
    getParam(FREQTRANS_WIDTH).setDuration(duration);

    // tell each dynamic variable what the SAMPLING_RATE will be:
    getParam(FREQUENCY).setSamplingRate(samplingRate);
    getParam(WAVE_SHAPE).setSamplingRate(samplingRate);
    getParam(TREMOLO_AMP).setSamplingRate(samplingRate);
    getParam(TREMOLO_RATE).setSamplingRate(samplingRate);
    getParam(VIBRATO_AMP).setSamplingRate(samplingRate);
    getParam(VIBRATO_RATE).setSamplingRate(samplingRate);
    getParam(PHASE).setSamplingRate(samplingRate);
    getParam(LOUDNESS_SCALAR).setSamplingRate(samplingRate);
    //getParam(FREQUENCY_DEVIATION).setSamplingRate(samplingRate);
    //getParam(GLISSANDO_ENV).setSamplingRate(samplingRate);
    getParam(FREQ_ENV).setSamplingRate(samplingRate);
    //getParam(DETUNING_ENV).setSamplingRate(samplingRate);
    getParam(AMPTRANS_WIDTH).setSamplingRate(samplingRate);
    getParam(FREQTRANS_WIDTH).setSamplingRate(samplingRate);

    // randomize the frequency deviation:
    //
    // calculate the variance:
    // grab the frequency at the start of the partial:
    //m_value_type base_freq = getParam(FREQUENCY).valueIterator().next();
    //m_value_type freq_variance=(base_freq/(getParam(PARTIAL_NUM)+1.0))/2.0;
    //
    // get a random nummber between -1 and 1:
    //m_value_type rand = (((double)random())/((double)RAND_MAX)*2.0)-1.0;
    //
    // vary the freq_dev by this value:
    // make a copy of the frequency deviation dynamic variable
    //DynamicVariable* freq_dev = getParam(FREQUENCY_DEVIATION).clone();
    // scale the copy:
    //freq_dev->scale(rand * freq_variance);
    //
    // make a copy of the glissando dynamic variable


    DynamicVariable* frequency_env = getParam(FREQUENCY).clone();
    frequency_env->setDuration(duration);

    //DynamicVariable* gliss_env = getParam(GLISSANDO_ENV).clone();
    DynamicVariable* freq_env = getParam(FREQ_ENV).clone();
    //DynamicVariable* detuning_env = getParam(DETUNING_ENV).clone();
    DynamicVariable* amptrans_amp_env = getParam(AMPTRANS_AMP_ENV).clone();
    DynamicVariable* amptrans_rate_env = getParam(AMPTRANS_RATE_ENV).clone();
    DynamicVariable* freqtrans_amp_env = getParam(FREQTRANS_AMP_ENV).clone();
    DynamicVariable* freqtrans_rate_env = getParam(FREQTRANS_RATE_ENV).clone();
    //gliss_env->setDuration(duration);
    freq_env->setDuration(duration);
    //detuning_env->setDuration(duration);
    amptrans_amp_env->setDuration(duration);
    amptrans_rate_env->setDuration(duration);
    freqtrans_amp_env->setDuration(duration);
    freqtrans_rate_env->setDuration(duration);

    // grab iterators for each DynamicVariable:
    typedef Iterator<m_value_type> ValIter;

    // ValIter freq_it = getParam(FREQUENCY).valueIterator();
    //more messing around
    ValIter frequency_it = frequency_env->valueIterator();

    ValIter wave_shape_it = getParam(WAVE_SHAPE).valueIterator();
    ValIter tremolo_amp_it = getParam(TREMOLO_AMP).valueIterator();
    ValIter tremolo_rate_it = getParam(TREMOLO_RATE).valueIterator();
    ValIter vibrato_amp_it = getParam(VIBRATO_AMP).valueIterator();
    ValIter vibrato_rate_it = getParam(VIBRATO_RATE).valueIterator();
    ValIter phase_it = getParam(PHASE).valueIterator();
    ValIter loudnes_scalar_it = getParam(LOUDNESS_SCALAR).valueIterator();
    ValIter amptrans_width_it = getParam(AMPTRANS_WIDTH).valueIterator();
    ValIter freqtrans_width_it = getParam(FREQTRANS_WIDTH).valueIterator();
    //ValIter freq_deviation_it = freq_dev->valueIterator();
    //ValIter gliss_it = gliss_env->valueIterator();
    ValIter freq_it = freq_env->valueIterator();
    //ValIter detuning_it = detuning_env->valueIterator();
    ValIter amptrans_amp_it = amptrans_amp_env->valueIterator();
    ValIter amptrans_rate_it = amptrans_rate_env->valueIterator();
    ValIter freqtrans_amp_it = freqtrans_amp_env->valueIterator();
    ValIter freqtrans_rate_it = freqtrans_rate_env->valueIterator();

    // Phase values start off at zero for now:
    m_value_type tremolo_phase = 0.0;
    m_value_type vibrato_phase = 0.0;
    m_value_type freq_phase = 0.0;

    // values used in the loop:
    m_value_type tremolo;
    m_value_type amplitude;
    m_value_type vibrato;
    m_value_type frequency;
    m_value_type phase;
    m_value_type sample;
    m_value_type amptransient;
    m_value_type freqtransient;
    m_value_type amptrans_width = 0.0;
    m_value_type freqtrans_width = 0.0;
    m_time_type amptransprob;
    m_time_type freqtransprob;

    srand(time(0));

    //flags to tell if we are in a transient
    int amptransflag = 0;
    int freqtransflag = 0;

    //placemarkers once in a transient
    int amptransplace = 0;
    int freqtransplace = 0;

    //counters to see if we should check for a transient
    int amptranscheck = (int)amptrans_width;
    int freqtranscheck = (int)freqtrans_width;
    float random;

    m_value_type freqmod = 0.0, trans_freqmod = 0;
    m_value_type amplifier = 0.0, trans_amplifier = 0;
    // loop over every sample:
    for (m_sample_count_type s=0; s < numSamplesToRender; s++)
    {

      // AMPLITUDE:

      //advance transient related iterators
      amptrans_width = amptrans_width_it.next();
      trans_amplifier = amptrans_amp_it.next();
      amptransprob = amptrans_rate_it.next();

      // grab the tremolo value
        tremolo =  tremolo_amp_it.next() * sin(2.0 * M_PI * tremolo_phase);
        // increment the tremolo phase
        tremolo_phase = pmod( tremolo_phase + (tremolo_rate_it.next() / samplingRate) );

	//once counter reaches 0, check for transient
	if(amptranscheck <= 0 && s+amptrans_width < numSamplesToRender)
	  {
	    random = ((float)std::rand()/(float)RAND_MAX);
	    if(random <= amptransprob)
	      {
		trans_amplifier *= (float)std::rand()/(float)RAND_MAX;
		if(((float)std::rand()/(float)RAND_MAX) <= 0.5)
		  trans_amplifier *= -1;
		amplifier = trans_amplifier;
		amptransflag = 1;
		//we are at beginning of transient
		amptransplace = 0;
	      }
	    //reset check counter
	    amptranscheck = (int)amptrans_width;
	  }

        // augment the amplitude value by the tremolo value, and the loudness scale:
	if(amptransflag)
	  {
	    //the following decides where we are inside the transient
	    //used to scale the transient multiplier effect
	    if(amptransplace < amptrans_width/2)
	      amptransient = amplifier*amptransplace/(amptrans_width/2);
	    else
	      amptransient = amplifier*(amptrans_width - amptransplace)/(amptrans_width/2);
	    amptransplace++;
	    if(amptransplace >= amptrans_width)
	      amptransflag = 0;
	  }
	else
	    amptransient = 0;

	//decrease the check counter
	amptranscheck--;

        amplitude = loudnes_scalar_it.next() * wave_shape_it.next() * (1.0 + tremolo);

	//apply transient modifier to amplitude
	amplitude = amplitude + amptransient*amplitude;














	// FREQUENCY:

	//advance frequency transient related iterators
	freqtrans_width = freqtrans_width_it.next();
	trans_freqmod = freqtrans_amp_it.next();
	freqtransprob = freqtrans_rate_it.next();

	//if we should check, check
	if(freqtranscheck <= 0 && s+freqtrans_width < numSamplesToRender)
	  {
	    random = ((float)std::rand()/(float)RAND_MAX);
	    if(random <= freqtransprob)
	      {
		trans_freqmod *= (float)std::rand()/(float)RAND_MAX;
		if(((float)std::rand()/(float)RAND_MAX) <= 0.5)
		  trans_freqmod *= -1;
		freqmod = trans_freqmod;
		freqtransflag = 1;
		freqtransplace = 0;
	      }
	    freqtranscheck = (int)freqtrans_width;
	  }

	//if we are inside a transient, make appropriate changes
	if(freqtransflag)
	  {
	    if(freqtransplace < freqtrans_width/2)
	      freqtransient = freqmod*freqtransplace/(freqtrans_width/2);
	    else
	      freqtransient = freqmod*(freqtrans_width - freqtransplace)/(freqtrans_width/2);
	    freqtransplace++;
	    if(freqtransplace >= freqtrans_width)
	      freqtransflag = 0;
	  }
	else
	    freqtransient = 0;
	freqtranscheck--;

        // perhaps the vibrato should be keyed to PartialNumber.

        // grab the vibrato value
        vibrato =  vibrato_amp_it.next() * sin(2.0 * M_PI * vibrato_phase);
        // increment the vibrato phase
        vibrato_phase = pmod( vibrato_phase + (vibrato_rate_it.next() / samplingRate) );

        // augment the frequency value by the freq-deviation and vibrato and detuning and freq_env and transients
        frequency  = frequency_it.next() * freq_it.next(); // * detuning_it.next()
        //frequency += freq_deviation_it.next();
        frequency *= 1.0 + vibrato;


	//apply frequency transient adjustments
	frequency += frequency*freqtransient;


        // increment the frequency phase:
        freq_phase = pmod (freq_phase + (frequency / samplingRate));

        // calculate the actual phase, augmenting freq_phase with
        // they dynamic variable phase (phase offset)
        phase = freq_phase + phase_it.next();


        // SAMPLE:

        switch( (int) getParam(WAVE_TYPE))
        {
            case 1:
                // random
                sample = amplitude * 2 * ((((double) std::rand()) / ((double) RAND_MAX)) - 0.5);
                break;

            default:
                // calculate the sample
                sample = amplitude * ( sin(2.0 * M_PI * phase));
                break;
        }

        // assign it:
        (*waveSample)[s] = sample;
        (*ampSample)[s] = amplitude;
    }

    // create a TrackObject:
    Track *returnTrack = new Track(waveSample, ampSample);

    // do the reverb, if necessary
    if(reverbObj != NULL)
    {
        Track &tmp = reverbObj->do_reverb_Track(*returnTrack);
        delete returnTrack;
        returnTrack = new Track(tmp);

        // delete the temporary track object
        delete &tmp;

    }


    // this section is added by Ming-ching on Dec.10 2012 to prevent memory leak
    delete frequency_env;
    delete freq_env;
    delete amptrans_amp_env;
    delete amptrans_rate_env;
    delete freqtrans_amp_env;
    delete freqtrans_rate_env;


    return returnTrack;
}

//----------------------------------------------------------------------------//
inline m_value_type Partial::pmod(m_value_type num)
{
    while (num > 1.0)
        num -= 1.0;
    return num;
}


//----------------------------------------------------------------------------//
void Partial::use_reverb(Reverb *newReverbObj)
{
	reverbObj = newReverbObj;
}

//----------------------------------------------------------------------------//
m_time_type Partial::getTotalDuration(m_time_type dryDuration)
{
	return dryDuration + ( (reverbObj != 0) ? reverbObj->getDecay() : 0 );
}

//----------------------------------------------------------------------------//
void Partial::xml_print( ofstream& xmlOutput, list<Reverb*>& revObjs, list<DynamicVariable*>& dynObjs )
{

	xmlOutput << "\t\t<partial>" << endl;

        // Output reverb ID and update reverb collection if necessary
        xmlOutput << "\t\t\t<reverb_ptr id=\"" << (long)reverbObj << "\" />" << endl;
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
	xmlOutput << "\t\t\t<relative_amplitude value=\"" << getParam(RELATIVE_AMPLITUDE) << "\" />" << endl;
	xmlOutput << "\t\t\t<partial_num value=\"" << getParam(PARTIAL_NUM) << "\" />" << endl;

	// Dynamic Paramters
	// Call the xml_print for each Dynamic Var

	xmlOutput << "\t\t\t<frequency>" << endl;
	getParam(FREQUENCY).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</frequency>" << endl;

	xmlOutput << "\t\t\t<wave_shape>" << endl;
	getParam(WAVE_SHAPE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</wave_shape>" << endl;

	xmlOutput << "\t\t\t<loudness_scalar>" << endl;
	getParam(LOUDNESS_SCALAR).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</loudness_scalar>" << endl;

	xmlOutput << "\t\t\t<tremolo_amp>" << endl;
	getParam(TREMOLO_AMP).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</tremolo_amp>" << endl;

	xmlOutput << "\t\t\t<tremolo_rate>" << endl;
	getParam(TREMOLO_RATE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</tremolo_rate>" << endl;

	xmlOutput << "\t\t\t<vibrato_amp>" << endl;
	getParam(VIBRATO_AMP).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</vibrato_amp>" << endl;

	xmlOutput << "\t\t\t<vibrato_rate>" << endl;
	getParam(VIBRATO_RATE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</vibrato_rate>" << endl;

	xmlOutput << "\t\t\t<phase>" << endl;
	getParam(PHASE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</phase>" << endl;

	//xmlOutput << "\t\t\t<frequency_deviation>" << endl;
	//getParam(FREQUENCY_DEVIATION).xml_print( xmlOutput, dynObjs );
	//xmlOutput << "\t\t\t</frequency_deviation>" << endl;

	/*xmlOutput << "\t\t\t<glissando_env>" << endl;
	getParam(GLISSANDO_ENV).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</glissando_env>" << endl;
	*/


	xmlOutput << "\t\t\t<freq_env>" << endl;
	getParam(FREQ_ENV).xml_print(xmlOutput, dynObjs);
	xmlOutput << "\t\t\t</freq_env>" << endl;


	//xmlOutput << "\t\t\t<detuning_env>" << endl;
	//getParam(DETUNING_ENV).xml_print( xmlOutput, dynObjs );
	//xmlOutput << "\t\t\t</detuning_env>" << endl;

	xmlOutput << "\t\t</partial>" << endl;
}

void Partial::xml_read(XmlReader::xmltag* partialtag, DISSCO_HASHMAP<long, Reverb *>* reverbHash, DISSCO_HASHMAP<long, DynamicVariable *> *dvHash)
{
	if(strcmp("partial",partialtag->name))
	{
		printf("Not a partial tag!  This is a %s tag.\n",partialtag->name);
		return;
	}

	char *value;

	if((value = partialtag->findChildParamValue("reverb_ptr", "id")) != 0)
	{
		long id=atoi(value);
		Reverb * temp;
		temp = (*reverbHash)[id];

		if(temp != NULL)
		{
			use_reverb(temp);
		}
	}

	if((value = partialtag->findChildParamValue(
	  "relative_amplitude", "value")) != 0)
	  	setParam(RELATIVE_AMPLITUDE, atof(value));

	if((value = partialtag->findChildParamValue("partial_num","value")) != 0)
		setParam(PARTIAL_NUM, atoi(value));

	// For sake of ease, I am going to forego searching for each item and instead
	// iterate thru the list of child tags.

	XmlReader::xmltag *dvtag;	XmlReader::xmltagset *child=partialtag->children;

	while(child)
	{
		dvtag=child->tag;

		if(strcmp(dvtag->name,"frequency") == 0)
			auxLoadParam(FREQUENCY,dvtag,dvHash);
		else if(strcmp(dvtag->name,"wave_shape") == 0)
			auxLoadParam(WAVE_SHAPE,dvtag,dvHash);
		else if(strcmp(dvtag->name,"loudness_scalar") == 0)
			auxLoadParam(LOUDNESS_SCALAR,dvtag,dvHash);
		else if(strcmp(dvtag->name,"tremolo_amp") == 0)
			auxLoadParam(TREMOLO_AMP,dvtag,dvHash);
		else if(strcmp(dvtag->name,"tremolo_rate") == 0)
			auxLoadParam(TREMOLO_RATE,dvtag,dvHash);
		else if(strcmp(dvtag->name,"vibrato_amp") == 0)
			auxLoadParam(VIBRATO_AMP,dvtag,dvHash);
		else if(strcmp(dvtag->name,"vibrato_rate") == 0)
			auxLoadParam(VIBRATO_RATE,dvtag,dvHash);
		else if(strcmp(dvtag->name,"phase") == 0)
			auxLoadParam(PHASE,dvtag,dvHash);
		//else if(strcmp(dvtag->name,"frequency_deviation") == 0)
		//auxLoadParam(FREQUENCY_DEVIATION,dvtag,dvHash);
		//else if(strcmp(dvtag->name,"glissando_env") == 0)
		//auxLoadParam(GLISSANDO_ENV,dvtag,dvHash);
		else if(strcmp(dvtag->name,"freq_env") == 0)
		  auxLoadParam(FREQ_ENV,dvtag,dvHash);
		//else if(strcmp(dvtag->name,"detuning_env") == 0)
		//auxLoadParam(DETUNING_ENV,dvtag,dvHash);

		child=child->next;
	}
}

void Partial::auxLoadParam(enum PartialDynamicParam param,XmlReader::xmltag *tag, DISSCO_HASHMAP<long, DynamicVariable *> *dvHash)
{
	char *value;

	// Try and do a lookup
	if((value = tag->findChildParamValue("dv_ptr", "id")) != 0)
	{
		long id=atol(value);
		DynamicVariable *dv=(*dvHash)[id];

		if(dv)
			setParam(param,*dv);
		else
			//printf("[%s:%d] PartialParam refrences non-existant dv %d!\n",__FILE__,__LINE__,id);
			cout<<"["<<__FILE__<<":"<<__LINE__<<"] PartialParam references non-existant dv "<<id<<"!"<<endl;
	}

	//Else read in the tag
	else
	{
		DynamicVariable *dv=DynamicVariable::create_dv_from_xml(tag);
		setParam(param,*dv);
	}
}

#endif //__PARTIAL_CPP
