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
//	Types.h
//
//----------------------------------------------------------------------------//

#ifndef __TYPES_H
#define __TYPES_H

//----------------------------------------------------------------------------//

/**
 * @file Types.h
 *
 * This file is included to define basic filetypes
 * for the application.  In this manner, we can easily
 * change from float-sound to double-sound by editing one line of code.
 * \author Braden Kowitz
 **/

/// Specifies a type for an individual sample value.
typedef float m_sample_type;

/// Specifies a sample count.
typedef long m_sample_count_type;

/// Specifies a type for an individual time value.
typedef float m_time_type;

/// Specifies a value type (used for frequency and amplitude).
typedef float m_value_type;

/// Specifies a rate for playback.
typedef unsigned int m_rate_type;

/**
 *  This is used in EnvelopeEntry to specify whether the entry
 *  should be played for a fixed amount of time or a percentage of total time.
 **/
enum stretch_type { FIXED, FLEXIBLE };
// enum stretch_type {STAT, DYN};

/**
 *  This is used in EnvelopeEntry to specify what kind of
 *  interpolation the entry should have.
 **/
enum interpolation_type { EXPONENTIAL, CUBIC_SPLINE, LINEAR };

/**
 *  This is used to make it easier to specify a single point for a
 *  Envelope.
 **/
struct xy_point {
    /// x value
    m_value_type x;
    /// y value
    m_value_type y;

    xy_point() {}
    xy_point(m_value_type x, m_value_type y) {
        this->x = x;
        this->y = y;
    }
};

/**
 *  This is used as a container for everything that a Envelope
 *  needs to know to create a particular segment in a shape.
 *
 *  \todo Once DVS is completely gone and done for, timeType and timeValue
 *  should be deleted!
 **/
struct envelope_segment {
    /// interpolation type to use for the segment
    interpolation_type interType;
    /// time stretch type to use for the segment
    stretch_type timeType;
    /// number of seconds or percentage of flex time that segment should play
    m_value_type timeValue;
    /// length stretch type to use for the segment
    stretch_type lengthType;
    /// length if applicable
    m_value_type length;
    /// x position of the right endpoint
    m_value_type x;
    /// y value at the right endpoint
    m_value_type y;
};

struct env_seg {
    /// interpolation type to use for the segment
    interpolation_type interType;
    /// length stretch type to use for the segment
    stretch_type lengthType;
    /// length of the segment if applicable
    m_value_type length;
    /// x position of the segment point
    m_value_type x;
    /// value at the right endpoint
    m_value_type y;
};

/// When no sampling rate is specified, uses 44.1kHz
static const m_rate_type DEFAULT_SAMPLING_RATE = 44100;

/// Loudness can be calculated at much slower rates. The default rate is 10Hz.
// static const m_rate_type DEFAULT_LOUDNESS_RATE = 10;
static const m_rate_type DEFAULT_LOUDNESS_RATE = 44100;

//----------------------------------------------------------------------------//
#endif  //__TYPES_H
