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
* Description:  ICY flow reader implementation
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Tue Feb 28 18:00:00 2008 by Rohit/Kranthi
*  Ref:
*  Setting RawDataTransferredL() into DataTransferTracker for Byte Counter Impl
*
*  </ccm_history>
* ============================================================================
*/
#include <utf.h>

#include "iricyflowreader.h"
#include "irdebug.h"
#include "irmediaenginebuffer.h"
#include "irmetadata.h"
#include "irnetworkbuffer.h"
#include "irstationconnection.h"
#include "irstationdataobserver.h"
#include "irstreamsourceerrors.h"
#include "irnetworkcontroller.h"

const TInt KMaxSongBufferSize = 61440;
const TInt KNoInputBuffers = 60;
const TInt KMaxBufferChunkSize = 1024;
const TInt KMaxSocketBufferSize = 1024;
const TInt KBufferPercentageInc = 1;
const TInt KSixteen = 16;
const TInt KThree=3;
_LIT8( KIRStreamTitle, "StreamTitle='" );
_LIT8( KIRStreamUrl, "StreamUrl='" );
_LIT8( KIRMetaDataEndIdentifier, "';" );
_LIT8( KIRSongDelimiter, " - " );

// masks and prefices used in UTF-8 recognition
const TInt KIRUtf8_2B1stByteMask = 0xE0;
const TInt KIRUtf8_3B1stByteMask = 0xF0;
const TInt KIRUtf8_4B1stByteMask = 0xF8;
const TInt KIRUtf8FollowingByteMask = 0xC0;

const TInt KIRUtf8_2B1stBytePrefix = 0xC0;
const TInt KIRUtf8_3B1stBytePrefix = 0xE0;
const TInt KIRUtf8_4B1stBytePrefix = 0xF0;
const TInt KIRUtf8FollowingBytePrefix = 0x80;
// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CIRIcyFlowReader::NewL
// ---------------------------------------------------------------------------
//
CIRIcyFlowReader* CIRIcyFlowReader::NewL( RSocket& aSocket, CIRStationConnection& aOwner,
	               MIRStationDataObserver& aDataObserver, TChannelInfo& aChannelInfo )
    {
    CIRIcyFlowReader* self = new ( ELeave ) CIRIcyFlowReader( aSocket, aOwner,
    					 aDataObserver, aChannelInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::CIRIcyFlowReader
// ---------------------------------------------------------------------------
//
CIRIcyFlowReader::CIRIcyFlowReader( RSocket& aSocket, CIRStationConnection& aOwner,
	              MIRStationDataObserver& aDataObserver, TChannelInfo& aChannelInfo )
    :CActive( EPriorityStandard ), iSocket( aSocket ), iOwner( aOwner ),
    				 iDataObserver( aDataObserver ),
      iSocketBufferPtr( NULL, 0 ), iChannelInfo( aChannelInfo )
    {

    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::~CIRIcyFlowReader
// ---------------------------------------------------------------------------
//
CIRIcyFlowReader::~CIRIcyFlowReader()
	{
	Cancel();
	while(!iSinkQ.IsEmpty())
		{
		//Deleting all the entries in sink buffers queue
		iTempBufferHolder = iSinkQ.First();
		iSinkQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		}
	while(!iSourceQ.IsEmpty())
		{
		//deleting all the entries in source buffers queue
		iTempBufferHolder = iSourceQ.First();
		iSourceQ.Remove(*iTempBufferHolder);
		delete iTempBufferHolder;
		}
	delete[] iSongBuffer;
	delete iSocketBuffer;
	delete iTempSongBuffer;
	delete iTempMetaBuffer;
    delete iMetaData;
	if(iNetworkControllerHandle)
		{
		iNetworkControllerHandle->Close();
		}
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::ConstructL()
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::ConstructL()
    {
    IRLOG_DEBUG( "CIRIcyFlowReader::ConstructL" );
    iAudioDataOffset = iChannelInfo.iAudioDataOffset;
    TInt f_off = _FOFF( CIRNetworkBuffer, iLink ); //for the buffer queue which is maintained
    iSourceQ.SetOffset( f_off ); 	// It is Queue of buffer given to socket to fill
    iSinkQ.SetOffset( f_off );  // It is Queue of buffer given to media engine
    InitializeBuffersL();
    iMetaData = CIRMetaData::NewL();
    CActiveScheduler::Add( this );
    iNetworkControllerHandle = CIRNetworkController::OpenL();
    IRLOG_DEBUG( "CIRIcyFlowReader::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::HandleReceivedDataL
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::HandleReceivedDataL( const TDesC8& aData )
    {
    switch ( iParsingState )
        {
        case EIRReadingAudioData:
            {
            if ( iAudioDataOffset + aData.Length() > iChannelInfo.iMetaInterval )
                {
                // Part of this data contains meta data information already.
                TInt audioDataAmount = iChannelInfo.iMetaInterval - iAudioDataOffset;
                // Only the audio part of the data is added to the song buffer.
                HandleReceivedAudioData( aData.Left( audioDataAmount ) );
                iParsingState = EIRReadingMetaDataLength;
                iAudioDataOffset = 0; // Resets the audio data offset, will start increment again after meta data is handled.
                HandleReceivedDataL( aData.Mid( audioDataAmount ) ); // Recursive call to handle meta data mixed in with this audio data block.
                }
            else // All of it is data is audio data.
                {
                HandleReceivedAudioData( aData );
                }
            break;
            }
        case EIRReadingMetaDataLength:
            {
            // ICY protocol specifies that meta data length is the first byte of the data multiplied by 16.
            iMetaDataLength = aData[0] * KSixteen;

            delete iTempMetaBuffer;
            iTempMetaBuffer = NULL;

            if ( iMetaDataLength > 0 ) // Meta data is provided, so we have to parse it.
                {
                iTempMetaBuffer = HBufC8::NewL( iMetaDataLength );
                iParsingState = EIRReadingMetaData;
                }
            else // No meta data available, so resume reading audio data.
                {
                iParsingState = EIRReadingAudioData;
                }

            if ( aData.Length() > 1 ) // Just to check that the data doesn't only contain the length byte.
                {
                HandleReceivedDataL( aData.Mid( 1 ) ); // Strips off the length byte. Recursive call as data can also contain audio data.
                }

            break;
            }
        case EIRReadingMetaData:
            {
            if ( iTempMetaBuffer->Length() + aData.Length() > iMetaDataLength )
                {
                // All of the meta data block is now received, and part of it is continuation to the audio data.
                TInt metaDataAmount = iMetaDataLength - iTempMetaBuffer->Length();
                HandleReceivedMetaData( aData.Left( metaDataAmount ) );
                ExtractMetadataL(); // Extracts the meta data from the temporary meta data buffer.
                iParsingState = EIRReadingAudioData;
                HandleReceivedDataL( aData.Mid( metaDataAmount ) ); // Strips off the meta data from the descriptor.
                }
            else // All of it is meta data.
                {
                HandleReceivedMetaData( aData );
                }
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::HandleReceivedAudioData
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::HandleReceivedAudioData( const TDesC8& aData )
    {
    // Check to see if we've got enough audio data to fill a buffer.
    if ( iTempSongBuffer->Length() + aData.Length() >= KMaxBufferChunkSize )
        {
		// Data contains more audio data than the buffer can handle.
		TInt amountToAdd = KMaxBufferChunkSize - iTempSongBuffer->Length();

		// Enough audio data to fill the buffer is added.
		iTempSongBuffer->Des().Append( aData.Left( amountToAdd ) );

		AddToSinkQueue( *iTempSongBuffer );
		
		//while loop is written for only if left over amount in aData is 
		//greater then 1024, then it should be again added into SinkQueue
		while(1)
			{
			iTempSongBuffer->Des().Zero();
	 		//calculates the length of the leftover amount to be added
			TInt length = aData.Length() - amountToAdd;

			if(length <= 0)
				{
				break;	
				}
			else if(length >= KMaxBufferChunkSize) //if the left over amount is >= 1024 then add to SinkQueue
				{
				iTempSongBuffer->Des().Append( aData.Mid( amountToAdd,KMaxBufferChunkSize ) );
		 		// updates the amountToAdd value by 1024
				amountToAdd += KMaxBufferChunkSize;

				AddToSinkQueue( *iTempSongBuffer );
				}
			else //if the left over amount is < 1024 then append to tempSongBuffer & break
				{
				// Then the overflowing audio data part is added to the new clean buffer.
				iTempSongBuffer->Des().Append( aData.Mid( amountToAdd ) );
				break;	
				}
			}
        }
    else // There is enough room in the temporary audio buffer to hold all of the data.
        {
        iTempSongBuffer->Des().Append( aData );
        }

    iAudioDataOffset += aData.Length();



    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::HandleReceivedMetaData
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::HandleReceivedMetaData( const TDesC8& aData )
    {
    iTempMetaBuffer->Des().Append( aData );
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::RunL
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::RunL()
    {
     // Active object request complete handler
    switch ( iStatus.Int() )
        {
        case KErrNone:
            {
			// Byte Counter Impl
			iNetworkControllerHandle->DataTransferTracker().RawDataTransferredL( 0,
					 iSocketBufferPtr.Size(), MIRDataTransferTracker::EIRTransferCategoryAudio);

            HandleReceivedDataL( *iSocketBuffer );

            if ( !iSourceQ.IsEmpty() )
            	{
            	//issue a read on empty buffer
            	IssueRead();
            	}
            else
            	{
            	if( iReBuffering )
            		{
            		//	if rebuffering call continue using sink buffer
            		FillRemainingBuffers();
            		}
            	if( iInitialBuffering )
            		{
            		//  if first time intimate media client to fill the buffer
            		iDataObserver.AudioDataEvent( MIRStationDataObserver::EBufferFilled, KErrNone );
            		iInitialBuffering = EFalse;
            		}
            	}

		 	break;
			}
        case KErrDisconnected:
        	{
        	IRLOG_ERROR( "CIRIcyFlowReader::RunL - KErrDisconnected");
        	iOwner.ConnectionError( KIRStreamSourceDisconnected );
        	}
            break;
        case KErrEof:
        	{
        	IRLOG_INFO( "CIRIcyFlowReader::RunL - KErrEof" );
        	iOwner.ConnectionError( KIRStreamSourceNoResponse );
        	}
        	break;
        default:
        	{
        	IRLOG_ERROR2( "CIRIcyFlowReader::RunL - Error (%d)", iStatus.Int() );
            iOwner.ConnectionError( KIRStreamSourceReadError );
        	}
            break;
        }
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::DoCancel
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::DoCancel()
    {
    iSocket.CancelRead();
    }



// ---------------------------------------------------------------------------
// CIRIcyFlowReader::IssueRead
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::IssueRead()
    {
	if( !IsActive() )
		{
	    iSocketBufferPtr.Zero();
	    iSocket.Read( iSocketBufferPtr, iStatus );
	    SetActive();
		}
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::Start
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::Start()
    {
    IRLOG_INFO( "CIRIcyFlowReader::Start" );
    // Initiate a new read from socket into iBuffer
    iInitialBuffering = ETrue;
    iBufferCounter = 0;
    iPublishStationInfo = ETrue;
	IssueRead();
	IRLOG_DEBUG( "CIRIcyFlowReader::Start - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRIcyFlowReader::InitializeBuffersL
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::InitializeBuffersL()
	{
	IRLOG_DEBUG( "CIRIcyFlowReader::InitializeBuffersL" );
	// Allocate the buffer for audio data on heap
	iSongBuffer = new TUint8[KMaxSongBufferSize];
	User::LeaveIfNull( iSongBuffer );

	IRLOG_INFO2( "CIRIcyFlowReader::InitializeBuffersL - Reserved %d bytes of memory", KMaxSongBufferSize );
	TUint8* bufferaddress = iSongBuffer;
	// since sink buffers are not created initially all buffers are filled with data and appended to sink buffer
	// Create buffers ans append to source buffer queue
	for(TInt buffercount = 0; buffercount < KNoInputBuffers; buffercount++ )
		{
		iTempBufferHolder = CIRNetworkBuffer::NewL(bufferaddress,
			KMaxBufferChunkSize);
		iSourceQ.AddLast(*iTempBufferHolder);
		bufferaddress += KMaxBufferChunkSize;
		}
	// Create a buffer for the data read from socket
	iSocketBuffer = HBufC8::NewL( KMaxSocketBufferSize );
	iSocketBufferPtr.Set( iSocketBuffer->Des() );
    iTempSongBuffer = HBufC8::NewL( KMaxSocketBufferSize );
	IRLOG_DEBUG( "CIRIcyFlowReader::InitializeBuffersL - Exiting." );
	}



// ---------------------------------------------------------------------------
// CIRIcyFlowReader::FillBuffer
// Fills the mediaengine buffer and rebuffers if necessary
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::FillBuffer(TDes8& aInputBuffer)
	{
	FillMediaEngineBuffer(aInputBuffer);
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::AddToSinkQueue
// Adds the filled buffers to the sink Q so that it can be copied to media
// engine buffer
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::AddToSinkQueue( const TDesC8& aData )
	{
	//call from runL
	//removes the buffer from source queue and put in sink queue

	if( !iSourceQ.IsEmpty() )
		{
		iTempBufferHolder = iSourceQ.First();
		TPtr8 bufferPointer(iTempBufferHolder->Des() ,KMaxBufferChunkSize,
			KMaxBufferChunkSize );
		bufferPointer.Copy(aData);
		iSourceQ.Remove(*iTempBufferHolder);
		iSinkQ.AddLast(*iTempBufferHolder);
		if( iInitialBuffering )
			{
			iBufferCounter += KBufferPercentageInc;
			iDataObserver.AudioDataEvent( MIRStationDataObserver::EBufferPercentage, iBufferCounter );
			}
		}
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::FillMediaEngineBuffer
// Fills the data into media engine's buffer
// aInputBuffer Buffer into which data is to be filled
// ---------------------------------------------------------------------------
//

void CIRIcyFlowReader::FillMediaEngineBuffer(const TDes8& aInputBuffer)
	{

	if( !iReBuffering )
		{
		//Determine no of bytes of data to be filled
		TInt copyLength = aInputBuffer.MaxLength();
		// Calculate the no of 1K chunks
		iNoOfChunks = copyLength/KMaxBufferChunkSize;
		// Initiailly remaining chunks to be filled is same as total no of
		// chunks to be filled
		iChunksRemaining = iNoOfChunks;
		IRLOG_DEBUG3( "CIRIcyFlowReader::FillMediaEngineBuffer - Copying %d bytes/%d chunks", copyLength, iNoOfChunks );
		// Store the starting address of buffer into which data is to be
		// copied
		iBufferFillPointer = (TUint8 *)aInputBuffer.Ptr();
		// Start filling of the empty media engine buffers
		FillRemainingBuffers();
		}
	}


// ---------------------------------------------------------------------------
// CIRIcyFlowReader::FillRemainingBuffers
// Fills the data into media engine's remaining buffers
// called when the stream source runs out of buffers and
// there is a pending request to media engine
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::FillRemainingBuffers()
	{
	TUint8* mediaBufferAddress = iBufferFillPointer;
	TInt chunksFilled = iNoOfChunks - iChunksRemaining;
	mediaBufferAddress += ( chunksFilled * KMaxBufferChunkSize );

	TInt bufferNumber = iChunksRemaining;
	while ( bufferNumber )
		{
		if ( !iSinkQ.IsEmpty() )
			{
			iTempBufferHolder = iSinkQ.First();
			TPtr8 mediaBufferPointer(mediaBufferAddress,KMaxBufferChunkSize,
				KMaxBufferChunkSize );
			mediaBufferPointer.Copy( iTempBufferHolder->Des() ,
				KMaxBufferChunkSize);
			TPtr8 tempBufferPointer(iTempBufferHolder->Des(),
				KMaxBufferChunkSize,KMaxBufferChunkSize );
			tempBufferPointer.Delete(KMaxBufferChunkSize,
				KMaxBufferChunkSize);
			iSinkQ.Remove(*iTempBufferHolder);
			iSourceQ.AddLast(*iTempBufferHolder);
			iChunksRemaining--;
			bufferNumber--;
			iReBuffering = EFalse;
			mediaBufferAddress += KMaxBufferChunkSize;
			//issue  source rebuffering here if source is not empty
			if( !iSourceQ.IsEmpty() )
				{
				IssueRead();
				}
			}
		else
			{
			//rebuffer if sink buffer is empty
			bufferNumber = 0;
			iReBuffering = ETrue;
			//issue  source rebuffering here if source is not empty
			if( !iSourceQ.IsEmpty() )
				{
				IssueRead();
				}
			//break from for loop
			}
		}
	iBufferCounter += (K100Percentage - KNoInputBuffers) / KIRInputBufferCount;
    if ( iBufferCounter > K100Percentage )
        {
        iBufferCounter = K100Percentage;
        }
    iDataObserver.AudioDataEvent( MIRStationDataObserver::EBufferPercentage, iBufferCounter );
	if( !iReBuffering )
		{
        iDataObserver.AudioDataEvent( MIRStationDataObserver::EOpenComplete, KErrNone );
		}
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::ExtractMetadataL
// Extracts the meta data from the stream
// ---------------------------------------------------------------------------
//
void CIRIcyFlowReader::ExtractMetadataL()
	{
    IRLOG_DEBUG( "CIRIcyFlowReader::ExtractMetaDataL" );

    // Erases old meta data information.
    iMetaData->SetArtistL( KNullDesC );
    iMetaData->SetSongL( KNullDesC );
    iMetaData->SetStreamUrlL( KNullDesC );

    TPtrC8 ptr( *iTempMetaBuffer );

    TInt streamTitleIndex = ptr.Find( KIRStreamTitle );
    TInt streamUrlIndex = ptr.Find( KIRStreamUrl );

    // Extracts the "StreamTitle" part of the meta data.
    if ( streamTitleIndex >= 0 )
        {
        TPtrC8 streamTitlePtr( ptr.Mid( streamTitleIndex + KIRStreamTitle().Length() ) );
        TInt streamTitleEndIndex = streamTitlePtr.Find( KIRMetaDataEndIdentifier );
        if ( streamTitleEndIndex >= 0 )
            {
            streamTitlePtr.Set( streamTitlePtr.Left( streamTitleEndIndex ) );

            TPtrC8 artistPtr( KNullDesC8 );
            TPtrC8 songPtr( KNullDesC8 );

            TInt songDelimiterIndex = streamTitlePtr.Find( KIRSongDelimiter );
            if ( songDelimiterIndex >= 0 )
                {
                artistPtr.Set( streamTitlePtr.Left( songDelimiterIndex ) );
                songPtr.Set( streamTitlePtr.Mid( songDelimiterIndex + 
                					KIRSongDelimiter().Length() ) );
                }
            else
                {
                IRLOG_WARNING( "CIRIcyFlowReader::ExtractMetaDataL - Song delimiter was not found" );
                artistPtr.Set( streamTitlePtr );
                }

            HBufC* artist = DecodeMetadataStringLC( artistPtr );
            iMetaData->SetArtistL( *artist );
            CleanupStack::PopAndDestroy( artist );

            HBufC* song = DecodeMetadataStringLC( songPtr );
            iMetaData->SetSongL( *song );
            CleanupStack::PopAndDestroy( song );
            }
        else
            {
            IRLOG_WARNING( "CIRIcyFlowReader::ExtractMetaDataL - \"StreamTitle\" end was not found" );
            }
        }
    else
        {
        IRLOG_WARNING( "CIRIcyFlowReader::ExtractMetaDataL - \"StreamTitle\" was not found" );
        }

    // Extracts the "StreamUrl" part of the meta data.
    if ( streamUrlIndex >= 0 )
        {
        TPtrC8 streamUrlPtr( ptr.Mid( streamUrlIndex + KIRStreamUrl().Length() ) );

        TInt streamUrlEndIndex = streamUrlPtr.Find( KIRMetaDataEndIdentifier );
        if ( streamUrlEndIndex >= 0 )
            {
            streamUrlPtr.Set( streamUrlPtr.Left( streamUrlEndIndex ) );
            HBufC* streamUrl = HBufC::NewLC( streamUrlPtr.Length() );
            streamUrl->Des().Copy( streamUrlPtr );  // 8 bit to 16 bit descriptor conversion.
            iMetaData->SetStreamUrlL( *streamUrl );
            CleanupStack::PopAndDestroy( streamUrl );
            }
        else
            {
            IRLOG_WARNING( "CIRIcyFlowReader::ExtractMetaDataL - \"StreamUrl\" end was not found" );
            }
        }
    else
        {
        IRLOG_WARNING( "CIRIcyFlowReader::ExtractMetaDataL - \"StreamUrl\" was not found" );
        }

    iDataObserver.MetadataReceived( *iMetaData );

    IRLOG_INFO4( "CIRIcyFlowReader::ExtractMetaDataL - Exit (artist=%S, song=%S, streamUrl=%S)", &iMetaData->Artist(), &iMetaData->Song(), &iMetaData->StreamUrl() );
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::DecodeMetadataStringLC
// ---------------------------------------------------------------------------
//
HBufC* CIRIcyFlowReader::DecodeMetadataStringLC( const TDesC8& aString ) const
    {
    IRLOG_DEBUG( "CIRIcyFlowReader::DecodeMetadataStringLC" );
    HBufC* decodedString = NULL;
    if ( IsUtf8Encoded( aString ) )
        {
        TRAPD( err, 
              IRLOG_DEBUG( "CIRIcyFlowReader::DecodeMetadataStringLC - String is UTF-8 encoded" );
              decodedString = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aString );
             )
        if ( err != KErrNone )
            {
            IRLOG_ERROR2( "CIRIcyFlowReader::DecodeMetadataStringLC - UTF-8 conversion failed, err=%d", err );
            decodedString = HBufC::NewL( aString.Length() ); 
            decodedString->Des().Copy( aString ); // 8 bit to 16 bit descriptor conversion (ISO-8859-1).    
            }
        }
    else
        {
        decodedString = HBufC::NewL( aString.Length() ); 
        decodedString->Des().Copy( aString ); // 8 bit to 16 bit descriptor conversion (ISO-8859-1).    
        }
    CleanupStack::PushL( decodedString );
    IRLOG_DEBUG2( "CIRIcyFlowReader::DecodeMetadataStringLC - Returning %S", decodedString );    
    return decodedString;
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowReader::IsUtf8Encoded
// ---------------------------------------------------------------------------
//
TBool CIRIcyFlowReader::IsUtf8Encoded( const TDesC8& aData ) const
    {
    IRLOG_DEBUG( "CIRIcyFlowReader::IsUtf8Encoded" );    
    TBool foundUtf8( EFalse );
    
    for ( TInt i(0); i + 1 < aData.Length() && !foundUtf8; i++ )
        {
        if ( ( aData[i] & KIRUtf8_2B1stByteMask ) == KIRUtf8_2B1stBytePrefix )
            {
            // Two-byte presentation: 110yyyyy 10zzzzzz
            if ( ( aData[i + 1] & KIRUtf8FollowingByteMask ) == KIRUtf8FollowingBytePrefix )
                {
                foundUtf8 = ETrue;
                }
            }
        else if ( ( aData[i] & KIRUtf8_3B1stByteMask ) == KIRUtf8_3B1stBytePrefix && 
                  i + 2 < aData.Length() )
            {
            // Three-byte presentation: 1110xxxx 10yyyyyy 10zzzzzz
            if ( ( aData[i + 1] & KIRUtf8FollowingByteMask ) == KIRUtf8FollowingBytePrefix && 
                 ( aData[i + 2] & KIRUtf8FollowingByteMask ) == KIRUtf8FollowingBytePrefix )
                {
                foundUtf8 = ETrue;
                }
            }
        else if ( ( aData[i] & KIRUtf8_4B1stByteMask ) == KIRUtf8_4B1stBytePrefix &&
                i + KThree < aData.Length() )
            {
            // Four-byte presentation: 11110www 10xxxxxx 10yyyyyy 10zzzzzz
            if ( ( aData[i + 1] & KIRUtf8FollowingByteMask ) == KIRUtf8FollowingBytePrefix && 
                 ( aData[i + 2] & KIRUtf8FollowingByteMask ) == KIRUtf8FollowingBytePrefix && 
                 ( aData[i + KThree] & KIRUtf8FollowingByteMask ) == KIRUtf8FollowingBytePrefix )
                {
                foundUtf8 = ETrue;
                }
            }
        else
            {
            // NOP
            }
        }
    IRLOG_DEBUG2( "CIRIcyFlowReader::IsUtf8Encoded - Returning %d", foundUtf8 );    
    return foundUtf8;
    }

