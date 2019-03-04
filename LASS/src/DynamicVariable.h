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
//	DynamicVariable.h
//
//----------------------------------------------------------------------------//

#ifndef __DYNAMIC_VARIABLE_H
#define __DYNAMIC_VARIABLE_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "Types.h"
#include "Iterator.h"
#include "XmlReader.h"
#include "Collection.h"

//----------------------------------------------------------------------------//

/**
*	An abstract definition of a simple Dynamic Variable.
*	This simply defines an m_value_type that changes over time.
*	Any implemention that derives from DynamicVariable can be
*	added.
*	\author Braden Kowitz
**/
class DynamicVariable
{
public:

    /**
    *	Default constructor for DynamicVariables.
    *	Sets the length to 1 second.
    *	Sets the rate to DEFAULT_SAMPLING_RATE
    **/
    DynamicVariable();

    /**
    *	Destructor.
    **/
    virtual ~DynamicVariable(){}

    /**
    *	Creates an exact duplicate of this variable.
    **/
    virtual DynamicVariable* clone() = 0;

    /**
    *	Returns an iterator object that will iterate over
    *	all values in this DynamicVariable.
    *	\return An iterator
    **/
    virtual Iterator<m_value_type> valueIterator() = 0;

    /**
    *	Scales this DynamicIterator by the given factor.
    *	\param factor The scaling factor
    **/
    virtual void scale(m_value_type factor) = 0;

    /**
    *	Returns the maximum value present in this variable.
    *   \return the max value
    **/
    virtual m_value_type getMaxValue() = 0;

    /**
    *	Sets the length of this dynamic variable.
    *	Will also affect the sample count for this variable.
    *	\param duration The duration
    **/
    void setDuration(m_time_type duration);
    
    /**
    *	Returns the Length in seconds of this Dynamic Variablei
    *	\return The duration in seconds
    **/
    m_time_type getDuration();
    
    /**
    *	Sets the sampling rate for this dynamic variable.
    *	Will also affect the sample count for this variable.
    *	\param rate The sampling rate
    **/
    void setSamplingRate(m_rate_type rate);
    
    /**
    *	Returns the sampling rate.
    *	\return The sampling rate
    **/
    m_rate_type getSamplingRate();

    /**
    *	Returns the number of samples at the current length and rate.
    *	\return number of samples
    **/
    m_sample_count_type getSampleCount();

    /**
    *	\deprecated
    *	This outputs an XML representation of the object to STDOUT
    **/
    virtual void xml_print( ofstream& xmlOutput, list<DynamicVariable*>& dynObjs ) = 0;

    /**
    *	\deprecated
    *	This outputs an XML representation of the object to STDOUT
    **/
    virtual void xml_print( ofstream& xmlOutput ) = 0;


    //static DynamicVariable* create_dv_from_xml(XmlReader::xmltag *dvtag, hash_map<long,DynamicVariable*> *dvHash);

    /**
    *	\deprecated
    *	This create a DynamicVariable from xml
    *	\param dvtag Some xml
    *	\return A DynamicVariable
    **/
    static DynamicVariable* create_dv_from_xml(XmlReader::xmltag *dvtag);

private:
    
    /**
    *	The duration
    **/
    m_time_type duration_;
    
    /**
    *	The sampling rate
    **/
    m_rate_type rate_;
};


//----------------------------------------------------------------------------//
#endif //__DYNAMIC_VARIABLE_H
