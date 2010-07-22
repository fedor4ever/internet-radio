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
#include <mmf/common/mmferrors.h>
#include <NokiaAudioPreference.h>
#include <coedef.h>
#include <QStringList>
#include <hxmetadatakeys.h>
#include "irqmetadata.h"
#include "irqenums.h"
#include "irqmmfadapter.h"
#include "irqlogger.h"

//Constants
const TUid KUidController        = { 0x101F8514 }; // Helix Video controller UID
const TInt KConnectingTime       = 30*1000000;     // 30 seconds
const TInt KVolumeMinPercentage  = 0;              // Minimum volume percentage
const TInt KVolumeMaxPercentage  = 100;            // Maximum volume percentage
const TInt KLoadingCompletePercentage = 100;       // Loading Complete percentage

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::IRQMMFAdapter
//  Constructor
//  Initialize viriants
// ---------------------------------------------------------------------------
//
IRQMMFAdapter::IRQMMFAdapter() :
    iVideoPlayer(NULL)
    ,iQMetaData(NULL)
    ,iPrepareTimer(NULL)
{
    iPlayState = EStopped;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::~IRQMMFAdapter
//  Destructor
//  Clean up resources
// ---------------------------------------------------------------------------
//
IRQMMFAdapter::~IRQMMFAdapter()
{
    destroyPlayer();

    delete iQMetaData;
    iQMetaData = NULL;

    if (iPrepareTimer)
    {
        if (iPrepareTimer->IsActive())
        {
            iPrepareTimer->Cancel();
        }
        delete iPrepareTimer;
        iPrepareTimer = NULL;
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::playStation
//  IRQPlayerAdapterInterface method
//  Play url via specific access point id
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::playStation(const QString &aUrl, int aApId)
{
    LOG_METHOD;
    TRAPD(error, playL(aUrl, aApId));
    if (NULL == iQMetaData)
    {
        emit errorOccured(EIRQErrorOutOfMemory);
    }

    if (KErrNone != error)
    {
        emit errorOccured(EIRQPlayerErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::playL
//  Play url via specific access point id
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::playL(const QString &aUrl, int aApId)
{
    // Save stream Url
    if (NULL == iQMetaData)
    {
        iQMetaData = new (ELeave) IRQMetaData();
    }
    else
    {
        // Clear MetaData
        iQMetaData->clear();
    }
    iQMetaData->setStreamUrl(aUrl);

    // Transfer url from QString to TDesC
    TPtrC stationUrl(reinterpret_cast<const TUint16*>(aUrl.utf16()));

    // Create player if it doesn't exist
    if (NULL == iVideoPlayer)
    {
        createPlayerL();
    }

    // If the status is not stopped, clean up last playback resources
    stop();

    // Open url
    iVideoPlayer->OpenUrlL(stationUrl, aApId, KNullDesC8, KUidController);
    iPlayState = EOpenning;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::stop
//  IRQPlayerAdapterInterface method
//  Stop playback, call Close() to clean up allocated resources
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::stop()
{
    if (iVideoPlayer && EStopped != iPlayState)
    {
        if (iPrepareTimer)
        {
            if (iPrepareTimer->IsActive())
            {
                iPrepareTimer->Cancel();
            }
        }

        iVideoPlayer->Stop();
        iVideoPlayer->Close();
        iPlayState = EStopped;
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::setVolume
//  IRQPlayerAdapterInterface method
//  Set volume to player
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::setVolume(int aVolume)
{
    LOG_METHOD;
    if (iVideoPlayer && iPlayState > EOpenning)
    {
        // aVolume is a percentage
        if (aVolume < KVolumeMinPercentage)
        {
            aVolume = KVolumeMinPercentage;
        }
        else if (aVolume > KVolumeMaxPercentage)
        {
            aVolume = KVolumeMaxPercentage;
        }
        int volume = aVolume*iVideoPlayer->MaxVolume()/KVolumeMaxPercentage;

        TRAPD(error, iVideoPlayer->SetVolumeL(volume));
        if (KErrNone != error)
        {
            emit errorOccured(EIRQPlayerErrorGeneral);
        }
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::getVolume
//  IRQPlayerAdapterInterface method
//  Get current volume value from player
// ---------------------------------------------------------------------------
//
int IRQMMFAdapter::getVolume()
{
    int volume = KVolumeMinPercentage;

    if (iVideoPlayer && iPlayState > EOpenning)
    {
        // Return a percentage
        volume = iVideoPlayer->Volume()*KVolumeMaxPercentage/iVideoPlayer->MaxVolume();
    }
    return volume;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::getPlayerInstance
//  IRQPlayerAdapterInterface method
//  Get audio player instance
// ---------------------------------------------------------------------------
//
void* IRQMMFAdapter::getPlayerInstance()
{
    return (void*)iVideoPlayer;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoOpenComplete
//  Callback function, MVideoPlayerUtilityObserver method
//  Called after calling CVideoPlayerUtility::OpenUrlL()
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoOpenComplete(TInt aError)
{
    LOG_METHOD;
    if (KErrNone == aError)
    {
        if (NULL == iPrepareTimer)
        {
            TRAPD(error, iPrepareTimer = CPeriodic::NewL(CPeriodic::EPriorityStandard));
            if (KErrNone != error)
            {
                emit errorOccured(EIRQErrorOutOfMemory);
                return;
             }
        }

        // Prepare to playback
        iVideoPlayer->Prepare();
        iPlayState = EConnecting;

        // Start a timer to check preparation status
        if (iPrepareTimer->IsActive())
        {
            // Cancel the previous request if pending
            iPrepareTimer->Cancel();
        }
        TTimeIntervalMicroSeconds32 interval(KConnectingTime);
        iPrepareTimer->Start(interval,interval,
                             TCallBack(IRQMMFAdapter::isPrepareCompleted,this));
    }
    else
    {
        emit errorOccured(EIRQPlayerErrorConnectingFailed);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoPrepareComplete
//  Callback function, MVideoPlayerUtilityObserver method
//  Called after calling CVideoPlayerUtility::Prepare. Since some audio types
//  are not explicitly(hxmmffourccmap.cpp), they are not retrieved here.
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoPrepareComplete(TInt aError)
{
    LOG_METHOD;
    // Cancel the previous request if pending
    if (iPrepareTimer->IsActive())
    {
        iPrepareTimer->Cancel();
    }

    if (KErrNone == aError)
    {
        // Get volume from preset
        int volumeval = KVolumeMaxPercentage/2;
        emit volumeExpected(volumeval);
        setVolume(volumeval);

        // Save bit rate
        int bitrate = 0;
        TRAPD(error, bitrate = iVideoPlayer->AudioBitRateL());
        if (KErrNone == error)
        {
            iQMetaData->setBitrate(bitrate/1000);
        }

        // Send signal ConnectionEstablished
        emit connectionEstablished(iQMetaData->getBitrate());

        // Set specific event to get meta data from player
        setMetadataEventConfig();

        // Start playback
        iVideoPlayer->Play();
        iPlayState = EBuffering;
    }
    else if (KErrServerBusy == aError)
    {
        emit errorOccured(EIRQPlayerErrorServerFull);
    }
    else
    {
        emit errorOccured(EIRQPlayerErrorConnectingFailed);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoPlayComplete
//  Callback function, MVideoPlayerUtilityObserver method
//  Notification that video playback has completed. This is not called if play
//  back is explicitly stopped by calling Stop. Moreover, radio station stream
//  has no end. So it should be NEVER called.
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoPlayComplete(TInt aError)
{
    LOG_METHOD;
    if (KErrNone != aError)
    {
        emit errorOccured(EIRQPlayerErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoEvent
//  Callback function, MVideoPlayerUtilityObserver method
//  Handle events from player.
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoEvent(TMMFEvent const & aEvent)
{
    LOG_METHOD;
    LOG_FORMAT( "aevent is %d", (int)aEvent);
    if (KMMFEventCategoryVideoPlayerGeneralError == aEvent.iEventType)
    {
        switch (aEvent.iErrorCode)
        {
            case KErrHardwareNotAvailable:
            case KErrMMAudioDevice:
                // Higher priority application has taken over the
                // audio device. --> Do stop.
                emit errorOccured(EIRQPlayerErrorAudioDeviceLost);
                break;
            case KErrDisconnected:
                emit errorOccured(EIRQPlayerErrorConnectionLost);
                break;
            case KErrTimedOut:    
                emit errorOccured(EIRQPlayerErrorTimeOut);
                break;
            case KErrServerBusy:    
                emit errorOccured(EIRQPlayerErrorServerFull);
                break;                                            
            default:
                emit errorOccured(EIRQPlayerErrorGeneral);			
                break;
        }
    }
    else if (KMMFRefreshMetaData == aEvent.iEventType)
    {
        // Get refreshed meta data
        TRAPD(error, getRefreshedMetaDataL(aEvent.iErrorCode));
        if (KErrNone != error)
        {
            emit errorOccured(EIRQPlayerErrorGeneral);
        }
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvpuoFrameReady
//  Callback function, MVideoPlayerUtilityObserver method
//  For video stream only, never called
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvloLoadingStarted
//  Callback function, MVideoLoadingObserver method
//  Start buffering after CVideoPlayerUtility::Play() is called
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvloLoadingStarted()
{
    LOG_METHOD;
    // Get buffering progress and send it to application
    int percentageComplete = 0;

    TRAPD(error, iVideoPlayer->GetVideoLoadingProgressL(percentageComplete));

    if (KErrNone == error)
    {
        // Send signal to UpdateProgress
        emit percentageBuffered(percentageComplete);
    }
    else
    {
        emit errorOccured(EIRQPlayerErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::MvloLoadingComplete
//  Callback function, MVideoLoadingObserver method
//  Send 100% buffering status out
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::MvloLoadingComplete()
{
    LOG_METHOD;
    iPlayState = EPlaying;

    // Send signal to update progress, 100%
    emit percentageBuffered(KLoadingCompletePercentage);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::getRefreshedMetaData
//  Get refreshed meta data according to the index
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::getRefreshedMetaDataL(TInt index)
{
    LOG_METHOD;
    if (iQMetaData)
    {
        CMMFMetaDataEntry* pMetadataEntry = iVideoPlayer->MetaDataEntryL(index);

        QString entryName = QString::fromUtf16(pMetadataEntry->Name().Ptr(),
                                               pMetadataEntry->Name().Length());

        // If the meta data is the same as last, we don't need to report it.
        if (iLastArtistSongName == entryName)
        {
            return;
        }
        else
        {
            iLastArtistSongName = entryName;
        }

        // Artist, song name
        if (entryName == HXAuthor)
        {
            QString songArtist = QString::fromUtf16(pMetadataEntry->Value().Ptr(),
                                                    pMetadataEntry->Value().Length());
            iQMetaData->setArtistSongName(songArtist);

            // Send signal HandleMetaDataReceived
            emit metaDataReceived(*iQMetaData);
        }
    }
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::createPlayer
//  Create player instance
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::createPlayerL()
{
    // Create player instance
    iVideoPlayer = CVideoPlayerUtility2::NewL(*this,KAudioPriorityAudioPlaybackStreaming ,
                                             (TMdaPriorityPreference)KAudioPrefRealOneStreaming);
    // Register loading notification
    iVideoPlayer->RegisterForVideoLoadingNotification(*this);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::destroyPlayer
//  Destroy player
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::destroyPlayer()
{
    delete iVideoPlayer;
    iVideoPlayer = NULL;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::SetMetadataEventConfig
//  Enable meta data event
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::setMetadataEventConfig()
{
    TMMFMessageDestinationPckg  destinationPckg(KUidInterfaceMMFControllerMetadataEventMsg);
    TPckgBuf<TBool>             metadataEventPckg(EMMFEnableMetadataEvent);

    //  Enable meta data event.
    iVideoPlayer->CustomCommandSync(destinationPckg,
                                    EMMFSetMetadataEventConfig,
                                    metadataEventPckg,
                                    KNullDesC8);
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::isPrepareCompleted
//  Static function for periodic call
// ---------------------------------------------------------------------------
//
TInt IRQMMFAdapter::isPrepareCompleted(TAny* aPtr)
{
    IRQMMFAdapter* self = static_cast<IRQMMFAdapter*>(aPtr);
    if (self)
    {
        self->checkPrepare();
    }
    return KErrNone;
}

// ---------------------------------------------------------------------------
//  IRQMMFAdapter::checkPrepare
//  Check if the preparation is complete
// ---------------------------------------------------------------------------
//
void IRQMMFAdapter::checkPrepare()
{
    LOG_METHOD;
    if (iPrepareTimer->IsActive())
    {
        // Cancel the previous request if pending
        iPrepareTimer->Cancel();
    }

    if (EConnecting == iPlayState)
    {
        emit errorOccured(EIRQPlayerErrorConnectingFailed);
        stop();
    }
}
