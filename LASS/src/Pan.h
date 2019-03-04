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
//	Pan.h	
//
//----------------------------------------------------------------------------//

#ifndef __PAN_H
#define __PAN_H

//----------------------------------------------------------------------------//

#include "Types.h"
#include "Spatializer.h"
#include "MultiTrack.h"
#include "Track.h"
#include "DynamicVariable.h"

//----------------------------------------------------------------------------//

/**
*	Pan is a simple Spatializer.
*	It implements simple panning accross a number of channels.
*	\todo Add destructor to Spatializer.
*	\author Braden Kowitz
**/
class Pan : public Spatializer
{
private:
    DynamicVariable* panVar_;
public:

    /**
    *	This is a simple constructor which creates a pan object around 
    *	a dynamic variable. The range should be [0,1] (else undefined).
    *	\param v The DynamicVariable
    **/
    Pan(DynamicVariable& v);
    
    /**
     * Destructor
    **/
    
    ~Pan();
    /**
    *	This returns an exact duplicate of this Pan object.
    *	\return The new Pan object
    **/
    Pan* clone();
    
    /**
    *	This sets the dynamic variable for a Pan object
    *	to something different than specified in the constructor.
    *	\param v The new DynamicVariable
    **/
    void set(DynamicVariable& v);

    /**
    *	This will return a new MultiTrack object with numTracks.
    *	The given track will be panned accross the channels.
    *	\param t The Track to pan
    *	\param numTracks The number of tracks
    *	\return A pointer to a new MultiTrack
    **/
    MultiTrack* spatialize(Track& t, int numTracks);

	/** 
	* \deprecated
	**/
	void xml_print( ofstream& xmlOutput );
};


//----------------------------------------------------------------------------//
#endif //__PAN_H
