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
//   Piece.cpp
//
//----------------------------------------------------------------------------//

#include "Piece.h"

#include "Libraries.h"
#include "Define.h"
#include "FileValue.h"
#include "Note.h"
#include "Output.h"
#include "Random.h"
#include "Utilities.h"
#include <fstream>



//----------------------------------------------------------------------------//

int PieceHelper::getDirectoryList(string dir, vector<string> &files) {
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return 0;
  
}

//----------------------------------------------------------------------------//

string PieceHelper::getFixedPath(string path) {
  if(path == "")
    return "./";
  if(path.c_str()[path.length() - 1] != '/')
    path = path + "/";
  return path;
}

//----------------------------------------------------------------------------//

string PieceHelper::getProjectName(string path) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    string f = files[i];
    if(f.length() >= 5 && f.substr(f.length() - 3, 3) == "dat") {
      g = f.erase(f.length() - 4);
      break;
    }
  }
  
  if(g == "") {
    cout << endl;
    cout << "=========================================================" << endl;
    cout << "Warning: there is no CMOD project file in this directory." << endl;
    cout << "=========================================================" << endl;
    cout << "Files in '" << path << "':" << endl;
    
    for(unsigned int i = 0; i < files.size(); i++)
      cout << files[i] << endl;
    cout << "=========================================================" << endl;
    cout << endl;
  }
  
  return g;
  
}


//----------------------------------------------------------------------------//

int PieceHelper::getSeedNumber(string seed) {
  if(seed == "time") {
    int now = (int)time(0);
    cout << "Using current time " << now << " as seed." << endl;
    return now;
  }
  
  const char* seed_c = seed.c_str();
  int seedNumber = 0;
  int digits = 1;
  for(int i = 0; i < seed.length(); i++)
    digits *= 10;
  for(int i = 0; seed_c[i] != 0; i++) {
    digits /= 10;
    seedNumber += digits * (int)(seed_c[i] - '0');
  }
  if(seedNumber < 0) seedNumber = -seedNumber;
  return seedNumber;
}

//----------------------------------------------------------------------------//

void PieceHelper::createSoundFilesDirectory(string path) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    if(files[i] == "SoundFiles")
      return;
  }
  
  string h = "mkdir " + path + "SoundFiles";
  system(h.c_str());

}

//----------------------------------------------------------------------------//

void PieceHelper::createScoreFilesDirectory(string path) {

  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  bool dirExists = false;
  for(unsigned int i = 0; i < files.size(); i++) {
    if(files[i] == "ScoreFiles") {
      dirExists = true;
      break;
    }
  }
  
  string h = "mkdir " + path + "ScoreFiles";
  if(!dirExists)
    system(h.c_str());
  h = "rm -f " + path + "ScoreFiles/*.fms";
  system(h.c_str());

}

//----------------------------------------------------------------------------//

bool PieceHelper::doesFileExist(string path, string filename) {
  string dir = string(path);
  vector<string> files = vector<string>();
  getDirectoryList(dir, files);
  string g = "";
  for(unsigned int i = 0; i < files.size(); i++) {
    if(files[i] == filename)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------//

string Piece::getNextSoundFile() {

  string soundPath =  "SoundFiles/";
  //cout<<"sound Path:"<<soundPath<<endl;
  for(int i = 1; i < 1000000; i++) {
    stringstream oss;
    oss << projectName << i << ".aiff";
    if(PieceHelper::doesFileExist(soundPath, oss.str()))
      continue;
    else
      return "SoundFiles/" + oss.str();
  }
  return "";
}


//----------------------------------------------------------------------------//

void Piece::Print() {
  Output::beginSubLevel("Information");
    Output::beginSubLevel("General");
      Output::addProperty("Title", title);
    Output::endSubLevel();
    
    Output::beginSubLevel("Audio");
      Output::addProperty("Using Synthesis", soundSynthesis ? "Yes" : "No");
      Output::addProperty("Channels", numChannels);
      Output::addProperty("Sample Rate", sampleRate, "Hz");
      Output::addProperty("Sample Resolution", sampleSize, "bits");
    Output::endSubLevel();
    
    Output::beginSubLevel("Execution");
      Output::addProperty("Seed", "(unspecified)");
      Output::addProperty("Threads", numThreads);
      Output::addProperty("File Flags", fileFlags);
      Output::addProperty("File List", fileList);
    Output::endSubLevel();
  Output::endSubLevel();
}


Piece::Piece(string _workingPath, string _projectTitle){
  path = _workingPath;
  projectName = _projectTitle;
  //Change working directory.
  chdir(_workingPath.c_str());

  //Parse .dissco File
  XMLPlatformUtils::Initialize();
  XercesDOMParser* parser = new XercesDOMParser();
  string disscoFile = _projectTitle+ ".dissco";
  parser->parse(disscoFile.c_str());
  
  //get the parsed DOM Document and read the configuration
  DOMDocument* xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();
  DOMElement* configurations = root->GFEC();
  DOMElement* element = configurations->GFEC();
  
  title = XMLTC(element);
  element = element->GNES();
  fileFlags = XMLTC(element);
  element = element->GNES();
  fileList = XMLTC(element);
  element = element->GNES();
  pieceStartTime = XMLTC(element);
  element = element->GNES();
  pieceDuration = XMLTC(element);
  element = element->GNES();
  soundSynthesis = (XMLTC(element).compare("True")==0)?true:false;
  element = element->GNES();
  numChannels = atoi(XMLTC(element).c_str());
  element = element->GNES();
  sampleRate = atoi(XMLTC(element).c_str());
  element = element->GNES();
  sampleSize = atoi(XMLTC(element).c_str());
  element = element->GNES();
  numThreads = atoi(XMLTC(element).c_str());
  element = element->GNES();
  bool outputParticel = (XMLTC(element).compare("True")==0)?true:false;
  
  //check if seed exists
  string seed;
  element = element->GNES();
  if(element->getFirstChild()){
    seed = XMLTC(element);
  }
  else{
    cout<<"Please key in the Random Seed:"<<endl;
    cin>>seed;
  }
  
  //Convert seed string to seed number and seed the random number generator
  int seedNumber = PieceHelper::getSeedNumber(seed);  
  Random::Seed((unsigned int)seedNumber);
  
  //construct the utilities object
  utilities = new Utilities(root, 
                            _workingPath, 
                            soundSynthesis, 
                            outputParticel,
                            numThreads,
                            numChannels, 
                            sampleRate, 
                            this);
  
  // setup TimeSpan and Tempo
  TimeSpan pieceSpan;
  pieceSpan.start = utilities->evaluate(pieceStartTime, NULL);
  pieceSpan.duration = utilities->evaluate(pieceDuration, NULL);
  Tempo mainTempo; //Though we supply this, "Top" will provide its own tempo.
  
  //Initialize the output class.
  if (utilities->getOutputParticel()){
    string particelFilename = _projectTitle + ".particel";
    Output::initialize(particelFilename);
    Output::beginSubLevel("Piece"); 
    Print();
  }
  
 
  //Create the Top event and recursively build its children.
  DOMElement* topElement =utilities->getEventElement(eventTop, fileList);
  Event* topEvent = new Event(topElement, 
        pieceSpan,0, mainTempo, utilities, NULL,NULL,NULL,NULL);
  topEvent->buildChildren();
  
  //get the final MultiTrack object and write it to disk
  if (soundSynthesis){
    MultiTrack* renderedScore = utilities->doneCMOD();
    string soundFilename = getNextSoundFile();
    //Write to file.
    AuWriter::write(*renderedScore, soundFilename);
    delete renderedScore;
  } else { 

		/* for score file */

     // output score to lilypond file 
     output_score(projectName);
 
    // execute lilypond to create pdf file
    system(("lilypond " + projectName + ".ly").c_str());
  } 
  
  if (outputParticel){
    //Finish particel output and free up the Output class members.
    Output::endSubLevel();
    Output::free();
  }

  cout << endl;
  cout << "-----------------------------------------------------------" <<
    endl;
  cout << "Build complete." << endl;
  cout << "-----------------------------------------------------------" <<
    endl << endl;
  cout.flush();
  
  
  //clean up
  delete utilities;
  delete parser;
  delete topEvent; //wait till the thread join
  XMLPlatformUtils::Terminate();
 
}

Piece::~Piece(){
  //do nothing
}
