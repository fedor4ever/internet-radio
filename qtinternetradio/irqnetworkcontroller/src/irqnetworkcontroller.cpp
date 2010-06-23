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
#include <QStringList>

#include "irqnetworkcontroller.h"
#include "irqnetworkcontrollerbody.h" 

// ---------------------------------------------------------------------------
// IRQNetworkController::openInstance()
// Static function to get a singleton instance of IRQNetworkController
// @return IRQNetworkController *
// ---------------------------------------------------------------------------
//
EXPORT_C IRQNetworkController* IRQNetworkController::openInstance()
{
    // Get singleton instance
    IRQNetworkController* irqnetworkcontroller =
                           reinterpret_cast<IRQNetworkController*>(Dll::Tls());

    if (NULL == irqnetworkcontroller)
    {
        TRAPD(error, irqnetworkcontroller = createInstanceL());
        if (KErrNone != error)
        {
            delete irqnetworkcontroller;
            irqnetworkcontroller = NULL;
            Dll::SetTls(NULL);
        }
    }
    else
    {
        irqnetworkcontroller->iSingletonInstances++;
    }

    return irqnetworkcontroller;
}

// ---------------------------------------------------------------------------
// IRQNetworkController::closeInstance()
// Close a singleton instance of IRQNetworkController
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNetworkController::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkController::GetNetworkStatus()
// Returns the variable which indicates if connection is active or not
// @return bool
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQNetworkController::getNetworkStatus() const
{
    return iBody->getNetworkStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::getIAPId()
// Gets the IAP Id of the chosen IAP
// @return IRQError
// ---------------------------------------------------------------------------
//
EXPORT_C IRQError IRQNetworkController::getIAPId(unsigned long& aIapId) const
{
    return iBody->getIAPId(aIapId);
}

// ---------------------------------------------------------------------------
// IRQNetworkController::chooseAccessPoint()
// Configures the Access Point which is used by all the components for network
// connectivity
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNetworkController::chooseAccessPoint()
{
    iBody->chooseAccessPoint();
}

/*
 * Cancel configuring access point
 */
EXPORT_C void IRQNetworkController::cancelConnecting()
{
    iBody->cancelConnecting();
    iBody->resetConnectionStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isOfflineMode()
// Indicates if the phone is in offline mode
// @return True if the phone is in offline mode else False
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQNetworkController::isOfflineMode()
{
    return iBody->isOfflineMode();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isWlanSupported()
// Indicates if the phone supports WLan usage
// @return True if the phone supports else False
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQNetworkController::isWlanSupported() const
{
    return iBody->isWlanSupported();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::resetConnectionStatus()
// Resets the connection status to Disconnected state
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNetworkController::resetConnectionStatus()
{
    iBody->resetConnectionStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::identifyConnectionType()
// Indicates the type of connection
// @return enum describing the type of connection ( GPRS/3G/WiFi )
// ---------------------------------------------------------------------------
//
EXPORT_C IRQConnectionType IRQNetworkController::identifyConnectionType() const
{
    return iBody->identifyConnectionType();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::notifyActiveNetworkObservers()
// Notifies all observers whose network request is active to reissue the request
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQNetworkController::notifyActiveNetworkObservers(IRQNetworkEvent aEvent)
{
    iBody->notifyActiveNetworkObservers(aEvent);
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isHandlingOverConnection()
// Indicates if the hand over of network connection has happened
// @return bool
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQNetworkController::isHandlingOverConnection()
{
    return iBody->isHandlingOverConnection();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::isConnectRequestIssued()
// Indicates if chooseAccessPoint is called
// @return bool
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQNetworkController::isConnectRequestIssued() const
{
    return iBody->isConnectRequestIssued();
}

// ---------------------------------------------------------------------------
// IRQNetworkController::IRQNetworkController()
// Default constructor
// ---------------------------------------------------------------------------
//
IRQNetworkController::IRQNetworkController()
{
}

// ---------------------------------------------------------------------------
// IRQNetworkController::~IRQNetworkController()
// Destructor
//
// ---------------------------------------------------------------------------
//
IRQNetworkController::~IRQNetworkController()
{
    delete iBody;
    iBody = NULL;
}

// ---------------------------------------------------------------------------
// IRQNetworkController::constructL()
// Two-Phase Constructor.
// ---------------------------------------------------------------------------
//
void IRQNetworkController::constructL()
{
    iBody = new (ELeave) IRQNetworkControllerBody();
    Q_ASSERT(iBody);
    iBody->initL();
    
    connect(iBody, SIGNAL(networkRequestNotified(IRQNetworkEvent)),
            this, SIGNAL(networkRequestNotified(IRQNetworkEvent)));
    connect(iBody, SIGNAL(pendingRequestsReset(bool)),
            this, SIGNAL(pendingRequestsReset(bool)));
    connect(iBody, SIGNAL(networkEventNotified(IRQNetworkEvent)),
            this, SIGNAL(networkEventNotified(IRQNetworkEvent)));
    connect(iBody, SIGNAL(errorOccured(IRQError)),
            this, SIGNAL(errorOccured(IRQError)));
}

// ---------------------------------------------------------------------------
// IRQNetworkController::createInstanceL()
// Creates IRQNetworkController instance
// ---------------------------------------------------------------------------
//
IRQNetworkController* IRQNetworkController::createInstanceL()
{
    IRQNetworkController* nwkController = new (ELeave) IRQNetworkController();
    nwkController->constructL();
    User::LeaveIfError(Dll::SetTls(nwkController));
    nwkController->iSingletonInstances = 1;
    return nwkController;
}
