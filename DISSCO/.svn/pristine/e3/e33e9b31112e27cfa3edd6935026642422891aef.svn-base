#include "LASS.h"

/**
 *   samplemultipan1.cpp
 *
 *   This sample program demonstrates how to use a MultiPan object
 *   to spatialize a sound for more than 2 speakers, and then how
 *   to write the rendered sound out to one file per channel so
 *   that one can import them separately into a recording suite
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

  // create an amplitude wave shape:
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
    
  s.setParam(DURATION, 10);
    
  /*
   * Here, we allocate a MultiPan objec that is set to use 8 channels.
   * We tell the object how we want the speakers to act at various time
   * values, then tell our sound to use the multipan object for 
   * spatialization.  
   *
   * Each call to addEntry has 9 parameters.  The first is a time value
   * (in percent of total duration), and the next 8 represent speaker
   * response ratios/percentages at that time value.  I say ratio or
   * percentage, because this example uses the numbers as percentages,
   * but you can also put in ratios.  The MultiPan object will scale
   * the numbers so that all speaker response values at any given
   * time add up to 1.0.  Thus, if you had a three speaker set up, you
   * could give addEntry(0.5, 1.0, 1.0, 4.0) and the response values
   * would be scaled so that speakers one and two each get 1/6th of the
   * sound while speaker three gets 4/6th of the sound.
   */

  int n_chan = 8;
  MultiPan multipan(n_chan);
  multipan.addEntry(0.000,  1.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000);
  multipan.addEntry(0.100,  0.000, 1.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000);
  multipan.addEntry(0.200,  0.000, 0.000, 1.000, 0.000, 0.000, 0.000, 0.000, 0.000);
  multipan.addEntry(0.300,  0.000, 0.000, 0.000, 1.000, 0.000, 0.000, 0.000, 0.000);
  multipan.addEntry(0.400,  0.000, 0.000, 0.000, 0.000, 1.000, 0.000, 0.000, 0.000);
  multipan.addEntry(0.500,  0.000, 0.000, 0.000, 0.000, 0.000, 1.000, 0.000, 0.000);
  multipan.addEntry(0.600,  0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 1.000, 0.000);
  multipan.addEntry(0.700,  0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 1.000);
  multipan.addEntry(0.800,  1.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000);
  multipan.addEntry(0.900,  0.000, 1.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000);
  multipan.addEntry(1.000,  0.000, 0.000, 1.000, 0.000, 0.000, 0.000, 0.000, 0.000);
  s.setSpatializer(multipan);

  // set the loudness of the sound:
  s.setParam(LOUDNESS, 200);

  //score:
  Score scor;
    
  s.setParam(START_TIME, 0.0);
  scor.add(s);

  // render in six channels, 44000 Hz
  MultiTrack* renderedScore = scor.render(n_chan,44100);
    
  // write to file
  //  note that for an 8-channel sound, most software can't handle
  //  a single 8-channel file, despite the fact that the AU and AIFF
  //  standards support this.  Instead, we'll write each channel to
  //  its own au file 
  Score::manageClipping(renderedScore, Score::CHANNEL_ANTICLIP);

  scor.xml_print();

  AuWriter::write(*renderedScore, "samplemultipan1.au"); 
  AuWriter::write_one_per_track(*renderedScore, 
				"samplemultipan1_chan0.au",
				"samplemultipan1_chan1.au",
				"samplemultipan1_chan2.au",
				"samplemultipan1_chan3.au",
				"samplemultipan1_chan4.au",
				"samplemultipan1_chan5.au",
				"samplemultipan1_chan6.au",
				"samplemultipan1_chan7.au");
    
  delete renderedScore;
}



