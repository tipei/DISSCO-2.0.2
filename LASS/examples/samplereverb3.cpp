#include "LASS.h"

/**
 *    samplereverb3.cpp
 *
 *    This file demonstrates the simple method of adding reverb effects
 *    to a single partial
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

  // create an amplitude wave shape (using DVS):
  xy.x = 0.0; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 0.2; xy.y = 0.8;
  xyPointCollection.add(xy);
  xy.x = 0.4; xy.y = 0.5;
  xyPointCollection.add(xy);
  xy.x = 1.0; xy.y = 0.0;
  xyPointCollection.add(xy);

  seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = 1;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = 1;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = 3;
  segmentCollection.add(seg);

  Envelope shape (xyPointCollection, segmentCollection);

  // set the loudness of the sound:
  s.setParam(LOUDNESS, 200);

  s.setPartialParam(WAVE_SHAPE, shape);
    
  s.setParam(DURATION, 1);
    
    
  // clear the collections in order to construct a fresh envelope:
  xyPointCollection.clear();
  segmentCollection.clear();

  // create a pan envelope:
  xy.x = 0.0; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 0.5; xy.y = 1.0;
  xyPointCollection.add(xy);
  xy.x = 1.0; xy.y = 0.0;
  xyPointCollection.add(xy);

  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
  segmentCollection.add(seg);
  seg.interType = EXPONENTIAL; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
  segmentCollection.add(seg);

  Envelope pan_left_to_right (xyPointCollection, segmentCollection);
  Pan panL2R (pan_left_to_right);

  // pan the sound around:
  s.setSpatializer (panL2R);

  // set the loudness of the sound:
  s.setParam(LOUDNESS, 200);

  //score:
  Score scor;

  /*
   * Add reverb to the sound using the simple constructor that
   * specifies only room size.  The room size is a value between
   * 0.0 and 1.0.  At 0.0, virtually no reverb can be detected,
   * whereas at 1.0, the reverb becomes overpowering and muddy, 
   * as in a giant stadium with terrible many echos and little 
   * direct sound.  0.3 is a moderate amount of reverb.
   */
  Envelope* percentReverb = new Envelope(pan_left_to_right);
  float hilow_spread   = 0.85;
  float all_pass_gain  = 0.70;
  float reverb_delay   = 0.10;
  Reverb *reverbObj = new Reverb(percentReverb, hilow_spread, all_pass_gain, reverb_delay, 44100);

  s.setParam(START_TIME, 0.0);
  scor.add(s);

  s.setParam(START_TIME, 3.0);
  scor.add(s);
    
  // use channel anticlipping
  scor.setClippingManagementMode(Score::CHANNEL_ANTICLIP);

  //output the xml!
  scor.xml_print();

  // render in one channel, 44100 Hz
  MultiTrack* renderedScore = scor.render(2,44100);
    
  // write to file
  AuWriter::write(*renderedScore, "samplereverb3.au");
    
  delete renderedScore;
  delete reverbObj;
}

