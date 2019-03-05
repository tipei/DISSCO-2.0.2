/*******************************************************************************
 *
 *  File Name     : EnvelopeLibraryWindow.cpp
 *  Date created  : May. 15 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the Envelope Library Window of LASSIE. 
 *                  The envelope library lists all the pre-made envelopes.
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
 
#include "EnvelopeLibraryWindow.h"

#include "EnvLibDrawingArea.h"
#include "ProjectViewController.h"
#include "EnvelopeLibraryEntry.h"
#include "MainWindow.h"

EnvelopeLibraryWindow::EnvelopeLibraryWindow(){
  set_title("Envelope Library");
    set_border_width(3);
    activeEnvelope = NULL;

  
  
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/EnvelopeLibraryWindow.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
   
   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file(
    "./LASSIE/src/UI/EnvelopeLibraryWindow.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   
   #endif
  
  /* try to enable save while envelope library window is active, but fail :(
    will do this later 
   menuRefActionGroup = Gtk::ActionGroup::create();  

  menuRefActionGroup->add(
    Gtk::Action::create("FileSave",Gtk::Stock::SAVE),
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::fileSave));
  */
   
  Gtk::VBox* box;
   
  attributesRefBuilder->get_widget("vbox1", box);
   
	Gtk::Alignment* a;
  attributesRefBuilder->get_widget("DrawingAreaAlignment", a);
  drawingArea = new EnvLibDrawingArea(this);
	a->add(*drawingArea);


  add(*box);
  drawingArea->show();
  box->show_all();
  

    attributesRefBuilder->get_widget("EnvelopeListAlignment", a);
  
 
  a->add(scrolledWindow);
   
  
  // Add the TreeView, inside a ScrolledWindow
  scrolledWindow.add(envelopeLibrary);

  // Only show the scrollbars when they are necessary:
  scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);


  attributesRefBuilder->get_widget("CoordinateLabel", mouseCoordinate);



  // Create the Tree model:
  refTreeModel = Gtk::TreeStore::create(columns);
  envelopeLibrary.set_model(refTreeModel);

  // All the items to be reordered with drag-and-drop:
  //palette.set_reorderable();



  // Add the TreeView's view columns:
  envelopeLibrary.append_column(
    "Number of Library", columns.columnObjectNumber);
  //palette.append_column("Name", columns.columnObjectName);
  // Connect signal:
  envelopeLibrary.signal_row_activated().connect(
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::objectActivated) );
  envelopeLibrary.signal_cursor_changed().connect(
    sigc::mem_fun(*this,&EnvelopeLibraryWindow::on_cursor_changed) );
    
    
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("XValueEntry", entry);
  entry->signal_changed().connect (
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::valueEntriesChanged));
  attributesRefBuilder->get_widget("YValueEntry", entry);
  entry->signal_changed().connect (
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::valueEntriesChanged));    
    

  //////////////////////////test for drag and drop///////////////////////////

  //this section is from palette view, has to modify variables name to use it


  //Make m_Button_Drag a DnD drag source:
  //palette.drag_source_set(projectView->listTargets);
   //viewName.drag_source_set(projectView->listTargets);
   //palette.enable_model_drag_source(
   //projectView->listTargets,Gdk::MODIFIER_MASK,Gdk::ACTION_COPY);


   //Connect signals: bind current event to the signal
   //palette.signal_drag_data_get().connect(
     //sigc::mem_fun(*this, &PaletteViewController::on_button_drag_data_get));

   //viewName.signal_drag_data_get().connect(
     //sigc::mem_fun(*this, &PaletteViewController::on_button_drag_data_get));

  //////////////////////end test ////////////////////////////////////////

  /////////////////////set up popup menu ///////////////////////////////

  m_pMenuPopup = 0;

  //    the signal handler i wished to put was overriden by the object default
  //signal handler, so to avoid this, we have to add a paramater to the connect
  //method (false)

  envelopeLibrary.signal_button_press_event().connect(
    sigc::mem_fun(*this,&EnvelopeLibraryWindow::onRightClick), false);



  //Fill menu:

  m_refActionGroup = Gtk::ActionGroup::create();

  //File|New sub menu:
  //These menu actions would normally already exist for a main menu, because a
  //context menu should not normally contain menu items that are only available
  //via a context menu.
  m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));

   m_refActionGroup->add(
     Gtk::Action::create("ContextAdd", "Create New Envelope"),
     sigc::mem_fun(*this, &EnvelopeLibraryWindow::createNewEnvelope));

  m_refActionGroup->add(
    Gtk::Action::create("ContextDuplicate", "Duplicate Envelope"),
    sigc::mem_fun(*this, &EnvelopeLibraryWindow::duplicateEnvelope));

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
    "    <menuitem action='ContextAdd'/>"
    "    <menuitem action='ContextDuplicate'/>"
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
  
  

  Gtk::Main::signal_key_snooper().connect(sigc::mem_fun(*this,&EnvelopeLibraryWindow::captureKeyStroke));
  
   
   show_all();
  
}

EnvelopeLibraryWindow::~EnvelopeLibraryWindow(){

}


  


bool EnvelopeLibraryWindow::onRightClick(GdkEventButton* event){

  // test if right click
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ){
    if(m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);

    return true; // It has been handled.
  }else{
    return false;
  }
}

void EnvelopeLibraryWindow::createNewEnvelope(){
  EnvelopeLibraryEntry* newEnvelope = activeProject->createNewEnvelope();
  
  activeProject->modified();
  Gtk::TreeModel::Row childrow= *(refTreeModel->append());

  childrow[columns.columnObjectNumber] = newEnvelope->getNumberString();

  childrow[columns.columnEntry] =newEnvelope;

}



void EnvelopeLibraryWindow::refreshEnvelopeList(){
	//this method is very inefficient, but for the first version we'll do this for
	//now.
	
	//remove the current refTreeModel and make a new one
	refTreeModel = Gtk::TreeStore::create(columns);
  envelopeLibrary.set_model(refTreeModel);

}





void EnvelopeLibraryWindow::objectActivated(
  const Gtk::TreeModel::Path& _path, Gtk::TreeViewColumn* /* _column */){

  Gtk::TreeModel::iterator iter = refTreeModel->get_iter(_path);

  if(iter){
    Gtk::TreeModel::Row row = *iter;
    selectedRow = *iter;

  }
}



void EnvelopeLibraryWindow::on_cursor_changed(){
//TODO
  
  Gtk::TreeModel::Children::iterator iter = envelopeLibrary.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  
  activeEnvelope = row[columns.columnEntry];
  drawingArea->resetFields();
  drawingArea->showGraph(row[columns.columnEntry]);
}


void EnvelopeLibraryWindow::setActiveProject(ProjectViewController* _project){
	//clean up the drawing area
	drawingArea->clearGraph();
  
  
  activeProject = _project;
  refreshEnvelopeList();
  
  EnvelopeLibraryEntry* thisEntry = activeProject->getEnvelopeLibraryEntries();
  while (thisEntry != NULL){
    Gtk::TreeModel::Row childrow= *(refTreeModel->append());
    childrow[columns.columnObjectNumber] = thisEntry->getNumberString();
    childrow[columns.columnEntry] =thisEntry;
    thisEntry = thisEntry->next;  
  }
  activeEnvelope = NULL;

}

EnvelopeLibraryEntry* EnvelopeLibraryWindow::getActiveEnvelope(){
  
  return activeEnvelope;
}


int EnvelopeLibraryWindow::captureKeyStroke(Gtk::Widget* _widget,
                                            GdkEventKey* _gdkEventKey){
  
  if (_gdkEventKey->type ==8 && 
      _gdkEventKey->keyval == 115 && 
      _gdkEventKey->state ==20){ //ctrl-s pressed
    activeProject->save();
  }
  
  return 0;
}


void EnvelopeLibraryWindow::valueEntriesChanged(){
  Gtk::Entry* x;
  Gtk::Entry* y;
  attributesRefBuilder->get_widget("XValueEntry", x);
  attributesRefBuilder->get_widget("YValueEntry", y);

  drawingArea->setActiveNodeCoordinate(x->get_text(), y->get_text()); 
}


void EnvelopeLibraryWindow::setEntries(string _x, string _y){
  Gtk::Entry* x;
  Gtk::Entry* y;
  attributesRefBuilder->get_widget("XValueEntry", x);
  attributesRefBuilder->get_widget("YValueEntry", y);
  
  x->set_text(_x);
  y->set_text(_y);

}

void EnvelopeLibraryWindow::duplicateEnvelope(){

  Gtk::TreeModel::Children::iterator iter = 
    envelopeLibrary.get_selection()->get_selected();
  if (iter == 0){
    cout<<" no envelope selected"<<endl;
    return;
  }
  
  Gtk::TreeModel::Row row = *iter;
  
  
	EnvelopeLibraryEntry* originalEnvelope = row[columns.columnEntry];
	
	EnvelopeLibraryEntry* newEnvelope = 
	  activeProject->duplicateEnvelope(originalEnvelope);
  activeProject->modified();
  Gtk::TreeModel::Row childrow= *(refTreeModel->append());

  childrow[columns.columnObjectNumber] = newEnvelope->getNumberString();

  childrow[columns.columnEntry] =newEnvelope;
}
