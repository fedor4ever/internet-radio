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
* Description:  Header file for CIRControlEventObserver
*
*/


#ifndef CIRCONTROLEVENTOBSERVER_H
#define CIRCONTROLEVENTOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "irui.h"

#define KMAXLENGTH 256


NONSHARABLE_CLASS( CIRControlEventObserver ) : public CActive
	{  
	
 public:

    /**
     * Property type. Cannot use RProperty::TType because EByteArray == EText.
     */
    enum TVRPropertyType
    	{
    	EVRPropertyInt, // Integral property type.
    	EVRPropertyByteArray, // Byte-array (8 bit), max size 512 bytes
    	EVRPropertyText // Text (16 bit), max size 512 bytes
    	};

    /**
     * Two-phased constructor.
     *
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aPropertyType property type to observe
     */
    IMPORT_C static CIRControlEventObserver* NewL(
        CIRUi& aObserver, 
        const TUid& aCategory, 
        const TUint aKey, 
        const RProperty::TType aPropertyType);

    /**
     * Destructor.
     */
    ~CIRControlEventObserver();
        
    /**
     * Subscribes the object to listen to the specified key. Does nothing if already active.
     * Must be called after construction, or after calling Cancel().
     */
    IMPORT_C void SecondConstructL();
    
 	
private:

    /**
     * C++ default constructor overload.
     * Two-phased constructor.
     *
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aPropertyType property type to observe
     */
	CIRControlEventObserver(
	    CIRUi& aObserver, 
	    const TUid& aCategory, 
	    const TUint aKey, 
	    const RProperty::TType aPropertyType);

    /**
     * By default Symbian 2nd phase constructor is private.
     **/
    void ConstructL();

protected:  
    
    // from base class Cactive
    /**
     * From CActive
     *
     * @see CActive::RunL()
     */
    void RunL();

    /**
     * From CActive, 
     * 
     * @see CActive::DoCancel()
     */
    void DoCancel();

private: // data

 
    /**
     * Observer to be notified when particular Property value has changed
     */
    CIRUi& iObserver;
    
    /** 
     * UID of Publish And Subscribe category
     */
    TUid iCategory;
    
    /**
     * Subkey to be used with Publish And Subscribe
     */
    TUint iKey;
    
    RProperty iProperty;

    RProperty::TType iPropertyType;

 	/** The cached value of the key. */
        mutable TBuf8<KMAXLENGTH> iText;
        TInt iValue;
	};

#endif  // CIRCONTROLEVENTOBSERVER_H

