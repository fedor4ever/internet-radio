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
#include <StereoWideningBase.h>
#include "irqmediaplayer.h"
#if defined(MMFADAPTER)
#include "irqmmfadapter.h"
#elif defined(PHONONAdapter)
#include "irqphononadapter.h"
#endif

#include "irqlogger.h"

//Constants
const int KDefaultStereoLevel = 100;            // Default stereo level

// ---------------------------------------------------------------------------
// IRQMediaPlayer::IRQMediaPlayer
// Constructor
// Creates player adpater and connect the signals
// ---------------------------------------------------------------------------
//
EXPORT_C IRQMediaPlayer::IRQMediaPlayer() :
    iStereoEffect(NULL)
{
#if defined(MMFADAPTER)
    iPlayer = new IRQMMFAdapter();
#elif defined(PHONONAdapter)
    iPlayer = new IRQPHONONAdapter();
#endif

    connect(iPlayer, SIGNAL(connectionEstablished(int)),
            this, SIGNAL(connectionEstablished(int)));
    connect(iPlayer, SIGNAL(errorOccured(IRQError)),
            this, SIGNAL(errorOccured(IRQError)));
    connect(iPlayer, SIGNAL(percentageBuffered(int)),
            this, SIGNAL(percentageBuffered(int)));
    connect(iPlayer, SIGNAL(metaDataReceived(IRQMetaData&)),
            this, SIGNAL(metaDataReceived(IRQMetaData&)));
    connect(iPlayer, SIGNAL(volumeExpected(int&)),
            this, SIGNAL(volumeExpected(int&)));
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::~IRQMediaPlayer
// Destructor
// Delete player adapter
// ---------------------------------------------------------------------------
//
IRQMediaPlayer::~IRQMediaPlayer()
{
    delete iPlayer;
    iPlayer = NULL;

    delete iStereoEffect;
    iStereoEffect = NULL;
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::playStation
// Plays a specific radio station via certain access point id
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMediaPlayer::playStation(const QString &aUrl, int aApId)
{
    if (iPlayer)
    {
        iPlayer->playStation(aUrl, aApId);
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::stop
// Stops the playback
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMediaPlayer::stop()
{
    if (iPlayer)
    {
        iPlayer->stop();
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::setVolume
// Sets the volume
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMediaPlayer::setVolume(int aVolume)
{
    if (iPlayer)
    {
        iPlayer->setVolume(aVolume);
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::getVolume
// Gets the current volume of playback
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQMediaPlayer::getVolume()
{
    int val = 0;

    if (iPlayer)
    {
        val = iPlayer->getVolume();
    }

    return val;
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::enableStereoEffect
// Turns on stereo effect
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMediaPlayer::enableStereoEffect()
{
    if (!iPlayer || IRQPlayerAdapterInterface::EPlaying != iPlayer->iPlayState)
    {
        return;
    }

    TRAP_IGNORE(enableStereoEffectL());
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::disableStereoEffect
// Turns off stereo effect
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMediaPlayer::disableStereoEffect()
{
    LOG_METHOD;
    if (iStereoEffect)
    {
        if (iStereoEffect->IsEnabled())
        {
            TRAPD(error, iStereoEffect->DisableL());
            if (KErrNone != error)
            {
                emit errorOccured(EIRQPlayerErrorSetStereoFailed);
            }
            delete iStereoEffect;
            iStereoEffect = NULL;
        }
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::enableStereoEffectL
// Turns on stereo effect
// ---------------------------------------------------------------------------
//
void IRQMediaPlayer::enableStereoEffectL()
{

    TUint stereoLevel = KDefaultStereoLevel;

    if (!iStereoEffect)
    {
#if defined(MMFADAPTER)
        CVideoPlayerUtility* playerInstance = (CVideoPlayerUtility*)iPlayer->getPlayerInstance();
#elif defined(PHONONAdapter)
        void* playerInstance = iPlayer->getPlayerInstance();
#endif
        iStereoEffect = CStereoWidening::NewL(*playerInstance, EFalse, stereoLevel);
    }

    if (!iStereoEffect->IsEnabled())
    {
        iStereoEffect->EnableL();
        iStereoEffect->SetStereoWideningLevelL(stereoLevel);
        iStereoEffect->ApplyL();
    }
}
