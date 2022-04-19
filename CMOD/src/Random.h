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
//  random.h
//
//----------------------------------------------------------------------------//

#ifndef RANDOM_H
#define RANDOM_H

// CMOD includes
#include "Libraries.h"

#include "Define.h"

//----------------------------------------------------------------------------//

/**
*  Random is a static class which encapsulates functions for random number
*  generation.
**/
class Random {
  public:
        enum distribution_type {
                RAND_FLAT,
                RAND_TRIANGLE,
                RAND_SIGN
        };
    /**
    *  Seeds the random number generator
    **/
    static void Seed(unsigned int n);

    /**
    *  Seeds the random number generator by system time
    **/
    static void SeedBySystemTime();
	
    /**
    *  Returns a random number in the range [0.0, 1.0]
    **/
    static double Rand();

    /**
    *  Returns a random number in the range [low, high]
    **/
    static double Rand(double low, double high);

    /**
    *  Returns a random number in the range [low, high] based on a
    *  distribution type
    **/
    static double Rand(double low, double high, distribution_type 
     distribution);

    /**
    *  Returns a random number in the range [0.00, 1.00] based on a
    *  distribution type
    **/
    static double Rand(distribution_type distribution);

    /**
    *  Returns a random integer in the range [low, high] based on
    *  a flat distribution.
    **/
    static int RandInt(int low, int high);

    /**
     * (Experimental)
     * TODO: find a way to support multiple RandomOrderInt functions of identical bounds.
     * 
     * Returns a random integer in the range [low, high].
     * Avoids repetition, given that
     * - number of children is greater than or equal to
     *   range of [low, high]
     * - every RandomOrderInt function with range [low, high] and
     *   numChildren is unique
     * @param id - Distinguishes between unique calls of the randOrderInt function
     */
    static int RandOrderInt(int low, int high, int id);

    /**
    *  Randomly chooses a value from a list.
    *  There are multiple versions for float, double, and integer lists
    **/
    static float ChooseFromList(float array[], int size);
    static double ChooseFromList(double array[], int size);
    static int ChooseFromList(int array[], int size);

    /**
    *  Returns an integer value in the range [0, size] given
    *  a list of probabilities representing the probability
    *  of the corresponding index being selected.
    **/
    static int ChooseFromProb(std::vector<double> probs);

    /**
    *  Assigns probabilities to a list using a linear distribution
    *  based on index.
    **/
    static void AssignProb(std::list<double> &myProbList);

    /**
    *  Finds a probability through a comparison of an average situation with
    *  a desired or "prefered" situation.  Borrowed from MP1, NT expression
    *  \parm value The value being tested
    *  \parm checkPoint The moment in time (x axis) of the testing.
    **/
    static double PreferedValueDistribution(double value, double checkPoint);

    static unsigned int GetSeed() { return seed; }

  private:
    static unsigned int seed;
    double * probArray;

    /**
     * Returns a vector containing ints [low, high]
     */
    static std::vector<int> InitializeChoices(int low, int high);
};

#endif // _RANDOM_H_
