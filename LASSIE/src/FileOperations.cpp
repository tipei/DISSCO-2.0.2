/*******************************************************************************
 *
 *  File Name     : FileOperations.h
 *  Date created  : Feb. 08 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains several File Operations used by LASSIE
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

#include "FileOperations.h"

#include "MainWindow.h"

ProjectViewController* FileOperations::newProject(MainWindow* _mainWindow){


  // setup the new project dialog window
  Gtk::FileChooserDialog dialog("New Project", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*_mainWindow);

  // Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  // Show the dialog and wait for a user response:
  int result = dialog.run();

  // Handle the response:
  if(result == Gtk::RESPONSE_OK){
    bool checker = false;
    while(checker == false){
      checker = checkFilePathValidity(dialog.get_filename());
      if(checker == false){
        return newProject(_mainWindow);
      }else{
         string command = dialog.get_filename();
         int mkdirChecker = mkdir(command.c_str(),S_IRWXU);
         if(mkdirChecker == -1){
           return newProject(_mainWindow);
         }else{
           string pathAndName = dialog.get_filename();
           //createDirectories(pathAndName);
           return new ProjectViewController(pathAndName, _mainWindow);
         }
      }
    }
  }

  return NULL;
}



string FileOperations::saveAs(MainWindow* _mainWindow){//return new path



  // setup the new project dialog window 
  Gtk::FileChooserDialog dialog("Save As...", Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*_mainWindow);
  
  // Add response buttons the the dialog:
  dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

 

  // Show the dialog and wait for a user response:
  int result = dialog.run();

  // Handle the response:
  if(result == Gtk::RESPONSE_OK){
    bool checker = false;
    while(checker == false){
      checker = checkFilePathValidity(dialog.get_filename());
      if(checker == false){
        return saveAs(_mainWindow);
      }else{
         string command = dialog.get_filename();;
         int mkdirChecker = mkdir(command.c_str(),S_IRWXU);
         if(mkdirChecker == -1){
           return saveAs(_mainWindow);
         }else{
           string pathAndName = dialog.get_filename();
           //createDirectories(pathAndName);           
           return pathAndName;
         }
      }
    }
  }
  else return "";
}

void FileOperations::createDirectories(string _pathAndName){
  string command;
  command = "mkdir "+ _pathAndName + "/T";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/H";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/M";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/L";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/B";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/S";
  system(command.c_str());
  command = "mkdir "+ _pathAndName + "/N";
  system(command.c_str());  	
  command = "mkdir "+ _pathAndName + "/PAT";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/REV";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/ENV";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/SIV";
  system(command.c_str());		 
  command = "mkdir "+ _pathAndName + "/SPA";
  system(command.c_str());
}


void FileOperations::close(MainWindow* _mainWindow){

  
}


string FileOperations::stringToFileName(string _filePath){
  int j = 0;
  int stringLength = _filePath.length()-1;
  string aWord;
  string fileName;

  aWord = aWord.assign(_filePath,stringLength,1);
  while(aWord != "/"){
    aWord = aWord.assign(_filePath,stringLength--,1);
    j++;
  }

  fileName = aWord.assign(_filePath,stringLength+2,j-1);
  return fileName;
}


string FileOperations::stringToPath(string _filePath){
  int stringLength = _filePath.length()-1;
  string aWord;

  aWord = aWord.assign(_filePath,stringLength,1);
  while(aWord != "/"){
    aWord = aWord.assign(_filePath,stringLength--,1);
  }
  _filePath = aWord.assign(_filePath,0,stringLength+2);
  return _filePath;
}


bool FileOperations::checkFilePathValidity(string _filePath){
  string fileName = stringToFileName(_filePath);
  int fileNameLength = fileName.length();

  string allowableSymbol(".-_"); // set the allowable symbol.
  string checkSymbol;
  int allowableSymbolLength = allowableSymbol.length();

  string checkWord;
  bool check = true;

  while(fileNameLength > 0){
    checkWord = checkWord.assign(fileName,fileNameLength-1,1);
    if(!((checkWord >= "0" // check "0~9" and "A~Z" and "a~z"
       && checkWord <= "9")
      || (checkWord >= "A"
       && checkWord <= "Z")
      || (checkWord >= "a"
       && checkWord <= "z"))){
      check = false;

      while(allowableSymbolLength > 0){// check symbol.
        checkSymbol =
        checkSymbol.assign(allowableSymbol,allowableSymbolLength-1,1);
        if(checkWord == checkSymbol) check = true;
        allowableSymbolLength--;
      }

      allowableSymbolLength = allowableSymbol.length();
    }
    fileNameLength--;
  }

  checkWord = checkWord.assign(fileName,0,1);
  fileNameLength = fileName.length();
  if(checkWord == "." || fileNameLength > 256) check = false;
  // check first word whether is "." and fileName length.
  return check;
}

ProjectViewController* FileOperations::openXMLProject(MainWindow* _mainWindow){  
  Gtk::FileChooserDialog dialog("Open existing .dissco project",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  
  dialog.set_transient_for(*_mainWindow);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

  Gtk::FileFilter filter;
  filter.set_name(".dissco files");
  filter.add_pattern("*.dissco");
  dialog.add_filter(filter);

  int result = dialog.run();
    
  if(result == Gtk::RESPONSE_OK){
    string filePath = dialog.get_filename();
    const size_t lastSlash = filePath.find_last_of('/');
    const string name = filePath.substr(lastSlash + 1);;
    
    filePath = filePath.substr(0, lastSlash);

    return new ProjectViewController(_mainWindow, filePath, name);
  }

  return NULL;
}

string FileOperations::pickDatFile(MainWindow* _mainWindow){
  Gtk::FileChooserDialog dialog("Select dat File",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);

  dialog.set_transient_for(*_mainWindow);
  dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  //Add filters, so that only certain file types can be selected:

  Gtk::FileFilter filter_dat;
  filter_dat.set_name(".dat files");
  filter_dat.add_pattern("*.dat");
  dialog.add_filter(filter_dat);

  Gtk::FileFilter filter_any;
  filter_any.set_name("Any files");
  filter_any.add_pattern("*");
  dialog.add_filter(filter_any);

  //Show the dialog and wait for a user response:
  int result = dialog.run();
  if (result == Gtk::RESPONSE_OK){
    return dialog.get_filename();  
  }
  else {
    return "Cancel";
  }
}

std::string FileOperations::pickLibFile(
  MainWindow* _mainWindow,
  string _defaultPath){
 string libFile =  _defaultPath + "/"+
                        FileOperations::stringToFileName(_defaultPath)
                        + ".lib"; 

    int flagLoopToFindLib = 0; //0 = keep countinue, 1 = cancel 
    FILE* testExist = fopen ( libFile.c_str(), "r");
    while (testExist == NULL && flagLoopToFindLib ==0){
      Gtk::MessageDialog dialog(*_mainWindow, "lib File Not Found");
      dialog.set_secondary_text(
          "The lib file:\n\n" + libFile +
          "\n\nis not found. Do you want to manually find the lib file?"
          );

      dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
      int result = dialog.run();  //ok = -5, cancel = -6
      dialog.hide();
      if (result == -6){
        flagLoopToFindLib = 1;
        break;
      }
      else {  //try choosing the file
      
        Gtk::FileChooserDialog dialog("Select lib File",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
        dialog.set_transient_for(*_mainWindow);
  
        dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
        dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

        Gtk::FileFilter filter_lib;
        filter_lib.set_name(".lib files");
        filter_lib.add_pattern("*.lib");
        dialog.add_filter(filter_lib);

        Gtk::FileFilter filter_any;
        filter_any.set_name("Any files");
        filter_any.add_pattern("*");
        dialog.add_filter(filter_any);

        int result = dialog.run();
        if (result  == Gtk::RESPONSE_OK){
          libFile = dialog.get_filename();  
        }
        else {
          libFile = "Cancel";
        }      
      
        
        if (libFile == "Cancel"){
          flagLoopToFindLib = 1;
          break;
        }
      } //end trying choosing teh file
      testExist = fopen ( libFile.c_str(), "r");
    }
    
    if (testExist!= NULL){
      fclose (testExist);
    }  
  return libFile;
}