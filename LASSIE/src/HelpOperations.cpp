/*******************************************************************************
 *
 *  File Name     : HelpOperations.cpp
 *  Date created  : Mar.20 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : This file contains Help Menu Operations used by LASSIE
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

#include "HelpOperations.h"

void HelpOperations::showAbout() {
    Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        refBuilder->add_from_file("./LASSIE/src/UI/HelpAbout.ui");
        refBuilder->add_from_file("./LASSIE/src/UI/HelpAbout.ui");
    } catch (const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
    } catch (const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
    }
#else
    std::auto_ptr<Glib::Error> error;
    if (!refBuilder->add_from_file("./LASSIE/src/UI/HelpAbout.ui", error)) {
        std::cerr << error->what() << std::endl;
    }
#endif /* !GLIBMM_EXCEPTIONS_ENABLED */

    Gtk::Dialog* helpAboutDialog;
    // Get the GtkBuilder-instantiated Dialog:

    refBuilder->get_widget("helpAboutDialog", helpAboutDialog);
    // newObjectDialog->set_parent(*this);

    // Get the GtkBuilder-instantiated Button, and connect a signal handler:
    helpAboutDialog->run();
    helpAboutDialog->hide();
}

void HelpOperations::showContents() {}
