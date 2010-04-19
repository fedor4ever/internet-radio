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

#include "irplsview.h"
#include "irapplication.h"
#include "irabstractviewmanager.h"
#include "irqnetworkcontroller.h"
#include "irplsmodel.h"
#include "irqisdsdatastructure.h"
#include "irplaycontroller.h"

IRPlsView::IRPlsView(IRApplication* aApplication, TIRViewId aViewId) : 
                     IrAbstractListViewBase(aApplication, aViewId), iPlsModel(NULL)
{
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SLOT(networkRequestNotified(IRQNetworkEvent)));
    setHeadingText(hbTrId("txt_irad_subtitle_stations_from_play_list"));
        
    iPlsModel = new IRPlsModel(this);
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
            iApplication->closeConnectingDialog();
            
            if (EIR_UseNetwork_SelectItem == getUseNetworkReason())
            {
                handleItemSelected();
            }
            setUseNetworkReason(EIR_UseNetwork_NoReason);
            
            break;
            
        default:
            break;
    }
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

void IRPlsView::updateView()
{
    iListView->reset();
    iListView->setCurrentIndex(iPlsModel->index(0));
}
