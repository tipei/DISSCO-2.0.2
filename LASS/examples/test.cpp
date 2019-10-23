#include "LASS.h"

/**
 *	test.cpp
 *
 *	This program generates tests through interactively asking
 *  questions of the user.
 **/

int main() {
    Collection<xy_point> xyPointCollection;
    Collection<envelope_segment> segmentCollection;
    xy_point xy;
    envelope_segment seg;

    // create an amplitude envelope:
    xy.x = 0.0;
    xy.y = 0.0;
    xyPointCollection.add(xy);
    xy.x = 0.02;
    xy.y = 1.0;
    xyPointCollection.add(xy);
    xy.x = 0.95;
    xy.y = 1.0;
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

    // score to use
    Score scor;

    double start_time = 0.0;
    int freq = -1;
    int parts = -1;
    double *part_val = NULL;
    double deviation = -1;
    int loud = -1;
    double duration = -1;

    // get user input
    int choice = 0;
    do {
        cout << "MOSS2 Test Generat0r:" << endl;
        cout << " 0. Exit" << endl;
        cout << " 1. add a sound" << endl;
        cout << " 2. Generate .au file" << endl;
        cout << endl;

        cin >> choice;

        switch (choice) {
            case 1: {
                if (freq == -1) {
                    // get frequency
                    cout << "Frequency: ";
                    cin >> freq;
                }

                if (parts == -1) {
                    // get partials
                    cout << "Partials: ";
                    cin >> parts;
                    part_val = new double[parts];
                    for (int i = 0; i < parts; i++) {
                        cout << "  Relative loudness of part " << i << ": ";
                        cin >> part_val[i];
                    }
                }

                if (deviation == -1) {
                    // get deviation
                    cout << "Deviation (0-1): ";
                    cin >> deviation;
                }

                if (loud == -1) {
                    // get loudness
                    cout << "Loudness: ";
                    cin >> loud;
                }

                if (duration == -1) {
                    // get duration
                    cout << "Duration: ";
                    cin >> duration;
                    cout << endl;
                }

                // loop for changes to values
                int choice2;
                do {
                    // print params
                    cout << "(1)Frequency\t"
                         << "(2)Partials\t"
                         << "(3)Deviation\t"
                         << "(4)Loudness\t"
                         << "(5)Duration\n";
                    cout << freq << "\t\t" << parts << "\t\t" << deviation << "\t\t" << loud
                         << "\t\t" << duration << endl
                         << endl;

                    // change things if needed
                    cout << "Enter number to change or 0 to continue: ";

                    cin >> choice2;
                    switch (choice2) {
                        case 1:
                            cout << "Frequency: ";
                            cin >> freq;
                            break;
                        case 2:
                            cout << "Partials: ";
                            cin >> parts;
                            part_val = new double[parts];
                            for (int i = 0; i < parts; i++) {
                                cout << "  Relative loudness of part " << i << ": ";
                                cin >> part_val[i];
                            }
                            break;
                        case 3:
                            cout << "Deviation (0-1): ";
                            cin >> deviation;
                            break;
                        case 4:
                            cout << "Loudness: ";
                            cin >> loud;
                            break;
                        case 5:
                            cout << "Duration: ";
                            cin >> duration;
                            break;
                    }
                } while (choice2 != 0);

                // create the sound and add to score
                Sound *sound = new Sound(parts, freq);
                sound->setParam(START_TIME, start_time);
                start_time += 2.0;
                sound->setParam(DURATION, duration);
                sound->setParam(LOUDNESS, loud);
                sound->setPartialParam(WAVE_SHAPE, shape);

                for (int i = 0; i < parts; i++) {
                    sound->get(i).setParam(RELATIVE_AMPLITUDE, part_val[i]);
                }

                scor.add(*sound);
                cout << "Added sound." << endl << endl;

                break;
            }
            case 2: {
                // render score of all sounds
                char filename[50];
                cout << "Enter name of file (.au): ";
                cin >> filename;

                MultiTrack *renderedScore = scor.render(2, 44100);
                AuWriter::write(*renderedScore, filename);
                delete renderedScore;

                break;
            }
        }  // switch

    } while (choice != 0);
}
