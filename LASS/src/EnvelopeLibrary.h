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
//      Envelope_Library.h
//
//----------------------------------------------------------------------------//

#ifndef __ENVELOPE_LIBRARY_H
#define __ENVELOPE_LIBRARY_H

//----------------------------------------------------------------------------//

#include "Types.h"
#include "Collection.h"
#include "Envelope.h"


/**
 *	This class manages a collection of Envelopes, providing methods for 
 *	loading and saving the collection to a library file as well as
 *	functionality for adding to, retrieving from, updating the collection 
 *	with, and displaying individual Envelope items. The name Envelope
 *	is used informally to designate an Envelope.
 *
 *	\note All methods in this class that take an index argument begin with a first value of 1 for user ease, though the actual Collection indices begin at 0.
 *
 *	\author Philipp Fraund
 **/

class EnvelopeLibrary 
{
public:

    /**
     *  This is the Constructor.
     **/
    EnvelopeLibrary ();

    /**
     *  This is the destructor.  It deallocates all the Envelopes stored in the
     *  library.
     **/
    ~EnvelopeLibrary ();

    /**
     *  This is a copy constructor.
     *	\param lib The EnvelopeLibrary to copy
     *	\return A copy of lib 
     **/
    EnvelopeLibrary (EnvelopeLibrary & lib);

    /**
     *  This is an overloaded assignment operator for EvelopeLibraries
     **/
    EnvelopeLibrary & operator= (EnvelopeLibrary & lib);

    /**
     *  This function writes the Envelope library to a disk file.
     *	\param filename The name of the file to write to
     *	\retval true If successful
     *	\retval false If unsuccessful
     **/
    bool saveLibrary (char * filename);

    /**
     *  This function reads the Envelope library from a disk file.
     *	\param filename The name of the file to read from
     *	\return The number of entries in the library of -1 if unsucessful
     **/
    int loadLibrary (char * filename);

    /**
     *  This function reads an Envelope library in the new format
     *	\param filename The name of the file to read from
     *	\return The number of entries in the library of -1 if unsucessful
     **/
    int loadLibraryNewFormat (char * filename);

    /**
     *  This function returns a pointer to a new Envelope given an
     *  index to an existing EnvelopeLibrary, or NULL if index is out of range.
     *	\param index Which Envelope in the EnvelopeLibrary to find
     *	\return A pointer to an envelope
     **/
    Envelope * getEnvelope (int index);

    /**
     *  This function returns a reference to a new Envelope given an
     *  index to an existing EnvelopeLibrary, or NULL if index is out of range.
     *	\param index Which Envelope in the EnvelopeLibrary to find
     *	\return A const reference to the original envelope
     **/
    const Envelope& getEnvelopeRef (int index);

    /**
     *	This function adds an Envelope (passed by pointer) to the
     *  EnvelopeLibrary and returns the index at which it has been added.
     *	\param env The new Envelope to add to the EnvelopeLibrary
     *	\return The index of the new Envelope in the EnvelopeLibrary
     **/
    int addEnvelope (Envelope * env);

    /**
     *  This function adds a Collection of n+1 points and a Collection of n
     *  segments to the EnvelopeLibrary and returns the index at which it
     *  has been added.
     *	\param points A Collection of n+1 points
     *	\param segments A Collection of n segments
     *	\return The index where they were added
     **/
    int addEnvelope (Collection <xy_point> points,
		     Collection <envelope_segment> segments);

    /**
     *  This function updates the EnvelopeLibrary entry at the index with an
     *  Envelope (passed by pointer)
     *	\param index Which Envelope in the EnvelopeLibrary to update
     *	\param env A pointer to the new Envelope
     *	\retval true Successful
     *	\retval false Unsuccessful (index out of range)
     **/
    bool updateEnvelope (int index, Envelope * env);

    /**
     *  This function displays the indexed Envelope's contents
     *  using the standard output stream.
     *	\param index The Envelope in the EvelopeLibrary to show
     **/

    void showEnvelope (int index);

    /**
     *  This function returns the number of envelopes existing in the 
     *	EnvelopeLibrary
     *	\return The number of envelopes
     **/

    int size ();


private:

    /**
     *  This is a data structure to hold the library of envelopes.
     **/

    Collection <Envelope *> library;
};

//----------------------------------------------------------------------------//
#endif //__ENVELOPE_LIBRARY_H
