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
//	Constant.h
//
//----------------------------------------------------------------------------//

#ifndef __CONSTANT_H
#define __CONSTANT_H

//----------------------------------------------------------------------------//
#include "DynamicVariable.h"
#include "Iterator.h"
#include "StandardHeaders.h"
#include "Types.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//

/**
 *	A constant Dynamic variable.
 *	Kind of an oxymoron, but it's needed.
 *	\author Braden Kowitz
 **/
class Constant : public DynamicVariable {
public:
    /**
     *	Constructor that creates a constant at a specified value.
     **/
    Constant(m_value_type value = 0);

    /**
     *	Creates an exact copy of this object.
     *   \return pointer to the copy
     **/
    Constant* clone();
    ~Constant() { /*cout<<"constant destructor get called"<<endl;*/
    }

    /**
     *	Sets the value of this constant.
     *   \param value An m_value_type
     **/
    void setValue(m_value_type value);

    /**
     *	Returns the value of this constant.
     *   \return An m_value_type
     **/
    m_value_type getValue();

    /**
     *	Returns a Iterator(ConstantIterator) (private sub-class)
     *   \return an Iterator<m_value_type>
     **/
    Iterator<m_value_type> valueIterator();

    /**
     *	Scales the constant by this value.
     *   \param factor an m_value_type
     **/
    void scale(m_value_type factor);

    /**
     *	Simply return the constant value.
     *   \return an m_value_type
     **/
    m_value_type getMaxValue();

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
     *   This sets the duration, sampling rate, and value if they
     *   they are contained in the xml.
     *   \param xml The xml to read
     **/
    void xml_read(XmlReader::xmltag* xml);

private:
    m_value_type value_;

    /**
     *	This iterator iterates over a constant value.
     *	for a given amount of steps.
     **/
    class ConstantIterator : public AbstractIterator<m_value_type> {
    public:
        /**
         *	This iterator will iterate over 'value'
         *	count times.
         *	\param value An m_value_type that holds the value
         *	\param count An m_sample_count_type that holds the number of
         *times to iterate
         **/
        ConstantIterator(m_value_type value, m_sample_count_type count);

        /**
         *	Creates an exact copy of this iterator.
         *	\return Copy of this iterator
         **/
        ConstantIterator* clone();

        /**
         *	Indicates whether is another value to iterate over.
         *	\retval true If there is another value to iterate over
         *	\retval false If there is not another value to iterate over
         **/
        bool hasNext();

        /**
         *	Gets the next value in the iteration.
         *	\todo What to do on an out of bounds error?
         *	\return the next value in the iteration
         **/
        m_value_type& next();

    private:
        m_value_type value_;
        m_sample_count_type count_;
    };
};

//----------------------------------------------------------------------------//
#endif  //__CONSTANT_H
