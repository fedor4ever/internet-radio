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
* Description:  ICY flow initiator implementation
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
*  Setting RawDataTransferredL() into DataTransferTracker for Byte Counter Impl
*
*  </ccm_history>
* ============================================================================
*/

#include <e32property.h>
#include <es_sock.h>
#include <in_sock.h>
#include <uriutils.h>

#include "iricyflowinitiator.h"
#include "irdebug.h"
#include "irnetworkcontroller.h"
#include "irpubsubkeys.h"
#include "irstationconnection.h"
#include "irstreamsourceerrors.h"
#include "irstreamsourceliterals.h"

// Constants
const TInt KIRFITimeOutValue = 10000000;
const TInt KIRHeaderMaxSize = 256;
_LIT8( KIRUriComponentSeparator, "/" );
const TInt KFour = 4;
const TInt KSixtyFour=64;
const TInt KTwoZeroFourEight=2048;
// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::NewL
// ---------------------------------------------------------------------------
//
CIRIcyFlowInitiator* CIRIcyFlowInitiator::NewL( RSocket& aSocket, const TDesC& aUri,
			 CIRStationConnection& aOwner, TChannelInfo& aChannelInfo )
    {
	CIRIcyFlowInitiator* self = new ( ELeave ) CIRIcyFlowInitiator( aSocket,
							 aUri, aOwner, aChannelInfo );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::CIRIcyFlowInitiator
// ---------------------------------------------------------------------------
//
CIRIcyFlowInitiator::CIRIcyFlowInitiator( RSocket& aSocket, const TDesC& aUri,
			 CIRStationConnection& aOwner, TChannelInfo& aChannelInfo ) :
    		CActive( CActive::EPriorityStandard ), iState( EIRIdle ),
    		iSocket( aSocket ), iUri( aUri ), iOwner( aOwner ), iChannelInfo( aChannelInfo )
    {
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::ConstructL
// ---------------------------------------------------------------------------
//
void CIRIcyFlowInitiator::ConstructL()
	{
    IRLOG_DEBUG( "CIRIcyFlowInitiator::ConstructL." );
	CActiveScheduler::Add( this );
	iSocketTimer = CIRSocketTimeOutTimer::NewL(CActive::EPriorityHigh,*this);

	iNetworkControllerHandle = CIRNetworkController::OpenL();
	iUAProfString.CreateL( *iNetworkControllerHandle->GetUAProfString() );

	iBuffer.CreateL( KIRHeaderMaxSize );
    iReadBuffer.CreateL( 1 );
    if ( !ExtractUriComponentsL() )
        {
        User::Leave( KErrCorrupt );
        }
    IRLOG_DEBUG( "CIRIcyFlowInitiator::ConstructL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::~CIRIcyFlowInitiator
// ---------------------------------------------------------------------------
//
CIRIcyFlowInitiator::~CIRIcyFlowInitiator()
	{
	Cancel();
	iHost.Close();
	iPath.Close();

	iBuffer.Close();
	iReadBuffer.Close();
	iUAProfString.Close();

	delete iSocketTimer;
	if(iNetworkControllerHandle)
		{
		iNetworkControllerHandle->Close();
		}
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::ExtractUriComponentsL
// ---------------------------------------------------------------------------
//
TBool CIRIcyFlowInitiator::ExtractUriComponentsL()
	{
	IRLOG_DEBUG( "CIRIcyFlowInitiator::ExtractUriComponentsL" );

	TBool retMe = EFalse;

	if( !UriUtils::HasInvalidChars( iUri ) )
		{
    	TUriParser uriParser;
    	uriParser.Parse( iUri );

        iHost.Close();
    	iHost.CreateL( uriParser.Extract( EUriHost ).Size() );
    	iHost.Copy( uriParser.Extract( EUriHost ) );
    	iPath.Close();
    	iPath.CreateL( uriParser.Extract( EUriPath ).Size() );
    	iPath.Copy( uriParser.Extract( EUriPath ) );

    	if ( iPath.Length() == 0 )
    		{
    		iPath.Close();
    		iPath.CreateL( KIRUriComponentSeparator );
    		}
        retMe = ETrue;
		}
	IRLOG_DEBUG( "CIRIcyFlowInitiator::ExtractUriComponentsL - Exiting." );
	return retMe;
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::RequestFlow
// ---------------------------------------------------------------------------
//
void CIRIcyFlowInitiator::RequestFlow()
	{
	IRLOG_DEBUG( "CIRIcyFlowInitiator::RequestFlow" );
	Cancel();

	iBuffer.ReAlloc(iPath.Length() + iHost.Length() + iUAProfString.Length() 
												+ KIcyRequest().Length());
	iBuffer.Format( KIcyRequest, &iPath, &iHost, &iUAProfString );

	// Cancel any pending timer requests
	iSocketTimer->Cancel();
	iSocketTimer->After( KIRFITimeOutValue );

	iState = EIRSending;

	IRDEBUGCODE(
	    RBuf requestCopy;
	    if ( requestCopy.Create( iBuffer.Length() ) == KErrNone  )
	        {
	        requestCopy.Copy( iBuffer );
	        IRLOG_DEBUG2( "CIRIcyFlowInitiator::RequestFlow - request= %S", &requestCopy );
	        requestCopy.Close();
	        }
    )

	// HTTP GET METHOD sent to server
	iSocket.Send( iBuffer, 0, iStatus );
	SetActive();
	IRLOG_DEBUG( "CIRIcyFlowInitiator::RequestFlow - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::RunL
// ---------------------------------------------------------------------------
//
void CIRIcyFlowInitiator::RunL()
	{
	IRLOG_INFO3( "CIRIcyFlowInitiator::RunL - iStatus=%d, iState=%d", iStatus.Int(), iState );

	if( iStatus == KErrNone )
		{
		switch( iState )
			{
			case EIRSending:
				iSocketTimer->Cancel();
				iSocketTimer->After( KIRFITimeOutValue );

				// Byte Counter Impl
                iNetworkControllerHandle->DataTransferTracker().RawDataTransferredL(
                		iBuffer.Size(),0, MIRDataTransferTracker::EIRTransferCategoryAudio);

                iBuffer.Zero();
                iBuffer.ReAlloc( KIRHeaderMaxSize );
                iState = EIRReceiving;
                iSocket.Read( iReadBuffer, iStatus );
                SetActive();
				break;
			case EIRReceiving:
				iSocketTimer->Cancel();
				if ( iBuffer.MaxLength() <= ( iBuffer.Length() + iReadBuffer.Length() ) )
					{
					iBuffer.ReAlloc( iBuffer.MaxLength() + KSixtyFour );
					}
				// Byte Counter Impl
                iNetworkControllerHandle->DataTransferTracker().RawDataTransferredL( 0,
                		 iBuffer.Size(), MIRDataTransferTracker::EIRTransferCategoryAudio);
				iBuffer.Append(iReadBuffer);
				iReadBuffer.Zero();
				
				// Check if we got the full header and if not, read more from the socket.
				if ( iBuffer.Find( KHeaderEnd ) == KErrNotFound )
					{
					// if we have received 2kb's of headers, Then there is propably
					// some sort error and its time to abort
					if ( iBuffer.Length() >= KTwoZeroFourEight )
						{
						IRLOG_ERROR( "CIRIcyFlowInitiator::RunL - EIRReceiving. Got 2kb's of headers." ); 
						// Cancel the timer if active
						iSocketTimer->Cancel();
						iOwner.ConnectionError( KIRStreamSourceReadError );
						iState = EIRIdle;
						break;
						}
					
					iSocket.Read( iReadBuffer, iStatus );
					SetActive();
					iSocketTimer->After( KIRFITimeOutValue );
					break;
					}
				ParseChannelInfoL();
				if ( ValidateChannelServer() )
					{
					TInt bitRateInt(0);
					TLex8 bitvariable( iChannelInfo.iBitRate );
					bitvariable.Val( bitRateInt );
					RProperty::Set( KUidActiveInternetRadioApp,
						KIRPSBitrate, bitRateInt );
					iOwner.FlowReady();
				    iState = EIRFinished;
					}
				else
					{
				    IRLOG_ERROR( "CIRIcyFlowInitiator::RunL - Invalid server" );
					iOwner.ConnectionError( KIRStreamSourceInvalidUrl );
				    iState = EIRIdle;
					}

				break;
			default:
				__ASSERT_DEBUG( EFalse, User::Invariant() );
                break;
			}
		}
	else // An error has occurred
		{
		switch( iState )
			{
			case EIRSending:
				IRLOG_ERROR( "CIRIcyFlowInitiator::RunL - EIRSending" );
				// Cancel the timer if active
				iSocketTimer->Cancel();
				// Error in sending data to channel server
				iOwner.ConnectionError( KIRStreamSourceWriteError );
				break;
			case EIRReceiving:
				IRLOG_ERROR( "CIRIcyFlowInitiator::RunL - EIRReceiving" );
				// Cancel the timer if active
				iSocketTimer->Cancel();
				//Error in response from channel server
				if( iStatus.Int() == KErrEof )
					{
					iOwner.ConnectionError( KIRStreamSourceNoResponse );
					}
				else
					{
					iOwner.ConnectionError( KIRStreamSourceReadError );
					}
				break;
			default:
				IRLOG_FATAL2( "CIRIcyFlowInitiator::RunL - Error in unexpected state (%d)", iStatus.Int() );
				__ASSERT_DEBUG( EFalse, User::Invariant() );
				break;
			}
		}
	IRLOG_DEBUG( "CIRIcyFlowInitiator::RunL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::RunError
// ---------------------------------------------------------------------------
//
TInt CIRIcyFlowInitiator::RunError( TInt aError )
    {
    IRLOG_ERROR2( "CIRIcyFlowInitiator::RunError - aError=%d", aError );
    iOwner.ConnectionError( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::DoCancel
// ---------------------------------------------------------------------------
//
void CIRIcyFlowInitiator::DoCancel()
    {
	IRLOG_DEBUG2( "CIRIcyFlowInitiator::DoCancel - iState = %d.", iState );
    iSocket.CancelAll();
    iSocketTimer->Cancel();
    iState = EIRIdle;
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::TimerExpired
// ---------------------------------------------------------------------------
//
void CIRIcyFlowInitiator::TimerExpired()
    {
	IRLOG_ERROR( "CIRIcyFlowInitiator::TimerExpired." );
    Cancel();
    iOwner.ConnectionError( KIRStreamSourceTimeOut );
    IRLOG_DEBUG( "CIRIcyFlowInitiator::TimerExpired - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::ValidateChannelServer
// ---------------------------------------------------------------------------
//
TBool CIRIcyFlowInitiator::ValidateChannelServer()
	{
	IRLOG_DEBUG( "CIRIcyFlowInitiator::ValidateChannelServer" );
	TBool retMe = EFalse;

	if ( iChannelInfo.iContentType.Match(KValidContentType) != KErrNotFound )
		{
		retMe = ETrue;
		}
	IRLOG_DEBUG2( "CIRIcyFlowInitiator::ValidateChannelServer - Exiting (%d).", retMe );
	return retMe;
	}


// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::ParseChannelInfoL
// ---------------------------------------------------------------------------
//
void CIRIcyFlowInitiator::ParseChannelInfoL()
    {
    IRLOG_DEBUG( "CIRIcyFlowInitiator::ParseChannelInfoL" );

    TInt offsetPositionStart( 0 );
    offsetPositionStart = iBuffer.Find( KHeaderEnd );
    if ( offsetPositionStart == KErrNotFound )
        {
        IRLOG_ERROR( "CIRIcyFlowInitiator::ParseChannelInfoL - Header delimiter not found." );
        User::Leave( KIRStreamSourceApplicationProtocolError );
        }
    offsetPositionStart += KFour;
    TPtrC8 start = iBuffer.Mid( offsetPositionStart );
    iChannelInfo.iAudioDataOffset = start.Length();
    // Call ExtractMetaInfo function for each Meta Field
    ExtractMetaInfoL( KIcyName, iChannelInfo.iStationName );
    ExtractMetaInfoL( KIcyGenre, iChannelInfo.iGenre );
    ExtractMetaInfoL( KIcyBitrate, iChannelInfo.iBitRate );
    // Extract the Content-Type header
	ExtractMetaInfoL( KContentType, iChannelInfo.iContentType );
	// Extract the content-type header ( invalid header name handling )
	ExtractMetaInfoL( KContentTypeInvalid, iChannelInfo.iContentType );
    RBuf8 metaIntervalBuf;
    metaIntervalBuf.CreateL( KMAXMETABUFLENGTH );
    metaIntervalBuf.CleanupClosePushL();

	ExtractMetaInfoL( KIcyMetaint, metaIntervalBuf );

	TLex8 convert( metaIntervalBuf );
    convert.Val( iChannelInfo.iMetaInterval );

    if ( iChannelInfo.iMetaInterval <= 0 ) // Invalid meta interval specified, the stream is corrupt and cannot be played.
        {
        IRLOG_ERROR( "CIRIcyFlowInitiator::ParseChannelInfoL - Invalid metainterval." );
        User::Leave( KIRStreamSourceApplicationProtocolError );
        }
    CleanupStack::PopAndDestroy();

    IRLOG_DEBUG( "CIRIcyFlowInitiator::ParseChannelInfoL - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRIcyFlowInitiator::ExtractMetaInfoL
// ---------------------------------------------------------------------------
//
TBool CIRIcyFlowInitiator::ExtractMetaInfoL(const TDesC8& aMetaField, TDes8 &aBuffer ) const
    {
    IRLOG_DEBUG2( "CIRIcyFlowInitiator::ExtractMetaInfoL - aMetaField = %S", &aMetaField );
    // This function contains the logic for parsing the
    // buffer obtained from RecieveMetaInfo() and
    // appends the Meta info into the ChannelInfo structure
    TInt offsetPositionStart( 0 );
    TInt offsetPositionEnd( 0 );
    TPtr8 startPos( NULL, 0 );

    offsetPositionStart = iBuffer.Find( aMetaField );

    if ( offsetPositionStart < 0 )
	    {
	    IRLOG_DEBUG( "CIRIcyFlowInitiator::ExtractMetaInfoL - Field not found, Exiting." );
        return EFalse;
        }
    // Increment the offset by the length of meta field
    offsetPositionStart += aMetaField.Length();

    TPtrC8 start = iBuffer.Mid( offsetPositionStart );

    offsetPositionEnd = start.Find( KCarReturn );

    if ( offsetPositionEnd == KErrNotFound )
        {
        IRLOG_ERROR( "CIRIcyFlowInitiator::ExtractMetaInfoL - Field delimiter not found." );
        User::Leave( KIRStreamSourceApplicationProtocolError );
        }
    // Extract the actual data
    TPtrC8 data = start.Left( offsetPositionEnd );

    // too large field for this implementation.
    if (data.Length() > aBuffer.MaxLength() )
        {
        IRLOG_ERROR3( "CIRIcyFlowInitiator::ExtractMetaInfoL - Received field was too large (%d, allowed maximum = %d)",
                      data.Length(), aBuffer.Length() );
        User::Leave( KIRStreamSourceApplicationProtocolError );
        }

    aBuffer.Copy( data );
    aBuffer.TrimAll();
	IRLOG_DEBUG( "CIRIcyFlowInitiator::ExtractMetaInfoL - Exiting." );
    return ETrue;
    }

