#include "LASS.h"

/**
 *    sampleFreqEnv.cpp
 *
 *    This file demonstrates how to modify the frequency of a sound's partials
 *	by assigning different envelopes to each partial individually.
 *    Includes use of FREQUENCY, and
 *      DETUNING_ENV.
 **/

int main(int argc, const char* argv[]) {
    int choice;

    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(4, 440);

    // adjust the partial balance:
    s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
    s.get(1).setParam(RELATIVE_AMPLITUDE, 0.50);
    s.get(2).setParam(RELATIVE_AMPLITUDE, 0.30);
    s.get(3).setParam(RELATIVE_AMPLITUDE, 0.20);

    // create envelopes:
    xy.x = 0.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 1.1;
    xyPointCollection.add(xy);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1.0;
    segmentCollection.add(seg);
    Envelope freq0(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    xy.x = 0.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 0.6;
    xyPointCollection.add(xy);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1.0;
    segmentCollection.add(seg);
    Envelope freq1(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    xy.x = 0.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 1.5;
    xyPointCollection.add(xy);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1.0;
    segmentCollection.add(seg);
    Envelope freq2(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    xy.x = 0.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 0.9;
    xyPointCollection.add(xy);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1.0;
    segmentCollection.add(seg);
    Envelope freq3(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    // choose which method you want to use
    cout << "	Enter f for FREQUENCY" << endl;
    cout << "	      t for DETUNING_ENV" << endl;
    choice = cin.get();

    switch (choice) {
        case 'f':
            s.get(0).setParam(FREQUENCY, freq0);
            s.get(1).setParam(FREQUENCY, freq1);
            s.get(2).setParam(FREQUENCY, freq2);
            s.get(3).setParam(FREQUENCY, freq3);
            break;

        case 't':
            s.get(0).setParam(DETUNING_ENV, freq0);
            s.get(1).setParam(DETUNING_ENV, freq1);
            s.get(2).setParam(DETUNING_ENV, freq2);
            s.get(3).setParam(DETUNING_ENV, freq3);
            break;
    }

    // create an amplitude envelope:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.1;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 0.4;
    xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 0.1;
    segmentCollection.add(seg);
    seg.interType = EXPONENTIAL;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 0.3;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 0.6;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    s.setPartialParam(WAVE_SHAPE, shape);

    s.setParam(DURATION, 8);

    // create a pan envelope:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope pan_left_to_right(xyPointCollection, segmentCollection);
    Pan panL2R(pan_left_to_right);

    // pan the sound around:
    s.setSpatializer(panL2R);

    // set the loudness of the sound:
    s.setParam(LOUDNESS, 200);

    // score:
    Score scor;

    // Create our render object
    float room_size = 0.02;
    Reverb* reverbObj = new Reverb(room_size, 44100);
    scor.use_reverb(reverbObj);

    s.setParam(START_TIME, 0.0);
    scor.add(s);

    // render in one channel, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "sampleFreqEnv.au");

    delete renderedScore;
    delete reverbObj;
}
