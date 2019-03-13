#include "LASS.h"

/**
*    sample0.cpp
*
*    two sounds (1.2s played at 0s and 1.5s played at 3s) with 4 partials,
*    left-to-right panning, and reverb 
**/

int main (int argc, const char * argv[]) {

    // Create one wave shape (using DVS)
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.5; xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 0.8; xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 1.2; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 1.7; xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 2.0; xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 2.2; xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = EXPONENTIAL; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .3;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .2;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .2;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .3;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .2;
    segmentCollection.add(seg);

    Envelope shape1 (xyPointCollection, segmentCollection);

    // clear the collections in order to construct a fresh envelope
    xyPointCollection.clear();
    segmentCollection.clear();

    // create a second wave shape (using DVS)
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

    Envelope shape2 (xyPointCollection, segmentCollection);

    // create two sounds:
    Sound s1(4,440), s2(4,440*3.0/2.0);

    // adjust the partial balance:
    s1.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
    s1.get(1).setParam(RELATIVE_AMPLITUDE, 0.30);
    s1.get(2).setParam(RELATIVE_AMPLITUDE, 0.10);
    s1.get(3).setParam(RELATIVE_AMPLITUDE, 0.05);

    s1.setPartialParam(WAVE_SHAPE, shape1);
    s1.setParam(DURATION, 1.2);

    // adjust the partial balance:
    s2.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
    s2.get(1).setParam(RELATIVE_AMPLITUDE, 0.38);
    s2.get(2).setParam(RELATIVE_AMPLITUDE, 0.13);
    s2.get(3).setParam(RELATIVE_AMPLITUDE, 0.07);

    s2.setPartialParam(WAVE_SHAPE, shape2);
    s2.setParam(DURATION, 1.5);

    // clear the collections in order to construct a fresh envelope
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

    // pan the sound arround:
    s1.setSpatializer (panL2R);
    s2.setSpatializer (panL2R);

    // set the loudness of the sound:
    s1.setParam(LOUDNESS, 200);
    s2.setParam(LOUDNESS, 200);
    
    //score:
    Score scor;

    // Create our render object:
    float room_size = 0.3;
    Reverb *reverbObj = new Reverb(room_size, 44100);
    room_size = 0.5;
    Reverb *reverbObj2 = new Reverb(room_size, 44100);
    scor.use_reverb(reverbObj);

    s1.setParam(START_TIME, 0.0);
    s1.use_reverb(reverbObj);
    scor.add(s1);

    s2.setParam(START_TIME, 3.0);
    s2.use_reverb(reverbObj2);
    scor.add(s2);

    //output the xml!
    scor.xml_print();

    // render in one channel, 44100 Hz:
    MultiTrack* renderedScore = scor.render(2,44100);
    
    // write to file:
    AuWriter::write(*renderedScore, "sample0.au");
    
    delete renderedScore;
    delete reverbObj;
    delete reverbObj2;
}

