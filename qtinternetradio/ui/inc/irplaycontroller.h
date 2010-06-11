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
#ifndef IRPLAYCONTROLLER_H
#define IRPLAYCONTROLLER_H

#include <QObject>
#include <hbglobal.h>

#include "irqenums.h"

class IRApplication;
class IRQMediaPlayer;
class IRQPreset;
class IRQMetaData;
class IRQSongHistoryEngine;
class IRQStatisticsReporter;

/**
 * This class is a wrapper for IRQMediaPlayer.
 */

class IRPlayController : public QObject
{
    Q_OBJECT

    
public slots:    
    // slots connected with own functions
    void cancelBuffering();
    
public:
    explicit IRPlayController(IRApplication* aApplication);
    ~IRPlayController();

    // Play a station
    void connectToChannel(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom);

    // Play control methods
    void resume();
    void stop(IRQTerminatedType aStopReason);
    int getVolume() const;
    void setVolume(int aVolume);

    // Check the playing status
    bool isPlaying() const;

    // Check the stopped status
    bool isStopped() const;

    // Check the idle status
    bool isIdle() const;
    
    // Get the preset of now playing
    IRQPreset * getNowPlayingPreset() const;
    
    QString getNowPlayingUrl() const;
    int getNowPlayingBitRate() const;

    // Get the meta data
    IRQMetaData * getMetaData() const;
    
    IRQTerminatedType getStopReason() const;
    
#ifdef _DEBUG
    int bitrateTrying() const;
#endif 

signals:
    // emitted when meta data is available
    void metaDataAvailable(IRQMetaData*);

    // emitted when playing started
    void playingStarted();

    // emitted when playing stopped
    void playingStopped();

    // emitted when logo needs to update
    void initializeLogo();

private slots:
    // slots connected with IRQMediaPlayer
    void connectionEstablished(int aBitrate);
    void errorOccured(IRQError aError);
    void updateProgress(int aProgress);
    void fetchVolume(int &aVolume);
    void handleMetaDataReceived(IRQMetaData& aIRmetaData);

    // slots connected with own functions  
    void handleError();

private:
    void createNote(const QString &aNote = hbTrId("txt_irad_info_failed_to_connect"));
    void connectSignalSlot();

    // Play next URL if the previous failed to connect.
    bool playNextUrl();

    void doPlay(const QString& aUrl);
	
	void startSession();
	void endSession(IRQTerminatedType aStopReason);

private:
    IRApplication *iApplication;
    IRQMediaPlayer *iMediaPlayer;
    
    IRQStatisticsReporter *iStatisticsReporter;
    IRQConnectedFrom iConnectedFrom;
    bool iGetServerResult;
    
    IRQPreset *iNowPlayingPreset;
    //reserve the info in nowplay view
    IRQPreset *iNowPlayingPresetBackup;
    // reference of IRQMediaPlayer meta data
    IRQMetaData *iMetaData;
    IRQSongHistoryEngine *iSongHistoryEngine;

    enum EPlayState
    {
        //initial state
        EIdle = 0,
        //a station has been stopped
        EStopped,
        //connecting station and buffering
        EBuffering,
        //playing
        EPlaying
    };

    EPlayState iPlayState;
    // If resuming the last played url fails, connect to all the preset.
    bool iResuming;

    // variants for play next URL
    // available bitrates in the preset
    QList<int> iAvailableBitrate;

    // the bitrate is now tried.
    int iTryingBitrate;

    // the URLs of a specific bitrate
    QList<QString> *iUrlArray;

#ifdef Q_CC_NOKIAX86
    QString iLastPlayedChannelName;
#endif

    // the bitrate from real station feedback.
    int iRealBitrate;
    QString iLastPlayedUrl;
    //reserve the info in nowplay view
    QString iLastPlayedUrlBackup; 
    // To handle error async
    IRQError iLastError;
    
    IRQTerminatedType iStopReason;
};

#endif  //IRPLAYCONTROLLER_H
