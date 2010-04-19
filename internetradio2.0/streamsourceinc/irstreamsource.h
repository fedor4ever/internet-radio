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
* Description:  Manages the entire connection.
*
*/


#ifndef IRSTREAMSOURCE_H
#define IRSTREAMSOURCE_H

#include <e32base.h>

#include "irctrlcommand.h"
#include "irsockettimeouttimer.h"
#include "irstationconnection.h"
#include "irstationdataobserver.h"


// Constants
const TInt KBufferingTimeOut = 60000000;

class CIRMetadataHandler;
class CIRNetworkController;
class MIRStreamSourceObserver;

/**
 * This class acts as a manager class for the streamsource component
 *
 * @code
 *
 * // Create a stream source object which is used for streaming data
 * // iObserver is the reference of the observer object which is called
 * // for issuing call backs.
 * CIRStreamSource iStreamSource = CIRStreamSource::NewL(iObserver);
 *
 * // Called to connect to the channel server iUrl specifying the 
 * // url to be connect to.
 * iStreamSource->ConnectToServerL(iUrl);
 *
 * // Called by the media engine to indicate that its buffers are to
 * // to be filled and iInputBuffer is the reference of the buffer to
 * // which data is to be copied
 * iStreamSource->FilltheBuffer(iInputbuffer);
 *
 * // Called to cancel a channel server connection request
 * iStreamSource->CancelRequest();
 *
 * // Called by the mediaclient to know the content-type
 * // of the audio stream
 * iStreamSource->ContentType();
 *
 * @endcode
 */ 

NONSHARABLE_CLASS ( CIRStreamSource ): public CBase, public MIRStreamFill,
	public MIRSocketTimeoutNotifier, public MIRStationConnectionObserver,
	 public MIRStationDataObserver
	{
public:

	/** 
	 *  Initiates a connection to the url.
	 *
	 *  @param aUrl Url of the channel server to connect to.
	 */
	IMPORT_C void ConnectToServerL(const TDesC& aUrl);

	/** 
	 *  Reinitiates a connection to the url by
     *
	 *  @param aUrl Url of the channel server to connect to.
	 */
	IMPORT_C void ReconnectL(const TDesC& aUrl );

	/** 
	 *  Creates an Instance of CIRStreamSource
	 *  @param aObserver Reference to the StreamSource Observer
	 *  @return CIRStreamSource* pointer to the created CIRStreamSource object
	 */	
	IMPORT_C static CIRStreamSource* NewL(MIRStreamSourceObserver& aObserver);

	/** 
	 *  Fills the buffer with the data from the stream
	 *  @param aInputBuffer Reference to the Input Buffer
	 */	
	IMPORT_C void FilltheBuffer(TDes8& aInputBuffer);

	/** 
	 *  Cancels the connection request
	 */	
	IMPORT_C void CancelRequest();

	/** 
	 *  Returns the content type header information of the channel
	 *  @return const TDesC8& content type header information of the channel
	 */	
	IMPORT_C const TDesC8& ContentTypeL();

	/** 
	 *  Default C++ Destructor
	 */
	virtual ~CIRStreamSource();

private:

	/** 
	 *  Creates a connection object and initiates the connection
	 *  to channel server
	 */	
	void DoConnectL(const TDesC& aUrl);

	/** 
	 *  Default Constructor
	 *  @param aObserver Reference to StreamSource observer
	 */
	CIRStreamSource(MIRStreamSourceObserver& aObserver);

	/** 
	 *  2nd Phase construction.
	 */
	void ConstructL();

//from base class MIRStationConnectionObserver

    /**
     * From base class MIRStationConnectionObserver.
     *
     * @see MIRStationConnectionObserver::ConnectionSuccessful()
     */
	void ConnectionSuccessful( CIRStationConnection* aConnection );

    /**
     * From base class MIRStationConnectionObserver.
     *
     * @see MIRStationConnectionObserver::ConnectionError()
     */
	void ConnectionError( CIRStationConnection* aConnection, TInt aErrorCode );

//	from base class MIRSocketTimeoutNotifier
     
    /**
     * From base class MIRSocketTimeoutNotifier.
     *
     * @see MIRSocketTimeoutNotifier::TimerExpired()
     */
	void TimerExpired();	

// from base class MIRStationDataObserver

    /**
     * From base class MIRStationDataObserver.
     *
     * @see MIRStationDataObserver::MetadataReceived()
     */
    void MetadataReceived( const CIRMetaData& aMetaData );

    /**
     * From base class MIRStationDataObserver.
     *
     * @see MIRStationDataObserver::AudioDataEvent()
     */
    void AudioDataEvent( const TInt aResponseCode,	TInt aValue );

    /**
     * Schedules an asynchronous cleanup for given connection.
     *
     * @param aConnection The connection to be deleted.
     */
    void AsyncCleanupConnection( CIRStationConnection* aConnection );

    /**
     * Callback method that performs the actual deletion of connections.
     *
     * @param aSelfPtr Pointer argument given when the callback/timer is created.
     * @return KErrNone Specified in the function pointer declaration.
     */	
    static TInt StaticConnectionCleanupCallback( TAny* aSelfPtr );	

private:

	/**
	 * Handle to the Network controller used to get the reference
	 * of the RSocketServ and RConnection. 
	 * Not owned.
	 */
    CIRNetworkController* iNetworkControllerHandle;

	/**
	 * Timer for handling timeouts. 
	 * Owned
	 */
    CIRSocketTimeOutTimer* iSocketTimer;

	/**
	 * Pointer to the connection to current channel server. 
	 * Not owned.
	 */
    CIRStationConnection* iCurrentConnection;

	/**
	 * Pointer to the connection to previous channel server. 
	 * Not owned.
	 */
    CIRStationConnection* iNewConnection;

	/**
	 * Array of pointers to connections. 
	 * Owned.
	 */
    RPointerArray<CIRStationConnection> iConnections;
    
    /**
     * Timer for delayed cleanup of connections. 
     * Owned.
     */	 
    CPeriodic* iConnectionCleanupTimer;

	/**
	 * Reference of the Stream source observer
	 */
    MIRStreamSourceObserver& iStreamSourceObserver;
	
	/**
	 * Is set when reconnecting to a channel and reset 
	 * when connecting for the first time
	 */
    TBool iReConnecting;

	};

#endif //IRSTREAMSOURCE_H



