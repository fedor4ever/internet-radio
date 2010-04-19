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

#ifndef IRACTIVEIDLEENGPUBSUB_H
#define IRACTIVEIDLEENGPUBSUB_H

#include <e32base.h>
#include <e32property.h>

class MIRActiveIdleEngPubSubNotifyHandler;

#define KMAXLENGTH 256

/**
* Listens to one specified Publish & Subscribe key, notifying the observer whenever that key is changed.
*/
NONSHARABLE_CLASS(CIRActiveIdleEngPubSub) : public CActive
    {
    public:
        
        /**
        * Static constructor.
        * @param    aObserver   The observer to be notified of the changes in the listened key.
        * @param    aCategory   The category of the key.
        * @param    aKey        The key to listen to.
        */
    static CIRActiveIdleEngPubSub* NewL( MIRActiveIdleEngPubSubNotifyHandler& 
    		aObserver, const TUid& aCategory, TUint aKey , 
    		const RProperty::TType aPropertyType  );
        
        /**
        * Destructor.
        */
        ~CIRActiveIdleEngPubSub();
        
        /**
        * Construction finalizer.
        * Subscribes the object to listen to the specified key.
        */
       	void SecondConstructL();
        
    protected: // From CActive

        /**
    *void RunL()
    *The function is called by the active scheduler
    *when a request completion event occurs,
        */
        void RunL();
        
        /**
    *void DoCancel()
    *Invoked when the active object is cancelled.
        */
        void DoCancel();
        
    private:

        /**
        * Constructor.    
        * @param    aObserver   The observer to be notified of the changes in the listened key.
        * @param    aCategory   The category of the key.
        * @param    aKey        The key to listen to.
        */
    CIRActiveIdleEngPubSub( MIRActiveIdleEngPubSubNotifyHandler& aObserver, 
    	const TUid& aCategory, TUint aKey, const RProperty::TType aPropertyType );
        
        /**
        * Second-phase constructor.
        */
        void ConstructL();
		
    protected:

        /** Handle to the property. */
        RProperty iProperty;
        RProperty::TType iPropertyType;
        /** The observer to be notified of the changes in the listened key. */
        MIRActiveIdleEngPubSubNotifyHandler& iObserver;
        /** The category of the key to listen to. */
        TUid iCategory;
        /** The key to listen to. */
        TUint iKey;
        /** The cached value of the key. */
        mutable TBuf8<KMAXLENGTH> iText;
        TInt iValue;
    };

#endif//IRACTIVEIDLEENGPUBSUB_H
