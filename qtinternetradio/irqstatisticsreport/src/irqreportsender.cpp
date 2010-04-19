/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  interface for updating network info
*
*/


#include "irqreportsender.h"
#include "irqisdsclient.h"
#include "irqsettings.h"
#include "irqnetworkcontroller.h"
#include "irreportsettings.h"

// ---------------------------------------------------------------------------
// IRQReportSender::IRQReportSender()
// Constructor
// ---------------------------------------------------------------------------
//
IRQReportSender::IRQReportSender(IRQIsdsClient *aIsdsClient):iIsdsClient(aIsdsClient),
                                                             iSettingManager(NULL),
                                                             iNetworkController(NULL)
{
    iNetworkController = IRQNetworkController::openInstance();
    
    iSettingManager = IRQSettings::openInstance();
}

// ---------------------------------------------------------------------------
// IRQReportSender::~IRQReportSender()
// Destructor
// ---------------------------------------------------------------------------
//
IRQReportSender::~IRQReportSender()
{
    if(iSettingManager)
    {
        iSettingManager->closeInstance();
    }
    
    if(iNetworkController)
    {
        iNetworkController->closeInstance();
    }    
}

// ---------------------------------------------------------------------------
// IRQReportSender::LogDbNeedFlush()
// From MIRLogDbStatusObserver
// ---------------------------------------------------------------------------
//
void IRQReportSender::LogDbNeedFlush()
{
    sendReport();
}

// ---------------------------------------------------------------------------
// IRQReportSender::sendReport()
// Send the statistics report to isds server
// ---------------------------------------------------------------------------
//
void IRQReportSender::sendReport()
{
    if(iNetworkController)
    {
    	  if(iNetworkController->getNetworkStatus() 
    	     && iIsdsClient && iSettingManager)
    	  {
            QString logFilePath = iSettingManager->privatePath();
            QString logFileName = QString::fromUtf16(KGZipLogFileName().Ptr(), KGZipLogFileName().Length());
            iIsdsClient->isdsPostLog(logFilePath + logFileName);
            emit reportSent();
        }
    }
}
