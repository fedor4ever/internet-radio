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
* Description:  Implementation of streaming and playback on ASF channel url
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 1, Fri Sep 19 13:00:00 2008 by e0328782 Rohit
*  Ref:
*  Created
*
*  </ccm_history>
* ============================================================================
*/

#include <e32property.h>
#include "irpubsubkeys.h"

#include "irasfplayer.h"				// this class
#include "irctrlcommand.h"				// MIRCtrlCmdObserver
#include "irstreamsourceobserver.h"		// MIRStreamSourceObserver
#include "irdebug.h"
#include "irmetadata.h"

//Constants
_LIT8( KMimeTypeASF, "audio/x-ms-wma" );
const TInt KFour = 4;
_LIT( KIRSongTitle, "title" );
_LIT( KIRSongArtist, "artist" );
_LIT( KTrailingAsf, ".asf" );
_LIT( KTrailingWma, ".wma" );
_LIT( KTrailingWmv, ".wmv" );


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CIRAsfPlayer::NewL
// function returns an instance of CIRAsfPlayer
// Two phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRAsfPlayer* CIRAsfPlayer::NewL(MIRStreamSourceObserver& aStreamObserver,
										  MIRCtrlCmdObserver& aCtrlCmdObserver)
	{
	IRLOG_DEBUG( "CIRAsfPlayer::NewL" );
	CIRAsfPlayer* self = CIRAsfPlayer::NewLC(aStreamObserver, aCtrlCmdObserver );
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRAsfPlayer::NewL - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::NewLC
// function creates an instance of CIRAsfPlayer
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRAsfPlayer* CIRAsfPlayer::NewLC(MIRStreamSourceObserver& aStreamObserver,
								  MIRCtrlCmdObserver& aCtrlCmdObserver)
	{
	IRLOG_DEBUG( "CIRAsfPlayer::NewLC" );
	CIRAsfPlayer* self = new (ELeave) CIRAsfPlayer(aStreamObserver, aCtrlCmdObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRAsfPlayer::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::~CIRAsfPlayer
// Default Destructor
// ---------------------------------------------------------------------------
//
CIRAsfPlayer::~CIRAsfPlayer()
	{
	IRLOG_DEBUG( "CIRAsfPlayer::~CIRAsfPlayer" );
    if( iMdaAudioPlayer )
	    {
	    delete iMdaAudioPlayer;
	    iMdaAudioPlayer = NULL;
	    }

	if( iMetaData )
		{
		delete 	iMetaData;
		iMetaData = NULL;
		}
	iStationUri.Close();

	IRLOG_DEBUG( "CIRAsfPlayer::~CIRAsfPlayer - Exiting." );
	}

//Function for Play control

// ---------------------------------------------------------------------------
// CIRAsfPlayer::PlayL
// function to intiate the player and play the stream
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAsfPlayer::PlayL(const TDesC& aUri)
	{
	IRRDEBUG2( "CIRAsfPlayer::PlayL - Entering aUri=%S", &aUri );

    iStationUri.Close();
    iStationUri.CreateL( aUri );

	User::LeaveIfNull( iMdaAudioPlayer );
    if( iMdaAudioPlayer )
	    {
	    iMdaAudioPlayer->Close();
    	iMdaAudioPlayer->OpenUrlL( iStationUri, KUseDefaultIap, KMimeTypeASF );
	    }

	IRLOG_DEBUG( "CIRAsfPlayer::PlayL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::Play
// function to resume playing (AsfPlayer should already initiated using Play(url))
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAsfPlayer::Play()
	{
	IRLOG_DEBUG( "CIRAsfPlayer::Play" );

	SetVolume( iCtrlCmdObserver.FetchVolume() );
    if( iMdaAudioPlayer )
	    {
		iMdaAudioPlayer->Play();
	    }

	IRLOG_DEBUG( "CIRAsfPlayer::Play - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::Stop
// function to which stop the player
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAsfPlayer::Stop()
	{
	IRLOG_DEBUG( "CIRAsfPlayer::Stop" );

    if( iMdaAudioPlayer )
	    {
		iMdaAudioPlayer->Stop();
	    }

	IRLOG_DEBUG( "CIRAsfPlayer::Stop - Exiting." );
	}

										//Functions for Volume Control

// ---------------------------------------------------------------------------
// CIRAsfPlayer::SetVolume
// function to set the volume
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRAsfPlayer::SetVolume(TInt aVolume )
	{
	IRLOG_DEBUG( "CIRAsfPlayer::SetVolume" );

    if( iMdaAudioPlayer )
	    {
		iMdaAudioPlayer->SetVolume( aVolume );
	    }

	IRLOG_DEBUG( "CIRAsfPlayer::SetVolume - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::Volume
// function to returns the volume, integer level of volume is the Output
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRAsfPlayer::Volume() const
	{
	IRLOG_DEBUG( "CIRAsfPlayer::Volume" );

	TInt volume = 0;
    if( iMdaAudioPlayer )
	    {
		iMdaAudioPlayer->GetVolume( volume );
	    }

	return volume;
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::MaxVolume
// function to returns the maximum volume
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRAsfPlayer::MaxVolume() const
	{
	IRLOG_DEBUG( "CIRAsfPlayer::MaxVolume" );

	TInt volume = 0;
    if( iMdaAudioPlayer )
	    {
		volume = iMdaAudioPlayer->MaxVolume();
	    }

	return volume;
	}

// Helper functions

// ---------------------------------------------------------------------------
// CIRAsfPlayer::DetectAsfChannel
// Function is used to detect ASF channel by its url ending in .asf/.wma/.wmv
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRAsfPlayer::DetectAsfChannel(const TDesC& aUri)
	{
	IRLOG_DEBUG("CIRAsfPlayer::DetectAsfChannel - Entering");
	TPtrC ptr( aUri );
	TPtrC UriTypePtr( ptr.Right(KFour) );

	TBool bAsf = UriTypePtr == KTrailingAsf || UriTypePtr == KTrailingWma
							   || UriTypePtr == KTrailingWmv;

	IRRDEBUG2("CIRAsfPlayer::DetectAsfChannel - ASF Channel detected = %d", (TInt)bAsf);
	IRLOG_DEBUG("CIRAsfPlayer::DetectAsfChannel - Exiting");
	return bAsf;
	}

// Two-phase construction

// ---------------------------------------------------------------------------
// CIRAsfPlayer::CIRAsfPlayer
// Default constructor
// ---------------------------------------------------------------------------
//
CIRAsfPlayer::CIRAsfPlayer(MIRStreamSourceObserver& aStreamObserver,
						   MIRCtrlCmdObserver& aCtrlCmdObserver):
	iMdaAudioPlayer(NULL), iMetaData(NULL),
	iCtrlCmdObserver( aCtrlCmdObserver ), iStreamObserver(aStreamObserver),
	iState( ENotReady )
	{
	IRLOG_DEBUG( "CIRAsfPlayer::CIRAsfPlayer" );
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::ConstructL
// Two phase constructor is used to intialize data members
// Function can leave if CMdaAudioOutputStream::NewL leaves
// ---------------------------------------------------------------------------
//
void CIRAsfPlayer::ConstructL()
	{
	IRLOG_DEBUG( "CIRAsfPlayer::ConstructL" );

	iMdaAudioPlayer = CMdaAudioPlayerUtility::NewL( *this );
	User::LeaveIfNull( iMdaAudioPlayer );

	iMdaAudioPlayer->RegisterForAudioLoadingNotification( *this );

	iMetaData = CIRMetaData::NewL();
	User::LeaveIfNull( iMdaAudioPlayer );

	IRLOG_DEBUG( "CIRAsfPlayer::ConstructL - Exiting." );
	}

//Call back functions

// ---------------------------------------------------------------------------
// MMdaAudioPlayerCallback::MapcInitComplete
// Invoked with OpenUrl() success/fail result
// ---------------------------------------------------------------------------
//
void CIRAsfPlayer::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	IRRDEBUG2( "CIRAsfPlayer::MapcInitComplete - Entering. aError = %d", aError );
    if( KErrNone == aError )
	    {
	    iState = EReadyToPlay;
	    Play();
	    iStreamObserver.Asf_ConnectionEstablished();
    	}
	else
		{
		iState = ENotReady;
		iStreamObserver.Asf_ErrorConnecting(aError);
		}
	IRLOG_DEBUG( "CIRAsfPlayer::MapcInitComplete - Exiting" );
	}

// ---------------------------------------------------------------------------
// MMdaAudioPlayerCallback::MapcPlayComplete
// Invoked after playback of an audio sample completes success/fail
// ---------------------------------------------------------------------------
//
void CIRAsfPlayer::MapcPlayComplete(TInt aError)
	{
	IRRDEBUG2( "CIRAsfPlayer::MapcPlayComplete - Entering. aError = %d", aError );
	if( KErrNone != aError )
		{
		iState = ENotReady;
		iStreamObserver.Asf_ErrorConnecting(aError);
		}
	else
		{
		// won't hit this as playing from stream
		}
	IRLOG_DEBUG( "CIRAsfPlayer::MapcPlayComplete - Exiting" );
	}

// ---------------------------------------------------------------------------
// MAudioLoadingObserver::MaloLoadingStarted
//
// ---------------------------------------------------------------------------
//
void CIRAsfPlayer::MaloLoadingStarted()
	{
	IRLOG_DEBUG( "CIRAsfPlayer::MaloLoadingStarted - Entering" );
	TInt percentComplete = 0;

	if( iMdaAudioPlayer )
		{
		iState = EReadyToPlay;

		TRAPD( err, iMdaAudioPlayer->GetAudioLoadingProgressL( percentComplete ));
		if(err == KErrNone)
			iStreamObserver.Asf_UpdateProgress( percentComplete );
		}
	IRLOG_DEBUG( "CIRAsfPlayer::MaloLoadingStarted - Exiting" );
	}

// ---------------------------------------------------------------------------
// MAudioLoadingObserver::MaloLoadingComplete
//
// ---------------------------------------------------------------------------
//
void CIRAsfPlayer::MaloLoadingComplete()
	{
	IRLOG_DEBUG( "CIRAsfPlayer::MaloLoadingComplete - Entering" );
	TInt percentComplete = 0;

	if( iMdaAudioPlayer )
		{
		iState = EPlaying;

		TRAPD( err, iMdaAudioPlayer->GetAudioLoadingProgressL( percentComplete ));
		if(err == KErrNone)
			iStreamObserver.Asf_UpdateProgress( percentComplete );

		TRAP( err, ReadMetadataL());
		if(err != KErrNone)
			{
			IRLOG_WARNING( "CIRAsfPlayer::MaloLoadingComplete - Failed to read/publish metadata" );
			}

		// publish birtate
		TUint bitrate = 0;
		err = iMdaAudioPlayer->GetBitRate( bitrate );
		RProperty::Set( KUidActiveInternetRadioApp, KIRPSBitrate, bitrate );
		}
	IRLOG_DEBUG( "CIRAsfPlayer::MaloLoadingComplete - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRAsfPlayer::ReadMetadataL
// Reads the meta data from the CMdaAudioPlayerUtility
// ---------------------------------------------------------------------------
//
void CIRAsfPlayer::ReadMetadataL()
	{
    IRLOG_DEBUG( "CIRAsfPlayer::ReadMetadataL - Entering" );

	if(iMetaData)
		{
	    // Erases old meta data information.
	    iMetaData->SetArtistL( KNullDesC );
	    iMetaData->SetSongL( KNullDesC );
	    iMetaData->SetStreamUrlL( KNullDesC );

		// Stream Url
		iMetaData->SetStreamUrlL( iStationUri );

	    TInt nMetadata = 0;
	    TInt err = iMdaAudioPlayer->GetNumberOfMetaDataEntries( nMetadata );
	    for( TInt ctr=0; ctr < nMetadata; ++ctr )
	        {
	        CMMFMetaDataEntry* pMetadataEntry = iMdaAudioPlayer->GetMetaDataEntryL( ctr );

	        // Song Title
	        if( pMetadataEntry->Name() == KIRSongTitle )
	        	{
	        	iMetaData->SetSongL( pMetadataEntry->Value() );
	        	}
	        // Song Artist
	        else if( pMetadataEntry->Name() == KIRSongArtist )
	        	{
	        	iMetaData->SetArtistL( pMetadataEntry->Value() );
	        	}
	        }

		if( iMetaData->Song().Length() == 0 )
			{
			IRLOG_WARNING( "CIRAsfPlayer::ExtractMetaDataL - song \"title\" not found in metadata" );
			}
		if( iMetaData->Artist().Length() == 0 )
			{
			IRLOG_WARNING( "CIRAsfPlayer::ExtractMetaDataL - song \"artist\" not found in metadata" );
			}

		// Notify observer
		iStreamObserver.Asf_HandleMetaDataReceivedL( *iMetaData );

	    IRLOG_DEBUG4( "CIRAsfPlayer::ReadMetadataL - Exiting (artist=%S, song=%S, streamUrl=%S)",
	    		     &iMetaData->Artist(), &iMetaData->Song(), &iMetaData->StreamUrl() );
		}
	}
