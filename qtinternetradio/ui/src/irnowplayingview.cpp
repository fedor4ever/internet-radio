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
#include <hbtoolbar.h>
#include <QPixmap>
#include <hbaction.h>
#include <hblabel.h>
#include <hbnotificationdialog.h>
#include <QSettings>

#ifdef NOWPLAYING_VIEW_OPTION_B
#include <hbframedrawer.h>
#include <hbframeitem.h>
#endif


#include "irviewmanager.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irqisdsdatastructure.h"
#include "irqmetadata.h"
#include "irqisdsclient.h"
#include "irqfavoritesdb.h"
#include "irlastplayedstationinfo.h"
#include "irqnetworkcontroller.h" 
#include "irqstatisticsreporter.h"
#include "irqenums.h"
#include "irmediakeyobserver.h"
#include "iruidefines.h"
#include "irnowplayingview.h"
#include "irstationshare.h"
#include "irqutility.h"
#include "irqlogger.h"
#include "irplaylist.h"
#include "irstationdetailsview.h"
#include "irservicedef.h"

#ifdef ADV_ENABLED
#include <QTimer>
#include <qgraphicssceneevent.h>
static const QString KDefaultAdvLink(""); // default advertisement link
#endif

const int KNowPlayingLogoSize = 300; // Now playing logo size
static const QString KDefaultStationLogo("qtg_large_internet_radio");
static const QString KPlayButtonIcon("qtg_mono_play");
static const QString KStopButtonIcon("qtg_mono_stop");

#ifdef NOWPLAYING_VIEW_OPTION_B
static const QString KLcdGraphics("qtg_fr_lcd");
#endif


static void saveStationLogo(const QPixmap &aStationLogo);

/*
 * Description : constructor
 */
IRNowPlayingView::IRNowPlayingView(IRApplication* aApplication, TIRViewId aViewId) :
    IRBaseView(aApplication, aViewId),
    iStatisticsReporter(NULL),
    iStationShare(NULL),
    iPlayStopAction(NULL),
    iLaunchActionNeeded(false),
    iLogoDownloadState(EIdle),    
    iSongName(NULL),
    iArtistName(NULL),
    iStationName(NULL),
    iStationLogo(NULL),
    iLogoNeedUpdate(true),    
    iFindinNmsAllowed(false),
    iSongNameAvailable(false)
#ifdef ADV_ENABLED
    ,iAdvImage(NULL)
    ,iAdvImageNeedUpdate(true)
    ,iAdvUrl(KDefaultAdvLink)    
#endif
{
    initialize();
    
    //if this view is not starting view, finish all initialization in constructor
    if (getViewManager()->views().count() > 0)
    {
        normalInit();
    }
    
    setFlag(EViewFlag_StickyViewEnabled);
}

/*
 * Description : destructor
 */
IRNowPlayingView::~IRNowPlayingView()
{
    if (iStatisticsReporter)
    {
        iStatisticsReporter->closeInstance();
    }
    
    delete iStationShare;        
}


/*
 * Description: initializing.
 */
void IRNowPlayingView::initialize()
{
    setObjectName(NOW_PLAYING_VIEW_OBJECT_NAME);
    QObjectList roots;
    roots.append(this);
    iLoader.setObjectTree(roots);

    // Load XML file
    iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME);
    
    initMenu();
    initToolBar();
    initWidget();    
}

/*
 * Description : create menu for now playing view
 */
void IRNowPlayingView::initMenu()
{
#ifdef SONGRECOGNITION_ENABLED  
    if(IRQUtility::identifySongAvailable())
    {
        LOG("Song Recognition is Available");
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, NOW_PLAYING_VIEW_SONG_RECOG_YES_SEC);
    }
    else
    {
        LOG("Song Recognition is NOT Available");
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, NOW_PLAYING_VIEW_SONG_RECOG_NO_SEC);
    }
#else
    LOG("Song Recognition is Disabled");
    iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, NOW_PLAYING_VIEW_SONG_RECOG_NO_SEC);
#endif    
    HbAction *openWebAddressAction = qobject_cast<HbAction *> (iLoader.findObject(GO_TO_STATION_ACTION));
    HbAction *shareStationAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_SHARE_STATION_ACTION));
    HbAction *settings = qobject_cast<HbAction *> (iLoader.findObject(SETTINGS_ACTION));
    HbAction *exitAction = qobject_cast<HbAction *> (iLoader.findObject(EXIT_ACTION));
    HbAction *songRecAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_IDENTIFYSONG));

    connect(openWebAddressAction, SIGNAL(triggered()), this, SLOT(handleGoToStationAction()));
    connect(shareStationAction, SIGNAL(triggered()), this, SLOT(handleShareStationAction()));
    connect(settings, SIGNAL(triggered()), this, SLOT(handleSettingAction()));
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));  
    connect(songRecAction, SIGNAL(triggered()), this, SLOT(handleIdentifySongAction()));    
}

/*
 * Description : create tool bar for now playing view
 */
void IRNowPlayingView::initToolBar()
{
    HbAction *musicStoreAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_MUSICSTORE));
    iPlayStopAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_PLAYSTOP));
    HbAction *add2FavAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_ADDTOFAV));
    HbAction *flipAction = qobject_cast<HbAction *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_FLIP));

    connect(musicStoreAction, SIGNAL(triggered()), this, SLOT(handleMusicStoreAction()));
    connect(iPlayStopAction, SIGNAL(triggered()), this, SLOT(handlePlayStopAction()));
    connect(add2FavAction, SIGNAL(triggered()), this, SLOT(handleAddToFavAction()));
    connect(flipAction, SIGNAL(triggered()), this, SLOT(handleDetailInfoAction()));
    
    //could be removed after toolbar issue is ok
    HbToolBar *viewToolBar = toolBar();
    viewToolBar->addAction(musicStoreAction);
    viewToolBar->addAction(iPlayStopAction);
    viewToolBar->addAction(add2FavAction);
    viewToolBar->addAction(flipAction);
}

void IRNowPlayingView::initWidget()
{
    iStationLogo = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_STATION_LOGO));
    iSongName = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_SONG_NAME));
    iArtistName = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_ARTIST_NAME));
    iStationName = qobject_cast<HbLabel *> (iLoader.findObject(NOW_PLAYING_VIEW_OBJECT_STATION_NAME));   
    iStationLogo->setIcon(HbIcon(KDefaultStationLogo));   
    iStationName->setPlainText("");
    iSongName->setPlainText("");
    iArtistName->setPlainText("");    
    
#ifdef NOWPLAYING_VIEW_OPTION_B
	HbWidget * viewContainer = qobject_cast<HbWidget *> (iLoader.findObject(VIEW_CONTAINER));
    HbFrameDrawer* drawer = new HbFrameDrawer(KLcdGraphics, HbFrameDrawer::NinePieces);
    HbFrameItem* backgroundItem = new HbFrameItem(drawer, viewContainer);
    if (backgroundItem)
    {
        viewContainer->setBackgroundItem(backgroundItem);
    }
#endif
    
#ifdef ADV_ENABLED
    iAdvImage = qobject_cast<HbLabel *> (iLoader.findObject( NOW_PLAYING_VIEW_OBJECT_ADVERTISEMENT_IMAGE));
    iAdvImage->setIcon(HbIcon(KDefaultStationLogo));
#endif    
}

/*
 * Description : update station name and category according to the information in preset
 */
void IRNowPlayingView::updateWidgets()
{
    if (iLaunchActionNeeded) // if lunch as starting view, leave the update action to launchAction()
    {
        return;
    }
    
    if(iPlayController->getNowPlayingPreset())
    {
        iStationName->setPlainText(iPlayController->getNowPlayingPreset()->name);
    }
    loadStationLogo();
}

void IRNowPlayingView::updateMusicStoreStatus()
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


/*
 * Description : Update logo info
 */
void IRNowPlayingView::updateStationLogo()
{    
    if( !iLogoNeedUpdate
        || iLogoDownloadState != EIdle )
    {
        return;
    }
    
    if( !iNetworkController->getNetworkStatus() )
    {
        // when launch as starting view, reach here.
        // but later, will be invoked again via handlePlayStarted()
        return;
    }    

    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    if( 1 == nowPlayingPreset->type )
    {
        // isds server channel
        iLogoDownloadState = EDownloadLogo; 
        iIsdsClient->isdsLogoDownSendRequest(nowPlayingPreset, 2, KNowPlayingLogoSize, KNowPlayingLogoSize);    
    }
    else
    {
        iLogoDownloadState = EIdle;
        iLogoNeedUpdate = false; 
#ifdef ADV_ENABLED        
        iAdvImageNeedUpdate = false;  
#endif             
    }
}

#ifdef ADV_ENABLED
/*
 * Description : Update advertisement image
 */
void IRNowPlayingView::updateAdvImage()
{
    if( !iAdvImageNeedUpdate
        || iLogoDownloadState != EIdle )
    {
        return;
    }

    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    if( (1 == nowPlayingPreset->type)  // isds server channel
        && (nowPlayingPreset->advertisementInUse.size() > 0) )
    {
        IRQPreset advPreset;
        advPreset.imgUrl = nowPlayingPreset->advertisementInUse;
        advPreset.type = 1;
        iLogoDownloadState = EDownLoadAdvImage;        
        iIsdsClient->isdsLogoDownSendRequest(&advPreset);
    }
    else
    {
        iLogoNeedUpdate = false;
        iAdvImageNeedUpdate = false;
        iLogoDownloadState = EIdle;
    }
}
#endif

/*
 * Description : from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRNowPlayingView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    if (!initCompleted())
    {
        return EIR_NoDefault;
    }
    
    Q_UNUSED(aReason);
    TIRHandleResult ret = EIR_DoDefault;

    switch (aCommand)
    {
    case EIR_ViewCommand_TOBEACTIVATED:       
        handleOrientationChanged(getViewManager()->orientation());                 
        updateMusicStoreStatus();
        updateWidgets();
        ret = EIR_NoDefault;
        break;

    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(handleLogoDownloaded(IRQPreset* )));
        updateStationLogo();
        getViewManager()->saveScreenShot();
        break;
        
    case EIR_ViewCommand_DEACTIVATE:
        if( iLogoDownloadState != EIdle )
        {
            // cancel downloading logo
            iIsdsClient->isdsLogoDownCancelTransaction();
            iLogoDownloadState = EIdle;            
        }        
        disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
                   this, SLOT(handleLogoDownloaded(IRQPreset* )));
        ret = EIR_NoDefault;
        break;
    }

    return ret;
}

/*
 * Description : actions when view is launched.
 */
void IRNowPlayingView::launchAction()
{      
    setUseNetworkReason(EIR_UseNetwork_StartingView);
    updateForLauchAction();
#ifdef HS_WIDGET_ENABLED	
    iPlayController->setConnectingStationName(iStationName->plainText());
#endif	
    iApplication->verifyNetworkConnectivity();
    getViewManager()->pushViewById(EIRView_MainView);
    iLaunchActionNeeded = false;
}

void IRNowPlayingView::lazyInit()
{
    iLaunchActionNeeded = true;
    
    if (!initCompleted())
    {       
        normalInit();
        
        //initization from handleCommand()
        handleCommand(EIR_ViewCommand_TOBEACTIVATED, EIR_ViewCommandReason_Show);
        handleCommand(EIR_ViewCommand_ACTIVATED, EIR_ViewCommandReason_Show);
    }
}

void IRNowPlayingView::normalInit()
{
    if (!initCompleted())
    {
        IRBaseView::lazyInit();
        
        iStatisticsReporter = IRQStatisticsReporter::openInstance();
            
        connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(updateMetaData(IRQMetaData*)));
        connect(iPlayController, SIGNAL(playingStarted()), this, SLOT(handlePlayStarted()));
        connect(iPlayController, SIGNAL(playingStopped()), this, SLOT(handlePlayStopped()));

        connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
                this, SLOT(handleNetworkEvent(IRQNetworkEvent)));
        connect(iApplication->getMediaKeyObserver(), SIGNAL(playPausePressed()), 
                this, SLOT(handlePlayPauseMediaKey()));
        connect(iApplication->getMediaKeyObserver(), SIGNAL(stopPressed()), 
                this, SLOT(handleStopMediaKey()));
        connect( getViewManager(), SIGNAL( orientationChanged(Qt::Orientation) ),
                 this, SLOT( handleOrientationChanged(Qt::Orientation) ) );
        
        setInitCompleted(true);
    }
}

void IRNowPlayingView::updateForLauchAction()
{
    //nowplaying view as starting view can have two cases : one is for last played station, the other is for 
    //the only one item in play list file    
    IRPlayList *playList = iApplication->getPlayList();
    if (playList && 1 == playList->getNumberOfEntries())
    {
        IRQPreset *preset = playList->getPresetForEntry(0);
        iStationName->setPlainText(preset->name);
        iFindinNmsAllowed = (0 == preset->musicStoreStatus.compare("yes",Qt::CaseInsensitive));
#ifdef HS_WIDGET_ENABLED            
        iPlayController->reloadNowplayingPreset(preset,false,EIRQAdhocExternal);
#endif
        iStationLogo->setIcon(HbIcon(KDefaultStationLogo));
        iLogoNeedUpdate = true;            
#ifdef ADV_ENABLED
        iAdvUrl = KDefaultAdvLink; 
        iAdvImage->setIcon(logoHbIcon);
        iAdvImageNeedUpdate = true;  
#endif
        delete preset;
    }
    else
    {
        IRLastPlayedStationInfo *lastPlayedStationInfo = iApplication->getLastPlayedStationInfo();
        IRQPreset *lastPreset = lastPlayedStationInfo->getLastPlayedStation();
        if (lastPreset)
        {
            iStationName->setPlainText(lastPreset->name); 
            iFindinNmsAllowed = (0 == lastPreset->musicStoreStatus.compare("yes",Qt::CaseInsensitive));
        }
        else
        {
            iStationName->setPlainText(QString("")); 
            iFindinNmsAllowed = false;
        }
        loadStationLogo();
    }
}

/********************************************************************************************************
 * SLOTs for Logo Download
 *******************************************************************************************************
 */
void IRNowPlayingView::handleLogoDownloaded(IRQPreset* aPreset)
{
    if( EIdle == iLogoDownloadState )
    {        
        return;
    } 
            
    if( NULL == aPreset )
    {        
        if( EDownloadLogo == iLogoDownloadState )
        {         
            iLogoNeedUpdate = false;  
#ifdef ADV_ENABLED
            QTimer::singleShot(1, this, SLOT(updateAdvImage()));
#endif            
        }
#ifdef ADV_ENABLED      
        else if( EDownLoadAdvImage == iLogoDownloadState )
        {
            iAdvImageNeedUpdate = false;            
        }
#endif
        iLogoDownloadState = EIdle;      
        return;
    }
    
 
    
    const unsigned char * logoRawData = aPreset->logoData.Ptr();
    QPixmap logoPixmap;
    if( logoPixmap.loadFromData(logoRawData, aPreset->logoData.Length()) )
    {
        if( EDownloadLogo == iLogoDownloadState )
        {		
            saveStationLogo(logoPixmap);
            QPixmap newLogoPixmap = 
                 logoPixmap.scaled(QSize(KNowPlayingLogoSize,KNowPlayingLogoSize),Qt::KeepAspectRatio);
            QIcon logoQIcon(newLogoPixmap);
            HbIcon logoHbIcon(logoQIcon);
            iStationLogo->setIcon(logoHbIcon);
            iPlayController->emitStationLogoUpdated(true);
            iLogoNeedUpdate = false;          
            getViewManager()->saveScreenShot();
#ifdef ADV_ENABLED
            QTimer::singleShot(1, this, SLOT(updateAdvImage()));
#endif
        }
#ifdef ADV_ENABLED      
        else if( EDownLoadAdvImage == iLogoDownloadState )
        {
            QIcon logoQIcon(logoPixmap);
            HbIcon logoHbIcon(logoQIcon);            
            iAdvImage->setIcon(logoHbIcon); 
            iAdvUrl = iPlayController->getNowPlayingPreset()->advertisementUrl;  
            iAdvImageNeedUpdate = false;            
        }
#endif
    }
    else
    {
        if( EDownloadLogo == iLogoDownloadState )
        {         
            iLogoNeedUpdate = false;  
#ifdef ADV_ENABLED
            QTimer::singleShot(1, this, SLOT(updateAdvImage()));
#endif            
        }
#ifdef ADV_ENABLED      
        else if( EDownLoadAdvImage == iLogoDownloadState )
        {
            iAdvImageNeedUpdate = false;            
        }
#endif        
    }

    iLogoDownloadState = EIdle;      
    
    delete aPreset;
    aPreset = NULL;
}



/********************************************************************************************************
 * SLOT for Network Event
 *******************************************************************************************************
 */
void IRNowPlayingView::handleNetworkEvent(IRQNetworkEvent aEvent)
{
    if( this != getViewManager()->currentView() )
    {
        return;
    }
    
    switch(aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        {
            if( EIR_UseNetwork_StartingView == getUseNetworkReason() )
            {
                IRPlayList *playList = iApplication->getPlayList();
                if (playList && 1 == playList->getNumberOfEntries())
                {
                    IRQPreset *preset = playList->getPresetForEntry(0);
                    iPlayController->connectToChannel(preset, EIRQAdhocExternal);
                    delete preset;
                }
                else
                {
                    IRLastPlayedStationInfo *lastPlayedStationInfo = iApplication->getLastPlayedStationInfo();
                    IRQPreset *lastPreset = lastPlayedStationInfo->getLastPlayedStation();
                    if (lastPreset)
                    {
                        iPlayController->connectToChannel( lastPreset, lastPlayedStationInfo->connectedFrom() );
                    }
                }
            }
            else if( EIR_UseNetwork_PlayStation == getUseNetworkReason() )
            {
                iPlayController->resume();
            }
            break;
        }
    default:
        break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

/********************************************************************************************************
 * SLOT for Orientation Changed
 *******************************************************************************************************
 */

void IRNowPlayingView::handleOrientationChanged(Qt::Orientation aOrientation)
{
    if (aOrientation == Qt::Vertical)
    {
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, PORTRAIT_SEC);
    }
    else
    {
        iLoader.load(NOW_PLAYING_VIEW_LAYOUT_FILENAME, LANDSCAPE_SEC);      
    }
}

/********************************************************************************************************
 * SLOT for Play Controller
 *******************************************************************************************************
 */
void IRNowPlayingView::handlePlayStarted()
{
    iPlayStopAction->setIcon(HbIcon(KStopButtonIcon));
}

void IRNowPlayingView::handlePlayStopped()
{
    iPlayStopAction->setIcon(HbIcon(KPlayButtonIcon));
    iSongName->setPlainText("");
    iArtistName->setPlainText("");
    iSongNameAvailable = false;
}

void IRNowPlayingView::updateMetaData(IRQMetaData* aMetaData)
{
    iSongNameAvailable = false;
    if(aMetaData)
    {
        iSongName->setPlainText(aMetaData->getSongName());
        iArtistName->setPlainText(aMetaData->getArtistName());
        if(!aMetaData->getSongName().isEmpty() ||
           !aMetaData->getArtistName().isEmpty())
        {
            iSongNameAvailable = true;
        }
    }
    else
    {
        iSongName->setPlainText("");
        iArtistName->setPlainText("");  
    }
}

/********************************************************************************************************
 * SLOT for Media Key
 *******************************************************************************************************
 */
void IRNowPlayingView::handlePlayPauseMediaKey()
{
    iPlayController->resume();
}

void IRNowPlayingView::handleStopMediaKey()
{
    iPlayController->stop(EIRQUserTerminated);
}

/********************************************************************************************************
 * SLOT for Toolbar Action
 *******************************************************************************************************
 */
void IRNowPlayingView::handleMusicStoreAction()
{
    if(!iFindinNmsAllowed)
    {
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_not_allowed_by_this_station"), HbMessageBox::MessageTypeInformation);
#else
        popupNote(hbTrId("Not allowed by this station"), HbMessageBox::MessageTypeInformation);        
#endif
        return;        
    }
    
    if(!iSongNameAvailable)
    {
        popupNote(hbTrId("txt_irad_info_no_song_info"), HbMessageBox::MessageTypeInformation);
        return;        
    }
    
    // Need to log the find song in NMS event, iStatisticsReporter->logNmsEvent(IRQStatisticsReporter::EIRFind,channelId); 
#ifdef SUBTITLE_STR_BY_LOCID
    popupNote(hbTrId("txt_irad_info_music_store_not_available"), HbMessageBox::MessageTypeInformation);
#else
    popupNote(hbTrId("Music store not available"), HbMessageBox::MessageTypeInformation);    
#endif
}

void IRNowPlayingView::handleIdentifySongAction()
{
    if(IRQUtility::openSongRecognition())
    {
        iStatisticsReporter->logSongRecogEvent();
    }
}


void IRNowPlayingView::handlePlayStopAction()
{
    switch (iPlayController->state())
    {
        case IRPlayController::EPlaying:
            iPlayController->stop(EIRQUserTerminated);
            return;
                    
        case IRPlayController::EStopped:
            setUseNetworkReason(EIR_UseNetwork_PlayStation);
            break;
            
        default:
            return;
    }
    
 
#ifdef HS_WIDGET_ENABLED		
    iPlayController->setConnectingStationName(iStationName->plainText());         
#endif
		
    if (iApplication->verifyNetworkConnectivity())
    {
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        iPlayController->resume();
    }
}

void IRNowPlayingView::handleAddToFavAction()
{
    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    int retValue = iFavorites->addPreset(*nowPlayingPreset);
    HbNotificationDialog *add2FavNote = new HbNotificationDialog();
    add2FavNote->setModal(true);
    add2FavNote->setAttribute(Qt::WA_DeleteOnClose);
    switch (retValue)
    {
    case EIRQErrorNone:
#ifdef SUBTITLE_STR_BY_LOCID       
        add2FavNote->setTitle(hbTrId("txt_irad_info_added_to_favorites"));
#else
        add2FavNote->setTitle(hbTrId("Added to Favorites"));         
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();
        break;

    case EIRQErrorOutOfMemory:
#ifdef SUBTITLE_STR_BY_LOCID 
        add2FavNote->setTitle(hbTrId("txt_irad_info_can_not_add_more"));
#else
        add2FavNote->setTitle(hbTrId("Can't add more"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();        
        break;

    case EIRQErrorAlreadyExist:
#ifdef SUBTITLE_STR_BY_LOCID 
        add2FavNote->setTitle(hbTrId("txt_irad_info_favorite_updated"));
#else
        add2FavNote->setTitle(hbTrId("Favorite updated"));        
#endif
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();           
        break;

    default:
        Q_ASSERT(false);
        break;
    }
}

void IRNowPlayingView::handleDetailInfoAction()
{
    getViewManager()->activateView(EIRView_StationDetailsView);
    IRStationDetailsView *channelHistoryView = static_cast<IRStationDetailsView*>(getViewManager()->getView(EIRView_StationDetailsView));
    channelHistoryView->setDetails();

}

/********************************************************************************************************
 * SLOT for Menu Action
 *******************************************************************************************************
 */
void IRNowPlayingView::handleGoToStationAction()
{
    getViewManager()->activateView(EIRView_OpenWebAddressView);
}

void IRNowPlayingView::handleShareStationAction()
{
    if (NULL == iStationShare)
    {
        iStationShare = new IRStationShare();
    }
    iStationShare->shareStations(*iPlayController->getNowPlayingPreset());
}

void IRNowPlayingView::handleSettingAction()
{
    getViewManager()->activateView(EIRView_SettingsView);
}

#ifdef ADV_ENABLED
void IRNowPlayingView::mousePressEvent(QGraphicsSceneMouseEvent *aEvent)
{
    QRectF advRect = iAdvImage->geometry();
    QPointF pos = aEvent->pos();
    
    if (advRect.contains(pos))
    {
        openAdvLink();
    }
}

void IRNowPlayingView::openAdvLink()
{
    IRQUtility::openAdvLink(iAdvUrl);
}
#endif

void IRNowPlayingView::loadStationLogo()
{
    if (iPlayController->isStationLogoAvailable())    
    { 
        QSettings settings(KIrSettingOrganization, KIrSettingApplication);
        if (settings.value(KIrSettingStationLogo).canConvert<QPixmap>())
        {
            QPixmap logoPixmap = settings.value(KIrSettingStationLogo).value<QPixmap>();
            QPixmap newLogoPixmap = 
                 logoPixmap.scaled(QSize(KNowPlayingLogoSize,KNowPlayingLogoSize),Qt::KeepAspectRatio);
            QIcon logoQIcon(newLogoPixmap);
            HbIcon logoHbIcon(logoQIcon);
            iStationLogo->setIcon(logoHbIcon);
            iLogoNeedUpdate = false;            
#ifdef ADV_ENABLED
            iAdvUrl = iPlayController->getNowPlayingPreset()->advertisementUrl; 
            iAdvImage->setIcon(logoHbIcon);
            iAdvImageNeedUpdate = false;  
#endif            	
            return;
        }  
    }

    iStationLogo->setIcon(HbIcon(KDefaultStationLogo));
    iLogoNeedUpdate = true;            
#ifdef ADV_ENABLED
    iAdvUrl = KDefaultAdvLink; 
    iAdvImage->setIcon(logoHbIcon);
    iAdvImageNeedUpdate = true;  
#endif
}

void saveStationLogo(const QPixmap &aStationLogo)
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    QVariant logoData(QVariant::Pixmap);
    logoData.setValue(aStationLogo);
    settings.setValue(KIrSettingStationLogo,logoData);    
}
