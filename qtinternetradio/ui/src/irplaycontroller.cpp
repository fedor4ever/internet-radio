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
#include <hbprogressdialog.h>
#include <hbmessagebox.h>
#include <QTimer>
#ifdef Q_CC_NOKIAX86
#include <QFile>
#include <QTextStream>
#endif

#include "irplaycontroller.h"
#include "irapplication.h"
#include "irqmediaplayer.h"
#include "irabstractviewmanager.h"
#include "irqisdsdatastructure.h"
#include "irlastplayedstationinfo.h"
#include "irqnetworkcontroller.h" 
#include "irqsonghistoryengine.h"
#include "irqmetadata.h"
#include "irqsettings.h"
#include "irqfavoritesdb.h"
#include "irqstatisticsreporter.h"

#ifdef Q_CC_NOKIAX86
void getRadioServerAddress(QString & aUrl);
#endif

//                                        public functions

/*
 * Description : constructor
 */
IRPlayController::IRPlayController(IRApplication* aApplication) :
    iApplication(aApplication),
    iMediaPlayer(new IRQMediaPlayer()),
    iStatisticsReporter(NULL),
    iConnectedFrom(EIRQIsds),
    iSessionStarted(false),  
    iGetServerResult(false),
    iBufferingDialog(NULL),
    iNowPlayingPreset(new IRQPreset()),
    iMetaData(NULL),
    iSongHistoryEngine(IRQSongHistoryEngine::openInstance()),
    iPlayState(EStopped),
    iResuming(false),
    iTryingBitrate(0),
    iUrlArray(0),
    iRealBitrate(0),
    iLastError(EIRQErrorNone),
    iStopReason(EIRQUnknownTermination)
{
    connectSignalSlot(); 
    iStatisticsReporter = iApplication->getStatisticsReporter();
}

/*
 * Description : destructor
 */
IRPlayController::~IRPlayController()
{
    iApplication = NULL;
    iPlayState = EStopped;

    delete iBufferingDialog;
    iBufferingDialog = NULL;

    stop(EIRQUserTerminated);
    delete iMediaPlayer;
    iMediaPlayer = NULL;

    delete iNowPlayingPreset;
    iNowPlayingPreset = NULL;

    delete iUrlArray;
    iUrlArray = NULL;

    iMetaData = NULL;

    if (iSongHistoryEngine)
    {
        iSongHistoryEngine->closeInstance();
        iSongHistoryEngine = NULL;
    }
}

/*
 * Description : connect to a channel specified by aPreset.
 * Parameters  : aPreset : the preset of the channel
 */
void IRPlayController::connectToChannel(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom)
{
    iConnectedFrom = aConnectedFrom;
    if (!aPreset)
    {
        return;
    }

    if (iMediaPlayer)
    {
        // sort the URL by ascending order and get all available rates.
        // iAvailableBitrate is cleared in getAvailableBitrates().
        aPreset->sortURLArray();
        aPreset->getAvailableBitrates(iAvailableBitrate);
        if (iAvailableBitrate.count() == 0)
        {
            return;
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
        *iNowPlayingPreset = *aPreset;
        delete iUrlArray;
        iUrlArray = NULL;
        iUrlArray = iNowPlayingPreset->getURLsForBitrate(selectedBitRate);
        if (iUrlArray)
        {
            QString url = iUrlArray->at(0);
#ifdef Q_CC_NOKIAX86
            if (iLastPlayedChannelName != aPreset->name)
            {
                emit initializeLogo();
            }
            url = "http://172.28.205.171:8000";
            getRadioServerAddress(url);
            iLastPlayedChannelName = aPreset->name;
#else
            if (iLastPlayedUrl != iUrlArray->at(0))
            {
                emit initializeLogo();
            }
#endif
            iLastPlayedUrl = url;
            iResuming = false;
            doPlay(url);
        }
    }
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
    if (iMediaPlayer)
    {
        iMediaPlayer->disableStereoEffect();

        iMediaPlayer->stop();

        if (EPlaying == iPlayState)
        {
            iStopReason = aStopReason;
            // playingStarted is emitted while iPlaying is set to true,
            // so when stop() is called and iPlaying is true, playingStopped
            // should be emitted.
            qDebug("IRPlayController::stop, emit playingStopped()");
            emit playingStopped();
        }
        iPlayState = EStopped;
    }
	endSession(aStopReason);
	qDebug("IRPlayController::stop, Exiting");
}

/*
 * Description : End Session
 */
void IRPlayController::endSession(IRQTerminatedType aStopReason)
{
    if(iStatisticsReporter && iSessionStarted)
    {
        if(!iGetServerResult)
        {
            iStatisticsReporter->logServerResult(iLastPlayedUrl,EIRQPlayerErrorConnectingFailed);
        }
        iStatisticsReporter->sessionEnded(aStopReason);
        iSessionStarted = false;
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

/*
 * Description : enable stereo effect
 */
void IRPlayController::enableStereo()
{
    if (iMediaPlayer)
    {
        iMediaPlayer->enableStereoEffect();
    }
}

/*
 * Description : disable stereo effect
 */
void IRPlayController::disableStereo()
{
    if (iMediaPlayer)
    {
        iMediaPlayer->disableStereoEffect();
    }
}

/*
 * Description : return the flag of playing state
 * Return      : true  : playing is ongoing
 *               false : playing is stopped
 */
bool IRPlayController::isPlaying() const
{
    return (EPlaying == iPlayState);
}

/*
 * Description : return the flag of stopped state
 * Return      : true  : playing is stopped
 *               false : playing is ongoing
 */
bool IRPlayController::isStopped() const
{
    return (EStopped == iPlayState);
}

/*
 * Description : return the now playing preset
 * Return      : pointer to the now playing preset
 */
IRQPreset * IRPlayController::getNowPlayingPreset() const
{
    return iNowPlayingPreset;
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

/*
 * Description : show a buffering dialog to inform user the buffering stage.
 *               If the dialog is not created yet, create first. 
 */
void IRPlayController::createBufferingDialog(const QObject *aReceiver, const char *aFunc)
{
    if (NULL == iBufferingDialog)
    {
        iBufferingDialog = new HbProgressDialog(HbProgressDialog::ProgressDialog);
        iBufferingDialog->setMinimum(0);
        iBufferingDialog->setMaximum(100);
        iBufferingDialog->setModal(true);
    }

    //disconnect everything connected to signal cancelled()
    iBufferingDialog->disconnect(SIGNAL(cancelled()));

    connect(iBufferingDialog, SIGNAL(cancelled()), aReceiver, aFunc);
    iBufferingDialog->setProgressValue(0);
    iBufferingDialog->setText("0%");
    iBufferingDialog->show();
}

/*
 * Description : close the buffering dialog
 */
void IRPlayController::closeBufferingDialog()
{
    if (iBufferingDialog)
    {
        iBufferingDialog->close();
        delete iBufferingDialog;
        iBufferingDialog = NULL;
    }
}

//                                           slot  functions

/*
 * Description : data connection has been established. Signal is emitted by media player.
 * 
 */
void IRPlayController::connectionEstablished(int aBitrate)
{
    if(iStatisticsReporter && iSessionStarted)
    {
        iStatisticsReporter->logServerResult(iLastPlayedUrl,EIRQErrorNone);
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
    iLastError = aError;

    QTimer::singleShot(1, this, SLOT(handleError()));
}

/*
 * Description : handle the error async.
 */
void IRPlayController::handleError()
{
    qDebug("IRPlayController::handleError(), Entering, iLastError - %d", iLastError);
    switch (iLastError)
    {
    case EIRQPlayerErrorServerFull:
    case EIRQPlayerErrorTimeOut:
    case EIRQPlayerErrorConnectingFailed:
        if(iStatisticsReporter)
        {
            iStatisticsReporter->logServerResult(iLastPlayedUrl,iLastError);
            iGetServerResult = true;
        }
		
        // if there's NO other URL to try, show warning.
        if (iNowPlayingPreset->getChannelURLCount() == 1)
        {
		    endSession(EIRQNoConnectionToServer);
            break;
        }

        if (iResuming)
        {
            HbMessageBox::information(tr("Connecting failed, try next URL"), (QObject*)NULL, NULL);
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
            endSession(EIRQNoConnectionToServer);
        }
        break;

    case EIRQPlayerErrorConnectionLost:
        qDebug("IRPlayController::handleError, connection lost");
        stop(EIRQNoConnectionToServer);
        break;
		
    case EIRQPlayerErrorGeneral:
    case EIRQPlayerErrorAudioDeviceLost:
    default:
        stop(EIRQUnknownTermination);
        break;
    }

    closeBufferingDialog();

    createNote();
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
    /* we added this condition for sometimes, the function will be called
     * when the state is playing. reference cr_9010
     */
    if( iBufferingDialog && EBuffering == iPlayState )
    {        
        iBufferingDialog->setProgressValue(aProgress);
        iBufferingDialog->setText(QString("%1%").arg(aProgress));       
    }
    
    if (100 == aProgress)
    {
        closeBufferingDialog();

        //updateProgress(100) sometimes can be called more than one time, to improve performance,
        //we only need to do the following work once.
        if (EBuffering == iPlayState)
        {
            iApplication->getViewManager()->activateView(EIRView_PlayingView);
            iPlayState = EPlaying;

            //update last played station
            IRLastPlayedStationInfo *lastPlayedStationInfo = iApplication->getLastPlayedStationInfo();
            lastPlayedStationInfo->updateLastPlayedStation(iNowPlayingPreset,iConnectedFrom);
            lastPlayedStationInfo->commitLastPlayedStation();

            //increase the played times of current preset
            iApplication->getFavoritesDB()->increasePlayedTimes(*iNowPlayingPreset);

            emit playingStarted();

            // if the metadata is available, show it.
            emit metaDataAvailable(iMetaData);

            // Save the station information to database
            IRQMetaData tmpMetaData;
            tmpMetaData.setBitrate(iRealBitrate);
            tmpMetaData.setStreamUrl(iLastPlayedUrl);
            iSongHistoryEngine->handleMetaDataReceived(tmpMetaData, *iNowPlayingPreset);
            // open stereo according to settings
            if (1 == iApplication->getSettings()->getStereoMode())
            {
                iMediaPlayer->enableStereoEffect();
            }
        }
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
                iNowPlayingPreset->musicStoreStatus);  
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
    if (!iResuming && EIRView_PlayingView == iApplication->getViewManager()->currentViewId())
    {
        iApplication->getViewManager()->backToPreviousView();
    }
}

//                                       private functions

/*
 * Description : show a note to user to inform that error occured.
 *                
 */
void IRPlayController::createNote(const QString &aNote)
{
    HbMessageBox::warning(aNote, (QObject*)NULL, NULL);
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
    connect(iMediaPlayer, SIGNAL(metaDataReceived(IRQMetaData&)),
            this, SLOT(handleMetaDataReceived(IRQMetaData&)));
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
                HbMessageBox::information(tr("Connecting failed, try next URL"), (QObject*)NULL, NULL); 
                delete iUrlArray;
                iUrlArray = iNowPlayingPreset->getURLsForBitrate(iTryingBitrate);
                iLastPlayedUrl = iUrlArray->at(0);
                doPlay(iLastPlayedUrl);
                return true;
            }
        }

    }
    else // try next url in iUrlArray
    {
        iLastPlayedUrl = iUrlArray->at(0);
        doPlay(iLastPlayedUrl);

        HbMessageBox::information(tr("Connecting failed, try next URL"), (QObject*)NULL, NULL);
        return true;
    }
    
    return false;
}

/*
 * Description : complete the play action
 */
void IRPlayController::doPlay(const QString& aUrl)
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
    startSession();
    createBufferingDialog(this, SLOT(cancelBuffering()));
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
	      
    if(iStatisticsReporter && !iSessionStarted)
    {
        iSessionStarted = iStatisticsReporter->sessionStarted(channelId,iConnectedFrom);
    }
}

#ifdef _DEBUG
int IRPlayController::bitrateTrying() const
{
    return iTryingBitrate;
}
#endif 

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
