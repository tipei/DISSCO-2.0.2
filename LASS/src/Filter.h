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
//	Filter.h
//
//----------------------------------------------------------------------------//

/* Best if viewed with 4-space tabs */

#ifndef __FILTER_H
#define __FILTER_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "Collection.h"
#include "Track.h"
#include "MultiTrack.h"

//----------------------------------------------------------------------------//

/**
 * The filter class is a pure virtual class only meant to ensure that all
 * filters have a common interface.  The idea is that a collection of
 * filters, specified by the user at run-time, perhaps, could be applied one
 * after the next without regard to what type of filter each one is.
 *
 * Each filter will likely have its own configuration that can be done, and
 * if, in the future, this becomes something that needs to be generalized
 * as well, perhaps this class could be expanded to include a ParameterLib.
 *
 * \note The Filter objects are stateful machines with internal feedback mechanisms.  Thus this filter should be allocated anew each time you begin a new channel and should not be mixed between channels.
 *	\author Jim Lindstrom
 **/
class Filter
{
public:

  virtual ~Filter(){}
    	/**
	 *This method applies the filter to a MultiTrack source, track by track.
	 * It does so by decomposing the wave into Track objects (organized as
	 * a Collection inside the MultiTrack object) and calling the
	 * virtual function, do_filter(SoundSample *inWave), for each Track.
	 *
	 * \param inWave A reference to a MultiTrack object to Filter
	 *
	 * \return A reference to a NEW MultiTrack, which the caller is 
	 * responsible for deleting when done. The original MultiTrack remains 
	 * intact and untouched
	 **/
	MultiTrack &do_filter_MultiTrack(MultiTrack &inWave);

	/**
	 * This method applies the filter to a source Track by calling the
	 * virtual function, do_filter(SoundSample *inWave).
	 *
	 * \param inWave A reference to a Track object to Filter
	 *
	 * \return A reference to a NEW MultiTrack, which the caller is 
	 * responsible for deleting when done. The original Track remains 
	 * intact and untouched.
	 **/
	Track &do_filter_Track(Track &inWave);

	/**
	 * This method calls do_filter(m_sample_type x) for each sample in a 
	 * track and builds a new SoundSample on the fly.
	 *
	 * \param inWave A pointer to a SoundSample object to Filter
	 *
	 * \return A reference to a NEW MultiTrack, which the caller is 
	 * responsible for deleting when done. The original Track remains 
	 * intact and untouched
	 **/
	SoundSample *do_filter_SoundSample(SoundSample *inWave);

	/**
	 * This method should be redefined by each class derived from Filter to
	 * perform the actual filtering.  It should take in a single sample and
	 * return the filtered sample.  If, in the future, real-time filtering
	 * should become feasible and is desired, this is the entry-point to 
	 * use.
	 *
	 * \param x_t A sample to filter
	 *
	 * \return the filtered sample
	 **/
	virtual m_sample_type do_filter(m_sample_type x_t) = 0;

	/**
	 * This method should be redefined by each class derived from Filter to
	 * reset the filter to an initial state.  It should have the same effect
	 * as deleting the filter and creating a new one.
	 **/
	virtual void reset(void) = 0;

	/**
	 * Every filter requires some sort of history of previous values.  Even
	 * the simplest 1st order IIR filter requires the previous output.  For
	 * filters, we only need one of the previous values.  In that case, a 
	 * queue structure is useful to enqueue the current output for later 
	 * use, and to dequeue a value from previous runs. 
	 *
	 * Note 1: The C++ STL includes a queue, but (1) it uses nonstandard 
	 * 'push' and 'pop' instead of 'enqueue' and 'dequeue' (respectively), 
	 * which decreases readability; (2) in filters with a fixed delay 
	 * factor, we can optimize for speed by using a fixed-length ring 
	 * buffer of the appropriate size (let queue length = D).
	 * Note 2: Some filters, such as FIR filters require a different type 
	 * of history that needs access to all the previous values (or some 
	 * subset of them that includes more than just the oldest value).  For 
	 * these filters, this queue allows indexed access from either end of 
	 * the queue.
	 *
	 * Three examples:
	 * 1. Consider the 1st order IIR filter.  It uses the equation
	 *    y(t) = x(t) + g*y(t-1).  In this case, the history queue will 
	 *    always have one old value.  Thus, we must inititalize it by 
	 *    enqueing a 0 at the start.  Then we can perform one time step 
	 *    with as follows:
	 *      y_t = x_t + (g * hist_queue.dequeue());
	 *      hist_queue.enqueue(y_t);
	 *      return y_t;
	 *
	 * 2. Now compare this to the comb filter.  It uses the equation
	 *    y(t) = x(t-D) + g*y(t-D).  Now we need two queues.  This is the 
	 *    reason that the queue class is provided, but not instantiated. 
	 *    Child-class filters can use the history queue however they like as
	 *    needed.
	 *      init: for(i=0;i<D;i++) { x_hist.enqueue(0); y_hist.enqueue(0); }
	 *
	 *      y_d = x_hist.dequeue() + (g * y_hist.dequeue());
	 *      y_hist.enqueue(y_d);
	 *      x_hist.enqueue(x_d);
	 *
	 * Note 3: If you're using a D-length queue for a delay of D cycles, 
	 * be sure that you dequeue before enqueuing so you don't overflow 
	 * the queue.
	 * 
	 *	\author Jim Lindstrom
 	 **/
	template<class ElemType> class hist_queue
	{
	public:

		/**
		 * This constructor initializes the queue to a given length.
		 *
		 * \param len The length of the queue
		 **/
		hist_queue(long len)
		{
			length = len;
			front_idx = back_idx = 0;
			if((array = new ElemType[length]) == NULL)
			{
				cerr << "Couldn't create array for Filter queue of length " << length << endl;
				exit(0);
			}
		}

		/**
		 * This is the destructor.
		 **/
		~hist_queue()
		{
  		delete [] array;	
		}

		/**
		 * This function removes and returns the oldest element in the 
		 * queue (FIFO).
		 * \return The oldest element
		 **/
		ElemType dequeue(void)
		{
			int old_back_idx;

			old_back_idx = back_idx;
			back_idx = (back_idx + 1) % length;
			return array[old_back_idx];
		}

		/**
		 * This function adds an element to the queue (FIFO).
		 *
		 * \param new_val The element to add to the queue
		 **/
		void enqueue(ElemType new_val)
		{
			array[front_idx] = new_val;
			front_idx = (front_idx + 1) % length;
		}

		/**
		 * This function returns, without removing, a value from the 
		 * queue indexed from the oldest element
		 *
		 * \param i The index from the end of the queue to return
		 **/
		ElemType index_from_oldest(long i)
		{
			long idx;

			// we know idx%length, find the idx in the right range though
			idx = back_idx+i;
			while(idx>=length)
				idx-=length;

			// return the index
			return array[idx];
		}

		/**
		 * Return, without removing, a value from the queue indexed 
		 * from the newest element.
		 *
		 * \param i The index from the end of the queue to return
		 **/
		ElemType index_from_newest(long i)
		{
			long idx;

			// we know idx%length, find the idx in the right range though
			idx = front_idx-i;
			while(idx<0)
				idx+=length;

			// return the index
			return array[idx];
		}

	private:

		/**
		 * This is the number of valid positions within the queue.
		 **/
		long length;

		/**
		 * This is the index PAST the newest item.
		 **/
		long front_idx;

		/**
		 * This is the index TO the oldest item.
		 **/
		long back_idx;

		/**
		 * This holds the values of the queue.
		 **/
		ElemType *array;
	};

};

//----------------------------------------------------------------------------//
#endif //__FILTER_H
