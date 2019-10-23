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
//	Iterator.h
//
//----------------------------------------------------------------------------//

#ifndef __ITERATOR_H
#define __ITERATOR_H

//----------------------------------------------------------------------------//

#include "AbstractIterator.h"

//----------------------------------------------------------------------------//

/**
 *	This is a wrapper around an AbstractIterator pointer.
 *	This gets around C++'s polymorphic limitations.
 *	It allows for simple management of this AbstractClass.
 *	\author Braden Kowitz
 **/
template <class T>
class Iterator {
private:
    AbstractIterator<T>* it_;

public:
    /**
     *	This is a constructor.
     *	\param it A pointer to an AbstractIterator
     **/
    Iterator(AbstractIterator<T>* it) : it_(it) {}

    /**
     *	This is a copy constructor
     *	\param I The iterator to make a copy of
     **/
    Iterator(const Iterator& I) : it_(I.it_->clone()) {}

    /**
     *	This is the destructor which deletes the underlying object.
     **/
    ~Iterator() { delete it_; }

    /**
     *	This is an overloaded assignment operator.
     *	\param I An iterator to assign
     **/
    Iterator& operator=(const Iterator& I) {
        if (this != &I)  // beware self assignment
        {
            delete it_;
            it_ = I.it_->clone();
        }
        return *this;
    }

    // MEMBER FUNCTIONS

    /**
     *	This checks whether there is another element in the iterator.
     *	\retval true If there is another element
     *	\retval false If there is not another element
     **/
    inline bool hasNext() { return it_->hasNext(); }

    /**
     *	This returns the next iterator.
     *	\return The next iterator.
     **/
    inline T& next() { return it_->next(); }
};

//----------------------------------------------------------------------------//
#endif  //_ITERATOR_H
