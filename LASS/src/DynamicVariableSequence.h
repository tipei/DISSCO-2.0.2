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
// DynamicVariableSequence.h
//
//----------------------------------------------------------------------------//

#ifndef __DYNAMIC_VARIABLE_SEQUENCE_H
#define __DYNAMIC_VARIABLE_SEQUENCE_H

//----------------------------------------------------------------------------//

#include "Collection.h"
#include "DynamicVariable.h"
#include "Interpolator.h"
#include "Iterator.h"
#include "StandardHeaders.h"
#include "Types.h"
#include "XmlReader.h"

/**
 *  This class is designed to "put together" several DynamicVariables that then
 *  act as a single DynamicVariable.  This allows easy combining of shapes
 *  for sounds defined by DynamicVariables to make bigger shapes.  It also adds
 *  the ability to specify whether a particular DynamicVariable plays for a
 *  fixed amount of time or for a percentage of time available.  For each
 *  DynamicVariableSequenceEntry that is stored in the DynamicVariableSequence,
 *  the user specifies how long the Entry will play for.  Checking is done
 *  to ensure that if invalid input is given, the values are adjusted
 *  accordingly.  The sampledvs*.cpp files in the samples directory
 *  demonstrate how checking is done.
 *
 *  As demonstrated in the sample files, the values for a fixed amount of time
 *  are the amount of time that this DynamicVariableSequenceEntry will play for,
 *  and the only reason that it would not play for this much time is if the
 *  sound that it is a shape for does not play for that much time.  In this
 *case, the amount of time will get scaled to fill up the whole time available.
 *
 *  Also demonstrated in the sample files are the ways that flexible time is
 *  assigned.  Basically, all values for flexible time are relative to one
 *  another.  This is easiest explained by examples:
 *
 *  - DynamicVariableSequence A will play for 10 seconds and has 3 Entries:
 *    - Entry 1 uses flexible time with a value of 0.1 as a "percentage."
 *    - Entry 2 uses flexible time with a value of 0.4 as a "percentage."
 *    - Entry 3 uses flexible time with a value of 0.5 as a "percentage."
 *
 *  - DynamicVariableSequence B will play for 10 seconds and has 3 Entries:
 *    - Entry 1 uses flexible time with a value of 10 as a "percentage."
 *    - Entry 2 uses flexible time with a value of 40 as a "percentage."
 *    - Entry 3 uses flexible time with a value of 50 as a "percentage."
 *
 *  - DynamicVariableSequence C will play for 10 seconds and has 3 Entries:
 *    - Entry 1 uses flexible time with a value of 20 as a "percentage."
 *    - Entry 2 uses flexible time with a value of 80 as a "percentage."
 *    - Entry 3 uses flexible time with a value of 100 as a "percentage."
 *
 *  Since all the "percentage" values are relative to one another, these will
 *  all define the same shape.  Thus, the value of every "percentage"
 *  is normalized to whatever value all the flexible times add up to.
 *
 *  \author Jon Kishkunas
 **/
class DynamicVariableSequence : public DynamicVariable {
public:
    /// Constructor - no parameters...assumes data will be given later.
    DynamicVariableSequence();

    /// Constructor - no parameters...assumes data will be given later.
    DynamicVariableSequence(DynamicVariableSequence& dvs);

    /**
     *	Constructor
     * 	\param xyPoints A Collection of points
     *	\param segments A Collection of information for the segments between the
     *points
     **/
    DynamicVariableSequence(Collection<xy_point> xyPoints, Collection<envelope_segment> segments);

    /**
     *	Destructor
     **/
    ~DynamicVariableSequence();

    /**
     *	This makes a copy of this object.
     *   \return A pointer to a DynamicVariableSequence
     **/
    DynamicVariableSequence* clone();

    /**
     *	This gets an approximation of the value at the specified time.
     *	\param time The time to begin looking at the value
     *	\param totalTime The elapsed time to look at the value
     *	\return The approximate value during that time
     **/
    m_value_type getValue(m_time_type time, m_time_type totalTime);

    /**
     *	This will give a text output for what is stored in the DVS.
     *	\note mainly for debugging
     **/
    void Print();

    /**
     *	This (re)defines the DVS point and segment descriptions.
     *	\param xyPoints A Collection of points
     *	\param segments A Collection of information about the segments between
     *points
     **/
    inline void DefineShape(Collection<xy_point> xyPoints, Collection<envelope_segment> segments);

    /**
     *	This adds to the DVS the given point and segment descriptions.
     *	\param xyPoints A Collection of points
     *	\param segments A Collection of information about the segments between
     *points
     **/
    void AddToShape(Collection<xy_point> xyPoints, Collection<envelope_segment> segments);

    /**
     *	This appends one DVS to another.
     *	\param shape The DVS to append to this one
     **/
    void AddToShape(DynamicVariableSequence* shape);

    /**
     *	This returns the Collection of points that make up the shape.
     *	\return A Collection of points
     **/
    Collection<xy_point>* getPoints();

    /** This returns the collection of segments that make up the shape.
     *	\return A Collection of segments
     **/
    Collection<envelope_segment>* getSegments();

    /**	This sets a segment by index.
     *	\param index Which segment to set
     *	\param segment The new segment
     **/
    void setSegment(int index, envelope_segment segment);

    /**
     *	This gets a segment by index.
     *	\param index Which segment to get
     *	\return An envelope_segment
     **/
    envelope_segment getSegment(int index);

    /**
     *	This sets a point by index.
     *	\param index Which point to set
     *	\param point The new point
     **/
    void setPoint(int index, xy_point point);

    /**
     *	This gets a point by index.
     *	\param index Which point to get
     *	\return A point
     **/
    xy_point getPoint(int index);

    /**
     *	This appends a point to the end of the DVS.
     *	\param point The point to append
     **/
    void addPoint(xy_point point);

    /**
     *	This appends a segment to the end of the DVS.
     *	\param segment The segment to append
     **/
    void addSegment(envelope_segment segment);

    /**
     * This gets the indexed segment's time, that is, it's actual playing time
     * (if using FIXED time) or percentage of time available (if using FLEXIBLE
     * time).
     *	\param index Which segment to get
     *	\return the playing time
     **/
    m_time_type getSegmentTime(int index);

    /**	This sets the indexed segment's time.
     *	\param index Which segment to set
     *	\param time The duration of the segment
     **/
    void setSegmentTime(int index, m_time_type time);

    /**
     *	This gets the time type for the segment with the given index
     *	\param index Which segment to examine
     *	\retval FIXED
     *	\retval FLEXIBLE
     **/
    stretch_type getSegmentTimeType(int index);

    /**
     *	This sets the indexed segment's time type.
     *	\param index Which segment to set
     *	\param timeType The type of time to set (FIXED/FLEXIBLE)
     **/
    void setSegmentTimeType(int index, stretch_type timeType);

    /**
     *	This gets the interpolation type for the segment with the given index.
     *	(LINEAR, EXPONENTIAL, etc).
     *	\param index Which segment to get
     *	\return The interpolation type
     **/
    interpolation_type getSegmentInterpolationType(int index);

    /**
     *	This sets the interpolation type for the segment with the given index.
     *	\param index Which segment to set
     *	\param interType The interpolation type
     **/
    void setSegmentInterpolationType(int index, interpolation_type interType);

    /** An iterator through all the values.
     *	\return An iterator
     **/
    Iterator<m_value_type> valueIterator();

    /**
     *	This scales all Entries' values by this factor.
     *	\param factor The factor to scale by
     **/
    void scale(m_value_type factor);

    /**
     *	This returns the maximum value of all the entries.
     *	\return The max value
     **/
    m_value_type getMaxValue();

    /**
     *	\deprecated
     *   This outputs an XML representation of the object to STDOUT
     **/
    void xml_print(ofstream& xmlOutput, list<DynamicVariable*>& dynObjs);

    /**
     *	\deprecated
     *	This outputs an XML representation of the object to STDOUT
     **/
    void xml_print(ofstream& xmlOutput);

    /**
     *	\deprecated
     *	This reads the DVS from xml
     **/
    void xml_read(XmlReader* xml, char* tagname);

private:
    /**
     *	This is a Collection to hold the x-y points that define this DVS
     **/
    Collection<xy_point>* xyPoints_;

    /**
     *	This is a Collection to hold the segment data for this DVS
     **/
    Collection<envelope_segment>* segments_;

    /**
     *	This is a Collection to hold interpolators
     **/
    Collection<Interpolator*>* interpolators_;

    /**
     *	Time values for segments that were generated based on the stored value
     **/
    Collection<m_time_type>* generatedSegmentTimes_;

    /**
     *	This is the value with which the segment times were generated
     **/
    m_time_type totalTime_;

    /**
     *	This is the sampling rate with which interpolators were generated
     **/
    m_rate_type currentInterpolatorRate_;

    /**
     *	This populates the private member variable with actual interpolators.
     *	\param rate The sampling rate
     **/
    void addInterpolators(m_rate_type rate);

    /**
     *	This populates the private member variable with actual times.
     *	\param totalTime The time
     **/
    void generateTimes(m_time_type totalTime);

    /**
     *	This checks if a number is a valid index for a segment.
     *	\param index The index to check
     *	\retval true Is valid
     *	\retval false Is not valid
     **/
    inline bool checkValidSegmentIndex(int index);

    /**
     *	This checks if a number is a valid index for a point.
     *	\param index The index to check
     *	\retval true Is valid
     *	\retval false Is not valid
     **/
    inline bool checkValidPointIndex(int index);
};

//----------------------------------------------------------------------------//
#endif  //__DYNAMIC_VARIABLE_SEQUENCE_H
