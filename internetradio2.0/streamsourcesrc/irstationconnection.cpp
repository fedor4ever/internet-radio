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
* Description:  Station connection implementation
*
*/


#include "irstationconnection.h"
#include "irdebug.h"
#include "iricyflowinitiator.h"
#include "iricyflowreader.h"
#include "irsocketopener.h"
#include "irstationdataobserver.h"

// --------------------------------------------------------------------------- 
//  CIRStationConnection::NewL
// ---------------------------------------------------------------------------
//
CIRStationConnection* CIRStationConnection::NewL( 
												MIRStationConnectionObserver& aConnectionObserver, 
                                                  MIRStationDataObserver& aDataObserver )
    {
	CIRStationConnection* self = new ( ELeave ) CIRStationConnection( aConnectionObserver, 
																	aDataObserver );
	CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);
	return self;
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::~CIRStationConnection
// ---------------------------------------------------------------------------
//
CIRStationConnection::~CIRStationConnection()
    {
    delete iSocketOpener;
    
    delete iIcyFlowInitiator;
    
    delete iIcyFlowReader;
    
    iSocket.Close();
    iUri.Close();
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::CIRStationConnection
// ---------------------------------------------------------------------------
//
CIRStationConnection::CIRStationConnection( MIRStationConnectionObserver& aConnectionObserver, 
											MIRStationDataObserver& aDataObserver ) :
    iConnectionObserver( aConnectionObserver ), iDataObserver( aDataObserver )
    {
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::ConstructL
// ---------------------------------------------------------------------------
//
void CIRStationConnection::ConstructL()
    {
	IRLOG_DEBUG( "CIRStationConnection::ConstructL" );    
    iConnectionType = EIRUndefined;
    }
    
// --------------------------------------------------------------------------- 
//  CIRStationConnection::ConnectL
// ---------------------------------------------------------------------------
//
void CIRStationConnection::ConnectL( const TDesC& aUri )
    {
	IRLOG_DEBUG( "CIRStationConnection::ConnectL" );        
    iUri.Close();
    iUri.CreateL( aUri );
    
    iConnectionType = EIRCandidate;

    delete iSocketOpener;	
    iSocketOpener = NULL;
    
    iSocketOpener = CIRSocketOpener::NewL( iSocket, *this );
    iSocketOpener->ConnectL( iUri );
	IRLOG_DEBUG( "CIRStationConnection::ConnectL - Exiting" );            
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::Close
// ---------------------------------------------------------------------------
//
void CIRStationConnection::Close()
    {
	IRLOG_DEBUG( "CIRStationConnection::Close" );    
    if ( iSocketOpener )
        {
        iSocketOpener->Cancel();
        }

    if ( iIcyFlowInitiator )
        {
        iIcyFlowInitiator->Cancel();
        }
    
	iChannelInfo.iStationName.Zero();
	iChannelInfo.iGenre.Zero();
    iChannelInfo.iBitRate.Zero();
    iChannelInfo.iContentType.Zero();
    iChannelInfo.iSongTitle.Zero();
    iChannelInfo.iArtistName.Zero();
    iChannelInfo.iMetaInterval = 0;
    iChannelInfo.iMetaDataTracker = 0;
    iChannelInfo.iReadSize = 0;
    iChannelInfo.iAudioDataOffset = 0;
    
    if ( iIcyFlowReader )
        {
        iIcyFlowReader->Cancel();
        }
        
    iSocket.Close(); 
	IRLOG_DEBUG( "CIRStationConnection::Close - Exiting." );           
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::SetConnectionType
// ---------------------------------------------------------------------------
//
void CIRStationConnection::SetConnectionType( TIRStationConnectionType aType )
    {
    iConnectionType = aType;
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::ConnectionType
// ---------------------------------------------------------------------------
//
CIRStationConnection::TIRStationConnectionType CIRStationConnection::ConnectionType() const
    {
    return iConnectionType;
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::Uri
// ---------------------------------------------------------------------------
//
const TDesC& CIRStationConnection::Uri() const
    {
    return iUri;
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::ContentType
// ---------------------------------------------------------------------------
//
const TDesC8& CIRStationConnection::ContentType() const
	{
	return iChannelInfo.iContentType;
	}

// --------------------------------------------------------------------------- 
//  CIRStationConnection::FillBuffer
// ---------------------------------------------------------------------------
//
void CIRStationConnection::FillBuffer( TDes8& aInputBuffer )
    {
	__ASSERT_DEBUG( iIcyFlowReader, User::Invariant() );				    

/*lint -save -e774 (Info -- Boolean within 'if' always evaluates to True )*/
    if ( iIcyFlowReader )    
        {
        iIcyFlowReader->FillBuffer( aInputBuffer );
        }
/*lint -restore*/        
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::ConnectionReady
// ---------------------------------------------------------------------------
//
void CIRStationConnection::ConnectionReady()
    {
	IRLOG_INFO2( "CIRStationConnection::ConnectionReady (%d).", this );
    delete iIcyFlowInitiator;    
    iIcyFlowInitiator = NULL;
    TRAPD( err, iIcyFlowInitiator = CIRIcyFlowInitiator::NewL( iSocket, iUri, *this, 
    															iChannelInfo ) )
    if ( err == KErrNone )
        {
        iIcyFlowInitiator->RequestFlow();           
        }
    else
        {
        ConnectionError( err );
        }
	IRLOG_DEBUG( "CIRStationConnection::ConnectionReady - Exiting." );
    }
    
// --------------------------------------------------------------------------- 
//  CIRStationConnection::FlowReady
// ---------------------------------------------------------------------------
//
void CIRStationConnection::FlowReady()    
    {
	IRLOG_INFO2( "CIRStationConnection::FlowReady (conn=%d).", this );    
    delete iIcyFlowReader;
    iIcyFlowReader = NULL;
    TRAPD( err, iIcyFlowReader = CIRIcyFlowReader::NewL( iSocket, *this, iDataObserver,
    																 iChannelInfo ) )
    if ( err == KErrNone )
        {
        iIcyFlowReader->Start();        
        iConnectionObserver.ConnectionSuccessful( this );        
        }
    else
        {
        ConnectionError( err );
        }
	IRLOG_DEBUG( "CIRStationConnection::FlowReady - Exiting." );        
    }

// --------------------------------------------------------------------------- 
//  CIRStationConnection::ConnectionError
// ---------------------------------------------------------------------------
//
void CIRStationConnection::ConnectionError( TInt aErrorCode )
    {
	IRLOG_ERROR3( "CIRStationConnection::ConnectionError - err=%d, conn=%d.", aErrorCode, this );            
    Close();
    iConnectionObserver.ConnectionError( this, aErrorCode );
	IRLOG_DEBUG( "CIRStationConnection::ConnectionError - Exiting." ); 
    }
