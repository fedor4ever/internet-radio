/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
*
*/



#ifndef IRMEDIACLIENT_H
#define IRMEDIACLIENT_H

#include <e32base.h>

#include "ircontrolparams.h"
#include "irmediaenginedef.h"
#include <mdaaudiooutputstream.h>

class CIRCtrlCmdObserver;
class CIRMediaEngineInterface;

//========================================class declaration CIRMediaEngineInterface============================================

/**
 * This is client class for irmediaengine which a ecom dll 
 * Any request to media engine is routed through media client
 * This client creates proper player for mp3 or acc/accp according 
 * to the mime type To start a player StartNewPlayerL function is to be
 * used, which in turn trigger play and this should be called before 
 * calling play or stop functions
 * The buffers are created and filled with data. 
 * media engines instance is created and associates the filled buffer
 * to media engine, finally the media engine's and buffer associated is deleted
 * for each format we delete and recreate the player and buffer
 * This also interacts with ircommandchannel.dll for filling data,triggering
 * audiofadeout etc.
 * 
 * @code
 *
 * CIRCtrlCmdObserver* channel; //instance of CIRCtrlCmdObserver defined
 *							   //in ircommandchannel.dll 
 * client = CIRMediaClient* NewL(); //instance of media client
 * TConfig config;
 * TBuf<20> format;
 * _LIT(KFormat,"audio/mpeg");
 * client->StartNewPlayer(config,KFormat,channel); //Initiates the player
 *             //Creates buffer fills the buffer, Creates instance of player 
 *             //and associates filled buffer with it triggers playing.
 *			   //Once this done irmediaengine takes care of buffering issues
 *			   //Deletes the old instance of player as well as buffer associated
 *			   //before creating a new one
 * client->Stop(); //stop the player
 * client->Play(); //plays the player
 * client->SetVolume(); //sets the volume
 * client->MaxVolume(); //gives the maximum volume
 * client->Volume(); //returns the current volume
 * client->BufferFilled(); //is a call back when buffer is filled this has to be called
 * client->StopMediaBuffering(); //stop buffering when buffering is handled in irmediaengine 
 * client->StopInitialBuffering(); //stop buffering when buffering is handled in
 * 									//irmediaclient
 * @endcode
 * 
 */

NONSHARABLE_CLASS(CIRMediaClient) : public CBase
	{											// Function declaration
	
	//Constructor functions
public:
	
	/**
	 * Function : NewL
	 * Two Phase Constructor - NewL
	 * @return an instance of the player component	
	 */	 
 	 static CIRMediaClient* NewL(CIRCtrlCmdObserver* aChannel);

 	/**
	 * Function : NewLC
	 * Two Phase Constructor - NewLC
	 * @return an instance of the player component
	 */	
  	 static CIRMediaClient* NewLC(CIRCtrlCmdObserver* aChannel);

	/**
	 * destructor function
	 * destructor of the player component
	 */
  	~CIRMediaClient();

private:  	
	//Constructor function
	
	/**	
	 * This is default Constructor
	 * for the class CIRMediaClient
	 */			
    CIRMediaClient();
    	
	/**	
	 * Two phase ConstructL
	 * network component is taken as input
	 * @param instance of network
	 */  	
    void ConstructL(CIRCtrlCmdObserver* aChannel);


public: 
	
												// Function to control the play
	  
	/**
	 * Function : Play
	 * Starts to play the stream
	 * @return KErrNone if player is existing if not it returns KErrNotFound
	 */ 
	 TInt Play();  
	    
	/**
	 * Function : Stop
	 * Stops to play the stream
	 */	      
  	 void Stop();
   	     
     											//Function to control the volume
       
	/**
	 * Function : SetVolume
	 * function to set the volume, 
	 * @param integer level of volume is the input
	 */		
	 void SetVolume(TInt aVolume);
	 
	/**
	 * Function : MaxVolume
	 * @returns the maximum volume , integer value is the Output
	 */			
	TInt MaxVolume() const;
	
	/**
	 * Function : Volume
	 * function to returns the volume
	 * @return integer level of voume is the Output
	 */		
	 TInt Volume() const;    

	/**
	 * Function: BufferFilled
	 * Set the whether a Fill Buffer is currently active
	 */
  	 void BufferFilled();

	/**
	 * Function: StartNewPlayerL
	 * Set the whether a Fill Buffer is currently active
	 * @param configuration settings
	 * @param format type
	 * @param command channel's instance
	 */
	 void StartNewPlayerL(TConfig& aConfig,const TDesC8& aFormat);
  	 
	 /**
	 * Function: StopMediaBuffering
	 * stops buffering of media engine 
	 */
  	 void StopMediaBuffering();

	 /**
	 * Function: StopMediaBuffering
	 * stops buffering from media client, media engine is not stopped 
	 */
	 void StopInitialBuffering();
	 
	 /**
	 * Function: StartMediaBuffering
	 * starts buffering of media engine 
	 */
	 void StartMediaBuffering();
	/**
    * GetMediaClientInstance()
    * Returns the Audio Player  Instance
    */
	CMdaAudioOutputStream* GetPlayerInstance();
	 
private:
	
    /**
     * Creates instance of the media engine ECom plugin.
     * 
     * @return ETrue, if the creation succeeded, EFalse otherwise 
     */
    TBool CreatePlayer();
    
	/**
	 * initializes the player with codec settings, initialised buffer 
	 * and Control command instance.
	 * 
	 * @param configuration settings
	 * @param command channel's instance
	 */	
	 void InitializePlayer( TConfig& aConfig,CIRCtrlCmdObserver* aChannel);
						

private:
								    	//data members

			
	//Instance of Interface definision			
	CIRMediaEngineInterface* iPlayer;
	
	//Instance of channel through with data is sent UI
	CIRCtrlCmdObserver* iChannel;
	
	//The instance of buffer which is currently playing
	TUint8* iCurrentBuffer;

	//The pointer to buffer which being created for a new player
	TUint8* iTempBuffer;

	//pointer to buffer being filled
	TUint8* iInputBufferPtr;

	//holds the format type
	TBuf8<20> iFormat;

	//buffer which is being filled
	TPtr8 iInputBuffer;
	
	//hold the configuration information
	TConfig iConfig;
	
	//Holds the percentage of data in the buffer
	TInt iBufferPercentage;	
	
	//Set of commands that can be sent to command channel
	TIRControlCommmand iCommand;
	
	//checks whether the player is a newly created player
	TBool iNewPlayer;

	//checks whether the execution of code is required for only first time or not 
	TBool iFirstTime;

	//checks whether to stop buffering
	TBool iStopBuffering;
	};

#endif //IRMEDIACLIENT_H


