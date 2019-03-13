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
//	Collection.cpp
//
//----------------------------------------------------------------------------//

#ifndef __COLLECTION_CPP
#define __COLLECTION_CPP

//----------------------------------------------------------------------------//

#include "Collection.h"

//----------------------------------------------------------------------------//
// CONSTRUCTORS, DESTRUCTOR, ASSIGNMENT

//----------------------------------------------------------------------------//
template<class T> Collection<T>::Collection()
{
}
    
//----------------------------------------------------------------------------//
template<class T> Collection<T>::Collection(const Collection<T>& c)
{
    // I think this implementation should work:
    // (not for collections of pointers, beware!)
    if(this != &c)
        vector_ = c.vector_;
    else
        cout << "Bad copy in Collection from " << &c.vector_ << " to " << &vector_ << endl;
}

//----------------------------------------------------------------------------//
template<class T> Collection<T>::~Collection()
{
    // there should be no dynamic memory to delete.
}

//----------------------------------------------------------------------------//
template<class T> Collection<T>& Collection<T>::operator=(const Collection<T>& c)
{
    if (this != &c) // beware self assignment
    {
        // clear this vector
        clear();
        
        // copy every element of c into this instance
        typename vector<T>::iterator it = vector_.begin();
        
        while(it != vector_.end())
        {
            add(*it);
            it++;
        }
    }

    return *this;
}
    
//----------------------------------------------------------------------------//
// PUBLIC METHODS

//----------------------------------------------------------------------------//
template<class T> void Collection<T>::add(const T& element)
{
    //vector_.push_back(element);
    // get an iterator pointing at the index
    typename vector<T>::iterator it = vector_.end();
    //for (int i=0; i<vector; i++)
    //	it++;
    vector_.insert(it,element);
}
 
//----------------------------------------------------------------------------//   
template<class T> void Collection<T>::add(int index, const T& element)
{
    if ( (index >= 0) && (index < (int) vector_.size()))
    {
        // get an iterator pointing at the index
        typename vector<T>::iterator it = vector_.begin();
        for (int i=0; i<index; i++)
            it++;
        vector_.insert(it,element);
    }
    else
    {
        cerr << "ERROR: Collection.add() Index out of bounds: " << index << endl;
    }
}

//----------------------------------------------------------------------------//    
template<class T> void Collection<T>::clear()
{
    vector_.clear();
}
    
//----------------------------------------------------------------------------//
template<class T> T& Collection<T>::get(int index)
{
    if ( (index >= 0) && (index < (int) vector_.size()))
    {
        return vector_[index];
    }
    else
    {
        cerr << "ERROR: Collection.get() Index out of bounds: " << index << endl;
        //int *x = 0; int y = *x; //Crash
        return vector_[0];
    }
}
    
//----------------------------------------------------------------------------//
template<class T> T Collection<T>::remove(int index)
{
    if ( (index >= 0) && (index < (int) vector_.size()))
    {
        // get an iterator pointing at the index
        typename vector<T>::iterator it = vector_.begin();
        for (int i=0; i<index; i++)
            it++;
        T old = *it;
        vector_.erase(it);
        return old;
    }
    else
    {
        cerr << "ERROR: Collection.remove() Index out of bounds: " << index << endl;
        return vector_[0];
    }
}
    
//----------------------------------------------------------------------------//
template<class T> T Collection<T>::set(int index, const T& element)
{
    if ( (index >= 0) && (index < (int) vector_.size()))
    {
        T old = vector_[index];
        vector_[index] = element;
        return old;
    }
    else
    {
        cerr << "ERROR: Collection.set() Index out of bounds: " << index << endl;
        return vector_[0];
    }
}

//----------------------------------------------------------------------------//
template<class T> int Collection<T>::size()
{
    return (int) vector_.size();
}


//----------------------------------------------------------------------------//
template<class T> void Collection<T>::sortCollection()
{
    sort(vector_.begin(), vector_.end());
}

//----------------------------------------------------------------------------//
template<class T> Iterator<T> Collection<T>::iterator()
{
    //return CollectionIterator<T>(vector_.begin(), vector_.end());
    return Iterator<T>(new CollectionIterator(vector_.begin(), vector_.end()));
}


//----------------------------------------------------------------------------//
// CollectionIterator

//----------------------------------------------------------------------------//
template<class T> Collection<T>::CollectionIterator
    ::CollectionIterator(typename vector<T>::iterator it, typename vector<T>::iterator end)
    : it_(it), end_(end)
{
}

//----------------------------------------------------------------------------//
template<class T> typename Collection<T>::CollectionIterator* Collection<T>::CollectionIterator::clone()
{
    return new CollectionIterator(it_, end_);
}

//----------------------------------------------------------------------------//
template<class T> bool Collection<T>::CollectionIterator::hasNext()
{
    return (it_ < end_);
}

//----------------------------------------------------------------------------//
template<class T> T& Collection<T>::CollectionIterator::next()
{
    T& value = *it_;
    it_++;
    return value;
}

// DEBUGGING PURPOSES ONLY
template<class T> void Collection<T>::printPointers(char * collType)
{
	// get an iterator pointing at the index
	cout << collType << ": " << endl;
        for (int i = 0; i < vector_.size(); i++)
	{
		cout <<  "   " << i << ": " << (long)(&vector_[i]) << endl;
	}
}


//----------------------------------------------------------------------------//
#endif //__COLLECTION_CPP


