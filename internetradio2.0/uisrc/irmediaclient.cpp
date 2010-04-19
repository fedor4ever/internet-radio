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


#include <e32property.h>
#include "irpubsubkeys.h"

#include "irctrlcmdobserver.h"
#include "irdebug.h"
#include "irmediaclient.h"
#include "irmediaenginebuffer.h"
#include "irmediaengineinterface.h"
#include "ircontrolparams.h"

const TInt KZeroVolumeLevel = 0;
const TInt KIRBitRateDivider = 8;    // To convert bits to bytes.
const TInt KIRByteMultiplier = 1024; // To convert kilo bytes to bytes.
const TInt KIRValidBitRate = 0;      // Starting point for valid bit rate.
const TInt KIRDefBitRate = 64;       // Default bit rate for buffer size calculation.
const TInt KIRMinBitRate = 8;        // Minimum bit rate for buffer size calculation.
const TInt KIRMaxBitRate = 192;      // Maximum bit rate for buffer size calculation.

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Function : NewL
// Two Phase Constructor - NewL
// ---------------------------------------------------------------------------
//	 
CIRMediaClient* CIRMediaClient::NewL(CIRCtrlCmdObserver* aChannel)
	{
	IRLOG_DEBUG( "CIRMediaClient::NewL - Entering" );
	CIRMediaClient* self = NewLC(aChannel);
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRMediaClient::NewL - Exiting." );
    return self;	
	}
	
// ---------------------------------------------------------------------------
// Function : NewLC
// Two Phase Constructor - NewLC
// ---------------------------------------------------------------------------
//
CIRMediaClient* CIRMediaClient::NewLC(CIRCtrlCmdObserver* aChannel)
	{
	IRLOG_DEBUG( "CIRMediaClient::NewLC - Entering" );
	CIRMediaClient* self = new (ELeave) CIRMediaClient();
    CleanupStack::PushL(self);
    self->ConstructL(aChannel);
	IRLOG_DEBUG( "CIRMediaClient::NewLC - Exiting." );
    return self;	
	}

// ---------------------------------------------------------------------------
// destructor function
// destructor of the player component
// ---------------------------------------------------------------------------
//
CIRMediaClient::~CIRMediaClient()
    {
	IRLOG_DEBUG( "CIRMediaClient::~CIRMediaClient - Entering" );
	//deletes the player
	delete iPlayer;
	//deletes the buffers associated with player
	delete[] iTempBuffer;
	delete[] iCurrentBuffer;
	IRLOG_DEBUG( "CIRMediaClient::~CIRMediaClient - Exiting." );
    }
    	
// ---------------------------------------------------------------------------
// This is default Constructor
// for the class CIRMediaClient
// ---------------------------------------------------------------------------
//	
CIRMediaClient::CIRMediaClient():iInputBuffer(NULL,0,0) 
	{
	IRLOG_DEBUG( "CIRMediaClient::CIRMediaClient" );
	}

// ---------------------------------------------------------------------------
// Two phase ConstructL
// network component is taken as input
// ---------------------------------------------------------------------------
//
void CIRMediaClient::ConstructL(CIRCtrlCmdObserver* aChannel)
    {
	IRLOG_DEBUG( "CIRMediaClient::ConstructL - Entering" );
	iChannel = aChannel;
	IRLOG_DEBUG( "CIRMediaClient::ConstructL - Exiting" );
	}
						
										   //For Play control
										   
// ---------------------------------------------------------------------------
// Function : PlayL
// Starts to play the stream
// ---------------------------------------------------------------------------
//
TInt CIRMediaClient::Play()
	{
	IRLOG_DEBUG( "CIRMediaClient::Play - Entering" );
	IRLOG_DEBUG( "CIRMediaClient::Play - Exiting" );
	//sents a play request to media engine
	if( iPlayer )
		{
		iPlayer->Play();
		IRLOG_DEBUG( "CIRMediaClient::Play - Exiting (1)." );
		return KErrNone;
		}
	else
		{
		IRLOG_DEBUG( "CIRMediaClient::Play - Exiting (2)." );
		return KErrNotFound;
		}	
	}

// ---------------------------------------------------------------------------
// Function : StopL
// Stops to play the stream
// ---------------------------------------------------------------------------
//
void CIRMediaClient::Stop()
	{
	IRLOG_DEBUG( "CIRMediaClient::Stop - Entering" );
	//sents a stop request to media engine
	if( iPlayer )
		{
		iPlayer->Stop();
		}
	else
		{
		iCommand = EStoppedPlaying;
		iChannel->SentRequest(iCommand,KErrNone);
		}
	IRLOG_DEBUG( "CIRMediaClient::Stop - Exiting." );
	}
										
										 //For Volume Control
// ---------------------------------------------------------------------------
// Function : SetVolume
// function to set the volume, 
// ---------------------------------------------------------------------------
//
void CIRMediaClient::SetVolume(TInt aVolume )
	{
	IRLOG_DEBUG( "CIRMediaClient::SetVolume - Entering" );
	//if player is exists it will set the volume
	if( iPlayer )
		{
		iPlayer->SetVolume(aVolume);	
		}		
	IRLOG_DEBUG( "CIRMediaClient::SetVolume - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : MaxVolume
// ---------------------------------------------------------------------------
//	
TInt CIRMediaClient::MaxVolume() const
	{
	IRLOG_DEBUG( "CIRMediaClient::MaxVolume - Entering" );
	IRLOG_DEBUG( "CIRMediaClient::MaxVolume - Exiting" );
	//if player is created it will sent the max volume else return zero instead of MaxVolume
	if( iPlayer )
		{
		IRLOG_DEBUG( "CIRMediaClient::MaxVolume - Exiting (1)." );
		//returns maximum volume if player exists
		return iPlayer->MaxVolume();	
		}		
	else
		{
		//else zero is returned
		IRLOG_DEBUG( "CIRMediaClient::MaxVolume - Exiting (2)." );
		return KZeroVolumeLevel;	
		}
	}

// ---------------------------------------------------------------------------
// Function : Volume
// function to returns the volume
// ---------------------------------------------------------------------------
//		
TInt CIRMediaClient::Volume() const
	{
	IRLOG_DEBUG( "CIRMediaClient::Volume - Entering" );
	IRLOG_DEBUG( "CIRMediaClient::Volume - Exiting" );
	//if player is created it will sent the volume else return zero instead of Volume
	if( iPlayer )
		{
		//if player exist it returns the current volume
		IRLOG_DEBUG( "CIRMediaClient::Volume - Exiting (1)." );
		return iPlayer->Volume();	
		}		
	else
		{
		//else will return negative volume
		IRLOG_DEBUG( "CIRMediaClient::Volume - Exiting (2)." );
		return KZeroVolumeLevel;	
		}
	}

// ---------------------------------------------------------------------------
// Function: BufferFilled
// Set the whether a Fill Buffer is currently active
// ---------------------------------------------------------------------------
//
void CIRMediaClient::BufferFilled()
	{
	IRLOG_DEBUG( "CIRMediaClient::BufferFilled - Entering" );
	if( iFirstTime )
		{
		//First all the buffers are filled
		iInputBufferPtr += iConfig.iPlayBufferSize;	
		if( (iInputBufferPtr + iConfig.iPlayBufferSize) <= 
			(iTempBuffer + iConfig.iPlayBufferCount*iConfig.iPlayBufferSize) )
			{
			//buffer is not completely filled
			iInputBuffer.Set(iInputBufferPtr,iConfig.iPlayBufferSize,iConfig.iPlayBufferSize);	
			
			if( iStopBuffering )
				{
				//if iStopBuffering is true we won't continue in this loop
				return;		
				}			
			else // if not stop buffering is called
				{
				//fills the buffer and sents the buffer percentage
				iChannel->FilltheBuffer(iInputBuffer);			
				}				
			}
		else
			{
			//buffer is completely filled
			iFirstTime = EFalse;
			iNewPlayer = EFalse;
			if( iStopBuffering )
				{
				//if stop buffering is true we will not proceed further
				return;	
				}
			else
				{
                // Error reporting done inside CreatePlayer
				if ( CreatePlayer() )
				    {
				    // Sending this event causes NowPlayingView activation
				    iBufferPercentage = K100Percentage;				
				    iChannel->SentRequest( EBufferFillStop, iBufferPercentage );	
				
				    //player has already buffered completely for the first time
				    //so even if StopInitialBuffering is called media client is help less
				    //to handle this situation iStopBuffering is made ETrue
				    iStopBuffering = ETrue;

				    //called for intializing a new player
				    InitializePlayer( iConfig,iChannel );
				
				    //calls play for the first time
				    iPlayer->Play();
				    }
				}									
			}
		}
	else
		{
		//buffer is already filled for first time
		//media engine is created and playing so Buffer filled 
		//function of media Engine should be called
		iPlayer->BufferFilled();				
		}			
	IRLOG_DEBUG( "CIRMediaClient::BufferFilled - Exiting." );
	}
	
// ---------------------------------------------------------------------------
// Function: StartNewPlayerL
// Set the whether a Fill Buffer is currently active
// ---------------------------------------------------------------------------
//
void CIRMediaClient::StartNewPlayerL(TConfig& aConfig,const TDesC8& aFormat
	)
	{
	IRLOG_DEBUG( "CIRMediaClient::StartNewPlayerL - Entering" );
	//format is copied
	iFormat.Copy(aFormat);

	//stop of intial buffering from media client is enable
	iStopBuffering = EFalse;
	
	iConfig = aConfig;

	TInt bitRate(0);
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSBitrate, bitRate );
    // Next line will round the bitrate down to be divisible by KIRBitRateDivider.
	bitRate = bitRate - (bitRate % KIRBitRateDivider);
	if( bitRate <= KIRValidBitRate )
	    {
	    // If invalid bit rate info, use default bit rate for calculating buffer sizes.
	    bitRate = KIRDefBitRate;
	    }
	else if( bitRate < KIRMinBitRate )
	    {
	    // If small bit rate info, use minimum bit rate for calculating buffer sizes.
	    // To make single buffer minimum size 1 kB.
	    bitRate = KIRMinBitRate;
	    }
	else if( bitRate > KIRMaxBitRate )
	    {
	    // If large bit rate info, use maximum bit rate for calculating buffer sizes.
	    // To make single buffer maximum size 24 kB.
	    bitRate = KIRMaxBitRate;
	    }
	else
	    {
	    }
	iConfig.iPlayBufferCount = KIRInputBufferCount;
    iConfig.iPlayBufferSize = KIRByteMultiplier*bitRate/KIRBitRateDivider;
	
	//creates a new instance of media buffer for new player
	iTempBuffer = new (ELeave) TUint8[iConfig.iPlayBufferCount*iConfig.iPlayBufferSize];
	iInputBufferPtr = iTempBuffer;

	//initial buffering starts
	iInputBuffer.Set(iInputBufferPtr,iConfig.iPlayBufferSize,iConfig.iPlayBufferSize);
	iNewPlayer = ETrue;
	iFirstTime = ETrue;

	//initial buffering starts
	iChannel->FilltheBuffer(iInputBuffer);	
	
	//starts audio fade out for the previous channel
	IRLOG_DEBUG( "CIRMediaClient::StartNewPlayerL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: StopMediaBuffering
// stops buffering of media engine, the change cannot be reverted 
// ---------------------------------------------------------------------------
//
void CIRMediaClient::StopMediaBuffering()
	{
	IRLOG_DEBUG( "CIRMediaClient::StopMediaBuffering - Entering" );
	//calls media engine's stop buffering cancels the buffering in media engine
	if( iPlayer )
		{
		iPlayer->StopPlayerBuffering();	
		}			
	IRLOG_DEBUG( "CIRMediaClient::StopMediaBuffering - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: StopInitialBuffering
// stops buffering from media client, media engine is not stopped 
// ---------------------------------------------------------------------------
//
void CIRMediaClient::StopInitialBuffering()
	{
	IRLOG_DEBUG( "CIRMediaClient::StopInitialBuffering - Entering" );
	//initially buffering is taken care by media client and then 
	//it shift the responsibility to media engine
	//this is done because audio fade out has to take place when other channel buffers
	//if media client is handling the buffer it should be stopped
	//media client is handling the buffering
	iStopBuffering = ETrue;
	
	delete[] iTempBuffer;
	iTempBuffer = NULL;
	
	iCommand=EStereo;
	iChannel->SentRequest(iCommand,KErrNone);
	delete iPlayer;
	iPlayer = NULL;
	
	iStopBuffering = ETrue;
	//return true to indicate success of cancelling of buffer
	IRLOG_DEBUG( "CIRMediaClient::StopInitialBuffering - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function: SetCodecSettings
// Creates an instance of the players and initialize it
// ---------------------------------------------------------------------------
//
TBool CIRMediaClient::CreatePlayer()
	{
    IRLOG_DEBUG( "CIRMediaClient::CreatePlayer" );
    TBool createResult( ETrue );
	//delete the instance of previously created player
	iCommand=EStereo;
	iChannel->SentRequest(iCommand,KErrNone);
    if ( iPlayer )
    	{
    	delete iPlayer;
    	iPlayer = NULL;
    	}
		
	//media engine's instance is created type is stored in iFormat (basically the mime type)
	TRAPD(error,iPlayer = CIRMediaEngineInterface::NewL(iFormat));
    if( error )
        {
        iChannel->SentRequest( EError, KIRCtrlCmdPlayerNotCreated );
        createResult = EFalse;
        }
    
    return createResult;    
	}
// ---------------------------------------------------------------------------
// CIRMediaClient::InitializePlayer
// ---------------------------------------------------------------------------
//
void CIRMediaClient::InitializePlayer( TConfig& aConfig,
	CIRCtrlCmdObserver* aChannel)
	{
	IRLOG_DEBUG( "CIRMediaClient::SetCodecSettings" );

	//delete the instance of buffer associated with previously playing player
	delete[] iCurrentBuffer;
	iCurrentBuffer = NULL;
	
	//current buffer is made same as previous buffer
	iCurrentBuffer = iTempBuffer;
	iTempBuffer = NULL;
		
	//intialize the media engine
	iPlayer->Intialize(aConfig,iCurrentBuffer,aChannel);
	IRLOG_DEBUG( "CIRMediaClient::SetCodecSettings - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetMediaClientInstance()
// Returns the Audio Player Instance
// ---------------------------------------------------------------------------
//
CMdaAudioOutputStream* CIRMediaClient::GetPlayerInstance()
	{
	IRLOG_DEBUG( "CIRMediaClient::GetMediaClientInstance " );
	return (iPlayer->GetAudioPlayer());	
	}
