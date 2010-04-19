/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer of accessory states.
*
*/


#include <accpolgenericid.h>

#include "iraccessoryobserver.h"
#include "irheadseteventobserver.h"
#include "irdebug.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRAccessoryObserver::CIRAccessoryObserver() : 
        CActive( CActive::EPriorityStandard )
    {
    IRLOG_DEBUG( "CIRAccessoryObserver::CIRAccessoryObserver" );
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRAccessoryObserver::ConstructL()
    {
    IRLOG_DEBUG( "CIRAccessoryObserver::ConstructL - Entering" );
    User::LeaveIfError( iAccessoryServer.Connect() );
    
    // Creates a new sub session within an existing session.
    User::LeaveIfError( iAccessoryModeSession.CreateSubSession( iAccessoryServer ) );
    User::LeaveIfError( iAccessoryModeSession.GetAccessoryMode( iAccPolAccessoryMode ) );

    CActiveScheduler::Add( this );      
    // Accessory mode is always listened
    iAccessoryModeSession.NotifyAccessoryModeChanged( iStatus, iAccPolAccessoryMode ); 
    SetActive();
    IRLOG_DEBUG( "CIRAccessoryObserver::ConstructL - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------------------------
//
CIRAccessoryObserver* CIRAccessoryObserver::NewL()
    {
    IRLOG_DEBUG( "CIRAccessoryObserver::NewL - Entering" );
    CIRAccessoryObserver* self = new( ELeave ) CIRAccessoryObserver;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRAccessoryObserver::NewL - Exiting" );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRAccessoryObserver::~CIRAccessoryObserver()
    {
    IRLOG_DEBUG( "CIRAccessoryObserver::~CIRAccessoryObserver - Entering" );
    Cancel();
	iAccessoryModeSession.CloseSubSession();
	iAccessoryServer.Close();
	IRLOG_DEBUG( "CIRAccessoryObserver::~CIRAccessoryObserver - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRAccessoryObserver::IsHeadsetConnected
// ---------------------------------------------------------------------------
//
TBool CIRAccessoryObserver::IsHeadsetConnected() const
	{
	IRLOG_DEBUG( "CIRAccessoryObserver::IsHeadsetConnected - Entering" );
	TBool accessoryConnected;
	
#ifdef __WINS__
    accessoryConnected = ETrue;
#else
	switch ( iAccPolAccessoryMode.iAccessoryMode )
        {
        case EAccModeWiredHeadset:    // Falls through.
        case EAccModeLoopset:         // Falls through.
        case EAccModeHeadphones:
            {
            accessoryConnected = ETrue;
	        break;
            }
        default:
            {
			accessoryConnected = EFalse;
			break;
			}
        }
#endif   
	IRLOG_DEBUG( "CIRAccessoryObserver::IsHeadsetConnected - Exiting" );
	return accessoryConnected;
	}


// ---------------------------------------------------------------------------
// CIRAccessoryObserver::SetObserver
// ---------------------------------------------------------------------------
//
void CIRAccessoryObserver::SetObserver( MIRHeadsetEventObserver* aObserver )
	{
	IRLOG_DEBUG( "CIRAccessoryObserver::SetObserver - Entering" );
	iObserver = aObserver;
	IRLOG_DEBUG( "CIRAccessoryObserver::SetObserver - Exiting" );
	}

	
// ---------------------------------------------------------------------------
// From class CActive
// CIRAccessoryObserver::RunL
// ---------------------------------------------------------------------------
//
void CIRAccessoryObserver::RunL()
    {    
    IRLOG_DEBUG( "CIRAccessoryObserver::RunL - Entering" );
    TRequestStatus status = iStatus;
    iAccessoryModeSession.NotifyAccessoryModeChanged( iStatus, 
        iAccPolAccessoryMode ); 
    SetActive(); 
    
    if ( status == KErrNone )
        {
        // Accessory mode may change when combined connection status changes 
        // or when audio routing status changes. 
        if(IsHeadsetConnected())
            {
            iObserver->HeadsetConnectedCallbackL();
            }
        else
            {
            iObserver->HeadsetDisconnectedCallbackL();
            }
        }
    IRLOG_DEBUG( "CIRAccessoryObserver::RunL - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// From class CActive
// CIRAccessoryObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CIRAccessoryObserver::DoCancel()
    {
    IRLOG_DEBUG( "CIRAccessoryObserver::RunL - Entering" );
    iAccessoryModeSession.CancelNotifyAccessoryModeChanged();
    IRLOG_DEBUG( "CIRAccessoryObserver::RunL - Exiting" );
    
    }
