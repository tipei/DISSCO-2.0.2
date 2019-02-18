/*******************************************************************************
 *
 *  File Name     : PaletteViewController.cpp
 *  Date created  : Jan. 29 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the palette view of LASSIE. The palette
 *                  contains all the object which can be used by the users.
 *==============================================================================
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
 ******************************************************************************/

#include "PaletteViewController.h"

#include "IEvent.h"
#include "ProjectViewController.h"
#include "SharedPointers.h"
#include "MainWindow.h"
#include "EventAttributesViewController.h"
#include "ObjectWindow.h"


int PaletteViewController::slot_comp_func( const Gtk::TreeModel::iterator& a,  const Gtk::TreeModel::iterator& b) {

  int cmp = (*a).get_value(columns.columnObjectOrder) - (*b).get_value(columns.columnObjectOrder);
  if (cmp == 0)
    return (*a).get_value(columns.columnObjectName).compare((*b).get_value(columns.columnObjectName));
  if (cmp > 0)
    return 1;
  return -1;
}

PaletteViewController::PaletteViewController(SharedPointers* _sharedPointers):
 // ProjectViewController* _projectView): 
  
  
  
    viewName("Objects List"), 
    envelopeLibButton("Envelope Library"){
  sharedPointers = _sharedPointers;
  projectView = sharedPointers->projectView;
  set_border_width(3);
  //viewName.set_justify(Gtk::JUSTIFY_LEFT);
  pack_start(viewName, Gtk::PACK_SHRINK);
  pack_start(scrolledWindow, Gtk::PACK_EXPAND_WIDGET);
  pack_start(envelopeLibButton, Gtk::PACK_SHRINK);
  
  
  envelopeLibButton.signal_clicked().connect (
    sigc::mem_fun(*this, &PaletteViewController::showEnvelopeLibrary));
  
  

  // Add the TreeView, inside a ScrolledWindow
  scrolledWindow.add(palette);

  // Only show the scrollbars when they are necessary:
  scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);



  // Create the Tree model:
  refTreeModel = Gtk::TreeStore::create(columns);
  palette.set_model(refTreeModel);
  
  
  // All the items to be reordered with drag-and-drop:
  palette.set_reorderable();


  // Add the TreeView's view columns:
  palette.append_column("Type", columns.columnObjectType);
  palette.append_column("Name", columns.columnObjectName);

  #ifdef DEBUG_BUILD
  palette.append_column("Order", columns.columnObjectOrder);
  #endif

  // Connect signal:
  palette.signal_row_activated().connect(
    sigc::mem_fun(*this, &PaletteViewController::objectActivated) );
  palette.signal_cursor_changed().connect(
    sigc::mem_fun(*this,&PaletteViewController::on_cursor_changed) );

  slot_comp = sigc::mem_fun(*this, &PaletteViewController::slot_comp_func);
  refTreeModel->set_default_sort_func(slot_comp);
  refTreeModel->set_sort_column(Gtk::TreeSortable::DEFAULT_SORT_COLUMN_ID, Gtk::SORT_ASCENDING );

  //columns.set_sort_column(columns.columnObjectOrder);
  /*
  Gtk::TreeView::Column* pColumn = refTreeModel->get_column(3);
  if (pColumn){
    pColumn->set_sort_column(columns.columnObjectOrder);
    cout<<"bla?"<<endl;
  }
  */


  //////////////////////////test for drag and drop///////////////////////////

  //Make m_Button_Drag a DnD drag source:
  //palette.drag_source_set(projectView->listTargets);
   viewName.drag_source_set(projectView->listTargets);
   palette.enable_model_drag_source(
   projectView->listTargets,Gdk::MODIFIER_MASK,Gdk::ACTION_COPY);


   //Connect signals: bind current event to the signal
   palette.signal_drag_data_get().connect(
     sigc::mem_fun(*this, &PaletteViewController::on_button_drag_data_get));

   viewName.signal_drag_data_get().connect(
     sigc::mem_fun(*this, &PaletteViewController::on_button_drag_data_get));

  //////////////////////end test ////////////////////////////////////////

  /////////////////////set up popup menu ///////////////////////////////

  m_pMenuPopup = 0;

  //    the signal handler i wished to put was overriden by the object default
  //signal handler, so to avoid this, we have to add a paramater to the connect
  //method (false)

  palette.signal_button_press_event().connect(
    sigc::mem_fun(*this,&PaletteViewController::onRightClick), false);



  //Fill menu:

  m_refActionGroup = Gtk::ActionGroup::create();

  //File|New sub menu:
  //These menu actions would normally already exist for a main menu, because a
  //context menu should not normally contain menu items that are only available
  //via a context menu.
  m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));

  //m_refActionGroup->add(
   // Gtk::Action::create("ContextAdd", "Add To Project Tree"),
    //sigc::mem_fun(*this, &PaletteViewController::AddToProjectTree));
  m_refActionGroup->add(
    Gtk::Action::create("ContextDelete", "Delete"),
    sigc::mem_fun(*this, &PaletteViewController::deleteObject));
    
  m_refActionGroup->add(
    Gtk::Action::create("ContextDuplicate", "Duplicate Object"),
    sigc::mem_fun(*this, &PaletteViewController::duplicateObject));    


  //TODO:
  /*
   //Add a ImageMenuElem:
   menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Something", m_Image,
   sigc::mem_fun(*this, &ExampleWindow::on_menu_file_popup_generic) ) ) ;
   */

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  //projectView->add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  Glib::ustring ui_info =
    "<ui>"
    "  <popup name='PopupMenu'>"
    "    <menuitem action='ContextDelete'/>"
    "		 <menuitem action='ContextDuplicate'/>"
    "  </popup>"
    "</ui>";

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex){
      std::cerr << "building menus failed: " <<  ex.what();
    }
  #else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get()){
      std::cerr << "building menus failed: " <<  ex->what();
    }
  #endif // GLIBMM_EXCEPTIONS_ENABLED

  // Get the menu:
  m_pMenuPopup = dynamic_cast<Gtk::Menu*>(
    m_refUIManager->get_widget("/PopupMenu"));

  if(!m_pMenuPopup) g_warning("menu not found");
  
  
  //test right click change cursor
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
    palette.get_selection();
    
  refTreeSelection->signal_changed().connect(
    sigc::mem_fun(*this, &PaletteViewController::on_selection_changed));
  
  


  show_all_children();
}


PaletteViewController::~PaletteViewController(){}


void PaletteViewController::objectActivated(
  const Gtk::TreeModel::Path& _path, Gtk::TreeViewColumn* /* _column */){

  Gtk::TreeModel::iterator iter = refTreeModel->get_iter(_path);

  if(iter){
    Gtk::TreeModel::Row row = *iter;
    selectedRow = *iter;
    if (row[columns.columnEntry] != NULL){
      projectView->showAttributes(row[columns.columnEntry]);
    }
    
  }
  
}

void PaletteViewController::insertEvent(IEvent* _event){

	ObjectWindow* windowToRefresh = NULL;
  Gtk::TreeModel::Row childrow;

  if (palette.get_selection()->get_selected() ==0) { //see if some row is selected
    childrow = *(refTreeModel->append());
  } else {
    Gtk::TreeModel::Children::iterator iter = palette.get_selection()->get_selected();
    Gtk::TreeModel::Row parent = *iter;
    childrow = *(refTreeModel->append(parent.children()));

    if (parent[columns.columnObjectName] == "Top"){
      windowToRefresh = projectView->topWindow;
    }
    else if (parent[columns.columnObjectName] == "High"){
      windowToRefresh = projectView->highWindow;
    }
    else if (parent[columns.columnObjectName] == "Mid"){
      windowToRefresh = projectView->midWindow;
    }
    else if (parent[columns.columnObjectName] == "Low"){
      windowToRefresh = projectView->lowWindow;
    }
    else if (parent[columns.columnObjectName] == "Bottom"){
      windowToRefresh = projectView->bottomWindow;
    }
    else if (parent[columns.columnObjectName] == "Spectrum"){
      windowToRefresh = projectView->spectrumWindow;
    }
    else if (parent[columns.columnObjectName] == "Envelope"){
      windowToRefresh = projectView->envWindow;
    }
    else if (parent[columns.columnObjectName] == "Sieve"){
      windowToRefresh = projectView->sivWindow;
    }
    else if (parent[columns.columnObjectName] == "Spatialization"){
      windowToRefresh = projectView->spaWindow;
    }
    else if (parent[columns.columnObjectName] == "Pattern"){
      windowToRefresh = projectView->patWindow;
    }
    else if (parent[columns.columnObjectName] == "Reverb"){
      windowToRefresh = projectView->revWindow;
    }
    else if (parent[columns.columnObjectName] == "Note"){
      windowToRefresh = projectView->noteWindow;
    }
     else if (parent[columns.columnObjectName] == "Filter"){
      windowToRefresh = projectView->filWindow;
    }
  }

  childrow[columns.columnObjectType] = _event->getEventTypeString();
  childrow[columns.columnObjectName] = _event->getEventName();
  childrow[columns.columnEntry] =_event;
  childrow[columns.columnObjectOrder] =_event->getEventOrderInPalette();

  if (windowToRefresh != NULL){
    windowToRefresh->refresh();
  }
}

void PaletteViewController::insertEvent(IEvent* _event, std::string _parentName){
  Gtk::TreeModel::Row insertTo;
  //find the row of the given name
  Gtk::TreeModel::Children children = refTreeModel->children();

  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end();
      ++iter){
    Gtk::TreeModel::Row row = *iter;
    if (row[columns.columnObjectName] == _parentName) insertTo = row;
  }

  //insert new event
  Gtk::TreeModel::Row childrow = *(refTreeModel->append(insertTo.children()));
  childrow[columns.columnObjectType] = _event->getEventTypeString();
  childrow[columns.columnObjectName] = _event->getEventName();
  childrow[columns.columnEntry] = _event;
  childrow[columns.columnObjectOrder] =_event->getEventOrderInPalette();
}


//called when object name is changed in the atributesview
void PaletteViewController::refreshObjectName(IEvent* _event){
  Gtk::TreeModel::Row toChange;
  //find the row of the given event
  Gtk::TreeModel::Children children = refTreeModel->children();
  for(Gtk::TreeModel::Children::iterator iter = children.begin();
      iter != children.end();
      iter++){
    Gtk::TreeModel::Row row = *iter;
    //std::cout<<row[columns.columnObjectName]<<std::endl;
    if (row[columns.columnEntry] == _event) toChange = row;
}
 //std::cout<<toChange[columns.columnObjectName]<<std::endl;
//toChange[columns.columnObjectName] = _event->getEventName();
}


void PaletteViewController::on_button_drag_data_get(
  const Glib::RefPtr<Gdk::DragContext>& context,
  Gtk::SelectionData& selection_data,
  guint info,
  guint time){

  Gtk::TreeModel::Children::iterator iter =
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;


  //right here set up a pointer to the right event object

  Glib::ustring name = row[columns.columnObjectName];

  selection_data.set(
    selection_data.get_target(),
    8 /* 8 bits format */,
    (const guchar*)name.c_str(),
    name.length() /* the length of I'm Data! in bytes */);
}


//originally want to show the attributes whenever the cursor move, 
//but it's dangerous so skipped. leave the function body for future usage.
void PaletteViewController::on_cursor_changed(){
//TODO
  //std::cout<<"cursor changed!!"<<std::endl;
  //Gtk::TreeModel::Children::iterator iter = 
    //palette.get_selection()->get_selected();
  //Gtk::TreeModel::Row row = *iter;
  //std::cout<<row[columns.columnObjectName]<<std::endl;

  //projectView->showAttributes(row[columns.columnEntry]);
}


void PaletteViewController::AddToProjectTree(){
  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  //projectView->addObjectFromPaletteToProjectTree(row[columns.columnEntry]);
}


bool PaletteViewController::onRightClick(GdkEventButton* event){

  Gtk::TreeModel::iterator iter = palette.get_selection()->get_selected();
  if(iter){ //If anything is selected
    Gtk::TreeModel::Row row = *iter;
    currentCursorPosition = row[columns.columnEntry];
  }
  else {
    currentCursorPosition = NULL;
  }
  

        
    // This chunk of code change the cursor to the object rightclicked on.
    int x;
    int y;
    Gtk::TreeModel::Path path;
    Gtk::TreeViewColumn* temp_column;

    palette.get_path_at_pos( (int)event->x,
                              (int)event->y,
                              path,
                              temp_column,
                              x,
                              y );
    palette.set_cursor(path);

  // test if right click
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ){ 
  
  
  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  if (!*iter){
	  m_refActionGroup->get_action("ContextDuplicate")->set_sensitive(false);
	  m_refActionGroup->get_action("ContextDelete")->set_sensitive(false);    
	}  
  else if (folderSelected()!= "None"){
  	m_refActionGroup->get_action("ContextDuplicate")->set_sensitive(false); 
  	m_refActionGroup->get_action("ContextDelete")->set_sensitive(false);
  }
  else {
  	m_refActionGroup->get_action("ContextDuplicate")->set_sensitive(true); 
  	m_refActionGroup->get_action("ContextDelete")->set_sensitive(true);
  }
 
    if(m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);



    return true; // It has been handled.
  }else{
    return false;
  }
}


//return None if not a folder is selected
Glib::ustring PaletteViewController::folderSelected(){
  if (palette.get_selection()->get_selected() ==0) return "None";

  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  if (row[columns.columnObjectType]!="Folder") return "None";
  return row[columns.columnObjectName];

}


IEvent* PaletteViewController::getCurrentSelectedEvent(){
  //std::cout<<"cursor changed!!"<<std::endl;
  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  return row[columns.columnEntry];
}


void PaletteViewController::showEnvelopeLibrary(){

  sharedPointers->mainWindow->showEnvelopeLibraryWindow();
}






void PaletteViewController::deleteKeyPressed(){
  //std::cout<<"cursor changed!!"<<std::endl;
  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  
  if (iter){
  Gtk::TreeModel::Row row = *iter;
    if (row){
      deleteObject();
    }

  }
}




void PaletteViewController::deleteObject(){
  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  IEvent* toDelete = row[columns.columnEntry];
  
  
  std::string fileName;
  switch (toDelete->getEventType()){
    case 0:
      fileName = "T/" + toDelete->getEventName();
      break;
    case 1:
      fileName = "H/" + toDelete->getEventName();
      break;
    case 2:
      fileName = "M/" + toDelete->getEventName();
      break;
    case 3:
      fileName = "L/" + toDelete->getEventName();
      break;
    case 4:
      fileName = "B/" + toDelete->getEventName();
      break;
    case 5:
      fileName = "S/" + toDelete->getEventName();
      break;
    case 6:
      fileName = "ENV/" + toDelete->getEventName();
      break;
    case 7:
      fileName = "SIV/" + toDelete->getEventName();
      break;
    case 8:
      fileName = "SPA/" + toDelete->getEventName();
      break;
    case 9:
      fileName = "PAT/" + toDelete->getEventName();
      break;
    case 10:
      fileName = "REV/" + toDelete->getEventName();
      break;
    case 12:
      fileName = "N/" + toDelete->getEventName();
      break;
  }
  string result = projectView->searchPossibleParents(fileName);
  
  if (result != ""){
    string prompt = "The object you are deleting is used by: " + result +
    ". Do you really want to delete it? "
    "CMOD might not run properly if you delete this object.";
  
  
  
    Gtk::MessageDialog dialog(prompt,
      false /* use_markup */,
      Gtk::MESSAGE_QUESTION,
      Gtk::BUTTONS_OK_CANCEL);

    if (dialog.run()== -6){ //user decide to cancel deleting
      return;
    }
  }
  

  refTreeModel->erase(*iter);

  refreshObjectOrder(toDelete->getEventFolderName());
  
  switchBackToPreviousCursor(toDelete);

  projectView->deleteObject(toDelete);
  
}


void PaletteViewController::duplicateObject(){



  Gtk::TreeModel::Children::iterator iter = 
    palette.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
	IEvent* originalEvent = row[columns.columnEntry];
  
  
	//Load the GtkBuilder file and instantiate its widgets:
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      refBuilder->add_from_file("./LASSIE/src/UI/DuplicateObjectDialog.ui");
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
      "./LASSIE/src/UI/DuplicateObjectDialog.ui", error)){
      std::cerr << error->what() << std::endl;
    }
  #endif /* !GLIBMM_EXCEPTIONS_ENABLED */

	

  //Get the GtkBuilder-instantiated Dialog:
  Gtk::Dialog* duplicateObjectDialog;
  refBuilder->get_widget("duplicateObjectDialog", duplicateObjectDialog);
  string prompt = "You are about to duplicate this object:\n\n" + 
    originalEvent->getEventTypeString()
    + "/"+ originalEvent->getEventName() + 
    "\n\nPlease name the newly created copy object";
   
   Gtk::Label* promptLabel;
   refBuilder->get_widget("PromptLabel", promptLabel);
   promptLabel->set_text(prompt);
   
   Gtk::Entry* nameEntry;
   refBuilder->get_widget("NameEntry", nameEntry);
   string name = originalEvent->getEventName()+"_copy";
   nameEntry->set_text(name);
   int response = duplicateObjectDialog->run();
   duplicateObjectDialog->hide();
   
   switchBackToPreviousCursor(NULL);
   
   
   if (response ==1 && nameEntry->get_text() == ""){

    //prompt error (no row selected in the palette)

    Gtk::MessageDialog dialog(
      "Please name your new object",
      false /* use_markup */,
      Gtk::MESSAGE_QUESTION,
      Gtk::BUTTONS_OK);

    dialog.run();
    return;
	}
  else if (response !=1){
  	return;
  } 
  else {
  	if (sharedPointers->projectView->checkNameExists(
  	  nameEntry->get_text(), originalEvent->getEventType())){
    	Gtk::MessageDialog dialog(
      	"Object with the same name exists.",
      	false /* use_markup */,
      	Gtk::MESSAGE_QUESTION,
      	Gtk::BUTTONS_OK);

    dialog.run();
    return;
	  }
	  string firstLetter = nameEntry->get_text().substr(0,1);

	  if (originalEvent->getEventType() == eventBottom &&
	    !(firstLetter== "n"||
	    firstLetter=="s")){
      Gtk::MessageDialog dialog(
        "A Bottom event should start with lowercase letter 's' or 'n'",
        false /* use_markup */,
        Gtk::MESSAGE_QUESTION,
        Gtk::BUTTONS_OK);

      dialog.run();
      return;
	  }
	  sharedPointers->eventAttributesView->saveCurrentShownEventData();
    IEvent* newIEvent = new IEvent( originalEvent, nameEntry->get_text());
    string typeString;
    /*
    if (newIEvent->getEventTypeString() =="Env."){
  	  typeString = "Envelope";
    }
    else if (newIEvent->getEventTypeString() =="Spat."){
    	typeString = "Spatialization";
    }
    else  if (newIEvent->getEventTypeString() =="Rev."){
    	typeString = "Reverb";
    }
    else   if (newIEvent->getEventTypeString() =="Pat."){
    	typeString = "Pattern";
    }
    else {*/
    	typeString = newIEvent->getEventTypeString();
    //}
      //put this event right below the original one
    // newIEvent->setEventOrderInPalette(
    // originalEvent->getEventOrderInPalette() + 5);
      newIEvent->setEventOrderInPalette(-1);
      insertEvent(newIEvent, typeString);
      projectView->modified();
      projectView->events.push_back(newIEvent);
      refreshObjectOrder(typeString);
  }
}


void PaletteViewController::on_selection_changed(){

}


void PaletteViewController::switchBackToPreviousCursor(
  IEvent* _ifDeleteOnCursor){

  if (currentCursorPosition ==NULL ||
    _ifDeleteOnCursor == currentCursorPosition){
    currentCursorPosition = NULL;
    return;
  }
  
  
  
  string folderName=currentCursorPosition->getEventFolderName();
  string eventName = currentCursorPosition->getEventName();
  typedef Gtk::TreeModel::Children type_children; //minimise code length.
  type_children children = refTreeModel->children();
  type_children::iterator iter = children.begin();
  bool entryNotFoundYet = true;
  Gtk::TreeModel::Row row;
  
  
  if (folderName =="Folder"){ // means the selected row is a folder)
  

    while (entryNotFoundYet &&iter!= children.end()){
      row = *iter;
      if (eventName != row[columns.columnObjectName]){
        iter++;
      }
      else {
        entryNotFoundYet = false;

        
      }
    }
   
  }

  else {  //the selected row is not a folder
    while (entryNotFoundYet &&iter!= children.end()){
      row = *iter;
      if (folderName != row[columns.columnObjectName]){
        iter++;
      }
      else {
        entryNotFoundYet = false;

        
      }
    }
  
  
    children = row.children();
    iter = children.begin();
    entryNotFoundYet = true;
  
      while (entryNotFoundYet && iter!= children.end()){
      row = *iter;

      if (eventName != row[columns.columnObjectName]){
        iter++;
      }
      else {
        entryNotFoundYet = false;
      }
    }
  }


  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =
    palette.get_selection();

  refTreeSelection->select(iter);


}



int PaletteViewController::getCurrentMaxObjectNumber(string _folderName){
  typedef Gtk::TreeModel::Children type_children; //minimize code length.
  type_children children = refTreeModel->children();
  type_children::iterator iter = children.begin();
  bool entryNotFoundYet = true;
  Gtk::TreeModel::Row row;

  while (entryNotFoundYet &&iter!= children.end()){
    row = *iter;
    if (_folderName != row[columns.columnObjectName]){
      iter++;
    }
    else {
      entryNotFoundYet = false;
    }
  }

  children = row.children();
  return (children.size() - 1) * 10;
}

void PaletteViewController::refreshObjectOrder( string _folderName){
  typedef Gtk::TreeModel::Children type_children; //minimise code length.
  type_children children = refTreeModel->children();
  type_children::iterator iter = children.begin();
  bool entryNotFoundYet = true;
  Gtk::TreeModel::Row row;

  while (entryNotFoundYet &&iter!= children.end()){
    row = *iter;
    if (_folderName != row[columns.columnObjectName]){
      iter++;
    }
    else {
      entryNotFoundYet = false;
    }
  }

  children = row.children();
  for (iter = children.begin(); iter!= children.end(); iter++){
    row = *iter;
    row[columns.columnObjectOrder] = -1;
    (*(row[columns.columnEntry])).setEventOrderInPalette(-1);
  }
}



ObjectWindowObjectPackage* PaletteViewController::getObjectsLinkedList(
  string _typeString){
  ObjectWindowObjectPackage* result = NULL;

 typedef Gtk::TreeModel::Children type_children; //minimize code length.
  type_children children = refTreeModel->children();
  type_children::iterator iter = children.begin();
  bool entryNotFoundYet = true;
  Gtk::TreeModel::Row row;

  while (entryNotFoundYet &&iter!= children.end()){
    row = *iter;
    if (_typeString != row[columns.columnObjectName]){
      iter++;
    }
    else {
      entryNotFoundYet = false;
    }
  }

  children = row.children();
  iter = children.begin();
  if (children.size() == 0){
    return NULL;
  }
  else if (children.size() == 1){
    result = new ObjectWindowObjectPackage(projectView);
    row = *iter;
    result->set_label(row[columns.columnObjectName]);
    result->ievent = row[columns.columnEntry];
    result->next = NULL;
    return result;
  }
  else{
    result = new ObjectWindowObjectPackage(projectView);
    row = *iter;
    result->set_label(row[columns.columnObjectName]);
    result->ievent = row[columns.columnEntry];
    iter++;
    ObjectWindowObjectPackage* currentPackage = result;
    ObjectWindowObjectPackage* newPackage;
    for (iter; iter!= children.end(); iter++){
      newPackage = new ObjectWindowObjectPackage(projectView);
      row = *iter;
      newPackage->set_label(row[columns.columnObjectName]);
      newPackage->ievent = row[columns.columnEntry];
      newPackage->prev = currentPackage;
      currentPackage->next = newPackage;
      currentPackage= newPackage;
    }
    currentPackage->next = NULL;
    return result;
  }

}


