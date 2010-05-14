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

#include <qtimer.h>
#include <hblistview.h>
#include <hbsearchpanel.h>
#include <hbaction.h>
#include <hbprogressdialog.h>

#include "irsearchchannelsview.h"
#include "iruidefines.h"
#include "irqisdsclient.h"
#include "irapplication.h"
#include "irviewmanager.h"
#include "irqnetworkcontroller.h"
#include "irchannelmodel.h"
#include "irqisdsdatastructure.h"
#include "irplaycontroller.h"
 
const uint KBitmapSize = 59; 

IRSearchChannelsView::IRSearchChannelsView(IRApplication* aApplication,
        TIRViewId aViewId): IRBaseView(aApplication, aViewId),
        iListView(NULL),iSearchPanelWidget(NULL),
        iSearchState(ESearch_init),iChannelModel(NULL),iSearchingDialog(NULL),
        iPreset(NULL),iLogoPreset(NULL),iConvertTimer(NULL)
{
    //if this view is not starting view, finish all initialization in constructor
    if (getViewManager()->views().count() > 0)
    {
        if (!initCompleted())
        {            
            normalInit();             
        }
    }
}

void IRSearchChannelsView::lazyInit()
{
    if (!initCompleted())
    {
        normalInit();                     
    }        
}

void IRSearchChannelsView::normalInit()
{
    if (!initCompleted())
    { 
        IRBaseView::lazyInit();
        loadLayout();
        connectWidget();  
        switch2InitState();       
        initMenu();        
        initTimer();
        setInitCompleted(true);
    }
}

void IRSearchChannelsView::initMenu()
{

}

void IRSearchChannelsView::initTimer()
{     
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10); 
    
    connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
}

IRSearchChannelsView::~IRSearchChannelsView()
{
    if( iSearchingDialog != NULL )
    {
        iSearchingDialog->close();
        delete iSearchingDialog;
        iSearchingDialog = NULL;
    }
    
    if( iPreset != NULL )
    {
        delete iPreset;
        iPreset = NULL;
    }
    
    if( iLogoPreset != NULL )
    {
        delete iLogoPreset;
        iLogoPreset = NULL;
    }
}

void IRSearchChannelsView::loadLayout()
{
    // Create widget hierarchy
    setObjectName( SEARCH_CHANNELS_VIEW_OBJECT_VIEW );
    iChannelModel = new IrChannelModel(this);
    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this );
    iLoader.setObjectTree( roots );
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME);
    iSearchPanelWidget = qobject_cast<HbSearchPanel *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_SEARCHPANEL_WIDGET));    
    iListView = qobject_cast<HbListView *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_SEARCHLISTVIEW_WIDGET));     
    iListView->setModel(iChannelModel);
}

void IRSearchChannelsView::connectWidget()
{     
    connect(iListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(clickItem(const QModelIndex&)));
    connect(iChannelModel, SIGNAL(dataAvailable()), this, SLOT(dataChanged()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)), this, SLOT(networkRequestNotified(IRQNetworkEvent)));     
    connect(iSearchPanelWidget,SIGNAL(criteriaChanged(const QString&)),this,SLOT(searchTextAlready(const QString&)));
}

void IRSearchChannelsView::connectIsdsClient()
{
    connect(iIsdsClient, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)),
            iChannelModel, SLOT(updateData(QList<IRQChannelItem *> *)));    
    connect(iIsdsClient, SIGNAL(operationException(IRQError)),
            this, SLOT(operationException(IRQError)));   
    connect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
            this, SLOT(presetResponse(IRQPreset *)));    
    connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
            this, SLOT(presetLogoDownload(IRQPreset* )));     
    connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
            this, SLOT(presetLogoDownloadError()));
    
   
}

void IRSearchChannelsView::disconnectIsdsClient()
{ 
    disconnect(iIsdsClient, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)),
            iChannelModel, SLOT(updateData(QList<IRQChannelItem *> *)));
    disconnect(iIsdsClient, SIGNAL(operationException(IRQError)), this,
            SLOT(operationException(IRQError))); 
    disconnect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                       this, SLOT(presetResponse(IRQPreset *)));
    disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
                       this, SLOT(presetLogoDownload(IRQPreset* )));
    disconnect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
                       this, SLOT(presetLogoDownloadError()));  
}


void IRSearchChannelsView::switch2InitState()
{
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME,SEARCH_CHANNELS_VIEW_NO_LISTVIEW_SECTION);    
    iSearchState = ESearch_init;     
}

void IRSearchChannelsView::switch2SearchedState()
{
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME,SEARCH_CHANNELS_VIEW_LISTVIEW_SECTION);    
    iSearchState = ESearch_Searched;     
}
void IRSearchChannelsView::switch2SearchingState()
{    
    iSearchState = ESearch_Searching;     
    startSearchingAnimation();
}
 
void IRSearchChannelsView::createSearchingDialog()
{
    if ( NULL == iSearchingDialog)
    {
        //needs to add this , otherwise memory leak
        iSearchingDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
        iSearchingDialog->setModal(true);
        iSearchingDialog->setTimeout(HbPopup::NoTimeout);
        QAction *cancelAction = iSearchingDialog->actions().at(0);
        cancelAction->setText(hbTrId("txt_common_button_cancel"));
        connect(cancelAction, SIGNAL(triggered()), this, SLOT(cancelRequest()));
    }
    
    iSearchingDialog->setText(hbTrId("txt_common_info_searching"));
    iSearchingDialog->show();
}

void IRSearchChannelsView::closeSearchingDialog()
{
    if( iSearchingDialog )
    {
        iSearchingDialog->close();
        iSearchingDialog->deleteLater();
        iSearchingDialog = NULL;
    }
}

void IRSearchChannelsView::startSearchingAnimation()
{
    
}

void IRSearchChannelsView::stopSearchingAnimation()
{
    
}


void IRSearchChannelsView::handleItemSelected()
{
    if (iListView)
    {
        int index = iListView->currentIndex().row();
        if (index != -1)  
        {
            if (iIsdsClient)
            {              
                //once an item is selected, we show a dialog to prevent user from clicking the
                //item again
                iPlayController->createBufferingDialog(this, SLOT(cancelRequest()));
                
                if (iIsdsClient->isdsIsChannelBanner())
                {
                    iIsdsClient->isdsListenRequest(index + 1);
                }
                else
                {
                    iIsdsClient->isdsListenRequest(index);
                }
            }
        }
    }
}

void IRSearchChannelsView::cancelRequest()
{
    iSearchState = ESearch_init;
    iIsdsClient->isdsCancelRequest();    
    stopSearchingAnimation();    
    //the following will remove into the stopSearchingAnimation() function
    closeSearchingDialog();
} 

void IRSearchChannelsView::networkRequestNotified(IRQNetworkEvent aEvent)
{        
    if (getViewManager()->currentView() != this)
    {       
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
         
        if(EIR_UseNetwork_StartSearch == getUseNetworkReason())
        {
            Q_ASSERT( !iKeyText.isEmpty() );
            iIsdsClient->isdsSearchRequest(iKeyText);
        }
        else if( EIR_UseNetwork_SelectItem == getUseNetworkReason() )
        {            
            handleItemSelected();
        }
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        break;        
     
    default:             
        stopSearchingAnimation();
        //the following will remove into the stopSearchingAnimation() function
        closeSearchingDialog();
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        iSearchState = ESearch_init;
        break;
    }   
} 

void IRSearchChannelsView::searchTextAlready(const QString& aSearchCriteria)
{
    iKeyText = aSearchCriteria.trimmed();
    
    if( iKeyText.isEmpty() )
    {
        //show some error info here to let user input some criteria
        return;
    }  
    
    setUseNetworkReason(EIR_UseNetwork_StartSearch);
    if (false == iApplication->verifyNetworkConnectivity(hbTrId("txt_common_info_searching")))
    {        
        switch2SearchingState();
        return;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);    
    switch2SearchingState();
    iIsdsClient->isdsSearchRequest(iKeyText);     
    createSearchingDialog();
}



void IRSearchChannelsView::operationException(IRQError aError)
{    
    iApplication->closeConnectingDialog();
    iPlayController->closeBufferingDialog();    
    closeSearchingDialog();
    stopSearchingAnimation();     
    iSearchState = ESearch_init;
    
    QString errStr;
    
    switch(aError)
    {
    case EIRQErrorNotFound:
        errStr = hbTrId("txt_irad_info_no_matching_stations_found");
        break;
    default:
        errStr = hbTrId("txt_irad_info_failed_to_connect");
        break;   
    }
    popupNote(errStr, HbMessageBox::MessageTypeWarning);     
}

void IRSearchChannelsView::clickItem(const QModelIndex&)
{
    setUseNetworkReason(EIR_UseNetwork_SelectItem);
    if (false == iApplication->verifyNetworkConnectivity())
    {
        return;
    }
    setUseNetworkReason(EIR_UseNetwork_NoReason);
    handleItemSelected();
}


void IRSearchChannelsView::presetResponse(IRQPreset *aPreset)
{    
    delete iPreset;
    iPreset = aPreset;
    
    if (iPreset)
    {
        iPlayController->connectToChannel(iPreset,EIRQIsds);
    }
}

void IRSearchChannelsView::convertAnother()
{
    iConvertTimer->stop();
    int leftCount = iIconIndexArray.count();
    
    if( leftCount > 0 )
    { 
        startConvert(iIconIndexArray[0]);   
    }
}

void IRSearchChannelsView::dataChanged()
{
    iApplication->closeConnectingDialog();     
    stopSearchingAnimation();
    
    //the following will remove into the stopSearchingAnimation() function
    closeSearchingDialog();    
    switch2SearchedState(); 
    iListView->reset();
    iListView->setCurrentIndex(iChannelModel->index(0));
    iListView->scrollTo(iChannelModel->index(0)); 
    GraphicsItemFlags  flags = iListView->flags();
    bool focusEnable = flags & QGraphicsItem::ItemIsFocusable;
    if( focusEnable )
    {
        //we move the focus to the listview and the search panel will
        //hide the virtual keyboard at the same time
        iListView->setFocus(Qt::OtherFocusReason);            
    }
    
    iIconIndexArray.clear();
    //initialize the iconindices
    for (int i = 0; i < iChannelModel->rowCount(); ++i)
    {
        if (iChannelModel->imageUrl(i) != "")
        {            
            iIconIndexArray.append(i);
        }
    }
    
    if( iIconIndexArray.count() > 0 )
    {
        iConvertTimer->start();        
    } 
}

void IRSearchChannelsView::startConvert(int aIndex)
{
    QString url = iChannelModel->imageUrl(aIndex);
 
    IRQPreset tempPreset;
    tempPreset.imgUrl = url;
    tempPreset.type = IRQPreset::EIsds;
    
    bool cached = iIsdsClient->isdsIsLogoCached(&tempPreset, KBitmapSize, KBitmapSize);
    
    if( !cached )
    {
        bool network = iNetworkController->getNetworkStatus();
        //if the network is not ready, some exception happens and we don't get the logos
        //for they are not so important.
        if( !network )
        {             
            return;
        }       
    }
    
    iIsdsClient->isdsLogoDownSendRequest(&tempPreset, 0, KBitmapSize, KBitmapSize); 
}

//if the logo is downloaded ok
void IRSearchChannelsView::presetLogoDownload(IRQPreset* aPreset)
{
    if( NULL == aPreset )
    {
        presetLogoDownloadError();
        return;
    } 
    
 
    delete iLogoPreset;            
    iLogoPreset = aPreset;    

    if (iLogoPreset->logoData != KNullDesC8)
    {         
        QPixmap tempMap;  
        const unsigned char * logoData = iLogoPreset->logoData.Ptr();     
        bool ret = tempMap.loadFromData(logoData, iLogoPreset->logoData.Length());
        QIcon convertIcon(tempMap);
       
        if( ret )
        {            
            HbIcon *hbIcon = new HbIcon(convertIcon);
            int index = iIconIndexArray[0];
            iChannelModel->setLogo(hbIcon, index);
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
void IRSearchChannelsView::presetLogoDownloadError()
{
    // if the logo download fails, try to download the next
    iIconIndexArray.removeAt(0);
    int leftCount = iIconIndexArray.count();
    if( leftCount > 0 )
    {
        iConvertTimer->start();
    }     
}

TIRHandleResult IRSearchChannelsView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    TIRHandleResult ret = IRBaseView::handleCommand(aCommand, aReason);
    
    switch (aCommand)
    {   
    case EIR_ViewCommand_ACTIVATED:
        //wether to start logo downloading when activated needs more discussing and testing, future.
        iIconIndexArray.clear();
        connectIsdsClient(); 
        break;
        
    case EIR_ViewCommand_DEACTIVATE:     
        disconnectIsdsClient();
        iIsdsClient->isdsCancelRequest();            
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction(); 
        break;

    default:
        break;
    }
    
    return ret;
}


 


