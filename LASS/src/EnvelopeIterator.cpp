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
// EnvelopeIterator.cpp
//
//----------------------------------------------------------------------------//

#ifndef __ENVELOPE_ITERATOR_CPP
#define __ENVELOPE_ITERATOR_CPP

//----------------------------------------------------------------------------//

#include "EnvelopeIterator.h"
#include "Envelope.h"

// #define DEBUG_MODE

//----------------------------------------------------------------------------//
EnvelopeIterator::EnvelopeIterator(
                                 Collection<Interpolator*> interpolators)
{
#ifdef DEBUG_MODE
    cout << "ENVI::Constructor" << endl;
    cout << "size of interpolators: " << interpolators.size() << endl;
#endif

    // make a new collection
    interpolators_ = new Collection<Interpolator*> (interpolators);

    // initialize Entry index (to the first valid Entry index value)
    iSegmentIndex_ = 0;

    // set our current Iterator to NULL
    // (once we need an iterator, we'll initialize it)
    currentIterator_ = NULL;

    // set a default initial value
    currentValue_ = 0.0;

    // initially assume we have at least 1 value to return
    moreValues_ = true;

#ifdef DEBUG_MODE
    cout << "ENVI::Constructor" << endl;
for (int iLoop = 0; iLoop < interpolators_->size(); iLoop++)
{
    cout << "  Interpolator #" << iLoop << " - has points: " << endl;
    if (interpolators_->get(iLoop)->size() != 2)
    {
        cout << "     ERROR IN SIZE (size = ";
        cout << interpolators_->get(iLoop)->size() << endl;
    }
    cout << "    (" << interpolators_->get(iLoop)->get(0).time_ << ", ";
    cout << interpolators_->get(iLoop)->get(0).value_ << ")" << endl;
    cout << "    (" << interpolators_->get(iLoop)->get(1).time_ << ", ";
    cout << interpolators_->get(iLoop)->get(1).value_ << ")" << endl;
}
#endif
}


//----------------------------------------------------------------------------//
EnvelopeIterator::~EnvelopeIterator()
{
    if (interpolators_ != NULL)
    {
        delete (interpolators_);
    }
    if (currentIterator_ != NULL)
    {
        delete (currentIterator_);
    }
}


//----------------------------------------------------------------------------//
// A B S T R A C T   I T E R A T O R   F U N C T I O N S
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
EnvelopeIterator* EnvelopeIterator::clone()
{
    return (new EnvelopeIterator(*this));
}


//----------------------------------------------------------------------------//
bool EnvelopeIterator::hasNext()
{
    // let them know if we think we have more values
    return (moreValues_);
}


//----------------------------------------------------------------------------//
m_value_type& EnvelopeIterator::next()
{
    bool gotNewValue = false;
    int iNumInterpolators = interpolators_->size();
    
    // if we haven't started iterating yet, start with the first iterator
    if ((currentIterator_ == NULL) && (iNumInterpolators > 0))
    {
        currentIterator_ = new Iterator<m_value_type> 
            (interpolators_->get(0)->valueIterator());
        
        iSegmentIndex_ = 0;
    }

    // if we have an iterator to work with and it can give us a value
    if ((currentIterator_ != NULL) && (currentIterator_->hasNext()))
    {
        // get a value from the current Iterator
        currentValue_ = currentIterator_->next();
        gotNewValue = true;

#ifdef DEBUG_MODE
    if (tempCounter_ % 1000 == 0)
    {
        cout << "got a value from the current iterator: " << currentValue_ << endl;
    }
    tempCounter_++;
#endif
    }
    else
    {
        // update our index to look at the next Iterator
        iSegmentIndex_++;

        // find another Iterator to grab values from
        while (iSegmentIndex_ < iNumInterpolators && (!gotNewValue))
        {
            // clear out the currently-stored Iterator
            if (currentIterator_ != NULL)
            {
                delete (currentIterator_);
            }

            // set current Iterator to the next Iterator
            currentIterator_ = new Iterator<m_value_type> 
                (interpolators_->get(iSegmentIndex_)->valueIterator());

            // see if we can grab values from this Iterator
            if ((*currentIterator_).hasNext())
            {
                // we've got a new value
                currentValue_ = currentIterator_->next();
                gotNewValue = true;
            }

            // if we're here, then this Iterator is empty (which is dumb, but,
            // I'll allow for it), so update our index and keep looking
            iSegmentIndex_++;
        }
        
        // we advanced one past the index that we want, so go back to it
        iSegmentIndex_--;
    }

    // if we couldn't find another value to return, set flag to say that we have
    // no more values, and return the current value (this gives one extra value
    // at the end, which should be fixed)
    if (gotNewValue == false)
    {
        moreValues_ = false;
    }

    return (currentValue_);
}


//----------------------------------------------------------------------------//
#endif //__ENVELOPE_ITERATOR_CPP

