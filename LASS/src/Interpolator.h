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
//	Interpolator.h
//
//----------------------------------------------------------------------------//

#ifndef __INTERPOLATOR_H
#define __INTERPOLATOR_H

//----------------------------------------------------------------------------//
#include "Collection.h"
#include "DynamicVariable.h"
#include "Iterator.h"
#include "StandardHeaders.h"
#include "Types.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//
/**
 *  This is a single entry for an interpolator.
 *
 *	\author Zeke McKinney
 *	\author Braden Kowitz
 *	\author Philipp Fraund
 **/

class InterpolatorEntry {
public:
    /**
     *   This is a constructor that makes an entry with no parameters.
     **/
    InterpolatorEntry() {}

    /**
     *	This is a constructor for convenience.
     **/
    InterpolatorEntry(m_time_type t, m_value_type v) : time_(t), value_(v) {}

    // M E M B E R   V A R I A B L E S

    /**
     *	The time for this entry.
     **/
    m_time_type time_;

    /**
     *	The value for this entry.
     **/
    m_value_type value_;

    // C O M P A R A T O R   O P E R A T O R S
    /**
     * This is an overloaded  less than operator
     **/
    bool operator<(const InterpolatorEntry& ie) const { return (time_ < ie.time_); }
    /**
     * This is an overloaded equal to operator
     **/
    bool operator==(const InterpolatorEntry& ie) const { return (time_ == ie.time_); }
    /**
     * This is an overloaded  greater than operator
     **/
    bool operator>(const InterpolatorEntry& ie) const { return (time_ > ie.time_); }
};

/**
 *  This is an abstract interpolator class from which more specific
 *interpolators (DynamicVariables used for interpolating between points) can be
 *derived.
 *
 *  See InterpolatorTypes.h for interpolators.
 *
 *	\author Zeke McKinney
 *	\author Braden Kowitz
 *	\author Philipp Fraund
 **/
class Interpolator : public DynamicVariable, public Collection<InterpolatorEntry> {
public:
    /**
     *	This is the default constructor.
     **/
    Interpolator();

    /**
     *	This function creates an exact copy of this object.
     *	\return A new Interpolator
     **/
    virtual Interpolator* clone() = 0;

    /**
     *	\todo We should have destructor and assignment and copy.
     **/

    /**
     *	This Adds an entry to this variable.
     *	This is added for convenience - this way, a user does not
     *	have to deal with InterpolatorEntry objects
     *	when simply creating an Interpolator.
     *	\param time The time of the new entry
     *	\param value The value of the new entry
     **/
    void addEntry(m_time_type time, m_value_type value);

    /**
     *	This function returns an InterpolatorIterator.
     *	\return An iterator
     **/
    virtual Iterator<m_value_type> valueIterator() = 0;

    /**
     *	This function scales every entry's value by this factor.
     *	\param scale The factor by which to scale
     **/
    void scale(m_value_type scale);

    /**
     *	This function returns the maximum value of all the entries.
     *	\return The maximum value
     **/
    m_value_type getMaxValue();

    /**
     *  This function returns the type of interpolator for use in
     *	reconstructing envelope parameters for EnvelopeLibrary from a
     *  DynamicVariableSequence. Virtual function implemented in
     *	InterpolatorTypes.
     *  \return Interpolator type
     **/

    virtual interpolation_type getType() = 0;

    /**
     *	\deprecated
     *   This outputs an XML representation of the object to STDOUT
     **/
    void xml_print(ofstream& xmlOutput, list<DynamicVariable*>& dynObjs);

    /**
     *	\deprecated
     **/
    void xml_print(ofstream& xmlOutput);

    /**
     *	\deprecated
     **/
    void xml_read(XmlReader::xmltag* soundtag);
};

//----------------------------------------------------------------------------//
#endif  //__INTERPOLATOR_H
