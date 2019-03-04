#include "LASS.h"

/**
 *    sampledetune1.cpp
 *
 *    This file demonstrates how to add detuning effects to a sound
 *    one 6s sound with 4 partials, left-to-right panning, reverb,
 *    and detuning (see comments below)
 **/

int main (int argc, const char * argv[]) {

    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(4,440);
    
    // adjust the partial balance:
    s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00); 
    s.get(1).setParam(RELATIVE_AMPLITUDE, 0.30); 
    s.get(2).setParam(RELATIVE_AMPLITUDE, 0.10); 
    s.get(3).setParam(RELATIVE_AMPLITUDE, 0.05); 

    /*
     * In order to add detuning to a sound, you modify the
     * DETUNE_* parameters with setParam().  
     *
     * The first option, DETUNE_DIRECTION, tells whether you 
     * want to tune the sound (start with randomly spread partials
     * and converge them over time) or detune the sound (start
     * with an 'in-tune' sound and diverge the frequencies over
     * time).  Positive values set to tuning, while negative
     * values set to detuning.
     *
     * The second option, DETUNE_VELOCITY, determines the rate at
     * which the tuning or detuning occurs, and is a value between -1.0
     * and +1.0.  A value of 1.0 means delay the transition between
     * tuned and detuned as long as possible.  A value of -1.0 means
     * perform the transition as rapidly as possible.  A value of 0.0
     * means detune or tune rather rapidly.  For instance, if we are
     * detuning (DETUNE_DIRECTION < 0.0) and we set DETUNE_VELOCITY
     * to, say, -0.8, then we will detune rather rapidly and spend most
     * of the sound in the detuned state.  Likewise, if we are
     * tuning (DETUNE_DIRECTION > 1.0) and we set DETUNE_VELOCITY
     * to, say, 0.0, we will start out out-of-tune, and the tuning
     * process will take up the whole duration of the sound.
     *
     * The third parameter, DETUNE_FUNDAMENTAL, tells whether to leave
     * the fundamental constant as partials tune or detune (this
     * is DETUNE_FUNDAMENTAL < 0.0) or whether to tune or detune the
     * fundamental just like the partials (this is DETUNE_FUNDAMENTAL
     * > 0.0)
     *
     * The last parameter, DETUNE_SPREAD, sets the rate of 'untuned-ness'
     * at the untuned end of the sound (the end of the sound if we're
     * detuning, the beginning of the sound if we're tuning).  It is
     * taken as a percent of the tuned frequency.  For example, at 50%,
     * if our fundamental frequency were 1000Hz, we would be setting
     * the range to be 1000Hz plus or minus 500Hz.
     */

    s.setParam(DETUNE_DIRECTION,  +1.0); // tune (as opposed to detune)
    s.setParam(DETUNE_VELOCITY,   -0.6); // tune rather early
    s.setParam(DETUNE_FUNDAMENTAL,-1.0); // don't tune the fundamental 
    s.setParam(DETUNE_SPREAD,      0.4); // use a 40% spread destination (.5 is half, which is an octave)

    // create an amplitude wave shape (using DVS):
    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.2; xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 0.4; xy.y = 0.5;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 3;
    segmentCollection.add(seg);

    Envelope shape (xyPointCollection, segmentCollection);   

    s.setPartialParam(WAVE_SHAPE, shape);
    
    s.setParam(DURATION, 6);
    
    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    // create a pan envelope:
    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 1.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope pan_left_to_right (xyPointCollection, segmentCollection);
    Pan panL2R (pan_left_to_right);

    // pan the sound around:
    s.setSpatializer (panL2R);  
       
    // set the loudness of the sound:
    s.setParam(LOUDNESS, 200);

    //score:
    Score scor;

    // Create our render object
    float room_size = 0.3;
    Reverb *reverbObj = new Reverb(room_size, 44100);
    scor.use_reverb(reverbObj);
    
    s.setParam(START_TIME, 0.0);
    scor.add(s);

    // render in one channel, 44100 Hz
    MultiTrack* renderedScore = scor.render(2,44100);
    
    // write to file
    AuWriter::write(*renderedScore, "sampledetune1.au");
    
    delete renderedScore;
    delete reverbObj;
}

