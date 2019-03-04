#include "LASS.h"

/**
*    sampleclipping1.cpp
*
*    one loud 3s sound (4 partials) added 3 times, panned to the left channel,
*    played at 0s and one soft 2s sound (6 partials) panned to the right
*    channel, played at 3s
*
*    files are output with the different clipping management modes:
*    NONE, CLIP, ANTICLIP, SCALE, CHANNEL_SCALE, and CHANNEL_ANTICLIP
**/

int main (int argc, const char * argv[]) {

    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // Create a loud sound in the left channel:
    Sound loudSound(4,440);

    // create an amplitude wave shape (using DVS):
    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.1; xy.y = 0.9;
    xyPointCollection.add(xy);
    xy.x = 0.2; xy.y = 0.5;
    xyPointCollection.add(xy);
    xy.x = 0.9; xy.y = 0.3;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 7;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);

    Envelope shape1 (xyPointCollection, segmentCollection);
    loudSound.setPartialParam(WAVE_SHAPE, shape1);
    
    // some other embellishments:
    loudSound.setParam(DURATION, 3);
    loudSound.setParam(LOUDNESS, 200);
    Constant zero(0.0);
    Pan left(zero);
    loudSound.setSpatializer(left);
    
    // clear the collections in order to construct a fresh envelope:
    xyPointCollection.clear();
    segmentCollection.clear();

    // Create a soft sound in the right channel:
    Sound softSound(6,532);
    
    // create an amplitude wave shape (using DVS):
    xy.x = 0.0; xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.4; xy.y = 0.9;
    xyPointCollection.add(xy);
    xy.x = 0.5; xy.y = 0.9;
    xyPointCollection.add(xy);
    xy.x = 0.6; xy.y = 0.1;
    xyPointCollection.add(xy);
    xy.x = 1.0; xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 4;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = 4;
    segmentCollection.add(seg);

    Envelope shape2 (xyPointCollection, segmentCollection);
    softSound.setPartialParam(WAVE_SHAPE, shape2);
    
    // some other embellishments:
    softSound.setParam(DURATION, 2);
    softSound.setParam(LOUDNESS, 50);
    Constant one(1.0);
    Pan right(one);
    softSound.setSpatializer( right );
    
    // Place the sounds into a score:   
    Score score;

    loudSound.setParam(START_TIME, 0.0);
    score.add(loudSound);
    score.add(loudSound);
    score.add(loudSound);

    softSound.setParam(START_TIME, 2.5);
    score.add(softSound);
    
    // render the score with no clipping management:
    MultiTrack* thisScore;
    MultiTrack* renderedScore = score.render(2,44000);
    
    cout << "NONE..." << endl;
    thisScore = new MultiTrack(*renderedScore);
    Score::manageClipping(thisScore, Score::NONE);
    AuWriter::write(*thisScore, "sampleclipping1-NONE.au");
    delete thisScore;
    
    // now, try different clipping management modes:  
    cout << "CLIP..." << endl;
    thisScore = new MultiTrack(*renderedScore);
    Score::manageClipping(thisScore, Score::CLIP);
    AuWriter::write(*thisScore, "sampleclipping1-CLIP.au");
    delete thisScore;
    
    cout << "SCALE..." << endl;
    thisScore = new MultiTrack(*renderedScore);
    Score::manageClipping(thisScore, Score::SCALE);
    AuWriter::write(*thisScore, "sampleclipping1-SCALE.au");
    delete thisScore;
    
    cout << "CHANNEL_SCALE..." << endl;
    thisScore = new MultiTrack(*renderedScore);
    Score::manageClipping(thisScore, Score::CHANNEL_SCALE);
    AuWriter::write(*thisScore, "sampleclipping1-CHANNEL_SCALE.au");
    delete thisScore;

    cout << "ANTICLIP..." << endl;
    thisScore = new MultiTrack(*renderedScore);
    Score::manageClipping(thisScore, Score::ANTICLIP);
    AuWriter::write(*thisScore, "sampleclipping1-ANTICLIP.au");
    delete thisScore;
    
    cout << "CHANNEL_ANTICLIP..." << endl;
    thisScore = new MultiTrack(*renderedScore);
    Score::manageClipping(thisScore, Score::CHANNEL_ANTICLIP);
    AuWriter::write(*thisScore, "sampleclipping1-CHANNEL_ANTICLIP.au");
    delete thisScore;

    // clean up:
    delete renderedScore;
}



