/*
 *    samplepan1.cpp
 *    Sample file for testing panning.
 *
 *    Written by:    Jon Kishkunas
 *                   2003/04/12
 *    Modified for DVS by: Philipp Fraund
 *                   2003/09/15
 */

#include "LASS.h"

int main(int argc, const char* argv[]) {
    // create a sound
    Sound s(4, 440);

    // not gonna adjust partial balance for now, I just want to test panning
    /*
        // adjust the partial balance
        s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
        s.get(1).setParam(RELATIVE_AMPLITUDE, 0.30);
        s.get(2).setParam(RELATIVE_AMPLITUDE, 0.10);
        s.get(3).setParam(RELATIVE_AMPLITUDE, 0.05);
    */

    // make a DVS for the sound envelope
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create an amplitude envelope:
    // give the sound a shape that is evenly loud when mirrored across the
    // middle, since we will start panning at one side and end at the other
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.2;
    xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 0.8;
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
    seg.timeValue = 3;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope amp_envelope(xyPointCollection, segmentCollection);
    s.setPartialParam(WAVE_SHAPE, amp_envelope);
    s.setParam(DURATION, 4);
    s.setParam(LOUDNESS, 250);

    // clear the collections in order to construct a fresh envelope
    xyPointCollection.clear();
    segmentCollection.clear();

    // create a pan envelope:
    // 0.0 (left) to 1.0 (right)
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

    // make a score
    Score scor;
    s.setParam(START_TIME, 0.0);
    scor.add(s);

    // render and write out
    MultiTrack* renderedScore = scor.render(2, 44100);
    AuWriter::write(*renderedScore, "samplepan1.au");

    // clean up
    delete renderedScore;
}
