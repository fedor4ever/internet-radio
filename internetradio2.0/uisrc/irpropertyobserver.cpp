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


#include "irdebug.h"
#include "irpropertyobserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIRPropertyObserver::CPropertyObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIRPropertyObserver::CIRPropertyObserver(
	MIRPropertyChangeObserver& aObserver, const TUid& aCategory, 
			const TUint aKey, const RProperty::TType aPropertyType)
    : CActive( CActive::EPriorityLow ),
    iObserver( aObserver ),
    iCategory( aCategory ),
    iKey( aKey ),
    iPropertyType( aPropertyType )
	{
	IRLOG_DEBUG( "CIRPropertyObserver::CIRPropertyObserver" );
	}

// -----------------------------------------------------------------------------
// CIRPropertyObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver::ConstructL()
	{
	IRLOG_DEBUG( "CIRPropertyObserver::ConstructL - Entering" );
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
	IRLOG_DEBUG( "CIRPropertyObserver::ConstructL - Exiting." );
	}

// -----------------------------------------------------------------------------
// CIRPropertyObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIRPropertyObserver* CIRPropertyObserver::NewL(
	MIRPropertyChangeObserver& aObserver, const TUid& aCategory,
														 const TUint aKey, const RProperty::TType aPropertyType)
	{
	IRLOG_DEBUG( "CIRPropertyObserver::NewL - Entering" );
    CIRPropertyObserver* self = new( ELeave )CIRPropertyObserver(
    	 aObserver, aCategory, aKey, aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRPropertyObserver::NewL - Exiting." );
    return self;
	}

// Destructor
CIRPropertyObserver::~CIRPropertyObserver()
	{
	IRLOG_DEBUG( "CIRPropertyObserver::~CIRPropertyObserver - Entering" );
	Cancel();
    iProperty.Close();
	IRLOG_DEBUG( "CIRPropertyObserver::~CIRPropertyObserver - Exiting." );
	}

// -----------------------------------------------------------------------------
// CIRPropertyObserver::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver::RunL()
	{
	IRLOG_DEBUG( "CIRPropertyObserver::RunL - Entering" );
    
    TInt err(KErrNone);
    
    switch (iPropertyType)
        {
        case RProperty::EInt:
	        {
	        err = iProperty.Get( iValueInt );
	        iObserver.HandlePropertyChangeL(iCategory, iKey, iValueInt);
	        break;
	        }
	    case RProperty::EText:
	        {
	        err = iProperty.Get( iValueText );
	        iObserver.HandlePropertyChangeL(iCategory, iKey, iValueText);
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
	IRLOG_DEBUG( "CIRPropertyObserver::RunL - Exiting." );
	}

// -----------------------------------------------------------------------------
// CIRPropertyObserver::DoCancel
// Cancels an outstanding active request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver::DoCancel()
	{
	IRLOG_DEBUG( "CIRPropertyObserver::DoCancel - Entering" );
    iProperty.Cancel();
	IRLOG_DEBUG( "CIRPropertyObserver::DoCancel - Exiting." );
	}

// -----------------------------------------------------------------------------
// CIRPropertyObserver::GetValue
// -----------------------------------------------------------------------------
//	
EXPORT_C void CIRPropertyObserver::GetValue(TInt& aValue) const
	{
	IRLOG_DEBUG( "CIRPropertyObserver::GetValue (TInt)  - Entering" );
	aValue = iValueInt;
	IRLOG_DEBUG( "CIRPropertyObserver::GetValue (TInt)  - Exiting" );
	}
	
// -----------------------------------------------------------------------------
// CIRPropertyObserver::GetValue
// -----------------------------------------------------------------------------
//	
EXPORT_C void CIRPropertyObserver::GetValue(TIRTextProperty& aValue) const
	{
	IRLOG_DEBUG( "CIRPropertyObserver::GetValue (TIRTextProperty)  - Entering" );
	aValue = iValueText;
	IRLOG_DEBUG( "CIRPropertyObserver::GetValue (TIRTextProperty)  - Exiting" );
	}
	
// -----------------------------------------------------------------------------
// CIRPropertyObserver::Subscribe
// Subscription of listened P&S keys
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver::Subscribe()
	{
	IRLOG_DEBUG( "CIRPropertyObserver::Subscribe - Entering" );
	if( !IsActive() )
		{
		iProperty.Subscribe( iStatus );
	    SetActive();
		}
	IRLOG_DEBUG( "CIRPropertyObserver::Subscribe - Exiting." );    	
	}

