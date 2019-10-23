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
//	MultiPan.cpp
//
//----------------------------------------------------------------------------//
#ifndef __MULTI_PAN_CPP
#define __MULTI_PAN_CPP

#include "MultiPan.h"

#include "Envelope.h"
#include "MultiTrack.h"
#include "Spatializer.h"
#include "Track.h"
#include "Types.h"

//----------------------------------------------------------------------------//

/** Simple constructor:
 *
 * If you use this constructor, you must call addEntry or
 * addEntryLocation one or more times to set the spatialization
 * data.  Use the other constructor if you want to instead
 * pass in a list of Interpolators (one per channel).
 *
 * @param nChans the number of channels (speakers) to pan
 *  across
 **/
MultiPan::MultiPan(int nChans) {
    int i;

    n_channels = nChans;

    for (i = 0; i < n_channels; i++) {
        EnvList.push_back(NULL);
        xyCollectionsList.push_back(new Collection<xy_point>());
        segCollectionsList.push_back(new Collection<envelope_segment>());
    }
    useEnvDirectly = false;
}

/** Advanced constructor:
 *
 * If you use this constructor, you pass in a list of
 * env's (one per channel).  If you want to call
 * addEntry or addEntryLocation to set the spatialization
 * data, use the other constructor.
 *
 * @param nChans the number of channels (speakers) to pan
 *  across
 * @param List a vector of env's (nChan number of
 *  env's, to be precise).  Each env's will
 *  control the percent that the corresponding speaker will
 *  respond to input sound
 **/
MultiPan::MultiPan(int nChans, vector<Envelope *> &List) {
    int i;

    n_channels = nChans;
    for (i = 0; i < n_channels; i++) EnvList.push_back((List[i])->clone());
    useEnvDirectly = true;
}

/** Empty Constructor
 *  This constructor is only called by MultiPan::clone()
 */
MultiPan::MultiPan() {}

/**
 * Destructor
 **/
MultiPan::~MultiPan() {
    while (!EnvList.empty()) {
        Envelope *l;

        l = EnvList.back();
        EnvList.pop_back();
        delete l;
    }

    while (!xyCollectionsList.empty()) {
        Collection<xy_point> *c;

        c = xyCollectionsList.back();
        xyCollectionsList.pop_back();
        delete c;
    }

    while (!segCollectionsList.empty()) {
        Collection<envelope_segment> *c;

        c = segCollectionsList.back();
        segCollectionsList.pop_back();
        delete c;
    }
}

/** Returns an exact duplicate of this MultiPan object.
 *
 * @return An exact copy of this MultiPan object, with its
 *  own interpolators
 **/
MultiPan *MultiPan::clone() {
    // FIXME: THIS IS BROKEN!!!!!! Fixed by Ming-ching. May 06, 2013
    // return new MultiPan(n_channels, EnvList);
    // new code is below with the added empty constructor;

    MultiPan *copy = new MultiPan();
    copy->useEnvDirectly = useEnvDirectly;
    copy->n_channels = n_channels;

    // if multipan
    if (useEnvDirectly) {
        for (int i = 0; i < n_channels; i++) {
            copy->EnvList.push_back((EnvList[i])->clone());
        }
    }

    // polar
    else {
        for (int i = 0; i < n_channels; i++) {
            copy->EnvList.push_back(EnvList[i]->clone());

            // This part below is not really needed because when "clone()" is
            // called, the envelopes are already constructed, and the
            // collections are not needed anymore.

            /*
                  Collection<xy_point>* xyCollection = new
               Collection<xy_point>();

                  for (int j = 0; j < xyCollectionsList[i]->size(); j ++){
                    xyCollection->add(xyCollectionsList[i]->get(j));
                  }
                  copy->xyCollectionsList.push_back(xyCollection);


                  Collection<envelope_segment>* segmentCollection = new
               Collection <envelope_segment>();

                  for (int j = 0; j < segCollectionsList[i]->size(); j ++){
                    segmentCollection->add(segCollectionsList[i]->get(j));
                  }
                  copy->segCollectionsList.push_back(segmentCollection);
      */
        }
    }

    return copy;
}

/**
 * Print some information for debugging purpose
 **/
void MultiPan::print() {
    cout << "MultiPan:" << endl;
    cout << "EnvList size = " << EnvList.size() << endl;
    cout << "xyCollectionsList size = " << xyCollectionsList.size() << endl;
    cout << "segCollectionsList size = " << segCollectionsList.size() << endl;
}

/** Add another spatialization point (using percents)
 *
 * Add another point to the dynamic variables (using
 * the parameters as ratios of speaker volumes).  Pass in the
 * time value, t, and a list of numbers (0.0 to 1.0 for each)
 * that gives each speakers resposne (as a percentage) to a
 * given sound.
 *
 * @param t the time stamp (as a percent of total sound
 *  length, from 0.0 to 1.0)
 **/
void MultiPan::addEntry(float t, ...) {
    va_list marker;
    int i;
    float y_i;

    if (useEnvDirectly == true) {
        cout << "Warning: called MultiPan::addEntry on a MultiPan object" << endl;
        cout << " that is using ENVs directly" << endl;
    }

    // now actually set the entries
    va_start(marker, t);
    for (i = 0; i < n_channels; i++) {
        y_i = va_arg(marker, double);
        addEntryHelperFn(i, t, y_i);
    }
    va_end(marker);
}

/** Add another spatialization point (using location)
 *
 * Add another point to the dynamic variables (using
 * the parameters as a location within a circular
 * array of speakers.  theta=0 means straight
 * ahead, positive thetas to the left, negatives to
 * the right.  radius ranges from 0 to 1.)
 *
 * @param t the time stamp (as a percent of total sound
 *  length, from 0.0 to 1.0)
 * @param theta the rotation angle (in radians) where the
 *  sound will appear to originate from.  An angle of zero
 *  is directly in front of you.  Positive angles rotate to
 *  your left, with positive pi (3.14...) being directly
 *  behind you.  Negative angles rotate to your left and
 *  negative pi (-3.14) is another name for the angle directly
 *  behind you.  For reference, pi/2 is straight left and
 *  -pi/2 is straight right.  To use pi, math.h defines
 *  the constant for you as 'M_PI'.  For instance, you could
 *  use 'M_PI/2' to refer to the angle at your left.
 * @param radius the distance from which the sound will appear
 *  to originate from.  These should range from 0.0 to 1.0
 **/
void MultiPan::addEntryLocation(float t, float theta, float radius) {
    typedef struct {
        float x, y, dist;
    } Speaker;
    Speaker **SpeakerList;
    Speaker *curSpeaker;
    float curTheta, curX, curY, total_dist;
    int i;

    if (useEnvDirectly == true) {
        cout << "Warning: called MultiPan::addEntry on a MultiPan object" << endl;
        cout << " that is using ENVs directly" << endl;
    }

    // create an array of speakers
    SpeakerList = new Speaker *[n_channels];
    // cout << "Speaker Locations:" << endl;;
    for (i = 0; i < n_channels; i++) {
        curSpeaker = new Speaker();

        curTheta = 2.0 * M_PI * (double)i / (double)n_channels;
        curSpeaker->x = cos(curTheta);
        curSpeaker->y = sin(curTheta);
        // cout << "\tx = " << curSpeaker->x;
        // cout << ", y=" << curSpeaker->y << endl;
        SpeakerList[i] = curSpeaker;
    }

// define a squared function, but just make it a macro, and be
//  reasonably sure its name doesn't conflict with anything else
#define JBL_SQRD(x) ((x) * (x))

    // figure out how far all the speakers are from the sound
    curX = radius * cos(theta);
    curY = radius * sin(theta);
    // cout << "radius=" << radius << " curX=" << curX << " curY=" << curY <<
    // endl;
    total_dist = 0;
    for (i = 0; i < n_channels; i++) {
        SpeakerList[i]->dist =
            1.0 /
            (1.0 * (JBL_SQRD(curX - SpeakerList[i]->x) + JBL_SQRD(curY - SpeakerList[i]->y)) + 0.5);
        total_dist += SpeakerList[i]->dist;
    }

    // now set the interpolator entries
    // cout << "Speaker Relative Amplitudes:" << endl;
    for (i = 0; i < n_channels; i++) {
        addEntryHelperFn(i, t, SpeakerList[i]->dist / total_dist);
        // cout << "\t" << SpeakerList[i]->dist/total_dist << endl;
        // cout << "dist=" << SpeakerList[i]->dist <<
        //" total_dist=" << total_dist << endl;
    }

    // kill the speakers

    for (i = 0; i < n_channels; i++) delete SpeakerList[i];
    delete[] SpeakerList;
    // the line below is the old code. it's wrong -- Ming-ching
    // delete SpeakerList;
}

/** Spatialize a track
 *
 * Will return a new MultiTrack object with numTracks.
 * the track will be panned accross the channels
 * by the passed in Dynamic Variable
 *
 * @param t a track to spatialize
 * @param numTracks the number of tracks to spatialize to. (This
 *  is redundant, but is kept for similarity of interface with
 *  the regular pan object.  MultiPan already knows because you
 *  have to pass in nChans in the constructor)
 * @return the track, spatialized to 'numTracks' number of tracks
 **/
MultiTrack *MultiPan::spatialize(Track &t, int numTracks) {
    m_sample_count_type sampleCount = t.getWave().getSampleCount();
    m_rate_type samplingRate = t.getWave().getSamplingRate();

    // create an empty multi track object
    MultiTrack *mt = new MultiTrack(numTracks, sampleCount, samplingRate);

    // cycle through interpolators and tell them duration and sampleCount
    for (int i = 0; i < n_channels; i++) {
        (EnvList[i])->setSamplingRate(samplingRate);
        (EnvList[i])->setDuration((float)sampleCount / (float)samplingRate);
    }

    // get references to the input
    SoundSample &inWave = t.getWave();
    SoundSample &inAmp = t.getAmp();

    // for each channel
    for (int c = 0; c < n_channels; c++) {
        // get an iterator for the current interpolator
        Iterator<m_value_type> iter = (EnvList[c])->valueIterator();

        // get references for this channel
        SoundSample &thisWave = mt->get(c)->getWave();
        SoundSample &thisAmp = mt->get(c)->getAmp();

        // Iterate over each sample
        m_value_type scale;
        for (m_sample_count_type i = 0; i < sampleCount; i++) {
            // calculate scaling factor -- FIXME, I don't
            //  understand what Pan.cpp does here, what is
            //  'pos' for?
            scale = iter.next();

            thisWave[i] = scale * inWave[i];
            thisAmp[i] = scale * inAmp[i];
        }
    }

    // Return the multitrack object
    return mt;
}

void MultiPan::addEntryHelperFn(int envIdx, float t, float amp) {
    xy_point xy;
    envelope_segment seg;

    xy.x = t;
    xy.y = amp;
    (xyCollectionsList[envIdx])->add(xy);

    if ((xyCollectionsList[envIdx])->size() > 1) {
        seg.interType = LINEAR;
        seg.lengthType = FLEXIBLE;  // FIXED; -- created lengths of envs too
                                    // long -- changed 8/09 rcavis
        seg.timeValue = 1.0;
        (segCollectionsList[envIdx])->add(seg);
    }
    /*
          Collection<xy_point> c1 =
       Collection<xy_point>(*xyCollectionsList[envIdx]);
          Collection<envelope_segment> c2 =
       Collection<envelope_segment>(*segCollectionsList[envIdx]);
          //cout<<"this new is excuted"<<endl;
          if (EnvList[envIdx]!=NULL) delete EnvList[envIdx];
          (EnvList[envIdx]) = new Envelope(*xyCollectionsList[envIdx],
                                                          *segCollectionsList[envIdx]);
          //cout << "CREATED ENV" << endl;
          //EnvList[envIdx]->print();
          //cout << "    END CREATE" << endl;
          */
}

/**
 * This funcioned should be called after all the location entries are added,
 * in other word, it's called right after the caller finishes calling
 * MultiPan::addEntryLocation.
 *
 * This function constructs the actual envelopes needed for spatialization
 * according to the location entries added.
 **/
void MultiPan::doneAddEntryLocation() {
    for (int i = 0; i < n_channels; i++) {
        // Collection<xy_point> c1 =
        // Collection<xy_point>(*xyCollectionsList[envIdx]);
        // Collection<envelope_segment> c2 =
        // Collection<envelope_segment>(*segCollectionsList[envIdx]); cout<<"this
        // new is excuted"<<endl;
        if (EnvList[i] != NULL) delete EnvList[i];
        (EnvList[i]) = new Envelope(*xyCollectionsList[i], *segCollectionsList[i]);
        // cout << "CREATED ENV" << endl;
        // EnvList[envIdx]->print();
        // cout << "    END CREATE" << endl;
    }
}

void MultiPan::xml_print(ofstream &xmlOutput) {
    xmlOutput << "\t\t<multipan>" << endl;

    xmlOutput << "\t\t</multipan>" << endl;
}

#endif  //__MULTI_PAN_CPP
