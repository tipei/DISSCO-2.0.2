/*
CMOD (composition module)
   Copyright (C) 2007  Sever Tipei (s-tipei@uiuc.edu)
      
   
   Update:
   This class is no longer in used by XML version of CMOD. It's here only
   for people who need to upgrade their DISSCO projects from the old format
   to the new format (See LASSIE/src/UpgradeProjectFormat.h ).
   
                                            --Ming-ching Chiu May 06 2013
                                            
                                            

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
//   filevalue.cpp
//
//----------------------------------------------------------------------------//

#include "FileValue.h"
#include "Random.h"
#include "Event.h"

//----------------------------------------------------------------------------//


EnvelopeLibrary envlib_cmod;
int FileValue::segmentNum = 0;

//----------------------------------------------------------------------------//

FileValue::FileValue(const FileValue &v)
{
  orig_type = v.orig_type;
  return_type = v.return_type;

  ftnString = v.ftnString;
  ftnArgs = v.ftnArgs;

  s = v.s;
  n = v.n;
  l = v.l;

  if (v.obj == NULL) {
    obj = NULL;
  } else {
    switch (return_type) {
      case FVAL_ENVELOPE:
        obj = new Envelope(*((Envelope *)(v.obj)));
        break;
      case FVAL_SIEVE:
        obj = new Sieve(*((Sieve *)(v.obj)));
        break;
      case FVAL_PATTERN:
        obj = new Patter(*((Patter *)(v.obj)));
        break;
      default:
        obj = NULL;
    }
  }
}

//----------------------------------------------------------------------------//

FileValue& FileValue::operator=(const FileValue &v)
{
  orig_type = v.orig_type;
  return_type = v.return_type;

  ftnString = v.ftnString;
  ftnArgs = v.ftnArgs;

  s = v.s;
  n = v.n;
  l = v.l;

  if (v.obj == NULL) {
    obj = NULL;
  } else {
    switch (return_type) {
      case FVAL_ENVELOPE:
        obj = new Envelope(*((Envelope *)(v.obj)));
        break;
      case FVAL_SIEVE:
        obj = new Sieve(*((Sieve *)(v.obj)));
        break;
      case FVAL_PATTERN:
        obj = new Patter(*((Patter *)(v.obj)));
        break;
      default:
        obj = NULL;
    }
  }

  return *this;
}

//----------------------------------------------------------------------------//

string FileValue::toString() {
  string result;

  if (orig_type == FVAL_FUNC) {
    result = "Function evaluated. ";
    switch (return_type) {
      case FVAL_NULL:
        result += "Null return type!";
        break;
      case FVAL_NUMBER:
        result += "Returned a number: ";
        //result += n;
        break;
      case FVAL_STRING:
        result += "Returned a string: " + s;
        break;
      case FVAL_LIST:
        result += "Returned a list, length = " + l.size();
        break;
      case FVAL_ENVELOPE:
        result += "Returned an envelope.";
        break;
      case FVAL_SIEVE:
        result += "Returned a sieve.";
        break;
      case FVAL_PATTERN:
        result += "Returned a pattern.";
        break;
      default:
        break;
    }
  } else {
    result = "Type is: " + TypeToString();
  }

  return result;
}

//----------------------------------------------------------------------------//

FileValue::~FileValue()
{
  if (return_type == FVAL_ENVELOPE) {
    Envelope* tmpEnv = (Envelope*)obj;
    if (tmpEnv) delete tmpEnv;
  } else if (return_type == FVAL_SIEVE) {
    Sieve* tmpSiv = (Sieve*)obj;
    if (tmpSiv) delete tmpSiv;
  } else if (return_type == FVAL_PATTERN) {
    Patter* tmpPat = (Patter*)obj;
    if (tmpPat) delete tmpPat;
  }
}

//----------------------------------------------------------------------------//

const string FileValue::TypeToString() const {
  switch (return_type) {
    case FVAL_NUMBER:
      return "Number";
    case FVAL_STRING:
      return "String";
    case FVAL_FUNC:
      return "Function";
    case FVAL_LIST:
      return "List";
    case FVAL_ENVELOPE:
      return "Envelope";
    case FVAL_SIEVE:
      return "Sieve";
    case FVAL_PATTERN:
      return "Pattern";
    default:
      return "Invalid Type";
  }
}

//----------------------------------------------------------------------------//

vector<FileValue*> FileValue::EvaluateArgs(int n, ...) {
  vector<FileValue*> argVect;
  if (orig_type != FVAL_FUNC) {
    cerr << "Unable to evaluate function arguments --- orig_type != function" << endl;
    if (evptr != NULL) cerr << "      (in file " << evptr->getEventName() << ")" << endl;
    exit(1);
  }

  va_list ap;
  va_start(ap, n);

  if (ftnArgs.size() != n) {
    cerr << "FileValue::EvaluateArgs - Wrong # of arguments passed to "
         << ftnString << ", expected:" << n << ", got:" << ftnArgs.size() << endl;
    if (evptr != NULL) cerr << "      (in file " << evptr->getEventName() << ")" << endl;
    exit(1);
  }

  list<FileValue>::iterator iter = ftnArgs.begin();
  int argNum = 1;
  while (iter != ftnArgs.end()) {
    iter->Evaluate(evptr);
    int arg_type = va_arg(ap, int);

    if (arg_type != iter->getReturnType()) {
      cerr << "FileValue::EvaluateArgs - Argument " << argNum << " of " 
           << ftnString << " has incorrect return_type (return_type= " << iter->getReturnType()
           << ")" << endl;
      if (evptr != NULL) cerr << "      (in file " << evptr->getEventName() << ")" << endl;
      exit(1);
    }

    argVect.push_back(&(*iter));

    iter++;
    argNum++;
  }

  va_end(ap);
  return argVect;
}

//----------------------------------------------------------------------------//

void FileValue::Evaluate(Event *ev) {
  evptr = ev;
  Evaluate();
  evptr = NULL;
}

//----------------------------------------------------------------------------//

void FileValue::Evaluate() {

  // this method only computes functions
  if (orig_type != FVAL_FUNC) {
    return_type = orig_type; 
    return;
  }

  if (ftnString == "Random") {
    ftn_Random();
  } else if (ftnString == "RandomInt") {
    ftn_RandomInt();
  } else if (ftnString == "RandomSeed") {
    ftn_RandomSeed();
  } else if (ftnString == "Randomizer") {
    ftn_Randomizer();
  } else if (ftnString == "Inverse") {
    ftn_Inverse();
  } else if (ftnString == "LN") {
    ftn_LN();
  } else if (ftnString == "Fibonacci") {
    ftn_Fibonacci();
  } else if (ftnString == "Decay") {
    ftn_Decay();
  } else if (ftnString == "Stochos") {
    ftn_Stochos();
  } else if (ftnString == "EnvLib") {
    ftn_EnvLib();
  } else if (ftnString == "Select") {
    ftn_Select();
  } else if (ftnString == "ValuePick") {
    ftn_ValuePick();
  } else if (ftnString == "ChooseL") {
    ftn_ChooseL();
  } else if (ftnString == "GetPattern") {
    ftn_GetPattern();
  } else if (ftnString == "ExpandPattern") {
    ftn_ExpandPattern();
  } else if (ftnString == "AdjustPattern") {
    ftn_AdjustPattern();
  } else if (ftnString == "MakeEnvelope") { 
    ftn_MakeEnvelope();
  } else if (ftnString == "MakeSieve") {
    ftn_MakeSieve();
  } else if (ftnString == "MakePattern") {
    ftn_MakePattern();
  } else if (ftnString == "MakeList") {
    ftn_MakeList();
  } else if (ftnString == "ReadENVFile") {
    ftn_ReadENVFile();
  } else if (ftnString == "ReadSIVFile") {
    ftn_ReadSIVFile();
  } else if (ftnString == "ReadPATFile") {
    ftn_ReadPATFile();
  } else if (ftnString == "ReadSPAFile") {
    ftn_ReadSPAFile();
  } else if (ftnString == "ReadREVFile") {
    ftn_ReadREVFile();
  /* ----------------------------------- */
  /* ------ STATIC FUNCTIONS ----------- */
  /* ----------------------------------- */
  } else if (ftnString == "CURRENT_TYPE") {
    static_ftn_CURRENT_TYPE();
  } else if (ftnString == "CURRENT_CHILD_NUM") {
    static_ftn_CURRENT_CHILD_NUM();
  } else if (ftnString == "CURRENT_PARTIAL_NUM") {
    static_ftn_CURRENT_PARTIAL_NUM();
  } else if (ftnString == "CURRENT_DENSITY") {
    static_ftn_CURRENT_DENSITY();
  } else if (ftnString == "CURRENT_SEGMENT") {
    static_ftn_CURRENT_SEGMENT();
  } else if (ftnString == "DURATION_UNITS") { //Deprecated--use AVAILABLE_EDU
    static_ftn_DURATION_UNITS();              //Deprecated--use AVAILABLE_EDU
  } else if (ftnString == "DURATION_EDU") { //Deprecated--use AVAILABLE_EDU
    static_ftn_DURATION_EDU();              //Deprecated--use AVAILABLE_EDU
  } else if (ftnString == "AVAILABLE_EDU") {
    static_ftn_AVAILABLE_EDU();
  } else if (ftnString == "PREVIOUS_CHILD_DURATION"){
    static_ftn_PREVIOUS_CHILD_DURATION();
  } else if (ftnString == "CURRENT_LAYER") {
    static_ftn_CURRENT_LAYER();
  } else {
    cerr << "Function " << ftnString << " not implemented! Exiting" << endl;
    exit(1);
  }


  /////////////////////////////////////////////////////////////////////
  /* NOTE: THIS ISN't WORTH THE CPU-SAVINGS... JUST RECOMPUTE EVERY  */
  /*   FUNCTION EVERY TIME!!! -- cavis                               */
  /*  //check if the inner args of the function are functions themselves
      // ... and if they're not, make sure this function doesn't get recalculated
      //     by setting orig_type to something other than FVAL_FUNC
      bool recompute = false;
      list<FileValue>::iterator iter = ftnArgs.begin();
      while (iter != ftnArgs.end()) {
        if (iter->getOrigType() == FVAL_FUNC) {
          recompute = true;
        }
        iter++;
      }
    
      // but DON'T CLEAR STATIC FUNCTIONS!!!
      if ( ftnString.substr(0, 8) == "CURRENT_" ) {
        recompute = true;
      }
      
      // and DON'T CLEAR RANDOM FUNCTIONS
      if ( ftnString.substr(0,6) == "Random"
          || ftnString == "ChooseL"
          || ftnString == "GetPattern" ) {
        recompute = true;
      }
    
      if (!recompute) {
        orig_type = return_type;
        ftnString.clear();
        ftnArgs.clear();
      }*/
  /////////////////////////////////////////////////////

  // Clear the function name and arguments
  if (return_type != FVAL_NUMBER)
    n = 0;

  if (return_type != FVAL_STRING)
    s.clear();

  if (return_type != FVAL_LIST)
    l.clear();
}

//----------------------------------------------------------------------------//

void FileValue::ftn_Random() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(2, FVAL_NUMBER, FVAL_NUMBER);
  float arg1 = args[0]->getFloat();
  float arg2 = args[1]->getFloat();
  n = Random::Rand(arg1, arg2);
}

//----------------------------------------------------------------------------//

void FileValue::ftn_RandomInt() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(2, FVAL_NUMBER, FVAL_NUMBER);
  int arg1 = args[0]->getInt();
  int arg2 = args[1]->getInt();
  n = Random::RandInt(arg1, arg2);
}

//----------------------------------------------------------------------------//

void FileValue::ftn_RandomSeed() {
  return_type = FVAL_NULL;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_NUMBER);
  float arg1 = args[0]->getFloat();
  if (arg1 < 0.0) {
    n = 0.0;
  } else {
    n = arg1;
  }
  Random::Seed((unsigned int)n);
}

void FileValue::ftn_Randomizer() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(2, FVAL_NUMBER, FVAL_NUMBER);
  float baseVal = args[0]->getFloat();
  float percDev = args[1]->getFloat();

  double devVal = baseVal * percDev;
  n = Random::Rand(baseVal - devVal, baseVal + devVal);
}

//----------------------------------------------------------------------------//

void FileValue::ftn_Inverse() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_NUMBER);
  float arg1 = args[0]->getFloat();
  n = (1. / arg1);
}

//----------------------------------------------------------------------------//

void FileValue::ftn_LN() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_NUMBER);
  double arg1 = args[0]->getFloat();
  n = ( 1. / pow(2.71828, arg1) );
}

//----------------------------------------------------------------------------//

void FileValue::ftn_Fibonacci() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_NUMBER);
  float arg1 = args[0]->getInt();

  int numA = 1;
  int numB = 1;
  for (int i = 3; i <= arg1; i++) {
    int swap = numB;
    numB += numA;
    numA = swap;
  }
  n = numB;
}

//----------------------------------------------------------------------------//

void FileValue::ftn_Decay() {
  return_type = FVAL_NUMBER;
  float decay;
  //float base, string type, float rate, int index
  vector<FileValue*> args = EvaluateArgs(4, FVAL_NUMBER, FVAL_STRING, FVAL_NUMBER, FVAL_NUMBER);
  float base = args[0]->getFloat();
  string type = args[1]->getString();
  float rate = args[2]->getFloat();
  float index = args[3]->getInt();

  if (type == "EXPONENTIAL") {
    decay = base * pow(rate, index);
  } else if (type == "LINEAR") {
    decay = base - (rate * index);
  }
  n = decay;
}

//----------------------------------------------------------------------------//

void FileValue::ftn_Stochos() {
  return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(3, FVAL_STRING, FVAL_LIST, FVAL_NUMBER);

  // Setup vars for stochos routine
  double checkPoint = 0;
  if (evptr != NULL) {
    checkPoint = evptr->getCheckPoint();
    // check for valid checkpoint
    if (checkPoint < 0 || checkPoint > 1) {
      cerr << "FileValue::Stochos -- checkpoint error!" << endl;
      cerr << "   checkPoint = " << checkPoint;
      cerr << ", filename = " << evptr->getEventName();
      cerr << endl;
    }
  }
  string method = args[0]->getString();
  //FileValue tmp = args[1];
  list<FileValue> envList = args[1]->getList();
  int offset = args[2]->getInt();

  vector<Envelope*> envVect;
  list<FileValue>::iterator iter = envList.begin();
  while (iter != envList.end()) {
    envVect.push_back(iter->getEnvelope(evptr));
    iter++;
  }

  float returnVal;
  if(method == "FUNCTIONS") {
    float randomNumber;

    // stacked up envelopes: their values at the same moment add up to 1
    for (int i = 0; i < envVect.size(); i++) {
      returnVal = envVect[i]->getValue(checkPoint, 1.);
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
      if (evptr != NULL) {
        cerr << "       in file " << evptr->getEventName() << endl;
      }
      exit(1);
    } 
    for(int i = 0; i < 2; i++) {
      if(envVect[3 * offset + i] != NULL) {
        limit[i] = envVect[3 * offset + i]->getValue(checkPoint, 1);
      } else {
        cerr << "FileValue::Stochos - NULL envelope. Trying to access envy["
             << 3 * offset + i<< "]=" <<  envVect[3 * offset + i] << endl;
      }
    }

    if(envVect[3 * offset + 2]) {
      returnVal = envVect[3 * offset + 2]->getValue(Random::Rand(), 1);
    } else {
      cerr << "FileValue::Stochos - NULL envelope. Trying to access envVect["
           << 3 * offset + 2 << "]=" << envVect[ 3 * offset + 2] << endl;
    }

    returnVal *= (limit[1] - limit[0]);
    returnVal += limit[0];
  } else {
    cerr << "FileValue::Stochos --- invalid method!  Use FUNCTIONS or RANGE_DISTRIB" << endl;
    exit(1);
  }

  n = returnVal;
}

//----------------------------------------------------------------------------//

void FileValue::ftn_EnvLib() {
  return_type = FVAL_ENVELOPE;
  vector<FileValue*> args = EvaluateArgs(2, FVAL_NUMBER, FVAL_NUMBER);

  int envNum = args[0]->getInt();
  double scale = args[1]->getFloat();

  Envelope *env = new Envelope(envlib_cmod.getEnvelopeRef(envNum));

  if (!env) {
    return_type = FVAL_NULL;
    cout << "FileValue::Evaluate - Envelope number " << envNum << " is null." << endl;
    obj = NULL;
  } else {
    env->scale(scale);
    obj = (void*)env;
  }
}

//----------------------------------------------------------------------------//

void FileValue::ftn_Select() {
  //return_type = FVAL_NUMBER;
  vector<FileValue*> args = EvaluateArgs(2, FVAL_LIST, FVAL_NUMBER);

  list<FileValue>* choiceList = args[0]->getListPtr(evptr);
  int chooseNum = args[1]->getInt(evptr);

  if (chooseNum >= choiceList->size() || chooseNum < 0) {
    cerr << "FileValue -- Error in Select(): index out of bounds of select list" << endl;
    cerr << "                    index = " << chooseNum << ", list has " 
         << choiceList->size() << " items." << endl; 
    if (evptr != NULL) {
      cerr << "                in file " << evptr->getEventName() << endl;
    }
    exit(1);
  }

  list<FileValue>::iterator iter = choiceList->begin();
  for (int i=0; i < chooseNum; i++) {
    iter++;
  }
  iter->Evaluate(evptr);

  if (iter->isNumber()) {
    return_type = FVAL_NUMBER;
    n = iter->getFloat();
  } else if (iter->isList()) {
    return_type = FVAL_LIST;
    l = iter->getList();
  } else if (iter->isString()) {
    return_type = FVAL_STRING;
    s = iter->getString();
  } else if (iter->isEnvelope()) {
    return_type = FVAL_ENVELOPE;
    obj = (void*)iter->getEnvelope(evptr);
  } else if (iter->isPattern()) {
    return_type = FVAL_PATTERN;
    obj = (void*)iter->getPattern(evptr);
  } else if (iter->isSieve()) {
    return_type = FVAL_SIEVE;
    obj = (void*)iter->getSieve(evptr);
  } else {
    cerr << "FileValue::Select() --- invalid type selected by Select()" << endl;
    exit(1);
  }
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ValuePick() {

  return_type = FVAL_NUMBER;
  vector<FileValue*> args;
  
  if (ftnArgs.size() ==10){ // new format with offset
    args = EvaluateArgs(10, FVAL_NUMBER, FVAL_ENVELOPE, FVAL_ENVELOPE, 
                                         FVAL_ENVELOPE, FVAL_STRING, FVAL_LIST, FVAL_STRING, 
                                         FVAL_LIST, FVAL_STRING, FVAL_LIST);
  }
  else { // old format without offset    
   args = EvaluateArgs(9, FVAL_NUMBER, FVAL_ENVELOPE, FVAL_ENVELOPE, 
                                         FVAL_ENVELOPE, FVAL_STRING, FVAL_LIST, FVAL_STRING, 
                                         FVAL_LIST, FVAL_STRING);
  }


  // setup routine vars
  double checkpoint = 0;
  if (evptr != NULL) {
    checkpoint = evptr->getCheckPoint();
  }

  float absRange = args[0]->getFloat();
  Envelope *envLow = args[1]->getEnvelope(NULL);
  Envelope *envHigh = args[2]->getEnvelope(NULL);
  Envelope *envDist = args[3]->getEnvelope(NULL);
  string eMethod = args[4]->getString();
  list<FileValue>* eArgList = args[5]->getListPtr(NULL);
  string wMethod = args[6]->getString();
  list<FileValue>* wArgList = args[7]->getListPtr(NULL);
  string modifyMethod = args[8]->getString();
  //int offset =(ftnArgs.size()==10)? args[9]->getInt(): 0;  //default offset = 0 if the function is in the old format
  
  list<FileValue>::iterator iter;
    vector<int> offsetVect;
  
  if (ftnArgs.size()==10){
    list<FileValue>* offset = args[9]->getListPtr(NULL); 
 
    iter = offset->begin();
    while (iter != offset->end()) {

      offsetVect.push_back( iter->getInt() );
      iter++;
    }
  }
  else { // offset is missing (old format)
    for (int i = 0; i < wArgList->size(); i ++){
      offsetVect.push_back(0);
    }
  }
  
   

  // convert the lists to vectors
  vector<int> eArgVect;
  iter = eArgList->begin();
  while (iter != eArgList->end()) {

    eArgVect.push_back( iter->getInt() );
    iter++;
  }

  vector<int> wArgVect;
  iter = wArgList->begin();

  while (iter != wArgList->end()) {

    wArgVect.push_back( iter->getInt() );
    iter++;
  }
  

  
  

  int minVal = (int)floor( envLow->getScaledValueNew(checkpoint, 1) * absRange + 0.5);
  int maxVal = (int)floor( envHigh->getScaledValueNew(checkpoint, 1) * absRange + 0.5);
//cout << "FileValue: ValuePick - minVal=" << minVal << " maxVal=" << maxVal <<
//	  " absRange=" << absRange << endl;
  Sieve si;
  si.Build(minVal, maxVal, eMethod.c_str(), wMethod.c_str(), eArgVect, wArgVect, offsetVect);

  //use third envelope to choose a value
  n = si.Modify(envDist, modifyMethod);

}

//----------------------------------------------------------------------------//

void FileValue::ftn_ChooseL() {
  return_type = FVAL_NUMBER;

  vector<FileValue*> args = EvaluateArgs(1, FVAL_SIEVE);
  Sieve* svPtr = args[0]->getSieve(NULL);
  n = svPtr->ChooseL();
}

//----------------------------------------------------------------------------//

void FileValue::ftn_GetPattern() {
  //cout<<"evaluate get pattern!"<<endl;
  return_type = FVAL_NUMBER;

  vector<FileValue*> args = EvaluateArgs(3, FVAL_STRING, FVAL_NUMBER, FVAL_PATTERN);
  patterMethod = args[0]->getString(evptr);
  patterOrigin = args[1]->getInt(evptr);
  cout<<"origin = "<<patterOrigin<<endl;
  Patter* patPtr = args[2]->getPattern(evptr);
  obj = patPtr;
  //n = patPtr->GetNextValue( patterMethod, patterOrigin );
   
  
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ExpandPattern() {
  return_type = FVAL_PATTERN;

  vector<FileValue*> args = EvaluateArgs(5, FVAL_STRING, FVAL_NUMBER, FVAL_NUMBER, FVAL_NUMBER, FVAL_PATTERN);
  string method = args[0]->getString();
  int mod = args[1]->getInt();
  int low = args[2]->getInt();
  int high = args[3]->getInt();

  Patter* oldPatPtr = args[4]->getPattern(evptr);

  oldPatPtr->Expand( method, mod, low, high );

  obj = (void*)oldPatPtr;
}

//----------------------------------------------------------------------------//

void FileValue::ftn_AdjustPattern() {
  return_type = FVAL_PATTERN;

  cerr << "Error: AdjustPattern function not implemented yet!" << endl;
  exit(1);
}

//----------------------------------------------------------------------------//

void FileValue::ftn_MakeEnvelope() {
  return_type = FVAL_ENVELOPE;

  /*  Arg1: <points for x>  ( note: 1 more points than segments)
      Arg2: <points for y> 
      Arg3: <interpolation types>
      Arg4: <flex or fixed length>
      Arg5: scale value for envelope  */
  vector<FileValue*> args = EvaluateArgs(5, FVAL_LIST, FVAL_LIST, FVAL_LIST, 
                                         FVAL_LIST, FVAL_NUMBER);

  list<FileValue>* xPoints = args[0]->getListPtr(NULL);
  list<FileValue>* yPoints = args[1]->getListPtr(NULL);
  list<FileValue>* interpTypes = args[2]->getListPtr(NULL);
  list<FileValue>* flexOrFix = args[3]->getListPtr(NULL);
  float scaleFactor = args[4]->getFloat();

  // Check for correct list sizes
  if (xPoints->size() != yPoints->size() || 
      interpTypes->size() != flexOrFix->size()) {	
    cerr << "Error evaluating MakeEnvelope: Parameter list lengths"
         << " are not correct." << endl;
    exit(1);
  }

  // create the collection of points
  Collection<xy_point> points;
  list<FileValue>::iterator xIter = xPoints->begin();
  list<FileValue>::iterator yIter = yPoints->begin();

  segmentNum = 0;
  float prevYVal = 0;
  float prevXVal = 0;
  while (xIter != xPoints->end() && yIter != yPoints->end()) {
    xy_point xy;
    xy.x = xIter->getFloat(evptr);
    xy.y = yIter->getFloat(evptr);

    if (xy.x < 0) { // flag to keep previous xval
      xy.x = prevXVal * 1.01;
    }
    if (xy.y < 0) { // flag to keep previous yval
      xy.y = prevYVal;
    }
    prevXVal = xy.x;
    prevYVal = xy.y;

    points.add(xy);
    xIter++;
    yIter++;
    segmentNum++;
  }

  // create the collection of segments
  Collection<envelope_segment> segments;
  list<FileValue>::iterator interpIter = interpTypes->begin();
  list<FileValue>::iterator ffIter = flexOrFix->begin();

  segmentNum = 0;
  while (interpIter != interpTypes->end() && ffIter != flexOrFix->end()) {
    envelope_segment seg;

    if (interpIter->getString() == "LINEAR") {
      seg.interType = LINEAR;
    } else if (interpIter->getString() == "CUBIC_SPLINE") {
      seg.interType = CUBIC_SPLINE;
    } else if (interpIter->getString() == "EXPONENTIAL") {
      seg.interType = EXPONENTIAL;
    } else {
      cerr << "Error in MakeEnvelope: Unrecognized interpolation type" << endl;
      exit(1);
    }

    if (ffIter->getString() == "FIXED") {
      seg.lengthType = FIXED;
    } else if (ffIter->getString() == "FLEXIBLE") {
      seg.lengthType = FLEXIBLE;
    } else {
      cerr << "Error in MakeEnvelope: Unrecognized envelope length stretch type." << endl;
      exit(1);
    }

    segments.add(seg);

    interpIter++;
    ffIter++;
    segmentNum++;
  }

  // Create a new envelope given the points and segments defined
  Envelope* madeEnv = new Envelope(points, segments);
  madeEnv->scale(scaleFactor);

  obj = (void*)madeEnv;

  // Clean up the temporary point and segment collections
  points.clear();
  segments.clear();
}

//----------------------------------------------------------------------------//

void FileValue::ftn_MakeSieve() {
  return_type = FVAL_SIEVE;

  /*  Arg1: int minValue
      Arg2: int maxValue
      Arg3: string elementMethod
      Arg4: <elementArgVector>
      Arg5: string weightMethod
      Arg6: <weightArgVector> 
      Arg7: <offsetVector> offset (only exist in the new format)
      */

  vector<FileValue*> args;
        
  if (ftnArgs.size() ==7){ // new format with offset
    args = EvaluateArgs(7, FVAL_NUMBER, FVAL_NUMBER, FVAL_STRING, 
                           FVAL_LIST, FVAL_STRING, FVAL_LIST,FVAL_LIST);
  }
  else { // old format
    args = EvaluateArgs(6, FVAL_NUMBER, FVAL_NUMBER, FVAL_STRING, 
                           FVAL_LIST, FVAL_STRING, FVAL_LIST);
  
  }                         

  int minVal = args[0]->getInt();
  int maxVal = args[1]->getInt();
  string eMethod = args[2]->getString();
  list<FileValue>* eArgs = args[3]->getListPtr(NULL);
  string wMethod = args[4]->getString();
  list<FileValue>* wArgs = args[5]->getListPtr(NULL);
  //int offset = (ftnArgs.size()==7)? args[6]->getInt(): 0; // default offset = 0 if it's old format

  list<FileValue>::iterator iter;
  vector<int> offsetVect;
  
  if (ftnArgs.size()==7){
    list<FileValue>* offset = args[6]->getListPtr(NULL); 
 
    iter = offset->begin();
    while (iter != offset->end()) {

      offsetVect.push_back( iter->getInt() );
      iter++;
    }
  }
  else { // offset is missing (old format)
    for (int i = 0; i < wArgs->size(); i ++){
      offsetVect.push_back(0);
    }
  }


  // convert the args into vectors of ints
  vector<int> eArgInts;
  vector<int> wArgInts;
  iter = eArgs->begin();
  while (iter != eArgs->end()) {
    eArgInts.push_back( iter->getInt(evptr) );
    iter++;
  }
  iter = wArgs->begin();
  while (iter != wArgs->end()) {
    wArgInts.push_back( iter->getInt(evptr) );
    iter++;
  }

  Sieve * siv = new Sieve();
  siv->Build(minVal, maxVal, eMethod.c_str(), wMethod.c_str(), eArgInts, wArgInts, offsetVect);

  obj = (void *)siv;
}

//----------------------------------------------------------------------------//

void FileValue::ftn_MakePattern() {
  /* <list intervals> */
  return_type = FVAL_PATTERN;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_LIST);

  list<FileValue>* intervalsFV = args[0]->getListPtr(NULL);

  // change intervals into a vector of ints
  vector<int> intervalsVect;
  list<FileValue>::iterator iter = intervalsFV->begin();
  while (iter != intervalsFV->end()) {
    intervalsVect.push_back( iter->getInt(evptr) );
    iter++;
  }

  Patter* newPatter = new Patter(0, intervalsVect);
  newPatter->SimplePat(); // create a simple pattern

  obj = (void*)newPatter;
}

//----------------------------------------------------------------------------//

void FileValue::ftn_MakeList() {
  return_type = FVAL_LIST;

  list<FileValue>::iterator iter = ftnArgs.begin();
  FileValue ftnVal = *iter;
  iter++;
  int listLen = iter->getInt(evptr);

  l.clear();
  for (int i = 0; i < listLen; i++) {
    l.push_back( ftnVal );
  }
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ReadENVFile() {
  return_type = FVAL_ENVELOPE;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_STRING);
  string fileName = args[0]->getString();
  /*
  EventFactory* tmpFact = factory_lib[fileName];
  if (tmpFact == NULL) {
    tmpFact = new EventFactory(fileName);
  }
  FileValue* tmpfv = tmpFact->getEnvelopeBuilder();

  setEnvelope( tmpfv->getEnvelope(evptr) );
  */
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ReadSIVFile() {
  return_type = FVAL_SIEVE;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_STRING);
  string fileName = args[0]->getString();
  /*
  EventFactory* tmpFact = factory_lib[fileName];
  if (tmpFact == NULL) {
    tmpFact = new EventFactory(fileName);
  }
  FileValue* tmpfv = tmpFact->getSieveBuilder();
/*  FileValue* tmpfv = tmpFact->getSieveBuild ??(); 		*/

  //setSieve( tmpfv->getSieve(evptr) );
  
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ReadPATFile() {
  return_type = FVAL_PATTERN;

  list<FileValue>::iterator iter = ftnArgs.begin();
  string fileName = iter->getString();
  iter++;
  /*
  EventFactory* tmpFact = factory_lib[fileName];
  if (tmpFact == NULL) {
    tmpFact = new EventFactory(fileName);
  }
  FileValue* tmpfv = tmpFact->getPatternBuilder();

  Patter* myPat = tmpfv->getPattern(evptr);

  if (iter != ftnArgs.end()) {
    myPat->moveOrigin( iter->getInt(evptr) );
  }

  setPattern( myPat );
  */
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ReadSPAFile() {
  return_type = FVAL_LIST;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_STRING);
  string fileName = args[0]->getString();
  /*
  EventFactory* tmpFact = factory_lib[fileName];
  if (tmpFact == NULL) {
    tmpFact = new EventFactory(fileName);
  }
  FileValue* tmpfv = tmpFact->getSpatialization();

  list<FileValue>* spaList = tmpfv->getListPtr(evptr);
  setList( *spaList );
  */
}

//----------------------------------------------------------------------------//

void FileValue::ftn_ReadREVFile() {
  return_type = FVAL_LIST;
  vector<FileValue*> args = EvaluateArgs(1, FVAL_STRING);
  string fileName = args[0]->getString();
  /*
  EventFactory* tmpFact = factory_lib[fileName];
  if (tmpFact == NULL) {
    tmpFact = new EventFactory(fileName);
  }
  FileValue* tmpfv = tmpFact->getReverberation();

  list<FileValue>* revList = tmpfv->getListPtr(evptr);
  setList( *revList );
  */
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_CURRENT_TYPE() {
  if (evptr == NULL) {
    return_type = FVAL_NULL;
  } else {
    return_type = FVAL_NUMBER;
    n = evptr->getCurrentChildType();
  }
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_CURRENT_CHILD_NUM() {
  if (evptr == NULL) {
    return_type = FVAL_NULL;
  } else {
    return_type = FVAL_NUMBER;
    n = evptr->getCurrentChild();
  }
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_CURRENT_PARTIAL_NUM() {
  if (evptr == NULL) {
    return_type = FVAL_NULL;
  } else {
    return_type = FVAL_NUMBER;
    n = evptr->getCurrPartialNum();
  }
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_CURRENT_DENSITY() {
  if (evptr == NULL) {
    return_type = FVAL_NULL;
  } else {
    return_type = FVAL_NUMBER;
  }
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_CURRENT_SEGMENT() {
  return_type = FVAL_NUMBER;
  n = segmentNum;
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_DURATION_UNITS() { //Deprecated--use AVAILABLE_EDU
  if (evptr == NULL) {
    cerr << "FileValue Error: DURATION_UNITS " //Deprecated--use AVAILABLE_EDU
      << "used outside of Event context" << endl;
    exit(1);
  }
  cout << "Warning: DURATION_UNITS now returns duration in EDU." << endl;
  cout << "You should use the new AVAILABLE_EDU function instead." << endl;
  return_type = FVAL_NUMBER;
  n = evptr->getAvailableEDU();
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_DURATION_EDU() { //Deprecated--use AVAILABLE_EDU
  if (evptr == NULL) {
    cerr << "FileValue Error: DURATION_EDU " //Deprecated--use AVAILABLE_EDU
      << "used outside of Event context" << endl;
    exit(1);
  }
  cout << "Warning: DURATION_EDU now returns duration in EDU." << endl;
  cout << "You should use the new AVAILABLE_EDU function instead." << endl;
  return_type = FVAL_NUMBER;
  n = evptr->getAvailableEDU();
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_AVAILABLE_EDU() {
  if (evptr == NULL) {
    cerr << "FileValue Error: AVAILABLE_EDU used outside of Event context" 
         << endl;
    exit(1);
  }
  return_type = FVAL_NUMBER;
  n = evptr->getAvailableEDU();
}

//----------------------------------------------------------------------------//

void FileValue::static_ftn_CURRENT_LAYER() {
  if (evptr == NULL) {
    cerr << "FileValue Error: CURRENT_LAYER used outside of Event context" 
        << endl;
    exit(1);
  }
  return_type = FVAL_NUMBER;
  n = evptr->getCurrentLayer();
}

void FileValue::static_ftn_PREVIOUS_CHILD_DURATION() {
  if (evptr == NULL) {
    cerr << "FileValue Error: PREVIOUS_CHILD_DURATION used outside of Event context" 
        << endl;
    exit(1);
  }
  return_type = FVAL_NUMBER;
  n = evptr->getPreviousChildEndTime();
}




