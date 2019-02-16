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
//	InterpolatorTypes.h
//
//----------------------------------------------------------------------------//

#ifndef __INTERPOLATOR_TYPES_H
#define __INTERPOLATOR_TYPES_H

//----------------------------------------------------------------------------//

#include "Types.h"
#include "InterpolatorIterator.h"
#include "Interpolator.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//
/**
*	This is a DynamicVariable that changes over time.
*	This linearly interpolates between a set of points
*	ordered in time. (LinearInterpolatorEntry)
*
*	\author Braden Kowitz
*	\author Philipp Fraund
**/
class LinearInterpolator : public Interpolator
{

public:
    
    /**
    *	This is the default constructor.
    **/
    LinearInterpolator();

    /**
    *	This makes a clone of a LinearInterpolator.
    *	\return A new LinearInterpolator
    **/
    LinearInterpolator* clone();

    /**
    *	This creates an iterator over LinearInterpolators.
    *	\return An iterator
    **/
    Iterator<m_value_type> valueIterator();

    /**
    *	This provides an implementation to get the type of interpolator.
    *	\return The interpolation type
    **/
    virtual interpolation_type getType();
};

/**
*  This is a DynamicVariable that changes over time.
*  This exponentially interpolates between a set of points
*  ordered in time.
*  \author Braden Kowitz
*  \author Philipp Fraund
**/
class ExponentialInterpolator : public Interpolator
{

public:
    
    /**
    *	This is the default constructor
    **/
    ExponentialInterpolator();
    
    /**
    *	This makes a clone of a ExponentialInterpolator.
    *	\return A new ExponentialInterpolator
    **/
    ExponentialInterpolator* clone();

    /**
    *	This creates an iterator over ExponentialInterpolators.
    *	\return An iterator
    **/
    Iterator<m_value_type> valueIterator();

    /**
    *	This provides an implementation to get the type of interpolator.
    *	\return The interpolation type
    **/
    virtual interpolation_type getType();
};

/**
*  This is a DynamicVariable that changes over time.
*  This does cubic spline interpolation between a set of points
*  ordered in time.
*  \author Braden Kowitz
*  \author Philipp Fraund
**/
class CubicSplineInterpolator : public Interpolator
{

public:
    
    /**
    *	This is the default constructor.
    **/
    CubicSplineInterpolator();

    /**
    *	This makes a clone of a CubicSplineInterpolator.
    *	\return A new CubicSplineInterpolator
    **/
    CubicSplineInterpolator* clone();

    /**
    *	This creates an iterator over CubicSplineInterpolators.
    *	\return An iterator
    **/
    Iterator<m_value_type> valueIterator();

    /**
    *	This provides an implementation to get the type of interpolator.
    *	\return The interpolation type
    **/
    virtual interpolation_type getType();
};

//----------------------------------------------------------------------------//
#endif //INTERPOLATOR_TYPES_H
