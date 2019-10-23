/*******************************************************************************
 *
 *  File Name     : MarkovModelLibraryWindow.h
 *  Date created  : April 15 2018
 *  Authors       : Fanbo Xiang(xiangfanbo@gmail.com), Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the Envelope Library Window of LASSIE.
 *                  The envelope library lists all the pre-made envelopes.
 *==============================================================================
 *
 *  This file is part of LASSIE.
 *  2018 Fanbo Xiang, Sever Tipei
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

#ifndef MARKOVMODELLIBRARYWINDOW_H
#define MARKOVMODELLIBRARYWINDOW_H

#include <sstream>

#include "../../LASS/src/MarkovModel.h"
#include "LASSIE.h"
#include "ProjectViewController.h"

#define ENTRY_WIDTH 100
#define ENTRY_HEIGHT 50
#define LABEL_HEIGHT 20
#define LABEL_WIDTH 50

class MarkovModelLibraryWindow : public Gtk::Window {
public:
    class ModelColumns : public Gtk::TreeModelColumnRecord {
    public:
        ModelColumns() { add(m_col_number); }
        Gtk::TreeModelColumn<int> m_col_number;
    } m_columns;

    MarkovModelLibraryWindow();
    ~MarkovModelLibraryWindow();

    void setActiveProject(ProjectViewController* project);
    void createNewModel();
    void duplicateModel();

    void update(int selection);

protected:
    /* The project */
    ProjectViewController* activeProject;

    Gtk::HPaned m_Paned;
    Gtk::VBox m_ListBox;
    Gtk::ScrolledWindow m_ListScroll;

    Gtk::VBox m_detailBox;
    Gtk::VBox m_detailBox1;
    Gtk::Entry m_sizeEntry;
    Gtk::Button m_sizeButton;
    Gtk::ScrolledWindow m_distScroll;
    Gtk::ScrolledWindow m_valueScroll;
    Gtk::ScrolledWindow m_matrixScroll;

    Glib::RefPtr<Gtk::ListStore> m_refListStore;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Gtk::Menu* m_pMenuPopup;
    Gtk::TreeView m_TreeView;

    Glib::RefPtr<Gtk::Builder> m_attributesRefBuilder;
    std::vector<Gtk::Entry*> m_distEntries;
    std::vector<Gtk::Entry*> m_valueEntries;
    std::vector<Gtk::Entry*> m_matrixEntries;

    std::string to_string() const;

    void onSetSize();

    virtual void on_hide() override;

private:
    void onSelectionChanged();
    void buildTable();
    bool onEntryChange(GdkEventFocus* event);
    bool onRightClick(GdkEventButton* event);
    int size = 0;
    int currentSelection = -1;
};

template <typename T>
T read_entry_as(Gtk::Entry& entry) {
    stringstream ss(entry.get_text().raw());
    T t;
    ss >> t;
    return t;
}
#endif /* MARKOVMODELLIBRARYWINDOW_H */
