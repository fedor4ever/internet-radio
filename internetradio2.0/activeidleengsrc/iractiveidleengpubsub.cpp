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


#include "iractiveidleengpubsub.h"
#include "iractiveidleengpubsubnotifyhandler.h"
#include "irdebug.h"

// ==================== MEMBER FUNCTIONS ===================

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::CIRActiveIdleEngPubSub
// Constructor.
// ---------------------------------------------------------------------------
//
CIRActiveIdleEngPubSub::CIRActiveIdleEngPubSub(
	MIRActiveIdleEngPubSubNotifyHandler& aObserver, const TUid& aCategory, 
	TUint aKey , const RProperty::TType aPropertyType): CActive( CActive::
	EPriorityStandard ),  iPropertyType(aPropertyType),iObserver( aObserver ), 
	iCategory( aCategory ), iKey( aKey )
    {
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::ConstructL
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEngPubSub::ConstructL()
    {
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::ConstructL" );	
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::ConstructL - Exiting." );	
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::SecondConstructL()
// Finalizes the construction of this object.
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEngPubSub::SecondConstructL()
	{
	IRLOG_DEBUG( "CIRActiveIdleEngPubSub::SecondConstructL" );	
	if(!IsActive())	
		{
	    iProperty.Subscribe( iStatus );
	    SetActive();	
		}

    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::SecondConstructL - Exiting." );	
	}

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::NewL
// Static constructor.
// ---------------------------------------------------------------------------
//
CIRActiveIdleEngPubSub* CIRActiveIdleEngPubSub::NewL( 
	MIRActiveIdleEngPubSubNotifyHandler& aObserver, const TUid& aCategory, 
	TUint aKey, const RProperty::TType aPropertyType  )
    {
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::NewL" );	
    CIRActiveIdleEngPubSub* self = new ( ELeave ) CIRActiveIdleEngPubSub( 
    							aObserver, aCategory, aKey, aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::NewL - Exiting." );	
    return self;
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::~CIRActiveIdleEngPubSub()
// Destructor
// ---------------------------------------------------------------------------
//
CIRActiveIdleEngPubSub::~CIRActiveIdleEngPubSub()
    {
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::~CIRActiveIdleEngPubSub" );	
    Cancel();
    iProperty.Close();
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::~CIRActiveIdleEngPubSub - Exiting." );	
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::DoCancel()
// Invoked when the active object is cancelled.
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEngPubSub::DoCancel()
    {
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::DoCancel" );	
    iProperty.Cancel();
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::DoCancel - Exiting." );	
    }

// ---------------------------------------------------------------------------
// CIRActiveIdleEngPubSub::RunL()
// The function is called by the active scheduler when a request completion event occurs,
// ---------------------------------------------------------------------------
//
void CIRActiveIdleEngPubSub::RunL()
    {
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::RunL" );	

    iProperty.Subscribe( iStatus );
    SetActive();
    TInt err(KErrNone);
    switch (iPropertyType)
        {
        // Subscribes the property of interger type 
        case RProperty::EInt:
	        {
	        err = iProperty.Get( iValue );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, iValue );
             	}
	        break;
	        }
	    // Subscribes the property of text type 
	    case RProperty::ELargeText:
	        {
	        err = iProperty.Get( iText );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, iText );
             	}
	        break;
	        }
	    default:
	        {
	        break;
	        }
        }
    
    if (err)
    	{
        iObserver.HandlePropertyChangeErrorL(iCategory, iKey, err);
    	}
	
    IRLOG_DEBUG( "CIRActiveIdleEngPubSub::RunL - Exiting." );	
    }
