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
* Description:  Observer of accessory states.
*
*/


#ifndef CIRACCESSORYOBSERVER_H
#define CIRACCESSORYOBSERVER_H

#include <accessoryserver.h>
#include <accessorymode.h>
#include <accpolaccessorymode.h>

class MIRHeadsetEventObserver;

/**
 *  Observer of accessory states.  
 *
 *  Instance of this class observes notifications of headset state changes. 
 *  In its turn it sends notifications to its observer of those changes.
 *
 */
NONSHARABLE_CLASS( CIRAccessoryObserver ): public CActive
    {
public:  

    /**
     * C++ default constructor.
     */
    CIRAccessoryObserver();

    /**
     * Static constructor.
     *
     * @param aAccessoryServer Existing session for The Accessory Server.
     */
    static CIRAccessoryObserver* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CIRAccessoryObserver();

    /**
     * Sets observer. The observer will be notified when headset has been 
     * pressed or headset is connected/disconnected.
      *
     * @param aObserver Observer
     */
    void SetObserver(MIRHeadsetEventObserver* aObserver);

    /**
     * Returns accessory connection state.
     *
     * @return <code>ETrue </code> if accessory is connected, 
     * <code>EFalse</code> otherwise
     */
    TBool IsHeadsetConnected() const;
    
    // from base class CActive
    /**
     * From CActive
     *
     * @see CActive::RunL()
     */    
    void RunL();

    /**
     * From CActive
     *
     * @see CActive::DoCancel()
     */        
    void DoCancel();
    
protected:  

private: 

    /**
     * By default Symbian 2nd phase constructor is private.
     * 
     * @param aAccessoryServer Existing session for The Accessory Server.
     */
    void ConstructL();

public:

protected:

    /** 
     * Instance that gets notifications of the headset state changes 
     */
    MIRHeadsetEventObserver* iObserver;

private:    // Data

    /** 
     * Session for The Accessory Server. 
     */
    RAccessoryServer iAccessoryServer;
    
    /** 
     * Accessory Mode sub-session 
     */            
    RAccessoryMode          iAccessoryModeSession;
    
    
    /** 
     * Accessory Mode structure, with active audio information. 
     */
    TAccPolAccessoryMode    iAccPolAccessoryMode;
    };

#endif      // CIRACCESSORYOBSERVER_H
            

