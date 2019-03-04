//----------------------------------------------------------------------------//
//
//	MOSS - Musical Object-Oriented Sound Synthesis
//
//	Reverb.h
//
//	Written by Andrew Kurtz & Jim Lindstrom 2002
//
//      Edited to use Reverb Envelopes by Andy Latham, March 2005
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __MOSS_REVERB_H
#define __MOSS_REVERB_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "SoundSample.h"
#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"
#include "LPCombFilter.h"
#include "AllPassFilter.h"
#include "XmlReader.h"
#include "Envelope.h"

//----------------------------------------------------------------------------//

#define REVERB_NUM_COMB_FILTERS 6

/**
 * An artificial reverberator class.
 * The Reverb class implements an artificial reverberator, built on the model
 * in Moore's "Elements of Computer Music" book.  That model, in turn, is
 * based on Schroder's and Moorer's work.  
 *
 * In order to use this class you need to do three things
 *   1. Create a reverb object using the parameters you want.  There are a few
 *      different constructors to choose from, depending on how much control
 *      you want to have (and thus, how much you want to be burdened with 
 *      details).
 *   2. Call a Sound, Track, or MultiTrack object's use_reverb method and pass
 *      in your Reverb object as the parameter
 *   3. Render the corresponding object and let reverb run (the sound, track,
 *      or multitrack will call Reverb::do_reverb for you once you call its
 *      use_reverb member to setup a reverb object)
 *
 * Note: Reverb relies on some filters with feedback and delay, thus the filters
 * are partially dependent on old data sent to Reverb.  In order to clear out
 * the history in these filters, call Reverb::Reset before reusing a Reverb
 * object
 *
 **/
class Reverb 
{
 public:

  /** The default constructor.  
   *
   * It uses default values for everything
   * @param samplingRate the number of samples per second that the reverb 
   *  will assume on any data it is fed
   **/
  Reverb(m_rate_type samplingRate);
	
  /** simple constructor.
   *
   * @param room_size this is constructed to give some reasonable results 
   *  when you input room_size values between 0.0 and 1.0.  I found some 
   *  parameter sets that I felt approximated various familiar rooms 
   *  (lincoln hall, MB 1201, great hall, a large stadium, etc), and mapped
   *  these into a room_size space of 0.0 to 1.0.  Then I used gnumeric 
   *  (spreadsheet for linux, like excel) to find a linear regression to 
   *  get all the parameters simply in terms of room_size.  Thus a value of
   *  0.0 will have virtually no reverb.  A value of 1.0 will have a very 
   *  high degree of reverb.
   * @param samplingRate the number of samples per second that the reverb 
   *   will assume on any data it is fed
   **/
  Reverb(float room_size, m_rate_type samplingRate);
	
  /** next-most-advanaced constructor.
   *
   * @param reverbPercent this determines the mix (in the resultant sound)
   *  between the direct sound (the input sound) and the reverbed sound.
   *  0.0 is no reverb, all direct sound.  1.0 is all reverb, no direct
   *  sound.
   * @param hilow_spread in most rooms the low frequecy response will be higher
   *  than the high frequency response.  If we hold other parameters 
   *  constant, the math gives a range of possible ratios between low 
   *  frequence to high frequence response.  A value of 1.0 will choose 
   *  the maximum low-frequence response, holding the high-frequency
   *  response at a given.  A value of 0.0 will mean an even response
   *  across all frequencies.
   * @param gainAllPass Moore's reverberator has only one all-pass filter,
   *  and this is its gain.  The default value is 0.7.  Higher values
   *  (up to 1.0) cause more ringing at certain frequencies.  Lower
   *  values (down to 0.0) cause less ringing.  
   * @param delay this is the length (in seconds) of the first echo response.
   * @param samplingRate the number of samples per second that the reverb 
   *   will assume on any data it is fed
   **/
  Reverb(Envelope *percentReverb, float hilow_spread, float gainAllPass, 
	 float delay, m_rate_type samplingRate);
	
  /** advanced constructor.
   *
   * @param reverbPercent this determines the mix (in the resultant sound)
   *  between the direct sound (the input sound) and the reverbed sound.
   *  0.0 is no reverb, all direct sound.  1.0 is all reverb, no direct
   *  sound.
   * @param combGainList this is the address of an array of 6 floats that
   *  are the comb filter gains.  
   * @param lpGainList this is the address of an array of 6 floats that are
   *  the low-pass filter gains
   * @param gainAllPass Moore's reverberator has only one all-pass filter,
   *  and this is its gain.  The default value is 0.7.  Higher values
   *  (up to 1.0) cause more ringing at certain frequencies.  Lower
   *  values (down to 0.0) cause less ringing.  
   * @param delay the length (in seconds) of the first echo response.
   * @param samplingRate is the number of samples per second that the reverb 
   *   will assume on any data it is fed
   **/
  Reverb(Envelope *percentReverb, float *combGainList, float *lpGainList, 
	 float gainAllPass, float delay, m_rate_type samplingRate);
	
  /** common code used by all constructors.
   *
   * @param percentReverb this determines the mix (in the resultant sound)
   *  between the direct sound (the input sound) and the reverbed sound.
   *  0.0 is no reverb, all direct sound.  1.0 is all reverb, no direct
   *  sound.
   * @param combGainList this is the address of an array of 6 floats that
   *  are the comb filter gains.  
   * @param lpGainList this is the address of an array of 6 floats that are
   *  the low-pass filter gains
   * @param gainAllPass Moore's reverberator has only one all-pass filter,
   *  and this is its gain.  The default value is 0.7.  Higher values
   *  (up to 1.0) cause more ringing at certain frequencies.  Lower
   *  values (down to 0.0) cause less ringing.  
   * @param delay this is the length (in seconds) of the first echo response.
   * @param samplingRate is the number of samples per second that the reverb 
   *   will assume on any data it is fed
   **/
  void ConstructorCommon(Envelope *percentReverb, float *combGainList, 
			 float *lpGainList, float gainAllPass, float delay, 
			 m_rate_type samplingRate);
	
  /** destructor.
   *
   * this will destroy the filters and release any dynamically
   * allocated memory
   **/
  ~Reverb();

  /**
   * This method applies reverb to a single sample
   **/
  m_sample_type do_reverb(m_sample_type x_t, float x_value, Envelope *percentReverb);
  m_sample_type do_reverb(m_sample_type x_t, float x_value);

  /**
   * This method resets internal parameters in the reverberator
   * to an initial state. The Reverb class 
   * relies on some filters with feedback and delay, thus the filters
   * are partially dependent on old data sent to Reverb.  In order to clear out
   * the history in these filters, call Reverb::Reset before reusing a Reverb
   * object
   **/
  void reset(void);
  /**
   * This method applies reverb to a source MultiTrack, track by track.
   * It does so by decomposing the wave into Track objects (organized with
   * a Collection inside the MultiTrack object) and calling the
   * virtual function, do_reverb(SoundSample *inWave), for each Track.
   *
   * @return The original MultiTrack remains intact and untouched.  The return
   * is a reference to a NEW MultiTrack, which the caller is responsible for
   * deleting when done
   **/
  MultiTrack &do_reverb_MultiTrack(MultiTrack &inWave, Envelope *percentreverb);
  MultiTrack &do_reverb_MultiTrack(MultiTrack &inWave);

  /**
   * This method applies the reverb to a source Track by calling the
   * virtual function, do_reverb(SoundSample *inWave)
   *
   * @return The original Track remains intact and untouched.  The return
   * is a reference to a NEW Track, which the caller is responsible for
   * deleting when done
   **/
  Track &do_reverb_Track(Track &inWave, Envelope *percentreverb);
  Track &do_reverb_Track(Track &inWave);

  /**
   * This method calls do_reverb(m_sample_type x) for each sample in a track
   * and builds a new SoundSample on the fly.
   *
   * @return The original TraSoundSample remains intact and untouched.  The
   * return is a reference to a NEW SoundSample, which the caller is
   * responsible for deleting when done
   **/
  SoundSample *do_reverb_SoundSample(SoundSample *inWave, Envelope *percentreverb);
  SoundSample *do_reverb_SoundSample(SoundSample *inWave);
  /**
   * This retrieves the decay length.  It is intended to be used by Sound,
   * Score, and Partial objects to figure out how many samples to allocate
   * for a sound.  I.e., the object must add enough cycles to the end of a
   * sound for the delay to die out.  It is in units of seconds.
   *
   * @return decay time (in seconds)
   **/
  float getDecay(void);

  /**
   * This returns a pointer to the envelope used for reverb.  This is needed
   * because partial.cpp requires it.
  **/
  Envelope* getEnvelope(void);

  /**
   *   This outputs an XML representation of the object to STDOUT
   *
   **/
  void xml_print( ofstream& xmlOutput );

  // These functions are used by the XMLParser to reconstruct a Reverb object

  /**
   * Used by XML parsing code to reconstruct a Reverb object.  This sets the
   * reverb's internal member data and, in general, shouldn't be used
   * anywhere else.
   **/
  void set_gainDirect(float new_gainDirect);   //this should be an envelope
  /**
   * Used by XML parsing code to reconstruct a Reverb object.  This sets the
   * reverb's internal member data and, in general, shouldn't be used
   * anywhere else.
   **/
  void set_gainReverb(float new_gainReverb);   //this should also be an envelope
  /**
   * Used by XML parsing code to reconstruct a Reverb object.  This sets the
   * reverb's internal member data and, in general, shouldn't be used
   * anywhere else.
   **/
  void set_allPassDelay(float new_allPassDelay);
  /**
   * Used by XML parsing code to reconstruct a Reverb object.  This sets the
   * reverb's internal member data and, in general, shouldn't be used
   * anywhere else.
   **/
  void set_decay_duration(float new_decay_duration);
  /**
   * Used by XML parsing code to reconstruct a Reverb object.  This sets the
   * reverb's internal member data and, in general, shouldn't be used
   * anywhere else.
   **/
  void setLPComb(int idx, LPCombFilter *f);
  /**
   * Used by XML parsing code to reconstruct a Reverb object.  This sets the
   * reverb's internal member data and, in general, shouldn't be used
   * anywhere else.
   **/
  void setAllPass(AllPassFilter *f);
  void xml_read( XmlReader::xmltag *reverbtag);

 private:
  /**
   * Reconstruct an Amp SoundSample given the Wave SoundSample.  The input
   * SoundSample is unmodified.  The returned SoundSample has been allocated
   * with 'new' and is the caller's responsibility to 'delete' at some point.
   *
   * @param a pointer to the 'wave' component of a Track
   * @return a new SoundSample to be used as the 'amp' component of a Track
   **/
  SoundSample *constructAmp(SoundSample *wave);

  float gainDirect; //this line and the next should be envelopes too
  float gainReverb; 
  float allPassDelay;
  LPCombFilter *lpcfilter[REVERB_NUM_COMB_FILTERS];
  AllPassFilter *apfilter;
  float decay_duration;
  Envelope *percentReverb;
};

//----------------------------------------------------------------------------//
#endif //__MOSS_REVERB_H
