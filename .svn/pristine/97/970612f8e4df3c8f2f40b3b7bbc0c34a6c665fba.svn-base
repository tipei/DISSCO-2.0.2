/*******************************************************************************
 *
 *  File Name     : FunctionGenerator.cpp
 *  Date created  : Jun. 8 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains the definitions of FunctionGenerator
 *                  class, which is responsible for the interface of generating
 *                  function string
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


#include "FunctionGenerator.h"
#include "IEvent.h"




// these definitions are for calling yyparse(). They are copied from lex.yy.c
#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif



#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
  {
  FILE *yy_input_file;

  char *yy_ch_buf;		/* input buffer */
  char *yy_buf_pos;		/* current position in input buffer */

  /* Size of input buffer in bytes, not including room for EOB
   * characters.
   */
  yy_size_t yy_buf_size;

  /* Number of characters read into yy_ch_buf, not including EOB
   * characters.
   */
  int yy_n_chars;

  /* Whether we "own" the buffer - i.e., we know we created it,
   * and can realloc() it to grow it, and should free() it to
   * delete it.
   */
  int yy_is_our_buffer;

  /* Whether this is an "interactive" input source; if so, and
   * if we're using stdio for input, then we want to use getc()
   * instead of fread(), to make sure we stop fetching input after
   * each newline.
   */
  int yy_is_interactive;

  /* Whether we're considered to be at the beginning of a line.
   * If so, '^' rules will be active on the next match, otherwise
   * not.
   */
  int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */

  /* Whether to try to fill the input buffer when we reach the
   * end of it.
   */
  int yy_fill_buffer;

  int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
  /* When an EOF's been seen but there's still some text to process
   * then we mark the buffer as YY_EOF_PENDING, to indicate that we
   * shouldn't try reading from the input source any more.  We might
   * still have a bunch of tokens to match, though, because of
   * possible backing-up.
   *
   * When we actually see the EOF, we change the status to "new"
   * (via yyrestart()), so that the user can continue scanning by
   * just pointing yyin at a new input file.
   */
#define YY_BUFFER_EOF_PENDING 2

  };
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};



extern YY_BUFFER_STATE yy_scan_string( const char*);
extern int yyparse();
extern map<const char*, FileValue*, ltstr> file_data;







FunctionGenerator::FunctionGenerator(
  FunctionReturnType _returnType,
  std::string _originalString){

  //check new  format
  /*
  if (_originalString.size()!=0&&_originalString.at(0)=='<'){
  }
  else {
    parseOldFormat(_returnType, _originalString);
    cout<<"parse Old Format"<<endl;

    return;
  }
 */



  set_title("Function Generator");
    set_border_width(3);
    result = "";
    returnType = _returnType;
    selectSubAlignments = NULL;
    selectNumOfNodes = 0;
    stochosSubAlignments = NULL;
    stochosNumOfNodes = 0;
    stochosMethodFlag = 0;
    makeEnvelopeSubAlignments = NULL;
    makeEnvelopeNumOfNodes = 0;
    SPANumOfChannels = 0;
    SPANumOfPartials = 0;
    SPAApplyFlag = 0; //junk here,
    SPAChannelAlignments = NULL;
    SPAMethodFlag = 0;//same here


  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/FunctionGenerator.ui");
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
    "./LASSIE/src/UI/FunctionGenerator.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget("mainVBox", vbox);
  get_vbox()->pack_start(*vbox, Gtk::PACK_EXPAND_WIDGET);






  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("FunctionListComboBox", combobox);

  functionListTreeModel = Gtk::ListStore::create(functionListColumns);
  combobox->set_model(functionListTreeModel);

  if (_returnType ==functionReturnInt){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomInt;
    row[functionListColumns.m_col_name] = "RandomInt";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionStochos;
    row[functionListColumns.m_col_name] = "Stochos";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionValuePick;
    row[functionListColumns.m_col_name] = "ValuePick";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionChooseL;
    row[functionListColumns.m_col_name] = "ChooseL";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionGetPattern;
    row[functionListColumns.m_col_name] = "GetPattern";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMarkov;
    row[functionListColumns.m_col_name] = "GetFromMarkovChain";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionFibonacci;
    row[functionListColumns.m_col_name] = "Fibonacci";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_TYPE;
    row[functionListColumns.m_col_name] = "CURRENT_TYPE";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_CHILD_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_CHILD_NUM";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_PARTIAL_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_PARTIAL_NUM";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_SEGMENT;
    row[functionListColumns.m_col_name] = "CURRENT_SEGMENT";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticAVAILABLE_EDU;
    row[functionListColumns.m_col_name] = "AVAILABLE_EDU";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticPREVIOUS_CHILD_DURATION;
    row[functionListColumns.m_col_name] = "PREVIOUS_CHILD_DURATION";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_LAYER;
    row[functionListColumns.m_col_name] = "CURRENT_LAYER";

  }
  else if (_returnType ==functionReturnFloat){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandom;
    row[functionListColumns.m_col_name] = "Random";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomInt;
    row[functionListColumns.m_col_name] = "RandomInt";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomizer;
    row[functionListColumns.m_col_name] = "Randomizer";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomDensity;
    row[functionListColumns.m_col_name] = "RandomDensity";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionInverse;
    row[functionListColumns.m_col_name] = "Inverse";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionLN;
    row[functionListColumns.m_col_name] = "LN";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionFibonacci;
    row[functionListColumns.m_col_name] = "Fibonacci";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionDecay;
    row[functionListColumns.m_col_name] = "Decay";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMarkov;
    row[functionListColumns.m_col_name] = "GetFromMarkovChain";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionStochos;
    row[functionListColumns.m_col_name] = "Stochos";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionValuePick;
    row[functionListColumns.m_col_name] = "ValuePick";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionChooseL;
    row[functionListColumns.m_col_name] = "ChooseL";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionGetPattern;
    row[functionListColumns.m_col_name] = "GetPattern";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_TYPE;
    row[functionListColumns.m_col_name] = "CURRENT_TYPE";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_CHILD_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_CHILD_NUM";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_PARTIAL_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_PARTIAL_NUM";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_SEGMENT;
    row[functionListColumns.m_col_name] = "CURRENT_SEGMENT";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticAVAILABLE_EDU;
    row[functionListColumns.m_col_name] = "AVAILABLE_EDU";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticPREVIOUS_CHILD_DURATION;
    row[functionListColumns.m_col_name] = "PREVIOUS_CHILD_DURATION";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_LAYER;
    row[functionListColumns.m_col_name] = "CURRENT_LAYER";

  }
  else if (_returnType ==functionReturnMakeListFun){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandom;
    row[functionListColumns.m_col_name] = "Random";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomInt;
    row[functionListColumns.m_col_name] = "RandomInt";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomizer;
    row[functionListColumns.m_col_name] = "Randomizer";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRandomDensity;
    row[functionListColumns.m_col_name] = "RandomDensity";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionInverse;
    row[functionListColumns.m_col_name] = "Inverse";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionLN;
    row[functionListColumns.m_col_name] = "LN";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionFibonacci;
    row[functionListColumns.m_col_name] = "Fibonacci";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionDecay;
    row[functionListColumns.m_col_name] = "Decay";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionStochos;
    row[functionListColumns.m_col_name] = "Stochos";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionValuePick;
    row[functionListColumns.m_col_name] = "ValuePick";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionChooseL;
    row[functionListColumns.m_col_name] = "ChooseL";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionGetPattern;
    row[functionListColumns.m_col_name] = "GetPattern";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionEnvLib;
    row[functionListColumns.m_col_name] = "EnvLib";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeEnvelope;
    row[functionListColumns.m_col_name] = "MakeEnvelope";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadENVFile;
    row[functionListColumns.m_col_name] = "ReadENVFile";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = NOT_A_FUNCTION;
    row[functionListColumns.m_col_name] = "----------";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_TYPE;
    row[functionListColumns.m_col_name] = "CURRENT_TYPE";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_CHILD_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_CHILD_NUM";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_PARTIAL_NUM;
    row[functionListColumns.m_col_name] = "CURRENT_PARTIAL_NUM";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_SEGMENT;
    row[functionListColumns.m_col_name] = "CURRENT_SEGMENT";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticAVAILABLE_EDU;
    row[functionListColumns.m_col_name] = "AVAILABLE_EDU";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticPREVIOUS_CHILD_DURATION;
    row[functionListColumns.m_col_name] = "PREVIOUS_CHILD_DURATION";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = function_staticCURRENT_LAYER;
    row[functionListColumns.m_col_name] = "CURRENT_LAYER";

  }

  else if (_returnType ==functionReturnList){
  Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeList;
    row[functionListColumns.m_col_name] = "MakeList";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionRawList;
    row[functionListColumns.m_col_name] = "RawList";

  }

  else if (_returnType ==functionReturnENV){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeEnvelope;
    row[functionListColumns.m_col_name] = "MakeEnvelope";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadENVFile;
    row[functionListColumns.m_col_name] = "ReadENVFile";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionEnvLib;
    row[functionListColumns.m_col_name] = "EnvLib";

    // this is based on Sever's request
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";


  }

  else if (_returnType ==functionReturnSPA){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSPA;
    row[functionListColumns.m_col_name] = "SPA";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadSPAFile;
    row[functionListColumns.m_col_name] = "ReadSPAFile";

    // this is based on Sever's request
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";


  }

    else if (_returnType ==functionReturnFIL){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeFilter;
    row[functionListColumns.m_col_name] = "MakeFilter";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadFILFile;
    row[functionListColumns.m_col_name] = "ReadFILFile";

    // this is based on Sever's request
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";


  }
  else if (_returnType ==functionReturnPAT){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakePattern;
    row[functionListColumns.m_col_name] = "MakePattern";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionExpandPattern;
    row[functionListColumns.m_col_name] = "ExpandPattern";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadPATFile;
    row[functionListColumns.m_col_name] = "ReadPATFile";

    // this is based on Sever's request
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";

  }


  else if (_returnType ==functionReturnREV){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionREV_Simple;
    row[functionListColumns.m_col_name] = "REV_Simple";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionREV_Medium;
    row[functionListColumns.m_col_name] = "REV_Medium";

    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionREV_Advanced;
    row[functionListColumns.m_col_name] = "REV_Advanced";
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadREVFile;
    row[functionListColumns.m_col_name] = "ReadREVFile";

        // this is based on Sever's request
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";

  }

  else if (_returnType ==functionReturnSIV){
    Gtk::TreeModel::Row row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionMakeSieve;
    row[functionListColumns.m_col_name] = "MakeSieve";


    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionReadSIVFile;
    row[functionListColumns.m_col_name] = "ReadSIVFile";

        // this is based on Sever's request
    row = *(functionListTreeModel->append());
    row[functionListColumns.m_col_id] = functionSelect;
    row[functionListColumns.m_col_name] = "Select";

  }





  combobox->pack_start(functionListColumns.m_col_name);

  combobox->signal_changed().connect(
    sigc::mem_fun(*this,&FunctionGenerator::function_list_combo_changed) );

   add_button( "gtk-ok", 0);
   add_button("gtk-cancel", 1);



   //connecting signals to functions
  Gtk::Entry* entry;
  Gtk::Button* button;
  Gtk::RadioButton* radiobutton;

   // Random
  attributesRefBuilder->get_widget(
    "RandomLowBoundFunButton", button);
  button->signal_clicked().connect(
    sigc::mem_fun(*this, & FunctionGenerator::randomLowBoundFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomHighBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomHighBoundFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomLowBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomEntryChanged));

  attributesRefBuilder->get_widget(
    "RandomHighBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, & FunctionGenerator::randomEntryChanged));



  //RandomInt

  attributesRefBuilder->get_widget(
    "RandomIntLowBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomIntLowBoundFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomIntHighBoundFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomIntHighBoundFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomIntLowBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomIntEntryChanged));

  attributesRefBuilder->get_widget(
    "RandomIntHighBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomIntEntryChanged));


  //Randomizer

  attributesRefBuilder->get_widget(
    "RandomizerBaseFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomizerBaseFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomizerDeviationFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomizerDeviationFunButtonClicked));

  attributesRefBuilder->get_widget(
    "RandomizerBaseEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomizerEntryChanged));

  attributesRefBuilder->get_widget(
    "RandomizerDeviationEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::randomizerEntryChanged));

  // RandomDensity

  attributesRefBuilder->get_widget("RandomDensityEnvelopeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, &FunctionGenerator::randomDensityEntryChanged));

  attributesRefBuilder->get_widget("RandomDensityLowBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, &FunctionGenerator::randomDensityEntryChanged));

  attributesRefBuilder->get_widget("RandomDensityHighBoundEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(*this, &FunctionGenerator::randomDensityEntryChanged));

  //Decay

  attributesRefBuilder->get_widget(
    "DecayBaseFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayBaseFunButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayRateFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayRateFunButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayIndexFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayIndexFunButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayTypeExponentialRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayTypeRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayTypeLinearRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayTypeRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "DecayBaseEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayEntryChanged));

  attributesRefBuilder->get_widget(
    "DecayRateEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayEntryChanged));

  attributesRefBuilder->get_widget(
    "DecayIndexEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::decayEntryChanged));


  //inverse

  attributesRefBuilder->get_widget(
    "InverseFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::inverseFunButtonClicked));

  attributesRefBuilder->get_widget(
    "InverseEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::inverseEntryChanged));

  // Markov
  attributesRefBuilder->get_widget("MarkovEntry", entry);
  entry->signal_changed().connect(
    sigc::mem_fun(
      *this, &FunctionGenerator::markovEntryChanged));


  //LN

  attributesRefBuilder->get_widget(
    "LNFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::LNFunButtonClicked));

  attributesRefBuilder->get_widget(
    "LNEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::LNEntryChanged));

    //Fibonacci\

    attributesRefBuilder->get_widget(
      "FibonacciFunButton", button);
    button->signal_clicked().connect(sigc::mem_fun(
      *this, & FunctionGenerator::FibonacciFunButtonClicked));

    attributesRefBuilder->get_widget(
      "FibonacciEntry", entry);
    entry->signal_changed().connect(sigc::mem_fun(
      *this, & FunctionGenerator::FibonacciEntryChanged));
  //select

  attributesRefBuilder->get_widget(
    "SelectIndexFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectIndexFunButtonClicked));

  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectEntryChanged));

  attributesRefBuilder->get_widget(
    "SelectAddNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectAddNodeButtonClicked));

  attributesRefBuilder->get_widget(
    "SelectInsertFunctionButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectFunButtonClicked));

  /*
  attributesRefBuilder->get_widget(
    "SelectListFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectListFunButtonClicked));

  attributesRefBuilder->get_widget(
    "SelectIndexFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectIndexFunButtonClicked));

  attributesRefBuilder->get_widget(
    "SelectListEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectEntryChanged));

  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::selectEntryChanged));
  */




  //stochos

  attributesRefBuilder->get_widget(
    "StochosRangeDistribRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator:: stochosMethodRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "StochosFunctionsRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator:: stochosMethodRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "StochosOffsetEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::stochosTextChanged));


  attributesRefBuilder->get_widget(
    "StochosAddNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::stochosAddNodeButtonClicked));

  attributesRefBuilder->get_widget(
    "StochosInsertFunctionButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::stochosFunButtonClicked));







  //ValuePick

  attributesRefBuilder->get_widget(
    "ValuePickElementsMeaningfulRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickElementsModsRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickElementsFakeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickWeightsPeriodicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickWeightsHierarchicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickWeightsIncludeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickTypeVariableRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickTypeConstantRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));


  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickLowEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickHighEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickDistEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickElementsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickOffsetEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));

  attributesRefBuilder->get_widget(
    "ValuePickWeightsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickTextChanged));



  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickAbsRangeFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ValuePickLowFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickLowFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ValuePickHighFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickHighFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ValuePickDistFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::valuePickDistFunButtonClicked));



  //ChooseL

  attributesRefBuilder->get_widget(
    "ChooseLFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::chooseLFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ChooseLEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::chooseLEntryChanged));


  //GetPattern

  attributesRefBuilder->get_widget(
    "GetPatternInOrderRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));

  attributesRefBuilder->get_widget(
    "GetPatternOtherRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));

  attributesRefBuilder->get_widget(
    "GetPatternTypeClustersRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));

  attributesRefBuilder->get_widget(
    "GetPatternTimeDependRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));

  attributesRefBuilder->get_widget(
    "GetPatternProbabilityRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));

  attributesRefBuilder->get_widget(
    "GetPatternOffsetFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternOffsetFunButtonClicked));

  attributesRefBuilder->get_widget(
    "GetPatternOffsetEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));


  attributesRefBuilder->get_widget(
    "GetPatternFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternFunButtonClicked));


  attributesRefBuilder->get_widget(
    "GetPatternEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::getPatternEntryChanged));


  //MakeList

  attributesRefBuilder->get_widget(
    "MakeListFunctionFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeListFunctionFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeListSizeFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeListSizeFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeListFunctionEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeListTextChanged));

  attributesRefBuilder->get_widget(
    "MakeListSizeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeListTextChanged));

  //EnvLib

  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::envLibEnvelopeFunButtonClicked));

  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::envLibScalingFactorFunButtonClicked));

  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::envLibTextChanged));

  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::envLibTextChanged));


  //ReadENVFile

  attributesRefBuilder->get_widget(
    "ReadENVFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::readENVFileTextChanged));


  //MakeEnvelope

  attributesRefBuilder->get_widget(
    "MakeEnvelopeScalingFactorFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makeEnvelopeScalingFactorFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeEnvelopeXValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makeEnvelopeXValueFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeEnvelopeYValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makeEnvelopeYValueFunButtonClicked));


  attributesRefBuilder->get_widget(
    "MakeEnvelopeScalingFactorEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeEnvelopeTextChanged));

  attributesRefBuilder->get_widget(
    "MakeEnvelopeXValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeEnvelopeTextChanged));
  //entry->set_text("1.0");
  //entry->set_editable(false);


  attributesRefBuilder->get_widget(
    "MakeEnvelopeYValueEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeEnvelopeTextChanged));

  makeEnvelopeSubAlignments = new MakeEnvelopeSubAlignment(this);
  makeEnvelopeNumOfNodes = 1;


  attributesRefBuilder->get_widget(
    "MakeEnvelopeInnerVBox", vbox);
  vbox->pack_start(*makeEnvelopeSubAlignments,Gtk::PACK_SHRINK);

  //MakePattern

    attributesRefBuilder->get_widget(
    "MakePatternIntervalsFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makePatternIntervalsFunButtonClicked));



  attributesRefBuilder->get_widget(
    "MakePatternIntervalsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makePatternTextChanged));

  //ExpandPattern

  attributesRefBuilder->get_widget(
    "ExpandPatternEquivalenceRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "ExpandPatternSymmetriesRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "ExpandPatternDistortRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternRadioButtonClicked));


  attributesRefBuilder->get_widget(
    "ExpandPatternModuloFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::expandPatternModuloFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ExpandPatternLowFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::expandPatternLowFunButtonClicked));
  attributesRefBuilder->get_widget(
    "ExpandPatternHighFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::expandPatternHighFunButtonClicked));
  attributesRefBuilder->get_widget(
    "ExpandPatternPatternFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::expandPatternPatternFunButtonClicked));

  attributesRefBuilder->get_widget(
    "ExpandPatternModuloEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternTextChanged));

  attributesRefBuilder->get_widget(
    "ExpandPatternLowEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternTextChanged));

  attributesRefBuilder->get_widget(
    "ExpandPatternHighEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternTextChanged));

  attributesRefBuilder->get_widget(
    "ExpandPatternPatternEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::expandPatternTextChanged));


  //ReadPATFile


  attributesRefBuilder->get_widget(
    "ReadPATFileNameEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::readPATFileTextChanged));


  //ReadFILFile
  attributesRefBuilder->get_widget(
    "ReadFILFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::readFILFileTextChanged));


  //MakeFilter
  attributesRefBuilder->get_widget(
    "MakeFilterFrequencyEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

  attributesRefBuilder->get_widget(
    "MakeFilterBandWidthEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

  attributesRefBuilder->get_widget(
    "MakeFilterDBGainEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

  attributesRefBuilder->get_widget(
    "MakeFilterFrequencyFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makeFilterFrequencyFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeFilterBandWidthFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makeFilterBandWidthFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeFilterDBGainFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::makeFilterDBGainFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeFilterLPFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

  attributesRefBuilder->get_widget(
    "MakeFilterHPFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

    attributesRefBuilder->get_widget(
    "MakeFilterBPFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

    attributesRefBuilder->get_widget(
    "MakeFilterLSFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

    attributesRefBuilder->get_widget(
    "MakeFilterHSFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

    attributesRefBuilder->get_widget(
    "MakeFilterNFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));

    attributesRefBuilder->get_widget(
    "MakeFilterPBEQFRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeFilterTextChanged));




  //ReadREVFile

  attributesRefBuilder->get_widget(
    "ReadREVFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::readREVFileTextChanged));

  //REV_Simple
    attributesRefBuilder->get_widget(
    "REV_SimpleEntryFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_SimpleEntryFunButtonClicked));

  attributesRefBuilder->get_widget(
    "REV_SimpleEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_SimpleEntryTextChanged));

  //REV_Medium

  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_MediumReverbPercentFunButtonClicked));


  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_MediumHilowSpreadFunButtonClicked));


  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_MediumGainAllPassFunButtonClicked));


  attributesRefBuilder->get_widget(
    "REV_MediumDelayFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_MediumDelayFunButtonClicked));

  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_MediumTextChanged));

  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_MediumTextChanged));

  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_MediumTextChanged));
  attributesRefBuilder->get_widget(
    "REV_MediumDelayEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_MediumTextChanged));

  //REV_Advanced
  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_AdvancedReverbPercentFunButtonClicked));


  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_AdvancedCombGainListFunButtonClicked));

  attributesRefBuilder->get_widget(
    "REV_LPGainListFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_AdvancedLPGainListFunButtonClicked));

  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_AdvancedGainAllPassFunButtonClicked));


  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::REV_AdvancedDelayFunButtonClicked));

  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_AdvancedTextChanged));

  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_AdvancedTextChanged));

  attributesRefBuilder->get_widget(
    "REV_LPGainListEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_AdvancedTextChanged));
  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_AdvancedTextChanged));
  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::REV_AdvancedTextChanged));

  //ReadSIVFile

  attributesRefBuilder->get_widget(
    "ReadSIVFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::readSIVFileTextChanged));



  //MakeSieve


  attributesRefBuilder->get_widget(
    "MakeSieveElementsMeaningfulRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveElementsModsRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveElementsFakeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget("MakeSieveElementsFibonacciRadioButton", radiobutton);
  radiobutton->signal_clicked()
    .connect(sigc::mem_fun(*this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsPeriodicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsHierarchicRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsIncludeRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));





  attributesRefBuilder->get_widget(
    "MakeSieveLowEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveHighEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));


  attributesRefBuilder->get_widget(
    "MakeSieveElementsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveOffsetEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveTextChanged));


  attributesRefBuilder->get_widget(
    "MakeSieveLowFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveLowFunButtonClicked));

  attributesRefBuilder->get_widget(
    "MakeSieveHighFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::makeSieveHighFunButtonClicked));




  //ReadSPAFile

  attributesRefBuilder->get_widget(
    "ReadSPAFileEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::readSPAFileTextChanged));



  //SPA
  attributesRefBuilder->get_widget(
    "SPAStereoRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SPAMethodRadioButtonClicked));
  attributesRefBuilder->get_widget(
    "SPAMulti_PanRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SPAMethodRadioButtonClicked));
  attributesRefBuilder->get_widget(
    "SPAPolarRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SPAMethodRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "SPAPartialRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SPAApplyByRadioButtonClicked));

  attributesRefBuilder->get_widget(
    "SPASoundRadioButton", radiobutton);
  radiobutton->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SPAApplyByRadioButtonClicked));

  //Stereo as default, apply by sound


  Gtk::HBox* hbox;


  SPAChannelAlignments = new SPAChannelAlignment(this, 1, 1);
  attributesRefBuilder->get_widget( "SPAMainHBox", hbox);
  hbox->pack_start(*SPAChannelAlignments, Gtk::PACK_SHRINK);
  SPAChannelAlignments->hideButtons();
  SPANumOfChannels = 1;
  SPANumOfPartials = 1;
  SPAApplyFlag = 0;
  SPAMethodFlag = 0;




  size_t locationOfKeyword;
  Gtk::TreeModel::iterator iter;
  Gtk::TreeModel::Row row;
  string parsingString;
  FileValue* value;
  list<FileValue>::iterator argumentsIter;



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

  //emptyString
  if (root ==NULL){
    return;
  }
  DOMElement* functionNameElement = root->getFirstElementChild();
  textData = ( DOMCharacterData*) functionNameElement->getFirstChild();

  if (textData){
    functionNameChars = XMLString::transcode(textData->getData());
    functionName = string(functionNameChars);
    XMLString::release(&functionNameChars);
  }

  if(functionName.compare("RandomInt")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;

    while(row[functionListColumns.m_col_name]!= "RandomInt"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomIntLowBoundEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomIntHighBoundEntry",entry);
    entry->set_text(getFunctionString(thisElement));

  }

  if(functionName.compare("Stochos")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Stochos"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

    thisElement = functionNameElement->getNextElementSibling();    //method
    int functionFlag;
    if (getFunctionString(thisElement)=="FUNCTIONS"){
      attributesRefBuilder->get_widget("StochosFunctionsRadioButton",radiobutton);
      radiobutton->set_active();
      functionFlag = 1;
    }
    else {
      attributesRefBuilder->get_widget("StochosRangeDistribRadioButton",radiobutton);
      radiobutton->set_active();
      functionFlag = 0;
    }

    thisElement = thisElement->getNextElementSibling();    //envelopes
    DOMElement* envelopeElement = thisElement->getFirstElementChild() ;//first envelope


    if (functionFlag ==1){ //functions
      while (envelopeElement != NULL){
        StochosSubAlignment* newSubAlignment =
            new StochosSubAlignment(this, 1);
        if ( stochosSubAlignments ==NULL){
          stochosSubAlignments = newSubAlignment;
        }
        else {
          stochosSubAlignments->appendNewNode(newSubAlignment);
        }
        Gtk::VBox* vbox;
        attributesRefBuilder->get_widget(
           "StochosInnerVBox", vbox);
        vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
        newSubAlignment->setFunctionsEntry(getFunctionString(envelopeElement));
        newSubAlignment->switchTo(1);
        stochosNumOfNodes ++;
        envelopeElement = envelopeElement->getNextElementSibling();
      }
    }
    else {
      while (envelopeElement != NULL){
        StochosSubAlignment* newSubAlignment =
            new StochosSubAlignment(this, 0);
        if ( stochosSubAlignments ==NULL){
          stochosSubAlignments = newSubAlignment;
        }
        else {
          stochosSubAlignments->appendNewNode(newSubAlignment);
        }
        Gtk::VBox* vbox;
        attributesRefBuilder->get_widget(
            "StochosInnerVBox", vbox);
        vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
        newSubAlignment->setMinEntry(getFunctionString(envelopeElement));
        envelopeElement = envelopeElement->getNextElementSibling();
        newSubAlignment->setMaxEntry(getFunctionString(envelopeElement));
        envelopeElement = envelopeElement->getNextElementSibling();
        newSubAlignment->setDistEntry(getFunctionString(envelopeElement));
        envelopeElement = envelopeElement->getNextElementSibling();
        newSubAlignment->switchTo(0);
        stochosNumOfNodes ++;
      }
    }
    stochosTextChanged();
    show_all_children();

    thisElement = thisElement->getNextElementSibling();    //offset
    attributesRefBuilder->get_widget("StochosOffsetEntry",entry);
    entry->set_text(getFunctionString(thisElement));
  }

  if(functionName.compare("Select")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Select"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

    thisElement = functionNameElement->getNextElementSibling(); //list

    // separate into a vector of strings
    std::vector<std::string> listElements = Utilities::listElementToStringVector(thisElement);
    // If the string isn't empty
    if (listElements.size() > 1 || listElements[0].compare("") != 0) {
      std::vector<std::string>::iterator functionString;
      for (functionString = listElements.begin();
            functionString != listElements.end(); ++functionString) {
        SelectSubAlignment* newSubAlignment = new SelectSubAlignment(this);
        if (selectSubAlignments == NULL){
          selectSubAlignments = newSubAlignment;
        }
        else {
          selectSubAlignments->appendNewNode(newSubAlignment);
        }
        Gtk::VBox* vbox;
        attributesRefBuilder->get_widget(
            "SelectInnerVBox", vbox);
        vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
        newSubAlignment->setFunctionsEntry(*functionString);
        selectNumOfNodes ++;
      }
    } //if
    selectEntryChanged();
    show_all_children();

    thisElement = thisElement->getNextElementSibling();    //offset
    attributesRefBuilder->get_widget("SelectIndexEntry",entry);
    entry->set_text(getFunctionString(thisElement));


/*
    //Select has 2 arguments
    // first argument is a list (in XML it's just a string, format: a,b,c,d...
    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("SelectListEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    // second argument is an int
    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("SelectIndexEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    //end parsing
*/
  }




  //check if CURRENT_TYPE
  if(functionName.compare("CURRENT_TYPE")==0){

    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_TYPE"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }

  //check if CURRENT_CHILD_NUM
   if(functionName.compare("CURRENT_CHILD_NUM")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_CHILD_NUM"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }

  //check if CURRENT_PARTIAL_NUM
  if(functionName.compare("CURRENT_PARTIAL_NUM")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_PARTIAL_NUM"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }

  //check if CURRENT_SEGMENT
 if(functionName.compare("CURRENT_SEGMENT")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_SEGMENT"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }

  //check if AVAILABLE_EDU
  if(functionName.compare("AVAILABLE_EDU")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "AVAILABLE_EDU"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }

  //check if PREVIOUS_CHILD_DURATION
  if(functionName.compare("PREVIOUS_CHILD_DURATION")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "PREVIOUS_CHILD_DURATION"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }



  //check if CURRENT_LAYER
  if(functionName.compare("CURRENT_LAYER")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "CURRENT_LAYER"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  }
  //check if ValuePick
   if(functionName.compare("ValuePick")==0){

    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ValuePick"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


    //ValuePick has 10 arguments.
    //Old version has 9 arguments (no offset, the last argument)

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ValuePickAbsRangeEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();    // second argument is an envelope (low)
    attributesRefBuilder->get_widget("ValuePickLowEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();    // third argument is an envelope (high)
    attributesRefBuilder->get_widget("ValuePickHighEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();    // fourth argument is an envelope (distribution)
    attributesRefBuilder->get_widget("ValuePickDistEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();  // 5th argument is a string  (Elements)

      if (getFunctionString(thisElement) == "MEANINGFUL"){
        attributesRefBuilder->get_widget(
          "ValuePickElementsMeaningfulRadioButton",radiobutton);
        radiobutton->set_active();

      }
      else if (getFunctionString(thisElement) == "MODS"){
        attributesRefBuilder->get_widget(
          "ValuePickElementsModsRadioButton",radiobutton);
        radiobutton->set_active();
      }

      else if (getFunctionString(thisElement) == "FAKE") {
        attributesRefBuilder->get_widget(
          "ValuePickElementsFakeRadioButton",radiobutton);
        radiobutton->set_active();
      } else { // fib
        attributesRefBuilder->get_widget("ValuePickElementsFibonacciRadioButton", radiobutton);
        radiobutton->set_active();
      }

      thisElement = thisElement->getNextElementSibling(); // 6th argument is a list without bracket

      attributesRefBuilder->get_widget("ValuePickElementsEntry",entry);
      entry->set_text(getFunctionString(thisElement));


      thisElement = thisElement->getNextElementSibling(); // 7th argument is a string  (weight)

      if (getFunctionString(thisElement) == "PERIODIC"){
        attributesRefBuilder->get_widget(
          "ValuePickWeightsPeriodicRadioButton",radiobutton);
        radiobutton->set_active();

      }
      else if (getFunctionString(thisElement) == "HIERARCHIC"){
        attributesRefBuilder->get_widget(
          "ValuePickWeightsHierarchicRadioButton",radiobutton);
        radiobutton->set_active();
      }

      else {  //Include
        attributesRefBuilder->get_widget(
          "ValuePickWeightsIncludeRadioButton",radiobutton);
        radiobutton->set_active();

      }

      thisElement = thisElement->getNextElementSibling();  // 8th argument is a list without bracket

      attributesRefBuilder->get_widget("ValuePickWeightsEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling(); // 9th argument is a string  (Elements)

      if (getFunctionString(thisElement) == "VARIABLE"){
        attributesRefBuilder->get_widget(
          "ValuePickTypeVariableRadioButton",radiobutton);
        radiobutton->set_active();

      }


      else {  //constant
        attributesRefBuilder->get_widget(
          "ValuePickTypeConstantRadioButton",radiobutton);
        radiobutton->set_active();

      }

       thisElement = thisElement->getNextElementSibling();
      //the 10th argument, offset, exists

        attributesRefBuilder->get_widget("ValuePickOffsetEntry",entry);

      entry->set_text(getFunctionString(thisElement));






    //end parsing
  }


  //check if ChooseL
  if(functionName.compare("ChooseL")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ChooseL"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ChooseLEntry",entry);
    entry->set_text(getFunctionString(thisElement));



    //end parsing
  }


  //check if GetPattern
  if(functionName.compare("GetPattern")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "GetPattern"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

      //getpattern has 3 argument

     thisElement = functionNameElement->getNextElementSibling();

     //first argument is a string (method)

      if (getFunctionString(thisElement) == "IN_ORDER"){
        attributesRefBuilder->get_widget(
          "GetPatternInOrderRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else if (getFunctionString(thisElement) == "OTHER"){
        attributesRefBuilder->get_widget("GetPatternOtherRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else if (getFunctionString(thisElement) == "TYPE_CLUSTERS"){
        attributesRefBuilder->get_widget(
          "GetPatternTypeClustersRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else if (getFunctionString(thisElement) == "TIME_DEPEND"){
        attributesRefBuilder->get_widget(
          "GetPatternTimeDependRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else{
        attributesRefBuilder->get_widget(
          "GetPatternProbabilityRadioButton",radiobutton);
        radiobutton->set_active();
      }



      thisElement = thisElement->getNextElementSibling();  // second argument is an integer

      attributesRefBuilder->get_widget("GetPatternOffsetEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling(); // third argument is a function


      attributesRefBuilder->get_widget("GetPatternEntry",entry);
      entry->set_text(getFunctionString(thisElement));


    //end parsing
  }


  if(functionName.compare("Random")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Random"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomLowBoundEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomHighBoundEntry",entry);
    entry->set_text(getFunctionString(thisElement));



  }

  //check if Randomizer
  if(functionName.compare("Randomizer")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Randomizer"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing
    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomizerBaseEntry",entry);
    entry->set_text(getFunctionString(thisElement));
    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomizerDeviationEntry",entry);
    entry->set_text(getFunctionString(thisElement));
    //end parsing
  }

  if (functionName.compare("RandomDensity") == 0) {
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while (row[functionListColumns.m_col_name] != "RandomDensity") {
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomDensityEnvelopeEntry", entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomDensityLowBoundEntry", entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("RandomDensityHighBoundEntry", entry);
    entry->set_text(getFunctionString(thisElement));
  }

  //check if Inverse
   if(functionName.compare("Inverse")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Inverse"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  //inverse has 1 argument

      thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("InverseEntry",entry);
    entry->set_text(getFunctionString(thisElement));



    //end parsing
  }

   // check if Markov
   if (functionName.compare("Markov") == 0) {
     iter = combobox->get_model()->get_iter("0");
     row = *iter;
     while  (row[functionListColumns.m_col_name] != "GetFromMarkovChain") {
       iter++;
       row = *iter;
     }
     combobox->set_active(iter);

     thisElement = functionNameElement->getNextElementSibling();
     attributesRefBuilder->get_widget("MarkovEntry", entry);
     entry->set_text(getFunctionString(thisElement));
   }

  //check if LN
   if(functionName.compare("LN")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "LN"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

      thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("LNEntry",entry);
    entry->set_text(getFunctionString(thisElement));



    //end parsing
  }

  //check if Fibonacci
   if(functionName.compare("Fibonacci")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Fibonacci"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

      thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("FibonacciEntry",entry);
    entry->set_text(getFunctionString(thisElement));



    //end parsing
  }
  //check if Decay
  if(functionName.compare("Decay")==0){

    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "Decay"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

  // start parsing

      //Decay has 4 argument

      // first argument is a float (base


    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("DecayBaseEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    thisElement = thisElement->getNextElementSibling();
   // 2nd argument is a string

      if (getFunctionString(thisElement) == "EXPONENTIAL"){
        attributesRefBuilder->get_widget(
          "DecayTypeExponentialRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else{
        attributesRefBuilder->get_widget(
          "DecayTypeLinearRadioButton",radiobutton);
        radiobutton->set_active();
      }

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("DecayRateEntry",entry);
    entry->set_text(getFunctionString(thisElement)); // 3nd argument is a string

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("DecayIndexEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    //end parsing
  }


  //check if EnvLib
  if(functionName.compare("EnvLib")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "EnvLib"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("EnvLibEnvelopeEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("EnvLibScalingFactorEntry",entry);
    entry->set_text(getFunctionString(thisElement));

  }


  //check if MakeEnvelope
  if(functionName.compare("MakeEnvelope")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeEnvelope"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    //begin parsing


    //MakeEnvelope has 5 arguments (string, list of envelopes, int)

    // first argument is a float list

      DOMElement* secondLevelElement;
      thisElement = functionNameElement->getNextElementSibling();
      secondLevelElement = thisElement->getFirstElementChild();

      makeEnvelopeSubAlignments->setXValueString(getFunctionString(secondLevelElement));
      secondLevelElement = secondLevelElement->getNextElementSibling();

      //makeEnvelopeInsertNode(MakeEnvelopeSubAlignment* _insertAfter){
      MakeEnvelopeSubAlignment* insertAfter = makeEnvelopeSubAlignments;


      while(secondLevelElement&&secondLevelElement->getNextElementSibling()){
        makeEnvelopeInsertNode(insertAfter);
        insertAfter = insertAfter->next;
        insertAfter->setXValueString(getFunctionString(secondLevelElement));
        secondLevelElement = secondLevelElement->getNextElementSibling();
      }


      attributesRefBuilder->get_widget("MakeEnvelopeXValueEntry",entry);
      entry->set_text(getFunctionString(secondLevelElement));

      thisElement = thisElement->getNextElementSibling();
      secondLevelElement = thisElement->getFirstElementChild();   // 2nd argument is a float list

      MakeEnvelopeSubAlignment* thisAlignment = makeEnvelopeSubAlignments;
      while(secondLevelElement&&secondLevelElement->getNextElementSibling()){
        thisAlignment->setYValueString(getFunctionString(secondLevelElement));
        thisAlignment = thisAlignment->next;
        secondLevelElement = secondLevelElement->getNextElementSibling();
      }

      set_position(Gtk::WIN_POS_CENTER);
      attributesRefBuilder->get_widget("MakeEnvelopeYValueEntry",entry);
      entry->set_text(getFunctionString(secondLevelElement));

      thisElement = thisElement->getNextElementSibling();
      secondLevelElement = thisElement->getFirstElementChild();// 3rd argument is a string list
      thisAlignment = makeEnvelopeSubAlignments;
      while(secondLevelElement){

        if (getFunctionString(secondLevelElement) == "LINEAR"){
          thisAlignment->setEnvSegmentType(0);
        }
        else if (getFunctionString(secondLevelElement) == "EXPONENTIAL"){
          thisAlignment->setEnvSegmentType(1);
        }
        else {
          thisAlignment->setEnvSegmentType(2);
        }
        thisAlignment = thisAlignment->next;
        secondLevelElement = secondLevelElement->getNextElementSibling();
      }



       thisElement = thisElement->getNextElementSibling();
      secondLevelElement = thisElement->getFirstElementChild(); // 4th argument is a string list
      thisAlignment = makeEnvelopeSubAlignments;
      while(secondLevelElement){
        if (getFunctionString(secondLevelElement) == "FLEXIBLE"){
          thisAlignment->setEnvSegmentProperty(0);
        }

        else {
          thisAlignment->setEnvSegmentProperty(1);
        }
        thisAlignment = thisAlignment->next;
        secondLevelElement = secondLevelElement->getNextElementSibling();
      }

      thisElement = thisElement->getNextElementSibling();  // 5th argument is a float

      attributesRefBuilder->get_widget("MakeEnvelopeScalingFactorEntry",entry);
      entry->set_text(getFunctionString(thisElement));


    //end parsing
  }

  //check if ReadENVFile
  if(functionName.compare("ReadENVFile")==0){
   iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadENVFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


      DOMElement* secondLevelElement;
      thisElement = functionNameElement->getNextElementSibling();

      attributesRefBuilder->get_widget("ReadENVFileEntry",entry);
      entry->set_text(getFunctionString(thisElement));



    //end parsing
  }


  //check if MakeList
   if(functionName.compare("MakeList")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeList"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  //inverse has 1 argument

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("MakeListFunctionEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("MakeListSizeEntry",entry);
    entry->set_text(getFunctionString(thisElement));
    //end parsing
  }

  /*
  // check if RawList
  locationOfKeyword = _originalString.find("<");
  if (int(locationOfKeyword)==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "RawList"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
  //TODO parse list

  }
  */
  //check if SPA
 if(functionName.compare("SPA")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "SPA"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
// start parsing



      value =&(*argumentsIter); // first argument is a string

    thisElement = functionNameElement->getNextElementSibling();

      if(getFunctionString(thisElement)=="STEREO"){
        attributesRefBuilder->get_widget("SPAStereoRadioButton",radiobutton);
        radiobutton->set_active();
      }

      else if (getFunctionString(thisElement)== "MULTI_PAN"){
        attributesRefBuilder->get_widget("SPAMulti_PanRadioButton",radiobutton);
        radiobutton->set_active();
      }

      else {   //polar
        attributesRefBuilder->get_widget("SPAPolarRadioButton",radiobutton);
        radiobutton->set_active();
      }

     thisElement = thisElement->getNextElementSibling();    // second argument is a string
      if(getFunctionString(thisElement)=="SOUND"){
        attributesRefBuilder->get_widget("SPASoundRadioButton",radiobutton);
        radiobutton->set_active();

      }

      else {   //partial
        attributesRefBuilder->get_widget("SPAPartialRadioButton",radiobutton);
        radiobutton->set_active();
      }

      thisElement = thisElement->getNextElementSibling();
      DOMElement* currentPartialsElement = thisElement->getFirstElementChild();
      DOMElement* currentPartialElement = currentPartialsElement->getFirstElementChild();

      SPAChannelAlignment* currentChannel = SPAChannelAlignments;
      SPAPartialAlignment* currentPartial = SPAChannelAlignments->partials;

      //make channels
      while(currentPartialsElement){
        if (currentPartialsElement->getNextElementSibling()){
          currentChannel->insertChannelButtonClicked();
          currentChannel = currentChannel->next;
        }
        currentPartialsElement = currentPartialsElement->getNextElementSibling();
      }
      //make partials
      while(currentPartialElement){
        if (currentPartialElement->getNextElementSibling()){
          currentPartial->insertPartialButtonClicked();
          currentPartial = currentPartial->next;
        }
        currentPartialElement = currentPartialElement->getNextElementSibling();
      }



      currentChannel = SPAChannelAlignments;
      currentPartialsElement = thisElement->getFirstElementChild();
      while (currentPartialsElement){
        currentPartialElement = currentPartialsElement->getFirstElementChild();
        currentPartial = currentChannel->partials;

        while(currentPartialElement){

          currentPartial->setText(getFunctionString(currentPartialElement));
          currentPartial = currentPartial->next;
          currentPartialElement = currentPartialElement->getNextElementSibling();
        }
        currentChannel = currentChannel->next;
        currentPartialsElement = currentPartialsElement->getNextElementSibling();
      }



      SPAApplyByRadioButtonClicked();



    set_position(Gtk::WIN_POS_CENTER);
    //end parsing
  }







  //check if ReadSPAFile
  if(functionName.compare("ReadSPAFile")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadSPAFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing
     // first argument is a string

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ReadSPAFileEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    //end parsing
  }

  //check if MakePattern
  if(functionName.compare("MakePattern")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakePattern"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


      //makepattern has 1 arguments
       // first argument is a list

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("MakePatternIntervalsEntry",entry);
    entry->set_text(getFunctionString(thisElement));



    //end parsing
  }

  //check if ExpandPattern
 if(functionName.compare("ExpandPattern")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ExpandPattern"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
     //first argument is the method

    thisElement = functionNameElement->getNextElementSibling();


      if (getFunctionString(thisElement) == "EQUIVALENCE"){
        attributesRefBuilder->get_widget(
          "ExpandPatternEquivalenceRadioButton",radiobutton);
        radiobutton->set_active();

      }
      else if (getFunctionString(thisElement) == "SYMMETRIES"){
        attributesRefBuilder->get_widget(
          "ExpandPatternSymmetriesRadioButton",radiobutton);
        radiobutton->set_active();
      }

      else {  //Distort
        attributesRefBuilder->get_widget(
          "ExpandPatternDistortRadioButton",radiobutton);
        radiobutton->set_active();

      }


      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("ExpandPatternModuloEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("ExpandPatternLowEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("ExpandPatternHighEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("ExpandPatternPatternEntry",entry);
      entry->set_text(getFunctionString(thisElement));



    //end parsing
  }

  //check if ReadPATFile

  if(functionName.compare("ReadPATFile")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadPATFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing
     // first argument is a string

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ReadPATFileNameEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    //end parsing
  }




  //check if REV_Simple
   if(functionName.compare("REV_Simple")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "REV_Simple"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);

    thisElement = functionNameElement->getNextElementSibling();    // one argument
    attributesRefBuilder->get_widget("REV_SimpleEntry",entry);
    entry->set_text(getFunctionString(thisElement));



    //end parsing
  }
  //check if REV_Medium
  if(functionName.compare("REV_Medium")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "REV_Medium"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing



      thisElement = functionNameElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_MediumReverbPercentEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_MediumHilowSpreadEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_MediumGainAllPassEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_MediumDelayEntry",entry);
      entry->set_text(getFunctionString(thisElement));



    //end parsing
  }

  //check if REV_Advanced
  if(functionName.compare("REV_Advanced")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "REV_Advanced"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);


      thisElement = functionNameElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_AdvancedReverbPercentEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_AdvancedCombGainListEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_LPGainListEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_AdvancedGainAllPassEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("REV_AdvancedDelayEntry",entry);
      entry->set_text(getFunctionString(thisElement));


    //end parsing
  }

  //check if ReadREVFile

  if(functionName.compare("ReadREVFile")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadREVFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing
     // first argument is a string

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ReadREVFileEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    //end parsing
  }

  //check if ReadFILFile

  if(functionName.compare("ReadFILFile")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadFILFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing
     // first argument is a string

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ReadFILFileEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    //end parsing
  }


  //check if MakeFilter

  if(functionName.compare("MakeFilter")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeFilter"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing

    thisElement = functionNameElement->getNextElementSibling();
    string type = getFunctionString(thisElement);

    if (type =="LPF"){
      attributesRefBuilder->get_widget(
          "MakeFilterLPFRadioButton",radiobutton);
        radiobutton->set_active();
    }
    else if (type =="HPF"){
      attributesRefBuilder->get_widget(
          "MakeFilterHPFRadioButton",radiobutton);
        radiobutton->set_active();
    }
    else if (type =="BPF"){
      attributesRefBuilder->get_widget(
          "MakeFilterBPFRadioButton",radiobutton);
        radiobutton->set_active();
    }
    else if (type =="LSF"){
      attributesRefBuilder->get_widget(
          "MakeFilterLSFRadioButton",radiobutton);
        radiobutton->set_active();
    }
    else if (type =="HSF"){
      attributesRefBuilder->get_widget(
          "MakeFilterHSFRadioButton",radiobutton);
        radiobutton->set_active();
    }
    else if (type =="NF"){
      attributesRefBuilder->get_widget(
          "MakeFilterNFRadioButton",radiobutton);
        radiobutton->set_active();
    }
    else if (type =="PBEQF"){
      attributesRefBuilder->get_widget(
          "MakeFilterPBEQFRadioButton",radiobutton);
        radiobutton->set_active();
    }


    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("MakeFilterFrequencyEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("MakeFilterBandWidthEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    thisElement = thisElement->getNextElementSibling();
    attributesRefBuilder->get_widget("MakeFilterDBGainEntry",entry);
    entry->set_text(getFunctionString(thisElement));

    //end parsing
  }

  //check if MakeSieve
 if(functionName.compare("MakeSieve")==0){

    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "MakeSieve"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
// start parsing
   // first argument is an int (low bound)



      thisElement = functionNameElement->getNextElementSibling();
      attributesRefBuilder->get_widget("MakeSieveLowEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("MakeSieveHighEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();


      if (getFunctionString(thisElement)== "MEANINGFUL"){
        attributesRefBuilder->get_widget(
          "MakeSieveElementsMeaningfulRadioButton",radiobutton);
        radiobutton->set_active();

      }
      else if (getFunctionString(thisElement) == "MODS"){
        attributesRefBuilder->get_widget(
          "MakeSieveElementsModsRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else if (getFunctionString(thisElement) == "FAKE"){
        attributesRefBuilder->get_widget(
          "MakeSieveElementsFakeRadioButton",radiobutton);
        radiobutton->set_active();
      }
      else {
        attributesRefBuilder->get_widget("MakeSieveElementsFibonacciRadioButton",radiobutton);
        radiobutton->set_active();
      }

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("MakeSieveElementsEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();


      if (getFunctionString(thisElement)== "PERIODIC"){
        attributesRefBuilder->get_widget(
          "MakeSieveWeightsPeriodicRadioButton",radiobutton);
        radiobutton->set_active();

      }
      else if (getFunctionString(thisElement)== "HIERARCHIC"){
        attributesRefBuilder->get_widget(
          "MakeSieveWeightsHierarchicRadioButton",radiobutton);
        radiobutton->set_active();
      }

      else {  //FAKE
        attributesRefBuilder->get_widget(
          "MakeSieveWeightsIncludeRadioButton",radiobutton);
        radiobutton->set_active();

      }

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("MakeSieveWeightsEntry",entry);
      entry->set_text(getFunctionString(thisElement));

      thisElement = thisElement->getNextElementSibling();
      attributesRefBuilder->get_widget("MakeSieveOffsetEntry",entry);
      entry->set_text(getFunctionString(thisElement));





    //end parsing
  }

  //check if ReadSIVFile





  if(functionName.compare("ReadSIVFile")==0){
    iter = combobox->get_model()->get_iter("0");
    row = *iter;
    while(row[functionListColumns.m_col_name]!= "ReadSIVFile"){
      iter++;
      row = *iter;
    }
    combobox->set_active(iter);
    // start parsing
     // first argument is a string

    thisElement = functionNameElement->getNextElementSibling();
    attributesRefBuilder->get_widget("ReadSIVFileEntry",entry);
    entry->set_text(getFunctionString(thisElement));


    //end parsing
  }








  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  textview->get_buffer()->set_text(_originalString);

  //show_all_children();



}

FunctionGenerator::~FunctionGenerator(){
  if (selectSubAlignments != NULL){
    selectSubAlignments->clear();
  }
  if (stochosSubAlignments != NULL){
    stochosSubAlignments->clear();
  }
  if (makeEnvelopeSubAlignments != NULL){
    makeEnvelopeSubAlignments->clear();
  }
  if (SPAChannelAlignments!=NULL){
    SPAChannelAlignments->clear();
  }
}




void FunctionGenerator::on_response (int response_id){
  hide();

  Gtk::TextView* textView;
  attributesRefBuilder->get_widget("resultStringTextView", textView);
  result = textView->get_buffer()->get_text();

}

std::string FunctionGenerator::getResultString(){
  return result;
}



void FunctionGenerator::function_list_combo_changed(){
  //declare pointers and initialize if needed;
  Gtk::Alignment* alignment;
  attributesRefBuilder->get_widget("FunctionParametersAlignment", alignment);
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::VBox* vbox;
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton;

  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("FunctionListComboBox", combobox);

  Gtk::TreeModel::iterator iter = combobox->get_active();
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    if(row)
    {
      //Get the data for the selected row, using our knowledge of the tree
      //model:

      CMODFunction function = row[functionListColumns.m_col_id];

      Gtk::Entry* entry;
      if (function == functionRandom){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("RandomVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("<Fun><Name>Random></Name><Low>0</Low><High>1</High></Fun>");
        attributesRefBuilder->get_widget(
          "RandomLowBoundEntry", entry);
        entry->set_text("0");


        attributesRefBuilder->get_widget(
          "RandomHighBoundEntry", entry);
        entry->set_text("1");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionRandomInt){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("RandomIntVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("<Fun><Name>RandomInt</Name><LowBound>0</LowBound><HighBound>1</HighBound></Fun>");
        attributesRefBuilder->get_widget(
          "RandomIntLowBoundEntry", entry);
        entry->set_text("0");


        attributesRefBuilder->get_widget(
          "RandomIntHighBoundEntry", entry);
        entry->set_text("1");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);
      }
      else if (function == functionRandomSeed){

        //TODO: implement the right behavior

      }
      else if (function == functionRandomizer ){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("RandomizerVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("<Fun><Name>Randomizer</Name><Base></Base><Deviation></Deviation></Fun>");
        attributesRefBuilder->get_widget(
          "RandomizerBaseEntry", entry);
        entry->set_text("");


        attributesRefBuilder->get_widget(
          "RandomizerDeviationEntry", entry);
        entry->set_text("");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);
      }
      else if (function == functionRandomDensity) {
        alignment->remove();
        attributesRefBuilder->get_widget("RandomDensityVBox", vbox);
        alignment->add(*vbox);
        textview->get_buffer()->set_text("<Fun><Name>RandomDensity</Name><Envelope></Envelope><Low></Low><High></High></Fun>");
      }
      else if (function == functionInverse ){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("InverseVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("<Fun><Name>Inverse</Name><Entry></Entry></Fun>");
        attributesRefBuilder->get_widget(
          "InverseEntry", entry);
        entry->set_text("");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);
      }
      else if (function == functionMarkov) {
        alignment->remove();
        attributesRefBuilder->get_widget("MarkovVBox", vbox);
        alignment->add(*vbox);
        // reset all data
        textview->get_buffer()->set_text("<Fun><Name>Markov</Name></Entry></Entry></Fun>");
        attributesRefBuilder->get_widget("MarkovEntry", entry);
        entry->set_text("");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400, 300);
      }

      else if (function == functionLN){
       alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("LNVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        textview->get_buffer()->set_text("<Fun><Name>LN</Name><Entry></Entry></Fun>");
        attributesRefBuilder->get_widget(
          "LNEntry", entry);
        entry->set_text("");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionFibonacci){
        alignment->remove(); //remove the current parameter box
         attributesRefBuilder->get_widget("FibonacciVBox", vbox);
         alignment->add (*vbox); //add random vbox in
         //reset all data
         textview->get_buffer()->set_text("<Fun><Name>Fibonacci</Name><Entry></Entry></Fun>");
         attributesRefBuilder->get_widget(
           "FibonacciEntry", entry);
         entry->set_text("");
         set_position(Gtk::WIN_POS_CENTER_ALWAYS);
         resize(400,300);

      }
      else if (function == functionDecay){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("DecayVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data

        textview->get_buffer()->set_text(
          "<Fun><Name>Decay</Name><Base></Base><Type>EXPONENTIAL</Type><Rate></Rate><Index>CURRENT_PARTIAL_NUM</Index></Fun>");
        attributesRefBuilder->get_widget(
          "DecayBaseEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "DecayRateEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "DecayIndexEntry", entry);
        entry->set_text("CURRENT_PARTIAL_NUM");

        attributesRefBuilder->get_widget(
          "DecayTypeExponentialRadioButton", radiobutton);
        radiobutton->set_active(true);
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionStochos){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("StochosVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data

        textview->get_buffer()->set_text(
          "<Fun><Name>Stochos</Name><Method>RANGE_DISTRIB</Method><Envelopes></Envelopes><Offset></Offset></Fun>");
        attributesRefBuilder->get_widget(
          "StochosOffsetEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "StochosRangeDistribRadioButton", radiobutton);
        radiobutton->set_active(true);
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(900,400);
      }
      else if (function == functionEnvLib){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("EnvLibVBox", vbox);
        alignment->add (*vbox); //addvbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "EnvLibEnvelopeEntry", entry);
        entry->set_text("");


        attributesRefBuilder->get_widget(
          "EnvLibScalingFactorEntry", entry);
        entry->set_text("1.0");

        envLibTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionSelect){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("SelectVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data

          textview->get_buffer()->set_text(
            "<Fun><Name>Select</Name><List></List><Index></Index></Fun>");
          attributesRefBuilder->get_widget(
            "SelectIndexEntry", entry);
          entry->set_text("");
          set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionValuePick){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ValuePickVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data

        attributesRefBuilder->get_widget(
          "ValuePickAbsRangeEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "ValuePickLowEntry", entry);
        entry->set_text("ENV");

        attributesRefBuilder->get_widget(
          "ValuePickHighEntry", entry);
        entry->set_text("ENV");

        attributesRefBuilder->get_widget(
          "ValuePickDistEntry", entry);
        entry->set_text("ENV");

        attributesRefBuilder->get_widget(
          "ValuePickElementsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");


        attributesRefBuilder->get_widget(
          "ValuePickWeightsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");

        attributesRefBuilder->get_widget(
          "ValuePickOffsetEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");

        valuePickTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);

        resize(500,400);
      }
      else if (function == functionChooseL){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ChooseLVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ChooseLEntry", entry);
        entry->set_text("SIV");

        chooseLEntryChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionGetPattern){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("GetPatternVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "GetPatternEntry", entry);
        entry->set_text("PAT");

        getPatternEntryChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionExpandPattern){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ExpandPatternVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ExpandPatternModuloEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "ExpandPatternLowEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "ExpandPatternHighEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "ExpandPatternPatternEntry", entry);
        entry->set_text("");

        expandPatternTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionAdjustPattern){
        //TODO: implement the right behavior

      }
      else if (function == functionMakeEnvelope){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeEnvelopeVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "MakeEnvelopeScalingFactorEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "MakeEnvelopeXValueEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "MakeEnvelopeYValueEntry", entry);
        entry->set_text("");


        makeEnvelopeTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(600,300);
      }
      else if (function == functionMakeSieve){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeSieveVBox", vbox);
        alignment->add (*vbox); //add vbox in
        //reset all data



        attributesRefBuilder->get_widget(
          "MakeSieveLowEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "MakeSieveHighEntry", entry);
        entry->set_text("");



        attributesRefBuilder->get_widget(
          "MakeSieveElementsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");


        attributesRefBuilder->get_widget(
          "MakeSieveWeightsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");

        attributesRefBuilder->get_widget(
          "MakeSieveOffsetEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");



        makeSieveTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(500,400);

      }
      else if (function == functionMakePattern){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakePatternVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data


        attributesRefBuilder->get_widget(
          "MakePatternIntervalsEntry", entry);
        entry->set_text("INT1, INT2, INT3 ...");

        makePatternTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionMakeList){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeListVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "MakeListFunctionEntry", entry);
        entry->set_text("");


        attributesRefBuilder->get_widget(
          "MakeListSizeEntry", entry);
        entry->set_text("");

        makeListTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);
      }
      else if (function == functionReadENVFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadENVFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadENVFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readENVFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionReadSIVFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadSIVFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadSIVFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readSIVFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionReadPATFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadPATFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadPATFileNameEntry", entry);
        entry->set_text("");
        entry->grab_focus();

        readPATFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionReadSPAFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadSPAFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadSPAFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readSPAFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionReadREVFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadREVFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadREVFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readREVFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionREV_Simple){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("REV_SimpleVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "REV_SimpleEntry", entry);
        entry->set_text("0.5");
        REV_SimpleEntryTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionREV_Medium){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("REV_MediumVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "REV_MediumReverbPercentEntry", entry);
        entry->set_text("ENV");
        attributesRefBuilder->get_widget(
          "REV_MediumHilowSpreadEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "REV_MediumGainAllPassEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "REV_MediumDelayEntry", entry);
        entry->set_text("");

        REV_MediumTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionREV_Advanced){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("REV_AdvancedVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "REV_AdvancedReverbPercentEntry", entry);
        entry->set_text("ENV");
        attributesRefBuilder->get_widget(
          "REV_AdvancedCombGainListEntry", entry);
        entry->set_text("0.46, 0.48, 0.50, 0.52, 0.53, 0.55");
        attributesRefBuilder->get_widget(
          "REV_LPGainListEntry", entry);
        entry->set_text("0.05, 0.06, 0.07, 0.05, 0.04, 0.02");

        attributesRefBuilder->get_widget(
          "REV_AdvancedGainAllPassEntry", entry);
        entry->set_text("");
        attributesRefBuilder->get_widget(
          "REV_AdvancedDelayEntry", entry);
        entry->set_text("");

        REV_AdvancedTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionSPA){

        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("SPAVBox", vbox);
        //if parse fail(or empty),
        //the label of the first entry should still be "Envelope"
        SPAApplyByRadioButtonClicked();
        alignment->add (*vbox); //add vbox in
        SPATextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionReadFILFile){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("ReadFILFileVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "ReadFILFileEntry", entry);
        entry->set_text("");
        entry->grab_focus();
        readFILFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }
      else if (function == functionMakeFilter){
        alignment->remove(); //remove the current parameter box
        attributesRefBuilder->get_widget("MakeFilterVBox", vbox);
        alignment->add (*vbox); //add random vbox in
        //reset all data
        attributesRefBuilder->get_widget(
          "MakeFilterDBGainEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "MakeFilterFrequencyEntry", entry);
        entry->set_text("");

        attributesRefBuilder->get_widget(
          "MakeFilterBandWidthEntry", entry);
        entry->set_text("");

        entry->grab_focus();
        readFILFileTextChanged();
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,300);

      }

      else if (function == function_staticCURRENT_TYPE){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>CURRENT_TYPE</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);
      }
      else if (function == function_staticCURRENT_CHILD_NUM){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>CURRENT_CHILD_NUM</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }
      else if (function == function_staticCURRENT_PARTIAL_NUM){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>CURRENT_PARTIAL_NUM</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }
      else if (function == function_staticCURRENT_DENSITY){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>CURRENT_DENSITY</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }
      else if (function == function_staticCURRENT_SEGMENT){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>CURRENT_SEGMENT</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }
      else if (function == function_staticAVAILABLE_EDU){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>AVAILABLE_EDU</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }
      else if (function == function_staticPREVIOUS_CHILD_DURATION){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>PREVIOUS_CHILD_DURATION</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }
      else if (function == function_staticCURRENT_LAYER){
        alignment->remove();
        textview->get_buffer()->set_text("<Fun><Name>CURRENT_LAYER</Name></Fun>");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);

      }

      else {  //not a function
        alignment->remove();
        textview->get_buffer()->set_text("");
        set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        resize(400,200);
      }



    }// end handling changed comboentry


  } //end if iter
  //else
    //std::cout << "invalid iter" << std::endl;
  show_all_children();
}


void FunctionGenerator::randomLowBoundFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomLowBoundEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat, entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;



}
void FunctionGenerator::randomHighBoundFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomHighBoundEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::randomEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomLowBoundEntry", entry);
  std::string stringbuffer = "<Fun><Name>Random</Name><Low>" + entry->get_text() + "</Low><High>";

  attributesRefBuilder->get_widget(
    "RandomHighBoundEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</High></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}






void FunctionGenerator::randomIntLowBoundFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomIntLowBoundEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;



}
void FunctionGenerator::randomIntHighBoundFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomIntHighBoundEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::randomIntEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomIntLowBoundEntry", entry);
  std::string stringbuffer = "<Fun><Name>RandomInt</Name><LowBound>" + entry->get_text() + "</LowBound><HighBound>";

  attributesRefBuilder->get_widget(
    "RandomIntHighBoundEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</HighBound></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}





void FunctionGenerator::randomizerBaseFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomizerBaseEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;



}
void FunctionGenerator::randomizerDeviationFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomizerDeviationEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::randomizerEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "RandomizerBaseEntry", entry);
  std::string stringbuffer = "<Fun><Name>Randomizer</Name><Base>" + entry->get_text() + "</Base><Deviation>";

  attributesRefBuilder->get_widget(
    "RandomizerDeviationEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</Deviation></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}


void FunctionGenerator::randomDensityEntryChanged() {
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("RandomDensityEnvelopeEntry", entry);
  std::stringstream ss;
  ss << "<Fun><Name>RandomDensity</Name><Envelope>" << entry->get_text() << "</Envelope>";
  attributesRefBuilder->get_widget("RandomDensityLowBoundEntry", entry);
  ss << "<LowBound>" << entry->get_text() << "</LowBound>";
  attributesRefBuilder->get_widget("RandomDensityHighBoundEntry", entry);
  ss << "<HighBound>" << entry->get_text() << "</HighBound>";
  ss << "</Fun>";
  textview->get_buffer()->set_text(ss.str());
}



void FunctionGenerator::decayBaseFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "DecayBaseEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;



}
void FunctionGenerator::decayRateFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "DecayRateEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::decayIndexFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "DecayIndexEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::decayTypeRadioButtonClicked(){
  decayTextChange();
}
void FunctionGenerator::decayEntryChanged(){
  decayTextChange();
}


void FunctionGenerator::decayTextChange(){
  Gtk::RadioButton* radiobutton;
  Gtk::Entry* entry;
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);


  attributesRefBuilder->get_widget(
    "DecayBaseEntry", entry);

  std::string stringbuffer = "<Fun><Name>Decay</Name><Base>" + entry->get_text()+ "</Base><Type>";

  attributesRefBuilder->get_widget(
    "DecayTypeExponentialRadioButton", radiobutton);

  if (radiobutton->get_active()){ //if exponential
    stringbuffer = stringbuffer + "EXPONENTIAL";
  }
  else { //linear
    stringbuffer = stringbuffer + "LINEAR";
  }
  attributesRefBuilder->get_widget(
    "DecayRateEntry", entry);
  stringbuffer = stringbuffer+ "</Type><Rate>" + entry->get_text()+ "</Rate><Index>";

  attributesRefBuilder->get_widget(
    "DecayIndexEntry", entry);
  stringbuffer = stringbuffer+ entry->get_text()+ "</Index></Fun>";

  textview->get_buffer()->set_text(stringbuffer);

}



void FunctionGenerator::inverseFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "InverseEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::inverseEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "InverseEntry", entry);
  std::string stringbuffer = "<Fun><Name>Inverse</Name><Entry>" + entry->get_text() +  "</Entry></Fun>";
  textview->get_buffer()->set_text(stringbuffer);
}

void FunctionGenerator::markovEntryChanged() {
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("MarkovEntry", entry);
  textview->get_buffer()->set_text(
    "<Fun><Name>Markov</Name><Entry>" + entry->get_text() + "</Entry></Fun>"
  );
}


void FunctionGenerator::LNFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "LNEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::LNEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "LNEntry", entry);
  std::string stringbuffer = "<Fun><Name>LN</Name><Entry>" + entry->get_text() +  "</Entry></Fun>";
  textview->get_buffer()->set_text(stringbuffer);
}

void FunctionGenerator::FibonacciFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "FibonacciEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::FibonacciEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "FibonacciEntry", entry);
  std::string stringbuffer = "<Fun><Name>Fibonacci</Name><Entry>" + entry->get_text() +  "</Entry></Fun>";
  textview->get_buffer()->set_text(stringbuffer);
}



/*
void FunctionGenerator::selectListFunButtonClicked(){
  Gtk::Entry* entry;
    FunctionGenerator* generator;
  attributesRefBuilder->get_widget(
    "SelectListEntry", entry);
  if (returnType == functionReturnInt){
    generator = new FunctionGenerator(functionReturnList,entry->get_text());
    generator->run();
  }
  else {  //return float
    generator = new FunctionGenerator(functionReturnList,entry->get_text());
    generator->run();
  }
  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}
*/

void FunctionGenerator::selectIndexFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;


}

void FunctionGenerator::selectEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);

  Gtk::Entry* entry;
  std::string stringbuffer = "<Fun><Name>Select</Name><List>";

  // Add first entry manually so list doesn't being with a comma
  SelectSubAlignment* current = selectSubAlignments;
  if (current != NULL) {
    stringbuffer = stringbuffer + current->toString();
    current = current ->next;
    while (current != NULL) {
      stringbuffer = stringbuffer + ", " + current->toString();
      current = current->next;
    }
  }

  attributesRefBuilder->get_widget(
    "SelectIndexEntry", entry);
  stringbuffer = stringbuffer + "</List><Index>" +entry->get_text() + "</Index></Fun>";

  textview->get_buffer()->set_text(stringbuffer);
}


void FunctionGenerator::selectAddNodeButtonClicked(){
  SelectSubAlignment* newSubAlignment =
    new SelectSubAlignment(this);
  if ( selectSubAlignments == NULL){
    selectSubAlignments = newSubAlignment;
  }
  else {
    selectSubAlignments->appendNewNode(newSubAlignment);
  }
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "SelectInnerVBox", vbox);
  vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
  selectEntryChanged();
  selectNumOfNodes ++;

  show_all_children();
}

void FunctionGenerator::selectFunButtonClicked(){
  Gtk::Entry* entry = dynamic_cast< Gtk::Entry * >(get_focus() );
  if( entry ){
    FunctionGenerator* generator =
      new FunctionGenerator(functionReturnMakeListFun, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() !=""&& result ==0){
      entry->set_text(generator->getResultString());
    }
    delete generator;
  }
  else {
    return;
  }
}

void FunctionGenerator::selectRemoveNode(SelectSubAlignment* _remove){
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "SelectInnerVBox", vbox);
  vbox->remove ( *_remove);
  selectNumOfNodes --;

  if (_remove == selectSubAlignments){ //if removing head
    selectSubAlignments = selectSubAlignments->next;
    if (selectSubAlignments != NULL){
      selectSubAlignments->prev = NULL;
    }
  }

  else {  //normal case
    _remove->prev->next = _remove->next;
    if (_remove->next != NULL){
      _remove->next->prev = _remove->prev;
    }
  }

  delete _remove;
  selectEntryChanged();
  show_all_children();

}


FunctionGenerator::SelectSubAlignment::SelectSubAlignment(
  FunctionGenerator* _parent){
  parent = _parent;
  next = NULL;
  prev = NULL;
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/FunGenSelectSubAlignment.ui");
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
    "./LASSIE/src/UI/FunGenSelectSubAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }
   #endif

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget("FunctionsHBox", hbox);
  add (*hbox);

  Gtk::Button* button;
  Gtk::Entry* entry;

  attributesRefBuilder->get_widget(
    "removeNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SelectSubAlignment::removeButtonClicked));

  attributesRefBuilder->get_widget(
    "FunctionsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SelectSubAlignment::textChanged));
  entry->set_text("");
}


FunctionGenerator::SelectSubAlignment::~SelectSubAlignment(){
}

void FunctionGenerator::SelectSubAlignment::appendNewNode(
  SelectSubAlignment* _newNode){
  if (next == NULL){
    next = _newNode;
    _newNode->prev = this;
  }

  else {
    next-> appendNewNode(_newNode);
  }
}

std::string FunctionGenerator::SelectSubAlignment::toString(){
  Gtk::Entry* entry;

  attributesRefBuilder->get_widget("FunctionsEntry", entry);
  return entry->get_text();
}

void FunctionGenerator::SelectSubAlignment::setFunctionsEntry(
  std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("FunctionsEntry", entry);
  entry->set_text(_string);
}

void FunctionGenerator::SelectSubAlignment::removeButtonClicked(){
  parent->selectRemoveNode(this);
}

void FunctionGenerator::SelectSubAlignment::textChanged(){
  parent->selectEntryChanged();
}

void FunctionGenerator::SelectSubAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;
}



void FunctionGenerator::stochosAddNodeButtonClicked(){
  StochosSubAlignment* newSubAlignment =
    new StochosSubAlignment(this, stochosMethodFlag);
  if ( stochosSubAlignments ==NULL){
    stochosSubAlignments = newSubAlignment;
  }
  else {
    stochosSubAlignments->appendNewNode(newSubAlignment);
  }
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "StochosInnerVBox", vbox);
  vbox->pack_start(*newSubAlignment,Gtk::PACK_SHRINK);
  stochosTextChanged();
  stochosNumOfNodes ++;

  show_all_children();
}



void FunctionGenerator::stochosTextChanged(){


  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::RadioButton* radiobutton;
  attributesRefBuilder->get_widget(
    "StochosRangeDistribRadioButton", radiobutton);

  Gtk::Entry* entry;

  std::string stringbuffer ="";
  stringbuffer = stringbuffer + "<Fun><Name>Stochos</Name><Method>" +
    ((radiobutton->get_active())?"RANGE_DISTRIB":"FUNCTIONS") + "</Method><Envelopes>";
  StochosSubAlignment* current = stochosSubAlignments;
  while (current != NULL){
    stringbuffer = stringbuffer + current->toString(stochosMethodFlag);
    current = current ->next;
  }

  attributesRefBuilder->get_widget(
    "StochosOffsetEntry", entry);

  stringbuffer = stringbuffer + "</Envelopes><Offset>"+ entry->get_text() + "</Offset></Fun>";

  textview->get_buffer()->set_text(stringbuffer);

}





void FunctionGenerator::stochosMethodRadioButtonClicked(){
  Gtk::RadioButton* radiobutton;
  Gtk::Entry* entry;
    attributesRefBuilder->get_widget(
      "StochosOffsetEntry", entry);
  attributesRefBuilder->get_widget(
    "StochosFunctionsRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stochosMethodFlag = 1;

    attributesRefBuilder->get_widget(
      "StochosOffsetEntry", entry);
    entry->set_sensitive(false);
    entry->set_text("0");

  }
  else {
    stochosMethodFlag = 0;
    entry->set_sensitive(true);
    entry->set_text("");
  }

  if (stochosSubAlignments!= NULL){
    stochosSubAlignments->switchTo( stochosMethodFlag);
    show_all_children();
  }

  stochosTextChanged();
}


void FunctionGenerator::stochosRemoveNode(StochosSubAlignment* _remove){
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget(
    "StochosInnerVBox", vbox);
  vbox->remove ( *_remove);
  stochosNumOfNodes --;

  if (_remove == stochosSubAlignments){ //if removing head
    stochosSubAlignments = stochosSubAlignments->next;
    if (stochosSubAlignments != NULL){
      stochosSubAlignments->prev = NULL;
    }
  }

  else {  //normal case

    _remove->prev->next = _remove->next;
    if (_remove->next != NULL){
      _remove->next->prev = _remove->prev;
    }
  }

  delete _remove;
  stochosTextChanged();
  show_all_children();

}

void FunctionGenerator::stochosFunButtonClicked(){


  Gtk::Entry* entry = dynamic_cast< Gtk::Entry * >(get_focus() );
  if( entry ){
    Gtk::Entry* offset;
    attributesRefBuilder->get_widget(
      "StochosOffsetEntry", offset);

    FunctionGenerator* generator =
      new FunctionGenerator((entry==offset)?
        functionReturnInt:
        functionReturnENV, entry->get_text());
    int result = generator->run();
    if (generator->getResultString() !=""&& result ==0){
      entry->set_text(generator->getResultString());
    }
    delete generator;
  }
  else {
    return;
  }
}


FunctionGenerator::StochosSubAlignment::StochosSubAlignment(
  FunctionGenerator* _parent, int _methodFlag){
  parent = _parent;
  next = NULL;
  prev = NULL;
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/FunGenStochosSubAlignment.ui");
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
    "./LASSIE/src/UI/FunGenStochosSubAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif
  if (_methodFlag == 0){
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("RangeDistribVBox", vbox);
    add(*vbox);

  }
  else {
    Gtk::HBox* hbox;
    attributesRefBuilder->get_widget("FunctionsHBox", hbox);
    add (*hbox);
  }


  Gtk::Button* button;
  Gtk::Entry* entry;



  attributesRefBuilder->get_widget(
    "removeNodeButton1", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::StochosSubAlignment::removeButtonClicked));

  attributesRefBuilder->get_widget(
    "removeNodeButton2", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, & FunctionGenerator::StochosSubAlignment::removeButtonClicked));


  attributesRefBuilder->get_widget(
    "FunctionsEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::StochosSubAlignment::textChanged));
  entry->set_text("ENV");


  attributesRefBuilder->get_widget(
    "MinEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::StochosSubAlignment::textChanged));
  entry->set_text("ENV");

  attributesRefBuilder->get_widget(
    "MaxEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::StochosSubAlignment::textChanged));
  entry->set_text("ENV");

  attributesRefBuilder->get_widget(
    "DistEntry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::StochosSubAlignment::textChanged));
  entry->set_text("ENV");

}


FunctionGenerator::StochosSubAlignment::~StochosSubAlignment(){
}

void FunctionGenerator::StochosSubAlignment::switchTo(int _methodFlag){
  remove();
  if (_methodFlag == 0){
    Gtk::VBox* vbox;
    attributesRefBuilder->get_widget("RangeDistribVBox", vbox);
    add(*vbox);

  }
  else {
    Gtk::HBox* hbox;
    attributesRefBuilder->get_widget("FunctionsHBox", hbox);
    add (*hbox);
  }
  if (next != NULL){
    next-> switchTo (_methodFlag);
  }
}

void FunctionGenerator::StochosSubAlignment::appendNewNode(
  StochosSubAlignment* _newNode){
  if (next == NULL){
    next = _newNode;
    _newNode->prev = this;
  }

  else {
    next-> appendNewNode(_newNode);
  }

}

std::string FunctionGenerator::StochosSubAlignment::toString( int _methodFlag){
  Gtk::Entry* entry;

  if (_methodFlag == 1){
    attributesRefBuilder->get_widget("FunctionsEntry", entry);
    return "<Envelope>"+ entry->get_text()+ "</Envelope>";
  }

  else {
    attributesRefBuilder->get_widget("MinEntry", entry);
    std::string stringbuffer = "<Envelope>" +entry->get_text() + "</Envelope>";
    attributesRefBuilder->get_widget("MaxEntry", entry);
    stringbuffer = stringbuffer + "<Envelope>"+entry->get_text() + "</Envelope>";
    attributesRefBuilder->get_widget("DistEntry", entry);
    stringbuffer = stringbuffer +"<Envelope>"+entry->get_text() + "</Envelope>";

    return stringbuffer;
  }
}

void FunctionGenerator::StochosSubAlignment::setFunctionsEntry(
  std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("FunctionsEntry", entry);
  entry->set_text(_string);
}

void FunctionGenerator::StochosSubAlignment::setMinEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("MinEntry", entry);
  entry->set_text(_string);
}


void FunctionGenerator::StochosSubAlignment::setMaxEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("MaxEntry", entry);
  entry->set_text(_string);
}


void FunctionGenerator::StochosSubAlignment::setDistEntry(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("DistEntry", entry);
  entry->set_text(_string);
}






void FunctionGenerator::StochosSubAlignment::removeButtonClicked(){
  parent->stochosRemoveNode(this);
}

void FunctionGenerator::StochosSubAlignment::textChanged(){
  parent->stochosTextChanged();
}


void FunctionGenerator::StochosSubAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;
}



void FunctionGenerator::valuePickAbsRangeFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::valuePickLowFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ValuePickLowEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::valuePickHighFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ValuePickHighEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::valuePickDistFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ValuePickDistEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

string FunctionGenerator::makeSieveNValuePickZeros(int _listLength){
  string toReturn = "";

  for (int i = 0; i < _listLength -1; i ++){
    toReturn = toReturn + "0, ";
  }
  toReturn = toReturn + "0";

  return toReturn;
}







void FunctionGenerator::valuePickTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton1;
  Gtk::RadioButton* radiobutton2;
  Gtk::RadioButton* radiobutton3;

  attributesRefBuilder->get_widget(
    "ValuePickAbsRangeEntry", entry);
  std::string stringbuffer = "<Fun><Name>ValuePick</Name><Range>"+ entry->get_text() + "</Range><Low>";

  attributesRefBuilder->get_widget(
    "ValuePickLowEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ "</Low><High>";

  attributesRefBuilder->get_widget(
    "ValuePickHighEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ "</High><Dist>";

  attributesRefBuilder->get_widget(
    "ValuePickDistEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ "</Dist><Method>";

  attributesRefBuilder->get_widget(
    "ValuePickElementsMeaningfulRadioButton", radiobutton1);

  attributesRefBuilder->get_widget(
    "ValuePickElementsModsRadioButton", radiobutton2);

  attributesRefBuilder->get_widget("ValuePickElementsFakeRadioButton", radiobutton3);

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "MEANINGFUL";

  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "MODS";
  }
  else if (radiobutton3->get_active()){
    stringbuffer = stringbuffer + "FAKE";
  } else {
    stringbuffer += "FIBONACCI";
  }


  attributesRefBuilder->get_widget(
    "ValuePickElementsEntry", entry);
  stringbuffer = stringbuffer + "</Method><Elements>" + entry->get_text()+ "</Elements><WeightMethod>";


  attributesRefBuilder->get_widget(
    "ValuePickWeightsPeriodicRadioButton", radiobutton1);

  attributesRefBuilder->get_widget(
    "ValuePickWeightsHierarchicRadioButton", radiobutton2);

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "PERIODIC";

  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "HIERARCHIC";
  }
  else{
    stringbuffer = stringbuffer + "INCLUDE";
  }

  attributesRefBuilder->get_widget(
    "ValuePickWeightsEntry", entry);
  stringbuffer = stringbuffer + "</WeightMethod><Weight>" + entry->get_text()+ "</Weight><Type>";

  attributesRefBuilder->get_widget(
    "ValuePickTypeVariableRadioButton", radiobutton1);

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "VARIABLE";
  }
  else {
    stringbuffer = stringbuffer + "CONSTANT";
  }

  attributesRefBuilder->get_widget(
    "ValuePickOffsetEntry", entry);
  stringbuffer = stringbuffer + "</Type><Offsets>" +entry->get_text()+ "</Offsets></Fun>";
  textview->get_buffer()->set_text(stringbuffer);

}





void FunctionGenerator::chooseLFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ChooseLEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnSIV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::chooseLEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ChooseLEntry", entry);
  std::string stringbuffer = "<Fun><Name>ChooseL</Name><Entry>" + entry->get_text() +  "</Entry></Fun>";
  textview->get_buffer()->set_text(stringbuffer);
}




void FunctionGenerator::getPatternOffsetFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "GetPatternOffsetEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::getPatternFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "GetPatternEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnPAT,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::getPatternEntryChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton;
  attributesRefBuilder->get_widget(
    "GetPatternOffsetEntry", entry);
  std::string stringbuffer = "<Fun><Name>GetPattern</Name><Method>";


  attributesRefBuilder->get_widget(
    "GetPatternInOrderRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "IN_ORDER";
  }

  attributesRefBuilder->get_widget(
    "GetPatternOtherRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "OTHER";
  }

  attributesRefBuilder->get_widget(
    "GetPatternTypeClustersRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "TYPE_CLUSTERS";
  }
  attributesRefBuilder->get_widget(
    "GetPatternTimeDependRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "TIME_DEPEND";
  }
  attributesRefBuilder->get_widget(
    "GetPatternProbabilityRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "PROBABILITY";
  }


  stringbuffer = stringbuffer + "</Method><Offset>" +entry->get_text() + "</Offset><Pattern>";
  attributesRefBuilder->get_widget(
    "GetPatternEntry", entry);

  stringbuffer = stringbuffer + entry->get_text() + "</Pattern></Fun>";



  textview->get_buffer()->set_text(stringbuffer);
}





void FunctionGenerator::makeListFunctionFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeListFunctionEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnMakeListFun,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::makeListSizeFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeListSizeEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makeListTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "MakeListFunctionEntry", entry);
  std::string stringbuffer = "<Fun><Name>MakeList</Name><Func>"+entry->get_text() + "</Func><Size>";

  attributesRefBuilder->get_widget(
    "MakeListSizeEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</Size></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}

/*

FunctionGenerator::RawListHBox::RawListHBox(FunctionGenerator* _parentGenerator){
  parentGenerator = _parentGenerator;
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file("./LASSIE/src/UI/RawList.ui");
  }
  catch (const Glib::FileError& ex){
    std::cerr << "FileError: " << ex.what() << std::endl;
  }
  catch (const Gtk::BuilderError& ex){
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }

   #else
  std::auto_ptr<Glib::Error> error;
  if (!attributesRefBuilder->add_from_file("./LASSIE/src/UI/RawList.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

}

*/



void FunctionGenerator::envLibEnvelopeFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::envLibScalingFactorFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::envLibTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "EnvLibEnvelopeEntry", entry);
  std::string stringbuffer = "<Fun><Name>EnvLib</Name><Env>" + entry->get_text() + "</Env><Scale>";

  attributesRefBuilder->get_widget(
    "EnvLibScalingFactorEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</Scale></Fun>";

  textview->get_buffer()->set_text(stringbuffer);
}


void FunctionGenerator::readENVFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "ReadENVFileEntry", entry);
  std::string stringbuffer = "<Fun><Name>ReadENVFile</Name><File>" + entry->get_text() + "</File></Fun>";


  textview->get_buffer()->set_text(stringbuffer);


}


FunctionGenerator::MakeEnvelopeSubAlignment::MakeEnvelopeSubAlignment(
  FunctionGenerator* _parent){
  parent = _parent;
  prev = NULL;
  next = NULL;

    attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/FunGenMakeEnvelopeSubAlignment.ui");
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
    "./LASSIE/src/UI/FunGenMakeEnvelopeSubAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget("mainVBox", vbox);
  add(*vbox);

  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegTypeComboBox", combobox);

  typeTreeModel = Gtk::ListStore::create(typeColumns);
  combobox->set_model(typeTreeModel);

  Gtk::TreeModel::Row row = *(typeTreeModel->append());
  row[typeColumns.m_col_type] = envSegmentTypeLinear;
  row[typeColumns.m_col_name] = "LINEAR";

  row = *(typeTreeModel->append());
  row[typeColumns.m_col_type] = envSegmentTypeExponential;
  row[typeColumns.m_col_name] = "EXPONENTIAL";

  row = *(typeTreeModel->append());
  row[typeColumns.m_col_type] = envSegmentTypeSpline;
  row[typeColumns.m_col_name] = "SPLINE";

  combobox->set_active(0);
  combobox->signal_changed().connect( sigc::mem_fun(
    *this,&FunctionGenerator::MakeEnvelopeSubAlignment::textChanged) );

    combobox->pack_start(typeColumns.m_col_name);


  attributesRefBuilder->get_widget("SegPropertyComboBox", combobox);

  propertyTreeModel = Gtk::ListStore::create(propertyColumns);
  combobox->set_model(propertyTreeModel);

  row = *(propertyTreeModel->append());
  row[propertyColumns.m_col_property] = envSegmentPropertyFlexible;
  row[propertyColumns.m_col_name] = "FLEXIBLE";

  row = *(propertyTreeModel->append());
  row[propertyColumns.m_col_property] = envSegmentPropertyFixed;
  row[propertyColumns.m_col_name] = "FIXED";

  combobox->set_active(0);
  combobox->signal_changed().connect( sigc::mem_fun(
    *this,&FunctionGenerator::MakeEnvelopeSubAlignment::textChanged) );

    combobox->pack_start(propertyColumns.m_col_name);

  Gtk::Button* button;

  attributesRefBuilder->get_widget(
    "XValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this,
    &FunctionGenerator::MakeEnvelopeSubAlignment::xValueFunButtonClicked));

  attributesRefBuilder->get_widget(
    "YValueFunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this,
    &FunctionGenerator::MakeEnvelopeSubAlignment::yValueFunButtonClicked));

  attributesRefBuilder->get_widget(
    "InsertNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this,
    &FunctionGenerator::MakeEnvelopeSubAlignment::insertNodeButtonClicked));

  attributesRefBuilder->get_widget(
    "RemoveNodeButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::MakeEnvelopeSubAlignment::removeButtonClicked));

  Gtk::Entry* entry;

  attributesRefBuilder->get_widget(
    "XValueEntry", entry);
      entry->set_text("");
  entry->signal_changed().connect(sigc::mem_fun(
    *this, &FunctionGenerator::MakeEnvelopeSubAlignment::textChanged));


  attributesRefBuilder->get_widget(
    "YValueEntry", entry);
      entry->set_text("");
  entry->signal_changed().connect(sigc::mem_fun(
    *this, &FunctionGenerator::MakeEnvelopeSubAlignment::textChanged));

}


FunctionGenerator::MakeEnvelopeSubAlignment::~MakeEnvelopeSubAlignment(){
}


std::string FunctionGenerator::MakeEnvelopeSubAlignment::getXValueString(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("XValueEntry", entry);
  return entry->get_text();
}

void FunctionGenerator::MakeEnvelopeSubAlignment::setXValueString(
  std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("XValueEntry", entry);
  return entry->set_text(_string);
}


std::string FunctionGenerator::MakeEnvelopeSubAlignment::getYValueString(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("YValueEntry", entry);
  return entry->get_text();
}

void FunctionGenerator::MakeEnvelopeSubAlignment::setYValueString(
  std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget("YValueEntry", entry);
  return entry->set_text(_string);
}


envSegmentType FunctionGenerator::MakeEnvelopeSubAlignment::getEnvSegmentType(){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegTypeComboBox", combobox);
  Gtk::TreeModel::iterator iter = combobox->get_active();
  Gtk::TreeModel::Row row = *iter;
  return row[typeColumns.m_col_type];
}

void FunctionGenerator::MakeEnvelopeSubAlignment::setEnvSegmentType(int _type){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegTypeComboBox", combobox);
  combobox->set_active(_type);
}

envSegmentProperty FunctionGenerator::
                   MakeEnvelopeSubAlignment::getEnvSegmentProperty(){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegPropertyComboBox", combobox);
  Gtk::TreeModel::iterator iter = combobox->get_active();
  Gtk::TreeModel::Row row = *iter;
  return row[propertyColumns.m_col_property];

}


void FunctionGenerator::MakeEnvelopeSubAlignment::setEnvSegmentProperty(
  int _property){
  Gtk::ComboBox* combobox;
  attributesRefBuilder->get_widget("SegPropertyComboBox", combobox);
  combobox->set_active(_property);


}


void FunctionGenerator::MakeEnvelopeSubAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;
}

void FunctionGenerator::MakeEnvelopeSubAlignment::xValueFunButtonClicked(){

  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "XValueEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::MakeEnvelopeSubAlignment::yValueFunButtonClicked(){

  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "YValueEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::MakeEnvelopeSubAlignment::insertNodeButtonClicked(){
  parent->makeEnvelopeInsertNode(this);

}

void FunctionGenerator::MakeEnvelopeSubAlignment::removeButtonClicked(){
  parent->makeEnvelopeRemoveNode(this);
}

void FunctionGenerator::MakeEnvelopeSubAlignment::textChanged(){
  parent->makeEnvelopeTextChanged();
}


void FunctionGenerator::makeEnvelopeTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;



  std::string stringbuffer = "<Fun><Name>MakeEnvelope</Name><Xs>";
  MakeEnvelopeSubAlignment* iter = makeEnvelopeSubAlignments;

  while (iter != NULL){
    stringbuffer = stringbuffer +"<X>" + iter->getXValueString()+"</X>";
    iter = iter->next;
  }

  attributesRefBuilder->get_widget( "MakeEnvelopeXValueEntry", entry);
  stringbuffer = stringbuffer +"<X>" + entry->get_text() + "</X></Xs><Ys>";

  iter = makeEnvelopeSubAlignments;
  while (iter != NULL){
    stringbuffer = stringbuffer + "<Y>" +iter->getYValueString()+"</Y>";
    iter = iter->next;
  }

  attributesRefBuilder->get_widget( "MakeEnvelopeYValueEntry", entry);

  stringbuffer = stringbuffer + "<Y>" +entry->get_text() + "</Y></Ys><Types>";
  iter = makeEnvelopeSubAlignments;
  while (iter != NULL){
    if(iter->getEnvSegmentType()==envSegmentTypeLinear){
      stringbuffer = stringbuffer + "<T>LINEAR</T>";
    }
    else if (iter->getEnvSegmentType()==envSegmentTypeExponential){
      stringbuffer = stringbuffer + "<T>EXPONENTIAL</T>";
    }
    else {
     stringbuffer = stringbuffer + "<T>SPLINE</T>";
    }
    iter = iter->next;
  }

  stringbuffer = stringbuffer + "</Types><Pros>";
  iter = makeEnvelopeSubAlignments;
  while (iter != NULL){
    if(iter->getEnvSegmentProperty()==envSegmentPropertyFlexible){
      stringbuffer = stringbuffer + "<P>FLEXIBLE</P>";
    }
    else {
     stringbuffer = stringbuffer + "<P>FIXED</P>";
    }

    iter = iter->next;
  }


  attributesRefBuilder->get_widget( "MakeEnvelopeScalingFactorEntry", entry);
  stringbuffer = stringbuffer+ "</Pros><Scale>" + entry->get_text() + "</Scale></Fun>";

  textview->get_buffer()->set_text(stringbuffer);

}

void FunctionGenerator::makeEnvelopeInsertNode(
  MakeEnvelopeSubAlignment* _insertAfter){
  MakeEnvelopeSubAlignment* newNode = new MakeEnvelopeSubAlignment(this);
  newNode->next = _insertAfter->next;
  newNode->prev = _insertAfter;
  _insertAfter->next = newNode;
  if (newNode->next != NULL){
    newNode->next->prev = newNode;
  }

  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget( "MakeEnvelopeInnerVBox", vbox);
  //vbox->clear();

  MakeEnvelopeSubAlignment* temp = makeEnvelopeSubAlignments;
  while (temp != NULL){
    vbox->remove(*temp);
    temp = temp->next;
  }

  temp = makeEnvelopeSubAlignments;
  while (temp != NULL){
    vbox->pack_start(*temp, Gtk::PACK_SHRINK);
    temp = temp->next;
  }




  makeEnvelopeTextChanged();
  makeEnvelopeNumOfNodes++;
  show_all_children();
}

void FunctionGenerator::makeEnvelopeRemoveNode(
  MakeEnvelopeSubAlignment* _remove){
  if (makeEnvelopeNumOfNodes ==1) {
    return; //not allow to remove the last node
  }

  if( makeEnvelopeSubAlignments == _remove){ //special case
    makeEnvelopeSubAlignments = makeEnvelopeSubAlignments->next;
    makeEnvelopeSubAlignments->prev = NULL;
  }

  else {
  _remove->prev->next = _remove->next;
    if (_remove->next != NULL){
      _remove->next->prev = _remove->prev;
    }
  }
  delete _remove;
  makeEnvelopeNumOfNodes--;
  Gtk::VBox* vbox;
  attributesRefBuilder->get_widget( "MakeEnvelopeInnerVBox", vbox);
  //vbox->clear();

  MakeEnvelopeSubAlignment* temp = makeEnvelopeSubAlignments;
  while (temp != NULL){
    vbox->pack_start(*temp, Gtk::PACK_SHRINK);
    temp = temp->next;
  }


  makeEnvelopeTextChanged();
  show_all_children();
}




void FunctionGenerator::makeEnvelopeScalingFactorFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeEnvelopeScalingFactorEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}






void FunctionGenerator::makeEnvelopeXValueFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeEnvelopeXValueEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makeEnvelopeYValueFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeEnvelopeYValueEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makePatternIntervalsFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakePatternIntervalsEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnList,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makePatternTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakePatternIntervalsEntry", entry);
  std::string stringbuffer = "<Fun><Name>MakePattern</Name><List>" + entry->get_text() + "</List></Fun>";
  textview->get_buffer()->set_text(stringbuffer);

}



void FunctionGenerator::expandPatternRadioButtonClicked(){
  expandPatternTextChanged();
}



void FunctionGenerator::expandPatternModuloFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ExpandPatternModuloEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::expandPatternLowFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ExpandPatternLowEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::expandPatternHighFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ExpandPatternHighEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::expandPatternPatternFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ExpandPatternPatternEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnPAT,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::expandPatternTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton;
  Gtk::RadioButton* radiobutton2;

  std::string stringbuffer = "<Fun><Name>ExpandPattern</Name><Method>";

  attributesRefBuilder->get_widget(
    "ExpandPatternEquivalenceRadioButton", radiobutton);
  attributesRefBuilder->get_widget(
    "ExpandPatternSymmetriesRadioButton", radiobutton2);

  if (radiobutton->get_active()){ //if equivalence
    stringbuffer = stringbuffer + "EQUIVALENCE";
  }
  else if (radiobutton2->get_active()){ // symmetries
    stringbuffer = stringbuffer + "SYMMETRIES";
  }
  else{
    stringbuffer = stringbuffer + "DISTORT";
  }



  attributesRefBuilder->get_widget(
    "ExpandPatternModuloEntry", entry);
  stringbuffer = stringbuffer + "</Method><Modulo>"+ entry->get_text() +  "</Modulo><Low>";
  attributesRefBuilder->get_widget(
    "ExpandPatternLowEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + "</Low><High>";

  attributesRefBuilder->get_widget(
    "ExpandPatternHighEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + "</High><Pattern>";

  attributesRefBuilder->get_widget(
    "ExpandPatternPatternEntry", entry);
  stringbuffer =stringbuffer + entry->get_text() + "</Pattern></Fun>";


  textview->get_buffer()->set_text(stringbuffer);

}




void FunctionGenerator::readPATFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "ReadPATFileNameEntry", entry);
  std::string stringbuffer ="<Fun><Name>ReadPATFile</Name><File>" + entry->get_text() +  "</File></Fun>";

  textview->get_buffer()->set_text(stringbuffer);

}


void FunctionGenerator::readFILFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "ReadFILFileEntry", entry);
   std::string stringbuffer ="<Fun><Name>ReadFILFile</Name><File>" + entry->get_text() +  "</File></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}


void FunctionGenerator::makeFilterTextChanged(){

  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton;


  std::string stringbuffer = "<Fun><Name>MakeFilter</Name><Type>";

  attributesRefBuilder->get_widget(
    "MakeFilterLPFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "LPF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(false);
      entry->set_text("");

  }

  attributesRefBuilder->get_widget(
    "MakeFilterHPFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "HPF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(false);
      entry->set_text("");
  }

  attributesRefBuilder->get_widget(
    "MakeFilterBPFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "BPF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(false);
      entry->set_text("");
  }

  attributesRefBuilder->get_widget(
    "MakeFilterHSFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "HSF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(true);

  }

  attributesRefBuilder->get_widget(
    "MakeFilterLSFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "LSF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(true);

  }

  attributesRefBuilder->get_widget(
    "MakeFilterNFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "NF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(false);
      entry->set_text("");

  }

  attributesRefBuilder->get_widget(
    "MakeFilterPBEQFRadioButton", radiobutton);
  if (radiobutton->get_active()){
    stringbuffer = stringbuffer + "PBEQF";
    attributesRefBuilder->get_widget(
      "MakeFilterDBGainEntry", entry);
      entry->set_sensitive(true);

  }



  attributesRefBuilder->get_widget(
    "MakeFilterFrequencyEntry", entry);

  stringbuffer = stringbuffer +"</Type><Frequency>" +entry->get_text()+ "</Frequency>";

  attributesRefBuilder->get_widget(
    "MakeFilterBandWidthEntry", entry);

  stringbuffer = stringbuffer +"<BandWidth>" +entry->get_text()+ "</BandWidth>";

  attributesRefBuilder->get_widget(
    "MakeFilterDBGainEntry", entry);

  stringbuffer = stringbuffer +"<dBGain>" +entry->get_text()+ "</dBGain></Fun>";


  textview->get_buffer()->set_text(stringbuffer);


}


void FunctionGenerator::makeFilterFrequencyFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeFilterFrequencyEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::makeFilterBandWidthFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeFilterBandWidthEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::makeFilterDBGainFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeFilterDBGainEntry", entry);

  if (entry->get_sensitive()==false) return;

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::readREVFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "ReadREVFileEntry", entry);
   std::string stringbuffer ="<Fun><Name>ReadREVFile</Name><File>" + entry->get_text() +  "</File></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}


void FunctionGenerator::REV_SimpleEntryFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_SimpleEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_SimpleEntryTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "REV_SimpleEntry", entry);
  std::string stringbuffer = "<Fun><Name>REV_Simple</Name><Size>" + entry->get_text() + "</Size></Fun>";


  textview->get_buffer()->set_text(stringbuffer);


}


void FunctionGenerator::REV_MediumReverbPercentFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumHilowSpreadFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumGainAllPassFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumDelayFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_MediumDelayEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_MediumTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "REV_MediumReverbPercentEntry", entry);
  std::string stringbuffer = "<Fun><Name>REV_Medium</Name><Percent>" + entry->get_text() + "</Percent><Spread>";

  attributesRefBuilder->get_widget(
    "REV_MediumHilowSpreadEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</Spread><AllPass>";

  attributesRefBuilder->get_widget(
    "REV_MediumGainAllPassEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</AllPass><Delay>";

  attributesRefBuilder->get_widget(
    "REV_MediumDelayEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</Delay></Fun>";
  textview->get_buffer()->set_text(stringbuffer);


}




void FunctionGenerator::REV_AdvancedReverbPercentFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_AdvancedCombGainListFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnList,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_AdvancedLPGainListFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_LPGainListEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnList,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}



void FunctionGenerator::REV_AdvancedGainAllPassFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}

void FunctionGenerator::REV_AdvancedDelayFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnFloat,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;
}



void FunctionGenerator::REV_AdvancedTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "REV_AdvancedReverbPercentEntry", entry);
  std::string stringbuffer = "<Fun><Name>REV_Advanced</Name><Percent>" + entry->get_text() + "</Percent><CombGainList>";

  attributesRefBuilder->get_widget(
    "REV_AdvancedCombGainListEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</CombGainList><LPGainList>";

  attributesRefBuilder->get_widget(
    "REV_LPGainListEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</LPGainList><AllPass>";

  attributesRefBuilder->get_widget(
    "REV_AdvancedGainAllPassEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</AllPass><Delay>";

  attributesRefBuilder->get_widget(
    "REV_AdvancedDelayEntry", entry);
  stringbuffer = stringbuffer + entry->get_text() + "</Delay></Fun>";
  textview->get_buffer()->set_text(stringbuffer);


}

void FunctionGenerator::readSIVFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "ReadSIVFileEntry", entry);
  std::string stringbuffer ="<Fun><Name>ReadSIVFile</Name><File>" + entry->get_text() +  "</File></Fun>";

  textview->get_buffer()->set_text(stringbuffer);


}





void FunctionGenerator::makeSieveLowFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeSieveLowEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}


void FunctionGenerator::makeSieveHighFunButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "MakeSieveHighEntry", entry);

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnInt,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::makeSieveTextChanged(){

  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;
  Gtk::RadioButton* radiobutton1;
  Gtk::RadioButton* radiobutton2;
  Gtk::RadioButton* radiobutton3;

  attributesRefBuilder->get_widget(
    "MakeSieveLowEntry", entry);
  std::string stringbuffer = "<Fun><Name>MakeSieve</Name><Low>"+ entry->get_text() + "</Low><High>";

  attributesRefBuilder->get_widget(
    "MakeSieveHighEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ "</High><Method>";


  attributesRefBuilder->get_widget(
    "MakeSieveElementsMeaningfulRadioButton", radiobutton1);

  attributesRefBuilder->get_widget(
    "MakeSieveElementsModsRadioButton", radiobutton2);

  attributesRefBuilder->get_widget("MakeSieveElementsFakeRadioButton", radiobutton3);

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "MEANINGFUL";

  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "MODS";
  }
  else if (radiobutton3->get_active()) {
    stringbuffer = stringbuffer + "FAKE";
  } else {
    stringbuffer += "FIBONACCI";
  }

  attributesRefBuilder->get_widget(
    "MakeSieveElementsEntry", entry);
  stringbuffer = stringbuffer + "</Method><Elements>" +entry->get_text()+ "</Elements><WeightMethod>";


  attributesRefBuilder->get_widget(
    "MakeSieveWeightsPeriodicRadioButton", radiobutton1);

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsHierarchicRadioButton", radiobutton2);

  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "PERIODIC";

  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer + "HIERARCHIC";
  }
  else{
    stringbuffer = stringbuffer + "INCLUDE";
  }

  attributesRefBuilder->get_widget(
    "MakeSieveWeightsEntry", entry);
  stringbuffer = stringbuffer + "</WeightMethod><Weight>"+entry->get_text()+ "</Weight><Offset>";


  attributesRefBuilder->get_widget(
    "MakeSieveOffsetEntry", entry);
  stringbuffer = stringbuffer + entry->get_text()+ "</Offset></Fun>";


  textview->get_buffer()->set_text(stringbuffer);


}


void FunctionGenerator::readSPAFileTextChanged(){
  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::Entry* entry;


  attributesRefBuilder->get_widget(
    "ReadSPAFileEntry", entry);
  std::string stringbuffer = "<Fun><Name>ReadSPAFile</Name><File>" + entry->get_text() + "</File></Fun>";


  textview->get_buffer()->set_text(stringbuffer);
}

FunctionGenerator::SPAPartialAlignment::SPAPartialAlignment(
  FunctionGenerator* _parent, SPAChannelAlignment* _parentChannel){
  parent = _parent;
  parentChannel = _parentChannel;
  prev = NULL;
  next = NULL;
  prevChan = NULL;
  nextChan = NULL;
  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/FunGenSPAPartialAlignment.ui");
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
    "./LASSIE/src/UI/FunGenSPAPartialAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget("MainHBox", hbox);
  add(*hbox);

  Gtk::Button* button;
  attributesRefBuilder->get_widget( "RemoveButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this,
    &FunctionGenerator::SPAPartialAlignment::removePartialButtonClicked));

  attributesRefBuilder->get_widget( "InsertButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this,
    &FunctionGenerator::SPAPartialAlignment::insertPartialButtonClicked));

  attributesRefBuilder->get_widget( "FunButton", button);
  button->signal_clicked().connect(sigc::mem_fun(
    *this, &FunctionGenerator::SPAPartialAlignment::funButtonClicked));


  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  entry->signal_changed().connect(sigc::mem_fun(
    *this, & FunctionGenerator::SPAPartialAlignment::textChanged));
}

FunctionGenerator::SPAPartialAlignment::~SPAPartialAlignment(){
}

void FunctionGenerator::SPAPartialAlignment::setNumber(int _number){
  std::stringstream out;
  out<< _number;
  std::string stringbuffer = "Partial " + out.str()+ " ";
  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);
  label->set_text(stringbuffer);
}

void FunctionGenerator::SPAPartialAlignment::clearPartialsInSameChannel(){
  if (next != NULL){
    next->clearPartialsInSameChannel();
  }
  delete this;
}


void FunctionGenerator::SPAPartialAlignment::clearPartialsWithSameNumber(){
  if (nextChan != NULL){
    nextChan->clearPartialsWithSameNumber();
  }
  delete this;

}

std::string FunctionGenerator::SPAPartialAlignment::getText(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  string stringbuffer = "<P>" + entry->get_text() + "</P>";
  return    stringbuffer;
}


void FunctionGenerator::SPAPartialAlignment::setText(std::string _string){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  entry->set_text(_string);
}



void FunctionGenerator::SPAPartialAlignment::textChanged(){
  parent->SPATextChanged();
}

void FunctionGenerator::SPAPartialAlignment::funButtonClicked(){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget(
    "entry", entry);
  if (!entry->get_sensitive()){
    return;
  }

  FunctionGenerator* generator =
    new FunctionGenerator(functionReturnENV,entry->get_text());
  generator->run();

  if (generator->getResultString() !=""){
    entry->set_text(generator->getResultString());
  }
  delete generator;

}

void FunctionGenerator::SPAPartialAlignment::insertPartialButtonClicked(){
  parent->SPAInsertPartial(this);
}

void FunctionGenerator::SPAPartialAlignment::removePartialButtonClicked(){
  parent->SPARemovePartial(this);
}

void FunctionGenerator::SPAPartialAlignment::entryEditSwitch(int _flag){
  Gtk::Entry* entry;
  attributesRefBuilder->get_widget( "entry", entry);
  if (_flag ==0){
    entry->set_sensitive(false);
  }
  else {
    entry->set_sensitive(true);
  }
}



FunctionGenerator::SPAChannelAlignment::SPAChannelAlignment(
  FunctionGenerator* _parent, int _numOfPartial, int _flagButtonsShown){
  parent = _parent;
  prev = NULL;
  next = NULL;
  flagButtonsShown = 1;


  attributesRefBuilder = Gtk::Builder::create();
  #ifdef GLIBMM_EXCEPTIONS_ENABLED
  try{
    attributesRefBuilder->add_from_file(
      "./LASSIE/src/UI/FunGenSPAChannelAlignment.ui");
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
    "./LASSIE/src/UI/FunGenSPAChannelAlignment.ui", error)){
    std::cerr << error->what() << std::endl;
  }

   #endif

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget("MainHBox", hbox);
  add(*hbox);

  attributesRefBuilder->get_widget("OuterVBox", outerVBox);
  attributesRefBuilder->get_widget("MainVBox", mainVBox);

  attributesRefBuilder->get_widget( "InsertChannelButton", insertButton);
  insertButton->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAChannelAlignment::insertChannelButtonClicked));

  attributesRefBuilder->get_widget( "RemoveChannelButton", removeButton);
  removeButton->signal_clicked().connect(sigc::mem_fun(*this, &FunctionGenerator::SPAChannelAlignment::removeChannelButtonClicked));

  if (_flagButtonsShown == 0){
    hideButtons();
  }

  partials = new SPAPartialAlignment(_parent, this);
  mainVBox->pack_start( *partials, Gtk::PACK_SHRINK);
  SPAPartialAlignment* temp1 = partials;
  SPAPartialAlignment* temp2;
  int counter = 1;
  while (counter < _numOfPartial){
    temp2 = new SPAPartialAlignment(_parent, this);
    temp1->next = temp2;
    temp2 ->prev = temp1;
    mainVBox->pack_start( *temp2, Gtk::PACK_SHRINK);
    temp1 = temp2;
    counter++;
  }
  refreshPartialNumbers();

}

FunctionGenerator::SPAChannelAlignment::~SPAChannelAlignment(){
  if (partials!= NULL){
    partials->clearPartialsInSameChannel();
  }
}

void FunctionGenerator::SPAChannelAlignment::setChannelTitle(
  std::string _title){
  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);
  label ->set_text (_title);
}

void FunctionGenerator::SPAChannelAlignment::setChannelNumber(int _number){

  std::stringstream out;
  out<< _number;

  std::string buffer = "Channel "+out.str();

  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);
  label ->set_text (buffer);

}




void FunctionGenerator::SPAChannelAlignment::hideButtons(){
  if (flagButtonsShown ==0 ){
    return;
  }
  outerVBox->remove(*removeButton);
  outerVBox->remove(*insertButton);
  flagButtonsShown = 0;
  show_all_children();
}

void FunctionGenerator::SPAChannelAlignment::showButtons(){
  if (flagButtonsShown ==1){
    return;
  }
  outerVBox->remove(*mainVBox);
  outerVBox->pack_start(*insertButton,Gtk::PACK_SHRINK);
  outerVBox->pack_start(*removeButton,Gtk::PACK_SHRINK);
  outerVBox->pack_start(*mainVBox,Gtk::PACK_EXPAND_WIDGET);
  flagButtonsShown = 1;
  show_all_children();
}

void FunctionGenerator::SPAChannelAlignment::clear(){
  if (next != NULL){
    next->clear();
  }
  delete this;
}

void FunctionGenerator::SPAChannelAlignment::refreshPartialNumbers(){
  int counter = 1;
  SPAPartialAlignment* current = partials;
  while (current != NULL){
    current -> setNumber(counter);
    counter ++;
    current = current ->next;
  }
}

void FunctionGenerator::SPAChannelAlignment::refreshLayout(){
  SPAPartialAlignment* current = partials;
  while (current != NULL){
    if (current->get_parent()==mainVBox){
      mainVBox->remove(*current);
    }
    current = current ->next;
  }
  current = partials;
  while (current!= NULL){
    mainVBox->pack_start(*current,Gtk::PACK_SHRINK);
    current = current ->next;
  }
}



std::string FunctionGenerator::SPAChannelAlignment::getText(){
  std::string stringbuffer = "<Partials>";
  SPAPartialAlignment* temp = partials;
  while (temp != NULL){
    stringbuffer = stringbuffer + temp->getText();
    temp = temp ->next;
  }
  stringbuffer = stringbuffer + "</Partials>";
  return stringbuffer;

}

int FunctionGenerator::SPAChannelAlignment::getButtonsShownStatus(){
  return flagButtonsShown;
}

void FunctionGenerator::SPAChannelAlignment::insertChannelButtonClicked(){
  parent->SPAInsertChannel(this);
}

void FunctionGenerator::SPAChannelAlignment::removeChannelButtonClicked(){
  parent->SPARemoveChannel(this);
}


void FunctionGenerator::SPARemoveChannel(SPAChannelAlignment* _remove){
  if (SPAMethodFlag!= 1||SPANumOfChannels ==1){
    return;
  }

  SPANumOfChannels --;

  if (_remove == SPAChannelAlignments){ //specialCase
    SPAChannelAlignments = SPAChannelAlignments->next;
    SPAChannelAlignments->prev = NULL;
    SPAPartialAlignment* partial = SPAChannelAlignments->partials;
    while (partial!= NULL){
      partial->prevChan = NULL;
      partial = partial->next;
    }
  }

  else { //normal cases
    _remove->prev->next = _remove->next;

    SPAPartialAlignment* partial = _remove->partials;
    while (partial!= NULL){
      partial->prevChan->nextChan = partial->nextChan;
      partial = partial->next;
    }
    if (_remove->next!= NULL){
      _remove->next->prev = _remove->prev;
      partial = _remove->partials;
      while (partial!= NULL){
        partial->nextChan->prevChan = partial->prevChan;
        partial = partial->next;
      }
    }
  }

  delete _remove;

  int counter = 1;
  SPAChannelAlignment* current = SPAChannelAlignments;
  while (current != NULL){
    current ->setChannelNumber( counter);
    current = current ->next;
    counter++;
  }
  SPATextChanged();
  show_all_children();
}

void FunctionGenerator::SPAInsertChannel(SPAChannelAlignment* _insertAfter){
  if (SPAMethodFlag!= 1){
    return;
  }

  Gtk::HBox* hbox;
  attributesRefBuilder->get_widget( "SPAMainHBox", hbox);
  SPAChannelAlignment* temp = SPAChannelAlignments;
  while (temp != NULL){
    if(temp->get_parent()==hbox){
      hbox->remove(*temp);
    }
    temp = temp->next;
  }





  SPANumOfChannels ++;
  SPAChannelAlignment* newChannel =
    new SPAChannelAlignment(this,
                            SPANumOfPartials,
                            _insertAfter->getButtonsShownStatus());

  newChannel->prev = _insertAfter;
  newChannel->next = _insertAfter->next;
  _insertAfter->next = newChannel;
  if (newChannel->next != NULL){
    newChannel->next->prev = newChannel;
  }

  SPAPartialAlignment* temp1 = _insertAfter->partials;
  SPAPartialAlignment* temp2 = newChannel->partials;

  while (temp1 != NULL){
    temp2->nextChan = temp1->nextChan;
    temp1->nextChan = temp2;
    temp2->prevChan = temp1;
    temp1 = temp1->next;
    temp2 = temp2->next;
  }

  temp2 = newChannel->partials;
  if (temp2->nextChan != NULL){
    while (temp2 != NULL){
      temp2->nextChan->prevChan = temp2;
      temp2 = temp2->next;
    }
  }


  temp = SPAChannelAlignments;
  while (temp !=NULL){
    hbox->pack_start(*temp,Gtk::PACK_SHRINK);
    temp = temp->next;
  }

    int counter = 1;
  SPAChannelAlignment* current = SPAChannelAlignments;
  while (current != NULL){
    current ->setChannelNumber( counter);
    current = current ->next;
    counter++;
  }
  SPAApplyByRadioButtonClicked();
  SPATextChanged();

  show_all_children();
}

void FunctionGenerator::SPARemovePartial(SPAPartialAlignment* _remove){

  if (SPANumOfPartials==1||SPAApplyFlag==0){
    return;
  }

  SPANumOfPartials --;
  SPAPartialAlignment* leftmost = _remove;
  while (leftmost->prevChan!= NULL){
    leftmost = leftmost->prevChan;
  }


  SPAPartialAlignment* temp = leftmost;

  if (temp->prev ==NULL){//specialCase
    while (temp!= NULL){
      temp->parentChannel->partials = temp->next;
      temp->next->prev = NULL;
      temp = temp->nextChan;
    }

  }
  else {// normal case
    while (temp != NULL){
      temp->prev->next = temp->next;
      if (temp->next!= NULL){
        temp->next->prev = temp->prev;
      }
      temp = temp->nextChan;
    }

  }

  SPAChannelAlignment* temp1 = SPAChannelAlignments;

  while (temp1 != NULL){
    temp1->refreshPartialNumbers();
    temp1 = temp1->next;
  }

  leftmost->clearPartialsWithSameNumber();
  SPATextChanged();
  show_all_children();



}


void FunctionGenerator::SPAInsertPartial (SPAPartialAlignment* _insertAfter){
  if (SPAApplyFlag== 0){
    return;
  }

  SPANumOfPartials ++;
  SPAPartialAlignment* leftmost = _insertAfter;
  while (leftmost->prevChan!= NULL){
    leftmost = leftmost->prevChan;
  }


  SPAPartialAlignment* temp1 = leftmost;
  SPAPartialAlignment* temp2 = NULL;
  SPAPartialAlignment* newPartial;

  while (temp1!= NULL){
    newPartial = new SPAPartialAlignment( this, temp1->parentChannel);
    newPartial->next = temp1->next;

    temp1->next = newPartial;
    newPartial->prev = temp1;
    if (newPartial->next != NULL){
      newPartial->next->prev = newPartial;
    }
    newPartial->prevChan = temp2;
    if (temp2!= NULL){
      temp2->nextChan = newPartial;
    }
    temp2 = newPartial;
    temp1 = temp1->nextChan;
  }

  SPAChannelAlignment* temp = SPAChannelAlignments;

  while (temp != NULL){
    temp->refreshPartialNumbers();

    temp->refreshLayout();

    temp = temp->next;
  }

  SPATextChanged();
  show_all_children();

}

void FunctionGenerator::SPAMethodRadioButtonClicked(){
  Gtk::RadioButton* stereo;
  Gtk::RadioButton* multi_pan;

  attributesRefBuilder->get_widget("SPAStereoRadioButton", stereo);
  attributesRefBuilder->get_widget("SPAMulti_PanRadioButton", multi_pan);


  if (stereo->get_active()){
    if (SPAMethodFlag ==0){
      return;
    }
    SPAChannelAlignment* toDelete =SPAChannelAlignments->next;

    if (toDelete != NULL){
      toDelete->clear();

      SPAPartialAlignment* partial = SPAChannelAlignments->partials;

      while (partial!= NULL){
        partial->nextChan = NULL;
        partial= partial->next;
      }
    }
    SPAChannelAlignments->next = NULL;

    SPAChannelAlignments->setChannelTitle("");
    SPANumOfChannels = 1;
    SPAMethodFlag = 0;
    SPAChannelAlignments->hideButtons();

  }//end stereo
  else if (multi_pan->get_active()){
    if (SPAMethodFlag ==1){
      return;
    }

    SPAMethodFlag = 1;
    SPAChannelAlignments->setChannelTitle("Channel 1");
    SPAChannelAlignments->showButtons();
    if (SPAChannelAlignments->next != NULL){

      SPAChannelAlignments->next->setChannelTitle("Channel 2");
      SPAChannelAlignments->next->showButtons();
    }

  }//end multipan
  else{ //polar
    if (SPAMethodFlag ==2){
      return;
    }

    if (SPANumOfChannels ==1){

      SPAMethodFlag =1; //just for excuting next statement;
      SPAInsertChannel(SPAChannelAlignments);

    }
    else if (SPANumOfChannels >2){

      SPAChannelAlignments->next->next->clear();

      SPAPartialAlignment* partial = SPAChannelAlignments->next->partials;

      while (partial!= NULL){
        partial->nextChan = NULL;
        partial= partial->next;
      }
    }


    SPANumOfChannels = 2;
    SPAMethodFlag = 2;
    SPAChannelAlignments->setChannelTitle("Theta");
    SPAChannelAlignments->next->setChannelTitle("Radius");
    SPAChannelAlignments->hideButtons();
    SPAChannelAlignments->next->hideButtons();
    SPAChannelAlignments->next->next = NULL;
  }//end polar

  SPATextChanged();
  show_all_children();

}

void FunctionGenerator::SPAPartialAlignment::setLabel(string _label){
  Gtk::Label* label;
  attributesRefBuilder->get_widget( "TitleLabel", label);
  label->set_text(_label);


}
void FunctionGenerator::SPAApplyByRadioButtonClicked(){
  Gtk::RadioButton* radiobutton;
  attributesRefBuilder->get_widget( "SPASoundRadioButton", radiobutton);

  if (radiobutton->get_active()){ //applied by sound
    SPAApplyFlag = 0;
    SPAChannelAlignment* temp = SPAChannelAlignments;

    while (temp != NULL){ //each channel
      SPAPartialAlignment* firstPartial = temp->partials;
      if (firstPartial!=NULL){
        firstPartial->setLabel("Envelope");
      }

      SPAPartialAlignment* partial = temp->partials->next;
      while (partial!= NULL){
        partial->entryEditSwitch(0);
        partial = partial->next;
      }
      temp = temp->next;
    }

  }
  else {//applied by partial
    SPAApplyFlag = 1;
    SPAChannelAlignment* temp = SPAChannelAlignments;

    while (temp != NULL){
      SPAPartialAlignment* firstPartial = temp->partials;
      if (firstPartial!=NULL){
        firstPartial->setLabel("Partial 1");
      }


      SPAPartialAlignment* partial = temp->partials->next;
      while (partial!= NULL){
        partial->entryEditSwitch(1);
        partial = partial->next;
      }
      temp = temp->next;
    }
  }
  SPATextChanged();
  show_all_children();
}



void FunctionGenerator::SPATextChanged(){
  std::string stringbuffer = "";
  int method;
  int apply;

  Gtk::TextView* textview;
  attributesRefBuilder->get_widget("resultStringTextView", textview);
  Gtk::RadioButton* radiobutton1;
  Gtk::RadioButton* radiobutton2;
  attributesRefBuilder->get_widget( "SPAStereoRadioButton", radiobutton1);
  attributesRefBuilder->get_widget( "SPAMulti_PanRadioButton", radiobutton2);
  stringbuffer = "<Fun><Name>SPA</Name><Method>";
  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer +"STEREO";
    method = 0;
  }
  else if (radiobutton2->get_active()){
    stringbuffer = stringbuffer +"MULTI_PAN";
    method = 1;
  }
  else {
    stringbuffer = stringbuffer +"POLAR";
    method = 2;
  }
  stringbuffer = stringbuffer + "</Method><Apply>";
  attributesRefBuilder->get_widget( "SPASoundRadioButton", radiobutton1);
  if (radiobutton1->get_active()){
    stringbuffer = stringbuffer + "SOUND";
    apply = 0;
  }
  else {
    stringbuffer = stringbuffer + "PARTIAL";
    apply =1;
  }

  stringbuffer = stringbuffer + "</Apply><Channels>";

  SPAChannelAlignment* temp = SPAChannelAlignments;
  while (temp != NULL){
        stringbuffer = stringbuffer + temp->getText();
        temp = temp->next;
  }

  stringbuffer = stringbuffer + "</Channels></Fun>";
  /*
  if (method ==0){
    if (apply == 0){
      stringbuffer =
        stringbuffer + "<" + SPAChannelAlignments->partials->getText()+ ">>";
    }
    else {
      stringbuffer = stringbuffer + "<" + SPAChannelAlignments->getText()+ ">>";
    }
  }//end method = 0
  else if (method ==1){
    if (apply ==0){
      stringbuffer = stringbuffer + "<<";
      SPAChannelAlignment* temp = SPAChannelAlignments;
      while (temp != NULL){
        stringbuffer = stringbuffer + temp->partials->getText();
        if (temp->next!=NULL){
          stringbuffer = stringbuffer + ">, <";
        }
        temp = temp->next;
      }
      stringbuffer = stringbuffer + ">>>";
    }
    else {
      stringbuffer = stringbuffer + "<<";
      SPAChannelAlignment* temp = SPAChannelAlignments;
      while (temp != NULL){
        stringbuffer = stringbuffer + temp->getText();
        if (temp->next!=NULL){
          stringbuffer = stringbuffer + ">, <";
        }
        temp = temp->next;
      }
      stringbuffer = stringbuffer + ">>>";

    }



  }//end method = 1

  else { //method = 2
  if (apply ==0){
      stringbuffer = stringbuffer + "<";
      SPAChannelAlignment* temp = SPAChannelAlignments;
      while (temp != NULL){
        stringbuffer = stringbuffer + temp->partials->getText();
        if (temp->next!=NULL){
          stringbuffer = stringbuffer + ">, <";
        }
        temp = temp->next;
      }
      stringbuffer = stringbuffer + ">>";
    }
    else {
      stringbuffer = stringbuffer + "<";
      SPAChannelAlignment* temp = SPAChannelAlignments;
      while (temp != NULL){
        stringbuffer = stringbuffer + temp->getText();
        if (temp->next!=NULL){
          stringbuffer = stringbuffer + ">, <";
        }
        temp = temp->next;
      }
      stringbuffer = stringbuffer + ">>";

    }

  }

  */
    textview->get_buffer()->set_text(stringbuffer);


}



std::string FunctionGenerator::getFunctionString(
  FileValue* _value,FunctionReturnType _returnType){
  std::string stringbuffer;
  char charbuffer[20];
  if (_value->isFunction()){
    stringbuffer= _value->getFtnString();

    bool isNotStatic = (stringbuffer.find("CURRENT") == string::npos &&
        stringbuffer.find("AVAILA") == string::npos &&
        stringbuffer.find("PREVIOUS_CHILD_DURATION") == string::npos
        );

    if (isNotStatic){
      stringbuffer = stringbuffer + "(";
    }
    list<FileValue> arguments = _value->getFtnArgs();
    list<FileValue>::iterator iter = arguments.begin();
    FileValue* argument;
    while (iter != arguments.end()){
      argument = &(*iter);
      stringbuffer = stringbuffer + getFunctionString (argument, _returnType);

      iter++;
      if (iter != arguments.end()){
        stringbuffer = stringbuffer + ",";
      }
    }

    if (isNotStatic){
      stringbuffer = stringbuffer + ")";
    }

  }
  else if (_value->isNumber()){
    if (_returnType == functionReturnInt){
      int intNumber = _value->getInt();
      sprintf( charbuffer, "%d", intNumber);
      stringbuffer = string(charbuffer);
    }
    else { //float
      float floatNumber = _value->getFloat();

      if (floatNumber -(int)floatNumber ==0){
        int intNumber = _value->getInt();
        sprintf( charbuffer, "%d", intNumber);
        stringbuffer = string(charbuffer);
      }
      else {
        sprintf( charbuffer, "%.5f", floatNumber);
        stringbuffer = string(charbuffer);
      }
    }
  }

  else if (_value->isString()){
    stringbuffer ="\""+ _value->getString() + "\"";
   // stringbuffer =_value->getString();
  }
  else if (_value->isList()){
    stringbuffer = fileValueListToString(_value->getList(), _returnType);


  }

  return stringbuffer;
}



list<std::string> FunctionGenerator::fileValueListToStringList(
  list<FileValue> _valueList,FunctionReturnType _returnType){
  list<string> stringList;

  list<FileValue>::iterator iter = _valueList.begin();
  FileValue* value;

  while (iter != _valueList.end()){
    value = &(*iter);
    stringList.push_back( getFunctionString(value, _returnType));
    iter++;
  }
  return stringList;
}



std::string FunctionGenerator::stringListToString(list<std::string> _list){
  string stringbuffer = "<";
  list<std::string>::iterator iter = _list.begin();
  while (iter != _list.end()){
    stringbuffer += *iter;
    iter ++;
    if (iter!= _list.end()){
      stringbuffer += ",";
    }
  }
  stringbuffer += ">";
  return stringbuffer;

}

std::string FunctionGenerator::fileValueListToString(
  list<FileValue> _valueList,FunctionReturnType _returnType){
  return stringListToString(fileValueListToStringList( _valueList, _returnType));
}

std::string FunctionGenerator::getFunctionString(DOMElement* _thisFunctionElement){

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
