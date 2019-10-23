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
//	Partial.h
//
//----------------------------------------------------------------------------//

#ifndef __PARTIAL_H
#define __PARTIAL_H

//----------------------------------------------------------------------------//
#include "ParameterLib.h"
#include "Reverb.h"
#include "StandardHeaders.h"
#include "Track.h"
#include "Types.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//
/**
 *	@file Partial.h
 *	Defines a Partial object, as well as the static and dynamic parameters
 *	that pertain to said object.
 **/

/**
 *	Defines the static parameters that can be set for a Partial object.
 *	- RELATIVE_AMPLITUDE
 *		- Used by Loudness to balance the amplitude of partials.
 *		- Set these values in any range you like.
 *	- PARTIAL_NUM
 *		- Defines the number partial.
 *		- 0 is the lowest partial.
 *		- Currently only used for Frequency Deviation calculations.
 *	- WAVE_TYPE
 *	    - Specify type of wave
 **/
enum PartialStaticParam { RELATIVE_AMPLITUDE, PARTIAL_NUM, WAVE_TYPE };

/**
 *	Defines the Dynamic parameters that can be set for a Partial object.
 *	- FREQUENCY
 *		- The pitch at which a partial is heard.
 *		- Set in Hz. (no bounds checking)
 *	- WAVE_SHAPE
 *		- How the partial changes it's amplitude over time.
 *		- Envelope should start and end at y=0; otherwise "clicks" and
 *	           "pops" will be created
 *	- TREMOLO_AMP
 *		- The amplitude of tremolo (size of the effect).
 *		- Given as a scaling factor to amplitude.
 *	- TREMOLO_RATE
 *		- Given in Hz. (see vibrato rate)
 *	- VIBRATO_AMP
 *		- The amplitude of vibrato (size of the effect).
 *		- Given as a scaling factor to frequency.
 *	- VIBRATO_RATE
 *		- Given in Hz. (6 Hz is a "normal" vibrato)
 *	- FREQUENCY_DEVIATION ****Commmented out (i.e. not used anywhere, but
 *can be put back in)*****
 *		- Specifies how randomly scaled the frequencies
 *		  of this partial will be.
 *		- Range [0 - 1]
 *		- Could be specified as a value or as an envelope which will
 *		  affect the frequency.  Exactly the same effect can be obtained
 *		  by using GLISSANDO_ENV or DETUNING_ENV. See comment for
 *		  glissando envelope.
 *	- LOUDNESS_SCALAR
 *		- Not to be set by users.
 *		- This variable is set by the Loudness routines.
 *		- The scaling factor is then taken into account at every sample.
 *	- GLISSANDO_ENV *******Commented Out (i.e. not used anywhere, but can be
 *put back in) *****
 *		- This is a glissando envelope.  It is multiplied by the
 *		- frequency at every point.  Thus, if you leave it at 1.0 (The
 *		  default, it will not affect anything).  If you leave it at 1.0
 *		  for most of the sound, then interpolate it down to 0.5, the
 *		  partial will trail off (by an octave down) at the end of the
 *		  sound.
 *		- The y values for this envelope can be any positive number.  a
 *		  value of 0 will kill the sound (0 frequency), and too high of
 *		  a value could make the frequency inaudible or above Nyquist
 *		  (no bounds checking).
 *	- DETUNING_ENV ******** (Commmented Out) *********
 *		- used to gradually tune or detune a partial.  It's an envelope,
 *		  and acts just like a glissando envelope, but more general.
 *       - AMPTRANS_AMP_ENV
 *               - used to control the value of amplitude transients
 *               - the value of the amp envelope is a maximum transient modifier
 *               - this value is multiplied by a random percentage and then
 *                 used to modify the amplitude as follows:
 *                  If the value after the percentage is applied is 0.7, then
 *		   the amplitude will be 1.7 or 0.3 times its original value.
 *       - AMPTRANS_RATE_ENV
 *               - used to control the rate of amplitude transients
 *               - the value of the rate envelope is the percentage chance of a
 *                 transient occuring at that particular time
 *       - FREQTRANS_AMP_ENV
 *               - used to control the amplitude of frequency transients
 *               - see AMPTRANS_AMP_ENV for an explanation
 *       - FREQTRANS_RATE_ENV
 *               - used to control the rate of frequency transients
 *               - see AMPTRANS_RATE_ENV for an explanation
 *       - AMPTRANS_WIDTH
 *               - the width of any amplitude transient in samples
 *               - defaults to 1103, or 0.025 seconds.
 *       - FREQTRANS_WIDTH
 *               -the width of any frequency transient in samples
 *               - defaults to 1103, or 0.025 seconds.
 **/
enum PartialDynamicParam {
    FREQUENCY,
    WAVE_SHAPE,
    TREMOLO_AMP,
    TREMOLO_RATE,
    VIBRATO_AMP,
    VIBRATO_RATE,
    PHASE,
    // FREQUENCY_DEVIATION,
    LOUDNESS_SCALAR,
    // GLISSANDO_ENV,
    FREQ_ENV,
    DETUNING_ENV,
    AMPTRANS_AMP_ENV,
    AMPTRANS_RATE_ENV,
    FREQTRANS_AMP_ENV,
    FREQTRANS_RATE_ENV,
    AMPTRANS_WIDTH,
    FREQTRANS_WIDTH
};

// the loudness routines will look at RELATIVE_AMPLITUDE, FREQUENCY,
// and WAVE_SHAPE, then fill out LOUDNES_SCALAR.

/**
 *	A partial is a building block of all sounds.  It is a simple Sin wave
 *	that is manipulated by several dynamic and static parameters.
 **/
class Partial : public ParameterLib<PartialStaticParam, PartialDynamicParam> {
public:
    /**
     * Default constructor that sets a few basic parameters
     * - RELATIVE_AMPLITUDE = 1.0
     * - PARTIAL_NUM = 1.0
     * - WAVE_SHAPE = 1.0
     * - FREQUENCY = 440
     * - LOUDNESS_SCALAR = 1.0
     * - All trans envelopes = 0
     * - Both TRANS_WIDTH = 1103
     **/
    Partial();

    /**
     * This returns a Track object of the rendered partial.
     * The object must be deleted by the user calling the function.
     * \param sampleCount The number of samples
     * \param duration The duration
     * \param samplingRate The sampling rate
     * \return A Track
     **/
    Track *render(m_sample_count_type sampleCount, m_time_type duration,
                  m_rate_type samplingRate = DEFAULT_SAMPLING_RATE);

    /**
     *   Use this object to perform reverb in the render() method
     *   \param newReverbObj A pointer to a Reverb object
     **/
    void use_reverb(Reverb *newReverbObj);

    /**
     *	\deprecated
     *   This outputs an XML representation of the object to STDOUT
     **/
    void xml_print(ofstream &xmlOutput, list<Reverb *> &revObjs, list<DynamicVariable *> &dynObjs);
    /**
     * \deprecated
     **/
    void xml_read(XmlReader::xmltag *partialtag, DISSCO_HASHMAP<long, Reverb *> *reverbHash,
                  DISSCO_HASHMAP<long, DynamicVariable *> *dvHash);

    /**
     *  This returns the total length (in seconds) of the partial.
     *  \param dryDuration The input 'dryDuration' is the duration
     *		of the sound without any effects added on.
     *  \return The total length of time returned includes
     *  		time for the reverb to die out.
     **/
    m_time_type getTotalDuration(m_time_type dryDuration);

private:
    /**
     * \deprecated
     *  Auxillary function to assist in loading dv's from XML
     **/
    void auxLoadParam(enum PartialDynamicParam param, XmlReader::xmltag *tag,
                      DISSCO_HASHMAP<long, DynamicVariable *> *dvHash);

    /**
     * This is phase-modulation. It basically does an inline
     * modulus 1 on a float value.
     **/
    m_value_type pmod(m_value_type num);

    /** A Reverb object **/
    Reverb *reverbObj;
};

//----------------------------------------------------------------------------//
#endif  //__PARTIAL_H
