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
* Description:  Header file for CIRPropertyObserver2
*
*/


#ifndef CIRPROPERTYOBSERVER2_H
#define CIRPROPERTYOBSERVER2_H

#include <e32base.h>
#include <e32property.h>

/**
 * MIRPropertyChangeObserver2
 *
 * Abstract interface for handling property change events.
 */
class MIRPropertyChangeObserver2
{

public:
 
    /**
     * This is a callback function which is called when a property value of 
     * type int is changed.
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     *
     * @param aValue the new value
     */
    virtual void HandlePropertyChangeL(
        const TUid& aCategory, 
        const TUint aKey, 
        const TInt aValue ) = 0;
    
    /**
     * This is a callback function which is called when a property value of 
     * type byte array is changed.
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aValue the new value
     **/
    virtual void HandlePropertyChangeL( 
        const TUid& aCategory, 
        const TUint aKey, 
        const TDesC8& aValue ) = 0;
    
    /**
     * This is a callback function which is called when a property value of 
     * type text is changed.
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aValue the new value
     **/
    virtual void HandlePropertyChangeL( 
        const TUid& aCategory, 
        const TUint aKey, 
        const TDesC& aValue ) = 0;
    
    /**
     * This is a callback function which is called when a P&S components 
     * returns an error
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event
     * @param aError an error code
     */
    virtual void HandlePropertyChangeErrorL(
        const TUid& aCategory, 
        const TUint aKey, 
        TInt aError) = 0;
    
};



/**
 * Observer class that observes changes of Property values and propogates 
 * them further.
 * The class defines a handle to a property, a single data value representing
 * an item of state information.
 *
 */
NONSHARABLE_CLASS( CIRPropertyObserver2 ) : public CActive
	{  
	
 public:

    /**
     * Property type. Cannot use RProperty::TType because EByteArray == EText.
     */
    enum TIRPropertyType
    	{
    	EIRPropertyInt, // Integral property type.
    	EIRPropertyByteArray, // Byte-array (8 bit), max size 512 bytes
    	EIRPropertyText // Text (16 bit), max size 512 bytes
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
    IMPORT_C static CIRPropertyObserver2* NewL(
        MIRPropertyChangeObserver2& aObserver, 
        const TUid& aCategory, 
        const TUint aKey, 
        const TIRPropertyType aPropertyType);

    /**
     * Destructor.
     */
    ~CIRPropertyObserver2();
        
    /**
     * Subscribes the object to listen to the specified key. Does nothing if already active.
     * Must be called after construction, or after calling Cancel().
     */
    IMPORT_C void ActivateL();
    
    /**
     * Returns current integer property value
     *
     * @return int value
     */	
	IMPORT_C TInt ValueInt() const;
	
	/**
     * Returns current byte array property value
     *
     * @return byte array value
     */	
	IMPORT_C const TDesC8& ValueDes8() const;
	
	/**
     * Returns current text property value
     *
     * @return text value
     */	
	IMPORT_C const TDesC& ValueDes() const;
		
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
	CIRPropertyObserver2(
	    MIRPropertyChangeObserver2& aObserver, 
	    const TUid& aCategory, 
	    const TUint aKey, 
	    const TIRPropertyType aPropertyType);

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
 	 * Value of a subscribed category property of type int. 
 	 */
 	TInt iValueInt;
 	
 	/** 
 	 * Value of a subscribed category property of type byte array. 
 	 */
 	HBufC8* iValueByteArray;
 	
 	/** 
 	 * Value of a subscribed category property of type text. 
 	 */
 	HBufC* iValueText;
 	
    /** 
     * Handle to Publish And Subscribe component
     */ 
    RProperty iProperty;
    
    /**
     * Observer to be notified when particular Property value has changed
     */
    MIRPropertyChangeObserver2& iObserver;
    
    /** 
     * UID of Publish And Subscribe category
     */
    TUid iCategory;
    
    /**
     * Subkey to be used with Publish And Subscribe
     */
    TUint iKey;
    
    /** 
     * Type of the observed property. 
     */
    TIRPropertyType iPropertyType;
    
	};

#endif  // CIRPROPERTYOBSERVER2_H

