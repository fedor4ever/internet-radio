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
#include <hbscrollbar.h> 
#include <hbiconitem.h>
#include <hbiconanimator.h>
#include <hblabel.h> 
#include <hbiconanimationmanager.h>
#include <HbGroupBox>

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
        iHeadingLabel(NULL),iListView(NULL),iSearchPanelWidget(NULL),iSearchState(ESearch_init),
        iChannelModel(NULL),iPreset(NULL),iLogoPreset(NULL),
        iConvertTimer(NULL) 
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
    HbMenu *viewMenu = menu();
    QObject *exitAction = iLoader.findObject(EXIT_ACTION);
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));
}

void IRSearchChannelsView::initTimer()
{     
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10); 
    
    connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
}

 

IRSearchChannelsView::~IRSearchChannelsView()
{   
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
    
    if( NULL != iChannelModel )
    {
        iChannelModel->save2Cache();
    }
 
}

void IRSearchChannelsView::loadLayout()
{
    // Create widget hierarchy
    setObjectName( SEARCH_CHANNELS_VIEW_OBJECT_VIEW );
    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this );
    iLoader.setObjectTree( roots );
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME);
    
    iHeadingLabel = qobject_cast<HbGroupBox *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_HEADINGTEXT_WIDGET));
    
    
    iSearchPanelWidget = qobject_cast<HbSearchPanel *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_SEARCHPANEL_WIDGET));    
    
    iListView = qobject_cast<HbListView *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_SEARCHLISTVIEW_WIDGET));   
    iListView->setFlag(ItemIsFocusable);
    HbScrollBar *scrollbar = iListView->verticalScrollBar();
    scrollbar->setVisible(true);
    scrollbar->setInteractive(true);
    iListView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    
    iChannelModel = new IrChannelModel(this);
    iChannelModel->initWithCache();
    iListView->setModel(iChannelModel);    
    //anywhere, before show the count, updated it ahead.
#ifdef SUBTITLE_STR_BY_LOCID
    QString headingStr = hbTrId("txt_irad_subtitle_search_results") + " (" + QString::number(iChannelModel->rowCount()) + ")"; 
#else
    QString headingStr = hbTrId("Search results") + " (" + QString::number(iChannelModel->rowCount()) + ")";
#endif

    setHeadingText(headingStr);  
}

void IRSearchChannelsView::connectWidget()
{     
    connect(iListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(clickItem(const QModelIndex&)));
    connect(iChannelModel, SIGNAL(dataAvailable()), this, SLOT(dataChanged()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)), this, SLOT(networkRequestNotified(IRQNetworkEvent)));     
    connect(iSearchPanelWidget,SIGNAL(criteriaChanged(const QString&)),this,SLOT(searchTextAlready(const QString&)));
    connect(iSearchPanelWidget, SIGNAL(exitClicked()), this, SLOT(minimizeSearchPanel()));
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
    iSearchState = ESearch_init;     
    iApplication->stopLoadingAnimation();
} 

void IRSearchChannelsView::switch2LoadingState()
{   
    iSearchPanelWidget->clearFocus();
    iListView->setFocus();
    iSearchState = ESearch_Loading;
    
    QSizeF searchPanelSize = iSearchPanelWidget->size();
    QSizeF windowSize = getViewManager()->size();
    QPointF pos(windowSize.width()/2,(windowSize.height() - searchPanelSize.height())/2);
    QPointF iconPos(LOADING_ANIMATION_ICON_SIZE/2, LOADING_ANIMATION_ICON_SIZE/2);
    pos -= iconPos;
    iApplication->startLoadingAnimation(pos);
}

void IRSearchChannelsView::handleItemSelected()
{
    if (iListView)
    {
        int index = iListView->currentIndex().row();
        IRQChannelItem* aItem = iChannelModel->getChannelItemByIndex(index);
        if( NULL != aItem )
        {
            iIsdsClient->isdsListenRequest(aItem->channelID,true);
        }        
    }
 
}

void IRSearchChannelsView::cancelRequest()
{     
    iIsdsClient->isdsCancelRequest();    
    switch2InitState(); 
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
        switch2InitState();         
        setUseNetworkReason(EIR_UseNetwork_NoReason);        
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
    if (false == iApplication->verifyNetworkConnectivity())
    {        
        switch2LoadingState();
        return;
    }
    
    //if previouse request is in progress, cancel it
    if( ESearch_Loading == iSearchState )
    {
        iIsdsClient->isdsCancelRequest();            
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction();
    }
 
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);    
    switch2LoadingState();
    iIsdsClient->isdsSearchRequest(iKeyText);    
}

void IRSearchChannelsView::operationException(IRQError aError)
{        
    switch2InitState();
    
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
    iChannelModel->cleanupDatabase();
}

void IRSearchChannelsView::clickItem(const QModelIndex&)
{
    setUseNetworkReason(EIR_UseNetwork_SelectItem);
    if (false == iApplication->verifyNetworkConnectivity())
    {
        switch2LoadingState();
        return;
    }
    setUseNetworkReason(EIR_UseNetwork_NoReason);
    
    switch2LoadingState();
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

//set the subtitle and counter;
void IRSearchChannelsView::setHeadingText(const QString &aText)
{
    if (iHeadingLabel)
    {
        iHeadingLabel->setHeading(aText);
    }    
}
void IRSearchChannelsView::dataChanged()
{
    switch2InitState(); 
    //here update count in subtitle
#ifdef SUBTITLE_STR_BY_LOCID
    QString headingStr = hbTrId("txt_irad_subtitle_search_results") + " (" + QString::number(iChannelModel->rowCount()) + ")"; 
#else
    QString headingStr = hbTrId("Search results") + " (" + QString::number(iChannelModel->rowCount()) + ")";
#endif
    setHeadingText(headingStr);   
    iListView->reset();
    if( iChannelModel->rowCount() )
    {
        iListView->setCurrentIndex(iChannelModel->index(0));
        iListView->scrollTo(iChannelModel->index(0));         
    }
    
    //we move the focus to the listview and the search panel will
    //hide the virtual keyboard at the same time
    iListView->setFocus();            
    
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

void IRSearchChannelsView::minimizeSearchPanel()
{
    if( ESearch_Loading == iSearchState )
    {
        if( ! ( iPlayController->isStopped() || iPlayController->isIdle() ) )
        {
            //cancel buffering
            iPlayController->cancelBuffering();
        }
         
        disconnectIsdsClient();
        iIsdsClient->isdsCancelRequest();
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction();
        switch2InitState();      
    }
    
    int count = iChannelModel->rowCount();
    if (0 == count)
    {         
        iSearchPanelWidget->clearFocus();
        iListView->setFocus();        
        getViewManager()->backToPreviousView();
    }
    else
    {
        iSearchPanelWidget->clearFocus();
        iListView->setFocus();        
    }
}
 

TIRHandleResult IRSearchChannelsView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{
    TIRHandleResult ret = IRBaseView::handleCommand(aCommand, aReason);
    
    switch (aCommand)
    {   
    case EIR_ViewCommand_ACTIVATED:         
        iIconIndexArray.clear();        
        iListView->clearFocus();
        iSearchPanelWidget->setFocus();        
        connectIsdsClient();
        break;
        
    case EIR_ViewCommand_DEACTIVATE:     
        disconnectIsdsClient();
        iIsdsClient->isdsCancelRequest();            
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction();
        switch2InitState();
        break;

    default:
        break;
    }
    
    return ret;
}


 


