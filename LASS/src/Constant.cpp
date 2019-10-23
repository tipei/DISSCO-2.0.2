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
//	Constant.cpp
//
//----------------------------------------------------------------------------//

#ifndef __CONSTANT_CPP
#define __CONSTANT_CPP

//----------------------------------------------------------------------------//

#include "Constant.h"

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Constant::Constant(m_value_type value) : value_(value) {}

//----------------------------------------------------------------------------//
Constant* Constant::clone() { return new Constant(*this); }

//----------------------------------------------------------------------------//
void Constant::setValue(m_value_type value) { value_ = value; }

//----------------------------------------------------------------------------//
m_value_type Constant::getValue() { return value_; }

//----------------------------------------------------------------------------//
Iterator<m_value_type> Constant::valueIterator() {
    return Iterator<m_value_type>(new ConstantIterator(value_, getSampleCount()));
}

//----------------------------------------------------------------------------//
void Constant::scale(m_value_type factor) { value_ *= factor; }

//----------------------------------------------------------------------------//
m_value_type Constant::getMaxValue() { return value_; }

//----------------------------------------------------------------------------//
// Constant::ConstantIterator

//----------------------------------------------------------------------------//
Constant ::ConstantIterator ::ConstantIterator(m_value_type value, m_sample_count_type count)
    : value_(value), count_(count) {}

//----------------------------------------------------------------------------//
Constant::ConstantIterator* Constant ::ConstantIterator ::clone() {
    return new ConstantIterator(value_, count_);
}

//----------------------------------------------------------------------------//
inline bool Constant ::ConstantIterator ::hasNext() { return (count_ > 0); }

//----------------------------------------------------------------------------//
inline m_value_type& Constant ::ConstantIterator ::next() {
    count_--;
    return value_;
}

//----------------------------------------------------------------------------//
void Constant::xml_print(ofstream& xmlOutput) {
    xmlOutput << "\t\t\t\t<dv_type value=\"constant\" />" << endl;
    xmlOutput << "\t\t\t\t<duration value=\"" << getDuration() << "\" />" << endl;
    xmlOutput << "\t\t\t\t<rate value=\"" << getSamplingRate() << "\" />" << endl;

    // Print out private vars and collections for Constant here
    xmlOutput << "\t\t\t\t<value value=\"" << value_ << "\" />" << endl;
}

//----------------------------------------------------------------------------//
void Constant::xml_read(XmlReader::xmltag* constanttag) {
    char* value;

    if ((value = constanttag->findChildParamValue("duration", "value")) != 0)
        setDuration(atof(value));

    if ((value = constanttag->findChildParamValue("rate", "value")) != 0)
        setSamplingRate(atoi(value));

    if ((value = constanttag->findChildParamValue("value", "value")) != 0) setValue(atof(value));
}

//----------------------------------------------------------------------------//
void Constant::xml_print(ofstream& xmlOutput, list<DynamicVariable*>& dynObjs) {
    dynObjs.size();  // remove warning about unused parameter...
    xml_print(xmlOutput);
}

//----------------------------------------------------------------------------//
#endif  //__CONSTANT_CPP
