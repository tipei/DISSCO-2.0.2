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
//	Track.h
//
//----------------------------------------------------------------------------//

#ifndef __TRACK_H
#define __TRACK_H

//----------------------------------------------------------------------------//

#include "SoundSample.h"

//----------------------------------------------------------------------------//

/**
 *	A track contains a SoundSample object with sound
 *	and an optional SoundSample member that contains Amplitude data.
 *	When the track object is deleted, it also deletes the wave and amp
 *	objects.
 *	\author Braden Kowitz
 **/
class Track {
public:
    // CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

    /**
     *	This is a constructor that creates a new Track object.
     *	The passed in wave and amp objects will be deleted
     *	when the Track object is deleted.  Take care.
     *	\param wave The SoundSample that represents the wave
     *	\param amp The SoundSample that represents the amplitude
     **/
    Track(SoundSample* wave, SoundSample* amp = 0);

    /**
     *	This is a constructor that creates a empty track with a set number
     *	of samples.  It optionally sets a preferred sample rate,
     *	and zeros out the data if needed.
     *	\param sampleCount The number of samples to create
     *	\param samplingRate The sampling rate
     *	\param zeroData Whether to zero the data (default is false)
     **/
    Track(m_sample_count_type sampleCount, m_rate_type samplingRate = DEFAULT_SAMPLING_RATE,
          bool zeroData = false);

    /**
     *	This is a copy constructor.
     *	\param t The track to copy
     **/
    Track(const Track& t);

    /**
     *	This is an overloaded assignment operator.
     *	\param t The Track to assign
     **/
    Track& operator=(const Track& t);

    /**
     *	This is the destructor.
     **/
    ~Track();

    // PUBLIC METHODS

    /**
     *	This function returns a reference to the wave object
     *	in this track.
     *	\return A SoundSample that describes the wave
     **/
    SoundSample& getWave();

    /**
     *	This function indicates whether this track has an amplitude
     *	SoundSample.
     *	\retval true There is an amplitude SoundSample
     *	\retval false There is not an amplitue SoundSample
     **/
    bool hasAmp();

    /**
     *	This function returns the amplitude SoundSample for this track.
     *	If this object does not have an amplitude track the function will
     *	return a new SoundSample object with zero entries.
     *   \return The SoundSample that represents the amplitude
     *	\note Always call hasAmp() first!
     *	\todo This causes a memory leak!
     **/
    SoundSample& getAmp();

    /**
     *	This function composites another wave on top of this wave.
     *	It also composites the amp values if both tracks contain amp.
     *	Specifing startTime (seconds) will shift the passed in track
     *	before composition.
     *	\param t The Track to composite
     *	\param startTime the time offset to start compositing (default is 0.0)
     **/
    void composite(Track& t, m_time_type startTime = 0.0);

    /**
     *	This function scales an entire track by a factor.
     *	\param factor The scaling factor
     **/
    void scale(m_value_type factor);

private:
    SoundSample* wave_;
    SoundSample* amp_;
};

//----------------------------------------------------------------------------//
#endif  //__TRACK_H
