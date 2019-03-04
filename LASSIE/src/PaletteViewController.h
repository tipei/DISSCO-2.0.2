/*******************************************************************************
 *
 *  File Name     : PaletteViewController.h
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

#ifndef PALETTE_VIEW_CONTROLLER_H
#define PALETTE_VIEW_CONTROLLER_H

#include "LASSIE.h"

class IEvent;
class ProjectViewController;
class EnvelopeLibraryWindow;
class SharedPointers;
class ObjectWindow;
class ObjectWindowObjectPackage;


class PaletteViewController: public Gtk::VBox {
  
public:
   PaletteViewController(SharedPointers* _sharedPointers);
  ~PaletteViewController();
  
  void insertEvent(IEvent* _event);
  void insertEvent(IEvent* _event, std::string _parentName);
  void refreshObjectName(IEvent* _event);
  Glib::ustring folderSelected(); // check if any row is selected;
  IEvent* getCurrentSelectedEvent();
  void deleteKeyPressed();
  int getCurrentMaxObjectNumber(string _folderName);
  void refreshObjectOrder( string _folderName);
  ObjectWindowObjectPackage* getObjectsLinkedList(string _typeString);
  
  
  

protected:
  // Signal handlers:
  void objectActivated(
    const Gtk::TreeModel::Path& _path, Gtk::TreeViewColumn* _column);
  void showEnvelopeLibrary();
  
  //drag and drop to projectTreeView
  void on_button_drag_data_get(
    const Glib::RefPtr<Gdk::DragContext>& context,
    Gtk::SelectionData& selection_data,
    guint info,
    guint time);

  bool onRightClick(GdkEventButton* event);
  void AddToProjectTree(); //deprecated
  void deleteObject();
  void duplicateObject();

  // Tree model columns:
  class Columns : public Gtk::TreeModel::ColumnRecord{
  public:
    Columns(){
      add(columnObjectType);
      add(columnObjectName);
    /*add (columnButton)*/;
      add(columnEntry);
      add(columnObjectOrder);
    }
    Gtk::TreeModelColumn<Glib::ustring> columnObjectType;
    Gtk::TreeModelColumn<Glib::ustring> columnObjectName;

    Gtk::TreeModelColumn<IEvent*> columnEntry;
    Gtk::TreeModelColumn<int> columnObjectOrder;
  };
  int slot_comp_func(const Gtk::TreeModel::iterator& a,  const Gtk::TreeModel::iterator& b);
  Gtk::TreeSortable::SlotCompare slot_comp;

  Columns columns;
  ProjectViewController* projectView;

  //Child widgets:
  Gtk::ScrolledWindow scrolledWindow;
  Gtk::TreeView palette;
  Glib::RefPtr<Gtk::TreeStore> refTreeModel;

  Gtk::Label viewName;
  Gtk::Button envelopeLibButton;	

  Gtk::TreeModel::Row selectedRow;
  void on_selection_changed();

private:
  void on_cursor_changed();
  void switchBackToPreviousCursor(IEvent* _ifDeleteOnCursor);
  
  // these three are for setting up pop up menu
  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
  Gtk::Menu* m_pMenuPopup;
  SharedPointers* sharedPointers;
  
  IEvent* currentCursorPosition;//this thing record the original activated row. after right click is done, it switch back to this row
  

};

#endif //PALETTE_VIEW_CONTROLLER_H



