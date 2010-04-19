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
* Description:  Socket opener implementation
*
*/


#include <e32property.h>
#include <HttpDownloadMgrCommon.h>
#include <in_sock.h>
#include <uriutils.h>

#include "irpubsubkeys.h"
#include "irdebug.h"
#include "irnetworkcontroller.h"
#include "irsocketopener.h"
#include "irstationconnection.h"
#include "irstreamsourceerrors.h"
#include "irstreamsourceliterals.h"

const TInt KIRSOTimeOutValue = 10000000;

// ---------------------------------------------------------------------------
// CIRSocketOpener::NewL
// ---------------------------------------------------------------------------
//
CIRSocketOpener* CIRSocketOpener::NewL( RSocket& aSocket, CIRStationConnection& aOwner )
	{
	CIRSocketOpener* self = new ( ELeave ) CIRSocketOpener( aSocket, aOwner );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// CIRSocketOpener::CIRSocketOpener
// ---------------------------------------------------------------------------
//
CIRSocketOpener::CIRSocketOpener( RSocket& aSocket, CIRStationConnection& aOwner )
	:CActive( EPriorityStandard ), iPort( KDefaultPort ), iSocket( aSocket), iOwner( aOwner )
	{
	}

// ---------------------------------------------------------------------------
// CIRSocketOpener::ConstructL
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::ConstructL()
	{
	IRLOG_DEBUG( "CIRSocketOpener::ConstructL" );	
	CActiveScheduler::Add( this );
	iState = EIRIdle;
	iSocketTimer = CIRSocketTimeOutTimer::NewL( CActive::EPriorityHigh,*this );

	CIRNetworkController* networkControllerHandle = CIRNetworkController::OpenL();
	CleanupClosePushL( *networkControllerHandle );

    TProtocolName protocolName( KTcpProtocol );
	TProtocolDesc protocolInfo;
	
	networkControllerHandle->GetIRSocketServer().FindProtocol( protocolName, protocolInfo );
	
	iResolver.Close();

	User::LeaveIfError( iResolver.Open( networkControllerHandle->GetIRSocketServer(),
	                                   protocolInfo.iAddrFamily, protocolInfo.iProtocol,
	                                   networkControllerHandle->GetIRConnection() ) );

	IRLOG_DEBUG( "CIRSocketOpener::ConstructL - Opened resolver." );
	User::LeaveIfError( iSocket.Open( networkControllerHandle->GetIRSocketServer(),
				 protocolInfo.iAddrFamily,protocolInfo.iSockType,protocolInfo.iProtocol,
	                                  networkControllerHandle->GetIRConnection() ) );
    IRLOG_DEBUG( "CIRSocketOpener::ConstructL - Opened socket." );
	                                   
    CleanupStack::PopAndDestroy();
	IRLOG_DEBUG( "CIRSocketOpener::ConstructL - Exiting." );	    
	}

// ---------------------------------------------------------------------------
// CIRSocketOpener::CIRSocketOpener
// ---------------------------------------------------------------------------
//
CIRSocketOpener::~CIRSocketOpener()
	{
	Cancel();
//	iResolver.Close()	
    iHost.Close();	
	delete iSocketTimer;	
	}

// ---------------------------------------------------------------------------
// CIRSocketOpener::ResolveAddress()
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::ResolveAddress()
	{
	IRLOG_INFO2( "CIRSocketOpener::ResolveAddress - %S", &iHost );
    Cancel();
	iState = EIRResolving;

	iSocketTimer->Cancel();
	iSocketTimer->After( KIRSOTimeOutValue );
	
	iResolver.GetByName( iHost, iResolvedHostInfo, iStatus );
	SetActive();

	IRLOG_DEBUG( "CIRSocketOpener::ResolveAddress - Exiting." );                
	}

// ---------------------------------------------------------------------------
// CIRSocketOpener::ConnectToAddress()
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::ConnectToAddress()
	{
	IRLOG_INFO( "CIRSocketOpener::ConnectToAddress" );
    Cancel();
	iState = EIRConnecting;

	iSocketTimer->Cancel();
	iSocketTimer->After( KIRSOTimeOutValue );
	
	iSocket.Connect( iSockAddr, iStatus );
	SetActive();

	IRLOG_DEBUG( "CIRSocketOpener::ConnectToAddress - Exiting." );    
    }   

// ---------------------------------------------------------------------------
// CIRSocketOpener::ExtractUriComponents
// ---------------------------------------------------------------------------
//
TBool CIRSocketOpener::ExtractUriComponentsL(const TDesC& aUrl)
	{
	IRLOG_DEBUG( "CIRSocketOpener::ExtractUriComponentsL" );    

    TBool retMe = EFalse;

	if( !UriUtils::HasInvalidChars( aUrl ) )
		{
    	// Initialize the Uri Parser
    	CUri8* uri = UriUtils::CreateUriL( aUrl );
    	CleanupStack::PushL( uri );
    	
    	const TUriC8& accessorUri = uri->Uri(); 

        iHost.Close();
    
        const TDesC8& host = accessorUri.Extract( EUriHost ); 
        if ( host.Length() > 0 )
            {
            if ( iHost.Create( host.Length() ) == KErrNone )
                {
                iHost.Copy( host );
            
	            TLex8 uriLex( accessorUri.Extract( EUriPort ) );
	            if ( uriLex.Val( iPort ) != KErrNone )
	                {
    	            iPort = KDefaultPort;
	                }
                retMe = ETrue;	            
                }
            }
        CleanupStack::PopAndDestroy( uri );
		}
	IRLOG_DEBUG2( "CIRSocketOpener::ExtractUriComponentsL - Returning %d.", retMe );        
	return retMe;
	}


// ---------------------------------------------------------------------------
// CIRSocketOpener::RunL
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::RunL()
	{
	IRLOG_INFO3( "CIRSocketOpener::RunL - iStatus = %d, iState = %d", iStatus.Int(), iState );

	if( iStatus == KErrNone )
		{
		switch( iState )
			{
			case EIRResolving:
				iSocketTimer->Cancel();
				iSockAddr = ( iResolvedHostInfo() ).iAddr;
				iSockAddr.SetPort( iPort );
				ConnectToAddress();
				break;
			case EIRConnecting:
    			iSocketTimer->Cancel();
				iState = EIRReady;
				iOwner.ConnectionReady();
				break;
			default:
                __ASSERT_DEBUG( EFalse, User::Invariant() );
    			break;
			}
		}
	else 
		{
		switch( iState )
			{
			case EIRResolving:
				IRLOG_ERROR( "CIRConnectionHandler::RunL - EIRResolving" );
				iSocketTimer->Cancel();
				iOwner.ConnectionError( KIRStreamSourceLookupError );
				break;
			case EIRConnecting: 
				IRLOG_ERROR( "CIRConnectionHandler::RunL - EConnecting"); 
				iSocketTimer->Cancel();
				iOwner.ConnectionError( KIRStreamSourceConnectionError );
				break;
			default:
				IRLOG_FATAL2( "CIRConnectionHandler::RunL - Error in unexpected state (%d)", iStatus.Int() );
                __ASSERT_DEBUG( EFalse, User::Invariant() );				
				break;
			}
		}
	IRLOG_DEBUG( "CIRSocketOpener::RunL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRSocketOpener::Connect
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::ConnectL( const TDesC& aUrl )
	{
    IRLOG_INFO2( "CIRSocketOpener::ConnectL - %S", &aUrl );
	if( ExtractUriComponentsL( aUrl ) )
		{
		ResolveAddress();		  	
		}
	else
		{
		IRLOG_ERROR( "CIRConnectionHandler::ConnectL - Invalid URL" );
		iOwner.ConnectionError( KIRStreamSourceInvalidUrl );		  		
		}  
	IRLOG_DEBUG( "CIRSocketOpener::ConnectL - Exiting." );		
	}

// ---------------------------------------------------------------------------
// CIRSocketOpener::TimerExpired
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::TimerExpired()
	{
	switch( iState )
		{
		case EIRResolving: 
			Cancel();
			IRLOG_ERROR( "CIRSocketOpener::TimerExpired - Timed out while looking up server." );
			iOwner.ConnectionError( KIRStreamSourceTimeOut );
			break;
		
		case EIRConnecting: 
		    Cancel();
			IRLOG_ERROR( "CIRSocketOpener::TimerExpired - Connecting to server timed out." );   	 		    
			iOwner.ConnectionError( KIRStreamSourceTimeOut );
			break;
		default:
            __ASSERT_DEBUG( EFalse, User::Invariant() );				
			break;
		} 
	}


// ---------------------------------------------------------------------------
// CIRSocketOpener::DoCancel
// ---------------------------------------------------------------------------
//
void CIRSocketOpener::DoCancel()
	{
	IRLOG_DEBUG( "CIRSocketOpener::DoCancel." );	
	iResolver.Cancel();
	iSocket.CancelAll();
	
	iSocketTimer->Cancel();
	IRLOG_DEBUG( "CIRSocketOpener::DoCancel - Exiting." );		
	}



