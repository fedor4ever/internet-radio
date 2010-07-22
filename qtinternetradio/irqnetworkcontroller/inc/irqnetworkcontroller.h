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
#ifndef IRQNETWORKCONTROLLER_H_
#define IRQNETWORKCONTROLLER_H_

#include <QObject>

#include "irqenums.h"
#include "irqevent.h"

class IRQNetworkControllerBody;

/**
 * This class provides the interface to IR Network Controller component
 *
 * IRQNetworkController follows a singleton pattern
 * hence the destructor is private and to destroy it we need to use an API
 *
 */

class IRQNetworkController : public QObject
{
    Q_OBJECT

public:

    /**
     *  Get the instance of IRQNetworkController
     *  @return IRQNetworkController*
     */
    IMPORT_C static  IRQNetworkController* openInstance();

    /**
     *  Close the instance of IRQNetworkController
     */
    IMPORT_C void closeInstance();

    /**
     *  Return the variable which indicates if connection is active or not
     *  @return bool
     */
    IMPORT_C bool getNetworkStatus() const;

    /**
     *  Return the IAP Id of the chosen IAP
     *  @return int Error code
     */
    IMPORT_C IRQError getIAPId(unsigned long& aIapId) const;
    
    /**
     *  Configure the Access Point which is used by all the components for network connectivity
     */
    IMPORT_C void  chooseAccessPoint();

    /*
     * Cancel configuring access point
     */
    IMPORT_C void cancelConnecting();
    
    /**
     *  This api is used to determine if the phone is in offline mode
     *  @return True if the phone is in offline mode else False
     */
    IMPORT_C bool isOfflineMode();

    /**
     *  This api is used to determine if the phone supports WLan usage
     *  @return True if the phone supports else False
     */
    IMPORT_C bool isWlanSupported() const;

    /**
     *  Reset the connection status to Disconnected state
     */
    IMPORT_C void resetConnectionStatus();

    /**
     *  Used to determine the type of connection
     *  @return enum describing the type of connection ( GPRS/3G/WiFi )
     */
    IMPORT_C IRQConnectionType identifyConnectionType() const;

    /**
     *  Notifies all observers whose network request is active to reissue the request
     */
    IMPORT_C void notifyActiveNetworkObservers(IRQNetworkEvent aEvent);

    /**
     *  Indicates if the hand over of network connection has happened
     */
    IMPORT_C bool isHandlingOverConnection();

    /**
     *  Indicates if chooseAccessPoint is called
     */
    IMPORT_C bool isConnectRequestIssued() const;
    
signals:

    /**
     *  Notifies all observers whose network request is active to reissue the request
     */
    void networkRequestNotified(IRQNetworkEvent aEvent);

    /**
     *  Notifies all observers whose network request is active to reset the pending request status
     */
    void pendingRequestsReset(bool aValue);

    /**
     *  Notifies the type of network event that occurred
     */
    void networkEventNotified(IRQNetworkEvent aEvent);

    /**
     *  Notifies the error
     */
    void errorOccured(IRQError aError);

private:
    /**
     *  Creates IRQNetworkController instance
     */
    static IRQNetworkController* createInstanceL();
        
    void constructL();

    /**
     *  Default C++ Constructor
     */
    IRQNetworkController();

    /**
     *  Default C++ Destructor
     */
    ~IRQNetworkController();

private:

    /**
     *  IRQNetworkControllerBody instance
     */
    IRQNetworkControllerBody* iBody;

    /**
     * Number of objects currently referring to the singleton object IRQNetworkController
     */
    int iSingletonInstances;
};

#endif /* IRQNETWORKCONTROLLER_H_ */
