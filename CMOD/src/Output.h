/*
CMOD (composition module)
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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "Libraries.h"
#include "Tempo.h"

struct OutputNode {
    string nodeName;

    vector<string> propertyNames;
    vector<string> propertyValues;
    vector<string> propertyUnits;

    vector<OutputNode*> subNodes;  // vector of pointers to the class itself
                                   // why not put this in Output class

    OutputNode(string name);
    ~OutputNode();

    void addProperty(string name, string value, string units);
    string getProperty(string name);
    bool isBottom(void);
    bool isNote(void);
    bool isBuildPhase(void);
    string getXML(void);
    void getFOMUS(vector<Tempo>& tempos, vector<string>& fomusdata);

    static string findAndReplace(string in, string needle, string replace);

    /**
     *  Sanitize - remove illegal symbols
     **/
    static string sanitize(string name);
};

/// The static output class.
class Output {
    static OutputNode* top;

    static ofstream* particelFile;
    static int level;

    static void writeLineToParticel(string line);

protected:
    static string getLevelIndentation(bool isProperty, bool isEndLevel);
    static string getPropertyIndentation(void);

public:
    static void initialize(string particelFilename);
    static void free(void);
    static OutputNode* getCurrentLevelNode(void);
    static void beginSubLevel(string name);
    static void addProperty(string name, string value, string units = "");
    template <class T>
    static void addProperty(string name, T value, string units = "") {
        stringstream oss;
        oss.setf(ios::fixed, ios::floatfield);
        oss.precision(2);
        oss << value;
        addProperty(name, string(oss.str()), units);
    }
    static void endSubLevel(void);
    static void exportToXML(string filename);
    static void exportToFOMUS(string filenamePrefix);
};

/**
 * Outputs all notes to written score
 *					 --shenyi
 **/
extern void output_score(string projectname);

#endif
