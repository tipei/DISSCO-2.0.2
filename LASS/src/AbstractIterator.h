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
//	AbstractIterator.h
//----------------------------------------------------------------------------//

#ifndef __ABSTRACT_ITERATOR_H
#define __ABSTRACT_ITERATOR_H

//----------------------------------------------------------------------------//

/**
 *	This is a templated abstract definition of the most basic iterator.
 *	\author Braden Kowitz
 **/
template <class T>
class AbstractIterator {
public:
    /**
     *	\return An exact copy of this iterator
     **/
    virtual AbstractIterator<T>* clone() = 0;

    /**
     *	\retval true If the iterator has another value
     *	\retval false If the iterator does not have another value
     **/
    virtual bool hasNext() = 0;

    /**
     *	\return The next value in the iterator as a reference
     **/
    virtual T& next() = 0;

    /**
     *	This is the destructor for the iterator.
     **/
    virtual ~AbstractIterator(){};
};

//----------------------------------------------------------------------------//
#endif  //__ABSTRACT_ITERATOR_H
