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
#ifndef OBJECT_WINDOW_H
#define OBJECT_WINDOW_H
 
#include "LASSIE.h"

//class EnvelopeLibraryEntry;
class MainWindow;
class ProjectViewController;
class IEvent;
//class EnvLibDrawingArea;





class ObjectWindowObjectPackage:public Gtk::Button {
public:
  ObjectWindowObjectPackage(ProjectViewController* _projectView);
  ~ObjectWindowObjectPackage();  
  int count();
  void clear();
  void showContent();
  ObjectWindowObjectPackage* prev;
  ObjectWindowObjectPackage* next;
  IEvent* ievent;
private:
  ProjectViewController* projectView;


};






class ObjectWindow: public Gtk::Window {

  


public:
  ObjectWindow(EventType _type, ProjectViewController* _projectView);
  ~ObjectWindow();
  
  void setActiveProject(ProjectViewController* _project);
  void print();
  void refresh();

  Glib::ustring folderSelected(); // check if any row is selected;
  //Event* getCurrentSelectedEvent();
  ProjectViewController* activeProject;  
  ObjectWindowObjectPackage* objects;
protected:
  
    virtual bool on_expose_event(GdkEventExpose* event);
  
  
 	


private:
  Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
  EventType type;
  int currentNumOfColumns;
  void sizeChange(int _newNumOfColumns);
  void numOfObjectsChange();

};
 
#endif
