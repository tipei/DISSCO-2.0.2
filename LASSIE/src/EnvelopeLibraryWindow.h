/*******************************************************************************
 *
 *  File Name     : EnvelopeLibraryWindow.h
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
#ifndef ENVELOPE_LIBRARY_WINDOW_H
#define ENVELOPE_LIBRARY_WINDOW_H

#include "LASSIE.h"

class EnvelopeLibraryEntry;
class MainWindow;
class ProjectViewController;
class EnvLibDrawingArea;
class EnvelopeLibraryWindow : public Gtk::Window {
public:
    EnvelopeLibraryWindow();
    ~EnvelopeLibraryWindow();

    void setActiveProject(ProjectViewController* _project);
    void createNewEnvelope();
    void duplicateEnvelope();
    void setEntries(string _x, string _y);
    EnvelopeLibraryEntry* getActiveEnvelope();

    Glib::ustring folderSelected();  // check if any row is selected;
    Gtk::Label* mouseCoordinate;
    Gtk::Label* ScaleBoundary;
    ProjectViewController* activeProject;

protected:
    // Signal handlers:
    void objectActivated(const Gtk::TreeModel::Path& _path, Gtk::TreeViewColumn* _column);
    bool onRightClick(GdkEventButton* event);
    Glib::RefPtr<Gtk::ActionGroup> menuRefActionGroup;
    void showEnvelopeLibrary();

    // drag and drop to projectTreeView
    // void on_button_drag_data_get(
    // const Glib::RefPtr<Gdk::DragContext>& context,
    // Gtk::SelectionData& selection_data,
    // guint info,
    // guint time);

    // bool onRightClick(GdkEventButton* event);
    // void AddToProjectTree();

    // Tree model columns:
    class Columns : public Gtk::TreeModel::ColumnRecord {
    public:
        Columns() {
            add(columnObjectNumber);
            add(columnEntry);
        }
        Gtk::TreeModelColumn<Glib::ustring> columnObjectNumber;
        Gtk::TreeModelColumn<EnvelopeLibraryEntry*> columnEntry;
    };

    Columns columns;
    // Child widgets:
    Gtk::ScrolledWindow scrolledWindow;
    Gtk::TreeView envelopeLibrary;
    Glib::RefPtr<Gtk::TreeStore> refTreeModel;
    Gtk::TreeModel::Row selectedRow;

private:
    void on_cursor_changed();
    void refreshEnvelopeList();
    void valueEntriesChanged();

    // these three are for setting up pop up menu
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    Gtk::Menu* m_pMenuPopup;

    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    EnvLibDrawingArea* drawingArea;
    EnvelopeLibraryEntry* activeEnvelope;
    void fileSave();
    int captureKeyStroke(Gtk::Widget* _widget, GdkEventKey* _gdkEventKey);
};

#endif
