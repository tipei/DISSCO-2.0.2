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
//	Loudness.h
//
//----------------------------------------------------------------------------//

#ifndef __LOUDNESS_H
#define __LOUDNESS_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "Types.h"
#include "Sound.h"
#include "Partial.h"

//----------------------------------------------------------------------------//

/**
*	This performs loudness calculations on sound objects:
*
*	STEPS:
*
*	-# Separate all partials into groups of critical bands.
*		- This division is done by the partial's frequency
*	-# Find the loudest partial by virtue of it's WaveShape variable.
*     		- And thus, find the highest WaveShape value.
*	-# Calculate the Band Gamma for each critical band. [eq 2.34]
*		- The max WaveShape from step 2 becomes the maxWaveShape here.
*		- bandGamma = 0
*		- for each partial 
*			- bandGamma += 
*			  pow( (thisWaveShape / maxWaveShape),
*                          (1.0 / log10(2.0) * this_band_b_value) );
*               - bandGamma = pow(bandGamma, (log10(2.0) * this_band_b_value) );
*	-# Find the maximum band-gamma: maxGamma
*	-# Calculate the numerator needed for [eq 2.37]
*		- gammaTotal = 0;
*		- for each band
*			- gammaTotal += bandGamma * this_band_f_value;
*		- numerator = desiredSonesValue / (maxGamma + gammaTotal);
*	-# Calculate scaling factor for each partial in each of the bands.
*		- (uses maxWaveShape & numerator)
*		- [eq 2.37]
*                 Ls = (thisWaveShape / maxWaveShape ) * numerator;
*		- [eq 2.4]
*		  Lp = log(ls) / log(2.0) * 10.0 + 40.0;
*		- [eq 2.13]
*		  L = (-a/b) + (1/b) * Lp
*		- [eq 2.5]
*		  A = pow( 10.0, (-1.0 * ( (120.0 - L) / 20.0 ) ) );
*		- ScalingFactor = A / thisWaveShape
*	Phew - done!
*	\author Braden Kowitz
**/
class Loudness
{    
public:

    /**
    *	This performs the Loudness calculation on a sound object.
    *	Basically, it looks at each partial's WAVE_SHPE, FREQUENCY, and 
    *	RELATIVE_AMPLITUDE parameters.  It looks at the sounds LOUDNESS 
    *	parameter, and sets the LOUDNES_SCALER DynamicVariable for each partial.
    **/
    static void calculate(Sound& snd, m_rate_type rate = DEFAULT_LOUDNESS_RATE);

private:
    
    /**
    *   This is the class that contains a snapshot of part of a partial.
    *	\author Braden Kowitz
    **/
    class PartialSnapshot
    {
    public:
	/** An identifier for the part of the partial **/
        int ID_;

	/** The frequency **/
        m_value_type freq_;

	/** The amplitude **/
        m_value_type amp_;
        
	/** This is a constructor for a PartialSnapshot
	*	\param ID Which part of the partial
	*	\param freq The frequency of the part of the partial
	*	\param amp The amplitude of the part of the partial
	**/
        PartialSnapshot(int ID, m_value_type freq, m_value_type amp);
        
	/** This returns the scalingfactor of the band.
	*	\param bandID Which band
	*	\param maxAmp The cap on the amplitude
	*	\param numerator
	*	\return The scaling factor
	**/
        m_value_type getScalingFactor(int bandID, m_value_type maxAmp, m_value_type numerator);

    };
    
    /**
    *   This is a class to hold a critical band.
    *	\author Braden Kowitz
    **/
    class CriticalBand
    {
    public:
	/** An identifier for the critical band **/
        int ID_;
	/** A vector to hold partial snapshots **/
        vector<PartialSnapshot> partials_;
        
	/** This is a constructor.
	*	\param ID Which critical band
	**/
        CriticalBand(int ID);
        
	/** This function gets the gamma for the band
	*	\param maxAmp A cap on the amplitude
	*	\return The band gamma
	**/
        m_value_type getBandGamma(m_value_type maxAmp);
        
    };
    
    
    /**
    *	This function returns the critical band index that matches the 
    *	specified frequency.
    *	\param frequency The freqency of the band to return
    *	\return The index of a CriticalBand
    **/
    static int criticalBandIndex(m_value_type frequency);

    /**
    *	These are the indices of the Critical Band Field.
    **/
    static const int LOWER_BOUND	= 0;
    static const int UPPER_BOUND	= 1;
    static const int CENTROID		= 2;
    static const int BANDWIDTH		= 3;
    static const int F_FACTOR		= 4;
    static const int OFFSET		= 5;
    static const int SLOPE		= 6;
    /**
    *	This holds the CriticalBand data.
    **/
    static const double BANDS[24][7];
    /** There are 24 critical bands **/
    static const int NUM_BANDS = 24;
};


//----------------------------------------------------------------------------//
#endif //__LOUDNESS_H
