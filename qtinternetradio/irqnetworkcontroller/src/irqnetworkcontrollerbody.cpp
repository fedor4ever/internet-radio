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
#include "irqnetworkcontrollerbody.h"

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::~IRQNetworkControllerBody()
// Destructor
//
// ---------------------------------------------------------------------------
//
IRQNetworkControllerBody::~IRQNetworkControllerBody()
{
    if (iNetworkController)
    {
        iNetworkController->Close();
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::GetNetworkStatus()
// Returns the variable which indicates if connection is active or not
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerBody::getNetworkStatus() const
{
    return (bool)iNetworkController->GetNetworkStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::getIAPId()
// Gets the IAP Id of the chosen IAP
// @return IRQError
// ---------------------------------------------------------------------------
//
IRQError IRQNetworkControllerBody::getIAPId(unsigned long& aIapId) const
{
    IRQError retval = EIRQErrorNone;

    if (KErrNotFound == iNetworkController->GetIAPId(aIapId))
    {
        retval = EIRQErrorNotFound;
    }

    return retval;
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::getAccessPointList()
// Returns the list of available access points
// @return QStringList &aList specifying the access point names
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::getAccessPointList(QStringList &aList)
{
    const CDesCArray *apArray = iNetworkController->GetAccessPointList();
    int count = apArray->MdcaCount();
    for (int i = 0; i < count; ++i)
    {
        QString str = QString::fromUtf16(apArray->MdcaPoint(i).Ptr(), apArray->MdcaPoint(i).Length());
        aList.append(str);
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::getApIdList()
// Returns the list of iap ids for available access points
// @return QList<unsigned long> &aList specifying the iap ids
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::getApIdList(QList<unsigned long> &aList)
{
    const RArray<TUint32>& accessPointIDArray = iNetworkController->GetApList();
    for (int i = 0; i < accessPointIDArray.Count(); ++i)
    {
        aList.append(accessPointIDArray[i]);
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::getBearerList()
// Returns the list of bearer ids for available access points
// @return QList<unsigned long> &aList specifying the bearer ids
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::getBearerList(QList<unsigned long> &aList)
{
    const RArray<TUint32>& bearerIDArray = iNetworkController->GetBearerList();
    for (int i = 0; i < bearerIDArray.Count(); ++i)
    {
        aList.append(bearerIDArray[i]);
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::getNetworkList()
// Returns the list of network ids for available access points
// @return QList<unsigned long> &aList specifying the network ids
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::getNetworkList(QList<unsigned long> &aList)
{
    const RArray<TUint32>& networkIDArray = iNetworkController->GetNetworkList();
    for (int i = 0; i < networkIDArray.Count(); ++i)
    {
        aList.append(networkIDArray[i]);
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::chooseAccessPoint()
// Configures the Access Point which is used by all the components for network
// connectivity
// ---------------------------------------------------------------------------
//
void  IRQNetworkControllerBody::chooseAccessPoint()
{
    TRAPD(error, iNetworkController->ChooseAccessPointL());
    if (KErrNone != error)
    {
        emit errorOccured(EIRQErrorGeneral);
    }
}

/*
 * Cancel configuring access point
 */
void IRQNetworkControllerBody::cancelConnecting()
{
    iNetworkController->CancelConnecting();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::isOfflineMode()
// Indicates if the phone is in offline mode
// @return True if the phone is in offline mode else False
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerBody::isOfflineMode()
{
    return (bool)iNetworkController->IsOfflineMode();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::isWlanSupported()
// Indicates if the phone supports WLan usage
// @return True if the phone supports else False
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerBody::isWlanSupported() const
{
    return (bool)iNetworkController->IsWlanSupported();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::resetConnectionStatus()
// Resets the connection status to Disconnected state
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::resetConnectionStatus()
{
    iNetworkController->ResetConnectionStatus();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::identifyConnectionType()
// Indicates the type of connection
// @return enum describing the type of connection ( GPRS/3G/WiFi )
// ---------------------------------------------------------------------------
//
IRQConnectionType IRQNetworkControllerBody::identifyConnectionType() const
{
    return (IRQConnectionType)iNetworkController->IdentifyConnectionType();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::notifyActiveNetworkObservers()
// Notifies all observers whose network request is active to reissue the request
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::notifyActiveNetworkObservers(IRQNetworkEvent aEvent)
{
    TRAPD(error, iNetworkController->NotifyActiveNetworkObserversL((TIRNetworkEvent)aEvent));
    if (KErrNone != error)
    {
        emit errorOccured(EIRQErrorGeneral);
    }
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::isHandlingOverConnection()
// Indicates if the hand over of network connection has happened
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerBody::isHandlingOverConnection()
{
    return (bool)iNetworkController->IsHandingOverConnection();
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::isConnectRequestIssued()
// Indicates if chooseAccessPoint is called
// @return bool
// ---------------------------------------------------------------------------
//
bool IRQNetworkControllerBody::isConnectRequestIssued() const
{
    return (bool)iNetworkController->iIsConnectRequestIssued;
}

// ---------------------------------------------------------------------------
// MIRActiveNetworkObserver::NotifyActiveNetworkObserversL()
// Callback which notifies all observers whose network request is active to reissue the request
// @param aEvent Indicates the type of network event that occurred
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
{
    emit networkRequestNotified((IRQNetworkEvent)aEvent);
}

// ---------------------------------------------------------------------------
// MIRActiveNetworkObserver::ResetPendingRequests()
// Callback which notifies all observers whose network request is active to
// reset the pending request status
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::ResetPendingRequests(TBool aValue)
{
    emit pendingRequestsReset((bool)aValue);
}

// ---------------------------------------------------------------------------
// MIRNetworkController::IRNetworkEventL()
// @param aEvent Indicates the type of network event that occurred
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::IRNetworkEventL(TIRNetworkEvent aEvent)
{
    emit networkEventNotified((IRQNetworkEvent)aEvent);
}

// ---------------------------------------------------------------------------
// IRQNetworkControllerBody::initL()
// Create resources
//
// ---------------------------------------------------------------------------
//
void IRQNetworkControllerBody::initL()
{
    // Get singleton instance
    iNetworkController = CIRNetworkController::OpenL(this);
    iNetworkController->RegisterActiveNetworkObserverL(*this);
}
