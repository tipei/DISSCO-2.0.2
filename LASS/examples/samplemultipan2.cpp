#include "LASS.h"

/**
 *   samplemultipan2.cpp
 *
 *   This sample demonstrates using the MultiPan object to perform
 *   spatialization for more than two speakers, then writing the
 *   rendered sound to a series of files -- one per channel.  This
 *   sample differs from the previous MultiPan example in the way
 *   it sets the speaker responses at given time periods.
 */

int main(int argc, const char* argv[]) {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create a sound:
    Sound s(4, 440);

    // adjust the partial balance:
    s.get(0).setParam(RELATIVE_AMPLITUDE, 1.00);
    s.get(1).setParam(RELATIVE_AMPLITUDE, 0.30);
    s.get(2).setParam(RELATIVE_AMPLITUDE, 0.10);
    s.get(3).setParam(RELATIVE_AMPLITUDE, 0.05);

    // create an amplitude wave shape:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.2;
    xy.y = 0.8;
    xyPointCollection.add(xy);
    xy.x = 0.4;
    xy.y = 0.5;
    xyPointCollection.add(xy);
    xy.x = 1.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);

    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 1;
    segmentCollection.add(seg);
    seg.interType = LINEAR;
    seg.lengthType = FLEXIBLE;
    seg.timeValue = 3;
    segmentCollection.add(seg);

    Envelope shape(xyPointCollection, segmentCollection);

    s.setPartialParam(WAVE_SHAPE, shape);

    s.setParam(DURATION, 10);

    /*
     * As in the previous sample file, we are creating a MultiPan
     * object that will handle 8 speakers, we set up the speaker
     * responses over time, then tell the sound to use this object
     * for spatialization.
     *
     * The difference here is in the method we call to set the
     * speaker responses.  Before, we called addEntry() which one
     * can use to explicitly set the response of individual speakers
     * at time values.  In this example though, we use addEntryLocation,
     * which is different.
     *
     * For addEntryLocation, it is assumed that you have a circular
     * ring of speakers that are evenly spread and all facing the
     * center.  In this configuration, we can talk about the focal
     * point where the sound appears to be coming from (to someone
     * sitting in the exact center of the ring).  The only tricky
     * part is that this value is represented with an angle from the
     * viewer (in radians, not degrees) and a radius from the viewer.
     * In other words, we're using polar coordinates.  For our angle
     * values, 0 means straight in front of the user (at speaker 0).
     * positive angles wrap to the user's left, negative angles wrap
     * to the user's right.  Behind the user at 180 degrees, the
     * positive and negative angles meet up at positive pi (the
     * mathematical constant), and negative pi.  In order to use
     * pi in the program, be sure math.h is included, then use M_PI.
     * Radiuses are specified in arbitrary units that range from 0.0
     * (meaning at the center of the circle) to 1.0 (meaning at the
     * ring of the circle, where the speakers are).
     */

    int n_chan = 8;
    MultiPan multipan(n_chan);
    multipan.addEntryLocation(0.00, M_PI / 4.0, 0.4);
    multipan.addEntryLocation(0.33, -M_PI / 2.0, 0.6);
    multipan.addEntryLocation(0.66, -M_PI / 4.0, 0.8);
    multipan.addEntryLocation(1.00, M_PI, 1.0);
    s.setSpatializer(multipan);

    // set the loudness of the sound:
    s.setParam(LOUDNESS, 200);

    // score:
    Score scor;

    s.setParam(START_TIME, 0.0);
    scor.add(s);

    // render in six channels, 44000 Hz
    MultiTrack* renderedScore = scor.render(n_chan, 44100);

    // write to file
    //  note that for an 8-channel sound, most software can't handle
    //  a single 8-channel file, despite the fact that the AU and AIFF
    //  standards support this.  Instead, we'll write each channel to
    //  its own au file
    Score::manageClipping(renderedScore, Score::CHANNEL_ANTICLIP);
    AuWriter::write(*renderedScore, "samplemultipan2.au");
    AuWriter::write_one_per_track(
        *renderedScore, "samplemultipan2_chan0.au", "samplemultipan2_chan1.au",
        "samplemultipan2_chan2.au", "samplemultipan2_chan3.au", "samplemultipan2_chan4.au",
        "samplemultipan2_chan5.au", "samplemultipan2_chan6.au", "samplemultipan2_chan7.au");

    delete renderedScore;
}
