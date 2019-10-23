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
/*
 *  File Name     : MarkovModel.h
 *  Date created  : March. 29 2018
 *  Authors       : Fanbo Xiang, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains the class of Markov model
 */

#ifndef MARKOVMODEL_H
#define MARKOVMODEL_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::vector;

template <typename T>
class MarkovModel {
public:
    /* Create a default Markov chain */
    MarkovModel(int size = 0);

    /* Get the number of states in the Markov chain */
    int getStateSize() const;

    /* Get the transition probability of a given state */
    const vector<double>& getTransitionProbabilities(int state) const;

    T getStateValue(int state) { return stateValues[state]; }

    double getTransitionProbability(int i, int j) { return transitionMatrix[i][j]; }

    double getInitialProbability(int state) { return initialDistribution[state]; }

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
    void checkState(int state, std::string functionName = "");

    vector<vector<double> > transitionMatrix;
    vector<double> initialDistribution;

    vector<T> stateValues;

    int thisIndex;
};

template <typename T>
void MarkovModel<T>::checkState(int state, std::string functionName) {
    if (state < 0 || state >= transitionMatrix.size()) {
        std::cerr << functionName << ".State Check Failed. Unexpected state: " << state
                  << std::endl;
        throw;
    }
}

template <typename T>
MarkovModel<T>::MarkovModel(int size) {
    thisIndex = -1;
    transitionMatrix.resize(size);
    stateValues.resize(size);
    initialDistribution.resize(size);
    for (int i = 0; i < transitionMatrix.size(); i++) {
        transitionMatrix[i].resize(size);
    }
}

template <typename T>
int MarkovModel<T>::getStateSize() const {
    return transitionMatrix.size();
}

template <typename T>
const vector<double>& MarkovModel<T>::getTransitionProbabilities(int state) const {
    checkState(state, "MarkovModel::getTransitionProbabilities()");
    return transitionMatrix[state];
}

template <typename T>
void MarkovModel<T>::makeConsistent() {
    for (int i = 0; i < transitionMatrix.size(); i++) {
        vector<double>& row = transitionMatrix[i];
        // find row sum
        double sum = 0;
        for (int i = 0; i < row.size(); i++) {
            sum += row[i];
        }

        if (sum == 0) {  // when sum is 0, use uniform distribution
            for (int i = 0; i < row.size(); i++) {
                row[i] = 1.0 / row.size();
            }
        } else {  // otherwise, normalize
            for (int i = 0; i < row.size(); i++) {
                row[i] /= sum;
            }
        }
    }

    // make sure initial distribution is a distribution
    double sum = 0;
    for (int i = 0; i < initialDistribution.size(); i++) {
        sum += initialDistribution[i];
    }
    if (sum == 0) {
        for (int i = 0; i < initialDistribution.size(); i++) {
            initialDistribution[i] = 1.0 / initialDistribution.size();
        }
    } else {
        for (int i = 0; i < initialDistribution.size(); i++) {
            initialDistribution[i] /= sum;
        }
    }
}

template <typename T>
std::string MarkovModel<T>::to_str() {
    std::stringstream ss;
    ss << getStateSize() << std::endl;
    for (int i = 0; i < stateValues.size(); i++) {
        ss << stateValues[i] << " ";
    }
    ss << std::endl;
    for (int i = 0; i < initialDistribution.size(); i++) {
        ss << initialDistribution[i] << " ";
    }
    ss << std::endl;
    for (int i = 0; i < transitionMatrix.size(); i++) {
        for (int j = 0; j < transitionMatrix[i].size(); j++) {
            ss << transitionMatrix[i][j] << " ";
        }
    }
    return ss.str();
}

template <typename T>
void MarkovModel<T>::from_str(std::string str) {
    std::stringstream ss(str);

    // resize
    int size;
    ss >> size;
    transitionMatrix.resize(size);
    stateValues.resize(size);
    initialDistribution.resize(size);

    for (int i = 0; i < transitionMatrix.size(); i++) {
        transitionMatrix[i].resize(size);
    }

    for (int i = 0; i < size; i++) {
        ss >> stateValues[i];
    }

    for (int i = 0; i < size; i++) {
        ss >> initialDistribution[i];
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            ss >> transitionMatrix[i][j];
        }
    }
}

template <typename T>
T MarkovModel<T>::nextSample(double rand) {
    if (thisIndex < 0) {
        double sum;
        int i = 0;
        for (; i < initialDistribution.size(); i++) {
            sum += initialDistribution[i];
            if (sum >= rand) break;
        }
        if (i == initialDistribution.size()) i -= 1;
        thisIndex = i;
        return stateValues[i];
    } else {
        double sum;
        int i = 0;
        for (; i < transitionMatrix[thisIndex].size(); i++) {
            sum += transitionMatrix[thisIndex][i];
            if (sum >= rand) break;
        }
        if (i == transitionMatrix[thisIndex].size()) i -= 1;
        thisIndex = i;
        return stateValues[i];
    }
}

#endif /* MARKOVMODEL_H */
