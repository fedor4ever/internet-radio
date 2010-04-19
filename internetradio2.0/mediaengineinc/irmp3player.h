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


#ifndef IRMP3PLAYER_H
#define IRMP3PLAYER_H

#include <audio.h>
#include <mdaaudiooutputstream.h>

#include "ircontrolparams.h"
#include "irmediaenginedef.h"
#include "irmediaengineinterface.h"

class CIRBufferContainer;
#ifdef __WINS__
class CIRTestingAudioPlayer;
#endif //__WINS__

//========================================class declaration CIRMP3Player============================================

/**
 * This class is the implementation of MP3 format.This forms a part of
 * media engine which is implemented as an ECom plugin. The interface class
 * is irmediaengineinterface.h . The mime type of mp3 is used to load this
 * plugin mimetype : audio/mpeg This is to be passed as the default data 
 * while creation of player
 * 
 * @code
 *
 * //mimetype is buffer with audio/mpeg as data
 * iPlayer = CIRMediaEngineInterface::NewL(mimetype);
 *
 * TConfig config;
 * config.iVolume = xxxxxx; //some integer value between 0 and Maximum Volume
 * TUint8* buffer; //is a buffer of size [KINPUT_BUFFER_SIZE] as defined in  IRMediaEngineBuffer.h
 *                 //this is should have filled buffer when you call Intialize
 * CIRCtrlCmdObserver* aChannel; //command channel dll's instant
 * TInt volume;
 * iPlayer->Intialize(config,buffer,channel);
 * iPlayer->Play();//to play
 * iPlayer->Stop();//to stop
 * iPlayer->SetVolume(volume);//set the volume
 * iPlayer->Volume;// returns current volume
 * iPlayer->MaxVolume();//return maximum volume
 * iPlayer->StopPlayerBuffering();//stop buffering if required
 * iPlayer->StartPlayerBuffering();//start buffering (incase if stop you can restart with this method)
 * iPlayer->BufferFilled(); //should be called when the buffer given by player is return
 *                     //this will mark the buffer as filled
 *
 * @endcode
 */

NONSHARABLE_CLASS(CIRMP3Player) : public CIRMediaEngineInterface,
								  public MMdaAudioOutputStreamCallback
	{
public:
									//Member Functions
	/**
	 * Function : NewL
	 * function returns an instance of CIRMP3Player
	 * Two phase constructor
	 * @return instance of CIRMP3Player
	 */
	static CIRMP3Player* NewL();

	/**
	 * Function : NewLC
	 * function creates an instance of CIRMP3Player
	 * Two phase constructor
	 * @return instance of CIRMP3Player
	 */
	static CIRMP3Player* NewLC();
	
	/**
	 * Function : ~CIRMP3Player
	 * Default Destructor
	 */
	~CIRMP3Player();

										//Function which control the play

	/**
	 * Function : Play
	 * function to which intiate the player
	 */
	void Play();
	
	/**
	 * Function : Stop
	 * function to which stop the player
	 */
	void Stop();   	     

										 //Function to control the volume
	       
	/**
	 * Function : SetVolume
	 * function to set the volume, integer level of volume is the input
	 * @param integer level of volume is the input
	 */			
	void SetVolume(TInt aVolume);
	
	/**
	 * Function : MaxVolume
	 * function to returns the maximum volume , integer value is the Output
	 * @return integer value is the Output
	 */		
	TInt MaxVolume() const;
	
	/**
	 * Function : Volume
	 * function to returns the volume, integer level of voume is the Output
	 * @return integer value is the Output
	 */		
	TInt Volume() const;
	
	/**
	 * Function: Intialize
	 * Set the codec type and sampling rate channel of stream
	 * This is set to initial settings which is required to start the player
	 * @param configuration infomation
	 * @param instance of data buffer, whose size is defined in IRMediaEngineBuffer.h
	 * @param instance of the channel through with information is sent to UI
	 */			
	void Intialize(TConfig& aConfig,TUint8* aInitParams,CIRCtrlCmdObserver* aChannel);
		
	/**
	 * Function: StopPlayerBuffering
	 * Function is used to stop bufferring 
	 */
	void StopPlayerBuffering();
	
	/**
	 * Function: BufferFilled
	 * Function which is called when the buffer filled with data
	 */
  	void BufferFilled();
  	
  	/**
	 * Function: GetAudioPlayer
	 * Function which is called when the Player Instance has Changed
	*/
	CMdaAudioOutputStream* GetAudioPlayer();

private:
	/**
	 * Function: CIRMP3Player
	 * This function is the default constructor
	 */
	CIRMP3Player();

	/**
	 * Function: ConstructL
	 * Two phase constructor is used to intialize data members
	 */
	void ConstructL();	

	/**	
	 * Function: CreateBufferL 
	 * Create the buffers and allocates address for each buffers
	 * keeps them in buffer queue
	 */       
	void CreateBufferL();
	
	/**
	 * Function: ReCreateBufferL
	 * Function is used to recreate the buffer
	 */	
	void ReCreateBufferL();
	
	 /**
	 * Function: GetMP3AudioProperties
	 * extract all the information to start the play from the stream by calling DoFindnDecodeMP3Header
	 */
	void GetMP3AudioProperties();
	
	/**
	 * Function: DoFindnDecodeMP3Header
   	 * Function is used to find the mp3 header and decode the mp3 header
	 */
	void DoFindnDecodeMP3Header();
	
	/**
	 * Function: CrossCheckHeader
	 * Function is cross checks sampling rate 
	 * @param a pointer to input buffer
	 * @return ETrue if the sampling rate matches with sampling rate of next MP3 frame
	 */		
	TBool CrossCheckHeader(TUint8* aInput);
								
								//Call back functions
public:	
	/**
	 * Function: MaoscBufferCopied
	 * call back to be implement for using CMdaAudioOutputStream
	 * Call as callback from the CMdaAudioOutputStream::WriteL
	 * after frame work copies the stream to buffer
	 * @param Error code
	 * @param reference to buffer
	 */		
	void MaoscBufferCopied(TInt aError, const TDesC8 &aBuffer);
	
	/**
	 * Function: MaoscPlayComplete
	 * call back to  beimplement for using CMdaAudioOutputStream
	 * after play is completed
	 * @param error code
	 */	    
	void MaoscPlayComplete(TInt aError);
		
	/**
	 * Function: MaoscOpenComplete
	 * call back to implement for using CMdaAudioOutputStream
	 * Call as callback from the CMdaAudioOutputStream::Open
	 * @param error code
	 */	    
	void MaoscOpenComplete(TInt aError);	

										        //data members
										        
private:	
	//Enum for Play State
	enum TState
		{
		ENotReady,    //Not ready to play
		EReadyToPlay, //Ready to play
		EPlaying      //Playing
		};

	// Player state	
	TState iState; 		


	//For implement Player functionalities
	
	//player utility 
	CMdaAudioOutputStream* iAudioPlayer;
#ifdef __WINS__	
	//Instance for TestingAudioPlayer
	CIRTestingAudioPlayer* iTestingAudioPlayer;
#endif //__WINS__
	
	//Holds the address of data buffer which is to be played
	TUint8* iTempbuffer;				

	//This represents a large input buffer which takes the input
	TUint8* iInputBufferPtr;			 
	
	//Temporary holder for buffer container
	CIRBufferContainer* iTempBufferHolder;
	
	//Instance of channel through with data is sent UI
	CIRCtrlCmdObserver* iChannel;
		
	//Setting for sampling rate, channel
	TMdaAudioDataSettings iSettings;  
				
	//FourCC representation of the codec
	TFourCC iDataType;	
				
	//hold the configuration information
	TConfig iConfig;  				
				
	//Hold the pointer to the buffer to be filled
	TPtr8 iInputBuffer;				 

	//Contains Data to be played
	TPtr8 iInput;
		
	//Queue of buffer which is filled
	TSglQue<CIRBufferContainer> iSourceBufferQ;
		
	//Queue of buffer which is empty
	TSglQue<CIRBufferContainer> iSinkBufferQ;
	
	//Holds the audio format data
	TAudioFormatData iAudioInfo;

	//Holds the percentage of data in the buffer
	TInt iBufferPercentage;
	
	// For First time play
	TBool iFirstTime;
	
	//this bool indicates buffering is taking place initially or rebuffering
	//mainly to intial parameter like volume during rebuffering
	TBool iNeedReBuffering;
	
	//Indicates that it is a new player
	TBool iNewPlayer;
	
	//This is used to StopPlayerBuffering while playing 
	TBool iStopPlayerBuffering;
	
	//This is used to stop buffering if its rebuffering, set if stop function is called
	//and  reset when play function is called
	TBool iStopState;

	//Indicates that player is rebuffering
	TBool iReBuffering;
	
	//skips sending the error code during stop if stop internally 
	//is during play called
	TBool iSkipPlayCompleted;
	};  

#endif//IRMP3PLAYER_H


