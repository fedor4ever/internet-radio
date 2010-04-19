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
#ifndef IRNOWPLAYINGVIEW_H
#define IRNOWPLAYINGVIEW_H

#include <hbeffect.h>

#include "irbaseview.h"
#include "irqevent.h"

class IRNowPlayingContainer;
class IRStationDetailsContainer;
class HbAction;
class IRQPreset;
class IRQMusicShop;
class IRQStatisticsReporter;
class IRQMetaData;
class IRQStationExporter;

/**
 * This class shows the station information when it's played.
 */

class IRNowPlayingView : public IRBaseView
      
{
    Q_OBJECT

public:
    ~IRNowPlayingView();

protected:
    IRNowPlayingView(IRApplication* aApplication, TIRViewId aViewId);
    //from base class IRBaseView
    TIRHandleResult handleSystemEvent(TIRSystemEventType aEvent);

    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);

    //from base class IRBaseView
    void launchAction();

    

private slots:
    void presetLogoDownload(IRQPreset* aPreset);
    void playStopControl();
    void stop();
    void playingStarted();
    void playingStopped();
    void add2FavControl();
    void flipControl();
    void hidePlayingWidgetComplete(HbEffect::EffectStatus status);
    void hideDetailsWidgetComplete(HbEffect::EffectStatus status);
    void stereoControl();
    void openWebAddressControl();
    void launchSettingsView();
    void updateSongInfo(IRQMetaData*);
    void updateLogoInfo();
    void initializeLogo();
    void networkRequestNotified(IRQNetworkEvent aEvent);
    void findInMusicShop();
    void helpAction();
    void playPressed();
    void gotoMusicStore();
    void recognizeSong();
    void openAdvLink();
    void shareStationViaMms();
    void prepareMenu();

private:
    void createMenu();
    void createToolBar();
    void updateStationInfo();
    void updateAdvImage();
    void showWidget();
    void initialize();
    void updateMusicStatus();

private:
    // preset to save station big logo
    IRQPreset *iLogoPreset;
    IRNowPlayingContainer *iNowPlayingContainer;
    IRStationDetailsContainer *iStationDetailsContainer;
    // save the pointer of idle container
    QGraphicsWidget *iIdleContainer;
    IRQMusicShop *iMusicShop;
    IRQStatisticsReporter *iStatisticsReporter;
    IRQStationExporter *iStationExporter;

    // menu bar actions
    HbAction *iStereoAction;

    // tool bar actions
    HbAction *iMusicShopAction;
    HbAction *iPlayStopAction;
    HbAction *iAdd2FavAction;
    HbAction *iFlipAction;

    QString iAdvImageUrl;
    QString iTempAdvClickThroughUrl;
    QString iAdvClickThroughUrl;
    bool    iGettingAdv;
    HbIcon *iStationLogo;

    int  iStereoMode;
    bool iShowStationInfo;

    enum LogoState
    {
        EDefaultLogo    = 0, // Use default logo
        ERequestIssued  = 1, // Issue request to get logo from server
        ELogoDetermined = 2, // Logo got from server or user defined
                             // stations with default logo
    };
    LogoState iLogoStatus;
    
    bool iFindinNmsAllowed;
    
    bool iSongNameAvailable;
    
    friend class IRViewManager;
};

#endif   //IRNOWPLAYINGVIEW_H
