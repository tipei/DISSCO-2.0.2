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

//----------------------------------------------------------------------------//
// @author Fanbo Xiang (xiangfanbo@gmail.com)
// ProbabilityEnvelope.h
//
//----------------------------------------------------------------------------//

#ifndef __PROBABILITYENVELOPE_H
#define __PROBABILITYENVELOPE_H

#include "Envelope.h"

class ProbabilityEnvelope : public Envelope {
public:
    ProbabilityEnvelope();
    ProbabilityEnvelope(const Envelope& other);

    /**
     * Estimate the area under the envelope curve
     * \param num_steps number of steps to discretize the time axis of the
     * envelope
     */
    m_value_type estimateAreaUnderCurve(int num_steps);

    /**
     * generate an internal table to count value at each time step
     * \param num_steps number of steps to discretize the time axis of the
     * envelope
     */
    void generateCountTable(int num_steps);

    /**
     * generate a random sample from the distribution
     * \param x a random number between 0 and 1 from a uniform distribution
     */
    m_value_type sample(double x);

    void print();

private:
    /** the "height" of probability at each time step */
    vector<m_value_type> stepCounts;

    /** current step count + all previous step counts */
    vector<m_value_type> cumulativeStepCounts;

    /** integral of stepCounts over stepTimes*/
    m_value_type totalCounts;

    /** the time value of each time step */
    vector<m_value_type> stepTimes;
};

#endif /* __PROBABILITYENVELOPE_H */
