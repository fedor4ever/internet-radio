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


#include <msvids.h> // Folder Ids

#include "irmessageobserver.h"
#include "irdebug.h"
#include "irui.h"
 


 
// When using the emulator the observed folder is Drafts, otherwise Inbox.
#ifdef __WINS__
	const TMsvId KDrafts = KMsvDraftEntryId;
    const TMsvId KObservedFolderId = KDrafts;
#else
	const TMsvId KInbox = KMsvGlobalInBoxIndexEntryId;
    const TMsvId KObservedFolderId = KInbox;
#endif
 
// ------------------------------------------------------------
// CIRMessageObserver::NewL(CIRUi* aUi)
// 2nd phase constructor.
// ------------------------------------------------------------
CIRMessageObserver* CIRMessageObserver::NewL(CIRUi* aUi)
    {
    IRLOG_DEBUG( "CIRMessageObserver::NewL - Entering" );
    CIRMessageObserver* self = CIRMessageObserver::NewLC(aUi);
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRMessageObserver::NewL - Exiting" );
    return self;
    }
 
// -----------------------------------------------------------
// CIRMessageObserver::NewL(CIRUi* aUi)
//  2nd phase constructor.
// -----------------------------------------------------------
CIRMessageObserver* CIRMessageObserver::NewLC(CIRUi* aUi)
    {
    IRLOG_DEBUG( "CIRMessageObserver::NewLC - Entering" );
    CIRMessageObserver* self = new (ELeave) CIRMessageObserver(aUi);
    CleanupStack::PushL(self);
    self->ConstructL();
    IRLOG_DEBUG( "CIRMessageObserver::NewLC - Exiting" );
    return self;
    }
 
// --------------------------------------------------------------
// CIRMessageObserver(CIRUi* aUi)
// Symbian OS default constructor.
// --------------------------------------------------------------
CIRMessageObserver::CIRMessageObserver(CIRUi* aUi) : iUi(aUi)
    {
    IRLOG_DEBUG( "CIRMessageObserver::CIRMessageObserver" );
    }
 
// --------------------------------------------------------------
// CIRMessageObserver::ConstructL()
// 2nd phase constructor.
// --------------------------------------------------------------
void CIRMessageObserver::ConstructL()
    {
    IRLOG_DEBUG( "CIRMessageObserver::ConstructL - Entering" );
    // SMS automatic receiving needs a session to the messaging server
    iMsvSession = CMsvSession::OpenAsyncL(*this);
    IRLOG_DEBUG( "CIRMessageObserver::ConstructL - Exiting" );
    }
 
// --------------------------------------------------------------
// ~CIRMessageObserver()
// Destructor.
// --------------------------------------------------------------
CIRMessageObserver::~CIRMessageObserver()
    {
    IRLOG_DEBUG( "CIRMessageObserver::~CIRMessageObserver - Entering" );
    if (iMsvSession)
        {
        iMsvSession->Cancel();
        }
    delete iMsvSession;
    iMsvSession = NULL;
    IRLOG_DEBUG( "CIRMessageObserver::~CIRMessageObserver - Exiting" );
    }
 
// --------------------------------------------------------------
// CIRMessageObserver::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
//        TAny* aArg2, TAny* /*aArg3*/)
// From MMsvSessionObserver.
// Called when a session event occurs.
// --------------------------------------------------------------
void CIRMessageObserver::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* /*aArg1*/,
        TAny* aArg2, TAny* /*aArg3*/)
    {
    IRLOG_DEBUG( "CIRMessageObserver::HandleSessionEventL - Entering" );
    switch (aEvent)
        {
		case EMsvEntriesCreated:
//d        case EMsvEntriesChanged:
            {
            // If the event happens in the observed folder, notify the observer
            if (aArg2 && *(static_cast<TMsvId*>(aArg2)) == KObservedFolderId)
                {
                iUi->HandleMessageAlertL();
                }
            break;
            }
        }
    IRLOG_DEBUG( "CIRMessageObserver::HandleSessionEventL - Exiting" );
    }



