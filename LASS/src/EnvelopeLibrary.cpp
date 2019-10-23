/*
LASS (additive sound synthesis library)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
//      Envelope_Library.cpp
//
//----------------------------------------------------------------------------//

#ifndef __ENVELOPE_LIBRARY_CPP
#define __ENVELOPE_LIBRARY_CPP

//----------------------------------------------------------------------------//

#include "EnvelopeLibrary.h"

#include "Collection.h"
#include "Envelope.h"
#include "Types.h"

using std::setw;

//----------------------------------------------------------------------------//
EnvelopeLibrary::EnvelopeLibrary() {}

//----------------------------------------------------------------------------//
EnvelopeLibrary::~EnvelopeLibrary() {
    while (library.size()) delete library.remove(0);
}

//----------------------------------------------------------------------------//
EnvelopeLibrary::EnvelopeLibrary(EnvelopeLibrary& lib) {
    for (int envs = 0; envs < lib.library.size(); envs++)
        library.add(lib.library.get(envs)->clone());
}

//----------------------------------------------------------------------------//
EnvelopeLibrary& EnvelopeLibrary::operator=(EnvelopeLibrary& lib) {
    if (&lib != this) {
        // delete old data
        while (library.size()) delete library.remove(0);

        // reassign new data
        for (int envs = 0; envs < lib.library.size(); envs++) library.add(lib.library.get(envs));
    }

    return *this;
}

//----------------------------------------------------------------------------//
bool EnvelopeLibrary::saveLibrary(char* filename) {
    Envelope* temp_env;
    Collection<xy_point>* temp_coll;

    if (library.size() == 0)  // empty library?
        return false;

    ofstream outData(filename, ios::out);  // open an output stream

    if (!outData) {
        cerr << "ERROR: file could not be written.\n";
        outData.close();
        return false;
    }

    else {
        outData.setf(ios::left, ios::adjustfield);  // format left for setw

        outData << library.size() << "\n\n";  // write number of envelopes

        for (int envs = 0; envs < library.size(); envs++) {
            temp_env = library.get(envs);
            temp_coll = temp_env->getPoints();

            outData << setw(10) << "Envelope" << setw(10) << (envs + 1)
                    << "\n";  // envelope index number

            outData << (temp_coll->size()) << "\n";  // number of points

            for (int pts = 0; pts < ((temp_coll->size()) - 1); pts++) {
                // write point data for this envelope
                // format: point nx, point ny
                outData << setw(10) << (temp_coll->get(pts).x) << setw(10)
                        << (temp_coll->get(pts).y);

                // write segment data for this envelope
                // format: interpolation_type n, time_type n, time_value n

                // interpolation type
                if ((temp_env->getSegmentInterpolationType(pts)) == LINEAR)
                    outData << setw(15) << "LINEAR";
                else if ((temp_env->getSegmentInterpolationType(pts)) == EXPONENTIAL)
                    outData << setw(15) << "EXPONENTIAL";
                else if ((temp_env->getSegmentInterpolationType(pts)) == CUBIC_SPLINE)
                    outData << setw(15) << "CUBIC_SPLINE";

                // length type
                if (temp_env->getSegmentLengthType(pts) == FLEXIBLE)
                    outData << setw(10) << "FLEXIBLE";
                else
                    outData << setw(10) << "FIXED";

                // time value, not actually used
                outData << setw(10) << (temp_env->getSegmentLength(pts)) << "\n";
            }
            // write the last set of points: point (n + 1)x, point (n + 1)y
            outData << setw(10) << (temp_coll->get((temp_coll->size()) - 1).x) << setw(10)
                    << (temp_coll->get((temp_coll->size()) - 1).y) << "\n\n";
        }
        outData.close();
    }

    return true;
}

//----------------------------------------------------------------------------//
int EnvelopeLibrary::loadLibrary(char* filename) {
    int count_envs, count_pts, env_number;
    float xpt, ypt;
    char cTemp[10000];
    interpolation_type intyp = LINEAR;
    stretch_type tstyp;
    m_time_type timev;
    xy_point point;
    envelope_segment segment;
    Collection<xy_point> env_points;
    Collection<envelope_segment> env_segments;
    Envelope* temp_env;

    ifstream inData(filename, ios::in);  // open an input stream

    if (!inData) {
        cerr << "ERROR: file could not be opened.\n";
        inData.close();
        return -1;
    }

    else {
        inData >> count_envs;  // read number of envelopes

        for (int envs = 0; envs < count_envs; envs++) {
            // clear temporary collections
            env_points.clear();
            env_segments.clear();

            inData >> cTemp;       // envelope descriptor, discard
            inData >> env_number;  // envelope number, discard
            inData >> count_pts;   // number of points

            for (int pts = 0; pts < (count_pts - 1); pts++) {
                // read point data for this envelope
                // format: point nx, point ny
                inData >> xpt;
                inData >> ypt;

                point.x = xpt;
                point.y = ypt;

                env_points.add(point);

                // read segment data for this envelope
                // format: interpolation_type n, time_type n, time_value n

                // conversions for enumerated types
                inData >> cTemp;
                if (strcmp(cTemp, "LINEAR") == 0)
                    intyp = LINEAR;
                else if (strcmp(cTemp, "EXPONENTIAL") == 0)
                    intyp = EXPONENTIAL;
                else if (strcmp(cTemp, "CUBIC_SPLINE") == 0)
                    intyp = CUBIC_SPLINE;

                inData >> cTemp;
                if (strcmp(cTemp, "FIXED") == 0)
                    tstyp = FIXED;
                else
                    tstyp = FLEXIBLE;

                inData >> timev;

                segment.interType = intyp;
                segment.lengthType = tstyp;
                segment.length = timev;

                env_segments.add(segment);
            }
            // read the last set of points: point (n + 1)x, point (n + 1)y
            inData >> xpt;
            inData >> ypt;

            point.x = xpt;
            point.y = ypt;

            env_points.add(point);

            // construct a Envelope and add it to the library
            temp_env = new Envelope(env_points, env_segments);
            library.add(temp_env);
        }

        inData.close();
    }

    return (library.size());
}

//----------------------------------------------------------------------------//

int EnvelopeLibrary::loadLibraryNewFormat(char* filename) {
    std::ifstream inData(filename, ios::in);  // open input stream

    if (!inData) {
        cerr << "ERROR: file could not be opened.\n";
        inData.close();
        return -1;

    } else {
        // tmp vars to build envs
        float xpt, ypt;
        xy_point point;
        std::string line, buf, interp, stretch;
        Collection<xy_point> env_points;

        interpolation_type intyp = LINEAR;
        stretch_type tstyp;
        envelope_segment segment;
        Collection<envelope_segment> env_segments;

        Envelope* temp_env;

        while (!std::getline(inData, line).eof()) {
            std::stringstream nextCheck(line);
            std::stringstream envLine(line);

            if (nextCheck >> buf) {
                if (buf == "Envelope") {
                    // start a new env
                    env_points.clear();
                    env_segments.clear();
                } else if (envLine >> xpt >> ypt) {
                    // must be data about env
                    point.x = xpt;
                    point.y = ypt;
                    env_points.add(point);

                    // calculate and set length (timev) for the previous segment
                    if (env_segments.size() > 0) {
                        float lastxpt = env_points.get(env_segments.size() - 1).x;
                        env_segments.get(env_segments.size() - 1).length = xpt - lastxpt;
                    }

                    if (envLine >> interp >> stretch) {
                        if (interp == "LINEAR") {
                            intyp = LINEAR;
                        } else if (interp == "EXPONENTIAL") {
                            intyp = EXPONENTIAL;
                        } else if (interp == "CUBIC_SPLINE") {
                            intyp = CUBIC_SPLINE;
                        }
                        if (stretch == "FIXED") {
                            tstyp = FIXED;
                        } else {
                            tstyp = FLEXIBLE;
                        }
                        segment.interType = intyp;
                        segment.lengthType = tstyp;
                        env_segments.add(segment);  // length gets set the next time around

                    } else {
                        // last line of an env ---- create it!
                        temp_env = new Envelope(env_points, env_segments);
                        library.add(temp_env);
                    }
                }
            }
        }  // end while

        inData.close();
    }

    return (library.size());
}

//----------------------------------------------------------------------------//

Envelope* EnvelopeLibrary::getEnvelope(int index) {
    if (index > library.size())
        return NULL;
    else
        return (library.get(index - 1))->clone();
}

//----------------------------------------------------------------------------//

const Envelope& EnvelopeLibrary::getEnvelopeRef(int index) { return *library.get(index - 1); }

//----------------------------------------------------------------------------//
int EnvelopeLibrary::addEnvelope(Envelope* env) {
    library.add(env);
    return (library.size());
}

//----------------------------------------------------------------------------//
int EnvelopeLibrary::addEnvelope(Collection<xy_point> points,
                                 Collection<envelope_segment> segments) {
    Envelope* temp_env;
    temp_env = new Envelope(points, segments);
    return (addEnvelope(temp_env));
}

//----------------------------------------------------------------------------//
bool EnvelopeLibrary::updateEnvelope(int index, Envelope* env) {
    if (index > library.size())
        return false;
    else {
        library.set((index - 1), env);
        return true;
    }
}

//----------------------------------------------------------------------------//
void EnvelopeLibrary::showEnvelope(int index) {
    Envelope* temp_env;

    if (index > library.size())
        return;
    else {
        temp_env = library.get(index - 1);
        temp_env->print();
    }
}

//----------------------------------------------------------------------------//
int EnvelopeLibrary::size() { return (library.size()); }

//----------------------------------------------------------------------------//
#endif  //__ENVELOPE_LIBRARY_CPP
