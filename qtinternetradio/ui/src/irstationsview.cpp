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
#include <hblistview.h>
#include <hbaction.h>
#include <QTimer>

#include "irabstractviewmanager.h"
#include "irstationsview.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irqisdsclient.h"
#include "irqenums.h"
#include "irqutility.h"
#include "irchannelmodel.h"
#include "irqisdsdatastructure.h"
#include "irqnetworkcontroller.h"

const int KBitmapSize = 59; 
const uint KConnectTimeOut = 15000; //if the connecting take more than 15 seconds, we will show a pop up

//                                      public functions

/*
 * Description : constructor
 */
IRStationsView::IRStationsView(IRApplication* aApplication, TIRViewId aViewId) 
                               : IrAbstractListViewBase(aApplication, aViewId),
                               iLogoPreset(NULL),  iPreset(NULL), 
                               iWaitDialog(NULL),
                               iViewParameter(EIRViewPara_InvalidId),iLastSelectitem(0),
                               iLastPopularItem(0)                          
{       
    iIconIndexArray.clear();
    iChannelModel = new IrChannelModel(this);
    iListView->setModel(iChannelModel);
    
    
    iConnectTimer = new QTimer(this);
    iConnectTimer->setInterval(KConnectTimeOut);     
    
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10);    
    
    connect(iChannelModel, SIGNAL(dataAvailable()), this, SLOT(dataChanged()));    
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent)));    
    connect(iConnectTimer, SIGNAL(timeout()), this, SLOT(connectTimeOut()));
    connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
}

/*
 * Description : destructor
 */
IRStationsView::~IRStationsView()
{
    delete iPreset;
    iPreset = NULL;

    delete iLogoPreset;
    iLogoPreset = NULL;

    delete iWaitDialog;
    iWaitDialog = NULL;
    
 
 
}

void IRStationsView::setViewParameter(TIRViewParameter aParameter)
{
    iViewParameter = aParameter;
}

TIRViewParameter IRStationsView::getViewParameter() const
{
    return iViewParameter;
}

void IRStationsView::loadCategoryStations(int aIndex, const QString &aHeadingText)
{
    connectToIsdsClient();
    setHeadingText(aHeadingText);
    setViewParameter(EIRViewPara_CategoryStations);
    
    bool cache = false;
    iIsdsClient->isdsChannelRequest(aIndex, cache);
    
    if (!cache)
    {
        createWaitDialog(hbTrId("txt_common_info_loading"));
    }
}

void IRStationsView::loadPopularStations(bool aShowWaitDialog)
{
    connectToIsdsClient();
    setViewParameter(EIRViewPara_PopularStations);
    setHeadingText(tr("Popular stations"));
    bool cache = false;
    iIsdsClient->isdsCategoryRequest(IRQIsdsClient::Ehotpicks, cache);
     
    if (!cache && aShowWaitDialog)
    {
        createWaitDialog(hbTrId("txt_common_info_loading"));
    }
}

void IRStationsView::loadSearchResult(const QString &aStr)
{
    connectToIsdsClient();
    setViewParameter(EIRViewPara_SearchResults);
    setHeadingText(hbTrId("txt_irad_subtitle_search_result"));
    iIsdsClient->isdsSearchRequest(aStr);    
    if( iConnectTimer->isActive() )
    {
        iConnectTimer->stop();
    }
    
    iConnectTimer->start();
    createWaitDialog(hbTrId("txt_common_info_searching"));
}

void IRStationsView::storeCurrentItem()
{
    switch (getViewParameter())
    {
    case EIRViewPara_PopularStations:
        iLastPopularItem = iListView->currentIndex().row();
        break;
    case EIRViewPara_CategoryStations:
        iLastSelectitem = iListView->currentIndex().row();
        break;
    default:
        break;
    }
}

void IRStationsView::resetCurrentItem()
{
    iLastSelectitem = 0;
}

/*
 * Description : from base class IRBaseView.
 *               handle view commands.
 * Parameters  : aCommand : see the definition of TIRViewCommand
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRStationsView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    int leftCount = 0;
    
    switch (aCommand)
    {   
    case EIR_ViewCommand_ACTIVATED:
        connect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                this, SLOT(presetResponse(IRQPreset *)));
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(presetLogoDownload(IRQPreset* )));
        connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
                this, SLOT(presetLogoDownloadError()));
        
        ret = EIR_NoDefault;
        break;
        
    case EIR_ViewCommand_DEACTIVATE:
        if (aReason == EIR_ViewCommandReason_Back)
        {
            cleanupResource();
        }
        
        iConnectTimer->stop();
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction();         
        
        disconnect(iIsdsClient, SIGNAL(presetResponse(IRQPreset *)),
                   this, SLOT(presetResponse(IRQPreset *)));
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

void IRStationsView::launchAction()
{
    setUseNetworkReason(EIR_UseNetwork_StartingView);
    iApplication->verifyNetworkConnectivity();
}

//                                      slots functions

/*
 * Description : slot function when an item in a list is clicked.
 *               issue a listen request to isds client
 * Parameters  : aItem : pointer to the clicked item.
 */
void IRStationsView::handleItemSelected()
{
    if (iListView)
    {
        int index = iListView->currentIndex().row();
        if (index != -1)  
        {
            if (iIsdsClient)
            {
                connect(iIsdsClient, SIGNAL(operationException(IRQError)),
                        this, SLOT(operationException(IRQError)));
                
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

/*
 * Description : data is received from isds client. Load the data to the list widget and display
 *               the stations view
 */
void IRStationsView::dataChanged()
{
	
	int currentItem = 0;
    iApplication->closeConnectingDialog();
    iConnectTimer->stop();
    
    disconnectIsdsClient();
    cleanupResource();
 
    
    switch (getViewParameter())
    {
    case EIRViewPara_SearchResults:
        currentItem = 0;
        break;
    case EIRViewPara_PopularStations:
        currentItem = iLastPopularItem;
        break;
    case EIRViewPara_CategoryStations:
        currentItem = iLastSelectitem;
        break;
	  default:
        break;
    }
    iListView->reset();
    iListView->setCurrentIndex(iChannelModel->index(currentItem));
    iListView->scrollTo(iChannelModel->index(currentItem));

    //initialize the iconindices
    for (int i = 0; i < iChannelModel->rowCount(); ++i)
    {
        if (iChannelModel->imageUrl(i) != "")
        {
            iIconIndexArray.append(i);
        }
    }

    getViewManager()->activateView(this);
    if (iWaitDialog)
    {
        iWaitDialog->close();
    }
}

/*
 * Description : the preset of the selected station is received from isds client.
 *               application is responsible for deleting old preset.
 *               call player controler to play the station
 * Parameters  : aPreset : the preset of the selected station            
 */
void IRStationsView::presetResponse(IRQPreset *aPreset)
{
    disconnect(iIsdsClient, SIGNAL(operationException(IRQError)),
               this, SLOT(operationException(IRQError)));
    
    delete iPreset;
    iPreset = aPreset;
    
    if (iPreset)
    {
        iPlayController->connectToChannel(iPreset,EIRQIsds);
    }
}


/*
 * Description  : slot function for operation failure. The signal is emitted by isds client
 * Parameters   : aError : see the definition of IRQError
 */
void IRStationsView::operationException(IRQError aError)
{
    iApplication->closeConnectingDialog();
    iPlayController->closeBufferingDialog();
    
    if (iWaitDialog)
    {
        iWaitDialog->close();
    }
    
    disconnectIsdsClient();
    QString errorString = hbTrId("txt_irad_info_failed_to_connect");
    
    switch (aError)
    {   
    case EIRQErrorNotFound:
        errorString = hbTrId("txt_irad_info_no_matching_stations_found");
        if (iConnectTimer->isActive())
        {
            iConnectTimer->stop();
        }
        break;
        default:
            break;
    }
    
    popupNote(errorString, HbMessageBox::MessageTypeWarning);
}

void IRStationsView::cancelRequest()
{
    if( iWaitDialog )
    {
        iWaitDialog->close();
    }
    
    if( iConnectTimer->isActive())
    {
        iConnectTimer->stop();
    }    
    iIsdsClient->isdsCancelRequest();
    disconnectIsdsClient();
}

void IRStationsView::startConvert(int aIndex)
{
    QString url = iChannelModel->imageUrl(aIndex);
 
    IRQPreset tempPreset;
    tempPreset.imgUrl = url;
    tempPreset.type = IRQPreset::EIsds;
    
    bool cached = iIsdsClient->isdsIsLogoCached(&tempPreset, KBitmapSize, KBitmapSize);
    
    if( !cached )
    {
        bool network = iApplication->verifyNetworkConnectivity(hbTrId("txt_irad_info_downloading_logos"));
        if( !network )
        {
            //if user has clicked an item and the connection is being setup, we don't change the reason
            if (EIR_UseNetwork_NoReason == getUseNetworkReason())
            {
                setUseNetworkReason(EIR_UseNetwork_DownloadLogo);
            }
            iConvertTimer->stop();
            return;
        }        
    }
    
    iIsdsClient->isdsLogoDownSendRequest(&tempPreset, 0, KBitmapSize, KBitmapSize); 
}

 
//if the logo is downloaded ok
void IRStationsView::presetLogoDownload(IRQPreset* aPreset)
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
void IRStationsView::presetLogoDownloadError()
{
    // if the logo download fails, try to download the next
    iIconIndexArray.removeAt(0);
    int leftCount = iIconIndexArray.count();
    if( leftCount > 0 )
    {
        iConvertTimer->start();
    }    
}

void IRStationsView::cleanupResource()
{
    iIconIndexArray.clear();

    //destroy icons in time to save memory
    iChannelModel->clearAndDestroyLogos();
}

void IRStationsView::networkRequestNotified(IRQNetworkEvent aEvent)
{
    if (getViewManager()->currentView() != this)
    {
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        if (EIR_UseNetwork_StartingView == getUseNetworkReason())
        {
            //when stations view is starting view(used to show popular stations), don't show wait dialog 
            loadPopularStations(false);
        }
        else if(EIR_UseNetwork_DownloadLogo == getUseNetworkReason())
        {
            iApplication->closeConnectingDialog();
            int leftCount = iIconIndexArray.count();
            if(0 != leftCount)
            {
            	  iConvertTimer->start();
            }            
        }
        else if (EIR_UseNetwork_SelectItem == getUseNetworkReason())
        {
            iApplication->closeConnectingDialog();
            handleItemSelected();
            int leftCount = iIconIndexArray.count();
            if(0 != leftCount)
            {
                iConvertTimer->start();
            }
        }
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        
        break;
        
    case EIRQConnectingCancelled:
    case EIRQDisplayNetworkMessageNoConnectivity:
        if (iListView->model()->rowCount() == 0)
        {
            getViewManager()->activateView(EIRView_MainView);
        }
        else
        {
            setCheckedAction();
        }
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        break;
        
    default:
        setCheckedAction();
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        break;
    }
}

void IRStationsView::connectToIsdsClient()
{
    connect(iIsdsClient, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)),
            iChannelModel, SLOT(updateData(QList<IRQChannelItem *> *)));
    
    connect(iIsdsClient, SIGNAL(operationException(IRQError)),
            this, SLOT(operationException(IRQError)));
}

void IRStationsView::disconnectIsdsClient()
{
    disconnect(iIsdsClient, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)),
               iChannelModel, SLOT(updateData(QList<IRQChannelItem *> *)));
    
    disconnect(iIsdsClient, SIGNAL(operationException(IRQError)),
               this, SLOT(operationException(IRQError)));
}

void IRStationsView::createWaitDialog(QString aStr)
{
    if (!iWaitDialog)
    {
        iWaitDialog = new HbMessageBox(aStr, HbMessageBox::MessageTypeInformation);
        iWaitDialog->setTimeout(HbPopup::NoTimeout);
        iWaitDialog->setModal(true);
        iWaitDialog->setDismissPolicy(HbPopup::NoDismiss);
        QList<QAction*> actionsList = iWaitDialog->actions();
        QAction *action = actionsList.at(0);
        action->setText(hbTrId("txt_common_button_cancel"));
        connect(action, SIGNAL(triggered()), this, SLOT(cancelRequest()));
    }

    iWaitDialog->open();
}

void IRStationsView::convertAnother()
{
    iConvertTimer->stop();
    int leftCount = iIconIndexArray.count();
    
    if(0 != leftCount)
    {
        startConvert(iIconIndexArray[0]);   
    }
}

void IRStationsView::connectTimeOut()
{    
    iConnectTimer->stop();
    cancelRequest();   
    popupNote(hbTrId("txt_irad_info_connecting_timout"), HbMessageBox::MessageTypeWarning);
}
