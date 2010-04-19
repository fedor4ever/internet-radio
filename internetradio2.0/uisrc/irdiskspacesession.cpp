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
* Description:  Contains logic for watching certain disk's space
*
*/


#include "irdebug.h"
#include "irdiskspacesession.h"
#include "irdiskspacewatcherobserver.h"

// ---------------------------------------------------------------------------
// C++ Constructor
// ---------------------------------------------------------------------------
//
CIRDiskSpaceSession::CIRDiskSpaceSession( 
        MIRDiskSpaceWatcherObserver& aDiskSpaceWatcherObserver, 
        const TDriveNumber& aDriveNumber, 
        const TInt64& aCriticalLevel,
        RFs& aFs ) : 
            CActive( CActive::EPriorityStandard ),       
            iObserver( aDiskSpaceWatcherObserver ),
            iDriveNumber( aDriveNumber ),
            iCriticalLevel ( aCriticalLevel ),
            iFs ( aFs )
    {
    }
            

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CIRDiskSpaceSession* CIRDiskSpaceSession::NewL( 
        MIRDiskSpaceWatcherObserver& aDiskSpaceWatcherObserver, 
        const TDriveNumber& aDriveNumber, 
        const TInt64& aCriticalLevel,
        RFs& aFs )
    {
    
    CIRDiskSpaceSession* self = new( ELeave ) CIRDiskSpaceSession( 
            aDiskSpaceWatcherObserver,
            aDriveNumber, 
            aCriticalLevel,
            aFs );
   
	CleanupStack::PushL( self );
	
	self->ConstructL();
        
	CleanupStack::Pop( self );
    return self;
    
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//            
CIRDiskSpaceSession::~CIRDiskSpaceSession()
    {        
    Cancel();
    }
            
            
// ---------------------------------------------------------------------------
// Second stage constructor
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceSession::ConstructL()
    {
    CActiveScheduler::Add( this );
    RunL();
    }
    
            
// ---------------------------------------------------------------------------
// DriveNumber
// ---------------------------------------------------------------------------
//
TDriveNumber CIRDiskSpaceSession::DriveNumber() 
    {
    return iDriveNumber;
    }

    
// ---------------------------------------------------------------------------
// SetObserverCount
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceSession::SetObserverCount( TInt aObserverCount ) 
    {
    iObserverCount = aObserverCount;
    }


// ---------------------------------------------------------------------------
// ObserverCount
// ---------------------------------------------------------------------------
//
TInt CIRDiskSpaceSession::ObserverCount() 
    {
    return iObserverCount;
    }

            
// ---------------------------------------------------------------------------
// From class CActive. 
// Cancels the pending request.
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceSession::DoCancel() 
    {    
    iFs.NotifyDiskSpaceCancel( iStatus );
    }


// ---------------------------------------------------------------------------
// From class CActive. 
// Invoked when the observed disk's free disk space has run below
// the supplied critical level.
// Notifies the observer and regenerates the request to RFs' NotifyDiskSpace.
// ---------------------------------------------------------------------------
//
void CIRDiskSpaceSession::RunL()
    { 
	IRRDEBUG2("CIRDiskSpaceSession::RunL - iDriveNumber=%d", iDriveNumber);
    IRRDEBUG2("CIRDiskSpaceSession::RunL - iCriticalLevel=%d", iCriticalLevel);
    iFs.NotifyDiskSpace( iCriticalLevel, iDriveNumber, iStatus );
    SetActive();
        
    if ( IsBelowCriticalLevel( iDriveNumber ) )
        {
        IRRDEBUG2("CIRDiskSpaceSession::RunL - LOW MEMORY detected", KNullDesC);
        TRAP_IGNORE( iObserver.NotifyLowDiskSpaceL() )
        }
    }
    

// ---------------------------------------------------------------------------
// Returns ETrue if below the critical level in defined disk, otherwise EFalse
// ---------------------------------------------------------------------------
//
TBool CIRDiskSpaceSession::IsBelowCriticalLevel(
        const TDriveNumber& aDriveNumber ) const    
    {
    TBool ret = EFalse;

    TVolumeInfo volInfo;
    if ( iFs.Volume( volInfo, aDriveNumber ) == KErrNone )
        {
        if ( volInfo.iFree < iCriticalLevel )
            {
            IRRDEBUG3( "CIRDiskSpaceSession::IsBelowCriticalLevel - Low Memory. Disk space [%d] is under the critical [%d] level!", 
                static_cast<TInt>( volInfo.iFree ), 
                static_cast<TInt>( iCriticalLevel ) ); 
            ret = ETrue;
            }
        else
	        {
            IRRDEBUG3( "CIRDiskSpaceSession::IsBelowCriticalLevel - Enough Disk space [%d] in DriveNumber [%d].", 
                static_cast<TInt>( volInfo.iFree ), 
                static_cast<TInt>( aDriveNumber ) ); 
	        }
        }
    
    return ret;
    }
