/*
CMOD (Composition MODule)
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
// patter.cpp
//
//----------------------------------------------------------------------------//

#include "Patter.h"

#include "Random.h"

//---------------------------------------------------------------------------//

Patter::Patter() {
    /* Just setup */
    origin = 0;
    nextIndex = 0;
}

//----------------------------------------------------------------------------//

Patter::Patter(int originNum, vector<int> intervalVect) {
    origin = originNum;
    intervals = intervalVect;
    nextIndex = 0;
}

//----------------------------------------------------------------------------//

Patter::Patter(const Patter& p) {
    intervals = p.intervals;
    patty = p.patty;
    origin = p.origin;
    nextIndex = p.nextIndex;
}

//----------------------------------------------------------------------------//

Patter& Patter::operator=(const Patter& rhs) {
    intervals = rhs.intervals;
    patty = rhs.patty;
    origin = rhs.origin;
    nextIndex = rhs.nextIndex;
    return *this;
}

//----------------------------------------------------------------------------//

Patter::~Patter() {
    intervals.clear();
    patty.clear();
}

//----------------------------------------------------------------------------//

void Patter::moveOrigin(int newOrigin) {
    origin = newOrigin;
    patty.clear();
    SimplePat();
    if (expMethod != "") {
        Expand(expMethod, expModulo, expLow, expHigh);
    }
}

//----------------------------------------------------------------------------//

int Patter::GetNextValue(string method, int newOrigin) {
    int returnValue;
    if (origin != newOrigin) {
        moveOrigin(newOrigin);
    }

    if (method == "IN_ORDER") {
        returnValue = patty[nextIndex];
        nextIndex = (nextIndex + 1) % patty.size();

    } else if (method == "OTHER") {
        //    ValuePick;
        //    ChooseFromList;
        //    SimpleIrand
    } else if (method == "TYPE_CLUSTERS") {  // stochos FUNCS
        // value = (int)ReadComputeFloat(checkPoint, offset);

    } else if (method == "TIME_DEPEND") {  // tone-row
        //    value = Patter::TimeDepend(checkPoint);

    } else if (method == "PROBABILITY") {
        //    value =

    } else {
        cerr << "Patter::GetNextValue - method not available" << endl;
        exit(1);
    }

    return returnValue;
}

//----------------------------------------------------------------------------//
void Patter::SimplePat() {
    patty.push_back(origin);

    for (int i = 0; i < intervals.size(); i++) {
        int lastNum = patty.back();
        int thisNum = lastNum + intervals[i];
        patty.push_back(lastNum + intervals[i]);
    }
}
//---------------------------------------------------------------------------//

void Patter::Expand(string method, int modulo, int low, int high) {
    // don't call Expand if origin = 0; delay until GetPattern is called!
    if (origin == 0) {
        expMethod = method;
        expModulo = modulo;
        expLow = low;
        expHigh = high;
    } else if (method == "EQUIVALENCE") {
        Equivalence(modulo, low, high);
    } else if (method == "SYMMETRIES") {
        Symmetries(modulo, low, high);
    } else if (method == "DISTORT") {
        Distort(modulo, low, high);
    } else {
        cerr << "Patter::Expand - no method available" << endl;
        exit(1);
    }
}

//----------------------------------------------------------------------------//

void Patter::Adjust() {
    cerr << "Patter::Adjust - this method is not available at the present time" << endl;
    exit(1);
}

//---------------------------------------------------------------------------//

void Patter::Equivalence(int modulo, int low, int high) {
    int newElement, numTerms, pointNum;
    int sign = -1;
    // cout << "\t PATTER Equivalence  origin:"<< this->origin << endl;

    // how many more terms are possible within range/
    // careful here, do not change: integers. Also: modulo can not be <= 0.
    if (modulo <= 0) {
        numTerms = 1;
    } else {
        numTerms = (high - origin) / modulo + 1 + (origin - low) / modulo;
    }
    // cout << "Patter::Equivalence - modulo=" << modulo << " low=" << low <<
    //     " high=" << high << " numTerms=" << numTerms << endl;

    // create two more arrays now that we know their cardinal
    vector<int> points;
    vector<double> probs;

    patty.clear();  // make sure the pattern is empty

    int lastNum;  // the last number to be added
    for (int location = 0; location <= intervals.size(); location++) {
        // use current patty element as origin for next interval
        if (location == 0) {
            lastNum = origin;
        } else {
            lastNum = patty.back() + intervals[location - 1];
        }

        points.clear();
        probs.clear();

        for (int j = 0; j < numTerms; j++) {  // for each possible equivalence

            for (int k = 0; k < 2; k++) {  // generate + - new elements
                newElement = lastNum + modulo * j * sign;
                //      cout << "newElement= " << newElement << " j=" << j <<"
                //      k=" << k << endl;

                if (j != 0 || k != 1) {  // only one origin !
                    if (newElement >= low && newElement <= high) {
                        //          cout << "   	 	     newElement=" <<
                        //          newElement << endl;
                        points.push_back(newElement);
                        // assign probabilities according to distance from
                        // origin
                        probs.push_back((double)(numTerms - j) / (double)numTerms);
                        //	    cout << "prob = " << (double)(numTerms - j)
                        /// (double)numTerms << endl;
                    }
                }
                sign *= -1;
            }
        }

        if (probs.size() == 0) {
            cout << "Patter::Equivalence() error: probs array empty!" << endl;
        }

        // normalize the probability array, and order from 0 to 1
        // (this should probably go back into utility --- cavis)
        double partSum = 0;
        double sum = 0;
        for (int i = 0; i < probs.size(); i++) {
            sum += probs[i];
        }

        for (int i = 0; i < probs.size(); i++) {
            probs[i] /= sum;
        }

        for (int i = 0; i < probs.size(); i++) {
            partSum += probs[i];
            probs[i] = partSum;
            //    cout << "Patter::Equivalence - probs{" << i << "]=" <<
            //    probs[i] << endl;
        }

        // choose a point at random
        int randPosition = Random::ChooseFromProb(probs);
        patty.push_back(points[randPosition]);
        //  cout << "	patty[" << location << "]=" << patty[location] << endl;
    }
}

//---------------------------------------------------------------------------//

void Patter::Symmetries(int modulo, int low, int high) {
    cerr << "Patter::Symmetries - this method is not available at the present "
            "time"
         << endl;
    exit(1);
}

//---------------------------------------------------------------------------//

void Patter::Distort(int modulo, int low, int high) {
    cerr << "Patter::Distort - this method is not available at the present time" << endl;
    exit(1);
}

//----------------------------------------------------------------------------//
