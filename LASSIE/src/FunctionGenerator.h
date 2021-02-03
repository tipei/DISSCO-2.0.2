/*******************************************************************************
 *
 *  File Name     : FunctionGenerator.h
 *  Date created  : Jun. 8 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This File contains the declaration of FunctionGenerator
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


#ifndef FUNCTIONGENERATOR_H
#define FUNCTIONGENERATOR_H

#include "LASSIE.h"
#include "EnvelopeLibraryEntry.h"


typedef enum {
  functionRandom,
  functionRandomInt,
  functionRandomOrderInt,
  functionRandomSeed,
  functionRandomizer,
  functionRandomDensity,
  functionInverse,
  functionLN,
  functionFibonacci,// not in used??
  functionDecay,

  functionMarkov, // generate from markov

  functionStochos,
  functionEnvLib,
  functionSelect,
  functionValuePick,

  functionChooseL,
  functionGetPattern,
  functionExpandPattern,
  functionAdjustPattern, // not in used??
  functionMakeEnvelope,
  functionMakeSieve,
  functionMakePattern,
  functionMakeList,
  functionRawList,
  functionReadENVFile,
  functionReadSIVFile,
  functionReadPATFile,
  functionReadSPAFile,
  functionReadREVFile,
  functionReadFILFile,

  functionSPA,

  functionREV_Simple,
  functionREV_Medium,
  functionREV_Advanced,

  functionMakeFilter,

  function_staticCURRENT_TYPE,
  function_staticCURRENT_CHILD_NUM,
  function_staticCURRENT_PARTIAL_NUM,
  function_staticCURRENT_DENSITY,
  function_staticCURRENT_SEGMENT,
  function_staticAVAILABLE_EDU,
  function_staticCURRENT_LAYER,
  function_staticPREVIOUS_CHILD_DURATION,

  functionSpectrum_Gen,

  NOT_A_FUNCTION




}CMODFunction;











class FileValue;


class FunctionGenerator :public Gtk::Dialog{

public:

  FunctionGenerator(FunctionReturnType _returnType,std::string _originalString);

  ~FunctionGenerator();
  std::string getResultString();
  std::string static getFunctionString(
    FileValue* _value,
    FunctionReturnType _returnType);
  std::string static getFunctionString(DOMElement* _thisFunctionElement);
  list<std::string> static fileValueListToStringList(
    list<FileValue> _valueList,
    FunctionReturnType _returnType);
  std::string static stringListToString(list<std::string> _list);
  std::string static fileValueListToString(
    list<FileValue> _valueList,
    FunctionReturnType _returnType);



 class SPAChannelAlignment;
  class SPAPartialAlignment: public Gtk::Alignment{
  public:
    SPAPartialAlignment(
      FunctionGenerator* _parent,
      SPAChannelAlignment* _parentChannel);
    ~SPAPartialAlignment();
    void setNumber(int _number);
    void clearPartialsInSameChannel();
    void clearPartialsWithSameNumber();
    void entryEditSwitch(int _flag);// 0 = disable, 1 = enable
    std::string getText();
    void setText(std::string _text);
    void insertPartialButtonClicked();
    void setLabel(string _label);
    SPAPartialAlignment* next;
    SPAPartialAlignment* prev;
    SPAPartialAlignment* prevChan;
    SPAPartialAlignment* nextChan;
    SPAChannelAlignment* parentChannel;
  private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    FunctionGenerator* parent;

    void textChanged();

    void removePartialButtonClicked();
    void funButtonClicked();


  };

  class SPAChannelAlignment: public Gtk::Alignment{
  public:
    SPAChannelAlignment(FunctionGenerator* _parent,int _numOfPartial,int _flagButtonsShown);
    ~SPAChannelAlignment();
    void setChannelNumber (int _number);
    void setChannelTitle(std::string _title);
    void hideButtons();
    void showButtons();
    void refreshPartialNumbers();  //////////////////////////////implement this
    void refreshLayout();
    void clear();
    void insertChannelButtonClicked();
    int getButtonsShownStatus(); // 0 = hiden, 1 = shown

    std::string getText();
    SPAChannelAlignment* prev;
    SPAChannelAlignment* next;
    SPAPartialAlignment* partials;
  private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    FunctionGenerator* parent;
    Gtk::Button* insertButton;
    Gtk::Button* removeButton;
    Gtk::VBox* mainVBox;
    Gtk::VBox* outerVBox;
    int flagButtonsShown; // 0 = hiden, 1 = shown


    void removeChannelButtonClicked();

  };

  int SPANumOfChannels;
  int SPANumOfPartials;
  int SPAApplyFlag; // 0 = sound, 1 = partial
  int SPAMethodFlag; //0 = stereo, 1 = Multi_Pan, 2 = Polar
  SPAChannelAlignment* SPAChannelAlignments;

  void SPARemoveChannel (SPAChannelAlignment* _remove);
  void SPAInsertChannel (SPAChannelAlignment* _insertAfter);
  void SPARemovePartial (SPAPartialAlignment* _remove);
  void SPAInsertPartial (SPAPartialAlignment* _insertAfter);

  void SPAMethodRadioButtonClicked();
  void SPAApplyByRadioButtonClicked();
  void SPATextChanged();


protected:
  //signal handelers
  void on_response (int response_id);
  void function_list_combo_changed();


private:


  Glib::RefPtr<Gtk::Builder> attributesRefBuilder;




  //combobox for list of functions

  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_id); add(m_col_name); }

    Gtk::TreeModelColumn<CMODFunction> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };
  ModelColumns functionListColumns;
  Glib::RefPtr<Gtk::ListStore> functionListTreeModel;

  //void on_function_list_combo_changed();  (duplicate?)
  std::string result;
  FunctionReturnType returnType; //store the type the generator should return

  //signal handlers
  //Random
  void randomLowBoundFunButtonClicked();
  void randomHighBoundFunButtonClicked();
  void randomEntryChanged();

  //RandomInt
  void randomIntLowBoundFunButtonClicked();
  void randomIntHighBoundFunButtonClicked();
  void randomIntEntryChanged();

  //RandomOrderInt
  void randomOrderIntLowBoundFunButtonClicked();
  void randomOrderIntHighBoundFunButtonClicked();
  void randomOrderIntEntryChanged();

  //Randomizer
  void randomizerBaseFunButtonClicked();
  void randomizerDeviationFunButtonClicked();
  void randomizerEntryChanged();

  // RandomDensity
  void randomDensityEntryChanged();

  //Decay
  void decayBaseFunButtonClicked();
  void decayRateFunButtonClicked();
  void decayIndexFunButtonClicked();
  void decayTypeRadioButtonClicked();
  void decayEntryChanged();
  void decayTextChange(); //helper to combine radiobutton click and entry change


  //inverse
  void inverseFunButtonClicked();
  void inverseEntryChanged();
  void markovEntryChanged();


  //LN
  void LNFunButtonClicked();
  void LNEntryChanged();

  //Fibonacci
  void FibonacciFunButtonClicked();
  void FibonacciEntryChanged();

  //select
  class SelectSubAlignment:public Gtk::Alignment {
  public:
    SelectSubAlignment(FunctionGenerator* _parent);
    ~SelectSubAlignment();
    void appendNewNode(SelectSubAlignment* _newNode);
    void clear();
    void setFunctionsEntry(std::string _string);
    std::string toString();
    SelectSubAlignment* prev;
    SelectSubAlignment* next;

  private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    FunctionGenerator* parent;
    void removeButtonClicked();
    void textChanged();
  };
  SelectSubAlignment* selectSubAlignments;
  int selectNumOfNodes;

  //void selectListFunButtonClicked();
  void selectIndexFunButtonClicked();
  void selectEntryChanged();
  void selectAddNodeButtonClicked();
  void selectFunButtonClicked();
  void selectRemoveNode(SelectSubAlignment* _remove);



  //Stochos
  class StochosSubAlignment:public Gtk::Alignment {
  public:
    StochosSubAlignment(FunctionGenerator* _parent, int _methodFlag);
    ~StochosSubAlignment();
    void switchTo(int _methodFlag);
    void appendNewNode(StochosSubAlignment* _newNode);
    void clear();
    void setFunctionsEntry(std::string _string);
    void setMinEntry(std::string _string);
    void setMaxEntry(std::string _string);
    void setDistEntry(std::string _string);
    std::string toString(int _methodFlag);// 0 = rang_distrib, 1 = Functions
    StochosSubAlignment* prev;
    StochosSubAlignment* next;

  private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    FunctionGenerator* parent;
    void removeButtonClicked();
    void textChanged();



  };
  StochosSubAlignment* stochosSubAlignments;
  int stochosNumOfNodes;
  int stochosMethodFlag; //0 = RANGE_DISTRIB, 1 = FUNCTIONS;

  void stochosMethodRadioButtonClicked();
  void stochosAddNodeButtonClicked();
  void stochosTextChanged();

  void stochosFunButtonClicked();
  void stochosRemoveNode(StochosSubAlignment* _remove);

  //ValuePick
  void valuePickTextChanged();
  void valuePickAbsRangeFunButtonClicked();
  void valuePickLowFunButtonClicked();
  void valuePickHighFunButtonClicked();
  void valuePickDistFunButtonClicked();
  string makeSieveNValuePickZeros(int _listLength);


  //chooseL
  void chooseLFunButtonClicked();
  void chooseLEntryChanged();


  //GetPattern
  void getPatternOffsetFunButtonClicked();
  void getPatternFunButtonClicked();
  void getPatternEntryChanged();


  //MakeList
  void makeListFunctionFunButtonClicked();
  void makeListSizeFunButtonClicked();
  void makeListTextChanged();

  //RawList
  class RawListHBox: public Gtk::HBox{
  public:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    RawListHBox(FunctionGenerator* _parentGenerator);
    ~RawListHBox();
    RawListHBox* next;
    RawListHBox* prev;
    FunctionGenerator* parentGenerator;

  };




  //EnvLib
  void envLibEnvelopeFunButtonClicked();
  void envLibScalingFactorFunButtonClicked();
  void envLibTextChanged();

  //ReadENVFile
  void readENVFileTextChanged();






  //MakeEnvelope
  class MakeEnvelopeSubAlignment:public Gtk::Alignment {
  public:
    MakeEnvelopeSubAlignment(FunctionGenerator* _parent);
    ~MakeEnvelopeSubAlignment();

    MakeEnvelopeSubAlignment* prev;
    MakeEnvelopeSubAlignment* next;
    std::string getXValueString();
    void setXValueString(std::string _string);
    std::string getYValueString();
    void setYValueString(std::string _string);
    envSegmentType getEnvSegmentType();
    void setEnvSegmentType ( int _type);
    envSegmentProperty getEnvSegmentProperty();
    void setEnvSegmentProperty( int _property);
    void clear();



  private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    FunctionGenerator* parent;
    void xValueFunButtonClicked();
    void yValueFunButtonClicked();
    void insertNodeButtonClicked();
    void removeButtonClicked();
    void textChanged();


  	class TypeModelColumns : public Gtk::TreeModel::ColumnRecord{
  	public:

    	TypeModelColumns()
    	{ add(m_col_type); add(m_col_name); }

    	Gtk::TreeModelColumn<envSegmentType> m_col_type;
    	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  	};
  	TypeModelColumns typeColumns;
  	Glib::RefPtr<Gtk::ListStore> typeTreeModel;

  	class PropertyModelColumns : public Gtk::TreeModel::ColumnRecord{
  	public:

    	PropertyModelColumns()
    	{ add(m_col_property); add(m_col_name); }

    	Gtk::TreeModelColumn<envSegmentProperty> m_col_property;
    	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  	};
  	PropertyModelColumns propertyColumns;
  	Glib::RefPtr<Gtk::ListStore> propertyTreeModel;


  };
  MakeEnvelopeSubAlignment* makeEnvelopeSubAlignments;
  int makeEnvelopeNumOfNodes;
  void makeEnvelopeTextChanged();
  void makeEnvelopeInsertNode(MakeEnvelopeSubAlignment* _insertAfter);
  void makeEnvelopeRemoveNode(MakeEnvelopeSubAlignment* _remove);
	void makeEnvelopeScalingFactorFunButtonClicked();
	void makeEnvelopeXValueFunButtonClicked();
	void makeEnvelopeYValueFunButtonClicked();


  //makePattern

  void makePatternIntervalsFunButtonClicked();
  void makePatternTextChanged();

  //expandPattern
  void expandPatternRadioButtonClicked();
  void expandPatternModuloFunButtonClicked();
  void expandPatternLowFunButtonClicked();
  void expandPatternHighFunButtonClicked();
  void expandPatternPatternFunButtonClicked();
  void expandPatternTextChanged();

  //ReadPATFile
  void readPATFileTextChanged();


  //ReadREVFile
  void readREVFileTextChanged();

  //ReadFILFile
  void readFILFileTextChanged();

  //MakeFilter
  void makeFilterTextChanged();
  void makeFilterDBGainFunButtonClicked();
  void makeFilterBandWidthFunButtonClicked();
  void makeFilterFrequencyFunButtonClicked();

  //REV_Simple
  void REV_SimpleEntryTextChanged();
  void REV_SimpleEntryFunButtonClicked();


  //REV_Medium
  void REV_MediumReverbPercentFunButtonClicked();
  void REV_MediumHilowSpreadFunButtonClicked();
  void REV_MediumGainAllPassFunButtonClicked();
  void REV_MediumDelayFunButtonClicked();
  void REV_MediumTextChanged();

  //REV_Advanced
  void REV_AdvancedReverbPercentFunButtonClicked();
  void REV_AdvancedCombGainListFunButtonClicked();
  void REV_AdvancedLPGainListFunButtonClicked();
  void REV_AdvancedGainAllPassFunButtonClicked();
  void REV_AdvancedDelayFunButtonClicked();
  void REV_AdvancedTextChanged();

  //ReadSIVFile
  void readSIVFileTextChanged();

  //MakeSieve
  void makeSieveTextChanged();

  void makeSieveLowFunButtonClicked();
  void makeSieveHighFunButtonClicked();

  //ReadSPAFile
  void readSPAFileTextChanged();




  //SPA

  //Spectrum_Gen
  void Spectrum_GenDistanceFunButtonClicked();
  void Spectrum_GenEnvelopeFunButtonClicked();
  void Spectrum_GenTextChanged();


};




#endif  //define FUNCTIONGENERATOR_H
