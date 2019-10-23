#include "LASS.h"

/**
 *    sampleglis2.cpp
 *
 *    This file demonstrates how to add glissando to a sound
 **/

int main(int argc, const char* argv[]) {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(1, 12000);

    // adjust the partial balance:
    s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
    //    s.get(1).setParam(RELATIVE_AMPLITUDE, 0.30);
    //    s.get(2).setParam(RELATIVE_AMPLITUDE, 0.10);
    //    s.get(3).setParam(RELATIVE_AMPLITUDE, 0.05);

    /*
     * This is the code that adds glissando.  Essentially you
     * need to create an envelope and pass that as the
     * GLISSAND_ENV parameter on the sound.  You could also
     * perform glissando on just a partial by calling the
     * partial's setPartialParm() rather than calling the
     * sound's setPartialParam().
     *
     * Here, I have used exponential interpolation for the
     * glissando envelope, but you can use whatever type you choose.
     * each entry of the gliss envelope is multiplied against
     * the frequency (at every sample), so if your envelope
     * has a value of 1.0 at all times, you will have no
     * glissando effect.
     *
     * In this particular example I begin at 1.0 at time zero,
     * which means the frequency is untouched initially.
     * Then from time=0.6 to time=1.0, I move from freq*1.0 to
     * freq*0.5, which means the sound drops by an octave
     * during the last 40% of its duration.  The exponential
     * interpolator makes this drop off rather smooth.  The
     * effect is similar to strumming a note on a guitar,
     * then sliding one's finger down the neck as it trails
     * off.
     */

    // create a glissando envelope:
    xy.x = 0.0;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 0.01;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = .0025;  // 12000 * .0025 = 30 Hz end
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 0.01;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 0.99;
    segmentCollection.add(seg);

    Envelope gliss(xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    // create an amplitude envelope:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.1;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 0.9;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FIXED;
    seg.timeValue = 0.1;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 0.8;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FIXED;
    seg.timeValue = 0.1;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

    s.setPartialParam(WAVE_SHAPE, shape);

    s.setParam(DURATION, 18);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();
    /*
    // create a pan envelope:
    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 1.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope pan_left_to_right (xyPointCollection, segmentCollection);
    Pan panL2R (pan_left_to_right);

    // pan the sound around:
    s.setSpatializer (panL2R);
    */
    // set the loudness of the sound:
    s.setParam(LOUDNESS, 200);

    // score:
    Score scor;

    // Create our render object
    float room_size = 0.3;
    Reverb* reverbObj = new Reverb(room_size, 44100);
    scor.use_reverb(reverbObj);

    s.setParam(START_TIME, 0.0);
    scor.add(s);

    /*
    s.setParam(START_TIME, 3.0);
    scor.add(s);
    */
    /*
    Sound s2(4, 1000);
    Sound s3(4, 2000);
    Sound s4(4, 4000);

    s2.setParam(START_TIME, 15.0);
    s2.setParam(DURATION, 5);
    s2.setParam(LOUDNESS, 200);
    s3.setParam(START_TIME, 16.0);
    s3.setParam(DURATION, 4);
    s3.setParam(LOUDNESS, 200);
    s4.setParam(START_TIME, 17.0);
    s4.setParam(DURATION, 3);
    s4.setParam(LOUDNESS, 300);

    scor.add(s2);
    scor.add(s3);
    scor.add(s4);
    */

    // render in one channel, 44100 Hz
    MultiTrack* renderedScore = scor.render(2, 44100);

    // write to file
    AuWriter::write(*renderedScore, "gliss-hilo-1p.au");

    delete renderedScore;
    delete reverbObj;
}
