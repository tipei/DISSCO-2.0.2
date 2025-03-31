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

#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "LASSIE.h"

#include "ProjectViewController.h"

// forward declaration
class MainWindow;

// define namespace
namespace FileOperations {
  ProjectViewController* newProject(MainWindow* _mainWindow);
  ProjectViewController* openXMLProject(MainWindow* _mainWindow);
  string saveAs(MainWindow* _mainWindow);//return new path
  string pickDatFile(MainWindow* _mainWindow);
  string pickLibFile(MainWindow* _mainWindow,string _defaultPath);  
  void close(MainWindow* _mainWindow);
  void createDirectories(string _pathAndName);
  string stringToFileName(string _filePath);
  string stringToPath(string _filePath);

  bool checkFilePathValidity(std::string _filePath);
};
#endif  //FILE_OPERATIONS_H