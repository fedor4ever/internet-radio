/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header for CIRDiskSpaceWatcher
*
*/

#ifndef CIRDISKSPACEWATCHER_H
#define CIRDISKSPACEWATCHER_H

#include <e32base.h>
#include <f32file.h>

#include "irdiskspacewatcherobserver.h"

class CIRDiskSpaceSession;

// Default drive. Used to save for example settings and cache files.
const TDriveNumber KVRDefaultDrive = EDriveC;

/**
 *  CIRDiskSpaceWatcher. Watcher of disk space on defined disks.
 *
 *  Observes a disk and notifies the observer if the supplied critical disk 
 *  space limit is reached. 
 */
NONSHARABLE_CLASS( CIRDiskSpaceWatcher ): public CBase, public MIRDiskSpaceWatcherObserver
    {
    
public:  // Methods

    /**
     * Static constructor.     
     * 
     * @param aCriticalLevel, critical disk space level
     * @param aDiskSpaceWatcherObserver observer of disk space status     
     */
    static CIRDiskSpaceWatcher* NewL( const TInt64& aCriticalLevel, 
        MIRDiskSpaceWatcherObserver& aDiskSpaceWatcherObserver );
    
    /**
     * Destructor.
     */
    virtual ~CIRDiskSpaceWatcher();

    /**
     * IsBelowCriticalLevel Returns whether or disk defined by parameter 
     * contains less than the critical level free disk space.
     * Please note that StartObservingDriveL must have been called 
     * before calling this. Otherwise method will return EFalse whetever
     * disk space is below critical level or not.
     *  
     * @param aDriveNumber drive which space is checked
     * @return ETrue if there is less free disk space than the critical 
     * level, otherwise EFalse.
     */
    TBool IsBelowCriticalLevel( const TDriveNumber& aDriveNumber ) const;
                        
    /**
     * Starts observing space of drive which is given as parameter
     *
     * @param aDriveNumber drive which will be observed
     */
    void StartObservingDriveL( const TDriveNumber& aDriveNumber );            
            
    /**
     * Stops observing space of drive which is given as parameter.
     * Please note that if drive is observed by some other object 
     * it will be observed until all objects have asked 
     * VRDiskSpaceWatcher to stop observing that drive.
     *
     * @param aDriveNumber drive which observing will be stopped. 
     */
    void StopObservingDrive( const TDriveNumber& aDriveNumber );    
    
    // From base class MIRDiskSpaceWatcherObserver
    /** 
     * From MIRDiskSpaceWatcherObserver
     *
     * @see MIRDiskSpaceWatcherObserver::NotifyLowDiskSpaceL()
     */
    
    void NotifyLowDiskSpaceL();
            
protected:  

private: 

    /**
     * C++ default constructor.
     * 
     * @param aCriticalLevel, critical disk space level
     * @param aDiskSpaceWatcherObserver observer of disk space status
     */
    CIRDiskSpaceWatcher( const TInt64& aCriticalLevel, 
        MIRDiskSpaceWatcherObserver& aDiskSpaceWatcherObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data

    /** 
     * Handle to the file system. 
     */
    RFs iFs;

    /** 
     * Critical disk space level. 
     */
    const TInt64 iCriticalLevel;

	/** 
	 * The observer to be notified when critical disk space limit is reached.
	 */
    MIRDiskSpaceWatcherObserver& iObserver;

    /** 
     * Contains active objects which do the actual disk space checking 
     */
    RPointerArray<CIRDiskSpaceSession> iDiskSpaceSessions;
    };

#endif      // IRDISKSPACEWATCHER_H   
