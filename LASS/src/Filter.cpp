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
//	Filter.cpp
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __FILTER_CPP
#define __FILTER_CPP

//----------------------------------------------------------------------------//

#include "Filter.h"

#include "Collection.h"
#include "MultiTrack.h"
#include "SoundSample.h"
#include "Track.h"

//----------------------------------------------------------------------------//
MultiTrack &Filter::do_filter_MultiTrack(MultiTrack &inWave) {
    MultiTrack *newMultiTrack;
    Track *newTrack;
    SoundSample *channel;

    // create the new MultiTrack
    newMultiTrack = new MultiTrack();

    // iterate over all individual tracks
    // for each track:
    //  grab the SoundSample from inside the Track
    //  filter the SoundSample, which returns a NEW SoundSample
    //  reset the filter so that it can be used fresh next time
    //  create a new track based on the returned, filtered SoundSample
    //  add this new track to the output MultiTrack
    Iterator<Track *> it = inWave.iterator();
    while (it.hasNext()) {
        channel = &it.next()->getWave();
        newTrack = new Track(do_filter_SoundSample(channel));
        reset();
        newMultiTrack->add(newTrack);
    }

    return *newMultiTrack;
}

//----------------------------------------------------------------------------//
Track &Filter::do_filter_Track(Track &inWave) {
    Track *newTrack;
    SoundSample *channel;

    // grab the SoundSample from inside the Track
    // filter the SoundSample, which returns a NEW SoundSample
    // create a new track based on the returned, filtered SoundSample
    channel = &inWave.getWave();
    newTrack = new Track(do_filter_SoundSample(channel));

    return *newTrack;
}

//----------------------------------------------------------------------------//
SoundSample *Filter::do_filter_SoundSample(SoundSample *inWave) {
    int i;
    SoundSample *outWave;

    // create new SoundSample
    outWave = new SoundSample(inWave->getSampleCount(), inWave->getSamplingRate());

    for (i = 0; i < inWave->getSampleCount(); i++) {
        (*outWave)[i] = do_filter((*inWave)[i]);
        //	outWave->operator[](i) = do_filter(inWave->operator[](i));
    }

    return outWave;
}

//----------------------------------------------------------------------------//
#endif  //__FILTER_CPP
