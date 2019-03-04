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
//	Spatializer.h	
//
//----------------------------------------------------------------------------//

#ifndef __SPATIALIZER_H
#define __SPATIALIZER_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "Types.h"
#include "MultiTrack.h"
#include "Track.h"

//----------------------------------------------------------------------------//

/**
*	A Spatializer is a definition of an object that
*	will spatalize (move around in space; this is a generalization
*	of panning) a single sound over multiple channels.
*	The default action of this spatializer will evenly distribute the
*	sound over the requested number of channels.
*	Each Spatializer must be dynamic enough to allow spatialization
*	to a number of different channels specified at run-time.
*	\author Braden Kowitz
**/
class Spatializer
{
public:
    virtual ~Spatializer();
    /**
    *	This will take a single Track object, and spatialize it
    *	to a MultiTrack object with numTracks tracks.
    *	The default behevior is to average the sound evenly
    *	accross all tracks.
    *	\param t The Track to spatialize
    *	\param numTracks The number of Tracks to create
    *	\return a MultiTrack
    **/
    virtual MultiTrack* spatialize(Track& t, int numTracks);

    /**
    *	This function creates an exact duplicate of this Spatializer.
    **/
    virtual Spatializer* clone();
    
    /**
    * This function prints some simple information for debugging purposes.
    **/
    virtual void print(){} //for debugging/verifying multipan clone function. --ming-ching May 6 2013

    /**
    *	\deprecated
    **/
    virtual void xml_print( ofstream& xmlOutput );
    
    

};


//----------------------------------------------------------------------------//
#endif //__SPATIALIZER_H
