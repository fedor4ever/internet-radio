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


#include <AudioPreference.h>

#include "irbuffercontainer.h"
#include "irctrlcmdobserver.h"
#include "irdebug.h"
#include "irmediaenginebuffer.h"
#include "irmp3player.h"
#include "irmp3playerlocal.h"

#ifdef __WINS__
#include "irtestingaudioplayer.h"
#endif //__WINS__



// Constants
const TInt KZero = 0;
const TInt KOne = 1;
const TInt KTwo = 2;
const TInt KThree = 3;

const TInt KTwentyFour = 24;
const TInt KSixteen = 16;
const TInt KEight = 8;
const TInt KSamplesPerFrame1 = 576;
const TInt KSamplesPerFrame2 = 1152;
const TInt KOneFourFour = 144;
const TInt KThousand = 1000;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Function : NewL
// function returns an instance of CIRMP3Player
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRMP3Player* CIRMP3Player::NewL()
	{
	IRLOG_DEBUG( "CIRMP3Player::NewL" );
	CIRMP3Player* self = CIRMP3Player::NewLC();
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRMP3Player::NewL - Exiting." );
	return self;
	}
	
// ---------------------------------------------------------------------------
// Function : NewLC
// function creates an instance of CIRMP3Player
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRMP3Player* CIRMP3Player::NewLC()
	{
	IRLOG_DEBUG( "CIRMP3Player::NewLC" );
	CIRMP3Player* self = new (ELeave) CIRMP3Player;
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRMP3Player::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : ~CIRMP3Player
// Default Destructor
// ---------------------------------------------------------------------------
//
CIRMP3Player::~CIRMP3Player()
	{
	IRLOG_DEBUG( "CIRMP3Player::~CIRMP3Player" );
	//delete the instance of the player
	delete iAudioPlayer;
	
#ifdef __WINS__
	if(iTestingAudioPlayer)
		{
		iTestingAudioPlayer->Close();
		delete iTestingAudioPlayer;
		iTestingAudioPlayer = NULL;
		}
	CActiveScheduler::Delete(iTestingAudioPlayer);

#endif //__WINS__

	while(!iSinkBufferQ.IsEmpty())
		{
		//Deleting all the entries in sink buffers queue
		iTempBufferHolder = iSinkBufferQ.First();
		iSinkBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		}
	while(!iSourceBufferQ.IsEmpty())
		{
		//deleting all the entries in source buffers queue
		iTempBufferHolder = iSourceBufferQ.First();
		iSourceBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		}
	IRLOG_DEBUG( "CIRMP3Player::~CIRMP3Player - Exiting." );	
	}
									//Function for Play control

// ---------------------------------------------------------------------------
// Function : Play
// function to which intiate the player
// ---------------------------------------------------------------------------
//
void CIRMP3Player::Play()
	{
	IRLOG_DEBUG( "CIRMP3Player::Play" );
	//! then we have to stop it then restart the play from begining
	if ( EPlaying == iState )
		{
		//internally stopped before playing so no need of sending
		//stop status		
		iSkipPlayCompleted = ETrue;
		
#ifdef __WINS__
		iTestingAudioPlayer->Stop();
#else 
		iAudioPlayer->Stop(); 
#endif //__WINS__
		
		iSkipPlayCompleted = EFalse;		
		}
	iState = ENotReady;
	iStopState = EFalse;
	iBufferPercentage = KZeroPercentage;
	
	//note : using TRAP_IGNORE to suppress a codescanner warning
	//"Ignoring the return value from Open() functions"
	//this cannot be checked as this symbian API returns void
#ifdef __WINS__
	TRAP_IGNORE ( iTestingAudioPlayer->Open() );
#else 
	//opening the current player component
	TRAP_IGNORE ( iAudioPlayer->Open(&iSettings) );
#endif //__WINS__
	iChannel->SentRequest(EPlayingState,KErrNone);	
	IRLOG_DEBUG( "CIRMP3Player::Play - Exiting." );	
	}

// ---------------------------------------------------------------------------
// Function : Stop
// function to which stop the player
// ---------------------------------------------------------------------------
//
void CIRMP3Player::Stop()
	{
	IRLOG_DEBUG( "CIRMP3Player::Stop" );	
	//If the current state is playing 	
	if ( EPlaying == iState )
		{
#ifdef __WINS__
		iTestingAudioPlayer->Stop();
#else
		iAudioPlayer->Stop();	
#endif //__WINS__
		
		}		
	else
		{
	IRRDEBUG2("CIRMP3Player::Stop EStoppedPlaying", KNullDesC); 
		//sending the updated status as stopped
		iChannel->SentRequest( EStoppedPlaying, KErrNone );	
		}		
	iState = EReadyToPlay;
	iStopState = ETrue;
	IRLOG_DEBUG( "CIRMP3Player::Stop - Exiting." );	
	}
										
										//Functions for Volume Control

// ---------------------------------------------------------------------------
// Function : SetVolume
// function to set the volume 
// ---------------------------------------------------------------------------
//	
void CIRMP3Player::SetVolume(TInt aVolume )
	{	
	IRLOG_DEBUG( "CIRMP3Player::SetVolume" );		
	//! If volume should be less than maximum value and greater than or equal to zero then set the volume
	if( KZeroVolume <= aVolume && iAudioPlayer->MaxVolume() >= aVolume )
		{
		iAudioPlayer->SetVolume( aVolume );	
		iConfig.iVolume = iCurrentVolume = aVolume;
		}		
	IRLOG_DEBUG( "CIRMP3Player::SetVolume - Exiting." );	
	}

// ---------------------------------------------------------------------------
// Function : MaxVolume
// function to returns the maximum volume 
// ---------------------------------------------------------------------------
//
TInt CIRMP3Player::MaxVolume() const
	{
	IRLOG_DEBUG( "CIRMP3Player::MaxVolume" );
	return iAudioPlayer->MaxVolume();	
	}

// ---------------------------------------------------------------------------
// Function : Volume
// function to returns the volume, integer level of volume is the Output
// ---------------------------------------------------------------------------
//
TInt CIRMP3Player::Volume() const
	{
	IRLOG_DEBUG( "CIRMP3Player::Volume" );
	return iAudioPlayer->Volume();
	}

									//Intialization of Codec Settings

// ---------------------------------------------------------------------------
// Function: Intialize
// Set the codec type and sampling rate channel of stream
// This is set to initial settings which is required to start the player
// ---------------------------------------------------------------------------
//
void CIRMP3Player::Intialize(TConfig& aConfig,TUint8* aInitParams,
	CIRCtrlCmdObserver* aChannel)
	{
	IRLOG_DEBUG( "CIRMP3Player::Intialize" );
	iInputBufferPtr = reinterpret_cast<TUint8*> (aInitParams); //instance of buffer
	iConfig = aConfig; //! Set all the configuration information like volume
	iPlayBufferSize = iConfig.iPlayBufferSize;
	iInputBufferSize = iConfig.iPlayBufferCount*iPlayBufferSize;
    iBufferOffset = iPlayBufferSize;
	iDataType.Set(KMMFFourCCCodeMP3); //! Set the data type as MP3
	iChannel = reinterpret_cast<CIRCtrlCmdObserver*> (aChannel);
	TRAPD(err,CreateBufferL()); // allocates buffer to the queue
	if( err )
		{
	IRRDEBUG2("CIRMP3Player::Intialize - EError", KNullDesC);
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
		return;	
		}
	iChannel->SentRequest(EPlayerChanged,KErrNone);									//creates an instance of the player
	IRLOG_DEBUG( "CIRMP3Player::Intialize - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: StopPlayerBuffering
// Function is used to stop buffering 
// ---------------------------------------------------------------------------
//
void CIRMP3Player::StopPlayerBuffering()
	{
	IRLOG_DEBUG( "CIRMP3Player::StopPlayerBuffering" );
	//stops the player from buffering
	iStopPlayerBuffering = ETrue;	
	}

// ---------------------------------------------------------------------------
// Function: BufferFilled
// Function which is called when network gets the buffer filled with data
// ---------------------------------------------------------------------------
//
void CIRMP3Player::BufferFilled()
	{
	IRLOG_DEBUG( "CIRMP3Player::BufferFilled" );
	if( !iNewPlayer )
		{
		if( !iSourceBufferQ.IsEmpty() )
			{
			//! Initially all unfilled buffers are in source buffer Queue
			//! Once the buffer in the source buffer queue is filled it is moved to queue of buffer
			//! to the sink
			iTempBufferHolder = iSourceBufferQ.First();
			iSourceBufferQ.Remove(*iTempBufferHolder);
			iSinkBufferQ.AddLast(*iTempBufferHolder);
			}
		}
	
	if( iFirstTime )
		{
		if( !iNewPlayer )
			{
			// During rebuffering only
			if( !iSourceBufferQ.IsEmpty() ) 
				{
				//if source buffer is empty
				//first buffer of source buffer is taken and refilled
				iTempBufferHolder = iSourceBufferQ.First();							
				iTempbuffer = iTempBufferHolder->Des();
				iInputBuffer.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);
				if( iStopState )
					{
                    IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (1)." );
					return;
					}
				if( !iStopPlayerBuffering )
					{								
					//Calls the fill the buffer for next subsequent times 
					//until the source buffer queue is empty
					iChannel->FilltheBuffer(iInputBuffer); 
					}
				else
					{
                    IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (2)." );
					//if stopPlayerBuffering is set it has return without buffering
					return;	
					}
				}
			else
				{
				//once it is rebuffered it has trigger play
				//it has to indicate that rebuffering has completed for client
				
				TInt err( KErrNone ); 
				if( !iReBuffering )
					{
					GetMP3AudioProperties();
					//Sets the audio properties of the player like sampling rate and channel
					TRAP(err, iAudioPlayer->SetAudioPropertiesL(iSettings.iSampleRate,
																iSettings.iChannels));
					if ( err )
						{
					IRRDEBUG2("CIRMP3Player::BufferFilled - EError", KNullDesC);
						iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );	
                        IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (3)." );
						return;
						}
					}
				iReBuffering = EFalse;
				//buffer reached 100% headers are decoded ready to play			
				iBufferPercentage = K100Percentage;
				iChannel->SentRequest( EBufferFillStop, iBufferPercentage ); 
				
				if( !iSinkBufferQ.IsEmpty() )
					{
					iTempBufferHolder = iSinkBufferQ.First();	
					}					
				iTempbuffer = iTempBufferHolder->Des();
				
				//Take first source buffer queue to be filled
				iInput.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);
				iFirstTime = EFalse;
				iState = EPlaying;
				if( !iStopState )
					{

#ifdef __WINS__
					iTestingAudioPlayer->Write();
#else
					//writing to mmp based buffer to trigger play
					TRAP(err,iAudioPlayer->WriteL(iInput));	

					if( err )
						{
						//error condition
			IRRDEBUG2("CIRMP3Player::BufferFilled - EError1", KNullDesC);
						iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );	
                        IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (4)." );
						return;
						}
#endif //__WINS__	
					}
				//! Calls the play for the first time after rebuffering
			
				}
			}
		else
			{
			//First time when the player is created
			TInt err( KErrNone );
			GetMP3AudioProperties();			
				
			//! Sets the audio properties of the player like sampling rate and channel
			//Sets the audio properties of the player like sampling rate and channel
			TRAP(err, iAudioPlayer->SetAudioPropertiesL(iSettings.iSampleRate,
														iSettings.iChannels));
			if ( err )
				{
		IRRDEBUG2("CIRMP3Player::BufferFilled - EError2", KNullDesC);
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );	
	            IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (5)." );
				return;
				}
			//buffer percentage is 100%							
			iBufferPercentage = K100Percentage;
			//indicating it has rebuffered
			iChannel->SentRequest( EBufferFadeInReady, iBufferPercentage ); 			
			
			if( !iSinkBufferQ.IsEmpty() )
				{
				iTempBufferHolder = iSinkBufferQ.First();
				}
			iTempbuffer = iTempBufferHolder->Des();
			
			//first buffer of sink is taken and played
			iInput.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);
			iNewPlayer = EFalse;
			iFirstTime = EFalse;
			iState = EPlaying;
			
			//Calls the play for the first time
			if( !iStopState )
				{
#ifdef __WINS__
				iTestingAudioPlayer->Write();
#else
				//writing to MMF buffers
				TRAP(err,iAudioPlayer->WriteL(iInput));	
				if( err )
					{
			IRRDEBUG2("CIRMP3Player::BufferFilled - EError3", KNullDesC);
					iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );	
					IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (6)." );	
					return;
					}	
#endif //__WINS__
				}
			}	
		}		
	IRLOG_DEBUG( "CIRMP3Player::BufferFilled - Exiting (7)." );
	}

// ---------------------------------------------------------------------------
// Function: CIRMP3Player
// This function is the default constructor
// ---------------------------------------------------------------------------
//
CIRMP3Player::CIRMP3Player() : iInputBuffer(NULL,0,0), iInput(NULL,0,0)
	{
	IRLOG_DEBUG( "CIRMP3Player::CIRMP3Player" );
	}

// ---------------------------------------------------------------------------
// Function: ConstructL
// Two phase constructor is used to intialize data members
// Function can leave if CMdaAudioOutputStream::NewL leaves
// ---------------------------------------------------------------------------
//
void CIRMP3Player::ConstructL()
	{
	IRLOG_DEBUG( "CIRMP3Player::ConstructL" );

	iAudioPlayer = CMdaAudioOutputStream::NewL(*this,KAudioPriorityRealOnePlayer,
		(TMdaPriorityPreference)KAudioPrefRealOneStreaming );

										//creates an instance of the player
										
#ifdef __WINS__			
	iTestingAudioPlayer = CIRTestingAudioPlayer::NewL(*this);
	CActiveScheduler::Add(iTestingAudioPlayer);
#endif //__WINS__

	TInt f_off = _FOFF(CIRBufferContainer,iLink); //for the buffer queue which is maintained

    iSinkBufferQ.SetOffset(f_off);	 //It is Queue of buffer used by media Sink
    iSourceBufferQ.SetOffset(f_off); // Source of buffer which is ready to fill
    
	iNewPlayer = ETrue;	//indicates that this a newly created player
	iStopPlayerBuffering = EFalse;	//indicates whether to stop buffering
	iState = ENotReady;	//current state not ready	
	iNeedReBuffering = EFalse;//if rebuffering is required this is to set true
	iStopState = EFalse; //
	IRLOG_DEBUG( "CIRMP3Player::ConstructL - Exiting." );	
	}

// ---------------------------------------------------------------------------
// Function: CreateBufferL 
// Created buffers in sink buffer queue and allocates memory to sink buffer queues
// ---------------------------------------------------------------------------
//
void CIRMP3Player::CreateBufferL()
	{
	IRLOG_DEBUG( "CIRMP3Player::CreatebufferL" );	
	TUint8* bufferaddress = iInputBufferPtr;
	//! source buffers not created since initially all buffers are filled with data and is ready to play	
	//! Created buffers in sink buffer queue and allocates memory to sink buffer queues
	for(TInt buffercount = 0; buffercount < KIRInputBufferCount; buffercount++)
		{		
		iTempBufferHolder = CIRBufferContainer::NewL(bufferaddress,
			iPlayBufferSize);
		iSinkBufferQ.AddLast(*iTempBufferHolder);
		bufferaddress += iBufferOffset;
		}
	IRLOG_DEBUG( "CIRMP3Player::CreatebufferL - Exiting." );	
	}
	
// ---------------------------------------------------------------------------
// Function: ReCreateBufferL 
// Recreates buffers of source queue
// ---------------------------------------------------------------------------
//
void CIRMP3Player::ReCreateBufferL()
	{
	IRLOG_DEBUG( "CIRMP3Player::ReCreateBufferL " );
	while(!iSinkBufferQ.IsEmpty())
		{
		//Deleting all the entries in sink buffers queue
		iTempBufferHolder = iSinkBufferQ.First();
		iSinkBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		iTempBufferHolder = NULL;	
		}
	while(!iSourceBufferQ.IsEmpty())
		{
		//deleting all the entries in source buffers queue
		iTempBufferHolder = iSourceBufferQ.First();
		iSourceBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		iTempBufferHolder = NULL;	
		}
	TUint8* bufferaddress = iInputBufferPtr; 
	//reallocates the buffer to source buffer queue
	for(TInt buffercount = 0; buffercount < KIRInputBufferCount; buffercount++)
		{		
		iTempBufferHolder = CIRBufferContainer::NewL(bufferaddress,
			iPlayBufferSize);
		iSourceBufferQ.AddLast(*iTempBufferHolder);
		bufferaddress += iBufferOffset;
		}	
	IRLOG_DEBUG( "CIRMP3Player::ReCreateBufferL - Exiting." );
	}

	
// ---------------------------------------------------------------------------
// Function: GetMP3AudioProperties
// extract all the information required to start the play from the stream
// ---------------------------------------------------------------------------
//
void CIRMP3Player::GetMP3AudioProperties()
	{
	IRLOG_DEBUG( "CIRMP3Player::GetMP3AudioProperties" );
	//Decoding MP3 header		
	DoFindnDecodeMP3Header();	
	
	//! Sets the sampling rate and channel information to data members
	switch(iAudioInfo.iSamplingRate)
		{
		case EMp3SamplingFreq8000: //sampling frequency 8000
			{	
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
			}
			break;
		case EMp3SamplingFreq11025: //sampling frequency 11025
			{			
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate11025Hz;
			}
			break;
		case EMp3SamplingFreq12000:	//sampling frequency 12000
			{	
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate12000Hz;
			}
			break;
		case EMp3SamplingFreq16000:	//sampling frequency 16000
			{			
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
			}
			break;
		case EMp3SamplingFreq22050:	//sampling frequency 22050
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate22050Hz;
			}
			break;
		case EMp3SamplingFreq24000:	//sampling frequency 24000
			{			
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate24000Hz;
			}
			break;
		case EMp3SamplingFreq32000:	//sampling frequency 32000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate32000Hz;
			}
			break;
		case EMp3SamplingFreq44100:	//sampling frequency 44100
			{			
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate44100Hz;
			}
			break;
		case EMp3SamplingFreq48000:	//sampling frequency 48000
			{	
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate48000Hz;
			}
			break;
		default:	//default sampling frequency 22050
			{	
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate22050Hz;
			}
			break;	
		}
	//sets the channel information	
	if( KMp3ChannelMono == iAudioInfo.iChannel ) //0 indicates mono
		{
		iSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;	
		}		
	else	//else 1 it indicates it is stereo
		{
		iSettings.iChannels = TMdaAudioDataSettings::EChannelsStereo;	
		}				
	IRLOG_DEBUG( "CIRMP3Player::GetMP3AudioProperties - Exiting." );
	}
	
// ---------------------------------------------------------------------------
// Function: DoFindnDecodeMP3Header
// Function is used to find the mp3 header and decode the mp3 header
// This function do have hard coded values and these value remains as such
// as long as the standard remains, so used as such
// ---------------------------------------------------------------------------
//
void CIRMP3Player::DoFindnDecodeMP3Header()
	{
	IRLOG_DEBUG( "CIRMP3Player::DoFindnDecodeMP3Header" );
	//first buffer is discarded
	TUint8* input = iInputBufferPtr;
	//end ptr is calculated
	TUint8* endptr = iInputBufferPtr + iInputBufferSize;
	
	//holds the 4 bytes of data
	TUint value = 0; //value zero is intialized
	TBool headerfound = EFalse;
	
	//value are to hardcoded as this is the standard way of implement
	// the MP3 header decoding
	while( input < endptr && !headerfound )
		{
		//stepsuccess indicates the success in each step or stage of header decoding,
		//incase of failure we will not go for next step
		TBool stepsuccess = EFalse;
		//initially no header is found
		//assign the value to zero initially for clearing the value
		//then assign next 32 bits of data to value	
		value = 0; //value is cleared
		value |= input[0]<<KTwentyFour; //first 8 bit is shift 24bits copied (or) to "value"
		value |= input[1]<<KSixteen; //second 8 bit is shift 24bits copied (or) to "value"
		value |= input[2]<<KEight;//third 8 bit is shift 24bits copied (or) to "value"
		value |= input[KThree];//last 8 bit is shift 24bits copied (or) to "value"
		
		// sync bytes found
		// for performance reasons check already that it is not data within an empty frame (all bits set)
		// therefore check wether the bits for bitrate are all set -> means that this is no header
		if ( (input[0] == 0xFF) && ( ( input[1] & 0xE0 ) == 0xE0 ) && ( ( input[2] & 0xF0 ) != 0xF0 ) )
			{
			//header is found
			//first level of header decoding is success, so stepsuccess = ETrue
			stepsuccess = ETrue;
			//version information is stored in 2 bits starting from 19th bit of header
			//two get only 2bits we are doing an AND operation with 0b011 (0x03)				
			TInt index = (value >> 19) & 0x03;
			//! Gets the version information of MPEG
			
			TInt version = index; 	 //MPEG 2.5 	00
									 //reservered   01
									 //MPEG 2       10
									 //MPEG 1       11
			//if header information is 0x01						 
			iAudioInfo.iVersion = version;
			if( 0x01 == iAudioInfo.iVersion ) 
				{
				//bit pattern 0x01 indicates that version is reserved 							  
				// and is considered as incorrect header information
				stepsuccess = EFalse;	
				}
				
			else
				{
				//bit pattern 0x00 indictes MPEG version is 2.5
				//bit pattern 0x10 indictes MPEG version is 2
				//MPEG 2.5 (0x00) and MPEG 2 (0x10) version
				if( (0x00 == iAudioInfo.iVersion) || 
					(0x02 == iAudioInfo.iVersion) )
					{
					//576 samples per frame
					//there is 576 samples per frame
					iAudioInfo.iSamplesPerFrame = KSamplesPerFrame1;
					}
				else 
					{
					//MPEG 1 version (0x11) 
					//for MPEG 1 there is 1152 samples per frame
					iAudioInfo.iSamplesPerFrame = KSamplesPerFrame2;
					}
				}				
				
			if( stepsuccess )
				{
				//layer information from the header is obtained from 2 bits starting 17th Bit
				//information is extracted to index. 				
				index = ( value >> 17 ) & 0x03;     
				if( 0x01 != index ) 	//Layer 3 check if its layer 2 to layer 1 								
					{				    //we will consider the header is not proper
					stepsuccess = EFalse;
					}
				else
					{
					iAudioInfo.iLayer = 2; //Layer 3
					//for selection bitrate we have need a combination of MPEG version, layer information
					//and selection bits. the selection bits starts from 12th bits of mp3 header
					//the selection byte is extracted into the index		
					index = (value >> 12) & 0x000F;
					TInt versionindex = 0;
					if( 0x11 == version )
						{
						//index of the array is 0
						versionindex = 0;	
						}
					else
						{
						//index of array is 1
						versionindex = 1;	
						}
					//getting the bit rate
					iAudioInfo.iBitRate = 
						KBitRate[versionindex][iAudioInfo.iLayer][index];  
					if( iAudioInfo.iBitRate == 0 ) //since bit rate is zero we consider as header is not proper
						{
						stepsuccess = EFalse;
						}						
					}			
				}
				
			if( stepsuccess )
				{
				//sampling rate is obtained by a combination of MPEG version and
				//selection bits. There are 2 selection bits starting from 10th bit
				//and is extracted (AND with 0x03) from header and stored in index 
				index = ( value >> 10 ) & 0x03;
				
				//! Gets the sampling frequency				
				iAudioInfo.iSamplingRate = 
					KMP3SamplingRate[iAudioInfo.iVersion][index];
				if( 0 == iAudioInfo.iSamplingRate ) //if sampling rate is 0 it is invalidated
					{
					stepsuccess = EFalse;	
					}
				//padding information is obtained from 9th bit of mp3 header
				iAudioInfo.iPadding = (value >> 9) & 0x01; //padding
				
				//the information for selection of channel is stored in 2bits starting from 6th bit of
				//mp3 header and is decoded and kept in index for selection of channel
				index = ( value >> 6 ) & 0x0003;				
				iAudioInfo.iChannel = KChannelInfo[index]; //0 for mono and 1 for stereo			
				
				//framesize = (144 * bitrate)/(padding + sampling rate)
				if( stepsuccess )
					{//multiplication of 1000 is to convert kbs to bs
					iAudioInfo.iFrameSize = (KOneFourFour * iAudioInfo.iBitRate * KThousand)
						/(iAudioInfo.iPadding + iAudioInfo.iSamplingRate);	
					}
				//cross checking the header with next header
				//both sampling frequency matches the function will return ETrue else EFalse	
				if( !CrossCheckHeader(input) ) 
					{
					stepsuccess = EFalse;
					}
				}
			if( stepsuccess )
				{
				//header is found
				headerfound = ETrue;	
				}
			else
				{
				//header is not found
				input++;	
				}
			} // sync bytes found
			else
			{
			//header sync bits doesn't match
			input++;	
			}
		}
	IRLOG_DEBUG( "CIRMP3Player::DoFindnDecodeMP3Header - Exiting." );
	}
	
// ---------------------------------------------------------------------------
// Function: CrossCheckHeader
// Function is cross checks sampling rate  
// This function do have hard coded values and these value remains as such
// as long as the standard remains, so used as such
// ---------------------------------------------------------------------------
//
TBool CIRMP3Player::CrossCheckHeader(TUint8* aInput)
	{
	IRLOG_DEBUG( "CIRMP3Player::CrossCheckHeader" );
	if( !aInput )
		{
		return EFalse;
		}
	//next header is to taken
	TUint8* input = aInput + iAudioInfo.iSamplesPerFrame;
	TUint8* endptr = iInputBufferPtr + iInputBufferSize 
		- KMp3FrameHeaderSize;
	TUint32 samplingrate = 0;
	TBool headerfound = EFalse;
	
	TUint value = 0;
	TInt version = 0;
	TUint channel = 0;
	TUint layer = 0;
	while( input < endptr && !headerfound )
		{
		//header is not found
		TInt successheader = EFalse;
		//assign the value to zero initially for clearing the value
		//then assign next 32 bits of data to value			
		value = 0;
		value |= input[KZero]<<KTwentyFour;
		value |= input[KOne]<<KSixteen;
		value |= input[KTwo]<<KEight;
		value |= input[KThree];
			
		// sync bytes found
		// for performance reasons check already that it is not data within an empty frame (all bits set)
		// therefore check wether the bits for bitrate are all set -> means that this is no header
		if ( (input[0] == 0xFF) && ( ( input[1] & 0xE0 ) == 0xE0 ) && ( ( input[2] & 0xF0 ) != 0xF0 ) )
			{
			//header is obtained
			successheader = ETrue;
			//version information is stored in 2 bits starting from 19th bit of header
			//two get only 2bits we are doing an AND operation with 0b011 (0x03)
			TInt index = (value >> 19) & 0x03;
			//! Gets the version information of MPEG
			
			version = index; 	 //MPEG 2.5 	00
										 //reservered   01
										 //MPEG 2       10
										 //MPEG 1       11
			//if header information is 0x01							 
			if( 0x01 == version ) //indicates that version is reserved and is considered 
				{				  //as incorrect header information
				//header is not valid							  
				successheader = EFalse;
				input++;
				headerfound = EFalse;	
				}
				
			index = ( value >> 6 ) & 0x0003;
			channel = KChannelInfo[index]; //0 for mono and 1 for stereo	
			
			index = ( value >> 17 ) & 0x03;
			if ( index == 0x01 )
				layer = 2;
				
			if( successheader )	
				{
				//sampling rate is obtained by a combination of MPEG version and
				//selection bits. There are 2 selection bits starting from 10th bit
				//and is extracted (AND with 0x03) from header and stored in index 
				index = (value >> 10) & 0x03;				
				//! Gets the sampling frequency				
				samplingrate = KMP3SamplingRate[version][index];
				if( 0 == samplingrate ) //if sampling rate is 0 it is invalidated
					{
					//header is not valid
					input++;	
					}
				else
					{
					//header is obtained
					headerfound = ETrue;	
					}	
				}					
			} // sync bytes found
			else
			{
			//header is not valid
			input++;	
			}
		}
	//if sampling rate match with one we initially decoded we return ETrue else EFalse
	// version change never possible
	// layer change never possible
	// sampling rate change never possible
	// from mono to stereo never possible
	if ( iAudioInfo.iSamplingRate == samplingrate && iAudioInfo.iVersion == version
			&& iAudioInfo.iChannel == channel
		 	&& iAudioInfo.iLayer == layer )
		{
		IRLOG_DEBUG( "CIRMP3Player::CrossCheckHeader - Exiting (1)." );
		return ETrue;	
		}		
	else
		{
		IRLOG_DEBUG( "CIRMP3Player::CrossCheckHeader - Exiting (2).");
		return EFalse;	
		}		
	}

	
									//Call back functions

// ---------------------------------------------------------------------------
// Function: MaoscBufferCopied
// call back to be implemented for using CMdaAudioOutputStream
// Call as callback from the CMdaAudioOutputStream::WriteL
// after frame work has copied stream to a buffer
// ---------------------------------------------------------------------------
//
void CIRMP3Player::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	IRLOG_DEBUG( "CIRMP3Player::MaoscBufferCopied" );	
	if( aError )
		{
		IRLOG_ERROR2( "CIRMP3Player::MaoscBufferCopied - Error in buffering (%d)", aError );
		//error in playing then return
		IRRDEBUG2("CIRMP3Player::MaoscBufferCopied - EError", KNullDesC);
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
		return;
		}
	else
		{
       if ( iCurrentVolume < iConfig.iVolume )
            {
            iCurrentVolume = iConfig.iVolume;
            IRLOG_INFO2( "CIRMP3Player::MaoscBufferCopied - Setting volume to %d", iCurrentVolume );
            iAudioPlayer->SetVolume( iCurrentVolume );
            }
		if( iSinkBufferQ.IsEmpty() )
			{
			//previously played buffer is empty
			//should never happen
		IRRDEBUG2("CIRMP3Player::MaoscBufferCopied - EError1", KNullDesC);
			iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
			return;	
			}
		else
			{
			//previously played buffer is removed	
			iTempBufferHolder = iSinkBufferQ.First();
			//played buffer is removed from sink queue		
			iSinkBufferQ.Remove(*iTempBufferHolder);
			//removed buffer is appended to end of source queue
			iSourceBufferQ.AddLast(*iTempBufferHolder);
			
			//the first buffer of the source buffer is given to refill					
			if( !iSourceBufferQ.IsEmpty() )
				{
				iTempBufferHolder = iSourceBufferQ.First();	
				}			
			//if stop is called during rebuffering
			if( !iStopPlayerBuffering )
				{
				// if player is stopped it has to stop buffering
				iTempbuffer = iTempBufferHolder->Des();
				iInputBuffer.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize); 
				iChannel->FilltheBuffer(iInputBuffer);	
				}
			//if stop state we have to return
			if( iStopState )
				{
				IRLOG_DEBUG( "CIRMP3Player::MaoscBufferCopied - Exiting (1)." );	
				//should continue this loop unless play is triggered
				return;	
				}
						
			//plays buffer if there buffer to play
			if( !iSinkBufferQ.IsEmpty() )
				{
				//sink Buffer queue has buffer to play
				iTempBufferHolder = iSinkBufferQ.First();
				iTempbuffer = iTempBufferHolder->Des();
							
				iInput.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);				

#ifdef __WINS__
				iTestingAudioPlayer->Write();
#else				
				//writing to MMF buffer
				TRAPD(err,iAudioPlayer->WriteL(iInput));

				if( err )
					{
			IRRDEBUG2("CIRMP3Player::MaoscBufferCopied - EError2", KNullDesC);
					//if playing failed
					iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
					return;	
					}
#endif //__WINS__
				}
			else
				{
				//rebuffering
				if( !iStopPlayerBuffering )
					{
					//if sink buffer is empty
					//we have to rebuffer
					iReBuffering = ETrue;
					Play();
					}								
				}
			}
		}
	IRLOG_DEBUG( "CIRMP3Player::MaoscBufferCopied - Exiting (2)." );	
	}

// ---------------------------------------------------------------------------
// Function: MaoscPlayComplete
// call back to implement for using CMdaAudioOutputStream
// Call as callback from the CMdaAudioOutputStream::WriteL
// after play is completed
// ---------------------------------------------------------------------------
//
void CIRMP3Player::MaoscPlayComplete(TInt aError)
	{
	IRLOG_DEBUG( "CIRMP3Player::MaoscPlayComplete" );
	if( !iSkipPlayCompleted )
		{
	IRRDEBUG2("CIRMP3Player::MaoscPlayComplete EStoppedPlaying", KNullDesC); 

		//sending the stop status
		iChannel->SentRequest( EStoppedPlaying, aError );	
		}
	IRLOG_DEBUG( "CIRMP3Player::MaoscPlayComplete - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: MaoscOpenComplete
// call back implemented for using CMdaAudioOutputStream
// Call as callback from the CMdaAudioOutputStream::Open
// ---------------------------------------------------------------------------
//
void CIRMP3Player::MaoscOpenComplete( TInt aError )
	{	
	IRLOG_DEBUG( "CIRMP3Player::MaoscOpenComplete" );
	if( aError )
		{		
		IRLOG_ERROR( "CIRMP3Player::MaoscOpenComplete - error end" );
		IRRDEBUG2("CIRMP3Player::MaoscOpenComplete - EError", KNullDesC);
		//open failed
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
		return;	
		}
	else
		{
		TInt err( KErrNone );
		
#ifdef __WINS__
	      err=0;
#else
		//Setting the data type of player as MP3
		TRAP(err, iAudioPlayer->SetDataTypeL(iDataType.FourCC()));  //set type the data as MP3 type
	      
		if ( err )
			{
			if ( KErrNotSupported == err )
				{
			IRRDEBUG2("CIRMP3Player::MaoscOpenComplete - EError1", KNullDesC);
				iChannel->SentRequest( EError, KErrNotSupported );
				}
			else
				{
			IRRDEBUG2("CIRMP3Player::MaoscOpenComplete - EError2", KNullDesC);
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
				}
			return;
			}
#endif //__WINS__
		
		
		//during rebuffering current volume is to be taken	
		if( iNeedReBuffering )
			{
			iConfig.iVolume = iChannel->FetchVolume();	
			}			
		
		//Computes the current volume and sets the volume
		TInt index = iAudioPlayer->MaxVolume()/KNoVolumeLevels;	
		iConfig.iVolume = iConfig.iVolume * index;
		//if the given volume is greater than max volume it made to maximum volume
		//this may happen if the first time volume exceeds the limits
		if( iConfig.iVolume > iAudioPlayer->MaxVolume() )
			{
			iConfig.iVolume = iAudioPlayer->MaxVolume();	
			}			
		//else if the given volume is less than zero it made to zero 
		//this may happen if the first time volume goes below the limits
		if( iConfig.iVolume < KZeroVolume )
			{
			iConfig.iVolume = KZeroVolume;	
			}			
		
		// The actual setting of the volume is delayed to MaoscBufferCopied method.
		// This was due to some error in N91 sound subsystem, which caused the
		// volume not to adjust in some cases. 
	    iCurrentVolume = 0;
        iAudioPlayer->SetVolume( iCurrentVolume );
		
		iFirstTime = ETrue;
		iNeedReBuffering = ETrue;
		
		if( !iNewPlayer )
			{
			//player is rebuffering or stop and played
			TRAP(err,ReCreateBufferL());
			if ( err )
				{
				IRRDEBUG2("CIRMP3Player::MaoscOpenComplete - EError3", KNullDesC);
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );	
				return;
				}
			
			//initiates the refilling of buffers
			iTempBufferHolder = iSourceBufferQ.First();
			iTempbuffer = iTempBufferHolder->Des();
			iInputBuffer.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);			
			iChannel->SentRequest( EBufferFillStart, iBufferPercentage );
			//Call FilltheBuffer for first time 
			iChannel->FilltheBuffer(iInputBuffer);	//start fill the data	
			}
		else
			{
			//First time playing
			BufferFilled();
			}
		}
	IRLOG_DEBUG( "CIRMP3Player::MaoscOpenComplete - Exiting." );
	}
	
// ---------------------------------------------------------------------------
// CIRMP3Player::GetMediaClientInstance()
// Returns the Audio Player Instance
// ---------------------------------------------------------------------------
//
CMdaAudioOutputStream* CIRMP3Player::GetAudioPlayer()
	{
	IRLOG_DEBUG( "CIRMP3Player::GetMediaClientInstance " );
	return iAudioPlayer;	
	}
