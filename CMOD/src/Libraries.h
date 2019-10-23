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

#ifndef __CMOD_LIBRARIES_H
#define __CMOD_LIBRARIES_H

// Several C++ standard library includes needed by CMOD
#include <pthread.h>

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <ctime>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// Pseudo-standard, but widely used headers
#include <dirent.h>

// The LASS library for additive sound synthesis
#include "../../LASS/src/LASS.h"
#include "../../LASS/src/Score.h"

// Include the Rational number class here since it is a fundamental type and has
// no .cpp file that is actually compiled.
#include "Rational.h"

// Turn on the std namespace by default.
using namespace std;

// This part is for Xerces

#include <stdexcept>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
using namespace xercesc;

// this is the math expression parser : muparser
#include "muParser/muParser.h"

typedef enum {
    eventTop = 0,
    eventHigh = 1,
    eventMid = 2,
    eventLow = 3,
    eventBottom = 4,
    eventSound = 5,
    eventEnv = 6,
    eventSiv = 7,
    eventSpa = 8,
    eventPat = 9,
    eventRev = 10,
    eventFolder = 11,
    eventNote = 12,
    eventFil = 13,
    eventMea = 14,
    eventSpec = 15

} EventType;

typedef enum {
    tempoPrefixNone = 0,
    tempoPrefixDotted = 1,
    tempoPrefixDoubleDotted = 2,
    tempoPrefixTriple = 3,
    tempoPrefixQuintuple = 4,
    tempoPrefixSextuple = 5,
    tempoPrefixSeptuple = 6
} TempoPrefix;

typedef enum {
    tempoNoteValueWhole = 0,
    tempoNoteValueHalf = 1,
    tempoNoteValueQuarter = 2,
    tempoNoteValueEighth = 3,
    tempoNoteValueSixteenth = 4,
    tempoNoteValueThirtySecond = 5
} TempoNoteValue;

#define GFEC getFirstElementChild  // for easy coding
#define GNES getNextElementSibling
#define XMLTC Utilities::XMLTranscode

#endif
