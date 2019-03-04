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
//	Interpolator.cpp
//
//
//----------------------------------------------------------------------------//

#ifndef __INTERPOLATOR_CPP
#define __INTERPOLATOR_CPP

//----------------------------------------------------------------------------//

#include "Interpolator.h"
#include "InterpolatorIterator.h"

//----------------------------------------------------------------------------//
Interpolator::Interpolator()
{
    // we don't have to do anything
}

//----------------------------------------------------------------------------//
void Interpolator::addEntry(m_time_type time, m_value_type value)
{
    //Create a new entry:
    InterpolatorEntry ie(time,value);
    //add the entry to this collection
    add(ie);
}

//----------------------------------------------------------------------------//
void Interpolator::scale(m_value_type scale)
{
    // iterate through this collection:
    Iterator<InterpolatorEntry> it = iterator();
    while (it.hasNext())
    {
        it.next().value_ *= scale;
    }
}


//----------------------------------------------------------------------------//
m_value_type Interpolator::getMaxValue()
{
    Iterator<InterpolatorEntry> it = iterator();
    
    m_value_type val = 0.0;
    m_value_type maxVal = 0.0;
    while (it.hasNext())
    {
        val = it.next().value_;
        if (val > maxVal) maxVal = val;
    }
    
    return maxVal;
}

//----------------------------------------------------------------------------//
void Interpolator::xml_print( ofstream& xmlOutput )
{
	DynamicVariable* pnt2dyn = this;

        xmlOutput << "<dv id=\"" << (long)pnt2dyn << "\">" << endl;
        xmlOutput << "\t<dv_type value=\"interp\" />" << endl;
        xmlOutput << "\t<interp_type value=\"" << getType() << "\" />" << endl;
        xmlOutput << "\t<duration value=\"" << getDuration() << "\" />" << endl;
        xmlOutput << "\t<rate value=\"" << getSamplingRate() << "\" />" << endl;

        //Print out private vars and collections for Interpolator here
	InterpolatorEntry myent(0,0);
	Iterator<InterpolatorEntry> it = iterator();
	while(it.hasNext())
	{
		myent = it.next();
		xmlOutput << "\t<entry time=\"" << myent.time_ << "\" ";
		xmlOutput << "value=\"" << myent.value_ << "\" />" << endl;
	}

	xmlOutput << "</dv>" << endl;
}


//----------------------------------------------------------------------------//
void Interpolator::xml_print( ofstream& xmlOutput, list<DynamicVariable*>& dynObjs )
{

        DynamicVariable* pnt2dyn = this;

        //Print the pointer value as an ID, then the "meat" gets printed later
        xmlOutput << "\t\t\t\t<dv_type value=\"interp\" />" << endl;
        xmlOutput << "\t\t\t\t<dv_ptr id=\"" << (long)pnt2dyn << "\" />" << endl;

        // Update dynamic variable list if necessary
        list<DynamicVariable*>::const_iterator dynit;
        for( dynit=dynObjs.begin(); dynit != dynObjs.end(); dynit++ )
        {
                if( (*dynit) == this )
                        break;
        }
        if( dynit == dynObjs.end() ){
                dynObjs.push_back( this );
        }

}

void Interpolator::xml_read(XmlReader::xmltag* envtag)
{
	char* value = 0;
	XmlReader::xmltag *entrytag;
	
	if((value = envtag->findChildParamValue("duration","value")) != 0)
	{
		setDuration(atof(value));
	}
	
	if((value = envtag->findChildParamValue("rate","value")) != 0)
	{
		setSamplingRate(atoi(value));
	}
	
	while((entrytag = envtag->children->find("entry")) != 0)
	{
		float time = 0.0f;
		float val = 0.0f;
		if((value=entrytag->getParamValue("time")) != 0)
			time = atof(value);
		if((value=entrytag->getParamValue("value")) != 0)
			val = atof(value);
		InterpolatorEntry ie(time,val);
		
		//Add the entry to this collection.
    add(ie);
	}
}

//----------------------------------------------------------------------------//
#endif //__INTERPOLATOR_CPP
