#include "LASS.h"

/**
 *    sampleexp1.cpp
 *    This sample file demonstrates curvature of an exponential curve in a ENV.
 **/

int main(int argc, const char* argv[]) {
    Sound s1(2, 440);

    // keep this > 2.0
    m_time_type timePlaying = 6.0;

    // make a ENV...
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 4.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 5.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 6.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);

    // the ENV will have an exponential attack
    seg.interType = EXPONENTIAL;
    seg.lengthType = FIXED;
    seg.timeValue = 4;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FIXED;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FIXED;
    seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope env(xyPointCollection, segmentCollection);

    // set up the sounds
    s1.setPartialParam(WAVE_SHAPE, env);
    s1.setParam(DURATION, timePlaying);
    s1.setParam(LOUDNESS, 250);

    // put the sounds in the score
    Score scor;
    s1.setParam(START_TIME, 0.0);
    scor.add(s1);

    // render in two channels, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "sampleexp1.au");

    delete renderedScore;
}
