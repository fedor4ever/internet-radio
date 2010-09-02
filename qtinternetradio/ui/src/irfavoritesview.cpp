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
#include <HbInputDialog>
#include <hbscrollbar.h>
#include <HbLineEdit>

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
                                   iStationShare(NULL),
                                   iMultiDeleteDialog(NULL),
                                   iMultiDeleteAction(NULL),
                                   iLogoPreset(NULL)
{    
    setFlag(EViewFlag_ClearStackWhenActivate|EViewFlag_StickyViewEnabled);
    
	initToolBar();
    iModel = new IRFavoritesModel(iFavorites, this);
    iListView->setModel(iModel);
    
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
    
    delete iMultiDeleteDialog;
    iMultiDeleteDialog = NULL; 
    
    delete iStationShare;
    iStationShare = NULL;
}

TIRHandleResult IRFavoritesView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    
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
        
        getViewManager()->saveScreenShot();
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


#ifdef HS_WIDGET_ENABLED
void IRFavoritesView::itemAboutToBeSelected(bool &aNeedNetwork)
{
    aNeedNetwork =  true;
    
    int currentIndex = iListView->currentIndex().row();
    iPlayController->setConnectingStationName(iModel->getPreset(currentIndex)->name);     
}
#endif

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
#ifdef SUBTITLE_STR_BY_LOCID
        header->setHeading(hbTrId("txt_irad_subtitle_select_items_to_delete"));
#else
        header->setHeading(hbTrId("Select items to delete"));        
#endif
        iMultiDeleteDialog->setHeadingWidget(header);
    }

    iMultiDeleteDialog->setModel(NULL);
    iMultiDeleteDialog->setModel(iModel);
    QList<QAction *> oriActions = iMultiDeleteDialog->actions();
    if (oriActions.count() >= 2)
    {
#ifdef SUBTITLE_STR_BY_LOCID
        oriActions.at(0)->setText(hbTrId("txt_common_button_delete"));
        oriActions.at(1)->setText(hbTrId("txt_common_button_cancel"));
#else
        oriActions.at(0)->setText("Delete");
        oriActions.at(1)->setText("Cancel");       
#endif
    }
    
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
#ifdef SUBTITLE_STR_BY_LOCID 
        iMultiDeleteAction = new HbAction(hbTrId("txt_irad_opt_delete_stations"), menu());
#else
        iMultiDeleteAction = new HbAction(hbTrId("Delete stations"), menu());        
#endif
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
#ifdef SUBTITLE_STR_BY_LOCID
    QString headingStr = hbTrId("txt_irad_subtitle_favorites") + " (" + QString::number(iModel->rowCount()) + ")";
#else
    QString headingStr = hbTrId("Favorites") + " (" + QString::number(iModel->rowCount()) + ")";
#endif
    setHeadingText(headingStr);
    updateIconIndexArray();
    
    iListView->reset();
    iListView->setCurrentIndex(iModel->index(0));
    iListView->verticalScrollBar()->setValue(0.0);
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
#ifdef SUBTITLE_STR_BY_LOCID
                popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
#else
                popupNote(hbTrId("Operation failed"), HbMessageBox::MessageTypeWarning);                
#endif
            }

            updateIconIndexArray();
#ifdef SUBTITLE_STR_BY_LOCID
            QString headingStr = hbTrId("txt_irad_subtitle_favorites") + " (" + QString::number(iModel->rowCount()) + ")";
#else
            QString headingStr = hbTrId("Favorites") + " (" + QString::number(iModel->rowCount()) + ")";
#endif
            setHeadingText(headingStr);  
			
            if (!iIconIndexArray.empty())
            {
                iConvertTimer->start();
            }
        }
    }   
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
            popupDeleteContextConfirmMessageBox();                            
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
    
    HbAction *action = NULL;
    
    HbMenu *contextMenu = new HbMenu(); 
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    connect(contextMenu, SIGNAL(triggered(HbAction*)), this, SLOT(actionClicked(HbAction*)));
    
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_irad_opt_share"));
#else
    action = contextMenu->addAction(hbTrId("Share"));
#endif
    action->setObjectName(KActionShareName);
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
#else
    action = contextMenu->addAction(hbTrId("Delete"));    
#endif
    action->setObjectName(KActionDeleteName);
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_common_menu_rename_item"));
#else
    action = contextMenu->addAction(hbTrId("Rename"));
#endif
    action->setObjectName(KActionRenameName);
#ifdef SUBTITLE_STR_BY_LOCID
    action = contextMenu->addAction(hbTrId("txt_common_menu_details"));
#else
    action = contextMenu->addAction(hbTrId("Details"));    
#endif
    action->setObjectName(KActionDetailsName);
    
    contextMenu->setPreferredPos(aCoords);
    contextMenu->open();         
} 

void IRFavoritesView::shareContextAction()
{
    int current = iListView->currentIndex().row();

    if (NULL == iStationShare)
    {
        iStationShare = new IRStationShare();
    }
    iStationShare->shareStations(*iModel->getPreset(current));    
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
    int current = iListView->currentIndex().row();
    IRQPreset *preset = iModel->getPreset(current);

    HbInputDialog *dlg = new HbInputDialog;
#ifdef SUBTITLE_STR_BY_LOCID    
    dlg->setPromptText(hbTrId("txt_common_menu_rename_item"));
#else
    dlg->setPromptText(hbTrId("Rename"));
#endif
    HbLineEdit* lineEdit = dlg->lineEdit();
    lineEdit->setMaxLength(256);
    dlg->setInputMode(HbInputDialog::TextInput);
    dlg->setValue(preset->name);   
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->open(this, SLOT(renameConfirmed(HbAction*)));
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
#ifdef SUBTITLE_STR_BY_LOCID
        QString headingStr = hbTrId("txt_irad_subtitle_favorites") + " (" + QString::number(iModel->rowCount()) + ")";
#else
        QString headingStr = hbTrId("Favorites") + " (" + QString::number(iModel->rowCount()) + ")";
#endif
        setHeadingText(headingStr);          
    }
    else
    {
#ifdef SUBTITLE_STR_BY_LOCID
        popupNote(hbTrId("txt_irad_info_operation_failed"), HbMessageBox::MessageTypeWarning);
#else
        popupNote(hbTrId("Operation failed"), HbMessageBox::MessageTypeWarning);        
#endif
    }
    
    if (!iIconIndexArray.empty())
    {
        iConvertTimer->start();
    }    
}

void IRFavoritesView::renameConfirmed(HbAction *aAction)
{
    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());
    if (dialog)
    {
        if (aAction == dialog->actions().at(0))
        {
            int current = iListView->currentIndex().row();
            IRQPreset *preset = iModel->getPreset(current);
            
            QString newName = dialog->value().toString().trimmed();
            if (newName.isEmpty())
            {
#ifdef SUBTITLE_STR_BY_LOCID
                newName = hbTrId("txt_irad_info_unnamed_station");
#else
                newName = hbTrId("Unnamed station");                
#endif
            }
            
            if (newName == preset->name)
            {
                return;
            }
            
            int ret = iFavorites->renamePreset(*preset, newName);
            switch (ret)
            {
            case EIRQErrorNotFound:
                //popup note : not found
                break;
                
            case EIRQErrorAlreadyExist:
                //popup note : already exists
                break;
                
            case EIRQErrorNone:
                iModel->updateFavoriteName(current, newName);
                break;
                
            default:
                break;
            }  
        }
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

void IRFavoritesView::popupDeleteContextConfirmMessageBox()
{
#ifdef SUBTITLE_STR_BY_LOCID
    HbMessageBox::question(hbTrId("txt_irad_info_delete_station"), this, SLOT(deleteContext(HbAction*)), HbMessageBox::Ok | HbMessageBox::Cancel);
#else
    HbMessageBox::question("Delete station?", this, SLOT(deleteContext(HbAction*)), HbMessageBox::Ok | HbMessageBox::Cancel);    
#endif
}

void IRFavoritesView::deleteContext(HbAction *aAction)
{
    HbMessageBox *dialog = static_cast<HbMessageBox*>(sender());
    if (dialog)
    {
        if (aAction == dialog->actions().at(0))
        {
            deleteContextAction();
        }
    }        
}
