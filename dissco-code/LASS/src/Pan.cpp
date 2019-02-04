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
//	Pan.cpp		
//
//----------------------------------------------------------------------------//

#ifndef __PAN_CPP
#define __PAN_CPP

//----------------------------------------------------------------------------//

#include "Pan.h"

#include "Iterator.h"

//----------------------------------------------------------------------------//

Pan::Pan(DynamicVariable& v)
    :panVar_(v.clone()){}
Pan::~Pan(){
  delete panVar_;
  //cout<<"pan descructor called"<<endl;
}    
void Pan::set(DynamicVariable& v)
{
    delete panVar_;
    panVar_ = v.clone();
}

Pan* Pan::clone()
{   
    return new Pan(*panVar_);
}

MultiTrack* Pan::spatialize(Track& t, int numTracks)
{
    m_sample_count_type sampleCount = t.getWave().getSampleCount();
    m_rate_type samplingRate = t.getWave().getSamplingRate();
   
    // create an empty multi Track object:
    MultiTrack* mt = new MultiTrack(numTracks, sampleCount, samplingRate);
   
    // tell panVar_ this information.
    panVar_->setSamplingRate(samplingRate);
    panVar_->setDuration( float(sampleCount) / float(samplingRate));
    
    // get references to the input
    SoundSample& inWave = t.getWave();
    SoundSample& inAmp = t.getAmp();


    // for each channel:
    for (int c=0; c<numTracks; c++)
    {
        // get an iterator for the pan variable:
        Iterator<m_value_type> panIter = panVar_->valueIterator();
        
        // get references for this channel.
        SoundSample& thisWave = mt->get(c)->getWave();
        SoundSample& thisAmp = mt->get(c)->getAmp();

        // create a position value for this channel:
        m_value_type pos = float(c) / float(numTracks-1);
        
        // Iterate over each sample
        m_value_type scale;
        for (m_sample_count_type i=0; i<sampleCount; i++)
        {
            // calculate the scaling factor:
            scale = 1.0 - fabs(pos - panIter.next());
            if (scale > 1.0) scale = 1.0;
            if (scale < 0.0) scale = 0.0;
            
            thisWave[i] = scale * inWave[i];
            thisAmp[i] = scale * inAmp[i];
        }
    }
     
    // Return the multitrack object
    return mt;
}

void Pan::xml_print( ofstream& xmlOutput )
{
	xmlOutput << "\t\t<pan>" << endl;
	
	xmlOutput << "\t\t</pan>" << endl;
}



//----------------------------------------------------------------------------//
#endif //__PAN_CPP
