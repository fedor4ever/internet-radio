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
* Description:  Header for CIRDiskSpaceSession
*
*/

#ifndef C_IRDISKSPACESESSION_H
#define C_IRDISKSPACESESSION_H

#include <e32base.h>
#include <f32file.h>

class MIRDiskSpaceWatcherObserver;

/**
 * Defines disk space observer. 
 *
 * Observers defined disk and notifies observer if disk space drops below
 * certain level.
 */
NONSHARABLE_CLASS( CIRDiskSpaceSession ) : public CActive
    {

public:

    /**
     * Static constructor 
     *
     * @param aDiskSpaceSessionObserver Observer who is interested if disk 
     * space drops below certain level.
     * @param aDriveNumber drive to observe
     * @param aCriticalLevel critical disk space level in bytes. If disk 
     * space falls below this observer is notified.
     */
    static CIRDiskSpaceSession* NewL(MIRDiskSpaceWatcherObserver& 
							         aDiskSpaceSessionObserver, 
							         const TDriveNumber& aDriveNumber, 
							         const TInt64& aCriticalLevel,
							         RFs& aFs );

    /**
     * Destructor
     */
    ~CIRDiskSpaceSession();
            
    /**
     * Tells observed drive.
     *
     * @return Drive number which is been observed by this session.
     */
    TDriveNumber DriveNumber();
        
    /**
     * Sets observer count
     *
     * @param aObserverCount observer count
     */
    void SetObserverCount( TInt aObserverCount );

    /**
     * Returns observer count
     *
     * @return observer count
     */    
    TInt ObserverCount();
    
    /**
     * IsBelowCriticalLevel Returns whether or disk defined by parameter 
     * contains less than the critical level free disk space.
     *
     * @param aDriveNumber drive which space is checked
     * @return ETrue if there is less free disk space than the critical 
     * level, otherwise EFalse.
     */
    TBool IsBelowCriticalLevel( const TDriveNumber& aDriveNumber ) const;
   
protected:

    // from base class CActive
    /**
     * From CActive 
     * 
     * @see CActive::DoCancel()
     */
    void DoCancel();

    /**
     * From CActive 
     *
     * Invoked when the observed disk's free disk space has run 
     * below the supplied critical level.
     * Notifies the observer and regenerates the request to RFs' 
     * NotifyDiskSpace.
     * @see CActive::RunL()
     */
    void RunL();

private:

    /**
     * C++ default constructor. Sets references.
     *
     * @param aDiskSpaceSessionObserver Observer who is interested if disk 
     * space drops below certain level.
     * @param aDriveNumber drive to observe
     * @param aCriticalLevel critical disk space level in bytes. If disk 
     * space falls below this observer is notified.     
     */
    CIRDiskSpaceSession(MIRDiskSpaceWatcherObserver& 
				        aDiskSpaceSessionObserver, 
				        const TDriveNumber& aDriveNumber, 
				        const TInt64& aCriticalLevel,
				        RFs& aFs );

    /**
     * Second phase constructor
     */
    void ConstructL();

private: 

	/** 
	 * The observer to be notified when critical disk space limit is reached. 
	 */
    MIRDiskSpaceWatcherObserver& iObserver;

    /** 
     * Drive to be observed. 
     */
    const TDriveNumber iDriveNumber;

    /** 
     * Critical disk space level. 
     */
    const TInt64 iCriticalLevel;

    /** 
     * Handle to the file system. 
     */
    RFs& iFs;

    /** 
     * Number of observers of this drive. When observer count drops to 
     * zero drive observation can be stopped.
     */
    TInt iObserverCount;

    };


#endif // C_IRDISKSPACESESSION_H
