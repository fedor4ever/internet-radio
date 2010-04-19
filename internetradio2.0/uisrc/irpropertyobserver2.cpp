/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Source file for CIRPropertyObserver2.
*
*/


#include "irpropertyobserver2.h"
#include "irdebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIRPropertyObserver2::CIRPropertyObserver2(MIRPropertyChangeObserver2& aObserver, 
					const TUid& aCategory, const TUint aKey, const TIRPropertyType aPropertyType)
    : CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iCategory( aCategory ),
    iKey( aKey ),
    iPropertyType( aPropertyType )
	{
	IRLOG_INFO( "CIRPropertyObserver2::CIRPropertyObserver2" );
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver2::ConstructL()
	{
	IRLOG_INFO( "CIRPropertyObserver2::ConstructL - Entering" );
    switch (iPropertyType)
        {
        case EIRPropertyInt:
	        {
	        break;
	        }
	    case EIRPropertyByteArray:
	        {
	        iValueByteArray = HBufC8::NewL( RProperty::KMaxPropertySize );
	        break;
	        }
	    case EIRPropertyText:
	        {
	        // Max size in bytes, length is size / 2
	        iValueText = HBufC::NewL( RProperty::KMaxPropertySize / 2 );
	        break;
	        }
	    default:
	        {
	        break;
	        }
        }

    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
    IRLOG_INFO( "CIRPropertyObserver2::ConstructL - Exiting" );
	}

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIRPropertyObserver2* CIRPropertyObserver2::NewL(MIRPropertyChangeObserver2& aObserver,
		                        const TUid& aCategory, const TUint aKey,
		                        	 const TIRPropertyType aPropertyType)
	{
	IRLOG_INFO( "CIRPropertyObserver2::NewL - Entering" );
    CIRPropertyObserver2* self = new( ELeave )CIRPropertyObserver2( aObserver, aCategory,
    				 aKey, aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_INFO( "CIRPropertyObserver2::NewL - Exiting" );
    return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIRPropertyObserver2::~CIRPropertyObserver2()
	{
	IRLOG_INFO( "CIRPropertyObserver2::~CIRPropertyObserver2 - Entering" );
	Cancel();
    iProperty.Close();
    delete iValueByteArray;
    delete iValueText;
    IRLOG_INFO( "CIRPropertyObserver2::~CIRPropertyObserver2 - Exiting" );
	}

// ---------------------------------------------------------------------------
// Subscribes to a property and reads the value, if not already active.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRPropertyObserver2::ActivateL()
	{
	IRLOG_INFO( "CIRPropertyObserver2::ActivateL - Entering" );
    if ( !IsActive() )
        {
        RunL();
        }
    IRLOG_INFO( "CIRPropertyObserver2::ActivateL - Exiting" );
	}

// -----------------------------------------------------------------------------
// CIRPropertyObserver2::RunL
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver2::RunL()
	{
	IRLOG_INFO( "CIRPropertyObserver2::RunL - Entering" );
	
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt err(KErrNone);
    
    switch (iPropertyType)
        {
        case EIRPropertyInt:
	        {
	        err = iProperty.Get( iValueInt );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, iValueInt );
             	}
	        break;
	        }
	    case EIRPropertyByteArray:
	        {
	        TPtr8 ptr8( iValueByteArray->Des() );
	        err = iProperty.Get( ptr8 );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, *iValueByteArray );
             	}
	        break;
	        }
	    case EIRPropertyText:
	        {
	        TPtr ptr( iValueText->Des() );
	        err = iProperty.Get( ptr );
	        if (!err)
            	{
                iObserver.HandlePropertyChangeL( iCategory, iKey, *iValueText );
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
    IRLOG_INFO( "CIRPropertyObserver2::RunL - Exiting" );
	}

// -----------------------------------------------------------------------------
// Cancels an outstanding active request
// -----------------------------------------------------------------------------
//
void CIRPropertyObserver2::DoCancel()
	{
	IRLOG_INFO( "CIRPropertyObserver2::DoCancel - Entering" );
    iProperty.Cancel();
    IRLOG_INFO( "CIRPropertyObserver2::DoCancel - Exiting" );
	}

// -----------------------------------------------------------------------------
// Getter for integer value
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CIRPropertyObserver2::ValueInt() const
	{
	IRLOG_INFO( "CIRPropertyObserver2::ValueInt" );
	return iValueInt;
	}
	
// -----------------------------------------------------------------------------
// Getter for byte array value
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CIRPropertyObserver2::ValueDes8() const
	{
	IRLOG_INFO( "CIRPropertyObserver2::ValueDes8" );
	return *iValueByteArray;
	}
	
// -----------------------------------------------------------------------------
// Getter for text value
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC& CIRPropertyObserver2::ValueDes() const
	{
	IRLOG_INFO( "CIRPropertyObserver2::ValueDes" );
	return *iValueText;
	}

