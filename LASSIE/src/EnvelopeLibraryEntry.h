/*******************************************************************************
 *
 *  File Name     : EnvelopeLibraryEntry.h
 *  Date created  : May.17 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains the class "EnvelopeLibraryEntry" of
 *                  LASSIE. Each EnvelopeLibraryEntry holds the information of
                    a premade envelope in the envelope library.It is also a
 *                  doubly-linked list.
 *
 *==============================================================================
 *
 *  This file is part of LASSIE.
 *  2010 Ming-ching Chiu, Sever Tipei
 *
 *
 *  LASSIE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LASSIE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with LASSIE.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef ENVELOPE_LIBRARY_ENTRY_H
#define ENVELOPE_LIBRARY_ENTRY_H

#include "LASSIE.h"

typedef enum {
    envSegmentTypeLinear,
    envSegmentTypeExponential,
    envSegmentTypeSpline
} envSegmentType;

typedef enum { envSegmentPropertyFlexible, envSegmentPropertyFixed } envSegmentProperty;

// this is a class hold a doublelinked list of envelope segments

class EnvLibEntryNode;
class EnvLibEntrySeg {
public:
    EnvLibEntryNode* leftNode;
    EnvLibEntryNode* rightNode;

    envSegmentType segmentType;
    envSegmentProperty segmentProperty;

    EnvLibEntrySeg();
    ~EnvLibEntrySeg() {}
};

class EnvLibEntryNode {
public:
    double x;
    double y;
    EnvLibEntrySeg* leftSeg;
    EnvLibEntrySeg* rightSeg;
    EnvLibEntryNode(double _x, double _y);
    int countNumOfNodes();
    ~EnvLibEntryNode() {}
};

class EnvelopeLibraryEntry {
public:
    EnvelopeLibraryEntry(int _number);
    EnvelopeLibraryEntry(Envelope* _envelope, int _number);
    EnvelopeLibraryEntry(EnvelopeLibraryEntry* _originalEnvelope, int _number);
    ~EnvelopeLibraryEntry();
    int count();

    EnvelopeLibraryEntry* createNewEnvelope();
    EnvelopeLibraryEntry* duplicateEnvelope(EnvelopeLibraryEntry* _originalEnvelope);
    Glib::ustring getNumberString();
    EnvLibEntryNode* head;

    int number;
    EnvelopeLibraryEntry* next;
    EnvelopeLibraryEntry* prev;
};

#endif
