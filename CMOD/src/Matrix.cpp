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
//  matrix.cpp
//
//----------------------------------------------------------------------------//

#include "Matrix.h"
#include "Random.h"

//----------------------------------------------------------------------------//

extern EnvelopeLibrary envlib;

//----------------------------------------------------------------------------//

Matrix::Matrix(int numTypes, int numAttacks, int numDurations,
               vector<int> numTypesInLayers, int maxVal,
               Tempo tempo, bool sieveAligned)
               : sieveAligned(sieveAligned)
               , tempo(tempo) {

  matr.clear();
  matr.resize(numTypes);
  for (int type = 0; type < numTypes; type++) {
    // do for each type

    matr[type].resize(numAttacks);
    for (int attNum = 0; attNum < numAttacks; attNum++) {
      // do for each attack

      matr[type][attNum].resize(numDurations);
      for (int durNum = 0; durNum < numDurations; durNum++) {
        // do for each dur

        matr[type][attNum][durNum].attdurprob = 0.0;
        matr[type][attNum][durNum].normprob = 0;
        matr[type][attNum][durNum].type = type;
        matr[type][attNum][durNum].stime = 0;
        matr[type][attNum][durNum].dur = 0;
      }
    }
  }

  for (int i = 0; i < numTypesInLayers.size(); i++) {
    for (int j = 0; j < numTypesInLayers[i]; j++) {
      typeLayers.push_back( i );
    }
  }

  sweepStart = 0;

  beatEDUs = tempo.getEDUPerTimeSignatureBeat().Num();
}


//----------------------------------------------------------------------------//

Matrix::Matrix(const Matrix &orig) {
  matr = orig.matr;
  typeLayers = orig.typeLayers;
  typeProb = orig.typeProb;
}

//----------------------------------------------------------------------------//

Matrix& Matrix::operator=(const Matrix &orig) {
  matr = orig.matr;
  typeLayers = orig.typeLayers;
  typeProb = orig.typeProb;
}

//----------------------------------------------------------------------------//

Matrix::~Matrix() {
}

//----------------------------------------------------------------------------//

void Matrix::setAttacks(Sieve* attackSieve, vector<Envelope*> attackEnvs) {
  vector<int> attTimes;
  vector<double> attProbs;

  attackSieve->FillInVectors(attTimes, attProbs);

  bool hasEnv = attackEnvs.size() >= matr.size();

  // add the results into the matrix
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack
      double attackSieveValue = attProbs[attNum];

//    cout << "Matrix::setAttacks - attNum=" << attNum << " attackSieveValue="
//         << attackSieveValue << endl;

      if (attNum < 0 || attNum >= matr[type].size()) {
        cerr << "Matrix::setAttacks -- error - getValue out of bounds;" << endl;
        cerr << "  attNum=" << attNum << ", matr[type].size()="
             << matr[type].size() << endl;
      }

      int attackStime = attTimes[attNum];
      att.insert(attackStime);
      //cout << "ATTS " << attackStime << endl;
      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        matr[type][attNum][durNum].attdurprob += attackSieveValue;
        if (hasEnv) {
          double attackEnvValue = attackEnvs[type]->getValue(attNum, matr[type].size());
          matr[type][attNum][durNum].attdurprob *= attackEnvValue;
        }
        matr[type][attNum][durNum].stime = attackStime;
/*
        cout << "Matrix::setAttacks - matr[" << type << "][" << attNum << "]["
             << durNum << "].attdurprob=" << matr[type][attNum][durNum].attdurprob
             << endl;
*/
      }
    }
  }

  cout <<  "Matrix::setAttacks valid time: " ;
  for(int i = 0; i < attTimes.size(); i++){
    if(attTimes[i] > beatEDUs){
      break;
    }
    short_attime.push_back(attTimes[i]);
    cout << attTimes[i] << " , ";
  }
  cout << endl;
}

//----------------------------------------------------------------------------//

void Matrix::setDurations(Sieve* durSieve, int maxVal, vector<Envelope*> durEnvs) {
  vector<int> durTimes;
  vector<double> durProbs;
  durSieve->FillInVectors(durTimes, durProbs);

  int start;
  int durEnd;

  // this marks the end-window of the parent event
  int maxStartTime = matr[0][matr[0].size()-1][0].stime;

  bool hasEnv = durEnvs.size() >= matr.size();
  //int oldType = 0;

  // add the results into the matrix
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        double durSieveVal = durProbs[durNum];

        if (durNum < 0 || durNum > matr[type][attNum].size()) {
          cerr << "Matrix::setDurations -- error - getValue out of bounds;" << endl;
          cerr << "  durNum=" << durNum << ", matr[type][attNum].size()="
              << matr[type][attNum].size() << endl;
        }
/*
	if(type != oldType) {
	  cout << "Matrix::setDurations - envelope for type " << type << endl;
	  durEnvs[type] -> print();
	  int sever; cin >> sever;
	  oldType = type;
        }
*/

        matr[type][attNum][durNum].dur = durTimes[durNum];

        durEnd = matr[type][attNum][durNum].stime + matr[type][attNum][durNum].dur;
/*
	cout << endl;
	cout << "durEnd=" << durEnd << " maxVal=" << maxVal << endl;
        cout << "  matr[" << type << "][" << attNum
      	     << "][" << durNum << "].stime=" << matr[type][attNum][durNum].stime
	     << " matr[" << type << "][" << attNum << "][" << durNum
	     << "].attdurprob=" << matr[type][attNum][durNum].attdurprob << endl;
*/
        if ((   sieveAligned
             && att.count(durEnd % tempo.getEDUPerTempoBeat().Num()) == 0
             && att.count(durEnd) == 0)
             || durEnd > maxVal) {
          matr[type][attNum][durNum].attdurprob = 0;
/*
          cout << "sieveAligned - type=" << type << " attNum=" << attNum
               << " durNum=" << durNum << " maxVal=" << maxVal << endl;
          int sever; cin >> sever;
*/
        } else {
          matr[type][attNum][durNum].attdurprob *= durSieveVal;
          if (hasEnv) {
            double durEnvVal = durEnvs[type]->getValue(durNum, matr[type][attNum].size());

//      cout << "Matrix::setDurations - durEnvVal=" << durEnvVal << endl;
//	int sever; cin >> sever;
            matr[type][attNum][durNum].attdurprob *= durEnvVal;
          }
//        cout << "DUREND " << durEnd << endl;
        }
/*
          cout << "  stime=" << matr[type][attNum][durNum].stime << " dur=" <<
	   matr[type][attNum][durNum].dur << endl;
	  cout << "    durSieveVal=" << durSieveVal
	   << " matr[" << type << "][" << attNum << "][" << durNum
	   << "].attdurprob=" << matr[type][attNum][durNum].attdurprob << endl;
*/
      }
    }
  }
}

//---------------------------------------------------------------------------//
void Matrix::setTypeProbs(vector<double> typeProbVect) {
  typeProb = typeProbVect;
}


//----------------------------------------------------------------------------//
MatPoint Matrix::chooseSweep(int remain) {
  MatPoint chosenPt = choose();

  // set probs to 0 for every point starting before the end of chosenPt
  removeSweepConflicts(chosenPt);

  // recompute the type prob vector
  recomputeTypeProbs(chosenPt.type, remain);

  return chosenPt;
}

MatPoint Matrix::chooseContinuum() {
  return choose();
}


//---------------------------------------------------------------------------//
MatPoint Matrix::chooseDiscrete(int remain) {
  MatPoint chosenPt = choose();

  // remove conflicts in the matrix (set probs to 0)
  removeConflicts(chosenPt);

  // recompute the type prob vector
  recomputeTypeProbs(chosenPt.type, remain);
  return chosenPt;
}


//---------------------------------------------------------------------------//
MatPoint Matrix::choose() {
  bool found = false;
  MatPoint chosenPt;
  MatPoint prevPt;
  double randNum = Random::Rand();

  // normalize the matrix (probs will add up to 1.0)
  bool success = normalizeMatrix();
  if (!success) {
    // failed to normalize -- out of space in the matrix
    MatPoint failed;
    failed.type = -1;
    return failed;
  }

  // find the nearest prob to that random in the matrix
  for (int type = 0; !found && type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; !found && attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; !found && durNum < matr[type][attNum].size(); durNum++) {
        // do for each dur

        // look for the closest prob, greater than the rand num
        if (matr[type][attNum][durNum].normprob >= randNum) {
          found = true;
          chosenPt = matr[type][attNum][durNum];

          //printing stuff
/*
          cout << endl << "MATRIX ---- PICKED: matr[" << type << "]["
              << attNum << "][" << durNum << "]  =  start:"
              << chosenPt.stime << ", dur:" << chosenPt.dur << ", type:"
              << chosenPt.type << ", randNum=" << randNum << endl;
	  cout << "  chosenPt.normprob=" << chosenPt.normprob << ", prevPt.normprob="
	       << prevPt.normprob << ", %prob:"
	       << (chosenPt.normprob-prevPt.normprob) << endl;
	  cout << "  " << endl;
//        printMatrix(true);
	  int sever; cin >> sever;
*/
        }
        prevPt = matr[type][attNum][durNum];
      }
    }
  }

  return chosenPt;
}

//---------------------------------------------------------------------------//

bool Matrix::normalizeMatrix() {
  double matrSum = 0;
  double lastSum = 0;
  // get the sum of the matrix
  for (int type = 0; type < matr.size(); type++) {
/*attackSieve
    cout << "		Matrix::normalizeMatrix - type=" << type
         << " matr.size=" << matr.size()
         << " matr[" << type << "].size=" << matr[type].size() << endl;
*/

    // do for each type
    for (int attNum = 0; attNum < matr[type].size(); attNum++) {

      // do for each attackattackSieve
      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {

        // do for each dur
        matr[type][attNum][durNum].normprob =
                matr[type][attNum][durNum].attdurprob * typeProb[type];
        matrSum += matr[type][attNum][durNum].normprob;

/*
 /	if(type == 0) {
	  cout << "Matrix::normalizeMatrix - typeProb[" << type << "]="
		<< typeProb[type] << endl;
	  cout << " matr[" << type << "][" << attNum << attackSieve"][" << durNum
		<< "].normprob= " << matr[type][attNum][durNum].normprob << endl;
	  cout << "      matrSum=" << matrSum << endl;
  	}
*/

      }
    }
  }

  if (matrSum == 0) {
    cerr << "MATRIX - ERROR: Sum of matrix is 0! (We're out of space)." << endl;
    return false; // indicate failure
  }


  // set the matrix probs to add up to 1
  for (int type = 0; type < matr.size(); type++) {
    // do for each type

    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      // do for each attack

      for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
        // do for each dur
        lastSum += (matr[type][attNum][durNum].normprob) / matrSum;
        matr[type][attNum][durNum].normprob = lastSum;
/*
 /	if(type == 0) {
	  cout << "		lastSum=" << lastSum << endl;
 /	}
*/
      }
    }
  }
  return true; // success!
}


//---------------------------------------------------------------------------//

void Matrix::recomputeTypeProbs(int chosenType, int remaining) {
  if (remaining != 0) {
    for (int i = 0; i < typeProb.size(); i++) {
      if (i == chosenType) {
        typeProb[i] = round(typeProb[i] * (remaining + 1) - 1) / remaining;
      } else {
        typeProb[i] = (typeProb[i] * (remaining + 1)) / remaining;
      }
    }
  }
}

//----------------------------------------------------------------------------//
int Matrix::verify_valid(int endTime){
  int length = short_attime.size();

  int low = 0;
  int high = length - 1;
  int eTime = endTime % beatEDUs;
  while(high > low+1){
    int mid = (high+low) / 2;
    if(short_attime[mid] < eTime){
      low = mid;
    } else if(short_attime[mid] > eTime) {
      high = mid;
    } else {
      return endTime;
    }
  }
  //cout << "endTime: " << eTime << " high_n: " << valid_time[high] << " low_n: " << valid_time[low] <<  endl;
  int offset = short_attime[high] - eTime <= eTime - short_attime[low] ? short_attime[high] - eTime : short_attime[low] - eTime;
  //cout << "endTime: " << endTime << " choose: " << endTime + offset << endl;
  return endTime + offset;
}
//----------------------------------------------attackSieve------------------------------//

void Matrix::removeConflicts(MatPoint &chosenPt) {
  int chosenStart = chosenPt.stime;
  //int chosenEnd = chosenStart + chosenPt.dur;
  int chosenEnd = verify_valid(chosenStart + chosenPt.dur);
  chosenPt.dur = chosenEnd - chosenStart;
  int chosenLayer = typeLayers[chosenPt.type];


  for (int type = 0; type < matr.size(); type++) {
    // only remove conflicts if this type is in the same layer
    //   as the chosen type
    if ( chosenLayer == typeLayers[type] ) {

      for (int attNum = 0; attNum < matr[type].size(); attNum++) {
        // do for each attackattackSieve

        for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
          // do for each dur

          int currStart = matr[type][attNum][durNum].stime;
          int currEnd = currStart + matr[type][attNum][durNum].dur;

          if (chosenStart >= currStart && chosenStart < currEnd) {
            // start time is in this window --- set prob to 0
            matr[type][attNum][durNum].attdurprob = 0;
          } else if ( chosenEnd > currStart && chosenEnd <= currEnd) {
            // start time is in this window --- set prob to 0
            matr[type][attNum][durNum].attdurprob = 0;
          } else if ( chosenStart <= currStart && chosenEnd >= currEnd) {
            // start time is in this window --- set prob to 0
            matr[type][attNum][durNum].attdurprob = 0;
          }
        }
      }
    }
  }

  //cout << "removeConflicts : start: " << chosenStart << " end: " << chosenEnd <<  endl;
}

//----------------------------------------------------------------------------//

void Matrix::removeSweepConflicts(MatPoint &chosenPt) {
  int chosenStart = chosenPt.stime;
  int chosenEnd = verify_valid(chosenStart + chosenPt.dur);
  chosenPt.dur = chosenEnd - chosenStart;
  int chosenLayer = typeLayers[chosenPt.type];

  for (int type = 0; type < matr.size(); type++) {
    // only remove conflicts if this type is in the same layer
    //   as the chosen type
    if ( chosenLayer == typeLayers[type] ) {

      for (int attNum = 0; attNum < matr[type].size(); attNum++) {
        // do for each attack

        for (int durNum = 0; durNum < matr[type][attNum].size(); durNum++) {
          // do for each dur

          int currStart = matr[type][attNum][durNum].stime;
          int currEnd = currStart + matr[type][attNum][durNum].dur;

          if (chosenEnd > currStart) {
            matr[type][attNum][durNum].attdurprob = 0;
          } else{
            sweepStart = attNum;
            return;
          }
        }
      }
    }
  }
  //cout << "removeSweepConflicts : start: " << chosenStart << " end: " << chosenEnd <<  endl;
}

//----------------------------------------------------------------------------//

void Matrix::printMatrix(bool normalized) {
  if (normalized) {
    cout << "$$$$$$$$$$$$$$$$$    NORMALIZED MATRIX      "
         << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
  } else {
    cout << "&&&&&&&&&&&&&&&&&    UN-NORMALIZED MATRIX   "
         << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
  }

  for (int type = 0; type < matr.size(); type++) {
    printf("Type %-2i", type);

    //print out a header row of possible start times
    for (int attNum = 0; attNum < matr[type].size(); attNum++) {
      printf("%7i  ", matr[type][attNum][0].stime);
    }
    cout << endl;

    // print the left column as a possible dur value, followed by probs
    for (int durNum = 0; durNum < matr[type][0].size(); durNum++) {
      printf("%5i  ", matr[type][0][durNum].dur);

      // print the probs
      for (int attNum = 0; attNum < matr[type].size(); attNum++) {
        if (normalized) {
          printf("%6.5f  ", matr[type][attNum][durNum].normprob);
        } else {
          printf("%6.5f  ", matr[type][attNum][durNum].attdurprob);
        }
      }
      cout << endl;
    }

  }

  if (normalized) {
    cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
         << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
  } else {
    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"
         << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
  }


}
