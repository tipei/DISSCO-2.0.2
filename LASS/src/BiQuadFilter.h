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
//	BiQuadFilter.h
//
//----------------------------------------------------------------------------//

// Best if viewed with 4-space tabs 

#ifndef __BI_QUAD_FILTER_H
#define __BI_QUAD_FILTER_H

//----------------------------------------------------------------------------//

#include "SoundSample.h"
#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"
#include "Filter.h"

//----------------------------------------------------------------------------//

/**
 *This program implements the biquad transfer function 
 *            b0 + b1*z^-1 + b2*z^-2
 *   H(z) = ------------------------                                 
 *           a0 + a1*z^-1 + a2*z^-2
 * according to the specs at  http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt.
 * low pass filter, High pass filter,band pass filter , Notch Filter , Peaking band EQ filter,
 * Low shelf filter, High shelf filters are implemented 
 * author Mert Bay 
 **/
class BiQuadFilter : public Filter
{
public:

	/**
	 * This is a constructor.
	 * \param gain The low-pass feedback gain
	 **/
	BiQuadFilter(float gain);


	/**
	 * This is a constructor.
	 * \param gain for the 3 tap low-pass feedback gain
	 **/
	BiQuadFilter(int type, m_sample_type dbGain, /* gain of filter */
                          m_sample_type freq,             /* center frequency */
                          m_sample_type srate,            /* sampling rate */
                          m_sample_type bandwidth);       /* bandwidth in octaves */
	/**
	 * This is a destructor.
	 **/
	~BiQuadFilter();

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
	float gains[3];
	/**
	 * This queue holds past samples to implement the delay
	 **/
	Filter::hist_queue<m_sample_type> *y_hist;
	Filter::hist_queue<m_sample_type> *x_hist;



    	m_sample_type ba0, ba1, ba2, ba3, ba4;
    	m_sample_type bx1, bx2, by1, by2;

/* filter types */
	enum {
    	LPF, /* low pass filter */
    	HPF, /* High pass filter */
    	BPF, /* band pass filter */
    	NOTCH, /* Notch Filter */
    	PEQ, /* Peaking band EQ filter */
    	LSH, /* Low shelf filter */
    	HSH /* High shelf filter */
	};




};

//----------------------------------------------------------------------------//
#endif //__BI_QUAD_FILTER_H
