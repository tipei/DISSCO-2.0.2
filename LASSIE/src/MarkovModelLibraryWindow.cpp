/*******************************************************************************
 *
 *  File Name     : MarkovModelLibraryWindow.cpp
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

#include "MarkovModelLibraryWindow.h"

MarkovModelLibraryWindow::MarkovModelLibraryWindow() {
  set_title("Markov Model Library");
  set_border_width(3);
  set_size_request(800, 600);

  m_attributesRefBuilder = Gtk::Builder::create();

  // #ifdef GLIBMM_EXCEPTIONS_ENABLED
  // try {
  //   m_attributesRefBuilder->add_from_file("./LASSIE/src/UI/MarkovModelLibraryWindow.ui");
  // }
  // catch (const Glib::FileError& ex){
  //   std::cerr << "FileError: " << ex.what() << std::endl;
  //   std::cout << "here is the problem" << std::endl;
  // }
  // catch (const Gtk::BuilderError& ex){
  //   std::cerr << "BuilderError: " << ex.what() << std::endl;
  // }
  // #else
  // Glib::auto_ptr<Glib::Error> error;
  // if (!m_attributesRefBuilder->add_from_file("./LASSIE/src/UI/MarkovModelLibraryWindow.ui", error)) {
  //   std::cerr << error->what() << std::endl;
  // }
  // #endif

  add(m_Paned);
  m_Paned.add1(m_ListBox);
  m_Paned.add2(m_detailBox);

  // size of the list on the left
  m_ListBox.set_size_request(200);

  m_refListStore = Gtk::ListStore::create(m_columns);
  m_TreeView.set_model(m_refListStore);

  m_ListBox.pack_start(m_ListScroll);
  m_ListScroll.add(m_TreeView);

  // handle selection on the left
  m_TreeView.get_selection()->signal_changed().connect(
    sigc::mem_fun(*this, &MarkovModelLibraryWindow::onSelectionChanged)
  );

  m_TreeView.append_column("Id", m_columns.m_col_number);

  m_Paned.show_all();

  m_detailBox.pack_start(m_detailBox1, false, false);
  m_detailBox.pack_start(*Gtk::manage(new Gtk::Label("Initial Distribution", 0, 0)), false, false);
  m_distScroll.set_size_request(-1, 2 * ENTRY_HEIGHT);
  m_detailBox.pack_start(m_distScroll, false, false);

  m_detailBox.pack_start(*Gtk::manage(new Gtk::Label("Values for states", 0, 0)), false, false);
  m_valueScroll.set_size_request(-1, 2 * ENTRY_HEIGHT);
  m_detailBox.pack_start(m_valueScroll, false, false);

  m_detailBox.pack_start(*Gtk::manage(new Gtk::Label("Transition Matrix", 0, 0)), false, false);
  m_detailBox.pack_start(m_matrixScroll);

  // construct the right part
  Gtk::HBox* box = Gtk::manage(new Gtk::HBox());
  m_detailBox1.pack_start(*box, false, false);
  box->pack_start(*Gtk::manage(new Gtk::Label("Markov chain size: ")), false, false);
  box->pack_start(m_sizeEntry, false, false);
  m_sizeButton.set_label("Set");
  m_sizeButton.signal_clicked().connect(sigc::mem_fun(*this, &MarkovModelLibraryWindow::onSetSize));
  box->pack_start(m_sizeButton, false, false);


  // context menu
  m_TreeView.signal_button_press_event().connect(
    sigc::mem_fun(*this, &MarkovModelLibraryWindow::onRightClick), false);
  m_refActionGroup = Gtk::ActionGroup::create();
  m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));
  m_refActionGroup->add(
    Gtk::Action::create("ContextAdd", "Create New Model"),
    sigc::mem_fun(*this, &MarkovModelLibraryWindow::createNewModel));

  m_refActionGroup->add(
    Gtk::Action::create("ContextDuplicate", "Duplicate Model"),
    sigc::mem_fun(*this, &MarkovModelLibraryWindow::duplicateModel));

  // new change 1/25/22
  m_refActionGroup->add(
    Gtk::Action::create("ContextRemove", "Delete Model"),
    sigc::mem_fun(*this, &MarkovModelLibraryWindow::removeModel));

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  // line "   <menuitem action='ContextRemove'/>" added 1/25/22

  Glib::ustring ui_info =
    "<ui>"
    "  <popup name='PopupMenu'>"
    "    <menuitem action='ContextAdd'/>"
    "    <menuitem action='ContextDuplicate'/>"
    "    <menuitem action='ContextRemove'/>"
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


}

MarkovModelLibraryWindow::~MarkovModelLibraryWindow() {
}

void MarkovModelLibraryWindow::createNewModel() {
  int newIdx = activeProject->createNewMarkovModel();
  activeProject->modified();
  Gtk::TreeModel::Row row = *(m_refListStore->append());
  row[m_columns.m_col_number] = newIdx;
}

void MarkovModelLibraryWindow::duplicateModel() {
  if (currentSelection < 0) return;

  int newIdx = activeProject->duplicateMarkovModel(currentSelection);
  if (newIdx == -1) {
    cout << "Duplication failed" << endl;
    return;
  }
  activeProject->modified();
  Gtk::TreeModel::Row row = *(m_refListStore->append());
  row[m_columns.m_col_number] = newIdx;
}

// added 1/25/22
void MarkovModelLibraryWindow::removeModel() {
  if (currentSelection < 0) return;
  //functionality to remove markov model from list.
  Gtk::TreeModel::Children::iterator iter = m_TreeView.get_selection()->get_selected();
  Gtk::TreeModel::Row row = *iter;
  activeProject->removeMarkovModel(int(row));

  activeProject->modified();
  int num = currentSelection;
  std::cout << "Deleted model " << currentSelection << std::endl;
  onSelectionChanged();
}

void MarkovModelLibraryWindow::onSelectionChanged() {
  Gtk::TreeModel::iterator iter = m_TreeView.get_selection()->get_selected();
  if (iter) {
    update((*iter)[m_columns.m_col_number]);
    m_Paned.show_all();
  }
}

void MarkovModelLibraryWindow::buildTable() {
  // clear cached entries
  m_matrixEntries.resize(0);
  m_distEntries.resize(0);
  m_valueEntries.resize(0);

  // clear entry widgets
  m_matrixScroll.remove();
  m_distScroll.remove();
  m_valueScroll.remove();

  // start populating distribution window
  Gtk::VBox* distBox = Gtk::manage(new Gtk::VBox());
  m_distScroll.add(*distBox);

  // construct containers for labels and entries
  Gtk::HBox* labelHBox = Gtk::manage(new Gtk::HBox());
  Gtk::HBox* entryHBox = Gtk::manage(new Gtk::HBox());
  distBox->pack_start(*labelHBox, false, false);
  distBox->pack_start(*entryHBox, false, false);

  int row_size = max(1, static_cast<int>(sqrt(size)/2));
  // create placeholders to give some left padding
  Gtk::HBox* placeholder1 = Gtk::manage(new Gtk::HBox());
  Gtk::HBox* placeholder2 = Gtk::manage(new Gtk::HBox());
  placeholder1->set_size_request(150 - (row_size * 2), LABEL_HEIGHT);
  placeholder2->set_size_request(150 - (row_size * 2), LABEL_HEIGHT);
  labelHBox->pack_start(*placeholder1, false, false);
  entryHBox->pack_start(*placeholder2, false, false);

  for (int i = 0; i < size; i++) {
    // construct labels
    Gtk::Label* label = Gtk::manage(new Gtk::Label(std::to_string(i+1)));
    label->set_size_request(ENTRY_WIDTH / row_size, LABEL_HEIGHT);

    // construct entries
    Gtk::HBox* entryBox = Gtk::manage(new Gtk::HBox());
    Gtk::Entry* entry = Gtk::manage(new Gtk::Entry());
    entryBox->set_size_request(ENTRY_WIDTH / row_size, ENTRY_HEIGHT);
    entryBox->add(*entry);
    entry->signal_focus_out_event().connect(
      sigc::mem_fun(*this, &MarkovModelLibraryWindow::onEntryChange)
    );

    // put labels and entries into their containers
    labelHBox->pack_start(*label, false, false);
    entryHBox->pack_start(*entryBox, false, false);

    // keep a record of the entries
    m_distEntries.push_back(entry);
  }


  // start populating value window
  Gtk::VBox* valueBox = Gtk::manage(new Gtk::VBox());
  m_valueScroll.add(*valueBox);

  // construct containers for labels and entries
  labelHBox = Gtk::manage(new Gtk::HBox());
  entryHBox = Gtk::manage(new Gtk::HBox());
  valueBox->pack_start(*labelHBox, false, false);
  valueBox->pack_start(*entryHBox, false, false);

  // create placeholders to give some left padding
  placeholder1 = Gtk::manage(new Gtk::HBox());
  placeholder2 = Gtk::manage(new Gtk::HBox());
  placeholder1->set_size_request(150 - (row_size * 2), LABEL_HEIGHT);
  placeholder2->set_size_request(150 - (row_size * 2), LABEL_HEIGHT);
  labelHBox->pack_start(*placeholder1, false, false);
  entryHBox->pack_start(*placeholder2, false, false);

  for (int i = 0; i < size; i++) {
    // construct labels
    Gtk::Label* label = Gtk::manage(new Gtk::Label(std::to_string(i+1)));
    label->set_size_request(ENTRY_WIDTH / row_size, LABEL_HEIGHT);

    // construct entries
    Gtk::HBox* entryBox = Gtk::manage(new Gtk::HBox());
    Gtk::Entry* entry = Gtk::manage(new Gtk::Entry());
    entryBox->set_size_request(ENTRY_WIDTH / row_size, ENTRY_HEIGHT);
    entryBox->add(*entry);
    entry->signal_focus_out_event().connect(
      sigc::mem_fun(*this, &MarkovModelLibraryWindow::onEntryChange)
    );

    // put labels and entries into their containers
    labelHBox->pack_start(*label, false, false);
    entryHBox->pack_start(*entryBox, false, false);

    // keep a record of the entries
    m_valueEntries.push_back(entry);
  }

  // start populating matrix window
  Gtk::VBox* vbox = Gtk::manage(new Gtk::VBox());
  m_matrixScroll.add(*vbox);

  // header row
  Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox());
  vbox->pack_start(*hbox, false ,false);
  Gtk::HBox* entryBox = Gtk::manage(new Gtk::HBox());
  entryBox->set_size_request(150 - (row_size * 2), LABEL_HEIGHT);
  hbox->pack_start(*entryBox, false, false);
  for (int j = 0; j < size; j++) {
    Gtk::Label* label = Gtk::manage(new Gtk::Label(std::to_string(j+1)));
    label->set_size_request(ENTRY_WIDTH / row_size, LABEL_HEIGHT);
    hbox->pack_start(*label, false, false);
  }

  for (int i = 0; i < size; i++) {
    Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox());
    vbox->pack_start(*hbox, false ,false);
    Gtk::HBox* entryBox = Gtk::manage(new Gtk::HBox());
    entryBox->set_size_request(150 - (row_size * 2), ENTRY_HEIGHT);
    hbox->pack_start(*entryBox, false, false);
    entryBox->add(*Gtk::manage(new Gtk::Label(std::to_string(i+1))));
    for (int j = 0; j < size; j++) {
      Gtk::HBox* entryBox = Gtk::manage(new Gtk::HBox());
      entryBox->set_size_request(ENTRY_WIDTH / row_size, ENTRY_HEIGHT);
      Gtk::Entry* entry = Gtk::manage(new Gtk::Entry());
      entry->signal_focus_out_event().connect(
        sigc::mem_fun(*this, &MarkovModelLibraryWindow::onEntryChange)
      );
      hbox->pack_start(*entryBox, false, false);
      entryBox->add(*entry);
      m_matrixEntries.push_back(entry); // cache the entry into a temporary vector for reference
    }
  }
  m_detailBox.show_all();
}

void MarkovModelLibraryWindow::update(int selection) {
  if (!activeProject) return;
  if (!activeProject->getMarkovModels().size()) return;

  if (currentSelection >= 0) {
    activeProject->getMarkovModels()[currentSelection].from_str(to_string());
    activeProject->modified();
  }
  currentSelection = selection;

  MarkovModel<float>& model = activeProject->getMarkovModels()[selection];

  // if model size and view size mismatch, rebuild the table
  if (size != model.getStateSize()) {
    size = model.getStateSize();
    buildTable();
  }

  // populate view
  for (int i = 0; i < size; i++) {
    m_distEntries[i]->set_text(std::to_string(model.getInitialProbability(i)));
  }
  for (int i = 0; i < size; i++) {
    m_valueEntries[i]->set_text(std::to_string(model.getStateValue(i)));
  }
  int k = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++, k++) {
      m_matrixEntries[k]->set_text(std::to_string(model.getTransitionProbability(i, j)));
    }
  }
}


bool MarkovModelLibraryWindow::onEntryChange(GdkEventFocus* event) {
}

std::string MarkovModelLibraryWindow::to_string() const {
  stringstream ss;
  ss << size << endl;
  for (auto& entry : m_valueEntries) {
    float n = read_entry_as<float>(*entry);
    if (isnan(n)) n = 0;
    ss << n << " ";
  }
  ss << "\n";

  for (auto& entry : m_distEntries) {
    float n = read_entry_as<float>(*entry);
    if (isnan(n)) n = 0;
    ss << n << " ";
  }
  ss << "\n";

  for (auto& entry : m_matrixEntries) {
    float n = read_entry_as<float>(*entry);
    if (isnan(n)) n = 0;
    ss << n << " ";
  }
  return ss.str();
}

void MarkovModelLibraryWindow::onSetSize() {
  // save old information
  int oldSize = size;
  vector<Glib::ustring> values;
  vector<Glib::ustring> dists;
  vector< vector<Glib::ustring> > matrix;
  matrix.resize(oldSize);
  for (auto& entry: m_valueEntries) {
    values.push_back(entry->get_text());
  }
  for (auto& entry: m_distEntries) {
    dists.push_back(entry->get_text());
  }
  int k = 0;
  for (int i = 0; i < oldSize; i++) {
    for (int j = 0; j < oldSize; j++, k++) {
      matrix[i].push_back(m_matrixEntries[k]->get_text());
    }
  }

  size = read_entry_as<int>(m_sizeEntry);
  if (size > 0) {
    buildTable();

    for (int i = 0; i < min(oldSize, size); i++) {
      m_valueEntries[i]->set_text(values[i]);
      m_distEntries[i]->set_text(dists[i]);
    }

    for (int i = 0; i < min(oldSize, size); i++) {
      for (int j = 0; j < min(oldSize, size); j++) {
        m_matrixEntries[i * size + j]->set_text(matrix[i][j]);
      }
    }
  }
}

void MarkovModelLibraryWindow::setActiveProject(ProjectViewController* project) {
  activeProject = project;

  if (!activeProject) return;
  if (!activeProject->getMarkovModels().size()) return;

  Gtk::TreeModel::Row row;

  m_refListStore = Gtk::ListStore::create(m_columns);
  m_TreeView.set_model(m_refListStore);
  for (int i = 0; i < activeProject->getMarkovModels().size(); i++) {
    row = *(m_refListStore->append());
    row[m_columns.m_col_number] = i;
  }
}

void MarkovModelLibraryWindow::on_hide() {
  Gtk::Window::on_hide();
  // store information back to the model
  if (currentSelection >= 0) {
    activeProject->getMarkovModels()[currentSelection].from_str(to_string());
    activeProject->modified();
  }
}

bool MarkovModelLibraryWindow::onRightClick(GdkEventButton* event) {
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
    if(m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);
    return true;
  }
  return false;
}
