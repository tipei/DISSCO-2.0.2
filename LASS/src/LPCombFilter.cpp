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
//	LPCombFilter.cpp
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __LP_COMB_FILTER_CPP
#define __LP_COMB_FILTER_CPP

//----------------------------------------------------------------------------//

#include "LPCombFilter.h"

#include "Collection.h"
#include "Filter.h"
#include "LowPassFilter.h"
#include "MultiTrack.h"
#include "SoundSample.h"
#include "Track.h"

//----------------------------------------------------------------------------//
LPCombFilter::LPCombFilter(float gain, long delay, float lpf_gain) {
    // set parameters
    g = gain;
    D = delay;
    lpf_g = lpf_gain;

    // create the lowpass-feedback filter
    lpf = new LowPassFilter(lpf_g);

    // initialize x_hist queue
    x_hist = new Filter::hist_queue<m_sample_type>(D);
    for (long i = 0; i < D; i++) x_hist->enqueue(0.0);
}
//----------------------------------------------------------------------------//
LPCombFilter::~LPCombFilter() {
    delete lpf;
    delete x_hist;
}

//----------------------------------------------------------------------------//
m_sample_type LPCombFilter::do_filter(m_sample_type x_t) {
    m_sample_type y_t;

    y_t = x_hist->dequeue();
    x_hist->enqueue(x_t + (g * lpf->do_filter(y_t)));

    return y_t;
}

//----------------------------------------------------------------------------//
void LPCombFilter::reset() {
    // recreate the low-pass-feedback unit and the x-history queue
    delete lpf;
    delete x_hist;

    // create the lowpass-feedback filter
    lpf = new LowPassFilter(lpf_g);

    // initialize x_hist queue
    x_hist = new Filter::hist_queue<m_sample_type>(D);
    for (long i = 0; i < D; i++) x_hist->enqueue(0.0);
}

//----------------------------------------------------------------------------//
void LPCombFilter::xml_print(ofstream &xmlOutput) {
    xmlOutput << "\t<LPCombFilter>" << endl;

    xmlOutput << "\t\t<g value=\"" << g << "\" />" << endl;
    xmlOutput << "\t\t<D value=\"" << D << "\" />" << endl;
    xmlOutput << "\t\t<lpf_g value=\"" << lpf_g << "\" />" << endl;
    // lpf->xml_print();

    xmlOutput << "\t</LPCombFilter>" << endl;
}

//----------------------------------------------------------------------------//
LPCombFilter::LPCombFilter() {}
void LPCombFilter::set_g(float new_g) { g = new_g; }
void LPCombFilter::set_D(long new_D) {
    D = new_D;

    // initialize x_hist queue
    x_hist = new Filter::hist_queue<m_sample_type>(D);
    for (long i = 0; i < D; i++) x_hist->enqueue(0.0);
}
void LPCombFilter::set_lpf_g(float new_lpf_g) {
    lpf_g = new_lpf_g;

    // create the lowpass-feedback filter
    lpf = new LowPassFilter(lpf_g);
}

void LPCombFilter::xml_read(XmlReader::xmltag *lptag) {
    char *value;
    if ((value = lptag->findChildParamValue("g", "value")) != 0) set_g(atof(value));
    if ((value = lptag->findChildParamValue("D", "value")) != 0) set_D(atoi(value));
    if ((value = lptag->findChildParamValue("lpf_g", "value")) != 0) set_lpf_g(atof(value));
}

//----------------------------------------------------------------------------//
#endif  //__LP_COMB_FILTER_CPP
