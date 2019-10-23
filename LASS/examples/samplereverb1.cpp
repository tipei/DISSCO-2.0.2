#include "LASS.h"

/**
 *    samplereverb1.cpp
 *
 *    This file demonstrates the simple method of adding reverb effects
 *    to a score
 **/

int main(int argc, const char* argv[]) {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(4, 440);

    // adjust the partial balance:
    s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
    s.get(1).setParam(RELATIVE_AMPLITUDE, 0.30);
    s.get(2).setParam(RELATIVE_AMPLITUDE, 0.10);
    s.get(3).setParam(RELATIVE_AMPLITUDE, 0.05);

    // create an amplitude wave shape (using DVS)
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.2;
    xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 0.4;
    xy.y = 0.5;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 3;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

    s.setPartialParam(WAVE_SHAPE, shape);

    s.setParam(DURATION, 1);

    // clear the collections in order to construct a fresh envelope
    xyPointCollection.clear();
    segmentCollection.clear();

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

    s.setSpatializer(panL2R);

    // set the loudness of the sound:
    s.setParam(LOUDNESS, 200);

    // score:
    Score scor;

    /*
     * Add reverb to the sound using a flat Envelope at .3
     */

    float room_size = .3;
    Reverb* reverbObj = new Reverb(room_size, 44100);
    scor.use_reverb(reverbObj);

    s.setParam(START_TIME, 0.0);
    scor.add(s);

    s.setParam(START_TIME, 3.0);
    scor.add(s);

    // output the xml!
    scor.xml_print();

    // render in one channel, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "samplereverb1.au");

    delete renderedScore;
    delete reverbObj;
}
