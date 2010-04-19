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
#include <hbaction.h>
#include <hbmenu.h>
#include <QPixmap>
#include <QTimer>

#include "irabstractviewmanager.h"
#include "irfavoritesview.h"
#include "irapplication.h"
#include "irqfavoritesdb.h"
#include "irqisdsclient.h"
#include "irqnetworkcontroller.h"
#include "irplaycontroller.h"
#include "irfavoritesmodel.h"
#include "irqenums.h"
#include "irqisdsdatastructure.h"
#include "irqutility.h"

const int KBitmapSize = 59;
//                                        public functions

/*
 * Description : constructor
 */
IRFavoritesView::IRFavoritesView(IRApplication *aApplication, TIRViewId aViewId) 
                                 : IrAbstractListViewBase(aApplication, aViewId),
                                   iClearFavoriteAction(NULL),iLogoPreset(NULL)
								  
{    
    iClearFavoriteAction = new HbAction(hbTrId("txt_irad_opt_clear_favorites"), menu());
    connect(iClearFavoriteAction, SIGNAL(triggered()), this, SLOT(clearAllFavorites()));    
  
    iModel = new IRFavoritesModel(iFavorites, this);
    iListView->setModel(iModel);
    iListView->setCurrentIndex(iModel->index(0));
    
    iConvertTimer = new QTimer(this);
    iConvertTimer->setInterval(10);
    
    connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent))); 
    connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
}

/*
 * Description : destructor
 */
IRFavoritesView::~IRFavoritesView()
{
    delete iLogoPreset;
    iLogoPreset = NULL;
}

TIRHandleResult IRFavoritesView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    int leftCount = 0;
    
    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:        
        connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
                this, SLOT(presetLogoDownload(IRQPreset* )));
        connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
                this, SLOT(presetLogoDownloadError()));
        
        
        iModel->checkFavoritesUpdate();          
        ret = EIR_NoDefault;
        break;

    case EIR_ViewCommand_DEACTIVATE:        		 
        
        iModel->clearAndDestroyLogos();
        iConvertTimer->stop();
        iIsdsClient->isdsLogoDownCancelTransaction();
        //iIconIndexArray must be cleared, because timer call back convertAnother() might be
        //called after view is deactivated. In that case, iModel->getImgURL(aIndex); will crash
        iIconIndexArray.clear();
        
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

void IRFavoritesView::handleItemSelected()
{
    int currentIndex = iListView->currentIndex().row();
    IRQPreset * currentPreset = iModel->getPreset(currentIndex);

    if (currentPreset)
    {
        if(IRQPreset::EIsds == currentPreset->type)
        {
            iPlayController->connectToChannel(currentPreset,EIRQPresetIsds);
        }
        else
        {
            iPlayController->connectToChannel(currentPreset,EIRQPresetAdhoc);
        }
    }        
}

// ---------------------------------------------------------------------------
// IRFavoritesView::clearAllList()
//---------------------------------------------------------------------------
void IRFavoritesView::clearAllFavorites()
{
    iIconIndexArray.clear();
    iModel->clearFavoriteDB();   
    iIsdsClient->isdsLogoDownCancelTransaction();
    iConvertTimer->stop();
    updateView();
}

void IRFavoritesView::networkRequestNotified(IRQNetworkEvent aEvent)
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

void IRFavoritesView::prepareMenu()
{
    HbMenu *viewMenu = menu();
    
    viewMenu->removeAction(iClearFavoriteAction);
    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(iOpenWebAddressAction, iClearFavoriteAction);
    }
}

void IRFavoritesView::startConvert(int aIndex)
{     
    QString url = iModel->getImgUrl(aIndex);
    
    IRQPreset tempPreset;
    tempPreset.imgUrl = url;
    tempPreset.type = IRQPreset::EIsds;

    iIsdsClient->isdsLogoDownSendRequest(&tempPreset, 0, KBitmapSize, KBitmapSize); 
}

//if the logo is downloaded ok
void IRFavoritesView::presetLogoDownload(IRQPreset* aPreset)
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
void IRFavoritesView::presetLogoDownloadError()
{
    iIconIndexArray.removeAt(0);
    int leftCount = 0;
    leftCount = iIconIndexArray.count();
    if( leftCount > 0 )
    {
        iConvertTimer->start();
    }    
}

void IRFavoritesView::convertAnother()
{
    iConvertTimer->stop();
    int leftCount = iIconIndexArray.count();

    if (0 != leftCount)
    {
        startConvert(iIconIndexArray[0]);
    }
}

void IRFavoritesView::modelChanged()
{
    QString headingStr = tr("Favorites") + " (" + QString::number(iModel->rowCount()) + ")";
    setHeadingText(headingStr);
    iIconIndexArray.clear();
    
    for (int i = 0; i < iModel->rowCount(); ++i)
    {
        if (iModel->getImgUrl(i) != "")
        {
            iIconIndexArray.append(i);
        }
    }
    
    iListView->reset();
    iListView->setCurrentIndex(iModel->index(0));
}

/*
 * Description : set the checked action in toolbar. for favoritesview, iFavoritesAction should
 *               be checked
 */
void IRFavoritesView::setCheckedAction()
{
    iFavoritesAction->setChecked(true);
}

void IRFavoritesView::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords)
{
    Q_UNUSED(aItem);             
    HbMenu *contextMenu = 0;
    HbAction *action = 0;
    
    contextMenu = new HbMenu(); 
    action = contextMenu->addAction(QString(hbTrId("txt_common_menu_delete")));
    action->setObjectName("delete");
    action = contextMenu->exec(aCoords);
    
    if( action )
    {
        QString objectName = action->objectName();      
        if( objectName == "delete")
        {
            deleteContextAction();                            
        }
    }          
} 

void IRFavoritesView::deleteContextAction()
{
    int current = iListView->currentIndex().row();
    bool ret = iModel->deleteOneFavorite(current);   
	  if( !ret )
	  {
	    popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
	  }
}
 
