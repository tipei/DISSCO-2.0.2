#include "LASS.h"

/**
 *   sampleenvelope1.cpp
 *
 *   This sample files demonstrates how to construct a basic shape by
 *   specifying points and segments for a Envelope (ENV).
 *
 *   There are also some point interpolation tests made.
 *   
 *   Written by Michael Aikins (maikins@uiuc.edu)
 *
 **/ 

int main (int argc, const char * argv[]) {
    Sound s1(2, 100);

    //Make a collection
    Collection<envelope_segment> segmentCollection;
    envelope_segment seg;

    //The initial segment info isn't important, kind of like a place holder
    // for the beginning of the Envelope
    seg.x = 0.0; seg.y = 0.0;
    seg.interType = EXPONENTIAL; seg.lengthType = FIXED;
    segmentCollection.add(seg);
    seg.lengthType = FLEXIBLE;
    seg.x = 0.5; seg.y = 0.8;
    segmentCollection.add(seg);
    seg.lengthType = FIXED;
    seg.interType = LINEAR;
    seg.x = 0.8; seg.y = 0.8;
    segmentCollection.add(seg);
    seg.x = 1.0; seg.y = 0.0;
    segmentCollection.add(seg);
    seg.x = 1.2; seg.y = 0.0;
    segmentCollection.add(seg);
    seg.x = 1.7; seg.y = 0.0;
    segmentCollection.add(seg);
    seg.x = 2.0; seg.y = 0.8;
    segmentCollection.add(seg);
    seg.x = 2.2; seg.y = 0.0;
    segmentCollection.add(seg);

    //I made this variable so that if I want to change the duration,
    //I don't need to change it in all the getValue calls
    float dur = 5.0;

    //The constructor call to the envelope class. This makes an envelope
    //using the information in segmentCollection
    Envelope env (segmentCollection);

    env.Print();
   
    // this tests approximation at various points using the getValue function
    // the first value being the time in question, the second being the 
    // total length of the envelope
    // note: 1.3 does not correspond to a point
    cout << "- Envelope getValue calls" << endl;
    cout << "    value 1: " << env.getValue(0.2, dur) << endl;
    cout << "    value 2: " << env.getValue(1.0, dur) << endl;
    cout << "    value 3: " << env.getValue(1.3, dur) << endl;
    cout << "- Envelope getValue calls END" << endl;


    //This sets the wave shape of the sound to the envelope we made
    s1.setPartialParam(WAVE_SHAPE, env);
    
    //Set some other parameters
    s1.setParam(DURATION, dur);
    s1.setParam(LOUDNESS, 150);

    Score scor;
    s1.setParam(START_TIME, 0.0);

    //Add the sound to a score so we can render it
    scor.add(s1);
    
    // write out xml, just makes it easier to see how things are stored
    scor.xml_print();

    // render in two channels, 44100 Hz
        MultiTrack* renderedScore = scor.render(2,44100);
    
    // write to file
    AuWriter::write(*renderedScore, "sampleenvelope1.au");
    
    delete renderedScore;
    
}
