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
// #include <string>










// //----------------------------------------------------------------------------//
//
// string int_to_str(int n){
//   stringstream ss;
//   ss << n;
//   return ss.str();
// }

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
  for(int i = 0; i < 1000000; i++) {
    stringstream oss;
    oss << projectName << "_" << i << ".aiff";
<<<<<<< HEAD
=======
    //cout << oss.str() << endl;
>>>>>>> 1fa4095cb02344559443154f4311c20f506d7c0f
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

  string temp = XMLTC(element);
  if(temp == "True" || temp == "False"){
    scorePrinting = (temp.compare("True")==0)?true:false;
    element = element->GNES();
  }
  numChannels = atoi(XMLTC(element).c_str());
  cout << "Channel: " << numChannels << "\n";
  element = element->GNES();

  sampleRate = atoi(XMLTC(element).c_str());
  cout << "Sample Rate: "<< sampleRate << "\n";
  element = element->GNES();

  sampleSize = atoi(XMLTC(element).c_str());
  cout << "Sample Size: "<< sampleSize << "\n";
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


  //Modify the XML file - Eperimental
  // string evName = utilities->topEventnames.at(0);
  // EventType evType = (EventType)0;
  //
  // for(int i = 0; i < 20; i++){
  //   geneticOptimization("redundancy", 34);
  // }
  //
  // cout<<"GA DONE"<<endl;

  //int newSeed = rand();
  //Random::Seed((unsigned int)newSeed);
//}

  //Create the Top event and recursively build its children.
  DOMElement* topElement = utilities->getEventElement(eventTop, fileList);
  utilities->currChild = 0;
  Event* topEvent = new Event(topElement,
        pieceSpan,0, mainTempo, utilities, NULL,NULL,NULL,NULL);
  topEvent->buildChildren();

  //get the final MultiTrack object and write it to disk
  if (soundSynthesis){
    cout << "Piece::Piece: " << "soundSynthesis " << endl;
    MultiTrack* renderedScore = utilities->doneCMOD();
    string soundFilename = getNextSoundFile();
    //Write to file.
    AuWriter::write(*renderedScore, soundFilename);
    delete renderedScore;
  }
  if (scorePrinting) {
    cout << "Piece::Piece: " << "Score output " << endl;

		/* for score file */

     // output score to lilypond file
     output_score(projectName);

    // execute lilypond to create pdf file
    system(("lilypond " + projectName + ".ly").c_str());

    // generating score files with different suffixes instead of replacing the older files.
    int suffix_rank = 0;
    int exist = 1;
    string suffix = "";
    while (exist){
      suffix = "_" + int_to_str(suffix_rank);
      std::ifstream infile(( "ScoreFiles/" + projectName + suffix + ".pdf").c_str());
      exist = infile.good();
      infile.close();
      suffix_rank++;
    }

    system(("mv " + projectName + ".pdf " + "ScoreFiles/" + projectName + suffix + ".pdf").c_str());

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

//Experiment 2
vector<DOMElement*> Piece::calcEventM(DOMElement* eventElement){

            vector<DOMElement*> childElements;
	    DOMElement* thisEventElement = eventElement->GFEC(); //type
	    string typeString = XMLTC(thisEventElement);
	    int type = atoi(typeString.c_str());
	    double mVal = 0.0;

	    thisEventElement = thisEventElement->GNES(); //name
	    string name = XMLTC(thisEventElement);

	    if(type <= 4){  //Top, High, Medium, Low, Bottom

	      thisEventElement = thisEventElement->GNES(); //maxChildDur
	      float maxChildDur = (float)utilities->evaluate(XMLTC(thisEventElement), (void*)this);

	      thisEventElement = thisEventElement->GNES(); //newEDUPerBeat
	      int newEDUPerBeat = (int) utilities->evaluate(XMLTC(thisEventElement),(void*)this);

	      thisEventElement = thisEventElement->GNES(); //Time Signature element

	      thisEventElement = thisEventElement->GNES(); //Tempo element

	      DOMElement* numChildrenElement = thisEventElement->GNES(); //Num children element

	      DOMElement* childEventDefElement = numChildrenElement->GNES();
	      DOMElement* childStartTimeElement = childEventDefElement->GFEC();
	      DOMElement* childTypeElement = childStartTimeElement->GNES();
cout << "pieceExperimental - childTypeElement=" << childTypeElement << endl;
int sever; cin >> sever;
	      DOMElement* childDurationElement = childTypeElement->GNES();
	      DOMElement* AttackSieveElement = childDurationElement->GNES();
	      DOMElement* DurationSieveElement = AttackSieveElement->GNES();
	      DOMElement* methodFlagElement = DurationSieveElement->GNES();
	      DOMElement* childStartTypeFlag = methodFlagElement->GNES();
	      DOMElement* childDurationTypeFlag = childStartTypeFlag->GNES();

	      //Read Flag values (Needed for modification)
	      string defFlag = XMLTC(methodFlagElement);
	      int definitionVal = atoi(defFlag.c_str());

	      if(definitionVal == 0){     //Only Continuum

		//Calculating start time orignality
		string startFlag = XMLTC(childStartTypeFlag);
		int startFlagVal = atoi(startFlag.c_str());

	      //layers, initialize child names
	      thisEventElement = childEventDefElement->GNES();
	      DOMElement* layerElement = thisEventElement->GFEC();
	      vector<DOMElement*> layerElements;
	      vector<DOMElement*> childTypeElements;

	      while (layerElement){

		layerElements.push_back(layerElement);
		DOMElement* childPackage = layerElement->GFEC()->GNES()->GFEC();

		while(childPackage){
		  childTypeElements.push_back(childPackage);
		  childPackage = childPackage->GNES();
		}
		layerElement = layerElement->GNES();
	      }

	      int numChildren;
	      DOMElement* flagElement = numChildrenElement->GFEC();
	      if (XMLTC(flagElement) =="0"){ // Continuum
		DOMElement* entry1Element = flagElement->GNES();
		if (XMLTC(entry1Element)==""){
		  numChildren = childTypeElements.size();
		}
		else {
		  numChildren =(int) utilities->evaluate(XMLTC(entry1Element), (void*)this);
		}
	      }
	      else if (XMLTC(flagElement) == "1"){ // Density
		DOMElement* densityElement = numChildrenElement->GFEC()->GNES();
		DOMElement* areaElement = densityElement->GNES();
		DOMElement* underOneElement = areaElement->GNES();
		double density = utilities->evaluate( XMLTC(densityElement),(void*)this);
		double area = utilities->evaluate( XMLTC(areaElement),(void*)this);
		double underOne = utilities->evaluate( XMLTC(underOneElement),(void*)this);
		double soundsPsec = pow(2, density * area - underOne); //this can't be right..

		numChildren = (int)(soundsPsec * utilities->evaluate(pieceDuration, NULL) + underOne/area);

	      }
	      else {// by layer
	      numChildren = 0;
		for (int i = 0; i < layerElements.size(); i ++){
		  numChildren +=utilities->evaluate(XMLTC(layerElements[i]->GFEC()),(void*)this);
		}
	      }

	      if(type == 4){ //Bottom
		XMLCh* extraInfoString = XMLString::transcode("ExtraInfo");
		DOMNodeList* extraInfoList = eventElement->getElementsByTagName(extraInfoString);
		DOMElement* extraInfo = (DOMElement*) extraInfoList->item(0);
		XMLString::release(&extraInfoString);

		//Frequency Entropy
		DOMElement* frequencyElement = extraInfo->GFEC();
		DOMElement* frequencyFlagElement = frequencyElement->GFEC();
		string flagNum = XMLTC(frequencyFlagElement);
		int flagVal = atoi(flagNum.c_str());

		if(flagVal == 0){ //Equal Temperament
		  mVal += EQUAL_TEMP * (log(1 / EQUAL_TEMP)/log(2));
		  DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();
	      }

	      else if(flagVal == 1){//Fundamental
		mVal += FUNDAMENTAL * (log(1 / FUNDAMENTAL)/log(2));
		DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();
		DOMElement* freqEntry2 = freqEntry1->GNES();
	      }

	      else if(flagVal == 2){//Continuum
	        mVal += CONTINUUM * (log(1 / CONTINUUM)/log(2));
		DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();
		DOMElement* continuumFlagElement = frequencyFlagElement->GNES();

		if (utilities->evaluate(XMLTC(continuumFlagElement), NULL)==0) { //Hertz
		   mVal += HZ * (log(1 / HZ)/log(2));
		  /* 3rd arg is a float (baseFreq in Hz) */
		}

		else  {
		   mVal += POW2 * (log(1 / POW2)/log(2));
		  /* 3rd arg is a float (power of 2) */
		  float step = utilities->evaluate(XMLTC(freqEntry1), (void*)this);
		  double range = log10(CEILING / MINFREQ) / log10(2.); // change log base
		  double baseFreqResult = pow(2, step * range) * MINFREQ;  // equal chance for all 8vs
		}
	      }

	      /*entropy = calculateEntropyRatio(samples, "Pow2", 0, 15000); //Freq in Hz
	      currentEntropy = utilities->eventValues[name];
	      utilities->eventValues.erase(name);
	      newEntropy = (currentEntropy + entropy); // Mean of ratios
	      utilities->eventValues.insert(std::pair<string, double> (name, newEntropy));
	      //cout<<"Entropy Ratio:"<<entropy<<endl;*/

	      printf("%f\n", mVal);

	    for(int i = 0; i < numChildren; i++){

	      double childType = utilities->evaluate(XMLTC(childTypeElement),(void*)this);
	      string childName = XMLTC(childTypeElements[childType]->GFEC());
	      EventType childEventType = (EventType) utilities->evaluate(XMLTC(childTypeElements[childType]->GFEC()->GNES()),(void*)this);

	      DOMElement* childElement = utilities->getEventElement(childEventType, childName);
	      childElements.push_back(childElement);
	  }

	   return childElements;
	  }
	}
}
}

//Experimental - For now only bottom events
void Piece::geneticOptimization(string fitnessFunction, double optimum){

  // Step 1: Calculating current Aesthetic of the piece - User decides function
  string evName = utilities->topEventnames.at(0);
  EventType evType = (EventType)0;
  utilities->currChild = 0;

  //for(int k = 0; k < 10; k++){
  DOMElement* topEvEl = utilities->getEventElement(evType, evName);
  vector<DOMElement*> children = calculateAesthetic(topEvEl);

  cout << "EventValues: " << utilities->eventValues.size() << endl;

  for(int i = 0; i < children.size(); i++){
      vector<DOMElement*> currChildren = calculateAesthetic(children[i]);

      for(int j = 0; j < currChildren.size(); j++){
        children.push_back(currChildren[j]);
      }

      cout<<"Piece::geneticOptimization: Child No: "<<i<<endl;
  }

    cout << "EventValues: " << utilities->eventValues.size() << endl;

  //(Only bottom events for now)
  double pieceAesthetic = 0.0;
  double fitnessSum = 0.0;
  int numUniqueEvents = 0;
  std::map<double, string> bottoms;
  std::vector<string> names;

  for(std::map<string, double>::iterator it = utilities->eventValues.begin(); it != utilities->eventValues.end(); ++it){
    names.push_back(it->first);
    cout<<it->first<<", "<<it->second<<endl;
    if(it->second != 0.0){
      pieceAesthetic += it->second;
      numUniqueEvents++;
      bottoms.insert(std::pair<double, string>(it->second, it->first));
      cout<< "CHOOSEN: "<<it->first<<", "<<it->second<<endl;
      fitnessSum += it->second;
    }
  }

  //pieceAesthetic /= numUniqueEvents;
  cout<<"Piece::geneticOptimization : Piece aesthetic: "<<pieceAesthetic<<endl;

//---------------------------------------------------------------------------

// Step 2: Find suitable parents
vector<string> sortedEvents;
vector<DOMElement*> parents;
vector<DOMElement*> child;
vector<double> prange;
double runningSum = 0.0;
EventType t = (EventType)4;

for(std::map<double, string>::iterator it = bottoms.begin(); it != bottoms.end(); ++it){
  sortedEvents.push_back(it->second);
  prange.push_back(runningSum + (it->first / fitnessSum));
  runningSum += it->first / fitnessSum;
}

if(pieceAesthetic < optimum){

  double randNum = Random::Rand();

  //Selecting First Parent
  for(int i = 0; i < prange.size(); i++){
    if(prange[i] >= randNum){
      cout<<"P1: "<<sortedEvents[i]<<endl;
      parents.push_back(utilities->getEventElement(t, sortedEvents[i]));
      fitnessSum -= utilities->eventValues[sortedEvents[i]];
      sortedEvents.erase(sortedEvents.begin() + i);
      runningSum = 0.0;
      prange.clear();
      break;
    }
  }

  //Recalculating Probability
  for(int i = 0; i < sortedEvents.size(); i++){
    cout<<"Piece::geneticOptimization :  Recalculating Probability "<< i <<endl;
    prange.push_back(runningSum + (utilities->eventValues[sortedEvents[i]] / fitnessSum));
    runningSum += utilities->eventValues[sortedEvents[i]] / fitnessSum;
  }

  //Selecting Second Parent
  randNum = Random::Rand();

  for(int i = 0; i < prange.size(); i++){
    if(prange[i] >= randNum){
      cout<<"P2: "<<sortedEvents[i]<<endl;
      parents.push_back(utilities->getEventElement(t, sortedEvents[i]));
      fitnessSum -= utilities->eventValues[sortedEvents[i]];
      sortedEvents.erase(sortedEvents.begin() + i);
      runningSum = 0.0;
      prange.clear();
      break;
    }
  }

  //Recalculating Probability
  for(int i = 0; i < sortedEvents.size(); i++){
    prange.push_back(runningSum + (utilities->eventValues[sortedEvents[i]] / fitnessSum));
    runningSum += utilities->eventValues[sortedEvents[i]] / fitnessSum;
  }

  //Selecting Child
  randNum = Random::Rand();
  for(int i = 0; i < prange.size(); i++){
    if(prange[i] >= randNum){
      cout<<"child: "<<sortedEvents[sortedEvents.size() - i]<<endl;
      child.push_back(utilities->getEventElement(t, sortedEvents[sortedEvents.size() - i]));
      sortedEvents.erase(sortedEvents.begin() + i);
      runningSum = 0.0;
      prange.clear();
      break;
    }
  }
}

else if(pieceAesthetic > optimum){


}

else{ //Nearly impossible for double values
  return;
}
//----------------------------------------------------------------------------

// Step 3: Crossover (For now single point crossover)
// For now, Attributes can come from either parent

crossoverMutation(parents[0], parents[1], child[0], 0.2);

//-------------------------------------------------------------------

// Step 4: Repeat Steps 1-3

//Resetting eventValues map for next iteration
utilities->eventValues.clear();
for(int i = 0; i < names.size(); i++){
  utilities->eventValues.insert(std::pair<string, double>(names[i], 0.0));
}

//geneticOptimization(fitnessFunction, optimum);


}

void Piece::crossoverMutation(DOMElement* parent1, DOMElement* parent2, DOMElement* child, double mutationProb){

  int freqParent = Random::RandInt(0, 1);
  XMLCh* extraInfoString = XMLString::transcode("ExtraInfo");
  DOMNodeList* extraInfoList;

  if(freqParent == 0){
    extraInfoList = parent1->getElementsByTagName(extraInfoString);
  }

  else{
    extraInfoList = parent2->getElementsByTagName(extraInfoString);
  }

  DOMElement* extraInfo = (DOMElement*) extraInfoList->item(0);

  DOMElement* parentFrequencyElement = extraInfo->GFEC();

  DOMNode* clone = parentFrequencyElement->cloneNode(true);

  DOMNodeList* extraInfoList2 = child->getElementsByTagName(extraInfoString);
  DOMElement* extraInfo2 = (DOMElement*) extraInfoList2->item(0);
  XMLString::release(&extraInfoString);

  /*DOMElement* frequencyFlagElement = extraInfo2->GFEC()->GFEC();
  string flagNum = XMLTC(frequencyFlagElement);
  int flagVal = atoi(flagNum.c_str());
  cout<<flagVal<<"\t";

  frequencyFlagElement = parentFrequencyElement->GFEC();
  flagNum = XMLTC(frequencyFlagElement);
  flagVal = atoi(flagNum.c_str());
  cout<<flagVal<<"\t";*/

  extraInfo2->replaceChild(clone, extraInfo2->GFEC());

  /*
  DOMElement* childFrequencyElement = extraInfo2->GFEC();
  frequencyFlagElement = childFrequencyElement->GFEC();
  flagNum = XMLTC(frequencyFlagElement);
  flagVal = atoi(flagNum.c_str());
  cout<<flagVal<<"\t";*/

  int loudParent = Random::RandInt(0, 1);
  extraInfoString = XMLString::transcode("ExtraInfo");

  if(loudParent == 0){
    extraInfoList = parent1->getElementsByTagName(extraInfoString);
  }

  else{
    extraInfoList = parent2->getElementsByTagName(extraInfoString);
  }

  extraInfo = (DOMElement*) extraInfoList->item(0);

  DOMElement* parentLoudnessElement = extraInfo->GFEC()->GNES();

  clone = parentLoudnessElement->cloneNode(true);

  extraInfoList2 = child->getElementsByTagName(extraInfoString);
  extraInfo2 = (DOMElement*) extraInfoList2->item(0);
  XMLString::release(&extraInfoString);

  extraInfo2->replaceChild(clone, extraInfo2->GFEC()->GNES());

  //Mutation - Use modifypiece method for random modification

  if(mutationProb * 10 >= Random::RandInt(1, 10)){
    cout<<"MUTATED"<<endl;
    modifyPiece(extraInfo2->GFEC()); //Mutate the new child
  }

}

//Experimental

vector<DOMElement*> Piece::modifyPiece(DOMElement* eventElement){

  //Read certain properties of this event/ Navigate the XML
  vector<DOMElement*> childElements;
  DOMElement* thisEventElement = eventElement->GFEC(); //type
  string typeString = XMLTC(thisEventElement);
  int type = atoi(typeString.c_str());

  thisEventElement = thisEventElement->GNES(); //name
  string name = XMLTC(thisEventElement);

     if(type == 4){ //Bottom
      XMLCh* extraInfoString = XMLString::transcode("ExtraInfo");
      DOMNodeList* extraInfoList = eventElement->getElementsByTagName(extraInfoString);
      DOMElement* extraInfo = (DOMElement*) extraInfoList->item(0);
      XMLString::release(&extraInfoString);

      //Modifying Frequency
      DOMElement* frequencyElement = extraInfo->GFEC();
      DOMElement* frequencyFlagElement = frequencyElement->GFEC();
      string flagNum = XMLTC(frequencyFlagElement);
      int flagVal = atoi(flagNum.c_str());

      if(flagVal == 0){
        DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();

        if(freqEntry1->GFEC() != NULL){
          DOMElement* funcElement = freqEntry1->GFEC();
          functionModifier(funcElement, 80);
        }

        else{

          string freqNum = XMLTC(freqEntry1);
          int freqVal = atoi(freqNum.c_str());
          freqVal = Random::RandInt(8, 80);

          char lval[33];
          sprintf(lval, "%d", freqVal);

          XMLCh *freq;
          freq = XMLString::transcode(lval);
          freqEntry1->getFirstChild()->setNodeValue(freq);
          XMLString::release(&freq);

      }
    }

    else if(flagVal == 1){    //Didnt change Partial Number
      DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();

      if(freqEntry1->GFEC() != NULL){
        DOMElement* funcElement = freqEntry1->GFEC();
        //functionModifier(funcElement, 100);
      }

      else{

        string freqNum = XMLTC(freqEntry1);
        int freqVal = atoi(freqNum.c_str());
        //freqVal = (freqVal + Random::RandInt(0, 10)) % 100;

        char lval[33];
        sprintf(lval, "%d", freqVal);

        XMLCh *freq;
        freq = XMLString::transcode(lval);
        freqEntry1->getFirstChild()->setNodeValue(freq);
        XMLString::release(&freq);

    }
    }

    else if(flagVal == 2){
      DOMElement* continuumFlagElement = frequencyFlagElement->GNES();
      string contflagNum = XMLTC(continuumFlagElement);
      int contflagVal = atoi(contflagNum.c_str());
      DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();

      if(freqEntry1->GFEC() != NULL){
        DOMElement* funcElement = freqEntry1->GFEC();
        if(contflagVal == 0){
         functionModifier(funcElement, 15000);
      }
        else{
         functionModifier(funcElement, 14);
        }
      }

      else{

        string freqNum = XMLTC(freqEntry1);
        int freqVal = atoi(freqNum.c_str());
        if(contflagVal == 0){
         freqVal = Random::RandInt(0, 15000);
      }
        else{
         freqVal = Random::RandInt(0, 14);
        }

        char lval[33];
        sprintf(lval, "%d", freqVal);

        XMLCh *freq;
        freq = XMLString::transcode(lval);
        freqEntry1->getFirstChild()->setNodeValue(freq);
        XMLString::release(&freq);

    }
    }

      //Modifying Loudness
      DOMElement* loudnessElement = frequencyElement->GNES();

      if(loudnessElement->GFEC() != NULL){
        DOMElement* funcElement = loudnessElement->GFEC();
        functionModifier(funcElement, 225);
      }

      else{
        //char *loudnessvalue = XMLString::transcode(loudnessElement->getFirstChild()->getNodeValue());
        //cout<<"hey:"<<loudnessvalue[0]<<loudnessvalue[1]<<loudnessvalue[2]<<endl;

        string typeString = XMLTC(loudnessElement);
        int val = atoi(typeString.c_str());
        int loudnessNum = val;
        loudnessNum = Random::RandInt(0, 225);
        //cout<<"loudness = "<< loudnessNum<<endl;

        char lval[33];
        sprintf(lval, "%d", loudnessNum);

        XMLCh *loud;
        loud = XMLString::transcode(lval);
        loudnessElement->getFirstChild()->setNodeValue(loud);
        XMLString::release(&loud);

    }

      loudnessElement->GNES(); // Spatialization element
      loudnessElement->GNES()->GNES(); // Reverb element
      loudnessElement->GNES()->GNES()->GNES(); // Filters element
      loudnessElement->GNES()->GNES()->GNES()->GNES(); // Modifiers element
    }

    /*for(int i = 0; i < numChildren; i++){

      double childType = utilities->evaluate(XMLTC(childTypeElement),(void*)this);
      string childName = XMLTC(childTypeElements[childType]->GFEC());
      EventType childEventType = (EventType) utilities->evaluate(XMLTC(childTypeElements[childType]->GFEC()->GNES()),(void*)this);

      DOMElement* childElement = utilities->getEventElement(childEventType, childName);
      childElements.push_back(childElement);

      //cout<<childName<<"!!!!!!!!!!!!!!!!!!!!!!"<<endl;

  }*/

   return childElements;
 }

 /*//Modifying Spectrum Class (Spectrum event has diff layout than others)
 else if(type == 5){
   thisEventElement = thisEventElement->GNES(); //Num Partials
   DOMElement* deviationElement = thisEventElement->GNES(); //Deviation

   //Modifying Deviation
   if(deviationElement->GFEC() != NULL){
     DOMElement* funcElement = deviationElement->GFEC();
     //functionModifier(funcElement, 1);
   }

   else{
     string typeString = XMLTC(deviationElement);
     double val = atof(typeString.c_str());
     //int devNum = (val + Random::RandInt(0, 22)) % 225;

     char lval[33];
     sprintf(lval, "%f", val);

     XMLCh *dev;
     dev = XMLString::transcode(lval);
     deviationElement->getFirstChild()->setNodeValue(dev);
     XMLString::release(&dev);
 }

 return childElements;
}*/
//}

//Experimental
void Piece::functionModifier(DOMElement* functionElement, int maxValue){ //Needs some range info

  string functionName = XMLTC(functionElement->GFEC());

  if(functionName.compare("RandomInt") == 0){

    //cout<<"RandomInt"<<endl;

    DOMElement* lowBoundElement = functionElement->GFEC()->GNES();
    DOMElement* highBoundElement = lowBoundElement->GNES();

    if(lowBoundElement->GFEC() != NULL){
      functionModifier(lowBoundElement->GFEC(), maxValue);
    }

    else{
      int lowBound = atoi(XMLTC(lowBoundElement).c_str());
      lowBound = (lowBound + Random::RandInt(0, maxValue/2)) % (maxValue/2);
      char val[33];
      sprintf(val, "%d", lowBound);
      XMLCh *low;
      low = XMLString::transcode(val);
      lowBoundElement->getFirstChild()->setNodeValue(low);
      XMLString::release(&low);

    }

    if(highBoundElement->GFEC() != NULL){
      functionModifier(highBoundElement->GFEC(), maxValue);
    }

    else{
      int highBound = atoi(XMLTC(highBoundElement).c_str());
      highBound = (highBound + Random::RandInt(0, maxValue/2)) % (maxValue/2) + maxValue/2;
      char val[33];
      sprintf(val, "%d", highBound);
      XMLCh *high;
      high = XMLString::transcode(val);
      highBoundElement->getFirstChild()->setNodeValue(high);
      XMLString::release(&high);

  }
  }

  else if (functionName.compare("Random") == 0){

    DOMElement* lowBoundElement = functionElement->GFEC()->GNES();
    DOMElement* highBoundElement = lowBoundElement->GNES();

    if(lowBoundElement->GFEC() != NULL){
      functionModifier(lowBoundElement->GFEC(), maxValue);
    }

    else{
      double lowBound = atof(XMLTC(lowBoundElement).c_str());
      lowBound = fmod((lowBound + Random::Rand(0, maxValue/2)), maxValue/2);
      char val[33];
      sprintf(val, "%f", lowBound);
      XMLCh *low;
      low = XMLString::transcode(val);
      lowBoundElement->getFirstChild()->setNodeValue(low);
      XMLString::release(&low);

    }

    if(highBoundElement->GFEC() != NULL){
      functionModifier(highBoundElement->GFEC(), maxValue);
    }

    else{
      double highBound = atof(XMLTC(highBoundElement).c_str());
      highBound = fmod((highBound + Random::Rand(0, maxValue/2)), maxValue/2) + maxValue/2;
      char val[33];
      sprintf(val, "%f", highBound);
      XMLCh *high;
      high = XMLString::transcode(val);
      highBoundElement->getFirstChild()->setNodeValue(high);
      XMLString::release(&high);

  }

  }

  else if (functionName.compare("Select") == 0){
    DOMElement* listElement = functionElement->GFEC()->GNES();
    DOMElement* indexElement = listElement->GNES();

    std::vector<std::string> list = utilities->listElementToStringVector(listElement);

    if(indexElement->GFEC() != NULL){
      functionModifier(indexElement->GFEC(), list.size() - 1);
    }

    else{
      int indexVal = atoi(XMLTC(indexElement).c_str());
      indexVal = (indexVal + 1)%(list.size() - 1);
      char val[33];
      sprintf(val, "%d", indexVal);
      XMLCh *index;
      index = XMLString::transcode(val);
      indexElement->getFirstChild()->setNodeValue(index);
      XMLString::release(&index);
    }
  }

  else if (functionName.compare("GetPattern") == 0){
    //Do Later

  }
  else if (functionName.compare("Randomizer") == 0){

    DOMElement* baseValElement = functionElement->GFEC()->GNES();
    DOMElement* percDevElement = baseValElement->GNES();

    if(baseValElement->GFEC() != NULL){
      functionModifier(baseValElement->GFEC(), maxValue);
    }

    else{
      double baseVal = atof(XMLTC(baseValElement).c_str());
      baseVal = fmod(baseVal + Random::Rand(0, baseVal/10), maxValue/2);
      char val[33];
      sprintf(val, "%f", baseVal);
      XMLCh *base;
      base = XMLString::transcode(val);
      baseValElement->getFirstChild()->setNodeValue(base);
      XMLString::release(&base);

    }

    if(percDevElement->GFEC() != NULL){
      functionModifier(percDevElement->GFEC(), maxValue);
    }

    else{
      double percDev = atof(XMLTC(percDevElement).c_str());
      percDev = fmod((percDev + Random::Rand(0, percDev/10)), 1);
      char val[33];
      sprintf(val, "%f", percDev);
      XMLCh *dev;
      dev = XMLString::transcode(val);
      percDevElement->getFirstChild()->setNodeValue(dev);
      XMLString::release(&dev);

   }

  }
  else if (functionName.compare("RandomDensity") == 0) {

    DOMElement* envelopeNumberElement = functionElement->GFEC()->GNES();
    DOMElement* lowBoundElement = envelopeNumberElement->GNES();
    DOMElement* highBoundElement = lowBoundElement->GNES();

    if(lowBoundElement->GFEC() != NULL){
      functionModifier(lowBoundElement->GFEC(), maxValue);
    }

    else{
      double lowBound = atof(XMLTC(lowBoundElement).c_str());
      lowBound = fmod((lowBound + Random::Rand(0, maxValue/2)), maxValue/2);
      char val[33];
      sprintf(val, "%f", lowBound);
      XMLCh *low;
      low = XMLString::transcode(val);
      lowBoundElement->getFirstChild()->setNodeValue(low);
      XMLString::release(&low);

    }

    if(highBoundElement->GFEC() != NULL){
      functionModifier(highBoundElement->GFEC(), maxValue);
    }

    else{
      double highBound = atof(XMLTC(highBoundElement).c_str());
      highBound = fmod((highBound + Random::Rand(0, maxValue/2)), maxValue/2) + maxValue/2;
      char val[33];
      sprintf(val, "%f", highBound);
      XMLCh *high;
      high = XMLString::transcode(val);
      highBoundElement->getFirstChild()->setNodeValue(high);
      XMLString::release(&high);

  }

  }
  else if (functionName.compare("ChooseL") == 0){
    //Do Later

  }

  else if (functionName.compare("ValuePick") == 0){
    //Do Later

  }

  else if (functionName.compare("Stochos") == 0){

  }
  else if (functionName.compare("Decay") == 0){

  }

  else if (functionName.compare("Fibonacci") == 0){

  }

  else if (functionName.compare("LN") == 0){

  }

  else if (functionName.compare("Inverse") == 0){

  }
}

  vector<DOMElement*> Piece::calculateAesthetic(DOMElement* eventElement){

    vector<DOMElement*> childElements;
    const int NUM_SAMPLES = 100;
    std::vector<double> samples;
    double entropy = 0.0;
    double currentEntropy, newEntropy;
    DOMElement* thisEventElement = eventElement->GFEC(); //type
    string typeString = XMLTC(thisEventElement);
    int type = atoi(typeString.c_str());

    thisEventElement = thisEventElement->GNES(); //name
    string name = XMLTC(thisEventElement);

    if(type <= 4){  //Top, High, Medium, Low, Bottom

      thisEventElement = thisEventElement->GNES(); //maxChildDur
      float maxChildDur = (float)utilities->evaluate(XMLTC(thisEventElement), (void*)this);

      thisEventElement = thisEventElement->GNES(); //newEDUPerBeat
      int newEDUPerBeat = (int) utilities->evaluate(XMLTC(thisEventElement),(void*)this);

      thisEventElement = thisEventElement->GNES(); //Time Signature element

      thisEventElement = thisEventElement->GNES(); //Tempo element

      DOMElement* numChildrenElement = thisEventElement->GNES(); //Num children element

      DOMElement* childEventDefElement = numChildrenElement->GNES();
      DOMElement* childStartTimeElement = childEventDefElement->GFEC();
      DOMElement* childTypeElement = childStartTimeElement->GNES();
      DOMElement* childDurationElement = childTypeElement->GNES();
      DOMElement* AttackSieveElement = childDurationElement->GNES();
      DOMElement* DurationSieveElement = AttackSieveElement->GNES();
      DOMElement* methodFlagElement = DurationSieveElement->GNES();
      DOMElement* childStartTypeFlag = methodFlagElement->GNES();
      DOMElement* childDurationTypeFlag = childStartTypeFlag->GNES();

      //Read Flag values (Needed for modification)
      string defFlag = XMLTC(methodFlagElement);
      int definitionVal = atoi(defFlag.c_str());

      if(definitionVal == 0){     //Only Continuum

        //Calculating start time orignality
        string startFlag = XMLTC(childStartTypeFlag);
        int startFlagVal = atoi(startFlag.c_str());


        /*//Calculating Duration entropy

        samples.clear();
        string durationFlag = XMLTC(childDurationTypeFlag);
        int durationFlagVal = atoi(durationFlag.c_str());
        //float durationInSec = 0.0;

        for(int i = 0; i < NUM_SAMPLES; i++){
          double rawChildDuration = utilities->evaluate(XMLTC(childDurationElement),(void*)this);

          // pre-quantize the duration in case "EDU" is used
          int rawChildDurationInt = (int)rawChildDuration;
          int maxChildDurInt = (int)maxChildDur;
          if(rawChildDurationInt > maxChildDurInt)
              rawChildDurationInt = maxChildDurInt;

          // how to process duration: EDU, SECONDS or PERCENTAGE
          if (durationFlagVal == 1) {//EDU (Not sure how to do this)
            //durationInSec = // convert to seconds
              //(float)rawChildDurationInt * tempo.getEDUDurationInSeconds().To<float>();

          } else if (durationFlagVal == 2) {//seconds
            samples.push_back(rawChildDuration);

          } else if (durationFlagVal == 0) {//fraction

          }
        }

        entropy = calculateEntropyRatio(samples, "Unit", 0.0, atoi(pieceDuration.c_str())); //duration
        currentEntropy = utilities->eventValues[name];
        utilities->eventValues.erase(name);
        newEntropy = (currentEntropy + entropy)/2.0; // Mean of ratios
        utilities->eventValues.insert(std::pair<string, double> (name, newEntropy));
        //cout<<"Entropy Ratio:"<<entropy<<endl;*/

      //layers, initialize child names
      thisEventElement = childEventDefElement->GNES();
      DOMElement* layerElement = thisEventElement->GFEC();
      vector<DOMElement*> layerElements;
      vector<DOMElement*> childTypeElements;

      while (layerElement){

        layerElements.push_back(layerElement);
        DOMElement* childPackage = layerElement->GFEC()->GNES()->GFEC();

        while(childPackage){
          childTypeElements.push_back(childPackage);
          childPackage = childPackage->GNES();
        }
        layerElement = layerElement->GNES();
      }

      int numChildren;
      DOMElement* flagElement = numChildrenElement->GFEC();
      if (XMLTC(flagElement) =="0"){ // Continuum
        DOMElement* entry1Element = flagElement->GNES();
        if (XMLTC(entry1Element)==""){
          numChildren = childTypeElements.size();
        }
        else {
          numChildren =(int) utilities->evaluate(XMLTC(entry1Element), (void*)this);
        }
      }
      else if (XMLTC(flagElement) == "1"){ // Density
        DOMElement* densityElement = numChildrenElement->GFEC()->GNES();
        DOMElement* areaElement = densityElement->GNES();
        DOMElement* underOneElement = areaElement->GNES();
        double density = utilities->evaluate( XMLTC(densityElement),(void*)this);
        double area = utilities->evaluate( XMLTC(areaElement),(void*)this);
        double underOne = utilities->evaluate( XMLTC(underOneElement),(void*)this);
        double soundsPsec = pow(2, density * area - underOne); //this can't be right..

        numChildren = (int)(soundsPsec * utilities->evaluate(pieceDuration, NULL) + underOne/area);

      }
      else {// by layer
      numChildren = 0;
        for (int i = 0; i < layerElements.size(); i ++){
          numChildren +=utilities->evaluate(XMLTC(layerElements[i]->GFEC()),(void*)this);
        }
      }

      if(type == 4){ //Bottom
        XMLCh* extraInfoString = XMLString::transcode("ExtraInfo");
        DOMNodeList* extraInfoList = eventElement->getElementsByTagName(extraInfoString);
        DOMElement* extraInfo = (DOMElement*) extraInfoList->item(0);
        XMLString::release(&extraInfoString);

        //Frequency Entropy
        samples.clear();
        DOMElement* frequencyElement = extraInfo->GFEC();
        DOMElement* frequencyFlagElement = frequencyElement->GFEC();
        string flagNum = XMLTC(frequencyFlagElement);
        int flagVal = atoi(flagNum.c_str());

        if(flagVal == 0){
          DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();

          for(int i = 0; i < NUM_SAMPLES; i++){
            double wellTempPitch = utilities->evaluate(XMLTC(freqEntry1), (void*)this);
            double baseFreqResult = C0 * pow(WELL_TEMP_INCR, wellTempPitch);
            samples.push_back(baseFreqResult);
          }
      }

      else if(flagVal == 1){
        DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();
        DOMElement* freqEntry2 = freqEntry1->GNES();

        for(int i = 0; i < NUM_SAMPLES; i++){
          float fund_freq = utilities->evaluate(XMLTC(freqEntry1), (void*)this);
          int overtone_step = utilities->evaluate(XMLTC(freqEntry2), (void*)this);
          double baseFreqResult = fund_freq * overtone_step;
          samples.push_back(baseFreqResult);
        }
      }

      else if(flagVal == 2){
        DOMElement* freqEntry1 = frequencyFlagElement->GNES()->GNES();
        DOMElement* continuumFlagElement = frequencyFlagElement->GNES();

        for(int i = 0; i < NUM_SAMPLES; i++){
        if (utilities->evaluate(XMLTC(continuumFlagElement), NULL)==0) { //Hertz
          samples.push_back(utilities->evaluate(XMLTC(freqEntry1), (void*)this));
          /* 3rd arg is a float (baseFreq in Hz) */
        }
        else  {
          /* 3rd arg is a float (power of 2) */
          float step = utilities->evaluate(XMLTC(freqEntry1), (void*)this);
          double range = log10(CEILING / MINFREQ) / log10(2.); // change log base
          double baseFreqResult = pow(2, step * range) * MINFREQ;  // equal chance for all 8vs
          samples.push_back(baseFreqResult);
        }
      }
      }

      entropy = calculateEntropyRatio(samples, "Pow2", 0, 15000); //Freq in Hz
      currentEntropy = utilities->eventValues[name];
      utilities->eventValues.erase(name);
      newEntropy = (currentEntropy + entropy); // Mean of ratios
      utilities->eventValues.insert(std::pair<string, double> (name, newEntropy));
      //cout<<"Entropy Ratio:"<<entropy<<endl;

      //LOUDNESS Entropy

      DOMElement *loudnessElement = frequencyElement->GNES();
      samples.clear();

      for(int i = 0; i < NUM_SAMPLES;){
        double loudval = utilities->evaluate(XMLTC(loudnessElement), (void*)this);
        if(loudval >= 1.0){
          samples.push_back(loudval);
          i++;
        }
      }

      entropy = calculateEntropyRatio(samples, "Pow2", 0, 256); //Loudness
      currentEntropy = utilities->eventValues[name];
      utilities->eventValues.erase(name);
      newEntropy = (currentEntropy + entropy); // Mean of ratios
      utilities->eventValues.insert(std::pair<string, double> (name, newEntropy));
      //cout<<"Entropy Ratio:"<<entropy<<endl;
    }

    for(int i = 0; i < numChildren; i++){

      double childType = utilities->evaluate(XMLTC(childTypeElement),(void*)this);
      string childName = XMLTC(childTypeElements[childType]->GFEC());
      EventType childEventType = (EventType) utilities->evaluate(XMLTC(childTypeElements[childType]->GFEC()->GNES()),(void*)this);

      DOMElement* childElement = utilities->getEventElement(childEventType, childName);
      childElements.push_back(childElement);
  }

   return childElements;
  }
}
}

  double Piece::calculateEntropyRatio(vector<double> sampleData, string partitionMethod, double min, double max){

    if(min == 0 && partitionMethod.compare("Pow2") == 0){
      min++; //For log to work
    }
    int numPartitions = 100; //Arbitrary
    vector<double> probs(numPartitions, 0.0);
    double shannonEntropy = 0.0, maxEntropy, redundancy;

    std::sort(sampleData.begin(), sampleData.end());

    double sampleRange = sampleData[sampleData.size() - 1] - sampleData[0];

    for(int i = 0; i < sampleData.size(); i++){
      if(partitionMethod.compare("Pow2") == 0){
        probs[(int)(log(sampleData[i])/log(2))] += 1.0/sampleData.size();
      }
      else if(partitionMethod.compare("Unit") == 0){
        probs[(int)sampleData[i]] += 1.0/sampleData.size();
      }
    }

    for(int i = 0; i < probs.size(); i++){

      if(probs[i] > 0.0){
      shannonEntropy += -1 * probs[i] * log(probs[i])/log(2);
    }
  }

  if(shannonEntropy < 0.0){     // C++ log stuff error?
    shannonEntropy = 0.0;
  }

  //cout<<"Shannon: "<<shannonEntropy;

  if(partitionMethod.compare("Pow2") == 0){

    numPartitions = ceil(log(max)/log(2)) - floor(log(min)/log(2));
  }

  else if(partitionMethod.compare("Unit") == 0){

    numPartitions = max - min;
  }

  maxEntropy = log(numPartitions)/log(2);
  //redundancy = 1 - (shannonEntropy/maxEntropy);
  redundancy = maxEntropy - shannonEntropy;
  double relativeShannonEntropy = shannonEntropy/maxEntropy;
  double benseOriginality = relativeShannonEntropy/redundancy;
  cout<<shannonEntropy<<endl;

  return redundancy;
  }
