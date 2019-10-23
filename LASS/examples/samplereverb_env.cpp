#include "LASS.h"

/**
 *    samplereverb2.cpp
 *
 *    This file demonstrates using the reverb on just one sound, not the entire
 *    score.
 **/

int main(int argc, const char *argv[]) {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    Sound *scale[24];
    int i;
    float freq[] = {261.669, 266.256, 293.690, 311.141, 329.614, 349.335, 370.120, 391.836,
                    415.135, 440.114, 465.925, 493.890, 523.042, 554.512, 587.006, 621.862,
                    659.228, 698.182, 739.647, 783.674, 830.270, 879.389, 932.793, 986.724};

    // create an amplitude envelope:
    xy.x = 0.00;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.10;
    xy.y = 1;
    xyPointCollection.add(xy);
    xy.x = 0.12;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.95;
    xy.y = 1;
    xyPointCollection.add(xy);
    xy.x = 0.96;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 1;
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
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

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
    seg.lengthType = FIXED;
    seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope pan_left_to_right(xyPointCollection, segmentCollection);
    // Pan panL2R (pan_left_to_right);

    // create the sounds
    for (i = 0; i < 24; i++) {
        scale[i] = new Sound(6, freq[i]);

        // adjust the partial balance:
        scale[i]->get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
        scale[i]->get(1).setParam(RELATIVE_AMPLITUDE, 0.50);
        scale[i]->get(2).setParam(RELATIVE_AMPLITUDE, 0.20);
        scale[i]->get(3).setParam(RELATIVE_AMPLITUDE, 0.15);
        scale[i]->get(4).setParam(RELATIVE_AMPLITUDE, 0.10);
        scale[i]->get(5).setParam(RELATIVE_AMPLITUDE, 0.06);

        scale[i]->setPartialParam(WAVE_SHAPE, shape);
        // scale[i]->setSpatializer(panL2R);

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
    Collection<envelope_segment> revsegmentCollection;

    envelope_segment revseg;
    revseg.x = 0.0;
    revseg.y = 0.0;
    revsegmentCollection.add(revseg);
    revseg.x = 1.0;
    revseg.y = 1.0;
    revsegmentCollection.add(revseg);
    revseg.interType = LINEAR;
    revseg.lengthType = FLEXIBLE;
    revsegmentCollection.add(revseg);

    Envelope *percentReverb = new Envelope(pan_left_to_right);
    float hilow_spread = 0.05;
    float all_pass_gain = 0.70;
    float reverb_delay = 0.60;
    Reverb *reverbObj = new Reverb(percentReverb, hilow_spread, all_pass_gain, reverb_delay, 44100);

    // Orchestrate a song
    scale[7]->setParam(START_TIME, 0.0);  // this sound has reverb
    scale[7]->setParam(DURATION, 15.0);
    scale[7]->setPartialParam(VIBRATO_AMP, .2);
    scale[7]->setPartialParam(VIBRATO_RATE, 8);
    scale[7]->use_reverb(reverbObj);
    scor.add(*scale[7]);

    scale[2]->setParam(START_TIME, 17.0);  // this sound does not
    scale[2]->setParam(DURATION, 5.0);
    scor.add(*scale[2]);

    // render in one channel, 44100 Hz
    MultiTrack *renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "samplereverb_env.au");

    delete renderedScore;
    delete reverbObj;
    for (i = 0; i < 24; i++) delete scale[i];
    delete percentReverb;
}
