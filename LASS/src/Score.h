/* LASS (additive sound synthesis library) Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)
Modified by Ming-ching Chiu 2013

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
//	Score.h	
//
//----------------------------------------------------------------------------//

#ifndef __SCORE_H
#define __SCORE_H

//----------------------------------------------------------------------------//
#include "StandardHeaders.h"

#include "XmlReader.h"
#include "Types.h"
#include "Collection.h"
#include "MultiTrack.h"
#include "Sound.h"
#include "Reverb.h"
#include "XmlReader.h"

class Utilities;

//----------------------------------------------------------------------------//

#define MAX_SOUND_OBJECTS 200
#define MAX_RENDERED_OBJECTS 20

/**
*	A Score simply consists of a collection of Sounds.
*	In addition to this, it provides functionality for 
*	managing clipping in a piece.
*	\author Braden Kowitz
*
* Update May 2013 by Ming-ching:
* Score is now responsible for lauching multi-threaded rendering and the final
* mix down.
**/

class Score{
public:

    /**
    *	This is the default constructor.  It sets the ClippingManagementMode 
    *	to NONE.
    **/
    Score(int _numThreads,  int _numChannels, int _samplingRate );
    
    /**
    * Sound objects created by CMOD is added to the score via this function.
    **/
    void add(Sound* _sound);
    
    /**
    * Worker threads give the renderedSound back to the main thread to compositie
    **/
    void addRenderedSound(m_time_type _startTime, MultiTrack* _renderedSound);
    
    /**
    * This function is called by the working threads to test if CMOD
    * has finished adding sounds to the score. If vector<Sound*> sounds has 
    * size 0 and doneGettingSoundObjects is true, the working thread returns.
    **/
    bool isDoneGettingSoundObjects(){return doneGettingSoundObjects;}
    
    /**
    * This function is called by the compositeThread.
    **/
    void compositeRenderedSounds();
    
    /**
    * The final mix down and clean up after all the sound objects are rendered.
    **/
    MultiTrack* joinThreadsAndMix();
    
    /**
    * Called by the working threads to determine if they need to update the
    * duration of the MultiTrack objects currently in use.
    **/
    m_time_type getScoreEndTime(){return scoreEndTime;}
    
    /**
    *	\enum ClippingManagementMode
    *	This sets the clipping management mode for this score.
    *	This post-process is run after the score is rendered.
    **/

    /**
    *	\var ClippingManagementMode NONE
    *		- No clipping management is taken at all.
    *		- This lets the composer render once,
    *		  then try different post-processes, saving some time.
    **/

    /**	
    *	\var ClippingManagementMode CLIP
    *		- Any value over 1 or under -1 is clipped to these limits.
    **/

    /**
    *	\var ClippingManagementMode SCALE
    *		- The max amplitude value is found in the entire score
    *             (all tracks).
    *		- Each track is then scaled by 1/maxAmplitude.
    **/

    /**
    *	\var ClippingManagementMode CHANNEL_SCALE
    *		- For each track, a max amplitude value if found.
    *		- This track is then scaled by 1/maxAmplitude
    **/

    /**
    *	\var ClippingManagementMode ANTICLIP
    *		- If the total amplitude accross tracks at any given
    *		  time is greater than 1, then that sample is scaled
    *		  on all tracks by 1/totalAmplitude.
    **/

    /**
    *	\var ClippingManagementMode CHANNEL_ANTICLIP
    *		- For each channel, and each sample
    *		- if a sample has an amplitude greater than 1,
    *		  then that sample is scaled by 1/amplitude.
    **/	
    enum ClippingManagementMode
    {
        NONE,
        CLIP,
        SCALE,
        CHANNEL_SCALE,
        ANTICLIP,
        CHANNEL_ANTICLIP
    };

    /**
    *	This function sets the ClippingManagementMode for this score.
    *	\param mode The ClippingManagementMode to set
    **/
    void setClippingManagementMode(ClippingManagementMode mode);

    /**
    *	This function gets the current ClippingManagementMode for this score.
    *	\return The ClippingManagementMode
    **/
    ClippingManagementMode getClippingManagementMode();

    /**
    *	This function manages clipping on a MultiTrack object with the 
    *	specified mode. This is performed automatically when a score is 
    *	rendered, but is available for the user to render once, then 
    *	post-process many times.
    *	\param mt The MultiTrack to clip
    *	\param mode The ClippingManagementMode
    **/
    static void manageClipping(MultiTrack* mt, ClippingManagementMode mode);

    /**
    *   This function performs reverb in the render() method.
    *	\param newReverbObj The Reverb object
    **/
	  void use_reverb(Reverb *newReverbObj);
	
	
	  /**
	  * returns the final rendered score.
	  **/
	  MultiTrack* doneAddingSounds();
	  
	  /**
	  * Increase the length of MultiTrack* score to fit sounds
	  **/
	  void checkScoreMultiTrackLength(); 
  
    
//    /** 
//    *	\deprecated
//    *	This outputs an XML representation of the object to STDOUT
//    *
//    **/
//    void xml_print( );
//    /**
//    *	\deprecated
//    **/
//    void xml_print( ofstream& xmlOutput );
//    /**
//    *	\deprecated
//    **/
//    void xml_print( const char * xmlOutputPath );
//    /**
//    *	\deprecated
//    **/
//    void xml_read( XmlReader::xmltag *scoretag);
    
    DISSCO_HASHMAP<long, Reverb *>* reverbHash;
    DISSCO_HASHMAP<long, DynamicVariable *>* dvHash;

private:
    ClippingManagementMode cmm_;
	
    Reverb *reverbObj;
   
    /**
    * This private function clips the MultiTrack.
    * \param mt The MultiTrack to clip
    **/
    static void clip(MultiTrack* mt);
   
    /**
    * This private function scales the MultiTrack.
    * \param mt The MultiTrack to scale
    **/
    static void scale(MultiTrack* mt);

    /**
    * This private function scales the channels in the MultiTrack.
    * \param mt The MultiTrack to scale
    **/
    static void channelScale(MultiTrack* mt);

    /**
    * This private function unclips the MultiTrack.
    * \param mt The MultiTrack to unclip
    **/
    static void anticlip(MultiTrack* mt);

    /**
    * This private function unclips the channels in a MultiTrack.
    * \param mt The MultiTrack to unclip
    **/
    static void channelAnticlip(MultiTrack* mt);
    
  
   
  
    /**
    * stores the Sound objects to be rendered.
    **/
    std::vector<Sound*> sounds;
    
    /**
    * stores the rendered sounds for the main thread to composite them
    **/
    std::vector<std::pair<m_time_type, MultiTrack*>*> renderedSounds;
    
    /**
    * The MultiTrack object which holds the actual score
    **/
    MultiTrack* scoreMultiTrack;
    
    /**
    * The max end time seen among the added sound objects
    **/
    m_time_type scoreEndTime;
    m_time_type scoreMultiTrackLength;
    
    /**
    * Number of threads
    **/
    int numThreads;
    
    /**
    * An array to hold thread objects
    **/
    pthread_t* threads;
    pthread_t compositeThread;
    
    /**
    * counter: # of sounds rendered.
    **/
    int soundsRendered;
    
    /**
    * counter: # of sound Objects passing in so far.
    **/
    int soundObjectsCreated;
    
    /**
    * A flag to indicate that the Score object has done receiving all the 
    * sound objects.
    **/
    bool doneGettingSoundObjects;
    bool workerThreadsAllJoined;
  
    /**
    * mutex to protect vector<Sound*> sounds
    **/
    pthread_mutex_t mutexSoundVector;
    
    /**
    * mutex to protect MultiTrack* scoreMultiTrack
    **/
    pthread_mutex_t mutexVectorRenderedSound;
    
    // Rubin Du July 2024: restructured and replaced conditions with semaphores for better resources management and synchronization
    ///**
    //* cond to brocast to worker threads the status of vector<Sound*> sounds
    //**/
    //pthread_cond_t conditionSoundVector;

    /**
    * semaphores for work queue management
    **/
    sem_t semEmptySlotsSounds;
    sem_t semFullSlotsSounds;
    sem_t semEmptySlotsRendered;
    sem_t semFullSlotsRendered;
    
    /**
    * Number of channels
    **/
    int numChannels;
    
    /**
    * Sampling rate
    **/
    int samplingRate;
    
};


//----------------------------------------------------------------------------//
#endif //__SCORE_H


