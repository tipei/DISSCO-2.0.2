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
// Envelope.h
//
//----------------------------------------------------------------------------//

#ifndef __ENVELOPE_H
#define __ENVELOPE_H

//----------------------------------------------------------------------------//
#include "Collection.h"
#include "DynamicVariable.h"
#include "Interpolator.h"
#include "InterpolatorIterator.h"
#include "Iterator.h"
#include "StandardHeaders.h"
#include "Types.h"
#include "XmlReader.h"
//#include "EnvelopeLibrary.h"

/**
 *  This class is designed to allow users to specify the shape of an envelope.
 *  This is done by using a collection of the env_seg type, which can be found
 *  in Types.h. For each segment, a user must specify the x value of the right
 *  endpoint of the segment, the type of interpolation to use for the segment,
 *  and whether or not the segment has a FIXED or FLEXIBLE length. If the total
 *  length specified by the user is greater than the sum of the lengths of the
 *  segments, then every segment with a length type of FLEXIBLE will be scaled
 *  up to fit the total length specified. Sample programs of how to use this
 *  class can be found in the samples directory, named sampleenvelope*.cpp
 *
 *  \note All of the public functions of the class increment the index by 1.
 *This means that a sample program will normally not access the dummy segment
 *information used for the first point. In other words, getSegment(0) will
 *actually retrieve segments_[1]. This is why the index is changed when
 *segments_->get() is used internally by the  Envelope functions.
 *
 *  \author Michael Aikins
 *  \author Greg Augustine
 *  \author Robert Boyce
 *
 *  \revised Fanbo Xiang
 **/

// used to find x coordinates that are very close to each other
#define ENV_MULT_EPSILON 0.01

class Envelope : public DynamicVariable {
public:
    /**
     * Constructor for an empty envelope
     * Components need to be added later
     **/
    Envelope();

    Envelope(const Envelope& other);
    ~Envelope();

    /**
     * \param xy_points the vertices to be used in the envelope
     * \param segs specifies the type of each segment
     **/
    Envelope(Collection<xy_point> xy_points, Collection<envelope_segment> segs);

    /**
     *	This is a constructor.  It makes a new envelope using a collection
     *	of envelope segments.
     *	\param segs A Collection of envelop segments
     */
    Envelope(Collection<envelope_segment> segs);

    /**
     *	This makes a new copy of this envelope
     *	\return A new Envelope
     **/
    Envelope* clone();

    /**
     *	This gets an approximation of the value at the specified time.
     *	\param x The type of value
     *	\param totalLength The length of time
     *	\return The approximate value
     **/
    m_value_type getValue(m_value_type x, m_value_type totalLength);

    /**
     *	This is a debugging function that gives a text output of
     *	what is stored in the Envelope.
     **/
    void print();

    /**
     *	This function (re)defines the Envelope given point and segment
     *	descriptions.
     **/
    void defineShape();

    /**
     *	This function adds to the env the given segment descriptions.
     *	\param segs A Collection of envelope segments
     **/
    void addToShape(Collection<envelope_segment> segs);

    /**
     *	This overloaded function appends one envelope to another.
     *	\param shape The Envelope to append
     **/
    void addToShape(Envelope* shape);

    /**
     *	This function multiplies two envelopes together.
     *	\param env1 The first Envelope
     *	\param env2 The second Envelope
     *	\return The new Envelope
     **/
    Envelope* multiply(Envelope& env1, Envelope& env2);

    /**
     *	This function returns a collection of points that make up the shape.
     *	\return A Collection of points
     **/
    Collection<xy_point>* getPoints();

    /**
     *	This function returns a collection of segments that make up the shape.
     *	\return A Collection of envelope segments
     **/
    Collection<envelope_segment>* getSegments();

    /**
     *	This function sets an envelope segment identified by its index.
     *	\param index Which segment to set
     *	\param segment The new segment value
     **/
    void setSegment(int index, envelope_segment segment);

    /**
     *	This function gets a segment by its index.
     *	\param index Which segment to get
     *	\return An envelope segment
     **/
    envelope_segment getSegment(int index);

    /**
     *	This function sets a point by identified by its index.
     *	\param index Which point to set
     *	\param point The new point value
     **/
    void setPoint(int index, xy_point point);

    /**
     *	This function gets a point by its index.
     *	\param index Which point to get
     *	\return The point
     **/
    xy_point getPoint(int index);

    /**
     *	This function appends a point to the end of the envelope. It's
     *	default interpolation type is LINEAR and stretch type is FIXED.
     *	\param point The point to append
     **/
    void addPoint(xy_point point);

    /**
     *	This function appends a segment to the end of the envelope.
     *	\param segment The segment to append
     **/
    void addSegment(envelope_segment segment);

    /**
     *	This function gets the indexed segment's length of time. If the segment
     *	length type is FLEXIBLE, the length returned will be the unscaled,
     *	original value.
     *	\param index Which segment to examine
     *	\return The segment's length
     **/
    m_value_type getSegmentLength(int index);

    /**
     *	This function sets the indexed segment's length of time.
     *	\param index Which segment to set
     *	\param length The new length of time
     **/
    void setSegmentLength(int index, m_value_type length);

    /**
     *	This function gets the type for the segment with the given index.
     *	\param index Which segment to examine
     *	\return The time type
     *	\retval FIXED
     *	\retval FLEXIBLE
     **/
    stretch_type getSegmentLengthType(int index);

    /**
     *	This function sets the indexed segment's time type.
     *	\param index Which segment to set
     *	\param lengthType The new time type
     **/
    void setSegmentLengthType(int index, stretch_type lengthType);

    /**
     *	This function gets the interpolation type for the segment with the
     *	given index.
     *	\param index Which segment to examine
     *	\return The interpolation type
     *	\retval LINEAR
     *	\retval EXPONENTIAL
     *	\retval etc.
     **/
    interpolation_type getSegmentInterpolationType(int index);

    /**
     *	This function sets the interpolation type for the segment with the
     *	given index.
     *	\param index Which segment to set
     *	\param interType The new interpolation type
     **/
    void setSegmentInterpolationType(int index, interpolation_type interType);

    /**
     *	This function returns an iterator that iterates through all the values.
     *	\return An iterator
     **/
    Iterator<m_value_type> valueIterator();

    /**
     *	This function scales all Entries' values by this factor.
     *	\param factor The factor by which to scale
     **/
    void scale(m_value_type factor);

    /**
     *	This function returns the maximum value in all the entries.
     *	\returns The maximum value
     **/
    m_value_type getMaxValue();

    /**
     *  This function finds the value of an envelope at a given point.
     *  The envelope is loaded first (from an Envelope library) then
     *  scaled according to a given coefficient using Envelope::scale.
     * \param checkPoint
     * \param envNum The envelope number in the library
     * \param coef The coefficient by which to scale the envelope
     * \returns The value of an envelope at a given point
     * \note This used to be in CMOD/utility.cpp
     **/
    float getScaledValueNew(double checkPoint, float coef);

    /**
     * This function computes an envelope number from the parameters,
     * loads the envelope from the library, scales it, and returns
     * the scaled envelope.
     * \param checkPoint
     * \param offset
     * \returns a scaled envelope
     * \note This used to be in CMOD/utility.cpp
     **/
    /*Envelope * FitEnvelopeNew(double checkPoint, int offset);*/

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
     **/
    void xml_read(XmlReader::xmltag* soundtag);

private:
    /**
     *	This is a Collection to hold the segment data for this DVS.
     **/
    Collection<envelope_segment>* segments_;

    /**
     *	This is a Collection to hold interpolators.
     **/
    Collection<Interpolator*>* interpolators_;

    /**
     *	This is a Collection of Time values for segments that were
     *	generated based on the stored value.  This collection will not
     *	include the 0 dummy segment, so element 0 will correspond to element
     *	1 in segments_.
     **/
    Collection<m_value_type>* generatedSegmentLengths_;

    /** This is the value with which the segment times were generated **/
    m_value_type totalLength_;

    /** This is the sampling rate with which interpolators were generated **/
    m_rate_type currentInterpolatorRate_;

    /**
     *	This function populate the private member variable with actual
     *	interpolators.
     *	\param rate The rate
     **/
    void addInterpolators(m_rate_type rate);

    /**
     *	This function populates the private member variable with actual
     *	lengths. This functions scales all FIXED and FLEXIBLE lengths
     *	appropriately.
     *	\note If you want to scale the envelope publicly, use the function
     *	scale.
     *	\param totalLength The total length of time to scale to
     **/
    void generateLengths(m_time_type totalLength);

    /**
     *	This function checks if a number is a valid index for a segment.
     *	\param index The index to check
     *	\retval true Is valid
     *	\retval false Is not valid
     **/
    inline bool checkValidSegmentIndex(int index);
};

#endif  //__ENVELOPE_H
