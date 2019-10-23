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
ProjectViewController* openProject(MainWindow* _mainWindow);
ProjectViewController* openXMLProject(MainWindow* _mainWindow);
std::string saveAs(MainWindow* _mainWindow);  // return new path
std::string pickDatFile(MainWindow* _mainWindow);
std::string pickLibFile(MainWindow* _mainWindow, std::string _defaultPath);
void close(MainWindow* _mainWindow);
void createDirectories(std::string _pathAndName);
std::string stringToFileName(std::string _filePath);
std::string stringToPath(std::string _filePath);

bool checkFilePathValidity(std::string _filePath);
// void Inappropriate_Project_Name();
};  // namespace FileOperations

class fileNameError : public Gtk::Window {
public:
    // ExampleWindow();
    fileNameError();
    ~fileNameError();

protected:
    void inappropriateProjectName();
};

class fileNameExist : public Gtk::Window {
public:
    fileNameExist();
    ~fileNameExist();

protected:
    void duplicateProjectName();
};
#endif  // FILE_OPERATIONS_H
