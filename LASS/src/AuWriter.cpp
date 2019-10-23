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
//	AuWriter.cpp
//
//----------------------------------------------------------------------------//

#ifndef __AU_WRITER_CPP
#define __AU_WRITER_CPP

//----------------------------------------------------------------------------//

#include "AuWriter.h"

#include "sndfile.h"

//----------------------------------------------------------------------------//
bool AuWriter::write(SoundSample& ss, string filename, int bits) {
    vector<SoundSample*> channels;
    channels.push_back(&ss);
    return write(channels, filename, bits);
}

//----------------------------------------------------------------------------//
bool AuWriter::write(Track& t, string filename, int bits) {
    vector<SoundSample*> channels;
    channels.push_back(&t.getWave());
    return write(channels, filename, bits);
}

//----------------------------------------------------------------------------//
bool AuWriter::write(MultiTrack& mt, string filename, int bits) {
    vector<SoundSample*> channels;

    Iterator<Track*> it = mt.iterator();
    while (it.hasNext()) {
        channels.push_back(&it.next()->getWave());
    }

    return write(channels, filename, bits);
}

//----------------------------------------------------------------------------//
bool AuWriter::write_one_per_track(MultiTrack& mt, char* filename, ...) {
    vector<SoundSample*> channels;
    va_list marker;

    bool ret_val = true;
    char* cur_filename = filename;
    Iterator<Track*> it = mt.iterator();

    va_start(marker, filename);
    while (it.hasNext()) {
        channels.push_back(&it.next()->getWave());
        ret_val = ret_val & write(channels, string(cur_filename));
        channels.pop_back();
        cur_filename = va_arg(marker, char*);
    }
    va_end(marker);

    return ret_val;
}

//----------------------------------------------------------------------------//
bool AuWriter::write(vector<SoundSample*>& channels, string filename, int bits) {
    // ensure that all of the SoundSamples, have the same samplingRate.
    // Also, find the lowest sampleCount value of all the SoundSamples.
    // warn if sections of other SoundSamples are going to be clipped
    // because of this.

    // Clear out the info structure.
    SF_INFO s_info;
    memset(&s_info, 0, sizeof(SF_INFO));

    // Ensure we actually have channels of sound to work with.
    if (channels.size() == 0) {
        cout << "ERROR: AuWriter: numChannels = 0" << endl;
        return false;
    }

    // Set the info parameters.
    s_info.channels = channels.size();
    s_info.samplerate = channels[0]->getSamplingRate();
    s_info.format = SF_FORMAT_PCM_24;
    bits = 0;  // Do not use the incoming format, 24-bit is all-purpose.

    // Determine what format to use based on the filename extension.
    if (filename.find(".wav") != string::npos) {
        s_info.format = s_info.format | SF_FORMAT_WAV;
    } else if (filename.find(".aiff") != string::npos) {
        s_info.format = s_info.format | SF_FORMAT_AIFF;
    } else if (filename.find(".au") != string::npos) {
        s_info.format = s_info.format | SF_FORMAT_AU;
    } else {
        cout << "ERROR: The output file '" << filename
             << "' does not have an extension that is recognizable. Try"
                " using .wav, .aiff, or .au."
             << endl;
        return false;
    }

    // Open the sound file.
    SNDFILE* s = sf_open(filename.c_str(), SFM_WRITE, &s_info);
    if (!s) {
        cout << "ERROR: The output file '" << filename << "' could not be opened by libsndfile."
             << endl;
        return false;
    }

    // Determine max number of samples in the channels.
    m_sample_count_type minSamples = channels[0]->getSampleCount();
    m_sample_count_type maxSamples = minSamples;
    for (int c = 0; c < (int)channels.size(); c++) {
        // Make sure every channel has the same sampling rate.
        if (channels[c]->getSamplingRate() != channels[0]->getSamplingRate()) {
            cerr << "ERROR: AuWriter: not all channels "
                 << "have the same sampling rate" << endl;
            return false;
        }

        // Check the number of samples.
        m_sample_count_type count = channels[c]->getSampleCount();
        if (count < minSamples) minSamples = count;
        if (count > maxSamples) maxSamples = count;
    }

    // Warn about different amounts of samples.
    if (maxSamples > minSamples) {
        m_time_type secondsClipped = ((m_time_type)(maxSamples - minSamples)) /
                                     ((m_time_type)channels[0]->getSamplingRate());

        cerr << "WARNING: AuWriter: "
             << "Because not all SoundSamples were of the same length, " << endl
             << secondsClipped << " seconds will be clipped off of the "
             << "end of one or more channels." << endl;
    }

    // Create multi-channel chunk.
    int chunkFrames = 1024 * 16;
    float* chunk = new float[chunkFrames * channels.size()];

    int outOfBounds = 0;

    for (m_sample_count_type currentIn = 0; currentIn < minSamples; currentIn += chunkFrames) {
        m_sample_count_type framesToWrite = chunkFrames;
        if (framesToWrite > minSamples - currentIn) framesToWrite = minSamples - currentIn;
        for (int c = 0; c < (int)channels.size(); c++) {
            for (m_sample_count_type i = currentIn; i < currentIn + framesToWrite; i++) {
                m_sample_type sample = (*channels[c])[i];

                // Check bounds.
                if (sample > 1.0) {
                    sample = 1.0;
                    outOfBounds++;
                }
                if (sample < -1.0) {
                    sample = -1.0;
                    outOfBounds++;
                }

                chunk[(i - currentIn) * channels.size() + c] = sample;
            }
        }
        sf_writef_float(s, chunk, (sf_count_t)framesToWrite);
    }
    sf_close(s);

    delete[] chunk;  // added by ming-ching to prevent memory leak. Dec.10 2012
    return true;
}

//----------------------------------------------------------------------------//
void AuWriter::WriteIntMsb(ostream& out, long l, int size) {
    if (size <= 0) return;
    WriteIntMsb(out, l >> 8, size - 1);  // Write MS Bytes
    out.put(l & 255);                    // Write LS Byte
}

//----------------------------------------------------------------------------//
#endif  //__AU_WRITER_CPP
