/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CIRSystemEventCollector
*
*/


#include <e32cmn.h>
#include <UiklafInternalCRKeys.h>
#include <centralrepository.h>

#include "iraccessoryobserver.h"
#include "irdiskspacewatcher.h"
//#include "irrepositorymanager.h"
#include "irsystemeventcollector.h"
#include "irsystemeventdetector.h"
#include "irdebug.h"

// Constants
const TInt KIRObserverArrayGranularity( 2 );
const TInt KIR_DEFAULT_CRITICAL_LEVEL = 3145728;	// 3MB
const TInt KThree =3;  
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CIRSystemEventCollector::CIRSystemEventCollector() : 
    iSystemEventObservers( KIRObserverArrayGranularity )
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::CIRSystemEventCollector" );
    }

// ---------------------------------------------------------------------------
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CIRSystemEventCollector::ConstructL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::ConstructL - Entering" );
    iSystemEventDetector = CIRSystemEventDetector::NewL( *this );

    iHeadsetObserver = CIRAccessoryObserver::NewL();
    iHeadsetObserver->SetObserver( this );

    TInt criticalLevel = 0;

	CRepository* cenrep = CRepository::NewLC(KCRUidUiklaf );
	TInt err = cenrep->Get(KUikOODDiskCriticalThreshold, criticalLevel);
	CleanupStack::PopAndDestroy(cenrep);
	if(err != KErrNone)
		{
		criticalLevel = KIR_DEFAULT_CRITICAL_LEVEL;
		IRLOG_WARNING( "CIRSystemEventCollector::ConstructL - Failed to read criticalLevel. Using default 3MB" );
		}
    criticalLevel = criticalLevel * KThree;
    iDiskSpaceWatcher = CIRDiskSpaceWatcher::NewL( static_cast<TInt64>( criticalLevel ), *this );
    iDiskSpaceWatcher->StartObservingDriveL( KVRDefaultDrive );

	IRRDEBUG3("CIRSystemEventCollector::ConstructL - DiskSpaceWather started. driveNum=%d, criticalLevel=%d ", KVRDefaultDrive, criticalLevel );

    IRLOG_DEBUG( "CIRSystemEventCollector::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSystemEventCollector* CIRSystemEventCollector::NewL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::NewL - Entering" );
    CIRSystemEventCollector* self = new( ELeave ) CIRSystemEventCollector;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRSystemEventCollector::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRSystemEventCollector::~CIRSystemEventCollector()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::~CIRSystemEventCollector - Entering" );
    delete iSystemEventDetector;
    delete iHeadsetObserver;
    
    if( iDiskSpaceWatcher )
	    {
	    delete iDiskSpaceWatcher;
	    iDiskSpaceWatcher = NULL;
	    }

    iSystemEventObservers.Close();
    IRLOG_DEBUG( "CIRSystemEventCollector::~CIRSystemEventCollector - Exiting" );
    }

// ---------------------------------------------------------------------------
// Adds a system event observer
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSystemEventCollector::AddObserverL( MIRSystemEventObserver* aHeadsetObserver )
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::AddObserverL - Entering" );
    iSystemEventObservers.AppendL( aHeadsetObserver );
    IRLOG_DEBUG( "CIRSystemEventCollector::AddObserverL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Removes a system event observer
// ---------------------------------------------------------------------------
//
 EXPORT_C void CIRSystemEventCollector::RemoveObserver( MIRSystemEventObserver* aSystemObserver )
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::RemoveObserver - Entering" );
    TInt objectIndex = iSystemEventObservers.Find( aSystemObserver );
    
    if ( objectIndex != KErrNotFound )
        {
        iSystemEventObservers.Remove( objectIndex );
        }
    IRLOG_DEBUG( "CIRSystemEventCollector::RemoveObserver - Exiting" );
    }

// ---------------------------------------------------------------------------
// Getter for mobile network state
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSystemEventCollector::IsMobileNetworkCoverage() const
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::IsMobileNetworkCoverage" );
    return iSystemEventDetector->IsMobileNetworkCoverage();
    }

// ---------------------------------------------------------------------------
// Getter for network state
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSystemEventCollector::IsNetworkCoverage() const
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::IsNetworkCoverage" );
    return iSystemEventDetector->IsNetworkCoverage();
    }

// ---------------------------------------------------------------------------
// Getter for call state
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSystemEventCollector::IsCallActive() const
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::IsCallActive" );
    return iSystemEventDetector->IsCallActive();
    }

// ---------------------------------------------------------------------------
// Getter for audio resource state
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSystemEventCollector::IsAudioResourcesAvailable() const
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::IsAudioResourcesAvailable" );
    return iSystemEventDetector->IsAudioResourcesAvailable();
    }

// ---------------------------------------------------------------------------
// Getter for headset connection status
// ---------------------------------------------------------------------------
EXPORT_C TBool CIRSystemEventCollector::IsHeadsetConnected() const
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::IsHeadsetConnected" );
    return iHeadsetObserver->IsHeadsetConnected();
    }
    
// ---------------------------------------------------------------------------
// Getter disk space state
// ---------------------------------------------------------------------------
EXPORT_C TBool CIRSystemEventCollector::IsDiskSpaceBelowCriticalLevel() const
    {
    return iDiskSpaceWatcher->IsBelowCriticalLevel( KVRDefaultDrive );
    }

// ---------------------------------------------------------------------------
// Getter Vocie UI state
// ---------------------------------------------------------------------------
EXPORT_C TBool CIRSystemEventCollector::IsVoiceUiActive() const
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::IsVoiceUiActive" );
    return iSystemEventDetector->IsVoiceUiActive();
    }

// ---------------------------------------------------------------------------
// Notifies the observers of system event
// ---------------------------------------------------------------------------
//
void CIRSystemEventCollector::NotifyObserversL(TIRSystemEventType aEvent)
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::NotifyObserversL - Entering" );
    for (TInt i = 0 ; i < iSystemEventObservers.Count() ; i++ )
        {
        iSystemEventObservers[i]->HandleSystemEventL( aEvent );
        }
    IRLOG_DEBUG( "CIRSystemEventCollector::NotifyObserversL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::NetworkUpCallbackL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::NetworkUpCallbackL - Entering" );
    NotifyObserversL( EIRNetworkCoverageUp );
    IRLOG_DEBUG( "CIRSystemEventCollector::NetworkUpCallbackL - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::NetworkDownCallbackL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::NetworkDownCallbackL - Entering" );
    NotifyObserversL( EIRNetworkCoverageDown );
    IRLOG_DEBUG( "CIRSystemEventCollector::NetworkDownCallbackL - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::CallActivatedCallbackL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::CallActivatedCallbackL - Entering" );
    NotifyObserversL( EIRCallActivated );
    IRLOG_DEBUG( "CIRSystemEventCollector::CallActivatedCallbackL - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::CallDeactivatedCallbackL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::CallDeactivatedCallbackL - Entering" );
    NotifyObserversL( EIRCallDeactivated );
    IRLOG_DEBUG( "CIRSystemEventCollector::CallDeactivatedCallbackL - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::AudioResourcesAvailableL()
    { 
    IRLOG_DEBUG( "CIRSystemEventCollector::AudioResourcesAvailableL - Entering" );
    NotifyObserversL( EIRAudioResourcesAvailable );
    IRLOG_DEBUG( "CIRSystemEventCollector::AudioResourcesAvailableL - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::AudioAutoResumeForbiddenL()
    {    
    IRLOG_DEBUG( "CIRSystemEventCollector::AudioAutoResumeForbiddenL - Entering" );
    NotifyObserversL( EIRAudioAutoResumeForbidden );
    IRLOG_DEBUG( "CIRSystemEventCollector::AudioAutoResumeForbiddenL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// From class MIRSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::ErrorCallbackL( TInt /* aError VRDEBUGVAR( aError ) d*/)
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::ErrorCallbackL" );
    // P&S get fail not considered as a critical issue.
	return;
    }

// ---------------------------------------------------------------------------
// From class MIRHeadsetEventObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::HeadsetConnectedCallbackL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::HeadsetConnectedCallbackL - Entering" );
    NotifyObserversL( EIRHeadsetConnected );
    IRLOG_DEBUG( "CIRSystemEventCollector::HeadsetConnectedCallbackL - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class MIRHeadsetEventObserver.
// ---------------------------------------------------------------------------
//        
void CIRSystemEventCollector::HeadsetDisconnectedCallbackL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::HeadsetDisconnectedCallbackL - Entering" );
    NotifyObserversL( EIRHeadsetDisconnected );
    IRLOG_DEBUG( "CIRSystemEventCollector::HeadsetDisconnectedCallbackL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class MIRDiskSpaceWatcherObserver.
// ---------------------------------------------------------------------------
//
void CIRSystemEventCollector::NotifyLowDiskSpaceL()
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::NotifyLowDiskSpaceL - Entering" );
    NotifyObserversL( EIRLowDiskSpace );
    IRLOG_DEBUG( "CIRSystemEventCollector::NotifyLowDiskSpaceL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class MIRAudioRoutingObserver.
// ---------------------------------------------------------------------------
//
void CIRSystemEventCollector::AudioRoutingChangedL( TVROutputDestination aOutputDestination )
    {
    IRLOG_DEBUG( "CIRSystemEventCollector::AudioRoutingChangedL - Entering" );
    TIRSystemEventType ev = EIRAudioRoutingHeadset; 
    if(aOutputDestination != EVRHeadset)
        {
        ev = EIRAudioRoutingSpeaker;
        }

    NotifyObserversL( ev );
    IRLOG_DEBUG( "CIRSystemEventCollector::AudioRoutingChangedL - Exiting" );
    }
