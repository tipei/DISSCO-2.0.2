//----------------------------------------------------------------------------//
//
//	MOSS - Musical Object-Oriented Sound Synthesis
//
//	Reverb.cpp
//
//	Written by Andrew Kurtz & Jim Lindstrom 2002
//
//      Edited for use with Reverb Envelopes by Andy Latham, March 2005
//      April 25, 2005:  Finished updating for reverb envelopes.
//        I'm not sure if the XML output will work correctly. -AL
//.
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __MOSS_REVERB_CPP
#define __MOSS_REVERB_CPP

//----------------------------------------------------------------------------//

#include "SoundSample.h"
#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"
#include "LPCombFilter.h"
#include "AllPassFilter.h"
#include "Reverb.h"
#include "Types.h"
#include "Filter.h"
#include <chrono>
#ifdef HAVE_CUDA
  #include "../CUDA/FilterGPU.h"
#endif

//----------------------------------------------------------------------------//


/**
 *  default constructor
 **/
Reverb::Reverb(m_rate_type samplingRate)
{
  float comb_gain_list[REVERB_NUM_COMB_FILTERS];
  float lp_gain_list[REVERB_NUM_COMB_FILTERS];
  int i;

  comb_gain_list[0] = 0.46;
  comb_gain_list[1] = 0.48;
  comb_gain_list[2] = 0.50;
  comb_gain_list[3] = 0.52;
  comb_gain_list[4] = 0.53;
  comb_gain_list[5] = 0.55;

  Collection<envelope_segment> segmentCollection;
  envelope_segment seg;

  seg.x = 0.0; seg.y = 0.83;
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE;
  segmentCollection.add(seg);
  seg.x = 1.0; seg.y = 0.83;
  segmentCollection.add(seg);

  percentReverb = new Envelope(segmentCollection);

  float hi_low_spread = 0.05;
  for(i=0;i<REVERB_NUM_COMB_FILTERS;i++)
    lp_gain_list[i] = 0.05 + (hi_low_spread * (0.95 - comb_gain_list[i]));

  ConstructorCommon(percentReverb, &comb_gain_list[0], &lp_gain_list[0],
		    0.7, 0.6, samplingRate);

}

Reverb::Reverb(float room_size, m_rate_type samplingRate)
{
  float comb_gain_list[REVERB_NUM_COMB_FILTERS];
  float lp_gain_list[REVERB_NUM_COMB_FILTERS];
  int i;

  Collection<envelope_segment> segmentCollection;
  envelope_segment seg;

  float flatReverb = 0.345 + (0.625 * room_size);
  seg.x = 0.0; seg.y = flatReverb;
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE;
  segmentCollection.add(seg);
  seg.x = 1.0; seg.y = flatReverb;
  segmentCollection.add(seg);
  percentReverb = new Envelope(segmentCollection);
  float hilow_spread  = 0.056 + (0.430 * room_size);
  float gainAllPass   = 0.7;
  float delay         = 0.176 + (0.233 * room_size);

//  percentReverb->Print();
  comb_gain_list[0] = 0.46;
  comb_gain_list[1] = 0.48;
  comb_gain_list[2] = 0.50;
  comb_gain_list[3] = 0.52;
  comb_gain_list[4] = 0.53;
  comb_gain_list[5] = 0.55;

  for(i=0;i<REVERB_NUM_COMB_FILTERS;i++)
    lp_gain_list[i] = 0.05 + (hilow_spread * (0.95 - comb_gain_list[i]));

  ConstructorCommon(percentReverb, &comb_gain_list[0], &lp_gain_list[0],
		    gainAllPass, delay, samplingRate);

}

Reverb::Reverb(Envelope *percentReverbinput, float hilow_spread, float gainAllPass,
	       float delay, m_rate_type samplingRate)
{
  float comb_gain_list[REVERB_NUM_COMB_FILTERS];
  float lp_gain_list[REVERB_NUM_COMB_FILTERS];
  int i;
  Envelope* temp = new Envelope(*percentReverbinput);
  percentReverb = new Envelope(*temp);

  comb_gain_list[0] = 0.46;
  comb_gain_list[1] = 0.48;
  comb_gain_list[2] = 0.50;
  comb_gain_list[3] = 0.52;
  comb_gain_list[4] = 0.53;
  comb_gain_list[5] = 0.55;

  for(i=0;i<REVERB_NUM_COMB_FILTERS;i++)
    {
//      cout<<i<<endl;
      lp_gain_list[i] = 0.05 + (hilow_spread * (0.95 - comb_gain_list[i]));
    }
  ConstructorCommon(percentReverb, &comb_gain_list[0], &lp_gain_list[0],
		    gainAllPass, delay, samplingRate);
  delete temp;
}

/**
 *  advanced constructor
 *
 * percentReverb - this Envelope that defines the intended mix between reverb
 *   sound and the original sound over the length of the sound.
 *   1.00 is all reverb sound, 0.00 is all original sound.
 * combGainList - this is an array of 6 floats that acts like an equalizer
 *   for the reverbed sound.  Each float controls the gain on one band of
 *   the reverbed sound.  If, for example, you zero the first number, you
 *   the reverberator will have no effect on the lowest frequencies in your
 *   source sound, but will still respond to higher frequencies.
 * gainAllPasss - this parameter has two effects.  For delay values in the
 *   range that 'Elements of Computer Music' intended for this reverberator
 *   to be used with (anything on the order of milliseconds), this
 *   allPassGain acts to delay the reverb'ed sound slightly and smooth the
 *   reverb'ed sound.  At much higher delay values, on the order of seconds
 *   or tenth of seconds, the gainAllPass parameter primarily acts as a rate
 *   of echo decay parameter.  If you imagine an infinite series of echos,
 *   multiplied by an exponentially decaying envelope, this parameter acts
 *   as the decay rate of the envelope (higher values mean higher decay rate,
 *   lower values mean lower decay rate, and thus more echos)
 * delay - this is the overall delay in units of seconds.  This is analogous
 *   to room size.
 * samplingRate - the sampling rate of the input sounds
 **/
Reverb::Reverb(Envelope *percentReverbinput, float *combGainList, float *lpGainList,
	       float gainAllPass, float delay, m_rate_type samplingRate)
{
  // We have to copy this envelope, to prevent a seg fault when the common
  // constructor deletes it  --- cavis
  Envelope* temp = new Envelope(*percentReverbinput);
  percentReverb = new Envelope(*temp);

  ConstructorCommon(percentReverbinput, combGainList, lpGainList, gainAllPass,
		    delay, samplingRate);
}

/**
 * constructor common - this code is used to unify code from the two
 *   constructors. Both do the same work, but based on varying degrees
 *   of user specification of parameters
 *
 * percentReverb - this Envelope defines the intended mix between reverb
 *   sound and the original sound over the length of the sound.
 *   1.00 is all reverb sound, 0.00 is all original sound.
 * combGainList - this is an array of 6 floats that acts like an equalizer
 *   for the reverbed sound.  Each float controls the gain on one band of
 *   the reverbed sound.  If, for example, you zero the first number, you
 *   the reverberator will have no effect on the lowest frequencies in your
 *   source sound, but will still respond to higher frequencies.
 * gainAllPasss - this parameter has two effects.  For delay values in the
 *   range that 'Elements of Computer Music' intended for this reverberator
 *   to be used with (anything on the order of milliseconds), this
 *   allPassGain acts to delay the reverb'ed sound slightly and smooth the
 *   reverb'ed sound.  At much higher delay values, on the order of seconds
 *   or tenth of seconds, the gainAllPass parameter primarily acts as a rate
 *   of echo decay parameter.  If you imagine an infinite series of echos,
 *   multiplied by an exponentially decaying envelope, this parameter acts
 *   as the decay rate of the envelope (higher values mean higher decay rate,
 *   lower values mean lower decay rate, and thus more echos)
 * delay - this is the overall delay in units of seconds.  This is analogous
 *   to room size.
 * hilow_spread - this is a value between 0.0 and 1.0 (inclusive) that
 *   determines the ratio of low-repsonse to high response.  For a value of
 *   1.0, the low-frequence response of the reverb will be as high as possible
 *   given the values of the other parameters.  For a value of 0.0, the reverb
 *   will have as uniform a response with respect to freqeuency as allowed by
 *   the other reverb parameters
 * samplingRate - the sampling rate of the input sounds
 **/
void Reverb::ConstructorCommon(Envelope *percentReverbinput, float *comb_gain_list,
			       float *lp_gain_list, float gainAllPass, float delay,
			       m_rate_type samplingRate)
{
  long combdelay[REVERB_NUM_COMB_FILTERS];
  int i;
  Envelope* temp = new Envelope(*percentReverbinput);
  delete percentReverb;
  percentReverb = temp;
  // figure out allpass filter parameters
  //gainReverb = percentReverb;              //these two lines need fixing
  //    actually, the above and below two lines probably won't be needed
  //gainDirect = 1 - percentReverb;

  combdelay[0] = (long)(samplingRate*0.050);
  combdelay[1] = (long)(samplingRate*0.056);
  combdelay[2] = (long)(samplingRate*0.061);
  combdelay[3] = (long)(samplingRate*0.068);
  combdelay[4] = (long)(samplingRate*0.072);
  combdelay[5] = (long)(samplingRate*0.078);

  // Make comb filters (figure out low-pass gains better)
  for(i=0;i<REVERB_NUM_COMB_FILTERS;i++)
    {
      lpcfilter[i] = new LPCombFilter(comb_gain_list[i], combdelay[i], lp_gain_list[i]);
    }

  // Make an all pass filter
  allPassDelay = delay;
  apfilter = new AllPassFilter(gainAllPass, (long)((m_time_type)allPassDelay*samplingRate) );

  // Find the decay time
  float alpha = 0.0; // alpha is the steady state gain (which is also the max of the gain fn
#define max(x,y) ((x) > (y) ? (x) : (y))
  for(i=0;i<6;i++)
    alpha = max(alpha, comb_gain_list[i]/(1.0 - lp_gain_list[i]));
  float T_r = -3.0 * delay / log(alpha);
  decay_duration = T_r*1.0;
}

/**
 *	destructor
 **/
Reverb::~Reverb()
{
  //cout << "&&&&&&&&&&&&&&&&&&&&& REVERB DESTRUCTOR CALLED!" << endl;
  int i;

  for(i=0;i<REVERB_NUM_COMB_FILTERS;i++)
    delete lpcfilter[i];

  delete apfilter;
  delete percentReverb;
}

/**
 *	This method applies reverb to a single sample
 **/
m_sample_type Reverb::do_reverb(m_sample_type x_t, float x_value)
{
  return do_reverb(x_t, x_value, percentReverb);
}
m_sample_type Reverb::do_reverb(m_sample_type x_t, float x_value, Envelope *percentReverbinput)
{
  m_sample_type y;
  Envelope* temp = new Envelope(*percentReverbinput);
  delete percentReverb;
  percentReverb = temp;

  // run the sample through various comb filters (for effeciency
  // reasons, I hard coded this (instead of looping from 0 to
  // (REVERB_NUM_COMB_FILTERS-1).
  y  = lpcfilter[0]->do_filter(x_t);
  y += lpcfilter[1]->do_filter(x_t);
  y += lpcfilter[2]->do_filter(x_t);
  y += lpcfilter[3]->do_filter(x_t);
  y += lpcfilter[4]->do_filter(x_t);
  y += lpcfilter[5]->do_filter(x_t);
  y /= (m_sample_type)REVERB_NUM_COMB_FILTERS;


  // after adding up the results, run it through an allpass filter
  y = apfilter->do_filter(y);
  // Mix it with the input sound
  float durationofEnv = percentReverb->getDuration();
  float EnvelopeValueAtx = percentReverb->getValue(x_value,durationofEnv);
  y = (EnvelopeValueAtx*y) + ((1 - EnvelopeValueAtx)*x_t);
  
  // if(x_at == 0)
  //   cout<<"y0 "<<y<<endl<<"EvenlopeValueAtx "<<EnvelopeValueAtx<<endl;
  
  // if(x_at == 1000)
  //   cout<<"y1000 "<<y<<endl<<"EvenlopeValueAtx "<<EnvelopeValueAtx<<endl;
  
  // if(x_at == 10000)
  //   cout<<"y10000 "<<y<<endl<<"EvenlopeValueAtx "<<EnvelopeValueAtx<<endl;
  
  // if(x_at == 100000)
  //   cout<<"y100000 "<<y<<endl<<"EvenlopeValueAtx "<<EnvelopeValueAtx<<endl;
  // x_at++;

  return y;
}

/**
 *  This method resets the reverb unit.  Comb-filters and all-pass
 *    filters are state-ful machines -- meaning that use not only the
 *    parameter from the current call, but also the previous n parameters
 *    and combine them, in some fashion, to produce the current output.
 *    Thus two calls to reverb or a filter with identical parameters might
 *    not produce the same output, unless you call the reset() member to
 *    reset internal buffers.
 **/
void Reverb::reset(void)
{
  int i;

  for(i=0;i<REVERB_NUM_COMB_FILTERS;i++)
    lpcfilter[i]->reset();
  apfilter->reset();
}

/**
 *	This method applies reverb to MultiTrack
 **/
MultiTrack &Reverb::do_reverb_MultiTrack(MultiTrack &inWave)
{
  return do_reverb_MultiTrack(inWave, percentReverb);
}

MultiTrack &Reverb::do_reverb_MultiTrack(MultiTrack &inWave, Envelope *percentReverbinput)
{
  MultiTrack *newMultiTrack;
  SoundSample *newWave, *newAmp;

  // create the new MultiTrack
  newMultiTrack = new MultiTrack();

  Envelope* temp = new Envelope(*percentReverbinput);
  delete percentReverb;
  percentReverb = temp;
  // iterate over all individual tracks
  // for each track:
  //  grab the SoundSample from inside the Track
  //  filter the SoundSample, which returns a NEW SoundSample
  //  reset the filter so that it can be used fresh next time
  //  create a new track based on the returned, filtered SoundSample
  //  add this new track to the output MultiTrack
  Iterator<Track*> it = inWave.iterator();
  while (it.hasNext())
    {
      Track *curTrack = it.next();

      newWave = do_reverb_SoundSample(&curTrack->getWave(), percentReverb);
      reset();
      newAmp  = constructAmp(newWave);
      reset();
      newMultiTrack->add(new Track(newWave, newAmp));
    }

  return *newMultiTrack;
}

/**
 *	This method applies reverb to Track
 **/
Track &Reverb::do_reverb_Track(Track &inWave)
{
  return do_reverb_Track(inWave, percentReverb);
}
Track &Reverb::do_reverb_Track(Track &inWave, Envelope *percentReverbinput)
{
  Track *newTrack;
  SoundSample *newWave, *newAmp;
  // delete percentReverb;
  // percentReverb = new Envelope(*percentReverbinput);

  Envelope* temp = new Envelope(*percentReverbinput);
  delete percentReverb;
  percentReverb = temp;
  // grab the SoundSample from inside the Track
  // filter the SoundSample, which returns a NEW SoundSample
  // create a new track based on the returned, filtered SoundSample
  newWave = do_reverb_SoundSample(&inWave.getWave(), percentReverb);
  reset();
  newAmp  = constructAmp(newWave);
  newTrack = new Track(newWave, newAmp);

  return *newTrack;
}

/**
 *	This method applies reverb to a SoundSample object
 **/
SoundSample *Reverb::do_reverb_SoundSample(SoundSample *inWave)
{
  return do_reverb_SoundSample(inWave, percentReverb);
}
SoundSample *Reverb::do_reverb_SoundSample(SoundSample *inWave, Envelope *percentReverbinput)
{

  int i;
  SoundSample *outWave;  
  Envelope* temp = new Envelope(*percentReverbinput);
  delete percentReverb;
  percentReverb = temp;

  #ifdef HAVE_CUDA
  outWave=do_reverb_SoundSample_GPU(inWave, percentReverb, lpcfilter, apfilter);
  #else
    // create new SoundSample
    outWave = new SoundSample(inWave->getSampleCount(),
	  		    inWave->getSamplingRate());

    for(i=0;i<inWave->getSampleCount();i++)
      (*outWave)[i] = do_reverb((*inWave)[i],(float) i / inWave->getSampleCount()
			      , percentReverb);
  #endif

  return outWave;
}

/**
 *  This method returns the delay (analogous to the hall size, and in
 *    units of seconds) that the reverberator is using
 **/
float Reverb::getDecay(void)
{
  // this value used to be multiplied by 4 for some reason that I can't fathom.  this
  // was the cause of a multitude of extra silence at the end of the file, so I killed it.
  //  -AL
  return decay_duration;
}

/**
 *  This method returns a pointer to the reverb envelope
 *
 **/
Envelope* Reverb::getEnvelope(void)
{
  return percentReverb;
}

/**
 *  Reconstruct an Amp SoundSample given the Wave SoundSample
 *
 * This function works by scanning from t=0 to t=max over a
 * 'wave' SoundSample and finding the max over a window around
 * every sample.  The size of the window (see windowRadius) is
 * calculated so that the lowest possible sound component (20
 * Hz) will fit one cycle into that window.  Then, no matter
 * what partials are present in a SoundSample, we can always
 * be assured of finding at least one full cycle of every frequency
 * component.
 *
 * the algorithm used here is a slight optimization of simply running
 * a nested loop (once over all samples, and an inner one over the
 * window surrounding that point, plus or minus windowRadius).  Instead,
 * we only search the full window once at the beginning, or if the previous
 * maximum amplitude value falls out of the left end (lower time values) of
 * the window.  Otherwise, we simply increment our step through time values
 * (pos) and compare the new sample in the right-most position in our window
 * with the max amplitude from the last step.  This way we SIGNIFICANTLY
 * cut down on running time.  The simple nested-loop algorithm evaluates every
 * point 2205 times.  This algorithm evaluates only as much as needed.  I
 * have no idea what that average- or worst-case complexity is, but suffice
 * to say, this is WAY faster.
 **/
SoundSample *Reverb::constructAmp(SoundSample *wave)
{
  m_sample_count_type nSamples, windowRadius;
  long ctr1, pos, maxAmpPos;
  m_rate_type sampRate;
  SoundSample *ampWave;
  m_sample_type maxAmp;

  // initialize various variables
  // loop over all samples
  nSamples = wave->getSampleCount();
  sampRate = wave->getSamplingRate();
  ampWave = new SoundSample(nSamples, sampRate, false);
  windowRadius = (m_sample_count_type) (1.0/20.0*sampRate / 2.0);

  // find the initial max (over a window of plus/minus windowRadius,
  // with the center at index=0 of the wave SoundSample
  maxAmp = (*wave)[0];
  maxAmpPos = 0;
  for(ctr1=1; ctr1<min(windowRadius,nSamples); ctr1++)
    {
      if((*wave)[ctr1] > maxAmp)
	{
	  maxAmp = (*wave)[ctr1];
	  maxAmpPos = ctr1;
	}
    }

  // loop over all samples
  for(; ctr1<nSamples; ctr1++)
    {
      // has the old maximum gone out of range (too far to the left
      //  of the window) ?
      if(maxAmpPos <= (ctr1 - windowRadius))
	{
	  // yes, so find a new maximum over the whole window
	  maxAmp = 0.0;
	  for(pos = max(ctr1-windowRadius,0); pos < min(ctr1+windowRadius,nSamples); pos++)
	    {
	      if((*wave)[pos] > maxAmp)
		{
		  maxAmp = (*wave)[pos];
		  maxAmpPos = pos;
		}
	    }
	}
      else if(maxAmp < (*wave)[min(ctr1+windowRadius, nSamples)])
	{
	  maxAmp = (*wave)[min(ctr1+windowRadius, nSamples)];
	  maxAmpPos = min(ctr1+windowRadius, nSamples);
	}
      (*ampWave)[ctr1] = maxAmp;
    }
  return ampWave;
}

/**
 *   This outputs an XML representation of the object to STDOUT
 *
 **/
void Reverb::xml_print( ofstream& xmlOutput )
{
  int i;
  Reverb* pnt2rev = this;
  xmlOutput << "<reverb id=\"" << (long)pnt2rev << "\">" << endl;
  //  I don't know if this next line will work -AL
  xmlOutput << "\t<percentReverb value=\"" << percentReverb << "\" />" << endl;
  xmlOutput << "\t<allPassDelay value=\"" << allPassDelay << "\" />" << endl;
  xmlOutput << "\t<decay_duration value=\"" << decay_duration << "\" />" << endl;

  // XML for each LPCombFilter
  for( i=0; i<REVERB_NUM_COMB_FILTERS; i++ )
    {
      lpcfilter[i]->xml_print( xmlOutput );
    }

  apfilter->xml_print( xmlOutput );
  xmlOutput << "</reverb>" << endl;
}

void Reverb::xml_read(XmlReader::xmltag *reverbtag)
{

  // Sanity Check
  if(strcmp("reverb",reverbtag->name))
    {
      printf("Not a reverb tag!  This is a %s tag!\n",reverbtag->name);
      return;
    }

  char *value;
  if((value = reverbtag->findChildParamValue("gainDirect","value")) != 0)
      set_gainDirect(atof(value));

  if((value = reverbtag->findChildParamValue("gainReverb","value")) != 0)
      set_gainReverb(atof(value));

  if((value = reverbtag->findChildParamValue("allPassDelay","value")) != 0)
      set_allPassDelay(atof(value));

  if((value = reverbtag->findChildParamValue("decay_duration","value")) != 0)
      set_decay_duration(atof(value));

  XmlReader::xmltag *childtag;
  int lpIndex = 0;
  while((childtag = reverbtag->children->find("LPCombFilter")) != 0)
  {
    LPCombFilter* lp = new LPCombFilter();
    lp->xml_read(childtag);
    setLPComb(lpIndex, lp);
    lpIndex++;
  }

  while((childtag = reverbtag->children->find("AllPassFilter")) != 0)
  {
    AllPassFilter* apf = new AllPassFilter();
    apf->xml_read(childtag);
    setAllPass(apf);
  }
}

void Reverb::set_gainDirect(float new_gainDirect)
{
  // new_gainDirect = 0.0;
  // TODO: unused variable
  (void) new_gainDirect;
  gainDirect = 1; //new_gainDirect
}

void Reverb::set_gainReverb(float new_gainReverb)
{
  // new_gainReverb = 0.0;
  // TODO: unused variable
  (void) new_gainReverb;
  gainReverb = 1; //new_gainReverb;
}

void Reverb::set_allPassDelay(float new_allPassDelay)
{
  allPassDelay = new_allPassDelay;
}

void Reverb::set_decay_duration(float new_decay_duration)
{
  decay_duration = new_decay_duration;
}

void Reverb::setLPComb(int idx, LPCombFilter *f)
{
  lpcfilter[idx] = f;
}

void Reverb::setAllPass(AllPassFilter *f)
{
  apfilter = f;
}

#endif //MOSS_REVERB_CPP
