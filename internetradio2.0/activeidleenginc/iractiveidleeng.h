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
* Description:  ?Description
*
*/

#ifndef IRACTIVEIDLEENG_H
#define IRACTIVEIDLEENG_H

#include "iractiveidleengpubsubnotifyhandler.h"

class CIRActiveIdleEngPubSub;
class MIRActiveIdleEngNotifyHandler;

/**
* Engine component for Internet Radio Active Idle plugin.
*
* This class is responsible for listening to relevant P&S keys and notifying the UI code whenever the listened keys change.
* The actual listening is done by multiple CIRActiveIdleStatusObserver objects.
*/
NONSHARABLE_CLASS(CIRActiveIdleEng) : public CBase, 
	public MIRActiveIdleEngPubSubNotifyHandler
    {
    public:
        
        /**
        * Static constructor.
        * @param    aObserver   The observer to be notified of the changes in the keys.
        * @return   The newly created CIRActiveIdleEng object.
        */
    IMPORT_C static CIRActiveIdleEng* NewL( 
    	MIRActiveIdleEngNotifyHandler& aObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CIRActiveIdleEng();
        
        /**
        * Construction finalizer.
        * Must be called after CIRActiveIdleEng::NewL. 
        */
        IMPORT_C void SecondConstructL();
        
    protected: // From MIRActiveIdleStatusObserver
        /**
    * Invoked when an error occurred while retrieving the new 
    * value of the key.
    * @param    aCategory   The category of the key that changed.
    * @param    aKey        The key that changed.
    * @param    aError      The error code.
        */
    void HandlePropertyChangeErrorL( const TUid& aCategory, TUint aKey, 
    	TInt aError );
        
        /**
    * Invoked when the listened key is changed.
    * @param    aCategory   The category of the key that changed.
    * @param    aKey        The key that changed.
    * @param    aText       The new value of the key.
        */
    void HandlePropertyChangeL( const TUid& aCategory, TUint aKey,
    	const TDesC8& aText );
        
        /**
    * Invoked when the listened key is changed.
    * @param    aCategory   The category of the key that changed.
    * @param    aKey        The key that changed.
    * @param    aValue      The new value of the key.
        */
    void HandlePropertyChangeL( const TUid& aCategory, TUint aKey, 
    	TInt aValue );

	private:
    
        /**
        * Constructor.
    * @param    aObserver   The observer to be notified of the changes 
    * in the keys.
        */
        CIRActiveIdleEng( MIRActiveIdleEngNotifyHandler& aObserver );
    
        /**
        * Second-phase constructor.
        */
        void ConstructL();        
        
    protected:
    
        /** The observer to be notified of the changes in the listened keys. */
        MIRActiveIdleEngNotifyHandler& iObserver;
        /** Array of status observers. */
        RPointerArray<CIRActiveIdleEngPubSub> iPropertyObserverArray;
        
        TInt		iVolume;

    };

#endif//IRACTIVEIDLEENG_H
