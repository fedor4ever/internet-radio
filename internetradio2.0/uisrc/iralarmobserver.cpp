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
* Description:  
*
*/


#include "iralarmobserver.h"
#include "irdebug.h"
#include "irui.h"



// ---------------------------------------------------------------------------
// Function : NewL
// Function returns an instance of CIRAlarmObserver
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRAlarmObserver* CIRAlarmObserver::NewL(CIRUi* aUi)
	{
	IRLOG_DEBUG( "CIRAlarmObserver::NewL - Entering" );
	CIRAlarmObserver* self = CIRAlarmObserver::NewLC(aUi);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRAlarmObserver::NewL - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : NewLC
// Function returns an instance of CIRAlarmObserver
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRAlarmObserver* CIRAlarmObserver::NewLC(CIRUi* aUi)
	{
	IRLOG_DEBUG( "CIRAlarmObserver::NewLC - Entering" );
	CIRAlarmObserver* self = new (ELeave) CIRAlarmObserver;
	CleanupStack::PushL(self);
	self->ConstructL(aUi);
	IRLOG_DEBUG( "CIRAlarmObserver::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Function : ~CIRAlarmObserver
// Default destructor calls Cancel function which cancel the active request 
// ---------------------------------------------------------------------------
//
CIRAlarmObserver::~CIRAlarmObserver()
	{
	IRLOG_DEBUG( "CIRAlarmObserver::~CIRAlarmObserver - Entering" );
	Cancel();
	iAlarmSession.Close();	
	IRLOG_DEBUG( "CIRAlarmObserver::~CIRAlarmObserver - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : Start
// Function starts the call observer
// ---------------------------------------------------------------------------
//
void CIRAlarmObserver::Start()
	{
	IRLOG_DEBUG( "CIRAlarmObserver::Start - Entering" );
	//! Starts monitoring of call events when application is running
	if( !IsActive() )
		{
		iAlarmSession.NotifyChange(iStatus,iAlarmId);
		SetActive();
		}
	IRLOG_DEBUG( "CIRAlarmObserver::Start - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : RunL
// handles only alarm start and stop events 
// ---------------------------------------------------------------------------
//
void CIRAlarmObserver::RunL()
	{
	IRLOG_DEBUG( "CIRAlarmObserver::RunL - Entering" );
	//! handles when call comes the player is stopped
	if( EAlarmChangeEventSoundPlaying == iStatus.Int() ) 
		{
		iUi->HandleAlarmStart();
		iAlarmOn = ETrue;
		}
	//! handles when call is diconnected, the play again is called
	else if( EAlarmChangeEventSoundStopped == iStatus.Int() ) 
		{
		if ( iAlarmOn )
			{
			iUi->HandleAlarmEnd();
			iAlarmOn = EFalse;	
			}
		else
			{
			iUi->HandleInitialAlarmEvent();
			}
		
		}
	else
		{
		//No implementation	
		}	
	Start(); //starts monitoring again
	IRLOG_DEBUG( "CIRAlarmObserver::RunL - Exiting." );	
	}
	
// ---------------------------------------------------------------------------
// Function : DoCancel
// Function stops the call observer if it is active
// ---------------------------------------------------------------------------
//
void CIRAlarmObserver::DoCancel()
	{
	IRLOG_DEBUG( "CIRAlarmObserver::DoCancel - Entering" );
	if( IsActive() )
		{
		iAlarmSession.NotifyChangeCancel();	
		}	
	IRLOG_DEBUG( "CIRAlarmObserver::DoCancel - Exiting" );
	}

// ---------------------------------------------------------------------------
// Function : RunError
// Function which handles RunL error
// ---------------------------------------------------------------------------
//
TInt CIRAlarmObserver::RunError(TInt aError)
	{
	IRLOG_DEBUG( "CIRAlarmObserver::RunError" );
	return aError;		
	}

// ---------------------------------------------------------------------------
// Function : ConstructL
// Function does all the initializations
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CIRAlarmObserver::ConstructL(CIRUi* aUi)
	{
	IRLOG_DEBUG( "CIRAlarmObserver::ConstructL - Entering" );
	iUi = aUi;
	iAlarmOn = EFalse;	
	User::LeaveIfError(iAlarmSession.Connect());
	CActiveScheduler::Add(this);			
	IRLOG_DEBUG( "CIRAlarmObserver::ConstructL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : CIRAlarmObserver
// Function is the default constructor, sets the CActive priority, Line status
// ---------------------------------------------------------------------------
//
CIRAlarmObserver::CIRAlarmObserver():CActive(CActive::EPriorityStandard)
	{
	IRLOG_DEBUG( "CIRAlarmObserver::CIRAlarmObserver" );
	}





