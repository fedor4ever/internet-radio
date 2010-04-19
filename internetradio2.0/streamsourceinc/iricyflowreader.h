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
* Description:  Reads data from opened ICY session
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Tue Feb 28 18:00:00 2008 by Rohit/Kranthi
*  Ref:
*  Added NetworkController instance for Byte Counter Impl
*
*  </ccm_history>
* ============================================================================
*/

#ifndef IR_IRICYFLOWREADER
#define IR_IRICYFLOWREADER


#include <e32base.h>
#include <es_sock.h>

class CIRMetaData;
class CIRNetworkBuffer;
class CIRStationConnection;
class MIRStationDataObserver;
class RSocket;
class TChannelInfo;
class CIRNetworkController;

/**
*  This class handles reading audio and metadata from a connected
* ICY session. It assumes that the socket is connected, the
* ICY session has been requested and initial ICY headers have been
* processed.
*
*/
NONSHARABLE_CLASS( CIRIcyFlowReader ) : public CActive
    {
public: // Constructors and destructors

    /**
     * Standard Symbian two-phase construction
     *
     * @param aSocket       Socket to be read.
     * @param aOwner        The owner of this CIRIcyFlowReader
     * @param aDataObserver The observer wishing to receive audio/metadata.
     * @param aChannelInfo  Information describing the ICY session
     * @return Instance of CIRIcyFlowReader
     */
	static CIRIcyFlowReader* NewL( RSocket& aSocket, CIRStationConnection& aOwner,
	                               MIRStationDataObserver& aDataObserver, 
	                               TChannelInfo& aChannelInfo );

    /**
     * Destructor
     */
	~CIRIcyFlowReader();

public:

	/**
	 * Starts the reading from the socket
	 */
	void Start();

	/**
     * Fills the buffer with the streamed data
     *
     * @param aInputBuffer Buffer into which audio data is to be copied
     */
	void FillBuffer(TDes8& aInputBuffer);

private: // Functions from base classes

	/**
	 * From CActive
	 */
	void RunL();

	/**
	 * From CActive
	 */
	void DoCancel();

private:
	/**
	 * Copies the buffer to the mediaengine sink queue
	 *
	 * @param aData Data to copy.
	 */
	void AddToSinkQueue( const TDesC8& aData );

	/**
	 * Fills the media engine buffer with audio data upon request
	 * by the media engine
	 *
	 * @param aInputBuffer Buffer into which audio data is to be copied
	 */
	void FillMediaEngineBuffer(const TDes8& aInputBuffer);

	/**
     * Fills the unfilled media engine buffers with the remaining audio data
     */
	void FillRemainingBuffers();

	/**
     * C++ default constructor
     */
	CIRIcyFlowReader( RSocket& aSocket, CIRStationConnection& aOwner,
  	                  MIRStationDataObserver& aDataObserver, TChannelInfo& aChannelInfo );

    /**
     * Second phase construction
     */
	void ConstructL();

private:

    /**
     * The parsing states
     */
    enum TIRParsingState
        {
        EIRReadingAudioData,        /**< Reading raw audio data. */
        EIRReadingMetaDataLength,   /**< Reading the length of the meta data. */
        EIRReadingMetaData          /**< Reading meta data. */
        };

	/**
	 * Initiates a read from socket.
	 */
	void IssueRead();

	/**
     * Initializes the buffers that will be used for streaming
     */
	void InitializeBuffersL();

	/**
     * Extracts the meta data from the stream
     */
	void ExtractMetadataL();

    /**
     * Handles received data.
     *
     * Can contain any amount of audio and/or meta data mixed in with it.
     *
     * @param aData Data received.
     */
    void HandleReceivedDataL( const TDesC8& aData );

    /**
     * Handles received audio data.
     *
     * Can only contain raw audio data.
     *
     * @param aData Data received.
     */
    void HandleReceivedAudioData( const TDesC8& aData );

    /**
     * Handles received meta data.
     *
     * Can only contain meta data.
     *
     * @param aData Data received.
     */
    void HandleReceivedMetaData( const TDesC8& aData );
    /**
     * Recognizes if the string is UTF-8 encoded and decodes it,
     * if required.
     * 
     * @param aString The string to be decoded.
     * @return The string in Unicode/ISO-8859-1 format.
     */
    HBufC* DecodeMetadataStringLC( const TDesC8& aString ) const;
    
    /**
     * Tests whether the string is UTF-8 encoded.
     * 
     * @param aString The string to be examined
     * @return ETrue, if the string is UTF-8 encoded, EFalse otherwise. 
     */
    TBool IsUtf8Encoded( const TDesC8& aString ) const;

private:

	/**
	 * The pointer to buffer which holds the audio data.
	 * Owned.
	 */
	TUint8* iSongBuffer;

	/**
	 * Buffer that holds the data received over the network.
	 * Owned.
	 */
	HBufC8* iSocketBuffer;

    /**
     * Buffer to contain unfinished song data.
     * Owned.
     */
    HBufC8* iTempSongBuffer;

    /**
     * Buffer to contain unfinished meta data.
     * Owned.
     */
    HBufC8* iTempMetaBuffer;

    /**
     * Current meta data information.
     * Owned.
     */
    CIRMetaData* iMetaData;

    /**
	 * The socket that read.
	 */
    RSocket& iSocket;

    /**
     * The owner of this object in the whole-part relation.
     */
    CIRStationConnection& iOwner;

    /**
     * The observer wishing to receive audio/metadata
     */
    MIRStationDataObserver& iDataObserver;

	/**
	 * Queue of buffer which is filled
	 */
	TSglQue<CIRNetworkBuffer> iSourceQ;

	/**
	 * Queue of buffer which is empty
	 */
	TSglQue<CIRNetworkBuffer> iSinkQ;

	/**
	 * Temporary holder for network buffer.
	 * Not owned.
	 */
	CIRNetworkBuffer* iTempBufferHolder;

    /**
     * Pointer descriptor to the network data buffer.
     */
    TPtr8 iSocketBufferPtr;

	/**
	 * Indicates the no. of bytes read from the socket
     */
	TSockXfrLength iSocketBytesRead;

	/**
	 * Indicates that stream source ( network side ) buffering is happening.
	 */
	TBool iInitialBuffering;

	/**
	 * Indicates rebuffering is occurring
	 */
	TBool iReBuffering;

	/**
	 * Indicates no of chunks of buffers to be copied
	 */
	TInt iNoOfChunks;

	/**
	 * Indicates the number of chunks are remaining
	 */
	TInt iChunksRemaining;

    /**
     * Length of the current meta data block in bytes.
     */
    TInt iMetaDataLength;

	/**
	 * Pointer to buffer to be filled.
	 * Not owned.
	 */
	TUint8* iBufferFillPointer;

	/**
	 * Used to give buffer fill value to progress bar
	 */
	TInt iBufferCounter;

	/**
	 * Used to determine the metadata position
	 */
	TInt iAudioDataOffset;

	/**
	 * Indicates the data parsing state.
	 */
	TIRParsingState iParsingState;

	/**
	 * Holds the channel information
	 */
	TChannelInfo& iChannelInfo;

	/**
	 * Indicates if the channel info should be published or not
	 */
	TBool iPublishStationInfo;

	/**
	 * NetworkController singleton instance
	 */
	CIRNetworkController* iNetworkControllerHandle;
	};

#endif // IR_IRICYFLOWREADER



