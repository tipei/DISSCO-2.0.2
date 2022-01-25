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
/*
*  File Name     : MarkovModel.h
*  Date created  : March. 29 2018
*  Authors       : Fanbo Xiang, Sever Tipei
*  Organization  : Music School, University of Illinois at Urbana Champaign
*  Description   : This file contains the class of Markov model
*/

#ifndef MARKOVMODEL_H
#define MARKOVMODEL_H

#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

using std::vector;

template<typename T>
class MarkovModel {
public:
  /* Create a default Markov chain */
  MarkovModel(int size = 0);

  /* Get the number of states in the Markov chain */
  int getStateSize() const;

  /* Get the transition probability of a given state */
  const vector<double>& getTransitionProbabilities(int state) const;

  T getStateValue(int state) {
    return stateValues[state];
  }

  double getTransitionProbability(int i, int j) {
    return transitionMatrix[i][j];
  }

  double getInitialProbability(int state) {
    return initialDistribution[state];
  }

  std::string to_str();

  void from_str(std::string str);

  /* normalize the model to make sure initial distribution sum to 1
     and the matrix is a Markov Matrix
   */
  void normalize() { makeConsistent(); }

  /* valid only when normalized */
  T nextSample(double rand);
private:
  /* makes the transition matrix and emission vectors consistent */
  void makeConsistent();

  /* assert a given state is valid */
  void checkState(int state, std::string functionName="");

  vector< vector<double> > transitionMatrix;
  vector<double> initialDistribution;

  vector<T> stateValues;

  int thisIndex;
};


#endif /* MARKOVMODEL_H */
