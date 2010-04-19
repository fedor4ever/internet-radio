/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Performs disk space watching.
*
*/


#include    <f32file.h>

#include    "irdebug.h"
#include    "irdiskspacewatcher.h"
#include    "irdiskspacesession.h"


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRDiskSpaceWatcher::CIRDiskSpaceWatcher( const TInt64& aCriticalLevel, 
        MIRDiskSpaceWatcherObserver& aDiskSpaceWatcherObserver ) :
        iCriticalLevel( aCriticalLevel ), 
        iObserver ( aDiskSpaceWatcherObserver )
    {
    }

// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceWatcher::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRDiskSpaceWatcher* CIRDiskSpaceWatcher::NewL( const TInt64& aCriticalLevel, 
        MIRDiskSpaceWatcherObserver& aDiskSpaceWatcherObserver )
    
    {
    CIRDiskSpaceWatcher* self = new( ELeave ) CIRDiskSpaceWatcher( aCriticalLevel, 
        aDiskSpaceWatcherObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRDiskSpaceWatcher::~CIRDiskSpaceWatcher()
    {   
    iDiskSpaceSessions.ResetAndDestroy();
    iDiskSpaceSessions.Close();
    iFs.Close();
    }


// ---------------------------------------------------------------------------
// Returns ETrue if below the critical level in defined disk, otherwise EFalse
// ---------------------------------------------------------------------------
//
TBool CIRDiskSpaceWatcher::IsBelowCriticalLevel( 
        const TDriveNumber& aDriveNumber ) const
    {
    TBool returnValue = EFalse; 
 
    if ( iDiskSpaceSessions.Count() > 0 )
        {        
        CIRDiskSpaceSession* session = iDiskSpaceSessions[0];
        returnValue = session->IsBelowCriticalLevel( aDriveNumber );
        }

    return returnValue;
    }


// ---------------------------------------------------------------------------
// CIRDiskSpaceWatcher::StartObservingDriveL
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceWatcher::StartObservingDriveL( 
        const TDriveNumber& aDriveNumber )
    {
    TBool found = EFalse;

    for ( TInt i = 0 ; i < iDiskSpaceSessions.Count() ; i++ )
        {
        CIRDiskSpaceSession* session = iDiskSpaceSessions[i];
        if ( session->DriveNumber() == aDriveNumber )
            {
            session->SetObserverCount( session->ObserverCount() + 1 );
            found = ETrue;
            /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
            break;
            /*lint -restore*/
            }
        }
    if ( !found )
        {
        CIRDiskSpaceSession* session = CIRDiskSpaceSession::NewL( *this, 
            aDriveNumber, iCriticalLevel, iFs );
        session->SetObserverCount( 1 );
        iDiskSpaceSessions.Append( session );
        }
    }


// ---------------------------------------------------------------------------
// CIRDiskSpaceWatcher::StopObservingDrive
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceWatcher::StopObservingDrive( 
        const TDriveNumber& aDriveNumber )     
    {
    for ( TInt i = 0 ; i < iDiskSpaceSessions.Count() ; i++ )
        {
        CIRDiskSpaceSession* session = iDiskSpaceSessions[i];
        if ( session->DriveNumber() == aDriveNumber )
            {
            session->SetObserverCount( session->ObserverCount() - 1 );
            if ( session->ObserverCount() == 0 )
                {
                iDiskSpaceSessions.Remove( i );
                delete session;
                session = NULL;
                }
            /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
            break;
            /*lint -restore*/
            }
        }
    
    }

    
// ---------------------------------------------------------------------------
// From class MIRDiskSpaceWatcherObserver.
// CIRDiskSpaceWatcher::NotifyLowDiskSpaceL
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceWatcher::NotifyLowDiskSpaceL()
    {
    IRRDEBUG2("CIRDiskSpaceWatcher::NotifyLowDiskSpaceL - Phone LOW MEMORY detected", KNullDesC);
    iObserver.NotifyLowDiskSpaceL();
    }
