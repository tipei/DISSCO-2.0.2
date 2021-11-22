/*******************************************************************************
 *
 *  File Name     : EventAttributesViewController.h
 *  Date created  : Jan. 29 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file implement the class of showing event attributes
 *                  In LASSIE
 *
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




#ifndef EVENT_ATTRIBUTES_VIEW_CONTROLLER_H
#define EVENT_ATTRIBUTES_VIEW_CONTROLLER_H

#include "LASSIE.h"

#include "IEvent.h"

// forward declaration




typedef enum {
  maxChildDurFunButton,
  unitsPerSecondFunButton,
  unitsPerBarFunButton,
  numOfChildFunButton1,
  numOfChildFunButton2,
  numOfChildFunButton3,
  childEventDefStartTimeFunButton,
  childEventDefChildTypeFunButton,
  childEventDefDurationFunButton,
  childEventDefAttackSieveFunButton,
  childEventDefDurationSieveFunButton,
  spectrumDeviationFunButton,
  spectrumGenerateFunButton

}FunctionButton;

class Tempo;
class ProjectViewController;
class SharedPointers;
class EventAttributesViewController;

//implement as doubly linked list for convenience
class BottomEventModifierAlignment: public Gtk::Alignment{
public:
  BottomEventModifierAlignment* prev;
  BottomEventModifierAlignment* next;
  BottomEventModifierAlignment(
    EventBottomModifier* _modifier,
    EventAttributesViewController* _attributesView);
  ~BottomEventModifierAlignment();
  EventBottomModifier* modifier;
  void saveToEvent();
protected:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_id); add(m_col_name); }

    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };

  class ModelColumns2 : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns2()
    { add(m_col_type); add(m_col_name); }

    Gtk::TreeModelColumn<ModifierType> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };




  void on_applyHow_combo_changed();
  void on_type_combo_changed();
  void removeModifierButtonClicked();

  void probablityEnvelopeButtonClicked();
  void ampValueEnvelopeButtonClicked();
  void rateValueEnvelopeButtonClicked();
  void widthEnvelopeButtonClicked();
  void modified();
  bool initialEnter;
  ModelColumns applyHowColumns;
  ModelColumns2 typeColumns;
  EventAttributesViewController* attributesView;

  Glib::RefPtr<Gtk::Builder> attributesRefBuilder;//in order to build alignment
  Glib::RefPtr<Gtk::ListStore> applyHowTreeModel;
  Glib::RefPtr<Gtk::ListStore> typeTreeModel;


};



class SoundPartialHBox:public Gtk::HBox{
public:
  EventAttributesViewController* attributes;
  SoundPartialHBox* prev;
  SoundPartialHBox* next;

  SoundPartialHBox(SpectrumPartial* _partial,EventAttributesViewController* _attribute);

  Gtk::Button removeButton;

  Gtk::Button functionButton;
  Gtk::Entry envelopeEntry;
  Gtk::Label label;
  SpectrumPartial* partial;


  ~SoundPartialHBox();

  void clear();
  void removeButtonClicked();
  void functionButtonClicked();
  void setPartialNumber(int _number);
  void saveString();
  void modified();


};





/*! \brief The class representing the view showing event attributes in LASSIE
 *
 ******************************************************************************/
class EventAttributesViewController:public Gtk::Frame {
public:

  /*! \brief Constructor of EventAttributesViewController
   *
   *  @param _projectView the pointer to its parent ProjectViewController object
   ****************************************************************************/
  EventAttributesViewController(SharedPointers* _sharedPointers);



  /*! \brief destructor of EventAttributesViewController
   *
   ****************************************************************************/
  ~EventAttributesViewController();



  /*! \brief shows the attributes of the event in LASSIE
   *
   *  @param _event The event to be shown in LASSIE
   ****************************************************************************/
  void showAttributesOfEvent(IEvent* _event);



  /*! \brief Called when user click the "Density" button in the view. This
   *         disabled the 2nd and the 3rd entry of numOfChild
   *
   ****************************************************************************/
  void densityButtonClicked();



  /*! \brief Called when user click the "Fixed" button in the view. This
   *         enable the 2nd and the 3rd entry of numOfChild
   *
   ****************************************************************************/
  void fixedButtonClicked();


  /*! \brief Called when user click the "By Layer" button in the view. This
   *         disabled all entries and enable weight of layerboxes
   *
   ****************************************************************************/
  void byLayerButtonClicked();


  /*! \brief Called by showAttributesOfEvent. It saves the attributes of the
   *         currently shown event to memory (not to the disk until user save
   *         the project). also called by projectView when save button is hit.
   *         This make sure the currently shown event is saved to the disk.
   *
   ****************************************************************************/
//  void makeSuperColliderCodeButtonClicked();

  void saveCurrentShownEventData();
  void refresh();

  void continuumButtonClicked();
  void sweepButtonClicked();
  void discreteButtonClicked();
  void deleteKeyPressed(Gtk::Widget* _focus);

	IEvent* getCurrentEvent();

  bool checkAttackSieve();
  bool checkDurationSieve();
  void removeModifierAlignment(BottomEventModifierAlignment* _alignTempoPrefixment);
  void removeSoundPartial(SoundPartialHBox* _remove);

  //signal handlers for insertfunction buttons
  void maxChildDurFunButtonClicked();
  void unitsPerSecondFunButtonClicked();
  void unitsPerBarFunButtonClicked();
  void numOfChildFunButton1Clicked();
  void numOfChildFunButton2Clicked();
  void numOfChildFunButton3Clicked();
  void childEventDefStartTimeFunButtonClicked();
  void childEventDefChildTypeFunButtonClicked();
  void childEventDefDurationFunButtonClicked();
  void childEventAttackSieveButtonClicked();
  void childEventDurationSieveButtonClicked();

  void attributesStandardRevButtonClicked();
  void attributesStandardFilButtonClicked();
  void attributesStandardSpaButtonClicked();



  void BSLoudnessButtonClicked();
  void BSSpatializationButtonClicked();
  void BSReverbButtonClicked();
  void BSFilterButtonClicked();
  void BSWellTemperedButtonClicked();
  void BSFunFreqButton1Clicked();
  void BSFunFreqButton2Clicked();
  void BSContinuumButtonClicked();
  void addPartialButtonClicked();
  void deviationFunButtonClicked();
  void generatespectrumFunButtonClicked();
  void modified();
  IEvent* getCurrentlyShownEvent();



  void insertFunctionString (FunctionButton _button);
  void buildNoteModifiersList();


private:


    SharedPointers* sharedPointers;


  /*! \brief Called by ShowAttributesOfEvent. It shows the data of the current
   *         event on screen.
   *
   ****************************************************************************/
  void showCurrentEventData();

  void addNewLayerButtonClicked();
  void addModifierButtonClicked();
  void on_label_drop_drag_data_received(
    const Glib::RefPtr<Gdk::DragContext>& context,
    int x ,
    int y,
    const Gtk::SelectionData& selection_data, guint info, guint time);

  void switchToSoundAttributes();
  void switchToEnvAttributes();
  void switchToSivAttributes();
  void switchToSpaAttributes();
  void switchToPatAttributes();
  void switchToRevAttributes();
  void switchToNoteAttributes();
  void switchToFilAttributes();
  void switchToMeaAttributes();

  Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
  Glib::RefPtr<Gtk::Builder> attributesRefBuilderSound;
  Gtk::ScrolledWindow scrolledWindow; // store the view in this in case oversize

  Gtk::Frame* frame; // another container
  IEvent* currentlyShownEvent;
  ProjectViewController* projectView;
  Gtk::VBox* layerBox;
  Gtk::Button* button;
  SoundPartialHBox* soundPartialHboxes;

  class LayerBox: public Gtk::VBox{
  public:
    LayerBox(EventAttributesViewController* _attributesView,
             ProjectViewController* _projectView,
             EventLayer* _childrenInThisLayer,
             bool _flagShowDiscreteColumns
             );

    ~LayerBox();
    bool onRightClick(GdkEventButton* event);
    int refreshChildTypeIndex(int _index);
    void deleteObject();

    // these three are for setting up pop up menu
    Glib::RefPtr<Gtk::UIManager> m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
    Gtk::Menu* m_pMenuPopup;




    //private:
  	Gtk::Label boxLabel;
    //Signal handlers:
    class Columns : public Gtk::TreeModel::ColumnRecord{
    public:

      Columns(){
        add(columnObjectChildTypeIndex);
        add(columnObjectType);
        add(columnObjectName);
        add(columnEntry);
        add(columnObjectWeight);
        add(columnObjectAttackEnv);
        add(columnObjectAttackEnvScale);
        add(columnObjectDurationEnv);
        add(columnObjectDurationEnvScale);

      }

      Gtk::TreeModelColumn<int> columnObjectChildTypeIndex;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectType;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectName;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectWeight;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectAttackEnv;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectAttackEnvScale;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectDurationEnv;
      Gtk::TreeModelColumn<Glib::ustring> columnObjectDurationEnvScale;
      Gtk::TreeModelColumn<EventDiscretePackage*> columnEntry;
    };

    Columns m_Columns;

    //Child widgets:

    ProjectViewController* projectView;
    EventAttributesViewController* attributesView;
    Gtk::ScrolledWindow m_ScrolledWindow2;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
    EventLayer* layerInEvent;

    //------------------ for editable cell

    Gtk::CellRendererText weightText;
    Gtk::TreeView::Column weightColumn;

    Gtk::CellRendererText attackEnvText;
    Gtk::TreeView::Column attackEnvColumn;

    Gtk::CellRendererText attackEnvScaleText;
    Gtk::TreeView::Column attackEnvScaleColumn;

    Gtk::CellRendererText durationEnvText;
    Gtk::TreeView::Column durationEnvColumn;

    Gtk::CellRendererText durationEnvScaleText;
    Gtk::TreeView::Column durationEnvScaleColumn;




    //--------------------------------------
    Gtk::VBox innerVBox;
    Gtk::HBox weightHBox;
    Gtk::Label weightLabel;
    Gtk::Button weightFunctionButton;
    Gtk::Button deleteLayerButton;
    Gtk::Entry weightEntry;


    void on_label_drop_drag_data_received(
      const Glib::RefPtr<Gdk::DragContext>& context,
      int x,
      int y,
      const Gtk::SelectionData& selection_data,
      guint info,
      guint time
    );
    void byLayerWeightButtonClicked();
    void deleteLayerButtonClicked();

    void renderWeight(
      Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);

    void renderAttackEnv(
      Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);

    void renderAttackEnvScale(
      Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);


    void renderDurationEnv(
      Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);


    void renderDurationEnvScale(
      Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);



    void saveWeight(
      const Glib::ustring& path_string, const Glib::ustring& new_text);

    void saveAttackEnv(
      const Glib::ustring& path_string, const Glib::ustring& new_text);

    void saveAttackEnvScale(
      const Glib::ustring& path_string, const Glib::ustring& new_text);

    void saveDurationEnv(
      const Glib::ustring& path_string, const Glib::ustring& new_text);

    void saveDurationEnvScale(
      const Glib::ustring& path_string, const Glib::ustring& new_text);

  };


  //Columns for 2 tempo-related comboboxes
  class ModelColumnsTempoPrefix : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumnsTempoPrefix()
    { add(m_col_type); add(m_col_name); }

    Gtk::TreeModelColumn<TempoPrefix> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };

  class ModelColumnsTempoNoteValue : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumnsTempoNoteValue()
    { add(m_col_type); add(m_col_name); }

    Gtk::TreeModelColumn<TempoNoteValue> m_col_type;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };


  ModelColumnsTempoPrefix tempoPrefixColumns;
  ModelColumnsTempoNoteValue tempoNoteValueColumns;
  Glib::RefPtr<Gtk::ListStore> tempoPrefixTreeModel;
  Glib::RefPtr<Gtk::ListStore> tempoNoteValueTreeModel;




  void tempoAsNoteValueButtonClicked();
  void tempoAsFractionButtonClicked();



  void freqFundamentalButtonClicked();
  void freqContinuumButtonClicked();
  void freqWellTemperedButtonClicked();
  void envelopeFunButtonClicked();
  void sieveFunButtonClicked();
  void patternFunButtonClicked();
  void reverbFunButtonClicked();
  void filterFunButtonClicked();
  void measureFunButtonClicked();
  void spatializationFunButtonClicked();
  bool entryChangedByShowCurrentEvent;
  bool deleteLayer(LayerBox* _deleteBox);
  void refreshChildTypeInLayer();
  string generateTempoStringByFraction();
  string generateTempoStringByNoteValue();
  void tempoAsNoteValueEntryChanged();

  TempoPrefix tempoPrefix;
  TempoNoteValue tempoNoteValue;
  Tempo* tempo;
  string originalTempoValueForFraction;
  string originalTempoValueForNoteValue;
  bool tempoEntryAsNoteValueModifiedFlag;
  std::vector<LayerBox*> layerBoxesStorage;
  std::vector<Gtk::CheckButton*> noteModifierCheckButtons;
  bool bottomSubAttributesShown;
  BottomEventModifierAlignment* modifiers;




};


#endif //EVENT_ATTRIBUTES_VIEW_CONTROLLER_H
