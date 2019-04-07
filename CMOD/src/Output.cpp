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

#include "Output.h"
#include "Note.h"

OutputNode* Output::top;
ofstream* Output::particelFile;
int Output::level;

OutputNode::OutputNode(string name) : nodeName(name) {
}

OutputNode::~OutputNode()
{
  for(int i = 0; i < (int)subNodes.size(); i++)
    delete subNodes[i];
  propertyNames.clear();
  propertyValues.clear();
  propertyUnits.clear();
  subNodes.clear();
}


//----------------------------------------------------------------------------//

bool OutputNode::isBottom(void) {
  return (nodeName[0] == 'B' && nodeName[1] == '/');
}


//----------------------------------------------------------------------------//

bool OutputNode::isNote(void) {
  return (nodeName == "Note");
}


//----------------------------------------------------------------------------//

bool OutputNode::isBuildPhase(void) {
  string n = nodeName;
  return (n == "Sweep" || n == "Continuum" || n == "Discrete");
}


//----------------------------------------------------------------------------//

void OutputNode::addProperty(string name, string value, string units)
{
  propertyNames.push_back(name);
  propertyValues.push_back(value);
  propertyUnits.push_back(units);
}


//----------------------------------------------------------------------------//
// find "name" in all properties and return its value
string OutputNode::getProperty(string name) {
  for(unsigned int i = 0; i < propertyNames.size(); i++)
    if(sanitize(propertyNames[i]) == sanitize(name))
      return propertyValues[i];
  return "";
}


//----------------------------------------------------------------------------//

string OutputNode::getXML(void) {
  string s;
  s += "<";
  s += sanitize(nodeName);
  s += ">\n";

  for(int i = 0; i < propertyNames.size(); i++) {
    s += "<";
    s += sanitize(propertyNames[i]);
    s += ">";

    s += propertyValues[i];

    s += "</";
    s += sanitize(propertyNames[i]);
    s += ">\n";
  }

  for(int i = 0; i < subNodes.size(); i++) {
    if(subNodes[i]->isBuildPhase())
      continue;
    s += subNodes[i]->getXML();
  }

  s += "</";
  s += sanitize(nodeName);
  s += ">\n";
  return s;
}


//----------------------------------------------------------------------------//
// move data in subNodes to fomusdata, each bottom event corresponding to one element in the vector

void OutputNode::getFOMUS(vector<Tempo>& tempos, vector<string>& fomusdata) {
  if(!isBottom()) {
    for(int i = 0; i < subNodes.size(); i++) {
      if(subNodes[i]->isBuildPhase())
        continue;
      subNodes[i]->getFOMUS(tempos, fomusdata);  //recursion until reach bottom event
    }
    return;
  }
  else {
	// set up tempo
    Tempo t;
    t.setStartTime(atof(getProperty("TempoStartTime").c_str()));
    t.setTimeSignature(getProperty("TimeSignature"));
    t.setEDUPerTimeSignatureBeat(getProperty("Divisions"));
    string s;
    s.append(getProperty("TempoBeat"));
    s.append("=");
    s.append(getProperty("Tempo"));
    t.setTempo(s);

	//set up fomusdata (time, duration, pitch)
    string n;
    for(unsigned int i = 0; i < subNodes.size(); i++) {
      if(!subNodes[i]->isNote())
        continue;
      Ratio beatNum = t.getEDUPerTimeSignatureBeat();
      Ratio start = subNodes[i]->getProperty("EDUStartTime");
      Ratio dur = subNodes[i]->getProperty("EDUDuration");
      Ratio pitch = subNodes[i]->getProperty("PitchNumber");
      pitch += Ratio(12, 1);
      n.append("time ");
      n.append((start / beatNum).toString());
      n.append(" dur ");
      n.append((dur / beatNum).toString());
      n.append(" pitch ");
      n.append(pitch.toPrettyString());
      n.append(";\n");
    }
    tempos.push_back(t);
    fomusdata.push_back(n);
  }
}


//----------------------------------------------------------------------------//

string OutputNode::findAndReplace(string in, string needle, string replace) {
  while(in.find(needle) != string::npos)
    in.replace(in.find(needle), needle.length(), replace);
  return in;
}


//----------------------------------------------------------------------------//

string OutputNode::sanitize(string name) {
  name = findAndReplace(name, " ", "");
  name = findAndReplace(name, "/", "_");
  return name;
}


//----------------------------------------------------------------------------//

void Output::writeLineToParticel(string line) {
  if(!particelFile) return;
  *particelFile << line << endl;
}


//----------------------------------------------------------------------------//

string Output::getLevelIndentation(bool isProperty, bool isEndLevel) {
  string indentation;
  for(int i = 0; i < level; i++)
    if(isProperty || isEndLevel || i < level - 1)
      indentation = indentation + "| ";
    else
      indentation = indentation + "+-";
  return indentation;
}


//----------------------------------------------------------------------------//

string Output::getPropertyIndentation(void) {
  return getLevelIndentation(true, false) + ". ";
}


//----------------------------------------------------------------------------//

void Output::initialize(string particelFilename) {
  top = 0;
  particelFile = 0;
  level = -1;

  if(particelFilename != "") {
    particelFile = new ofstream();
    particelFile->open(particelFilename.c_str());
  }
}


//----------------------------------------------------------------------------//

void Output::free(void)
{
  delete top;
  delete particelFile;
}


//----------------------------------------------------------------------------//

OutputNode* Output::getCurrentLevelNode(void) {
  if(!top)
    return 0;

  OutputNode* currentNode = top;
  for(int i = 1; i <= level; i++)
    currentNode = currentNode->subNodes.back();
  return currentNode;
}


//----------------------------------------------------------------------------//

void Output::beginSubLevel(string name) {
  if(!top)
    top = new OutputNode(name);
  else
    getCurrentLevelNode()->subNodes.push_back(new OutputNode(name));
  level++;

  //Immediately write level to particel.
  writeLineToParticel(getLevelIndentation(false, false) +
    getCurrentLevelNode()->nodeName);
}


//----------------------------------------------------------------------------//

void Output::addProperty(string name, string value, string units) {
  OutputNode* current = getCurrentLevelNode();
  if(!current)
    cerr << "Warning: Top level does not exist. Property can not be added."
      << endl;
  else
    current->addProperty(name, value, units);

  //Immediately write property to particel.
  string stringToWrite = getPropertyIndentation() + name + ": " + value;
  if(units != "")
    stringToWrite = stringToWrite + " " + units;
  writeLineToParticel(stringToWrite);
}


//----------------------------------------------------------------------------//

void Output::endSubLevel(void) {
  //Before closing level immediately write to particel.
  writeLineToParticel(getLevelIndentation(false, true) + "End " +
    getCurrentLevelNode()->nodeName);
  level--;
}


//----------------------------------------------------------------------------//

void Output::exportToXML(string filename) {
  ofstream* xmlFile;
  if(filename == "")
    return;
  xmlFile = new ofstream();
  xmlFile->open(filename.c_str());
  *xmlFile << top->getXML();
}


//----------------------------------------------------------------------------//

void Output::exportToFOMUS(string filenamePrefix) {
  if(filenamePrefix == "")
    return;
  vector<Tempo> tempos;
  vector<string> fomusData;
  top->getFOMUS(tempos, fomusData);// get the data for tempos and formusData
  while(tempos.size() > 0) {
    stringstream ss_stem;
    float ttime = tempos[0].getStartTime();
    ss_stem << filenamePrefix << ttime << "s";
    string fmsFile = ss_stem.str() + ".fms";
    string lyFile = ss_stem.str() + ".ly";
    string pdfFile = ss_stem.str() + ".pdf";
    string svgFile = ss_stem.str() + ".svg";

    string currentFOMUSData = fomusData[0];
    /*For all of the FOMUS data that has equivalent tempo, merge them to the
    same FOMUS file.*/
    Tempo t = tempos[0];
    for(unsigned int i = 1; i < tempos.size(); i++) {
      if(tempos[0].isTempoSameAs(tempos[i])) {
        if(fomusData[i].size() > 0) {
          currentFOMUSData.append("/////////////////////////////////////\n");
          currentFOMUSData.append(fomusData[i]);
        }
        tempos.erase(tempos.begin() + i);
        fomusData.erase(fomusData.begin() + i);
        i--;
      }
    }
    tempos.erase(tempos.begin());
    fomusData.erase(fomusData.begin());

    if(currentFOMUSData.size() > 0) {
      string FOMUSHeader;
      stringstream tempoTime;
      int min = (int)(ttime / 60.f);
      float sec = (ttime - min * 60.f);

      tempoTime << t.getTempoBeat().toPrettyString() << "=" <<
        t.getTempoBeatsPerMinute().toPrettyString();
      tempoTime << " at " << min << ":";
      if(sec < 10.f)
        tempoTime << "0";
      tempoTime << sec;

      Ratio r = t.getTimeSignature();
      FOMUSHeader.append("//Header\n");
      FOMUSHeader.append("title \"");
      FOMUSHeader.append(tempoTime.str());
      FOMUSHeader.append("\"\n");
      FOMUSHeader.append("time 0 |timesig ( ");
      Ratio rn = r.Num();
      Ratio rd = r.Den();
      FOMUSHeader.append(rn.toPrettyString());
      FOMUSHeader.append(" ");
      FOMUSHeader.append(rd.toPrettyString());
      FOMUSHeader.append(" )|\n");
      FOMUSHeader.append("\n//Notes\n");

      {
      ofstream fomusFile;
      fomusFile.open(fmsFile.c_str());
      fomusFile << FOMUSHeader;
      fomusFile << currentFOMUSData;
      }

      {
      string fomusToLilypond = "fomus -i ";
      fomusToLilypond += fmsFile;
      fomusToLilypond += " -o ";
      fomusToLilypond += lyFile;
      system(fomusToLilypond.c_str());
      }

      {
      string pdfToSVG = "pdf2svg ";
      pdfToSVG += pdfFile;
      pdfToSVG += " ";
      pdfToSVG += svgFile;
      system(pdfToSVG.c_str());
      }
      /*
      {
        string firsvg = "firefox ";
        firsvg.append(svgFile);
        firsvg.append(" &");
        system(firsvg.c_str());
      }
      */
    }
  }
}

void validateSoundsAndRests () {

  vector<Note*>::iterator it;
  Note * cur = *(soundsOnly.begin());
  Note * next;
  Note * r;
  int flag = 0;
  int tuple_end = 0; //record the end time of current tuple
  int in_tuple = 0; //currently the note is within a tuple or not
  int tuple_division = 0;

  //check if there is a rest in the beginning
  if (cur->start_t != 0)
	   r = cur->createRest( 0, cur->start_t, cur->loudness_out, cur->start_t);
  tuple_end = r-> notate_rest(0, tuple_end);
  //check if current note/rest is the beginning of a tuple
  if ((in_tuple == 0) && (r->tuplet == 1))
    tuple_division = (int)((r->tuplet_name[0]) - '0');
    cout << "tuple_division is " << tuple_division << endl;


  // iterate through the vector of all notes pointers
  for (it = soundsOnly.begin(); it!=soundsOnly.end()-2; it++){
    cur = *it;

  //check if current is NULL
    if ((cur == NULL) || (*(it+1) == NULL))
      break;

    //assign value for next note
    next = *(it+1);

      /* is this necessary? */
      //check stime
    flag = cur->checkValue(cur->start_t);

    /* need to be modified */
    //check duration & end time
    cur->checkDuration(cur->start_t, cur->end_t);

    /* overlapping situation, need modification */
    if ( next->start_t < cur->end_t)
      next->start_t = cur->end_t;

    if (cur->end_t <= tuple_end)  //check if current note is within a tuple
      cur->notate(1, tuple_end);
    else {
      tuple_end = cur->notate(0, tuple_end);
      in_tuple = 0;
    }
      //check if current note/rest is the beginning of a tuple
    if ((in_tuple == 0) && (cur->tuplet == 1)){
        tuple_division = (int)((cur->tuplet_name[0]) - '0');
        in_tuple = 1;
    }
    cout << "tuple_division is " << tuple_division << endl;
    cout << "in output: tuple end is  " << tuple_end << endl;


    int gap = next->start_t - cur->end_t;
    // a rest exists between the notes
    /* if gap is very small e.g. gap < 10, how to deal with it?*/
    if (gap > 0) {
      flag = cur->checkValue(gap);

      r = cur->createRest (cur->end_t, next->start_t, cur->loudness_out, next->start_t);
      if (r->end_t <= tuple_end)  //check if current rest is within a tuple
          r->notate_rest(1, tuple_end);
      else {
        tuple_end = r->notate_rest(0, tuple_end);
        in_tuple = 0;
      }
      //check if current note/rest is the beginning of a tuple
      if ((in_tuple == 0) && (r->tuplet == 1)){
        tuple_division = (int)((r->tuplet_name[0]) - '0');
        in_tuple = 1;
      }
      cout << "tuple_division is " << tuple_division << endl;
      cout << "in output: tuple end is  " << tuple_end << endl;
    }

  }

  //processing the last note
  cur = *(soundsOnly.end()-1);
  cur->checkDuration(cur->start_t, cur->end_t);
  if (cur->end_t <= tuple_end)
      tuple_end = cur->notate(1, tuple_end);
  else tuple_end = cur->notate(0, tuple_end);
}



//---------------------------------------------------------------------------//

void output_score(string projectname){

  /* no notes to be written */
  if (soundsOnly.empty()) {
    cout << "	empty" << endl;
     return;
  }

  /* some variables */
  ofstream * fout; 		//output filestream
  ofstream * file_test; // file to verity data
  Note* cur; 			//pointer point to current note
  Note* next;
  string out_str; 		//string to hold output data
  string loudness_prev = ""; 	//store previous loudness
  int prev_time = 0; 		// store starting time of previous note
  int tuplet_s_t = 0; 		//store start EDU of tuplet
  int dur_rest;
  string rest;
  string chord_type;
  int tuplet = 0; 		//tuplet flag
  int chord = 0; 		//chord flag
  Rational<int> temp; 		// temporary variable for print
  int t;

//  cur = *soundsOnly.begin();
//  cout << "Output::output_score before validateS&R" << endl;
  validateSoundsAndRests();
//cout << "OUTPUT NEXT " << next->start_t << endl;
//  cout << "Output::output_score AFTER validateS&R" << endl;
//  int sever; cin >> sever;

  /* open the file stream and output the beginning */
  fout = new std::ofstream;
  file_test = new std::ofstream;
//  string file_name = projectname + ".ly";
  fout->open((projectname + ".ly").c_str());
  file_test->open("note_data.txt");

/*
  // add automatic note splitting
  *fout << "\\new Voice \\with {" << endl;
  *fout << "\\remove \"Note_heads_engraver\"" << endl;
  *fout << "\\consists \"Completion_heads_engraver\"" << endl;
  *fout << "\\remove \"Rest_engraver\"" << endl;
  *fout << "\\consists \"Completion_rest_engraver\"" << endl;
  *fout << "}" << endl;
*/

  *fout << "\\layout {\n  indent = 0 \n  line-width = 180 \n  ragged-right = t \n}" << endl;

  *fout << "{" << endl;


  *fout << "\\time " << timesignature << endl;


        int prev=0;      //ATT !
	int dif;

  vector<Note*>::iterator it; //iterator
  it = soundsAndRests.begin();
  //cout << "In output_score: start to output to lilypond" << endl;

  int counter = 0;

  /* iterate through the vector of all notes pointers */
  while (it!=soundsAndRests.end()){
	cur = *it;

  if (cur-> start_t % 1500 == 0)
   *fout << " \\bar \"\" \\break" << endl;

  // for debug PURPOSE
  *file_test << "Index: " << counter << endl;
  *file_test << "start time: " << cur->start_t << "   end time: " << cur->end_t << endl;
  *file_test << "pitch " << cur->pitch_out << "  duration: " << cur->type_out <<" tuples: " << cur->tuplet << endl;
  *file_test << "loudbess " << cur->loudness_out << endl;
  counter ++;


  //________________________
	int dif= prev - cur->start_t;
  	if (dif > 0) {
//	  cout << "output_score - cur->start_t=" << cur->start_t
//           << " cur->end_t=" << cur->end_t << " prev=" << prev
//            << " dif=" << dif << endl;
        }
        prev=cur->end_t;

	if ((it+1)!= soundsAndRests.end())
	   next = *(it + 1);
	else next = NULL;

	//check for right bracket
	if (cur->tuplet == 0 )
	   if (tuplet ==1 ){
		tuplet = 0;
		*fout << " }  " ;
	   }
	if (tuplet==1 && cur->start_t >= tuplet_s_t + beatEDUs){
	   tuplet = 0;
	   *fout << " } ";
	}

	// check for chord
	if (chord == 1 && next!=NULL)
	   if (next->start_t != cur->start_t){
		chord = 0;
		*fout << cur->pitch_out << ">" << chord_type << " ";
		prev_time = cur->end_t;
		it ++;
		continue;
	   }


	//type and pitch
	if (cur->tuplet == 1 && next!=NULL && next->tuplet!=0)
		if (tuplet == 0){
			tuplet = 1;
			tuplet_s_t = cur->start_t;
			*fout << "\\tuplet ";
			*fout << cur->tuplet_name;
      if (cur->tuplet_name == "") *fout << "3/2";
      *fout << " { ";
		}

	//check for chords
	if (chord == 0 && next!=NULL)
   	   if (next->start_t == cur->start_t && cur->pitch_out!="r" && next->pitch_out!="r"){
		chord = 1;
		*fout << "<";
		chord_type = cur->type_out;
	   }

	if (chord == 1)
		*fout << cur->pitch_out;
	else {
		*fout << cur->pitch_out << cur->type_out;
		while (!cur->modifiers_out.empty()){
			*fout << cur->modifiers_out.back();
			cur->modifiers_out.pop_back();
		}
	}
	//if (cur->split ==1) *fout << "~";

	//loudness
	if (cur->pitch_out != "r") {
          if (cur->loudness_out != loudness_prev) {
	             *fout << cur->loudness_out;
	              cout << "loudness " << cur->loudness_out << " " << loudness_prev << endl;
	              loudness_prev = cur->loudness_out;
	        }
	}

//	if (cur->split ==1) *fout << "~";

	//if (chord ==1 ) chord_out = cur->type_out;
	*fout << " ";
	prev_time = cur->end_t;
	it ++;
  }

  if (chord == 1) *fout << "> " << chord_type;
  if (tuplet == 1) *fout << "}";


  /* output one last thing and close file stream*/
  *fout << "\\bar \"|.\"";
  *fout << endl << "}" << endl;
  fout->close();

}
//=================================================================
//=================================================================
