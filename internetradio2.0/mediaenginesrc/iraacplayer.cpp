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

#include "iraacplayer.h"
#include "iraacplayerlocal.h"
#include "irbuffercontainer.h"
#include "irctrlcmdobserver.h"
#include "irdebug.h"
#include "irmediaenginebuffer.h"

// Constants

const TInt KZERO = 0 ;
const TInt KONE = 1 ;
const TInt KTWO = 2 ;
const TInt KTHREE = 3 ;
const TInt KFOUR = 4 ;
const TInt KFIVE = 5 ;
const TInt KSIX = 6 ;
const TInt KELEVEN = 11 ;
const TInt KTWENTYONE = 21 ;
const TInt KTWENTYFOUR = 24 ;
const TInt KSIXTEEN = 16 ;
const TInt KEIGHT = 8 ;
const TInt KONEFIVETHREESIX = 1536 ;





// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Function : NewL
// function returns an instance of CIRAACPlayer
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRAACPlayer* CIRAACPlayer::NewL()
	{
	IRLOG_DEBUG( "CIRAACPlayer::NewL" );
	CIRAACPlayer* self = CIRAACPlayer::NewLC();
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRAACPlayer::NewL - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : NewLC
// function creates an instance of CIRAACPlayer
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRAACPlayer* CIRAACPlayer::NewLC()
	{
	IRLOG_DEBUG( "CIRAACPlayer::NewLC" );
	CIRAACPlayer* self = new (ELeave) CIRAACPlayer;
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRAACPlayer::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : ~CIRAACPlayer
// Default Destructor
// ---------------------------------------------------------------------------
//
CIRAACPlayer::~CIRAACPlayer()
	{
	IRLOG_DEBUG( "CIRAACPlayer::~CIRAACPlayer" );
	delete iAudioPlayer;
	while(!iSinkBufferQ.IsEmpty())//deleting all the entries in sink buffers queue
		{
		iTempBufferHolder = iSinkBufferQ.First();
		iSinkBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		}
	while(!iSourceBufferQ.IsEmpty())//deleting all the entries in source buffers queue
		{
		iTempBufferHolder = iSourceBufferQ.First();
		iSourceBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		}
	IRLOG_DEBUG( "CIRAACPlayer::~CIRAACPlayer - Exiting." );
	}


									//Function for Play control

// ---------------------------------------------------------------------------
// Function : Play
// function which intiates the player to start playing
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::Play()
	{
	IRLOG_DEBUG( "CIRAACPlayer::Play" );
	if ( EPlaying == iState ) //If the current state is playing
		{
		//internally stopped before playing so no need of sending
		//stop status
		iSkipPlayCompleted = ETrue;
		iAudioPlayer->Stop(); //then we have to stop it then restart the play from begining
		iSkipPlayCompleted = EFalse;
		}
	iState = ENotReady;
	iBufferPercentage = KZeroPercentage;
	iHeaderFound = EFalse;
	iStopState = EFalse;

	//note : using TRAP_IGNORE to suppress a codescanner warning
	//"Ignoring the return value from Open() functions"
	//this cannot be checked as this symbian API returns void

	//opening the current player component
	 TRAP_IGNORE(iAudioPlayer->Open(&iSettings); )
	iChannel->SentRequest(EPlayingState,KErrNone);			
	IRLOG_DEBUG( "CIRAACPlayer::Play - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : Stop
// function which stops the player
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::Stop()
	{
	IRLOG_DEBUG( "CIRAACPlayer::Stop" );
	//If the current state is playing
	if ( EPlaying == iState )
		{
		iAudioPlayer->Stop();
		}
	else
		{
		//sending the updated status as stopped
		iChannel->SentRequest( EStoppedPlaying, KErrNone );
		}
	iState = EReadyToPlay;
	iStopState = ETrue;
	IRLOG_DEBUG( "CIRAACPlayer::Stop - Exiting." );
	}

										//Functions for Volume Control

// ---------------------------------------------------------------------------
// Function : SetVolume
// function to set the volume
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::SetVolume(TInt aVolume )
	{
	IRLOG_DEBUG( "CIRAACPlayer::SetVolume" );
	//If volume should be less than maximum value and greater than or equal to zero then set the volume
	if( KZeroVolume <= aVolume && iAudioPlayer->MaxVolume() >= aVolume )
		{
		iAudioPlayer->SetVolume((TInt)(aVolume));
		iConfig.iVolume = iCurrentVolume = aVolume;
		}
	IRLOG_DEBUG( "CIRAACPlayer::SetVolume - Exiting." );
	}
// ---------------------------------------------------------------------------
// Function : MaxVolume
// function to returns the maximum volume
// ---------------------------------------------------------------------------
//
TInt CIRAACPlayer::MaxVolume() const
	{
	IRLOG_DEBUG( "CIRAACPlayer::MaxVolume" );
	return iAudioPlayer->MaxVolume();
	}
// ---------------------------------------------------------------------------
// Function : Volume
// function to returns the volume, integer level of voume is the Output
// ---------------------------------------------------------------------------
//
TInt CIRAACPlayer::Volume() const
	{
	IRLOG_DEBUG( "CIRAACPlayer::Volume" );
	return iAudioPlayer->Volume();
	}

									//Intialization of Codec Settings

// ---------------------------------------------------------------------------
// Function: Intialize
// Set the codec type and sampling rate channel of stream
// This is set to initial settings which is required to start the player
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::Intialize(TConfig& aConfig,TUint8* aInitParams,
	CIRCtrlCmdObserver* aChannel)
	{
	IRLOG_DEBUG( "CIRAACPlayer::Intialize" );
	iInputBufferPtr = reinterpret_cast<TUint8*> (aInitParams); //instance of buffer
	//Configuration information
	iConfig = aConfig; //! Set all the configuration information like volume
	iPlayBufferSize = iConfig.iPlayBufferSize;
	iInputBufferSize = iConfig.iPlayBufferCount*iPlayBufferSize;
    iBufferOffset = iPlayBufferSize;
	iDataType.Set(KMMFFourCCCodeAAC); //! Set the data type as AAC
	iChannel = reinterpret_cast<CIRCtrlCmdObserver*> (aChannel);
	iChannel->SentRequest(EPlayerChanged,KErrNone);									//creates an instance of the player
	TRAPD(err,CreateBufferL()); //creates the buffer
	if( err )
		{
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
		return;
		}
	IRLOG_DEBUG( "CIRAACPlayer::Intialize - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: StopPlayerBuffering
// Function is used to freeze bufferring
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::StopPlayerBuffering()
	{
	IRLOG_DEBUG( "CIRAACPlayer::StopPlayerBuffering" );
	iStopPlayerBuffering = ETrue;
	}

// ---------------------------------------------------------------------------
// Function: BufferFilled
// Function which is called when network gets the buffer filled with data
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::BufferFilled()
	{
	IRLOG_DEBUG( "CIRAACPlayer::BufferFilled" );
	if( !iNewPlayer )
		{
		//if not playing for the first time
		if( !iSourceBufferQ.IsEmpty() )
			{
			//Initially all unfilled buffers are in source buffer Queue
			//Once the buffer in the source buffer queue is filled it is moved to queue of buffer
			//to the sink
			iTempBufferHolder = iSourceBufferQ.First();
			iSourceBufferQ.Remove(*iTempBufferHolder);
			iSinkBufferQ.AddLast(*iTempBufferHolder);
			}
		}

	if( iFirstTime )
		{
		//if it is playing for first time or rebuffering
		if( !iNewPlayer )
			{
			//if not playing for the first time
			if( !iSourceBufferQ.IsEmpty() )
				{
				//fill the entire buffer source queue
				//if the entire source buffer queue is not filled
				if( !iStopPlayerBuffering )
					{
					iTempBufferHolder = iSourceBufferQ.First();
					iTempbuffer = iTempBufferHolder->Des();
					iInputBuffer.Set
						(iTempbuffer,iPlayBufferSize,iPlayBufferSize);

					if( !iStopState )
						{
						//! Calls the fill the buffer for next subsequent times until the source buffer queue is empty
						iChannel->FilltheBuffer(iInputBuffer);
						IRLOG_DEBUG( "CIRAACPlayer::BufferFilled - Exiting (1)."  );
						return;
						}
					}
				}
			else
				{
				//if the entire source buffer queue is filled
				//gets the header informations like sampling rate and channel
				//this function here is called to find proper header and
				//rearrange buffer accordingly
				GetAACAudioProperties();

				//buffer is completetly filled and indicates that buffering is completed
				iBufferPercentage = K100Percentage;
				iChannel->SentRequest( EBufferFillStop, iBufferPercentage );

				//takes the first buffer from sink buffer queue
				if( !iSinkBufferQ.IsEmpty() )
					{
					iTempBufferHolder = iSinkBufferQ.First();
					}
				iTempbuffer = iTempBufferHolder->Des();

				iInput.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);
				iFirstTime = EFalse;
				iState = EPlaying;
				if( !iStopState )
					{
					//! Calls the play for the first time
					TRAPD(err,iAudioPlayer->WriteL(iInput));
					if( err )
						{
						iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
                        IRLOG_DEBUG( "CIRAACPlayer::BufferFilled - Exiting (2)." );
						return;
						}
					}
				}
			}
		else
			{
			//if a new player
			TInt err( KErrNone );
			//get audio properties like sampling rate and channel
			if( !iHeaderFound )
				{
				GetAACAudioProperties();
				}

			//sets the audio properties like sampling rate and channel
			TRAP(err, iAudioPlayer->SetAudioPropertiesL( iSettings.iSampleRate,
														 iSettings.iChannels));
			if( err )
				{
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
                IRLOG_DEBUG( "CIRAACPlayer::BufferFilled - Exiting (3)." );
				return;
				}

			//indicates that it is ready to play to client
			iBufferPercentage = K100Percentage;
			iChannel->SentRequest( EBufferFadeInReady, iBufferPercentage );

			if( !iSinkBufferQ.IsEmpty() )
				{
				iTempBufferHolder = iSinkBufferQ.First();
				}
			iTempbuffer = iTempBufferHolder->Des();

			//takes the first buffer from sink buffer queue
			iInput.Set(iTempbuffer,iPlayBufferSize,iPlayBufferSize);
			iFirstTime = EFalse;
			iState = EPlaying;
			iNewPlayer = EFalse;
			if( !iStopState )
				{
				//! Calls the play for the first time
				TRAP(err,iAudioPlayer->WriteL(iInput));
				if( err )
					{
					iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
                    IRLOG_DEBUG( "CIRAACPlayer::BufferFilled - Exiting (4)." );
					return;
					}
				}
			}
		}
	IRLOG_DEBUG( "CIRAACPlayer::BufferFilled - Exiting (5)." );
	}

// ---------------------------------------------------------------------------
// Function: CIRAACPlayer
// This function is the default constructor
// ---------------------------------------------------------------------------
//
CIRAACPlayer::CIRAACPlayer():iInputBuffer(NULL,0,0),iInput(NULL,0,0)
	{
	IRLOG_DEBUG( "CIRAACPlayer()::CIRAACPlayer" );
	}

// ---------------------------------------------------------------------------
// Function: ConstructL
// Two phase constructor is used to intialize data members
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::ConstructL()
	{
	IRLOG_DEBUG( "CIRAACPlayer::ConstructL" );


	iAudioPlayer = CMdaAudioOutputStream::NewL(*this,KAudioPriorityRealOnePlayer,
		(TMdaPriorityPreference)KAudioPrefRealOneStreaming );



										//creates an instance of the player

	TInt f_off = _FOFF(CIRBufferContainer,iLink); //for the buffer queue which is maintained

    iSinkBufferQ.SetOffset(f_off);	 //It is Queue of buffer used by media Sink
    iSourceBufferQ.SetOffset(f_off); // Source of buffer which is ready to fill

   	iNewPlayer = ETrue;    //indicates that this a newly created player
	iStopPlayerBuffering = EFalse; //indicates whether to freeze buffering
	iNeedReBuffering = EFalse; //if rebuffering is required this is to set true
	iState = ENotReady;//current state not ready
	IRLOG_DEBUG( "CIRAACPlayer::ConstructL - Exiting." );

	}

// ---------------------------------------------------------------------------
// Function: CreateBufferL
// Allocates memory and  creates buffers of sink queue
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::CreateBufferL()
	{
	IRLOG_DEBUG( "CIRAACPlayer::CreateBufferL" );
	TUint8* bufferaddress = iInputBufferPtr;

	//Created memory to buffer and allocates buffers to buffer queues
	for(TInt buffercount = 0;buffercount < KIRInputBufferCount; buffercount++)
		{
		iTempBufferHolder =
			CIRBufferContainer::NewL(bufferaddress,iPlayBufferSize);
		iSinkBufferQ.AddLast(*iTempBufferHolder);
		bufferaddress += iBufferOffset;
		}
	IRLOG_DEBUG( "CIRAACPlayer::CreateBufferL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: ReCreateBufferL
// Recreates buffers of source queue
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::ReCreateBufferL()
	{
	IRLOG_DEBUG( "CIRAACPlayer::ReCreateBufferL" );
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
	for(TInt buffercount = 0;buffercount < KIRInputBufferCount; buffercount++)
		{
		//reallocates the buffer to source buffer queue
		iTempBufferHolder =
			CIRBufferContainer::NewL(bufferaddress,iPlayBufferSize);
		iSourceBufferQ.AddLast(*iTempBufferHolder);
		bufferaddress += iBufferOffset;
		}
	IRLOG_DEBUG( "CIRAACPlayer::ReCreateBufferL - Exiting." );
	}

									//Call back functions

// ---------------------------------------------------------------------------
// Function: MaoscBufferCopied
// call back to be implemented for using CMdaAudioOutputStream
// Call as callback from the CMdaAudioOutputStream::WriteL
// after frame work has copied stream to a buffer * @param Error code
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	IRLOG_DEBUG( "CIRAACPlayer::MaoscBufferCopied" );
	if( aError )
		{
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
		IRLOG_DEBUG( "CIRAACPlayer::MaoscBufferCopied - Exiting (1)." );
		return;
		}
	else
		{
        if ( iCurrentVolume < iConfig.iVolume )
            {
            iCurrentVolume = iConfig.iVolume;
            IRLOG_INFO2( "CIRAACPlayer::MaoscBufferCopied - Setting volume to %d", iCurrentVolume );
            iAudioPlayer->SetVolume( iCurrentVolume );
            }
		if( !iSinkBufferQ.IsEmpty() )
			{
			//removing the previously read buffer
			iTempBufferHolder = iSinkBufferQ.First();
			iSinkBufferQ.Remove(*iTempBufferHolder);
			if(iTempBufferHolder->Size() == iPlayBufferSize)
				{
				iSourceBufferQ.AddLast(*iTempBufferHolder);
				}
			else
				{
				delete iTempBufferHolder;
				iTempBufferHolder = NULL;
				}
			}

		if( !iStopPlayerBuffering )
			{
			if( !iSourceBufferQ.IsEmpty() )
				{
				//refilling the first empty buffer from the source queue
				iFirstTime = EFalse;
				iTempBufferHolder = iSourceBufferQ.First();
				iTempbuffer = iTempBufferHolder->Des();
				iInputBuffer.Set
					(iTempbuffer,iPlayBufferSize,iPlayBufferSize);
				iChannel->FilltheBuffer(iInputBuffer);
				}
			}
		if( iStopState )
			{
            IRLOG_DEBUG( "CIRAACPlayer::MaoscBufferCopied - Exiting (2)." );
			return;
			}
		//starts to play next buffer
		if( !iSinkBufferQ.IsEmpty() )
			{
			//takes the next buffer and plays
			iTempBufferHolder = iSinkBufferQ.First();
			iTempbuffer = iTempBufferHolder->Des();
			iInput.Set(iTempbuffer,iTempBufferHolder->Size(),
				iTempBufferHolder->Size());
			//writing to MMF buffer
			TRAPD(err,iAudioPlayer->WriteL(iInput));
			if( err )
				{
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
                IRLOG_DEBUG( "CIRAACPlayer::MaoscBufferCopied - Exiting (3)." );
				return;
				}
			}
		else
			{
			//If no buffer to play so rebuffer
			if( !iStopPlayerBuffering )
				{
				Play();
				}
			}
		}
	IRLOG_DEBUG( "CIRAACPlayer::MaoscBufferCopied - Exiting (4)." );
	}

// ---------------------------------------------------------------------------
// Function: MaoscPlayComplete
// call back to be implemented for using CMdaAudioOutputStream
// after play is completed
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::MaoscPlayComplete(TInt aError)
	{
	IRLOG_DEBUG( "CIRAACPlayer::MaoscPlayComplete" );
	if( !iSkipPlayCompleted )
		{
		//sending the error code
		iChannel->SentRequest( EStoppedPlaying, aError );
		}
	IRLOG_DEBUG( "CIRAACPlayer::MaoscPlayComplete - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: MaoscOpenComplete
// call back to be implemented for using CMdaAudioOutputStream
// Call as callback from the CMdaAudioOutputStream::Open
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::MaoscOpenComplete( TInt aError )
	{
	IRLOG_DEBUG( "CIRAACPlayer::MaoscOpenComplete" );
	if( aError )
		{
		IRLOG_DEBUG( "CIRAACPlayer::MaoscOpenComplete - Exiting (1)." );
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
		return;
		}
	else
		{
		//Setting the data type of player as MP3
		TRAPD(err, iAudioPlayer->SetDataTypeL(iDataType.FourCC()));  //set type the data as MP3 type
		if ( err )
			{
			if ( KErrNotSupported == err )
				{
				iChannel->SentRequest( EError, KErrNotSupported );
				}
			else
				{
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
				}
            IRLOG_DEBUG( "CIRAACPlayer::MaoscOpenComplete - Exiting (2)." );
			return;
			}

		//during rebuffering current volume is to be taken
		if( iNeedReBuffering )
			{
			iConfig.iVolume = iChannel->FetchVolume();
			}
		//Computes the current volume and sets the volume
		TInt index = iAudioPlayer->MaxVolume()/KNoVolumeLevels;
		iConfig.iVolume = iConfig.iVolume * index;
		//if volume level given is greater than Maximum volume
		//volume is set to maximum volume level
		if( iConfig.iVolume > iAudioPlayer->MaxVolume() )
			{
			iConfig.iVolume = iAudioPlayer->MaxVolume();
			}
		//if volume level given is lesser than zero
		//volume is set to zero volume level
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
			//if rebuffering
			TRAP(err,ReCreateBufferL());
			if ( err )
				{
				iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
                IRLOG_DEBUG( "CIRAACPlayer::MaoscOpenComplete - Exiting (3)." );
				return;
				}
			if( !iStopPlayerBuffering )
				{
				//initiates rebuffering for the first time
				iTempBufferHolder = iSourceBufferQ.First();
				iTempbuffer = iTempBufferHolder->Des();
				iInputBuffer.Set(iTempbuffer,iTempBufferHolder->Size(),
					iTempBufferHolder->Size());

				iChannel->SentRequest( EBufferFillStart, iBufferPercentage );
				//! Call FilltheBuffer for first time
				iChannel->FilltheBuffer(iInputBuffer);	//start fill the data
				}
			}
		else
			{
			//First time playing
			BufferFilled();
			}
		}
	IRLOG_DEBUG( "CIRAACPlayer::MaoscOpenComplete - Exiting (4)." );
	}

// ---------------------------------------------------------------------------
// Function: GetAACAudioProperties
// extract all the information to start the playing from the stream
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::GetAACAudioProperties()
	{
	IRLOG_DEBUG( "CIRAACPlayer::GetAACAudioProperties" );
	//decoding the AAC header
	TRAPD(err,DoFindnDecodeAACHeaderL());
	if( err )
		{
		iChannel->SentRequest( EError, KIRCtrlCmdGeneralPlayerError );
        IRLOG_DEBUG( "CIRAACPlayer::GetAACAudioProperties - Exiting (1)." );
		return;
		}
	switch(iAudioInfo.iSamplingRate)
		{
		case EAACSamplingFreq8000: //sampling frequency 8000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
			}
			break;
		case EAACSamplingFreq11025: //sampling frequency 11025
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate11025Hz;
			}
			break;
		case EAACSamplingFreq12000:	//sampling frequency 12000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate12000Hz;
			}
			break;
		case EAACSamplingFreq16000:	//sampling frequency 16000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate16000Hz;
			}
			break;
		case EAACSamplingFreq22050:	//sampling frequency 22050
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate22050Hz;
			}
			break;
		case EAACSamplingFreq24000:	//sampling frequency 24000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate24000Hz;
			}
			break;
		case EAACSamplingFreq32000:	//sampling frequency 32000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate32000Hz;
			}
			break;
		case EAACSamplingFreq44100:	//sampling frequency 44100
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate44100Hz;
			}
			break;
		case EAACSamplingFreq48000:	//sampling frequency 48000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate48000Hz;
			}
			break;
		case EAACSamplingFreq64000:	//sampling frequency 64000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate64000Hz;
			}
			break;
		case EAACSamplingFreq96000:	//sampling frequency 96000
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate96000Hz;
			}
			break;
		default:	//default sampling frequency 22050
			{
			iSettings.iSampleRate = TMdaAudioDataSettings::ESampleRate22050Hz;
			}
			break;
		}

	//sets the channel information
	if( KAACChannelMono == iAudioInfo.iChannel ) //if 0 it indicates mono
		{
		iSettings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		}
	else //else if it is 1 it indicates stereo
		{
		iSettings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
		}
	IRLOG_DEBUG( "CIRAACPlayer::GetAACAudioProperties - Exiting (2)." );
	}

// ---------------------------------------------------------------------------
// Function: DoFindnDecodeAACHeader
// Function return a non modifiable pointer descriptor of data buffer
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::DoFindnDecodeAACHeaderL()
	{
	IRLOG_DEBUG( "CIRAACPlayer::DoFindnDecodeAACHeaderL." );
	TUint8* input = iInputBufferPtr;
	TUint8* endptr = iInputBufferPtr + iInputBufferSize - KAACHeaderSize;
	while ( (endptr > input) && (!iHeaderFound) )
		{
		//ADIF header is checked
		if( ('A' == input[KZERO]) && ('D' == input[KONE]) && ('I' == input[KTWO])	&&
			('F' ==input[KTHREE]) )
			{
			iHeaderFound = ETrue;
			}
		else
			{
			//checking for sync bits
			if( (0xFF == input[0]) && (0xF0 == (0xF6 & input[1])) )
				{
				//sync bits are obtained
				TInt framelength = 0;
				framelength |= (input[KTHREE] & 0x03) << KELEVEN;
				framelength |= (input[KFOUR] << KTHREE);
				framelength |= (input[KFIVE] >> KFIVE) & 0x07;
				if( (0 >= framelength) &&
					(endptr > (input + framelength + KFOUR)) )
					{
					iHeaderFound = EFalse;
					}
				else
					{
					//cross checked
					TUint8* nextheader = input + framelength;
					if( (0xFF == nextheader[0]) &&
						(0xF0 == ( 0xF6 & nextheader[1])) )
						{
					 	iHeaderFound = ETrue;
						}
					}
				}
			}
		if( iHeaderFound )
			{
			//header is found
			TUint value = 0;
			value |= input[0] << KTWENTYFOUR;
			value |= input[1] << KSIXTEEN;
			value |= input[2] << KEIGHT;
			value |= input[KTHREE];
			//version information is obtained from 19th bit
			//of acc/accp header
			TInt index = ( value >> 20 ) & 0x0FFF;
			if( 0x0FFF == (index & 0xFFF) )
				{
				iAudioInfo.iVersion = (value >> 19) & 0x01; //MPEG version
														    //0 MPEG 4
														    //1 MPEG 2
				//selection bits for sampling rate starts with 10th bit
				//4 bits are need for getting the sampling rate.
				//this bit combination is extracted and stored in index
				index = ( value >> 10 ) & 0x00F;
				iAudioInfo.iSamplingRate = KAacSamplingRate[index]; //gets the sampling rate
				if( 0 == iAudioInfo.iSamplingRate ) //if sampling rate is zero then we consider the header is wrong
					{
					iHeaderFound = EFalse;
					}
				else
					{
					//decoding channel information
					index = (value >> 6) & 0x07;
					if( index == 0 || index == 2 )
						{
						iAudioInfo.iChannel = 1; //channel information
						}
					else if( index == 1 )
						{
						//1 represents stereo and
						iAudioInfo.iChannel = 0; //0 represents mono
						}
					else
						{
						iHeaderFound = EFalse;
						}

					if( iHeaderFound )
						{
						//additional check is done for validation
						//of aac header
						index = ( value >> 10 ) & 0x00F;

						value = 0;
						value |= (input[KFOUR] << KTWENTYFOUR);
						value |= (input[KFIVE] << KSIXTEEN);
						value |= (input[KSIX] << KEIGHT);

						index |= ( (value >> KTWENTYONE ) & 0x07FF );
						TInt RawData = ((value >> KEIGHT) &0x03) + 1;
						if( index > (KONEFIVETHREESIX * RawData) )
							{
							iHeaderFound = EFalse;
							}
						}
					}
				}
			}
		//if header is not found we go for next header
		if( !iHeaderFound )
			{
			input++;
			}
		}
	if( iHeaderFound )
		{
		ReArrangeBufferL(input);
		}
	IRLOG_DEBUG( "CIRAACPlayer::DoFindnDecodeAACHeaderL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: ReArrangeBuffer
// Function rearranges the buffer, and should be called after finding the header
// from the address of buffers specified, the data buffers are kept in sink queue and
// remaining buffers are discarded that is put into source queue.
// ---------------------------------------------------------------------------
//
void CIRAACPlayer::ReArrangeBufferL(TUint8* aInput)
	{
	IRLOG_DEBUG( "CIRAACPlayer::ReArrangeBufferL" );
	//if header is found we are rearranging the buffer
	TUint8* input = aInput;
	TUint8* endptr = iInputBufferPtr + iInputBufferSize;
	TUint8* tempbuffer = input;

	while(!iSinkBufferQ.IsEmpty())
		{
		//removing all the entries in sink buffers queue
		iTempBufferHolder = iSinkBufferQ.First();
		iSinkBufferQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		iTempBufferHolder = NULL;
		}

	//adds the valid data
	for(tempbuffer = input; endptr > (tempbuffer + iBufferOffset);
		tempbuffer += iBufferOffset)
		{
		iTempBufferHolder =
			CIRBufferContainer::NewL(tempbuffer,iPlayBufferSize);
		iSinkBufferQ.AddLast(*iTempBufferHolder);
		}

	//adds the valid data
	TInt length = endptr - tempbuffer;
	if( length > 0 )
		{
		// this buffer will be removed after first time you play
		iTempBufferHolder =
			CIRBufferContainer::NewL(tempbuffer,length);
		iSinkBufferQ.AddLast(*iTempBufferHolder);
		}

	//adds the buffers to source buffer queue
	for(tempbuffer = iInputBufferPtr;
		input > (tempbuffer + iBufferOffset);
		tempbuffer += iBufferOffset)
		{
		iTempBufferHolder =
			CIRBufferContainer::NewL(tempbuffer,iPlayBufferSize);
		iSourceBufferQ.AddLast(*iTempBufferHolder);
		}

	IRLOG_DEBUG( "CIRAACPlayer::ReArrangeBufferL - Exiting." );
	}

// CIRMP3Player::GetMediaClientInstance()
// Returns the Audio Player Instance
// ---------------------------------------------------------------------------
//
CMdaAudioOutputStream* CIRAACPlayer::GetAudioPlayer()
	{
	IRLOG_DEBUG( "CIRMP3Player::GetMediaClientInstance " );
	return iAudioPlayer;	
	}
