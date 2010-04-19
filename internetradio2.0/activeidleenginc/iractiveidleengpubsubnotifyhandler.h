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


#ifndef IRACTIVEIDLEENGPUBSUBNOTIFYHANDLER_H
#define IRACTIVEIDLEENGPUBSUBNOTIFYHANDLER_H

#include <e32cmn.h>

/**
* Observer interface for CIRActiveIdleEngPubSub.
* The observer is notified whenever the listened key is changed. If there was an error in getting the new value of the key,
* the observer will be notified of it as well.
*/
NONSHARABLE_CLASS(MIRActiveIdleEngPubSubNotifyHandler)
    {
    public:

        /**
        * Invoked when the listened key is changed.
        * @param    aCategory   The category of the key that changed.
        * @param    aKey        The key that changed.
        * @param    aValue      The new value of the key.
        */
    virtual void HandlePropertyChangeL( const TUid& aCategory, TUint aKey,
    	 TInt aValue ) = 0;
        
        /**
        * Invoked when the listened key is changed.
        * @param    aCategory   The category of the key that changed.
        * @param    aKey        The key that changed.
        * @param    aText       The new value of the key.
        */
    virtual void HandlePropertyChangeL( const TUid& aCategory, TUint aKey,
    	 const TDesC8& aText ) = 0;
        
        /**
        * Invoked when an error occurred while retrieving the new value of the key.
        * @param    aCategory   The category of the key that changed.
        * @param    aKey        The key that changed.
        * @param    aError      The error code.
        */
    virtual void HandlePropertyChangeErrorL( const TUid& aCategory,
     TUint aKey, TInt aError ) = 0;
    };

#endif//IR_ACTIVEIDLEENGPUBSUBNOTIFYHANDLER
