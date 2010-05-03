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

class HbProgressDialog;
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

public:
    explicit IRPlayController(IRApplication* aApplication);
    ~IRPlayController();

    // Play a station
    void connectToChannel(IRQPreset *aPreset, IRQConnectedFrom aConnectedFrom);

    // Create a buffering dialog
    void createBufferingDialog(const QObject *aReceiver, const char *aFunc);

    // Close the buffering dialog
    void closeBufferingDialog();

    // Play control methods
    void resume();
    void stop(IRQTerminatedType aStopReason);
    int getVolume() const;
    void setVolume(int aVolume);
    void enableStereo();
    void disableStereo();

    // Check the playing status
    bool isPlaying() const;

    // Check the playing status
    bool isStopped() const;

    // Get the preset of now playing
    IRQPreset * getNowPlayingPreset() const;

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
    void cancelBuffering();
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
    bool iSessionStarted;
    bool iGetServerResult;
    
    HbProgressDialog *iBufferingDialog;
    IRQPreset *iNowPlayingPreset;
    // reference of IRQMediaPlayer meta data
    IRQMetaData *iMetaData;
    IRQSongHistoryEngine *iSongHistoryEngine;

    enum EPlayState
    {
        //initial state
        EStopped = 0,
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
    // To handle error async
    IRQError iLastError;
    
    IRQTerminatedType iStopReason;
};

#endif  //IRPLAYCONTROLLER_H
