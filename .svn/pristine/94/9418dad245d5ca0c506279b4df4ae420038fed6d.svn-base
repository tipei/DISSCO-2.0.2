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
//	AllPassFilter.cpp
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __ALL_PASS_FILTER_CPP
#define __ALL_PASS_FILTER_CPP
//----------------------------------------------------------------------------//

#include "SoundSample.h"
#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"
#include "Filter.h"
#include "LPCombFilter.h"
#include "LowPassFilter.h"
#include "AllPassFilter.h"

//----------------------------------------------------------------------------//

AllPassFilter::AllPassFilter(float gain, long delay)
{
	// set parameters
	g = gain;
	g_sqrd = g*g;
	D = delay;


	// initialize x_hist queue and y_hist queue
	x_hist = new Filter::hist_queue<m_sample_type>(D);
	for(long i=0; i<D; i++)
		x_hist->enqueue(0.0);

        y_hist = new Filter::hist_queue<m_sample_type>(D);
        for(long j=0; j<D; j++)
                y_hist->enqueue(0.0);
}
//----------------------------------------------------------------------------//

AllPassFilter::~AllPassFilter()
{
	delete y_hist;
	delete x_hist;
}

//----------------------------------------------------------------------------//

m_sample_type AllPassFilter::do_filter(m_sample_type x_t)
{
	m_sample_type y_t;
        
	y_t=-g*x_t + (1-g_sqrd)*(x_hist->dequeue() + g*y_hist->dequeue()); 
	x_hist->enqueue(x_t);
        y_hist->enqueue(y_t);
	return y_t;
}

//----------------------------------------------------------------------------//

void AllPassFilter::reset()
{
	delete y_hist;
	delete x_hist;


	// initialize x_hist queue and y_hist queue
	x_hist = new Filter::hist_queue<m_sample_type>(D);
	for(long i=0; i<D; i++)
		x_hist->enqueue(0.0);
  
        y_hist = new Filter::hist_queue<m_sample_type>(D);
        for(long j=0; j<D; j++)
                y_hist->enqueue(0.0);       
}

//----------------------------------------------------------------------------//

void AllPassFilter::xml_print( ofstream& xmlOutput )
{

	xmlOutput << "\t<AllPassFilter>" << endl;
	xmlOutput << "\t\t<g value=\"" << g << "\" />" << endl;
	//cout << "\t\t<g_sqrd value=\"" << g_sqrd << "\" />" << endl;
	xmlOutput << "\t\t<D value=\"" << D << "\" />" << endl;
	xmlOutput << "\t</AllPassFilter>" << endl;

}
//----------------------------------------------------------------------------//

AllPassFilter::AllPassFilter()
{
}

void AllPassFilter::set_g(float gain)
{
	g = gain;
	g_sqrd = g*g;
}
void AllPassFilter::set_D(long delay)
{
	D = delay;

	// initialize x_hist queue and y_hist queue
	x_hist = new Filter::hist_queue<m_sample_type>(D);
	for(long i=0; i<D; i++)
		x_hist->enqueue(0.0);
        
        y_hist = new Filter::hist_queue<m_sample_type>(D);
        for(long j=0; j<D; j++)
                y_hist->enqueue(0.0);         

}

void AllPassFilter::xml_read(XmlReader::xmltag *apftag)
{
	char *value;
	if((value = apftag->findChildParamValue("g","value")) != 0)
		set_g(atof(value));
	
	if((value = apftag->findChildParamValue("D","value")) != 0)
		set_D(atoi(value));
}

//----------------------------------------------------------------------------//

#endif //__ALL_PASS_FILTER_CPP

