/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/
#include "irsonghistoryinfo.h"
#include "irsonghistorydb.h"
#include "irqsonghistoryinfo.h"
#include "irqisdsdatastructure.h"
#include "irqmetadata.h"
#include "irqsonghistoryengine.h"

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::openInstance()
// Static function to get a singleton instance of IRQSongHistoryEngine
// @return IRQSongHistoryEngine *
// ---------------------------------------------------------------------------
//
EXPORT_C IRQSongHistoryEngine* IRQSongHistoryEngine::openInstance()
{
    // Get singleton instance
    IRQSongHistoryEngine* irqSongHistoryEngine =
                           reinterpret_cast<IRQSongHistoryEngine*>(Dll::Tls());

    if (NULL == irqSongHistoryEngine)
    {
        TRAPD(error, irqSongHistoryEngine = createInstanceL());
        if (KErrNone != error)
        {
            delete irqSongHistoryEngine;
            irqSongHistoryEngine = NULL;
            Dll::SetTls(NULL);
        }
    }
    else
    {
        irqSongHistoryEngine->iSingletonInstances++;
    }

    return irqSongHistoryEngine;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::closeInstance()
// Close a singleton instance of IRQSongHistoryEngine
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSongHistoryEngine::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::createInstanceL()
// Creates IRQSongHistoryEngine instance
// @return IRQSongHistoryEngine*
// ---------------------------------------------------------------------------
//
IRQSongHistoryEngine* IRQSongHistoryEngine::createInstanceL()
{
    IRQSongHistoryEngine* irqSongHistoryEngine = new (ELeave) IRQSongHistoryEngine();
    irqSongHistoryEngine->constructL();
    User::LeaveIfError(Dll::SetTls(irqSongHistoryEngine));
    irqSongHistoryEngine->iSingletonInstances = 1;

    return irqSongHistoryEngine;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::constructL()
// Two-Phase Constructor.
// ---------------------------------------------------------------------------
//
void IRQSongHistoryEngine::constructL()
{
    iSongHistoryDb = CIRSongHistoryDb::NewL();
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::IRQSongHistoryEngine()
// standard C++ Constructor
// ---------------------------------------------------------------------------
//
IRQSongHistoryEngine::IRQSongHistoryEngine()
{
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::~IRQSongHistoryEngine()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
IRQSongHistoryEngine::~IRQSongHistoryEngine()
{
    delete iSongHistoryDb;
    iSongHistoryDb = NULL;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::handleMetaDataReceivedL()
// Invoked when channel name or song & artist name are changed.
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSongHistoryEngine::handleMetaDataReceived(const IRQMetaData& aMetaData,
        const IRQPreset& aPreset)
{
    TRAPD(error, handleMetaDataReceivedL(aMetaData, aPreset));
    if (KErrNone != error)
    {
        
    }
}

EXPORT_C void IRQSongHistoryEngine::handleSongMetaDataReceived(const IRQMetaData& aMetaData,
        QString& aMusicshopStatus)
{
    TRAPD(error, handleSongMetaDataReceivedL(aMetaData,aMusicshopStatus));
    if (KErrNone != error)
    {        
    }
}

EXPORT_C void IRQSongHistoryEngine::handleSongMetaDataReceivedL(const IRQMetaData& aMetaData,
        QString& aMusicshopStatus)
{
    TPtrC songName(reinterpret_cast<const TUint16*>(aMetaData.getSongName().utf16()), aMetaData.getSongName().length());
    TPtrC artistName(reinterpret_cast<const TUint16*>(aMetaData.getArtistName().utf16()), aMetaData.getArtistName().length());    
    TPtrC musicFlag(reinterpret_cast<const TUint16*>(aMusicshopStatus.utf16()), aMusicshopStatus.length());    
    TBool bExceededMax = iSongHistoryDb->AddToSongHistoryDb2L(songName,
                             artistName,
                             musicFlag);                            
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::handleMetaDataReceivedL()
// ---------------------------------------------------------------------------
//
void IRQSongHistoryEngine::handleMetaDataReceivedL(const IRQMetaData& aMetaData,
        const IRQPreset& aPreset)
{
    // Read Channel properties
    TPtrC songName(reinterpret_cast<const TUint16*>(aMetaData.getSongName().utf16()), aMetaData.getSongName().length());
    TPtrC artistName(reinterpret_cast<const TUint16*>(aMetaData.getArtistName().utf16()), aMetaData.getArtistName().length());
    TPtrC channelUrl(reinterpret_cast<const TUint16*>(aMetaData.getStreamUrl().utf16()), aMetaData.getStreamUrl().length());
    TPtrC channelName(reinterpret_cast<const TUint16*>(aPreset.name.utf16()), aPreset.name.length());
    TPtrC channelDesc(reinterpret_cast<const TUint16*>(aPreset.description.utf16()), aPreset.description.length());
    TPtrC imageUrl(reinterpret_cast<const TUint16*>(aPreset.imgUrl.utf16()), aPreset.imgUrl.length());
    TPtrC musicFlag(reinterpret_cast<const TUint16*>(aPreset.musicStoreStatus.utf16()), aPreset.musicStoreStatus.length());
    TUint8 channelType = aPreset.type;
    TUint16 channelId = aPreset.presetId;
    TUint16 bitrate = aMetaData.getBitrate();

    if (channelType == 1)
    {
        TInt ret = iSongHistoryDb->GetIdPresentInDbL( channelId );
        if(ret)
        {
            TInt modified = iSongHistoryDb->UpdateSongHistoryDbL( channelId ,
                       channelName ,channelUrl ,imageUrl ,musicFlag);   
            // TODO notify uplayer to change layout?
        }
    }
    else if (channelType != 0)
    {
        // the channel type can only be 1 (isds channel) or 0 (user defined channel)
        channelType = 0;
    }
    
    iSongHistoryDb->SearchAndDeleteRecordL(channelName, channelUrl, channelType);
    
	RBuf delSongName;
	delSongName.CleanupClosePushL();
	RBuf delArtistName;
	delArtistName.CleanupClosePushL();
	RBuf delChannelName;
	delChannelName.CleanupClosePushL();
	RBuf delChannelUrl;
	delChannelUrl.CleanupClosePushL();
    TBool bExceededMax = iSongHistoryDb->AddToSongHistoryDbL(songName,
                         artistName,
                         channelName,
                         channelUrl,
                         delSongName,
                         delArtistName,
                         delChannelName,
                         delChannelUrl ,
                         channelType,
                         channelId,
                         bitrate,
                         channelDesc,
                         imageUrl,
                         musicFlag);

    CleanupStack::PopAndDestroy(4, &delSongName);
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::detectSongHistoryItemChangesL()
// To detect any change in the song history items.
// ---------------------------------------------------------------------------
//
IRQSongHistoryEngine::TSongHistoryItemChange IRQSongHistoryEngine::detectSongHistoryItemChangesL(
        const IRQMetaData& aMetaData, const QString& aChannelName)
{
	if(aChannelName.length() == 0)
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

	iSongHistoryDb->GetLastHistoryItemL(song, artist, channel, url);

	TSongHistoryItemChange retval = ENOCHANGE;

    QString tmpsong = QString::fromUtf16(song.Ptr(), song.Length());
    QString tmpartist = QString::fromUtf16(artist.Ptr(), artist.Length());
    QString tmpchannel = QString::fromUtf16(channel.Ptr(), channel.Length());
    QString tmpurl = QString::fromUtf16(url.Ptr(), url.Length());
	if (tmpsong == aMetaData.getSongName() && tmpartist == aMetaData.getArtistName()
	       && tmpchannel == aChannelName && tmpurl == aMetaData.getStreamUrl())
	{
		retval = ENOCHANGE;
	}
	else
	{
		retval = ECHANNELCHANGED;
	}

    CleanupStack::PopAndDestroy(4, &song);

	return retval;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::getAllHistory
// Invoked when when UI request.
// Getting the data from SongHistoryDb.
// ---------------------------------------------------------------------------

EXPORT_C void IRQSongHistoryEngine::getAllHistory(QList<IRQSongHistoryInfo *>& aSongHistoryArr)
{
    TRAP_IGNORE(getAllHistoryL(aSongHistoryArr));
}

EXPORT_C void IRQSongHistoryEngine::getAllSongHistory(QList<IRQSongInfo *>& aSongHistoryArr)
{
    TRAP_IGNORE(getAllSongHistoryL(aSongHistoryArr));
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::getAllHistoryL
// Invoked when when UI request.
// Getting the data from SongHistoryDb.
// ---------------------------------------------------------------------------

void IRQSongHistoryEngine::getAllHistoryL(QList<IRQSongHistoryInfo *>& aSongHistoryArr)
{
    // clear the array
    while (!aSongHistoryArr.isEmpty())
    {
        delete aSongHistoryArr.takeFirst();
    }

    int songCount = iSongHistoryDb->CountSongHistoryDb();

    if (songCount == 0)
    {
        return;
    }

    // get song history list
    RPointerArray<CIRSongHistoryInfo> historyDataArr;
    for (int arrCount = 0 ; arrCount < songCount ; arrCount++ )
    {
        CIRSongHistoryInfo *songHistory = CIRSongHistoryInfo::NewL();
        CleanupStack::PushL(songHistory);
        historyDataArr.AppendL(songHistory);
        CleanupStack::Pop(songHistory);
    }

    iSongHistoryDb->GetAllSongHistoryListL(historyDataArr);

    // create IRQSongHistoryInfo array and set
    QString songName;
    QString artist;
    QString streamUrl;
    QString channelName;
    int channelType;
    int channelId;
    int bitrate;
    QString channelDesc;
    QString imageUrl;
    QString musicStoreStatus;
    for (int arrCount = 0 ; arrCount < songCount ; arrCount++ )
    {
        songName = QString::fromUtf16(historyDataArr[arrCount]->GetSongInfo().Ptr(),
                             historyDataArr[arrCount]->GetSongInfo().Length());
        artist = QString::fromUtf16(historyDataArr[arrCount]->GetArtistInfo().Ptr(),
                             historyDataArr[arrCount]->GetArtistInfo().Length());
        streamUrl = QString::fromUtf16(historyDataArr[arrCount]->GetStreamUrl().Ptr(),
                             historyDataArr[arrCount]->GetStreamUrl().Length());
        channelName = QString::fromUtf16(historyDataArr[arrCount]->GetChannelName().Ptr(),
                             historyDataArr[arrCount]->GetChannelName().Length());
        channelType = historyDataArr[arrCount]->GetChannelType();
        channelId = historyDataArr[arrCount]->GetChannelId();
        bitrate = historyDataArr[arrCount]->GetBitrate();
        channelDesc = QString::fromUtf16(historyDataArr[arrCount]->GetChannelDesc().Ptr(),
                             historyDataArr[arrCount]->GetChannelDesc().Length());
        imageUrl = QString::fromUtf16(historyDataArr[arrCount]->GetImageUrl().Ptr(),
                                     historyDataArr[arrCount]->GetImageUrl().Length());
        musicStoreStatus = QString::fromUtf16(historyDataArr[arrCount]->GetChannelMusicStatus().Ptr(),
                             historyDataArr[arrCount]->GetChannelMusicStatus().Length());
       IRQSongHistoryInfo* irqsongHistory = new IRQSongHistoryInfo();
       if (NULL == irqsongHistory)
       {
           break;
       }
       irqsongHistory->setHistoryInfo(artist, songName, streamUrl, channelName, channelDesc 
                                      , imageUrl, musicStoreStatus, channelType, channelId, bitrate);
       aSongHistoryArr.append(irqsongHistory);
    }
    
    historyDataArr.ResetAndDestroy();
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::getAllHistoryL
// Invoked when when UI request.
// Getting the data from SongHistoryDb.
// ---------------------------------------------------------------------------

void IRQSongHistoryEngine::getAllSongHistoryL(QList<IRQSongInfo *>& aSongHistoryArr)
{
    // clear the array
    while (!aSongHistoryArr.isEmpty())
    {
        delete aSongHistoryArr.takeFirst();
    }

    int songCount = iSongHistoryDb->CountSongHistoryDb2();

    if (songCount == 0)
    {
        return;
    }

    // get song history list
    RPointerArray<CIRSongHistoryInfo> historyDataArr;
    for (int arrCount = 0 ; arrCount < songCount ; arrCount++ )
    {
        CIRSongHistoryInfo *songHistory = CIRSongHistoryInfo::NewL();
        CleanupStack::PushL(songHistory);
        historyDataArr.AppendL(songHistory);
        CleanupStack::Pop(songHistory);
    }

    iSongHistoryDb->GetAllSongHistoryList2L(historyDataArr);

    // create IRQSongHistoryInfo array and set
    QString songName;
    QString artist;     
    QString musicStoreStatus;
    
    for (int arrCount = 0; arrCount < songCount; arrCount++)
    {
        songName = QString::fromUtf16(
                historyDataArr[arrCount]->GetSongInfo().Ptr(),
                historyDataArr[arrCount]->GetSongInfo().Length());
        artist = QString::fromUtf16(
                historyDataArr[arrCount]->GetArtistInfo().Ptr(),
                historyDataArr[arrCount]->GetArtistInfo().Length());
        musicStoreStatus = QString::fromUtf16(
                historyDataArr[arrCount]->GetChannelMusicStatus().Ptr(),
                historyDataArr[arrCount]->GetChannelMusicStatus().Length());
        IRQSongInfo* irqsong = new IRQSongInfo();

        irqsong->setAllInfo(songName, artist, musicStoreStatus);
        aSongHistoryArr.append(irqsong);
    }
    
    historyDataArr.ResetAndDestroy();
}
// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::clearAllHistory
// Invoked when when UI request.
// Clears the SongHistoryDb.
// ---------------------------------------------------------------------------

 EXPORT_C int IRQSongHistoryEngine::clearAllHistory( )
 {
     return iSongHistoryDb->ClearAllSongHistoryFromDb();
 }
 
 EXPORT_C int IRQSongHistoryEngine::clearAllSongHistory( )
 {
     return iSongHistoryDb->ClearAllSongHistoryFromDb2();
 }

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::syncHistory
// Invoked when when UI request.
// Synchronises the Song history DB with the new data in the ISDS.
// ---------------------------------------------------------------------------
EXPORT_C void IRQSongHistoryEngine::syncHistory(int aChannelId)
{
    TRAPD(err,iSongHistoryDb->SyncSongHistoryDbL(aChannelId));
    if (err)
    {
    }
}

// ---------------------------------------------------------------------------
// IRQSongHistoryEngine::updateSongHistoryDb
// Invoked when when UI request.
// Synchronises the Song history DB with the new data in the ISDS.
// ---------------------------------------------------------------------------
EXPORT_C void IRQSongHistoryEngine::updateSongHistoryDb(int aChannelId,
        const QString& aChannelName,
        const QString& aChannelUrl,
        const QString& aImageUrl,
        const QString& aMusicFlag)
{
    TInt ret = KErrNone ;
    TUint16 channelId = aChannelId; // prevent data overflow

    TRAPD(err,ret = iSongHistoryDb->GetIdPresentInDbL( channelId ));

    if (err)
    {
        return;
    }

    if (ret)
    {
        TPtrC channelName(reinterpret_cast<const TUint16*>(aChannelName.utf16()));
        TPtrC channelUrl(reinterpret_cast<const TUint16*>(aChannelUrl.utf16()));
        TPtrC imageUrl(reinterpret_cast<const TUint16*>(aImageUrl.utf16()));
        TPtrC musicFlag(reinterpret_cast<const TUint16*>(aMusicFlag.utf16()));
        TRAP_IGNORE(iSongHistoryDb->UpdateSongHistoryDbL( channelId ,
                channelName,
                channelUrl,
                imageUrl,
                musicFlag));
    }
}

EXPORT_C bool IRQSongHistoryEngine::deleteOneItem(int aIndex)
{    
    int retValue = 0;
    retValue = iSongHistoryDb->DeleteOneHistory(aIndex);
    if( KErrNone != retValue )
    {
        return false;
    }
    
    return true;
}
