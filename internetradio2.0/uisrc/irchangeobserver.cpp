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
 

#include "irchangeobserver.h"
#include "irdebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIRObserver::CIRObserver
// C++ default constructor CIRObserver
// -----------------------------------------------------------------------------
//
CIRObserver::CIRObserver(MIRChangeObserver& aObserver, const TUid& aCategory, 
	const TUint aKey, const RProperty::TType aPropertyType)
    : CActive( CActive::EPriorityLow ),
    iObserver( aObserver ),
    iCategory( aCategory ),
    iKey( aKey ),
    iPropertyType( aPropertyType )
	{
	IRLOG_DEBUG( "CIRObserver::CIRObserver" );
	}

// -----------------------------------------------------------------------------
// CIRObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIRObserver::ConstructL()
	{
	IRLOG_DEBUG( "CIRObserver::ConstructL - Entering" );
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
    RunL();
	IRLOG_DEBUG( "CIRObserver::ConstructL - Exiting." );
	}

// -----------------------------------------------------------------------------
// CIRObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRObserver* CIRObserver::NewL(MIRChangeObserver& aObserver, const TUid& 
	aCategory, const TUint aKey, const RProperty::TType aPropertyType)
	{
	IRLOG_DEBUG( "CIRObserver::NewL - Entering" );
    CIRObserver* self = new( ELeave )CIRObserver( aObserver, aCategory, aKey,
    	aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRObserver::NewL - Exiting." );
    return self;
	}

// -----------------------------------------------------------------------------
// CIRObserver::~CIRObserver
// C++ default destructor
// -----------------------------------------------------------------------------
//
CIRObserver::~CIRObserver()
	{
	IRLOG_DEBUG( "CIRObserver::~CIRObserver - Entering" );
	Cancel();
	iProperty.Close(); //Closes the publish and subscribe handler
	IRLOG_DEBUG( "CIRObserver::~CIRObserver - Exiting." );
	}

// -----------------------------------------------------------------------------
// CIRObserver::RunL
// The function is called by the active scheduler 
// when a request completion event occurs
// -----------------------------------------------------------------------------
//
void CIRObserver::RunL()
	{
	IRLOG_DEBUG( "CIRObserver::RunL - Entering" );
	//Subscribe the handler
    iProperty.Subscribe( iStatus );
    SetActive();
    TInt err(KErrNone);
    switch (iPropertyType)
        {
        case RProperty::EInt:
	        {
	        err = iProperty.Get( iValueInt );
	        if (!err)
            	{
            	//callback function called when a property value of type int.
                iObserver.HandleChangeL( iCategory, iKey, iValueInt );
             	}
	        break;
	        }
	     case RProperty::EText:
	        {
	        break;
	       	}
	    default:
	        {
	        break;
	        }
        }
	  IRLOG_DEBUG( "CIRObserver::RunL - Exiting." );
      }

// -----------------------------------------------------------------------------
// CIRObserver::DoCancel
// Cancels an outstanding active request
// -----------------------------------------------------------------------------
//
void CIRObserver::DoCancel()
	{
	IRLOG_DEBUG( "CIRObserver::DoCancel - Entering" );
    iProperty.Cancel();
    IRLOG_DEBUG( "CIRObserver::DoCancel - Exiting" );
	}

// -----------------------------------------------------------------------------
// CIRObserver::GetValue
// -----------------------------------------------------------------------------
//	
void CIRObserver::GetValue(TInt& aValue) const
	{
	IRLOG_DEBUG( "CIRObserver::GetValue - Entering" );
	//aValue assigned with the current Property value
	aValue = iValueInt;
	IRLOG_DEBUG( "CIRObserver::GetValue - Exiting" );
	}

// End of File

