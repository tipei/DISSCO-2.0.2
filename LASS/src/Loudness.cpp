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
//
//	Loudness.cpp
//
//----------------------------------------------------------------------------//
// Comment!!! -ndvk

#ifndef __LOUDNESS_CPP
#define __LOUDNESS_CPP

//----------------------------------------------------------------------------//

#include "Loudness.h"
#include "InterpolatorTypes.h"
#include "Partial.h"

//----------------------------------------------------------------------------//




//----------------------------------------------------------------------------//
void Loudness::calculate(Sound& snd, m_rate_type rate)
{
    // count the number of Partials:
    int numPartials = snd.size();

    // Create a linear interpolator for each partial.
    // this will become that partial's loudness scaling factor
    vector<LinearInterpolator> scalingFactors(numPartials);
    
    // Set each partial's Frequency
    // Dynamic Variable to the proper duration and sampling rate:
    m_time_type duration = snd.getParam(DURATION);
    // Also, Collect a frequency iterator from each partial
    vector< Iterator<m_value_type> > freqIter;
    // finally, grab the maximum waveshape value from each partial
    vector<m_value_type> maxWaveShape;
    // go:
    for (int i=0; i<numPartials; i++)
    {
        Partial& part = snd.get(i);
        part.getParam(FREQUENCY).setDuration(duration);
        part.getParam(FREQUENCY).setSamplingRate(rate);
        freqIter.push_back( part.getParam(FREQUENCY).valueIterator() );
        maxWaveShape.push_back( part.getParam(WAVE_SHAPE).getMaxValue() );
    }

    // create a vector of critical band objects (one for each valid band)
    vector<CriticalBand> CBands;
    for (int i=0; i<NUM_BANDS; i++)
            CBands.push_back( CriticalBand(i) );
    
    // calculate the number of samples to do loudness on:
    m_sample_count_type numSamples = (m_sample_count_type) ((m_time_type)rate * (m_time_type)duration);
    
    // iterate over time:
    for (int s=0; s<numSamples; s++)
    {
        // calculate a relative time [0,1]
        m_time_type relativeTime = float(s) / float(numSamples);
        
        // the loudestPartial:
        m_value_type maxAmp = 0.0;
        
        // for each partial
        for (int p=0; p<numPartials; p++)
        {
            // get the values:
            m_value_type freq = freqIter[p].next();
            m_value_type waveShape = maxWaveShape[p];
                        
            // scale the amplitude by the relative amplitude:
            m_value_type thisAmp = waveShape * snd.get(p).getParam(RELATIVE_AMPLITUDE);
            
            // is this the loudest partial?
            if (thisAmp > maxAmp) maxAmp = thisAmp;
        
            // create a snapshot for this moment in time:
            PartialSnapshot snap(p, freq, waveShape);
            
            // add this snapshot to the appropriate critical band:
            CBands[criticalBandIndex(freq)].partials_.push_back(snap);
        }
        
        // calculate the band gamma for each band:
        vector<m_value_type> bandGamma(NUM_BANDS);
        for (int i=0; i<NUM_BANDS; i++)
            bandGamma[i] = CBands[i].getBandGamma(maxAmp);
        
        // find the maximum band gamma:
        m_value_type maxGamma = 0;
        for (int i=0; i<NUM_BANDS; i++)
            if (bandGamma[i] > bandGamma[(int)maxGamma]) bandGamma[(int)maxGamma] = bandGamma[i];

        // calculate the numerator:
        m_value_type gammaTotal = 0.0;
        for (int i=0; i<NUM_BANDS; i++)
            if (i != maxGamma) gammaTotal += bandGamma[i] * BANDS[i][F_FACTOR];
        m_value_type numerator = snd.getParam(LOUDNESS) / (bandGamma[(int)maxGamma] + gammaTotal);
        
        // for each band:
        for (int b=0; b<NUM_BANDS; b++)
        {
            int size = CBands[b].partials_.size();
            // for each partial
            for (int p=0; p<size; p++)
            {
                //calculate the scaling factor
                m_value_type scaleFactor = CBands[b].partials_[p].getScalingFactor(
                    b, maxAmp, numerator);
                //grab the id of this partial:
                int partial_id = CBands[b].partials_[p].ID_;
                
                // scale the factor by the relative amplitude again:
                scaleFactor *= snd.get(partial_id).getParam(RELATIVE_AMPLITUDE);

                //add the scaling factor for this moment in time.
                scalingFactors[partial_id].addEntry(relativeTime, scaleFactor);
            }
        }
        
        
        // clear the critical bands:
        for (int i=0; i<NUM_BANDS; i++)
            CBands[i].partials_.clear();
        
        
    } // end iterating over time

    // set every partial's scaling factors:
    for (int p=0; p<numPartials; p++)
        snd.get(p).setParam(LOUDNESS_SCALAR, scalingFactors[p]);
}



//----------------------------------------------------------------------------//
int Loudness::criticalBandIndex(m_value_type freq)
{
    // border conditions:
    if (freq < BANDS[0][LOWER_BOUND])
    {
        return 0;
        cout << "WARNING: Frequency (" << freq 
             << ") outside of any critical bands." << endl;
    }
    if (freq > BANDS[NUM_BANDS-1][UPPER_BOUND])
    {
        return NUM_BANDS-1;
        cout << "WARNING: Frequency (" << freq 
             << ") outside of any critical bands." << endl;
    }
    
    // common case:
    for (int i=0; i<NUM_BANDS; i++)
    {
        if (freq < BANDS[i][UPPER_BOUND])
            return i;
    }
    
    cout << "ERROR: didn't find a critical band for this frequency." << freq << endl;
    return NUM_BANDS-1;
}


//----------------------------------------------------------------------------//
//		CRITICAL BAND CLASS
//----------------------------------------------------------------------------//
    
//----------------------------------------------------------------------------//
Loudness::CriticalBand::CriticalBand(int ID)
    :ID_(ID) {}
        
        
//----------------------------------------------------------------------------//
m_value_type Loudness::CriticalBand::getBandGamma(m_value_type maxAmp)
{
    m_value_type bandGamma = 0.0;
    
    for(int p = 0; p < (int)partials_.size(); p++)
    {
        bandGamma += pow(
            (double)(partials_[p].amp_ / maxAmp),
            (double)(1.0 / log10(2.0) / BANDS[ID_][SLOPE]) );
    }

    bandGamma = pow(
        (double)bandGamma,
        (double)(log10(2.0) / BANDS[ID_][SLOPE]) );

    return bandGamma;
}
        
  
//----------------------------------------------------------------------------//
//		PARTIAL SNAPSHOT CLASS
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Loudness::PartialSnapshot::PartialSnapshot(int ID, m_value_type freq, m_value_type amp)
    :ID_(ID), freq_(freq), amp_(amp) {}
        
//----------------------------------------------------------------------------//
m_value_type Loudness::PartialSnapshot::getScalingFactor(
    int bandID, m_value_type maxAmp, m_value_type numerator)
{
    // [eq 2.37]
    m_value_type Ls = (amp_ / maxAmp) * numerator;
    
    // [eq 2.4]
    m_value_type Lp = log(Ls) / log(2.0) * 10.0 + 40.0;
    
    // [eq 2.13]
    m_value_type L = BANDS[bandID][OFFSET] + ( BANDS[bandID][SLOPE] * Lp );

    // [eq 2.5]
    m_value_type A = pow(10.0, (-1.0 * ( (120.0 - L) / 20.0 ) ) );


    return (A / amp_);
}
    
  
  
  
  
//----------------------------------------------------------------------------//
// DECLARE THE CRITICAL BAND DATA:
//----------------------------------------------------------------------------//
//Definition for the fixed critical bands
//Column 1:   Lower bounds of band
//Column 2:   Upper bounds of band
//Column 3:   Centeriod (in Hz) of frequency band
//Column 4:   Bandwidth (in Hz) of frequency band
//Column 5:   F factor of frequency band
//Column 6:   Offset $-a/b$ of frequency band, $L = (-a/b) + (1/b) L_p$
//Column 7:   Slope $1/b$ of frequency band, $L = (-a/b) + (1/b) L_p$
//----------------------------------------------------------------------------//
const double Loudness::BANDS[24][7] = 
  {
    {2.000000E+01,1.000000E+02,5.000000E+01,8.000000E+01,7.500099E-02,3.434098E+01,7.426288E-01},
    {1.000000E+02,2.000000E+02,1.500000E+02,1.000000E+02,3.000000E-01,1.102623E+01,8.863131E-01},
    {2.000000E+02,3.000000E+02,2.500000E+02,1.000000E+02,1.800000E-01,3.658798E+00,9.307306E-01},
    {3.000000E+02,4.000000E+02,3.500000E+02,1.000000E+02,1.300000E-01,6.079359E-01,9.552449E-01},
    {4.000000E+02,5.100000E+02,4.500000E+02,1.100000E+02,1.113188E-01,-1.050592E+00,9.798435E-01},
    {5.100000E+02,6.300000E+02,5.700000E+02,1.200000E+02,9.825261E-02,-1.768491E+00,9.962857E-01},
    {6.300000E+02,7.700000E+02,7.000000E+02,1.400000E+02,9.388891E-02,-1.488782E+00,9.993805E-01},
    {7.700000E+02,9.200000E+02,8.400000E+02,1.500000E+02,8.463656E-02,-7.874900E-01,1.001088E+00},
    {9.200000E+02,1.080000E+03,1.000000E+03,1.600000E+02,7.749530E-02,-2.397041E-02,9.996908E-01},
    {1.080000E+03,1.270000E+03,1.170000E+03,1.900000E+02,7.818675E-02,3.330517E-01,9.926001E-01},
    {1.270000E+03,1.480000E+03,1.370000E+03,2.100000E+02,7.454711E-02,2.924882E-01,9.866766E-01},
    {1.480000E+03,1.720000E+03,1.600000E+03,2.400000E+02,7.344415E-02,-9.319628E-02,9.820759E-01},
    {1.720000E+03,2.000000E+03,1.850000E+03,2.800000E+02,7.366146E-02,-7.555149E-01,9.725524E-01},
    {2.000000E+03,2.320000E+03,2.150000E+03,3.200000E+02,7.269603E-02,-1.368559E+00,9.559184E-01},
    {2.320000E+03,2.700000E+03,2.500000E+03,3.800000E+02,7.400790E-02,-2.310496E+00,9.405730E-01},
    {2.700000E+03,3.150000E+03,2.900000E+03,4.500000E+02,7.500003E-02,-4.106658E+00,9.374723E-01},
    {3.150000E+03,3.700000E+03,3.400000E+03,5.500000E+02,7.773245E-02,-5.443059E+00,9.383848E-01},
    {3.700000E+03,4.400000E+03,4.000000E+03,7.000000E+02,8.272464E-02,-5.612481E+00,9.440814E-01},
    {4.400000E+03,5.300000E+03,4.800000E+03,9.000000E+02,8.793907E-02,-4.374397E+00,9.618959E-01},
    {5.300000E+03,6.400000E+03,5.800000E+03,1.100000E+03,8.895339E-02,1.201688E-01,9.705871E-01},
     {6.400000E+03,7.700000E+03,7.000000E+03,1.300000E+03,8.745852E-02,6.446360E+00,9.652756E-01},
    {7.700000E+03,9.500000E+03,8.500000E+03,1.800000E+03,9.774416E-02,1.050712E+01,9.499248E-01},
    {9.500000E+03,1.200000E+04,1.050000E+04,2.500000E+03,1.074515E-01,8.187814E+00,9.339424E-01},
    {1.200000E+04,1.550000E+04,1.350000E+04,3.500000E+03,1.167188E-01,-1.822547E+00,9.533570E-01},
  };

  
  
  
//----------------------------------------------------------------------------//
#endif //__LOUDNESS_H
