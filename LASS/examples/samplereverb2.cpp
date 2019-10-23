#include "LASS.h"

/**
 *    samplereverb2.cpp
 *
 *    This file demonstrates the more advanced constructor for reverb,
 *    using an envelope for reverb that starts at zero, climbs to 1 at the
 *middle and goes back to zero for the ending, playing the same sound every 2
 *seconds.
 *
 **/

int main(int argc, const char* argv[]) {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    Sound* scale[24];
    int i;
    float freq[] = {261.669, 266.256, 293.690, 311.141, 329.614, 349.335, 370.120, 391.836,
                    415.135, 440.114, 465.925, 493.890, 523.042, 554.512, 587.006, 621.862,
                    659.228, 698.182, 739.647, 783.674, 830.270, 879.389, 932.793, 986.724};

    // create an amplitude envelope:
    xy.x = 0.00;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.15;
    xy.y = 0.9;
    xyPointCollection.add(xy);
    xy.x = 0.30;
    xy.y = 0.7;
    xyPointCollection.add(xy);
    xy.x = 0.70;
    xy.y = 0.6;
    xyPointCollection.add(xy);
    xy.x = 1.00;
    xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1.5;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1.5;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 4.0;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 3.0;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    // create the reverb envelope:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = .5;
    xy.y = 1.0;
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

    //  this doesn't ever get used to pan, it's just a leftover and will become
    //  the reverb envelope below
    Envelope pan_left_to_right(xyPointCollection, segmentCollection);

    // create the sounds
    for (i = 0; i < 24; i++) {
        scale[i] = new Sound(6, freq[i]);

        // adjust the partial balance:
        scale[i]->get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
        scale[i]->get(1).setParam(RELATIVE_AMPLITUDE, 0.25);
        scale[i]->get(2).setParam(RELATIVE_AMPLITUDE, 0.13);
        scale[i]->get(3).setParam(RELATIVE_AMPLITUDE, 0.17);
        scale[i]->get(4).setParam(RELATIVE_AMPLITUDE, 0.10);
        scale[i]->get(5).setParam(RELATIVE_AMPLITUDE, 0.06);

        scale[i]->setPartialParam(WAVE_SHAPE, shape);

        // set the loudness of the sound:
        scale[i]->setParam(LOUDNESS, 200);
    }

    // score:
    Score scor;

    /*
     * This is the more advanced (and more customizable) means of
     * adding reverb to a sound.  With this constructor, you get
     * four handles:
     *
     * Reverb Percent - this specifies the ratio in the output sound
     *  of direct (unreverbed) sound to reverbed sound. 0.0 means all
     *  direct sound (no reverb will be audible in the output sound),
     *  while 1.0 means you will not hear the direct sound, only the
     *  reverb sound
     *
     * HiLow Spread - this parameter determines the difference in low-
     *  frequency response and high-frequency response.  A value of 0.0
     *  means that the reverb will have an even response across all
     *  frequencies.  A value of 1.0 means that the reverb will be much
     *  more responsive at low frequencies.  This parameter combines
     *  many physical properties of the room (including hardness of
     *  walls, degree of obstructions present in the room, volume of
     *  the room, etc) into a simplified, but more manageable factor.
     *
     * AllPassGain - this is the gain on the reverb's allpass filter.
     *  It should be between 0.0 and 1.0.  The higher the values the
     *  more ringing that occurs at certain frequencies.  Lower values
     *  cause less ringing, but too low of a value can cause an
     *  unsmooth response.  In almost all cases, a value of 0.7 will
     *  be sufficient, but you may desire more or less ringing in
     *  certain situations
     *
     * ReverbDelay - this is the delay (in units of seconds) of the
     *  early response echos.  It must be greater than 0.0, but can
     *  be as high as one desires.  Practical values probably range
     *  from around 0.05 to 1.50, but you can be the judge.
     */
    cout << "before envelope creation" << endl;
    Envelope* percentReverb = new Envelope(pan_left_to_right);
    float hilow_spread = 0.05;
    float all_pass_gain = 0.7;
    float reverb_delay = 0.6;
    cout << "before call to Reverb" << endl;
    Reverb* reverbObj = new Reverb(percentReverb, hilow_spread, all_pass_gain, reverb_delay, 44100);
    cout << "reverb.cpp at least doesn't crash" << endl;

    // Orchestrate a "song"
    scale[1]->setParam(START_TIME, 0.0);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 2);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 4.0);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 6);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 8);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 10);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 12);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 14);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 16);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 18);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 20);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 22);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scale[1]->setParam(START_TIME, 24);
    scale[1]->setParam(DURATION, 0.5);
    scor.add(*scale[1]);
    scor.use_reverb(reverbObj);

    // render in one channel, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    cout << "about to write to file" << endl;
    // write to file
    AuWriter::write(*renderedScore, "samplereverb2.au");

    delete renderedScore;
    delete reverbObj;
    for (i = 0; i < 24; i++) delete scale[i];
}
