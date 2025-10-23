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
//	AllPassFilter.h
//
//----------------------------------------------------------------------------//

#ifndef __ALL_PASS_FILTER_H
#define __ALL_PASS_FILTER_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "SoundSample.h"
#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"
#include "LowPassFilter.h"
#include "Filter.h"
#include "XmlReader.h"

//----------------------------------------------------------------------------//

/**
 * The comb filter class implements an allpass filter
 *  as described on page 385 of F. Richard Moore's 'Elements of Computer Music":
 *
 *  y(t) = -gx(t) + (1-g^2)*(x(t-D) + g*y(t-D))
 *
 * \note This filter (and all other 'class Filter's) are stateful machines with internal feedback mechanisms.  Thus this filter should be reset() each time you begin a new channel and should not be mixed between channels.
 * \author Andrew Kurtz
 * \author Jim Lindstrom
 **/
class AllPassFilter : public Filter
{
public:
	/**
	 * This is the constructor.
	 * \param gain The feedback gain (should be in the range 0.0 to 1.0)
	 * \param delay The allpass delay (in units of samples)
	 **/
	AllPassFilter(float gain, long delay);

	/**
	 * This is the destructor
	 **/
	~AllPassFilter();

	/**
	 * This method applies an allpass filter to a single sample
	 * \param x_t The input sample
	 * \return The sampled filter
	 **/
	m_sample_type do_filter(m_sample_type x_t);

	/**
	 * This method should be redefined by each class derived from Filter
	 * to reset the filter to an initial state.  It should have the 
	 * same effect as deleting the filter and creating a new one.
	 **/
	void reset(void);

	/**
	 * \deprecated
	 * This outputs an XML representation of the object to STDOUT.
	 * \param xmlOutput The output channel
	 **/
	void xml_print( ofstream& xmlOutput );

	/**
	 * This is a constructor.
	 **/
	AllPassFilter();

	/**
	 * This sets the gain.
	 * \param gain The gain as a float
	 **/
	void set_g(float gain);

	/**
	 * This sets the delay.
	 * \param delay The delay as a long
	**/
	void set_D(long delay);

	/**
	 * \deprecated
	 * Reads some xml and sets the gain and delay.
	 * \param apftag A pointer to the xml to read
	**/
	void xml_read( XmlReader::xmltag *apftag);

	float get_g();
	long get_D();
private:
	/**
	 * The gain for the comb component of the filter
	 **/
	float g;

	/**
	 * The square of the gain
	 **/
	float g_sqrd;

	/**
	 * The delay for the comb component of the filter
	 **/
	long D;

	/**
	 * This queue holds past samples to implement the delay
	 **/
	Filter::hist_queue<m_sample_type> *y_hist;

	/**
	 * This queue holds past samples to implement the delay
	 **/
	Filter::hist_queue<m_sample_type> *x_hist;
};

//----------------------------------------------------------------------------//

#endif //___ALL_PASS_FILTER_H
