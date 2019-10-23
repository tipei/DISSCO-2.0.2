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
// EnvelopeIterator.h
//
//----------------------------------------------------------------------------//

#ifndef __ENVELOPE_ITERATOR_H
#define __ENVELOPE_ITERATOR_H

//----------------------------------------------------------------------------//
#include "AbstractIterator.h"
#include "Envelope.h"
#include "StandardHeaders.h"
#include "Types.h"

//----------------------------------------------------------------------------//

/**
 *   An iterator that interpolates over a Envelope.  Since a
 *   Envelope acts like a single DynamicVariable, when we want
 *   to use it as such, we need a way to iterate through its value.
 *   That's what this does.
 *   \author Michael Aikins
 **/
class EnvelopeIterator : public AbstractIterator<m_value_type> {
public:
    /**
     *   This is the constructor which initalizes some basic values.
     *	\param interpolators
     **/
    EnvelopeIterator(Collection<Interpolator*> interpolators);

    /**
     *   This is the Destructor.
     **/
    ~EnvelopeIterator();

    // A B S T R A C T   I T E R A T O R   F U N C T I O N S

    /**
     *	This makes a copy of this iterator.
     *   \returns A copy of this iterator.
     **/
    EnvelopeIterator* clone();

    /**
     *	Indicate whether the iterator has another value to return.
     *	\retval true If there is another value to return
     *	\retval false If there is not another value to return
     **/
    bool hasNext();

    /**
     *   Gets the next value in the iteration.
     *   \note Because this returns a reference type, value_ can be changed by
     *the caller.  Steps should be taken to prevent this (with a pass-to-caller
     *member variable perhaps). \return a reference to the next value in the
     *iteration
     **/
    m_value_type& next();

private:
    /**
     *	This is a collection that holds the interpolators for all the segments.
     **/
    Collection<Interpolator*>* interpolators_;

    /**
     *	This is the current segment's index.
     **/
    int iSegmentIndex_;

    /**
     *	This is the current DV iterator.
     **/
    Iterator<m_value_type>* currentIterator_;

    /**
     *	This is the current value.
     **/
    m_value_type currentValue_;

    /**
     *	This indicates whether we have more values.
     **/
    bool moreValues_;

    long tempCounter_;
};

//----------------------------------------------------------------------------//
#endif  //__ENVELOPE_ITERATOR_H
