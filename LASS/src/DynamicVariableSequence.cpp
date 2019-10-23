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
// DynamicVariableSequence.cpp
//
//----------------------------------------------------------------------------//

#ifndef __DYNAMIC_VARIABLE_SEQUENCE_CPP
#define __DYNAMIC_VARIABLE_SEQUENCE_CPP

// #define DEBUG_MODE 1

//----------------------------------------------------------------------------//

#include "DynamicVariableSequence.h"

#include "DynamicVariableSequenceIterator.h"
#include "InterpolatorTypes.h"

using std::setw;

//----------------------------------------------------------------------------//
DynamicVariableSequence::DynamicVariableSequence() {
    xyPoints_ = NULL;
    segments_ = NULL;
    generatedSegmentTimes_ = NULL;
    interpolators_ = NULL;
    totalTime_ = -1;
    currentInterpolatorRate_ = 0;
}

DynamicVariableSequence::DynamicVariableSequence(DynamicVariableSequence& dvs) {
    xyPoints_ = dvs.getPoints();
    segments_ = dvs.getSegments();

    // there's not an easy way to transfer these, so the copy does not contain
    // the generated interpolators, but these are only stored to help
    // optimize the code
    generatedSegmentTimes_ = NULL;
    interpolators_ = NULL;
    totalTime_ = -1;
    currentInterpolatorRate_ = 0;
}

//----------------------------------------------------------------------------//
DynamicVariableSequence::DynamicVariableSequence(Collection<xy_point> xyPoints,
                                                 Collection<envelope_segment> segments) {
    xyPoints_ = NULL;
    segments_ = NULL;
    generatedSegmentTimes_ = NULL;
    interpolators_ = NULL;
    totalTime_ = -1;
    currentInterpolatorRate_ = 0;

    // define the shape, given the Collections
    DefineShape(xyPoints, segments);
}

//----------------------------------------------------------------------------//
DynamicVariableSequence::~DynamicVariableSequence() {
    // make sure we don't cause a memory leak
    if (xyPoints_ != NULL) {
        delete (xyPoints_);
    }
    if (segments_ != NULL) {
        delete (segments_);
    }
    if (interpolators_ != NULL) {
        Interpolator* interTemp = NULL;

        // remove each interpolator* from the collection and delete the
        // allocated memory
        while (interpolators_->size() > 0) {
            interTemp = interpolators_->remove(0);
            delete (interTemp);
        }

        delete (interpolators_);
    }
    if (generatedSegmentTimes_ != NULL) {
        delete (generatedSegmentTimes_);
    }
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::Print() {
    if (segments_ == NULL) {
        cout << "Envelope has not been initialized.\n";
        return;
    }

    int iNumSegments = segments_->size();
    int iLoop = 0;

    cout << endl;
    cout << "DVS has " << iNumSegments << " segments." << endl;

    for (iLoop = 0; iLoop < iNumSegments; iLoop++) {
        // print a line with form (x, y):
        cout << " x, y: "
             << "(";
        cout << setw(5) << xyPoints_->get(iLoop).x;
        cout << ", ";
        cout << setw(5) << xyPoints_->get(iLoop).y;
        cout << ")" << endl;

        // print the entry number
        cout << "     Entry " << iLoop << ":" << endl;

        // print the time type
        cout << "       Time type: ";
        if (this->getSegmentTimeType(iLoop) == FLEXIBLE) {
            cout << "FLEXIBLE" << endl;
        } else {
            cout << "FIXED" << endl;
        }

        // print the time value
        cout << "       Time value: ";
        cout << this->getSegmentTime(iLoop) << endl;

        // print the interpolation type
        cout << "       Interpolation Type: ";
        switch (this->getSegmentInterpolationType(iLoop)) {
            case LINEAR:
                cout << "LINEAR" << endl;
                break;
            case EXPONENTIAL:
                cout << "EXPONENTIAL" << endl;
                break;
            case CUBIC_SPLINE:
                cout << "CUBIC_SPLINE" << endl;
                break;
        }
    }

    // output the last point
    cout << " x, y: "
         << "(";
    cout << setw(5) << xyPoints_->get(iLoop).x;
    cout << ", ";
    cout << setw(5) << xyPoints_->get(iLoop).y;
    cout << ")" << endl;

    cout << "End of DVS." << endl;
    cout << endl;
}

//----------------------------------------------------------------------------//
DynamicVariableSequence* DynamicVariableSequence::clone() {
    // give them a new DVS that is made using the current DVS's data
    return (new DynamicVariableSequence(*xyPoints_, *segments_));
}

//----------------------------------------------------------------------------//

m_value_type DynamicVariableSequence::getValue(m_time_type time, m_time_type timeTotal) {
    if (time > timeTotal || time < 0) {
        // they are dumb for passing in invalid values, so let them know
        cout << "DVS::getValue(...): invalid time or totalTime parameter" << endl;
    }

    // Handle special case where absolute end of dvs is asked for
    if (time == timeTotal) {
        return xyPoints_->get(xyPoints_->size() - 1).y;
    }

    // TODO: Make come sort of cached flag that will see if this
    //	actually needs to be performed or not.
    generateTimes(timeTotal);

    // First, find the segment index that this time/timeTotal refers to
    m_time_type current = 0;
    int timeIndex = 0;
    while (current < time) {
        current += generatedSegmentTimes_->get(timeIndex);
        timeIndex++;
    }

    // If we have overshot, lets go back
    if (current != time) {
        timeIndex--;
        current -= generatedSegmentTimes_->get(timeIndex);
    }

    // Spawn an interpolator of the proper type
    Interpolator* interp;
    switch (getSegmentInterpolationType(timeIndex)) {
        case EXPONENTIAL:
            interp = new ExponentialInterpolator();
            break;

        case CUBIC_SPLINE:
            interp = new CubicSplineInterpolator();
            break;

        default:
            interp = new LinearInterpolator();
            break;
    }

    // set the Interpolator's sampling rate to a fixed granularity
    interp->setSamplingRate(100);

    // set interpolator duration to one second to make it easy
    interp->setDuration(1.0);

    // We know that this segment interpolates between xy point
    // timeIndex and timeIndex+1
    xy_point left, right;
    left = getPoint(timeIndex);
    right = getPoint(timeIndex + 1);

    // Now tell the interpolator what values we are going between
    interp->addEntry(0, left.y);
    interp->addEntry(1, right.y);

    // Figure out which sample we want from the interpolator.
    // This is done by taking the percentage we have to iterate thru
    // the interpolator, and then multiplying that by the number of samples
    int sample = int(round(((time - current) / generatedSegmentTimes_->get(timeIndex)) * 100.0));

    // Create a value iterator to get values from the interpolator
    Iterator<m_value_type> tempIterator = interp->valueIterator();

    // Now lets skip the first sample-1 samples...
    for (int i = 0; i < sample; i++) {
        tempIterator.next();
    }

    // Now grab the one we want
    m_value_type returnval = tempIterator.next();

    delete interp;

    return returnval;
}

/* //The old getValue
m_value_type DynamicVariableSequence::getValue (m_time_type time,
                                                m_time_type timeTotal)
{
    m_rate_type newSamplingRate = 50;
    m_rate_type oldSamplingRate = this->getSamplingRate();
    m_time_type oldDuration = this->getDuration();
    m_value_type returnValue = 0;

    if (time > timeTotal || time < 0)
    {
        // they are dumb for passing in invalid values, so let them know
        cout << "DVS::getValue(...): invalid time or totalTime parameter" <<
endl;
    }
    else
    {
        // set the dynamic variable parameters to temporary values
        this->setSamplingRate(newSamplingRate);
        this->setDuration(timeTotal);

        // get an iterator for this shape
        Iterator<m_value_type> *tempIterator =
            new Iterator<m_value_type> (valueIterator());

        // we can calculate how many samples are needed for the floor (time)
        m_time_type timeSum = (m_time_type) floor ((double) time);
        int iNumGetValCalls = newSamplingRate * ((int) floor (time));

        // now, calculate how many samples are needed for (time - floor (time))
        while (timeSum < time)
        {
            timeSum = timeSum + (1.0 / newSamplingRate);
            iNumGetValCalls++;
        }

        // it seems to be a few samples off...this is a hack to get good values
        iNumGetValCalls += 2;

        // iterate, based on the values calculated
        for (int iLoop = 0; iLoop < iNumGetValCalls; iLoop++)
        {
            returnValue = tempIterator->next();
        }

        // cout << "  iNumGetValCalls = " << iNumGetValCalls << endl;
        // cout << "  timeSum = " << timeSum << endl;

        // set the dynamic variable paramaters back to what they were
        this->setSamplingRate(oldSamplingRate);
        this->setDuration(oldDuration);
    }

    return (returnValue);

}
*/

//----------------------------------------------------------------------------//
inline void DynamicVariableSequence::DefineShape(Collection<xy_point> xyPoints,
                                                 Collection<envelope_segment> segments) {
    // make sure we don't cause a memory leak
    if (xyPoints_ != NULL) {
        delete (xyPoints_);
    }
    if (segments_ != NULL) {
        delete (segments_);
    }

    if (xyPoints.size() != (segments.size() + 1)) {
        cout << "WARNING: Invalid Collection sizes for points or segments" << endl;
        cout << "points.size ( ) should be segments.size ( ) + 1" << endl;
    }

    // set our local variables to copies of the variables passed in
    xyPoints_ = new Collection<xy_point>(xyPoints);
    segments_ = new Collection<envelope_segment>(segments);
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::AddToShape(Collection<xy_point> xyPoints,
                                         Collection<envelope_segment> segments) {
    // case: Adding to an empty Collection
    if ((xyPoints_ == NULL) || (segments_ == NULL)) {
        xyPoints_ = new Collection<xy_point>(xyPoints);
        segments_ = new Collection<envelope_segment>(segments);
        return;
    }

    int iNumSegments = segments.size();
    m_time_type timeOffset = xyPoints_->get(xyPoints_->size() - 1).x;
    xy_point pointTemp;

    // verify that the last old point and the first new point have the
    // same y value (usually zero), that they can be appended
    if ((xyPoints_->get(xyPoints_->size() - 1).y) != (xyPoints.get(0).y)) {
        cout << "WARNING: Y values of last point of first envelope\n";
        cout << "and first point of second envelope do not match.\n";
        cout << "This implementation corrupts the first point of the\n";
        cout << "second envelope\n";
    }

    // for every element in the new collections
    for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
        // add the current segment and point + 1 to our existing Collections
        pointTemp = xyPoints.get(iIndex + 1);
        pointTemp.x = pointTemp.x + timeOffset;
        xyPoints_->add(pointTemp);
        segments_->add(segments.get(iIndex));
    }
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::AddToShape(DynamicVariableSequence* shape) {
    if ((shape->xyPoints_ == NULL) || (shape->segments_ == NULL)) {
        cout << "ERROR: DVS to be added is undefined.\n";
        return;
    }

    AddToShape(*(shape->xyPoints_), *(shape->segments_));
}

//----------------------------------------------------------------------------//
Collection<xy_point>* DynamicVariableSequence::getPoints() {
    // the point data are stored in a member variable, so return a copy of it
    return (new Collection<xy_point>(*xyPoints_));
}

//----------------------------------------------------------------------------//
Collection<envelope_segment>* DynamicVariableSequence::getSegments() {
    // the point data are stored in a member variable, so return a copy of it
    return (new Collection<envelope_segment>(*segments_));
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::setSegment(int index, envelope_segment segment) {
    // lets the Collection class hand an invalid index
    segments_->set(index, segment);
}

//----------------------------------------------------------------------------//
envelope_segment DynamicVariableSequence::getSegment(int index) {
    // lets the Collection class hand an invalid index
    return segments_->get(index);
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::setPoint(int index, xy_point point) {
    // lets the Collection class hand an invalid index
    xyPoints_->set(index, point);
}

//----------------------------------------------------------------------------//
xy_point DynamicVariableSequence::getPoint(int index) {
    // lets the Collection class hand an invalid index
    return xyPoints_->get(index);
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::addPoint(xy_point point) { xyPoints_->add(point); }

//----------------------------------------------------------------------------//
void DynamicVariableSequence::addSegment(envelope_segment segment) { segments_->add(segment); }

//----------------------------------------------------------------------------//
m_time_type DynamicVariableSequence::getSegmentTime(int index) {
    // lets the Collection class hand an invalid index
    return (segments_->get(index).timeValue);
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::setSegmentTime(int index, m_time_type time) {
    // make sure the time is positive, otherwise bad things will happen
    if (time < 0) {
        time = -time;
    }

    // assumes a valid index is supplied
    envelope_segment segTemp = segments_->get(index);
    segTemp.timeValue = time;
    segments_->set(index, segTemp);
}

//----------------------------------------------------------------------------//
stretch_type DynamicVariableSequence::getSegmentTimeType(int index) {
    // assumes a valid index is supplied
    return (segments_->get(index).timeType);
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::setSegmentTimeType(int index, stretch_type timeType) {
    // assumes a valid index is supplied
    envelope_segment segTemp = segments_->get(index);
    segTemp.timeType = timeType;
    segments_->set(index, segTemp);
}

//----------------------------------------------------------------------------//
interpolation_type DynamicVariableSequence::getSegmentInterpolationType(int index) {
    // assumes a valid index is supplied
    return (segments_->get(index).interType);
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::setSegmentInterpolationType(int index, interpolation_type interType) {
    // assumes a valid index is supplied
    envelope_segment segTemp = segments_->get(index);
    segTemp.interType = interType;
    segments_->set(index, segTemp);
}

//----------------------------------------------------------------------------//
inline bool DynamicVariableSequence::checkValidSegmentIndex(int index) {
    // check if index is valid
    return ((index >= 0) && (segments_->size() > index));
}

//----------------------------------------------------------------------------//
inline bool DynamicVariableSequence::checkValidPointIndex(int index) {
    // check if index is valid
    return ((index >= 0) && (xyPoints_->size() > index));
}

//----------------------------------------------------------------------------//
Iterator<m_value_type> DynamicVariableSequence::valueIterator() {
    // optimize so that if the envelope is used repeatedly without changing
    // the time value that is assigned, interpolators are not re-created
    // every time
    if (totalTime_ != this->getDuration() || currentInterpolatorRate_ != this->getSamplingRate()) {
        totalTime_ = this->getDuration();
        currentInterpolatorRate_ = this->getSamplingRate();

        // populate the times member variable with times
        generateTimes(totalTime_);

        // populate the interpolator member variable with interpolators, based
        // on the times just set
        addInterpolators(currentInterpolatorRate_);
    }

    // give them the big iterator
    return (Iterator<m_value_type>(new DynamicVariableSequenceIterator(*interpolators_)));
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::generateTimes(m_time_type totalTime) {
#ifdef DEBUG_MODE
    cout << "DVS::generateTimes" << endl;
#endif
    int iNumSegments = segments_->size();
    double dTotalFixedTime = 0;
    double dTotalFlexPercent = 0;
    double dScaleRatio = 1;

    totalTime_ = totalTime;

    // get rid of previously generated times and make a new collection
    if (generatedSegmentTimes_ != NULL) {
        delete (generatedSegmentTimes_);
    }
    generatedSegmentTimes_ = new Collection<m_time_type>();

    // figure out how they want to allocate time for each segment and populate
    // the generated times with their current values
    for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
        // if this segment has fixed-time, increment fixed time running total
        if (this->getSegmentTimeType(iIndex) == FIXED) {
            dTotalFixedTime += this->getSegmentTime(iIndex);
        }
        // else increment flex-time running total
        else {
            dTotalFlexPercent += this->getSegmentTime(iIndex);
        }
        // append their current value to our values for generated times
        generatedSegmentTimes_->add(this->getSegmentTime(iIndex));
    }

    // now, correct the generated values as necessary (i.e. the rest of this)

    // if they specified more FIXED time than actual time, or if there is no
    // FIEXIBLE time and they did not specify enough FIXED time, scale the
    // FIXED times to the actual time (and set total amount of FLEXIBLE time
    // to zero)
    if (dTotalFixedTime > totalTime_ || (dTotalFixedTime < totalTime_ && dTotalFlexPercent == 0)) {
        // no flexible time is available
        dTotalFlexPercent = 0;

        // get a ratio to scale all fixed time entries by
        dScaleRatio = totalTime_ / dTotalFixedTime;

#ifdef DEBUG_MODE
        cout << "DVS::generateTimes FIXED dScaleRatio = " << dScaleRatio << endl;
#endif

        for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
            // scale every FIXED time value
            if (this->getSegmentTimeType(iIndex) == FIXED) {
                generatedSegmentTimes_->set(iIndex, this->getSegmentTime(iIndex) * dScaleRatio);
            } else {
                // all flex time entries are set to 0
                generatedSegmentTimes_->set(iIndex, 0);
            }
        }
    }
    // else if their percentages do not add up to 100% (i.e. 1.0) and they are
    // trying to use flex time, scale the flex time values
    else if (dTotalFlexPercent != 1.0 && dTotalFlexPercent != 0) {
        dScaleRatio = 1.0 / dTotalFlexPercent;

#ifdef DEBUG_MODE
        cout << "DVS::generateTimes FLEXIBLE dScaleRatio = " << dScaleRatio << endl;
#endif

        // scale percentages
        for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
            if (this->getSegmentTimeType(iIndex) == FLEXIBLE) {
                generatedSegmentTimes_->set(iIndex, this->getSegmentTime(iIndex) * dScaleRatio);
            }
        }
    }

    // if we have FLEXIBLE time to set, the values stored right now are only
    // ratios of time available, so we need to set them to actual times
    if (dTotalFlexPercent > 0) {
        // calculate the time available to use for flex durations
        double flexTimeAvailable = totalTime_ - dTotalFixedTime;

#ifdef DEBUG_MODE
        cout << "DVS::generateTimes flexTimeAvailable = " << flexTimeAvailable << endl;
#endif

        // set actual durations for FLEXIBLE time segments
        for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
            // if this is a flex-time entry, set time accordingly
            if (this->getSegmentTimeType(iIndex) == FLEXIBLE) {
                generatedSegmentTimes_->set(
                    iIndex, generatedSegmentTimes_->get(iIndex) * flexTimeAvailable);
            }
        }
    }

#ifdef DEBUG_MODE
    cout << "DVS::generateTimes all done.  Segments times are:" << endl;

    for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
        cout << "Segment " << iIndex << ": " << generatedSegmentTimes_->get(iIndex) << endl;
    }
#endif
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::addInterpolators(m_rate_type rate) {
#ifdef DEBUG_MODE
    cout << "DVS::addInterpolators" << endl;
    cout << "num segments: " << segments_->size() << endl;
    cout << "num points: " << xyPoints_->size() << endl;
#endif

    int iNumSegments = segments_->size();

    // clear out interpolators that are stored, if there are any stored
    if (interpolators_ != NULL) {
        delete (interpolators_);
    }

    // initialize a new Collection for interpolators
    interpolators_ = new Collection<Interpolator*>();

    // for each segment that we have, store a new interpolator
    for (int iIndex = 0; iIndex < iNumSegments; iIndex++) {
        // add an interpolator for the current segment to the Collection
        switch (getSegmentInterpolationType(iIndex)) {
            case EXPONENTIAL:
                interpolators_->add(new ExponentialInterpolator());
                break;

            case CUBIC_SPLINE:
                interpolators_->add(new CubicSplineInterpolator());
                break;

            default:
                interpolators_->add(new LinearInterpolator());
                break;
        }

        // set the Interpolator's sampling rate to the rate specified
        interpolators_->get(iIndex)->setSamplingRate(rate);

        m_time_type tempTimeValue = generatedSegmentTimes_->get(iIndex);

        // add a start and end point to this interpolator:
        interpolators_->get(iIndex)->addEntry(0, xyPoints_->get(iIndex).y);
        interpolators_->get(iIndex)->addEntry(tempTimeValue, xyPoints_->get(iIndex + 1).y);

        // set interpolator duration
        interpolators_->get(iIndex)->setDuration(tempTimeValue);

#ifdef DEBUG_MODE
        cout << "Interpolator #" << iIndex << " - added points: " << endl;
        cout << "   (0, " << xyPoints_->get(iIndex).y << ")" << endl;
        cout << "   (" << generatedSegmentTimes_->get(iIndex) << ", ";
        cout << xyPoints_->get(iIndex + 1).y << ")" << endl;
#endif
    }
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::scale(m_value_type factor) {
    int iNumPoints = xyPoints_->size();
    xy_point pointTemp;

    // for every point that we've got stored
    for (int iIndex = 0; iIndex < iNumPoints; iIndex++) {
        // scale this value by the factor given
        pointTemp.x = xyPoints_->get(iIndex).x;
        pointTemp.y = (xyPoints_->get(iIndex).y) * factor;
        xyPoints_->set(iIndex, pointTemp);
    }
}

//----------------------------------------------------------------------------//
m_value_type DynamicVariableSequence::getMaxValue() {
    int iNumPoints = xyPoints_->size();
    m_value_type maxVal = 0.0;

    // for every point that we've got stored
    for (int iIndex = 0; iIndex < iNumPoints; iIndex++) {
        // if this point's value is greater than the max, set max = this value
        if (xyPoints_->get(iIndex).y > maxVal) {
            maxVal = xyPoints_->get(iIndex).y;
        }
    }

    return maxVal;
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::xml_print(ofstream& xmlOutput) {
    DynamicVariable* pnt2dyn = this;

    xmlOutput << "<dv id=\"" << (long)pnt2dyn << "\">" << endl;
    xmlOutput << "\t<dv_type value=\"dvs\" />" << endl;
    xmlOutput << "\t<duration value=\"" << getDuration() << "\" />" << endl;
    xmlOutput << "\t<rate value=\"" << getSamplingRate() << "\" />" << endl;

    // Print out private vars and collections for DVS here
    xmlOutput << "\t<totalTime value=\"" << totalTime_ << "\" />" << endl;
    xmlOutput << "\t<currentInterpolatorRate value=\"" << currentInterpolatorRate_ << "\" />"
              << endl;
    xy_point mypoint;
    Iterator<xy_point> it_points = xyPoints_->iterator();
    while (it_points.hasNext()) {
        mypoint = it_points.next();
        xmlOutput << "\t<xyPoints x=\"" << mypoint.x << "\" ";
        xmlOutput << "value=\"" << mypoint.y << "\" />" << endl;
    }
    envelope_segment myseg;
    Iterator<envelope_segment> it_segments = segments_->iterator();
    while (it_segments.hasNext()) {
        myseg = it_segments.next();
        xmlOutput << "\t<segments interType=\"" << myseg.interType << "\" ";
        xmlOutput << "timeType=\"" << myseg.timeType << "\" ";
        xmlOutput << "timeValue=\"" << myseg.timeValue << "\" />" << endl;
    }
    // Note: the interpolators_ and generatedSegmentTimes_ collections
    //      doesn't need to be backed up; it is used for internal
    //      optimization only, not storing data

    xmlOutput << "</dv>" << endl;
}

//----------------------------------------------------------------------------//
void DynamicVariableSequence::xml_print(ofstream& xmlOutput, list<DynamicVariable*>& dynObjs) {
    DynamicVariable* pnt2dyn = this;

    // Print the pointer value as an ID, then the "meat" gets printed later
    xmlOutput << "\t\t\t\t<dv_type value=\"dvs\" />" << endl;
    xmlOutput << "\t\t\t\t<dv_ptr id=\"" << (long)pnt2dyn << "\" />" << endl;

    // Update dynamic variable list if necessary
    list<DynamicVariable*>::const_iterator dynit;
    for (dynit = dynObjs.begin(); dynit != dynObjs.end(); dynit++) {
        if ((*dynit) == this) break;
    }
    if (dynit == dynObjs.end()) {
        dynObjs.push_back(this);
    }
}

void DynamicVariableSequence::xml_read(XmlReader* xml, char* tagname) {
    printf("IM HERE!");
    XmlReader::xmltag* tag = new XmlReader::xmltag;
    while (xml->readTag(tag)) {
        if (!strcmp(tag->name, tagname) && tag->isClosing) break;

        if (!strcmp(tag->name, "dv_type")) {
            if (strcmp(tag->getParamValue("value"), "dvs")) {
                cout << "OH GOD!  I WAS HOPING FOR dvs BUT I GOT " << tag->getParamValue("value")
                     << endl;
            }
        }
    }
    delete tag;
}

//----------------------------------------------------------------------------//
#endif  //__DYNAMIC_VARIABLE_SEQUENCE_CPP
