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

#ifndef _EVENT_PARSER_H_
#define _EVENT_PARSER_H_

// CMOD includes
#include "parser/Parser.h"
#include "Libraries.h"
#include "Define.h"
#include "Piece.h"

class EventFactory; // forward declare to avoid cyclic dependency
//----------------------------------------------------------------------------//

    /**
    *  Structure
    **/
    extern "C" {
      int yyparse(void);
      int yylex(void);
      int yywrap();
    }

//----------------------------------------------------------------------------//

    /**
    *  Parses an event file and sets up the event factory
    *  \param filename Name of the file to be parsed
    *  \param ef Pointer to an event factory
    *  \param piece Pointer to the piece being created with CMOD
    **/
    //int parseFile(std::string filename, EventFactory *ef, Piece *piece);

    /**
    *  Sets the parser to read from 'filenam'
    *  \param filename Name of the file from which the parser will read
    **/
    int setFile(std::string filename);


#endif /* _EVENT_PARSER_H_ */
