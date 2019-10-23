/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)
Modified by Ming-ching Chiu 2013


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
//  Main.cpp
//
//  This is the main program for CMOD, the computer-assisted
//  composition software. It creates the Piece object which reads the .dissco
//  file (in XML format).
//---------------------------------------------------------------------------//

/**
 *  1/29/07: Justin King added doxygen commenting
 *  10/29/12: Ming-ching Chiu revised the code to remove filevalue
 **/

#include <time.h>

#include "Note.h"
#include "Piece.h"

// added by Sever must be a more elegant way
#include <fstream>
#include <iostream>
using namespace std;

extern ofstream* outputFile;
// for Note class

int main(int parameterCount, char** parameterList) {
    time_t startTime;
    time(&startTime);

    // added by Sever
    outputFile = new ofstream;
    outputFile->open("../SQ/NotationFile.txt", ios::out);
    *outputFile << "Greetingd from Maine" << endl;
    // for Note class

    // Determine settings.
    cout << endl;
    cout << "=========================SETTINGS==========================" << endl;

    // Determine the project path.
    string path;
    if (parameterCount >= 2) path = parameterList[1];
    if (path == "--help" || path == "-help" || path == "help") {
        cout << "Usage: cmod          Runs CMOD in the current directory." << endl;
        cout << "       cmod <path>   Runs CMOD in the <path> directory." << endl;
        // cout << "       cmod <path> <process-offset=0> <process-count=1>" <<
        // endl; cout << "                     Renders a specific mask of
        // sounds." << endl;
        cout << "       cmod help   Displays this help." << endl;
        return 0;
    }

    size_t lastSlash = path.find_last_of('/');
    string workingPath = path.substr(0, lastSlash + 1);
    cout << "Working in path: " << workingPath << endl;

    // Determine the project name.
    // remove extension .dissco
    path = path.substr(0, path.length() - 7);
    string projectName;

    if (lastSlash != string::npos) {
        projectName = path.substr(lastSlash + 1);
    } else {
        projectName = path;
    }

    // Determine project sound file output.
    PieceHelper::createSoundFilesDirectory(workingPath);
    PieceHelper::createScoreFilesDirectory(workingPath);

    // Create the piece!
    Piece* piece = new Piece(workingPath, projectName);
    delete piece;
    // delete outputFile;		//Sever

    time_t endTime;
    time(&endTime);

    int seconds = difftime(endTime, startTime);
    int hr = seconds / 3600;
    int min = (seconds % 3600) / 60;
    int sec = seconds % 60;
    printf("Computation Time: %02d:%02d:%02d.\n", hr, min, sec);

    return 0;
}
