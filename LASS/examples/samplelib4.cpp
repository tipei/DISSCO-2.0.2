#include "LASS.h"

/**
 *    This sample file demonstrates the use of a Envelope
 *    from an entry in the EnvelopeLibrary to a full rendered sound.
 *    Note: this program requires a valid library named "library1" (created
 *    with samplelib1.cpp)
 **/

int main(int argc, const char* argv[]) {
    EnvelopeLibrary envlib;
    int number_of_envelopes;

    number_of_envelopes = envlib.loadLibrary("library1");

    // display library entry 1
    envlib.showEnvelope(1);

    Envelope* env;

    // fetch the first envelope from the library
    env = envlib.getEnvelope(1);

    Sound s1(2, 440);

    // note, dereference Envelope * for use with Sound
    // or wherever an object is expected
    s1.setPartialParam(WAVE_SHAPE, *env);
    s1.setParam(DURATION, 2.2);
    s1.setParam(LOUDNESS, 150);

    Score scor;
    s1.setParam(START_TIME, 0.0);
    scor.add(s1);

    // render in two channels, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "samplelib4.au");

    delete renderedScore;
}
