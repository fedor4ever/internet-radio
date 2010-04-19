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
#include <QPixmap>
#include <QTimer>

#include "irabstractviewmanager.h"
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

const int KBitmapSize = 59;

//                                         public functions

/*
 * Description : constructor
 */
IRHistoryView::IRHistoryView(IRApplication *aApplication, TIRViewId aViewId) :
    IrAbstractListViewBase(aApplication, aViewId), iClearHistoryAction(NULL),
    iLogoPreset(NULL)
{
    
    iModel = new IRHistoryModel(this);
    iListView->setModel(iModel);
    iListView->setCurrentIndex(iModel->index(0));
    
    iClearHistoryAction = new HbAction(hbTrId("txt_irad_opt_clear_station_history"), menu());
    iSongHistoryAction = new HbAction(hbTrId("txt_irad_opt_recently_played_songs"), menu());	 
    menu()->insertAction(iOpenWebAddressAction, iSongHistoryAction);
    
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10);
    
    connect(iClearHistoryAction, SIGNAL(triggered()), this, SLOT(clearAllList()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
    this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));
    connect(iSongHistoryAction, SIGNAL(triggered()), this, SLOT(gotoSongHistory()));
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
    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                this, SLOT(presetResponse(IRQPreset *)));
        connect(iIsdsClient, SIGNAL(operationException(IRQError)),
                this, SLOT(operationException(IRQError)));
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(presetLogoDownload(IRQPreset* )));
        connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
                this, SLOT(presetLogoDownloadError()));
        
        showHistory();
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
    
    case EIR_ViewCommand_EffectFinished:
        /* when the effect is finished, we start showing the logos  */
        leftCount = iIconIndexArray.count();
        if( leftCount > 0 )
        {
            iConvertTimer->start();
        }
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
        iPlayController->createBufferingDialog(this, SLOT(cancelRequest()));
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
    iPlayController->closeBufferingDialog();

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
        iApplication->closeConnectingDialog();

        if (EIR_UseNetwork_SelectItem == getUseNetworkReason())
        {
            handleItemSelected();
        }
        
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        break;
        
    default:
        setCheckedAction();
        break;
    }
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
    updateView();
}

void IRHistoryView::prepareMenu()
{
    HbMenu *viewMenu = menu();
    
    viewMenu->removeAction(iClearHistoryAction);
    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(iOpenWebAddressAction, iClearHistoryAction);
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
 
 
void IRHistoryView::addContextAction()
{        
    QModelIndex current = iListView->currentIndex();     
    IRQSongHistoryInfo * currentInfo = iModel->getHistoryInfo(current.row());
    IRQPreset preset;
    convertStationHistory2Preset(*currentInfo, preset);   
    int retValue = iFavorites->addPreset(preset);

    switch (retValue)
    {
    case EIRQErrorNone:
	    popupNote(hbTrId("txt_irad_menu_add_to_favorite"), HbMessageBox::MessageTypeInformation);
        
        break;

    case EIRQErrorOutOfMemory:
	    popupNote(hbTrId("txt_irad_info_can_not_add_more"), HbMessageBox::MessageTypeInformation);
		break;

    case EIRQErrorAlreadyExist:
	    popupNote(hbTrId("txt_irad_info_favorite_updated"), HbMessageBox::MessageTypeInformation);
		break;
 
    default:         
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
 
 
 
void IRHistoryView::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords)
{
    Q_UNUSED(aItem);             
    HbMenu *contextMenu = 0;
    HbAction *action = 0;
    
    contextMenu = new HbMenu();
    action = contextMenu->addAction(QString(hbTrId("txt_irad_menu_add_to_favorite")));
    action->setObjectName("add");
    action = contextMenu->addAction(QString(hbTrId("txt_common_menu_delete")));
    action->setObjectName("delete");
    action = contextMenu->exec(aCoords);
    
    if( action )
    {
        QString objectName = action->objectName();
        if ( objectName == "add" )
        {
            addContextAction();                          	        	            
        }
        else if( objectName == "delete")
        {
            deleteContextAction();                            
        }
    }          
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
    aPreset.description = aHistoryInfo.getChannelDesc();
    aPreset.musicStoreStatus = aHistoryInfo.getMusicStoreStatus();
}

 
 
 

 
