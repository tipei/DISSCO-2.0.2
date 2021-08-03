/*
CMOD (composition module)
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
//  TimeSpan.h
//
//----------------------------------------------------------------------------//

#ifndef TIMESPAN_H
#define TIMESPAN_H

//CMOD includes
#include "Libraries.h"
#include "Rational.h"

/*A TimeSpan is a span in time. Explicitly it has a start time and a duration.
Implicitly it also has an end time. A time-span may be expressed in terms of
seconds and, in the case of it being an exact interval, EDU.*/

//----------------------------------------------------------------------------//
struct TimeSpan {
  float start;
  float duration;
  Ratio startEDU;
  unsigned long long int startEDUAbsolute;
  Ratio durationEDU;
  
  TimeSpan() : start(0), duration(0), startEDU(0, 0), startEDUAbsolute(0), durationEDU(0, 0) {}
};
#endif /* TIMESPAN_H */

