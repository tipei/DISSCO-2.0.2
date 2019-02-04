#include "LASS.h"

/**
 *    sampleglis2.cpp
 *
 *    This file demonstrates how to add glissando to a sound
 **/

int main (int argc, const char * argv[]) {

    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(2,30);
    
    // adjust the partial balance:
    s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00); 
    s.get(1).setParam(RELATIVE_AMPLITUDE, 1.00); 
//    s.get(2).setParam(RELATIVE_AMPLITUDE, 0.10); 
//    s.get(3).setParam(RELATIVE_AMPLITUDE, 0.05); 

	s.get(1).setParam(FREQUENCY, 20);

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
    xy.x = 0.0; xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 0.01; xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 400;    // 30 * 400 = 1200 Hz end
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 0.01;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue =0.99;
    segmentCollection.add(seg);

    Envelope gliss (xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    // create an amplitude envelope:
    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.1; xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 0.9; xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = 0.1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 0.8;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = 0.1;
    segmentCollection.add(seg);

    Envelope shape (xyPointCollection, segmentCollection);

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

    //score:
    Score scor;

    // Create our render object
    float room_size = 0.3;
    Reverb *reverbObj = new Reverb(room_size, 44100);
    scor.use_reverb(reverbObj);
    
    s.setParam(START_TIME, 0.0);

	// create a sound: 1 partials/2000 Hz
	Sound s0(1, 440);
	s0.setParam(START_TIME, 2.0);
	s0.setParam(DURATION, 2);
	s0.setParam(LOUDNESS, 200);
	s0.setPartialParam(WAVE_SHAPE, shape);
	s0.get(1).setParam(FREQUENCY, 500);

	// create sound: 1 part/4000 Hz
	// THIS ONE IS TOO SOFT?
	Sound s1(2, 550);
	s1.setParam(START_TIME, 4.0);
	s1.setParam(DURATION, 2);
	s1.setParam(LOUDNESS, 200);
	s1.setPartialParam(WAVE_SHAPE, shape);
	s1.get(1).setParam(FREQUENCY, 600);

	// create sound: 1 part/5000 Hz
	Sound s2(2, 640);
	s2.setParam(START_TIME, 6.0);
	s2.setParam(DURATION, 2);
	s2.setParam(LOUDNESS, 200);
	s2.setPartialParam(WAVE_SHAPE, shape);
	s2.get(1).setParam(FREQUENCY, 720);

	// create sound: 1 part/6000 Hz
	// LOUDER?
	Sound s3(2, 800);
	s3.setParam(START_TIME, 8.0);
	s3.setParam(DURATION, 2);
	s3.setParam(LOUDNESS, 200);
	s3.setPartialParam(WAVE_SHAPE, shape);
	s3.get(1).setParam(FREQUENCY, 900);


	// create a sound: 1 partials/2000 Hz
	Sound s4(2, 440);
	s4.setParam(START_TIME, 10.0);
	s4.setParam(DURATION, 2);
	s4.setParam(LOUDNESS, 200);
	s4.setPartialParam(WAVE_SHAPE, shape);
	s4.get(1).setParam(FREQUENCY, 550);

	// create sound: 1 part/4000 Hz
	// THIS ONE IS TOO SOFT?
	Sound s5(2, 600);
	s5.setParam(START_TIME, 12.0);
	s5.setParam(DURATION, 2);
	s5.setParam(LOUDNESS, 200);
	s5.setPartialParam(WAVE_SHAPE, shape);
	s5.get(1).setParam(FREQUENCY, 1000);

	// create sound: 1 part/5000 Hz
	Sound s6(2, 1200);
	s6.setParam(START_TIME, 14.0);
	s6.setParam(DURATION, 2);
	s6.setParam(LOUDNESS, 200);
	s6.setPartialParam(WAVE_SHAPE, shape);
	s6.get(1).setParam(FREQUENCY, 2200);

	// create sound: 1 part/6000 Hz
	// LOUDER?
	Sound s7(2, 1600);
	s7.setParam(START_TIME, 16.0);
	s7.setParam(DURATION, 2);
	s7.setParam(LOUDNESS, 200);
	s7.setPartialParam(WAVE_SHAPE, shape);
	s7.get(1).setParam(FREQUENCY, 3200);

	Sound s8(3, 3500);
	s8.setParam(START_TIME, 18.0);
	s8.setParam(DURATION, 2);
	s8.setParam(LOUDNESS, 200);
	s8.setPartialParam(WAVE_SHAPE, shape);
	s8.get(1).setParam(FREQUENCY, 3600);
	s8.get(2).setParam(FREQUENCY, 4000);
	

	Sound s9(4, 8000);
	s9.setParam(START_TIME, 20.0);
	s9.setParam(DURATION, 2);
	s9.setParam(LOUDNESS, 200);
	s9.setPartialParam(WAVE_SHAPE, shape);
	s9.get(1).setParam(FREQUENCY, 9000);
	s9.get(2).setParam(FREQUENCY, 10000);
	s9.get(3).setParam(FREQUENCY, 11000);

	Sound s10(16, 12000);
	s10.setParam(START_TIME, 22.0);
	s10.setParam(DURATION, 2);
	s10.setParam(LOUDNESS, 200);
	s10.setPartialParam(WAVE_SHAPE, shape);


    scor.add(s);
	scor.add(s0);
	scor.add(s1);
	scor.add(s2);
	scor.add(s3);
	scor.add(s4);
	scor.add(s5);
	scor.add(s6);
	scor.add(s7);
	scor.add(s8);
	scor.add(s9);
	scor.add(s10);


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
    MultiTrack* renderedScore = scor.render(2,44100);
    
    // write to file
    AuWriter::write(*renderedScore, "complex-200l.au");
    
    delete renderedScore;
    delete reverbObj;
}

