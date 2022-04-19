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
//  random.cpp
//
//----------------------------------------------------------------------------//

#include "Random.h"

//----------------------------------------------------------------------------//

unsigned int Random::seed = 0;

//----------------------------------------------------------------------------//

void Random::Seed(unsigned int n) { // Allow seed to be changed multiple times for brewing piece

  seed = n;
  srand(n);
}

//----------------------------------------------------------------------------//

void Random::SeedBySystemTime() {

  if (seed == 0){
  // Get the current system time and seed it
  Seed(time(NULL));
  }
}

//----------------------------------------------------------------------------//

double Random::Rand() {

  return (double)rand() / ((double)RAND_MAX + 1.0);
}

//----------------------------------------------------------------------------//

double Random::Rand(double low, double high) {

  return Rand() * (high - low) + low;
}

//----------------------------------------------------------------------------//

double Random::Rand(double low, double high, Random::distribution_type
 distribution) {
  double randNum = Rand();

  switch (distribution) {
    case RAND_FLAT:
      return Rand(low, high);
      break;

    case RAND_TRIANGLE:
      return  (Rand(low, high) + Rand(low, high)) * 0.5 * (high - low) + low;
      break;

    case RAND_SIGN:

      if (randNum <= 0.5)
        return Rand(low, high);
      else
        return Rand(low, high)*-1.0;
        break;

    default:
      return 0.0;
  }
}

//----------------------------------------------------------------------------//

double Random::Rand(Random::distribution_type distribution) {

        return Rand(0.0,1.0,distribution);
}


//----------------------------------------------------------------------------//

int Random::RandInt(int lowNum, int highNum) {
  int range = (highNum - lowNum) + 1;
  int result = lowNum + (int)( range * Rand() );
  return result;
}

//----------------------------------------------------------------------------//

int Random::RandOrderInt(int low, int high, int id) {
  static map<int, vector<int> > choicesMap;
  
  // Initialize choices if a new random order function is found, or
  // an existing list of choices is empty (occurs when high - low + 1 < numChildren)
  if (choicesMap.find(id) == choicesMap.end() || choicesMap[id].empty()) {
    choicesMap[id] = InitializeChoices(low, high);
  }

  vector<int>& choices = choicesMap[id];

  // Choose a random element from available choices
  int randIndex = RandInt(0, choices.size() - 1);
  int result = choices[randIndex];

  // Remove chosen element from choices
  choices.erase(choices.begin() + randIndex);

  return result;
}

//----------------------------------------------------------------------------//

float Random::ChooseFromList(float array[], int size) {

  return array[RandInt(0, size-1)];
}

//----------------------------------------------------------------------------//

double Random::ChooseFromList(double array[], int size) {

  return array[RandInt(0, size-1)];
}

//----------------------------------------------------------------------------//

int Random::ChooseFromList(int array[], int size) {

  return array[RandInt(0, size-1)];
}

//----------------------------------------------------------------------------//

int Random::ChooseFromProb(vector<double> probs) {
  double randomNumber = Rand();

  for (int i = 0; i < probs.size(); i++) {
    if (randomNumber <= probs[i]) {
      return i;
    }
  }

  std::cerr << "Error in Random::ChooseFromProb" << std::endl;
  std::cerr << "         probs.size()=" << probs.size() << ", randNum=" << randomNumber << std::endl;
  std::cerr << "         probs=< ";
  for (int i = 0; i < probs.size(); i++) std::cerr << probs[i] << " ";
  std::cerr << ">" << std::endl;
  exit(1);
}

//----------------------------------------------------------------------------//

void Random::AssignProb(list<double> &myProbList) {
  double len = myProbList.size();
  double prob = 0.0;

  list<double>::iterator iter = myProbList.begin();
  while (iter != myProbList.end()) {
    prob += (1 / len);
    *iter = prob;
    iter++;
  }
}

//----------------------------------------------------------------------------//

double Random::PreferedValueDistribution(double value, double checkPoint) {
  double probability;

  probability = exp(FIRST_CONST + SECOND_CONST * pow(2, (value - 0.5)) *
   pow(2, checkPoint));

  return probability;
}

//----------------------------------------------------------------------------//

vector<int> Random::InitializeChoices(int low, int high) {
  vector<int> choices;
  for (int i = low; i <= high; i++) {
    choices.push_back(i);
  }
  return choices;
}
