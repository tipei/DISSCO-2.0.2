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

#ifndef __TRACK_CPP
#define __TRACK_CPP

//----------------------------------------------------------------------------//

#include "Track.h"

//----------------------------------------------------------------------------//
// CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

//----------------------------------------------------------------------------//
Track::Track(SoundSample* wave, SoundSample* amp) : wave_(wave), amp_(amp) {}

//----------------------------------------------------------------------------//
Track::Track(m_sample_count_type sampleCount, m_rate_type samplingRate, bool zeroData)
    : wave_(new SoundSample(sampleCount, samplingRate, zeroData)),
      amp_(new SoundSample(sampleCount, samplingRate, zeroData)) {}

//----------------------------------------------------------------------------//
Track::Track(const Track& t) {
    wave_ = new SoundSample(*t.wave_);
    if (t.amp_ == 0) {
        amp_ = 0;
    } else {
        amp_ = new SoundSample(*t.amp_);
    }
}

//----------------------------------------------------------------------------//
Track& Track::operator=(const Track& t) {
    if (this != &t)  // beware self assignment
    {
        wave_ = new SoundSample(*t.wave_);
        if (t.amp_ == 0) {
            amp_ = 0;
        } else {
            amp_ = new SoundSample(*t.amp_);
        }
    }
    return *this;
}

//----------------------------------------------------------------------------//
Track::~Track() {
    delete wave_;
    if (amp_ != 0) delete amp_;
}

//----------------------------------------------------------------------------//
// PUBLIC METHODS

//----------------------------------------------------------------------------//
SoundSample& Track::getWave() { return *wave_; }

//----------------------------------------------------------------------------//
bool Track::hasAmp() { return (amp_ != 0); }

//----------------------------------------------------------------------------//
SoundSample& Track::getAmp() {
    if (amp_ != 0) {
        return *amp_;
    } else {
        cerr << "Track::getAmp() ERROR, no Amp SoundSample available." << endl;
        return (*new SoundSample(0));
    }
}

//----------------------------------------------------------------------------//
void Track::composite(Track& t, m_time_type startTime) {
    wave_->composite(*t.wave_, startTime);
    if (hasAmp() && t.hasAmp()) {
        amp_->composite(*t.amp_, startTime);
    }
}

//----------------------------------------------------------------------------//
void Track::scale(m_value_type factor) {
    wave_->scale(factor);
    if (amp_ != 0) amp_->scale(factor);
}

//----------------------------------------------------------------------------//
#endif  //__TRACK_CPP
