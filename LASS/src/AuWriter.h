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
//	AuWriter.h
//
//----------------------------------------------------------------------------//

#ifndef __AU_WRITER_H
#define __AU_WRITER_H

//----------------------------------------------------------------------------//
#include "MultiTrack.h"
#include "SoundSample.h"
#include "StandardHeaders.h"
#include "Track.h"

//----------------------------------------------------------------------------//

#define _16_BIT_LINEAR 3
#define _24_BIT_LINEAR 4
#define _32_BIT_LINEAR 5

/**
 *	Writes out SoundSample, Track, and MultiTrack objects to an AU file.
 *	3/24/2006: John Loucaides added 16/24/32 bit selection
 *	\author Braden Kowitz
 **/
class AuWriter {
public:
    /**
     *	This writes a SoundSample object out as an AU file.
     *	\param ss The SoundSample to write out
     *	\param filename The name of the file to write out to
     *	\param bits per sample set to either _16_BIT_LINEAR, _24_BIT_LINEAR, or
     *_32_BIT_LINEAR \retval true On success \retval false On failure
     **/
    static bool write(SoundSample& ss, string filename, int bits = _16_BIT_LINEAR);

    /**
     *	This writes a Track object out as an AU file.
     *	\param t The Track to write out
     *	\param filename The name of the file to write out to
     *	\param bits per sample set to either _16_BIT_LINEAR, _24_BIT_LINEAR, or
     *_32_BIT_LINEAR \retval true On success \retval false On failure
     **/
    static bool write(Track& t, string filename, int bits = _16_BIT_LINEAR);

    /**
     *	This writes a MultiTrack object out as an AU file.
     *   \note Some systems may not correctly read AU files with more than 2
     *tracks.  Try converting the file to another format if you have issues
     *(tool: SOX, sound exchange) \param mt The MultiTrack to write out \param
     *filename The name of the file to write out to \param bits per sample set
     *to either _16_BIT_LINEAR, _24_BIT_LINEAR, or _32_BIT_LINEAR \retval true
     *On success \retval false On failure
     **/
    static bool write(MultiTrack& mt, string filename, int bits = _16_BIT_LINEAR);

    /**
     *	This writes the individual Tracks in a MultiTrack to a set of
     *	filenames specified.
     *	\param mt The MultiTrack to write out
     *	\param filename A pointer to an array holding the names of the files to
     *write out to \retval true On success \retval false On failure
     **/
    static bool write_one_per_track(MultiTrack& mt, char* filename, ...);

private:
    /**
     *	This write the channels of a SoundSample out as an AU file
     *	\param channels A vector of SoundSamples to write out
     *	\param filename The name of the file to write out to
     *	\param bits per sample set to either _16_BIT_LINEAR, _24_BIT_LINEAR, or
     *_32_BIT_LINEAR \retval true On success \retval false On failure
     **/
    static bool write(vector<SoundSample*>& channels, string filename, int bits = _16_BIT_LINEAR);

    /**
     *	\todo Recursive inlining?  This is insane.  This needs to be reworked.
     **/
    void static inline WriteIntMsb(ostream& out, long l, int size);
};

//----------------------------------------------------------------------------//
#endif  //__AU_WRITER_H
