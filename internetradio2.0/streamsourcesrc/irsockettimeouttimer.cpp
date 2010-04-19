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
#include "irsockettimeouttimer.h"

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CIRSocketTimeOutTimer::CIRSocketTimeOutTimer( const TInt aPriority, 
	MIRSocketTimeoutNotifier& aNotify) : CTimer( aPriority ),
	iNotify( aNotify )
	{
	// Definition not required
	}

// ---------------------------------------------------------------------------
// Default destructor
// ---------------------------------------------------------------------------
//
CIRSocketTimeOutTimer::~CIRSocketTimeOutTimer()
	{
	// Cancel any pending requests
	Cancel();
	}

// ---------------------------------------------------------------------------
// Creates the CTimer object
// ---------------------------------------------------------------------------
//
void CIRSocketTimeOutTimer::ConstructL()
	{
	// Create the Timer
	CTimer::ConstructL();
	// Add the active object active scheduler 
	CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
// CIRSocketTimeOutTimer::NewL
// Creates an object of CIRSocketTimeOutTimer 
// ---------------------------------------------------------------------------
//
CIRSocketTimeOutTimer* CIRSocketTimeOutTimer::NewL( const TInt aPriority,
	MIRSocketTimeoutNotifier& aNotify )
	{
	CIRSocketTimeOutTimer* self = CIRSocketTimeOutTimer::NewLC( aPriority,
		aNotify );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CIRSocketTimeOutTimer::NewLC
// Creates an object of CIRSocketTimeOutTimer 
// ---------------------------------------------------------------------------
//
CIRSocketTimeOutTimer* CIRSocketTimeOutTimer::NewLC( const TInt aPriority, 
	MIRSocketTimeoutNotifier& aNotify )
	{
	CIRSocketTimeOutTimer* self = new (ELeave) CIRSocketTimeOutTimer( 
		aPriority, aNotify );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CIRSocketTimeOutTimer::RunL
// Called when the timer expires 
// This is used to notify the observer that timer has expired
// ---------------------------------------------------------------------------
//    
void CIRSocketTimeOutTimer::RunL()
	{
	IRLOG_INFO2( "CIRSocketTimeOutTimer::RunL - Timer expired (%d).", iStatus.Int() );	
	if( iStatus == KErrNone )
		{
		iNotify.TimerExpired();
		}
	else
		{
		User::Leave(iStatus.Int());
		}
	}

// ---------------------------------------------------------------------------
// CIRSocketTimeOutTimer::RunError
// Called if RunL leaves
// ---------------------------------------------------------------------------
//    
TInt CIRSocketTimeOutTimer::RunError( TInt /*aError*/ )
	{
	// Ignore any timer expiry errors as they are not critical in this case
	IRLOG_DEBUG( "CIRSocketTimeOutTimer::RunError" );
	return KErrNone;
	}
