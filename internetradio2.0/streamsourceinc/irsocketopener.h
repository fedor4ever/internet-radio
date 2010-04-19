/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Opens TCP connection.
*
*/


#ifndef IRSOCKETOPENER_H
#define IRSOCKETOPENER_H

#include <e32base.h>
#include <es_sock.h>

#include "irsockettimeouttimer.h"

class RSocket;
class TSockAddr;
class CIRStationConnection;


/**
 * Creates the socket connection a channel server
 */
NONSHARABLE_CLASS ( CIRSocketOpener ) : public CActive,	public MIRSocketTimeoutNotifier
    {
public:
    
    /**
     * Standard Symbian two-phase construction
     *
     * @param aSocket Socket to be opened.
     * @param aOwner The owner of this CIRSocketOpener
     * @return Instance of CIRSocketOpener
     */
    static CIRSocketOpener* NewL( RSocket& aSocket, CIRStationConnection& aOwner );
    
    /**
     * Default C++ destructor
     */
    ~CIRSocketOpener();
    
    /**
     * Called when the timer times out
     */
    void TimerExpired();
    
    /**
     * Connects to the channel server
     *
     * @param aUri URI of channel server.
     */
    void ConnectL( const TDesC& aUri );

protected:
	
	 // from base class CActive
	 
    /**
     * From base class CActive
     *
     * @see CActive::RunL();
     */
    void RunL();
    
    /**
     * From base class CActive
     *
     * @see CActive::DoCancel();
     */
    void DoCancel();

private:

    /**
    * Constructor.
    */
    CIRSocketOpener( RSocket& aSocket, CIRStationConnection& aOwner );

    /**
    * Standard Symbian second-phase construction.
    */
    void ConstructL();
    
    /**
     * Issues asynchronous resolving of the DNS name to IP address
     */
    void ResolveAddress();
	
	/**
     * Issues asynchronous connecting to the channel server
     */
	void ConnectToAddress();
	
	/**
     * Extracts the relevant URI components to member variables.
     *
     * @param aUri URI of the channel server
     */
    TBool ExtractUriComponentsL(const TDesC& aUri);

private:
	
	/**
     * The states of CIRSocketOpener
     */
    enum TIRSocketOpenerState
		{
		EIRIdle,
		EIRResolving,
		EIRConnecting,
		EIRReady
		};

private:
		
	/**
     * Used for timeout mechanism. 
     * Owned.
     */
	CIRSocketTimeOutTimer* iSocketTimer;    

	/**
     * Contains the host part of the URI. 
     * Owned.
     */
	RBuf iHost;
	
	/**
     * For DNS resolving. 
     * Owned.
     */
	RHostResolver iResolver;
    
	/**
     * The current state of CIRSocketOpener
     */
	TIRSocketOpenerState iState;

	/**
     * The address of the server in the form understood by the socket. 
     * Contains the IP address and the port.
     */
	TSockAddr iSockAddr;
	
	/**
     * Contains the resolved host information.
     */
	TNameEntry iResolvedHostInfo;

    /**
     * Contains the port of the URI.
     */	
	TInt iPort;
	
	/**
	 * Socket to be opened.
	 */
	RSocket& iSocket;

    /**
     * The owner in the whole-part relation.
     */	
	CIRStationConnection& iOwner;
    };

#endif // IRSOCKETOPENER_H

