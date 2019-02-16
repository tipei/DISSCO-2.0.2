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
//	Collection.h
//
//----------------------------------------------------------------------------//

#ifndef __COLLECTION_H
#define __COLLECTION_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "Iterator.h"
#include "AbstractIterator.h"

//----------------------------------------------------------------------------//

/**
*	Represents a collection of objects.  They are kept in order,
*	with consecutive index numbers running from 0 to (size-1).
*	Each entry is also accompanied by a string name.
*	Note: This class is heavily modeled from Java Collection and Vector 
*	\author Braden Kowitz
**/
template<class T> class Collection
{
public:
    
    // CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

    /**
    *	This is the default constructor, which creates an empty collection.
    **/
    Collection();
    
    /**
    *	This is a copy constructor.
    *	\param c The Collection from which to make a copy
    **/
    Collection(const Collection& c);

    /**
    *	This is the destructor.
    **/
    virtual ~Collection();

    /**
    *	This assigns one Collection to another.
    *	\param c The Collection to assign
    **/
    Collection& operator=(const Collection& c);
    
    // PUBLIC METHODS
    
    /**
    *	Appends the given element to the end of the Collection.
    *	\note This will make a COPY of the element in the Collection.
    *	\param element An element to add to the collection
    **/
    void add(const T& element);
    
    /**
    *	Inserts the given element at the specified index.
    *	The element at that index, and subsequent elements
    *	are shifted to the right.
    *	\note This will make a COPY of the element in the Collection.
    *	\exception IndexOutOfBoundsException
    *   \param index An integer specifying where to insert the element
    *	\param element The element to insert
    **/
    void add(int index, const T& element);
    
    /**
    *	Clears every element from the collection.
    **/
    void clear();
    
    /**
    *	\exception IndexOutOfBoundsException
    *	\param index An integer specifying which element to retrieve
    *	\return A reference to the element at the specified index
    **/
    T& get(int index);
    
    /**
    *	Removes the element at the specified index.
    *	All other elements are shifted left to fill the hole.
    *	\exception IndexOutOfBoundsException
    *   \param index An integer specifying which element to delete
    *   \return A reference to the element at the specified index
    **/
    T remove(int index);
    
    /**
    *	Replaces the element at the specified index.
    *   \exception IndexOutOfBoundsException
    *   \param index An integer specifying which element to replace
    *   \param element An element to replace the old one 
    *	\return A reference to the element that was replaced
    **/
    T set(int index, const T& element);
    
    /**
    *	Returns the size of this collection.
    *	This is a 0-based array so the entries 
    *	are indexed from 0 to (size-1).
    *   \return The size of the collection
    **/
    int size();

    /**
    *	This sorts the collection.
    *	\note only use this function if template type of this collection has well defined operator< , operator== , and operator> . Examples of this are standard types like int and float, as well as custom types like LinearInterpolatorEntry.
    **/
    void sortCollection();
    
    /**
    *	Returns an iterator over the elements in this collection.
    *   \return an iteration
    **/
    Iterator<T> iterator();
    
    void printPointers(char * collType);

private:
    
    vector<T> vector_;

    /**
    *   An abstract iterator that makes it possible to iterate over
    *   all values in a collection.
    **/
    class CollectionIterator : public AbstractIterator<T>
    {
    public:

        /**
        *	Constructor that takes two STL vector iterators
        *	over which the iterator will iterate.
        **/
        CollectionIterator(typename vector<T>::iterator it, typename vector<T>::iterator end);

        /**
        *	Creates an exact copy of this iterator.
	*	\return exact copy of this iterator
        **/
        CollectionIterator* clone();
    
        /**
        *	Indicates whether the iterator has another value.
	*	\return true If iterator has another value
	*	\return false If iterator does not have another value
        **/
        bool hasNext();
        
        /**
        *	Gets the next value for the iterator.
	*	\return the next iterator
	*	\todo What to do when out of bounds?
        **/
        T& next();
        
    private:
    
        /**
        *	Iterator which this class wraps.
        **/
        typename vector<T>::iterator it_;
        
        /**
        *	The end position, so this iterator knows when to stop.
        **/
        typename vector<T>::iterator end_;
    };
};

//----------------------------------------------------------------------------//
// this is needed because this is a templated class.
// Is there a better solution to this problem?
#include "Collection.cpp"

//----------------------------------------------------------------------------//
#endif //__COLLECTION_H


