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
#include <hbmessagebox.h>
#include <QTimer>
#ifdef Q_CC_NOKIAX86
#include <QFile>
#include <QTextStream>
#endif
#ifdef HS_WIDGET_ENABLED
#include <QSettings>
#endif

#include "irplaycontroller.h"
#include "irapplication.h"
#include "irqmediaplayer.h"
#include "irviewmanager.h"
#include "irqisdsdatastructure.h"
#include "irlastplayedstationinfo.h"
#include "irqnetworkcontroller.h" 
#include "irqsonghistoryengine.h"
#include "irqmetadata.h"
#include "irqsettings.h"
#include "irqfavoritesdb.h"
#include "irqstatisticsreporter.h"
#include "irenummapper.h" 
#include "irqlogger.h"
#ifdef HS_WIDGET_ENABLED
#include "irservicedef.h"
#endif

#ifdef Q_CC_NOKIAX86
void getRadioServerAddress(QString & aUrl);
#endif

BEGIN_ENUM_MAP( KTerminationTypeMap )
    ENUM_MAP_ITEM( EIRQUserTerminated, IRQStatisticsReporter::EIRUserTerminated ),
    ENUM_MAP_ITEM( EIRQNoConnectionToServer, IRQStatisticsReporter::EIRNoConnToServer ),
    ENUM_MAP_ITEM( EIRQNoConnectionToNetwork, IRQStatisticsReporter::EIRNoConnToNetwork )            
END_ENUM_MAP( KTerminationTypeMap )

#define MAP_TO_ENGINE_TerminationType(ui_enum) \
          MAP_TO_ENGINE_ENUM( IRQTerminatedType, \
             IRQStatisticsReporter::IRTerminatedType, \
             ui_enum, KTerminationTypeMap )
    
BEGIN_ENUM_MAP( KServerResultMap )
    ENUM_MAP_ITEM( EIRQPlayerErrorServerFull, IRQStatisticsReporter::EIRServerFull ),
    ENUM_MAP_ITEM( EIRQPlayerErrorTimeOut, IRQStatisticsReporter::EIRTimeOut ),
    ENUM_MAP_ITEM( EIRQPlayerErrorConnectingFailed, IRQStatisticsReporter::EIRConnectFailed )              
END_ENUM_MAP( KServerResultMap )

#define MAP_TO_ENGINE_ServerResult(ui_enum) \
          MAP_TO_ENGINE_ENUM( IRQError, \
             IRQStatisticsReporter::IRServerResult, \
             ui_enum, KServerResultMap )
             
BEGIN_ENUM_MAP( KConnectedFromMap )
    ENUM_MAP_ITEM( EIRQIsds, IRQStatisticsReporter::EIRIsds ),
    ENUM_MAP_ITEM( EIRQPresetAdhoc, IRQStatisticsReporter::EIRPresetAdhoc ),
    ENUM_MAP_ITEM( EIRQPresetIsds, IRQStatisticsReporter::EIRPresetIsds ),
    ENUM_MAP_ITEM( EIRQHistoryAdhoc, IRQStatisticsReporter::EIRHistoryAdhoc ),
    ENUM_MAP_ITEM( EIRQHistoryIsds, IRQStatisticsReporter::EIRHistoryIsds ),
    ENUM_MAP_ITEM( EIRQAdhocManual, IRQStatisticsReporter::EIRAdhocManual ),                
    ENUM_MAP_ITEM( EIRQAdhocExternal, IRQStatisticsReporter::EIRAdhocExternal )            
END_ENUM_MAP( KConnectedFromMap )

#define MAP_TO_ENGINE_ConnectedFrom(ui_enum) \
          MAP_TO_ENGINE_ENUM( IRQConnectedFrom, \
             IRQStatisticsReporter::IRConnectedFrom, \
             ui_enum, KConnectedFromMap )                 


static bool loadStationLogoFlag();
static void saveStationLogoFlag(bool aIsStationLogoAvailable);

//                                        public functions

/*
 * Description : constructor
 */
IRPlayController::IRPlayController(IRApplication* aApplication) :
    iApplication(aApplication),
    iMediaPlayer(new IRQMediaPlayer()),
    iStatisticsReporter(NULL),
    iGetServerResult(false),
    iNowPlayingPreset(new IRQPreset()),
    iNowPlayingPresetBackup(new IRQPreset()),
    iConnectedFrom(EIRQIsds),
    iConnectedFromBackup(EIRQIsds),
    iStationLogoAvailable(false),
    iStationLogoAvailableBackup(false),
    iMetaData(NULL),
    iSongHistoryEngine(NULL),
    iPlayState(EIdle),
    iResuming(false),
    iTryingBitrate(0),
    iUrlArray(0),
    iRealBitrate(0),
    iLastError(EIRQErrorNone),
    iStopReason(EIRQUnknownTermination),
    iErrorNote(NULL)
{
    // use the last played station to initiliaze the backup value.
    // can regard the player is bootup, and initilize its LCD screen with last played station info if available.
    IRQPreset *lastPlayedPreset = NULL;
    if( !iApplication->isEmbeddedInstance() )
    {
        lastPlayedPreset = iApplication->getLastPlayedStationInfo()->getLastPlayedStation();        
    }
    
    if (lastPlayedPreset)
    {
        *iNowPlayingPreset      =   *lastPlayedPreset;
        iLastPlayedUrl          =   getFirstTryUrl(lastPlayedPreset);
        iConnectedFrom          =   iApplication->getLastPlayedStationInfo()->connectedFrom();
        iStationLogoAvailable   =   loadStationLogoFlag();
    }
    
    if (iNowPlayingPreset->getChannelURLCount())
    {
        iPlayState = EStopped;
    }
    else
    {
        iPlayState = EIdle;
    }    
    
    connectSignalSlot(); 
    iStatisticsReporter = IRQStatisticsReporter::openInstance();
	
	if( !iApplication->isEmbeddedInstance() )
    {
        iSongHistoryEngine = IRQSongHistoryEngine::openInstance();
    }
}

/*
 * Description : destructor
 */
IRPlayController::~IRPlayController()
{
    stop(EIRQUserTerminated);
    delete iMediaPlayer;
    iMediaPlayer = NULL;

    delete iNowPlayingPreset;
    iNowPlayingPreset = NULL;
    delete iNowPlayingPresetBackup;
    iNowPlayingPresetBackup = NULL;

    delete iUrlArray;
    iUrlArray = NULL;
    
    delete iErrorNote;
    iErrorNote = NULL;

    saveStationLogoFlag(iStationLogoAvailable);
	
    if (iSongHistoryEngine)
    {
        iSongHistoryEngine->closeInstance();
        iSongHistoryEngine = NULL;
    }
    
    if (iStatisticsReporter)
    {
        iStatisticsReporter->closeInstance();
    }
}

/*
 * Description : connect to a channel specified by aPreset.
 * Parameters  : aPreset : the preset of the channel
 */
void IRPlayController::connectToChannel(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom)
{
    if (!aPreset)
    {
        return;
    }

    if (iMediaPlayer)
    {       
        QString firstTryUrl = getFirstTryUrl(aPreset);
        if (firstTryUrl.isEmpty())
        {
            return;
        }
        
        *iNowPlayingPresetBackup = *iNowPlayingPreset;         
        iLastPlayedUrlBackup = iLastPlayedUrl;
        iConnectedFromBackup = iConnectedFrom;
        
        *iNowPlayingPreset = *aPreset;           
        iLastPlayedUrl = firstTryUrl;
        iConnectedFrom = aConnectedFrom;
        iResuming = false;
        doPlay(iLastPlayedUrl);
    }
}

QString IRPlayController::getFirstTryUrl(IRQPreset *aPreset)
{
    QString firstTryUrl;
    // sort the URL by ascending order and get all available rates.
    // iAvailableBitrate is cleared in getAvailableBitrates().
    aPreset->sortURLArray();
    aPreset->getAvailableBitrates(iAvailableBitrate);
    if (iAvailableBitrate.count() == 0)
    {
        return firstTryUrl;
    }
    
    int selectedBitRate = 0;
    IRQPreferredQuality preferredQuality = iApplication->getSettings()->getPreferredQuality();
    switch(preferredQuality)
    {
        case EIRQStandardQuality:
            selectedBitRate = iAvailableBitrate.first();
            break;
        case EIRQHighQuality:
            selectedBitRate = iAvailableBitrate.last();
            break;
        default:
            selectedBitRate = iAvailableBitrate.first();
            break;
    }

    // get URL to play
    iTryingBitrate = selectedBitRate;
    
    delete iUrlArray;
    iUrlArray = NULL;
    iUrlArray = aPreset->getURLsForBitrate(selectedBitRate);
    if (iUrlArray)
    {
        firstTryUrl = iUrlArray->at(0);
#ifdef Q_CC_NOKIAX86
        firstTryUrl = "http://172.28.205.171:8000";
        getRadioServerAddress(firstTryUrl);
#endif
    }
    
    return firstTryUrl;
}

/*
 * Description : RESUME playing after play is stopped.
 *               Use the last played url as station's url.
 */
void IRPlayController::resume()
{
    qDebug("IRPlayController::resume(), Entering");
    if (iMediaPlayer && (EStopped == iPlayState))
    {
        iResuming = true;
        
        if (iLastPlayedUrl != "")
        {
            qDebug("IRPlayController::resume(), iLastPlayedUrl is not empty, doPlay()");
            doPlay(iLastPlayedUrl);
        }
    }
    qDebug("IRPlayController::resume(), Exiting");
}

/*
 * Description : stop playing
 */
void IRPlayController::stop(IRQTerminatedType aStopReason)
{
    qDebug("IRPlayController::stop, Entering, aStopReason=%d", aStopReason);        
    
    switch (iPlayState)
    {   
        case EPlaying:
            iPlayState = EStopped; 
            iStopReason = aStopReason;
            if (iMediaPlayer)
            {
                iMediaPlayer->disableStereoEffect();
                iMediaPlayer->stop(); 
            }
            // playingStarted is emitted while iPlaying is set to true,
            // so when stop() is called and iPlaying is true, playingStopped
            // should be emitted.
            qDebug("IRPlayController::stop, emit playingStopped()");             
            emit playingStopped();              
            break;

#ifdef HS_WIDGET_ENABLED			
        case EConnecting:     
            iStopReason = aStopReason;
            if (iMediaPlayer)
            {
                iMediaPlayer->disableStereoEffect();
                iMediaPlayer->stop(); 
            }            
            // No need to restore because when connecting occurs, because the connectToChannel() has NOT been invoked yet. 
            // Only need to reset the player state                  
            if (iNowPlayingPreset->getChannelURLCount())
            {
                iPlayState = EStopped;
            }
            else
            {
                iPlayState = EIdle;
            }
            
            // Only need to restore the station logo flag since we may force it to be false when connecting started.   
            // force logo to be default when current view is NOT nowplaying view && is not resuming (start playing a different station.) 
            if (EIRView_PlayingView != iApplication->getViewManager()->currentViewId()
                && iConnectingStationName != iNowPlayingPreset->name)
            {
                iStationLogoAvailable = iStationLogoAvailableBackup;
                emit stationLogoUpdated(iStationLogoAvailable);             
            }                      
            emit connectingCancelled(iNowPlayingPreset->name);
            break;
#endif
            
        case EBuffering:
            if (iMediaPlayer)
            {
                iMediaPlayer->disableStereoEffect();
                iMediaPlayer->stop(); 
            }
            // when player is forced to stop in bufferring state, we have to revert to backup preset.
            // only with exception when current view is nowplaying view.
            // in IR case, user can only stay in nowplaying view when buffering:
            // 1. starging view is now playing view; 2. open a pls file with only one url
            if (EIRView_PlayingView != iApplication->getViewManager()->currentViewId())
            {
                iLastPlayedUrl        = iLastPlayedUrlBackup;
                *iNowPlayingPreset    = *iNowPlayingPresetBackup;
                iConnectedFrom        = iConnectedFromBackup;               
            }
            
            if (iNowPlayingPreset->getChannelURLCount())
            {
                iPlayState = EStopped;
            }
            else
            {
                iPlayState = EIdle;
            }
            
            iStopReason = aStopReason;
                        
#ifdef HS_WIDGET_ENABLED            
            // Need to restore the station logo flag since we may force it to be false when buffering started.  
            // force logo to be default when current view is NOT nowplaying view && is not resuming (start playing a different station.) 
            if (EIRView_PlayingView != iApplication->getViewManager()->currentViewId()
                && !iResuming)
            {
                iStationLogoAvailable = iStationLogoAvailableBackup;
                emit stationLogoUpdated(iStationLogoAvailable);             
            }                         
            emit bufferingCancelled(iNowPlayingPreset->name);
#endif
            break;
		
			
        case EIdle:
        case EStopped:		
        default:              
            break;
    }
	endSession(aStopReason);
	qDebug("IRPlayController::stop, Exiting");
}

/*
 * Description : End Session
 */
void IRPlayController::endSession(IRQTerminatedType aStopReason)
{
    if(iStatisticsReporter)
    {
        if(!iGetServerResult)
        {
            iStatisticsReporter->logServerResult(iLastPlayedUrl,IRQStatisticsReporter::EIRConnectFailed);
        }
        iStatisticsReporter->sessionEnded(MAP_TO_ENGINE_TerminationType(aStopReason));
    }
}

/*
 * Description : get current volume setting from media player or
 *               central repository
 * Return      : current volume
 */
int IRPlayController::getVolume() const
{
    if (EPlaying == iPlayState)
    {
        return iMediaPlayer->getVolume();
    }
    else
    {
        return iApplication->getSettings()->getVolumeSetting();
    }
}

/*
 * Description : set volume to media player and central repository
 * Parameters  : aVolume : volume value, between 0 and 100
 */
void IRPlayController::setVolume(int aVolume)
{
    if (EPlaying == iPlayState)
    {
        iMediaPlayer->setVolume(aVolume);
    }
    iApplication->getSettings()->setVolumeSetting(aVolume);
}

#ifdef HS_WIDGET_ENABLED
void IRPlayController::setConnectingStationName(const QString &aStationName, bool aForceConnecting)
{
    iConnectingStationName = aStationName;
    if (aForceConnecting
         || !iApplication->getNetworkController()->getNetworkStatus())
    {
        LOG_FORMAT("IRPlayController::setConnectingStationName, the station name is %s", STRING2CHAR(aStationName));
        iPlayState = EConnecting;   
        
        // force logo to be default when current view is NOT nowplaying view && is not resuming (start playing a different station.) 
        if (EIRView_PlayingView != iApplication->getViewManager()->currentViewId()
            && iConnectingStationName != iNowPlayingPreset->name)
        {
            emitStationLogoUpdated(false);
        }        
        emit connectingStarted(aStationName);
    }
}

QString IRPlayController::getConnectingStationName() const
{
    return iConnectingStationName;
}

void IRPlayController::reloadNowplayingPreset(IRQPreset *aPreset, bool aIsLogoAvailable, IRQConnectedFrom aConnectedFrom)
{
    if (aPreset)
    {
        *iNowPlayingPreset      =   *aPreset;
        iLastPlayedUrl          =   getFirstTryUrl(aPreset);
        iConnectedFrom          =   aConnectedFrom;
        iStationLogoAvailable   =   aIsLogoAvailable;
    }
    
    if (iNowPlayingPreset->getChannelURLCount())
    {
        iPlayState = EStopped;
    }
    else
    {
        iPlayState = EIdle;
    }     
}

bool IRPlayController::isStationLogoAvailable() const
{
    return iStationLogoAvailable;
}

void IRPlayController::emitStationLogoUpdated(bool aIsLogoAvailable)
{
    iStationLogoAvailableBackup = iStationLogoAvailable;
    iStationLogoAvailable = aIsLogoAvailable;
    emit stationLogoUpdated(iStationLogoAvailable);
}

bool loadStationLogoFlag()
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    return settings.value(KIrSettingStationLogoAvailable,false).toBool();
}

#endif

IRPlayController::EPlayState IRPlayController::state() const
{
    return iPlayState;
}

/*
 * Description : return the flag of playing state
 * Return      : true  : playing is ongoing
 *               false : playing is not ongoing
 */
bool IRPlayController::isPlaying() const
{
    return (EPlaying == iPlayState);
}

/*
 * Description : return the flag of stopped state
 * Return      : true  : playing is stopped
 *               false : playing is not stopped
 */
bool IRPlayController::isStopped() const
{
    return (EStopped == iPlayState);
}

/*
 * Description : return the flag of idle state
 * Return      : true  : playing is idle
 *               false : playing is not idle
 */
bool IRPlayController::isIdle() const
{
    return (EIdle == iPlayState);
}

/*
 * Description : return the now playing preset
 * Return      : pointer to the now playing preset
 */
IRQPreset * IRPlayController::getNowPlayingPreset() const
{
    return iNowPlayingPreset;
}

QString IRPlayController::getNowPlayingUrl() const
{
    return iLastPlayedUrl;
}

int IRPlayController::getNowPlayingBitRate() const
{
    return iRealBitrate;
}

/*
 * Description : return current metadata
 * Return      : pointer to current metadata
 */
IRQMetaData * IRPlayController::getMetaData() const
{
    return iMetaData;
}

IRQTerminatedType IRPlayController::getStopReason() const
{
    return iStopReason;
}

//                                           slot  functions

/*
 * Description : data connection has been established. Signal is emitted by media player.
 * 
 */
void IRPlayController::connectionEstablished(int aBitrate)
{
    if(iStatisticsReporter)
    {
        iStatisticsReporter->logServerResult(iLastPlayedUrl,IRQStatisticsReporter::EIRConnected);
        iGetServerResult = true;
        iStatisticsReporter->markSessionStart();
    }

    iMetaData = NULL;
    iRealBitrate = aBitrate;

    // update bitrate for user defined channel
    if (0 == iNowPlayingPreset->type)
    {
        if (iTryingBitrate != iRealBitrate)
        {
            iNowPlayingPreset->setUrlBitrate(0,iRealBitrate);
            //when bitrate is available, it should be written to favorites db
            iApplication->getFavoritesDB()->replaceUserDefinedPreset(*iNowPlayingPreset);
            iTryingBitrate = iRealBitrate;
        }
    }
}

/*
 * Description : error occurred in media player. Signal is emitted by media player.
 * Parameters  : aError : error value
 */
void IRPlayController::errorOccured(IRQError aError)
{
    LOG_METHOD;
    LOG_FORMAT("the error is occured %d",aError);
    iLastError = aError;

    QTimer::singleShot(1, this, SLOT(handleError()));
}

/*
 * Description : handle the error async.
 */
void IRPlayController::handleError()
{
    LOG_METHOD;
    LOG_FORMAT("the last error is %d", iLastError);
    qDebug("IRPlayController::handleError(), Entering, iLastError - %d", iLastError);
    switch (iLastError)
    {
    case EIRQPlayerErrorServerFull:
    case EIRQPlayerErrorTimeOut:
    case EIRQPlayerErrorConnectingFailed:
        if(iStatisticsReporter)
        {
            iStatisticsReporter->logServerResult(iLastPlayedUrl,MAP_TO_ENGINE_ServerResult(iLastError));
            iGetServerResult = true;
        }
		
        // if there's NO other URL to try, show warning.
        if (iNowPlayingPreset->getChannelURLCount() == 1)
        {
		    stop(EIRQNoConnectionToServer);
            break;
        }

        if (iResuming)
        {
#ifdef SUBTITLE_STR_BY_LOCID
            HbMessageBox::information(hbTrId("txt_irad_connecting_failed_try_next_address"), (QObject*)NULL, NULL, HbMessageBox::Ok);
#else
            HbMessageBox::information("Connecting failed, try next URL", (QObject*)NULL, NULL, HbMessageBox::Ok);
#endif
            connectToChannel(iNowPlayingPreset,iConnectedFrom);
            iResuming = false;
            return;
        }

        // try next
        if (true == playNextUrl())
        {
            return;
        }
        else
        {
            stop(EIRQNoConnectionToServer);
        }
        break;

    case EIRQPlayerErrorConnectionLost:
        qDebug("IRPlayController::handleError, connection lost");
        stop(EIRQNoConnectionToServer);
        break;
        
    case EIRQPlayerErrorAudioDeviceLost:
        //this is a temporary way to handle the plug-out event
        iApplication->stopLoadingAnimation();
		stop(EIRQCallIsActivated);
        return;
        
    case EIRQPlayerErrorGeneral:
    default:
        stop(EIRQUnknownTermination);
        break;
    }

    iApplication->stopLoadingAnimation();
    popupNote();
    qDebug("IRPlayController::handleError(), Exiting");
}

/*
 * Description : buffering process has updated, change the display of buffering dialog.
 *               If aProress is 100, buffering is complete, now playing view needs to 
 *               be shown to user.
 * Parameters  : aProgress : progress value, between 0 and 100.
 */
void IRPlayController::updateProgress(int aProgress)
{
    if (100 == aProgress)
    {
        //updateProgress(100) sometimes can be called more than one time, to improve performance,
        //we only need to do the following work once.        
        if (EPlaying == iPlayState)
        {
            iApplication->stopLoadingAnimation();
            return;
        }
        
        iPlayState = EPlaying;
        iApplication->stopLoadingAnimation();

        iApplication->getViewManager()->activateView(EIRView_PlayingView);
        if( !iApplication->isEmbeddedInstance() )
        {
            //update last played station
            IRLastPlayedStationInfo *lastPlayedStationInfo = iApplication->getLastPlayedStationInfo();
            lastPlayedStationInfo->updateLastPlayedStation(iNowPlayingPreset,iConnectedFrom);
            lastPlayedStationInfo->commitLastPlayedStation();
        }        

        //increase the played times of current preset
        iApplication->getFavoritesDB()->increasePlayedTimes(*iNowPlayingPreset);

        emit playingStarted();

        // if the metadata is available, show it.
        emit metaDataAvailable(iMetaData);
        
        if( !iApplication->isEmbeddedInstance() )
        {
            // Save the station information to database
            IRQMetaData tmpMetaData;
            tmpMetaData.setBitrate(iRealBitrate);
            tmpMetaData.setStreamUrl(iLastPlayedUrl);
            iSongHistoryEngine->handleMetaDataReceived(tmpMetaData, *iNowPlayingPreset);            
        }

        // open stereo defaultly
        iMediaPlayer->enableStereoEffect();
    }
}

/*
 * Description : get volume value from application setting. media player use the value
 *               to set volume.
 * Parameters  : aVolume : volume value
 */
void IRPlayController::fetchVolume(int &aVolume)
{
    aVolume = iApplication->getSettings()->getVolumeSetting();
}

/*
 * Description : handle the receiption of metadata. Notify now playing view to update display
 * Parameters  : aIRmetaData : the metadata object
 */
void IRPlayController::handleMetaDataReceived(IRQMetaData& aIRmetaData)
{
    iMetaData = &aIRmetaData;
    //TO DO: there maybe a potential bug when the user cancel the play, 	
    if ((aIRmetaData.getSongName().trimmed() != "")
            || (aIRmetaData.getArtistName().trimmed() != ""))
    {
        //when we are play the musicplayer and get the metadata from lower layer, we save the 
        //song's metadata into the db.  After we save it to db, we emit the next signal to notify the UI         
        iSongHistoryEngine->handleSongMetaDataReceived(*iMetaData,
                *iNowPlayingPreset);  
    }   

    if (EPlaying == iPlayState)
    {
        // This signal will cause addBanner() work. Sometimes the metadata will come before
        // the buffering is 100%, we need to avoid to show playing banner before 100% buffering.
        emit metaDataAvailable(iMetaData);        
    }    
}

/*
 * Description : during buffering stage, cancel playing request
 */
void IRPlayController::cancelBuffering()
{
    stop(EIRQUserTerminated);
    iApplication->stopLoadingAnimation();
}

//                                       private functions

/*
 * Description : show a note to user to inform that error occured.
 *                
 */
void IRPlayController::popupNote(const QString &aNote)
{
    if (NULL == iErrorNote)
    {
        iErrorNote = new HbMessageBox(HbMessageBox::MessageTypeWarning);
        iErrorNote->setModal(true);
        iErrorNote->setTimeout(HbPopup::StandardTimeout);
    }
    
    iErrorNote->setText(aNote);
    // if there is already on error note showing, only change the text 
    if (!iErrorNote->isVisible())
    {
        iErrorNote->show();
    }
}

/*
 * Description : establish the signal&slot connection between media player and play controller
 */
void IRPlayController::connectSignalSlot()
{
    connect(iMediaPlayer, SIGNAL(connectionEstablished(int)), this, SLOT(connectionEstablished(int)));
    connect(iMediaPlayer, SIGNAL(errorOccured(IRQError)), this, SLOT(errorOccured(IRQError)));
    connect(iMediaPlayer, SIGNAL(percentageBuffered(int)), this, SLOT(updateProgress(int)));
    connect(iMediaPlayer, SIGNAL(volumeExpected(int&)), this, SLOT(fetchVolume(int&)));
    
    if( !iApplication->isEmbeddedInstance() )
    {
        connect(iMediaPlayer, SIGNAL(metaDataReceived(IRQMetaData&)),
                this, SLOT(handleMetaDataReceived(IRQMetaData&)));        
    } 
}

/*
 * Description : try to play the next url in the preset.
 * return value: true: it will play next URL; false, not.
 */
bool IRPlayController::playNextUrl()
{
    // remove the first url from iUrlArray
    iUrlArray->removeFirst();

    if (iUrlArray->isEmpty()) // try next bitrate
    {
        int index = iAvailableBitrate.indexOf(iTryingBitrate);
        if (-1 != index)
        {
            bool tryingContinue = true;
            IRQPreferredQuality preferredQuality = iApplication->getSettings()->getPreferredQuality();
            switch(preferredQuality)
            {
                case EIRQHighQuality:
                    if(index > 0)
                    {
                        iTryingBitrate = iAvailableBitrate.at(--index);
                    }
                    else
                    {
                        tryingContinue = false;
                    }
                    break;
                    
                case EIRQStandardQuality:
                default:
                    if(index < (iAvailableBitrate.count()-1))
                    {
                        iTryingBitrate = iAvailableBitrate.at(++index);
                    }
                    else
                    {
                        tryingContinue = false;
                    }
                    break;
            }        
            
            if(tryingContinue)
            {
#ifdef SUBTITLE_STR_BY_LOCID
                HbMessageBox::information(hbTrId("txt_irad_connecting_failed_try_next_address"), (QObject*)NULL, NULL, HbMessageBox::Ok);
#else
                HbMessageBox::information("Connecting failed, try next URL", (QObject*)NULL, NULL, HbMessageBox::Ok);
#endif                 
                delete iUrlArray;
                iUrlArray = iNowPlayingPreset->getURLsForBitrate(iTryingBitrate);
                iLastPlayedUrl = iUrlArray->at(0);
                // Set the status to EStopped because it didn't start to play at all and need not to 
                // do stop action.
                iPlayState = EStopped;
                doPlay(iLastPlayedUrl);
                return true;
            }
        }

    }
    else // try next url in iUrlArray
    {
#ifdef SUBTITLE_STR_BY_LOCID
        HbMessageBox::information(hbTrId("txt_irad_connecting_failed_try_next_address"), (QObject*)NULL, NULL, HbMessageBox::Ok);
#else
        HbMessageBox::information("Connecting failed, try next URL", (QObject*)NULL, NULL, HbMessageBox::Ok);
#endif   
        iLastPlayedUrl = iUrlArray->at(0);
        // Set the status to EStopped because it didn't start to play at all and need not to 
        // do stop action. 
        iPlayState = EStopped;
        doPlay(iLastPlayedUrl);

        return true;
    }
    
    return false;
}

/*
 * Description : complete the play action
 */
void IRPlayController::doPlay(const QString aUrl)
{
    // stop player, disable stereo effect, emit playstopped signal 
    stop(EIRQUserTerminated);

    //call getIAPId() every time before refering to it, because in ALR, the access point can 
    //be changed
    unsigned long apId = 0;
    iApplication->getNetworkController()->getIAPId(apId);
    qDebug("IRPlayController::doPlay, access point : %d", apId);
    iMediaPlayer->playStation(aUrl, apId);
    iPlayState = EBuffering;
#ifdef HS_WIDGET_ENABLED	
    // force logo to be default when current view is NOT nowplaying view && is not resuming (start playing a different station.) 
    if (EIRView_PlayingView != iApplication->getViewManager()->currentViewId()
        && !iResuming)
    {
        emitStationLogoUpdated(false);             
    }   
    emit bufferingStarted(iNowPlayingPreset->name);
#endif	
    startSession();
    iApplication->startLoadingAnimation(this, SLOT(cancelBuffering()));
}

/*
 * Description : start a session
 */
void IRPlayController::startSession()
{
	iGetServerResult = false;

    int channelId = 0;
    if(iNowPlayingPreset)
    {
        channelId = iNowPlayingPreset->presetId;
    }
	      
    if(iStatisticsReporter)
    {
        iStatisticsReporter->sessionStarted(channelId,MAP_TO_ENGINE_ConnectedFrom(iConnectedFrom));
    }
}

#ifdef _DEBUG
int IRPlayController::bitrateTrying() const
{
    return iTryingBitrate;
}
#endif 

void saveStationLogoFlag(bool aIsStationLogoAvailable)
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    QVariant data(QVariant::Bool);
    data.setValue(aIsStationLogoAvailable);
    settings.setValue(KIrSettingStationLogoAvailable,data);
}

//get IP address configuration of test radio server
#ifdef Q_CC_NOKIAX86
void getRadioServerAddress(QString & aUrl)
{
    QFile file("C:\\data\\QTIRConfigure.txt");
    
    if (file.open(QIODevice::ReadOnly)) 
    {
        QTextStream stream(&file);
        QString line;
        QStringList parameter;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            parameter = line.split("=");
            if (parameter.count() == 2)
            {
                if (parameter.first() == "RadioServerAddress")
                {
                    aUrl = parameter.last();
                    break;
                }
            }
        }
        file.close();
    }
}
#endif
