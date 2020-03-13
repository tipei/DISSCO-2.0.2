/******************************************************************************
*
*  File Name     : ProjectViewController.cpp
*  Date created  : Jan. 27 2010
*  Authors       : Ming-ching Chiu, Sever Tipei
*  Organization  : Music School, University of Illinois at Urbana Champaign
*  Description   : This file contains the implementation of class "Project".
*                  Each Project object holds the information of the project
*                  such as name, structure, etc.
*
*===============================================================================
*
*  This file is part of LASSIE.
*  2010 Ming-ching Chiu, Sever Tipei
*
*
*  LASSIE is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  LASSIE is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with LASSIE.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "ProjectViewController.h"

//#include <stdio.h>
//#include <stdlib.h>

#include "IEvent.h"
#include "EventAttributesViewController.h"
#include "PaletteViewController.h"
#include "FileOperations.h"
#include "MainWindow.h"
#include "EnvelopeLibraryEntry.h"
#include "SharedPointers.h"
#include "FunctionGenerator.h"
#include "ObjectWindow.h"
#include <dirent.h>
#include <sstream>

#include "../../CMOD/src/muParser/muParser.h"





// these definitions are for calling yyparse(). They are copied from lex.yy.c
#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif



#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};


extern YY_BUFFER_STATE yy_scan_string( const char*);
extern int yyparse();
extern map<const char*, FileValue*, ltstr> file_data;



/*! \brief The constructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::ProjectViewController(MainWindow* _mainWindow){
  emptyProject = true;
  xmlDocument=NULL;

  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  listTargets.push_back( Gtk::TargetEntry("text/plain") );


  //////////////////////////////////////////////////////////////////////////

  std::string topName = "";

  //add the Paned widget "leftTwoPlusAttributes" as a child
  add(leftTwoPlusAttributes);

  //no title when initializing the window
  pathAndName   = "";
  projectTitle  = "";
  fileFlag      = "";
  duration      = "";
  numOfChannels = "2";
  sampleRate    = "44100";
  sampleSize    = "16";
  numOfThreads  = "1";
  topEvent      = "0";
//	measure       = "0.6";
  synthesis     = true;
	score 				= false;

  sharedPointers = new SharedPointers();
      sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;
  //create three new children widgets
  //eventAttributesView = new EventAttributesViewController(  sharedPointers );
  //paletteView = new PaletteViewController(  sharedPointers );


  //set the attributes of two Paned widgets and add children view in them
  //leftTwoPlusAttributes.set_position(200);
  //leftTwoPlusAttributes.pack1(*paletteView,true,false);
  //leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);


  envelopeLibraryEntries = NULL;

  topWindow = NULL;
  highWindow = NULL;
  midWindow = NULL;
  lowWindow = NULL;
  bottomWindow = NULL;
  spectrumWindow = NULL;
  envWindow = NULL;
  sivWindow = NULL;
  spaWindow = NULL;
  patWindow = NULL;
  revWindow = NULL;
  noteWindow = NULL;
  filWindow = NULL;
	meaWindow = NULL;

  show_all_children();
}

void ProjectViewController::initializeModifiers() {
  defaultNoteModifiers.insert(pair<string,bool>("-8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("+8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("bend",true));
  defaultNoteModifiers.insert(pair<string,bool>("dry",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissKeys",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissStringRes",true));
  defaultNoteModifiers.insert(pair<string,bool>("graceTie",true));
  defaultNoteModifiers.insert(pair<string,bool>("letVibrate",true));
  defaultNoteModifiers.insert(pair<string,bool>("moltoVibrato",true));
  defaultNoteModifiers.insert(pair<string,bool>("mute",true));
  defaultNoteModifiers.insert(pair<string,bool>("pedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("pluck",true));
  defaultNoteModifiers.insert(pair<string,bool>("pressSilently",true));
  defaultNoteModifiers.insert(pair<string,bool>("resonance",true));
  defaultNoteModifiers.insert(pair<string,bool>("resPedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("sfz",true));
  defaultNoteModifiers.insert(pair<string,bool>("sffz",true));
  defaultNoteModifiers.insert(pair<string,bool>("tenuto",true));
  defaultNoteModifiers.insert(pair<string,bool>("tremolo",true));
  defaultNoteModifiers.insert(pair<string,bool>("vibrato",true));

  std::map<string, bool>::iterator iter = defaultNoteModifiers.begin();
  while (iter != defaultNoteModifiers.end()) {
    (*iter).second = true;
    iter++;
  }

}

/*! \brief The constructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::ProjectViewController(
  std::string _pathAndName, MainWindow* _mainWindow){
  xmlDocument = NULL;
  emptyProject = false;
  modifiedButNotSaved = true;
  seed="";



  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////

  std::string topName = "0";
  projectTitle  = FileOperations::stringToFileName(_pathAndName);
  fileFlag      = "THMLBsnv";
  duration      = "";
  numOfChannels = "2";
  sampleRate    = "44100";
  sampleSize    = "16";
  numOfThreads  = "1";
  topEvent      = "0";
//	measure       = "0.6";
  synthesis     = true;
	score 				= false;
  outputParticel= false;


  // initialize default note modifiers
  initializeModifiers();

  //add the Paned widget "leftTwoPlusAttributes" as a child
  add(leftTwoPlusAttributes);

  //no title when initializing the window
  pathAndName = _pathAndName;
  //datPathAndName = pathAndName+ "/"+projectTitle+".dat";
  //libPathAndName = pathAndName+ "/"+projectTitle+".lib";
  sharedPointers = new SharedPointers();
  sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;

   //create three new children widgets

  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);


  eventAttributesView->buildNoteModifiersList();

  //set the attributes of two Paned widgets and add children view in them
  leftTwoPlusAttributes.set_position(200);
  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);


  showContents();


  //make a new shared pointers object so all object can find each other easily
  //without calling layers of layers of parents/ children


  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;



  IEvent* newEvent = new IEvent();
  newEvent->setEventName("0");
  newEvent->setEventType(eventTop);
  paletteView->insertEvent(newEvent,"Top");
  events.push_back(newEvent);

  envelopeLibraryEntries = NULL;


  topWindow = new ObjectWindow(eventTop,this);
  highWindow = new ObjectWindow(eventHigh,this);
  midWindow = new ObjectWindow(eventMid,this);
  lowWindow = new ObjectWindow(eventLow,this);
  bottomWindow = new ObjectWindow(eventBottom,this);
  spectrumWindow = new ObjectWindow(eventSound,this);
  envWindow = new ObjectWindow(eventEnv,this);
  sivWindow = new ObjectWindow(eventSiv,this);
  spaWindow = new ObjectWindow(eventSpa,this);
  patWindow = new ObjectWindow(eventPat,this);
  revWindow = new ObjectWindow(eventRev,this);
  filWindow = new ObjectWindow(eventFil, this);
  noteWindow = new ObjectWindow(eventNote,this);
	meaWindow = new ObjectWindow(eventMea,this);

  show_all_children();

}


/*! \brief The destructor of ProjectViewController
*
*******************************************************************************/
ProjectViewController::~ProjectViewController(){
  //delete projectTreeView;
  delete eventAttributesView;
  delete paletteView;
  //std::cout << "ProjectViewController destructor is called." << std::endl;
}


//----------------------------------------------------------------------------//

void ProjectViewController::cleanUpContents(){

  MainWindow* _mainWindow = sharedPointers->mainWindow;
  //delete projectTreeView;
  delete eventAttributesView;
  delete paletteView;
  delete sharedPointers;
  sharedPointers = new SharedPointers;
    sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;


  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);


  //sharedPointers->projectTreeView = projectTreeView;
  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;


  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);
  //paletteAndTree.pack1(*paletteView,true,false);
  //paletteAndTree.pack2(*projectTreeView,true,false);

  show_all_children();
}


//----------------------------------------------------------------------------//

void ProjectViewController::showContents(){
  IEvent* event;
  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Top");
  event->setEventOrderInPalette(0);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("High");
  event->setEventOrderInPalette(1);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Mid");
  event->setEventOrderInPalette(2);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Low");
  event->setEventOrderInPalette(3);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Bottom");
  event->setEventOrderInPalette(4);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Spectrum");
  event->setEventOrderInPalette(5);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Note");
  event->setEventOrderInPalette(6);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Envelope");
  event->setEventOrderInPalette(7);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Sieve");
  event->setEventOrderInPalette(8);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Spatialization");
  event->setEventOrderInPalette(9);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Pattern");
  event->setEventOrderInPalette(10);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Reverb");
  event->setEventOrderInPalette(11);
  paletteView->insertEvent(event);
  events.push_back(event);

  event = new IEvent();
  event->setEventType(eventFolder);
  event->setEventName("Filter");
  event->setEventOrderInPalette(12);
  paletteView->insertEvent(event);
  events.push_back(event);

	// event = new IEvent();
  // event->setEventType(eventFolder);
  // event->setEventName("Measurement");
  // event->setEventOrderInPalette(13);
  // paletteView->insertEvent(event);
  // events.push_back(event);
}


//----------------------------------------------------------------------------//

void ProjectViewController::hideContents(){
  cleanUpContents();
}


//----------------------------------------------------------------------------//

void ProjectViewController::insertObject(){

  Glib::ustring selectedPaletteFolder = paletteView->folderSelected();
  //make sure a folder is selected in palette.
  if (selectedPaletteFolder == "None"){

    Gtk::MessageDialog dialog(
      "Please select a folder to store the new object.",
      false /* use_markup */,
      Gtk::MESSAGE_QUESTION,
      Gtk::BUTTONS_OK);

    dialog.run();
    return;
	}

  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      refBuilder->add_from_file("./LASSIE/src/UI/FileNewObject.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!refBuilder->add_from_file("./LASSIE/src/UI/FileNewObject.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

  //Get the GtkBuilder-instantiated Dialog:
  refBuilder->get_widget("newObjectDialog", newObjectDialog);

  Gtk::Button* okButton;
  refBuilder->get_widget("newObjectDialogOKButton", okButton);

  Gtk::Button* cancelButton;
  refBuilder->get_widget("newObjectDialogCancelButton", cancelButton);

  if(okButton){
    okButton->signal_clicked().connect(
      sigc::mem_fun(*this,&ProjectViewController::newObjectButtonClicked) );
    }

    if(cancelButton){
      cancelButton->signal_clicked().connect(
        sigc::mem_fun(*this,&ProjectViewController::newObjectButtonClicked) );
    }

  Gtk::Entry* nameEntry;
  refBuilder->get_widget("newObjectNameEntry", nameEntry);

  EventType type;
  std::string flagForFolderMatching;
  Gtk::RadioButton* typeButton;

  //set the defult radio button of type
  if (selectedPaletteFolder =="Top"){
    refBuilder->get_widget("buttonTop", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "High"){
    refBuilder->get_widget("buttonHigh", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Mid"){
    refBuilder->get_widget("buttonMid", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Low"){
    refBuilder->get_widget("buttonLow", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Bottom"){
    refBuilder->get_widget("buttonBottom", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Spectrum"){
    refBuilder->get_widget("buttonSound", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Envelope"){
    refBuilder->get_widget("buttonEnv", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Sieve"){
    refBuilder->get_widget("buttonSiv", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Spatialization"){
    refBuilder->get_widget("buttonSpa", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Pattern"){
    refBuilder->get_widget("buttonPat", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Reverb"){
    refBuilder->get_widget("buttonRev", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Note"){
    refBuilder->get_widget("buttonNote", typeButton);
    typeButton->set_active(true);
  }
  else if (selectedPaletteFolder == "Filter"){
    refBuilder->get_widget("buttonFil", typeButton);
    typeButton->set_active(true);
  }
	// else if (selectedPaletteFolder == "Measurement"){
  //   refBuilder->get_widget("buttonMea", typeButton);
  //   typeButton->set_active(true);
  // }

	int result = newObjectDialog->run();

  while (result ==1){
    std::string name = nameEntry->get_text();
    std::string first = name.substr(0,1);

    if (name == ""){
    //prompt error (lack of name) and then loop
    //std::cout<<"User didn't name the object!"<<std::endl;
      Gtk::MessageDialog dialog(
        "Please Name the Object",
        false /* use_markup */,
        Gtk::MESSAGE_QUESTION,
        Gtk::BUTTONS_OK);

      dialog.run();
      dialog.hide();
      result = newObjectDialog->run();
    }
    else if (selectedPaletteFolder == "Bottom"&& first!="s"&&first!= "n"){

      Gtk::MessageDialog dialog(
        "The name of a Bottom event should start with 's'"
        " (sound) or 'n' (note).",
        false /* use_markup */,
        Gtk::MESSAGE_QUESTION,
        Gtk::BUTTONS_OK);

      dialog.run();
      dialog.hide();
      result = newObjectDialog->run();

    }
    else {
      break;
    }
  }

  if(result ==1){

    refBuilder->get_widget("buttonTop", typeButton);
    if (typeButton->get_active()) {
      type = eventTop;
      flagForFolderMatching = "Top";
    }

    refBuilder->get_widget("buttonHigh", typeButton);
    if (typeButton->get_active()) {
      type = eventHigh;
      flagForFolderMatching = "High";
    }

    refBuilder->get_widget("buttonMid", typeButton);
    if (typeButton->get_active()) {
      type = eventMid;
      flagForFolderMatching = "Mid";
    }

    refBuilder->get_widget("buttonLow", typeButton);
    if (typeButton->get_active()) {
      type = eventLow;
      flagForFolderMatching = "Low";
    }

    refBuilder->get_widget("buttonBottom", typeButton);
    if (typeButton->get_active()) {
      type = eventBottom;
      flagForFolderMatching = "Bottom";
    }

    refBuilder->get_widget("buttonSound", typeButton);
    if (typeButton->get_active()) {
      type = eventSound;
      flagForFolderMatching = "Spectrum";
    }

    refBuilder->get_widget("buttonEnv", typeButton);
    if (typeButton->get_active()) {
      type = eventEnv;
      flagForFolderMatching = "Envelope";
    }

    refBuilder->get_widget("buttonSiv", typeButton);
    if (typeButton->get_active()) {
      type = eventSiv;
      flagForFolderMatching = "Sieve";
    }

    refBuilder->get_widget("buttonSpa", typeButton);
    if (typeButton->get_active()) {
      type = eventSpa;
      flagForFolderMatching = "Spatialization";
    }

    refBuilder->get_widget("buttonPat", typeButton);
    if (typeButton->get_active()) {
      type = eventPat;
      flagForFolderMatching = "Pattern";
    }

    refBuilder->get_widget("buttonRev", typeButton);
    if (typeButton->get_active()) {
      type = eventRev;
      flagForFolderMatching = "Reverb";
    }

    refBuilder->get_widget("buttonFil", typeButton);
    if (typeButton->get_active()) {
      type = eventFil;
      flagForFolderMatching = "Filter";
    }

		// refBuilder->get_widget("buttonMea", typeButton);
    // if (typeButton->get_active()) {
    //   type = eventMea;
    //   flagForFolderMatching = "Measurement";
    // }

    refBuilder->get_widget("buttonNote", typeButton);
    if (typeButton->get_active()) {
      type = eventNote;
      flagForFolderMatching = "Note";
    }

    refBuilder->get_widget("buttonFolder", typeButton);
    if (typeButton->get_active()) {
      type = eventFolder;
      flagForFolderMatching = "Folder";
    }

    //if mismatch, what does the user want.
    //0 = change folder. 1 = change type 2 = keep it this way
    int decision =2;


    if (selectedPaletteFolder != flagForFolderMatching){

      //Load the GtkBuilder file and instantiate its widgets:
      Glib::RefPtr<Gtk::Builder> refBuilder1 = Gtk::Builder::create();
      #ifdef GLIBMM_EXCEPTIONS_ENABLED
        try{
          refBuilder1->add_from_file(
            "./LASSIE/src/UI/PaletteInsertTypeMismatch.ui");
        }
        catch(const Glib::FileError& ex){
          std::cerr << "FileError: " << ex.what() << std::endl;
        }
        catch(const Gtk::BuilderError& ex){
          std::cerr << "BuilderError: " << ex.what() << std::endl;
        }
      #else
        std::auto_ptr<Glib::Error> error;
        if (!refBuilder1->add_from_file(
          "./LASSIE/src/UI/FileNewObject.ui", error)){
          std::cerr << error->what() << std::endl;
        }
      #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

      //Get the GtkBuilder-instantiated Dialog:
      Gtk::Dialog* mismatchDialog;
      std::string stringBuffer;
      refBuilder1->get_widget("MismatchDialog", mismatchDialog);

      Gtk::Label* folderName;
      refBuilder1->get_widget("folderName", folderName);
      stringBuffer =   "The folder you selected to store the object is \""
                     + selectedPaletteFolder
                     + "\".";

      folderName->set_text(stringBuffer);

      Gtk::Label* objectType;
      refBuilder1->get_widget("objectType", objectType);
      stringBuffer =   "The object you want to create has the type \""
                     + flagForFolderMatching
                     + "\".";

      objectType->set_text(stringBuffer);

      Gtk::Button* okButton;
      refBuilder1->get_widget("OKButton", okButton);

      Gtk::Button* cancelButton;
      refBuilder1->get_widget("CancelButton", cancelButton);

      Gtk::RadioButton* changeFolderButton;
      refBuilder1->get_widget("changeFolderButton", changeFolderButton);
      stringBuffer =   "Put this object in the \""
                     + flagForFolderMatching
                     + "\" folder.";

      changeFolderButton->set_label(stringBuffer);

      Gtk::RadioButton* changeTypeButton;
      refBuilder1->get_widget("changeTypeButton", changeTypeButton);
      stringBuffer =   "Change the type of the object to \""
                     + selectedPaletteFolder
                     + "\".";

      changeTypeButton->set_label(stringBuffer);

      Gtk::RadioButton* justDoItButton;
      refBuilder1->get_widget("justDoItButton", justDoItButton);

      int result = mismatchDialog->run();
      mismatchDialog->hide();

      if (changeFolderButton->get_active()){
        decision = 0;
      }
      else if (changeTypeButton->get_active()){
        decision = 1;
      }
      else{
        decision = 2;
      }
    } //end handle type mismatch

    if (decision == 0){
      //change folder
      std::cout << " mismatched object and folder is not properly implemented.";
      std::cout << " don't do this! might cause CMOD not functioning properly.";
      std::cout << " also duplicated file names are not checked here.";
      std::cout << std::endl;
      modified();
      IEvent* newEvent = new IEvent();
      newEvent->setEventName(nameEntry->get_text());
      newEvent->setEventType(type);
      paletteView->insertEvent(newEvent,flagForFolderMatching);
      events.push_back(newEvent);
    }
    else if(decision ==1){
      //change type

      std::cout << " mismatched object and folder is not properly implemented.";
      std::cout << " don't do this! might cause CMOD not functioning properly.";
      std::cout << " also duplicated file names are not checked here.";
      std::cout << std::endl;
      modified();
      IEvent* newEvent = new IEvent();
      newEvent->setEventName(nameEntry->get_text());

      if (selectedPaletteFolder =="Top"){
        newEvent->setEventType(eventTop);
      }
      else if (selectedPaletteFolder =="High"){
        newEvent->setEventType(eventHigh);
      }
      else if (selectedPaletteFolder =="Mid"){
        newEvent->setEventType(eventMid);
      }
      else if (selectedPaletteFolder =="Low"){
        newEvent->setEventType(eventLow);
      }
      else if (selectedPaletteFolder =="Bottom"){
        newEvent->setEventType(eventBottom);
      }
      else if (selectedPaletteFolder =="Spectrum"){
        newEvent->setEventType(eventSound);
      }
      else if (selectedPaletteFolder =="Envelope"){
        newEvent->setEventType(eventEnv);
      }
      else if (selectedPaletteFolder =="Sieve"){
        newEvent->setEventType(eventSiv);
      }
      else if (selectedPaletteFolder =="High"){
        newEvent->setEventType(eventHigh);
      }
      else if (selectedPaletteFolder =="Spatialization"){
        newEvent->setEventType(eventSpa);
      }
      else if (selectedPaletteFolder =="Pattern"){
        newEvent->setEventType(eventPat);
      }
      else if (selectedPaletteFolder =="Reverb"){
        newEvent->setEventType(eventRev);
      }

      else if (selectedPaletteFolder =="Filter"){
        newEvent->setEventType(eventFil);
      }
      else if (selectedPaletteFolder =="Note"){
        newEvent->setEventType(eventNote);
      }
			// else if (selectedPaletteFolder =="Measurement"){
			// 	newEvent->setEventType(eventMea);
			// }
      paletteView->insertEvent(newEvent);
      events.push_back(newEvent);
    }
    else{ //decision == 2
      std::string name = nameEntry->get_text();

      std::vector<IEvent*>::iterator it;
      int renameDialogFlag = 1;
      //check object name exist.
      for(it = events.begin(); it != events.end(); it ++){
        if(   (*it)->getEventName() == nameEntry->get_text()
           && (*it)->getEventType() == type
           || nameEntry->get_text() == ""
           && renameDialogFlag == 1){

          Glib::RefPtr<Gtk::Builder> refBuilder2
          = Gtk::Builder::create();
          Gtk::Dialog* renameDialog;
          Gtk::Entry* newNameEntry;
          Gtk::Button* okButton;
          Gtk::Button* cancelButton;

          refBuilder2->add_from_file(
          "./LASSIE/src/UI/CheckObjectNameExists.ui");
          refBuilder2->get_widget("renameDialog", renameDialog);
          refBuilder2->get_widget("okButton", okButton);
          refBuilder2->get_widget("cancelButton", cancelButton);

          renameDialogFlag = renameDialog->run();
          renameDialog->hide();

          refBuilder2->get_widget("newNameEntry", newNameEntry);
          nameEntry = newNameEntry;

          it = events.begin();
        }
      }

      if(   nameEntry->get_text() != ""
         && renameDialogFlag != 0){
        modified();
        IEvent* newEvent = new IEvent();
        newEvent->setEventName(nameEntry->get_text());

        newEvent->setEventType(type);


        int order = paletteView->getCurrentMaxObjectNumber(
          newEvent->getEventFolderName());
        order += 10;

        newEvent->setEventOrderInPalette(order);

        paletteView->insertEvent(newEvent);
        events.push_back(newEvent);

      }
    }
  }

  delete newObjectDialog;
  newObjectDialog = NULL;
}


//----------------------------------------------------------------------------//

std::string ProjectViewController::getPathAndName(){
  return pathAndName;
}


//----------------------------------------------------------------------------//

void ProjectViewController::newObjectButtonClicked(){
  newObjectDialog->hide();
}


//----------------------------------------------------------------------------//

void ProjectViewController::projectPropertiesDialogButtonClicked(){
  projectPropertiesDialog->hide();
}


//----------------------------------------------------------------------------//

//called by palette View to show the attributes of selected event
void ProjectViewController::showAttributes(IEvent* _event){
  if (_event->getEventType() ==eventFolder){
    if (_event->getEventName() == "Top" ){
      topWindow->show_all_children();
      topWindow->show();
    }
    else if (_event->getEventName() == "High" ){
      highWindow->show();
    }
    else if (_event->getEventName() == "Mid" ){
      midWindow->show();
    }
    else if (_event->getEventName() == "Low" ){
      lowWindow->show();
    }
    else if (_event->getEventName() == "Bottom" ){
      bottomWindow->show();
    }
    else if (_event->getEventName() == "Spectrum" ){
      spectrumWindow->show();
    }
    else if (_event->getEventName() == "Note" ){
      noteWindow->show();
    }
    else if (_event->getEventName() == "Envelope" ){
      envWindow->show();
    }
    else if (_event->getEventName() == "Sieve" ){
      sivWindow->show();
    }
    else if (_event->getEventName() == "Spatialization" ){
      spaWindow->show();
    }
    else if (_event->getEventName() == "Pattern" ){
      patWindow->show();
    }
    else if (_event->getEventName() == "Reverb" ){
      revWindow->show();
    }
    else if (_event->getEventName() == "Filter" ){
      filWindow->show();
    }
		// else if (_event->getEventName() == "Measurement" ){
    //   meaWindow->show();
    // }
  }

  eventAttributesView->showAttributesOfEvent(_event);

}


//----------------------------------------------------------------------------//

void ProjectViewController::refreshObjectNameInPalette(IEvent* _event){
  paletteView->refreshObjectName(_event);
  //projectTreeView->refreshObjectName(_event);
}


//----------------------------------------------------------------------------//

void ProjectViewController::setProperties (){
  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      refBuilder->add_from_file("./LASSIE/src/UI/ProjectPropertiesDialog.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!refBuilder->add_from_file(
           "./LASSIE/src/UI/ProjectPropertiesDialog.ui", error)){

      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

  //Get the GtkBuilder-instantiated Dialog:
  refBuilder->get_widget("projectPropertiesDialog", projectPropertiesDialog);
  //newObjectDialog->set_parent(*this);

  //Get the GtkBuilder-instantiated Button, and connect a signal handler:
  Gtk::Button* okButton;
  refBuilder->get_widget("OK", okButton);

  Gtk::Button* cancelButton;
  refBuilder->get_widget("Cancel", cancelButton);

  Gtk::Button* button3;
  refBuilder->get_widget("button3", button3);
  button3->signal_clicked().connect(
      sigc::mem_fun(
        *this,
        &ProjectViewController::projectPropertiesDialogFunctionButtonClicked) );

  if (okButton){
    okButton->signal_clicked().connect(
      sigc::mem_fun(
        *this,&ProjectViewController::projectPropertiesDialogButtonClicked) );
  }

  if (cancelButton){
    cancelButton->signal_clicked().connect(
    sigc::mem_fun(
      *this,&ProjectViewController::projectPropertiesDialogButtonClicked) );
  }

  Gtk::Entry* entry;
  refBuilder->get_widget("titleEntry", entry);
  entry->set_text(projectTitle);

  refBuilder->get_widget("flagEntry", entry);
  entry->set_text(fileFlag);

  refBuilder->get_widget("durationEntry", entry);
  entry->set_text(duration);
  entry->grab_focus();

  refBuilder->get_widget("channelEntry", entry);
  entry->set_text(numOfChannels);

  refBuilder->get_widget("sampleRateEntry", entry);
  entry->set_text(sampleRate);

  refBuilder->get_widget("sampleSizeEntry", entry);
  entry->set_text(sampleSize);

  refBuilder->get_widget("threadsEntry", entry);
  entry->set_text(numOfThreads);

  refBuilder->get_widget("topEventEntry", entry);
  entry->set_text(topEvent);

	// refBuilder->get_widget("measureEntry", entry);
	// entry->set_text(measure);

  Gtk::CheckButton* button;
  refBuilder->get_widget("synthesisCheckBox", button);

  button->set_active(synthesis);

	refBuilder->get_widget("ScoreCheckBox", button);

  button->set_active(score);

  refBuilder->get_widget("generateParticelCheckBox", button);

  button->set_active(outputParticel);

  int result = projectPropertiesDialog->run();

  if (result ==1){
    refBuilder->get_widget("titleEntry", entry);
    projectTitle = entry->get_text();

    refBuilder->get_widget("flagEntry", entry);
    fileFlag = entry->get_text();

    refBuilder->get_widget("durationEntry", entry);
    duration = entry->get_text();

    refBuilder->get_widget("channelEntry", entry);
    numOfChannels = entry->get_text();

    refBuilder->get_widget("sampleRateEntry", entry);
    sampleRate = entry->get_text();

    refBuilder->get_widget("sampleSizeEntry", entry);
    sampleSize = entry->get_text();

    refBuilder->get_widget("threadsEntry", entry);
    numOfThreads = entry->get_text();

    refBuilder->get_widget("topEventEntry", entry);
    topEvent = entry->get_text();

		// refBuilder->get_widget("measureEntry", entry);
		// measure = entry->get_text();

    refBuilder->get_widget("synthesisCheckBox", button);

    synthesis = button->get_active();
		refBuilder->get_widget("ScoreCheckBox", button);

    score = button->get_active();

    refBuilder->get_widget("generateParticelCheckBox", button);

    outputParticel =button->get_active();

  }

  delete projectPropertiesDialog;
  projectPropertiesDialog = NULL;
  //refreshProjectDotDat();
  modified();

}


//----------------------------------------------------------------------------//

void ProjectViewController::refreshProjectDotDat(){

  std::string stringbuffer;
  FILE* dat  = fopen(datPathAndName.c_str(), "w");

  stringbuffer = "title = \"" + projectTitle + "\";\n";

  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse() == 0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal title value!"<<endl;
  }

  stringbuffer = "fileFlags = \"" + fileFlag + "\";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal fileFlag value!"<<endl;
  }

  stringbuffer = "fileList = \"T/" + topEvent + "\";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
    if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal fileList value!"<<endl;
  }

	// stringbuffer = "fileMeasure = \"T/" + measure + "\";\n";
  // yy_scan_string( stringbuffer.c_str());//set parser buffer
  //   if (yyparse()==0){
  //   fputs(stringbuffer.c_str(),dat);
  // }
  // else {
  //   cout<<"illegal filemeasure value!"<<endl;
  // }

  stringbuffer = "pieceStartTime = 0;\n";
  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "pieceDuration = " + duration + ";\n\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal pieceDuration value!"<<endl;
  }


  if (synthesis){
    stringbuffer = "soundSynthesis = TRUE;\n";
  }
  else{
    stringbuffer = "soundSynthesis = FALSE;\n";
  }

  fputs(stringbuffer.c_str(),dat);

	if (score){
    stringbuffer = "ScorePrint = TRUE;\n";
  }
  else{
    stringbuffer = "ScorePrint = FALSE;\n";
  }

  fputs(stringbuffer.c_str(),dat);

  stringbuffer = "numChannels = " + numOfChannels + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal numChannels value!"<<endl;
  }

  stringbuffer = "sampleRate = " + sampleRate + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal sampleRate value!"<<endl;
  }

  stringbuffer = "sampleSize = " + sampleSize + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal sampleSize value!"<<endl;
  }

  stringbuffer = "numThreads = " + numOfThreads + ";\n";
  yy_scan_string( stringbuffer.c_str());//set parser buffer
  if (yyparse()==0){
    fputs(stringbuffer.c_str(),dat);
  }
  else {
    cout<<"illegal numThreads value!"<<endl;
  }


  /* metadata */

  std::string buffer2;
  buffer2 = (projectTitle == "")? "": projectTitle;
  stringbuffer = "LASSIETITLE = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (fileFlag == "")? "": fileFlag;
  stringbuffer = "LASSIEFILEFLAGS = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (topEvent == "")? "": topEvent;
  stringbuffer = "LASSIEFILELIST = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

	// buffer2 = (measure == "")? "": measure;
  // stringbuffer = "LASSIEFILEMEASURE = `" + buffer2 + "`;\n";
  // fputs(stringbuffer.c_str(),dat);

  stringbuffer = "LASSIEPIECESTARTTIME = `0`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (duration == "")? "": duration;
  stringbuffer = "LASSIEPIECEDURATION = `" + buffer2 + "`;\n\n";
  fputs(stringbuffer.c_str(),dat);

  if (synthesis){
    stringbuffer = "LASSIESOUNDSYNTHESIS = `TRUE`;\n";
  }
  else{
    stringbuffer = "LASSIESOUNDSYNTHESIS = `FALSE`;\n";
  }

  fputs(stringbuffer.c_str(),dat);

	if (score){
    stringbuffer = "LASSIESCOREPRINT = `TRUE`;\n";
  }
  else{
    stringbuffer = "LASSIESCOREPRINT = `FALSE`;\n";
  }

  fputs(stringbuffer.c_str(),dat);

  buffer2 = (numOfChannels == "")? "": numOfChannels;
  stringbuffer = "LASSIENUMCHANNELS = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (sampleRate == "")? "": sampleRate;
  stringbuffer = "LASSIESAMPLERITE = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (sampleSize == "")? "": sampleSize;
  stringbuffer = "LASSIESAMPLESIZE = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  buffer2 = (numOfThreads == "")? "": numOfThreads;
  stringbuffer = "LASSIENUMTHREADS = `" + buffer2 + "`;\n";
  fputs(stringbuffer.c_str(),dat);

  fclose(dat);
}


//---------------------------------------------------------------------------//

void ProjectViewController::save(){
  modifiedButNotSaved = false;
  sharedPointers->mainWindow->setSavedTitle();

  eventAttributesView->saveCurrentShownEventData();
  /*
  for (std::vector<IEvent*>::iterator iter = events.begin();
       iter != events.end();
       ++iter){
    (*iter)->saveToDisk(pathAndName);
  }
  */
  clearDeletedEvents();
  //saveEnvelopeLibrary();
  //saveNoteModifierConfiguration();
  //refreshProjectDotDat();

  // The code below is used to save XML file

  string fileName = pathAndName + "/" +projectTitle+".dissco";

  FILE* file  = fopen(fileName.c_str(), "w");

  fputs("<?xml version='1.0'?>\n",file);
  fputs("<ProjectRoot>\n", file);

  string stringBuffer;

  fputs("  <ProjectConfiguration>\n", file);

  stringBuffer = "    <Title>"+ projectTitle + "</Title>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <FileFlag>"+ fileFlag + "</FileFlag>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <TopEvent>"+ topEvent + "</TopEvent>\n";
  fputs(stringBuffer.c_str(),file);

	// stringBuffer = "    <Measure>"+ measure + "</Measure>\n";
  // fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <PieceStartTime>0</PieceStartTime>\n";
  fputs(stringBuffer.c_str(),file);


  stringBuffer = "    <Duration>"+ duration + "</Duration>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = synthesis? "    <Synthesis>True</Synthesis>\n":"    <Synthesis>False</Synthesis>\n";
  fputs(stringBuffer.c_str(),file);

	stringBuffer = score? "    <Score>True</Score>\n":"    <Score>False</Score>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <NumberOfChannels>"+ numOfChannels + "</NumberOfChannels>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <SampleRate>"+ sampleRate + "</SampleRate>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <SampleSize>"+ sampleSize + "</SampleSize>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <NumberOfThreads>"+ numOfThreads + "</NumberOfThreads>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = outputParticel? "    <OutputParticel>True</OutputParticel>\n":"    <OutputParticel>False</OutputParticel>\n";
  fputs(stringBuffer.c_str(),file);

  stringBuffer = "    <Seed>" + seed + "</Seed>\n";
  fputs(stringBuffer.c_str(),file);

  fputs("  </ProjectConfiguration>\n", file);

  fputs("  <NoteModifiers>\n", file);
  fputs("    <DefaultModifiers>",file);

  stringBuffer = "";
  std::map<string, bool>::iterator iter = defaultNoteModifiers.begin();

  while (iter != defaultNoteModifiers.end()){
    //cout <<(*iter).second;
    string buffer2 = ((*iter).second)? "1":"0";
    stringBuffer = stringBuffer + buffer2;
    iter++;
    if (iter!= defaultNoteModifiers.end()){
      stringBuffer = stringBuffer + ", ";
    }
  }
  fputs(stringBuffer.c_str(),file);
  fputs("</DefaultModifiers>\n",file);

  fputs("    <CustomModifiers>\n",file);
  if (customNoteModifiers.size() !=0){

    std::vector<string>::iterator iter2 = customNoteModifiers.begin();

    while (iter2 != customNoteModifiers.end()){
      stringBuffer ="      <Modifier>"+  *iter2 + "</Modifier>\n" ;
      iter2 ++;
      fputs(stringBuffer.c_str(),file);
    }
  }

  fputs("    </CustomModifiers>\n",file);
  fputs("  </NoteModifiers>\n", file);


  //save Envelopes

  fputs("  <EnvelopeLibrary>\n",file);

  char charbuffer[60];

  if (envelopeLibraryEntries !=NULL){ //safety check


    EnvelopeLibraryEntry* envLib = envelopeLibraryEntries;
    int count = envLib->count();

    //put the first number in to the file
    //std::cout<<count<<std::endl;
    sprintf(charbuffer,"%d\n",count);
    fputs(charbuffer,file);

    count = 1;//use count as a index counter

    while (envLib!=NULL){
      sprintf (charbuffer, "Envelope %d\n", count);
          fputs(charbuffer,file);
      //envLib->print();


      int lineNumber = envLib->head->countNumOfNodes();
      sprintf(charbuffer,"%d\n",lineNumber);
      fputs(charbuffer,file);

      EnvLibEntryNode* currentNode;
      EnvLibEntrySeg* libSeg = envLib-> head->rightSeg;
      while (libSeg!=NULL){
        currentNode = libSeg->leftNode;


        sprintf(charbuffer,"%.3f", currentNode->x);
        stringBuffer = charbuffer;
        stringBuffer = stringBuffer + "     ";
        //stringBuffer =  charbuffer+ "\t";


        sprintf(charbuffer,"%.3f", currentNode->y);

        stringBuffer = stringBuffer + charbuffer +"     ";

        if (libSeg->segmentType == envSegmentTypeLinear){
          stringBuffer = stringBuffer + "LINEAR              ";
        }
        else if (libSeg->segmentType ==envSegmentTypeExponential){
          stringBuffer = stringBuffer + "EXPONENTIAL         ";
        }
        else {
          stringBuffer = stringBuffer + "CUBIC_SPLINE        ";
        }


        if (libSeg->segmentProperty == envSegmentPropertyFlexible){
          stringBuffer = stringBuffer + "FLEXIBLE    ";
        }
        else {
          stringBuffer = stringBuffer + "FIXED       ";
        }


        sprintf (charbuffer,"%.3f\n", libSeg->rightNode->x - currentNode->x);
        stringBuffer = stringBuffer+charbuffer;


        fputs(stringBuffer.c_str(),file);
        libSeg = libSeg->rightNode->rightSeg;

      }

      currentNode = currentNode->rightSeg->rightNode;

      sprintf(charbuffer,"%.3f", currentNode->x);
      stringBuffer = charbuffer;
      stringBuffer = stringBuffer + "     ";

      sprintf(charbuffer,"%.3f", currentNode->y);
      stringBuffer = stringBuffer + charbuffer+ "\n";
      fputs(stringBuffer.c_str(),file);

      //end print one envelope
      count++;
      envLib = envLib->next;
    }

  } // end if envelope library is not null


  fputs("  </EnvelopeLibrary>\n",file);

  // save markov models
  fputs("  <MarkovModelLibrary>\n", file);
  fprintf(file, "%ld\n", markovModels.size());
  for (int i = 0; i < markovModels.size(); i++) {
    fprintf(file, "%s\n", markovModels[i].to_str().c_str());
  }
  fputs("  </MarkovModelLibrary>\n", file);


  fputs("  <Events>\n", file);
  for (std::vector<IEvent*>::iterator iter = events.begin();
       iter != events.end();
       iter++){
    if ((*iter)->getEventType()!=eventFolder){

      stringBuffer = (*iter)->getXMLString();
      fputs(stringBuffer.c_str(), file);
    }

    (*iter)->saveToDisk(pathAndName);

  }
  fputs("  </Events>\n",file);

  fputs("</ProjectRoot>",file);
  fclose(file);

}


//---------------------------------------------------------------------------//

PaletteViewController* ProjectViewController::getPalette(){
  return paletteView;
}


//---------------------------------------------------------------------------//

IEvent* ProjectViewController::getEventByTypeAndName(
  EventType type,std::string _name){

  std::vector<IEvent*>::iterator i = events.begin();
  bool found = false;
  IEvent* beReturn = NULL;

  while( i != events.end()&&!found){
    if (    (*i)->getEventName()
         == _name&& (*i)->getEventType()
         == type){

      found == true;
      beReturn = *i;
    }
    i ++;
  }
  return beReturn;
}


//---------------------------------------------------------------------------//

EnvelopeLibraryEntry* ProjectViewController::createNewEnvelope(){
  if (envelopeLibraryEntries == NULL) {
    envelopeLibraryEntries = new EnvelopeLibraryEntry(1);
    return envelopeLibraryEntries;
  }
  else {
    return envelopeLibraryEntries->createNewEnvelope();
  }
}

//---------------------------------------------------------------------------//

EnvelopeLibraryEntry* ProjectViewController:: duplicateEnvelope(EnvelopeLibraryEntry* _originalEnvelope) {
  return envelopeLibraryEntries->duplicateEnvelope(_originalEnvelope);
}


int ProjectViewController::createNewMarkovModel() {
  markovModels.push_back(MarkovModel<float>());
  return markovModels.size() - 1;
}

int ProjectViewController::duplicateMarkovModel(int idx) {
  if (idx < 0 || idx >= markovModels.size()) {
    return -1;
  }
  markovModels.push_back(MarkovModel<float>(markovModels[idx]));
  return markovModels.size() - 1;
}

vector<MarkovModel<float>>& ProjectViewController::getMarkovModels() {
  return markovModels;
}


//---------------------------------------------------------------------------//

void ProjectViewController::saveEnvelopeLibrary(){
  std::string stringbuffer;
  char charbuffer[60];
  FILE* file  = fopen(libPathAndName.c_str(), "w");

  if (envelopeLibraryEntries ==NULL){ //safety check
    //std::cout<<"There is no premade envelopes in the library"<<std::endl;
    fclose(file);
    return;
  }


//---------------------------------------------------------------------------//

  EnvelopeLibraryEntry* envLib = envelopeLibraryEntries;
  int count = envLib->count();

  //put the first number in to the file
  //std::cout<<count<<std::endl;
  sprintf(charbuffer,"%d\n",count);
  fputs(charbuffer,file);

  count = 1;//use count as a index counter

  while (envLib!=NULL){
    sprintf (charbuffer, "Envelope %d\n", count);
        fputs(charbuffer,file);
    //envLib->print();

    int lineNumber = envLib->head->countNumOfNodes();
    sprintf(charbuffer,"%d\n",lineNumber);
    fputs(charbuffer,file);

    EnvLibEntryNode* currentNode;
    EnvLibEntrySeg* libSeg = envLib-> head->rightSeg;
    while (libSeg!=NULL){
      currentNode = libSeg->leftNode;


      sprintf(charbuffer,"%.3f", currentNode->x);
      stringbuffer = charbuffer;
      stringbuffer = stringbuffer + "     ";
      //stringbuffer =  charbuffer+ "\t";


      sprintf(charbuffer,"%.3f", currentNode->y);

      stringbuffer = stringbuffer + charbuffer +"     ";

      if (libSeg->segmentType == envSegmentTypeLinear){
        stringbuffer = stringbuffer + "LINEAR              ";
      }
      else if (libSeg->segmentType ==envSegmentTypeExponential){
        stringbuffer = stringbuffer + "EXPONENTIAL         ";
      }
      else {
        stringbuffer = stringbuffer + "CUBIC_SPLINE        ";
      }

      if (libSeg->segmentProperty == envSegmentPropertyFlexible){
        stringbuffer = stringbuffer + "FLEXIBLE    ";
      }
      else {
        stringbuffer = stringbuffer + "FIXED       ";
      }

      sprintf (charbuffer,"%.3f\n", libSeg->rightNode->x - currentNode->x);
      stringbuffer = stringbuffer+charbuffer;


      fputs(stringbuffer.c_str(),file);
      libSeg = libSeg->rightNode->rightSeg;
    }

    currentNode = currentNode->rightSeg->rightNode;

    sprintf(charbuffer,"%.3f", currentNode->x);
    stringbuffer = charbuffer;
    stringbuffer = stringbuffer + "     ";

    sprintf(charbuffer,"%.3f", currentNode->y);
    stringbuffer = stringbuffer + charbuffer+ "\n";
    fputs(stringbuffer.c_str(),file);

    //end print one envelope

    count++;
    envLib = envLib->next;
  }

  fclose(file);
}


/*! \brief Constructor for opening XML file
*
*/

ProjectViewController::ProjectViewController(
      MainWindow* _mainWindow,
      std::string _pathAndName,
      std::string _projectTitle
){

  seed = "";
  modifiedButNotSaved = false;
  ///////////////////////////////////////////////drag and drop//////////////
  listTargets.push_back( Gtk::TargetEntry("STRING") );
  //listTargets.push_back( Gtk::TargetEntry("text/plain") );
  //////////////////////////////////////////////////////////////////////////
  //datPathAndName = _datPathAndName;
  //libPathAndName = _libPathAndName;
  pathAndName = _pathAndName;

  sharedPointers = new SharedPointers();
  sharedPointers->mainWindow = _mainWindow;
  sharedPointers->projectView = this;

  //Start Parsing File
  XMLPlatformUtils::Initialize();
  XercesDOMParser* parser = new XercesDOMParser();

  string disscoFile = pathAndName +"/"+_projectTitle;
	//removeExtraEntry(disscoFile);

  parser->parse(disscoFile.c_str());
  xmlDocument = parser->getDocument();
  DOMElement* root = xmlDocument->getDocumentElement();

  char* charBuffer;
  DOMCharacterData* textData;

  //Configurations
  DOMElement* configuration = root->getFirstElementChild();
  //title
  DOMElement* element = configuration ->getFirstElementChild();
  projectTitle = IEvent::getFunctionString( element);

  //fileFlag
  element = element->getNextElementSibling();
  fileFlag = IEvent::getFunctionString(element);
  //topEvent
  element = element->getNextElementSibling();
  topEvent = IEvent::getFunctionString(element);

	//measure
   element = element->getNextElementSibling();
   measure = IEvent::getFunctionString(element);
	 //if measure !=0,there is a measure entry in .dissco file, parse next element
	 //else, take the previous parse as the one for piece start time
	 if (measure!="0"){
		 //piece start time is skipped (it's zero, hardcoded)
		 element = element->getNextElementSibling();
	 }

  //duration
  element = element->getNextElementSibling();
  duration = IEvent::getFunctionString(element);
  //synthesis
  element = element->getNextElementSibling();
  synthesis = (IEvent::getFunctionString(element)=="True")?true:false;


	element = element->getNextElementSibling();

	string temp = IEvent::getFunctionString(element);
	if (temp == "True" || temp == "False") {
		score = (temp =="True")?true:false;
		element = element->getNextElementSibling();
	}
  //numOfChannels
  numOfChannels = IEvent::getFunctionString(element);

  //sampleRate
  element = element->getNextElementSibling();
  sampleRate = IEvent::getFunctionString(element);

  //sampleSize
  element = element->getNextElementSibling();
  sampleSize = IEvent::getFunctionString(element);

  //numOfThreads
  element = element->getNextElementSibling();
  numOfThreads = IEvent::getFunctionString(element);
  // Output Particel
  element = element->getNextElementSibling();
  if (element){
    outputParticel = (IEvent::getFunctionString(element)=="True")?true:false;
  }
  else outputParticel=false;
  //restore notemodifiers

/*  defaultNoteModifiers.insert(pair<string,bool>("-8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("+8va",true));
  defaultNoteModifiers.insert(pair<string,bool>("bend",true));
  defaultNoteModifiers.insert(pair<string,bool>("dry",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissKeys",true));
  defaultNoteModifiers.insert(pair<string,bool>("glissStringRes",true));
  defaultNoteModifiers.insert(pair<string,bool>("graceTie",true));
  defaultNoteModifiers.insert(pair<string,bool>("letVibrate",true));
  defaultNoteModifiers.insert(pair<string,bool>("moltoVibrato",true));
  defaultNoteModifiers.insert(pair<string,bool>("mute",true));
  defaultNoteModifiers.insert(pair<string,bool>("pedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("pluck",true));
  defaultNoteModifiers.insert(pair<string,bool>("pressSilently",true));
  defaultNoteModifiers.insert(pair<string,bool>("resonance",true));
  defaultNoteModifiers.insert(pair<string,bool>("resPedal",true));
  defaultNoteModifiers.insert(pair<string,bool>("sfz",true));
  defaultNoteModifiers.insert(pair<string,bool>("sffz",true));
  defaultNoteModifiers.insert(pair<string,bool>("tenuto",true));
  defaultNoteModifiers.insert(pair<string,bool>("tremolo",true));
  defaultNoteModifiers.insert(pair<string,bool>("vibrato",true));*/
	// initialize default note modifiers
	defaultNoteModifiers.insert(pair<string,bool>("-8va",true));
	defaultNoteModifiers.insert(pair<string,bool>("+8va",true));
	defaultNoteModifiers.insert(pair<string,bool>("accent",true));
	defaultNoteModifiers.insert(pair<string,bool>("espressivo",true));
	defaultNoteModifiers.insert(pair<string,bool>("marcato",true));
	defaultNoteModifiers.insert(pair<string,bool>("portato",true));
	defaultNoteModifiers.insert(pair<string,bool>("staccatissimo",true));
	defaultNoteModifiers.insert(pair<string,bool>("staccato",true));
	defaultNoteModifiers.insert(pair<string,bool>("tenuto",true));
	defaultNoteModifiers.insert(pair<string,bool>("prall",true));
	defaultNoteModifiers.insert(pair<string,bool>("prallup",true));
	defaultNoteModifiers.insert(pair<string,bool>("pralldown",true));
	defaultNoteModifiers.insert(pair<string,bool>("upprall",true));
	defaultNoteModifiers.insert(pair<string,bool>("downprall",true));
	defaultNoteModifiers.insert(pair<string,bool>("prallprall",true));
	defaultNoteModifiers.insert(pair<string,bool>("lineprall",true));
	defaultNoteModifiers.insert(pair<string,bool>("prallmordent",true));
	defaultNoteModifiers.insert(pair<string,bool>("mordent",true));
	defaultNoteModifiers.insert(pair<string,bool>("upmordent",true));
	defaultNoteModifiers.insert(pair<string,bool>("downmordent",true));
	defaultNoteModifiers.insert(pair<string,bool>("trill",true));
	defaultNoteModifiers.insert(pair<string,bool>("turn",true));
	defaultNoteModifiers.insert(pair<string,bool>("reverseturn",true));
	defaultNoteModifiers.insert(pair<string,bool>("shortfermata",true));
	defaultNoteModifiers.insert(pair<string,bool>("fermata",true));
	defaultNoteModifiers.insert(pair<string,bool>("longfermata",true));
	defaultNoteModifiers.insert(pair<string,bool>("verylongfermata",true));
	defaultNoteModifiers.insert(pair<string,bool>("upbow",true));
	defaultNoteModifiers.insert(pair<string,bool>("downbow",true));
	defaultNoteModifiers.insert(pair<string,bool>("flageolet",true));
	defaultNoteModifiers.insert(pair<string,bool>("open",true));
	defaultNoteModifiers.insert(pair<string,bool>("halfopen",true));
	defaultNoteModifiers.insert(pair<string,bool>("lheel",true));
	defaultNoteModifiers.insert(pair<string,bool>("rheel",true));
	defaultNoteModifiers.insert(pair<string,bool>("ltoe",true));
	defaultNoteModifiers.insert(pair<string,bool>("rtoe",true));
	defaultNoteModifiers.insert(pair<string,bool>("snappizzicato",true));
	defaultNoteModifiers.insert(pair<string,bool>("stopped",true));
	defaultNoteModifiers.insert(pair<string,bool>("segno",true));
	defaultNoteModifiers.insert(pair<string,bool>("coda",true));
	defaultNoteModifiers.insert(pair<string,bool>("varcoda",true));
	defaultNoteModifiers.insert(pair<string,bool>("null",true));


  DOMElement* noteModifiers = configuration->getNextElementSibling();
  element = noteModifiers->getFirstElementChild(); //default modifiers
  textData = ( DOMCharacterData*) element->getFirstChild();
  charBuffer = XMLString::transcode(textData->getData());

  std::map<string, bool>::iterator
      modifierMapIter = defaultNoteModifiers.begin();
  int index = 0;
  while( modifierMapIter != defaultNoteModifiers.end()){
    (*modifierMapIter).second = true;
//    (*modifierMapIter).second = (charBuffer[index]=='1')?true:false;
    modifierMapIter ++;
    index = index + 3;
  }
  XMLString::release(&charBuffer);

  element = element->getNextElementSibling(); //custom modifiers
  DOMElement* modifier = element->getFirstElementChild();

  while (modifier != NULL){
    textData = ( DOMCharacterData*) modifier->getFirstChild();
    charBuffer = XMLString::transcode(textData->getData());
    customNoteModifiers.push_back(string(charBuffer));
    XMLString::release(&charBuffer);
    modifier = modifier->getNextElementSibling();
  }


  //construct new eventAttributesView after constructing the note modifiers
  //create three new children widgets
  //projectTreeView = new ProjectTreeViewController(sharedPointers);
  eventAttributesView = new EventAttributesViewController(sharedPointers);
  paletteView = new PaletteViewController(sharedPointers);

eventAttributesView->buildNoteModifiersList();
  //set the attributes of two Paned widgets and add children view in them
  leftTwoPlusAttributes.set_position(200);
  leftTwoPlusAttributes.pack1(*paletteView,true,false);
  leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);

  add(leftTwoPlusAttributes);
  showContents();


  sharedPointers->eventAttributesView = eventAttributesView;
  sharedPointers->paletteView = paletteView;


  //Envelope Library

  envelopeLibraryEntries =NULL;
  DOMElement* envelopeLibraryElement = noteModifiers->getNextElementSibling();
  textData = ( DOMCharacterData*) envelopeLibraryElement->getFirstChild();

  charBuffer = XMLString::transcode(textData->getData());
  string fileString = pathAndName + ".lib.temp";
  FILE* file  = fopen(fileString.c_str(), "w");
	if (file==NULL){
		cout<<strerror(errno)<<endl;
	}
  fputs (charBuffer, file);
  XMLString::release(&charBuffer);
  fclose(file);

  EnvelopeLibrary* envelopeLibrary = new EnvelopeLibrary();
  envelopeLibrary->loadLibraryNewFormat((char*)fileString.c_str());
  string deleteCommand = "rm " + fileString;
  system(deleteCommand.c_str());

  EnvelopeLibraryEntry* previousEntry = NULL;
  Envelope* thisEnvelope;

  for (int i = 1; i <= envelopeLibrary->size(); i ++){
    thisEnvelope = envelopeLibrary->getEnvelope(i);
    EnvelopeLibraryEntry* thisEntry =
      convertToLASSIEEnvLibEntry(thisEnvelope, i);
    delete thisEnvelope;

    if (previousEntry ==NULL){
      envelopeLibraryEntries = thisEntry;
      previousEntry = thisEntry;
      thisEntry->prev = NULL;
    }
    else{
      previousEntry->next = thisEntry;
      thisEntry->prev = previousEntry;
    }
    previousEntry = thisEntry;
  }

  delete envelopeLibrary;

  DOMElement* currentElement = envelopeLibraryElement;

  // Markov Model
  DOMElement* markovModelLibraryElement = envelopeLibraryElement->getNextElementSibling();
  string tagName = charBuffer = XMLString::transcode(markovModelLibraryElement->getTagName());
  XMLString::release(&charBuffer);
  if (tagName == "MarkovModelLibrary") {
    currentElement = markovModelLibraryElement;
    DOMText* text = dynamic_cast<DOMText*>(markovModelLibraryElement->getFirstChild());
    string data = charBuffer = XMLString::transcode(text->getWholeText());
    XMLString::release(&charBuffer);

    std::stringstream ss(data);

    // read the number of models
    int size;
    ss >> size;
    markovModels.resize(size);

    // read individual models
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

      markovModels[i].from_str(modelText);
    }
  }

  //  constructs all events
  DOMElement* domEvents =  currentElement->getNextElementSibling();
  DOMElement* eventElement = domEvents->getFirstElementChild();

  while (eventElement != NULL){
    IEvent* newEvent = new IEvent( eventElement);
    paletteView->insertEvent(newEvent, newEvent->getEventTypeString());
    events.push_back(newEvent);
    eventElement =eventElement->getNextElementSibling();
  }

  std::vector<IEvent*>::iterator eventsIter = events.begin();

  for (eventsIter; eventsIter != events.end(); eventsIter++){
    (*eventsIter)->link(this);
  }


  topWindow = new ObjectWindow(eventTop,this);
  highWindow = new ObjectWindow(eventHigh,this);
  midWindow = new ObjectWindow(eventMid,this);
  lowWindow = new ObjectWindow(eventLow,this);
  bottomWindow = new ObjectWindow(eventBottom,this);
  spectrumWindow = new ObjectWindow(eventSound,this);
  envWindow = new ObjectWindow(eventEnv,this);
  sivWindow = new ObjectWindow(eventSiv,this);
  spaWindow = new ObjectWindow(eventSpa,this);
  patWindow = new ObjectWindow(eventPat,this);
  revWindow = new ObjectWindow(eventRev,this);
  filWindow = new ObjectWindow(eventFil, this);
  noteWindow = new ObjectWindow(eventNote,this);
	meaWindow = new ObjectWindow(eventMea, this);

  show_all_children();
}


//---------------------------------------------------------------------------//

EnvelopeLibraryEntry* ProjectViewController::getEnvelopeLibraryEntries(){
	return envelopeLibraryEntries;
}


//---------------------------------------------------------------------------//

EnvelopeLibraryEntry* ProjectViewController::
  convertToLASSIEEnvLibEntry( Envelope* _envelope, int index){
  return new EnvelopeLibraryEntry (_envelope, index);
}


//---------------------------------------------------------------------------//

IEvent* ProjectViewController::findIEvent(
  EventType _type, std::string _eventName){

  IEvent* toReturn = NULL;

  std::vector<IEvent*>::iterator eventsIter = events.begin();

  while (eventsIter != events.end()){
    if (  (_eventName.compare( (*eventsIter)->getEventName() ) == 0 ) &&
          ( _type == (*eventsIter)->getEventType() ) ){
      toReturn = (*eventsIter);
      break;
    }
    eventsIter++;
  }

  if (toReturn ==NULL){
    std::cout<<"Find event fail :-(    EventName:"
      <<_eventName<<"  type:" << (int) _type<< std::endl;
  }
  return toReturn;
}


//---------------------------------------------------------------------------//

void ProjectViewController::deleteKeyPressed(Gtk::Widget* _focus){
 	if (_focus ==NULL){
 		return;
 	}

  if (_focus->is_ancestor((Gtk::Widget&)*paletteView)){
    paletteView->deleteKeyPressed();
  }
  else {
    eventAttributesView->deleteKeyPressed(_focus);
  }
}


//---------------------------------------------------------------------------//

void ProjectViewController::nKeyPressed(Gtk::Widget* _focus){
  if (_focus ==NULL){
    return;
  }

  if (_focus->is_ancestor((Gtk::Widget&)*paletteView)){
    insertObject();
  }
}


//---------------------------------------------------------------------------//

bool ProjectViewController::getEmptyProject(){
  return emptyProject;
}


//---------------------------------------------------------------------------//

void ProjectViewController::projectPropertiesDialogFunctionButtonClicked (){
  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat, duration);
    generator->run();

  if (generator->getResultString() !=""){
    duration = generator->getResultString();
  }

  delete generator;

  projectPropertiesDialog->hide();
  delete projectPropertiesDialog;
  setProperties();

}


//---------------------------------------------------------------------------//

void ProjectViewController::modified(){
  if (modifiedButNotSaved == false){
    modifiedButNotSaved = true;
    sharedPointers->mainWindow->setUnsavedTitle();

  }
}


//---------------------------------------------------------------------------//

bool ProjectViewController::getSaved(){
  return !modifiedButNotSaved;
}


//---------------------------------------------------------------------------//

void ProjectViewController::showAttributesView(bool _show){
  if (_show && leftTwoPlusAttributes.get_child2() ==NULL ){

    leftTwoPlusAttributes.pack2(*eventAttributesView,true,false);
    show_all_children();
  }
  else if (!_show &&leftTwoPlusAttributes.get_child2() != NULL){
    leftTwoPlusAttributes.remove(*(leftTwoPlusAttributes.get_child2()));
    show_all_children();

  }
}


//---------------------------------------------------------------------------//

void ProjectViewController::configureNoteModifiers(){
  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> noteModifiersConfigurationDialogRefBuilder =
    Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      noteModifiersConfigurationDialogRefBuilder->add_from_file(
        "./LASSIE/src/UI/NoteModifiersConfiguration.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!noteModifiersConfigurationDialogRefBuilder->add_from_file(
      "./LASSIE/src/UI/NoteModifiersConfiguration.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */


  //Get the GtkBuilder-instantiated Dialog:
  noteModifiersConfigurationDialogRefBuilder->get_widget(
    "NoteConfigurationDialog", noteModifiersConfigurationDialog);
    noteModifiersConfigurationDialog->set_title("Note Modifiers Configuration");

  noteModifiersConfigurationDialogRefBuilder->get_widget(
    "CustomModifiersVBox",noteModifiersConfigurationCustomVBox );

  CustomNoteModifierHBox* box;
  vector<string>::iterator iter = customNoteModifiers.begin();

  for (iter; iter!= customNoteModifiers.end(); iter++){
    box = new CustomNoteModifierHBox(this, *iter);

    customNotModifierHBoxes.push_back(box);
    noteModifiersConfigurationCustomVBox->pack_start(*box,Gtk::PACK_SHRINK);
  }

  Gtk::CheckButton* checkButton;
// /////////////////////////////////////////////////////////// //
cout << defaultNoteModifiers.size() << endl;
  if (defaultNoteModifiers["-8va"]){
    noteModifiersConfigurationDialogRefBuilder->get_widget(
      "Minus8vaButton", checkButton);
    checkButton->set_active(true);
   }

   if (defaultNoteModifiers["+8va"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "Plus8vaButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["bend"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "BendButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["dry"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "DryButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["glissKeys"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GlissKeysButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["glissStringRes"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GlissStringResButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["graceTie"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "GraceTieButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["letVibrate"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "LetVibrateButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["moltoVibrato"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "MoltoVibratoButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["mute"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "MuteButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["pedal"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PedalButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["pluck"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PluckButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["pressSilently"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "PressSilentlyButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["resonance"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "ResonanceButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["resPedal"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "ResPedalButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["sfz"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "SfzButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["sffz"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "SffzButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["tenuto"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "TenutoButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["tremolo"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "TremoloButton", checkButton);
     checkButton->set_active(true);
   }

   if (defaultNoteModifiers["vibrato"]){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
      "VibratoButton", checkButton);
     checkButton->set_active(true);
   }

   Gtk::Button* button;
   noteModifiersConfigurationDialogRefBuilder->get_widget(
     "AddModifierButton", button);

   button->signal_clicked().connect(sigc::mem_fun
     (*this, &ProjectViewController::ConfigureNoteModifiersAddButtonClicked));


   noteModifiersConfigurationDialog->show_all_children();

   int result = noteModifiersConfigurationDialog->run();

   noteModifiersConfigurationDialog->hide();

//  ----Sever stopped here
   if (result == 1){
     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "Minus8vaButton", checkButton);
     defaultNoteModifiers["-8va"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "Plus8vaButton", checkButton);
     defaultNoteModifiers["+8va"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "BendButton", checkButton);
     defaultNoteModifiers["bend"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "DryButton", checkButton);
     defaultNoteModifiers["dry"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "GlissKeysButton", checkButton);
     defaultNoteModifiers["glissKeys"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "GlissStringResButton", checkButton);
     defaultNoteModifiers["glissStringRes"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "GraceTieButton", checkButton);
     defaultNoteModifiers["graceTie"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "LetVibrateButton", checkButton);
     defaultNoteModifiers["letVibrate"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "MoltoVibratoButton", checkButton);
     defaultNoteModifiers["moltoVibrato"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "MuteButton", checkButton);
     defaultNoteModifiers["mute"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "PedalButton", checkButton);
     defaultNoteModifiers["pedal"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "PluckButton", checkButton);
     defaultNoteModifiers["pluck"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "PressSilentlyButton", checkButton);
     defaultNoteModifiers["pressSilently"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "ResonanceButton", checkButton);
     defaultNoteModifiers["resonance"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "ResPedalButton", checkButton);
     defaultNoteModifiers["resPedal"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "SfzButton", checkButton);
     defaultNoteModifiers["sfz"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "SffzButton", checkButton);
     defaultNoteModifiers["sffz"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "TenutoButton", checkButton);
     defaultNoteModifiers["tenuto"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "TremoloButton", checkButton);
     defaultNoteModifiers["tremolo"] = checkButton->get_active();

     noteModifiersConfigurationDialogRefBuilder->get_widget(
       "VibratoButton", checkButton);
     defaultNoteModifiers["vibrato"] = checkButton->get_active();


     std::vector<CustomNoteModifierHBox*>::iterator iter =
       customNotModifierHBoxes.begin();
     customNoteModifiers.clear();
     string modifierText;
     for (iter; iter!= customNotModifierHBoxes.end(); iter++){
       modifierText = (*iter)->getText();
       if (modifierText != ""){
         customNoteModifiers.push_back( (*iter)->getText());
       }
     }

     modified();
     eventAttributesView->buildNoteModifiersList();

   }

   noteModifiersConfigurationDialog = NULL;
   noteModifiersConfigurationCustomVBox = NULL;

   std::vector<CustomNoteModifierHBox*>::iterator iter2 =
     customNotModifierHBoxes.begin();
   for (iter2; iter2!= customNotModifierHBoxes.end(); iter2++){
     delete (*iter2);
   }
   customNotModifierHBoxes.clear();
}


//---------------------------------------------------------------------------//

void ProjectViewController::ConfigureNoteModifiersAddButtonClicked(){
  CustomNoteModifierHBox* newHBox = new CustomNoteModifierHBox(this);
  customNotModifierHBoxes.push_back(newHBox);
  noteModifiersConfigurationCustomVBox->pack_start(*newHBox,Gtk::PACK_SHRINK);
  noteModifiersConfigurationDialog->show_all_children();

}


//---------------------------------------------------------------------------//

CustomNoteModifierHBox::CustomNoteModifierHBox(
                        ProjectViewController* _projectView){
  projectView = _projectView;
  label.set_text("Modifier Name: ");
  removeButton.set_label("Remove Modifier");
  pack_start(label,Gtk::PACK_SHRINK);
  pack_start(entry,Gtk::PACK_EXPAND_WIDGET);
  pack_start(removeButton,Gtk::PACK_SHRINK);

  removeButton.signal_clicked().connect(sigc::mem_fun
    (*this, &CustomNoteModifierHBox::removeButtonClicked));
}

//---------------------------------------------------------------------------//

CustomNoteModifierHBox::CustomNoteModifierHBox(
  	ProjectViewController* _projectView, string _string){
  entry.set_text(_string);

  projectView = _projectView;
  label.set_text("Modifier Name: ");
  removeButton.set_label("Remove Modifier");
  pack_start(label,Gtk::PACK_SHRINK);
  pack_start(entry,Gtk::PACK_EXPAND_WIDGET);
  pack_start(removeButton,Gtk::PACK_SHRINK);

  removeButton.signal_clicked().connect(sigc::mem_fun
    (*this, &CustomNoteModifierHBox::removeButtonClicked));

}


//---------------------------------------------------------------------------//

CustomNoteModifierHBox::~CustomNoteModifierHBox(){
}


//---------------------------------------------------------------------------//

std::string CustomNoteModifierHBox::getText(){
  return entry.get_text();
}


//---------------------------------------------------------------------------//

void CustomNoteModifierHBox::removeButtonClicked(){
  projectView->removeCustomNoteModifier(this);
}


//---------------------------------------------------------------------------//

void ProjectViewController::removeCustomNoteModifier(
  CustomNoteModifierHBox* _hbox){
  std::vector<CustomNoteModifierHBox*>::iterator iter =
    customNotModifierHBoxes.begin();
  while(*iter != _hbox && iter != customNotModifierHBoxes.end()){
    iter++;
  }

  customNotModifierHBoxes.erase(iter);
  noteModifiersConfigurationCustomVBox->remove(*_hbox);
  delete _hbox;
  noteModifiersConfigurationDialog->show_all_children();

}


//---------------------------------------------------------------------------//

void ProjectViewController::saveNoteModifierConfiguration(){
  std::string stringbuffer = pathAndName + "/.noteModifiersConfiguration";
  //char charbuffer[60];

  FILE* file  = fopen(stringbuffer.c_str(), "w");

  stringbuffer = "LASSIENOTEDEFAULTMODIFIER = <";

  std::map<string, bool>::iterator iter = defaultNoteModifiers.begin();

  while (iter != defaultNoteModifiers.end()){
    //cout <<(*iter).second;
    string buffer2 = ((*iter).second)? "1":"0";
    stringbuffer = stringbuffer + buffer2;
    iter++;
    if (iter!= defaultNoteModifiers.end()){
      stringbuffer = stringbuffer + ", ";
    }
  }

  stringbuffer = stringbuffer + ">;\n"  ;
  fputs(stringbuffer.c_str(),file);

  if (customNoteModifiers.size() !=0){

    stringbuffer = "LASSIENOTECUSTOMMODIFIER = <";

    std::vector<string>::iterator iter2 = customNoteModifiers.begin();

    while (iter2 != customNoteModifiers.end()){
      stringbuffer = stringbuffer + "`" + *iter2 + "`";
      iter2 ++;
      if (iter2!= customNoteModifiers.end()){
        stringbuffer = stringbuffer + ", ";
      }
    }
    stringbuffer = stringbuffer + ">;\n";
    fputs(stringbuffer.c_str(),file);
  }

  fclose(file);

}


//---------------------------------------------------------------------------//

std::map<std::string, bool> ProjectViewController::getDefaultNoteModifiers(){
  return defaultNoteModifiers;
}


//---------------------------------------------------------------------------//

std::vector<std::string> ProjectViewController::getCustomNoteModifiers(){
  return customNoteModifiers;
}


//--------------------------------------------------------------------------//

void ProjectViewController::saveAs(std::string _newPathAndName){
  cout<<"'save as' is deprecated."<<endl;
  /*
	pathAndName = _newPathAndName;
	projectTitle  = FileOperations::stringToFileName(pathAndName);
	datPathAndName = pathAndName+ "/"+projectTitle+".dat";
  libPathAndName = pathAndName+ "/"+projectTitle+".lib";

	saveEnvelopeLibrary();
  saveNoteModifierConfiguration();
  refreshProjectDotDat();

  eventAttributesView->saveCurrentShownEventData();
  for (std::vector<IEvent*>::iterator iter = events.begin();
       iter != events.end();
       ++iter){
    (*iter)->saveAsToDisk(pathAndName);
  }
	*/

}


//--------------------------------------------------------------------------//

bool ProjectViewController::checkNameExists(string _name, EventType _type){
  vector<IEvent*>::iterator iter = events.begin();
  bool returnValue = false;
  while (!returnValue && iter != events.end()){

    if ((*iter)->getEventName() == _name && (*iter)->getEventType() == _type){
      returnValue = true;
    }

    iter++;
  }

  return returnValue;

}


//--------------------------------------------------------------------------//

void ProjectViewController::deleteObject(IEvent* _toDelete){
  modified();
  std::vector<IEvent*>::iterator iter =  events.begin();

  while ( (*iter)!= _toDelete && iter!= events.end()){
    iter++;
  }

  events.erase(iter);

  deletedEvents.push_back( _toDelete);

  //check if the object is currently shown in the attributes view.
  if (_toDelete == eventAttributesView->getCurrentEvent()){
    eventAttributesView->showAttributesOfEvent(NULL);
  }

}


//--------------------------------------------------------------------------//

void ProjectViewController::clearDeletedEvents(){
  std::vector <IEvent*>::iterator iter= deletedEvents.begin();
  /*

  for (iter; iter!= deletedEvents.end(); iter++){
    string pathAndNameOfDeletingObject; // = pathAndName;
    string eventName = (*iter)->getEventName();

    switch ((*iter)->getEventType()){
      case 0:
        pathAndNameOfDeletingObject = pathAndName + "/T/" + eventName;

        break;
      case 1:
        pathAndNameOfDeletingObject = pathAndName + "/H/" + eventName;

        break;
      case 2:
        pathAndNameOfDeletingObject = pathAndName + "/M/" + eventName;

        break;
      case 3:
        pathAndNameOfDeletingObject = pathAndName + "/L/" + eventName;

        break;
      case 4:
        pathAndNameOfDeletingObject = pathAndName + "/B/" + eventName;

        break;
      case 5:
        pathAndNameOfDeletingObject = pathAndName + "/S/" + eventName;

        break;

      case 6:
        pathAndNameOfDeletingObject = pathAndName + "/ENV/" + eventName;
        break;
      case 7 :
        pathAndNameOfDeletingObject = pathAndName + "/SIV/" + eventName;
        break;

      case 8:
        pathAndNameOfDeletingObject = pathAndName + "/SPA/" + eventName;
        break;

      case 9:
        pathAndNameOfDeletingObject = pathAndName + "/PAT/" + eventName;
        break;
      case 10:
        pathAndNameOfDeletingObject = pathAndName + "/REV/" + eventName;
        break;
      case 11:
        break;
      case 12:
        pathAndNameOfDeletingObject = pathAndName + "/N/" + eventName;
        break;
    }

    //cout<<"deleting object: "<<  pathAndNameOfDeletingObject<<"..."<<endl;
    string command = "rm "+ pathAndNameOfDeletingObject;
    system(command.c_str());
    delete *iter;


  }
  */
  deletedEvents.clear();

}


//----------------------------------------------------------------------------//

std::string ProjectViewController::searchPossibleParents(string _fileName){
  //cout<<"_fileName is :"<<_fileName<<endl;
  string result = "";
  std::vector<IEvent*>::iterator iter= events.begin();

  for( iter; iter!= events.end(); iter++){

    if ((*iter)->haveString(_fileName)){
      result = result + (*iter)->getEventFolderName() + "/" +
        (*iter)->getEventName() + ", ";

    }
  }

  return result;
}

//remove abandoned entry measurement in project dialog by searching .dissco file
//for the line and removing it
void ProjectViewController::removeExtraEntry(string _fileName){
	FILE* disscofile;
	if ((disscofile=fopen(_fileName.c_str(),"r+")) == NULL){
		cout<<"Cannot open dissco file while trying to remove measurement"<<endl;
	}
	char temp[50];
	char* location;
	char target[]="    <Measure> ";
	for (int i=0;i<8;i++){
		fgets(temp,50,disscofile);
		if (strncmp(target,temp,13)==0){
			cout<<temp<<endl;
		}


	}
	fclose(disscofile);

}
