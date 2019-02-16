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
//	ParameterLib.h	
//
//----------------------------------------------------------------------------//

#ifndef __PARAMETER_LIB_H
#define __PARAMETER_LIB_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"  
#include "Types.h"
#include "DynamicVariable.h"

//----------------------------------------------------------------------------//

/**
*	A parameter lib object stores and retrieves DynamicVariables
*	and static variables by given indices which are represented as enums.
*
*	\author Braden Kowitz
**/
template<class StaticT, class DynamicT> class ParameterLib
{
public:

    // CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

    /**
    *	This is the default constructor.
    **/
    ParameterLib();
    
    /**
    *	This is a copy constructor, which makes a duplicate of the given
    *	ParameterLib
    *	\param pl The ParameterLib to copy
    **/
    ParameterLib(const ParameterLib<StaticT, DynamicT>& pl);
    
    /**
    *	This is an overloaded assignment operator.
    *	\param pl The ParameterLib to assign
    **/
    ParameterLib<StaticT, DynamicT>& operator=(
        const ParameterLib<StaticT,
        DynamicT>& pl);
    
    /**
    *	This is the destructor.
    **/
    virtual ~ParameterLib();

    // --- PARAMETER ACCESS ---

    // DYNAMIC VARIABLES
    
    /**
    *	This sets a DynamicVariable to the specified setting.
    *	The dynamic variable is copied into this object.
    *	\param p The DynamicVariable to set
    *	\param v The new DynamicVariable
    **/
    void setParam(DynamicT p, DynamicVariable& v);
    
    /**
    *	This sets a dynamic variable with an m_value_type.
    *	Behind the scenes, a Constant object is created.
    *	\param p The DynamicVariable to set
    *	\param v The new value type
    **/
    void setParam(DynamicT p, m_value_type v);

    /**
    *	Returns a reference to the dynamic variable at
    *	a position.  If no variable exists, a Constant
    *	object set to zero is used instead.
    *	\param p The position of the DynamicVariable
    *	\return A reference to the DynamicVariable
    **/
    DynamicVariable& getParam(DynamicT p);


    /**                                                                                                                   
     *   Returns a reference to the envelop used on the                                                                                                                                            
     *   object.
     **/
//    DynamicVariable& getParamEnv(DynamicT p);
    
    // STATIC VARIABLES
    
    /**
    *	This sets a static parameter of this object.
    *	\param p The static parameter
    *	\param v The new value
    **/
    void setParam(StaticT p, m_value_type v);
    
    /**
    *	This returns a static parameter of this object.
    *	If no parameter was ever set, it returns 0.0.
    *	\param p The position of the static parameter
    *	\return The value of the static parameter
    **/
    m_value_type getParam(StaticT p);
    
private:
    
    /**
    *	Holds the static parameters in an association
    **/
    DISSCO_HASHMAP<int , m_value_type> staticParams_;
    
    /**
    *	Holds the dynamic parameters in an association
    **/
    DISSCO_HASHMAP<int , DynamicVariable*> dynamicParams_;
    
};


//----------------------------------------------------------------------------//
// this is needed because this is a templated class.
// Is there a better solution to this problem?
#include "ParameterLib.cpp"

//----------------------------------------------------------------------------//
#endif //__PARAMETER_LIB_H
