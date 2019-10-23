#include "LASS.h"

/**
 *    samplenoise1.cpp
 *
 *    This sample file demonstrates a random noise.
 **/

int main(int argc, const char* argv[]) {
    // create a sound
    Sound s(4, 440);

    // make a DVS for the sound envelope
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create an amplitude envelope:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.2;
    xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 0.4;
    xy.y = 0.8;
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

    Envelope amp_envelope(xyPointCollection, segmentCollection);
    s.setPartialParam(WAVE_SHAPE, amp_envelope);
    s.setParam(DURATION, 5);
    s.setParam(LOUDNESS, 250);

    // set it to use a random sample value
    s.setPartialParam(WAVE_TYPE, 1);

    // make a score
    Score scor;
    s.setParam(START_TIME, 0.0);
    scor.add(s);

    // render and write out
    MultiTrack* renderedScore = scor.render(2, 44100);
    AuWriter::write(*renderedScore, "samplenoise1.au");

    // clean up
    delete renderedScore;
}
