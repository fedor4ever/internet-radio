/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Source file of CIRActiveIdleStateDetector
*
*/


#include "irdebug.h"
#include <apgtask.h>
#include <eikenv.h>

#include "iractiveidlestatedetector.h"
#include "iractiveidleengnotifyhandler.h"
#include "ircommon.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRActiveIdleStateDetector::CIRActiveIdleStateDetector(MIRActiveIdleEngNotifyHandler& 
	aStateObserver):CActive(CActive::EPriorityStandard),iStateObserver(aStateObserver)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRActiveIdleStateDetector::~CIRActiveIdleStateDetector()
    {
	Cancel();
    }

// ---------------------------------------------------------------------------
// Starts listening
// ---------------------------------------------------------------------------
//
void CIRActiveIdleStateDetector::StartListeningL()
    {
    IRLOG_DEBUG("CIRActiveIdleStateDetector::StartListeningL.");
	if ( !IsActive() )
	    {
    	TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
    	TApaTask task = taskList.FindApp( KUidInternetRadioApp );
    		
    	if ( task.Exists() )
    		{
    		TThreadId threadId = task.ThreadId();
    		RThread thread;
    		User::LeaveIfError( thread.Open( threadId ) );
    		CleanupClosePushL( thread );
    		User::LeaveIfError( thread.Process( iProcess ) );
    		CleanupStack::PopAndDestroy();
            IRLOG_INFO("CIRActiveIdleStateDetector::StartListeningL - Logon.");
        	iProcess.Logon( iStatus );
        	SetActive();	
    		}
		else
		    {
            IRLOG_INFO("CIRActiveIdleStateDetector::StartListeningL  - sending EIRPSApplicationClosing.");
	        iStateObserver.HandleApplicationRunningStateChangeL( EIRPSApplicationClosing );
		    }
	    }
    IRLOG_DEBUG("CVRActiveIdleStateDetector::StartListeningL - Exiting.");	    
    }

// ---------------------------------------------------------------------------
// Implements cancellation of an outstanding request
// ---------------------------------------------------------------------------
//
void CIRActiveIdleStateDetector::DoCancel()
    {
    IRLOG_DEBUG("CIRActiveIdleStateDetector::DoCancel");
	iProcess.LogonCancel( iStatus );
	iProcess.Close();
    }

// ---------------------------------------------------------------------------
// Handles request completion event
// ---------------------------------------------------------------------------
//
void CIRActiveIdleStateDetector::RunL()
    {
    IRLOG_INFO2("CIRActiveIdleStateDetector::RunL (%d) sending EIRPSApplicationClosing", iStatus.Int());
	iProcess.Close();
	// Internet Radio has been closed
	iStateObserver.HandleApplicationRunningStateChangeL( EIRPSApplicationClosing );
    }

