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
* Description:  Stream source main class implementation
*
*/


#include <e32property.h>
#include "irpubsubkeys.h"

#include "irdebug.h"
#include "irnetworkcontroller.h"
#include "irstreamsource.h"
#include "irstreamsourceerrors.h"
#include "irstreamsourceobserver.h"
#include "irstationconnection.h"


// Constants
const TInt KIRSCConnectionCleanupDelay = 10000000;
const TInt KHundred = 100;

// ---------------------------------------------------------------------------
//  CIRStreamSource::ConnectToServerL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRStreamSource::ConnectToServerL( const TDesC& aUrl )
    {
    IRLOG_INFO2( "CIRStreamSource::ConnectToServerL - aUrl=%S", &aUrl );
	#ifndef __WINS__
	 					 	 
    //Connecting for the first time
 /*****************************testing*****************/
    	/*if ( iCurrentConnection )	
	    {
	    iCurrentConnection->Close();
	    }
	AsyncCleanupConnection( iCurrentConnection );
    iCurrentConnection = NULL;*/
/*****************************************************/
   	
   		#endif //__WINS__
    if ( iNetworkControllerHandle->GetNetworkStatus() )
        {
        iReConnecting = EFalse;
        DoConnectL( aUrl );    
        }
    else
	    {
		iStreamSourceObserver.ErrorConnecting( KIRStreamSourceConnectionError );
	    }
    IRLOG_DEBUG( "CIRStreamSource::ConnectToServerL - Exiting." );
	}

// ---------------------------------------------------------------------------
//  CIRStreamSource::ReconnectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRStreamSource::ReconnectL( const TDesC& aUrl )
    {
    IRLOG_INFO( "CIRStreamSource::ReconnectL" );
	if ( iNetworkControllerHandle->GetNetworkStatus() )
	    {
        iReConnecting = ETrue;
        DoConnectL( aUrl );
	    }
	else
		{
		iStreamSourceObserver.ErrorConnecting( KIRStreamSourceConnectionError );
		}
    IRLOG_DEBUG( "CIRStreamSource::ReconnectL - Exiting." );
	}
	
// ---------------------------------------------------------------------------
//  CIRStreamSource::DoConnectL
//  Creates the connection object which initiates the connection
//  to the channel server
// ---------------------------------------------------------------------------
//
void CIRStreamSource::DoConnectL( const TDesC& aUrl )
	{
    IRLOG_INFO2( "CIRStreamSource::DoConnectL iCurrentConn=%d", iCurrentConnection );	

    AsyncCleanupConnection( iNewConnection );
    iNewConnection = NULL;        
    StaticConnectionCleanupCallback( this ); // delete it (and them) now

    CIRStationConnection* connection = CIRStationConnection::NewL( *this, *this );
    CleanupStack::PushL( connection );
    iConnections.AppendL( connection );
    CleanupStack::Pop( connection );
    iNewConnection = connection;
    
    iNewConnection->ConnectL( aUrl );
    
	IRLOG_INFO2( "CIRStreamSource::DoConnectL - Exiting, iNewConnection=%d.", iNewConnection );
	}

// ---------------------------------------------------------------------------
//  CIRStreamSource::CIRStreamSource
//  Default Constructor
// ---------------------------------------------------------------------------
//
CIRStreamSource::CIRStreamSource( MIRStreamSourceObserver &aObserver ):
	iStreamSourceObserver( aObserver )
    {
	// no implementation
    }

// ---------------------------------------------------------------------------
// CIRStreamSource::~CIRStreamSource
// Default Destructor
// ---------------------------------------------------------------------------
//
CIRStreamSource::~CIRStreamSource()
    {    
    IRLOG_DEBUG( "CIRStreamSource::~CIRStreamSource" );
	
    // close the network controller handle	
	if( iNetworkControllerHandle )
		{
		iNetworkControllerHandle->Close();	
        }
	delete iSocketTimer;
    
    delete iConnectionCleanupTimer;
    
    iConnections.ResetAndDestroy();
    iConnections.Close();
    }

// --------------------------------------------------------------------------- 
//  CIRStreamSource::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CIRStreamSource *CIRStreamSource::NewL( MIRStreamSourceObserver   &aObserver )
    {
    CIRStreamSource *self = new( ELeave ) CIRStreamSource( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRStreamSource::ConstructL
// ---------------------------------------------------------------------------
//
void CIRStreamSource::ConstructL()
    {
    // Open handle to network controller
    iNetworkControllerHandle = CIRNetworkController::OpenL();
    // create a timer for timeouts
    iSocketTimer = CIRSocketTimeOutTimer::NewL( CActive::EPriorityHigh,	*this ); 
    }

// --------------------------------------------------------------------------- 
//  CIRStreamSource::FilltheBuffer
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRStreamSource::FilltheBuffer( TDes8& aInputBuffer )
    {
    IRLOG_DEBUG( "CIRStreamSource::FilltheBuffer" );

    if( iCurrentConnection )
    	{
   	 	iCurrentConnection->FillBuffer( aInputBuffer );
    	}
    else
    	{
    	iReConnecting = ETrue;
    	iStreamSourceObserver.ErrorConnecting( KIRStreamSourceNoResponse );
    	}
    IRLOG_DEBUG( "CIRStreamSource::FilltheBuffer - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRStreamSource::TimerExpired
// ---------------------------------------------------------------------------
//
void CIRStreamSource::TimerExpired()
	{
	// CancelRequest gets called by UI.We could of course call it here, but you never
	// know what it would cause in UI without thorough testing.
	iStreamSourceObserver.ErrorConnecting( KIRStreamSourceTimeOut );
	}

// ---------------------------------------------------------------------------
// CIRStreamSource::TimerExpired
// ---------------------------------------------------------------------------
//
void CIRStreamSource::MetadataReceived( const CIRMetaData& aMetaData )
    {
    TRAP_IGNORE( iStreamSourceObserver.HandleMetaDataReceivedL( aMetaData ) )
    }

// ---------------------------------------------------------------------------
// CIRStreamSource::AudioDataEvent
// ---------------------------------------------------------------------------
//
void CIRStreamSource::AudioDataEvent( const TInt aResponseCode,	TInt aValue )
	{
	IRLOG_DEBUG3( "CIRStreamSource::AudioDataEvent - aResponseCode=%d, aFeedValue=%d", aResponseCode, aValue );
	switch( aResponseCode )
		{
		case EBufferFilled:
			{
			IRLOG_DEBUG( "CIRStreamSource::AudioDataEvent - EBufferFilled" );
			// Indicate the observer
			if( !iReConnecting )
				{
				iStreamSourceObserver.PlayNow();
				}
			else
				{
				iStreamSourceObserver.Play();	
				}
			}
			break;
		case EOpenComplete:
			{
			IRLOG_DEBUG( "CIRStreamSource::AudioDataEvent - EOpenComplete" );
			// Indicate the observer that streamsource buffering complete	
			iStreamSourceObserver.OpenComplete();
			}
			break;
		case EBufferPercentage:
			{
			// Indicate buffering progress to observer
			iStreamSourceObserver.UpdateProgress( aValue );
			if ( aValue == KHundred )
				{
			    IRLOG_INFO( "CIRStreamSource::AudioDataEvent - 100 percent EBufferPercentage" );				
				iSocketTimer->Cancel();
				} 
			}
			break;
		default:
			{
			__ASSERT_DEBUG( EFalse, User::Invariant() );
			}
			break;	
		}
	IRLOG_DEBUG( "CIRStreamSource::AudioDataEvent - Exiting." );	
	}
	
// ---------------------------------------------------------------------------
//  CIRStreamSource::ContentTypeL
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CIRStreamSource::ContentTypeL()
	{
    __ASSERT_ALWAYS( iCurrentConnection, User::Leave( KErrNotReady ) );

    return iCurrentConnection->ContentType();
	}

// ---------------------------------------------------------------------------
// CIRStreamSource::CancelRequest
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRStreamSource::CancelRequest()
	{
	IRLOG_INFO3( "CIRStreamSource::CancelRequest - iNewConnection=%d, iCurrentConnection=%d", iNewConnection, iCurrentConnection );	
	iSocketTimer->Cancel();
		
    if ( iNewConnection )		
        {
        iNewConnection->Close();
        AsyncCleanupConnection( iNewConnection );
        iNewConnection = NULL;
        }
    else
        {
		if ( iCurrentConnection )
		    {
		    iCurrentConnection->Close();		    
		    }
		AsyncCleanupConnection( iCurrentConnection );
		iCurrentConnection = NULL;
        }        
	IRLOG_DEBUG( "CIRStreamSource::CancelRequest - Exiting." );	
	}

// ---------------------------------------------------------------------------
// CIRStreamSource::ConnectionSuccessfulL
// ---------------------------------------------------------------------------
//
void CIRStreamSource::ConnectionSuccessful( CIRStationConnection* aConnection )
	{
	IRLOG_INFO3( "CIRStreamSource::ConnectionSuccessful aConnection=%d, iCurrentConn=%d", 
	             aConnection, iCurrentConnection );	
	__ASSERT_DEBUG( aConnection->ConnectionType() == CIRStationConnection::EIRCandidate,
	                User::Invariant() );
	                
    // publish the url
    RProperty::Set( KUidActiveInternetRadioApp, KIRPSChannelUrl, aConnection->Uri() );	

	if( !iReConnecting )
		{
		iStreamSourceObserver.StopBuffering();
	    }	
	    
	if ( iCurrentConnection )	
	    {
	    iCurrentConnection->Close();
	    }
	AsyncCleanupConnection( iCurrentConnection );
    iCurrentConnection = NULL;

    aConnection->SetConnectionType( CIRStationConnection::EIRPermanent );
    iCurrentConnection = aConnection;
    iNewConnection = NULL;

    iStreamSourceObserver.ConnectionEstablished();
    // Buffering starts now, so let's set the timer.
	TTimeIntervalMicroSeconds32 timeOut( KBufferingTimeOut );
	iSocketTimer->Cancel();
	iSocketTimer->After( timeOut );

	IRLOG_DEBUG( "CIRStreamSource::ConnectionSuccessful - Exiting." );	
	}

// ---------------------------------------------------------------------------
// CIRStreamSource::ConnectionError
// ---------------------------------------------------------------------------
//
void CIRStreamSource::ConnectionError( CIRStationConnection* aConnection, TInt aErrorCode )
	{
	IRLOG_ERROR3( "CIRStreamSource::ConnectionError - aConnection=%d, aErrorCode=%d", aConnection, aErrorCode );
    
	if ( aConnection->ConnectionType() == CIRStationConnection::EIRCandidate )
	    {
        AsyncCleanupConnection( aConnection );
        iNewConnection = NULL; // To Do: implement more sophisticated pending connection handling.	    
		iReConnecting = ETrue;        
	    }
    else
        {
        if ( aErrorCode == KIRStreamSourceDisconnected )
            {
			CancelRequest();            
            }
        }	    
	iStreamSourceObserver.ErrorConnecting( aErrorCode );
	IRLOG_DEBUG( "CIRStreamSource::ConnectionError - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRStreamSource::AsyncCleanupConnection
// ---------------------------------------------------------------------------
//
void CIRStreamSource::AsyncCleanupConnection( CIRStationConnection* aConnection )
    {
    IRLOG_DEBUG2( "CIRStreamSource::AsyncCleanupConnection - aConnection=%d.", aConnection );    
    // let's not double-add any connections.
    if ( aConnection )
        {
        IRLOG_INFO2( "CIRStreamSource::AsyncCleanupConnection - Scheduled deletion of connection %d.", aConnection );            
        delete iConnectionCleanupTimer;
        iConnectionCleanupTimer = NULL;
        TRAPD( err, iConnectionCleanupTimer = CPeriodic::NewL( CActive::EPriorityStandard ) )
        if ( err == KErrNone )
            {
            iConnectionCleanupTimer->Start( KIRSCConnectionCleanupDelay, 0,
            			 TCallBack( StaticConnectionCleanupCallback, this ) );
            }
        else
            {
            IRLOG_ERROR2( "CIRStreamSource::AsyncCleanupConnection - CPeriodic creation left with %d.", err );
            // delete the existing dying connections.
            StaticConnectionCleanupCallback( this );    
            }
        aConnection->SetConnectionType( CIRStationConnection::EIRDying );                    
        }
    IRLOG_DEBUG( "CIRStreamSource::AsyncCleanupConnection - Exiting." );                    
    }

// ---------------------------------------------------------------------------
// CIRStreamSource::StaticConnectionCleanupCallback
// ---------------------------------------------------------------------------
//
TInt CIRStreamSource::StaticConnectionCleanupCallback( TAny* aSelfPtr )
	{
	CIRStreamSource* self = reinterpret_cast<CIRStreamSource*>( aSelfPtr );
	if ( self )
		{
	IRRDEBUG2("CIRStreamSource::StaticConnectionCleanupCallback - Entering", KNullDesC );

        IRLOG_DEBUG( "CIRStreamSource::StaticConnectionCleanupCallback." );
		delete self->iConnectionCleanupTimer;
		self->iConnectionCleanupTimer = NULL;
		TInt it = self->iConnections.Count() - 1;
		while ( it >= 0 )
		    {
		    if ( self->iConnections[it]->ConnectionType() == CIRStationConnection::EIRDying )
		        {
		 IRRDEBUG2("CIRStreamSource::StaticConnectionCleanupCallback - Entering1", KNullDesC );
        IRLOG_INFO2( "CIRStreamSource::StaticConnectionCleanupCallback - Deleting connection %d.", 
                self->iConnections[it] );		    		        
        delete self->iConnections[it];
        self->iConnections.Remove( it );
		 IRRDEBUG2("CIRStreamSource::StaticConnectionCleanupCallback - Exiting1", KNullDesC );

		        }
		    it--;
		    }
	IRRDEBUG2("CIRStreamSource::StaticConnectionCleanupCallback - Exiting", KNullDesC );

        IRLOG_DEBUG( "CIRStreamSource::StaticConnectionCleanupCallback - Exiting." );		    
		}
	return KErrNone;
	}

