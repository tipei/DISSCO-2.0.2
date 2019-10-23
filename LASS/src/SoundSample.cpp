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
//	SoundSample.cpp
//
//----------------------------------------------------------------------------//

#ifndef __SOUND_SAMPLE_CPP
#define __SOUND_SAMPLE_CPP

//----------------------------------------------------------------------------//

#include "SoundSample.h"

//----------------------------------------------------------------------------//

/**
 * Because m_time_type is a float, there can be a slight round-off error that
 * causes different calculations to produce different numbers of samples for
 * a given Track/SoundSample.  We would like to warn the user if he tries to
 * composite two sections of vastly different lengths, but we need to have
 * some acceptable error to account for round-off errors inherent in the
 * program and not caused by users
 **/
#define MIN_CLIP_WARNING 10

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

//----------------------------------------------------------------------------//
SoundSample::SoundSample(m_sample_count_type sampleCount, m_rate_type samplingRate, bool zeroData)
    : samplingRate_(samplingRate), data_(sampleCount) {
    if (zeroData) {
        for (m_sample_count_type s = 0; s < sampleCount; s++) {
            data_[s] = 0.0;
        }
    }
}

//----------------------------------------------------------------------------//
SoundSample::SoundSample(const SoundSample& ss)
    : samplingRate_(ss.samplingRate_), data_(ss.data_) {}

//----------------------------------------------------------------------------//
SoundSample& SoundSample::operator=(const SoundSample& ss) {
    if (this != &ss)  // beware self assignment
    {
        samplingRate_ = ss.samplingRate_;
        data_ = ss.data_;
    }
    return *this;
}

//----------------------------------------------------------------------------//
SoundSample::~SoundSample() {
    // no dynamic data
}

//----------------------------------------------------------------------------//
// PUBLIC METHODS

//----------------------------------------------------------------------------//
void SoundSample::setSamplingRate(m_rate_type samplingRate) { samplingRate_ = samplingRate; }

//----------------------------------------------------------------------------//
m_rate_type SoundSample::getSamplingRate() { return samplingRate_; }

//----------------------------------------------------------------------------//
m_sample_count_type SoundSample::getSampleCount() { return data_.size(); }

//----------------------------------------------------------------------------//
m_sample_type& SoundSample::operator[](m_sample_count_type index) { return data_[index]; }
//----------------------------------------------------------------------------//
void SoundSample::composite(SoundSample& ss, m_time_type startTime) {
    // make sure both sound samples have the same sampling rate.
    if (samplingRate_ != ss.samplingRate_) {
        cerr << "ERROR: SoundSample::composite() "
             << "two SoundSamples have different sampling rates. "
             << "Bailing out." << endl;
        return;
    }

    // find the number of samples to composite
    m_sample_count_type samplesToCopy = ss.data_.size();
    m_sample_count_type samplesToSkip = m_sample_count_type(startTime * float(samplingRate_));
    m_sample_count_type lengthNeeded = samplesToCopy + samplesToSkip;

    if (m_sample_count_type(data_.size()) < lengthNeeded) {
        if (m_sample_count_type(data_.size()) + MIN_CLIP_WARNING < lengthNeeded) {
            cerr << "WARNING: SoundSample::composite() "
                 << "sample being composited past end: "
                 << "clipping will occur." << endl;
        }

        samplesToCopy = m_sample_count_type(data_.size()) - samplesToSkip;

        if (samplesToCopy <= 0) return;
    }

    // copy the data:
    for (m_sample_count_type s = 0; s < samplesToCopy; s++) {
        data_[s + samplesToSkip] += ss.data_[s];
    }
}

//----------------------------------------------------------------------------//
// Iterator<m_sample_type> SoundSample::sampleIterator()
//{
//}

//----------------------------------------------------------------------------//
void SoundSample::scale(m_value_type factor) {
    for (int i = 0; i < (int)data_.size(); i++) data_[i] *= factor;
}

//----------------------------------------------------------------------------//
#endif  //__SOUND_SAMPLE_CPP
