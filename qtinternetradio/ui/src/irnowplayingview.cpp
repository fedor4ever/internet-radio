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
#include <QPixmap>
#include <QTimer>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbaction.h>

#include "irabstractviewmanager.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irnowplayingview.h"
#include "irnowplayingcontainer.h"
#include "irstationdetailscontainer.h"
#include "irqisdsdatastructure.h"
#include "irqmetadata.h"
#include "irqutility.h"
#include "irqisdsclient.h"
#include "irqfavoritesdb.h"
#include "irlastplayedstationinfo.h"
#include "irqnetworkcontroller.h" 
#include "irqmusicshop.h"
#include "irqstatisticsreporter.h"
#include "irqenums.h"
#include "irmediakeyobserver.h"
#include "irqsettings.h"
#include "irqstationexporter.h"
#include "iruidefines.h"

// Const
const int KNowPlayingLogoSize = 300; // Now playing logo size
const char* NOW_PLAYING_VIEW_OBJECT_NAME = "ex-IRNowPlayingView"; // object name in the XML
const char* KDefaultAdvertisementLink = ""; // default advertisement link

//                                     public functions

/*
 * Description : constructor
 */
IRNowPlayingView::IRNowPlayingView(IRApplication* aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId),
    iLogoPreset(NULL),
    iStationDetailsContainer(NULL),
    iIdleContainer(NULL),
    iMusicShop(NULL),
    iStationExporter(NULL),
    iStereoAction(NULL),
    iMusicShopAction(NULL),
    iPlayStopAction(NULL),
    iAdd2FavAction(NULL),
    iAdvClickThroughUrl(KDefaultAdvertisementLink),
    iGettingAdv(false),
    iStationLogo(NULL),
    iStereoMode(0),
    iShowStationInfo(FALSE),
    iLogoStatus(EDefaultLogo),
    iFindinNmsAllowed(false),
    iSongNameAvailable(false)
{
    // Create widget hierarchy
    setObjectName(NOW_PLAYING_VIEW_OBJECT_NAME);

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append(this); // IRNowPlayingView is referred in XML file
    iLoader.setObjectTree(roots);

    // Load XML file
    iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME);

    createMenu();
    createToolBar();
    initialize();

    connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(updateSongInfo(IRQMetaData*)));
    connect(iPlayController, SIGNAL(initializeLogo()), this, SLOT(initializeLogo()));
    connect(iPlayController, SIGNAL(playingStarted()), this, SLOT(playingStarted()));
    connect(iPlayController, SIGNAL(playingStopped()), this, SLOT(playingStopped()));

    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    connect(iApplication->getMediaKeyObserver(), SIGNAL(playPausePressed()), 
            this, SLOT(playPressed()));
    connect(iApplication->getMediaKeyObserver(), SIGNAL(stopPressed()), 
            this, SLOT(stop()));

    // Initialize logo
    QIcon icon(":/playback/default_cd.png");
    iStationLogo = new HbIcon(icon);

    // Initialize advertisement image
    QIcon advIcon(":/playback/default_cd.jpg");
    HbIcon advInitialImage(advIcon);
    iStationExporter = new IRQStationExporter();
    iNowPlayingContainer = new IRNowPlayingContainer(getViewManager());
    connect(getViewManager(), SIGNAL(orientationChanged(Qt::Orientation)), 
            iNowPlayingContainer, SLOT(orientationChanged(Qt::Orientation)));
    connect(iNowPlayingContainer, SIGNAL(volumePressed()), 
            iApplication->getMediaKeyObserver(), SLOT(showVolumeSlider()));
    connect(iNowPlayingContainer, SIGNAL(advertisementPressed()), 
                this, SLOT(openAdvLink()));
    setWidget(iNowPlayingContainer);
    iNowPlayingContainer->setLogo(*iStationLogo);
    iNowPlayingContainer->setAdvImage(advInitialImage);

    HbEffect::add("nowplaying", ":/effect/view_flip_hide.fxml", "hide");
    HbEffect::add("nowplaying", ":/effect/view_flip_show.fxml", "show");
}

/*
 * Description : destructor
 */
IRNowPlayingView::~IRNowPlayingView()
{
    delete iIdleContainer;
    iIdleContainer = NULL;

    delete iStationExporter;
    iStationExporter = NULL;

    iNowPlayingContainer = NULL;
    iStationDetailsContainer = NULL;

    delete iLogoPreset;
    iLogoPreset = NULL;

    delete iStationLogo;
    iStationLogo = NULL;
    
    HbEffect::remove("nowplaying", ":/playback/view_flip_hide.fxml", "hide");
    HbEffect::remove("nowplaying", ":/playback/view_flip_show.fxml", "show");
}

/*
 * Description : from base class IRBaseView.
 *               handle system events reported by system event collector
 * Parameters  : aEvent : see the definiton of TIRSystemEventType.
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRNowPlayingView::handleSystemEvent(TIRSystemEventType aEvent)
{
    Q_UNUSED(aEvent);
    return EIR_DoDefault;
}

/*
 * Description : from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRNowPlayingView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = EIR_DoDefault;

    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(presetLogoDownload(IRQPreset* )));
        showWidget();
        break;

    case EIR_ViewCommand_DEACTIVATE:
        if (ERequestIssued == iLogoStatus)
        {
            // cancel downloading logo
            iIsdsClient->isdsLogoDownCancelTransaction();
            iLogoStatus = EDefaultLogo;
        }
        disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
                   this, SLOT(presetLogoDownload(IRQPreset* )));
        break;
    }

    return ret;
}

/*
 * Description : actions when view is launched.
 */
void IRNowPlayingView::launchAction()
{
    IRLastPlayedStationInfo *lastPlayedStationInfo =
            iApplication->getLastPlayedStationInfo();
    IRQPreset *preset = lastPlayedStationInfo->getLastPlayedStation();
    if ( preset && (0 == preset->musicStoreStatus.compare("yes",Qt::CaseInsensitive)))
    {
        iFindinNmsAllowed = true;        
    }
    else
    {
        iFindinNmsAllowed = false;
    }
          
    iShowStationInfo = true;
    setUseNetworkReason(EIR_UseNetwork_StartingView);
    iApplication->verifyNetworkConnectivity();
}

/*
 * Description : start to converter the downloaded logo.
 */
void IRNowPlayingView::presetLogoDownload(IRQPreset* aPreset)
{
    if (NULL == aPreset)
        return;

    // Sometimes the logo downloaded by stationview will reach here because
    // the isdsLogoDownCancelTransaction is delayed. So we judge the imageUrl here.
    if (!iGettingAdv && (aPreset->imgUrl != iPlayController->getNowPlayingPreset()->imgUrl))
    {
        delete aPreset;
        aPreset = NULL;
        return;
    }
 
    delete iLogoPreset;
    iLogoPreset = aPreset;
    
    const unsigned char * logoData = iLogoPreset->logoData.Ptr();
    QPixmap tempMap;
    bool ret = tempMap.loadFromData(logoData, iLogoPreset->logoData.Length());
    if( !ret )
    {
        if (iGettingAdv)
        {
            iGettingAdv = false;
            QTimer::singleShot(1, this, SLOT(updateLogoInfo()));
        }
        else
        {
            iLogoStatus = ELogoDetermined;
        }
        return;
    }
    
    QIcon convertIcon(tempMap);
    HbIcon hbIcon(convertIcon);   
    
    if (iGettingAdv)
    {
        iNowPlayingContainer->setAdvImage(hbIcon);
        iAdvClickThroughUrl = iTempAdvClickThroughUrl;
        iGettingAdv = false;
        QTimer::singleShot(1, this, SLOT(updateLogoInfo()));
    }
    else
    {
        *iStationLogo = hbIcon;

        // update the logo
        iNowPlayingContainer->setLogo(hbIcon);
        // temp solution, it will be deleted once there's adv info in preset
        iNowPlayingContainer->setAdvImage(*iStationLogo);

        if (iStationDetailsContainer)
        {
            iStationDetailsContainer->setLogo(hbIcon);
        }
        iLogoStatus = ELogoDetermined;
    } 
}

/*
 * Description: slot function for signal triggered() of iPlayStopAction and
 *              playStopPressed() of irmediakeyobserver.
 *              According to the playing state, play or stop.
 */
void IRNowPlayingView::playStopControl()
{
    setUseNetworkReason(EIR_UseNetwork_PlayStation);
    if (false == iApplication->verifyNetworkConnectivity())
    {
        return;
    }
    setUseNetworkReason(EIR_UseNetwork_NoReason);
    
    if (iPlayController->isPlaying())
    {
        iPlayController->stop(EIRQUserTerminated);
    }
    else
    {
        iPlayController->resume();
    }
}

/*
 * Description: slot function for signal stopPressed() of irmediakeyobserver.
 */
void IRNowPlayingView::stop()
{
    iPlayController->stop(EIRQUserTerminated);
}

/*
 * Description : update station info and now playing view
 */
void IRNowPlayingView::playingStarted()
{
    if (iShowStationInfo)
    {
        updateSongInfo(iPlayController->getMetaData());
        updateAdvImage();
        updateStationInfo();
        iShowStationInfo = false;
    }
    updateMusicStatus();
    HbIcon icon(QString("qtg_mono_stop"));
    iPlayStopAction->setIcon(icon);
}

/*
 * Description : update now playing view
 */
void IRNowPlayingView::playingStopped()
{
    HbIcon icon(QString("qtg_mono_play"));
    iPlayStopAction->setIcon(icon);
}

/*
 * Description: slot function for signal triggered() of iAdd2FavAction.
 *              add this preset to favorites
 */
void IRNowPlayingView::add2FavControl()
{
    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    int retValue = iFavorites->addPreset(*nowPlayingPreset);
    // show the information from favorites DB
    switch (retValue)
    {
    case EIRQErrorNone:
        popupNote(hbTrId("txt_irad_info_added_to_favorites"), HbMessageBox::MessageTypeInformation);
        break;

    case EIRQErrorOutOfMemory:
        popupNote(hbTrId("txt_irad_info_can_not_add_more"), HbMessageBox::MessageTypeInformation);
        break;

    case EIRQErrorAlreadyExist:
        popupNote(hbTrId("txt_irad_info_favorite_updated"), HbMessageBox::MessageTypeInformation);
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

/*
 * Description: slot function for signal triggered() of flip action.
 *              change the container of now playing view.
 */
void IRNowPlayingView::flipControl()
{
    // disconnect the signal to avoid users to click several times quickly.
    iFlipAction->disconnect(SIGNAL(triggered()));
    if (iIdleContainer == iStationDetailsContainer)
    {
        if (NULL == iStationDetailsContainer)
        {
            iStationDetailsContainer = new IRStationDetailsContainer(getViewManager());
        }

        IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
        iStationDetailsContainer->setDetails(nowPlayingPreset, *iStationLogo);

        HbEffect::start(iNowPlayingContainer, "nowplaying", QString("hide"), this, "hidePlayingWidgetComplete");
    }
    else
    {
        HbEffect::start(iStationDetailsContainer, "nowplaying", QString("hide"), this, "hideDetailsWidgetComplete");
    }
}

// ---------------------------------------------------------------------------
// Slot: Handle animation completion on main widget
// ---------------------------------------------------------------------------
//
void IRNowPlayingView::hidePlayingWidgetComplete(HbEffect::EffectStatus status)
{
    Q_UNUSED(status);
    iIdleContainer = takeWidget();
    setWidget(iStationDetailsContainer);
    iFlipAction->setToolTip(hbTrId("txt_irad_tooltip_tiny_now_playing"));
    HbEffect::start(iStationDetailsContainer, "nowplaying", QString("show"));

    // reconnect the signal
    iFlipAction->disconnect(SIGNAL(triggered()));
    connect(iFlipAction, SIGNAL(triggered()), this, SLOT(flipControl()));
}

// ---------------------------------------------------------------------------
// Slot: Handle animation completion on list widget
// ---------------------------------------------------------------------------
//
void IRNowPlayingView::hideDetailsWidgetComplete(HbEffect::EffectStatus status)
{
    Q_UNUSED(status);
    iIdleContainer = takeWidget();
    setWidget(iNowPlayingContainer);
    iFlipAction->setToolTip(hbTrId("txt_irad_tooltip_tiny_details"));
    HbEffect::start(iNowPlayingContainer, "nowplaying", QString("show"));

    // reconnect the signal
    iFlipAction->disconnect(SIGNAL(triggered()));
    connect(iFlipAction, SIGNAL(triggered()), this, SLOT(flipControl()));
}

/*
 * Description: slot function for signal triggered() of iStereoAction.
 *              According to the stereo state.
 */
void IRNowPlayingView::stereoControl()
{
    if (iStereoMode)
    {
        iPlayController->disableStereo();
        iStereoAction->setText(hbTrId("txt_irad_opt_activate_stereo"));
        iStereoMode = 0;
    }
    else
    {
        iPlayController->enableStereo();
        iStereoAction->setText(hbTrId("txt_irad_opt_deactivate_stereo"));
        iStereoMode = 1;
    }

    iApplication->getSettings()->setStereoMode(iStereoMode);
}

/*
 * Description: active open web address view
 */
void IRNowPlayingView::openWebAddressControl()
{
    getViewManager()->activateView(EIRView_OpenWebAddressView);
}

/*
 * Description: active settings view
 */
void IRNowPlayingView::launchSettingsView()
{
    getViewManager()->activateView(EIRView_SettingsView);
}

/*
 * Description : slot function for signal metaDataAvailable(IRQMetaData*) of play controller.
 *               If metadata is available, update the text
 */
void IRNowPlayingView::updateSongInfo(IRQMetaData* aMetaData)
{
#ifdef _DEBUG
    if (iApplication->iTestPreferredBitrate)
    {
        IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
        QList<int> bitRateList;
        bitRateList.clear();
        nowPlayingPreset->getAvailableBitrates(bitRateList);
        QString songName;
        QString bitrate;
        for(int i=0; i<bitRateList.count(); i++)
        {
            songName = songName + bitrate.setNum(bitRateList[i]) + " : ";
        }
        songName = songName + " = " + bitrate.setNum(iPlayController->bitrateTrying());
        iNowPlayingContainer->setSongName(songName);
        return;
    }
#endif
    iSongNameAvailable = false;
    if (aMetaData)
    {
        iNowPlayingContainer->setSongName(aMetaData->getArtistSongName());
        if(!aMetaData->getSongName().isEmpty() ||
           !aMetaData->getArtistName().isEmpty())
        {
            iSongNameAvailable = true;
        }
    }
    else
    {
        iNowPlayingContainer->setSongName("");
    }
    
    if(iFindinNmsAllowed && iSongNameAvailable)
    {
        iMusicShopAction->setIcon(HbIcon(":/playback/icon_musicshop_enable.png"));
    }
    else
    {
        iMusicShopAction->setIcon(HbIcon(":/playback/icon_musicshop_disable.png"));
    }    
}

// ---------------------------------------------------------------------------
// IRNowPlayingView::initializeLogo()
// Initialize the logo
//---------------------------------------------------------------------------
void IRNowPlayingView::initializeLogo()
{
    QIcon icon(":/playback/default_cd.png");
    HbIcon hbIcon(icon);
    *iStationLogo = hbIcon;
    iLogoStatus = EDefaultLogo;

    // While starting to play a new station, the container should be NowPlayingContainer
    if (iStationDetailsContainer == widget())
    {
        flipControl();
    }
}

/*
 * Description : slot function for active network event
 * Parameters  : aEvent, see the definition of IRQNetworkEvent
 */
void IRNowPlayingView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    //when we get here, it means that current view is the first view 
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        iApplication->closeConnectingDialog();
        if (EIR_UseNetwork_StartingView == getUseNetworkReason())
        {
            IRLastPlayedStationInfo *lastPlayedStationInfo =
                    iApplication->getLastPlayedStationInfo();
            IRQPreset *preset = lastPlayedStationInfo->getLastPlayedStation();
            if (preset)
            {
                iPlayController->connectToChannel(preset,lastPlayedStationInfo->connectedFrom());
            }
        }
        else if (EIR_UseNetwork_PlayStation == getUseNetworkReason())
        {
            playStopControl();
        }

        setUseNetworkReason(EIR_UseNetwork_NoReason);
        
        break;
        
    case EIRQConnectingCancelled:
    case EIRQDisplayNetworkMessageNoConnectivity:
        {
            IRQPreset *preset = iPlayController->getNowPlayingPreset();
            //if IRPlayController::connectToChannel(IRQPreset*) hasn't been called, back to main view.
            //otherwise stay in nowplaying view.
            if (NULL == preset || preset->name == "")
            {
                getViewManager()->activateView(EIRView_MainView);
            }
        }
        break;
        
    default:
        break;
    }
}

//                                       private functions

/*
 * Description : create menu for now playing view
 */
void IRNowPlayingView::createMenu()
{
    iStereoAction = qobject_cast<HbAction *> (iLoader.findObject("activateStereo"));
    iStereoMode = iApplication->getSettings()->getStereoMode();
    if (iStereoMode == 1)
    {
        iStereoAction->setText(hbTrId("txt_irad_opt_deactivate_stereo"));
    }
    HbAction *openWebAddressAction = qobject_cast<HbAction *> (iLoader.findObject("openWebAddressAction"));
    HbAction *shareStationAction = qobject_cast<HbAction *> (iLoader.findObject("shareStationAction"));
    HbAction *songRecAction = qobject_cast<HbAction *> (iLoader.findObject("songRecAction"));
    HbAction *gotoMusicStoreAction = qobject_cast<HbAction *> (iLoader.findObject("gotoMusicStoreAction"));
    HbAction *settings = qobject_cast<HbAction *> (iLoader.findObject("settings"));
    HbAction *helpAction = qobject_cast<HbAction *> (iLoader.findObject("help"));
    HbAction *exitAction = qobject_cast<HbAction *> (iLoader.findObject("exit"));

    connect(iStereoAction, SIGNAL(triggered()), this, SLOT(stereoControl()));
    connect(openWebAddressAction, SIGNAL(triggered()), this, SLOT(openWebAddressControl()));
    connect(shareStationAction, SIGNAL(triggered()), this, SLOT(shareStationViaMms()));
    connect(songRecAction, SIGNAL(triggered()), this, SLOT(recognizeSong()));
    connect(gotoMusicStoreAction, SIGNAL(triggered()), this, SLOT(gotoMusicStore()));
    connect(settings, SIGNAL(triggered()), this, SLOT(launchSettingsView()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(helpAction()));
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));

    HbMenu *viewMenu = menu();
    connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(prepareMenu()));    
}

/*
 * Description : create tool bar for now playing view
 */
void IRNowPlayingView::createToolBar()
{
    iMusicShopAction = qobject_cast<HbAction *> (iLoader.findObject("musicShop"));
    iPlayStopAction = qobject_cast<HbAction *> (iLoader.findObject("playStop"));
    iAdd2FavAction = qobject_cast<HbAction *> (iLoader.findObject("addToFav"));
    iFlipAction = qobject_cast<HbAction *> (iLoader.findObject("flip"));

    iMusicShopAction->setAutoRepeat(false);
    iPlayStopAction->setAutoRepeat(false);
    iAdd2FavAction->setAutoRepeat(false);
    iFlipAction->setAutoRepeat(false);
    connect(iMusicShopAction, SIGNAL(triggered()), this, SLOT(findInMusicShop()));
    connect(iPlayStopAction, SIGNAL(triggered()), this, SLOT(playStopControl()));
    connect(iAdd2FavAction, SIGNAL(triggered()), this, SLOT(add2FavControl()));
    connect(iFlipAction, SIGNAL(triggered()), this, SLOT(flipControl()));
}

/*
 * Description : update station name and category according to the information in preset
 */
void IRNowPlayingView::updateStationInfo()
{
    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    const QString &stationName = nowPlayingPreset->name;
    iNowPlayingContainer->setStationName(stationName);

    QString category = nowPlayingPreset->genreName + " / "
            + nowPlayingPreset->countryName + " / "
            + nowPlayingPreset->languageName;
    if (" /  / " == category)
    {
        category = "";
    }
    iNowPlayingContainer->setCategory(category);
}

/*
 * Description : Update advertisement image
 */
void IRNowPlayingView::updateAdvImage()
{
    // TODO: in the future, it should judge the change of advertisement url
    if (ELogoDetermined == iLogoStatus || ERequestIssued == iLogoStatus)
    {
        return;
    }

    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();

    // TODO: initialize adv, in the future it will use default adv image
    QIcon icon(":/playback/default_cd.png");
    HbIcon hbIcon(icon);
    iNowPlayingContainer->setAdvImage(hbIcon);

    if (nowPlayingPreset->type)
    {
        // isds server channel
        // TODO: it should use adv in preset, now the variants is not clear
        if (nowPlayingPreset->advertisementInUse.size() > 0
                && iAdvImageUrl != nowPlayingPreset->advertisementInUse)
        {
            iAdvImageUrl = nowPlayingPreset->advertisementInUse;
            iTempAdvClickThroughUrl = nowPlayingPreset->advertisementUrl;
            IRQPreset advPreset;
            advPreset.imgUrl = iAdvImageUrl;
            advPreset.type = 1;
            iGettingAdv = true;
            iIsdsClient->isdsLogoDownSendRequest(&advPreset);
        }
        else
        {
            updateLogoInfo();
        }
    }
    else
    {
        updateLogoInfo();
    }
}

/*
 * Description : Update logo info
 */
void IRNowPlayingView::updateLogoInfo()
{
    if (ELogoDetermined == iLogoStatus || ERequestIssued == iLogoStatus
            || iGettingAdv)
    {
        return;
    }

    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();

    iNowPlayingContainer->setLogo(*iStationLogo);
    if (nowPlayingPreset->type)
    {
        // isds server channel
        iLogoStatus = ERequestIssued;
        iIsdsClient->isdsLogoDownSendRequest(nowPlayingPreset, 2, KNowPlayingLogoSize, KNowPlayingLogoSize);
    }
    else
    {
        // users defined channel
        if (iStationDetailsContainer)
        {
            iStationDetailsContainer->setLogo(*iStationLogo);
        }
        iLogoStatus = ELogoDetermined;
    }
}

/*
 * Description: show container
 */
void IRNowPlayingView::showWidget()
{
    if (iStationDetailsContainer)
    {
        IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
        iStationDetailsContainer->setDetails(nowPlayingPreset, *iStationLogo);
    }

    updateMusicStatus();
    updateStationInfo();
    updateSongInfo(iPlayController->getMetaData());
    updateAdvImage();
}

/*
 * Description: initializing.
 */
void IRNowPlayingView::initialize()
{
    iMusicShop = iApplication->getMusicShop();
    iStatisticsReporter = iApplication->getStatisticsReporter();
}



/*
* Description : slot function for music shop button in toolbar
*/
void IRNowPlayingView::findInMusicShop()
{
    if(!iFindinNmsAllowed)
    {
        popupNote(hbTrId("txt_irad_info_disallowed_by_this_station"), HbMessageBox::MessageTypeInformation);
        return;        
    }
    
    if(!iSongNameAvailable)
    {
        popupNote(hbTrId("txt_irad_info_no_song_info"), HbMessageBox::MessageTypeInformation);
        return;        
    }
        
    if(NULL == iMusicShop)
    {
        popupNote(hbTrId("txt_irad_info_music_shop_not_available"), HbMessageBox::MessageTypeInformation);
        return;
    }
    
    IRQMetaData *metaData = iPlayController->getMetaData();
    if( iMusicShop->findInMusicShop(metaData->getSongName(), metaData->getArtistName()))
    {
        if(iStatisticsReporter)
        {
            int channelId = iPlayController->getNowPlayingPreset()->presetId;
            iStatisticsReporter->logNmsEvent(EIRQFind,channelId);
        }
    }
    else
    {
        popupNote(hbTrId("txt_irad_info_music_shop_not_available"), HbMessageBox::MessageTypeInformation);
    }
}

void IRNowPlayingView::helpAction()
{
    popupNote(hbTrId("Not Ready"), HbMessageBox::MessageTypeInformation);
}

void IRNowPlayingView::playPressed()
{
    if (getViewManager()->currentView() == this)
    {
        iPlayController->resume();
    }
}

void IRNowPlayingView::gotoMusicStore()
{
    if(IRQMusicShop::launchMusicShop())
    {
        if(iStatisticsReporter)
        {
            int channelId = 0;
            if(iPlayController)
            {
                IRQPreset *preset = iPlayController->getNowPlayingPreset();
                if(preset)
                {
                    channelId = preset->presetId;
                }	      
            }            
            iStatisticsReporter->logNmsEvent(EIRQLaunch,channelId);
        }        
    }
    else
    {
        popupNote(hbTrId("txt_irad_info_music_shop_not_available"), HbMessageBox::MessageTypeInformation);
    }
}

void IRNowPlayingView::updateMusicStatus()
{
    IRQPreset *preset = iPlayController->getNowPlayingPreset();
    if(preset && (0 == preset->musicStoreStatus.compare("yes",Qt::CaseInsensitive)))
    {
        iFindinNmsAllowed = true;
    }
    else
    {
        iFindinNmsAllowed = false;
    }
}

void IRNowPlayingView::recognizeSong()
{
    if(IRQMusicShop::launchShazam())
    {
        if(iStatisticsReporter)
        {           
            iStatisticsReporter->logSongRecogEvent();    
        }   
    }
    else
    {
        popupNote(hbTrId("txt_irad_info_song_recognition_not_available"), HbMessageBox::MessageTypeInformation);
    }
}

void IRNowPlayingView::openAdvLink()
{
    IRQUtility::openAdvLink(iAdvClickThroughUrl);
}
void IRNowPlayingView::shareStationViaMms()
{
    popupNote(hbTrId("Not Ready"), HbMessageBox::MessageTypeInformation);
}

void IRNowPlayingView::prepareMenu()
{
    HbAction *songRecAction = qobject_cast<HbAction *> (iLoader.findObject("songRecAction"));
    songRecAction->setEnabled(iPlayController->isPlaying());
}
