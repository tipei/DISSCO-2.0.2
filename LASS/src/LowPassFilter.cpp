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
//	LowPassFilter.cpp
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __LOW_PASS_FILTER_CPP
#define __LOW_PASS_FILTER_CPP

//----------------------------------------------------------------------------//

#include "LowPassFilter.h"

#include "Collection.h"
#include "Filter.h"
#include "MultiTrack.h"
#include "SoundSample.h"
#include "Track.h"

//----------------------------------------------------------------------------//
LowPassFilter::LowPassFilter(float gain) {
    // set parameters
    g = gain;

    // initialize y_hist queue
    y_hist = new Filter::hist_queue<m_sample_type>(1);
    y_hist->enqueue(0.0);
}

//----------------------------------------------------------------------------//
LowPassFilter::~LowPassFilter() { delete y_hist; }

//----------------------------------------------------------------------------//
m_sample_type LowPassFilter::do_filter(m_sample_type x_t) {
    m_sample_type y_t;

    // y(t) = x(t) + g*y(t-1)
    y_t = x_t + (g * y_hist->dequeue());
    y_hist->enqueue(y_t);

    return y_t;
}

//----------------------------------------------------------------------------//
void LowPassFilter::reset() {
    // reinitialize y_hist queue
    delete y_hist;
    y_hist = new Filter::hist_queue<m_sample_type>(1);
    y_hist->enqueue(0.0);
}

//----------------------------------------------------------------------------//
void LowPassFilter::xml_print() {
    cout << "\t\t<LowPassFilter>" << endl;
    cout << "\t\t\t<g value=\"" << g << "\" />" << endl;
    cout << "\t\t</LowPassFilter>" << endl;
}

//----------------------------------------------------------------------------//
#endif  //__LOW_PASS_FILTER_CPP
