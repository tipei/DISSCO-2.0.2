#include "PartialWindow.h"
#include "FunctionGenerator.h"

/* TODO
 * Find a way to stop user from creating too many partials
 * Save the result string in the main Bottom window
 * 
 * Later:
 * Finish markov changes
 * Misc. fixes in DISSCO doc
 */


/* 
 * FUNCTION: PartialWindow::PartialWindow()
 * DESCRIPTION: Constructor for Partial Window. The steps for setting up a GUI
 *              window are as follows:
 *                1) Initialize internals - counts, sub alignment linked list, gtk builder
 *                2) Get widgets from the relevant .ui using gtk builder API
 *                3) Attach signal handlers to buttons
 *                4) Now, check if _originalString contained prior information. Place the previous 
 *                   partial configuration into the window to restore settings.
 *                5) Place the correct VBox into the new window
 *                6) show_all_children() to display everything in window.
 *              PartialWindow uses sub alignments - rows inside the window - and contains
 *              them within an internal linked list. The PartialSubAlignment is its own class
 *              that contains functionality for table rows.
 * ARGS: _originalString -- This is used to retain the previously input partials. Takes the result
 *                          string that is already present in the box. 
 * RETURNS: None
 */
PartialWindow::PartialWindow(std::string _originalString, ModifierType _type) {

    // Initialize internal vars
    partialSubAlignments = NULL;
    partialNumOfNodes = 0;
    type = _type;

    set_title("Customize Partials");
    set_border_width(3);
    attributesRefBuilder = Gtk::Builder::create();

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/Partial.ui");
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
    "./LASSIE/src/UI/Partial.ui", error)){
    std::cerr << error->what() << std::endl;
  }

#endif

    // GTK elements for pulling widgets from .ui file
    Gtk::Alignment* alignment;
    Gtk::TextView* textview;
    Gtk::VBox* vbox;
    Gtk::Entry* entry;
    Gtk::RadioButton* radiobutton;
    Gtk::Button* button;


    // BUILD THE WINDOW
    // Get all the widgets from .ui, attach signals for buttons
    // mimic buttons of stochos
    // make alignment class for partials
    // See line 3256. This initializes stuff 
    // show_all_children() This is how you show all child widgets
    add_button( "gtk-ok", 0);
    add_button("gtk-cancel", 1);

    // Connect signals for Partial buttons -- add node, insert function
    attributesRefBuilder->get_widget(
      "PartialAddNodeButton", button);
    button->signal_clicked().connect(sigc::mem_fun(
      *this, & PartialWindow::AddNodeButtonClicked));

    attributesRefBuilder->get_widget(
      "PartialInsertFunctionButton", button);
    button->signal_clicked().connect(sigc::mem_fun(
      *this, & PartialWindow::FunButtonClicked));

    // Check original string by XML parsing
    XMLPlatformUtils::Initialize();
    XercesDOMParser* parser = new XercesDOMParser();

    xercesc::MemBufInputSource myxml_buf  ((const XMLByte*)_originalString.c_str(), _originalString.size(),
                                      "function (in memory)");

    parser->parse(myxml_buf);

    DOMDocument* xmlDocument = parser->getDocument();
    DOMElement* root = xmlDocument->getDocumentElement();

    char* functionNameChars;
    char* charBuffer;
    DOMCharacterData* textData;
    string functionName;
    DOMElement* thisElement;

    // See if string empty/invalid
    if (root != NULL){
      DOMElement* functionNameElement = root->getFirstElementChild();
      textData = ( DOMCharacterData*) functionNameElement->getFirstChild();

      if (textData){
        functionNameChars = XMLString::transcode(textData->getData());
        functionName = string(functionNameChars);
        XMLString::release(&functionNameChars);
      }

      if (functionName.compare("Partials") == 0) {

        thisElement = functionNameElement->getNextElementSibling();    //envelopes
        DOMElement* envelopeElement = thisElement->getFirstElementChild() ;//first envelope

        // Create rows (sub alignments) corresponding to what was previously written
        while (envelopeElement != NULL){
          PartialSubAlignment* newSubAlignment =
              new PartialSubAlignment(this, ++partialNumOfNodes);
          
          // Add to internal linked list
          if ( partialSubAlignments ==NULL){
            partialSubAlignments = newSubAlignment;
          }
          else {
            partialSubAlignments->appendNewNode(newSubAlignment);
          }

          attributesRefBuilder->get_widget(
              "PartialInnerVBox", vbox);
          vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
          newSubAlignment->setProbEntry(getFunctionString(envelopeElement));
          envelopeElement = envelopeElement->getNextElementSibling();
          newSubAlignment->setAmpValueEntry(getFunctionString(envelopeElement));
          envelopeElement = envelopeElement->getNextElementSibling();
          newSubAlignment->setWidthEntry(getFunctionString(envelopeElement));
          envelopeElement = envelopeElement->getNextElementSibling();
          newSubAlignment->setRateValueEntry(getFunctionString(envelopeElement));
          envelopeElement = envelopeElement->getNextElementSibling();
        }

        textChanged();
        show_all_children();
      }
    }

    attributesRefBuilder->get_widget("PartialVBox", vbox);

    get_vbox()->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET);

    set_position(Gtk::WIN_POS_CENTER_ALWAYS);
    resize(1280,600);
    show_all_children();
}


/* 
 * FUNCTION: PartialWindow::~PartialWindow
 * DESCRIPTION: Destructor for a PartialWindow. Deletes all PartialSubAlignment nodes
 *              in linked list before freeing.
 * ARGS: None
 * RETURNS: None
 */
PartialWindow::~PartialWindow() {
  // Delete nodes in the linked list for partial nodes
  if (partialSubAlignments != NULL){
    partialSubAlignments->clear();
  }
}



/* 
 * FUNCTION: PartialWindow::getFunctionString 
 * DESCRIPTION: Used to transcode an element back into the XML format, to place into box
 * ARGS: _thisFunctionElement -- The element to examine in order to find the full xml line 
 * RETURNS: returnString -- The XML string to place in an entry
 */
std::string PartialWindow::getFunctionString(DOMElement* _thisFunctionElement){

  char* charBuffer;
  DOMCharacterData* textData;
  string returnString;
  DOMElement* child = _thisFunctionElement->getFirstElementChild();
  if (child ==NULL){ //not containing any child, return string

    textData = ( DOMCharacterData*) _thisFunctionElement->getFirstChild();
    if (textData){
      charBuffer = XMLString::transcode(textData->getData());
      returnString = string(charBuffer);
      XMLString::release(&charBuffer);
    } else returnString = "";

    return returnString;
  }

  //contain function!
  XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
  DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
  XMLCh* bla = theSerializer->writeToString (child);
  returnString = XMLString::transcode(bla);
  XMLString::release(&bla);
  delete theSerializer;
  return returnString;
}



/* 
 * FUNCTION: PartialWindow::AddNodeButtonClicked 
 * DESCRIPTION: Handler that is called when pressing the 'Add Node' button.
 *              Creates a new sub alignment (row), and adds to the inner VBox
 * ARGS: None
 * RETURNS: None
 */
void PartialWindow::AddNodeButtonClicked(){

  partialNumOfNodes ++;

  PartialSubAlignment* newSubAlignment =
    new PartialSubAlignment(this, partialNumOfNodes);
  if (partialSubAlignments ==NULL){
    partialSubAlignments = newSubAlignment;
  }
  else {
    partialSubAlignments->appendNewNode(newSubAlignment);
  }

  Gtk::VBox* vbox;
  Gtk::Entry* entry;

  // Place sub alignment into vbox
  attributesRefBuilder->get_widget(
    "PartialInnerVBox", vbox);
  vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);

  textChanged();

  show_all_children();
}



/* 
 * FUNCTION: PartialWindow::FunButtonClicked
 * DESCRIPTION: Handler that is called when pressing the 'Insert Function' button.
 *              Pops out a FunctionGenerator window and places the result string in
 *              text box.
 * ARGS: None
 * RETURNS: None
 */
void PartialWindow::FunButtonClicked(){

  // Get the current entry box
  Gtk::Entry* entry = dynamic_cast< Gtk::Entry * >(get_focus() );
  if( entry ){
    FunctionGenerator* generator =
      new FunctionGenerator(functionReturnENV, entry->get_text());

    int result = generator->run();
    if (generator->getResultString() !=""&& result ==0){
      entry->set_text(generator->getResultString());
    }
    delete generator;
  }
}



/* 
 * FUNCTION: PartialWindow::partialRemoveNode
 * DESCRIPTION: Callback invoked by 'Remove Node' handler. Removes and deletes the
 *              given PartialSubAlignment row from linked list and vbox.
 * ARGS: _remove -- the partial node to remove
 * RETURNS: None
 */
void PartialWindow::partialRemoveNode(PartialSubAlignment* _remove){
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "PartialInnerVBox", vbox);
  vbox->remove ( *_remove);
  partialNumOfNodes --;

  if (_remove == partialSubAlignments){ //if removing head
    partialSubAlignments = partialSubAlignments->next;
    if (partialSubAlignments != NULL){
      partialSubAlignments->prev = NULL;
    }
  }

  else {  //normal case

    _remove->prev->next = _remove->next;
    if (_remove->next != NULL){
      _remove->next->prev = _remove->prev;
    }
  }

  delete _remove;

  // Update result string and display window
  textChanged();
  show_all_children();
}



/* 
 * FUNCTION: PartialWindow::textChanged
 * DESCRIPTION: Updates the result string displayed at the bottom of the window.
 *              The result string should contain all envelopes for partials.
 *              This will be read later by CMOD to extract individual envelopes.
 * ARGS: None
 * RETURNS: None
 */
void PartialWindow::textChanged(){

  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;

  std::string stringbuffer ="";
  stringbuffer = stringbuffer + "<Fun><Name>Partials</Name><Envelopes>";
  PartialSubAlignment* current = partialSubAlignments;
  while (current != NULL){
    stringbuffer = stringbuffer + current->toString();
    current = current ->next;
  }

  stringbuffer = stringbuffer + "</Envelopes></Fun>";

  textview->get_buffer()->set_text(stringbuffer);
}



/* 
 * FUNCTION: PartialWindow::getResultString
 * DESCRIPTION: Returns the result string in the bottom field of the window.
 *              This is the string that contains all of the envelopes for the
 *              different partials.
 * ARGS: None
 * RETURNS: result -- result string
 */
std::string PartialWindow::getResultString(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  return textview->get_buffer()->get_text();
}



// ------------ Partial Sub Alignment methods

/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::PartialSubAlignment
 * DESCRIPTION: Constructor for a PartialSubAlignment. Pulls VBox from .ui,
 *              adds buttons, builds interface.
 * ARGS: _parent -- PartialWindow parent
 *       _partialNum -- Partial number corresponding to the new row
 * RETURNS: None
 */
PartialWindow::PartialSubAlignment::PartialSubAlignment(
  PartialWindow* _parent, int _partialNum){
  this->parent = _parent;
  this->partialNum = _partialNum;
  this->next = NULL;
  this->prev = NULL;
  this->attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/PartialSubAlignment.ui");
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
    "./LASSIE/src/UI/PartialSubAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget("PartialSubAlignmentVBox", vbox);
  add(*vbox);


  Gtk::Button* button;
  Gtk::Entry* entry;
  Gtk::Label* label;
  // Set the partial number on the left column of the window
  attributesRefBuilder->get_widget("PartialNumLabel", label);
  label->set_text("Partial #" + std::to_string(partialNum));

  // Signal for removing PartialSubAlignment rows
  attributesRefBuilder->get_widget(
    "removeNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & PartialWindow::PartialSubAlignment::removeButtonClicked));

  ModifierType type = parent->type;
  // Attach signals to the envelopes for modifying the result string
  if (type == modifierAmptrans || type == modifierFreqtrans){
    attributesRefBuilder->get_widget("RateValueEntry", entry);
    entry->set_sensitive(true);
    attributesRefBuilder->get_widget("WidthEntry", entry);
    entry->set_sensitive(true);
  }
  else if (type == modifierTremolo|| type == modifierVibrato){
    attributesRefBuilder->get_widget("RateValueEntry", entry);
    entry->set_sensitive(true);
    attributesRefBuilder->get_widget("WidthEntry", entry);
    entry->set_sensitive(false);
  }
  else{
    attributesRefBuilder->get_widget("RateValueEntry", entry);
    entry->set_sensitive(false);
    attributesRefBuilder->get_widget("WidthEntry", entry);
    entry->set_sensitive(false);

  }

  attributesRefBuilder->get_widget(
    "ProbabilityEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & PartialWindow::PartialSubAlignment::textChanged));
  if (entry->get_sensitive())
    entry->set_text("PROB");

  attributesRefBuilder->get_widget(
    "AmpValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & PartialWindow::PartialSubAlignment::textChanged));
  if (entry->get_sensitive())
    entry->set_text("AMP");

  attributesRefBuilder->get_widget(
    "WidthEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & PartialWindow::PartialSubAlignment::textChanged));
  if (entry->get_sensitive())
    entry->set_text("WIDTH");

  attributesRefBuilder->get_widget(
    "RateValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & PartialWindow::PartialSubAlignment::textChanged));
  if (entry->get_sensitive())
    entry->set_text("RATE");

}



void PartialWindow::PartialSubAlignment::setProbEntry(std::string _string) {
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("ProbabilityEntry", entry);
  entry->set_text(_string);
}

void PartialWindow::PartialSubAlignment::setAmpValueEntry(std::string _string) {
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("AmpValueEntry", entry);
  entry->set_text(_string);
}

void PartialWindow::PartialSubAlignment::setWidthEntry(std::string _string) {
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("WidthEntry", entry);
  entry->set_text(_string);
}

void PartialWindow::PartialSubAlignment::setRateValueEntry(std::string _string) {
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("RateValueEntry", entry);
  entry->set_text(_string);
}



/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::appendNewNode
 * DESCRIPTION: Adds a new PartialSubAlignment (row) to the linked list
 * ARGS: _newNode -- the new sub alignment row to add to linked list
 * RETURNS: None
 */
void PartialWindow::PartialSubAlignment::appendNewNode(PartialSubAlignment* _newNode) {
  if (next == NULL){
    next = _newNode;
    _newNode->prev = this;
  }
  else {
    next-> appendNewNode(_newNode);
  }
}



/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::removeButtonClicked
 * DESCRIPTION: Invokes callback for the PartialWindow to remove this row from its linked list
 * ARGS: None
 * RETURNS: None
 */
void PartialWindow::PartialSubAlignment::removeButtonClicked() {
  parent->partialRemoveNode(this);
}


/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::removeButtonClicked
 * DESCRIPTION: Invokes callback for the PartialWindow to remove this row from its linked list
 * ARGS: None
 * RETURNS: None
 */
void PartialWindow::PartialSubAlignment::textChanged() {
  parent->textChanged();
}



/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::toString
 * DESCRIPTION: Builds a string based on the envelopes created in this PartialSubAlignment
 * ARGS: None
 * RETURNS: stringBuffer -- the string containing all of the envelopes for this row
 */
std::string PartialWindow::PartialSubAlignment::toString() {
    Gtk::Entry* entry;
    attributesRefBuilder->get_widget("ProbabilityEntry", entry);
    std::string stringbuffer = "<Envelope>" +entry->get_text() + "</Envelope>";
    attributesRefBuilder->get_widget("AmpValueEntry", entry);
    stringbuffer = stringbuffer + "<Envelope>"+entry->get_text() + "</Envelope>";
    attributesRefBuilder->get_widget("WidthEntry", entry);
    stringbuffer = stringbuffer +"<Envelope>"+entry->get_text() + "</Envelope>";
    attributesRefBuilder->get_widget("RateValueEntry", entry);
    stringbuffer = stringbuffer +"<Envelope>"+entry->get_text() + "</Envelope>";

    return stringbuffer; 
}



/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::~PartialSubAlignment
 * DESCRIPTION: Empty destructor
 * ARGS: None
 * RETURNS: None
 */
PartialWindow::PartialSubAlignment::~PartialSubAlignment(){}



/* 
 * FUNCTION: PartialWindow::PartialSubAlignment::clear
 * DESCRIPTION: Clears the PartialSubAlignment linked list (but not the parent's)
 * ARGS: None
 * RETURNS: None
 */
void PartialWindow::PartialSubAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;
}
