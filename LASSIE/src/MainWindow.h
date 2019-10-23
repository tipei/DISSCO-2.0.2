/*******************************************************************************
 *
 *  File Name     : MainWindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "LASSIE.h"

// Forward Declaration

class ProjectViewController;
class EnvelopeLibraryWindow;
class MarkovModelLibraryWindow;

class MainWindow : public Gtk::Window {
public:
    //==========================================================================
    //============================Functions===========================
    //==========================================================================

    /*! \brief The constructor of MainWindow
     *
     ***************************************************************************/
    MainWindow();
    ~MainWindow();
    void showEnvelopeLibraryWindow();
    void showMarkovModelLibraryWindow();
    void setUnsavedTitle();
    void setSavedTitle();
    void disableNewAndOpenProject();

private:
    //==========================================================================
    //=============================Fields=============================
    //==========================================================================

    /*! \brief A vector containing projects currently opened
     *
     *  This field is left blank for the first version of LASSIE because
     *  this version of gtkmm doesn't support document view.
     *
     ***************************************************************************/
    std::vector<ProjectViewController*> projects;

    /* \brief The pointer to the projectView widget
     *
     **************************************************************************/
    ProjectViewController* projectView;

    /*! \brief The Container at the top of the main window containing
     *menu bar,tool bar, and a ProjectView
     *
     ***************************************************************************/
    Gtk::VBox mainBox;

    EnvelopeLibraryWindow* envelopeLibraryWindow;
    MarkovModelLibraryWindow* markovModelLibraryWindow;

    /*! \brief The UIManager in charge of building the layout of
     * mainBox
     *
     ***************************************************************************/
    Glib::RefPtr<Gtk::UIManager> menuRefUIManager;

    /*! \brief The ActionGroup storing the action to be taken when
     * items in menu or tool bar are clicked.
     *
     ***************************************************************************/
    Glib::RefPtr<Gtk::ActionGroup> menuRefActionGroup;

    // here is set XML UI block
    Glib::ustring ui_info;
    Glib::ustring ui_info_action;
    Glib::ustring ui_info_befor;
    Glib::ustring ui_info_after;

    Gtk::Widget* pointerToMenubarWidget;
    Gtk::Widget* pointerToToolbarWidget;

    //==========================================================================
    //============================Functions===========================
    //==========================================================================

    // These are signal handler called by clicking menu items on the
    // top of main window

    void menuFileNewProject();  // File -> New -> Project
    void menuFileNewObject();   // File -> New -> Object
    void menuFileOpen();        // File -> Open
    void menuFileOpenXML();     // File -> Open XML Project
    void menuFileSave();        // File -> Save
    void menuFileSaveAs();      // File -> Save As
    void menuFileClose();       // File -> Close
    void menuFilePrint();       // File -> Print
    void menuFileQuit();        // File -> Quit

    void menuEditUndo();         // Edit -> Undo
    void menuEditRedo();         // Edit -> Redo
    void menuEditCut();          // Edit -> Cut
    void menuEditCopy();         // Edit -> Copy
    void menuEditPaste();        // Edit -> Paste
    void menuEditSelectAll();    // Edit -> Select All
    void menuEditPreferences();  // Edit -> Preferences

    void menuHelpContents();  // Help -> Contents
    void menuHelpAbout();     // Help -> About

    /*! \brief Called by menuNewProject() to actual create a new
     * project
     *
     ***************************************************************************/
    void createNewProject();

    void changeCurrentProjectViewTo(ProjectViewController* _newProject);
    void menuProjectProperties();
    void menuProjectSynthesize();
    void menuProjectGenerateSCFile();
    void menuProjectConfigureNoteModifiers();

    void includeUi_info(Glib::ustring pathAndName, Glib::ustring ctrl);
    void menuPathAndName();

    int captureKeyStroke(Gtk::Widget*, GdkEventKey*);
};

#endif  // MAINWINDOW_H
