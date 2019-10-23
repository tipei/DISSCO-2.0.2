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
//	LPCombFilter.h
//
//----------------------------------------------------------------------------//

// Best if viewed with 4-space tabs

#ifndef __LP_COMB_FILTER_H
#define __LP_COMB_FILTER_H

//----------------------------------------------------------------------------//
#include "Collection.h"
#include "Filter.h"
#include "LowPassFilter.h"
#include "MultiTrack.h"
#include "SoundSample.h"
#include "StandardHeaders.h"
#include "Track.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//

/**
 * This is a comb filter with a lowpass feedback element.
 * The comb filter class implements a comb filter with a lowpass feedback loop
 * as described on page 385 of 'Elements of Computer Music':
 *
 *  y(t) = x[t-D] + g * lowpass(x(t-D))<BR>
 *   where lowpass(s[t]) = s[t] + coef*lowpass(s[t-1])
 *
 *   Note that for stability, we must make sure that: -1 < g/(1 - coef) < +1
 * <PRE>
 *                       ______________________
 *                      |                      |
 *  x(t) --->(+)------->|    D-sample Delay    |--------------------> y(t)
 *            ^         |______________________|           |
 *            |                                            |
 *            |          ______________________            |
 *            |         |                      |           |
 *            |-(*g)----| IIR Lowpass Feedback |<--(+)-----|
 *                   |  |______________________|    ^
 *                   |                              |
 *                   |________(*coef)_______________|
 * </PRE>
 *
 * \note: This filter (and all other 'class Filter's) are stateful machines with
 * internal feedback mechanisms.  Thus this filter should be reset()
 * each time you begin a new channel and should not be mixed between channels.
 *
 * \author Jim Lindstrom
 **/
class LPCombFilter : public Filter {
public:
    /**
     * This is a constructor.
     * \param gain The feedback gain (0.0 to 1.0) applied to the IIR
     *             lowpass feedback unit
     * \param delay The comb delay, in units of samples.
     * \param lpf_gain The internal gain of the lowpass feedback unit
     **/
    LPCombFilter(float gain, long delay, float lpf_gain);

    /**
     * This is the destructor.
     **/
    ~LPCombFilter();

    /**
     * This method applies a lpcomb filter to a single sample
     * \param x_t The input sample
     * \return The filtered sample
     **/
    m_sample_type do_filter(m_sample_type x_t);

    /**
     * This method should be redefined by each class derived from Filter to
     * reset the filter to an initial state.  It should have the same effect
     * as deleting the filter and creating a new one.
     **/
    void reset(void);

    /**
     *	\deprecated
     *   This outputs an XML representation of the object to STDOUT
     **/
    void xml_print(ofstream &xmlOutput);

    /**
     * This constructor is used when recreating the object from an XML
     * file when you don't know all the parameters until you've read
     * them in.  Don't use this constructor unless you intend to use
     * the proper calls to set the gain, delay, and lpf.
     **/
    LPCombFilter();

    /**
     * This sets the gain.
     * \param new_g The gain
     **/
    void set_g(float new_g);

    /**
     * This sets the delay.
     * \param D The delay
     **/
    void set_D(long D);

    /** This sets the low-pass feedback.
     * \param new_lpf_g
     **/
    void set_lpf_g(float new_lpf_g);

    /**
     *	\deprecated
     **/
    void xml_read(XmlReader::xmltag *lptag);

private:
    /**
     * The gain for the comb component of the filter
     **/
    float g;

    /**
     * The delay for the comb component of the filter
     **/
    long D;

    /**
     * The gain for the lowpass-feedback component of the filter
     **/
    float lpf_g;

    /**
     * This implements the lowpass-feedback portion of the filter
     **/
    LowPassFilter *lpf;

    /**
     * This queue holds past samples to implement the delay
     **/
    Filter::hist_queue<m_sample_type> *x_hist;
};

//----------------------------------------------------------------------------//

#endif  //__LP_COMB_FILTER_H
