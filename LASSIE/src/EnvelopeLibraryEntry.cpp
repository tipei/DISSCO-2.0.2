/*******************************************************************************
 *
 *  File Name     : EnvelopeLibraryEntry.cpp
 *  Date created  : May.17 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains the class "EnvelopeLibraryEntry" of
 *                  LASSIE. Each EnvelopeLibraryEntry holds the information of
 *                  a premade envelope in the envelope library. It is also a
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

#include "EnvelopeLibraryEntry.h"
//#include <stdlib.h>

EnvelopeLibraryEntry::EnvelopeLibraryEntry(int _number) {
    number = _number;
    prev = NULL;
    next = NULL;

    head = new EnvLibEntryNode(0, 0);
    head->rightSeg = new EnvLibEntrySeg();
    head->rightSeg->leftNode = head;
    head->rightSeg->rightNode = new EnvLibEntryNode(1, 0);
    head->rightSeg->rightNode->leftSeg = head->rightSeg;
}

EnvelopeLibraryEntry::EnvelopeLibraryEntry(EnvelopeLibraryEntry* _originalEnvelope, int _number) {
    number = _number;
    prev = NULL;
    next = NULL;

    EnvLibEntryNode* originalEnvelopeCurrentNode = _originalEnvelope->head;
    EnvLibEntryNode* duplicationCurrentNode;

    head = new EnvLibEntryNode(originalEnvelopeCurrentNode->x, originalEnvelopeCurrentNode->y);
    duplicationCurrentNode = head;

    while (originalEnvelopeCurrentNode->rightSeg != NULL) {
        EnvLibEntrySeg* newSegment = new EnvLibEntrySeg();
        newSegment->segmentType = originalEnvelopeCurrentNode->rightSeg->segmentType;
        newSegment->segmentProperty = originalEnvelopeCurrentNode->rightSeg->segmentProperty;

        duplicationCurrentNode->rightSeg = newSegment;
        newSegment->leftNode = duplicationCurrentNode;

        EnvLibEntryNode* newNode =
            new EnvLibEntryNode(originalEnvelopeCurrentNode->rightSeg->rightNode->x,
                                originalEnvelopeCurrentNode->rightSeg->rightNode->y);

        newSegment->rightNode = newNode;
        newNode->leftSeg = newSegment;

        originalEnvelopeCurrentNode = originalEnvelopeCurrentNode->rightSeg->rightNode;

        duplicationCurrentNode = newNode;
    }
}

EnvelopeLibraryEntry::~EnvelopeLibraryEntry() {  // delete segments!}
    // TODO: lassie doesn't support deleting envelope so far so no worry about
    // it.
}

int EnvelopeLibraryEntry::count() {
    if (next == NULL)
        return 1;
    else
        return next->count() + 1;
}

EnvelopeLibraryEntry* EnvelopeLibraryEntry::createNewEnvelope() {
    if (next != NULL)
        return next->createNewEnvelope();
    else {
        next = new EnvelopeLibraryEntry(number + 1);
        next->prev = this;
        return next;
    }
}

EnvelopeLibraryEntry* EnvelopeLibraryEntry::duplicateEnvelope(
    EnvelopeLibraryEntry* _originalEnvelope) {
    if (next != NULL)
        return next->duplicateEnvelope(_originalEnvelope);
    else {
        next = new EnvelopeLibraryEntry(_originalEnvelope, number + 1);
        next->prev = this;
        return next;
    }
}

Glib::ustring EnvelopeLibraryEntry::getNumberString() {
    std::string temp;

    char tem1[20];
    sprintf(tem1, "%d", number);

    temp = string(tem1);

    Glib::ustring ret = temp;
    return ret;
}

EnvelopeLibraryEntry::EnvelopeLibraryEntry(Envelope* _envelope, int _number) {
    prev = NULL;
    next = NULL;
    number = _number;
    Collection<envelope_segment>* segments = _envelope->getSegments();

    EnvLibEntryNode* currentNode = NULL;
    EnvLibEntryNode* prevNode = NULL;
    EnvLibEntrySeg* currentSeg = NULL;
    EnvLibEntrySeg* prevSeg = NULL;

    int i = 0;
    for (i; i < segments->size() - 1; i++) {
        prevNode = currentNode;
        prevSeg = currentSeg;
        currentNode = new EnvLibEntryNode(segments->get(i).x, segments->get(i).y);
        currentSeg = new EnvLibEntrySeg();
        if (i == 0) {
            head = currentNode;
        } else {
            prevSeg->rightNode = currentNode;
        }
        currentNode->leftSeg = prevSeg;
        currentNode->rightSeg = currentSeg;
        currentSeg->leftNode = currentNode;

        if (_envelope->getSegmentLengthType(i) == FIXED) {
            currentSeg->segmentProperty = envSegmentPropertyFixed;
        } else {
            currentSeg->segmentProperty = envSegmentPropertyFlexible;
        }

        if (_envelope->getSegmentInterpolationType(i) == EXPONENTIAL) {
            currentSeg->segmentType = envSegmentTypeExponential;
        } else if (_envelope->getSegmentInterpolationType(i) == CUBIC_SPLINE) {
            currentSeg->segmentType = envSegmentTypeSpline;
        } else {
            currentSeg->segmentType = envSegmentTypeLinear;
        }

    }  // end of for loop

    currentSeg->rightNode = new EnvLibEntryNode(segments->get(i).x, segments->get(i).y);

    currentSeg->rightNode->leftSeg = currentSeg;
}

EnvLibEntrySeg::EnvLibEntrySeg()
    : leftNode(NULL),
      rightNode(NULL),
      segmentType(envSegmentTypeLinear),
      segmentProperty(envSegmentPropertyFlexible) {}

EnvLibEntryNode::EnvLibEntryNode(double _x, double _y)
    : leftSeg(NULL), rightSeg(NULL), x(_x), y(_y) {}

int EnvLibEntryNode::countNumOfNodes() {
    return (rightSeg == NULL) ? 1 : 1 + rightSeg->rightNode->countNumOfNodes();
}
