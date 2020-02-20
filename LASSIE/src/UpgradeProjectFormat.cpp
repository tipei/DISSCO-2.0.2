/*
CMOD (composition module)
2005  Sever Tipei (s-tipei@uiuc.edu)
2013  Ming-ching Chiu

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
//  UpgradeFileFormat.cpp
//
//  This program convert the projects from the old FileValue format to the new
//  XML Format.
//
//---------------------------------------------------------------------------//

#include "../../CMOD/src/CMOD.h"
#include "../../CMOD/src/Libraries.h"
#include "../../CMOD/src/parser/lex.yy.c"
#include "UpgradeProjectFormat.h"



int main(int parameterCount, char **parameterList) {

  //Determine settings.
  cout << endl;
  cout << "=========================SETTINGS==========================" << endl;

  //Determine the project path.
  string path;
  if(parameterCount >= 2)
    path = parameterList[1];
  if(path == "--help" || path == "-help" || path == "help") {
    cout << "Usage: UpgradeProjectFormat" <<endl;
    cout << "       UpgradeProjectFormat <path>   Runs in the <path> directory." << endl;
    cout << "       UpgradeProjectFormat help   Displays this help." << endl;
    return 0;
  }
  path = PieceHelper::getFixedPath(path);
  cout << "Working in path: " << path << endl;



  //Determine the project name.
  string projectName = PieceHelper::getProjectName(path);
  if(projectName == "")
    return 0;
  cout << "Project name: " << projectName << endl;

  string disscoXMLString = path + projectName + ".dissco";
  FILE* disscoXML = fopen ( disscoXMLString.c_str(), "w");


  //Convert Project dat
  string datFileName = path + projectName + ".dat";


  //YY_FLUSH_BUFFER;//flush the buffer make sure the buffer is clean
  FILE *yytmp;
  extern FILE *yyin;
  yytmp = fopen(datFileName.c_str(), "r");

  if (yytmp == NULL) {
    cout << "ERROR: File " << datFileName << " does not exist!" << endl;
    exit(1);
  }


  yyin = yytmp;
  //extern map<const char*, FileValue*, ltstr> file_data;
  yyparse();
  fclose(yyin);

  std::string stringbuffer = "<?xml version='1.0'?>\n"
                             "<ProjectRoot>\n"
                             "  <ProjectConfiguration>\n";


  FileValue* value;

  value = file_data["LASSIETITLE"];
  stringbuffer = stringbuffer + "    <Title>" + value->getString() + "</Title>\n";
  value = file_data["LASSIEFILEFLAGS"];
  stringbuffer = stringbuffer + "    <FileFlag>" + value->getString() + "</FileFlag>\n";

  //topEvent      = piece->fileList
  value = file_data["LASSIEFILELIST"];
  stringbuffer = stringbuffer + "    <TopEvent>" + value->getString() + "</TopEvent>\n    <PieceStartTime>0</PieceStartTime>\n";

  value = file_data["LASSIEFILEMEASURE"];
  stringbuffer = stringbuffer + "    <Measure>" + value->getString() + "</Measure>\n";

  value = file_data["LASSIEPIECEDURATION"];
  string buffer2 = convertFunctionFromLASSIEMetaData(value->getString());
  if (buffer2 =="") cout<<"Warning! PieceDuration in the .dat file is not valid."<<endl;
  stringbuffer = stringbuffer + "    <Duration>" + buffer2 + "</Duration>\n";

  value = file_data["LASSIESOUNDSYNTHESIS"];
  buffer2 = value->getString();
  stringbuffer = stringbuffer + "    <Synthesis>" + ((buffer2 =="TRUE")?"True":"False") + "</Synthesis>\n";

  value = file_data["LASSIESCOREPRINT"];
  buffer2 = value->getString();
  stringbuffer = stringbuffer + "    <Score>" + ((buffer2 =="TRUE")?"True":"False") + "</Score>\n";



  value = file_data["LASSIENUMCHANNELS"];
  stringbuffer = stringbuffer + "    <NumberOfChannels>" + value->getString() + "</NumberOfChannels>\n";


  value = file_data["LASSIESAMPLERITE"];
  stringbuffer = stringbuffer + "    <SampleRate>" + value->getString() + "</SampleRate>\n";

  value = file_data["LASSIESAMPLESIZE"];
  stringbuffer = stringbuffer + "    <SampleSize>" + value->getString() + "</SampleSize>\n";

  value = file_data["LASSIENUMTHREADS"];
  stringbuffer = stringbuffer + "    <NumberOfThreads>" + value->getString() + "</NumberOfThreads>\n    <OutputParticel>False</OutputParticel>\n    <Seed></Seed>\n  </ProjectConfiguration>\n";



  fputs(stringbuffer.c_str(),disscoXML);

  stringbuffer = path + ".noteModifiersConfiguration";

  yytmp = fopen(stringbuffer.c_str(), "r");
  string buffer3;
  if (yytmp == NULL) {
    cout << "ERROR: File " << stringbuffer << " does not exist!" << endl;
    buffer2 = "1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1";
    buffer3 = "";
  }

  else{

    yyin = yytmp;
    yyrestart (yyin);
    yyparse();
    fclose (yyin);



    buffer2 = "";
    value = file_data["LASSIENOTEDEFAULTMODIFIER"];


    std::list<FileValue> valueList = value->getList();
    std::list<FileValue>::iterator valueListIter = valueList.begin();
    while ( valueListIter != valueList.end()){

      char charbuffer [1];
      sprintf(charbuffer, "%d",  (*valueListIter).getInt());

      buffer3 = string(charbuffer);
      buffer2 = buffer2 + buffer3;
      valueListIter++;
      if (valueListIter!=valueList.end()){
        buffer2 = buffer2 + ", ";
      }
    }

    buffer3 = "";
    value = file_data["LASSIENOTECUSTOMMODIFIER"];
    if (value){
      valueList = value->getList();
      valueListIter = valueList.begin();
      for (valueListIter;valueListIter != valueList.end(); valueListIter++){
        buffer3 = buffer3 + "      <Modifier>" +(*valueListIter).getString()+"</Modifier>\n";
      }

    }

  }
  stringbuffer = "  <NoteModifiers>\n"
                   "    <DefaultModifiers>" + buffer2 + "</DefaultModifiers>\n"
                   "    <CustomModifiers>\n" + buffer3 +
                   "    </CustomModifiers>\n"
                   "  </NoteModifiers>\n";
  fputs(stringbuffer.c_str(),disscoXML);


  stringbuffer = path + projectName + ".lib";


  FILE* libFile = fopen(stringbuffer.c_str(), "r");
  std::ostringstream contents;
  std::ifstream file(stringbuffer.c_str());
  if ( !file.is_open() ) {
      cout<<"lib file not opened"<<endl;
  }
  contents << file.rdbuf();
  stringbuffer = "  <EnvelopeLibrary>\n" + contents.str() + "  </EnvelopeLibrary>\n";
  fputs (stringbuffer.c_str(), disscoXML);
  fclose(libFile);
  stringbuffer = "  <Events>\n";
  fputs (stringbuffer.c_str(), disscoXML);


  //  convert all events
  std::string directory;
  DIR *dp;
  struct dirent *dirp;
  // make Top Events
  directory = path+ "T";
  string eventPathAndName;
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;

      fputs ((convertEvent(eventPathAndName, eventTop)).c_str(), disscoXML);
    }
  }

  closedir(dp);


  //  make High Events
  directory = path+ "H";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventHigh)).c_str(), disscoXML);
    }
  }

  closedir(dp);


  //  make Mid Events
  directory = path+ "M";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventMid)).c_str(), disscoXML);
    }
  }

  closedir(dp);

  //  make Low Events
 directory = path+ "L";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventLow)).c_str(), disscoXML);
    }
  }

  closedir(dp);

  //  make Bottom Events
 directory = path+ "B";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventBottom)).c_str(), disscoXML);
    }
  }

  closedir(dp);

  //  make Sound Events


 directory = path+ "S";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventSound)).c_str(), disscoXML);
    }
  }

  closedir(dp);

  //make note event
 directory = path+ "N";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventNote)).c_str(), disscoXML);
    }
  }

  closedir(dp);


  //  make Env Events
 directory = path+ "ENV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventEnv)).c_str(), disscoXML);
    }
  }

  closedir(dp);
  //  make Siv Events
 directory = path+ "SIV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventSiv)).c_str(), disscoXML);
    }
  }

  closedir(dp);
  //  make Pat Events
 directory = path+ "PAT";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventPat)).c_str(), disscoXML);
    }
  }

  closedir(dp);

  //  make Spa Events
 directory = path+ "SPA";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventSpa)).c_str(), disscoXML);
    }
  }

  closedir(dp);
  //  make Rev Events
 directory = path+ "REV";
  if((dp  = opendir(directory.c_str())) == NULL) {
     std::cout << "Error opening " << directory << std::endl;
  }
  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_name[0]!= '.'){
      eventPathAndName = directory + "/" + dirp->d_name;
      fputs ((convertEvent(eventPathAndName, eventRev)).c_str(), disscoXML);
    }
  }

  closedir(dp);

  stringbuffer = "  </Events>\n</ProjectRoot>";
  fputs (stringbuffer.c_str(), disscoXML);

  fclose (disscoXML);

  cout<<"Conversion Done."<<endl;
  return 0;

}





string convertEvent(string _eventPathAndName, EventType _type){
  string result = "  <Event orderInPalette=' ";


  YY_FLUSH_BUFFER;//flush the buffer make sure the buffer is clean
  FILE *yytmp;
  yytmp = fopen(_eventPathAndName.c_str(), "r");

  if (yytmp == NULL) {
    cout << "ERROR: File " << _eventPathAndName << " does not exist!" << endl;
    exit(1);
  }



  yyin = yytmp;
  yyrestart (yyin );
  yyparse();
  if (_type >= 5){
    return parseNonEvent(_type);
  }


  char typeBuffer[10];
  sprintf(typeBuffer, "%d", _type);




  FileValue* value;


  value = file_data["LASSIEeventName"];

  std::list<FileValue> nameAndOrderList = value->getList();
  int eventOrderInPalette;

  std::list<FileValue>::iterator iter = nameAndOrderList.begin();
  value = &(*iter);
  string eventName = value->getString();
  if (nameAndOrderList.size()==0){ //old file format
    eventOrderInPalette = rand();
  }
  else {
    iter++;
    value = &(*iter);
    eventOrderInPalette = value->getInt();
  }

  char charBuffer[10];
  sprintf (charBuffer," %d", eventOrderInPalette);

  int tempoMethodFlag;
  int tempoPrefix;
  int tempoNoteValue;
  string tempoFractionEntry1;
  string tempoFractionEntry2;
  string tempoValueEntry;


  value = file_data["LASSIEtempoMethodFlag"];
  if (value != NULL){
    tempoMethodFlag = value ->getInt(); //0 = as note value, 1 = as fraction
    value = file_data["LASSIEtempoPrefix"];
    tempoPrefix =(TempoPrefix) ((value == NULL)? 0: value ->getInt());
    value = file_data["LASSIEtempoNoteValue"];
    tempoNoteValue =(TempoNoteValue)((value == NULL)? 0: value ->getInt());
    value =file_data["LASSIEtempoFractionEntry1"];
    tempoFractionEntry1 = (value == NULL)? "": value->getString();
    value = file_data["LASSIEtempoFractionEntry2"];
    tempoFractionEntry2 = (value == NULL)? "": value->getString();
    value = file_data["LASSIEtempoValueEntry"];
    tempoValueEntry = (value == NULL)? "": value->getString();
  }
  else{ // can't find lassie metadata
    size_t whereIsSlash;
    size_t whereIsEqualSign;
    value = file_data["tempo"];
    if (value != NULL){
      std::string stringbuffer = value->getString();
      whereIsSlash = stringbuffer.find("/");
      whereIsEqualSign = stringbuffer.find("=");

      if (whereIsSlash==string::npos){ // it's in "note value
        tempoMethodFlag = 0;
        std::string firstHalf =
          value->getString().substr(0, int (whereIsSlash));

        if(firstHalf.find("thirt") != string::npos)
            tempoNoteValue = tempoNoteValueThirtySecond;
        else if(firstHalf.find("six") != string::npos)
          tempoNoteValue = tempoNoteValueSixteenth;
        else if(firstHalf.find("eig") != string::npos)
          tempoNoteValue = tempoNoteValueEighth;
        else if(firstHalf.find("quar") != string::npos)
          tempoNoteValue = tempoNoteValueQuarter;
        else if(firstHalf.find("hal") != string::npos)
          tempoNoteValue = tempoNoteValueHalf;
        else if(firstHalf.find("who") != string::npos)
          tempoNoteValue = tempoNoteValueWhole;

        if(firstHalf.find("doub") != string::npos)
              tempoPrefix = tempoPrefixDoubleDotted;
        else if(firstHalf.find("dot") != string::npos)
              tempoPrefix = tempoPrefixDotted;
        else if(firstHalf.find("tripl") != string::npos)
              tempoPrefix = tempoPrefixTriple;
        else if(firstHalf.find("quin") != string::npos)
          tempoPrefix = tempoPrefixQuintuple;
        else if(firstHalf.find("sext") != string::npos)
          tempoPrefix = tempoPrefixSextuple;
        else if(firstHalf.find("sept") != string::npos)
          tempoPrefix = tempoPrefixSeptuple;
        else tempoPrefix = tempoPrefixNone;
      }
      else { //it's in fractional
        tempoMethodFlag = 1;
        tempoFractionEntry1 = value->getString().substr(0, int(whereIsSlash));
        tempoFractionEntry2 = value->getString().substr(
          int(whereIsSlash)+1, int(whereIsEqualSign) -1 - int(whereIsSlash)  );
      }
      tempoValueEntry =value->getString().substr(
        int(whereIsEqualSign)+1, value->getString().length()-1);
    }

  }

  string maxChildDur;
  value = file_data["LASSIEmaxChildDur"];
  if (value!= NULL){
    maxChildDur = convertFunctionFromLASSIEMetaData(value->getString());
  }
  else {
    value = file_data["maxChildDur"];
    maxChildDur = (value == NULL)?"": convertFunction(value->getString());
  }

  string unitsPerSecond;
  value = file_data["LASSIEEDUPerBeat"];
  if (value!= NULL){
    unitsPerSecond = convertFunctionFromLASSIEMetaData(value->getString());
  }
  else {
    value = file_data["EDUPerBeat"];
    unitsPerSecond =(value == NULL)? "":  convertFunction(value->getString());
  }



  string timeSignatureEntry1;
  string timeSignatureEntry2;

  value = file_data["LASSIEtimeSignatureEntry1"];
  if (value!= NULL){
    timeSignatureEntry1 = value->getString();
    value = file_data["LASSIEtimeSignatureEntry2"];
    timeSignatureEntry2 =(value == NULL)? "": value ->getString();
  }
  else {
    value = file_data["timeSignature"];
    if (value ==NULL){
      timeSignatureEntry1 ="";
      timeSignatureEntry2 ="";
    }
    else {
      size_t whereIsSlash;
      size_t whereIsEqualSign;
      std::string stringbuffer = value->getString();
      whereIsSlash = stringbuffer.find("/");

      if (whereIsSlash==string::npos){
        timeSignatureEntry1 =="";
        timeSignatureEntry2 =="";
      }
      else {
        timeSignatureEntry1 = value->getString().substr(0, int(whereIsSlash));
        timeSignatureEntry2 = value->getString().substr(
          int(whereIsSlash)+1, value->getString().length()-1);
      }
    }

  }


  string numChildrenEntry1;
  string numChildrenEntry2;
  string numChildrenEntry3;
  int flagNumChildren;

  value = file_data["LASSIEnumChildrenEntry1"];
  if (value != NULL){
    numChildrenEntry1 = convertFunctionFromLASSIEMetaData(value->getString());
    value = file_data["LASSIEnumChildrenEntry2"];
    numChildrenEntry2 = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

    value = file_data["LASSIEnumChildrenEntry3"];
    numChildrenEntry3 = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());
   // 0 = fixed, 1 = density, 2 = By layer
    value = file_data["LASSIEflagNumChildren"];
    flagNumChildren = (value == NULL)? 0: value->getInt();

  }



  value = file_data["LASSIEchildEventDefEntry1"];
  string childEventDefEntry1 = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

  value = file_data["LASSIEchildEventDefEntry2"];
  string childEventDefEntry2 = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

  value = file_data["LASSIEchildEventDefEntry3"];
  string childEventDefEntry3 = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

  value = file_data["LASSIEchildEventDefAttackSieve"];
  string childEventDefAttackSieve = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

  value = file_data["LASSIEchildEventDefDurationSieve"];
  string childEventDefDurationSieve = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());


  // 0 = continuum, 1 = sweep, 2 = discrete

  value = file_data["LASSIEflagChildEventDef"];
  int flagChildEventDef =(value == NULL)? 0: value ->getInt();

   // 0 = percentage, 1 = units, 2 = seconds

  value = file_data["LASSIEflagChildEventDefStartType"];
  int flagChildEventDefStartType = (value == NULL)? 0: value->getInt();

   // 0 = percentage, 1 = units, 2 = seconds

  value = file_data["LASSIEflagChildEventDefDurationType"];
  int flagChildEventDefDurationType = (value == NULL)? 0: value->getInt();




  char tempoMethodFlagChar[10];
  sprintf(tempoMethodFlagChar, "%d", tempoMethodFlag);

  char tempoPrefixChar[10];
  sprintf(tempoPrefixChar, "%d", tempoPrefix);

  char tempoNoteValueChar[10];
  sprintf(tempoNoteValueChar, "%d", tempoNoteValue);

  char flagNumChildrenChar[10];
  sprintf(flagNumChildrenChar, "%d", flagNumChildren);

  char flagChildEventDefChar[10];
  sprintf(flagChildEventDefChar, "%d", flagChildEventDef);

  char flagChildEventDefStartTypeChar[10];
  sprintf(flagChildEventDefStartTypeChar, "%d", flagChildEventDefStartType);

  char flagChildEventDefDurationTypeChar[10];
  sprintf(flagChildEventDefDurationTypeChar, "%d", flagChildEventDefDurationType);


  string layerbuffer = "";


  value = file_data["LASSIEeventLayers"];

  std::list<FileValue> layersFileValue = value->getList();
  std::list<FileValue>::iterator i = layersFileValue.begin();

  for (;i != layersFileValue.end(); i ++){
    layerbuffer = layerbuffer + convertEventLayer (&(*i));
  }



  string stringbuffer = "    <Event orderInPalette='" + string(charBuffer)+"'>\n"
    "      <EventType>" + string(typeBuffer) + "</EventType>\n"
    "      <Name>" + eventName + "</Name>\n"


    "      <MaxChildDuration>" + maxChildDur + "</MaxChildDuration>\n"
    "      <EDUPerBeat>" + unitsPerSecond + "</EDUPerBeat>\n"
    "      <TimeSignature>\n"
    "        <Entry1>"+ timeSignatureEntry1 + "</Entry1>\n"
    "        <Entry2>"+ timeSignatureEntry2 + "</Entry2>\n"
    "      </TimeSignature>\n"
    "      <Tempo>\n"
    "        <MethodFlag>" + tempoMethodFlagChar + "</MethodFlag>\n"
    "        <Prefix>" + tempoPrefixChar + "</Prefix>\n"
    "        <NoteValue>" + tempoNoteValueChar + "</NoteValue>\n"
    "        <FractionEntry1>" + tempoFractionEntry1 + "</FractionEntry1>\n"
    "        <FractionEntry2>" + tempoFractionEntry2 + "</FractionEntry2>\n"
    "        <ValueEntry>" + tempoValueEntry + "</ValueEntry>\n"
    "      </Tempo>\n"
    "      <NumberOfChildren>\n"
    "        <MethodFlag>"+flagNumChildrenChar +"</MethodFlag>\n"
    "        <Entry1>" + numChildrenEntry1 + "</Entry1>\n"
    "        <Entry2>" + numChildrenEntry2 + "</Entry2>\n"
    "        <Entry3>" + numChildrenEntry3 + "</Entry3>\n"
    "      </NumberOfChildren>\n"
    "      <ChildEventDefinition>\n"
    "        <Entry1>"+ childEventDefEntry1 + "</Entry1>\n"
    "        <Entry2>"+ childEventDefEntry2 + "</Entry2>\n"
    "        <Entry3>"+ childEventDefEntry3 + "</Entry3>\n"
    "        <AttackSieve>"+ childEventDefAttackSieve + "</AttackSieve>\n"
    "        <DurationSieve>" + childEventDefDurationSieve + "</DurationSieve>\n"
    "        <DefinitionFlag>" + flagChildEventDefChar + "</DefinitionFlag>\n"
    "        <StartTypeFlag>" + flagChildEventDefStartTypeChar + "</StartTypeFlag>\n"
    "        <DurationTypeFlag>" + flagChildEventDefDurationTypeChar + "</DurationTypeFlag>\n"
    "      </ChildEventDefinition>\n"
    "      <Layers>\n" + layerbuffer + "      </Layers>\n";


    if (_type == eventBottom){


      value = file_data["LASSIEBOTTOMfrequencyFlag"];
      int frequencyFlag = (value == NULL)? 0: value->getInt();

      //0 = hertz, 1 =] power of two
      value = file_data["LASSIEBOTTOMfrequencyContinuumFlag"];
      int frequencyContinuumFlag = (value == NULL)? 0: value->getInt();

      value = file_data["LASSIEBOTTOMfrequencyEntry1"];
      string frequencyEntry1 = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

      value = file_data["LASSIEBOTTOMfrequencyEntry2"];
      string frequencyEntry2  = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

      value = file_data["LASSIEBOTTOMloudness"];
      string loudness  = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

      value = file_data["LASSIEBOTTOMspatialization"];
      string spatialization  = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

      value = file_data["LASSIEBOTTOMreverb"];
      string reverb  = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());



      value = file_data["LASSIEBOTTOMmodifiers"];


      string modifierBuffer = "";
      if (value != NULL ){
        std::list<FileValue> modifierList = value->getList();
        std::list<FileValue>::iterator modifierListIter =modifierList.begin();

        while (modifierListIter!= modifierList.end()){

          std::list<FileValue> thisModifierList = modifierListIter->getList();
          std::list<FileValue>::iterator modifierIter =thisModifierList.begin();


          int type =  modifierIter->getInt();

          modifierIter++;
          int applyHowFlag = modifierIter->getInt();

          modifierIter++;
          string probability = convertFunctionFromLASSIEMetaData(modifierIter->getString());

          modifierIter++;
          string ampValue = convertFunctionFromLASSIEMetaData(modifierIter->getString());

          modifierIter++;
          string rateValue = convertFunctionFromLASSIEMetaData(modifierIter->getString());
          modifierIter++;
          string width = convertFunctionFromLASSIEMetaData(modifierIter->getString());
          modifierIter++;

          string groupName = "";
          if (modifierIter != thisModifierList.end()){
            groupName=modifierIter->getString();
          }


          char temp1[10];
          char temp2[10];

          sprintf(temp1, "%d", type);
          sprintf(temp2, "%d", applyHowFlag);

          modifierBuffer = modifierBuffer +
              "            <Modifier>\n"
              "              <Type>" + string(temp1) + "</Type>\n"
              "              <ApplyHow>" + string(temp2) + "</ApplyHow>\n"
              "              <Probability>" + probability + "</Probability>\n"
              "              <Amplitude>" + ampValue + "</Amplitude>\n"
              "              <Rate>" + rateValue +"</Rate>\n"
              "              <Width>"+ width + "</Width>\n"
              "              <GroupName>" + groupName + "</GroupName>\n"
              "            </Modifier>\n";


          modifierListIter++;

        }//end while

      } //end if




      char frequencyFlagChar[10];
      sprintf(frequencyFlagChar, "%d", frequencyFlag);
      char frequencyContinuumFlagChar[10];
      sprintf(frequencyContinuumFlagChar, "%d", frequencyContinuumFlag);

      string modifiersbuffer = "        <Modifiers>\n" + modifierBuffer + "        </Modifiers>\n";


      string bottomBuffer =
        "      <ExtraInfo>\n"
        "        <FrequencyInfo>\n"
        "          <FrequencyFlag>" + string (frequencyFlagChar) + "</FrequencyFlag>\n"
        "          <FrequencyContinuumFlag>" + string (frequencyContinuumFlagChar) + "</FrequencyContinuumFlag>\n"
        "          <FrequencyEntry1>" + frequencyEntry1 + "</FrequencyEntry1>\n"
        "          <FrequencyEntry2>" + frequencyEntry2 + "</FrequencyEntry2>\n"
        "        </FrequencyInfo>\n"
        "        <Loudness>" + loudness + "</Loudness>\n"
        "        <Spatialization>" + spatialization + "</Spatialization>\n"
        "        <Reverb>" + reverb + "</Reverb>\n"
        "        <Filter />"
          + modifiersbuffer +
        "      </ExtraInfo>\n";
      stringbuffer = stringbuffer + bottomBuffer;

    }


  stringbuffer = stringbuffer + "    </Event>\n";
  return stringbuffer;


}



string convertEventLayer ( FileValue* _layerFileValue){


  std::list<FileValue> fileValueList = _layerFileValue->getList();

  std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();

  //the first value in the list is teh name
  string byLayer = convertFunctionFromLASSIEMetaData(fileValueListIter->getString());

  fileValueListIter ++;
  std::list<FileValue> discretePackages = fileValueListIter->getList();



  string packageBuffer = "";

  std::list<FileValue>::iterator discretePackagesIter = discretePackages.begin();
  if (discretePackagesIter->getInt() != 666){//see dummy
    for (;discretePackagesIter!= discretePackages.end(); discretePackagesIter++){
      packageBuffer = packageBuffer + convertDiscretePackages(&(*discretePackagesIter));
    }
  }

  string stringBuffer =
    "        <Layer>\n";
  stringBuffer = stringBuffer +
    "          <ByLayer>" + byLayer + "</ByLayer>\n"
    "          <DiscretePackages>\n" + packageBuffer +
    "          </DiscretePackages>\n"
    "        </Layer>\n";

  return stringBuffer;



}


string convertDiscretePackages (FileValue* _packageFileValue){


  std::list<FileValue> stringList = _packageFileValue->getList();
  std::list<FileValue>::iterator i = stringList.begin();

  string eventName = i->getString();
  i ++;
  int eventType = i->getInt();
  i ++;
  string weight = i->getString();
  i ++;
  string attackEnv = i->getString();
  i ++;
  string attackEnvScale = i->getString();
  i ++;
  string durationEnv = i->getString();
  i ++;
  string durationEnvScale = i->getString();

  char charBuffer[5];

  sprintf (charBuffer, "%d", eventType);



  std::string a =
    "            <Package>\n"
    "              <EventName>" + eventName + "</EventName>\n"
    "              <EventType>" + string(charBuffer) +"</EventType>\n" //this is eventType
    "              <Weight>" + weight + "</Weight>\n"
    "              <AttackEnvelope>"+ attackEnv + "</AttackEnvelope>\n"
    "              <AttackEnvelopeScale>"+ attackEnvScale + "</AttackEnvelopeScale>\n"
    "              <DurationEnvelope>"+ durationEnv + "</DurationEnvelope>\n"
    "              <DurationEnvelopeScale>"+ durationEnvScale + "</DurationEnvelopeScale>\n"
    "            </Package>\n";

 return a;
}



string parseNonEvent(EventType _type){
  string result = "";
  FileValue* value;

  value = file_data["LASSIEeventName"];

  std::list<FileValue> nameAndOrderList = value->getList();
  int eventOrderInPalette;

  std::list<FileValue>::iterator iter = nameAndOrderList.begin();
  value = &(*iter);
  string eventName = value->getString();
  if (nameAndOrderList.size()==0){ //old file format
    eventOrderInPalette = rand();
  }
  else {
    iter++;
    value = &(*iter);
    eventOrderInPalette = value->getInt();
  }

  char orderInPaletteChar[10];
  sprintf (orderInPaletteChar," %d", eventOrderInPalette);




  if (_type == eventSound){

    value = file_data["LASSIESOUNDnumPartials"];
    string numPartials = ((value == NULL)? "": value->getString());

    value = file_data["LASSIESOUNDdeviation"];
    string deviation = (value == NULL)? "": convertFunctionFromLASSIEMetaData(value->getString());

    value = file_data["LASSIESOUNDspectrum"];

    string spectrumString = "";
    std::list<FileValue> fileValueList = value->getList();
    std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();


    for (;
      fileValueListIter!= fileValueList.end();
      fileValueListIter++){
      spectrumString = spectrumString +
                       "        <Partial>" + convertFunctionFromLASSIEMetaData(fileValueListIter->getString()) +
                       "</Partial>\n";
    }



    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>5</EventType>\n" //5 is sound
    "      <Name>" + eventName + "</Name>\n"
    "      <NumberOfPartials>" + numPartials + "</NumberOfPartials>\n"
    "      <Deviation>" + deviation + "</Deviation>\n"
    "      <Spectrum>\n" + spectrumString +
    "      </Spectrum>\n"
    "    </Event>\n";

    //extraInfo->setSpectrum((value == NULL)? "": value->getString());
  }


  else if (_type == eventEnv){
    value = file_data["LASSIEENV"];
    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>6</EventType>\n" //6 is envelope
    "      <Name>" + eventName + "</Name>\n"
    "      <EnvelopeBuilder>" + convertFunctionFromLASSIEMetaData(value->getString()) + "</EnvelopeBuilder>\n"
    "    </Event>\n";


  }

  else if (_type == eventSiv){

    value = file_data["LASSIESIV"];
    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>7</EventType>\n" //7 is siv
    "      <Name>" + eventName + "</Name>\n"
    "      <SieveBuilder>" + convertFunctionFromLASSIEMetaData(value->getString()) + "</SieveBuilder>\n"
    "    </Event>\n";

  }



  else if (_type == eventSpa){

    value = file_data["LASSIESPA"];
    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>8</EventType>\n" //8 is spa
    "      <Name>" + eventName + "</Name>\n"
    "      <SpatializationBuilder>" + convertFunctionFromLASSIEMetaData(value->getString()) + "</SpatializationBuilder>\n"
    "    </Event>\n";
  }

  else if (_type == eventPat){

    value = file_data["LASSIEPAT"];
    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>9</EventType>\n" // is pattern
    "      <Name>" + eventName + "</Name>\n"
    "      <PatternBuilder>" + convertFunctionFromLASSIEMetaData(value->getString()) + "</PatternBuilder>\n"
    "    </Event>\n";
  }

  else if (_type == eventRev){

    value = file_data["LASSIEREV"];
    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>10</EventType>\n" //10 is Reverb
    "      <Name>" + eventName + "</Name>\n"
    "      <ReverbBuilder>" + convertFunctionFromLASSIEMetaData(value->getString()) + "</ReverbBuilder>\n"
    "    </Event>\n";
  }

  else if (_type == eventNote){

    string modifiersBuffer = "";

    value = file_data["noteModifiers"];
    std::list<FileValue> fileValueList = value->getList();
    std::list<FileValue>::iterator fileValueListIter = fileValueList.begin();

    for (fileValueListIter;
      fileValueListIter!= fileValueList.end();
      fileValueListIter++){
      modifiersBuffer = modifiersBuffer + "        <Modifier>"+fileValueListIter->getString() + "</Modifier>\n";
    }

    modifiersBuffer = "      <Modifiers>\n" + modifiersBuffer + "      </Modifiers>\n";



    result =
    "    <Event orderInPalette='" + string(orderInPaletteChar) +"'>\n"
    "      <EventType>12</EventType>\n" //12 is Note
    "      <Name>" + eventName + "</Name>\n" + modifiersBuffer +
    "    </Event>\n";


  }


  return result;

}





string convertFunctionHelperForEvaluation(FileValue* _value){


  string stringbuffer;
  char charbuffer[20];
  if (_value->isFunction()){
    stringbuffer= _value->getFtnString();

    bool isNotStatic = (stringbuffer.find("CURRENT") == string::npos &&
        stringbuffer.find("AVAILA") == string::npos &&
        stringbuffer.find("PREVIOUS_CHILD_DURATION") == string::npos
        );

    if (isNotStatic){
      stringbuffer = stringbuffer + "(";
    }
    list<FileValue> arguments = _value->getFtnArgs();
    list<FileValue>::iterator iter = arguments.begin();
    FileValue* argument;
    while (iter != arguments.end()){
      argument = &(*iter);
      stringbuffer = stringbuffer + convertFunctionHelperForEvaluation (argument);

      iter++;
      if (iter != arguments.end()){
        stringbuffer = stringbuffer + ",";
      }
    }

    if (isNotStatic){
      stringbuffer = stringbuffer + ")";
    }

    return stringbuffer;

  }




  else if (_value->isNumber()){
    double intpart;
    double number = _value->getFloat();
    if (modf(number, &intpart) == 0.0){
      int intNumber = _value->getInt();
      sprintf( charbuffer, "%d", intNumber);
      return string(charbuffer);
    }

    else {
      sprintf( charbuffer, "%f", number);
      morphNumericString(charbuffer);
      return string(charbuffer);

    }
  }

  else if (_value->isString()){
    return "\"" + _value->getString() + "\"";
  }

  else if (_value->isList()){
    return fileValueListToStringForEvaluation(_value->getList());

  }

  cout<<"Warning! FileValue failed to return an argument string"<<endl;
  return "";


}




string fileValueListToStringForEvaluation(
  list<FileValue> _valueList){
  string result = "<";

  list<FileValue>::iterator iter = _valueList.begin();
  FileValue* value;

  while (iter != _valueList.end()){
    value = &(*iter);
    result = result +  convertFunctionHelperForEvaluation(value);
    iter++;
    if (iter != _valueList.end()){
      result = result + ", ";
    }
  }
  result = result + ">";
  return result;
}













void morphNumericString (char *s) {
  char *p;
  int count;

  p = strchr (s,'.');         // Find decimal point, if any.
  if (p != NULL) {
    count = 10;              // Adjust for more or less decimals.
    while (count >= 0) {    // Maximum decimals allowed.
      count--;
      if (*p == '\0') break;   // If there's less than desired.

      p++;
                           // Next character.
    }

    *p-- = '\0';            // Truncate string.
    while (*p == '0') *p-- = '\0';       // Remove trailing zeros
    if (*p == '.') *p = '\0';        // If all decimals were zeros, remove ".".


  }
}

string convertFunctionFromLASSIEMetaData(string _originalString){
  size_t locationOfKeyword;

  string parsingString;
  FileValue* value;
  list<FileValue>::iterator argumentsIter;


  //check if RandomInt
  locationOfKeyword =_originalString.find("RandomInt");

  if (int(locationOfKeyword)==0){

    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){

      value = file_data["LASSIEFUNCTION"];

      //RandomInt has 2 arguments
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument
      string low = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));
      argumentsIter++;

      value =&(*argumentsIter);
      string high = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      string returned = "<Fun><Name>RandomInt</Name><LowBound>" + low + "</LowBound><HighBound>" + high + "</HighBound></Fun>";



      return returned;
    }

  }




  //check if Random
  locationOfKeyword =_originalString.find("Random");
  size_t checkNotRandomInt = _originalString.find("RandomInt");
  size_t checkNotRandomize = _originalString.find("Randomizer");
  size_t checkNotRandomSeed = _originalString.find("RandomSeed");
  if (int(locationOfKeyword)==0
      &&int(checkNotRandomInt)!=0
      &&int(checkNotRandomize)!=0
      &&int(checkNotRandomSeed)!=0){


    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){

      value = file_data["LASSIEFUNCTION"];

      //Random has 2 arguments
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument
      string low = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));
      argumentsIter++;

      value =&(*argumentsIter);
      string high = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      string returned = "<Fun><Name>Random</Name><LowBound>" + low + "</LowBound><HighBound>" + high + "</HighBound></Fun>";



      return returned;


    }


  }





//check if Randomizer
  locationOfKeyword =_originalString.find("Randomizer");
  if (int(locationOfKeyword)==0){
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //randomizer has 2 arguments


      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument
      string base = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));
      argumentsIter++;

      value =&(*argumentsIter);
      string dev = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));
      string returned = "<Fun><Name>Randomizer</Name><Base>" + base + "</Base><Deviation>" + dev + "</Deviation></Fun>";

      return returned;

    }

    //end parsing
  }

  //check if Select
  locationOfKeyword =_originalString.find("Select");
  if (int(locationOfKeyword)==0){


    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];

      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a list

      string listString = "";

      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          listString = listString + ", ";
        }

        value = &(*listIter);
        listString = listString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }





      //string listString = convertFunctionFromLASSIEMetaData(convertFunctionHelper(value));



      argumentsIter++;
      value =&(*argumentsIter); // second argument is an int
      string index = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));
      std::string stringbuffer = "<Fun><Name>Select</Name><List>" + listString + "</List>"
              + "<Index>" + index + "</Index></Fun>";
      return stringbuffer;

    }

    //end parsing
  }

//check if Decay
  locationOfKeyword =_originalString.find("Decay");
  if (int(locationOfKeyword)==0){

  // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //Decay has 4 argument

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float (base
      string a = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is a string
      string b = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter++;
      value =&(*argumentsIter); // 3nd argument is a string
      string c = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter++;
      value =&(*argumentsIter); // 4nd argument is an int
      string d = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      string returned = "<Fun><Name>Decay</Name><Base>" + a + "</Base><Type>" + b +
           "</Type><Rate>" + c + "</Rate><Index>" + d + "</Index></Fun>";


      return returned;
    }

    //end parsing
  }

  //check if Inverse
  locationOfKeyword =_originalString.find("Inverse");
  if (int(locationOfKeyword)==0){

    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //inverse has 1 argument

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float
      string a = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      return "<Fun><Name>Inverse</Name><Entry>" + a +  "</Entry></Fun>";


    }

    //end parsing
  }


//check if LN
  locationOfKeyword =_originalString.find("LN");
  if (int(locationOfKeyword)==0){
  // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //LN has 1 argument

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float
      string a = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      return "<Fun><Name>LN</Name><Entry>" + a +  "</Entry></Fun>";

    }

    //end parsing
  }



  //check if CURRENT_TYPE
  locationOfKeyword =_originalString.find("CURRENT_TYPE");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>CURRENT_TYPE</Name></Fun>";

  }

  //check if CURRENT_CHILD_NUM
  locationOfKeyword =_originalString.find("CURRENT_CHILD_NUM");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>CURRENT_CHILD_NUM</Name></Fun>";

  }

  //check if CURRENT_PARTIAL_NUM
  locationOfKeyword =_originalString.find("CURRENT_PARTIAL_NUM");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>CURRENT_PARTIAL_NUM</Name></Fun>";

  }

  //check if CURRENT_SEGMENT
  locationOfKeyword =_originalString.find("CURRENT_SEGMENT");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>CURRENT_SEGMENT</Name></Fun>";

  }

  //check if AVAILABLE_EDU
  locationOfKeyword =_originalString.find("AVAILABLE_EDU");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>AVAILABLE_EDU</Name></Fun>";

  }

  //check if PREVIOUS_CHILD_DURATION
  locationOfKeyword =_originalString.find("PREVIOUS_CHILD_DURATION");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>PREVIOUS_CHILD_DURATION</Name></Fun>";

  }



  //check if CURRENT_LAYER
  locationOfKeyword =_originalString.find("CURRENT_LAYER");
  if (int(locationOfKeyword)==0){
    return "<Fun><Name>CURRENT_LAYER</Name></Fun>";

  }





  locationOfKeyword =_originalString.find("Stochos");
  if (int(locationOfKeyword)==0){


    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //Stochos has 3 arguments (string, list of envelopes, int)
      list<FileValue> arguments = value->getFtnArgs();
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument


      string method = value->getString();


      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument, list of envelopes



      string envelopes ="";


      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){

        value = &(*listIter);
        string envelope = convertFunctionHelperForEvaluation(value);

        envelope = convertFunctionFromLASSIEMetaData(envelope);

        envelopes = envelopes +"<Envelope>" + envelope+ "</Envelope>";
      }


      //third argument
      argumentsIter++;
      value =&(*argumentsIter);
      string offset = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));




      return "<Fun><Name>Stochos</Name><Method>" +
   method + "</Method><Envelopes>" + envelopes + "</Envelopes><Offset>"+ offset + "</Offset></Fun>";


    }
  }


  //check if MakeEnvelope
  locationOfKeyword =_originalString.find("MakeEnvelope");
  if (int(locationOfKeyword)==0){
    //cout<<"Make Envelope found! "<<endl; //_originalString<<endl;


    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;

    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //MakeEnvelope has 5 arguments (xs, ys, types, pros, scale)

      list<FileValue> arguments = value->getFtnArgs();
      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float list

      string xs ="";


      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        value = &(*listIter);
        xs = xs +"<X>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</X>";
      }

      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument float list

      string ys = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        value = &(*listIter);
        ys = ys +"<Y>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</Y>";
      }


      argumentsIter++;
      value =&(*argumentsIter); // 3nd argument string list
      string types = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        value = &(*listIter);
        types = types +"<T>" + value->getString() + "</T>";
      }
      argumentsIter++;
      value =&(*argumentsIter); // 4nd argument string list
      string pros = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        value = &(*listIter);
        pros = pros +"<P>" + value->getString() + "</P>";
      }

      argumentsIter++;
      value =&(*argumentsIter); // 5nd argument
      string scale = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));




      return "<Fun><Name>MakeEnvelope</Name><Xs>" + xs + "</Xs><Ys>" + ys + "</Ys><Types>" + types
          + "</Types><Pros>" + pros + "</Pros><Scale>" + scale + "</Scale></Fun>";


    }
    //end parsing
    cout<<"see make envelope but parse failed"<<endl;
  }




   //check if ValuePick
  locationOfKeyword =_originalString.find("ValuePick");
  if (int(locationOfKeyword)==0){

        // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ValuePick has 10 arguments.

      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a float (absolute range
      string absRange = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is an envelope (low)
      string envLow = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter++;
      value =&(*argumentsIter); // third argument is an envelope (high)
      string envHigh = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter++;
      value =&(*argumentsIter); // fourth argument is an envelope (distribution)
      string envDist = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter++;
      value =&(*argumentsIter); // // 5th argument is a string  (Elements)
      string elements = value->getString();




      argumentsIter++;
      value =&(*argumentsIter); // 6th argument is a list without bracket

      string elementListString = "";

      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          elementListString = elementListString + ", ";
        }

        value = &(*listIter);
        elementListString = elementListString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }

      argumentsIter++;
      value =&(*argumentsIter); // // 7th argument is a string  (weight)
      string weight = value->getString();






      argumentsIter++;
      value =&(*argumentsIter); // 8th argument is a list without bracket

      string weightListString = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          weightListString = weightListString + ", ";
        }

        value = &(*listIter);
        weightListString = weightListString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }



      argumentsIter++;
      value =&(*argumentsIter); // 9th argument is a string  (type)
      string type = value->getString();

      argumentsIter++;
      value =&(*argumentsIter); // 10th argument is a list without bracket

      string offsetListString = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          offsetListString = offsetListString + ", ";
        }

        value = &(*listIter);
        offsetListString = offsetListString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }

      return "<Fun><Name>ValuePick</Name><Range>"+ absRange + "</Range><Low>" + envLow + "</Low><High>"
              + envHigh + "</High><Dist>" + envDist + "</Dist><Method>" + elements + "</Method><Elements>"
              + elementListString + "</Elements><WeightMethod>" + weight + "</WeightMethod><Weight>" + weightListString
              + "</Weight><Type>" + type + "</Type><Offsets>" +offsetListString+ "</Offsets></Fun>";

    }

    //end parsing
  }






  //check if ChooseL
  locationOfKeyword =_originalString.find("ChooseL");
  if (int(locationOfKeyword)==0){


    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs(); //chooseL has 1 argument

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a function


      return "<Fun><Name>ChooseL</Name><Entry>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) +  "</Entry></Fun>";
    }

    //end parsing
  }



  //check if GetPattern
  locationOfKeyword =_originalString.find("GetPattern");
  if (int(locationOfKeyword)==0){


    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //getpattern has 3 argument
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter);  //first argument is a string (method)

      string method = value->getString();


      argumentsIter++;
      value =&(*argumentsIter); // second argument is an integer
      string origin = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter++;
      value =&(*argumentsIter); // third argument is a function
      string pattern = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      return "<Fun><Name>GetPattern</Name><Method>" + method + "</Method><Offset>" + origin
            + "</Offset><Pattern>" + pattern + "</Pattern></Fun>";

    }

    //end parsing
  }


    //check if EnvLib
  locationOfKeyword =_originalString.find("EnvLib");
  if (int(locationOfKeyword)==0){

  // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      list<FileValue> arguments = value->getFtnArgs();  //EnvLib has 2 arguments

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is an int

      string envNum = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is an Float
      string scale =  convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      return "<Fun><Name>EnvLib</Name><Env>" + envNum + "</Env><Scale>"  + scale + "</Scale></Fun>";

    }

    //end parsing
  }


  //check if MakeList
  locationOfKeyword =_originalString.find("MakeList");
  if (int(locationOfKeyword)==0){

  // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ReadEnvFile has 2 arguments
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a function

      string fun = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter++;
      value =&(*argumentsIter); // 2nd argument is a number
      string size =  convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      return "<Fun><Name>MakeList</Name><Func>"+fun + "</Func><Size>" + size + "</Size></Fun>";


    }

    //end parsing
  }


   //check if SPA
  locationOfKeyword =_originalString.find("STEREO");
  size_t lok2 = _originalString.find("MULTI_PAN");
  size_t lok3 = _originalString.find("POLAR");


  if (int(locationOfKeyword)==2||int(lok2) ==2 ||int(lok3) ==2){

// start parsing

    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //SPA is a list with 3 elements (4 elements if the method is polar)
      list<FileValue> arguments = value->getList();


      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string

      string method = value->getString();

      argumentsIter++;
      value =&(*argumentsIter); // second argument is a string
      string applyHow = value->getString();



      argumentsIter++;
      value =&(*argumentsIter); // third argument is a list of list

      string channels = "";


      if (method == "STEREO"){
        list<FileValue> listElements = value->getList();
        list<FileValue>::iterator listIter = listElements.begin();


        string partials = "<Partials>";
        value = &(*listIter);


        for (; listIter!= listElements.end(); listIter++){
          value = &(*listIter);
          partials = partials + "<P>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</P>";
        }
        partials = partials + "</Partials>";
        channels = channels + partials;
      }



      else if ( method == "MULTI_PAN"){
        list<FileValue> listElements = value->getList();
        list<FileValue>::iterator listIter = listElements.begin();
        for (; listIter!= listElements.end(); listIter++){
          string partials = "<Partials>";
          value = &(*listIter);
          list<FileValue> listEnvs = value->getList();
          list<FileValue>::iterator listIter2 = listEnvs.begin();

          for (; listIter2!= listEnvs.end(); listIter2++){
            value = &(*listIter2);
            partials = partials + "<P>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</P>";
          }
          partials = partials + "</Partials>";
          channels = channels + partials;

        }

      }





      else {//polar


        list<FileValue> listElements = value->getList();
        list<FileValue>::iterator listIter = listElements.begin();

        string partials = "<Partials>";

        for (; listIter!= listElements.end(); listIter++){

          value = &(*listIter);
          partials = partials + "<P>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</P>";
        }

        partials = partials + "</Partials>";
        channels = channels + partials;

        argumentsIter++; //4th argument
        value =&(*argumentsIter); //
        listElements = value->getList();
        listIter = listElements.begin();

        partials = "<Partials>";

        for (; listIter!= listElements.end(); listIter++){

          value = &(*listIter);
          partials = partials + "<P>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</P>";
        }

        partials = partials + "</Partials>";
        channels = channels + partials;



      }


    return "<Fun><Name>SPA</Name><Method>" + method +  "</Method><Apply>" + applyHow + "</Apply><Channels>" + channels + "</Channels></Fun>";



    }

    //end parsing
  }


  //check if MakePattern
  locationOfKeyword =_originalString.find("MakePattern");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];

      //makepattern has 2 arguments
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a list
      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      string listString = "";

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          listString = listString + ", ";
        }

        value = &(*listIter);
        listString = listString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }

      return "<Fun><Name>MakePattern</Name><List>" + listString + "</List></Fun>";

    }

    //end parsing
  }



  //check if ExpandPattern
  locationOfKeyword =_originalString.find("ExpandPattern");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //expandpattern has 5 arguments
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();

      value =&(*argumentsIter); //first argument is the method
      string method = value->getString();


      argumentsIter ++;
      value =&(*argumentsIter); // second argument is an int
      string modulo = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter ++;
      value =&(*argumentsIter); // 3rd argument is an int
      string low = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter ++;
      value =&(*argumentsIter); // 4th argument is an int
      string high = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter ++;
      value =&(*argumentsIter); // 5th argument is a function string
      string pattern = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      return  "<Fun><Name>ExpandPattern</Name><Method>"+ method+ "</Method><Modulo>"+ modulo +  "</Modulo><Low>" + low
              + "</Low><High>" + high +  "</High><Pattern>" + pattern + "</Pattern></Fun>";


    }

    //end parsing
  }


  //check if REV_Simple
  locationOfKeyword =_originalString.find("<\"SIMPLE");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //Rev_Simple has 2 elements in the list
      list<FileValue> arguments = value->getList();

      argumentsIter = arguments.begin();
      argumentsIter ++;
      value =&(*argumentsIter); // the only useful argument (2nd) is a float

      return "<Fun><Name>REV_Simple</Name><Size>" + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value)) + "</Size></Fun>";

    }

    //end parsing
  }



  //check if REV_Medium
  locationOfKeyword =_originalString.find("<\"MEDIUM");
  if (int(locationOfKeyword)==0){

    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //Rev_medium has 5 elements in the list
      list<FileValue> arguments = value->getList();

      argumentsIter = arguments.begin();
      argumentsIter ++;
      value =&(*argumentsIter); // 2nd argument is an envelope
      string env = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter ++;
      value =&(*argumentsIter); // 3rd argument is an float
      string spread = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter ++;
      value =&(*argumentsIter); // 4th argument is an float
      string gain = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter ++;
      value =&(*argumentsIter); // 5th argument is an float
      string delay = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      return "<Fun><Name>REV_Medium</Name><Percent>" + env + "</Percent><Spread>" + spread + "</Spread><AllPass>"
             + gain + "</AllPass><Delay>" + delay + "</Delay></Fun>";

    }

    //end parsing
  }



  //check if REV_Advanced
  locationOfKeyword =_originalString.find("<\"ADVANCED");
  if (int(locationOfKeyword)==0){


    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //Rev_advanced has 6 elements in the list
      list<FileValue> arguments = value->getList();

      argumentsIter = arguments.begin();
      argumentsIter ++;
      value =&(*argumentsIter); // 2nd argument is an envelope
      string env = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter ++;
      value =&(*argumentsIter); // 3rd argument is a list
      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      string gainlist = "";

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          gainlist = gainlist + ", ";
        }

        value = &(*listIter);
        gainlist = gainlist + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }

      argumentsIter ++;
      value =&(*argumentsIter); // 4th argument is an list


      listElements = value->getList();
      listIter = listElements.begin();

      string lp = "";

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          lp = lp + ", ";
        }

        value = &(*listIter);
        lp = lp + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }

      argumentsIter ++;
      value =&(*argumentsIter); // 5th argument is an float
      string gain = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter ++;
      value =&(*argumentsIter); // 6th argument is an float
      string delay = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      return "<Fun><Name>REV_Advanced</Name><Percent>" + env + "</Percent><CombGainList>" + gainlist
             +"</CombGainList><LPGainList>" + lp + "</LPGainList><AllPass>" + gain +"</AllPass><Delay>" + delay + "</Delay></Fun>";

    }

    //end parsing
  }


  //check if MakeSieve
  locationOfKeyword =_originalString.find("MakeSieve");
  if (int(locationOfKeyword)==0){

// start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //makesieve has 7 arguments.

      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is an int (low bound)
      string low = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      argumentsIter++;
      value =&(*argumentsIter); // second argument is an int (high bound)
      string high = convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));


      argumentsIter++;
      value =&(*argumentsIter); // 3rd argument is a string  (Elements)
      string element = value->getString();


      argumentsIter++;
      value =&(*argumentsIter); // 4th argument is a list without bracket
      string elementListString = "";

      list<FileValue> listElements = value->getList();
      list<FileValue>::iterator listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          elementListString = elementListString + ", ";
        }

        value = &(*listIter);
        elementListString = elementListString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }



      argumentsIter++;
      value =&(*argumentsIter); // 5th argument is a string  (weight)
      string weight = value->getString();


      argumentsIter++;
      value =&(*argumentsIter); // 6th argument is a list without bracket
      string weightListString = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          weightListString = weightListString + ", ";
        }

        value = &(*listIter);
        weightListString = weightListString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }




      argumentsIter++;

      value =&(*argumentsIter); // 7th argument is a list without bracket

      string offsetListString = "";

      listElements = value->getList();
      listIter = listElements.begin();

      for (; listIter!= listElements.end(); listIter++){
        if (listIter!= listElements.begin()){
          offsetListString = offsetListString + ", ";
        }

        value = &(*listIter);
        offsetListString = offsetListString + convertFunctionFromLASSIEMetaData(convertFunctionHelperForEvaluation(value));

      }

      return "<Fun><Name>MakeSieve</Name><Low>"+ low + "</Low><High>" + high + "</High><Method>" + element
                +"</Method><Elements>" +elementListString+ "</Elements><WeightMethod>" + weight
                +"</WeightMethod><Weight>"+weightListString+ "</Weight><Offset>" + offsetListString + "</Offset></Fun>";

    }

    //end parsing
  }


   //check if ReadSIVFile
  locationOfKeyword =_originalString.find("ReadSIVFile");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ReadSIVFile has 1 argument
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      string functionString;

      if (value->isFunction()){  //this must be select.
        string resultString = convertFunctionHelperForEvaluation(value);


        //remove "SPA/"
        size_t index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = resultString.find("SIV/", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          resultString.replace(index, 4, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }

        functionString = convertFunctionFromLASSIEMetaData(resultString);

        // remove double quote

        index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = functionString.find("\"", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          functionString.replace(index, 1, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }


      }

      else{
        functionString = value->getString();
        if (functionString.length() >=4){
          functionString = functionString.substr(4, functionString.length()-4);
        }
      }
      return "<Fun><Name>ReadSIVFile</Name><File>" + functionString +  "</File></Fun>";


    }

    //end parsing
  }

   //check if ReadENVFile
  locationOfKeyword =_originalString.find("ReadENVFile");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ReadENVFile has 1 argument
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      string functionString;

      if (value->isFunction()){  //this must be select.
        string resultString = convertFunctionHelperForEvaluation(value);


        //remove "SPA/"
        size_t index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = resultString.find("ENV/", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          resultString.replace(index, 4, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }

        functionString = convertFunctionFromLASSIEMetaData(resultString);

        // remove double quote

        index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = functionString.find("\"", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          functionString.replace(index, 1, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }


      }

      else{
        functionString = value->getString();
        if (functionString.length() >=4){
          functionString = functionString.substr(4, functionString.length()-4);
        }
      }
      return "<Fun><Name>ReadENVFile</Name><File>" + functionString +  "</File></Fun>";


    }

    //end parsing
  }

  //check if ReadSPAFile
  locationOfKeyword =_originalString.find("ReadSPAFile");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ReadSPAFile has 1 argument
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string

      string functionString;

      if (value->isFunction()){  //this must be select.
        string resultString = convertFunctionHelperForEvaluation(value);


        //remove "SPA/"
        size_t index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = resultString.find("SPA/", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          resultString.replace(index, 4, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }

        functionString = convertFunctionFromLASSIEMetaData(resultString);

        // remove double quote

        index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = functionString.find("\"", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          functionString.replace(index, 1, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }


      }

      else{
        functionString = value->getString();
        if (functionString.length() >=4){
          functionString = functionString.substr(4, functionString.length()-4);
        }
      }


      return "<Fun><Name>ReadSPAFile</Name><File>" + functionString +  "</File></Fun>";


    }

    //end parsing
  }

  //check if ReadPATFile
  locationOfKeyword =_originalString.find("ReadPATFile");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ReadPATFile has 1 argument
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      string functionString;

      if (value->isFunction()){  //this must be select.
        string resultString = convertFunctionHelperForEvaluation(value);


        //remove "PAT/"
        size_t index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = resultString.find("PAT/", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          resultString.replace(index, 4, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }

        functionString = convertFunctionFromLASSIEMetaData(resultString);

        // remove double quote

        index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = functionString.find("\"", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          functionString.replace(index, 1, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }


      }

      else{
        functionString = value->getString();
        if (functionString.length() >=4){
          functionString = functionString.substr(4, functionString.length()-4);
        }
      }


      return "<Fun><Name>ReadPATFile</Name><File>" + functionString +  "</File></Fun>";


    }


    //end parsing
  }
 //check if ReadREVFile
  locationOfKeyword =_originalString.find("ReadREVFile");
  if (int(locationOfKeyword)==0){

    // start parsing
    parsingString= "LASSIEFUNCTION = " + _originalString + ";" ;
    yy_scan_string(parsingString.c_str());//set parser buffer

    int parsingResult = yyparse();
    if (parsingResult ==0){
      value = file_data["LASSIEFUNCTION"];
      //ReadREVFile has 1 argument
      list<FileValue> arguments = value->getFtnArgs();

      argumentsIter = arguments.begin();
      value =&(*argumentsIter); // first argument is a string
      string functionString;

      if (value->isFunction()){  //this must be select.
        string resultString = convertFunctionHelperForEvaluation(value);


        //remove "REV/"
        size_t index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = resultString.find("REV/", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          resultString.replace(index, 4, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }

        functionString = convertFunctionFromLASSIEMetaData(resultString);

        // remove double quote

        index = 0;
        while (true) {
          /* Locate the substring to replace. */
          index = functionString.find("\"", index);
          if (index == string::npos) break;

          /* Make the replacement. */
          functionString.replace(index, 1, "");

          /* Advance index forward so the next iteration doesn't pick it up as well. */
          index ++;
        }


      }

      else{
        functionString = value->getString();
        if (functionString.length() >=4){
          functionString = functionString.substr(4, functionString.length()-4);
        }
      }

      //cout<<functionString<<endl;

      return "<Fun><Name>ReadREVFile</Name><File>" + functionString +  "</File></Fun>";


    }

    //end parsing
  }

  return _originalString;
}
