#include "LASS.h"

/**
 *    samplecubic1.cpp
 *
 *    one 3s sound with 4 partials, cubic-spline interpolation,
 *    left-to-right panning
 *    played at 0s and 2s
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

    // create an amplitude wave shape (using DVS):
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

    seg.interType = CUBIC_SPLINE;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = CUBIC_SPLINE;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = CUBIC_SPLINE;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 3;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

    s.setPartialParam(WAVE_SHAPE, shape);

    s.setParam(DURATION, 3);

    // clear the collections in order to construct a fresh envelope:
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

    // pan the sound around:
    s.setSpatializer(panL2R);

    // set the loudness of the sound:
    s.setParam(LOUDNESS, 200);

    // score:
    Score scor;

    s.setParam(START_TIME, 0.0);
    scor.add(s);

    s.setParam(START_TIME, 2.0);
    scor.add(s);

    // render in one channel, 44000 Hz
    MultiTrack* renderedScore = scor.render(2, 44000);

    // write to file
    AuWriter::write(*renderedScore, "samplecubic1.au");

    delete renderedScore;
}
