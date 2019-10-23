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
//	SoundSample.h
//
//----------------------------------------------------------------------------//

#ifndef __SOUND_SAMPLE_H
#define __SOUND_SAMPLE_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"
#include "Types.h"

//----------------------------------------------------------------------------//

/**
 *	This is a sample of sound with a fixed number of individual samples.
 *	For the sake of speed, this class has no range checking.
 *	Be careful when using it as it may cause a bus error.
 *	\author Braden Kowitz
 **/
class SoundSample {
public:
    // CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

    /**
     *	This is a constructor which takes the number of samples and the
     *	sampling rate. It will optionally zero all the data if requested.
     *	\param sampleCount The number of samples
     *	\param samplingRate The sampling rate
     *	\param zeroData Whether to zero the data (default is false)
     **/
    SoundSample(m_sample_count_type sampleCount, m_rate_type samplingRate = DEFAULT_SAMPLING_RATE,
                bool zeroData = false);

    /**
     *	This is a copy constructor.
     *	\param ss The SoundSample to copy
     **/
    SoundSample(const SoundSample& ss);

    /**
     *	This is an overloaded assignment operator.
     *	\param ss The SoundSample to assign
     **/
    SoundSample& operator=(const SoundSample& ss);

    /**
     *	This is the destructor.
     **/
    ~SoundSample();

    // PUBLIC METHODS

    /**
     *	Each sound sample stores its sampling rate.
     *	This is good information to know for composition and
     *	writing out to file.
     *	This function sets the sampling rate.
     *	\param samplingRate The new sampling rate
     **/
    void setSamplingRate(m_rate_type samplingRate);

    /**
     *	This function returns the sampling rate for this SoundSample.
     *	\return The current sampling rate
     **/
    m_rate_type getSamplingRate();

    /**
     *	Each SoundSample object is created with a specified number
     *	of samples.  This function returns that number.
     *	\return The current number of samples
     **/
    m_sample_count_type getSampleCount();

    /**
     *	This overloaded operator provides element access to this structure.
     *	Valid ranges are from 0 to (sampleCount-1).
     *	For efficency, this performs no range checking: be careful!
     *	\param index Which sample to access
     *	\note This should be inline, but I'm having problems making it work.
     **/
    m_sample_type& operator[](m_sample_count_type index);

    /**
     *	This function composites another SoundSample on top of this object.
     *	Both must have the same samplingRate.
     *	\param ss The SoundSample to composite
     *	\param startTime When to start applying the SoundSample
     **/
    void composite(SoundSample& ss, m_time_type startTime);

    /**
     *	This function scales every value in this SoundSample by a given factor.
     *	\param factor The scaling factor
     **/
    void scale(m_value_type factor);

private:
    m_rate_type samplingRate_;

    vector<m_sample_type> data_;
};

//----------------------------------------------------------------------------//
#endif  //__SOUND_SAMPLE_H
