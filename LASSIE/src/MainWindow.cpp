/*******************************************************************************
*
*  File Name     : MainWindow.cpp
*  Date created  : Jan. 24 2010
*  Authors       : Ming-ching Chiu, Sever Tipei
*  Organization  : Music School, University of Illinois at Urbana Champaign
*  Description   : This file contains the class of main window of LASSIE
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

#include "MainWindow.h"

#include "ProjectViewController.h"
#include "FileOperations.h"
#include "HelpOperations.h"
#include "EnvelopeLibraryWindow.h"
#include "MarkovModelLibraryWindow.h"


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
extern YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size );
extern void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer );
extern void yy_delete_buffer ( YY_BUFFER_STATE buffer );
extern FILE* yyin;

#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
#endif

bool marked_as_unsaved = false; /* to be safe that we don't overwrite files that just happen to begin with '*' */

/** \namespace \brief helper functions to be used in this file _only_ **/
namespace MWHelper {
  std::string formatProjectViewWindowTitle(std::string pathAndName, std::string projectTitle, bool need_to_save){
    std::string title = pathAndName + "/" +  projectTitle +  ".dissco - LASSIE";
    if(need_to_save){
      title = "*" + title;
      marked_as_unsaved = true;
    }
    return title;
  }
}


/*! \brief The constructor of MainWindow
*
*******************************************************************************/
MainWindow::MainWindow(){

  set_title("LASSIE");
  set_default_size(1000, 600);
  set_decorated(true);
  set_position(Gtk::WIN_POS_CENTER_ALWAYS);


  add(mainBox); // put the main box into the window


  // Create actions for menus and toolbars:
  menuRefActionGroup = Gtk::ActionGroup::create();

  // File|New sub menu:
  menuRefActionGroup->add(
    Gtk::Action::create(
      "FileNewProject",
      Gtk::Stock::NEW,
      "Create a new project"),
    sigc::mem_fun(*this, &MainWindow::menuFileNewProject));

  menuRefActionGroup->add(
    Gtk::Action::create(
      "FileNewObject",
      Gtk::Stock::ADD,
      "_Create a new Object",
      "Create a new object"),
		  Gtk::AccelKey("<alt>N"),
    sigc::mem_fun(*this, &MainWindow::menuFileNewObject));


  // File and its sub-menu
  menuRefActionGroup->add(Gtk::Action::create("FileMenu", "File"));



  menuRefActionGroup->add(
    Gtk::Action::create(
      "FileOpen",
      Gtk::Stock::OPEN,
      "_Open an existing project",
      "Open an existing project"),
    //sigc::mem_fun(*this, &MainWindow::menuFileOpen));
    sigc::mem_fun(*this, &MainWindow::menuFileOpenXML));
  menuRefActionGroup->add(
    Gtk::Action::create(
      "FileSave",
      Gtk::Stock::SAVE,
      "_Save the project",
      "Save the project"),
    sigc::mem_fun(*this, &MainWindow::menuFileSave));

/*
  menuRefActionGroup->add(
    Gtk::Action::create("OpenProjectOldFormat", "Open Project in Old Format"),
    sigc::mem_fun(*this, &MainWindow::menuFileOpen));
*/

  menuRefActionGroup->add(
    Gtk::Action::create("FileSaveAs",Gtk::Stock::SAVE_AS),
    sigc::mem_fun(*this, &MainWindow::menuFileSaveAs));

  menuRefActionGroup->add(
    Gtk::Action::create("FileClose",Gtk::Stock::CLOSE),
    sigc::mem_fun(*this, &MainWindow::menuFileClose));

  menuRefActionGroup->add(
    Gtk::Action::create("FilePrint",Gtk::Stock::PRINT),
    sigc::mem_fun(*this, &MainWindow::menuFilePrint));

  menuRefActionGroup->add(
    Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
    sigc::mem_fun(*this, &MainWindow::menuFileQuit));



  // Edit and its sub-menu
  menuRefActionGroup->add(Gtk::Action::create("EditMenu", "Edit"));

  menuRefActionGroup->add(
    Gtk::Action::create("EditUndo", Gtk::Stock::UNDO),
    sigc::mem_fun(*this, &MainWindow::menuEditUndo));

  menuRefActionGroup->add(
    Gtk::Action::create("EditRedo", Gtk::Stock::REDO),
    sigc::mem_fun(*this, &MainWindow::menuEditUndo));

  menuRefActionGroup->add(
    Gtk::Action::create("EditCut", Gtk::Stock::CUT),
    sigc::mem_fun(*this, &MainWindow::menuEditCut));

  menuRefActionGroup->add(
    Gtk::Action::create("EditCopy", Gtk::Stock::COPY),
    sigc::mem_fun(*this, &MainWindow::menuEditCopy));

  menuRefActionGroup->add(
    Gtk::Action::create("EditPaste", Gtk::Stock::PASTE),
    sigc::mem_fun(*this, &MainWindow::menuEditPaste));

  menuRefActionGroup->add(
    Gtk::Action::create("EditSelectAll", Gtk::Stock::SELECT_ALL),
    sigc::mem_fun(*this, &MainWindow::menuEditSelectAll));

  menuRefActionGroup->add(
    Gtk::Action::create("EditPreferences", Gtk::Stock::PREFERENCES),
    sigc::mem_fun(*this, &MainWindow::menuEditSelectAll));


  // Project Menu
  menuRefActionGroup->add(Gtk::Action::create("ProjectMenu", "Project"));

  menuRefActionGroup->add(
    Gtk::Action::create("ProjectProperties", Gtk::Stock::PROPERTIES),
    sigc::mem_fun(*this, &MainWindow::menuProjectProperties));

  menuRefActionGroup->add(
    Gtk::Action::create("Run", "Run"),
    sigc::mem_fun(*this, &MainWindow::menuProjectRun));

/*
  menuRefActionGroup->add(
    Gtk::Action::create("GenerateSCFile", "Generate SuperCollider File"),
    sigc::mem_fun(*this, &MainWindow::menuProjectGenerateSCFile));
*/

  menuRefActionGroup->add(
    Gtk::Action::create("ConfigureNoteModifiers", "Configure Note Modifiers"),
    sigc::mem_fun(*this, &MainWindow::menuProjectConfigureNoteModifiers));


  // Help and its sub-menu
  menuRefActionGroup->add( Gtk::Action::create("HelpMenu", "Help"));

  menuRefActionGroup->add(
    Gtk::Action::create("HelpContents",Gtk::Stock::HELP, "Contents"),
    sigc::mem_fun(*this, &MainWindow::menuHelpContents));

  menuRefActionGroup->add(
    Gtk::Action::create("HelpAbout", Gtk::Stock::ABOUT),
    sigc::mem_fun(*this, &MainWindow::menuHelpAbout));



  menuRefActionGroup->get_action("FileNewObject")->set_sensitive(false);
  //menuRefActionGroup->get_action("FileNewObject")->add_accel_label("N");
  menuRefActionGroup->get_action("FileSave")->set_sensitive(false);
  menuRefActionGroup->get_action("FileSaveAs")->set_sensitive(false);
  menuRefActionGroup->get_action("ProjectProperties")->set_sensitive(false);
  menuRefActionGroup->get_action("Run")->set_sensitive(false);
//  menuRefActionGroup->get_action("GenerateSCFile")->set_sensitive(false);
  menuRefActionGroup->get_action(
    "ConfigureNoteModifiers")->set_sensitive(false);



  // UIManager reads the XML below to layout the menu.
  // add_accel_group activates using hotkeys within this window.
  menuRefUIManager = Gtk::UIManager::create();
  menuRefUIManager->insert_action_group(menuRefActionGroup);
  add_accel_group(menuRefUIManager->get_accel_group());



  // Layout the actions in a menubar and toolbar:
  ui_info_befor =
        "<ui>"
        "  <menubar name='menuBar'>"
        "    <menu action='FileMenu'>"
        "      <menuitem action='FileNewProject'/>"
        "      <menuitem action='FileOpen'/>"
        "      <menuitem action='FileSave'/>"
        "      <menuitem action='FileSaveAs'/>"
        "      <separator/>";

  ui_info_after =
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "      <separator/>"
        "    </menu>"
        "     <menu action='ProjectMenu'>"
        "      <menuitem action='FileNewObject'/>"
        "      <menuitem action='ProjectProperties'/>"
        "      <menuitem action = 'Run'/>"
        "      <menuitem action = 'ConfigureNoteModifiers'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpContents'/>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action = 'FileNewProject'/>"
        "    <toolitem action='FileOpen'/>"
        "    <toolitem action='FileSave'/>"
        "    <toolitem action='FileSaveAs'/>"
        "    <separator/>"
        "    <toolitem action='FileNewObject'/>"
        "    <separator/>"
        "    <toolitem action='HelpContents'/>"
        "  </toolbar>"
        "</ui>";

  ui_info_action = "";

  ui_info = ui_info_befor + ui_info_after;

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    menuRefUIManager->add_ui_from_string(ui_info);
  }
  catch(const Glib::Error& ex){
    std::cerr << "building menus failed: " <<  ex.what();
  }
  #else
  std::auto_ptr<Glib::Error> ex;
  menuRefUIManager->add_ui_from_string(ui_info, ex);
  if(ex.get()){
    std::cerr << "building menus failed: " <<  ex->what();
  }
  #endif // GLIBMM_EXCEPTIONS_ENABLED


  // Get the menubar and toolbar widgets, and add them to mainBox container.

  pointerToMenubarWidget =
    menuRefUIManager->get_widget("/menuBar");
  if(pointerToMenubarWidget){ // add menu
    mainBox.pack_start(*pointerToMenubarWidget, Gtk::PACK_SHRINK);
  }
  pointerToToolbarWidget =
    menuRefUIManager->get_widget("/ToolBar");
  if(pointerToToolbarWidget){ // add tool
    mainBox.pack_start(*pointerToToolbarWidget, Gtk::PACK_SHRINK);
  }
  ((Gtk::Toolbar*)pointerToToolbarWidget)->set_toolbar_style(Gtk::TOOLBAR_BOTH);





  // make an empty project view to the mainbox.
  // notice that when adding projectView, Pack_EXPAND_WIDGET is used,
  // not PACK_SHRINK,
  // because we want the projectView to fill in the space when
  // the main window expand.
  projectView = new ProjectViewController(this);
  mainBox.pack_start(*projectView, Gtk::PACK_EXPAND_WIDGET);
  // put the pointer of the current projectView into <vector> projects
  projects.push_back(projectView);



  // create an instance of EnvelopeLibraryWindow
  //envelopeLibraryWindow = (EnvelopeLibraryWindow*) new Gtk::Window();
  envelopeLibraryWindow = new EnvelopeLibraryWindow();
  envelopeLibraryWindow->hide();
  envelopeLibraryWindow->setActiveProject(projectView);

  markovModelLibraryWindow = new MarkovModelLibraryWindow();
  markovModelLibraryWindow->hide();
  markovModelLibraryWindow->setActiveProject(projectView);

  Gtk::Main::signal_key_snooper().connect(
    sigc::mem_fun(*this,&MainWindow::captureKeyStroke));


  show_all_children();
}


/*! \brief The destructor of MainWindow
*
*******************************************************************************/
MainWindow::~MainWindow(){
  for(std::vector<ProjectViewController*>::
        iterator it = projects.begin();
      it!=projects.end();
      it++){

    delete *it;
  }
}

// here is the "File menu" action functions block
////////////////////////////////////////////////////////////////////////////////

void MainWindow::menuFileNewProject(){
  ProjectViewController* newProject = FileOperations::newProject(this);

  if(newProject!= NULL){
    menuRefActionGroup->get_action("FileNewObject")->set_sensitive(true);
    menuRefActionGroup->get_action("FileSave")->set_sensitive(true);
    menuRefActionGroup->get_action("FileSaveAs")->set_sensitive(true);
    menuRefActionGroup->get_action("ProjectProperties")->set_sensitive(true);
    menuRefActionGroup->get_action("Run")->set_sensitive(true);
//    menuRefActionGroup->get_action("GenerateSCFile")->set_sensitive(true);


    menuRefActionGroup->get_action(
      "ConfigureNoteModifiers")->set_sensitive(true);
    disableNewAndOpenProject();

    projects.push_back(newProject);
    MainWindow::includeUi_info(newProject->getPathAndName(),"add");
    changeCurrentProjectViewTo(newProject);

    std::string title = MWHelper::formatProjectViewWindowTitle(newProject->getPathAndName(), newProject->getProjectTitle(), true);
    set_title(title);

    newProject->setProperties();
  }
}

//-----------------------------------------------------------------------------

void MainWindow::menuFileNewObject(){
  if (projectView!= NULL &&projectView->getEmptyProject() == false){
    projectView->insertObject();
  }
}


//-----------------------------------------------------------------------------

void MainWindow::menuFileOpenXML(){

  ProjectViewController* openProject = FileOperations::openXMLProject(this);

  if(openProject!= NULL){
    menuRefActionGroup->get_action("FileNewObject")->set_sensitive(true);
    menuRefActionGroup->get_action("FileSave")->set_sensitive(true);
    menuRefActionGroup->get_action("FileSaveAs")->set_sensitive(true);
    menuRefActionGroup->get_action("ProjectProperties")->set_sensitive(true);
    menuRefActionGroup->get_action("Run")->set_sensitive(true);
//    menuRefActionGroup->get_action("GenerateSCFile")->set_sensitive(true);
    menuRefActionGroup->get_action(
      "ConfigureNoteModifiers")->set_sensitive(true);
    disableNewAndOpenProject();

    projects.push_back(openProject);

    MainWindow::includeUi_info(openProject->getPathAndName(),"add");
    changeCurrentProjectViewTo(openProject);
    std::string title = MWHelper::formatProjectViewWindowTitle(openProject->getPathAndName(), openProject->getProjectTitle(), false);
    set_title(title);

    openProject->setProperties();
    openProject->initializeModifiers();
  }
}


//-----------------------------------------------------------------------------

void MainWindow::menuFileSave(){
  if (projectView->getEmptyProject()== false){
    projectView->save();
  }
}

//-----------------------------------------------------------------------------


/**  \brief called externally to mark/unmark title with '*' indicator if the file is modified/unmodified, respectively **/
void MainWindow::markTitleAsUnsaved(bool mark){
  if(mark == true){
    set_title("*" + get_title());
    marked_as_unsaved = true;
    return;
  }
  
  if(marked_as_unsaved == true) set_title(get_title().erase(0,1));
  marked_as_unsaved = false;
}

//-----------------------------------------------------------------------------

void MainWindow::menuFileSaveAs(){

// 	string newPathAndName = FileOperations::saveAs(this);

// 	if (newPathAndName =="") {
// 		return ;
// 	}
// 	std::string title = MWHelper::formatProjectViewWindowTitle(this, projectView->pathAndName, projectView->projectTitle, false);
//  set_title(title);

// 	projectView->saveAs(newPathAndName);
}


//-----------------------------------------------------------------------------

void MainWindow::menuFileClose(){
  FileOperations::close(this);
}


//-----------------------------------------------------------------------------

void MainWindow::menuFilePrint(){}//TODO


//-----------------------------------------------------------------------------

void MainWindow::menuFileQuit(){
  hide(); //Closes the main window to stop the Gtk::Main::run().
}
////////////////////////////////////////////////////////////////////////////////

// here is the "Edit menu" action functions block
////////////////////////////////////////////////////////////////////////////////
void MainWindow::menuEditUndo(){}//TODO
void MainWindow::menuEditRedo(){}//TODO
void MainWindow::menuEditCut(){}//TODO
void MainWindow::menuEditCopy(){}//TODO
void MainWindow::menuEditPaste(){}//TODO
void MainWindow::menuEditSelectAll(){}//TODO
void MainWindow::menuEditPreferences(){}//TODO
////////////////////////////////////////////////////////////////////////////////

// here is the "Help menu" action functions block
////////////////////////////////////////////////////////////////////////////////
void MainWindow::menuHelpContents(){
  HelpOperations::showContents();
}


//-----------------------------------------------------------------------------

void MainWindow::menuHelpAbout(){
  HelpOperations::showAbout();
}


//-----------------------------------------------------------------------------

void MainWindow::changeCurrentProjectViewTo(
                  ProjectViewController* _newProject)
{
  mainBox.remove(*projectView);
  mainBox.pack_start(*_newProject, Gtk::PACK_EXPAND_WIDGET);
  projectView = _newProject;
  envelopeLibraryWindow->setActiveProject(_newProject);
  markovModelLibraryWindow->setActiveProject(_newProject);

  show_all_children();
}


//-----------------------------------------------------------------------------

void MainWindow::menuPathAndName(){}//TODO


//-----------------------------------------------------------------------------

void MainWindow::includeUi_info(Glib::ustring pathAndName,
                                Glib::ustring ctrl)
{
  if(ctrl == "add"){
    mainBox.remove(*pointerToMenubarWidget);
    mainBox.remove(*pointerToToolbarWidget);

    ui_info_action = ui_info_action +
                     "<menuitem action='" +
                     pathAndName +
                        "'/>";

    ui_info = ui_info_befor + ui_info_action + ui_info_after;

    menuRefActionGroup->add(
      Gtk::Action::create(pathAndName,pathAndName),
      sigc::mem_fun(*this, &MainWindow::menuPathAndName)
    );

    menuRefUIManager = Gtk::UIManager::create();
    menuRefUIManager->insert_action_group(menuRefActionGroup);
    add_accel_group(menuRefUIManager->get_accel_group());

    menuRefUIManager->add_ui_from_string(ui_info);

    pointerToMenubarWidget = menuRefUIManager->get_widget("/menuBar");
    if(pointerToMenubarWidget){ // add menu
      mainBox.pack_start(*pointerToMenubarWidget, Gtk::PACK_SHRINK);
    }

    pointerToToolbarWidget = menuRefUIManager->get_widget("/ToolBar");
    if(pointerToToolbarWidget){ // add tool
      ((Gtk::Toolbar*)pointerToToolbarWidget)->
        set_toolbar_style(Gtk::TOOLBAR_BOTH);
      mainBox.pack_start(*pointerToToolbarWidget, Gtk::PACK_SHRINK);
    }
  }else if(ctrl == "delete"){}//TODO
}


//-----------------------------------------------------------------------------

void MainWindow::menuProjectProperties(){
	projectView->setProperties();
}

/* to do:
*    implements functions and function descriptions
*/


//-----------------------------------------------------------------------------

void MainWindow::showEnvelopeLibraryWindow(){
  envelopeLibraryWindow->show();
}


void MainWindow::showMarkovModelLibraryWindow() {
  markovModelLibraryWindow->show();
}


//-----------------------------------------------------------------------------

int MainWindow::captureKeyStroke(
  Gtk::Widget* _widget,GdkEventKey* _gdkEventKey){

  //delete key stroke
  if (_gdkEventKey->type ==8 &&_gdkEventKey->keyval == 65535){
    if (projectView != NULL&& projectView->getPathAndName() != " "){
      projectView->deleteKeyPressed(get_focus());
    }
  }
/*
  //this chunk is handled by accelkey of FileNewObject
  if (_gdkEventKey->type == 8 &&_gdkEventKey->keyval == 110){ //n stroke!
    if (projectView!= NULL&& projectView->getPathAndName() != ""){
      rojectView->nKeyPressed(get_focus());// for palette to add object
    }
  }
*/
/*
 GdkEventType type;   8 press 9 release
 GdkWindow *window; this window
 gint8 send_event; don't know what's this
 guint32 time; event time
 guint state; usually 16 ctrl keys otherwise
 guint keyval; ascii
 gint length;
 gchar *string;  return ascii code
 guint16 hardware_keycode; keyboard key code
 guint8 group;
*/

  return 0;
}


//-----------------------------------------------------------------------------

void MainWindow::menuProjectRun(){
  Gtk::Dialog* runDialog;

  //Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> runDialogRefBuilder =
    Gtk::Builder::create();


  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      runDialogRefBuilder->add_from_file(
        "./LASSIE/src/UI/RunDialog.ui");
    }
    catch(const Glib::FileError& ex){
      std::cerr << "FileError: " << ex.what() << std::endl;
    }
    catch(const Gtk::BuilderError& ex){
      std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
  #else
    std::auto_ptr<Glib::Error> error;

    if (!runDialogRefBuilder->add_from_file(
      "./LASSIE/src/UI/RunDialog.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif // !GLIBMM_EXCEPTIONS_ENABLED


  //Get the GtkBuilder-instantiated Dialog:
  runDialogRefBuilder->get_widget("RunDialog", runDialog);

  Gtk::Entry* randomSeedEntry;
  runDialogRefBuilder->get_widget(
    "RunDialogRandomSeedEntry", randomSeedEntry);
	randomSeedEntry->grab_focus();

  int result = runDialog->run();

	runDialog->hide();

  if(result == 1 && projectView) {

    string pathAndName = projectView->getPathAndName();
    string projectPath = pathAndName + "/";
    string projectName = projectView->getProjectTitle();
    //string dat = projectPath + projectName + ".dat";
    projectView->setSeed(randomSeedEntry->get_text());
    projectView->save();

    cout<<"projectPath"<<projectPath<<endl;
    //FILE* newBufferFile = fopen (dat.c_str(), "r");
    //yyin = fopen (dat.c_str(), "r");

    // create a new buffer for parser
    //YY_BUFFER_STATE newParserBuffer = yy_create_buffer ( yyin,YY_BUF_SIZE);

    //yy_switch_to_buffer (newParserBuffer);

    //Determine project sound file output.
    PieceHelper::createSoundFilesDirectory(projectPath);

    /*
    string soundFilename =
           PieceHelper::getNextSoundFile(projectPath, projectName);

    if(soundFilename == "")
      cout << "A new soundfile name could not be reserved." << endl;
      //TODO: Make message box to warn user.
      //(But this error should  not happen!)
    else {

      */

      //if (fork() == 0){
      //cout<<"this is the child. execute cmod"<<endl;

      string command = "xterm -e \"bash -c \\\"./cmod " + projectPath
                         + projectName + ".dissco";
      command = command + " ; exec bash\\\"\"";
      system(command.c_str()) ;
      //yy_delete_buffer(newParserBuffer);

      projectView->setSeed(""); //reset seed
    }
}


//-----------------------------------------------------------------------------

void MainWindow::disableNewAndOpenProject(){
  menuRefActionGroup->get_action("FileNewProject")->set_sensitive(false);

  menuRefActionGroup->get_action("FileOpen")->set_sensitive(false);
}


//-----------------------------------------------------------------------------

void MainWindow::menuProjectConfigureNoteModifiers(){
  if (projectView!= NULL &&projectView->getEmptyProject() == false){
    projectView->configureNoteModifiers();
  }
}


//-----------------------------------------------------------------------------
// this function is not used
void MainWindow::menuProjectGenerateSCFile(){
  projectView->getTop()->makeSuperColliderCode();
}
