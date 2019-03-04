/*
CMOD (composition module)
Copyright (C) 2007  Sever Tipei (s-tipei@uiuc.edu)
   
   
   Update:
   This class is no longer in used by XML version of CMOD. It's here only
   for people who need to upgrade their DISSCO projects from the old format
   to the new format (See LASSIE/src/UpgradeProjectFormat.h ).
   
                                            --Ming-ching Chiu May 06 2013
                                            
                                            
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
//   Filevalue.h  This class is used as middlewear between the data files
//   and the rest of the CMOD.  Whe information is read  from the data files
//   it is stored in various keywords that are defined in the parser (see
//   parser.l for a complete list).  These file values are then flagged with
//   an appropriate type (number, string, list, function, envelope) and
//   the useful values from within then can be accessed using the appropriate
//   getter functions.
//
//----------------------------------------------------------------------------//

#ifndef FILEVALUE_H
#define FILEVALUE_H

//----------------------------------------------------------------------------//

// CMOD includes
#include "Libraries.h"

#include "Define.h"
#include "Patter.h"
#include "Sieve.h"
class Event; // forward declared to avoid cyclic dependency

//----------------------------------------------------------------------------//

enum file_value_data_type {
  FVAL_NULL,
  FVAL_NUMBER,
  FVAL_STRING,
  FVAL_LIST,
  FVAL_FUNC,
  FVAL_ENVELOPE,
  FVAL_SIEVE,
  FVAL_PATTERN
};

//----------------------------------------------------------------------------//

/**
* Class FileValue.
* \param orig_type Enumeration holding original parsed type
* \param return_type Enum holding the type to return (after computing any functions)
* \param ftnString String indicating a name of a function
* \param ftnArgs List of arguments passed to a function
* \param s String to be returned
* \param n Number to be returned
* \param l List of FileValue objects to be returned
* \param obj Variable of type void* for holding different types of return objects
**/
class FileValue
{
private:
  file_value_data_type orig_type;  // the type in the original file
  file_value_data_type return_type;  // the type to return

  string ftnString;   // the string rep of a function
  list<FileValue> ftnArgs;  // arguments for a function
  static int segmentNum;  // static segment for making envelopes

  
  string patterMethod;
  int patterOrigin;
  

  /* return data */
  string s;
  double n;
  list<FileValue> l;
  void * obj;
  
  

  Event* evptr; // pointer to event evaluating this filevalue

public:
  /**
   *  CONSTRUCTOR: FileValue - Generic empty constructor.
   **/
  FileValue() : return_type(FVAL_NULL), obj(NULL), evptr(NULL) { };

  /**
   *  CONSTRUCTOR: FileValue - Copy constructor that creates another instance 
   *  of FileValue with parameters equivalent to those of the FileValue 
   *  reference argument.
   *  \param v Reference to a FileValue object
   **/
  FileValue(const FileValue &v);

  /**
   *  Assignment Operator
   **/
  FileValue & operator=(const FileValue &rhs);

  /**
   *  String output
   **/
  string toString();

  /**
   *  DESTRUCTOR: ~FileValue
   **/
  ~FileValue();

  //-----------  Methods returning value that won't be Evaluate()-ed.  ----------------//
  /**
   *  Method to return value of the original type that was parsed from the input file
   *  \return Value of original type
   **/
  const file_value_data_type getOrigType() const { return orig_type; }

  /**
   *  Method to return value of the original type that was parsed from the input file
   *  \return Value of original type
   **/
  const file_value_data_type getReturnType() const { return return_type; }

  /**
   *  Method to return value of string ftnString
   *  \return Reference to the string ftnString
   **/
  const string & getFtnString() const { return ftnString; }
  
  /**
   *  Return a reference to a list of FileValue objects (function arguments)
   *  \return Reference to a list of FileValue objects
   **/
  const list<FileValue> & getFtnArgs() const { return ftnArgs; }

  /**
   *  Return a pointer to a list of FileValue objects (function arguments)
   *  \return Pointer to a list of FileValue objects
   **/
  list<FileValue> * getFtnArgsPtr() { return &ftnArgs; }


  //-----------  Return values that can be dependant on an event  ----------------//
  //-----------    (will be Evaluate()-ed before returning!)      ----------------//
  /**
   *  Evaluate any functions and return a string
   *  \return Reference to the string s
   **/
  const string & getString(Event* ev) { Evaluate(ev); return s; }
  const string & getString() const { return s; }

  /**
   *  Evaluate any functions and return an int
   **/
  const int getInt(Event* ev) { Evaluate(ev); return(int) (getFtnString()=="GetPattern"?((Patter*)obj)->GetNextValue( patterMethod, patterOrigin):n); }
  const int getInt() const { return (int) (getFtnString()=="GetPattern"?((Patter*)obj)->GetNextValue( patterMethod, patterOrigin):n); }

  /**
   *  Evaluate any functions and return an int
   **/
  const float getFloat(Event* ev) { Evaluate(ev); return (float)( getFtnString()=="GetPattern"?((Patter*)obj)->GetNextValue( patterMethod, patterOrigin):n);}
  const float getFloat() const { return (float)( getFtnString()=="GetPattern"?((Patter*)obj)->GetNextValue( patterMethod, patterOrigin):n); }

  /**
   *  Evaluate any functions and return a list of FileValues
   **/
  const list<FileValue> & getList(Event* ev) { Evaluate(ev); return l; }
  const list<FileValue> & getList() const { return l; }

  /**
   *  Evaluate any functions and return a pointer to a list of FileValues
   **/
  list<FileValue>* getListPtr(Event* ev) { Evaluate(ev); return &l; }

  /**
   *  Evaluate any functions and return a pointer to an Envelope
   **/
  Envelope * getEnvelope(Event* ev) { Evaluate(ev); return (Envelope*)obj; }

  /**
   *  Evaluate any functions and return a pointer to a Sieve
  **/
  Sieve * getSieve(Event* ev) { Evaluate(ev); return (Sieve*)obj; }

  /**
   *  Evaluate any functions and return a pointer to a Pattern
   **/
  Patter * getPattern(Event* ev) { Evaluate(ev); return (Patter*)obj; }

  //----------------------   Methods setting variables  -----------------------//

  /**
   *  Sets the original type 
   *  \param t The file_value_data_type to set orig_type to
   **/
  void setOrigType(file_value_data_type t) { orig_type = t; }

  /**
   *  Sets the return type 
   *  \param t The file_value_data_type to set return_type to
   **/
  void setReturnType(file_value_data_type t) { return_type = t; }

  /**
   *  Sets the function name
   *  \param ss The string name of the function
   **/
  void setFtnString(string ss) { ftnString = ss; }

  /**
   *  Sets the function arguments to a list of FileValues
   *  \param ll list of FileValue objects
   **/
  void setFtnArgs(list<FileValue> ll) { ftnArgs = ll; }

  /**
   *  Sets the class variable "s" to the string argument ss
   *  \param ss The string to set s to
   **/
  void setString(std::string ss) { s = ss; }

  /**
  *  Sets the class variable "n" to the double argument d
  *  \param d The double to set n to
  **/
  void setNumber(double d) { n = d; }

  /**
  *  Sets the class list structure "l" to the list structure argument ll
  *  \param ll The list of FileValue objects to set l to
  **/
  void setList(std::list<FileValue> ll) { l = ll; }

  /**
  *  Sets the class variable "obj" to point to an Envelope object
  *  \param e The pointer to an Envelope object that obj will point to
  **/
  void setEnvelope(Envelope *e) { obj = (void *)e; }

  /**
  *  Sets the class variable "obj" to point to a Sieve object
  *  \param s The pointer to a Sieve object that obj will point to
  **/
  void setSieve(Sieve* s) { obj = (void *)s; }

  /**
   *  Sets the class variable "obj" to point to a Patter object
   *  \param s The pointer to a Patter object that obj will point to
   **/
  void setPattern(Patter* p) { obj = (void *)p; }

  //---------  Boolean functions to determine the value of return type  ------------//

  /**
  *  Checks if return_type = FVAL_NULL
  *  \return True if return_type = FVAL_NULL, false otherwise
  **/
  bool isNull() const { return (return_type == FVAL_NULL); }

  /**
  *  Checks if return_type = FVAL_NUMBER
  *  \return True if return_type = FVAL_NUMBER, false otherwise
  **/
  bool isNumber() const { return (return_type == FVAL_NUMBER); }

  /**
  *  Checks if return_type = FVAL_LIST
  *  \return True if return_type = FVAL_LIST, false otherwise
  **/
  bool isList() const { return (return_type == FVAL_LIST); }

  /**
  *  Checks if return_type = FVAL_STRING
  *  \return True if return_type = FVAL_STRING, false otherwise
  **/
  bool isString() const { return (return_type == FVAL_STRING); }

  /**
  *  Checks if orig_type = FVAL_FUNC (return value will be something else)
  *  \return True if orig_type = FVAL_FUNC, false otherwise
  **/
  bool isFunction() const { return (orig_type == FVAL_FUNC); }

  /**
  *  Checks if return_type = FVAL_ENVELOPE
  *  \return True if return_type = FVAL_ENVELOPE, false otherwise
  **/
  bool isEnvelope() const { return (return_type == FVAL_ENVELOPE); }

  /**
  *  Checks if return_type = FVAL_SIEVE
  *  \return True if return_type = FVAL_SIEVE, false otherwise
  **/
  bool isSieve() const { return (return_type == FVAL_SIEVE); }

  /**
   *  Checks if return_type = FVAL_PATTERN
   *  \return True if return_type = FVAL_PATTERN, false otherwise
   **/
  bool isPattern() const { return (return_type == FVAL_PATTERN); }

  //------------------------  Evaluating Methods  -----------------------------//

  /**
  *  Converts the class variable "type" to its string representation
  **/
  const string TypeToString() const;

  /**
  *  Wrapper function that calls Evaluate().  Class variable 'evptr' is set 
  *  to the Event pointer argument
  **/
  void Evaluate();

  /**
  *  Function that sets class variables based on the value of the string s
  *  \param ev Pointer to an Event object
  **/
  void Evaluate(Event *ev);

private:

  /**
  *  Method to evaluate arguments contained in a created list.
  *  Returns true if arguments are the correct types, and false otherwise.
  *  \param n Size of list
  *  \param A number of file_value_data_type values
  **/
  vector<FileValue*> EvaluateArgs(int n, ...);


  /**
  *  Helper methods for all the fuctions supported 
  *    in the CMOD input files
  **/
  void ftn_Random();
  void ftn_RandomInt();
  void ftn_RandomSeed();
  void ftn_Randomizer();
  void ftn_Inverse();
  void ftn_LN();
  void ftn_Fibonacci();
  void ftn_Decay();

  void ftn_Stochos();
  void ftn_EnvLib();
  void ftn_Select();
  void ftn_ValuePick();

  void ftn_ChooseL();
  void ftn_GetPattern();
  void ftn_ExpandPattern();
  void ftn_AdjustPattern();
  void ftn_MakeEnvelope();
  void ftn_MakeSieve();
  void ftn_MakePattern();
  void ftn_MakeList();
  void ftn_ReadENVFile();
  void ftn_ReadSIVFile();
  void ftn_ReadPATFile();
  void ftn_ReadSPAFile();
  void ftn_ReadREVFile();

  /**
  *  Helper methods for the static functions 
  *    supported in the CMOD input files
  **/
  void static_ftn_CURRENT_TYPE();
  void static_ftn_CURRENT_CHILD_NUM();
  void static_ftn_CURRENT_PARTIAL_NUM();
  void static_ftn_CURRENT_DENSITY();
  void static_ftn_CURRENT_SEGMENT();
  void static_ftn_DURATION_UNITS(); //Deprecated--use AVAILABLE_EDU
  void static_ftn_DURATION_EDU(); //Deprecated--use AVAILABLE_EDU
  void static_ftn_AVAILABLE_EDU();
  void static_ftn_CURRENT_LAYER();
  void static_ftn_PREVIOUS_CHILD_DURATION();

  //----------------------------------------------------------------------------//
};

#endif // FILEVALUE_H
