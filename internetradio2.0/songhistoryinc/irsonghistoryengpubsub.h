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
* Description:  Active class notifying metada and channel name changes to SongHistoryDb engine
*
*/

#ifndef CIRSONGHISTORYENGPUBSUB_H
#define CIRSONGHISTORYENGPUBSUB_H

#define KMAXLENGTH 250
#include <e32base.h>
#include <e32property.h>

class MIRSongHistoryEngPubSubNotifyHandler;



/**
* Listens to one specified Publish & Subscribe key, notifying the observer whenever that key is changed.
*/
NONSHARABLE_CLASS(CIRSongHistoryEngPubSub) : public CActive
    {
    public:

        /**
        * Static constructor.
        * @param    aObserver   The observer to be notified of the changes in the listened key.
        * @param    aCategory   The category of the key.
        * @param    aKey        The key to listen to.
        */
    static CIRSongHistoryEngPubSub* NewL( MIRSongHistoryEngPubSubNotifyHandler&
    		aPubSubObserver, const TUid& aCategory, TUint aKey ,
    		const RProperty::TType aPropertyType  );
        /**
        * Static constructor.
        * @param    aObserver   The observer to be notified of the changes in the listened key.
        * @param    aCategory   The category of the key.
        * @param    aKey        The key to listen to.
        */
    static CIRSongHistoryEngPubSub* NewLC( MIRSongHistoryEngPubSubNotifyHandler&
    		aPubSubObserver, const TUid& aCategory, TUint aKey ,
    		const RProperty::TType aPropertyType  );
        /**
        * Destructor.
        */
        ~CIRSongHistoryEngPubSub();


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
    CIRSongHistoryEngPubSub( MIRSongHistoryEngPubSubNotifyHandler& aPubSubObserver,
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
        MIRSongHistoryEngPubSubNotifyHandler& iPubSubObserver;

        /** The category of the key to listen to. */
        TUid iCategory;

        /** The key to listen to. */
        TUint iKey;

        /** The cached value of the key. */
        mutable TBuf8<KMAXLENGTH> iText;


    };

#endif//IR_SONGHISTORYENGPUBSUB
