/*
CMOD (Composition MODule)
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


//---------------------------------------------------------------------------//
//
//    patter.h
//
//    This class generates a pattern of values using a user defined origin
//    and a sequence of intervals and it delivers one value at a time.  
//    Same input may be used to generate various related paterns produced 
//    through mod m equivalences, symmetrical set forms (R, I, RI), and 
//    distortions.  It uses input from user defined files describing the 
//    type of object and its parameters (box, sound, etc.) as well as the 
//    pattern itself (PAT file).
//    \author Sever Tipei     July 14, 2005, July 2007
//
//---------------------------------------------------------------------------//

#ifndef PATTER_H
#define PATTER_H

// CMOD includes
#include "Libraries.h"

#include "Define.h"

//---------------------------------------------------------------------------//

class Patter {
  private:
    std::vector<int> intervals;
    std::vector<int> patty;

    int origin;
    int nextIndex;

    //for use in expand
    std::string expMethod;
    int expModulo;
    int expLow;
    int expHigh;


  public:
        
    /**
     *  Constructor for empty object.  This will be used by eventfactory system
     *  for evaluating patter objects.
     **/
    Patter();

    /**
    *  Advanced constructor.
    *  \param originNum starting point
    *  \param intervalVect list of intervals
    **/
    Patter(int originNum, std::vector<int> intervalVect);

    /**
     *  Copy Constructor
     **/
    Patter(const Patter& p);

    /**
     *  Assignment Operator
     **/
    Patter& operator=(const Patter& rhs);


    /**
    *  Destructor.  Calls Patter::clear();
    **/
    ~Patter();

    /**
     *  Moves the origin of this pattern to a new point
     **/
    void moveOrigin( int newOrigin );

    /**
     *  Returns the next value from the pattern
     *  \param method which way to compute the return value
     **/
    int GetNextValue(std::string method, int newOrigin = 0);

    /**
     *  Creates a patty array by starting with the origin and adding to each 
     *  location the next member of the interval array interArray
     **/
    void SimplePat();

    /**
     *  Coordinates various methos of expanding and/or modifying the pattern: 
     *  equivalence classes, transpositions, symmetric forms (I, R, RI),
     *  distortions (augmentation, diminution, etc,), etc.
     *    ====  presently args refer only to method EQUIVALENCE  ====
     *  \param method (EQUIVALENCE, SYMMETRIES, DISTORT) 
     *  \param modulo 
     *  \param low 
     *  \param high 
     **/
    void Expand(std::string method, int modulo, int low, int high);

    /**
     *  Makes adjustments to an existing pattern
     *    ====  Not available at the present time  ====
     **/
    void Adjust();

  private:

    /**
    *  Finds all intervals allowed by a given range which are equivalent mod m
    *  with the initial list.  The "numTerms" equivalence should not be
    *  reduced since the terms are integers.
    *  \param modulo interval of repeat
    *  \param low lower limit
    *  \param high higher limit
    **/
    void Equivalence(int modulo, int low, int high);

    /**
    *  Creates symmetrical patterns.
    *    ====  Not available at the present time  ====
    *  \param modulo interval of repeat
    *  \param low lower limit
    *  \param high higher limit
    **/
    void Symmetries(int modulo, int low, int high);


    /**
    *  Distorts an existing pattern
    *    ====  Not available at the present time  ====
    *  \param modulo interval of repeat
    *  \param low lower limit
    *  \param high higher limit
    **/
    void Distort(int modulo, int low, int high);

//----------------------------------------------------------------------------//
};
#endif

