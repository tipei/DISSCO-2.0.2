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
//	InterpolatorIterator.h
//
//----------------------------------------------------------------------------//

#ifndef __INTERPOLATOR_ITERATOR_H
#define __INTERPOLATOR_ITERATOR_H

#include "AbstractIterator.h"
#include "StandardHeaders.h"
#include "Types.h"

/**
 *	This is an iterator that interpolates between set values.
 *	Users append entries of from-value to-value and num-steps.
 *	The iterator will then provide iteratation over those ranges.
 *	\author Zeke McKinney
 **/
class InterpolatorIterator : public AbstractIterator<m_value_type> {
    // private:
protected:
    /**
     *	An entry for this iterator.
     *	This is kept private so that it may change in the future without
     *	difficulty.
     **/
    class Entry {
    public:
        /**
         *	This is a constructor for an interpolator iterator entry.
         *	\param t_from The start time
         *	\param t_to The end time
         *	\param v_from The beginning value
         *	\param v_to The end value
         *	\param steps the number of steps to take
         **/
        Entry(m_time_type t_from, m_time_type t_to, m_value_type v_from, m_value_type v_to,
              m_sample_count_type steps)
            : t_from_(t_from), t_to_(t_to), v_from_(v_from), v_to_(v_to), steps_(steps) {}
        /// time starts at this
        m_time_type t_from_;
        /// time ends at this
        m_time_type t_to_;
        /// value starts at this
        m_value_type v_from_;
        /// value ends at this
        m_value_type v_to_;
        /// number of steps to take inbetween
        m_sample_count_type steps_;
    };

    /**
     *	This class works by keeping entries in a queue.
     **/
    list<Entry> queue_;

    /**
     *	The number of steps left until the next entry needs
     *	to be accessed.
     **/
    m_sample_count_type stepsLeft_;

    /**
     *	The current value.
     **/
    m_value_type value_;

    /**
     *	The amount the value changed each call to next().
     **/
    m_value_type delta_;

public:
    /**
     *	This is a constructor which initalizes some basic values.
     **/
    InterpolatorIterator();

    // no dynamic data, so destructor, copy constructor,
    // and assignment operator are not needed.

    // C R E A T I O N  F U N C T I O N S

    /**
     *	This defines a linear segment to append to this iterator.
     *	\note This interface makes discontinuities possible.
     *   \todo Perhaps for future versions: void append(const
     *InterpolatorIterator&); \param t_from The start time \param t_to The end
     *time \param v_from The beginning value \param v_to The ending value \param
     *steps The number of steps to take
     **/
    void append(m_time_type t_from, m_time_type t_to, m_value_type v_from, m_value_type v_to,
                m_sample_count_type steps);

    // A B S T R A C T   I T E R A T O R   F U N C T I O N S

    /**
     *	This makes a copy of the this iterator.
     *	\return A copy of the iterator
     **/
    virtual InterpolatorIterator* clone() = 0;

    /**
     *	Indicates whether there is another value to get.
     *	\retval true If there is another value to return.
     *	\retval false If there is no other value to return.
     **/
    bool hasNext();

    /**
     *	Returns the next value in the iteration.
     *	\note  Because this returns a reference type, value_ can be changed by
     *the caller.  Steps should be taken to prevent this (with a pass-to-caller
     *member variable perhaps)
     **/
    virtual m_value_type& next() = 0;
};

/**
 *  This is an iterator that will iterate over values in a LinearInterpolator.
 **/
class LinearInterpolatorIterator : public InterpolatorIterator {
public:
    /// constructor for iterator
    LinearInterpolatorIterator();

    /// clone for iterator
    LinearInterpolatorIterator* clone();

    /// get next value function
    m_value_type& next();
};

/**
 *  This is an interator that will interate over values in an
 *  ExponentialInterpolator.
 **/
class ExponentialInterpolatorIterator : public InterpolatorIterator {
public:
    /// constructor for iterator
    ExponentialInterpolatorIterator();

    /// make a clone of the iterator
    ExponentialInterpolatorIterator* clone();

    /// get the next value in the iterator
    m_value_type& next();
};

/**
 *  This is an interpolator that will iterate over values in a
 *  CubicSplineInterpolator.
 **/
class CubicSplineInterpolatorIterator : public InterpolatorIterator {
private:
    m_time_type x0;
    m_value_type y0;
    m_time_type x3;
    m_value_type y3;

public:
    /// constructor for the iterator
    CubicSplineInterpolatorIterator();

    /// make a clone of the iterator
    CubicSplineInterpolatorIterator* clone();

    /// get the next value in the iterator
    m_value_type& next();
};

//----------------------------------------------------------------------------//
#endif  //__INTERPOLATOR_ITERATOR_H
