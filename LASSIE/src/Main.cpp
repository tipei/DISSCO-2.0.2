/*******************************************************************************
 *
 *  File Name     : main.cpp
 *  Date created  : Jan. 23 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This is the main file of LASSIE, a gtkmm implemented 
 *                  interface for CMOD.
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

#include "MainWindow.h"

/*! \brief The main function of LASSIE.
 
 parameters: argc - the number of arguments
 argv - array of strings containing arguments
 return    : return 0 if terminate normally
 ******************************************************************************/
int main (int argc,char** argv){
  // create a Gtk main function
  Gtk::Main kit(argc, argv);
  
  // create the MainWindow object
  MainWindow mainWindow;
  
  // Shows the window and returns when it is closed.
  Gtk::Main::run(mainWindow);
  return 0;
}

