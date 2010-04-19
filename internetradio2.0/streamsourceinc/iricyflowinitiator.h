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
* Description:  Initiates an ICY session.
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Tue Mar 11 20:00:00 2008 by Rohit
*  Ref:
*  Merged IRv1.0 Engine code changes
*
*  Version: 2, Tue Feb 28 18:00:00 2008 by Rohit/Kranthi
*  Ref:
*  Added NetworkController instance for Byte Counter Impl
*
*  </ccm_history>
* ============================================================================
*/

#ifndef IR_IRICYFLOWINITIATOR
#define IR_IRICYFLOWINITIATOR

#include <e32base.h>

#include "irsockettimeouttimer.h"

class RSocket;
class CIRStationConnection;
class TChannelInfo;
class CIRNetworkController;

/**
* This class manages the connection to a channel server
*/
NONSHARABLE_CLASS ( CIRIcyFlowInitiator ) : public CActive,  public MIRSocketTimeoutNotifier
    {
public:

    /**
     * Standard Symbian two-phase construction
     *
     * @param aSocket       Socket to be read.
     * @param aUri          The URI of the ICY source (channel server)
     * @param aOwner        The owner of this CIRIcyFlowReader
     * @param aChannelInfo  Information describing the ICY session (to be filled)
     * @return Instance of CIRIcyFlowInitiator
     */
    static CIRIcyFlowInitiator* NewL( RSocket& aSocket, const TDesC& aUri,
    			 CIRStationConnection& aOwner, TChannelInfo& aChannelInfo );

    /**
     * Default C++ destructor
     */
    ~CIRIcyFlowInitiator();

    /**
     * From base class MIRSocketTimeoutNotifier
     */
    void TimerExpired();

	/**
     * Send an ICY session request (ICY GET) to the channel server
     */
    void RequestFlow();

protected:

// From base class CActive

    /**
     * From base class CActive.
     */
    void RunL();

    /**
     * From base class CActive.
     */
	TInt RunError(TInt aError);

    /**
     * From base class CActive.
     */
    void DoCancel();

private:
	/**
     * The states of the flow initiator
     */
    enum TIRIcyFlowInitiatorState
		{
		EIRIdle,
        EIRSending,
		EIRReceiving,
		EIRFinished
		};

private:

    /**
     * C++ constructor.
     *
     * @param aSocket       Socket to be read.
     * @param aUri          The URI of the ICY source (channel server)
     * @param aOwner        The owner of this CIRIcyFlowReader
     * @param aChannelInfo  Information describing the ICY session (to be filled)
     * @return Instance of CIRIcyFlowInitiator
     */
    CIRIcyFlowInitiator( RSocket& aSocket, const TDesC& aUri, CIRStationConnection& aOwner,
    						 TChannelInfo& aChannelInfo );

    /**
     * Standard Symbian second-phase construction.
     */
    void ConstructL();

	/**
     * Parse and validate the url
     *
     * @return ETrue if all the URI components could be extracted, EFalse otherwise.
     */
    TBool ExtractUriComponentsL();

   	/**
     * Validates the response from the channel server
     *
     * @return ETrue if channel server is acceptable.
     */
    TBool ValidateChannelServer();

	/**
     * Parses the channel headers to get the channel information
     */
	void ParseChannelInfoL();

	/**
	 * Extract the specified meta data information
	 *
	 * @param aMetaField Metafield to be extracted
 	 * @param aBuffer Buffer into which the meta data is to be extracted
     * @return ETrue if extraction is sucessful, EFalse otherwise
     */
	TBool ExtractMetaInfoL(const TDesC8& aMetaField, TDes8 &aBuffer ) const;

private:

	/**
     * Used for timeout mechanism.
     * Owned.
     */
	CIRSocketTimeOutTimer* iSocketTimer;

    /**
     * Holds the resolved host information.
     * Owned.
     */
	RBuf8 iHost;

	/**
     * Holds the resolved path information.
     * Owned.
     */
	RBuf8 iPath;

	/**
	 * Incoming data buffer.
	 * Owned.
	 */
	RBuf8 iBuffer;

	/**
	 * Buffer used to read the data from socket. 
	 * Owned.
	 */
	RBuf8 iReadBuffer;	
	
	/** 
	 * Holds the UAProf string.
	 * Owned.
	 */
	RBuf8 iUAProfString;

	/**
	 * Indicates the state of this IRIcyFlowInitiator
	 */
	TIRIcyFlowInitiatorState iState;

	/**
	 * Socket connected to channel server
	 */
	RSocket& iSocket;

    /**
     * Reference to the URI associated with the channel server
     */
    const TDesC& iUri;

    /**
     * Owner of this object in whole-part relation.
     */
    CIRStationConnection& iOwner;

	/**
     * Information about the current ICY session.
     */
	TChannelInfo& iChannelInfo;

	CIRNetworkController* iNetworkControllerHandle;
    };

#endif // IR_IRICYFLOWINITIATOR



