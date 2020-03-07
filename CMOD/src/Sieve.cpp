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

//---------------------------------------------------------------------------//
//
//	Sieve.cpp
//
//---------------------------------------------------------------------------//

#include <iterator>
#include "Sieve.h"
#include "Random.h"
#include "ModParser.h"
//---------------------------------------------------------------------------//

Sieve::Sieve() {
  skip = 0;
}

//---------------------------------------------------------------------------//

Sieve::Sieve(string aFile) {
  skip = 0;
  fileName = aFile;
}

//---------------------------------------------------------------------------//

Sieve::~Sieve() {
}

//---------------------------------------------------------------------------//

string Sieve::getFileName() {
  return fileName;
}


//---------------------------------------------------------------------------//

void Sieve::BuildFromExpr(int minVal, int maxVal,
                          const char *eMethod, const char *wMethod,
                          std::string expr, vector<int> wArgVect, vector<int> offsetVect) {
  ModParser mp(offsetVect);
  mp.parseExpr(expr, minVal, maxVal);
  eList = mp.getElements();
  Sieve::Weights(mp.getMods(), wMethod, wArgVect, mp.getOffsets());
}


//---------------------------------------------------------------------------//

void Sieve::Build(int minVal, int maxVal,
                  const char *eMethod, const char *wMethod,
                  vector<int> eArgVect, vector<int> wArgVect, vector<int> offsetVect) {

  Sieve::Elements(minVal, maxVal, eMethod, eArgVect, offsetVect);
  Sieve::Weights(eArgVect, wMethod, wArgVect, offsetVect);
}


//---------------------------------------------------------------------------//

void Sieve::FillInVectors(vector<int>& intVect, vector<double>& doubleVect) {


  list<int>::iterator eIter = eList.begin();
  list<double>::iterator wIter = wList.begin();

  if ( eList.size() >= wList.size() ) {
    while (eIter != eList.end()) {
      intVect.push_back( *eIter );
      doubleVect.push_back( *wIter );
      eIter++;
      wIter++;
    }
  } else if ( wList.size() > eList.size() ) {
    while (wIter != wList.end()) {
      intVect.push_back( *eIter );
      doubleVect.push_back( *wIter );
      eIter++;
      wIter++;
    }
  }
}


//---------------------------------------------------------------------------//

int Sieve::GetNumItems() {
  int result = 0;

  if (eList.size() >= wList.size()) {
    result = eList.size();
  } else {
    result = wList.size();
  }
  return result;
}

//---------------------------------------------------------------------------//

int Sieve::Modify(Envelope *env, string method) {
  Sieve::AddEnvelope(env, method);
  Sieve::CumulWeights();

  return Sieve::ChooseL();
}


//---------------------------------------------------------------------------//

int Sieve::ChooseL() {
  double randomNumber = Random::Rand();

  list<int>::iterator eIter = eList.begin();
  list<double>::iterator wIter = wList.begin();

  while (eIter != eList.end() && (randomNumber > *wIter)) {
    eIter++;
    wIter++;
  }

  return *eIter;
}


//---------------------------------------------------------------------------//

void Sieve::Elements(int minVal, int maxVal,
                     const char *method,
                     vector<int> eArgVect, std::vector<int> offsetVect) {
  if(strcmp(method, "MEANINGFUL") == 0) {		//only meaningful elem.
    Sieve::Meaningful(minVal, maxVal, eArgVect, offsetVect);
  } else if(strcmp(method, "MODS") == 0) {		//uses moduli
    Sieve::Multiples(minVal, maxVal, eArgVect, offsetVect);
  } else if(strcmp(method, "FAKE") == 0) {		//all elem, same weight
    Sieve::Fake(minVal, maxVal);
  } else if(strcmp(method, "FIBONACCI") == 0) {       	//Fibonacci sieve
    cerr << " see harmSieve" << endl;
    exit(1);
  } else if(strcmp(method, "OVERTONES") == 0) {       	//overtone series
    cerr << "utility::SieveElements - overtones not available yet" << endl;
    exit(1);
  } else if(strcmp(method, "MULT_PARAMS") == 0) {     	//multiple parameters
    cerr << "utility::SieveElements - multiple params not available yet"
        << endl;
    exit(1);
  } else {
    cerr << "no method to build sieve: "<< method << endl;
    exit(1);
  }
}


//---------------------------------------------------------------------------//

void Sieve::Weights(std::vector<int> eArgVect,
		    const char *method,
                    std::vector<int> wArgVect,
		    std::vector<int> offsetVect) {
  if(strcmp(method, "PERIODIC") == 0) {
    wArgVect.resize(2);
    Sieve::PeriodicWeights(wArgVect);
  } else if(strcmp(method, "HIERARCHIC") == 0) {
    Sieve::HierarchicWeights(eArgVect, wArgVect, offsetVect);
  } else if(strcmp(method, "INCLUDE") == 0) {
    Sieve::IncludeWeights(wArgVect);
  } else {
    cerr << "Sieve::Weights - no method for asigning weights" << endl;
    exit(1);
  }
}


//---------------------------------------------------------------------------//

void Sieve::Meaningful(int minVal, int maxVal, vector<int> eArgVect, std::vector<int> offsetVect) {
  skip = 0;

  for (int i = 0; i < eArgVect.size(); i++) {
    if( (eArgVect[i]+ offsetVect[i]) >= minVal && (eArgVect[i] + offsetVect[i]) <= maxVal) {
/*
    cout << "Sieve::Meaningful - eArgVect[" << i << "]=" << eArgVect[i]
         << " offsetVect[" << i << "]=" << offsetVect[i] << endl;
*/
      eList.push_back( eArgVect[i] + offsetVect[i] );
    }

    if(eArgVect[i] + offsetVect[i]< minVal) {
      skip++;
    }
  }

  // sort the list, ascending
  eList.sort();
/*
  for(list<int>::iterator iter = eList.begin(); iter != eList.end(); iter++) {
	cout << *iter << endl;
  }
*/
}

//---------------------------------------------------------------------------//

void Sieve::Multiples(int minVal, int maxVal, vector<int> numMods, std::vector<int> offsetVect) {
  int element, modulo;

  eList.clear();

  skip = 0;

 /* cout << " --- SIEVE::MULTIPLES == " << endl;
  cout << "numMods" << endl;
  for (int q = 0; q < numMods.size(); q++){
    cout << numMods[q] << ", ";
  }
  cout << endl;
*/
  for (int i = 0; i < numMods.size(); i++) {

    if (minVal == 0 && offsetVect[i] >=0) { //put 0 in the list if minVal ==0 and at least one of the offset is 0
      eList.push_back(offsetVect[i]);
    }

    int newElement = numMods[i] + offsetVect[i];

    while (newElement <= maxVal) {
      if ( newElement >= minVal ) {
        // note: we don't have to check for duplicates before adding, because
        //     we'll sort and remove consecutive duplicates at the end
        eList.push_back(newElement);
      } else if (newElement < minVal) {
        skip++;
      }
      newElement += numMods[i]; // increment to the next multiple
    }
  }

  eList.sort(); // sort into ascending order

  cout << "Sieve::Multiples - eList" << endl;
  for (list<int>::iterator it = eList.begin(); it != eList.end(); ++it) {
    cout << *it << ", ";
  }
  cout << endl;
  int sever; cin >> sever;

  eList.unique();  //remove consecutive duplicate values
}

//---------------------------------------------------------------------------//

void Sieve::Fake(int minVal, int maxVal) {
  skip = 0;
  eList.clear();

  for (int i = minVal; i <= maxVal; i++) {
    eList.push_back(i);
  }

}

//---------------------------------------------------------------------------//

void Sieve::PeriodicWeights(const vector<int>& wArgVect) {
/*
=>changed by Sever: elist is >= wArgVect.size; loop should go to eList.size
  for (int count = 0; count < wArgVect.size(); count++) {
    if (count < eList.size()) {
      wList.push_back(wArgVect[count % wArgVect.size()]);
    }
  }
*/
  for (int count = 0; count < eList.size(); count++) {
    wList.push_back(wArgVect[count % wArgVect.size()]);
  }
}

//---------------------------------------------------------------------------//

void Sieve::HierarchicWeights(const std::vector<int>& eArgVect,
				std::vector<int> wArgVect,
				std::vector<int> offsetVect) {
  int whichMod;
  double probability;

  list<int>::iterator eIter = eList.begin();
  list<double>::iterator wIter = wList.begin();

  while (eIter != eList.end()) {
    whichMod = 0;
    probability = 0;

    while (whichMod < wArgVect.size()) {

      if((*eIter - offsetVect[whichMod]) % eArgVect[whichMod] == 0) {
         probability += wArgVect[whichMod];
      }
      whichMod++;
    }

    wList.push_back(probability);

    eIter++;
    wIter++;
  }
}

//---------------------------------------------------------------------------//

void Sieve::IncludeWeights(const vector<int>& wArgVect) {
//for(int i = 0; i < wArgVect.size(); i++) {
  for(int i = 0; i < eList.size(); i++) {
    if(i >= skip && i < eList.size()+ skip) {
      wList.push_back(wArgVect[i]);
    }
  }
}

//---------------------------------------------------------------------------//

void Sieve::AddEnvelope(Envelope *env, string method) {
  float value;
  double checkPoint;
  double probability;

  NormalizewList();

  int index = 0;
  list<double>::iterator iter = wList.begin();
  while (iter != wList.end()) {
    if (wList.size() > 1) {
      checkPoint = (float)index / (float)(wList.size()-1);
      // check for valid checkpoint
      if (checkPoint < 0 || checkPoint > 1) {
        cerr << "Sieve::AddEnvelope -- checkpoint error:" << endl;
        cerr << "   checkPoint = " << checkPoint << endl;
      }
    } else {
      checkPoint = 0;
    }
    value = env->getValue(checkPoint, 1.);
    if (method == "VARIABLE") {
      probability = Random::PreferedValueDistribution(value, checkPoint);
    } else if (method == "CONSTANT") {
      probability = value;
    }

    //update this element
    *iter = *iter + probability;

    index++;
    iter++;
  }
}

//---------------------------------------------------------------------------//

void Sieve::CumulWeights() {
    double cumul = 0;

    NormalizewList();

    list<double>::iterator iter = wList.begin();
    while (iter != wList.end()) {
      cumul += *iter;
      *iter = cumul;
      iter++;
    }
}

//---------------------------------------------------------------------------//

void Sieve::NormalizewList() {
  double wListSum = 0;

  // get the sum of all the items
  list<double>::iterator iter = wList.begin();
  while (iter != wList.end()) {
    wListSum += *iter;
    iter++;
  }

  // go through the list again, normalizing all elements
  iter = wList.begin();
  while (iter != wList.end()) {
    *iter = *iter / wListSum;
    iter++;
  }
}

//---------------------------------------------------------------------------//

void Sieve::print_eList(){
  list<int>::iterator iter = eList.begin();
  while (iter != eList.end()) {
    cout << *iter << " ";
    iter++;
  }
  cout << endl;
  int sever; cin >> sever;
}

//---------------------------------------------------------------------------//

void Sieve::print_wList(){
  list<double>::iterator iter = wList.begin();
  while (iter != wList.end()) {
    cout << *iter << " ";
    iter++;
  }
  cout << endl;
  int sever; cin >> sever;
}

