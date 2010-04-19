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
#ifndef IRQMEDIAPLAYER_H_
#define IRQMEDIAPLAYER_H_

#include <QObject>
#include "irqenums.h"

class IRQPlayerAdapterInterface;
class IRQMetaData;
class CStereoWidening;

/**
 * This class provides the interface to IR Media Player component
 */

class IRQMediaPlayer : public QObject
{
    Q_OBJECT

public:

    /**
     *  Constructor
     */
    IMPORT_C IRQMediaPlayer();

    /**
     *  Destructor
     */
    ~IRQMediaPlayer();

    /**
     *  Returns the current volume of playback, it's a percentage
     *  @return int
     */
    IMPORT_C int  getVolume();

public slots:

    /**
     *  Plays a radio station
     */
    IMPORT_C void playStation(const QString &aUrl, int aApId);

    /**
     *  Stops playback
     */
    IMPORT_C void stop();

    /**
     *  Sets the volume to player, it's a percentage
     */
    IMPORT_C void setVolume(int aVolume);

    /**
     *  Turns on the stereo effect
     */
    IMPORT_C void enableStereoEffect();

    /**
     *  Turns off the stereo effect
     */
    IMPORT_C void disableStereoEffect();

signals:

    /**
     *  Notifies the successful connection with a radio station
     */
    void connectionEstablished(int aBitrate);

    /**
     *  Notifies the error
     */
    void errorOccured(IRQError aError);

    /**
     *  Notifies the buffering status
     */
    void percentageBuffered(int aPercentage);

    /**
     *  Notifies the meta data
     */
    void metaDataReceived(IRQMetaData& aIRMetaData);

    /**
     *  Gets the preset volume, it's a percentage
     */
    void volumeExpected(int& aVolume);

private:
    /**
     *  Turns on the stereo effect
     */
    void enableStereoEffectL();

private:

    /**
     *  Player adpater interface
     */
    IRQPlayerAdapterInterface* iPlayer;

    /**
     *  Used for handling Stereo Mode
     */
    CStereoWidening* iStereoEffect;
};

#endif /* IRQMEDIAPLAYER_H_ */
