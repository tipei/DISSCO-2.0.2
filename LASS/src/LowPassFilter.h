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
//	LowPassFilter.h
//
//----------------------------------------------------------------------------//

// Best if viewed with 4-space tabs 

#ifndef __LOW_PASS_FILTER_H
#define __LOW_PASS_FILTER_H

//----------------------------------------------------------------------------//

#include "SoundSample.h"
#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"
#include "Filter.h"

//----------------------------------------------------------------------------//

/**
 *	This is a simple 1st order IIR low-pass filter:
 *
 * y(t) = x(t) + g*y(t-1)
 * <PRE>
 *                       ______________________
 *                      |                      |
 *  x(t) --->(+)------->|        IIR LPF       |--------------------> y(t)
 *            ^         |______________________|           |
 *            |                                            |
 *            |_________________(*g)_______________________|
 * </PRE>
 *
 * \note This filter (and all other filters) are stateful machines with
 * internal feedback mechanisms.  Thus this filter should be allocated anew
 * each time you begin a new channel and should not be mixed between channels.
 *
 * \author Jim LIndstrom
 **/
class LowPassFilter : public Filter
{
public:

	/**
	 * This is a constructor.
	 * \param gain The low-pass feedback gain
	 **/
	LowPassFilter(float gain);

	/**
	 * This is a destructor.
	 **/
	~LowPassFilter();

	/**
	 * This method applies a low-pass filter to a single sample
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
	void xml_print();

private:

	/**
	 * The gain for the filter
	 **/
	float g;

	/**
	 * This queue holds past samples to implement the delay
	 **/
	Filter::hist_queue<m_sample_type> *y_hist;
};

//----------------------------------------------------------------------------//
#endif //__LOW_PASS_FILTER_H
