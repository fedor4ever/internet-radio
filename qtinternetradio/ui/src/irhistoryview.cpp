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
#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbnotificationdialog.h>
#include <QPixmap>
#include <QTimer>

#include "irviewmanager.h"
#include "irapplication.h"
#include "irqisdsclient.h"
#include "irplaycontroller.h"
#include "irhistoryview.h"
#include "irqisdsdatastructure.h"
#include "irhistorymodel.h"
#include "irqsonghistoryinfo.h"
#include "irqnetworkcontroller.h" 
#include "irqutility.h"
#include "irqenums.h"
#include "irqfavoritesdb.h"
#include "irstationdetailsview.h"
#include "iruidefines.h"


const int KBitmapSize = 59;
const QString KActionAddName("Add");
const QString KActionDeleteName("Delete");
const QString KActionDetailsName("Details");

//                                         public functions

/*
 * Description : constructor
 */
IRHistoryView::IRHistoryView(IRApplication *aApplication, TIRViewId aViewId) :
    IrAbstractListViewBase(aApplication, aViewId), iClearHistoryAction(NULL),
    iLogoPreset(NULL)
{
    //this view won't be starting view, don't need lazy init
    IrAbstractListViewBase::lazyInit();
    setInitCompleted(true);
    
    iModel = new IRHistoryModel(this);
    iListView->setModel(iModel);
    iListView->setCurrentIndex(iModel->index(0));
    
    iClearHistoryAction = new HbAction(hbTrId("txt_irad_menu_clear_list"), menu());

    
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10);
    
    connect(iClearHistoryAction, SIGNAL(triggered()), this, SLOT(clearAllList()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
    this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));
    connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
}

void IRHistoryView::gotoSongHistory()
{
    getViewManager()->activateView(EIRView_SongHistoryView);
}

/* 
 * Description : destructor
 */
IRHistoryView::~IRHistoryView()
{
    delete iLogoPreset;
    iLogoPreset = NULL; 
}

/*
 * Description : virtual functions from base class IRBaseView. 
 *               handle view commands
 * 
 * see also    : IRBaseView::handleCommand
 */
TIRHandleResult IRHistoryView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    int leftCount = 0;
    
    switch (aCommand)
    {
        
    case EIR_ViewCommand_TOBEACTIVATED:       
        showHistory();
        ret = EIR_NoDefault;
        break;
                
    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                this, SLOT(presetResponse(IRQPreset *)));
        connect(iIsdsClient, SIGNAL(operationException(IRQError)),
                this, SLOT(operationException(IRQError)));
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(presetLogoDownload(IRQPreset* )));
        connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
                this, SLOT(presetLogoDownloadError()));
        
        leftCount = iIconIndexArray.count();
        if( leftCount > 0 )
        {
            iConvertTimer->start();
        }
        ret = EIR_NoDefault;
        break;

    case EIR_ViewCommand_DEACTIVATE:

        iModel->clearAndDestroyLogos();
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction();     
        
        //iIconIndexArray must be cleared, because timer call back convertAnother() might be
        //called after view is deactivated. In that case, iModel->getImgURL(aIndex); will crash
        iIconIndexArray.clear();
                
        disconnect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                   this, SLOT(presetResponse(IRQPreset *)));
        disconnect(iIsdsClient, SIGNAL(operationException(IRQError)),
                   this, SLOT(operationException(IRQError)));
        disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
                   this, SLOT(presetLogoDownload(IRQPreset* )));
        disconnect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
                   this, SLOT(presetLogoDownloadError()));
        ret = EIR_NoDefault;
        break;

    default:
        break;
    }

    return ret;
}

//                                 slots functions 

/*
 * Description : slot function when an item in a list is clicked.
 *               issue a listen request to isds client
 * Parameters  : aItem : pointer to the clicked item.
 */
void IRHistoryView::handleItemSelected()
{
    int index = iListView->currentIndex().row();
    IRQSongHistoryInfo *hisInfo = iModel->getHistoryInfo(index);
    if (NULL == hisInfo)
    {
        return;
    }

    if (hisInfo->getChannelType())
    {
        // channel from isds server, get this preset
        iApplication->createLoadingDialog(this, SLOT(cancelRequest()));
        iIsdsClient->isdsListenRequest(hisInfo->getChannelId(), true);
    }
    else
    {
        // user defined channel
        IRQChannelServerURL server;
        server.bitrate = hisInfo->getBitrate();
        server.url = hisInfo->getStreamUrl();
        server.serverName = hisInfo->getChannelName();
        IRQPreset preset;
        preset.insertChannelServer(server);
        preset.name = hisInfo->getChannelName();
        preset.description = hisInfo->getChannelDesc();
        preset.shortDesc = hisInfo->getChannelDesc();
        preset.genreName = hisInfo->getGenreName();
        preset.countryName = hisInfo->getCountryName();
        preset.languageName = hisInfo->getLanguageName();
        preset.type = 0;
        preset.uniqID = 0;
        preset.presetId = 0;

        iPlayController->connectToChannel(&preset,EIRQHistoryAdhoc);
    }
}

// ---------------------------------------------------------------------------
// IRHistoryView::presetResponse()
// gets the preset from isds client and play
//---------------------------------------------------------------------------
void IRHistoryView::presetResponse(IRQPreset *aPreset)
{
    iPlayController->connectToChannel(aPreset,EIRQHistoryIsds);
}

void IRHistoryView::operationException(IRQError aError)
{
    Q_UNUSED(aError);
    iApplication->closeLoadingDialog();

    popupNote(hbTrId("txt_irad_info_failed_to_connect"), HbMessageBox::MessageTypeWarning);
}

void IRHistoryView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        if (EIR_UseNetwork_SelectItem == getUseNetworkReason())
        {
            handleItemSelected();
        }
        break;
        
    default:
        setCheckedAction();
        break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

void IRHistoryView::cancelRequest()
{
    iIsdsClient->isdsCancelRequest();
}

// ---------------------------------------------------------------------------
// IRHistoryView::showHistory()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRHistoryView::showHistory()
{
    if (iModel->checkHistoryUpdate())
    {
        iListView->reset();
        iListView->setCurrentIndex(iModel->index(0));

        //because we get all the history refreshed, so clear the icon array.
        iIconIndexArray.clear();

        //initialize the iconindices
        for (int i = 0; i < iModel->rowCount(); ++i)
        {
            if (iModel->getImageUrl(i) != "")
            {
                iIconIndexArray.append(i);
            }
        }  
    }
}

// ---------------------------------------------------------------------------
// IRHistoryView::clearAllList()
// gets the List which was stored earlier
//---------------------------------------------------------------------------
void IRHistoryView::clearAllList()
{
    iIconIndexArray.clear();
    iModel->clearAllList();
    iConvertTimer->stop();
    iIsdsClient->isdsLogoDownCancelTransaction();
    iListView->reset();
}

void IRHistoryView::prepareMenu()
{
    HbMenu *viewMenu = menu();
    
    viewMenu->removeAction(iClearHistoryAction);

    HbAction * settingAction = qobject_cast<HbAction *>(iLoader.findObject(SETTINGS_ACTION));

    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(settingAction, iClearHistoryAction);
    }
} 

void IRHistoryView::startConvert(int aIndex)
{
    QString url = iModel->getImageUrl(aIndex);

    IRQPreset tempPreset;
    tempPreset.imgUrl = url;
    tempPreset.type = IRQPreset::EIsds;
    iIsdsClient->isdsLogoDownSendRequest(&tempPreset, 0, KBitmapSize, KBitmapSize);
}

//if the logo is downloaded ok
void IRHistoryView::presetLogoDownload(IRQPreset* aPreset)
{
    if (NULL == aPreset)
    {
        presetLogoDownloadError();
        return;
    }

 
    delete iLogoPreset;             
    iLogoPreset = aPreset;
     
    if (iLogoPreset->logoData != KNullDesC8)
    {
        const unsigned char * logoData = iLogoPreset->logoData.Ptr();
        QPixmap tempMap;
        bool ret = tempMap.loadFromData(logoData,iLogoPreset->logoData.Length());
        if( ret )
        {
            QIcon convertIcon(tempMap);
            HbIcon *hbIcon = new HbIcon(convertIcon);
            int index = iIconIndexArray[0];
            iModel->setLogo(hbIcon, index);
            iIconIndexArray.removeAt(0);
            int leftCount = iIconIndexArray.count();
            if( leftCount > 0 )
            {
                iConvertTimer->start();  
            }
            return;
        }           
    } 
 
    presetLogoDownloadError();
}

 

//if the logo download fails
void IRHistoryView::presetLogoDownloadError()
{
    // if the logo download fails, try to download the next
    iIconIndexArray.removeAt(0);
    int leftCount = 0;
    leftCount = iIconIndexArray.count();
    if( leftCount > 0 )
    {
        iConvertTimer->start();
    }    
}

void IRHistoryView::convertAnother()
{     
    iConvertTimer->stop();
    int leftCount = iIconIndexArray.count();

    if (0 != leftCount)
    {
        startConvert(iIconIndexArray[0]);
    }
}

void IRHistoryView::modelChanged()
{
    QString headingStr = hbTrId("Station History") + " (" + QString::number(iModel->rowCount()) + ")";  
    setHeadingText(headingStr);
}
 
void IRHistoryView::actionClicked(HbAction *aAction)
{
    if ( aAction )
    {
        QString objectName = aAction->objectName();
        if ( objectName == KActionAddName )
        {
            addContextAction();
        }
        else if( objectName == KActionDeleteName)
        {
            deleteContextAction();
        }
        else if( objectName == KActionDetailsName)
        {
            detailsContextAction();
        }
    }
}

void IRHistoryView::addContextAction()
{        
    QModelIndex current = iListView->currentIndex();     
    IRQSongHistoryInfo * currentInfo = iModel->getHistoryInfo(current.row());
    IRQPreset preset;
    convertStationHistory2Preset(*currentInfo, preset);   
    int retValue = iFavorites->addPreset(preset);

    HbNotificationDialog *add2FavNote = new HbNotificationDialog();
    add2FavNote->setModal(true);
    add2FavNote->setAttribute(Qt::WA_DeleteOnClose);
        
    switch (retValue)
    {
    case EIRQErrorNone:
        add2FavNote->setTitle(hbTrId("txt_irad_info_added_to_favorites"));
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();
        break;

    case EIRQErrorOutOfMemory:
        add2FavNote->setTitle(hbTrId("txt_irad_info_can_not_add_more"));
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();        
        break;

    case EIRQErrorAlreadyExist:
        add2FavNote->setTitle(hbTrId("txt_irad_info_favorite_updated"));
        //add2FavNote->setIcon(HbIcon( QString("qtg_large_ok")));
        add2FavNote->show();           
        break;
 
    default:
        Q_ASSERT(false);         
        break;
    }    
} 

void IRHistoryView::deleteContextAction()
{
    int current = iListView->currentIndex().row();     
    bool ret = iModel->deleteOneItem(current);     
    if( !ret )
	  {
	    popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
	  }
}
void IRHistoryView::detailsContextAction()
{   
    int selectedItemIndex = iListView->currentIndex().row();
    IRQSongHistoryInfo *channelDetailInfo = iModel->getHistoryInfo(selectedItemIndex);

    IRQPreset channelPreset;
    convertStationHistory2Preset(*channelDetailInfo, channelPreset);

    IRStationDetailsView *stationDetailsView = static_cast<IRStationDetailsView*>(getViewManager()->getView(EIRView_StationDetailsView, true));    
    stationDetailsView->setDetails(&channelPreset);

    getViewManager()->activateView(EIRView_StationDetailsView);
}

void IRHistoryView::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords)
{
    Q_UNUSED(aItem);
    Q_UNUSED(aCoords);
    
    HbAction *action = NULL;
    HbMenu *contextMenu = new HbMenu;
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    connect(contextMenu, SIGNAL(triggered(HbAction*)), this, SLOT(actionClicked(HbAction*)));
    
    action = contextMenu->addAction(hbTrId("txt_irad_menu_add_to_favorite"));
    action->setObjectName(KActionAddName);
    action = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
    action->setObjectName(KActionDeleteName);
    action = contextMenu->addAction(hbTrId("txt_common_menu_details"));
    action->setObjectName(KActionDetailsName);
    
    contextMenu->open();
}

void IRHistoryView::convertStationHistory2Preset(const IRQSongHistoryInfo& aHistoryInfo, IRQPreset& aPreset)
{
    IRQChannelServerURL url;
    url.url = aHistoryInfo.getStreamUrl();
    url.bitrate = aHistoryInfo.getBitrate();
    aPreset.name = aHistoryInfo.getChannelName();
    aPreset.insertChannelServer(url);
    aPreset.type = aHistoryInfo.getChannelType();
    aPreset.presetId = aHistoryInfo.getChannelId();
    aPreset.shortDesc = aHistoryInfo.getChannelDesc();  
    aPreset.imgUrl = aHistoryInfo.getImageUrl();
    aPreset.genreName = aHistoryInfo.getGenreName();
    aPreset.countryName = aHistoryInfo.getCountryName();
    aPreset.languageName = aHistoryInfo.getLanguageName();
    aPreset.description = aHistoryInfo.getChannelDesc();
    aPreset.musicStoreStatus = aHistoryInfo.getMusicStoreStatus();
}

 
 
 

 
