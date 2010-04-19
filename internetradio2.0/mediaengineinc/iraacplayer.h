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


#ifndef IRAACPLAYER_H
#define IRAACPLAYER_H

#include <audio.h>
#include <mdaaudiooutputstream.h>

#include "ircontrolparams.h"
#include "irmediaenginedef.h"
#include "irmediaengineinterface.h"

class CIRBufferContainer;

//========================================class declaration CIRAACPlayer============================================

/**
 * This class is the implementation of AAC/AAC+ format.This forms a part of
 * media engine which is implemented as an ECom plugin. The interface class
 * is irmediaengineinterface.h  The mime type of mp3 is used to load this
 * plugin mimetype : audio/aac or audio/accp This is to be passed as the   
 * default data while creation of player
 * 
 * @code
 *
 * //mimetype is buffer with audio/aac or audio/aacp as data
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
 *
 */

NONSHARABLE_CLASS(CIRAACPlayer) : public CIRMediaEngineInterface,
	public MMdaAudioOutputStreamCallback
	{
public:
									//Member Functions
	/**
	 * Function : NewL
	 * function to return an instance of CIRAACPlayer
	 * Two phase constructor
	 * @return instance of CIRAACPlayer
	 */
	static CIRAACPlayer* NewL();

	/**
	 * Function : NewLC
	 * function to creates an instance of CIRAACPlayer
	 * Two phase constructor
	 * @return instance of CIRAACPlayer
	 */
	static CIRAACPlayer* NewLC();	

	/**
	 * Function : ~CIRAACPlayer
	 * Default Destructor
	 */
	~CIRAACPlayer();

										//Function which control the play

	/**
	 * Function : Play
	 * function to which intiates the playing
	 */
	void Play();
	
	/**
	 * Function : Stop
	 * function to stop the player
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
	void Intialize(TConfig& aConfig,TUint8* aInitParams,
		CIRCtrlCmdObserver* aChannel);			
	
	/**
	 * Function: StopPlayerBuffering
	 * Function is used to freeze bufferring 
	 */
	void StopPlayerBuffering();
	
	/**
	 * Function: BufferFilled
	 * Function which is called when network gets the buffer filled with data
	 */
  	void BufferFilled();
  	
  	/**
	 * Function: GetAudioPlayer
	 * Function which is called To return the Current Audio Player
	*/
	CMdaAudioOutputStream* GetAudioPlayer();

private:	
	/**
	 * Function: CIRAACPlayer
	 * This function is the default constructor
	 */
	CIRAACPlayer();

	/**
	 * Function: ConstructL
	 * Two phase constructor is used to intialize data members
	 */
	void ConstructL();	

	/**	
	 * Function: CreatebufferL 
	 * Create the buffers and allocates address for each buffers
	 * keeps them in buffer queue
	 */       
	void CreateBufferL();
	
	/**
	 * Function: ReCreateBufferL
	 * Function is used to recreate the buffer
	 */	
	void ReCreateBufferL();

										//Call back functions from MMdaAudioOutputStreamCallback
	
	/**
	 * Function: MaoscBufferCopied
	 * call back to implement for using CMdaAudioOutputStream
	 * Call as callback from the CMdaAudioOutputStream::WriteL
	 * after frame work copies the stream to buffer
	 * @param Error
	 * @param reference to buffer
	 */		
	void MaoscBufferCopied(TInt aError, const TDesC8 &aBuffer);
		
	/**
	 * Function: MaoscPlayComplete
	 * call back to implement for using CMdaAudioOutputStream
	 * Call as callback from the CMdaAudioOutputStream::WriteL
	 * after play is completed
	 * @param aError
	 */	    
	void MaoscPlayComplete(TInt aError);
	
	/**
	 * Function: MaoscOpenComplete
	 * call back to implement for using CMdaAudioOutputStream
	 * Call as callback from the CMdaAudioOutputStream::Open
	 * @param aError
	 */	    
	void MaoscOpenComplete(TInt aError);				
		
	/**
	 * Function: GetAACAudioProperties
	 * extract all the information to start the play from the stream
	 */
	void GetAACAudioProperties();
	
	/**
	 * Function: DoFindnDecodeAACHeader
	 * Function return a non modifiable pointer descriptor of data buffer
	 */
	void DoFindnDecodeAACHeaderL();
	
	/**
	 * Function: ReArrangeBuffer
	 * Function rearranges the buffer, and should be called after finding the header
	 * from the address of buffers specified, the data buffers are kept in sink queue and 
	 * remaining buffers are discarded that is put into source queue.
	 * 
	 * @param address of starting of header
	 */	
	void ReArrangeBufferL(TUint8* aInput);


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
	
	//Temporary holder for buffer container
	CIRBufferContainer* iTempBufferHolder;
	
	//Holds the address of data buffer which is to be played
	TUint8* iTempbuffer;				

	//This represents a large input buffer which takes the input
	TUint8* iInputBufferPtr;
	
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
	
	//checks whether header is found or not
	TBool iHeaderFound;
	
	//this bool indicates buffering is taking place initially or rebuffering
	//mainly to intial parameter like volume during rebuffering
	TBool iNeedReBuffering;
	
	//Indicates that it is a new player
	TBool iNewPlayer;
	
	//This is used to StopPlayerBuffering
	TBool iStopPlayerBuffering;	
	
	//This is used to stop buffering if its rebuffering, set if stop function is called
	//and  reset when play function is called
	TBool iStopState;
	
	//skips sending the error code during stop if stop internally 
	//is during play called
	TBool iSkipPlayCompleted;	
	};  

#endif //IRAACPLAYER_H
