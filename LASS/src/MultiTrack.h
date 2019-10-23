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
//	MultiTrack.h
//
//----------------------------------------------------------------------------//

#ifndef __MULTI_TRACK_H
#define __MULTI_TRACK_H

//----------------------------------------------------------------------------//

#include "Collection.h"
#include "Track.h"

//----------------------------------------------------------------------------//

/**
 *	A MultiTrack is basically a collection of track pointers.
 *	When the MultiTrack object is deleted, it deletes each of the tracks
 *	in the collection.  When it is copied or assigned, it copies the
 *	underlying objects. When tracks are removed or replaced, they
 *	are not deleted.
 *
 *	The idea is that copying entire MultiTrack objects should be covered,
 *	but editing elements in the MultiTrack objects should leave users
 *	to their own object management.  Just remember, a MultiTrack owns a
 *	pointer when it is added to it.
 *
 *	\author Braden Kowitz
 **/
class MultiTrack : public Collection<Track*> {
public:
    /**
     *	This is the default constructor which creates a MultiTrack
     *   with 0 tracks.
     **/
    MultiTrack();

    /**
     *	This constructor creates an empty MultiTrack.
     *	- This is good for composition.
     *	- It also zeros out data if requested.
     *	\param channels The number of channels
     *	\param numSamples The number of samples
     *	\param samplingRate The sampling rate
     **/
    MultiTrack(int channels, m_sample_count_type numSamples, m_rate_type samplingRate);

    /**
     *	This is a copy Constructor.
     *	\todo The argument should be const.
     *	\param mt The MultiTrack to copy
     **/
    MultiTrack(MultiTrack& mt);

    /**
     *	This is an overloaded assignment operator.
     *	\todo The argument should be const.
     *	\param mt The MultiTrack to assign.
     *	\return A MultiTrack
     **/
    MultiTrack& operator=(MultiTrack& mt);

    /**
     *	This is a destructor.
     **/
    ~MultiTrack();

    /**
     *	This composites another MultiTrack object on top of this one.
     *	- Any extra tracks are skipped (with warning).
     *	- startTime will offset the argument MultiTrack before compositing.
     **/
    void composite(MultiTrack& mt, m_time_type startTime = 0);
};

//----------------------------------------------------------------------------//
#endif  //__MULTI_TRACK_H
