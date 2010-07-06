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

#include <QCoreApplication>
#include <QSettings>

#include "irmemorycollector.h"
#include "irapplication.h"
#include "irviewmanager.h"
#include "irservicedef.h"

IRMemoryCollector::IRMemoryCollector(IRApplication* aApplication) : iApplication(aApplication)
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

void IRMemoryCollector::aboutToQuit()
{
    IRViewManager *viewManager = iApplication->getViewManager();
    
    //we need to save the status for splash view, need to extend in future
    TIRViewId exitingID = viewManager->getExitingView();
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    QVariant data(QVariant::Bool);
    bool isNowplaying = (EIRView_PlayingView == exitingID)? true:false;
    data.setValue(isNowplaying);
    settings.setValue(KIrSettingSplashNowplaying,data); 
    
    delete iApplication;
    iApplication = NULL;
        
    delete viewManager;  
}
