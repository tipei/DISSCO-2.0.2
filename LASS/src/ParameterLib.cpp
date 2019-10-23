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
//	ParameterLib.cpp
//
//----------------------------------------------------------------------------//

#ifndef __PARAMETER_LIB_CPP
#define __PARAMETER_LIB_CPP

//----------------------------------------------------------------------------//
#include "ParameterLib.h"

#include "Constant.h"
#include "StandardHeaders.h"

//----------------------------------------------------------------------------//
// CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
ParameterLib<StaticT, DynamicT>::ParameterLib() {
    // nothing
}

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
ParameterLib<StaticT, DynamicT>::ParameterLib(const ParameterLib<StaticT, DynamicT>& pl) {
    // copy member variables
    staticParams_ = pl.staticParams_;

    // and copy over the dynamic variables
    DISSCO_HASHMAP<int, DynamicVariable*>::const_iterator it = pl.dynamicParams_.begin();

    while (it != pl.dynamicParams_.end()) {
        if ((*it).second != 0)  // beware null pointers
        {
            // make a new copy of the DynamicVariable for *this* ParameterLib
            dynamicParams_[(*it).first] = (*it).second->clone();
        }

        it++;
    }
}

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
ParameterLib<StaticT, DynamicT>& ParameterLib<StaticT, DynamicT>::operator=(
    const ParameterLib<StaticT, DynamicT>& pl) {
    if (this != &pl)  // beware self assignment
    {
        // copy member variables
        staticParams_ = pl.staticParams_;

        // delete dynamic memory:
        DISSCO_HASHMAP<int, DynamicVariable*>::const_iterator it = dynamicParams_.begin();
        while (it != dynamicParams_.end()) {
            // (it) is a pair<DynamicT, DynamicVariable*>
            if ((*it).second != 0)  // beware null pointers
            {
                // delete the dynamic memory
                delete (*it).second;
                // NOTE: pointer still says arround, map is cleared later
            }
            it++;
        }
        dynamicParams_.clear();

        // copy over dynamic variables
        it = pl.dynamicParams_.begin();
        while (it != pl.dynamicParams_.end()) {
            if ((*it).second != 0)  // beware null pointers
            {
                // make a new copy of the DynamicVariable for *this*
                // ParameterLib
                dynamicParams_[(*it).first] = (*it).second->clone();
            }

            it++;
        }
    }

    return *this;
}

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
ParameterLib<StaticT, DynamicT>::~ParameterLib() {
    // delete dynamic memory:
    DISSCO_HASHMAP<int, DynamicVariable*>::iterator it = dynamicParams_.begin();

    while (it != dynamicParams_.end()) {
        // (it) is a pair<DynamicT, DynamicVariable*>
        if ((*it).second != 0)  // beware null pointers
        {
            // delete the dynamic memory
            delete (*it).second;
            // NOTE: pointer still says arround, map is cleared later
        }
        it++;
    }

    dynamicParams_.clear();
}

//----------------------------------------------------------------------------//
// PARAMETER ACCESS

//----------------------------------------------------------------------------//
// DYNAMIC VARIABLES

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
void ParameterLib<StaticT, DynamicT>::setParam(DynamicT p, DynamicVariable& v) {
    // is there an entry at this position already?
    DISSCO_HASHMAP<int, DynamicVariable*>::iterator it;
    it = dynamicParams_.find(p);
    if (it != dynamicParams_.end()) {
        // delete the entry
        delete (*it).second;
    }

    // store the new entry
    // make a copy of the passed-in dynamic parameter
    dynamicParams_[p] = v.clone();
}

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
void ParameterLib<StaticT, DynamicT>::setParam(DynamicT p, m_value_type v) {
    Constant c(v);
    setParam(p, c);
}

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
DynamicVariable& ParameterLib<StaticT, DynamicT>::getParam(DynamicT p) {
    // is there an entry at this position already?
    DISSCO_HASHMAP<int, DynamicVariable*>::iterator it;
    it = dynamicParams_.find(p);
    if (it != dynamicParams_.end()) {
        // simply return the entry
        return *((*it).second);
    } else {
        // set the entry to zero, and return the value
        dynamicParams_[p] = new Constant(0.0);
        return *(dynamicParams_[p]);
    }
}

//----------------------------------------------------------------------------//
/*
template<class StaticT, class DynamicT>
    DynamicVariable& ParameterLib<StaticT, DynamicT>::getParamEnv(DynamicT p)
{
  DISSCO_HASHMAP<int, DynamicVariable*>::iterator it;
      it = dynamicParams_.find(p);
    if (it != dynamicParams_.end())
    {
      Envelope env1 (*((*it).second));
        return env1;
    }

    else
      return;
}
*/

//----------------------------------------------------------------------------//
// STATIC VARIABLES

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
void ParameterLib<StaticT, DynamicT>::setParam(StaticT p, m_value_type v) {
    staticParams_[p] = v;
}

//----------------------------------------------------------------------------//
template <class StaticT, class DynamicT>
m_value_type ParameterLib<StaticT, DynamicT>::getParam(StaticT p) {
    // is there an entry at this position already?
    DISSCO_HASHMAP<int, m_value_type>::iterator it;
    it = staticParams_.find(p);
    if (it != staticParams_.end()) {
        // return the entry
        return (*it).second;
    } else {
        // set the entry to 0.0, and return 0.0;
        staticParams_[p] = 0.0;
        return 0.0;
    }
}

//----------------------------------------------------------------------------//
#endif  //__PARAMETER_LIB_CPP
