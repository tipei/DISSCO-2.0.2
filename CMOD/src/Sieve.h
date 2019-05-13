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
//	Sieve.h
//
//      This class deals with seves or logical filters.
//      See Xenakis "Formalized Music", Tipei 1981 ICMC proceedings and
//      Christopher Ariza "AthenaCL" at www.athenacl.org
//      \author Sever Tipei     January 27, 2006
//
//---------------------------------------------------------------------------//

#ifndef SIEVE_H
#define SIEVE_H

// CMOD includes
#include "Libraries.h"

#include "Define.h"

//---------------------------------------------------------------------------//


class Sieve {
  private:
    string fileName;
    std::list<int> eList;      //list of elements allowed by the sieve
    std::list<double> wList;   //probabilities for sieve elements
    int skip;             //elemets < minVal, skipped

  public:

    /**
    *  Generic constructor; creates an empty list of integers (elist) and an
    *  empty list of doubles (wList).  Sets size = 0 and skip = 0.
    **/
    Sieve();


    /**
    *  Constructor for (almost) empty object.  Creates an empty list of
    *  integers (elist) and an empty list of doubles (wList).  Sets size = 0
    *  and skip = 0.  Sets the related file name SIEVE/...
    *  \param name of related file for this sieve.
    **/
    Sieve(std::string aFile);


    /**
    *  Destructor
    **/
    ~Sieve();

    /**
    *  Returns the Sieve file name
    *  \return name of the Sieve file
    **/
    std::string getFileName();

    /** UNDER CONSTRUCTION
    *  Build constructs the sieve: an expression specifying a list of integers
    *  \param minVal smallest value allowed on list
    *  \param maxVal largest value allowed on list
    *  \param eMethod method for selecting elements of sieve
    *  \param wMethod method for selecting weights
    *  \param expr string to parse into a list
    *  \param wArgVector data for selecting weights
    **/
    void BuildFromExpr(int minVal, int maxVal,
                       const char *eMethod, const char *wMethod,
                       std::string expr, std::vector<int> wArgVect, std::vector<int> offsetVect);

    /**
    *  Build constructs the sieve: a list of elements and a list of weights
    *  \param minVal smallest value allowed on list
    *  \param maxVal largest value allowed on list
    *  \param eMethod method for selecting elements of sieve
    *  \param wMethod method for selecting weights
    *  \param eArgVector data for selecting elements of sieve
    *  \param wArgVector data for selecting weights
    *  \param offsetVector offset of the elements
    **/
    void Build(int minVal, int maxVal,
               const char *eMethod, const char *wMethod,
               std::vector<int> eArgVect, std::vector<int> wArgVect, std::vector<int> offsetVect);

    /**
    *  FillInVectors calls CumulWeights to normalize and and add the result in
    *  a cumulative way resulting in probabilities from 0 to 1 and then copies
    *  them into the vectors
    *  \param intVect a vector of ints
    *  \param doubleVect a vector of doubles
    **/
    void FillInVectors(std::vector<int>& intVect, std::vector<double>& doubleVect);

    /**
     *  Returns the number of items that can be returned by this sieve
     **/
    int GetNumItems();

    /**
    *  Modify adds the values provided by a modulating envelope to the
    *  probability values of the sieve, adds the result in a cumulative way
    *  resulting in probabilities from 0 to 1 and chooses an element of the
    *  sieve by using ChooseL
    *  \param Envelope *env an envelope
    *  \param method name of the method used
    *  \return value chosen by ChooseL
    **/
    int Modify(Envelope *env, std::string method);

    /**
     *   This function chooses an element from a list of integers by matching its
     *   probability of occurence stored in a corresponding list of doubles with a
     *   random number.
     **/
    int ChooseL();

//---------------------------------------------------------------------------//

  private:
    class Token;
    /**
    *  Elemets offers a choice between different methods of selecting the
    *  elements of a sieve to be stored in a linkList of integers.
    *  \param minVal smallest value allowed by the sieve
    *  \param maxVal largest value allowed by the sieve
    *  \param eArgVect vector containing the elements of the sieve
    *  \param offsetVector offset of the elements
    **/
    void Elements(int minVal, int maxVal,
                  const char *method,
                  std::vector<int> eArgVect,std::vector<int> offsetVect);

    /**
    *  Weights assigns weights to be stored in a linkList of doubles.
    *  by calling methods like PERIODIC, HIERARCHIC, INCLUDE, etc.
    *  \param method method of assigning weights
    *  \param wArgVect vector of ints contains info about weights
    **/
    void Weights(std::vector<int> eArgVect,
		 const char *method,
                 std::vector<int> wArgVect,
		 std::vector<int> offsetVect);

    /**
    *  Meaningful makes a list of elements which are meaningful (pre-
    *  selected).
    *  \param minVal smallest value allowed
    *  \param maxVal largest value allowed
    *  \param eArgVect list of available values
    *  \param offsetVector offset of elements
    **/
    void Meaningful(int minVal, int maxVal, std::vector<int> eArgVect, std::vector<int> offsetVect);

    /**
    *  Multiples makes a linkList of multiples of given moduli numbers
    *  (equivalence classes) within min and max values.
    *  \param minVal smallest value allowed
    *  \param maxVal largest value allowed
    *  \param numMods vector containing the moduli to be used
    *  \param offsetVector offset of elements
    **/
    void Multiples(int minVal, int maxVal,
                   std::vector<int> numMods, std::vector<int> offsetVect);

    /**
    *  Fake.  Lists all elemets within a given range.
    *  \param minVal smalles element allowed by sieve
    *  \param maxVal larges element allowed by sieve
    **/
    void Fake(int minVal, int maxVal);

    /**
    *  PeriodicWeights assigns weights to the elemets of a sieve by cycling
    *  through a given ordered list of weights.  It stores them in a list
    *  of weights (wList) of the same size as a companion list of sieve
    *  elemets (eList).
    *  \param wArgVector vector of ints contains info about weights
    **/
    //TODO: you should really pass in value by reference, try change it to other functions
    void PeriodicWeights(const std::vector<int>& wArgVect);

    /**
    *  HierarchicWeights assigns weights to the elemets of a sieve by
    *  adding for each elemet the probabilities associated with each
    *  equivalence class and stores them in the wList linkList.
    *  \param wArgVector vector of ints contains info about weights
    **/
    void HierarchicWeights(const std::vector<int>& eArgVect,
			   std::vector<int> wArgVect,
			   std::vector<int> offestVect);

    /**
    *  IncludeWeights reads weights corresponding to a list of elements.
    *  Used frequently as a companion to the Meaningful function.
    *  \param wArgVector vector of ints contains info about weights
    **/
    void IncludeWeights(const std::vector<int>& wArgVect);

    /**
    *  AddEnvelope uses a function/envelope which  defines a distribution
    *  within a range after a sieve was already constructed.  It normalizes
    *  the wList containing the weights, and, for each *  of its locations,
    *  finds a value on the (input) distribution function.
    *  Then, if VARIABLE is chosen, modifies that distribution through the
    *  comparison with a prefered value at that moment.  The wList is
    *  updated by adding the new probability to the existing weight of
    *  each sieve location.  Used only with the wList of probabilities.
    *  \param env an envelope describing a distribution
    *  \param method to be usedin choosing the envelope/distribution
    **/
    void AddEnvelope(Envelope *env, std::string method);

    /**
    *  Cumulative weights.  Each weigh on the list is divided by their sum
    *  and then added to a cumulative weight or probability.
    **/
    void CumulWeights();

    /**
    *  CumulArray.  Takes an array of doubles, divides each element by the sum of
    *  all elements, and adds the value to the preceding value (sum of values) so
    *  that the values range from 0 to 1.
    *   ====  Not used; use Utility::CumulArray if needed  ====
    **/
//  void CumulArray(int size);

    void NormalizeWList();

};
#endif
