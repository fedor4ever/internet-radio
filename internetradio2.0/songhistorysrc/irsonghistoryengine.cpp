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
* Description:  Engine class to notify channel name/metadata changes to SongHistoryDb and UI
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Thu Apr 14 12:00:00 2008 by Rohit
*  Ref:
*  Pc lint fixes
*
*  Version: 2, Thu Apr 10 20:00:00 2008 by Rohit
*  Ref:
*  Implemented SongHistory changes for channelwise songs
*
*  </ccm_history>
* ============================================================================
*/

#include "irpubsubkeys.h"
#include "irsonghistoryengine.h"
#include "uinotifyhandler.h"
#include "irsonghistoryengpubsub.h"
#include "irmetadata.h"
#include "irdebug.h"

class CIRSongHistoryDb;


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::CIRSongHistoryEngine
// Default Constructor.
// ---------------------------------------------------------------------------
//
CIRSongHistoryEngine::CIRSongHistoryEngine(MSongHistoryUINotifyHandler& aUiObserver )
						:iUiObserver( aUiObserver )
{
}

// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::ConstructL
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CIRSongHistoryEngine::ConstructL()
{
    IRLOG_DEBUG( "CIRSongHistoryEngine::ConstructL" );

	iSongHistoryDb = CIRSongHistoryDb::NewL();



    //Subsciber for the meta data
    iMetaDataSub = CIRSongHistoryEngPubSub::NewL( *this,
    		KUidActiveInternetRadioApp,KIRPSMetaData, RProperty::ELargeText) ;

    IRLOG_DEBUG( "CIRSongHistoryEngine::ConstructL - Exiting." );
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::NewL()
// Static constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSongHistoryEngine* CIRSongHistoryEngine::NewL( 
				MSongHistoryUINotifyHandler & aUiObserver)
{
    IRLOG_DEBUG( "CIRSongHistoryEngine::NewL - Entering" );
	CIRSongHistoryEngine* self = NewLC( aUiObserver);
	CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRSongHistoryEngine::NewL - Exiting." );
    return self;
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::NewLC()
// Static constructor.
// ---------------------------------------------------------------------------
//

EXPORT_C CIRSongHistoryEngine* CIRSongHistoryEngine::NewLC( 
							MSongHistoryUINotifyHandler & aUiObserver)
{
    IRLOG_DEBUG( "CIRSongHistoryEngine::NewLC - Entering" );
    CIRSongHistoryEngine* self = new ( ELeave ) CIRSongHistoryEngine( aUiObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
	IRLOG_DEBUG( "CIRSongHistoryEngine::NewLC - Exiting." );
    return self;
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::~CIRSongHistoryEngine()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSongHistoryEngine::~CIRSongHistoryEngine()
{
    IRLOG_DEBUG( "CIRSongHistoryEngine::~CIRSongHistoryEngine - Entering" );

    delete iSongHistoryDb;
    delete iChannelNameSub;
    delete iMetaDataSub;

    IRLOG_DEBUG( "CIRSongHistoryEngine::~CIRSongHistoryEngine - Exiting." );
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::HandleMetaDataReceivedL()
// Invoked when any listened P&S key is changed.
// ---------------------------------------------------------------------------
//

EXPORT_C void CIRSongHistoryEngine::HandleMetaDataReceivedL( const CIRMetaData& aMetaData )
{
	IRLOG_DEBUG( "CIRSongHistoryEngine::HandleMetaDataChangeL - Entering" );

	// Read Channel Name
	RBuf 			 channelName;

	TBuf<KMAXLENGTH> channelNameBuffer;
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSDataChannel, channelNameBuffer);
	channelName.Create(channelNameBuffer.Length());
	channelName.CleanupClosePushL();
	channelName.Copy(channelNameBuffer);

	TInt channelType;
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSChannelType, channelType);

	TInt channelId;
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSChannelId, channelId);

	// Read Channel Url
	RBuf 			 channelUrl;
	
	TBuf<KMAXLENGTH> channelUrlBuffer;
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSChannelUrl, channelUrlBuffer);
	channelUrl.Create(channelUrlBuffer.Length());
	channelUrl.CleanupClosePushL();
	channelUrl.Copy(channelUrlBuffer);
	

	
	RBuf 			 channelDesc;

	TBuf<KMAXLENGTH> channelDescBuffer;

		RProperty::Get ( KUidActiveInternetRadioApp, KIRPSDataChannelDesc, channelDescBuffer);
		channelDesc.Create(channelDescBuffer.Length());
		channelDesc.CleanupClosePushL();
		channelDesc.Copy(channelDescBuffer);

    RBuf             imageUrl;
    imageUrl.Create(KNullDesC().Length());
    imageUrl.CleanupClosePushL();
    imageUrl.Copy(KNullDesC);

	RBuf 			 musicFlag;

	TBuf<KMAXLENGTH> musicFlagBuffer;
	
	if(channelType==1)
		{
		RProperty::Get ( KUidActiveInternetRadioApp, KIRPSChannelMusicFlag, musicFlagBuffer);
		musicFlag.Create(musicFlagBuffer.Length());
		musicFlag.CleanupClosePushL();
		musicFlag.Copy(musicFlagBuffer);
		}
	else
		{
		musicFlag.Create(KNullDesC().Length());
		musicFlag.CleanupClosePushL();
		musicFlag.Copy(KNullDesC);
		}	
    
    if(channelType==1)
	    {
	    TInt ret = iSongHistoryDb->GetIdPresentInDbL( channelId );
	    if(ret)
		    {
		    TInt modified = iSongHistoryDb->UpdateSongHistoryDbL( channelId ,
		    		 channelName ,channelUrl ,imageUrl ,musicFlag);	
		    if(modified)
			    {
    	        iUiObserver.ChannelChangedL( aMetaData.Song(),aMetaData.Artist(),
    	        		 channelName, channelUrl );
			    }
		    }
	    	
	    }

	TSongHistoryItemChange changeCode = DetectSongHistoryItemChangesL(aMetaData,
							 channelName, channelUrl);

    if(changeCode==EBLANKENTRIES || changeCode == ENOCHANGE )
    {
    
        CleanupStack::PopAndDestroy(&musicFlag);
        CleanupStack::PopAndDestroy(&imageUrl);
        CleanupStack::PopAndDestroy(&channelDesc);
        CleanupStack::PopAndDestroy(&channelUrl);
        CleanupStack::PopAndDestroy(&channelName);
    	return;
    }

	RBuf delSongName;
	delSongName.CleanupClosePushL();
	RBuf delArtistName;
	delArtistName.CleanupClosePushL();
	RBuf delChannelName;
	delChannelName.CleanupClosePushL();
	RBuf delChannelUrl;
	delChannelUrl.CleanupClosePushL();
	TBool bExceededMax = iSongHistoryDb->AddToSongHistoryDbL( aMetaData.Song(),
															  aMetaData.Artist(),
															  channelName,
															  channelUrl, 
															  delSongName, 
															  delArtistName, 
															  delChannelName, 
															  delChannelUrl ,
															  channelType,
															  channelId,
															  64,
															  channelDesc,
															  imageUrl,
															  musicFlag
															  );
	if( bExceededMax )
	{
		if( 0 == iSongHistoryDb->GetChannelSongsCountL(delChannelName,delChannelUrl) )
		{
			iUiObserver.RemoveChannelEntry(delSongName, delArtistName, delChannelName, delChannelUrl);
		}
		else
		{
			iUiObserver.RemoveSongEntry(delSongName, delArtistName, delChannelName, delChannelUrl);
		}
	}


	if( changeCode == ECHANNELCHANGED ) // channel change detected
    {
    	iUiObserver.ChannelChangedL( aMetaData.Song(),aMetaData.Artist(), channelName, channelUrl );
    }
    else	// same channel playing next song
    {
		iUiObserver.NextSongReceivedL( aMetaData.Song(),aMetaData.Artist(), channelName, channelUrl );
    }
    
   	CleanupStack::PopAndDestroy(&delChannelUrl);
	CleanupStack::PopAndDestroy(&delChannelName);
	CleanupStack::PopAndDestroy(&delArtistName);
	CleanupStack::PopAndDestroy(&delSongName);

    CleanupStack::PopAndDestroy(&musicFlag);
    CleanupStack::PopAndDestroy(&imageUrl);
    CleanupStack::PopAndDestroy(&channelDesc);
    CleanupStack::PopAndDestroy(&channelUrl);
    CleanupStack::PopAndDestroy(&channelName);

    
	IRLOG_DEBUG( "CIRSongHistoryEngine::HandleMetaDataChangeL - Exiting." );
}
// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::DetectSongHistoryItemChangesL()
// To detect any change in the song history items.
// ---------------------------------------------------------------------------
//

TSongHistoryItemChange CIRSongHistoryEngine::DetectSongHistoryItemChangesL(
			const CIRMetaData& aMetaData, const RBuf& aChannelName, const RBuf& aChannelUrl)
{
	if(aChannelName.Length() == 0)
	{
		return EBLANKENTRIES;
	}

	//If the meta data is not available
	if(((aMetaData.Song()).Length() == 0) && ((aMetaData.Artist()).Length() == 0))
	{
		return EBLANKENTRIES;
	}

	RBuf song;
	song.CleanupClosePushL();
	RBuf artist;
	artist.CleanupClosePushL();
	RBuf channel;
	channel.CleanupClosePushL();
	RBuf url;
	url.CleanupClosePushL();

	TRAPD( error, iSongHistoryDb->GetLastHistoryItemL(song, artist, channel, url));
	if( error )
	{
		IRLOG_DEBUG( "CIRSongHistoryEngine::DetectSongHistoryItemChangesL() - Failed while GetLastHistoryItemL()" );
	}

	TSongHistoryItemChange retval = ENOCHANGE;	// pc-lint # 644 fix

	if((song == aMetaData.Song() ) && (artist == aMetaData.Artist()) &&
						 (channel == aChannelName)  && (url == aChannelUrl))
	{
		retval = ENOCHANGE;
	}
	else if( channel != aChannelName  || url != aChannelUrl )
	{
		retval = ECHANNELCHANGED;
	}
	else if( song != aMetaData.Song() || artist != aMetaData.Artist() )
	{
		retval = EMETADATACHANGED;
	}
	else
	{
		// pc-lint # 961 fix
	}

	CleanupStack::PopAndDestroy(&url);
    CleanupStack::PopAndDestroy(&channel);
    CleanupStack::PopAndDestroy(&artist);
    CleanupStack::PopAndDestroy(&song);

	return retval;
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::HandlePropertyChangeErrorL
// Invoked when an error has occured while fetching the new value of any listened P&S key.
// ---------------------------------------------------------------------------
//
void CIRSongHistoryEngine::HandlePropertyChangeErrorL( const TUid& /*aCategory*/,
	TUint /*aKey*/, TInt aError )
{
    IRLOG_DEBUG( "CIRSongHistoryEngine::HandlePropertyChangeErrorL" );
    if (aError < 0)
    	{
    		//Some action has to be taken
    	}
    IRLOG_DEBUG( "CIRSongHistoryEngine::HandlePropertyChangeErrorL" );
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::HandlePropertyChangeL
// Invoked when any listened P&S key is changed.
// ---------------------------------------------------------------------------

void CIRSongHistoryEngine::HandlePropertyChangeL( const TUid& aCategory,
			 TUint aKey, const TDesC8& aText )
{
	IRLOG_DEBUG( "CIRSongHistoryEngine::HandlePropertyChangeL" );
	if ( aCategory == KUidActiveInternetRadioApp )
	{
		if( aKey == KIRPSDataChannel )
		{
		}
		else if( aKey == KIRPSMetaData )
		{
			CIRMetaData* metaData = CIRMetaData::NewL( aText );
			CleanupStack::PushL( metaData );
			HandleMetaDataReceivedL( *metaData );
			CleanupStack::PopAndDestroy( metaData );
		}
		else
		{
		// To avoid PC lint error
		}
	}
	IRLOG_DEBUG( "CIRSongHistoryEngine::HandlePropertyChangeL - Exiting." );
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::GetAllHistoryL
// Invoked when when UI request.
// Getting the data from SongHistoryDb.
// ---------------------------------------------------------------------------

EXPORT_C void CIRSongHistoryEngine::GetAllHistoryL( 
				RPointerArray<CIRSongHistoryInfo>& aSongHistoryInfo )
{
	iSongHistoryDb->GetAllSongHistoryListL(aSongHistoryInfo);
	
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::ClearAllHistory
// Invoked when when UI request.
// Clears the SongHistoryDb.
// ---------------------------------------------------------------------------

EXPORT_C TInt CIRSongHistoryEngine::ClearAllHistory( )
{	
	TInt ret = 0;
	ret = iSongHistoryDb->ClearAllSongHistoryFromDb();
	return ret;
}

EXPORT_C TInt CIRSongHistoryEngine::DeleteOneHistory(TInt aIndex)
{     
    return iSongHistoryDb->DeleteOneHistory(aIndex);
}


// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::GetHistoryCount
// Invoked when when UI request.
// Gets the count of the SongHistoryDb.
// ---------------------------------------------------------------------------

EXPORT_C TInt CIRSongHistoryEngine::GetHistoryCount()
{
	TInt HistoryCount = 0;
	HistoryCount = iSongHistoryDb->CountSongHistoryDb();
	return HistoryCount;
}

// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::SyncHistory
// Invoked when when UI request.
// Synchronises the Song history DB with the new data in the ISDS.
// ---------------------------------------------------------------------------

EXPORT_C TInt CIRSongHistoryEngine::SyncHistory(TInt aChannelId)
{
	IRLOG_DEBUG( "CIRSongHistoryEngine::SyncHistory - Entering." );
	TRAPD(err,iSongHistoryDb->SyncSongHistoryDbL(aChannelId));
	if(err)
	{
	
	}
	IRLOG_DEBUG( "CIRSongHistoryEngine::SyncHistory - Exiting." );
    return KErrNone;
}
// ---------------------------------------------------------------------------
// CIRSongHistoryEngine::UpdateSongHistoryDB
// Invoked when when UI request.
// Synchronises the Song history DB with the new data in the ISDS.
// ---------------------------------------------------------------------------

EXPORT_C void CIRSongHistoryEngine::UpdateSongHistoryDb(TInt aChannelId,
											const TDesC& aChannelName, 
											const TDesC& aChannelUrl,
											const TDesC& aMusicFlag)
{
	IRLOG_DEBUG( "CIRSongHistoryEngine::UpdateSongHistoryDb - Entering." );
	TInt ret = KErrNone ;
	TRAPD(err,ret =	iSongHistoryDb->GetIdPresentInDbL( aChannelId ));
	if(err)
	{
	return;
	}
	if(ret)
		{
		TRAPD(err,iSongHistoryDb->UpdateSongHistoryDbL( aChannelId ,
		                                                aChannelName ,
		                                                aChannelUrl ,
		                                                aChannelUrl,
		                                                aMusicFlag));
		if(err)
			{
			return ;
			}
		}
		
	IRLOG_DEBUG( "CIRSongHistoryEngine::UpdateSongHistoryDb - Exiting." );
}
