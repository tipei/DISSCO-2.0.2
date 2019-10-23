/*******************************************************************************
 *
 *  File Name     : EventAttributesViewController.cpp
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

#include "EventAttributesViewController.h"

#include "../../CMOD/src/CMOD.h"
#include "FunctionGenerator.h"
#include "IEvent.h"
#include "MainWindow.h"
#include "PaletteViewController.h"
#include "ProjectViewController.h"
#include "SharedPointers.h"

EventAttributesViewController::EventAttributesViewController(SharedPointers* _sharedPointers) {
    soundPartialHboxes = NULL;
    tempoEntryAsNoteValueModifiedFlag = false;
    tempo = new Tempo();
    sharedPointers = _sharedPointers;
    projectView = sharedPointers->projectView;
    currentlyShownEvent = NULL;
    modifiers = NULL;  // for bottom
    add(scrolledWindow);
    entryChangedByShowCurrentEvent = false;
    // Load the GtkBuilder files and instantiate their widgets:
    attributesRefBuilder = Gtk::Builder::create();
    attributesRefBuilderSound = Gtk::Builder::create();

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        attributesRefBuilder->add_from_file("./LASSIE/src/UI/Attributes.ui");
    } catch (const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
    } catch (const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
    try {
        attributesRefBuilder->add_from_file("./LASSIE/src/UI/BottomSubAttributes.ui");
    } catch (const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
    } catch (const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
    }

    try {
        attributesRefBuilderSound->add_from_file("./LASSIE/src/UI/SoundAttributes.ui");
    } catch (const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
    } catch (const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
#else
    std::auto_ptr<Glib::Error> error;
    if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/Attributes.ui", error)) {
        std::cerr << error->what() << std::endl;
    }

    if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/BottomSubAttributes.ui", error)) {
        std::cerr << error->what() << std::endl;
    }

    if (!attributesRefBuilderSound->add_from_file("./LASSIE/src/UI/SoundAttributes.ui", error)) {
        std::cerr << error->what() << std::endl;
    }
#endif /* !GLIBMM_EXCEPTIONS_ENABLED */

    // Get the GtkBuilder-instantiated Dialog:

    attributesRefBuilder->get_widget("attributesStandard", frame);
    frame->set_size_request(700, -1);
    // scrolledWindow.add(*frame);
    scrolledWindow.set_size_request(700, 600);

    Gtk::Button* addLayerButton;
    attributesRefBuilder->get_widget("addNewLayerButton", addLayerButton);
    addLayerButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::addNewLayerButtonClicked));

    // disable event name editing
    // TODO: enable event name editing and implement related behaviors
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesStandardNameEntry", entry);
    entry->set_editable(false);

    // connect num of children buttons to proper functions
    Gtk::RadioButton* radioButton;
    attributesRefBuilder->get_widget("attributesStandardNumChildrenFixedButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::fixedButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenDensityButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::densityButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenByLayerButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::byLayerButtonClicked));

    Gtk::VBox* vBox;
    attributesRefBuilder->get_widget("AttributestimeSignatureVBox", vBox);
    // vBox->set_width(30);

    // setup Tempo sub HBox

    Gtk::HBox* hBox;
    attributesRefBuilder->get_widget("attributesStandardTempoAsNoteValueHBox", hBox);
    Gtk::Alignment* alignment;
    attributesRefBuilder->get_widget("attributesStandardTempoAlignment", alignment);
    if (hBox->get_parent() == NULL) {
        alignment->add(*hBox);
    }

    Gtk::ComboBox* combobox;

    // set up tempo Prefix combo box
    attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
    tempoPrefixTreeModel = Gtk::ListStore::create(tempoPrefixColumns);
    combobox->set_model(tempoPrefixTreeModel);

    Gtk::TreeModel::Row row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixNone;
    row[tempoPrefixColumns.m_col_name] = "-------------------------";

    row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixDotted;
    row[tempoPrefixColumns.m_col_name] = "Dotted";

    row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixDoubleDotted;
    row[tempoPrefixColumns.m_col_name] = "Double Dotted";

    row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixTriple;
    row[tempoPrefixColumns.m_col_name] = "Triple";

    row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixQuintuple;
    row[tempoPrefixColumns.m_col_name] = "Quintuple";

    row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixSextuple;
    row[tempoPrefixColumns.m_col_name] = "Sextuple";

    row = *(tempoPrefixTreeModel->append());
    row[tempoPrefixColumns.m_col_type] = tempoPrefixSeptuple;
    row[tempoPrefixColumns.m_col_name] = "Septuple";

    combobox->pack_start(tempoPrefixColumns.m_col_name);

    // set up tempo notevalue combo box
    attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
    tempoNoteValueTreeModel = Gtk::ListStore::create(tempoNoteValueColumns);
    combobox->set_model(tempoNoteValueTreeModel);

    row = *(tempoNoteValueTreeModel->append());
    row[tempoNoteValueColumns.m_col_type] = tempoNoteValueWhole;
    row[tempoNoteValueColumns.m_col_name] = "Whole";

    row = *(tempoNoteValueTreeModel->append());
    row[tempoNoteValueColumns.m_col_type] = tempoNoteValueHalf;
    row[tempoNoteValueColumns.m_col_name] = "Half";

    row = *(tempoNoteValueTreeModel->append());
    row[tempoNoteValueColumns.m_col_type] = tempoNoteValueQuarter;
    row[tempoNoteValueColumns.m_col_name] = "Quarter";

    row = *(tempoNoteValueTreeModel->append());
    row[tempoNoteValueColumns.m_col_type] = tempoNoteValueEighth;
    row[tempoNoteValueColumns.m_col_name] = "8th";

    row = *(tempoNoteValueTreeModel->append());
    row[tempoNoteValueColumns.m_col_type] = tempoNoteValueSixteenth;
    row[tempoNoteValueColumns.m_col_name] = "16th";

    row = *(tempoNoteValueTreeModel->append());
    row[tempoNoteValueColumns.m_col_type] = tempoNoteValueThirtySecond;
    row[tempoNoteValueColumns.m_col_name] = "32nd";

    combobox->pack_start(tempoNoteValueColumns.m_col_name);

    attributesRefBuilder->get_widget("attributesStandardTempleMethodNoteValueRadioButton",
                                     radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::tempoAsNoteValueButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardTempleMethodFractionRadioButton",
                                     radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::tempoAsFractionButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
    entry->signal_changed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::tempoAsNoteValueEntryChanged));

    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
    entry->signal_changed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::tempoAsNoteValueEntryChanged));

    // connect childEventDef buttons to propre functions

    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
    attributesRefBuilder->get_widget("attributesChildEventDefContinuumButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::continuumButtonClicked));

    attributesRefBuilder->get_widget("attributesChildEventDefSweepButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::sweepButtonClicked));

    attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::discreteButtonClicked));

    // for bottom event
    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::freqWellTemperedButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesFundamentalButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::freqFundamentalButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesContinuumButton", radioButton);

    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::freqContinuumButtonClicked));

    // put welltempered as defult layout
    Gtk::Alignment* allignment;
    attributesRefBuilder->get_widget("BottomSubAttributesFrequencyAllignment", allignment);
    Gtk::HBox* wellTemperedVBox;
    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedHBox", wellTemperedVBox);

    allignment->add(*wellTemperedVBox);

    bottomSubAttributesShown = false;

    Gtk::Button* button;

    /*

      // connect make super collider code button

      attributesRefBuilder->get_widget(
        "attributesMakeSuperColliderCodeButton", button);

      button->signal_clicked().connect(sigc::mem_fun(*this, &
      EventAttributesViewController::makeSuperColliderCodeButtonClicked));

    */

    // connecting signal handlers
    attributesRefBuilder->get_widget("BottomSubAttributesNewModifierButton", button);

    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::addModifierButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardMaxChildDurFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::maxChildDurFunButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardUnitsPerSecondFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::unitsPerSecondFunButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenFunButton1", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::numOfChildFunButton1Clicked));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenFunButton2", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::numOfChildFunButton2Clicked));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenFunButton3", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::numOfChildFunButton3Clicked));

    attributesRefBuilder->get_widget("ChildEventDefButton1", button);
    button->signal_clicked().connect(sigc::mem_fun(
        *this, &EventAttributesViewController::childEventDefStartTimeFunButtonClicked));

    attributesRefBuilder->get_widget("ChildEventDefButton2", button);
    button->signal_clicked().connect(sigc::mem_fun(
        *this, &EventAttributesViewController::childEventDefChildTypeFunButtonClicked));

    attributesRefBuilder->get_widget("ChildEventDefButton3", button);
    button->signal_clicked().connect(sigc::mem_fun(
        *this, &EventAttributesViewController::childEventDefDurationFunButtonClicked));

    attributesRefBuilder->get_widget("ChildEventAttackSieveButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::childEventAttackSieveButtonClicked));

    attributesRefBuilder->get_widget("ChildEventDurationSieveButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::childEventDurationSieveButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardFilButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::attributesStandardFilButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardSpaButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::attributesStandardSpaButtonClicked));

    attributesRefBuilder->get_widget("attributesStandardRevButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::attributesStandardRevButtonClicked));

    //----------bottom sub

    attributesRefBuilder->get_widget("BottomSubAttributesLoudnessButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSLoudnessButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesSpatializationButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSSpatializationButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesReverbButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSReverbButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesFilterButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSFilterButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedButto", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSWellTemperedButtonClicked));

    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSFunFreqButton1Clicked));

    attributesRefBuilder->get_widget("BSubAttributesFunFreqButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSFunFreqButton2Clicked));

    attributesRefBuilder->get_widget("BSubAttributesContinuumButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::BSContinuumButtonClicked));

    // envelope
    attributesRefBuilder->get_widget("envelopeAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::envelopeFunButtonClicked));

    // sieve
    attributesRefBuilder->get_widget("SieveAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::sieveFunButtonClicked));

    // spa
    attributesRefBuilder->get_widget("SpatializationAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::spatializationFunButtonClicked));

    // rev
    attributesRefBuilder->get_widget("ReverbAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::reverbFunButtonClicked));

    // rev
    attributesRefBuilder->get_widget("FilterAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::filterFunButtonClicked));

    // measure
    attributesRefBuilder->get_widget("MeasureAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::measureFunButtonClicked));

    // Pattern
    attributesRefBuilder->get_widget("PatternAttributesFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::patternFunButtonClicked));

    // Spectrum, used to be Sound
    attributesRefBuilderSound->get_widget("SoundAttributesAddPartialButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::addPartialButtonClicked));

    attributesRefBuilderSound->get_widget("SoundAttributesDeviationFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::deviationFunButtonClicked));

    attributesRefBuilderSound->get_widget("SoundAttributesGenerateSpectrumFunButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::generatespectrumFunButtonClicked));

    // this section connect entrychange to modified
    // in order to show '*' at the title
    attributesRefBuilder->get_widget("attributesStandardMaxChildDurEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardUnitsPerSecondEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry1", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry2", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry2", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
    combobox->signal_changed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
    combobox->signal_changed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStartTypePercentageButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesDurationTypePercentageButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardSpaEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardRevEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("attributesStandardFilEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    Gtk::VBox* environmentAttributes;
    Gtk::VBox* mainAttributes;

    attributesRefBuilder->get_widget("attributesMainVBox", mainAttributes);

    attributesRefBuilder->get_widget("attributesStandardEnvironment", environmentAttributes);
    mainAttributes->pack_start((Gtk::Widget&)*environmentAttributes, Gtk::PACK_SHRINK);

    attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));
    show_all_children();

    attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesHertzButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesPowerOfTwoButton", radioButton);
    radioButton->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesContinuumEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry1", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry2", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesLoudnessEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesSpatializationEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("BottomSubAttributesReverbEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));
    attributesRefBuilder->get_widget("BottomSubAttributesFilterEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilderSound->get_widget("SoundAttributesNumPartialEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilderSound->get_widget("SoundAttributesDeviationEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilderSound->get_widget("SoundAttributesGenerateSpectrumEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilderSound->get_widget("SoundAttributesAddPartialButton", button);
    button->signal_pressed().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("envelopeAttributesBuilderEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("SieveAttributesBuilderEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("PatternAttributesBuilderEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("MeasureAttributesBuilderEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("ReverbAttributesBuilderEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    attributesRefBuilder->get_widget("SpatializationAttributesBuilderEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &EventAttributesViewController::modified));

    buildNoteModifiersList();
}

void EventAttributesViewController::modified() {
    if (entryChangedByShowCurrentEvent == false) {
        sharedPointers->projectView->modified();
    }
}

bool EventAttributesViewController::checkAttackSieve() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
    std::string text = entry->get_text();

    IEvent* sieve = projectView->getEventByTypeAndName(eventSiv, text);
    if (sieve == NULL) {
        return false;
    }
    return true;
}

bool EventAttributesViewController::checkDurationSieve() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);
    std::string text = entry->get_text();

    IEvent* sieve = projectView->getEventByTypeAndName(eventSiv, text);
    if (sieve == NULL) {
        return false;
    }
    return true;
}

EventAttributesViewController::~EventAttributesViewController() {}

void EventAttributesViewController::showAttributesOfEvent(IEvent* _event) {
    if (_event == NULL) {  // if the current shown event is deleted.
        currentlyShownEvent = NULL;

        // clear previous object's bottom modifiers if exist
        while (modifiers != NULL) {
            BottomEventModifierAlignment* temp = modifiers->next;

            modifiers->get_parent()->remove(*modifiers);
            delete modifiers;
            modifiers = temp;
        }

        // clear soundPartialHboxes ;
        if (soundPartialHboxes != NULL) {
            soundPartialHboxes->clear();
            soundPartialHboxes = NULL;
        }
        // scrolledWindow.remove(); //remove the child from the main scrolled
        // window
        Gtk::Viewport* temp = (Gtk::Viewport*)scrolledWindow.get_child();
        if (temp) {
            temp->remove();
        }

    } else {
        saveCurrentShownEventData();
        currentlyShownEvent = _event;

        entryChangedByShowCurrentEvent = true;

        showCurrentEventData();

        entryChangedByShowCurrentEvent = false;
        // if (sharedPointers->projectView->getSaved()){
        // cout<<"its saved"<<endl;
        // sharedPointers->mainWindow->setSavedTitle();
        //}
    }
}

void EventAttributesViewController::saveCurrentShownEventData() {
    // check if the current event is NULL or not
    if (currentlyShownEvent == NULL) {
        return;
    }
    if (soundPartialHboxes != NULL) {
        soundPartialHboxes->saveString();
        currentlyShownEvent->setChangedButNotSaved(true);
    }

    Gtk::Entry* entry;  // use to point to a text entry
    Gtk::Label* label;
    Gtk::RadioButton* radioButton;
    std::string stringBuffer;  // to set/get attributes

    // save Name
    attributesRefBuilder->get_widget("attributesStandardNameEntry", entry);
    currentlyShownEvent->setEventName(entry->get_text());
    projectView->refreshObjectNameInPalette(currentlyShownEvent);

    // if selected object is not a folder
    if (currentlyShownEvent->getEventType() <= eventBottom) {
        // save maxChildDur
        attributesRefBuilder->get_widget("attributesStandardMaxChildDurEntry", entry);
        currentlyShownEvent->setMaxChildDur(entry->get_text());

        // save time signature
        attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry1", entry);
        currentlyShownEvent->setTimeSignatureEntry1(entry->get_text());

        attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry2", entry);
        currentlyShownEvent->setTimeSignatureEntry2(entry->get_text());

        // save unitPerSecond
        attributesRefBuilder->get_widget("attributesStandardUnitsPerSecondEntry", entry);
        currentlyShownEvent->setUnitsPerSecond(entry->get_text());

        // save Tempo

        attributesRefBuilder->get_widget("attributesStandardTempleMethodNoteValueRadioButton",
                                         radioButton);
        if (radioButton->get_active()) {  // save as note value
            currentlyShownEvent->setTempoMethodFlag(0);
        } else {  // save as fraction
            currentlyShownEvent->setTempoMethodFlag(1);
        }

        Gtk::ComboBox* combobox;
        attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
        Gtk::TreeModel::iterator iter = combobox->get_active();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            if (row) {
                currentlyShownEvent->setTempoPrefix(row[tempoPrefixColumns.m_col_type]);
            }
        }
        attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
        iter = combobox->get_active();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            if (row) {
                currentlyShownEvent->setTempoNoteValue(row[tempoNoteValueColumns.m_col_type]);
            }
        }

        attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
        currentlyShownEvent->setTempoFractionEntry1(entry->get_text());

        // save the value here in order to restore data when showing attributes
        attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
        currentlyShownEvent->setTempoFractionEntry2(entry->get_text());

        attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
        currentlyShownEvent->setTempoValueEntry(entry->get_text());

        // save numOfChildren
        Gtk::RadioButton* radioButton;
        Gtk::RadioButton* radioButton2;
        attributesRefBuilder->get_widget("attributesStandardNumChildrenFixedButton", radioButton);
        attributesRefBuilder->get_widget("attributesStandardNumChildrenDensityButton",
                                         radioButton2);

        if (radioButton->get_active()) {  // fixed
            currentlyShownEvent->setFlagNumChildren(0);

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
            currentlyShownEvent->setNumChildrenEntry1(entry->get_text());
            currentlyShownEvent->setNumChildrenEntry2("0");
            currentlyShownEvent->setNumChildrenEntry3("0");
        }                                       // end fixed
        else if (radioButton2->get_active()) {  // density
            currentlyShownEvent->setFlagNumChildren(1);

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
            currentlyShownEvent->setNumChildrenEntry1(entry->get_text());

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
            currentlyShownEvent->setNumChildrenEntry2(entry->get_text());

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
            currentlyShownEvent->setNumChildrenEntry3(entry->get_text());
        }       // end density
        else {  // by layer
            currentlyShownEvent->setFlagNumChildren(2);

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
            currentlyShownEvent->setNumChildrenEntry1("");

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
            currentlyShownEvent->setNumChildrenEntry2("");

            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
            currentlyShownEvent->setNumChildrenEntry3("");
        }  // end by layer

        // save bylayer data  and probability of types for decrete childEventDef
        // use

        std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
        for (i; i != layerBoxesStorage.end(); i++) {
            (*i)->layerInEvent->byLayer = (*i)->weightEntry.get_text();
        }

        // set childEventDef here
        attributesRefBuilder->get_widget("attributesChildEventDefContinuumButton", radioButton);

        if (radioButton->get_active()) {  // continuum
            currentlyShownEvent->setFlagChildEventDef(0);

            attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
            currentlyShownEvent->setChildEventDefEntry1(entry->get_text());

            attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
            currentlyShownEvent->setChildEventDefEntry2(entry->get_text());

            attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
            currentlyShownEvent->setChildEventDefEntry3(entry->get_text());

            // set start type
            attributesRefBuilder->get_widget("attributesStartTypePercentageButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefStartType(0);
            }

            attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefStartType(1);
            }
            attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefStartType(2);
            }

            // set duration type

            attributesRefBuilder->get_widget("attributesDurationTypePercentageButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefDurationType(0);
            }

            attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefDurationType(1);
            }
            attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefDurationType(2);
            }

        }  // end continuum

        attributesRefBuilder->get_widget("attributesChildEventDefSweepButton", radioButton);

        if (radioButton->get_active()) {  // sweep
            currentlyShownEvent->setFlagChildEventDef(1);

            attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
            currentlyShownEvent->setChildEventDefEntry1(entry->get_text());

            attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
            currentlyShownEvent->setChildEventDefEntry2(entry->get_text());

            attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
            currentlyShownEvent->setChildEventDefEntry3(entry->get_text());

            // set start type
            attributesRefBuilder->get_widget("attributesStartTypePercentageButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefStartType(0);
            }

            attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefStartType(1);
            }
            attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefStartType(2);
            }

            // set duration type

            attributesRefBuilder->get_widget("attributesDurationTypePercentageButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefDurationType(0);
            }

            attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefDurationType(1);
            }
            attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton", radioButton);
            if (radioButton->get_active()) {
                currentlyShownEvent->setFlagChildEventDefDurationType(2);
            }

        }  // end sweep

        attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);
        if (radioButton->get_active()) {  // descrete
            currentlyShownEvent->setFlagChildEventDef(2);
            Gtk::Entry* entry;
            attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
            currentlyShownEvent->setChildEventDefAttackSieve(entry->get_text());

            attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);
            currentlyShownEvent->setChildEventDefDurationSieve(entry->get_text());

            currentlyShownEvent->setFlagChildEventDefStartType(-1);
            currentlyShownEvent->setFlagChildEventDefDurationType(-1);

        }  // descrete

        if (currentlyShownEvent->getEventType() == eventTop ||
            currentlyShownEvent->getEventType() == eventHigh ||
            currentlyShownEvent->getEventType() == eventMid ||
            currentlyShownEvent->getEventType() == eventLow) {
            // save environment
            attributesRefBuilder->get_widget("attributesStandardSpaEntry", entry);
            currentlyShownEvent->setSpa(entry->get_text());

            attributesRefBuilder->get_widget("attributesStandardRevEntry", entry);
            currentlyShownEvent->setRev(entry->get_text());

            attributesRefBuilder->get_widget("attributesStandardFilEntry", entry);
            currentlyShownEvent->setFil(entry->get_text());
        }

        // save BottomExtraInfo
        if (currentlyShownEvent->getEventType() == eventBottom) {
            attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedButton", radioButton);
            if (radioButton->get_active()) {
                // well tempered
                currentlyShownEvent->getEventExtraInfo()->setFrequencyFlag(0);

                attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedEntry", entry);
                currentlyShownEvent->getEventExtraInfo()->setFrequencyEntry1(entry->get_text());
            }

            attributesRefBuilder->get_widget("BottomSubAttributesFundamentalButton", radioButton);
            if (radioButton->get_active()) {
                // 1 = fundamental
                currentlyShownEvent->getEventExtraInfo()->setFrequencyFlag(1);
                attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry1", entry);
                currentlyShownEvent->getEventExtraInfo()->setFrequencyEntry1(entry->get_text());

                attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry2", entry);
                currentlyShownEvent->getEventExtraInfo()->setFrequencyEntry2(entry->get_text());
            }
            attributesRefBuilder->get_widget("BottomSubAttributesContinuumButton", radioButton);
            if (radioButton->get_active()) {
                // 2 = continuum
                currentlyShownEvent->getEventExtraInfo()->setFrequencyFlag(2);
                attributesRefBuilder->get_widget("BottomSubAttributesContinuumEntry", entry);
                currentlyShownEvent->getEventExtraInfo()->setFrequencyEntry1(entry->get_text());

                attributesRefBuilder->get_widget("BottomSubAttributesHertzButton", radioButton);
                if (radioButton->get_active()) {  // hertz
                    currentlyShownEvent->getEventExtraInfo()->setFrequencyContinuumFlag(0);

                } else {  // power of 2
                    currentlyShownEvent->getEventExtraInfo()->setFrequencyContinuumFlag(1);
                }
            }

            attributesRefBuilder->get_widget("BottomSubAttributesLoudnessEntry", entry);
            currentlyShownEvent->getEventExtraInfo()->setLoudness(entry->get_text());
            attributesRefBuilder->get_widget("BottomSubAttributesSpatializationEntry", entry);
            currentlyShownEvent->getEventExtraInfo()->setSpatialization(entry->get_text());
            attributesRefBuilder->get_widget("BottomSubAttributesReverbEntry", entry);
            currentlyShownEvent->getEventExtraInfo()->setReverb(entry->get_text());

            attributesRefBuilder->get_widget("BottomSubAttributesFilterEntry", entry);
            currentlyShownEvent->getEventExtraInfo()->setFilter(entry->get_text());

        }  // end handle BottomExtraInfo

        if (modifiers != NULL) {
            modifiers->saveToEvent();
        }

    }  // end if normal events
    else if (currentlyShownEvent->getEventType() == eventSound) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

        Gtk::Entry* entry;

        attributesRefBuilderSound->get_widget("SoundAttributesDeviationEntry", entry);
        extraInfo->setDeviation(entry->get_text());

        attributesRefBuilderSound->get_widget("SoundAttributesGenerateSpectrumEntry", entry);
        extraInfo->setSpectrumGenBuilder(entry->get_text());

    }

    else if (currentlyShownEvent->getEventType() == eventEnv) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("envelopeAttributesBuilderEntry", entry);
        extraInfo->setEnvelopeBuilder(entry->get_text());

    } else if (currentlyShownEvent->getEventType() == eventSiv) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("SieveAttributesBuilderEntry", entry);
        extraInfo->setSieveBuilder(entry->get_text());

    } else if (currentlyShownEvent->getEventType() == eventSpa) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("SpatializationAttributesBuilderEntry", entry);
        extraInfo->setSpatializationBuilder(entry->get_text());
    }

    else if (currentlyShownEvent->getEventType() == eventPat) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("PatternAttributesBuilderEntry", entry);
        extraInfo->setPatternBuilder(entry->get_text());

    } else if (currentlyShownEvent->getEventType() == eventRev) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("ReverbAttributesBuilderEntry", entry);
        extraInfo->setReverbBuilder(entry->get_text());

    } else if (currentlyShownEvent->getEventType() == eventFil) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("FilterAttributesBuilderEntry", entry);
        extraInfo->setFilterBuilder(entry->get_text());

    } else if (currentlyShownEvent->getEventType() == eventMea) {
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        attributesRefBuilder->get_widget("MeasureAttributesBuilderEntry", entry);
        extraInfo->setMeasureBuilder(entry->get_text());

    } else if (currentlyShownEvent->getEventType() == eventNote) {
        currentlyShownEvent->setChangedButNotSaved(true);
        IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
        extraInfo->clearNoteModifiers();
        std::vector<Gtk::CheckButton*>::iterator iter2 = noteModifierCheckButtons.begin();
        for (iter2; iter2 != noteModifierCheckButtons.end(); iter2++) {
            if ((*iter2)->get_active()) {
                extraInfo->addNoteModifiers((*iter2)->get_label());
            }
        }
    }

    // disable everything if it is a folder
    else if (currentlyShownEvent->getEventType() == eventFolder) {
        attributesRefBuilder->get_widget("attributesStandardMaxChildDurEntry", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry1", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry2", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardUnitsPerSecondEntry", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry2", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
        entry->set_sensitive(false);

        Gtk::ComboBox* combobox;
        attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
        combobox->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
        combobox->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
        entry->set_sensitive(false);

        attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
        entry->set_sensitive(false);
    }
}

void EventAttributesViewController::showCurrentEventData() {
    // clear previous object's bottom modifiers if exist
    while (modifiers != NULL) {
        BottomEventModifierAlignment* temp = modifiers->next;

        modifiers->get_parent()->remove(*modifiers);
        delete modifiers;
        modifiers = temp;
    }

    // clear soundPartialHboxes ;
    if (soundPartialHboxes != NULL) {
        soundPartialHboxes->clear();
        soundPartialHboxes = NULL;
    }

    Gtk::Viewport* temp = (Gtk::Viewport*)scrolledWindow.get_child();
    if (temp) {
        temp->remove();
    }

    Gtk::Entry* entry;  // use to point to a text entry
    Gtk::Label* label;
    Gtk::RadioButton* radioButton;
    std::string stringBuffer;  // to set/get attributes

    // show type
    attributesRefBuilder->get_widget("attributesStandardType", label);
    stringBuffer = "IEvent Type: " + currentlyShownEvent->getEventTypeString();
    label->set_text(stringBuffer);

    // show Name

    attributesRefBuilder->get_widget("attributesStandardNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());

    int selectTheEventAction;

    if (currentlyShownEvent->getEventType() == eventTop ||
        currentlyShownEvent->getEventType() == eventHigh ||
        currentlyShownEvent->getEventType() == eventMid ||
        currentlyShownEvent->getEventType() == eventLow ||
        currentlyShownEvent->getEventType() == eventBottom) {
        selectTheEventAction = 0;

    } else {
        selectTheEventAction = currentlyShownEvent->getEventType();
    }

    Gtk::VBox* layerBoxes;
    Gtk::VBox* attributesMainVBox;
    std::vector<LayerBox*>::iterator i;

    std::list<EventLayer*>::iterator j;

    switch (selectTheEventAction) {
        case 0:  // selected object is an event

            // show maxChildDur

            attributesRefBuilder->get_widget("attributesStandard", frame);
            scrolledWindow.add(*frame);

            if (bottomSubAttributesShown && currentlyShownEvent->getEventType() != eventBottom) {
                // remove sub bottom
                Gtk::VBox* bottomSubAttributes;
                Gtk::VBox* mainAttributes;
                Gtk::VBox* environmentAttributes;

                attributesRefBuilder->get_widget("BottomSubAttributesVBox", bottomSubAttributes);
                attributesRefBuilder->get_widget("attributesMainVBox", mainAttributes);

                mainAttributes->remove(*bottomSubAttributes);

                attributesRefBuilder->get_widget("attributesStandardEnvironment",
                                                 environmentAttributes);
                mainAttributes->pack_start((Gtk::Widget&)*environmentAttributes, Gtk::PACK_SHRINK);

                bottomSubAttributesShown = false;
            }

            if (!bottomSubAttributesShown && currentlyShownEvent->getEventType() == eventBottom) {
                // add sub bottom
                Gtk::VBox* bottomSubAttributes;
                Gtk::VBox* mainAttributes;
                Gtk::VBox* environmentAttributes;

                attributesRefBuilder->get_widget("attributesStandardEnvironment",
                                                 environmentAttributes);
                attributesRefBuilder->get_widget("attributesMainVBox", mainAttributes);
                mainAttributes->remove(*environmentAttributes);

                attributesRefBuilder->get_widget("BottomSubAttributesVBox", bottomSubAttributes);
                mainAttributes->pack_start((Gtk::Widget&)*bottomSubAttributes, Gtk::PACK_SHRINK);
                bottomSubAttributesShown = true;
            }

            attributesRefBuilder->get_widget("attributesStandardMaxChildDurEntry", entry);
            entry->set_sensitive(true);
            entry->set_text(currentlyShownEvent->getMaxChildDur());

            // show Time Signature

            attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry1", entry);
            entry->set_sensitive(true);
            entry->set_text(currentlyShownEvent->getTimeSignatureEntry1());

            attributesRefBuilder->get_widget("attributesStandardTimeSignatureEntry2", entry);
            entry->set_sensitive(true);
            entry->set_text(currentlyShownEvent->getTimeSignatureEntry2());

            // show unitPerSecond
            attributesRefBuilder->get_widget("attributesStandardUnitsPerSecondEntry", entry);
            entry->set_sensitive(true);
            entry->set_text(currentlyShownEvent->getUnitsPerSecond());

            // show tempo
            attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
            entry->set_sensitive(true);
            attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry2", entry);
            entry->set_sensitive(true);
            attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
            entry->set_sensitive(true);

            Gtk::ComboBox* combobox;
            attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
            combobox->set_sensitive(true);

            attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
            combobox->set_sensitive(true);

            if (currentlyShownEvent->getTempoMethodFlag() == 0) {  // tempo as note value
                attributesRefBuilder->get_widget(
                    "attributesStandardTempleMethodNoteValueRadioButton", radioButton);
                radioButton->set_active();
                tempoAsNoteValueButtonClicked();
                attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox",
                                                 combobox);
                combobox->set_active(currentlyShownEvent->getTempoPrefix());

                attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
                combobox->set_active(currentlyShownEvent->getTempoNoteValue());

                // when switching, it'll generate the proper string
                tempoEntryAsNoteValueModifiedFlag = true;

                // check if original value is an integer or a fraction

                string originalValue = currentlyShownEvent->getTempoValueEntry();
                size_t foundSlash = originalValue.find("/");
                if (foundSlash != string::npos) {  // it is fraction
                    string entry1 = originalValue.substr(0, foundSlash);

                    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1",
                                                     entry);
                    entry->set_text(entry1);

                    char charBuffer[20];

                    int value = atoi(
                        (originalValue.substr(foundSlash + 1, originalValue.length())).c_str());

                    value = value * 60;
                    sprintf(charBuffer, "%d", value);

                    string ratioString = entry1 + "/" + charBuffer;

                    Ratio ratio = Ratio(ratioString);

                    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1",
                                                     entry);

                    sprintf(charBuffer, "%d", ratio.Num());
                    entry->set_text(string(charBuffer));
                    sprintf(charBuffer, "%d", ratio.Den());
                    originalTempoValueForFraction = string(charBuffer);
                    tempoEntryAsNoteValueModifiedFlag = false;
                }

                // entry 1 set # of note
                // original for value

                attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
                entry->set_text(currentlyShownEvent->getTempoValueEntry());

            } else {  // tempo as fraction
                attributesRefBuilder->get_widget(
                    "attributesStandardTempleMethodFractionRadioButton", radioButton);
                radioButton->set_active();
                tempoAsFractionButtonClicked();

                attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox",
                                                 combobox);
                combobox->set_active(currentlyShownEvent->getTempoPrefix());

                attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
                combobox->set_active(currentlyShownEvent->getTempoNoteValue());

                attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
                entry->set_text(currentlyShownEvent->getTempoFractionEntry1());

                attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
                entry->set_text(currentlyShownEvent->getTempoValueEntry());
                // so that when switch, it'll generate the proper string
                tempoEntryAsNoteValueModifiedFlag = true;
            }

            // show numOfChildren
            if (currentlyShownEvent->getFlagNumChildren() == 0) {  // fixed
                attributesRefBuilder->get_widget("attributesStandardNumChildrenFixedButton",
                                                 radioButton);
                radioButton->set_active();

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel1", label);
                label->set_text("Number Of Children To Create:");
                label->set_tooltip_text("INT");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getNumChildrenEntry1());

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel2", label);
                label->set_text("");
                label->set_tooltip_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
                entry->set_sensitive(false);

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel3", label);
                label->set_text("");
                label->set_tooltip_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
                entry->set_sensitive(false);

            }                                                           // end fixed
            else if (currentlyShownEvent->getFlagNumChildren() == 1) {  // density
                attributesRefBuilder->get_widget("attributesStandardNumChildrenDensityButton",
                                                 radioButton);
                radioButton->set_active();

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel1", label);
                label->set_text("Density:");
                label->set_tooltip_text("FLOAT, Between 0 and 1.0");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getNumChildrenEntry1());

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel2", label);
                label->set_text("Area:");
                label->set_tooltip_text("INT");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getNumChildrenEntry2());

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel3", label);
                label->set_text("Under One:");
                label->set_tooltip_text("INT");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getNumChildrenEntry3());
            }       // end density
            else {  // by layer
                attributesRefBuilder->get_widget("attributesStandardNumChildrenByLayerButton",
                                                 radioButton);
                radioButton->set_active();

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel1", label);
                label->set_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel2", label);
                label->set_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel3", label);
                label->set_text("");

                attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
                entry->set_sensitive(false);
                entry->set_text("");
            }

            // ChildEventDef

            Gtk::Alignment* alignment;
            Gtk::VBox* vbox;

            if (currentlyShownEvent->getFlagChildEventDef() == 0) {  // continuum

                attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepAlignment",
                                                 alignment);
                if (alignment->get_child() == NULL) {  // alignment is empty
                    attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepVBox",
                                                     vbox);
                    alignment->add(*vbox);

                    attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteAlignment",
                                                     alignment);
                    alignment->remove();
                }

                attributesRefBuilder->get_widget("attributesChildEventDefContinuumButton",
                                                 radioButton);

                radioButton->set_active();

                attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry",
                                                 entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefEntry1());

                attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefEntry2());

                attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefEntry3());

                attributesRefBuilder->get_widget("attributesStartTypePercentageButton",
                                                 radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
                radioButton->set_sensitive(true);

                attributesRefBuilder->get_widget("attributesDurationTypePercentageButton",
                                                 radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton",
                                                 radioButton);
                radioButton->set_sensitive(true);

                switch (currentlyShownEvent->getFlagChildEventDefStartType()) {
                    case 0:
                        attributesRefBuilder->get_widget("attributesStartTypePercentageButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 1:
                        attributesRefBuilder->get_widget("attributesStartTypeUnitsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 2:
                        attributesRefBuilder->get_widget("attributesStartTypeSecondsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                }

                switch (currentlyShownEvent->getFlagChildEventDefDurationType()) {
                    case 0:
                        attributesRefBuilder->get_widget("attributesDurationTypePercentageButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 1:
                        attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 2:
                        attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                }
            }  // end continuum

            else if (currentlyShownEvent->getFlagChildEventDef() == 1) {  // sweep

                attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepAlignment",
                                                 alignment);
                if (alignment->get_child() == NULL) {  // alignment is empty
                    attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepVBox",
                                                     vbox);
                    alignment->add(*vbox);

                    attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteAlignment",
                                                     alignment);
                    alignment->remove();
                }

                attributesRefBuilder->get_widget("attributesChildEventDefSweepButton", radioButton);
                radioButton->set_active();

                attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry",
                                                 entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefEntry1());

                attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefEntry2());

                attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefEntry3());

                attributesRefBuilder->get_widget("attributesStartTypePercentageButton",
                                                 radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
                radioButton->set_sensitive(true);

                attributesRefBuilder->get_widget("attributesDurationTypePercentageButton",
                                                 radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
                radioButton->set_sensitive(true);
                attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton",
                                                 radioButton);
                radioButton->set_sensitive(true);

                switch (currentlyShownEvent->getFlagChildEventDefStartType()) {
                    case 0:
                        attributesRefBuilder->get_widget("attributesStartTypePercentageButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 1:
                        attributesRefBuilder->get_widget("attributesStartTypeUnitsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 2:
                        attributesRefBuilder->get_widget("attributesStartTypeSecondsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                }

                switch (currentlyShownEvent->getFlagChildEventDefDurationType()) {
                    case 0:
                        attributesRefBuilder->get_widget("attributesDurationTypePercentageButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 1:
                        attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                    case 2:
                        attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton",
                                                         radioButton);
                        radioButton->set_active();
                        break;
                }

            }       // end sweep
            else {  // discrete

                attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteAlignment",
                                                 alignment);
                if (alignment->get_child() == NULL) {  // alignment is empty
                    attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteVBox", vbox);
                    alignment->add(*vbox);

                    attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepAlignment",
                                                     alignment);
                    alignment->remove();
                }

                attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton",
                                                 radioButton);
                radioButton->set_active();

                attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefAttackSieve());

                attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry",
                                                 entry);
                entry->set_sensitive(true);
                entry->set_text(currentlyShownEvent->getChildEventDefDurationSieve());

                attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
                entry->set_sensitive(false);
                entry->set_text("");

                attributesRefBuilder->get_widget("attributesStartTypePercentageButton",
                                                 radioButton);
                radioButton->set_sensitive(false);
                attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
                radioButton->set_sensitive(false);
                attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
                radioButton->set_sensitive(false);

                attributesRefBuilder->get_widget("attributesDurationTypePercentageButton",
                                                 radioButton);
                radioButton->set_sensitive(false);
                attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
                radioButton->set_sensitive(false);
                attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton",
                                                 radioButton);
                radioButton->set_sensitive(false);

            }  // end discrete

            // construct layers
            attributesRefBuilder->get_widget("layerBoxes", layerBoxes);

            i = layerBoxesStorage.begin();
            for (i; i != layerBoxesStorage.end(); ++i) {
                layerBoxes->remove(**i);
            }
            layerBoxesStorage.clear();

            j = currentlyShownEvent->layers.begin();
            for (j; j != currentlyShownEvent->layers.end(); ++j) {
                LayerBox* newbox =
                    new LayerBox(this, projectView, *j,
                                 (currentlyShownEvent->getFlagChildEventDef() == 2 ? true : false));

                layerBoxes->pack_start((Gtk::Widget&)*newbox, Gtk::PACK_SHRINK);
                layerBoxesStorage.push_back(newbox);
                if (currentlyShownEvent->getFlagNumChildren() == 2) {  // if bylayer
                    newbox->weightEntry.set_sensitive(true);
                    newbox->weightEntry.set_text((*j)->byLayer);
                } else {
                    newbox->weightEntry.set_sensitive(false);
                    newbox->weightEntry.set_text("");
                }
            }

            if (currentlyShownEvent->getFlagChildEventDef() != 2) {
                std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
                for (i; i != layerBoxesStorage.end(); i++) {
                    if ((*i)->weightColumn.get_tree_view() != NULL) {
                        (*i)->m_TreeView.remove_column((*i)->weightColumn);
                        (*i)->m_TreeView.remove_column((*i)->attackEnvColumn);
                        (*i)->m_TreeView.remove_column((*i)->attackEnvScaleColumn);
                        (*i)->m_TreeView.remove_column((*i)->durationEnvColumn);
                        (*i)->m_TreeView.remove_column((*i)->durationEnvScaleColumn);
                        //(*i)->weightEntry.set_text("");
                    }
                }
            } else {
                std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
                for (i; i != layerBoxesStorage.end(); i++) {
                    if ((*i)->weightColumn.get_tree_view() == NULL) {
                        (*i)->m_TreeView.append_column((*i)->weightColumn);
                        (*i)->m_TreeView.append_column((*i)->attackEnvColumn);
                        (*i)->m_TreeView.append_column((*i)->attackEnvScaleColumn);
                        (*i)->m_TreeView.append_column((*i)->durationEnvColumn);
                        (*i)->m_TreeView.append_column((*i)->durationEnvScaleColumn);
                        //(*i)->weightEntry.set_text("");
                    }
                }
            }

            refreshChildTypeInLayer();

            // show environment

            if (currentlyShownEvent->getEventType() == eventTop ||
                currentlyShownEvent->getEventType() == eventHigh ||
                currentlyShownEvent->getEventType() == eventMid ||
                currentlyShownEvent->getEventType() == eventLow) {
                attributesRefBuilder->get_widget("attributesStandardFilEntry", entry);
                entry->set_text(currentlyShownEvent->getFil());

                attributesRefBuilder->get_widget("attributesStandardSpaEntry", entry);
                entry->set_text(currentlyShownEvent->getSpa());

                attributesRefBuilder->get_widget("attributesStandardRevEntry", entry);
                entry->set_text(currentlyShownEvent->getRev());
            }

            // show bottom extra info
            if (currentlyShownEvent->getEventType() == eventBottom) {
                IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();
                if (extraInfo->getFrequencyFlag() == 0) {  // well_tempered
                    freqWellTemperedButtonClicked();
                    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedButton",
                                                     radioButton);
                    radioButton->set_active();

                    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedEntry", entry);
                    entry->set_text(extraInfo->getFrequencyEntry1());

                    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry1", entry);
                    entry->set_text("");

                    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry2", entry);
                    entry->set_text("");

                    attributesRefBuilder->get_widget("BottomSubAttributesContinuumEntry", entry);
                    entry->set_text("");

                } else if (extraInfo->getFrequencyFlag() == 1) {  // Fundamental
                    freqFundamentalButtonClicked();
                    attributesRefBuilder->get_widget("BottomSubAttributesFundamentalButton",
                                                     radioButton);
                    radioButton->set_active();

                    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedEntry", entry);
                    entry->set_text("");

                    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry1", entry);
                    entry->set_text(extraInfo->getFrequencyEntry1());

                    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry2", entry);
                    entry->set_text(extraInfo->getFrequencyEntry2());

                    attributesRefBuilder->get_widget("BottomSubAttributesContinuumEntry", entry);
                    entry->set_text("");

                } else {  // Continuum
                    freqContinuumButtonClicked();
                    attributesRefBuilder->get_widget("BottomSubAttributesContinuumButton",
                                                     radioButton);
                    radioButton->set_active();

                    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedEntry", entry);
                    entry->set_text("");

                    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry1", entry);
                    entry->set_text("");

                    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry2", entry);
                    entry->set_text("");

                    attributesRefBuilder->get_widget("BottomSubAttributesContinuumEntry", entry);
                    entry->set_text(extraInfo->getFrequencyEntry1());

                    if (extraInfo->getFrequencyContinuumFlag() == 0) {  // hertz
                        attributesRefBuilder->get_widget("BottomSubAttributesHertzButton",
                                                         radioButton);
                        radioButton->set_active();

                    } else {  // power of two
                        attributesRefBuilder->get_widget("BottomSubAttributesPowerOfTwoButton",
                                                         radioButton);
                        radioButton->set_active();
                    }
                }  // end three cases well_tempered, fundamental , continuum

                attributesRefBuilder->get_widget("BottomSubAttributesLoudnessEntry", entry);
                entry->set_text(extraInfo->getLoudness());

                if (extraInfo->getChildTypeFlag() == 0) {  // enable spa and reverb entries

                    attributesRefBuilder->get_widget("BottomSubAttributesSpatializationEntry",
                                                     entry);
                    entry->set_text(extraInfo->getSpatialization());
                    entry->set_sensitive(true);

                    attributesRefBuilder->get_widget("BottomSubAttributesReverbEntry", entry);
                    entry->set_text(extraInfo->getReverb());
                    entry->set_sensitive(true);

                    attributesRefBuilder->get_widget("BottomSubAttributesFilterEntry", entry);
                    entry->set_text(extraInfo->getFilter());
                    entry->set_sensitive(true);

                } else {
                    attributesRefBuilder->get_widget("BottomSubAttributesSpatializationEntry",
                                                     entry);
                    entry->set_text("");
                    entry->set_sensitive(false);

                    attributesRefBuilder->get_widget("BottomSubAttributesReverbEntry", entry);
                    entry->set_text("");
                    entry->set_sensitive(false);

                    attributesRefBuilder->get_widget("BottomSubAttributesFilterEntry", entry);
                    entry->set_text("");
                    entry->set_sensitive(false);
                }

                // show/hide modifiers

                int childType = extraInfo->getChildTypeFlag();

                Gtk::Label* label;
                Gtk::VBox* vbox;
                Gtk::VBox* vbox2;
                if (childType == 0) {  // sound
                    attributesRefBuilder->get_widget("BottomSubAttributesNewModifierButton",
                                                     button);
                    attributesRefBuilder->get_widget("BottomSubAttributesVBox", vbox2);
                    attributesRefBuilder->get_widget("BottomSubAttributesModifiersVBox", vbox);
                    attributesRefBuilder->get_widget("ModifierLabel", label);
                    /* old code
                    if (button->get_parent() ==NULL){
                      attributesRefBuilder->get_widget(
                            "ModifierLabel", label);
                            vbox2->pack_start(*label, Gtk::PACK_SHRINK);
                            vbox2->pack_start(*vbox, Gtk::PACK_SHRINK);
                            vbox2->pack_start(*button, Gtk::PACK_SHRINK);
                    }
                    */
                    //////////////////////////////////////////new code

                    if (button->get_parent() != NULL) {
                        button->get_parent()->remove(*button);
                    }
                    if (label->get_parent() != NULL) {
                        label->get_parent()->remove(*label);
                    }
                    if (vbox->get_parent() != NULL) {
                        vbox->get_parent()->remove(*vbox);
                    }

                    vbox2->pack_start(*label, Gtk::PACK_SHRINK);
                    vbox2->pack_start(*vbox, Gtk::PACK_SHRINK);
                    vbox2->pack_start(*button, Gtk::PACK_SHRINK);

                    /////////////////////////////////////////////////// end new
                    /// code
                    EventBottomModifier* EBmodifiers =
                        currentlyShownEvent->getEventExtraInfo()->getModifiers();

                    BottomEventModifierAlignment* nextModifierAlignment;
                    while (EBmodifiers != NULL) {
                        if (modifiers == NULL) {  // if the first modifier

                            modifiers = new BottomEventModifierAlignment(EBmodifiers, this);
                            modifiers->prev = NULL;

                            nextModifierAlignment = modifiers;
                            vbox->pack_start(*nextModifierAlignment, Gtk::PACK_SHRINK);

                        } else {  // rest of the modifiers

                            nextModifierAlignment->next =
                                new BottomEventModifierAlignment(EBmodifiers, this);
                            nextModifierAlignment->next->prev = nextModifierAlignment;
                            nextModifierAlignment = nextModifierAlignment->next;
                            vbox->pack_start(*nextModifierAlignment, Gtk::PACK_SHRINK);
                        }
                        EBmodifiers = EBmodifiers->next;
                    }

                } else if (childType == 1) {  // note

                    attributesRefBuilder->get_widget("BottomSubAttributesNewModifierButton",
                                                     button);

                    attributesRefBuilder->get_widget("BottomSubAttributesVBox", vbox2);
                    attributesRefBuilder->get_widget("BottomSubAttributesModifiersVBox", vbox);

                    if (button->get_parent() != NULL) {
                        attributesRefBuilder->get_widget("ModifierLabel", label);

                        vbox2->remove(*label);
                        vbox2->remove(*button);
                        vbox2->remove(*vbox);
                    }

                }

                else {  // childType == 2, visual
                }

            }  // end showing bottom extra info

            else {  // modifirs for events

                Gtk::Label* label;
                Gtk::VBox* vbox;
                Gtk::VBox* vbox2;

                attributesRefBuilder->get_widget("BottomSubAttributesNewModifierButton", button);
                attributesRefBuilder->get_widget("attributesMainVBox", vbox2);
                attributesRefBuilder->get_widget("BottomSubAttributesModifiersVBox", vbox);

                attributesRefBuilder->get_widget("ModifierLabel", label);
                if (button->get_parent() != NULL) {
                    button->get_parent()->remove(*button);
                }
                if (label->get_parent() != NULL) {
                    label->get_parent()->remove(*label);
                }
                if (vbox->get_parent() != NULL) {
                    vbox->get_parent()->remove(*vbox);
                }
                // cout<<"add them to the right vbox"<<endl;
                vbox2->pack_start(*label, Gtk::PACK_SHRINK);
                vbox2->pack_start(*vbox, Gtk::PACK_SHRINK);
                vbox2->pack_start(*button, Gtk::PACK_SHRINK);

                EventBottomModifier* EBmodifiers = currentlyShownEvent->getModifiers();

                if (EBmodifiers != NULL) {
                }
                BottomEventModifierAlignment* nextModifierAlignment;
                while (EBmodifiers != NULL) {
                    if (modifiers == NULL) {  // if the first modifier

                        modifiers = new BottomEventModifierAlignment(EBmodifiers, this);
                        modifiers->prev = NULL;

                        nextModifierAlignment = modifiers;
                        vbox->pack_start(*nextModifierAlignment, Gtk::PACK_SHRINK);

                    } else {  // rest of the modifiers

                        nextModifierAlignment->next =
                            new BottomEventModifierAlignment(EBmodifiers, this);
                        nextModifierAlignment->next->prev = nextModifierAlignment;
                        nextModifierAlignment = nextModifierAlignment->next;
                        vbox->pack_start(*nextModifierAlignment, Gtk::PACK_SHRINK);
                    }
                    EBmodifiers = EBmodifiers->next;

                }  // end while

            }  // end event modifiers

            break;

        case 5:
            switchToSoundAttributes();
            break;
        case 6:
            switchToEnvAttributes();
            break;
        case 7:
            switchToSivAttributes();
            break;
        case 8:
            switchToSpaAttributes();
            break;
        case 9:
            switchToPatAttributes();
            break;
        case 10:
            switchToRevAttributes();
            break;

        case 11:

            break;

        case 12:
            switchToNoteAttributes();
            break;
        case 13:
            switchToFilAttributes();
            break;
        case 14:
            switchToMeaAttributes();
            break;
    }

    show_all_children();
}

void EventAttributesViewController::switchToSoundAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilderSound->get_widget("SoundAttributesStandard", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilderSound->get_widget("SoundAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilderSound->get_widget("SoundAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilderSound->get_widget("SoundAttributesNumPartialEntry", entry);

    entry->set_text(extraInfo->getNumPartials());
    attributesRefBuilderSound->get_widget("SoundAttributesDeviationEntry", entry);
    entry->set_text(extraInfo->getDeviation());

    attributesRefBuilderSound->get_widget("SoundAttributesGenerateSpectrumEntry", entry);

    entry->set_text(extraInfo->getSpectrumGenBuilder());

    SpectrumPartial* partials = extraInfo->getSpectrumPartials();

    SoundPartialHBox* currentBox = NULL;
    Gtk::VBox* vbox;
    attributesRefBuilderSound->get_widget("SpectrumVBox", vbox);

    if (partials != NULL) {
        soundPartialHboxes = new SoundPartialHBox(partials, this);
        partials = partials->next;
        currentBox = soundPartialHboxes;
        vbox->pack_start(*currentBox, Gtk::PACK_SHRINK);
    }

    while (partials != NULL) {
        currentBox->next = new SoundPartialHBox(partials, this);
        currentBox->next->prev = currentBox;
        currentBox = currentBox->next;
        partials = partials->next;
        vbox->pack_start(*currentBox, Gtk::PACK_SHRINK);
    }

    if (soundPartialHboxes != NULL) {
        soundPartialHboxes->setPartialNumber(1);
    }

    attributesRefBuilderSound->get_widget("SoundAttributesNumPartialEntry", entry);
    entry->set_text(extraInfo->getNumPartials());

    show_all_children();
}

void EventAttributesViewController::switchToEnvAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("envelopeAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("envelopeAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("envelopeAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("envelopeAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getEnvelopeBuilder());
}

void EventAttributesViewController::switchToSpaAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("SpatializationAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("SpatializationAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("SpatializationAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("SpatializationAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getSpatializationBuilder());
}

void EventAttributesViewController::switchToPatAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("PatternAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("PatternAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("PatternAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("PatternAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getPatternBuilder());
}

void EventAttributesViewController::switchToSivAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("SieveAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("SieveAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("SieveAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("SieveAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getSieveBuilder());
}

void EventAttributesViewController::switchToRevAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("ReverbAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("ReverbAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("ReverbAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("ReverbAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getReverbBuilder());
}

void EventAttributesViewController::switchToFilAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("FilterAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("FilterAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("FilterAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("FilterAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getFilterBuilder());
}

void EventAttributesViewController::switchToMeaAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("MeasureAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("MeasureAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("MeasureAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    attributesRefBuilder->get_widget("MeasureAttributesBuilderEntry", entry);

    entry->set_text(extraInfo->getFilterBuilder());
}

void EventAttributesViewController::switchToNoteAttributes() {
    Gtk::Frame* frame;
    IEvent::EventExtraInfo* extraInfo = currentlyShownEvent->getEventExtraInfo();

    attributesRefBuilder->get_widget("NoteAttributesFrame", frame);
    scrolledWindow.add(*frame);
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("NoteAttributesNameEntry", entry);
    entry->set_text(currentlyShownEvent->getEventName());
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("NoteAttributesTypeEntry", entry);
    entry->set_text(currentlyShownEvent->getEventTypeString());
    entry->set_sensitive(false);

    // clear all checkbutton

    std::vector<Gtk::CheckButton*>::iterator iter = noteModifierCheckButtons.begin();
    for (iter; iter != noteModifierCheckButtons.end(); iter++) {
        (*iter)->set_active(false);
    }

    std::list<std::string> modifierList = extraInfo->getNoteModifiers();
    std::list<std::string>::iterator iter2 = modifierList.begin();
    for (iter2; iter2 != modifierList.end(); iter2++) {
        iter = noteModifierCheckButtons.begin();
        while (iter != noteModifierCheckButtons.end()) {
            if ((*iter)->get_label() == *iter2) {
                (*iter)->set_active(true);
                break;
            }
            iter++;
        }
    }
}

void EventAttributesViewController::LayerBox::on_label_drop_drag_data_received(
    const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,
    const Gtk::SelectionData& selection_data, guint info, guint time) {
    IEvent* insertEvent = projectView->getPalette()->getCurrentSelectedEvent();

    if (insertEvent->getEventType() == eventFolder) {
        Gtk::MessageDialog dialog("A Folder cannot be placed into this event.",
                                  false /* use_markup */, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);

        dialog.set_secondary_text(
            "Folders in the palette are only used to organized different types "
            "of event. They cannot be placed into the events. ");

        int result = dialog.run();

        // show warning
        return;
    }

    // setup attributes tree view here

    Gtk::TreeModel::Row row = *(m_refTreeModel->append());
    row[m_Columns.columnObjectType] = insertEvent->getEventTypeString();
    row[m_Columns.columnObjectName] = insertEvent->getEventName();
    row[m_Columns.columnEntry] = layerInEvent->addChild(insertEvent);

    context->drag_finish(false, false, time);

    if (layerInEvent->size() == 1) {
        attributesView->refresh();  // so that the right click of delete event works
    }
    attributesView->refreshChildTypeInLayer();
    projectView->modified();

    set_size_request(-1, layerInEvent->size() * 25 + 140);

    Gtk::Adjustment* adjustment = m_ScrolledWindow2.get_vadjustment();
    adjustment->set_value(adjustment->get_upper());
}

void EventAttributesViewController::refresh() { showAttributesOfEvent(currentlyShownEvent); }

void EventAttributesViewController::continuumButtonClicked() {
    if (currentlyShownEvent == NULL) return;
    modified();
    Gtk::RadioButton* radioButton;
    attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);

    if (radioButton->get_active()) {
        std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
        for (i; i != layerBoxesStorage.end(); i++) {
            (*i)->m_TreeView.remove_column((*i)->weightColumn);
            (*i)->m_TreeView.remove_column((*i)->attackEnvColumn);
            (*i)->m_TreeView.remove_column((*i)->attackEnvScaleColumn);
            (*i)->m_TreeView.remove_column((*i)->durationEnvColumn);
            (*i)->m_TreeView.remove_column((*i)->durationEnvScaleColumn);
            //(*i)->weightEntry.set_text("");
        }
    }
    Gtk::Alignment* alignment;
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepAlignment", alignment);
    if (alignment->get_child() == NULL) {  // alignment is empty
        attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepVBox", vbox);
        alignment->add(*vbox);

        attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteAlignment", alignment);
        alignment->remove();
    }

    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
    entry->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
    entry->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
    entry->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);

    entry->set_sensitive(false);
    currentlyShownEvent->setChildEventDefAttackSieve(entry->get_text());
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);

    entry->set_sensitive(false);
    currentlyShownEvent->setChildEventDefDurationSieve(entry->get_text());
    // entry->set_text( ""   );

    attributesRefBuilder->get_widget("attributesStartTypePercentageButton", radioButton);
    radioButton->set_sensitive(true);
    // radioButton->set_active();

    attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
    radioButton->set_sensitive(true);
    attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
    radioButton->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesDurationTypePercentageButton", radioButton);
    radioButton->set_sensitive(true);
    // radioButton->set_active();
    attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
    radioButton->set_sensitive(true);
    attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton", radioButton);
    radioButton->set_sensitive(true);
}

void EventAttributesViewController::sweepButtonClicked() {
    if (currentlyShownEvent == NULL) return;
    modified();
    Gtk::RadioButton* radioButton;
    attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);

    if (radioButton->get_active()) {
        std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
        for (i; i != layerBoxesStorage.end(); i++) {
            (*i)->m_TreeView.remove_column((*i)->weightColumn);
            (*i)->m_TreeView.remove_column((*i)->attackEnvColumn);
            (*i)->m_TreeView.remove_column((*i)->attackEnvScaleColumn);
            (*i)->m_TreeView.remove_column((*i)->durationEnvColumn);
            (*i)->m_TreeView.remove_column((*i)->durationEnvScaleColumn);
            //(*i)->weightEntry.set_text("");
        }
    }

    Gtk::Alignment* alignment;
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepAlignment", alignment);
    if (alignment->get_child() == NULL) {  // alignment is empty
        attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepVBox", vbox);
        alignment->add(*vbox);

        attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteAlignment", alignment);
        alignment->remove();
    }

    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
    entry->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
    entry->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
    entry->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);

    entry->set_sensitive(false);
    currentlyShownEvent->setChildEventDefAttackSieve(entry->get_text());
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);

    entry->set_sensitive(false);
    currentlyShownEvent->setChildEventDefDurationSieve(entry->get_text());
    // entry->set_text( ""   );

    attributesRefBuilder->get_widget("attributesStartTypePercentageButton", radioButton);
    radioButton->set_sensitive(true);
    // radioButton->set_active();

    attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
    radioButton->set_sensitive(true);
    attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
    radioButton->set_sensitive(true);

    attributesRefBuilder->get_widget("attributesDurationTypePercentageButton", radioButton);
    radioButton->set_sensitive(true);
    // radioButton->set_active();
    attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
    radioButton->set_sensitive(true);
    attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton", radioButton);
    radioButton->set_sensitive(true);
}

void EventAttributesViewController::discreteButtonClicked() {
    if (currentlyShownEvent == NULL) return;
    modified();
    Gtk::RadioButton* radioButton;
    attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);

    if (!radioButton->get_active()) {
        std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
        for (i; i != layerBoxesStorage.end(); i++) {
            (*i)->m_TreeView.append_column((*i)->weightColumn);
            (*i)->m_TreeView.append_column((*i)->attackEnvColumn);
            (*i)->m_TreeView.append_column((*i)->attackEnvScaleColumn);
            (*i)->m_TreeView.append_column((*i)->durationEnvColumn);
            (*i)->m_TreeView.append_column((*i)->durationEnvScaleColumn);

            //(*i)->weightEntry.set_text("");
        }
    }

    Gtk::Alignment* alignment;
    Gtk::VBox* vbox;

    attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteAlignment", alignment);
    if (alignment->get_child() == NULL) {  // alignment is empty
        attributesRefBuilder->get_widget("ChildrenEventsDefinitionDiscreteVBox", vbox);
        alignment->add(*vbox);

        attributesRefBuilder->get_widget("ChildEventsDefinitionContinuumSweepAlignment", alignment);
        alignment->remove();
    }

    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
    entry->set_sensitive(true);
    entry->set_text(currentlyShownEvent->getChildEventDefAttackSieve());

    attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);
    entry->set_sensitive(true);
    entry->set_text(currentlyShownEvent->getChildEventDefDurationSieve());

    attributesRefBuilder->get_widget("attributesStartTypePercentageButton", radioButton);
    radioButton->set_sensitive(false);
    attributesRefBuilder->get_widget("attributesStartTypeUnitsButton", radioButton);
    radioButton->set_sensitive(false);
    attributesRefBuilder->get_widget("attributesStartTypeSecondsButton", radioButton);
    radioButton->set_sensitive(false);

    attributesRefBuilder->get_widget("attributesDurationTypePercentageButton", radioButton);
    radioButton->set_sensitive(false);
    attributesRefBuilder->get_widget("attributesDurationTypeUnitsButton", radioButton);
    radioButton->set_sensitive(false);
    attributesRefBuilder->get_widget("attributesDurationTypeSecondsButton", radioButton);
    radioButton->set_sensitive(false);

    attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);
    if (radioButton->get_active() == false) {
        radioButton->set_active();
    }
}

void EventAttributesViewController::densityButtonClicked() {
    if (currentlyShownEvent == NULL) return;
    modified();
    currentlyShownEvent->setFlagNumChildren(1);
    Gtk::Label* label;
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel1", label);
    label->set_text("Density:");
    label->set_tooltip_text("FLOAT");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
    entry->set_sensitive(true);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel2", label);
    label->set_text("Area:");
    label->set_tooltip_text("INT");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
    entry->set_sensitive(true);
    entry->set_text("8");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel3", label);
    label->set_text("Under One:");
    label->set_tooltip_text("INT");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
    entry->set_sensitive(true);
    entry->set_text("4");

    // disable "bylayer entry in all layers
    std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
    for (i; i != layerBoxesStorage.end(); i++) {
        (*i)->weightEntry.set_sensitive(false);
        (*i)->weightEntry.set_text("");
    }
}

void EventAttributesViewController::fixedButtonClicked() {
    if (currentlyShownEvent == NULL) return;
    modified();

    currentlyShownEvent->setFlagNumChildren(0);
    Gtk::Label* label;
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel1", label);
    label->set_text("Number Of Children To Create (optional):");
    label->set_tooltip_text("INT, or leave it blank and let CMOD decide");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
    entry->set_sensitive(true);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel2", label);
    label->set_text("");
    label->set_tooltip_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel3", label);
    label->set_text("");
    label->set_tooltip_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    // disable "bylayer entry in all layers
    std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();

    for (i; i != layerBoxesStorage.end(); i++) {
        (*i)->weightEntry.set_sensitive(false);
        (*i)->weightEntry.set_text("");
    }
}

void EventAttributesViewController::byLayerButtonClicked() {
    if (currentlyShownEvent == NULL) return;
    modified();

    currentlyShownEvent->setFlagNumChildren(2);
    Gtk::Label* label;
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel1", label);
    label->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel2", label);
    label->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenLabel3", label);
    label->set_text("");

    attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
    entry->set_sensitive(false);
    // entry->set_text("");

    // enable and shows "bylayer entry in all layers
    std::vector<LayerBox*>::iterator i = layerBoxesStorage.begin();
    for (i; i != layerBoxesStorage.end(); i++) {
        (*i)->weightEntry.set_sensitive(true);
        (*i)->weightEntry.set_text((*i)->layerInEvent->getByLayer());
    }
}

IEvent* EventAttributesViewController::getCurrentEvent() { return currentlyShownEvent; }

EventAttributesViewController::LayerBox::~LayerBox() {}

EventAttributesViewController::LayerBox::LayerBox(EventAttributesViewController* _attributesView,
                                                  ProjectViewController* _projectView,
                                                  EventLayer* _childrenInThisLayer,
                                                  bool _flagShowDiscreteColumns) {
    weightHBox.set_tooltip_text("number of children to create in this layer");
    deleteLayerButton.set_tooltip_text("delete this layer");
    innerVBox.set_tooltip_text("Drag an event here from the Objects List");

    layerInEvent = _childrenInThisLayer;
    attributesView = _attributesView;
    projectView = _projectView;

    pack_start(m_ScrolledWindow2, Gtk::PACK_EXPAND_WIDGET);

    set_size_request(-1, layerInEvent->size() * 25 + 140);
    // innerVBox.set_size_request(140,85);

    set_border_width(10);
    // pack_start(m_TreeView, Gtk::PACK_EXPAND_WIDGET);

    weightLabel.set_text("Number of children in this layer: ");

    weightFunctionButton.set_label("Insert Function");
    deleteLayerButton.set_label("Delete This Layer");

    weightFunctionButton.signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::byLayerWeightButtonClicked));

    deleteLayerButton.signal_clicked().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::deleteLayerButtonClicked));

    weightEntry.set_text(_childrenInThisLayer->getByLayer());

    weightHBox.pack_start(weightLabel, Gtk::PACK_SHRINK);
    weightHBox.pack_start(weightEntry, Gtk::PACK_EXPAND_WIDGET);
    weightHBox.pack_start(weightFunctionButton, Gtk::PACK_SHRINK);
    weightHBox.pack_start(deleteLayerButton, Gtk::PACK_SHRINK);
    innerVBox.pack_start(weightHBox, Gtk::PACK_SHRINK);

    innerVBox.pack_start(m_TreeView);

    m_ScrolledWindow2.add(innerVBox);

    // Only show the scrollbars when they are necessary:
    m_ScrolledWindow2.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    // Create the Tree model:
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    m_TreeView.set_model(m_refTreeModel);

    std::list<EventDiscretePackage*>::iterator i = _childrenInThisLayer->children.begin();

    int index = 1;
    for (i; i != _childrenInThisLayer->children.end(); i++) {
        Gtk::TreeModel::Row row = *(m_refTreeModel->append());
        row[m_Columns.columnObjectChildTypeIndex] = index++;
        row[m_Columns.columnObjectType] = (*i)->event->getEventTypeString();
        row[m_Columns.columnObjectName] = (*i)->event->getEventName();
        row[m_Columns.columnObjectWeight] = (*i)->weight;
        row[m_Columns.columnObjectAttackEnv] = (*i)->attackEnv;
        row[m_Columns.columnObjectAttackEnvScale] = (*i)->attackEnvScale;
        row[m_Columns.columnObjectDurationEnv] = (*i)->durationEnv;
        row[m_Columns.columnObjectDurationEnvScale] = (*i)->durationEnvScale;
        row[m_Columns.columnEntry] = (*i);
    }

    /////////////////////set up popup menu ///////////////////////////////

    m_pMenuPopup = 0;
    //    the signal handler i wished to put was overriden by the object default
    // signal handler, so to avoid this, we have to add a paramater to the
    // connect method (false)

    m_TreeView.signal_button_press_event().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::onRightClick), false);

    // Fill menu:

    m_refActionGroup = Gtk::ActionGroup::create();

    // File|New sub menu:
    // These menu actions would normally already exist for a main menu, because
    // a context menu should not normally contain menu items that are only
    // available via a context menu.
    m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));

    // m_refActionGroup->add(
    // Gtk::Action::create("ContextAdd", "Add To Project Tree"),
    // sigc::mem_fun(*this, &PaletteViewController::AddToProjectTree));
    m_refActionGroup->add(
        Gtk::Action::create("ContextDelete", "Delete"),
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::deleteObject));

    // TODO:
    /*
     //Add a ImageMenuElem:
     menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem("_Something", m_Image,
     sigc::mem_fun(*this, &ExampleWindow::on_menu_file_popup_generic) ) ) ;
     */

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);

    // projectView->add_accel_group(m_refUIManager->get_accel_group());

    // Layout the actions in a menubar and toolbar:
    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='PopupMenu'>"
        "    <menuitem action='ContextDelete'/>"
        "  </popup>"
        "</ui>";

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        m_refUIManager->add_ui_from_string(ui_info);
    } catch (const Glib::Error& ex) {
        std::cerr << "building menus failed: " << ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if (ex.get()) {
        std::cerr << "building menus failed: " << ex->what();
    }
#endif  // GLIBMM_EXCEPTIONS_ENABLED

    // Get the menu:
    m_pMenuPopup = dynamic_cast<Gtk::Menu*>(m_refUIManager->get_widget("/PopupMenu"));

    if (!m_pMenuPopup) g_warning("menu not found");

    //}

    // Fill the TreeView's model
    m_TreeView.append_column("Child Type", m_Columns.columnObjectChildTypeIndex);
    m_TreeView.append_column("Class", m_Columns.columnObjectType);
    m_TreeView.append_column("Name", m_Columns.columnObjectName);

    //================================================================
    // editable modification
    // m_TreeView.append_column_editable("Weight",
    // m_Columns.columnObjectWeight);

    weightColumn.set_title("Weight");
    weightColumn.pack_start(weightText);

    attackEnvColumn.set_title("Attack Envelope");
    attackEnvColumn.pack_start(attackEnvText);

    attackEnvScaleColumn.set_title("A. Env. scaler");
    attackEnvScaleColumn.pack_start(attackEnvScaleText);

    durationEnvColumn.set_title("Duration Envelope");
    durationEnvColumn.pack_start(durationEnvText);

    durationEnvScaleColumn.set_title("D. Env. scaler");
    durationEnvScaleColumn.pack_start(durationEnvScaleText);

    if (_flagShowDiscreteColumns) {
        m_TreeView.append_column(weightColumn);
        m_TreeView.append_column(attackEnvColumn);
        m_TreeView.append_column(attackEnvScaleColumn);
        m_TreeView.append_column(durationEnvColumn);
        m_TreeView.append_column(durationEnvScaleColumn);
    }

    // Tell the view column how to render the model values:
    weightColumn.set_cell_data_func(
        weightText, sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::renderWeight));

    attackEnvColumn.set_cell_data_func(
        attackEnvText,
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::renderAttackEnv));

    attackEnvScaleColumn.set_cell_data_func(
        attackEnvScaleText,
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::renderAttackEnvScale));

    durationEnvColumn.set_cell_data_func(
        durationEnvText,
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::renderDurationEnv));

    durationEnvScaleColumn.set_cell_data_func(
        durationEnvScaleText,
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::renderDurationEnvScale));

    // Make the CellRenderer editable, and handle its editing signals:
#ifdef GLIBMM_PROPERTIES_ENABLED
    weightText.property_editable() = true;
    attackEnvText.property_editable() = true;
    attackEnvScaleText.property_editable() = true;
    durationEnvText.property_editable() = true;
    durationEnvScaleText.property_editable() = true;

#else
    weightText.set_property("editable", true);
    attackEnvText.set_property("editable", true);
    attackEnvScaleText.set_property("editable", true);
    durationEnvText.set_property("editable", true);
    durationEnvScaleText.set_property("editable", true);
#endif

    weightText.signal_edited().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::saveWeight));

    attackEnvText.signal_edited().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::saveAttackEnv));

    attackEnvScaleText.signal_edited().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::saveAttackEnvScale));

    durationEnvText.signal_edited().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::saveDurationEnv));

    durationEnvScaleText.signal_edited().connect(
        sigc::mem_fun(*this, &EventAttributesViewController::LayerBox::saveDurationEnvScale));

    // If this was a CellRendererSpin then you would have to set the adjustment:
    // weightText.property_adjustment() = &m_spin_adjustment;

    //=============================================================================
    // Make m_Label_Drop a DnD drop destination:

    // using enable_model_drag_dest insted of drag_dest_set here!!
    drag_dest_set(_projectView->listTargets);
    // Connect signals:
    signal_drag_data_received().connect(sigc::mem_fun(
        *this, &EventAttributesViewController::LayerBox::on_label_drop_drag_data_received));
    show_all_children();
}

void EventAttributesViewController::addNewLayerButtonClicked() {
    if (currentlyShownEvent == NULL || currentlyShownEvent->getEventType() >= 5) {
        // if not a legal event, return;
        return;
    }

    projectView->modified();

    Gtk::VBox* layerBoxes;
    attributesRefBuilder->get_widget("layerBoxes", layerBoxes);

    Gtk::RadioButton* radioButton;
    attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);
    bool showDiscretePackage = radioButton->get_active();

    LayerBox* newbox =
        new LayerBox(this, projectView, currentlyShownEvent->addLayer(), showDiscretePackage);

    layerBoxes->pack_start((Gtk::Widget&)*newbox, Gtk::PACK_SHRINK);

    layerBoxesStorage.push_back(newbox);
    int numOfChildrenFlag = currentlyShownEvent->getFlagNumChildren();

    if (numOfChildrenFlag != 2) {  // not bylayer
        newbox->weightEntry.set_sensitive(false);
    }

    // so that the right click of delete event works
    showAttributesOfEvent(currentlyShownEvent);

    Gtk::Adjustment* adjustment = scrolledWindow.get_vadjustment();
    // 200 is the height of a layer box
    adjustment->set_value(adjustment->get_value() + 200);
}

void EventAttributesViewController::LayerBox::renderWeight(Gtk::CellRenderer* renderer,
                                                           const Gtk::TreeModel::iterator& iter) {
    // Get the value from the model and show it appropriately in the view:
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring model_value = row[m_Columns.columnObjectWeight];

        // This is just an example.
        // In this case, it would be easier to use append_column_editable() or
        // append_column_numeric_editable()
        // char buffer[32];
        // sprintf(buffer, "%d", model_value);

        // Glib::ustring view_text = buffer;
#ifdef GLIBMM_PROPERTIES_ENABLED
        // m_cellrenderer_validated.property_text() = view_text;
        weightText.property_text() = model_value;
#else
        // m_cellrenderer_validated.set_property("text", view_text);
        weightText.set_property("text", model_value);
#endif
    }
}

void EventAttributesViewController::LayerBox::renderAttackEnv(
    // if modified needed, refer to the comments in renderweight
    Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
w:
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring model_value = row[m_Columns.columnObjectAttackEnv];

#ifdef GLIBMM_PROPERTIES_ENABLED
        attackEnvText.property_text() = model_value;
#else
        attackEnvText.set_property("text", model_value);
#endif
    }
}

void EventAttributesViewController::LayerBox::renderAttackEnvScale(
    // if modified needed, refer to the comments in renderweight
    Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
w:
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring model_value = row[m_Columns.columnObjectAttackEnvScale];

#ifdef GLIBMM_PROPERTIES_ENABLED
        attackEnvScaleText.property_text() = model_value;
#else
        attackEnvScaleText.set_property("text", model_value);
#endif
    }
}

void EventAttributesViewController::LayerBox::renderDurationEnv(
    // if modified needed, refer to the comments in renderweight
    Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
w:
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring model_value = row[m_Columns.columnObjectDurationEnv];

#ifdef GLIBMM_PROPERTIES_ENABLED
        durationEnvText.property_text() = model_value;
#else
        durationEnvText.set_property("text", model_value);
#endif
    }
}

void EventAttributesViewController::LayerBox::renderDurationEnvScale(
    // if modified needed, refer to the comments in renderweight
    Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter) {
w:
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring model_value = row[m_Columns.columnObjectDurationEnvScale];

#ifdef GLIBMM_PROPERTIES_ENABLED
        durationEnvScaleText.property_text() = model_value;
#else
        durationEnvScaleText.set_property("text", model_value);
#endif
    }
}

void EventAttributesViewController::LayerBox::saveWeight(const Glib::ustring& path_string,
                                                         const Glib::ustring& new_text) {
    Gtk::TreePath path(path_string);

    // Convert the inputed text to an integer, as needed by our model column:

    // Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Put the new value in the model:
        row[m_Columns.columnObjectWeight] = new_text;

        EventDiscretePackage* discretePackage = row[m_Columns.columnEntry];
        discretePackage->weight = new_text;
    }
}

void EventAttributesViewController::LayerBox::saveAttackEnv(const Glib::ustring& path_string,
                                                            const Glib::ustring& new_text) {
    Gtk::TreePath path(path_string);

    // Convert the inputed text to an integer, as needed by our model column:

    // Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Put the new value in the model:
        row[m_Columns.columnObjectAttackEnv] = new_text;

        EventDiscretePackage* discretePackage = row[m_Columns.columnEntry];
        discretePackage->attackEnv = new_text;
    }
}

void EventAttributesViewController::LayerBox::saveAttackEnvScale(const Glib::ustring& path_string,
                                                                 const Glib::ustring& new_text) {
    Gtk::TreePath path(path_string);

    // Convert the inputed text to an integer, as needed by our model column:

    // Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Put the new value in the model:
        row[m_Columns.columnObjectAttackEnvScale] = new_text;

        EventDiscretePackage* discretePackage = row[m_Columns.columnEntry];
        discretePackage->attackEnvScale = new_text;
    }
}

void EventAttributesViewController::LayerBox::saveDurationEnv(const Glib::ustring& path_string,
                                                              const Glib::ustring& new_text) {
    Gtk::TreePath path(path_string);

    // Convert the inputed text to an integer, as needed by our model column:

    // Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Put the new value in the model:
        row[m_Columns.columnObjectDurationEnv] = new_text;

        EventDiscretePackage* discretePackage = row[m_Columns.columnEntry];
        discretePackage->durationEnv = new_text;
    }
}

void EventAttributesViewController::LayerBox::saveDurationEnvScale(const Glib::ustring& path_string,
                                                                   const Glib::ustring& new_text) {
    Gtk::TreePath path(path_string);

    // Convert the inputed text to an integer, as needed by our model column:

    // Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;

        // Put the new value in the model:
        row[m_Columns.columnObjectDurationEnvScale] = new_text;

        EventDiscretePackage* discretePackage = row[m_Columns.columnEntry];
        discretePackage->durationEnvScale = new_text;
    }
}

void EventAttributesViewController::freqFundamentalButtonClicked() {
    modified();
    Gtk::Alignment* allignment;
    attributesRefBuilder->get_widget("BottomSubAttributesFrequencyAllignment", allignment);
    Gtk::VBox* fundamentalVBox;
    attributesRefBuilder->get_widget("BottomSubAttributesFundamentalVBox", fundamentalVBox);
    allignment->remove();
    allignment->add(*fundamentalVBox);
}

void EventAttributesViewController::freqContinuumButtonClicked() {
    modified();
    Gtk::Alignment* allignment;
    attributesRefBuilder->get_widget("BottomSubAttributesFrequencyAllignment", allignment);
    Gtk::HBox* continuumHBox;
    attributesRefBuilder->get_widget("BottomSubAttributesContinuumHBox", continuumHBox);
    allignment->remove();
    allignment->add(*continuumHBox);
}

void EventAttributesViewController::freqWellTemperedButtonClicked() {
    modified();
    Gtk::Alignment* allignment;
    attributesRefBuilder->get_widget("BottomSubAttributesFrequencyAllignment", allignment);
    Gtk::HBox* wellTemperedVBox;
    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedHBox", wellTemperedVBox);
    allignment->remove();
    allignment->add(*wellTemperedVBox);
}

BottomEventModifierAlignment::BottomEventModifierAlignment(
    EventBottomModifier* _modifier, EventAttributesViewController* _attributesView) {
    modifier = _modifier;
    attributesView = _attributesView;
    prev = NULL;
    next = NULL;
    Gtk::VBox* vbox;
    Gtk::ComboBox* combobox;

    // Load the GtkBuilder files and instantiate their widgets:
    attributesRefBuilder = Gtk::Builder::create();

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        attributesRefBuilder->add_from_file("./LASSIE/src/UI/Modifier.ui");
    } catch (const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
    } catch (const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
    }

#else
    std::auto_ptr<Glib::Error> error;
    if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/Modifier.ui", error)) {
        std::cerr << error->what() << std::endl;
    }

#endif /* !GLIBMM_EXCEPTIONS_ENABLED */

    // Get the GtkBuilder

    attributesRefBuilder->get_widget("MainVBox", vbox);

    add(*vbox);

    // set up combox for type and applyhow

    attributesRefBuilder->get_widget("applyHowCombobox", combobox);

    applyHowTreeModel = Gtk::ListStore::create(applyHowColumns);
    combobox->set_model(applyHowTreeModel);

    Gtk::TreeModel::Row row = *(applyHowTreeModel->append());
    row[applyHowColumns.m_col_id] = 0;
    row[applyHowColumns.m_col_name] = "SOUND";

    row = *(applyHowTreeModel->append());
    row[applyHowColumns.m_col_id] = 1;
    row[applyHowColumns.m_col_name] = "PARTIAL";

    combobox->pack_start(applyHowColumns.m_col_name);

    combobox->signal_changed().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::on_applyHow_combo_changed));

    combobox->set_active(modifier->getApplyHowFlag());

    attributesRefBuilder->get_widget("typeCombobox", combobox);

    typeTreeModel = Gtk::ListStore::create(typeColumns);
    combobox->set_model(typeTreeModel);

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierTremolo;
    row[typeColumns.m_col_name] = "TREMOLO";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierVibrato;
    row[typeColumns.m_col_name] = "VIBRATO";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierGlissando;
    row[typeColumns.m_col_name] = "GLISSANDO";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierBend;
    row[typeColumns.m_col_name] = "BEND";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierDetune;
    row[typeColumns.m_col_name] = "DETUNE";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierAmptrans;
    row[typeColumns.m_col_name] = "AMPTRANS";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierFreqtrans;
    row[typeColumns.m_col_name] = "FREQTRANS";

    row = *(typeTreeModel->append());
    row[typeColumns.m_col_type] = modifierWave_type;
    row[typeColumns.m_col_name] = "WAVE_TYPE";

    combobox->set_active(modifier->getModifierTypeInt());

    combobox->pack_start(typeColumns.m_col_name);

    combobox->signal_changed().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::on_type_combo_changed));

    // reflect modifier data

    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("groupNameEntry", entry);
    entry->set_text(modifier->getGroupName());

    attributesRefBuilder->get_widget("probablityEnvelopeEntry", entry);
    entry->set_text(modifier->getProbability());

    attributesRefBuilder->get_widget("ampValueEnvelopeEntry", entry);
    entry->set_text(modifier->getAmpValue());

    attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
    entry->set_text(modifier->getRateValue());

    attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
    entry->set_text(modifier->getWidth());

    ModifierType type = modifier->getModifierType();
    if (type == modifierAmptrans || type == modifierFreqtrans) {
        attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
        entry->set_sensitive(true);
        attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
        entry->set_sensitive(true);

    } else if (type == modifierTremolo || type == modifierVibrato) {
        attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
        entry->set_sensitive(true);
        attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
        entry->set_sensitive(false);
    } else {
        attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
        entry->set_sensitive(false);
        attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
        entry->set_sensitive(false);
    }

    Gtk::Button* button;
    attributesRefBuilder->get_widget("removeModifierButton", button);

    button->signal_clicked().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::removeModifierButtonClicked));

    attributesRefBuilder->get_widget("probablityEnvelopeButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::probablityEnvelopeButtonClicked));

    attributesRefBuilder->get_widget("ampValueEnvelopeButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::ampValueEnvelopeButtonClicked));

    attributesRefBuilder->get_widget("rateValueEnvelopeButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::rateValueEnvelopeButtonClicked));

    attributesRefBuilder->get_widget("widthEnvelopeButton", button);
    button->signal_clicked().connect(
        sigc::mem_fun(*this, &BottomEventModifierAlignment::widthEnvelopeButtonClicked));

    attributesRefBuilder->get_widget("groupNameEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &BottomEventModifierAlignment::modified));

    attributesRefBuilder->get_widget("probablityEnvelopeEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &BottomEventModifierAlignment::modified));

    attributesRefBuilder->get_widget("ampValueEnvelopeEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &BottomEventModifierAlignment::modified));

    attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &BottomEventModifierAlignment::modified));

    attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(*this, &BottomEventModifierAlignment::modified));

    show_all_children();
}

void BottomEventModifierAlignment::modified() { attributesView->modified(); }

BottomEventModifierAlignment::~BottomEventModifierAlignment() {}

void BottomEventModifierAlignment::on_applyHow_combo_changed() {
    attributesView->modified();
    Gtk::ComboBox* combobox;
    attributesRefBuilder->get_widget("applyHowCombobox", combobox);

    Gtk::TreeModel::iterator iter = combobox->get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            modifier->setApplyHowFlag(row[applyHowColumns.m_col_id]);
        }
    }
}

void BottomEventModifierAlignment::on_type_combo_changed() {
    attributesView->modified();
    Gtk::ComboBox* combobox;
    attributesRefBuilder->get_widget("typeCombobox", combobox);

    Gtk::TreeModel::iterator iter = combobox->get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            // Get the data for the selected row, using our knowledge of the
            // tree model:

            ModifierType type = row[typeColumns.m_col_type];
            modifier->setModifierType(type);
            Gtk::Entry* entry;
            if (type == modifierAmptrans || type == modifierFreqtrans) {
                attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
                entry->set_sensitive(true);
                attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
                entry->set_sensitive(true);

            } else if (type == modifierTremolo || type == modifierVibrato) {
                attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
                entry->set_sensitive(true);
                attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
                entry->set_sensitive(false);

            } else {
                attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
                entry->set_sensitive(false);
                attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
                entry->set_sensitive(false);
            }
        }

    }  // end if iter
}

void EventAttributesViewController::addModifierButtonClicked() {
    if (entryChangedByShowCurrentEvent == false) {
        modified();
    }
    EventBottomModifier* newModifier;
    if (currentlyShownEvent->getEventType() == eventBottom) {
        newModifier = currentlyShownEvent->getEventExtraInfo()->addModifier();
    } else {
        newModifier = currentlyShownEvent->addModifier();
    }

    BottomEventModifierAlignment* newModifierAlignment =
        new BottomEventModifierAlignment(newModifier, this);
    if (modifiers == NULL) {
        newModifierAlignment->prev = NULL;
        modifiers = newModifierAlignment;
    } else {  // modifiers exist
        BottomEventModifierAlignment* temp = modifiers;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        newModifierAlignment->prev = temp;
        temp->next = newModifierAlignment;
    }
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("BottomSubAttributesModifiersVBox", vbox);
    vbox->pack_start(*newModifierAlignment, Gtk::PACK_SHRINK);

    show_all_children();

    Gtk::Adjustment* adjustment = scrolledWindow.get_vadjustment();
    adjustment->set_value(adjustment->get_upper());
}

void BottomEventModifierAlignment::saveToEvent() {
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("groupNameEntry", entry);
    modifier->setGroupName(entry->get_text());

    attributesRefBuilder->get_widget("probablityEnvelopeEntry", entry);
    modifier->setProbability(entry->get_text());

    attributesRefBuilder->get_widget("ampValueEnvelopeEntry", entry);
    modifier->setAmpValue(entry->get_text());

    ModifierType type = modifier->getModifierType();

    if (type == modifierTremolo || type == modifierVibrato) {
        attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
        modifier->setRateValue(entry->get_text());

        attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
        modifier->setWidth("");

    } else if (type == modifierAmptrans || type == modifierFreqtrans) {
        attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
        modifier->setRateValue(entry->get_text());

        attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
        modifier->setWidth(entry->get_text());
    }

    else {
        attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);
        modifier->setRateValue("");

        attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);
        modifier->setWidth("");
    }

    if (next != NULL) {
        next->saveToEvent();
    }
}

void BottomEventModifierAlignment::removeModifierButtonClicked() {
    attributesView->modified();
    attributesView->removeModifierAlignment(this);
}

void EventAttributesViewController::removeModifierAlignment(
    BottomEventModifierAlignment* _alignment) {
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("BottomSubAttributesModifiersVBox", vbox);
    vbox->remove(*_alignment);

    if (currentlyShownEvent->getEventType() == eventBottom) {
        currentlyShownEvent->getEventExtraInfo()->removeModifier(_alignment->modifier);
    } else {
        currentlyShownEvent->removeModifier(_alignment->modifier);
    }

    BottomEventModifierAlignment* search = modifiers;
    if (modifiers == _alignment) {
        modifiers = modifiers->next;
        if (modifiers != NULL) {
            modifiers->prev = NULL;
        }
    } else {
        _alignment->prev->next = _alignment->next;
        if (_alignment->next != NULL) {
            _alignment->next->prev = _alignment->prev;
        }
    }

    delete _alignment;

    show_all_children();
}

void EventAttributesViewController::maxChildDurFunButtonClicked() {
    insertFunctionString(maxChildDurFunButton);
}

void EventAttributesViewController::unitsPerSecondFunButtonClicked() {
    insertFunctionString(unitsPerSecondFunButton);
}

void EventAttributesViewController::numOfChildFunButton1Clicked() {
    insertFunctionString(numOfChildFunButton1);
}
void EventAttributesViewController::numOfChildFunButton2Clicked() {
    insertFunctionString(numOfChildFunButton2);
}
void EventAttributesViewController::numOfChildFunButton3Clicked() {
    insertFunctionString(numOfChildFunButton3);
}
void EventAttributesViewController::childEventDefStartTimeFunButtonClicked() {
    insertFunctionString(childEventDefStartTimeFunButton);
}
void EventAttributesViewController::childEventDefChildTypeFunButtonClicked() {
    insertFunctionString(childEventDefChildTypeFunButton);
}
void EventAttributesViewController::childEventDefDurationFunButtonClicked() {
    insertFunctionString(childEventDefDurationFunButton);
}
void EventAttributesViewController::childEventAttackSieveButtonClicked() {
    insertFunctionString(childEventDefAttackSieveFunButton);
}
void EventAttributesViewController::childEventDurationSieveButtonClicked() {
    insertFunctionString(childEventDefDurationSieveFunButton);
}

void EventAttributesViewController::deviationFunButtonClicked() {
    insertFunctionString(spectrumDeviationFunButton);
}

void EventAttributesViewController::generatespectrumFunButtonClicked() {
    insertFunctionString(spectrumGenerateFunButton);
}

void EventAttributesViewController::insertFunctionString(FunctionButton _button) {
    Gtk::Entry* entry;
    FunctionGenerator* generator = NULL;

    if (_button == maxChildDurFunButton) {
        attributesRefBuilder->get_widget("attributesStandardMaxChildDurEntry", entry);

        generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
    } else if (_button == unitsPerSecondFunButton) {
        attributesRefBuilder->get_widget("attributesStandardUnitsPerSecondEntry", entry);
        generator = new FunctionGenerator(functionReturnInt, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
    }

    else if (_button == numOfChildFunButton1) {
        Gtk::RadioButton* radioButton1;
        Gtk::RadioButton* radioButton2;
        attributesRefBuilder->get_widget("attributesStandardNumChildrenFixedButton", radioButton1);
        attributesRefBuilder->get_widget("attributesStandardNumChildrenDensityButton",
                                         radioButton2);

        if (radioButton1->get_active()) {
            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
            generator = new FunctionGenerator(functionReturnInt, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        } else if (radioButton2->get_active()) {
            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry1", entry);
            generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        } else {  // by layer.
        }

    } else if (_button == numOfChildFunButton2) {
        Gtk::RadioButton* radioButton;
        attributesRefBuilder->get_widget("attributesStandardNumChildrenDensityButton", radioButton);

        if (radioButton->get_active()) {
            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry2", entry);
            generator = new FunctionGenerator(functionReturnInt, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        }
    } else if (_button == numOfChildFunButton3) {
        Gtk::RadioButton* radioButton;
        attributesRefBuilder->get_widget("attributesStandardNumChildrenDensityButton", radioButton);

        if (radioButton->get_active()) {
            attributesRefBuilder->get_widget("attributesStandardNumChildrenEntry3", entry);
            generator = new FunctionGenerator(functionReturnInt, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        }
    } else if (_button == childEventDefStartTimeFunButton) {
        Gtk::RadioButton* radioButton;
        attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);
        if (!radioButton->get_active()) {  // if sweep or continuu
            attributesRefBuilder->get_widget("attributesChildEventDefEntry1", entry);
            generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        }
    } else if (_button == childEventDefChildTypeFunButton) {
        Gtk::RadioButton* radioButton;
        attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);
        if (!radioButton->get_active()) {  // if sweep or continuum
            attributesRefBuilder->get_widget("attributesChildEventDefEntry2", entry);
            generator = new FunctionGenerator(functionReturnInt, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        }
    } else if (_button == childEventDefDurationFunButton) {
        Gtk::RadioButton* radioButton;
        attributesRefBuilder->get_widget("attributesChildEventDefDiscreteButton", radioButton);
        if (!radioButton->get_active()) {  // if sweep or continuum
            attributesRefBuilder->get_widget("attributesChildEventDefEntry3", entry);
            generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
            int result = generator->run();
            if (generator->getResultString() != "" && result == 0) {
                entry->set_text(generator->getResultString());
            }
        }
    } else if (_button == childEventDefAttackSieveFunButton) {
        attributesRefBuilder->get_widget("attributesChildEventDefAttackSieveEntry", entry);
        generator = new FunctionGenerator(functionReturnSIV, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
    } else if (_button == childEventDefDurationSieveFunButton) {
        attributesRefBuilder->get_widget("attributesChildEventDefDurationSieveEntry", entry);
        generator = new FunctionGenerator(functionReturnSIV, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
    } else if (_button == spectrumDeviationFunButton) {
        attributesRefBuilderSound->get_widget("SoundAttributesDeviationEntry", entry);
        generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }

    }

    else if (_button == spectrumGenerateFunButton) {
        attributesRefBuilderSound->get_widget("SoundAttributesGenerateSpectrumEntry", entry);
        generator = new FunctionGenerator(functionReturnSPE, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
    }

    if (generator != NULL) {
        delete generator;
    }
}

void EventAttributesViewController::LayerBox::byLayerWeightButtonClicked() {
    if (weightEntry.get_sensitive()) {
        FunctionGenerator* generator =
            new FunctionGenerator(functionReturnFloat, weightEntry.get_text());
        std::cout << "NOT sure if byLayer is int or float. ask sever" << std::endl;
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            weightEntry.set_text(generator->getResultString());
        }
        delete generator;
    } else
        return;
}

void EventAttributesViewController::LayerBox::deleteLayerButtonClicked() {
    bool result = attributesView->deleteLayer(this);
    if (result) {
        layerInEvent->deleteLayer();  //*  crashed by this line
        projectView->modified();
    }
    attributesView->refreshChildTypeInLayer();
}

void EventAttributesViewController::attributesStandardRevButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesStandardRevEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnREV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::attributesStandardFilButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesStandardFilEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnFIL, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::attributesStandardSpaButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesStandardSpaEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnSPA, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::BSLoudnessButtonClicked() {
    Gtk::Entry* entry;

    attributesRefBuilder->get_widget("BottomSubAttributesLoudnessEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnInt, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::BSSpatializationButtonClicked() {
    if (currentlyShownEvent->getEventExtraInfo()->getChildTypeFlag() != 0) {
        return;
    }

    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesSpatializationEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnSPA, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}
void EventAttributesViewController::BSReverbButtonClicked() {
    if (currentlyShownEvent->getEventExtraInfo()->getChildTypeFlag() != 0) {
        return;
    }
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesReverbEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnREV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::BSFilterButtonClicked() {
    if (currentlyShownEvent->getEventExtraInfo()->getChildTypeFlag() != 0) {
        return;
    }
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesFilterEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnFIL, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::BSWellTemperedButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesWellTemperedEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnInt, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}
void EventAttributesViewController::BSFunFreqButton1Clicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry1", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}
void EventAttributesViewController::BSFunFreqButton2Clicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesFunFreqEntry2", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnInt, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}
void EventAttributesViewController::BSContinuumButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("BottomSubAttributesContinuumEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnFloat, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void BottomEventModifierAlignment::probablityEnvelopeButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("probablityEnvelopeEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnENV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}
void BottomEventModifierAlignment::ampValueEnvelopeButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("ampValueEnvelopeEntry", entry);
    FunctionGenerator* generator = new FunctionGenerator(functionReturnENV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void BottomEventModifierAlignment::rateValueEnvelopeButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("rateValueEnvelopeEntry", entry);

    if (entry->get_sensitive()) {
        FunctionGenerator* generator = new FunctionGenerator(functionReturnENV, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
        delete generator;
    }
}
void BottomEventModifierAlignment::widthEnvelopeButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("widthEnvelopeEntry", entry);

    if (entry->get_sensitive()) {
        FunctionGenerator* generator = new FunctionGenerator(functionReturnENV, entry->get_text());
        int result = generator->run();
        if (generator->getResultString() != "" && result == 0) {
            entry->set_text(generator->getResultString());
        }
        delete generator;
    }
}

void EventAttributesViewController::envelopeFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("envelopeAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnENV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::sieveFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("SieveAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnSIV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::patternFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("PatternAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnPAT, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::reverbFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("ReverbAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnREV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::filterFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("FilterAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnFIL, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::measureFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("MeasureAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnFIL, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::spatializationFunButtonClicked() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("SpatializationAttributesBuilderEntry", entry);

    FunctionGenerator* generator = new FunctionGenerator(functionReturnSPA, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        entry->set_text(generator->getResultString());
    }
    delete generator;
}

void EventAttributesViewController::tempoAsNoteValueButtonClicked() {
    if (currentlyShownEvent == NULL) {
        return;
    }
    modified();

    Gtk::Entry* entry;
    Gtk::Label* label;

    attributesRefBuilder->get_widget("attributesStandardTempoMidLabel", label);

    label->set_text("=");

    attributesRefBuilder->get_widget("attributesStandardTempoRightLabel", label);

    label->set_text("");

    Gtk::Alignment* alignment;
    attributesRefBuilder->get_widget("attributesStandardTempoSecondaryAlignment", alignment);
    alignment->remove();

    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);

    originalTempoValueForFraction = entry->get_text();

    if (tempoEntryAsNoteValueModifiedFlag == false) {
        entry->set_text(originalTempoValueForNoteValue);
    } else {
        tempo->setTempo(generateTempoStringByFraction());

        Ratio ratio = tempo->getTempoBeatsPerMinute();

        char charbuffer[20];
        sprintf(charbuffer, "%d", ratio.Num());

        string stringbuffer = string(charbuffer);

        if (ratio.Den() != 1) {
            sprintf(charbuffer, "%d", ratio.Den());

            stringbuffer = stringbuffer + "/" + string(charbuffer);
        }

        entry->set_text(stringbuffer);
    }
    tempoEntryAsNoteValueModifiedFlag = false;
}

void EventAttributesViewController::tempoAsFractionButtonClicked() {
    if (currentlyShownEvent == NULL) {
        return;
    }

    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
    originalTempoValueForNoteValue = entry->get_text();

    modified();
    Gtk::Alignment* alignment;
    attributesRefBuilder->get_widget("attributesStandardTempoSecondaryAlignment", alignment);
    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);
    alignment->add(*entry);

    Gtk::Label* label;
    attributesRefBuilder->get_widget("attributesStandardTempoMidLabel", label);

    label->set_text(" in ");

    attributesRefBuilder->get_widget("attributesStandardTempoRightLabel", label);

    label->set_text(" second(s)");

    if (tempoEntryAsNoteValueModifiedFlag == false) {
        attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
        entry->set_text(originalTempoValueForFraction);
    } else {  // modified

        // check if fraction

        attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);
        string originalValue = entry->get_text();

        size_t foundSlash = originalValue.find("/");
        string entry1;
        int value;
        char charBuffer[20];
        string ratioString;
        Ratio ratio;
        if (foundSlash != string::npos) {  // it is fraction
            entry1 = originalValue.substr(0, foundSlash);

            value =
                atoi((originalValue.substr(foundSlash + 1, originalValue.length())).c_str()) * 60;
            sprintf(charBuffer, "%d", value);
            ratioString = entry1 + "/" + charBuffer;
        } else {
            ratioString = originalValue + "/60";
        }

        ratio = Ratio(ratioString);

        attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);

        sprintf(charBuffer, "%d", ratio.Num());

        entry->set_text(string(charBuffer));

        attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);

        sprintf(charBuffer, "%d", ratio.Den());
        entry->set_text(string(charBuffer));
    }

    tempoEntryAsNoteValueModifiedFlag = false;
}

void EventAttributesViewController::deleteKeyPressed(Gtk::Widget* _focus) {
    if (currentlyShownEvent == NULL || layerBoxesStorage.size() == 0) return;
    cout << "not returned!" << endl;
    vector<LayerBox*>::iterator layerBoxesIter = layerBoxesStorage.begin();

    while (layerBoxesIter != layerBoxesStorage.end()) {
        if (_focus->is_ancestor((Gtk::Widget&)**layerBoxesIter)) {
            (*layerBoxesIter)->deleteObject();

            break;
        }

        layerBoxesIter++;
    }
}

bool EventAttributesViewController::LayerBox::onRightClick(GdkEventButton* event) {
    // This chunk of code change the cursor to the object rightclicked on.
    int x;
    int y;
    Gtk::TreeModel::Path path;
    Gtk::TreeViewColumn* temp_column;

    m_TreeView.get_path_at_pos((int)event->x, (int)event->y, path, temp_column, x, y);
    m_TreeView.set_cursor(path);

    if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {  // test if right click
        if (m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);

        return true;  // It has been handled.
    } else {
        return false;
    }
}

void EventAttributesViewController::LayerBox::deleteObject() {
    Gtk::TreeModel::Children::iterator iter = m_TreeView.get_selection()->get_selected();
    Gtk::TreeModel::Row row = *iter;

    if (iter) {
        EventDiscretePackage* removedEventPackage = row[m_Columns.columnEntry];
        IEvent* removedEvent = removedEventPackage->event;
        // remove removeEvent's parent
        if (removedEvent->removeParent(attributesView->getCurrentlyShownEvent()) != true) {
            cout << "remove parent fail" << endl;
        }

        // remove removedEvent from parent's eventlayer

        if (!layerInEvent->removeChild(removedEventPackage)) {  // this call will delete the package
                                                                // so don't have to do it here
            cout << "remove from parent fail" << endl;
        }

        // erase row from view
        m_refTreeModel->erase(*iter);
    }
    set_size_request(-1, layerInEvent->size() * 25 + 140);
    attributesView->refreshChildTypeInLayer();
    projectView->modified();
}

IEvent* EventAttributesViewController::getCurrentlyShownEvent() { return currentlyShownEvent; }

bool EventAttributesViewController::deleteLayer(LayerBox* _deleteBox) {
    if (layerBoxesStorage.size() == 1) {
        return false;
    }
    Gtk::VBox* layerBoxes;
    attributesRefBuilder->get_widget("layerBoxes", layerBoxes);

    std::vector<LayerBox*>::iterator layerBoxesIter = layerBoxesStorage.begin();

    for (layerBoxesIter; layerBoxesIter != layerBoxesStorage.end(); layerBoxesIter++) {
        layerBoxes->remove(**layerBoxesIter);
    }

    layerBoxesIter = layerBoxesStorage.begin();

    while (_deleteBox != *layerBoxesIter) {
        layerBoxesIter++;
    }

    layerBoxesStorage.erase(layerBoxesIter);
    delete _deleteBox;
    // restore layerboxes
    for (layerBoxesIter = layerBoxesStorage.begin(); layerBoxesIter != layerBoxesStorage.end();
         layerBoxesIter++) {
        layerBoxes->pack_start(**layerBoxesIter, Gtk::PACK_SHRINK);
    }
    return true;
}

void EventAttributesViewController::addPartialButtonClicked() {
    currentlyShownEvent->setChangedButNotSaved(true);

    SoundPartialHBox* newBox =
        new SoundPartialHBox(currentlyShownEvent->getEventExtraInfo()->addPartial(), this);
    SoundPartialHBox* end = soundPartialHboxes;

    if (soundPartialHboxes == NULL) {
        soundPartialHboxes = newBox;
    } else {
        while (end->next != NULL) {
            end = end->next;
        }
        end->next = newBox;
        newBox->prev = end;
    }

    Gtk::VBox* vbox;
    attributesRefBuilderSound->get_widget("SpectrumVBox", vbox);
    vbox->pack_start(*newBox, Gtk::PACK_SHRINK);

    soundPartialHboxes->setPartialNumber(1);

    Gtk::Entry* entry;
    attributesRefBuilderSound->get_widget("SoundAttributesNumPartialEntry", entry);
    entry->set_text(currentlyShownEvent->getEventExtraInfo()->getNumPartials());
    show_all_children();
    Gtk::Adjustment* adjustment = scrolledWindow.get_vadjustment();
    adjustment->set_value(adjustment->get_upper());
}

SoundPartialHBox::SoundPartialHBox(SpectrumPartial* _partial,
                                   EventAttributesViewController* _attributes) {
    attributes = _attributes;
    prev = NULL;
    next = NULL;
    removeButton.set_label("Remove Partial");

    functionButton.set_label("Insert Function");
    partial = _partial;

    envelopeEntry.set_text(partial->envString);

    pack_start(label, Gtk::PACK_SHRINK);
    pack_start(envelopeEntry, Gtk::PACK_EXPAND_WIDGET);
    pack_start(functionButton, Gtk::PACK_SHRINK);
    pack_start(removeButton, Gtk::PACK_SHRINK);

    removeButton.signal_clicked().connect(
        sigc::mem_fun(*this, &SoundPartialHBox::removeButtonClicked));

    functionButton.signal_clicked().connect(
        sigc::mem_fun(*this, &SoundPartialHBox::functionButtonClicked));

    envelopeEntry.signal_changed().connect(sigc::mem_fun(*this, &SoundPartialHBox::modified));
}

void SoundPartialHBox::modified() { attributes->modified(); }

SoundPartialHBox::~SoundPartialHBox() {}

void SoundPartialHBox::functionButtonClicked() {
    FunctionGenerator* generator =
        new FunctionGenerator(functionReturnENV, envelopeEntry.get_text());
    int result = generator->run();
    if (generator->getResultString() != "" && result == 0) {
        envelopeEntry.set_text(generator->getResultString());
        partial->envString = envelopeEntry.get_text();
    }
    delete generator;
}

void SoundPartialHBox::setPartialNumber(int _number) {
    char charbuffer[20];
    sprintf(charbuffer, "%d", _number);

    string labeltext = "Partial " + string(charbuffer);
    label.set_text(labeltext);

    if (next != NULL) {
        next->setPartialNumber(_number + 1);
    }
}

void SoundPartialHBox::removeButtonClicked() {
    attributes->removeSoundPartial(this);
    attributes->modified();
}

void EventAttributesViewController::removeSoundPartial(SoundPartialHBox* _remove) {
    if (currentlyShownEvent->getEventExtraInfo()->getNumPartials() == "1") {
        return;
    }
    currentlyShownEvent->setChangedButNotSaved(true);
    Gtk::VBox* vbox;
    attributesRefBuilderSound->get_widget("SpectrumVBox", vbox);
    vbox->remove(*_remove);
    currentlyShownEvent->getEventExtraInfo()->deletePartial(_remove->partial);

    if (soundPartialHboxes == _remove) {
        soundPartialHboxes = soundPartialHboxes->next;
        if (soundPartialHboxes != NULL) {
            soundPartialHboxes->prev = NULL;
        }
    }

    else {
        SoundPartialHBox* search = soundPartialHboxes;
        while (search != _remove) {
            search = search->next;
        }

        search->prev->next = search->next;
        if (search->next != NULL) {
            search->next->prev = search->prev;
        }
    }
    delete _remove;

    Gtk::Entry* entry;
    attributesRefBuilderSound->get_widget("SoundAttributesNumPartialEntry", entry);
    entry->set_text(currentlyShownEvent->getEventExtraInfo()->getNumPartials());
    if (soundPartialHboxes != NULL) {
        soundPartialHboxes->setPartialNumber(1);
    }
    show_all_children();
}

void SoundPartialHBox::clear() {
    if (next != NULL) {
        next->clear();
    }
    delete this;
}

void SoundPartialHBox::saveString() {
    partial->envString = envelopeEntry.get_text();
    if (next != NULL) {
        next->saveString();
    }
}

void EventAttributesViewController::refreshChildTypeInLayer() {
    int index = 0;
    std::vector<LayerBox*>::iterator layerBoxesIter = layerBoxesStorage.begin();
    for (layerBoxesIter; layerBoxesIter != layerBoxesStorage.end(); layerBoxesIter++) {
        index = (*layerBoxesIter)->refreshChildTypeIndex(index);
    }
}

int EventAttributesViewController::LayerBox::refreshChildTypeIndex(int _index) {
    int index = _index;
    Gtk::TreeIter iter = m_refTreeModel->children().begin();

    for (iter; iter != m_refTreeModel->children().end(); iter++) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            row[m_Columns.columnObjectChildTypeIndex] = index;
            index++;
        }
    }
    return index;
}

void EventAttributesViewController::buildNoteModifiersList() {
    if (currentlyShownEvent != NULL && currentlyShownEvent->getEventType() == eventNote) {
        saveCurrentShownEventData();
    }

    Gtk::Table* table;
    attributesRefBuilder->get_widget("NoteAttributesModifiersTable", table);

    // clean current noteModifierCheckButtons
    std::vector<Gtk::CheckButton*>::iterator iter = noteModifierCheckButtons.begin();
    for (iter; iter != noteModifierCheckButtons.end(); iter++) {
        table->remove(**iter);
        delete (*iter);
    }

    noteModifierCheckButtons.clear();

    int x = 0;
    int y = 0;

    Gtk::CheckButton* button;
    std::map<std::string, bool> defaultModifiers = projectView->getDefaultNoteModifiers();
    std::map<std::string, bool>::iterator modifierIter = defaultModifiers.begin();
    for (modifierIter; modifierIter != defaultModifiers.end(); modifierIter++) {
        if ((*modifierIter).second) {
            button = new Gtk::CheckButton((*modifierIter).first, false);

            button->signal_pressed().connect(
                sigc::mem_fun(*this, &EventAttributesViewController::modified));
            noteModifierCheckButtons.push_back(button);
            table->attach(*button, x, x + 1, y, y + 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
            x++;
            if (x == 3) {
                x = 0;
                y++;
            }
        }
    }

    std::vector<std::string> customNoteModifiers = projectView->getCustomNoteModifiers();
    std::vector<std::string>::iterator modifierIter2 = customNoteModifiers.begin();
    for (modifierIter2; modifierIter2 != customNoteModifiers.end(); modifierIter2++) {
        button = new Gtk::CheckButton(*modifierIter2, false);  // ubuntu 11.04
        button->signal_pressed().connect(
            sigc::mem_fun(*this, &EventAttributesViewController::modified));
        noteModifierCheckButtons.push_back(button);
        table->attach(*button, x, x + 1, y, y + 1, Gtk::EXPAND | Gtk::FILL, Gtk::SHRINK);
        x++;
        if (x == 3) {
            x = 0;
            y++;
        }
    }

    show_all_children();

    if (currentlyShownEvent != NULL && currentlyShownEvent->getEventType() == eventNote) {
        showCurrentEventData();
    }
}

string EventAttributesViewController::generateTempoStringByNoteValue() {
    TempoPrefix tempoPrefix;
    TempoNoteValue tempoNoteValue;

    Gtk::Entry* entry;
    Gtk::ComboBox* combobox;
    attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
    Gtk::TreeModel::iterator iter = combobox->get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            tempoPrefix = row[tempoPrefixColumns.m_col_type];
        }
    }
    attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
    iter = combobox->get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            tempoNoteValue = row[tempoNoteValueColumns.m_col_type];
        }
    }

    string stringbuffer = "";

    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);

    if (tempoPrefix == tempoPrefixDotted) {
        stringbuffer = stringbuffer + "dotted ";
    } else if (tempoPrefix == tempoPrefixDoubleDotted) {
        stringbuffer = stringbuffer + "double dotted ";
    } else if (tempoPrefix == tempoPrefixTriple) {
        stringbuffer = stringbuffer + "triple ";
    } else if (tempoPrefix == tempoPrefixQuintuple) {
        stringbuffer = stringbuffer + "quintuple ";
    } else if (tempoPrefix == tempoPrefixSextuple) {
        stringbuffer = stringbuffer + "sextuple ";
    } else if (tempoPrefix == tempoPrefixSeptuple) {
        stringbuffer = stringbuffer + "septuple ";
    }

    if (tempoNoteValue == tempoNoteValueWhole) {
        stringbuffer = stringbuffer + "whole = ";
    } else if (tempoNoteValue == tempoNoteValueHalf) {
        stringbuffer = stringbuffer + "half = ";
    } else if (tempoNoteValue == tempoNoteValueQuarter) {
        stringbuffer = stringbuffer + "quarter = ";
    } else if (tempoNoteValue == tempoNoteValueEighth) {
        stringbuffer = stringbuffer + "eighth = ";
    } else if (tempoNoteValue == tempoNoteValueSixteenth) {
        stringbuffer = stringbuffer + "sixteenth = ";
    } else if (tempoNoteValue == tempoNoteValueThirtySecond) {
        stringbuffer = stringbuffer + "thirtysecond = ";
    }

    stringbuffer = stringbuffer + entry->get_text();

    return stringbuffer;
}

string EventAttributesViewController::generateTempoStringByFraction() {
    TempoPrefix tempoPrefix;
    TempoNoteValue tempoNoteValue;

    Gtk::Entry* entry;
    Gtk::ComboBox* combobox;
    attributesRefBuilder->get_widget("attributesStandardTempoNotePrefixCombobox", combobox);
    Gtk::TreeModel::iterator iter = combobox->get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            tempoPrefix = row[tempoPrefixColumns.m_col_type];
        }
    }
    attributesRefBuilder->get_widget("attributesStandardTempoNoteCombobox", combobox);
    iter = combobox->get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            tempoNoteValue = row[tempoNoteValueColumns.m_col_type];
        }
    }

    string stringbuffer = "";
    if (tempoPrefix == tempoPrefixDotted) {
        stringbuffer = stringbuffer + "dotted ";
    } else if (tempoPrefix == tempoPrefixDoubleDotted) {
        stringbuffer = stringbuffer + "double dotted ";
    } else if (tempoPrefix == tempoPrefixTriple) {
        stringbuffer = stringbuffer + "triple ";
    } else if (tempoPrefix == tempoPrefixQuintuple) {
        stringbuffer = stringbuffer + "quintuple ";
    } else if (tempoPrefix == tempoPrefixSextuple) {
        stringbuffer = stringbuffer + "sextuple ";
    } else if (tempoPrefix == tempoPrefixSeptuple) {
        stringbuffer = stringbuffer + "septuple ";
    }

    if (tempoNoteValue == tempoNoteValueWhole) {
        stringbuffer = stringbuffer + "whole = ";
    } else if (tempoNoteValue == tempoNoteValueHalf) {
        stringbuffer = stringbuffer + "half = ";
    } else if (tempoNoteValue == tempoNoteValueQuarter) {
        stringbuffer = stringbuffer + "quarter = ";
    } else if (tempoNoteValue == tempoNoteValueEighth) {
        stringbuffer = stringbuffer + "eighth = ";
    } else if (tempoNoteValue == tempoNoteValueSixteenth) {
        stringbuffer = stringbuffer + "sixteenth = ";
    } else if (tempoNoteValue == tempoNoteValueThirtySecond) {
        stringbuffer = stringbuffer + "thirtysecond = ";
    }

    attributesRefBuilder->get_widget("attributesStandardTempoAsFractionEntry1", entry);

    char charbuffer[20];
    int numerator = atoi(entry->get_text().c_str()) * 60;
    sprintf(charbuffer, " %d", numerator);
    stringbuffer += string(charbuffer);

    attributesRefBuilder->get_widget("attributesStandardTempoValueEntry", entry);

    stringbuffer = stringbuffer + "/" + entry->get_text();

    return stringbuffer;
}

void EventAttributesViewController::tempoAsNoteValueEntryChanged() {
    tempoEntryAsNoteValueModifiedFlag = true;
}

/*

void EventAttributesViewController::makeSuperColliderCodeButtonClicked(){
  if (currentlyShownEvent){
    currentlyShownEvent->makeSuperColliderCode();

  }

}

*/
