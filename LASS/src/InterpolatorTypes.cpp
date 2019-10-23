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
//	InterpolatorTypes.cpp
//
//----------------------------------------------------------------------------//

#ifndef __INTERPOLATOR_TYPES_CPP
#define __INTERPOLATOR_TYPES_CPP

//----------------------------------------------------------------------------//

#include "InterpolatorTypes.h"

//----------------------------------------------------------------------------//
LinearInterpolator::LinearInterpolator() {}

//----------------------------------------------------------------------------//
LinearInterpolator* LinearInterpolator::clone() {
    // create a copy of this
    LinearInterpolator* li = new LinearInterpolator(*this);
    // return it
    return li;
}

//----------------------------------------------------------------------------//
Iterator<m_value_type> LinearInterpolator::valueIterator() {
    // now, we create a LinearInterpolatorIterator with these entries:
    LinearInterpolatorIterator* lii = new LinearInterpolatorIterator();

    // special case for zero entries:
    if (size() == 0) {
        lii->append(0, 0, 0, 0, long(getDuration() * getSamplingRate()));
        return Iterator<m_value_type>(lii);
    }

    // we need to sort the values in this LinearInterpolator by time:
    sortCollection();

    // We are going to scale all of the values here by the duration.
    // so, the higest value in this collection will be mapped to Duration.
    // we do this by specifying a sampling rate for the conversion:

    m_time_type maxTime = get(size() - 1).time_;
    m_value_type mySamplingRate = (getDuration() / maxTime) * getSamplingRate();

    // an iterator for this collection:
    Iterator<InterpolatorEntry> it = iterator();

    // grab the first value:
    InterpolatorEntry lie = it.next();
    m_value_type lastValue = lie.value_;
    m_time_type lastTime = lie.time_;

    // special case for only one entry:
    if (size() == 1) {
        lii->append(lastTime, lastTime, lastValue, lastValue,
                    long(getDuration() * getSamplingRate()));
    }

    while (it.hasNext()) {
        // get the next entry:
        lie = it.next();

        // convert this entry into a LinearInterpolatorIterator::Entry
        m_time_type timeDiff = lie.time_ - lastTime;
        m_sample_count_type steps = (m_sample_count_type)(timeDiff * mySamplingRate);
        // and add it to the iterator:
        lii->append(lastTime, lie.time_, lastValue, lie.value_, steps);

        // update values:
        lastValue = lie.value_;
        lastTime = lie.time_;
    }

    // wrap the abstract iterator as an iterator and return:
    return Iterator<m_value_type>(lii);
}

//----------------------------------------------------------------------------//
interpolation_type LinearInterpolator::getType() { return LINEAR; }

//----------------------------------------------------------------------------//
ExponentialInterpolator::ExponentialInterpolator() {}

//----------------------------------------------------------------------------//
ExponentialInterpolator* ExponentialInterpolator::clone() {
    // create a copy of this
    ExponentialInterpolator* ei = new ExponentialInterpolator(*this);
    // return it
    return ei;
}

//----------------------------------------------------------------------------//
Iterator<m_value_type> ExponentialInterpolator::valueIterator() {
    // now, we create a ExponentialInterpolatorIterator with these entries:
    ExponentialInterpolatorIterator* eii = new ExponentialInterpolatorIterator();

    // special case for zero entries:
    if (size() == 0) {
        eii->append(0, 0, 0, 0, long(getDuration() * getSamplingRate()));
        return Iterator<m_value_type>(eii);
    }

    // we need to sort the values in this ExponentialInterpolator by time:
    sortCollection();

    // We are going to scale all of the values here by the duration.
    // so, the higest value in this collection will be mapped to Duration.
    // we do this by specifying a sampling rate for the conversion:

    m_time_type maxTime = get(size() - 1).time_;
    m_value_type mySamplingRate = (getDuration() / maxTime) * getSamplingRate();

    // an iterator for this collection:
    Iterator<InterpolatorEntry> it = iterator();

    // grab the first value:
    InterpolatorEntry eie = it.next();
    m_value_type lastValue = eie.value_;
    m_time_type lastTime = eie.time_;

    // special case for only one entry:
    if (size() == 1) {
        eii->append(lastTime, lastTime, lastValue, lastValue,
                    long(getDuration() * getSamplingRate()));
    }

    while (it.hasNext()) {
        // get the next entry:
        eie = it.next();

        // convert this entry into a ExponentialInterpolatorIterator::Entry
        m_time_type timeDiff = eie.time_ - lastTime;
        m_sample_count_type steps = (m_sample_count_type)(timeDiff * mySamplingRate);
        // and add it to the iterator:
        eii->append(lastTime, eie.time_, lastValue, eie.value_, steps);

        // update values:
        lastValue = eie.value_;
        lastTime = eie.time_;
    }

    // wrap the abstract iterator as an iterator and return:
    return Iterator<m_value_type>(eii);
}

//----------------------------------------------------------------------------//
interpolation_type ExponentialInterpolator::getType() { return EXPONENTIAL; }

//----------------------------------------------------------------------------//
CubicSplineInterpolator::CubicSplineInterpolator() {}

//----------------------------------------------------------------------------//
CubicSplineInterpolator* CubicSplineInterpolator::clone() {
    // create a copy of this
    CubicSplineInterpolator* csi = new CubicSplineInterpolator(*this);
    // return it
    return csi;
}

//----------------------------------------------------------------------------//
Iterator<m_value_type> CubicSplineInterpolator::valueIterator() {
    // now, we create a CubicSplineInterpolatorIterator with these entries:
    CubicSplineInterpolatorIterator* csii = new CubicSplineInterpolatorIterator();

    // special case for zero entries:
    if (size() == 0) {
        csii->append(0, 0, 0, 0, long(getDuration() * getSamplingRate()));
        return Iterator<m_value_type>(csii);
    }

    // we need to sort the values in this CubicSplineInterpolator by time:
    sortCollection();

    // We are going to scale all of the values here by the duration.
    // so, the higest value in this collection will be mapped to Duration.
    // we do this by specifying a sampling rate for the conversion:

    m_time_type maxTime = get(size() - 1).time_;
    m_value_type mySamplingRate = (getDuration() / maxTime) * getSamplingRate();

    // an iterator for this collection:
    Iterator<InterpolatorEntry> it = iterator();

    // grab the first value:
    InterpolatorEntry csie = it.next();
    m_value_type lastValue = csie.value_;
    m_time_type lastTime = csie.time_;

    // special case for only one entry:
    if (size() == 1) {
        csii->append(lastTime, lastTime, lastValue, lastValue,
                     long(getDuration() * getSamplingRate()));
    }

    while (it.hasNext()) {
        // get the next entry:
        csie = it.next();

        // convert this entry into a CubicSplineInterpolatorIterator::Entry
        m_time_type timeDiff = csie.time_ - lastTime;
        m_sample_count_type steps = (m_sample_count_type)(timeDiff * mySamplingRate);
        // and add it to the iterator:
        csii->append(lastTime, csie.time_, lastValue, csie.value_, steps);

        // update values:
        lastValue = csie.value_;
        lastTime = csie.time_;
    }

    // wrap the abstract iterator as an iterator and return:
    return Iterator<m_value_type>(csii);
}

//----------------------------------------------------------------------------//
interpolation_type CubicSplineInterpolator::getType() { return CUBIC_SPLINE; }

//----------------------------------------------------------------------------//
#endif  // INTERPOLATOR_TYPES_CPP
