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
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbgroupbox.h>
#include <hbselectiondialog.h>
#include <QPixmap>
#include <QTimer>

#include "irviewmanager.h"
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
#include "iruidefines.h"
#include "irstationshare.h"
#include "irstationdetailsview.h"
const int KBitmapSize = 59;

const QString KActionShareName("Share");
const QString KActionDeleteName("Delete");
const QString KActionRenameName("Rename");
const QString KActionDetailsName("Details");

//                                        public functions

/*
 * Description : constructor
 */
IRFavoritesView::IRFavoritesView(IRApplication *aApplication, TIRViewId aViewId) 
                                 : IrAbstractListViewBase(aApplication, aViewId),
                                   iMultiDeleteDialog(NULL),
                                   iMultiDeleteAction(NULL),
                                   iLogoPreset(NULL)
{    
    setFlag(EViewFlag_ClearStackWhenActivate|EViewFlag_StickyViewEnabled);
    
    //if this view is not starting view, finish all initialization in constructor
    if (getViewManager()->views().count() > 0)
    {
        normalInit();
    }
}

/*
 * Description : destructor
 */
IRFavoritesView::~IRFavoritesView()
{
    delete iLogoPreset;
    iLogoPreset = NULL;
    
    delete iMultiDeleteDialog;
    iMultiDeleteDialog = NULL;    
}

TIRHandleResult IRFavoritesView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    
    if (!initCompleted())
    {
        return EIR_DoDefault;
    }
    
    TIRHandleResult ret = IrAbstractListViewBase::handleCommand(aCommand, aReason);
    int leftCount = 0;
    
    switch (aCommand)
    {
    case EIR_ViewCommand_TOBEACTIVATED:       
        iModel->checkFavoritesUpdate();
        ret = EIR_NoDefault;
        break;
                
    case EIR_ViewCommand_ACTIVATED:        
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

void IRFavoritesView::lazyInit()
{
    if (!initCompleted())
    {
        normalInit();
        
        //initialization from handleCommand()
        handleCommand(EIR_ViewCommand_TOBEACTIVATED, EIR_ViewCommandReason_Show);
        handleCommand(EIR_ViewCommand_ACTIVATED, EIR_ViewCommandReason_Show);
    }
}

void IRFavoritesView::normalInit()
{
    if (!initCompleted())
    {
        IrAbstractListViewBase::lazyInit();
        initToolBar();
        
        iModel = new IRFavoritesModel(iFavorites, this);
        iListView->setModel(iModel);
        
        iConvertTimer = new QTimer(this);
        iConvertTimer->setInterval(10);
        
        connect(iModel, SIGNAL(modelChanged()), this, SLOT(modelChanged()));
        connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
                this, SLOT(networkRequestNotified(IRQNetworkEvent))); 
        connect(iConvertTimer, SIGNAL(timeout()), this, SLOT(convertAnother()));
        
        setInitCompleted(true);
    }
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
// IRFavoritesView::deleteFavorites()
//---------------------------------------------------------------------------
void IRFavoritesView::deleteFavorites()
{
    if (NULL == iMultiDeleteDialog)
    {
        iMultiDeleteDialog = new HbSelectionDialog;        
        iMultiDeleteDialog->setSelectionMode(HbAbstractItemView::MultiSelection);
        
        HbGroupBox * header = new HbGroupBox;
        header->setHeading(hbTrId("txt_irad_select_items_to_delete"));
        iMultiDeleteDialog->setHeadingWidget(header);
        
        
        QList<QAction *> oriActions = iMultiDeleteDialog->actions();
        for(int i = 0; i< oriActions.count(); i++)
        {
            iMultiDeleteDialog->removeAction(oriActions.at(i));
            delete oriActions.at(i);
        }

        iMultiDeleteDialog->addAction(new HbAction(hbTrId("txt_common_button_delete")));
        iMultiDeleteDialog->addAction(new HbAction(hbTrId("txt_common_button_cancel")));
    }

    iMultiDeleteDialog->setModel(iModel);
    iMultiDeleteDialog->open(this,SLOT(deleteDialogClosed(HbAction*)));
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

void IRFavoritesView::prepareMenu()
{
    if (NULL == iMultiDeleteAction)
    {
        iMultiDeleteAction = new HbAction(hbTrId("txt_irad_opt_delete_favorites"), menu());
        connect(iMultiDeleteAction, SIGNAL(triggered()), this, SLOT(deleteFavorites()));
    }

    HbMenu *viewMenu = menu();

    viewMenu->removeAction(iMultiDeleteAction);

    HbAction * settingAction = qobject_cast<HbAction *>(iLoader.findObject(SETTINGS_ACTION));

    if (iModel->rowCount() > 0)
    {
        viewMenu->insertAction(settingAction, iMultiDeleteAction);
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
    QString headingStr = QString::number(iModel->rowCount()) + QString(" ") + hbTrId("txt_irad_subtitle_stations");
    setHeadingText(headingStr);
    updateIconIndexArray();
    
    iListView->reset();
    iListView->setCurrentIndex(iModel->index(0));
}

void IRFavoritesView::deleteDialogClosed(HbAction *aAction)
{
    if (iMultiDeleteDialog->actions().at(0) == aAction)
    {
        QModelIndexList selectedIndexes = iMultiDeleteDialog->selectedModelIndexes();
        if (!selectedIndexes.empty())
        {
            if (!iIconIndexArray.empty())
            {
                iIsdsClient->isdsLogoDownCancelTransaction();
                iConvertTimer->stop();
            }
            
            if(!iModel->deleteMultiFavorites(selectedIndexes))
            {
                popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
            }

            updateIconIndexArray();
            QString headingStr = QString::number(iModel->rowCount()) + QString(" ") + hbTrId("txt_irad_subtitle_stations");
            setHeadingText(headingStr);  
			
            if (!iIconIndexArray.empty())
            {
                iConvertTimer->start();
            }
        }
    }
    iMultiDeleteDialog->setModel(NULL);    
}

void IRFavoritesView::actionClicked(HbAction *aAction)
{
    if ( aAction )
    {
        QString objectName = aAction->objectName();
        if (objectName == KActionShareName)
        {
            shareContextAction();
        }
        else if ( objectName == KActionDeleteName)
        {
            deleteContextAction();                            
        }
        else if (objectName == KActionRenameName)
        {
            renameContextAction();
        }
        else if (objectName == KActionDetailsName)
        {
            detailsContextAction();
        }
    } 
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
    Q_UNUSED(aCoords);
    
    HbAction *action = NULL;
    
    HbMenu *contextMenu = new HbMenu(); 
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    connect(contextMenu, SIGNAL(triggered(HbAction*)), this, SLOT(actionClicked(HbAction*)));
    
    action = contextMenu->addAction(hbTrId("txt_irad_opt_share_station"));
    action->setObjectName(KActionShareName);
    action = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
    action->setObjectName(KActionDeleteName);
    action = contextMenu->addAction(hbTrId("txt_common_menu_rename_item"));
    action->setObjectName(KActionRenameName);
    action = contextMenu->addAction(hbTrId("txt_common_menu_details"));
    action->setObjectName(KActionDetailsName);
    
    contextMenu->open();         
} 

void IRFavoritesView::shareContextAction()
{
    int current = iListView->currentIndex().row();

    IRStationShare stationShare;
    stationShare.shareStations(*iModel->getPreset(current));    
}

void IRFavoritesView::updateIconIndexArray()
{
    iIconIndexArray.clear();
    
    for (int i = 0; i < iModel->rowCount(); ++i)
    {
        if (iModel->getImgUrl(i) != "" 
            && !iModel->isLogoReady(i))
        {
            iIconIndexArray.append(i);
        }
    }    
}

void IRFavoritesView::renameContextAction()
{
    
}

void IRFavoritesView::detailsContextAction()
{   
    int selectedItemIndex = iListView->currentIndex().row();
    IRQPreset *channelPreset = iModel->getPreset(selectedItemIndex); 
    
    IRStationDetailsView *stationDetailsView = static_cast<IRStationDetailsView*>(getViewManager()->getView(EIRView_StationDetailsView, true));
    stationDetailsView->setDetails(channelPreset);    

    getViewManager()->activateView(EIRView_StationDetailsView);
}

void IRFavoritesView::deleteContextAction()
{
    if (!iIconIndexArray.empty())
    {
        iIsdsClient->isdsLogoDownCancelTransaction();
        iConvertTimer->stop();
    }  
    
    int current = iListView->currentIndex().row();    
    if (iModel->deleteOneFavorite(current))
    {
        updateIconIndexArray();
        QString headingStr = QString::number(iModel->rowCount()) + QString(" ") + hbTrId("txt_irad_subtitle_stations");
        setHeadingText(headingStr);          
    }
    else
    {
        popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
    }
    
    if (!iIconIndexArray.empty())
    {
        iConvertTimer->start();
    }    
}

void IRFavoritesView::initToolBar()
{
    //add HbActions to the toolbar, the HbActions have been created in IrAbstractListViewBase
    HbToolBar *viewToolBar = toolBar();
    viewToolBar->addAction(iGenresAction);
    viewToolBar->addAction(iCollectionsAction);
    viewToolBar->addAction(iFavoritesAction);
    viewToolBar->addAction(iSearchAction);
}
