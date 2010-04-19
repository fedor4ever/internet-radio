/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef IRQDISKSPACEOBSERVER_H_
#define IRQDISKSPACEOBSERVER_H_

#include <e32base.h>  // CActive
#include <f32file.h>  // RFs

class MIRQDiskSpaceObserver;

/**
 * Defines disk space observer. 
 *
 * Observers defined disk and notifies observer if disk space drops below
 * certain level.
 */
class  IRQDiskSpaceObserver  : public CActive
    {

public:

    /**
     * Static constructor 
     *
     * @param aObserver Observer who is interested if disk 
     * space drops below certain level.
     */
    static IRQDiskSpaceObserver* NewL(MIRQDiskSpaceObserver* aObserver);

    /**
     * Destructor
     */
    ~IRQDiskSpaceObserver();
    
    /**
     * Start Monitoring 
     */
    void start(const TInt64 aCriticalLevel);

    /**
     * Tells observed critical level.
     *
     * @return critical level which is been observed by this session.
     */
    TInt64 criticalLevel() const;
    
    /**
     * Returns whether or disk defined by parameter 
     * contains less than the critical level free disk space.
     */
    bool isBelowCriticalLevel(const TInt64 aCriticalLevel) const;
       
protected:
    /**
     * From CActive 
     */
    void DoCancel();
    void RunL();

private:

    /**
     * C++ constructor.  
     */
    IRQDiskSpaceObserver(MIRQDiskSpaceObserver* aObserver);

    /**
     * Second phase constructor
     */
    void ConstructL();
    
    void run();
    
private: 

    /** 
     * The observer to be notified when critical disk space limit is reached. 
     */
    MIRQDiskSpaceObserver* iObserver;

    /** 
     * Critical disk space level. 
     */
    TInt64 iCriticalLevel;

    /** 
     * Handle to the file system. 
     */
    RFs iFs;
};


#endif // IRQDISKSPACEOBSERVER_H_
