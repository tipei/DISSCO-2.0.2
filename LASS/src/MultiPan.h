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
//	MultiPan.h	
//
//----------------------------------------------------------------------------//

#ifndef __MULTI_PAN_H
#define __MULTI_PAN_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "Types.h"
#include "Spatializer.h"
#include "MultiTrack.h"
#include "Track.h"
#include "Envelope.h"

//----------------------------------------------------------------------------//

/**
* MultiPan is a Spatializer.
* It is to be used instead of Pan when you need to spatialize a sound
* over more than 2 speakers.  To spatialize the sound over time you
* call addEntry or addEntryLocation one or more times and give a timestamp
* for each call.
* \author Jim Lindstrom
**/
class MultiPan : public Spatializer
{
public:

	/**
	 * This is a constructor.
	 * To use this constructor, you must call addEntry or
	 * addEntryLocation one or more times to set the spatialization
	 * data.  Use the other constructor if you want to instead
	 * pass in a list of Interpolators (one per channel).
	 *
	 * \param nChans The number of channels (speakers) to pan
	 *  across
	 **/
	MultiPan(int nChans);

	/**
	 * This is a constructor.
	 * To use this constructor, pass in a list of envs (one per channel).  
	 * If you want to call addEntry or addEntryLocation to set the 
	 * spatialization data, use the other constructor.
	 *
	 * \param nChans The number of channels (speakers) to pan
	 *  across
	 * \param List A vector of envs (nChan number of
	 *  envs, to be precise).  Each env will control the percent 
	 * that the corresponding speaker will  respond to input sound.
	 **/
	MultiPan(int nChans, vector<Envelope*> &List);

	/**
	 * This is the destructor.
	 **/
	~MultiPan();
    
	/**
	 * This returns an exact duplicate of this MultiPan object.
	 * \return An exact copy of this MultiPan object, with its
	 *  own env 
	 **/
	MultiPan* clone();
    
	/**
	 * Add another spatialization point to the dynamic variables (using
	 * the parameters as ratios of speaker volumes).  Pass in the
	 * time value, t, and a list of numbers (0.0 to 1.0 for each)
	 * that gives each speaker's response (as a percentage) to a
	 * given sound.
	 *
	 * \param t The time stamp (as a percent of total sound
	 *  length, from 0.0 to 1.0)
	 **/
	void addEntry(float t, ...);
    
	/** 
	 * Add another spatialization point to the dynamic variables using
	 * the parameters as a location within a circular
	 * array of speakers.  The value theta=0 means straight
	 * ahead, positive thetas to the left, negatives to
	 * the right.  The radius ranges from 0 to 1.
	 *
	 * \param t The time stamp (as a percent of total sound
	 *  length, from 0.0 to 1.0)
	 * \param theta The rotation angle (in radians) where the 
	 *  sound will appear to originate from.  An angle of zero
	 *  is directly in front of you.  Positive angles rotate to
	 *  your left, with positive pi (3.14...) being directly
	 *  behind you.  Negative angles rotate to your left and 
	 *  negative pi (-3.14) is another name for the angle directly
	 *  behind you.  For reference, pi/2 is straight left and
	 *  -pi/2 is straight right.  To use pi, math.h defines
	 *  the constant for you as 'M_PI'.  For instance, you could
	 *  use 'M_PI/2' to refer to the angle at your left.  
	 * \param radius The distance from which the sound will appear
	 *  to originate from.  These should range from 0.0 to 1.0.
	 **/
	void addEntryLocation(float t, float theta, float radius);

  void doneAddEntryLocation();
	/**
	 * This spatializes a track and returns a new MultiTrack 
	 * object with numTracks. The track will be panned accross 
	 * the channels by the passed in DynamicVariable.
	 * \param t A track to spatialize
	 * \param numTracks The number of tracks to spatialize to. (This
	 *  is redundant, but is kept for similarity of interface with
	 *  the regular pan object.  MultiPan already knows because you
	 *  have to pass in nChans in the constructor)
	 * \return the track, spatialized to 'numTracks' number of tracks
	 **/
	MultiTrack* spatialize(Track& t, int numTracks);

	/**
	* \deprecated
	**/
	void xml_print( ofstream& xmlOutput );
	
	
	void print();

private:
	/**
	 * This is a helper function used by addEntry and addEntryLocation to 
	 * add a point to an env.
	 **/
	void addEntryHelperFn(int envIdx, float t, float amp);
	
	/** Empty Constructor
	 * This constructor is only called by MultiPan::clone()
	 **/
	MultiPan();

	bool useEnvDirectly;
	vector<Envelope*> EnvList;
	vector<Collection<xy_point>*> xyCollectionsList;
	vector<Collection<envelope_segment>*> segCollectionsList;
	//not in used elsewhere in the program so I commented it out. --Ming-ching May 06, 2013
	//vector<int> nPoints;
	int n_channels;
};


//----------------------------------------------------------------------------//
#endif //__MULTI_PAN_H
