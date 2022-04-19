/*******************************************************************************
 *
 *  File Name     : ProjectViewController.h
 *  Date created  : Jan. 27 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains the class "Project" of LASSIE. Each
 *                  Project object holds the information of the project such as
 *                  name, structure, etc.
 *
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

#ifndef PROJECT_VIEW_CONTROLLER_H
#define PROJECT_VIEW_CONTROLLER_H

#include "LASSIE.h"

#include "IEvent.h"

//Forward declarations

class ObjectWindow;
class EventAttributesViewController;
class PaletteViewController;
class EnvelopeLibraryEntry;
class MainWindow;
class SharedPointers;

class Envelope;
class ProjectViewController;

class CustomNoteModifierHBox:public Gtk::HBox{
public:
  CustomNoteModifierHBox(ProjectViewController* _projectView);
  CustomNoteModifierHBox(
                    ProjectViewController* _projectView,
                    std::string _string);
  ~CustomNoteModifierHBox();
  std::string getText();

private:
  Gtk::Label label;
  Gtk::Entry entry;
  Gtk::Button removeButton;
  ProjectViewController* projectView;
  void removeButtonClicked();

};


class ProjectViewController: public Gtk::VBox  {
public:

  //==========================================================================
  //============================Functions=====================================
  //==========================================================================

  /*! \brief The constructor of ProjectViewController
   *
   ***************************************************************************/
  ProjectViewController(MainWindow* _mainWindow);
  ProjectViewController(std::string _pathAndName, MainWindow* _mainWindow);
  ProjectViewController( MainWindow* _mainWindow, std::string _pathAndName,
                          	std::string _projectTitle);
  //this one is for the old format
  //ProjectViewController(std::string _pathAndName, MainWindow* _mainWindow,
  //std::string _datPathAndName, std::string _libPathAndName);

  /*! \brief The destructor of ProjectViewController
   *
   ***************************************************************************/
  ~ProjectViewController();

  // functions for modifying the envelop list
  EnvelopeLibraryEntry* createNewEnvelope();
  EnvelopeLibraryEntry* duplicateEnvelope(EnvelopeLibraryEntry* _originalEnvelope);

  // functions for modifying the Markov Model list
  int createNewMarkovModel();
  int duplicateMarkovModel(int idx);
  int removeMarkovModel(int idx);

  void initializeModifiers();

  void showContents();
  void hideContents();
  void insertObject();
  void cleanUpContents();
  std::string getPathAndName();
  void showAttributes(IEvent* _event);
  void refreshObjectNameInPalette(IEvent* _event);
  void setProperties();
  void save();
  void saveAs(std::string _newPathAndName);
  bool getSaved();
  void modified();
  void deleteKeyPressed(Gtk::Widget* _focus);
  bool getEmptyProject();
  void nKeyPressed(Gtk::Widget* _focus);
  void showAttributesView(bool _show);

  void configureNoteModifiers();
  void removeCustomNoteModifier(CustomNoteModifierHBox* _hbox);
  bool checkNameExists(string _name, EventType _type);
  void deleteObject(IEvent* _toDelete);
  void setSeed(string _seed){seed = _seed;}
  void removeExtraEntry(string _fileName);

  std::map<std::string, bool> getDefaultNoteModifiers();
  std::vector<std::string> getCustomNoteModifiers();

  IEvent* findIEvent(EventType _type, std::string _eventName);
  IEvent* getTop(){return top;}

  PaletteViewController* getPalette();
  EnvelopeLibraryEntry* getEnvelopeLibraryEntries();
  vector<MarkovModel<float>>& getMarkovModels();

  IEvent* getEventByTypeAndName( EventType type, std::string _name);
  std::string searchPossibleParents(string _fileName);

  ///////////////////////////////////drag and drop targets
  std::list<Gtk::TargetEntry> listTargets;

  //////////////////////////////////////////////////////////////////
  //so that palette can push new event back to ievent
  std::vector <IEvent*> events;

  //to be delete (and also the files) when "Save" is clicked
  std::vector <IEvent*> deletedEvents;

  ObjectWindow* topWindow;
  ObjectWindow* highWindow;
  ObjectWindow* midWindow;
  ObjectWindow* lowWindow;
  ObjectWindow* bottomWindow;
  ObjectWindow* spectrumWindow;
  ObjectWindow* envWindow;
  ObjectWindow* sivWindow;
  ObjectWindow* spaWindow;
  ObjectWindow* patWindow;
  ObjectWindow* revWindow;
  ObjectWindow* filWindow;
  ObjectWindow* noteWindow;
  ObjectWindow* meaWindow;


private:

  //==========================================================================
  //=============================Fields=======================================
  //==========================================================================

  /*! \brief title of the project
   *
   ***************************************************************************/
  std::string pathAndName;


  /*! \brief pointer to eventAttributesView
   *
   ***************************************************************************/
  EventAttributesViewController* eventAttributesView;

  /*! \brief pointer to paletteView
   *
   ***************************************************************************/
  PaletteViewController* paletteView;

  /*! \brief pointer to leftTwoPlusAttributes
   *
   *          leftTwoPlusAttributes is a Paned widget containing another
   *          Paned widget (paletteAndTree) on the left and the attributes
   *          frame on it's right.
   *
   ***************************************************************************/
  Gtk::HPaned leftTwoPlusAttributes;

  IEvent* top;
  bool modifiedButNotSaved;

  std::vector< MarkovModel<float> > markovModels;

  //this thing is a double-linked list
  EnvelopeLibraryEntry* envelopeLibraryEntries;
  Gtk::Dialog* newObjectDialog;
  Gtk::Dialog* noteModifiersConfigurationDialog;
  Gtk::VBox* noteModifiersConfigurationCustomVBox;
  void newObjectButtonClicked();

  Gtk::Dialog* projectPropertiesDialog;

  void projectPropertiesDialogButtonClicked();
  void projectPropertiesDialogFunctionButtonClicked();
  void ConfigureNoteModifiersAddButtonClicked();
  void saveEnvelopeLibrary();
  void saveNoteModifierConfiguration();
  void refreshProjectDotDat();
  EnvelopeLibraryEntry* convertToLASSIEEnvLibEntry(
    Envelope* _envelope,
    int index);

  std::string topEvent;
  std::string projectTitle;
  std::string fileFlag;
  std::string duration;
  std::string numOfChannels;
  std::string sampleRate;
  std::string sampleSize;
  std::string numOfThreads;
  std::string datPathAndName;
  std::string libPathAndName;
  std::string seed;
  std::string measure;

  bool score;
  bool synthesis;
  bool outputParticel;
  bool emptyProject;
  std::vector<std::string> customNoteModifiers;
  std::map<std::string, bool> defaultNoteModifiers;
  std::vector<CustomNoteModifierHBox*> customNotModifierHBoxes;
  SharedPointers* sharedPointers;
  void clearDeletedEvents();

  DOMDocument *xmlDocument;

};

#endif //PROJECT_VIEW_CONTROLLER_H
