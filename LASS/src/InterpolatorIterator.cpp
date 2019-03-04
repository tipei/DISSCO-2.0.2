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
//	InterpolatorIterator.cpp
//
//----------------------------------------------------------------------------//

#ifndef __INTERPOLATOR_ITERATOR_CPP
#define __INTERPOLATOR_ITERATOR_CPP

//----------------------------------------------------------------------------//

#include "InterpolatorIterator.h"

//----------------------------------------------------------------------------//
InterpolatorIterator::InterpolatorIterator()
    :stepsLeft_(0)
{
}


//----------------------------------------------------------------------------//
void InterpolatorIterator::append(m_time_type t_from, m_time_type t_to, m_value_type v_from, m_value_type v_to, m_sample_count_type steps)
{
    Entry e(t_from,t_to,v_from,v_to,steps);
    queue_.push_back( e );
}



//----------------------------------------------------------------------------//
// A B S T R A C T   I T E R A T O R   F U N C T I O N S
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool InterpolatorIterator::hasNext()
{
    // for some reason, this isn't actually called here
    // but since it's abstract in AbstractIterator, it has
    // to be defined... but really it does nothing, I'm pretty sure
    // -ZM

    // fixed - Jon Kishkunas
    return (!((queue_.empty()) && (!stepsLeft_)));
}


//----------------------------------------------------------------------------//

LinearInterpolatorIterator::LinearInterpolatorIterator()
{
}

LinearInterpolatorIterator* LinearInterpolatorIterator::clone()
{
    return new LinearInterpolatorIterator(*this);
}

m_value_type& LinearInterpolatorIterator::next()
{

    if (stepsLeft_ > 0)
    {
        stepsLeft_--;
        value_ += delta_;
        return value_;
    }
    else if (!queue_.empty())
    {
        // pop the top entry
        
        Entry e = queue_.front();
        //the line below is substituted by the line above -- Ming-ching
        //Entry& e = queue_.front();
        queue_.pop_front();

        // set up for the iterator:
        stepsLeft_ = e.steps_;        
        value_ = e.v_from_;
        delta_ = (e.v_to_ - e.v_from_) / ((m_value_type) e.steps_);

        // return the value:
        stepsLeft_--;
        return value_;
    }
    else
    {
        // User is reading past the end of the iterator.
        // just return the value.
        return value_;
    }
    
}

ExponentialInterpolatorIterator::ExponentialInterpolatorIterator()
{
}

ExponentialInterpolatorIterator* ExponentialInterpolatorIterator::clone()
{
    return new ExponentialInterpolatorIterator(*this);
}


/*************************************************************************/
// This is a very useful function to essentially find the y-value of the
// x-value (time-value) passed in as an argument.  If the point asked for 
// is at the start of a segment, this is an easy function.  However, if it 
// is not, the value must be interpolated, either linearly or exponentially,
// because these are the two path types currently defined.
//
// The exponential interpolation was derived by Hans Kaper one day.  It
// is correct.  Basically, the derivation starts with the exponential
// equation y(t) = y1*e^(L*(t-t1)) and through simplification and such,
// gets to the following equation:
//
//  1 + ((y(t) - y1)/y1) = (1 + ((y2 - y1)/y1)) ^ ((t - t1)/(t2 - t1))
//
// Then, the following simplifications are done:
// 
//   X = (y(t) - y1)/y1
//  dy = (y2 - y1)/y1
//  dt = (t - t1)/(t2 - t1)
// 
// So, the equation becomes:
//
//   1 + X = (1 + dy) ^ dt
// 
// which is what is used in this function.  This shows a very important
// detail.  This gets the value relative to this exponential function,
// not an absolute value.
/*************************************************************************/

m_value_type& ExponentialInterpolatorIterator::next()
{
    if( queue_.empty() ) return value_;

    Entry& e = queue_.front();
    if( ! stepsLeft_ ) // we're at the beginning of an entry, so just return
                       // that value and set up the steps for the next
    {
        stepsLeft_ = e.steps_;
        value_ = e.v_from_;
    }
    if (stepsLeft_ > 0)
    {
        stepsLeft_--;
        m_value_type y1 = e.v_from_;
	    m_value_type y2 = e.v_to_;
        m_value_type dy;

        if( y1 == 0 )
            y1 = .0001;
        if( y2 == 0 )
            y2 = .0001;

        m_value_type alpha = 3;
        if (y1 > y2)
        {
            alpha = -alpha;
        }

        dy = (y2 - y1) / y1;
	    m_time_type t1 = e.t_from_;
        m_time_type t2 = e.t_to_;
        m_time_type t = (((t2 - t1) / e.steps_) * (e.steps_ - stepsLeft_)) + t1;
        m_value_type I = (t - t1) / (t2 - t1);
        m_value_type base = 2.718282;
        value_ = y1 + (y2 - y1) * ((1 - pow (base, (I * alpha))) / (1 - pow (base, alpha)));
    }

    if( ! stepsLeft_ )
    {
        queue_.pop_front();
        if( queue_.empty() ) return value_;
        e = queue_.front();
        stepsLeft_ = e.steps_;
        value_ = e.v_from_;
    }
    
    return value_;
}

CubicSplineInterpolatorIterator::CubicSplineInterpolatorIterator() 
  : x0(-1), y0(-1), x3(0), y3(0)
{
}

CubicSplineInterpolatorIterator* CubicSplineInterpolatorIterator::clone()
{
    return new CubicSplineInterpolatorIterator(*this);
}

/*************************************************************************/
// The ith cubic polynomial u_i(x) which connects (xi,yi) with slope
// mi to (x_i+1,y_i+1) with slope m_i+1 as x ranges from xi to
// x_i+1 is:
//
// u_i(x) = ai+bi(x-xi)+ci(x-xi)^2+di(x-xi)^3, where
// ai = yi,
// bi = mi,
// ci = (3(y_i+1-yi)/(x_i+1-xi) - 2mi - m_i+1) / (x_i+1 - xi)
// di = (mi + m_i+1 - 2(y_i+1 - yi)/(x_i+1 - xi)) / (x_i+1 - xi)^2
//
// In this function, we use:
// mi = (y_i+1-y_i-1)/(x_i+1-x_i-1) for i=2,...,n-1 which is just the
// average of the slopes surrounding a point, and for the ends:
// m1 = (y2-y1)/(x2-x1), mn = (yn-y_n-1)/(xn-x_n-1)
//
// This is achieved by taking four (time,value) points and using them
// to make the above calculations:  two points specifying the range,
// and a point before and one after.
/*************************************************************************/
m_value_type& CubicSplineInterpolatorIterator::next()
{

    if( queue_.empty() ) return value_;
    
    Entry e = queue_.front();
    // the line below is substituted by the line above --Ming-ching
    //Entry& e = queue_.front();
    queue_.pop_front();
    int chk = 0;
    if( queue_.empty() ) { chk = 1; }
    else { // get the point ahead of the range since there are more
           // entries in the queue
      Entry& e2 = queue_.front();
      x3 = e2.t_to_;
      y3 = e2.v_to_;
    }
    queue_.push_front(e);

    // we need this in the larger scope
    m_time_type x1 = 0.0;
    m_value_type y1 = 0.0;

    if( ! stepsLeft_ ) // we're at the beginning of an entry, so just return
                       // that value and set up the steps for the next
    {
        stepsLeft_ = e.steps_;
        value_ = e.v_from_;
    }
    else if (stepsLeft_ > 0)
    {
        stepsLeft_--;
	x1 = e.t_from_;
        m_time_type x2 = e.t_to_;
        m_time_type x = (( ( x2 - x1 ) / e.steps_ ) * ( e.steps_ - stepsLeft_ )) + x1;
        y1 = e.v_from_;
	m_value_type y2 = e.v_to_;

        // since we can't lookahead, just give the next points the end of 
        // the range's values
        if( chk ) { x3 = x2; y3 = y2; }

        m_value_type l_slp = (y2-y1)/(m_value_type)(x2-x1);

        m_value_type m1;
        if( x0 == -1 && y0 == -1 ) // front of queue
          m1 = l_slp;
        else
          m1 = (y2-y0)/(m_value_type)(x2-x0);

        m_value_type m2 = (y3-y1)/(x3-x1);

        // our coefficients for the polynomial
        m_value_type a = y1;
        m_value_type b = m1;
        m_value_type c = ((3*l_slp)-(2*m1)-m2)/(x2-x1);
	m_value_type d = (m1+m2-(2*l_slp))/(pow((x2-x1),2));

        m_value_type x_dif = x - x1;

        // the polynomial
        m_value_type ux = a + (b*x_dif) + (c*(pow(x_dif,2))) + (d*(pow(x_dif,3)));

        value_ = ux;
    }

    if( ! stepsLeft_ ) {

        // set the last point to the beginning of the current point
        // as we move ahead
        x0 = x1;
	y0 = y1;
        queue_.pop_front();
        if( queue_.empty() ) return value_;
        e = queue_.front();
        stepsLeft_ = e.steps_;
        value_ = e.v_from_;
    }

    return value_;
}

//----------------------------------------------------------------------------//
#endif //__INTERPOLATOR_ITERATOR_CPP

