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
//	DynamicVariable.cpp
//
//----------------------------------------------------------------------------//

#ifndef __DYNAMIC_VARIABLE_CPP
#define __DYNAMIC_VARIABLE_CPP

//----------------------------------------------------------------------------//
//#include "Score.h"
#include "DynamicVariable.h"

#include "Constant.h"
#include "DynamicVariableSequence.h"
#include "Envelope.h"
#include "Interpolator.h"
#include "InterpolatorTypes.h"
#include "Types.h"

//----------------------------------------------------------------------------//
DynamicVariable::DynamicVariable() : duration_(1.0), rate_(DEFAULT_SAMPLING_RATE) {}

//----------------------------------------------------------------------------//
void DynamicVariable::setDuration(m_time_type duration) { duration_ = duration; }

//----------------------------------------------------------------------------//
m_time_type DynamicVariable::getDuration() { return duration_; }

//----------------------------------------------------------------------------//
void DynamicVariable::setSamplingRate(m_rate_type rate) { rate_ = rate; }

//----------------------------------------------------------------------------//
m_rate_type DynamicVariable::getSamplingRate() { return rate_; }

//----------------------------------------------------------------------------//
m_sample_count_type DynamicVariable::getSampleCount() {
    return (m_sample_count_type)(rate_ * duration_);
}

//----------------------------------------------------------------------------//
// DynamicVariable* DynamicVariable::create_dv_from_xml(XmlReader::xmltag
// *dvtag, hash_map<long,DynamicVariable*> dvHash)
DynamicVariable *DynamicVariable::create_dv_from_xml(XmlReader::xmltag *dvtag) {
    // First look up the dv_type which is always a child tag

    char *value = dvtag->findChildParamValue("dv_type", "value");

    DynamicVariable *dv = NULL;

    // Make a new constant
    if (!strcmp(value, "constant")) {
        Constant *constdv = new Constant;
        constdv->xml_read(dvtag);
        dv = constdv;
    }
    // Deprecated; ignoring.
    /*
            else if(!strcmp(value,"dvs"))
            {
                    value=dvtag->findChildParamValue("dv_ptr","value");
                    long id=atoi(value);
            }
    */
    else if (!strcmp(value, "envelope")) {
        Envelope *envdv = new Envelope;
        envdv->xml_read(dvtag);
        dv = envdv;
    } else if (!strcmp(value, "interp")) {
        int interp_type = atoi(dvtag->findChildParamValue("interp_type", "value"));
        Interpolator *interp = NULL;
        if (interp_type == EXPONENTIAL)
            interp = new ExponentialInterpolator;
        else if (interp_type == LINEAR)
            interp = new LinearInterpolator;
        else if (interp_type == CUBIC_SPLINE)
            interp = new CubicSplineInterpolator;
        interp->xml_read(dvtag);
        dv = interp;
    }

    if (!dv) {
        printf("[%s:%d] Error creating DV: Unknown type %s\n", __FILE__, __LINE__, value);
        return new Constant;
    }

    return dv;
}

//----------------------------------------------------------------------------//
#endif  //__DYNAMIC_VARIABLE_CPP
