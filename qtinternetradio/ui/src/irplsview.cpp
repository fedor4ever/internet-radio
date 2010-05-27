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

#include "irplsview.h"
#include "irapplication.h"
#include "irviewmanager.h"
#include "irqnetworkcontroller.h"
#include "irplsmodel.h"
#include "irqisdsdatastructure.h"
#include "irplaycontroller.h"
#include "irplaylist.h"

const QString KActionDeleteName("delete");

IRPlsView::IRPlsView(IRApplication* aApplication, TIRViewId aViewId) : 
                     IrAbstractListViewBase(aApplication, aViewId), iPlsModel(NULL)
{
    //this view won't be starting view, don't need lazy init
    IrAbstractListViewBase::lazyInit();
    setInitCompleted(true);
    
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    
    IRPlayList *playList = iApplication->getPlayList();
    setHeadingText(playList->getFileName());
        
    iPlsModel = new IRPlsModel(playList, this);
    iListView->setModel(iPlsModel);
    iListView->setCurrentIndex(iPlsModel->index(0));
}

IRPlsView::~IRPlsView()
{
}

void IRPlsView::networkRequestNotified(IRQNetworkEvent aEvent)
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
            break;
    }
    
    setUseNetworkReason(EIR_UseNetwork_NoReason);
}

void IRPlsView::handleItemSelected()
{
    int currentRow = iListView->currentIndex().row();
    IRQChannelServerURL server;
    server.bitrate = 0;
    server.url = iPlsModel->getFile(currentRow);
    server.serverName = iPlsModel->getTitle(currentRow);
    IRQPreset preset;
    preset.insertChannelServer(server);
    preset.name = iPlsModel->getTitle(currentRow);
    preset.description = iPlsModel->getFile(currentRow);
    preset.shortDesc = iPlsModel->getFile(currentRow);
    preset.type = 0;
    preset.uniqID = 0;
    preset.presetId = 0;

    iPlayController->connectToChannel(&preset, EIRQAdhocExternal);
}

void IRPlsView::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF &aCoords)
{
    Q_UNUSED(aItem);
    Q_UNUSED(aCoords);
    
    HbMenu *contextMenu = new HbMenu;
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);
    HbAction *action = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
    action->setObjectName(KActionDeleteName);
    contextMenu->open();
    connect(contextMenu, SIGNAL(triggered(HbAction*)), this, SLOT(actionClicked(HbAction*)));
}

void IRPlsView::launchAction()
{
    getViewManager()->pushViewById(EIRView_MainView);
}

void IRPlsView::actionClicked(HbAction *aAction)
{
    if (aAction)
    {
        QString name = aAction->objectName();
        if (KActionDeleteName == name)
        {
            IRPlayList *playList = iApplication->getPlayList();
            playList->deleteItem(iListView->currentIndex().row());
            iListView->reset();
        }
    }
}
