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
* Description:  ?Description
*
*/


#include "irmetadata.h"
#include "irpubsubkeys.h"

#include "iractiveidleeng.h"
#include "iractiveidleengnotifyhandler.h"
#include "iractiveidleengpubsub.h"
#include "irdebug.h"


// ---------------------------------------------------------------------------
// CIRActiveIdleEng::CIRActiveIdleEng
// Default Constructor.
// ---------------------------------------------------------------------------
//
CIRActiveIdleEng::CIRActiveIdleEng( MIRActiveIdleEngNotifyHandler& aObserver )
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::ConstructL
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEng::ConstructL()
    {
    IRLOG_DEBUG( "CIRActiveIdleEng::ConstructL" );	
    //Subscriber for application running state 
    User::LeaveIfError(iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSDataApplicationRunning , 
    	RProperty::EInt) ) );
    //Subsciber for meta data
    User::LeaveIfError( iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSMetaData, 
    	RProperty::ELargeText)));
    //Subsciber for the volume level 
    User::LeaveIfError( iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSVolume , 
    	RProperty::EInt) ) );
    //Subsciber for the key KIRPSPlayStop at stop state
    User::LeaveIfError( iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSPlayStop ,
    	RProperty::EInt) ) );
    //Subsciber for the channel name
     User::LeaveIfError( iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSDataChannel, 
    	RProperty::ELargeText) ) );
    //Subscriber for favourite channel preset index 
    User::LeaveIfError( iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSPresetIndex ,
    	RProperty::EInt) ) );
    
    //Subscriber for favourite channel preset index 
    User::LeaveIfError( iPropertyObserverArray.Append( CIRActiveIdleEngPubSub::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSBufferingState ,
    	RProperty::EInt) ) );
    IRLOG_DEBUG( "CIRActiveIdleEng::ConstructL - Exiting." );	
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::SecondConstructL()
// Finalizes the construction of the object
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRActiveIdleEng::SecondConstructL()
	{
	
	IRLOG_DEBUG( "CIRActiveIdleEng::SecondConstructL" );
	for ( TInt i = 0; i < iPropertyObserverArray.Count(); i++ )
		{
		iPropertyObserverArray[i]->SecondConstructL();	
		}
	IRLOG_DEBUG( "CIRActiveIdleEng::SecondConstructL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::NewL
// Static constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRActiveIdleEng* CIRActiveIdleEng::NewL( 
	MIRActiveIdleEngNotifyHandler& aObserver )
    {
    IRLOG_DEBUG( "CIRActiveIdleEng::NewL" );
    CIRActiveIdleEng* self = new ( ELeave ) CIRActiveIdleEng( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRActiveIdleEng::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::~CIRActiveIdleEng()
// standard C++ destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRActiveIdleEng::~CIRActiveIdleEng()
    {
    IRLOG_DEBUG( "CIRActiveIdleEng::~CIRActiveIdleEng" );
    iPropertyObserverArray.ResetAndDestroy();
    iPropertyObserverArray.Close();
    IRLOG_DEBUG( "CIRActiveIdleEng::~CIRActiveIdleEng - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::HandlePropertyChangeL
// Invoked when any listened P&S key is changed.
// ---------------------------------------------------------------------------
void CIRActiveIdleEng::HandlePropertyChangeL( const TUid& aCategory,
											TUint aKey,const TDesC8& aText ) 
	{
	IRLOG_DEBUG( "CIRActiveIdleEng::HandlePropertyChangeL" );
	if ( aCategory == KUidActiveInternetRadioApp )
		{
		if( aKey == KIRPSDataChannel )
			{
			iObserver.HandleChannelChangeL();
			}
		else if( aKey == KIRPSMetaData )
			{
			CIRMetaData* metaData = CIRMetaData::NewL( aText );
			CleanupStack::PushL( metaData );
			iObserver.HandleMetaDataReceivedL( *metaData );
			CleanupStack::PopAndDestroy( metaData );
			}
		else
			{
			// To avoid PC lint error
			}
		}
	IRLOG_DEBUG( "CIRActiveIdleEng::HandlePropertyChangeL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::HandlePropertyChangeL
// Invoked when any listened P&S key is changed.
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEng::HandlePropertyChangeL( 
	const TUid& aCategory, TUint aKey, TInt aValue )
	{
	IRLOG_DEBUG( "CIRActiveIdleEng::HandlePropertyChangeL" );
	if ( aCategory == KUidActiveInternetRadioApp )
		{
		if( aKey == KIRPSDataApplicationRunning)
			{
			iObserver.HandleApplicationRunningStateChangeL( 
				static_cast<TIRPSApplicationRunningState>( aValue ) );				
			}
		else if( aKey == KIRPSVolume)
			{
			iObserver.HandleVolumeChangeL( aValue );
			}
		else if( aKey == KIRPSPlayStop)
			{
			iObserver.HandlePlayStopL( static_cast<TIRStopPlayState>( aValue ) );
			}	   
		else if( aKey == KIRPSBufferingState)
			{
			IRRDEBUG2("CIRActiveIdleEng::HandlePropertyChangeL KIRPSBufferingState=%d ",aValue);
			iObserver.HandleBufferingStateL(static_cast<TIRBufferingState>(aValue));
			}	   
		else
			{
			// To avoid PC lint error
			}
		}
	IRLOG_DEBUG( "CIRActiveIdleEng::HandlePropertyChangeL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRActiveIdleEng::HandlePropertyChangeErrorL
// Invoked when an error has occured while fetching the new value of any listened P&S key.
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEng::HandlePropertyChangeErrorL( const TUid& /*aCategory*/,
	TUint /*aKey*/, TInt aError )
    {
    IRLOG_DEBUG( "CIRActiveIdleEng::HandlePropertyChangeErrorL" );
    if (aError < 0)
    	{
    	HandlePropertyChangeL(KUidActiveInternetRadioApp, 
    		KIRPSDataApplicationRunning, EIRPSApplicationClosing);
    	}
    IRLOG_DEBUG( "CIRActiveIdleEng::HandlePropertyChangeErrorL" );
    }




