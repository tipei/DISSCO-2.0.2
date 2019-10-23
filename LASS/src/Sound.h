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
//	Sound.h
//
//----------------------------------------------------------------------------//

#ifndef __SOUND_H
#define __SOUND_H

//----------------------------------------------------------------------------//
#include "Collection.h"
#include "DynamicVariable.h"
#include "Filter.h"
#include "InterpolatorTypes.h"
#include "MultiTrack.h"
#include "Partial.h"
#include "Reverb.h"
#include "Spatializer.h"
#include "StandardHeaders.h"
#include "Types.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//

/**
 *	@file Sound.h
 *	Defines a Sound object, as well as the static and dynamic parameters
 *	that pertain to said object.
 **/

/**
 *	\var SoundStaticParam START_TIME
 *		- When placed in a Score, the sound will start at this time.
 **/

/**
 *	\var SoundStaticParam DURATION
 *		- The sound lasts this long (in seconds)
 **/

/**
 *	\var SoundStaticParam LOUDNESS
 *		- Describes how loud the sound should be perceived
 *		  at its loudest point.
 *		- Given in Sones, the valid range is [0,255]
 **/

/**
 *	\var SoundStaticParam LOUDNESS_RATE
 *		- Loudness does not have to be calculated EVERY sample.
 *		- This specifies how often loudness is calculated.
 *		- the default is 10Hz.
 **/

/**
 *	\var SoundStaticParam DETUNE_SPREAD
 *		- Detuning describes the effect of letting all partials
 *		  exponentially converge from random points to their intended
 *		  frequencies, or to diverge in the reverse fashion
 *		- Detuning spread refers to the the variance in frequences
 *		  at the divergent end of the sound (the beginning for
 *		  convergence (tuning), the end for divergence (detuning))
 *		- A value of 0.0 effectively disables detuning
 *		- A value of, say, 0.3 causes the detuned frequencies to fall
 *		  within the range [(1.0-0.3)*Freq, (1.0+0.3)*Freq].  In other
 *		  words, the SPREAD value is a percent that corresponds to a
 *		  range in which the partial will (randomly) fall at the max.
 *		  detuning portion of the sound.
 **/

/**
 *	\var SoundStaticParam DETUNE_DIRECTION
 *		- -1.0 means do detuning (divergence)
 *		- +1.0 means do tuning (convergence)
 **/

/**
 *	\var SoundStaticParam DETUNE_VELOCITY
 *		- Values over [-1.0, +1.0]
 *		- For VELOCITY=0.5, the transition will be linear from start
 *		  to end.  For Velocity = -1.0, the transition will be nearly
 *		  instantaneous and immediate (and exponentially
 *		  interpolated).  For velocity +1.0, the transition will be
 *		  instantaneous, but will occur at the extreme end of the
 *		  sound (And will be exponentially interpolated)
 *		- If y is the amount of tuning and x is time, think of -1.0 as
 *		  a vertical line on the left (at the beginning) and then a flat
 *		  horizontal line.  For +1.0, the horizontal line is at the
 *		  beginning (on the left), and there is a sharp transition at
 *		  the right end (at the end).
 **/

/**
 *	\var SoundStaticParam DETUNE_FUNDAMENTAL
 *		- Positive values = TRUE (tune/detune the fundamental)
 *		- Negative Values = FALSE (do not tune/detune ...)
 **/

enum SoundStaticParam {
    START_TIME,
    DURATION,
    LOUDNESS,
    LOUDNESS_RATE,
    DETUNE_SPREAD,
    DETUNE_DIRECTION,
    DETUNE_VELOCITY,
    DETUNE_FUNDAMENTAL
};

/**
 *	\enum SoundDynamicParam
 *	Defines the DynamicParameters that can be set for a Sound object.
 **/
enum SoundDynamicParam {};

/**
 *	Conceptually, A Sound is made up from a set of partials.
 *	\author Braden Kowitz
 **/
class Sound : public Collection<Partial>, public ParameterLib<SoundStaticParam, SoundDynamicParam> {
private:
    Spatializer* spatializer_;

public:
    /**
     *	This is a default constructor that sets a few basic parameters
     *	- DURATION = 1.0
     *	- START_TIME = 0.0
     *	- LOUDNESS = 100 (out of 255) Sones
     *	- LOUDNESS_RATE = 10 Hz
     **/
    Sound();

    /**
     *	This is a constructor that creates a Sound object
     *	with proper partials already created.
     *	\param numPartials The number of partials in the Sound
     *	\param baseFreq The base freqency of the Sound
     **/
    Sound(int numPartials, m_value_type baseFreq);

    /**
     * Destructor
     *
     */
    ~Sound();
    /**
     *	This function iterates through the partials, calling setParam on them.
     *	\param p The PartialStaticParam to set
     *	\param v The value to set the parameter to
     **/
    void setPartialParam(PartialStaticParam p, m_value_type v);

    /**
     *	This function iterates through the partials, calling setParam on them.
     *	\param p The PartialDynamicParam to set
     *	\param v The value to set the parameter to
     *	\todo Change FREQUENCY param like old MOSS
     **/
    void setPartialParam(PartialDynamicParam p, DynamicVariable& v);

    /**
     *	This function iterates through the partials, calling setParam on them.
     *	\param p The PartialDynamicParam to set
     *	\param v The value to set the parameter to
     **/
    void setPartialParam(PartialDynamicParam p, m_value_type v);

    /**
     *	This function iterates through the partials, calling getParam on them.
     *	\param p The PartialDynamicParam to get the envelope which was used on
     *it
     **/
    void getPartialParamEnv(PartialDynamicParam p);

    /**
     *	This returns a MultiTrack object of the rendered partial.
     *	\param numChannels The number of channels in the MultiTrack
     *	\param samplingRate The sampling rate
     *	\return A MultiTrack
     *	\note The object must be deleted by the user calling the function.
     **/
    MultiTrack* render(int numChannels, m_rate_type samplingRate = DEFAULT_SAMPLING_RATE);

    /**
     *	This function sets the Spatializer used for this sound.
     *	The given parameter is copied into this object's memory.
     *	\param s The Spatializer to use
     **/
    void setSpatializer(Spatializer& s);

    /**
     *   This function performs filter in the render() method
     *	\param newFilterbObj The Filter object
     **/
    void use_filter(Filter* newFilterObj);

    /**
     *   This function performs reverb in the render() method
     *	\param newReverbObj The Reverb object
     **/
    void use_reverb(Reverb* newReverbObj);

    /**
     *	\deprecated
     *   This outputs an XML representation of the object to STDOUT
     **/
    void xml_print(ofstream& xmlOutput, list<Reverb*>& revObjs, list<DynamicVariable*>& dynObjs);
    /**
     *	\deprecated
     **/
    void xml_read(XmlReader::xmltag* soundtag, DISSCO_HASHMAP<long, Reverb*>* reverbHash,
                  DISSCO_HASHMAP<long, DynamicVariable*>* dvHash);

    /**
     *  This returns the total duration of the sound.  If there is no reverb
     *  on this sound or any of its partials, this will return exactly
     *  the value of getParam(DURATION).  If any partial has reverb,
     *  though, that partial will need extra time for the reverb to
     *  die out.  Likewise, if the sound has reverb, that will also
     *  take time to die out.
     *	\return The total duration of the sound including reverb
     **/
    m_time_type getTotalDuration(void);

private:
    /**
     *	This function creates a detuning envelope.
     *	\param env The envelope
     **/
    void setup_detuning_env(ExponentialInterpolator* env);

    /**
     *	Pointer to a filter object that will apply filter to this sound
     **/
    Filter* filterObj;

    /**
     *	Pointer to a reverb object that will apply reverb to this sound
     **/
    Reverb* reverbObj;
};

//----------------------------------------------------------------------------//
#endif  //__SOUND_H
