/*******************************************************************************
 *
 *  File Name     : EnvLibDrawingArea.cpp
 *  Date created  : Jan. 29 2010
 *  Authors       : Ming-ching Chiu, Sever Tipei
 *  Organization  : Music School, University of Illinois at Urbana Champaign
 *  Description   : 
 *
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
 
#include "EnvLibDrawingArea.h"

#include "EnvelopeLibraryWindow.h"
#include "EnvelopeLibraryEntry.h"
#include "ProjectViewController.h"
EnvLibDrawingArea::EnvLibDrawingArea(
  EnvelopeLibraryWindow* _envelopeLibraryWindow){
  
  
  envelopeLibraryWindow = _envelopeLibraryWindow;
  activeSegment = NULL;
  activeNode = NULL;
	add_events (Gdk::POINTER_MOTION_MASK | 
	            Gdk::ENTER_NOTIFY_MASK | 
	            Gdk::BUTTON_PRESS_MASK |
	            Gdk::BUTTON_RELEASE_MASK);


	signal_motion_notify_event().connect(sigc::mem_fun(*this,&EnvLibDrawingArea::mouseMoving));
	signal_button_press_event().connect(sigc::mem_fun(*this,&EnvLibDrawingArea::mouseButtonPressed));
	signal_button_release_event().connect(sigc::mem_fun(*this,&EnvLibDrawingArea::mouseButtonReleased));
	mouseLeftButtonPressedDown = false;
	
	
	
	 /////////////////////set up popup menu ///////////////////////////////

  m_pMenuPopup = 0;

  //    the signal handler i wished to put was overriden by the object default
  //signal handler, so to avoid this, we have to add a paramater to the connect
  //method (false)


  //Fill menu:

  m_refActionGroup = Gtk::ActionGroup::create();

  //File|New sub menu:
  //These menu actions would normally already exist for a main menu, because a
  //context menu should not normally contain menu items that are only available
  //via a context menu.
  m_refActionGroup->add(Gtk::Action::create("ContextMenu", "Context Menu"));


   m_refActionGroup->add(
    Gtk::Action::create("ContextAdd", "Insert Node"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::insertEnvelopeSegment));
   m_refActionGroup->add(
    Gtk::Action::create("ContextRemove", "RemoveNode"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::removeNode)); 
    
    
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetFlexible", "Set Flexible"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setFlexible));
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetFixed", "Set Fixed"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setFixed)); 

   m_refActionGroup->add(
    Gtk::Action::create("ContextSetLinear", "Set Linear"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setLinear));
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetSpline", "Set Spline"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setSpline));  
   m_refActionGroup->add(
    Gtk::Action::create("ContextSetExponential", "Set Exponential"),
    sigc::mem_fun(*this, &EnvLibDrawingArea::setExponential));
    
    
    
  

  m_refUIManager = Gtk::UIManager::create();
  m_refUIManager->insert_action_group(m_refActionGroup);

  //projectView->add_accel_group(m_refUIManager->get_accel_group());

  //Layout the actions in a menubar and toolbar:
  
  
  Glib::ustring ui_info =
    "<ui>"
    "  <popup name='PopupMenu'>"
    "    <menuitem action='ContextAdd'/>"
    "    <menuitem action='ContextRemove'/>"
    "    <separator/>"
    "    <menuitem action ='ContextSetFixed' />"    
    "    <menuitem action ='ContextSetFlexible' />"
    "    <separator/>"
    "    <menuitem action ='ContextSetLinear' />"    
    "    <menuitem action ='ContextSetSpline' />"    
    "    <menuitem action ='ContextSetExponential' />"
    "  </popup>"
    "</ui>";

  #ifdef GLIBMM_EXCEPTIONS_ENABLED
    try{
      m_refUIManager->add_ui_from_string(ui_info);
    }
    catch(const Glib::Error& ex){
      std::cerr << "building menus failed: " <<  ex.what();
    }
  #else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if(ex.get()){
      std::cerr << "building menus failed: " <<  ex->what();
    }
  #endif // GLIBMM_EXCEPTIONS_ENABLED

  // Get the menu:
  m_pMenuPopup = dynamic_cast<Gtk::Menu*>(
    m_refUIManager->get_widget("/PopupMenu"));

  if(!m_pMenuPopup) g_warning("menu not found");
  
}

EnvLibDrawingArea::~EnvLibDrawingArea()
{
}


bool EnvLibDrawingArea::on_expose_event(GdkEventExpose* event)
{
  EnvelopeLibraryEntry* activeEnvelope = 
    envelopeLibraryWindow->getActiveEnvelope();
  
  if (activeEnvelope!=NULL){
    showGraph(activeEnvelope);
  }  
  return true;
}


bool EnvLibDrawingArea::mouseMoving (GdkEventMotion* event){

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

 double x = event->x * (width + 1) /( width* width);
 
 // adjust pixals off by one
 double y = 1.0 - (event->y * (height + 1) / (height* height)); 
 
 //get rid of floating digits
 int xalias = x* 1000;
 int yalias = y* 1000;
 x = xalias / 1000.0;
 y = yalias / 1000.0;
 

  char xs[10];
  char ys[10];
  
  sprintf(xs, "%.3f", x);
  sprintf(ys, "%.3f", y);
  
  std::string xss = xs;
  std::string yss = ys;
  
  std::string n = "x:"+ xss +"\ny:" + yss;
	envelopeLibraryWindow->mouseCoordinate->set_text(n);

	
	
	if (mouseLeftButtonPressedDown){
    if (x>1){
      mouseX = 1;
    }
    else if (x< 0){
      mouseX = 0;
    }    
    else {
      mouseX = x;
    }

    if (y>1){
      mouseY = 1;
    }
    else if (y< 0){
      mouseY = 0;
    }    
    else {
      mouseY = y;
    }


 	  moveNode();
  }
  
}

bool EnvLibDrawingArea::mouseButtonPressed (GdkEventButton* event){
  EnvelopeLibraryEntry* activeEnvelope = 
    envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL){
    return false;
  }
  
  activeSegment =NULL; //reset activeSegment
  	  

  mouseX = event->x;
  mouseY = event->y;
    
    
  Glib::RefPtr<Gdk::Window> window = get_window();
  int width = 0;
  int height = 0;
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();
  }
  mouseX = event->x;
  mouseY = height - event->y;    
    
  EnvLibEntryNode* candidateNode = activeEnvelope->head;
  double nodeX = candidateNode->x * width * width / (width +1);
  double nodeY = candidateNode->y * height * height / (height + 1);  
      
  bool nodeActivated = false;  

  while(true){

    
    
    if (mouseX>=(nodeX-5)&&
        mouseX<=(nodeX+5)&&
        mouseY>=(nodeY-5)&&
        mouseY<=(nodeY+5)){
      activeNode = candidateNode;
      nodeActivated = true;
      break;
    }
    if (candidateNode->rightSeg ==NULL){
      break;
    }
    candidateNode = candidateNode->rightSeg->rightNode;
    nodeX = candidateNode->x * width * width / (width +1);
    nodeY = candidateNode->y * height * height / (height + 1); 
  }

  if (nodeActivated == false){
    activeNode =NULL;
    envelopeLibraryWindow->setEntries("","");

  }
  
  else {
    double activex = activeNode->x;
    double activey = activeNode->y;
  
    std::string xstring;
    std::string ystring;
    char tem1[20] ;
    sprintf(tem1,"%.4f",activex);
    xstring = string (tem1);
    sprintf(tem1,"%.4f",activey); 
    ystring = string (tem1);
    envelopeLibraryWindow->setEntries(xstring,ystring);
 
  }
  
  showGraph(activeEnvelope);
  
  
  // test if right click
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ){ 
    mouseX = event->x;
    mouseY = event->y;
    if (activeNode ==NULL){
      m_refActionGroup->get_action("ContextRemove")->set_sensitive(false);
    }
    else {
      m_refActionGroup->get_action("ContextRemove")->set_sensitive(true);
    }
    if(m_pMenuPopup) m_pMenuPopup->popup(event->button, event->time);
  }
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 1)){
  	
    mouseLeftButtonPressedDown = true; 
  }
  return true;
}

bool EnvLibDrawingArea::mouseButtonReleased(GdkEventButton* event){
  if( event->button == 1){
		mouseLeftButtonPressedDown = false;
		activeSegment = NULL;
		
		
	}
		
}


void EnvLibDrawingArea::clearGraph(){
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
   

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
 
    // clear the drawing area
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0,0, width, height);
    cr->fill();
   }
      
}

void EnvLibDrawingArea::showGraph(EnvelopeLibraryEntry* _envelope){



  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
   

    EnvLibEntryNode* node;
    EnvLibEntrySeg* segment = _envelope->head->rightSeg;

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
 
    // clear the drawing area
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0,0, width, height);
    cr->fill();
    cr->stroke();
   
      

    while (segment!=NULL){ // draw this segment
      // clip to the area indicated by the expose event so that we only redraw
      // the portion of the window that needs to be redrawn
      //cr->rectangle(event->area.x, event->area.y,
      //      event->area.width, event->area.height);
      //cr->clip();
      node = segment->leftNode;
      
      double startx = node->x * width * width / (width +1);
      double starty = height - node->y * height * height / (height + 1);
      
      
      double endx = segment->rightNode->x *width * width / (width+1);
      
      double endy = height-(segment->rightNode->y*height*height / (height + 1));  
     
     //set color here
      if (segment->segmentType == envSegmentTypeLinear){
            cr->set_source_rgb(0.0, 0.5, 0.8);
      }
      else if (segment->segmentType == envSegmentTypeExponential){
          cr->set_source_rgb(0.8, 0.0, 0.0);
      }
      else {
          cr->set_source_rgb(0.0, 1, 0.2);
        
      }
     
      if (segment->segmentProperty == envSegmentPropertyFixed){
        cr->set_line_width(1.0);
         
      }
      else {
        cr->set_line_width(3.0);
      }
      
      

      cr->move_to(startx, starty);
      cr->line_to(endx, endy);
      cr->stroke();

      cr->set_source_rgb(0.8, 0.5, 0.4);
      cr->arc(startx, starty, 5, 0, 2 * 3.141592654);
      cr->fill();
      cr->arc(endx, endy, 5, 0, 2 * 3.141592654);
      cr->fill();
      
      
      segment = segment->rightNode->rightSeg;
    }
    
    if (activeNode !=NULL){
    
      double activeX = activeNode->x * width * width / (width +1);
      double activeY = height - activeNode->y * height * height / (height + 1);
      
      cr->set_source_rgb(0.4, 0.8, 0.9);
      cr->arc(activeX, activeY, 8, 0, 2 * 3.141592654);
      cr->fill();
    
    
    }

  }

}


void EnvLibDrawingArea::insertEnvelopeSegment(){
    EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
    if (activeEnvelope ==NULL) return;
    envelopeLibraryWindow->activeProject->modified();

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();


  //get rid of ugly floating digits
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  int aliasx = insertx* 1000;
  int aliasy = inserty* 1000;
  
  insertx = aliasx / 1000.0;         
  inserty = aliasy / 1000.0;
  
  EnvLibEntryNode* leftNode = activeEnvelope->head;
  EnvLibEntryNode* rightNode = leftNode->rightSeg->rightNode;
  
  while (leftNode->x <insertx&& rightNode->x< insertx){
    leftNode = rightNode;
    rightNode = rightNode->rightSeg->rightNode;  
  }
  
  EnvLibEntryNode* newNode = new EnvLibEntryNode(insertx,inserty);
  EnvLibEntrySeg* newSeg = new EnvLibEntrySeg();
  
  
  leftNode->rightSeg->rightNode = newNode;
  newNode->leftSeg = leftNode->rightSeg;
  newNode->rightSeg = newSeg;
  newSeg->leftNode = newNode;
  newSeg->rightNode = rightNode;
  rightNode->leftSeg = newSeg;
  newSeg-> segmentType = leftNode->rightSeg->segmentType;
  newSeg-> segmentProperty = leftNode->rightSeg->segmentProperty;
  
  
  showGraph(activeEnvelope);

}






void EnvLibDrawingArea::setFixed(){
  EnvelopeLibraryEntry* activeEnvelope = 
    envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntryNode* leftNode = activeEnvelope ->head;
  EnvLibEntryNode* rightNode = leftNode->rightSeg->rightNode;
  
  while (leftNode->x <insertx&& rightNode->x< insertx){
    leftNode = rightNode;
    rightNode = rightNode->rightSeg->rightNode;  
  }

  leftNode->rightSeg->segmentProperty = envSegmentPropertyFixed;
  showGraph(activeEnvelope);


}
void EnvLibDrawingArea::setFlexible(){
  EnvelopeLibraryEntry* activeEnvelope = 
  envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntryNode* leftNode = activeEnvelope ->head;
  EnvLibEntryNode* rightNode = leftNode->rightSeg->rightNode;
  
  while (leftNode->x <insertx&& rightNode->x< insertx){
    leftNode = rightNode;
    rightNode = rightNode->rightSeg->rightNode;  
  }

  leftNode->rightSeg->segmentProperty = envSegmentPropertyFlexible;
 
  showGraph(activeEnvelope);


}


void EnvLibDrawingArea::setLinear(){  
  EnvelopeLibraryEntry* activeEnvelope = 
  envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntryNode* leftNode = activeEnvelope ->head;
  EnvLibEntryNode* rightNode = leftNode->rightSeg->rightNode;
  
  while (leftNode->x <insertx&& rightNode->x< insertx){
    leftNode = rightNode;
    rightNode = rightNode->rightSeg->rightNode;  
  }


  leftNode->rightSeg->segmentType = envSegmentTypeLinear;
 
  showGraph(activeEnvelope);

}



void EnvLibDrawingArea::setSpline(){

  EnvelopeLibraryEntry* activeEnvelope = 
    envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntryNode* leftNode = activeEnvelope ->head;
  EnvLibEntryNode* rightNode = leftNode->rightSeg->rightNode;
  
  while (leftNode->x <insertx&& rightNode->x< insertx){
    leftNode = rightNode;
    rightNode = rightNode->rightSeg->rightNode;  
  }



  leftNode->rightSeg->segmentType = envSegmentTypeSpline;
 
  showGraph(activeEnvelope);


}



void EnvLibDrawingArea::setExponential(){

  EnvelopeLibraryEntry* activeEnvelope = 
    envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  envelopeLibraryWindow->activeProject->modified();

  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();


  
  double insertx = mouseX * ( width +1) / (width* width)  ;  
  double inserty = 1 - mouseY * ( height+1) / (height * height) ;
  
  
  EnvLibEntryNode* leftNode = activeEnvelope ->head;
  EnvLibEntryNode* rightNode = leftNode->rightSeg->rightNode;
  
  while (leftNode->x <insertx&& rightNode->x< insertx){
    leftNode = rightNode;
    rightNode = rightNode->rightSeg->rightNode;  
  }



  leftNode->rightSeg->segmentType = envSegmentTypeExponential;
 
  showGraph(activeEnvelope);


}





void EnvLibDrawingArea::removeNode(){
  if (activeNode->leftSeg ==NULL || activeNode->rightSeg ==NULL){
    return; //can't remove head or tail
  }

  activeNode->leftSeg->rightNode = activeNode->rightSeg->rightNode;
  activeNode->rightSeg->rightNode->leftSeg = activeNode->leftSeg;
  delete activeNode->rightSeg;
  delete activeNode;
  activeNode = NULL;

  EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
  showGraph(activeEnvelope);

}





void EnvLibDrawingArea::moveNode(){


  EnvelopeLibraryEntry* activeEnvelope = envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope ==NULL) return;
  if (activeNode==NULL){ 
    return;
  }

  envelopeLibraryWindow->activeProject->modified();  
  
  double leftBound;
  double rightBound;
  
  if (activeNode->leftSeg!=NULL){
    leftBound = activeNode->leftSeg->leftNode->x + 0.001;
  }
  if (activeNode->rightSeg!=NULL){
    rightBound = activeNode->rightSeg->rightNode->x - 0.001;
  }
  
 

  if (activeNode->leftSeg == NULL|| activeNode->rightSeg==NULL){ //head and tail
    activeNode->y = mouseY;
  }
  
  else {  
 
    activeNode->x = (mouseX>= leftBound)?((mouseX<=rightBound)?mouseX:rightBound):leftBound;
    activeNode->y = mouseY;
  }
  
  
  
  
  
  char xs[10];
  char ys[10];
  
  sprintf(xs, "%.4f", activeNode->x);
  sprintf(ys, "%.4f", activeNode->y);
  
  std::string xss = string(xs);
  std::string yss = string(ys);
  
  
  envelopeLibraryWindow->setEntries(xss, yss);
  
  
  
  
  showGraph(activeEnvelope);

}

void EnvLibDrawingArea::resetFields(){
  activeNode = NULL;
  moveLeftBound =0;
  moveRightBound = 0;
  head = NULL;;
  tail = NULL;  
}


void EnvLibDrawingArea::setActiveNodeCoordinate(string _x, string _y){
  EnvelopeLibraryEntry* activeEnvelope = 
    envelopeLibraryWindow->getActiveEnvelope();
  if (activeEnvelope == NULL){
    return;
  }

  if (activeNode ==NULL){
    return;
  }
  activeNode->x = atof (_x.c_str());
  activeNode->y = atof (_y.c_str());
  
  showGraph(activeEnvelope);
}

