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
#include <qinputcontext.h>
#include <qapplication.h>

#include "irsearchchannelsview.h"
#include "iruidefines.h"
#include "irqisdsclient.h"
#include "irapplication.h"
#include "irviewmanager.h"
#include "irqnetworkcontroller.h"
#include "irchannelmodel.h"
#include "irqisdsdatastructure.h"
#include "irplaycontroller.h"
#include "irqlogger.h"
#include "irqsettings.h"
 
const uint KBitmapSize = 59;

IRSearchChannelsView::IRSearchChannelsView(IRApplication* aApplication,
        TIRViewId aViewId): IRBaseView(aApplication, aViewId),
        iListView(NULL),iSearchPanelWidget(NULL),iSearchState(ESearch_init),
        iChannelModel(NULL),iPreset(NULL),iLogoPreset(NULL),
        iConvertTimer(NULL) 
{
    LOG_METHOD;
    iSettings = IRQSettings::openInstance();    
    loadLayout();
    connectWidget();  
    switch2InitState();       
    initMenu();        
    initTimer();
}

void IRSearchChannelsView::initMenu()
{
    LOG_METHOD;
    HbMenu *viewMenu = menu();
    QObject *exitAction = iLoader.findObject(EXIT_ACTION);
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));
}

void IRSearchChannelsView::initTimer()
{     
    LOG_METHOD;
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10); 
    
    connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
}

 

IRSearchChannelsView::~IRSearchChannelsView()
{   
    LOG_METHOD;
    
    if (iSettings)
    {
        iSettings->setSearchText(iKeyText);
        iSettings->closeInstance();
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
    
    if( NULL != iChannelModel )
    {
        iChannelModel->save2Cache();
    }
 
}

void IRSearchChannelsView::loadLayout()
{
    LOG_METHOD;
    // Create widget hierarchy
    setObjectName( SEARCH_CHANNELS_VIEW_OBJECT_VIEW );
    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this );
    iLoader.setObjectTree( roots );
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME);   
    
    iSearchPanelWidget = qobject_cast<HbSearchPanel *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_SEARCHPANEL_WIDGET));    
    QString searchText;
    iSettings->getSearchText(searchText);
    iKeyText = searchText;
    iSearchPanelWidget->setCriteria(searchText);
   
    iListView = qobject_cast<HbListView *>(iLoader.findWidget(SEARCH_CHANNELS_VIEW_SEARCHLISTVIEW_WIDGET));   
    iListView->setFlag(ItemIsFocusable);
    HbScrollBar *scrollbar = iListView->verticalScrollBar();
    scrollbar->setVisible(true);
    scrollbar->setInteractive(true);
    iListView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    
    iChannelModel = new IrChannelModel(this);
    iChannelModel->initWithCache();
    iListView->setModel(iChannelModel);
    scrollbar->setValue(0.0);    
    
    iListView->installEventFilter(this);
          
#ifdef SUBTITLE_STR_BY_LOCID
    setTitle(hbTrId("txt_irad_title_internet_radio"));
         
#else
    setTitle("Internet radio");      
#endif

}

void IRSearchChannelsView::connectWidget()
{     
    LOG_METHOD;
    connect(iListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(clickItem(const QModelIndex&)));
    connect(iChannelModel, SIGNAL(dataAvailable()), this, SLOT(dataChanged()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)), this, SLOT(networkRequestNotified(IRQNetworkEvent)));     
    connect(iSearchPanelWidget,SIGNAL(criteriaChanged(const QString&)),this,SLOT(searchTextAlready(const QString&)));
    connect(iSearchPanelWidget, SIGNAL(exitClicked()), this, SLOT(minimizeSearchPanel()));
}

void IRSearchChannelsView::connectIsdsClient()
{
    LOG_METHOD;
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
    LOG_METHOD;
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
    LOG_METHOD;
    iSearchState = ESearch_init;     
    iApplication->stopLoadingAnimation();
} 

void IRSearchChannelsView::switch2LoadingState()
{   
    LOG_METHOD;
    iSearchPanelWidget->clearFocus();
    iListView->setFocus();
    iSearchState = ESearch_Loading; 
    iApplication->startLoadingAnimation(this, SLOT(minimizeSearchPanel()));
}

void IRSearchChannelsView::handleItemSelected()
{
    LOG_METHOD;
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
    LOG_METHOD;  
    iIsdsClient->isdsCancelRequest();    
    switch2InitState(); 
} 

void IRSearchChannelsView::networkRequestNotified(IRQNetworkEvent aEvent)
{       
    LOG_METHOD;
    LOG_FORMAT("aEvent = %d", (int)aEvent ); 
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
    LOG_METHOD;
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
    LOG_METHOD;
    switch2InitState();
    
    QString errStr;    
    switch(aError)
    {
    case EIRQErrorNotFound:
#ifdef SUBTITLE_STR_BY_LOCID 
        errStr = hbTrId("txt_irad_info_no_matching_station_found");
#else
        errStr = hbTrId("No matching station found");        
#endif
        break;
    default:
#ifdef SUBTITLE_STR_BY_LOCID
        errStr = hbTrId("txt_irad_info_failed_to_connect");
#else
        errStr = hbTrId("Connecting failed");        
#endif
        break;   
    }
    
    popupNote(errStr, HbMessageBox::MessageTypeWarning);     
    iChannelModel->cleanupDatabase();
}

void IRSearchChannelsView::clickItem(const QModelIndex&)
{
    LOG_METHOD;
    setUseNetworkReason(EIR_UseNetwork_SelectItem);
#ifdef HS_WIDGET_ENABLED	
    int index = iListView->currentIndex().row();
    IRQChannelItem* currentItem = iChannelModel->getChannelItemByIndex(index);    
    iPlayController->setConnectingStationName(currentItem->channelName,true);
#endif	
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
    LOG_METHOD;
    delete iPreset;
    iPreset = aPreset;
    
    if (iPreset)
    {
        iPlayController->connectToChannel(iPreset,EIRQIsds);
    }
}

void IRSearchChannelsView::convertAnother()
{
    LOG_METHOD;
    iConvertTimer->stop();
    int leftCount = iIconIndexArray.count();
    
    if( leftCount > 0 )
    { 
        startConvert(iIconIndexArray[0]);   
    }
}
 
void IRSearchChannelsView::dataChanged()
{
    LOG_METHOD;
    switch2InitState();      
    iListView->reset();
    if( iChannelModel->rowCount() )
    {
        iListView->setCurrentIndex(iChannelModel->index(0));
        iListView->verticalScrollBar()->setValue(0.0);
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
    LOG_METHOD;
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
    LOG_METHOD;
    if( NULL == aPreset )
    {
        presetLogoDownloadError();
        return;
    } 
    
 
    delete iLogoPreset;            
    iLogoPreset = aPreset;    

    if (iLogoPreset->logoData.size() > 0)
    {         
        QPixmap tempMap;      
        bool ret = tempMap.loadFromData((const unsigned char*)iLogoPreset->logoData.constData(), iLogoPreset->logoData.size());
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
    LOG_METHOD;
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
    LOG_METHOD;
    if( ESearch_Loading == iSearchState )
    {
        iPlayController->cancelBuffering(); 
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

    hideVkb();
}
 

TIRHandleResult IRSearchChannelsView::handleCommand(TIRViewCommand aCommand,
        TIRViewCommandReason aReason)
{
    LOG_METHOD;
    LOG_FORMAT("aCommand = %d", (int)aCommand);
    TIRHandleResult ret = IRBaseView::handleCommand(aCommand, aReason);
    
    switch (aCommand)
    {   
    case EIR_ViewCommand_ACTIVATED:         
        iIconIndexArray.clear();        
        iListView->clearFocus();
        iSearchPanelWidget->setFocusDelegation(FocusDelegationFirstChild);  
        iSearchPanelWidget->setFocus();        
        if( (0 == iChannelModel->rowCount()) 
            && iKeyText.isEmpty() )
        {
            showVkb();
        }
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

bool IRSearchChannelsView::eventFilter(QObject *object, QEvent *event)
{
    if( object == iListView
        && event->type() == QEvent::FocusOut )
    {
        hideVkb();
    }
  
    return false;
}

void IRSearchChannelsView::showVkb()
{
    QInputContext *ic = qApp->inputContext(); 
    if (ic)
    {
        QEvent *event = new QEvent(QEvent::RequestSoftwareInputPanel);
        ic->filterEvent(event);
        delete event;
    }
}

void IRSearchChannelsView::hideVkb()
{
    QInputContext *ic = qApp->inputContext(); 
    if (ic)
    {
        QEvent *event = new QEvent(QEvent::CloseSoftwareInputPanel);
        ic->filterEvent(event);
        delete event;
    }
}


