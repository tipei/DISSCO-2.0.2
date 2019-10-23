#include "LASS.h"

/**
 *   samplemultiply.cpp
 *
 *   This sample files demonstrates multiplying two envelopes.
 *
 *   There are also some point interpolation tests made.
 *
 *   Written by Greg Augustine 9/04
 *
 *   Copied from sampleenvelope1.cpp by Michael Aikins (maikins@uiuc.edu)
 *
 **/

int main(int argc, const char* argv[]) {
    Sound s1(2, 100);

    // Make a collection
    Collection<envelope_segment> segmentCollection, segmentCollection2;
    envelope_segment seg, seg2;

    // The initial segment info isn't important, kind of like a place holder
    // for the beginning of the Envelope
    seg.x = 0.0;
    seg.y = 0.0;
    seg.interType = EXPONENTIAL;
    seg.lengthType = FIXED;
    segmentCollection.add(seg);

    seg.lengthType = FLEXIBLE;
    seg.x = 0.5;
    seg.y = 0.8;
    segmentCollection.add(seg);

    seg.lengthType = FIXED;
    seg.interType = LINEAR;
    seg.x = 0.8;
    seg.y = 0.8;
    segmentCollection.add(seg);

    seg.x = 1.1;
    seg.y = 0.20;
    segmentCollection.add(seg);

    seg.x = 1.2;
    seg.y = 0.40;
    segmentCollection.add(seg);

    seg.x = 1.7;
    seg.y = 0.30;
    segmentCollection.add(seg);

    seg.x = 2.0;
    seg.y = 0.8;
    segmentCollection.add(seg);

    seg.x = 2.2;
    seg.y = 0.90;
    segmentCollection.add(seg);

    // I made this variable so that if I want to change the duration,
    // I don't need to change it in all the getValue calls
    float dur = 5.0;

    // The constructor call to the envelope class. This makes an envelope
    // using the information in segmentCollection
    Envelope env1(segmentCollection);

    seg2.x = 0.0;
    seg.y = 0.0;
    seg2.interType = EXPONENTIAL;
    seg2.lengthType = FIXED;
    segmentCollection2.add(seg2);

    seg2.lengthType = FLEXIBLE;
    seg2.x = 0.5;
    seg2.y = 0.8;
    segmentCollection2.add(seg2);

    seg2.lengthType = FIXED;
    seg2.interType = LINEAR;
    seg2.x = 0.8;
    seg2.y = 0.7;
    segmentCollection2.add(seg2);

    seg2.x = 1.0;
    seg2.y = 0.50;
    segmentCollection2.add(seg2);

    seg2.x = 1.2;
    seg2.y = 0.30;
    segmentCollection2.add(seg2);

    seg2.x = 1.87;
    seg2.y = 0.70;
    segmentCollection2.add(seg2);

    seg2.x = 2.10;
    seg2.y = 0.45;
    segmentCollection2.add(seg2);

    seg2.x = 2.2;
    seg2.y = 1.0;
    segmentCollection2.add(seg2);

    Envelope env2(segmentCollection2);

    Envelope* env = env2.multiply(env1, env2);

    (env1).Print();
    (env2).Print();
    (*env).Print();

    Envelope envcheck = *env;

    // this tests approximation at various points using the getValue function
    // the first value being the time in question, the second being the
    // total length of the envelope
    // note: 1.3 does not correspond to a point
    cout << "- Envelope getValue calls" << endl;
    cout << "    value 1: " << envcheck.getValue(0.2, dur) << endl;
    cout << "    value 2: " << envcheck.getValue(1.0, dur) << endl;
    cout << "    value 3: " << envcheck.getValue(1.3, dur) << endl;
    cout << "- Envelope getValue calls END" << endl;

    // This sets the wave shape of the sound to the envelope we made
    s1.setPartialParam(WAVE_SHAPE, envcheck);

    // Set some other parameters
    s1.setParam(DURATION, dur);
    s1.setParam(LOUDNESS, 150);

    Score scor;
    s1.setParam(START_TIME, 0.0);

    // Add the sound to a score so we can render it
    scor.add(s1);

    // write out xml, just makes it easier to see how things are stored
    scor.xml_print("samplemultiply.xml");

    // render in two channels, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "samplemult.au");

    delete renderedScore;
}
