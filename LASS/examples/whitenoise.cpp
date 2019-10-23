#include "LASS.h"

/**
 *    whitenoise.cpp
 *
 *    This file demonstrates how to make white noise. It also
 *		demonstrates the use of a filter on a MultiTrack object.
 **/

int main(int argc, const char* argv[]) {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(1, 30);

    // score:
    Score scor;

    s.setParam(START_TIME, 0.0);
    // set the partial type to RANDOM!!!
    s.setPartialParam(WAVE_TYPE, 1);

    scor.add(s);

    // render in one channel, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // Let's run a filter on it!
    LowPassFilter fil(0.5);  // gain = 0.5

    MultiTrack filteredScore = fil.do_filter_MultiTrack(*renderedScore);

    // write to file
    AuWriter::write(filteredScore, "whitenoise.au");
    AuWriter::write(*renderedScore, "whitenoise-orig.au");

    delete renderedScore;
    //    delete reverbObj;
}
