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
#include "irqnwkinfoobserver.h"
#include "irqnwkinfoobserverimpl.h"

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::openInstance()
// Static function to get a singleton instance of IRQNwkInfoObserver
// ---------------------------------------------------------------------------
//
EXPORT_C IRQNwkInfoObserver* IRQNwkInfoObserver::openInstance()
{
    // Get singleton instance
    IRQNwkInfoObserver* nwkInfoObserver =
                           reinterpret_cast<IRQNwkInfoObserver*>(Dll::Tls());

    if(NULL == nwkInfoObserver)
    {
        TRAPD(error, nwkInfoObserver = createInstanceL());
        if (KErrNone != error)
        {
            delete nwkInfoObserver;
            nwkInfoObserver = NULL;
            Dll::SetTls(NULL);
        }
    }
    else
    {
        nwkInfoObserver->iSingletonInstances++;
    }

    return nwkInfoObserver;
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::closeInstance()
// Close a singleton instance of IRQNwkInfoObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNwkInfoObserver::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserver::IRQNwkInfoObserver():iBody(NULL),
                                         iSingletonInstances(0)
{
    
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserver::~IRQNwkInfoObserver()
{
    stopMonitorNwkInfo();
    
    delete iBody;
    iBody = NULL;
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::createInstanceL()
// Creates IRQNwkInfoObserver instance
// @return IRQNwkInfoObserver*
// ---------------------------------------------------------------------------
//
IRQNwkInfoObserver* IRQNwkInfoObserver::createInstanceL()
{
    IRQNwkInfoObserver* nwkInfoObserver = new (ELeave) IRQNwkInfoObserver();
    nwkInfoObserver->constructL();
    User::LeaveIfError(Dll::SetTls(nwkInfoObserver));
    nwkInfoObserver->iSingletonInstances = 1;

    return nwkInfoObserver;
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::constructL()
// Two-Phase Constructor.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserver::constructL()
{
    iBody = IRQNwkInfoObserverImpl::NewL(this);
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::startMonitorNwkInfo()
// start monitoring network info change
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNwkInfoObserver::startMonitorNwkInfo()
{
    if(iBody)
    {
        iBody->startNwkInfoMonitor();
    }
}

// ---------------------------------------------------------------------------
// IRQNwkInfoObserver::stopMonitorNwkInfo()
// stop monitoring network info change
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNwkInfoObserver::stopMonitorNwkInfo()
{
    if(iBody)
    {
        iBody->Cancel();
    }
}

// ---------------------------------------------------------------------------
// From MIRQNwkInfoUpdate.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserver::updateCurrentNwkInfo(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC)
{
    emit currentNwkChanged(aCurrentNwkMCC,aCurrentNwkMNC);
}


// ---------------------------------------------------------------------------
// From MIRQNwkInfoUpdate.
// ---------------------------------------------------------------------------
//
void IRQNwkInfoObserver::updateHomeNwkInfo(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC)
{
    emit homeNwkChanged(aHomeNetworkMCC,aHomeNetworkMNC);
}
