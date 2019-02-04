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
//	StandardHeaders.h
//
//----------------------------------------------------------------------------//

#ifndef __STANDARDHEADERS_H
#define __STANDARDHEADERS_H

//----------------------------------------------------------------------------//

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>


/*The C++ standards committee only recently agreed on the new name
for "unordered_map". It used to be referred to as a hash_map, which
will be used here for now.*/
#include <ext/hash_map>
#define DISSCO_HASHMAP hash_map

/* C++0x Alternative (must compile with -std=c++0x):
  #include <unordered_map>
  #define DISSCO_HASHMAP unordered_map
*/

//Also need pthreads from the POSIX library (link with -lpthread)
#include <pthread.h>

using namespace std;
using namespace __gnu_cxx; 

#endif

