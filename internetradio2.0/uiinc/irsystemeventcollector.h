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
* Description:  Declaration of CIRSystemEventCollector
*
*/


#ifndef CIRSYSTEMEVENTCOLLECTOR_H
#define CIRSYSTEMEVENTCOLLECTOR_H



#include <e32base.h>

#include "iraudioroutingobserver.h"
#include "irdiskspacewatcherobserver.h"
#include "irheadseteventobserver.h"
#include "irsystemeventdetectorobserver.h"
#include "irsystemeventobserver.h"

class CIRAccessoryObserver;
class CIRDiskSpaceWatcher;
class CIRSystemEventDetector;

/**
 *  Collects system events and forwards them to observers
 *  Class also wraps up the getters for some system states
 *
 *  @lib vrsystemutils.lib
 */
NONSHARABLE_CLASS(CIRSystemEventCollector) : 
    public CBase,
    public MIRAudioRoutingObserver,
    public MIRDiskSpaceWatcherObserver, 
    public MIRHeadsetEventObserver,
    public MIRSystemEventDetectorObserver
    {
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CIRSystemEventCollector* NewL();
    
    /**
     * Destructor
     */
    ~CIRSystemEventCollector();
    
    /**
     * Adds a system event observer
     *
     * @param aSystemObserver The observer to be added
     */
    IMPORT_C void AddObserverL( MIRSystemEventObserver* aSystemObserver );
    
    /**
    * Removes a system event observer
    *
    * @param aSystemObserver The observer to be removed
    */
    IMPORT_C void RemoveObserver( MIRSystemEventObserver* aSystemObserver );

    /**
    * Returns data network coverage state (wlan or mobile).
    *
    * @return ETrue if there is network coverage
    */
    IMPORT_C TBool IsNetworkCoverage() const;

    /**
    * Returns mobile network coverage state.
    *
    * @return ETrue if there is network coverage
    */
    IMPORT_C TBool IsMobileNetworkCoverage() const;

    /**
    * Returns current call state.
    *
    * @return ETrue if call is active
    */
    IMPORT_C TBool IsCallActive() const;

    /**
    * Returns current audio resources state.
    *
    * @return ETrue if audio resources are available
    */
    IMPORT_C TBool IsAudioResourcesAvailable() const;
    
    /**
     * Returns current headset connection status
     *
     * @return 
     */
    IMPORT_C TBool IsHeadsetConnected() const;
    
    /**
     * Returns the state of disk space
     */
    IMPORT_C TBool IsDiskSpaceBelowCriticalLevel() const;
    
    /**
	 * Returns current Voice UI state.
	 *
	 * @return ETrue if Voice UI is active, EFalse otherwise
	 */
	IMPORT_C TBool IsVoiceUiActive() const;

private:

    CIRSystemEventCollector();

    void ConstructL();    
    
    void NotifyObserversL(TIRSystemEventType aEvent);

// from base class MIRSystemEventDetectorObserver
    void NetworkUpCallbackL();
    void NetworkDownCallbackL();
    void CallActivatedCallbackL();
    void CallDeactivatedCallbackL();
    void AudioResourcesAvailableL();
    void AudioAutoResumeForbiddenL();
    void ErrorCallbackL(TInt aError);
    
// from base class MIRHeadsetEventObserver 
    void HeadsetConnectedCallbackL();
    void HeadsetDisconnectedCallbackL();
    
// from base class MIRDiskSpaceWatcherObserver    
    void NotifyLowDiskSpaceL();

// from base class MIRAudioRoutingObserver  
    void AudioRoutingChangedL(TVROutputDestination aOutputDestination);

private: // data

    /** 
     * Array of system event observers
     */
    RPointerArray<MIRSystemEventObserver> iSystemEventObservers;

    /** 
     * Detects system events
     */
    CIRSystemEventDetector* iSystemEventDetector;

    /** 
     * Detects low disk space 
     */
    CIRDiskSpaceWatcher* iDiskSpaceWatcher;
	
    /** 
     * Observer for accessory events 
     */
    CIRAccessoryObserver* iHeadsetObserver;
    };

#endif // CIRSYSTEMEVENTCOLLECTOR_H
