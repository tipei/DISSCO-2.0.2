#include "LASS.h"


/**
*    This sample file demonstrates how to add transients to a sound.
*    This is done by using a few of 6 Partial Paramaters:
*    AMPTRANS_AMP_ENV, AMPTRANS_RATE_ENV, AMPTRANS_WIDTH,
*    FREQTRANS_AMP_ENV, FREQTRANS_RATE_ENV, and FREQTRANS_WIDTH.
*    
*    The AMP_ENVs store the envelope used to determine the size of the
*    transients, while the RATE_ENVs store the probability that a transient
*    will occur.
*
*    Both TRANS_WIDTHs are stored as number of samples, defaulting to 1103, 
*    which is 0.025 seconds with a 44120 sampling rate.
*
*    This sample program modifies sampledvs7.cpp to include transients.
*
*    Note: You can send constant values to each of the envelope parameters!
*    This makes it easier for you since you can have constant values without
*    going through the hassle of defining a ENV.
*    Example: s1.setPartialParam(AMPTRANS_AMP_ENV, 0.6);
**/ 

int main (int argc, const char * argv[]) {
    Sound s1(2, 440);

    Collection<xy_point> xyPointCollection, xy2;
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

    seg.interType = EXPONENTIAL; seg.lengthType = FIXED; seg.timeValue = .5;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = .3;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = .2;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = .2;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = .5;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = .3;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FIXED; seg.timeValue = .2;
    segmentCollection.add(seg);

    Envelope env (xyPointCollection, segmentCollection);

    //create another env to store the transient information
    xy.x = 0.0; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 0.5; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 0.8; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 1.0; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 1.2; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 1.7; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 2.0; xy.y = 0.6;
    xy2.add(xy);
    xy.x = 2.2; xy.y = 0.6;
    xy2.add(xy);

    Envelope transenv (xy2, segmentCollection);

    //set the paramaters
    s1.setPartialParam(AMPTRANS_AMP_ENV, transenv);
    //s1.setPartialParam(FREQTRANS_AMP_ENV, transenv);
    s1.setPartialParam(AMPTRANS_RATE_ENV, transenv);
    //s1.setPartialParam(FREQTRANS_RATE_ENV, transenv);
    s1.setPartialParam(AMPTRANS_WIDTH, 1103);
    //s1.setPartialParam(FREQTRANS_WIDTH, 1103);

	xy = env.getPoint(2);

	cout << xy.x << " " << xy.y << endl;

    cout << "- ENV getValue calls" << endl;
    cout << "    value 1: " << env.getValue(0.5, 2.2) << endl;
    cout << "    value 2: " << env.getValue(1.0, 2.2) << endl;
    cout << "    value 3: " << env.getValue(1.3, 2.2) << endl;
    cout << "- ENV getValue calls END" << endl;

    s1.setPartialParam(WAVE_SHAPE, env);
    s1.setParam(DURATION, 2.2);
    s1.setParam(LOUDNESS, 150);

    Score scor;
    s1.setParam(START_TIME, 0.0);
    scor.add(s1);
    
    // write out xml
    scor.xml_print();

    // render in two channels, 44100 Hz
    MultiTrack* renderedScore = scor.render(2,44100);
    
    // write to file
    AuWriter::write(*renderedScore, "sampletransient1.au");
    
    delete renderedScore;
}
