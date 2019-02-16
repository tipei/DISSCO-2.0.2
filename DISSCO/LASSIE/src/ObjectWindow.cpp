/*******************************************************************************
 *
 *  File Name     : ObjectWindow.h
 *  Date created  : May. 09 2011
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the Object Window of LASSIE.
 *                  The Object Window is a list of a class of objects.
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

#include "ObjectWindow.h"

//#include "EnvLibDrawingArea.h"
#include "ProjectViewController.h"
//#include "EnvelopeLibraryEntry.h"
#include "MainWindow.h"
#include "PaletteViewController.h"


#define COLUMN_WIDTH 200


ObjectWindow::ObjectWindow(
  EventType _type, ProjectViewController* _projectView){
  resize(500, COLUMN_WIDTH);
  type = _type;
  activeProject = _projectView;
  currentNumOfColumns = 1;
  string title = "";



  switch (_type){
    case 0: {
      title = "Top Events";
      objects = activeProject->getPalette()->getObjectsLinkedList("Top");
      }
      break;
    case 1: {title = "High Events";
          objects = activeProject->getPalette()->getObjectsLinkedList("High");
      }
      break;
    case 2: {title = "Mid Events";
          objects = activeProject->getPalette()->getObjectsLinkedList("Mid");
      }
      break;
    case 3: {title = "Low Events";
          objects = activeProject->getPalette()->getObjectsLinkedList("Low");
      }
      break;
    case 4: {title = "Bottom Events";
          objects = activeProject->getPalette()->getObjectsLinkedList("Bottom");
      }
      break;
    case 5: {title = "Spectra";
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Spectrum");
      }
      break;
    case 6: {title = "Envelopes";
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Envelope");
      }
      break;
    case 7: {title = "Sieves";
          objects = activeProject->getPalette()->getObjectsLinkedList("Sieve");
      }
      break;
    case 8: {title = "Spatializations";
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Spatialization");
      }
      break;
    case 9: {title = "Patterns";
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Pattern");
      }
      break;
    case 10: {title = "Reverberations";
          objects = activeProject->getPalette()->getObjectsLinkedList("Reverb");
      }
      break;
    case 12: {title = "Notes";
          objects = activeProject->getPalette()->getObjectsLinkedList("Note");
      }
      break;
    case 13: {title = "Filters";
          objects = activeProject->getPalette()->getObjectsLinkedList("Filter");
      }
      break;
    case 14: {title = "Measurements";
          objects = activeProject->getPalette()->getObjectsLinkedList("Measurement");
      }
      break;
  }





  set_title(title);
  set_border_width(3);


  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/ObjectWindow.ui");
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
    "./LASSIE/src/UI/ObjectWindow.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

  Gtk::ScrolledWindow* scrolledWindow;
  attributesRefBuilder->get_widget("scrolledwindow", scrolledWindow);

  add( *scrolledWindow);
  Gtk::Table* table;
  attributesRefBuilder->get_widget("objectTable", table);
  table->set_homogeneous(true);
  table->set_col_spacing(currentNumOfColumns, COLUMN_WIDTH);
  numOfObjectsChange();

  show_all_children();



}

ObjectWindow::~ObjectWindow(){

}





void ObjectWindow::setActiveProject(ProjectViewController* _project){
	//clean up the drawing area

  activeProject = _project;
}





void ObjectWindow::refresh(){
  //remove objects from the table
  Gtk::Table* table;
  attributesRefBuilder->get_widget("objectTable", table);
  vector<Gtk::Widget*> children = table->get_children();
  vector<Gtk::Widget*>::iterator iter = children.begin();
  for (iter; iter!= children.end(); iter++){
    table->remove(*(*iter));
  }


  if (objects!= NULL){
    objects->clear();
    objects = NULL;
  }
  switch (type){
    case 0: {

      objects = activeProject->getPalette()->getObjectsLinkedList("Top");
      }
      break;
    case 1: {
          objects = activeProject->getPalette()->getObjectsLinkedList("High");
      }
      break;
    case 2: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Mid");
      }
      break;
    case 3: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Low");
      }
      break;
    case 4: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Bottom");
      }
      break;
    case 5: {
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Spectrum");
      }
      break;
    case 6: {
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Envelope");
      }
      break;
    case 7: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Sieve");
      }
      break;
    case 8: {
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Spatialization");
      }
      break;
    case 9: {
          objects = activeProject->getPalette()->
            getObjectsLinkedList("Pattern");
      }
      break;
    case 10: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Reverb");
      }
      break;
    case 12: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Note");
      }
      break;
    case 13: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Filter");
      }
      break;
    case 14: {
          objects = activeProject->getPalette()->getObjectsLinkedList("Measurement");
      }
      break;
  }

  numOfObjectsChange();


}




bool ObjectWindow::on_expose_event(GdkEventExpose* event){
  //sizeChange();

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  //cout<<"width: "<<width<<", height: "<<height<<endl;
  int newNumOfColumns = width / COLUMN_WIDTH;
  //cout<<"new: "<<newNumOfColumns<<", current: "<<currentNumOfColumn
  if (newNumOfColumns != currentNumOfColumns){
    sizeChange(newNumOfColumns);
  }



  return true;
}

void ObjectWindow::sizeChange(int _newNumOfColumns){
  currentNumOfColumns = _newNumOfColumns;

  //cout<<"New Num of Columns"<< _newNumOfColumns<<endl;

  Gtk::Table* table;
  attributesRefBuilder->get_widget("objectTable", table);

  vector<Gtk::Widget*> children = table->get_children();
  vector<Gtk::Widget*>::iterator iter = children.begin();
  for (iter; iter!= children.end(); iter++){
    table->remove(*(*iter));
  }

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  currentNumOfColumns = width / COLUMN_WIDTH ;
  if (currentNumOfColumns ==0){
    currentNumOfColumns =1;
  }

  if (objects==NULL){
    return;
  }
  int rows = objects->count() / currentNumOfColumns + 1;

  //table->set_col_spacing(currentNumOfColumns, COLUMN_WIDTH);

  table->resize( rows, currentNumOfColumns);

  ObjectWindowObjectPackage* currentPackage = objects;
  int rowIndex = 0;
  int columnIndex = 0;

  while (currentPackage != NULL){

    table->attach(
      *currentPackage,columnIndex, columnIndex+1, rowIndex, rowIndex+1);

    columnIndex ++;
    currentPackage= currentPackage->next;
    if (columnIndex ==currentNumOfColumns){
      columnIndex = 0;
      rowIndex ++;
    }
  }


  show_all_children();



}


ObjectWindowObjectPackage::ObjectWindowObjectPackage(
  ProjectViewController* _projectView){
  projectView = _projectView;
  this->signal_pressed().connect(
    sigc::mem_fun(
      *this,&ObjectWindowObjectPackage::showContent) );
}


void ObjectWindowObjectPackage::showContent(){
  projectView->showAttributes(ievent);

}
ObjectWindowObjectPackage::~ObjectWindowObjectPackage(){
}
int ObjectWindowObjectPackage::count(){
  if (next ==NULL){
    return 1;
  }
  else {
    return 1 + next->count();
  }
}


void ObjectWindow::print(){
  cout<<get_title()<<":"<<endl;
  ObjectWindowObjectPackage* object = objects;
  while (object!= NULL){
    cout<<object->ievent->getEventName()<<endl;
    object= object->next;
  }

}


void ObjectWindow::numOfObjectsChange(){
  Gtk::Table* table;
  attributesRefBuilder->get_widget("objectTable", table);
  vector<Gtk::Widget*> children = table->get_children();
  vector<Gtk::Widget*>::iterator iter = children.begin();
  for (iter; iter!= children.end(); iter++){
    table->remove(*(*iter));
  }

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  currentNumOfColumns = width / COLUMN_WIDTH ;
  if (currentNumOfColumns ==0){
    currentNumOfColumns =1;
  }

  if (objects == NULL){
    return;
  }
  int rows = objects->count() / currentNumOfColumns + 1;

  //table->set_col_spacing(currentNumOfColumns, COLUMN_WIDTH);
  //table->set_row_spacing(rows, 25);
  table->resize( rows, currentNumOfColumns);

  ObjectWindowObjectPackage* currentPackage = objects;
  int rowIndex = 0;
  int columnIndex = 0;

  while (currentPackage != NULL){

    table->attach(*currentPackage,columnIndex, columnIndex+1, rowIndex, rowIndex+1);

    columnIndex ++;
    currentPackage= currentPackage->next;
    if (columnIndex ==currentNumOfColumns){
      columnIndex = 0;
      rowIndex ++;
    }
  }


  show_all_children();


}


void ObjectWindowObjectPackage::clear(){
  if (next!= NULL){
    next->clear();
  }
  delete this;
}
