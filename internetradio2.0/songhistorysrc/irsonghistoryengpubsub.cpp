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
* Description:  Publish / Subscriber class for ChannelName and MetaData
*
*/


#include "irsonghistoryengpubsub.h"
#include "irsonghistoryengpubsubnotifyhandler.h"
#include "irdebug.h"


// ==================== MEMBER FUNCTIONS ===================

// ---------------------------------------------------------------------------
// CIRSongHistoryEngPubSub::CIRSongHistoryEngPubSub
// Constructor.
// ---------------------------------------------------------------------------
//
CIRSongHistoryEngPubSub::CIRSongHistoryEngPubSub(
	MIRSongHistoryEngPubSubNotifyHandler& aPubSubObserver, const TUid& aCategory,
	TUint aKey , const RProperty::TType aPropertyType): CActive( CActive::
	EPriorityStandard ),  iPropertyType(aPropertyType),iPubSubObserver( aPubSubObserver ),
	iCategory( aCategory ), iKey( aKey )
    {
    }

// ---------------------------------------------------------------------------
// CIRSongHistoryEngPubSub::ConstructL
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CIRSongHistoryEngPubSub::ConstructL()
    {
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::ConstructL" );
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
    iProperty.Subscribe( iStatus );
    SetActive();
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::ConstructL - Exiting." );
    }

//-----------------------------------------------------------------------------
//  CIRHistoryListContainer::NewL
//  Creates a new instance of CIRSongHistoryEngPubSub
//-----------------------------------------------------------------------------
//
CIRSongHistoryEngPubSub* CIRSongHistoryEngPubSub::NewL(
	MIRSongHistoryEngPubSubNotifyHandler& aPubSubObserver, const TUid& aCategory,
	TUint aKey, const RProperty::TType aPropertyType  )
    {
	IRLOG_DEBUG( "CIRSongHistoryEngPubSub::NewL - Entering" );
    CIRSongHistoryEngPubSub* self = CIRSongHistoryEngPubSub::NewLC(aPubSubObserver,
    					 aCategory,aKey, aPropertyType  );
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRSongHistoryEngPubSub::NewL - Exiting." );
    return self;
    }

//-----------------------------------------------------------------------------
//  CIRHistoryListContainer::NewLC
//  Creates a new instance of CIRSongHistoryEngPubSub and leaves it on the cleanup stack.
//-----------------------------------------------------------------------------
//
CIRSongHistoryEngPubSub* CIRSongHistoryEngPubSub::NewLC(
	MIRSongHistoryEngPubSubNotifyHandler& aPubSubObserver, const TUid& aCategory,
	TUint aKey, const RProperty::TType aPropertyType  )
    {
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::NewLC - Entering" );
    CIRSongHistoryEngPubSub* self = new (ELeave) CIRSongHistoryEngPubSub(
    							aPubSubObserver, aCategory, aKey, aPropertyType);
    CleanupStack::PushL(self);
    self->ConstructL();
	IRLOG_DEBUG( "CIRSongHistoryEngPubSub::NewLC - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRSongHistoryEngPubSub::~CIRSongHistoryEngPubSub()
// Destructor
// ---------------------------------------------------------------------------
//
CIRSongHistoryEngPubSub::~CIRSongHistoryEngPubSub()
    {
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::~CIRSongHistoryEngPubSub" );
    Cancel();
    iProperty.Close();
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::~CIRSongHistoryEngPubSub - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRSongHistoryEngPubSub::DoCancel()
// Invoked when the active object is cancelled.
// ---------------------------------------------------------------------------
//
void CIRSongHistoryEngPubSub::DoCancel()
    {
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::DoCancel" );
    iProperty.Cancel();
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::DoCancel - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRSongHistoryEngPubSub::RunL()
// The function is called by the active scheduler when a request completion event occurs,
// ---------------------------------------------------------------------------
//
void CIRSongHistoryEngPubSub::RunL()
    {
    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::RunL" );

    iProperty.Subscribe( iStatus );
    SetActive();
    TInt err(KErrNone);


	err = iProperty.Get( iText );
	if (!err)
		{
			iPubSubObserver.HandlePropertyChangeL( iCategory, iKey, iText );
		}

    else
    	{
        	iPubSubObserver.HandlePropertyChangeErrorL(iCategory, iKey, err);
    	}

    IRLOG_DEBUG( "CIRSongHistoryEngPubSub::RunL - Exiting." );
    }
