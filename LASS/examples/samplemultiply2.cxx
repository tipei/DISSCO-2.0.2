#include "LASS.h"

/**
 *   samplemultiply2.cpp
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

    // I made this variable so that if I want to change the duration,
    // I don't need to change it in all the getValue calls
    float dur = 5.0;

    // This loads an envelope library, then multiplies two envelopes chosen
    // together.

    EnvelopeLibrary envlib;

    envlib.loadLibrary("testlib");

    Envelope* env1;
    Envelope* env2;

    env1 = envlib.getEnvelope(27);
    env2 = envlib.getEnvelope(34);

    (*env1).Print();
    (*env2).Print();

    Envelope* env = env2->multiply(*env1, *env2);

    envlib.addEnvelope(env);

    envlib.saveLibrary("testlib");

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
    scor.xml_print("samplemultiply2.xml");

    // render in two channels, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "samplemult.au");

    delete renderedScore;
}
