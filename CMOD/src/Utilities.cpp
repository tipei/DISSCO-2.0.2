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

//----------------------------------------------------------------------------//
//
//  Utilities.h
//  Created by Ming-ching Chiu 2012-2013
//
//  The Utilities object is designed to evaluate the XML strings to their
//  proper format, whether an Event, a number, an Envelope or other objects such
//  as Pattern, Sieve, etc.
//
//  The Utilities is also the interface between the CMOD Event and LASS Score.
//  CMOD Events add the Sound/Note objects they produce to LASS Score through
//  The Utilities.
//
//  Maintained by Fanbo Xiang 2018
//----------------------------------------------------------------------------//
#include "Utilities.h"
#include "Random.h"
#include "Event.h"
#include "Piece.h"
#include "Patter.h"
#include "../../LASS/src/ProbabilityEnvelope.h" // consider moving this into LASS.h
#include <string>

Utilities::Utilities(DOMElement* root,
                     string _workingPath,
                     bool _soundSynthesis,
                     bool _outputParticel,
                     int _numThreads,
                     int _numChannels,
                     int _samplingRate,
                     Piece* _piece):
  soundSynthesis(_soundSynthesis),
  outputParticel(_outputParticel),
  numThreads(_numThreads),
  samplingRate(_samplingRate),
  piece(_piece),
  numChannels(_numChannels){

  // New LASS Score
  if (soundSynthesis){
    score = new Score (numThreads,  numChannels, _samplingRate );
    score->setClippingManagementMode(Score::CHANNEL_ANTICLIP);
  }
  else {
    score = NULL;
  }


  // Construct Envelope library
  DOMElement* envelopeLibraryElement = root->GFEC()->GNES()->GNES();
  string envLibContent = XMLTranscode(envelopeLibraryElement);
  string fileString = "lib.temp";
  FILE* file  = fopen(fileString.c_str(), "w");
  fputs (envLibContent.c_str(), file);
  fclose(file);

  envelopeLibrary = new EnvelopeLibrary();
  envelopeLibrary->loadLibraryNewFormat((char*)fileString.c_str());
  string deleteCommand = "rm " + fileString;
  system(deleteCommand.c_str());

  // Construct Markov Model Library
  DOMElement* markovModelLibraryElement = envelopeLibraryElement->GNES();
  char* text = XMLString::transcode(markovModelLibraryElement->getTagName());
  string tagName = text;
  XMLString::release(&text);
  if (tagName != "MarkovModelLibrary") {
    cout << "Project is outdated, please save the project in the latest version of DISSCO" << endl;
    exit(1);
  }
  string data = XMLTC(markovModelLibraryElement);
  std::stringstream ss(data);
  int size;
  ss >> size;
  markovModelLibrary.resize(size);
  string modelText, line;
  getline(ss, line, '\n');
  for (int i = 0; i < size; i++) {
    getline(ss, line, '\n');
    modelText = line + '\n';
    getline(ss, line, '\n');
    modelText += line + '\n';
    getline(ss, line, '\n');
    modelText += line + '\n';
    getline(ss, line, '\n');
    modelText += line;
    markovModelLibrary[i].from_str(modelText);
    markovModelLibrary[i].normalize();
  }


  //events and other objects

  DOMElement* eventElements = markovModelLibraryElement->GNES();
  DOMElement* thisEventElement = eventElements->GFEC();
  //Counters to assign numbers to the events. Experimental

   int topCounter = 0;
   int highCounter = 0;
   int midCounter = 0;
   int lowCounter = 0;
   int bottomCounter = 0;
   int spectrumCounter = 0;
   int envelopeCounter = 0;
   int sieveCounter = 0;
   int spatializationCounter = 0;
   int patternCounter = 0;
   int reverbCounter = 0;
   int filterCounter = 0;
   int notesCounter = 0;

   //put the pointer of events and objects into the proper map
   while(thisEventElement){
     int type = atoi(XMLTranscode(thisEventElement->GFEC()).c_str());
     string eventName=  XMLTranscode(thisEventElement->GFEC()->GNES());

     switch (type){
       case 0:
         topEventElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

         topEventnames.insert(
               pair<int, string>(topCounter, eventName));

         eventValues.insert(
               pair<string, double>(eventName, 0.0));

         break;
       case 1:
         highEventElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

         highEventnames.insert(
               pair<int, string>(highCounter, eventName));

         eventValues.insert(
               pair<string, double>(eventName, 0.0));

         break;
       case 2:
         midEventElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               midEventnames.insert(
                     pair<int, string>(midCounter, eventName));

                     eventValues.insert(
                           pair<string, double>(eventName, 0.0));

         break;
       case 3:
         lowEventElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               lowEventnames.insert(
                     pair<int, string>(lowCounter, eventName));

                     eventValues.insert(
                           pair<string, double>(eventName, 0.0));

         break;
       case 4:
         bottomEventElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               bottomEventnames.insert(
                     pair<int, string>(bottomCounter, eventName));

                     eventValues.insert(
                           pair<string, double>(eventName, 0.0));

         break;
       case 5:
         spectrumElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               spectrumEventnames.insert(
                     pair<int, string>(spectrumCounter, eventName));

                     eventValues.insert(
                           pair<string, double>(eventName, 0.0));

         break;
       case 6:
         envelopeElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               envelopeEventnames.insert(
                     pair<int, string>(envelopeCounter, eventName));
         break;
       case 7:
         sieveElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               sieveEventnames.insert(
                     pair<int, string>(sieveCounter, eventName));
         break;
       case 8:
         spatializationElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               spatializationEventnames.insert(
                     pair<int, string>(spatializationCounter, eventName));
         break;
       case 9:
         patternElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               patternEventnames.insert(
                     pair<int, string>(patternCounter, eventName));
         break;
       case 10:
         reverbElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               reverbEventnames.insert(
                     pair<int, string>(reverbCounter, eventName));
         break;
       case 12:
         notesElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               notesEventnames.insert(
                     pair<int, string>(notesCounter, eventName));
         break;
       case 13:
         filterElements.insert(
               pair<string, DOMElement*>(eventName, thisEventElement));

               filterEventnames.insert(
                     pair<int, string>(filterCounter, eventName));
         break;
     }
     thisEventElement = thisEventElement->GNES();
   }
 }



//----------------------------------------------------------------------------//

Utilities::~Utilities(){
  if (score != NULL){
    delete score;
  }
  delete envelopeLibrary;
}


//----------------------------------------------------------------------------//

DOMElement* Utilities::getEventElement(EventType _type, string _eventName){
  map<string, DOMElement*>::iterator it;

  switch((int)_type){
    case 0:
      it = topEventElements.find(_eventName);
      break;
    case 1:
      it = highEventElements.find(_eventName);
      break;
    case 2:
      it = midEventElements.find(_eventName);
      break;
    case 3:
      it = lowEventElements.find(_eventName);
      break;
    case 4:
      it = bottomEventElements.find(_eventName);
      break;
    case 5:
      it = spectrumElements.find(_eventName);
      break;
    case 6:
      it = envelopeElements.find(_eventName);
      break;
    case 7:
      it = sieveElements.find(_eventName);
      break;
    case 8:
      it = spatializationElements.find(_eventName);
      break;
    case 9:
      it = patternElements.find(_eventName);
      break;
    case 10:
      it = reverbElements.find(_eventName);
      break;
    case 12:
      it = notesElements.find(_eventName);
      break;
    case 13:
      it = filterElements.find(_eventName);
      break;
  }
  return it->second;
}


//----------------------------------------------------------------------------//

string Utilities::XMLTranscode(DOMElement* _thisFunctionElement){
  // handle empty string
  if (_thisFunctionElement->getFirstChild() == NULL){
    return "";
  }

  //flatten the element to a string
  XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
  XMLCh* bla = theSerializer->writeToString (_thisFunctionElement);
  char* toTranscode = XMLString::transcode(bla);
  string returnString = string (toTranscode);
  XMLString::release(&toTranscode);
  XMLString::release(&bla);
  delete theSerializer;

  //remove the tag
  int tagLength = (int) XMLString::stringLen(_thisFunctionElement->getTagName());
  returnString = returnString.substr(tagLength+2, returnString.size() - tagLength * 2 - 5);

  return returnString;
}


//----------------------------------------------------------------------------//

Sieve* Utilities::evaluateSieve(std::string _input, void* _object){
  return evaluateSieveFunction(_input, _object);
}


//----------------------------------------------------------------------------//

double Utilities::evaluate(std::string _input, void* _object){
  if (_input =="") return 0;

  string workingString = _input;
  // Test if there is any function in this string (look for <Fun>), if so,
  // replace the function with the evaluated number. Repeat until all the
  // functions are replaced by numbers.
  size_t locOfFun = workingString.find("<Fun>");
  int functionStringLength;
  while ( locOfFun!=string::npos){
    size_t locOfEndFun = findTheEndOfFirstFunction (workingString);
    functionStringLength = ((int) locOfEndFun) + 6 - ((int) locOfFun);//6 is the length of "</fun>"
    string functionString = workingString.substr(locOfFun, functionStringLength);
    string evaluatedFunction = evaluateFunction( functionString, _object);
    string front = workingString.substr(0, locOfFun);
    string back = workingString.substr(((int)locOfEndFun) +6);
    workingString = front + evaluatedFunction + back;

    // look for next function
    locOfFun = workingString.find("<Fun>");
  }

  // evaluate the expression to the final result
  mu::Parser p;
  p.SetExpr(workingString);
  double result;

  try {
    result = p.Eval();
  }
  catch (mu::ParserError){
    cerr<<"Oooops, we find a typo in your project."<<endl;
    if (_object != NULL){
        cerr<<"The string we see is located in Event (or Bottom): "<<
          ((Event*)_object)->getEventName()<<endl;
    }
    cerr<<"The string we see is: "<< _input<<endl;
    exit(1);
  }

  return result;
}

//----------------------------------------------------------------------------//

bool Utilities::isSieveFunction(DOMElement* input)
{
  return (   (input = input->GFEC())
          && (input = input->GFEC())
          && (XMLTC(input) == "ChooseL" || XMLTC(input) == "ValuePick"));
}

//----------------------------------------------------------------------------//

void* Utilities::evaluateObject(string _input,
                                void* _object,
                                EventType _returnType){

  //remove any spaces
  string input =  removeSpaces( _input);

  // call the proper method
  if (_returnType == eventEnv){
    return getEnvelope(input, _object);
  }
  else if (_returnType == eventSpa){
    return (void*) getSPAFunctionElement( _object);
  }
  else if (_returnType == eventRev){
    return (void*) getREVFunctionElement( _object);
  }
  else if (_returnType ==eventPat){
    return (void*) getPattern( input, _object);
  }
  else if (_returnType ==eventSiv){
    return (void*) getSieve( input, _object);
  }
  else if (_returnType ==eventFil){
    return (void*) getFILFunctionElement( _object);
  }
}


//----------------------------------------------------------------------------//

std::vector<std::string> Utilities::listElementToStringVector(
            DOMElement* _listElement){
  std::vector<std::string> list;
  string listString = XMLTranscode(_listElement);
  bool doneProcessingListString = false;
  while (!doneProcessingListString){
    //  Step 1: Isolate the first element of the list and put its string
    //          representation in the vector.
    bool listElementLocated = false;
    size_t locationOfComma = listString.find(","); //this can be npos
    size_t locationOfFun = listString.find("<Fun>"); //this can be npos
    while (!listElementLocated){

      if (locationOfComma == string::npos){ //reach the end of the string
        listElementLocated =true;
      }
      else if (locationOfFun==string::npos || locationOfFun> locationOfComma){
        listElementLocated = true;
      }
      else { //find the end of this fun, and find comma and fun again
        size_t endOfThisFun =Utilities::findTheEndOfFirstFunction(listString.substr(locationOfFun)) + locationOfFun;

        locationOfComma = listString.find(",", endOfThisFun);
        locationOfFun = listString.find("<Fun>",endOfThisFun);
      }

    } // end of inner while

    //  Step 2: Cut the string from the listString
    if (locationOfComma ==string::npos){
      list.push_back(listString);
    }
    else {
      string thisListElement = listString.substr(0, locationOfComma);
      list.push_back(thisListElement);
      listString = listString.substr(locationOfComma +1);
    }
    //  Step 3: check if the listString is empty
    if (locationOfComma ==string::npos){
      doneProcessingListString = true;
    }
  }

  return list;
}


//----------------------------------------------------------------------------//

//this function assume that at least 1 "<Fun>" exists;
size_t Utilities::findTheEndOfFirstFunction(string _input){

  int depth = 1; // The depth of nested functions

  // When seeing a <Fun>, depth ++, seeing a </Fun>, depth --
  // When depth = 0, the outermost function has ended.
  size_t location = _input.find("<Fun>");
  while (depth!=0){
    size_t nextFun = _input.find("<Fun>", location + 1);
    size_t nextEndFun = _input.find("</Fun>", location + 1);

    if (nextFun < nextEndFun){
      depth ++;
      location = nextFun;
    }
    else {
      depth --;
      location = nextEndFun;
    }
  }
  return location;
}

//----------------------------------------------------------------------------//

void Utilities::addSound(Sound* _sound){
  if (!soundSynthesis){
    delete _sound;
    return;
  }
  else {
    score->add(_sound);
  }
}

//----------------------------------------------------------------------------//

MultiTrack* Utilities::doneCMOD(){
  if (score != NULL){
   return score->doneAddingSounds();
  }
  else return NULL;
}


//----------------------------------------------------------------------------//

string Utilities::removeSpaces(string _originalString){
  string input = _originalString;
  size_t index = 0;

  // Iterate through each instance of " " and remove
  while (true) {
    index = input.find(" ", index);
    if (index == string::npos) break;
    input.replace(index, 1, "");
    index ++;
  }
  return input;
}


//----------------------------------------------------------------------------//

string Utilities::evaluateFunction(string _functionString,void* _object){
  // convert the function string to a DOMElement
  XercesDOMParser* parser = new XercesDOMParser();
  xercesc::MemBufInputSource myxml_buf  (
            (const XMLByte*)_functionString.c_str(),
            _functionString.size(),
            "function (in memory)");

  parser->parse(myxml_buf);
  DOMDocument* xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();

  char* functionNameChars;
  char* charBuffer;
  DOMCharacterData* textData;
  string functionName;
  DOMElement* thisElement;
  string resultString = "";

  DOMElement* functionNameElement = root->GFEC();
  textData = ( DOMCharacterData*) functionNameElement->getFirstChild();

  if (textData){
    functionNameChars = XMLString::transcode(textData->getData());
    functionName = string(functionNameChars);
    XMLString::release(&functionNameChars);
  }

  // check the function name and call the proper method for evaluation
  if(functionName.compare("RandomInt")==0){
    resultString = function_RandomInt(root, _object);
  }

  else if (functionName.compare("Random")==0){
    resultString = function_Random(root, _object);
  }

  else if (functionName.compare("Select")==0){
    resultString = function_Select(root, _object);
  }

  else if (functionName.compare("GetPattern")==0){
    resultString = function_GetPattern(root, _object);
  }
  else if (functionName.compare("Markov") == 0) {
    resultString = function_Markov(root, _object);
  }

  else if (functionName.compare("Randomizer")==0){
    resultString = function_Randomizer(root, _object);
  }
  else if (functionName.compare("RandomDensity") == 0) {
    resultString = function_RandomDensity(root, _object);
  }
  else if (functionName.compare("ChooseL")==0){
    resultString = function_ChooseL(root, _object);
  }

  else if (functionName.compare("ValuePick")==0){
    resultString = function_ValuePick(root, _object);
  }

  else if (functionName.compare("Stochos")==0){
    resultString = function_Stochos(root, _object);
  }
  else if (functionName.compare("Decay")==0){
    resultString = function_Decay(root, _object);
  }

  else if (functionName.compare("Fibonacci")==0){
    resultString = function_Fibonacci(root, _object);
  }

  else if (functionName.compare("LN")==0){
    resultString = function_LN(root, _object);
  }
   else if (functionName.compare("Inverse")==0){
    resultString = function_Inverse(root, _object);
  }

  // static functions
  else if (functionName.compare("CURRENT_TYPE")==0){
    resultString = static_function_CURRENT_TYPE( _object);
  }

  else if (functionName.compare("CURRENT_CHILD_NUM")==0){
    resultString = static_function_CURRENT_CHILD_NUM( _object);
  }

  else if (functionName.compare("CURRENT_PARTIAL_NUM")==0){
    resultString = static_function_CURRENT_PARTIAL_NUM( _object);
  }
  else if (functionName.compare("CURRENT_DENSITY")==0){
    resultString = static_function_CURRENT_DENSITY( _object);
  }
  else if (functionName.compare("CURRENT_SEGMENT")==0){
    resultString = static_function_CURRENT_SEGMENT( _object);
  }
  else if (functionName.compare("AVAILABLE_EDU")==0){
    resultString = static_function_AVAILABLE_EDU( _object);
  }
  else if (functionName.compare("CURRENT_LAYER")==0){
    resultString = static_function_CURRENT_LAYER( _object);
  }
  else if (functionName.compare("PREVIOUS_CHILD_DURATION")==0){
    resultString = static_function_PREVIOUS_CHILD_DURATION( _object);
  }

  delete parser;
  return resultString;
}


//---------------------------------------------------------------------------//
Sieve* Utilities::evaluateSieveFunction(string _functionString,void* _object){
  // convert the function string to a DOMElement
  XercesDOMParser* parser = new XercesDOMParser();
  xercesc::MemBufInputSource myxml_buf  (
            (const XMLByte*)_functionString.c_str(),
            _functionString.size(),
            "function (in memory)");

  parser->parse(myxml_buf);
  DOMDocument* xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();

  char* functionNameChars;
  char* charBuffer;
  DOMCharacterData* textData;
  string functionName;
  DOMElement* thisElement;
  Sieve* resultSieve;

  DOMElement* functionNameElement = root->GFEC();
  textData = ( DOMCharacterData*) functionNameElement->getFirstChild();

  if (textData){
    functionNameChars = XMLString::transcode(textData->getData());
    functionName = string(functionNameChars);
    XMLString::release(&functionNameChars);
  }

  if (functionName.compare("ChooseL")==0){
    resultSieve = sieve_ChooseL(root, _object);
  }
  else if (functionName.compare("ValuePick")==0){
    resultSieve = sieve_ValuePick(root, _object);
  }

  delete parser;
  return resultSieve;
}

//----------------------------------------------------------------------------//

string Utilities::static_function_CURRENT_TYPE(void* _object){
  if (_object !=NULL){
    double resultNum = ((Event*)_object)->getCurrentChildType();
    char result [50];
    sprintf(result, "%f",  resultNum);
    return string(result);
  }
  else {
    cerr<<"Utilities:Warning! static_function_CURRENT_TYPE has no object to look up."<<endl;
    return "0";
  }
}

//----------------------------------------------------------------------------//

string Utilities::static_function_CURRENT_CHILD_NUM(void* _object){
  if (_object !=NULL){
    double resultNum = ((Event*)_object)->getCurrentChild();
    char result [50];
    sprintf(result, "%f",  resultNum);
    return string(result);
  }
  else {
    cerr<<"Utilities:Warning! static_function_CURRENT_NUM has no object to look up."<<endl;
    return "0";
  }
}

//----------------------------------------------------------------------------//

string Utilities::static_function_CURRENT_PARTIAL_NUM(void* _object){
  if (_object !=NULL){
    double resultNum = ((Bottom*)_object)->getCurrPartialNum();
    char result [50];
    sprintf(result, "%f",  resultNum);
    return string(result);
  }
  else {
    cerr<<"Utilities:Warning! static_function_CURRENT_PARTIAL_NUM has no object to look up."<<endl;
    return "0";
  }
}

//----------------------------------------------------------------------------//

string Utilities::static_function_CURRENT_DENSITY(void* _object){
  cout<<"Utilities:Warning! static_function_CURRENT_DENSITY is not implemented in CMOD 2.0 yet."<<endl;
}

//----------------------------------------------------------------------------//

string Utilities::static_function_CURRENT_SEGMENT(void* _object){
    //should recieve an envelope as the _object? --Ming-ching May 07 2013
    cout<<"Utilities:Warning! static_function_CURRENT_SEGMENT is not implemented in CMOD 2.0 yet."<<endl;
}

//----------------------------------------------------------------------------//

string Utilities::static_function_AVAILABLE_EDU(void* _object){
  if (_object !=NULL){
    double resultNum = ((Event*)_object)->getAvailableEDU();
    char result [50];
    sprintf(result, "%f",  resultNum);
    return string(result);
  }
  else {
    cerr<<"Utilities:Warning! static_function_AVAILABLE_EDU has no object to look up."<<endl;
    return "0";
  }
}

//----------------------------------------------------------------------------//

string Utilities::static_function_CURRENT_LAYER(void* _object){
  if (_object !=NULL){
    double resultNum = ((Event*)_object)->getCurrentLayer();
    char result [50];
    sprintf(result, "%f",  resultNum);
    return string(result);
  }
  else {
    cerr<<"Utilities:Warning! static_function_CURRENT_LAYER has no object to look up."<<endl;
    return "0";
  }
}

//----------------------------------------------------------------------------//

string Utilities::static_function_PREVIOUS_CHILD_DURATION(void* _object){
if (_object !=NULL){
    double resultNum = ((Event*)_object)->getPreviousChildDuration();
    char result [50];
    sprintf(result, "%f",  resultNum);
    return string(result);
  }
  else {
    cerr<<"Utilities:Warning! static_function_PREVIOUS_CHILD_DURATION has no object to look up."<<endl;
    return "0";
  }
}

//----------------------------------------------------------------------------//

string Utilities::function_Inverse(DOMElement* _functionElement, void* _object){

  DOMElement* elementIter = _functionElement->GFEC()->GNES();
  double entry = evaluate(XMLTranscode(elementIter ),_object);

  double resultNum = ( 1. / entry );
  char result [50];
  sprintf(result, "%f",  resultNum);
  return string(result);
}


string Utilities::function_Markov(DOMElement* _functionElement, void* _object) {
  DOMElement* elementIter = _functionElement->GFEC()->GNES();
  int entry = (int)evaluate(XMLTranscode(elementIter), _object);

  float resultNum = markovModelLibrary[entry].nextSample(Random::Rand());
  cout << "Markov picked " << resultNum << endl;
  char result [50];
  sprintf(result, "%f", resultNum);
  return string(result);
}

//----------------------------------------------------------------------------//

string Utilities::function_LN(DOMElement* _functionElement, void* _object){

  DOMElement* elementIter = _functionElement->GFEC()->GNES();
  double entry = evaluate(XMLTranscode(elementIter ),_object);

  double resultNum = ( 1. / pow(2.71828, entry) );
  char result [50];
  sprintf(result, "%f",  resultNum);
  return string(result);
}

//----------------------------------------------------------------------------//

string Utilities::function_Fibonacci(DOMElement* _functionElement, void* _object){
  DOMElement* elementIter = _functionElement->GFEC()->GNES();
  int entry = evaluate(XMLTranscode(elementIter ),_object);

  int numA = 1;
  int numB = 1;
  for (int i = 3; i <= entry; i++) {
    int swap = numB;
    numB += numA;
    numA = swap;
  }
  int resultNum = numB;
  char result [50];
  sprintf(result, "%i",  resultNum);
  return string(result);
}

//----------------------------------------------------------------------------//

string Utilities::function_Decay(DOMElement* _functionElement, void* _object){
//  <Fun>
//    <Name>Decay</Name>
//    <Base>base</Base>
//    <Type>EXPONENTIAL</Type>
//    <Rate>rate</Rate>
//    <Index>CURRENT_PARTIAL_NUM</Index>
//  </Fun>

  DOMElement* elementIter = _functionElement->GFEC()->GNES();
  double base = evaluate(XMLTranscode(elementIter ),_object);

  elementIter = elementIter->GNES();
  string type = XMLTranscode(elementIter);

  elementIter = elementIter->GNES();
  double rate = evaluate(XMLTranscode(elementIter ),_object);

  elementIter = elementIter->GNES();
  double index = evaluate(XMLTranscode(elementIter ),_object);


  double decay;

  if (type == "EXPONENTIAL") {
    decay = base * pow(rate, index);
  } else if (type == "LINEAR") {
    decay = base - (rate * index);
  }

  char result [50];
  sprintf(result, "%f",  decay);
  return string(result);
}

//----------------------------------------------------------------------------//

string Utilities::function_Stochos(DOMElement* _functionElement, void* _object){
//  <Fun>
//    <Name>Stochos</Name>
//    <Method>RANGE_DISTRIB</Method>
//    <Envelopes>
//      <Envelope>ENV</Envelope>
//      <Envelope>ENV</Envelope>
//      <Envelope>ENV</Envelope>
//    </Envelopes>
//    <Offset>3</Offset>
//  </Fun>

  // setup routine vars
  double checkpoint = 0;

  if (_object != NULL) {
    checkpoint = ((Event*)_object)->getCheckPoint();
    if (checkpoint < 0 || checkpoint > 1) {
      cerr << "Utilities:Stochos -- checkpoint error!" << endl;
      cerr << "   checkPoint = " << checkpoint;
      cerr << ", filename = " << ((Event*)_object)->getEventName();
      cerr << endl;
    }
  }

  DOMElement* elementIter = _functionElement->GFEC()->GNES();
  string method = XMLTC(elementIter);

  elementIter = elementIter->GNES();
  DOMElement* envElementIter = elementIter->GFEC();
  vector<Envelope*> envVect;
  while (envElementIter!=NULL) {
    //cout<<XMLTC(envElementIter)<<endl;
    envVect.push_back((Envelope*)evaluateObject(XMLTC(envElementIter), _object, eventEnv));
    envElementIter = envElementIter->GNES();
  }


  elementIter = elementIter->GNES();
  int offset = (int) evaluate ( XMLTC(elementIter), _object);
  float returnVal;

  if(method == "FUNCTIONS") {
    float randomNumber;

    // stacked up envelopes: their values at the same moment add up to 1
    for (int i = 0; i < envVect.size(); i++) {
      returnVal = envVect[i]->getValue(checkpoint, 1.);
      if(envVect.size() > 1) {                      // probability areas
        if(i == 0) randomNumber = Random::Rand();
        if (returnVal >= randomNumber) {
          returnVal = i;
          i = envVect.size(); // done: break out of the for loop
        }
      }
    }
  } else if (method == "RANGE_DISTRIB") {
    float limit[2];

    // distribution within given range; takes 3 envs: min, MAX, val in between
    if(3 * offset >= envVect.size()) {
      cerr << "Error - Stochos - Not enough envelopes on the list: envVect.size="
           << envVect.size() << " 3*offset=" << 3 * offset << endl;
      if (_object != NULL) {
        cerr << "       in file " << ((Event*)_object)->getEventName() << endl;
      }
      exit(1);
    }
    for(int i = 0; i < 2; i++) {
      if(envVect[3 * offset + i] != NULL) {
        limit[i] = envVect[3 * offset + i]->getValue(checkpoint, 1);
      } else {
        cerr << "Stochos - NULL envelope. Trying to access envy["
             << 3 * offset + i<< "]=" <<  envVect[3 * offset + i] << endl;
      }
    }

    if(envVect[3 * offset + 2]) {
      returnVal = envVect[3 * offset + 2]->getValue(Random::Rand(), 1);
    } else {
      cerr << "Stochos - NULL envelope. Trying to access envVect["
           << 3 * offset + 2 << "]=" << envVect[ 3 * offset + 2] << endl;
    }

    returnVal *= (limit[1] - limit[0]);
    returnVal += limit[0];
  } else {
    cerr << "Stochos --- invalid method!  Use FUNCTIONS or RANGE_DISTRIB" << endl;
    exit(1);
  }

  for (int i = 0; i < envVect.size(); i ++){
    delete envVect[i];
  }

  char result [50];
  sprintf(result, "%f",  returnVal);
  return string(result);

}

//----------------------------------------------------------------------------//

string Utilities::function_ValuePick(DOMElement* _functionElement, void* _object){
  Sieve* si = sieve_ValuePick(_functionElement, _object);
  int resultNum = si->ChooseL();
  delete si;
  char result [50];
  sprintf(result, "%i",  resultNum);
  return string(result);
}

//----------------------------------------------------------------------------//

Sieve* Utilities::sieve_ValuePick(DOMElement* _functionElement, void* _object){

//<Fun>
//  <Name>ValuePick</Name>
//  <Range>range</Range>
//  <Low>anEnv</Low>
//  <High>anEnv</High>
//  <Dist>anEnv</Dist>
//  <Method>MEANINGFUL</Method>
//  <Elements>1,2,3,4</Elements>
//  <WeightMethod> PERIODIC</WeightMethod>
//  <Weight>1,1,1,1</Weight>
//  <Type>VARIABLE</Type>
//  <Offsets>0,0,0,0</Offsets>
//</Fun>

// setup routine vars
  double checkpoint = 0;

  if (_object != NULL) {
    checkpoint = ((Event*)_object)->getCheckPoint();
  }

  DOMElement* elementIter = _functionElement->GFEC()->GNES();

  double absRange =  evaluate (XMLTC(elementIter), _object);

  elementIter = elementIter->GNES();
  Envelope *envLow = (Envelope*)evaluateObject(XMLTC(elementIter), _object, eventEnv);

  elementIter = elementIter->GNES();
  Envelope *envHigh = (Envelope*)evaluateObject(XMLTC(elementIter), _object, eventEnv);

  elementIter = elementIter->GNES();
  Envelope *envDist = (Envelope*)evaluateObject(XMLTC(elementIter), _object, eventEnv);

  elementIter = elementIter->GNES();
  string eMethod = XMLTC(elementIter);

  elementIter = elementIter->GNES();
  vector<std::string> eArgs = listElementToStringVector( elementIter);
  vector<int> eArgVect;
  if (eMethod != "MODS") {
    for (int i = 0; i < eArgs.size(); i ++){
      eArgVect.push_back((int)evaluate(eArgs[i], _object));
    }
  }
  elementIter = elementIter->GNES();
  string wMethod = XMLTC(elementIter);

  elementIter = elementIter->GNES();
  vector<std::string> wArgs = listElementToStringVector( elementIter);
  vector<int> wArgVect;
  for (int i = 0; i < wArgs.size(); i ++){
    wArgVect.push_back((int)evaluate(wArgs[i], _object));
  }


  elementIter = elementIter->GNES();
  string modifyMethod = XMLTC(elementIter);

  elementIter = elementIter->GNES();
  vector<std::string> offsetString = listElementToStringVector( elementIter);
  vector<int> offsetVect;
  for (int i = 0; i < offsetString.size(); i ++){
    offsetVect.push_back((int)evaluate(offsetString[i], _object));
  }


  int minVal = (int)floor( envLow->getScaledValueNew(checkpoint, 1) * absRange + 0.5);
  int maxVal = (int)floor( envHigh->getScaledValueNew(checkpoint, 1) * absRange + 0.5);

  Sieve si;
  if (eMethod == "MODS") {
      si.BuildFromExpr(minVal, maxVal,
                         eMethod.c_str(), wMethod.c_str(),
                         eArgs[0], wArgVect,
                         offsetVect);
  } else {
    si.Build(minVal, maxVal, eMethod.c_str(), wMethod.c_str(), eArgVect, wArgVect, offsetVect);
  }

  si.Modify(envDist, modifyMethod);
  delete envLow;
  delete envHigh;
  delete envDist;
  return new Sieve(si);
}

//----------------------------------------------------------------------------//

Sieve* Utilities::sieve_ChooseL(DOMElement* _functionElement, void* _object) {
  string sivFunctionString =  XMLTC(_functionElement->GFEC()->GNES()) ;
  return (Sieve*)evaluateObject(sivFunctionString, _object, eventSiv);
}

//----------------------------------------------------------------------------//

string Utilities::function_ChooseL(DOMElement* _functionElement, void* _object){
  //<Fun><Name>ChooseL</Name><Entry>SIV</Entry></Fun>
  string sivFunctionString =  XMLTC(_functionElement->GFEC()->GNES()) ;
  Sieve* svPtr = (Sieve*)evaluateObject(sivFunctionString, _object, eventSiv);
  double resultNum = svPtr->ChooseL();
  delete svPtr;
  char result [50];
  sprintf(result, "%f",  resultNum);
  return string(result);
}

//----------------------------------------------------------------------------//

string Utilities::function_MakeList(DOMElement* _functionElement, void* _object){
  cout<<"Utilites: Make_List is not implemented yet."<<endl;
//<Fun><Name>MakeList</Name><Func></Func><Size></Size></Fun>

  DOMElement* listElement = _functionElement->getFirstElementChild()->getNextElementSibling();

  std::vector<std::string> stringList = listElementToStringVector(listElement);
  DOMElement* boundElement = listElement->getNextElementSibling();

  int bound = (int)evaluate(XMLTranscode(boundElement), _object);
/*
  char result [50];
  sprintf(result, "%f",  evaluate( list[index], _object));
  return string(result);
*/
    vector<int> intList;

    for (int i = 0; i < intList.size(); i ++){
      int num = (int) evaluate(stringList[i], _object);
      if (num <= bound) {
        intList.push_back(num);
      } else {
  return "intList";
      }
    }

  return "intList";
}

//----------------------------------------------------------------------------//

string Utilities::function_Randomizer(DOMElement* _functionElement, void* _object){

  DOMElement* baseValElement = _functionElement->getFirstElementChild()->getNextElementSibling();
  DOMElement* percDevElement = baseValElement->getNextElementSibling();

  double baseVal = evaluate(XMLTranscode(baseValElement ),_object);
  double percDev = evaluate(XMLTranscode(percDevElement), _object);

  double devVal = baseVal * percDev;
  double resultNum = Random::Rand(baseVal - devVal, baseVal + devVal);

  char result [50];
  sprintf(result, "%f",  resultNum);
  return string(result);

}

//----------------------------------------------------------------------------//

string Utilities::function_Random(DOMElement* _functionElement, void* _object){

  DOMElement* lowBoundElement = _functionElement->getFirstElementChild()->getNextElementSibling();
  DOMElement* highBoundElement = lowBoundElement->getNextElementSibling();

  double lowBound = evaluate(XMLTranscode(lowBoundElement ),_object);
  double highBound = evaluate(XMLTranscode(highBoundElement), _object);

  char result [50];
  sprintf(result, "%f",  Random::Rand(lowBound, highBound));
  return string(result);

}

//----------------------------------------------------------------------------//

string Utilities::function_Select(DOMElement* _functionElement, void* _object){

  DOMElement* listElement = _functionElement->getFirstElementChild()->getNextElementSibling();
  DOMElement* indexElement = listElement->getNextElementSibling();

  std::vector<std::string> list = listElementToStringVector(listElement);

  int index = (int)evaluate(XMLTranscode(indexElement), _object);
  char result [50];
  sprintf(result, "%f",  evaluate( list[index], _object));
  return string(result);

}

//----------------------------------------------------------------------------//

string Utilities::function_SelectObject(DOMElement* _functionElement, void* _object){

  DOMElement* listElement = _functionElement->getFirstElementChild()->getNextElementSibling();
  DOMElement* indexElement = listElement->getNextElementSibling();
  std::vector<std::string> list = listElementToStringVector(listElement);
  int index = (int)evaluate(XMLTranscode(indexElement), _object);
  return list[index];
}

//----------------------------------------------------------------------------//

string Utilities::function_GetPattern(DOMElement* _functionElement, void* _object){

  DOMElement* elementIter = _functionElement->GFEC()->GNES();//method

  string method = XMLTranscode(elementIter);
  elementIter = elementIter->GNES();//origin
  int origin = (int)evaluate(XMLTranscode(elementIter), _object);

  string patternString = XMLTranscode(_functionElement);

  Patter* pattern = NULL;
  for (int i = 0; i < ((Event*) _object)->patternStorage.size(); i++){

    if (patternString == (static_cast<Event*> (_object))->patternStorage[i]->getKey()){
      //cout<<"find existing pattern"<<endl;
      pattern = ((Event*) _object)->patternStorage[i]->getPattern();
      break;
    }
  }
  if (pattern ==NULL){ //this pattern doesn't exist, make a new pattern and put
                       //it in the _object's patternStorage. the _object is
                       //responsible for cleaning up the memory when it's done.

    pattern = (Patter*) (evaluateObject(XMLTranscode(elementIter->GNES()) , _object, eventPat));
    ((Event*) _object)->addPattern(patternString, pattern);

  }

  double returnValue = pattern->GetNextValue(method, origin);
  char result [50];
  sprintf(result, "%f", returnValue);
  return string(result);

}

//----------------------------------------------------------------------------//

string Utilities::function_RandomInt(DOMElement* _functionElement, void* _object){

  DOMElement* lowBoundElement = _functionElement->getFirstElementChild()->getNextElementSibling();
  DOMElement* highBoundElement = lowBoundElement->getNextElementSibling();

  int lowBound = (int)evaluate(XMLTranscode(lowBoundElement), _object);
  int highBound = (int)evaluate(XMLTranscode(highBoundElement), _object);

  char result [50];
  sprintf(result, "%d",  Random::RandInt(lowBound, highBound));
  return string(result);
}

string Utilities::function_RandomDensity(DOMElement* _functionElement, void* _object) {
  DOMElement* envelopeNumberElement = _functionElement->getFirstElementChild()->getNextElementSibling();
  DOMElement* lowBoundElement = envelopeNumberElement->getNextElementSibling();
  DOMElement* highBoundElement = lowBoundElement->getNextElementSibling();

  int envelopeNumber = (int)evaluate(XMLTranscode(envelopeNumberElement), _object);
  double lowBound = evaluate(XMLTranscode(lowBoundElement), _object);
  double highBound = evaluate(XMLTranscode(highBoundElement), _object);

  ProbabilityEnvelope env(*(envelopeLibrary->getEnvelope(envelopeNumber)));
  // initialize the count table
  env.generateCountTable(1000);

  // sample from the count table
  double rand = Random::Rand(0, 1);
  double resultNumber = env.sample(rand) * (highBound - lowBound) + lowBound;
  // cout << "lowbound: " << lowBound << ", highbound: " << highBound << ", result: " << resultNumber << endl;

  char result [50];
  sprintf(result, "%lf", resultNumber);
  return string(result);
}

//----------------------------------------------------------------------------//

Sieve* Utilities::getSieve(string _functionString, void* _object){
  XercesDOMParser* parser = new XercesDOMParser();

  string toParse = "<root>" + _functionString + "</root>";
  xercesc::MemBufInputSource myxml_buf (
     (const XMLByte*)toParse.c_str(),
     toParse.size(),
     "function (in memory)");

  parser->parse(myxml_buf);
  DOMDocument* xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();

  Sieve* sieve = getSieveHelper (_object, root);
  delete parser;
  return sieve;
}

//----------------------------------------------------------------------------//

Sieve* Utilities::getSieveHelper(void* _object, DOMElement* _SIVFunction){

  // Get the function name
  DOMElement* functionNameElement = _SIVFunction->GFEC()->GFEC();

  // If the function is ReadSIVFile:
  // Get the _SIVFunction from the filename, and recursively call itself
  // on that function.
  if (XMLTranscode(functionNameElement).compare("ReadSIVFile")==0){
    string fileName = XMLTranscode(functionNameElement->GNES());
    DOMElement* k = getEventElement(eventSiv, fileName);
    return getSieveHelper(_object, k->GFEC()->GNES()->GNES());
  }

  // If the function is MakeSieve:
  // Read each of the values from _SIVFunction and create a new Sieve with
  // those values.
  else if (XMLTranscode(functionNameElement).compare("MakeSieve")==0){

    // Get minVal
    DOMElement* elementIter = _SIVFunction->GFEC()->GFEC()->GNES();
    int minVal = evaluate(XMLTC(elementIter), _object);

    // Get maxVal
    elementIter = elementIter->GNES();
    int maxVal = evaluate(XMLTC(elementIter), _object);

    // Get eMethod
    elementIter = elementIter->GNES();
    string eMethod = XMLTC(elementIter);

    // Get eArgInts
    elementIter = elementIter->GNES();
    vector<std::string> eArgs = listElementToStringVector( elementIter);
    vector<int> eArgInts;
    if (eMethod != "MODS") {
      for (int i = 0; i < eArgs.size(); i ++){
        eArgInts.push_back((int)evaluate(eArgs[i], _object));
      }
    }

    // Get wMethod
    elementIter = elementIter->GNES();
    string wMethod = XMLTC(elementIter);

    // Get wArgInts
    elementIter = elementIter->GNES();
    vector<std::string> wArgs = listElementToStringVector( elementIter);
    vector<int> wArgInts;
    for (int i = 0; i < wArgs.size(); i ++){
      wArgInts.push_back((int)evaluate(wArgs[i], _object));
    }

    // Get offsetVect
    elementIter = elementIter->GNES();
    vector<std::string> offsetString = listElementToStringVector( elementIter);
    vector<int> offsetVect;

    if (offsetString.size()==1){ //only one number, so we need to copy it to form a vector to match the number elements
      for (int i = 0; i < eArgs.size(); i ++){
        offsetVect.push_back((int)evaluate(offsetString[0], _object));
      }
    }
    else {
      for (int i = 0; i < offsetString.size(); i ++){
        offsetVect.push_back((int)evaluate(offsetString[i], _object));
      }
    }

    // Build the Sieve
    Sieve* siv = new Sieve();
    if (eMethod == "MODS") {
      siv->BuildFromExpr(minVal, maxVal,
                         eMethod.c_str(), wMethod.c_str(),
                         eArgs[0], wArgInts,
                         offsetVect);
    } else {
      siv->Build(minVal, maxVal,
                 eMethod.c_str(), wMethod.c_str(),
                 eArgInts, wArgInts,
                 offsetVect);
    }
    return siv;
  }//end MakeSieve

  // If the function is Select:
  // Parse the Select function from _SIVFunction and recursively call itself
  // on the new function.
  else if (XMLTranscode(functionNameElement).compare("Select")==0){
    string selectedListElementString = "<root>" + function_SelectObject(_SIVFunction->GFEC(), _object) + "</root>";

    XercesDOMParser* parser = new XercesDOMParser();

    xercesc::MemBufInputSource myxml_buf  (
      (const XMLByte*)selectedListElementString.c_str(),
       selectedListElementString.size(),
       "function (in memory)");

    parser->parse(myxml_buf);

    DOMDocument* xmlDocument = parser->getDocument();
    DOMElement* root = xmlDocument->getDocumentElement();

    cout <<"	before Sieve* sieve =  getSieveHelper(_object, root);" << endl;

    Sieve* sieve =  getSieveHelper(_object, root);
    delete parser;
    return sieve;

  }

  // Otherwise, the function fails.
  cerr<<"Utilities::Warning! Sieve Construction Failed"<<endl;
  return NULL;
}


//----------------------------------------------------------------------------//

Patter* Utilities::getPattern(string _functionString, void* _object){

  XercesDOMParser* parser = new XercesDOMParser();

  string toParse = "<root>" + _functionString + "</root>";
    xercesc::MemBufInputSource myxml_buf  (
      (const XMLByte*)toParse.c_str(),
       toParse.size(),
       "function (in memory)");

  parser->parse(myxml_buf);

  DOMDocument* xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();

  Patter* pattern = getPatternHelper (_object, root);
  delete parser;
  return pattern;
}

//----------------------------------------------------------------------------//

Patter* Utilities::getPatternHelper(void* _object, DOMElement* _PATFunction){
  // Get the function name
  DOMElement* functionNameElement = _PATFunction->getFirstElementChild()->getFirstElementChild();

  // If the function is ReadPATFile:
  // Get the _PATFunction from the filename, and recursively call itself
  // on that function.
  if (XMLTranscode(functionNameElement).compare("ReadPATFile")==0){
    string fileName = XMLTranscode(functionNameElement->GNES());
    DOMElement* k = getEventElement(eventPat, fileName);

    return getPatternHelper(_object, k->GFEC()->GNES()->GNES());

  }

  // If the function is MakePattern:
  // Read each of the values from _PATFunction and create a new Patter with
  // those values.
  else if (XMLTranscode(functionNameElement).compare("MakePattern")==0){

    DOMElement* listElement = _PATFunction->GFEC()->GFEC()->GNES();
    vector<string> stringList =listElementToStringVector (listElement);
    vector<int> intList;
    for (int i = 0; i < stringList.size(); i ++){
      int num = (int) evaluate(stringList[i], _object);
      intList.push_back(num);
    }

    Patter* newPattern = new Patter(0, intList);
    newPattern->SimplePat();
    return newPattern; //Utilities will put this pattern in _object's patternStorage

  }

  // If the function is ExpandPattern:
  // First find the parameters to expand by.
  // Recursively call itself on the pattern to expand.
  // Expand the pattern.
  //
  // TODO: Figure out if this is true.
  else if (XMLTranscode(functionNameElement).compare("ExpandPattern")==0){
    cout<<"see Expand pattern:"<<endl;
    cout<<XMLTranscode(_PATFunction->GFEC())<<endl;
    DOMElement* elementIter = _PATFunction->GFEC()->GFEC()->GNES();
    string method = XMLTranscode(elementIter);

    // Find Expand parameters
    elementIter = elementIter->GNES();
    int mod = evaluate ( XMLTranscode( elementIter), _object);

    elementIter = elementIter->GNES();
    int low = evaluate ( XMLTranscode( elementIter), _object);

    elementIter = elementIter->GNES();
    int high = evaluate ( XMLTranscode( elementIter), _object);

    // Recurse on the pattern to expand
    elementIter = elementIter->GNES();
    Patter* pattern = getPatternHelper(_object, elementIter);
    pattern->Expand( method, mod, low, high );
    return pattern;

  }

  // If the function is Select:
  // Parse the Select function from _PATFunction and recursively call itself
  // on the new function.
  else if (XMLTranscode(functionNameElement).compare("Select")==0){
    string selectedListElementString = "<root>" + function_SelectObject(_PATFunction->GFEC(), _object) + "</root>";

    // Here we need to push a temporary parser in the _object to stored the
    // parsed selectedListElementString...
    XercesDOMParser* parser = new XercesDOMParser();

    xercesc::MemBufInputSource myxml_buf  (
      (const XMLByte*)selectedListElementString.c_str(),
       selectedListElementString.size(),
       "function (in memory)");

    parser->parse(myxml_buf);

    DOMDocument* xmlDocument = parser->getDocument();
    DOMElement* root = xmlDocument->getDocumentElement();



    Patter* pat =  getPatternHelper(_object, root);
    delete parser;
    return pat;

  }
  cerr<<"Utilities::Warning! Pattern Construction Failed"<<endl;
  return NULL;
}

//----------------------------------------------------------------------------//

DOMElement* Utilities::getSPAFunctionElement(void* _object){
  getSPAFunctionElementHelper(_object,NULL,true);
}

//----------------------------------------------------------------------------//

DOMElement* Utilities::getSPAFunctionElementHelper(void* _object, DOMElement* _SPAFunction, bool _initialCall){
  DOMElement* SPAElement;

  if (_initialCall ==true){
    Bottom* thisBottom = (Bottom*)_object;
    SPAElement = thisBottom->getSPAElement();
  }
  else {
    SPAElement = _SPAFunction;
  }
  DOMElement* functionNameElement = SPAElement->getFirstElementChild()->getFirstElementChild();

  if (XMLTranscode(functionNameElement).compare("ReadSPAFile")==0){
    string functionString = XMLTC(SPAElement);
    string fileName;

    size_t select = functionString.find("Select", 0);
    if (select == string::npos){
      fileName = XMLTranscode(functionNameElement->GNES());
    }
    else{ //see select inside readSPA
      //cout<<functionString<<endl;
      string selectedListElementString = "<root><Fun><Name>ReadSPAFile</Name><File>" + function_SelectObject(SPAElement->GFEC()->GFEC()->GNES()->GFEC(), _object) + "</File></Fun></root>";
      // Here we need to push a temporary parser in the _object to stored the
      // parsed selectedListElementString...

      selectedListElementString = removeSpaces (selectedListElementString);


      XercesDOMParser* parser = new XercesDOMParser();

      xercesc::MemBufInputSource myxml_buf  (
        (const XMLByte*)selectedListElementString.c_str(),
        selectedListElementString.size(),
        "function (in memory)");

      parser->parse(myxml_buf);

      DOMDocument* xmlDocument = parser->getDocument();
      DOMElement* root = xmlDocument->getDocumentElement();
      ((Event*)_object)->addTemporaryXMLParser(parser);
      //after the statement above, the ownership of the parser is transfered to
      // the _object. the _object is responsible to clean up the memory of the
      // parser, which is done by the std::vector automatically.
      return getSPAFunctionElementHelper(_object, root, false);

    }//end see select inside readSPA


    fileName = removeSpaces (fileName);


    DOMElement* k = getEventElement(eventSpa, fileName);

    return getSPAFunctionElementHelper(_object, k->GFEC()->GNES()->GNES(), false);

  }
  else if (XMLTranscode(functionNameElement).compare("Select")==0){
    string selectedListElementString = "<root>" + function_SelectObject(SPAElement->GFEC(), _object) + "</root>";

    // Here we need to push a temporary parser in the _object to stored the
    // parsed selectedListElementString...
    XercesDOMParser* parser = new XercesDOMParser();

    xercesc::MemBufInputSource myxml_buf  (
      (const XMLByte*)selectedListElementString.c_str(),
       selectedListElementString.size(),
       "function (in memory)");

    parser->parse(myxml_buf);

    DOMDocument* xmlDocument = parser->getDocument();
    DOMElement* root = xmlDocument->getDocumentElement();

    ((Event*)_object)->addTemporaryXMLParser(parser);
    //after the statement above, the ownership of the parser is transfered to
    // the _object. the _object is responsible to clean up the memory of the
    // parser, which is done by the std::vector automatically.
    return getSPAFunctionElementHelper(_object, root, false);

  }
  else { //function name = SPA
    return SPAElement->getFirstElementChild();
  }
}

//----------------------------------------------------------------------------//

DOMElement* Utilities::getREVFunctionElement(void* _object){
  getREVFunctionElementHelper(_object,NULL,true);
}

//----------------------------------------------------------------------------//

DOMElement* Utilities::getREVFunctionElementHelper(void* _object, DOMElement* _REVFunction, bool _initialCall){

  DOMElement* REVElement;

  if (_initialCall ==true){
    Bottom* thisBottom = (Bottom*)_object;
    REVElement = thisBottom->getREVElement();
  }
  else {
    REVElement = _REVFunction;
  }

  DOMElement* functionNameElement = REVElement->getFirstElementChild()->getFirstElementChild();

  if (XMLTranscode(functionNameElement).compare("ReadREVFile")==0){
    string functionString = XMLTC(REVElement);
    string fileName;

    size_t select = functionString.find("Select", 0);
    if (select == string::npos){
      fileName = XMLTranscode(functionNameElement->GNES());
    }
    else{ //see select inside readREV
      //cout<<functionString<<endl;
      string selectedListElementString = "<root><Fun><Name>ReadREVFile</Name><File>" + function_SelectObject(REVElement->GFEC()->GFEC()->GNES()->GFEC(), _object) + "</File></Fun></root>";
    // Here we need to push a temporary parser in the _object to stored the
    // parsed selectedListElementString...

      selectedListElementString = removeSpaces (selectedListElementString);


      XercesDOMParser* parser = new XercesDOMParser();

      xercesc::MemBufInputSource myxml_buf  (
        (const XMLByte*)selectedListElementString.c_str(),
        selectedListElementString.size(),
        "function (in memory)");

      parser->parse(myxml_buf);

      DOMDocument* xmlDocument = parser->getDocument();
      DOMElement* root = xmlDocument->getDocumentElement();
      ((Event*)_object)->addTemporaryXMLParser(parser);
      //after the statement above, the ownership of the parser is transfered to
      // the _object. the _object is responsible to clean up the memory of the
      // parser, which is done by the std::vector automatically.
      return getREVFunctionElementHelper(_object, root, false);

    }//end see select inside readSPA


    fileName = removeSpaces (fileName);


    DOMElement* k = getEventElement(eventRev, fileName);

    return getREVFunctionElementHelper(_object, k->GFEC()->GNES()->GNES(), false); ;

  }
  else if (XMLTranscode(functionNameElement).compare("Select")==0){
    string selectedListElementString = "<root>" + function_SelectObject(REVElement->GFEC(), _object) + "</root>";

    // Here we need to push a temporary parser in the _object to stored the
    // parsed selectedListElementString...
    XercesDOMParser* parser = new XercesDOMParser();

    xercesc::MemBufInputSource myxml_buf  (
      (const XMLByte*)selectedListElementString.c_str(),
       selectedListElementString.size(),
       "function (in memory)");

    parser->parse(myxml_buf);

    DOMDocument* xmlDocument = parser->getDocument();
    DOMElement* root = xmlDocument->getDocumentElement();

    ((Event*)_object)->addTemporaryXMLParser(parser);
    //after the statement above, the ownership of the parser is transfered to
    // the _object. the _object is responsible to clean up the memory of the
    // parser, which is done by the std::vector automatically.
    return getREVFunctionElementHelper(_object, root, false);

  }

  else { // simple or medium or advanced
    return REVElement->getFirstElementChild();
  }
}


//----------------------------------------------------------------------------//

DOMElement* Utilities::getFILFunctionElement(void* _object){
  getFILFunctionElementHelper(_object,NULL,true);
}

//----------------------------------------------------------------------------//

DOMElement* Utilities::getFILFunctionElementHelper(void* _object, DOMElement* _FILFunction, bool _initialCall){

  DOMElement* FILElement;

  if (_initialCall ==true){
    Bottom* thisBottom = (Bottom*)_object;
    FILElement = thisBottom->getFILElement();
  }
  else {
    FILElement = _FILFunction;
  }



  if (FILElement->GFEC()==NULL || FILElement->GFEC()->GFEC()==NULL) return NULL;
   DOMElement* functionNameElement = FILElement->getFirstElementChild()->getFirstElementChild();

  if ( XMLTC(FILElement) ==""){
    cout<<"no filter"<<endl;
    return NULL;
  }


  if (XMLTranscode(functionNameElement).compare("ReadFILFile")==0){
    string functionString = XMLTC(FILElement);
    string fileName;

    size_t select = functionString.find("Select", 0);
    if (select == string::npos){
      fileName = XMLTranscode(functionNameElement->GNES());
    }
    else{ //see select inside readFIL
      //cout<<functionString<<endl;
      string selectedListElementString = "<root><Fun><Name>ReadFILFile</Name><File>" + function_SelectObject(FILElement->GFEC()->GFEC()->GNES()->GFEC(), _object) + "</File></Fun></root>";
    // Here we need to push a temporary parser in the _object to stored the
    // parsed selectedListElementString...

      selectedListElementString = removeSpaces (selectedListElementString);


      XercesDOMParser* parser = new XercesDOMParser();

      xercesc::MemBufInputSource myxml_buf  (
        (const XMLByte*)selectedListElementString.c_str(),
        selectedListElementString.size(),
        "function (in memory)");

      parser->parse(myxml_buf);

      DOMDocument* xmlDocument = parser->getDocument();
      DOMElement* root = xmlDocument->getDocumentElement();
      ((Event*)_object)->addTemporaryXMLParser(parser);
      //after the statement above, the ownership of the parser is transfered to
      // the _object. the _object is responsible to clean up the memory of the
      // parser, which is done by the std::vector automatically.
      return getFILFunctionElementHelper(_object, root, false);

    }//end see select inside readSPA


    fileName = removeSpaces (fileName);


    DOMElement* k = getEventElement(eventFil, fileName);

    return getFILFunctionElementHelper(_object, k->GFEC()->GNES()->GNES(), false); ;

  }
  else if (XMLTranscode(functionNameElement).compare("Select")==0){
    string selectedListElementString = "<root>" + function_SelectObject(FILElement->GFEC(), _object) + "</root>";

    // Here we need to push a temporary parser in the _object to stored the
    // parsed selectedListElementString...
    XercesDOMParser* parser = new XercesDOMParser();

    xercesc::MemBufInputSource myxml_buf  (
      (const XMLByte*)selectedListElementString.c_str(),
       selectedListElementString.size(),
       "function (in memory)");

    parser->parse(myxml_buf);

    DOMDocument* xmlDocument = parser->getDocument();
    DOMElement* root = xmlDocument->getDocumentElement();

    ((Event*)_object)->addTemporaryXMLParser(parser);
    //after the statement above, the ownership of the parser is transfered to
    // the _object. the _object is responsible to clean up the memory of the
    // parser, which is done by the std::vector automatically.
    return getFILFunctionElementHelper(_object, root, false);

  }

  else {
    return FILElement->getFirstElementChild();
  }
}


//----------------------------------------------------------------------------//

Envelope* Utilities::getEnvelope(string _input, void* _object){

  XercesDOMParser* parser = new XercesDOMParser();
  xercesc::MemBufInputSource myxml_buf  ((const XMLByte*)_input.c_str(), _input.size(), "function (in memory)");

  parser->parse(myxml_buf);
  DOMDocument* xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();

  char* functionNameChars;
  char* charBuffer;
  DOMCharacterData* textData;
  string functionName;
  DOMElement* thisElement;


  DOMElement* functionNameElement = root->getFirstElementChild();
  textData = ( DOMCharacterData*) functionNameElement->getFirstChild();

  if (textData){
    functionNameChars = XMLString::transcode(textData->getData());
    functionName = string(functionNameChars);
    XMLString::release(&functionNameChars);
  }
  Envelope* returnEnvelope;
  if(functionName.compare("EnvLib")==0){
      returnEnvelope = envLib(functionNameElement->getNextElementSibling(), _object);
  }
  else if (functionName.compare("MakeEnvelope")==0){
    returnEnvelope = makeEnvelope(functionNameElement->getNextElementSibling(), _object);
  }
  else if (functionName.compare("ReadENVFile")==0){
    returnEnvelope = readEnvFile(functionNameElement->getNextElementSibling(), _object);
  }
  else if (functionName.compare("Select")==0){
    string selectedListElementString = function_SelectObject(root, _object);
    delete parser;
    return getEnvelope(selectedListElementString,  _object);
  }
  else {
    delete parser;
    cout<<"warning: evaluating envelope failed."<<endl;
    return NULL;
  }
  delete parser;
  return returnEnvelope;
}

//----------------------------------------------------------------------------//

Envelope* Utilities::envLib(DOMElement* _functionElement, void* _object){
//  <Env>3</Env>
//  <Scale>1.0</Scale>
  int envelopeNumber = evaluate(XMLTranscode(_functionElement), _object);
  Envelope* env = envelopeLibrary->getEnvelope(envelopeNumber);
  //cout <<"EnvLib: #"<<envelopeNumber<<endl;
  double scale = evaluate(XMLTranscode(_functionElement->getNextElementSibling()), _object);
  env->scale(scale);
  return env;

}

//----------------------------------------------------------------------------//

Envelope* Utilities::readEnvFile(DOMElement* _functionElement, void* _object){
  //<File>object name</File>

  DOMElement* file = getEventElement(eventEnv, XMLTranscode(_functionElement));

//  <Event orderInPalette=' 0'>
//      <EventType>6</EventType>
//      <Name>oeu</Name>
//      <EnvelopeBuilder><Fun><Name>EnvLib</Name><Env>1</Env><Scale>1.0</Scale></Fun></EnvelopeBuilder>
//    </Event>
//

  DOMElement* builder = file->getFirstElementChild()->
    getNextElementSibling()->
    getNextElementSibling();

  return (Envelope*) evaluateObject(XMLTranscode(builder), _object, eventEnv);

}

//----------------------------------------------------------------------------//

Envelope* Utilities::makeEnvelope(DOMElement* _functionElement, void* _object){

//<Xs>
//  <X>0</X>
//  <X>0.5</X>
//  <X>1</X>
//</Xs>
//<Ys>
//  <Y>1</Y>
//  <Y>0</Y>
//  <Y>0.5</Y>
//</Ys>
//<Types>
//  <T>LINEAR</T>
//  <T>LINEAR</T>
//</Types>
//<Pros>
//  <P>FLEXIBLE</P>
//  <P>FLEXIBLE</P>
//</Pros>
//<Scale>1</Scale>

  DOMElement* x = _functionElement->GFEC();
  _functionElement = _functionElement->GNES();
  DOMElement* y = _functionElement->GFEC();
  _functionElement = _functionElement->GNES();
  DOMElement* t = _functionElement->GFEC();
  _functionElement = _functionElement->GNES();
  DOMElement* p = _functionElement->GFEC();

  double scale = evaluate(XMLTranscode(_functionElement->getNextElementSibling()), _object);

  // create the collection of points
  Collection<xy_point> points;


  float prevYVal = 0;
  float prevXVal = 0;
  while (x!=NULL && y!=NULL) {
    xy_point xy;
    xy.x = evaluate(XMLTranscode(x), _object);
    xy.y = evaluate(XMLTranscode(y), _object);

    if (xy.x - prevXVal < 0) { // flag to keep previous xval
      xy.x = prevXVal * 1.01;
    }
    if (xy.y < 0) { // flag to keep previous yval
      xy.y = prevYVal;
    }
    prevXVal = xy.x;
    prevYVal = xy.y;

    points.add(xy);
    x = x->getNextElementSibling();
    y = y->getNextElementSibling();

  }

  // create the collection of segments
  Collection<envelope_segment> segments;
  while (t!=NULL && p!=NULL) {
    envelope_segment seg;

    if ( XMLTranscode(t).compare("LINEAR")==0) {
      seg.interType = LINEAR;
    }
    else if (XMLTranscode(t).compare("SPLINE")==0) {
      seg.interType = CUBIC_SPLINE;
    }
    else if (XMLTranscode(t).compare("EXPONENTIAL")==0) {
      seg.interType = EXPONENTIAL;
    }
    else {
      cerr << "Error in MakeEnvelope: Unrecognized interpolation type" << endl;
      exit(1);
    }

    if (XMLTranscode(p).compare("FIXED")==0) {
      seg.lengthType = FIXED;
    }
    else if (XMLTranscode(p).compare("FLEXIBLE")==0) {
      seg.lengthType = FLEXIBLE;
    }
    else {
      cerr << "Error in MakeEnvelope: Unrecognized envelope length stretch type." << endl;
      exit(1);
    }

    segments.add(seg);

    t = t->getNextElementSibling();
    p = p->getNextElementSibling();
  }

  // Create a new envelope given the points and segments defined
  Envelope* madeEnv = new Envelope(points, segments);
  madeEnv->scale(scale);

  // Clean up the temporary point and segment collections
  points.clear();
  segments.clear();

  return madeEnv;

}

//get the envelope shape in env lib corresponding to env_num
Envelope* Utilities::getEnvelopeshape(int env_num, double scale){
    Envelope* env = envelopeLibrary->getEnvelope(env_num);
    if (env == NULL){
      cout << "Error in getEnvelopeShape: env_num exceeds size of EnvLibrary" << endl;
      return NULL;
    }
    env->scale(scale);
    return env;
}
