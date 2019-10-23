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
//	MultiTrack.cpp
//
//----------------------------------------------------------------------------//

#ifndef __MULTI_TRACK_CPP
#define __MULTI_TRACK_CPP

//----------------------------------------------------------------------------//

#include "MultiTrack.h"

//----------------------------------------------------------------------------//

#include "Iterator.h"

//----------------------------------------------------------------------------//
MultiTrack::MultiTrack() {
    // nothing
}

//----------------------------------------------------------------------------//
MultiTrack::MultiTrack(int channels, m_sample_count_type numSamples, m_rate_type samplingRate) {
    for (int i = 0; i < channels; i++) {
        // create a blank track: (zero'd out)
        Track* t = new Track(numSamples, samplingRate, true);
        // add this blank track:
        add(t);
    }
}

//----------------------------------------------------------------------------//
MultiTrack::MultiTrack(MultiTrack& mt) : Collection<Track*>(mt) {
    // copy every track from mt to this object.
    Iterator<Track*> it = mt.iterator();
    while (it.hasNext()) add(new Track(*it.next()));
}

//----------------------------------------------------------------------------//
MultiTrack& MultiTrack::operator=(MultiTrack& mt) {
    if (this != &mt)  // beware of self assignment
    {
        // delete any tracks:
        Iterator<Track*> it = iterator();
        while (it.hasNext()) delete it.next();

        // clear this object
        clear();

        // copy every track from mt to this object.
        it = mt.iterator();
        while (it.hasNext()) add(new Track(*it.next()));
    }

    return *this;
}

//----------------------------------------------------------------------------//
MultiTrack::~MultiTrack() {
    // delete any tracks:
    Iterator<Track*> it = iterator();
    while (it.hasNext()) delete it.next();

    // clear this object
    clear();
}

//----------------------------------------------------------------------------//
void MultiTrack::composite(MultiTrack& mt, m_time_type startTime) {
    Iterator<Track*> it1 = iterator();
    Iterator<Track*> it2 = mt.iterator();

    while (it1.hasNext() && it2.hasNext()) {
        it1.next()->composite(*it2.next(), startTime);
    }

    // we should do some warning if sizes aren't equal and such.
}

//----------------------------------------------------------------------------//
#endif  //__MULTI_TRACK_CPP
