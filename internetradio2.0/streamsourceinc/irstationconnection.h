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
* Description:  Represents a connection to a radio station.
*
*/


#ifndef MIRSTATIONCONNECTIONOBSERVER_H
#define MIRSTATIONCONNECTIONOBSERVER_H

#include <e32base.h>
#include <es_sock.h>

class CIRStationConnection;
class MIRStationDataObserver;
class CIRSocketOpener;
class CIRIcyFlowInitiator;
class CIRIcyFlowReader;
class TChannelInfo;

/**
 * Observer interface for the CIRStationConnection.
 */
class MIRStationConnectionObserver
	{
public:	
	/**
	 * Called to indicate sucessful connection
	 *
	 * @param aConnection Pointer to successful connection.
	 */	
	virtual void ConnectionSuccessful( CIRStationConnection* aConnection ) = 0;
	
	/*
	 * Called to indicate failure in connection.
	 *
	 * @param aConnection The connection that failed.
	 * @param aErrorCode Error code indicating the type of the error
	 */
	virtual void ConnectionError( CIRStationConnection* aConnection, TInt aErrorCode ) = 0;
    };


const TInt KMAXMETABUFLENGTH = 256;
/**
 * Holds the meta data information 
 */
class TChannelInfo
	{
public:
	TBuf8<KMAXMETABUFLENGTH>	iStationName;
	TBuf8<KMAXMETABUFLENGTH>	iGenre;
	TBuf8<KMAXMETABUFLENGTH>	iBitRate;
	TBuf8<KMAXMETABUFLENGTH>	iContentType;
	TBuf8<KMAXMETABUFLENGTH> 	iSongTitle;
	TBuf8<KMAXMETABUFLENGTH> 	iArtistName;
	TInt iMetaInterval;
	TInt iMetaDataTracker;
	TInt iReadSize;
	TInt iAudioDataOffset;
	};

/**
 * Instance of the class encompasses the connection establishment, error handling and
 * audio/meta data reading from the stream. 
 *
 *
 */
NONSHARABLE_CLASS ( CIRStationConnection ): public CBase
	{
public:
    /**
     * The type of the connection
     */
    enum TIRStationConnectionType
		{
		EIRUndefined,
		EIRPermanent,
		EIRCandidate,
		EIRDying
		};
	
public:


	/** 
	 * Creates an instance of CIRStationConnection
	 *
	 * @param aConnectionObserver   Observer for connection related events.
	 * @param aDataObserver         Observer for the audio/meta data.
	 * @return Instance of CIRStationConnection
	 */	
    static CIRStationConnection* NewL( MIRStationConnectionObserver& aConnectionObserver,
    			 MIRStationDataObserver& aDataObserver );

	/** 
	 *  Initiates a connection to internet radio station specified by the URI.
     *
	 *  @param aUri URI of the radio station server.
	 */
    void ConnectL( const TDesC& aUri );

    /**
     * Closes the connection. First cancels any pending asynchronous request and 
     * then closes the socket.
     */
    void Close();

    /**
     * Sets the connection type.
     *
     * @param aType New connection type 
     */
    void SetConnectionType( TIRStationConnectionType aType );
    
    /**
     * Gets the connection type.
     *
     * @return The type of the connection.
     */    
    TIRStationConnectionType ConnectionType() const;
    
    /**
     * Gets the URI associated with the connection.
     *
     * @return URI associated with the connection.
     */
    const TDesC& Uri() const;

    /**
     * Gets the content type of the connected stream.
     *
     * @return The content type of the connected stream.
     */
    const TDesC8& ContentType() const;
    
	/** 
	 *  Fills the buffer with the data from the stream.
	 *
	 *  @param aInputBuffer Buffer into which the data is copied.
	 */	
    void FillBuffer( TDes8& aInputBuffer );

	/** 
	 *  Default C++ Destructor
	 */
	~CIRStationConnection();
	
public:
    // Methods for composite parts to call
    /**
     * Called by a composite part when the transport layer session is ready.
     */
	void ConnectionReady();
	
	/**
	 * Called by a composite part when the application layer session is ready.
	 */
	void FlowReady();		
	
	/*
     * Called by a composite part to report an error in connection.
     *
	 * @param aErrorCode type of error
	 */
	void ConnectionError( TInt aErrorCode );

private:

	/** 
	 *  Default Constructor
	 * @param aConnectionObserver   Observer for connection related events.
	 * @param aDataObserver         Observer for the audio/meta data.
	 * @return Instance of CIRStationConnection
     */
	CIRStationConnection( MIRStationConnectionObserver& aStationConnectionObserver,
					 MIRStationDataObserver& aDataObserver );

	/** 
	 *  2nd Phase construction.
	 */
	void ConstructL();


private:

	/**
	 * Starts the TCP connection. 
	 * Owned.
	 */
	CIRSocketOpener* iSocketOpener;

    /**
     * Starts the ICY session. 
     * Owned.
     */
    CIRIcyFlowInitiator* iIcyFlowInitiator;

	/**
	 * Reads the ICY session. 
	 * Owned.
	 */
	CIRIcyFlowReader* iIcyFlowReader;

    /** 
     * The socket. 
     * Owned.
     */
	RSocket iSocket;
	
    /**
     * URI of the radio channel. 
     * Owned.
     */
    RBuf iUri;

    /**
     * The connection event observer 
     */
	MIRStationConnectionObserver& iConnectionObserver;
	
	/**
	 * Audio/metadata observer.
	 */
	MIRStationDataObserver& iDataObserver;

    /**
     * Information related to connected ICY channel.
     */
    TChannelInfo iChannelInfo;

    /**
     * Type of the connection.
     */    
    TIRStationConnectionType iConnectionType;
	};

#endif // MIRSTATIONCONNECTIONOBSERVER_H
