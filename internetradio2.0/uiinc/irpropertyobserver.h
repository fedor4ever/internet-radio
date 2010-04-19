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


#ifndef CIRPROPERTYOBSERVER_H
#define CIRPROPERTYOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/** 
 * Defines text property with maximum size, that is RProperty::
 * KKMaxPropertySize bytes. 
 */
typedef TBuf8<RProperty::KMaxPropertySize> TIRTextProperty;

/**
 * MIRPropertyChangeObserver
 *
 * Abstract interface for handling property change events.
 */
class MIRPropertyChangeObserver
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
     * type text is changed.
     *
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aValue the new value
     **/
    /*lint -save -e1053*/
    virtual void HandlePropertyChangeL( 
        const TUid& aCategory, 
        const TUint aKey, 
        const TIRTextProperty& aValue ) = 0;
    /*lint -restore*/
    
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
 class CIRPropertyObserver : public CActive
	{  
	
 public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with 
     * Publish And Subscribe
     * @param aPropertyType property type to observe
     */
    IMPORT_C static CIRPropertyObserver* NewL(
        MIRPropertyChangeObserver& aObserver, 
        const TUid& aCategory, 
        const TUint aKey, 
        const RProperty::TType aPropertyType);

    /**
     * Destructor.
     */
    ~CIRPropertyObserver();
    
    /**
     * Updates an int value reference in correspondence with the current 
     * Property value
     *
     * @param aValue handle to a value which will be updated
     **/	
	IMPORT_C void GetValue( TInt& aValue ) const;
	
	/**
     * Updates a text value reference in correspondence with the current 
     * Property value
     *
     * @param aValue handle to a value which will be updated
     */	
    /*lint -save -e1053*/
	IMPORT_C void GetValue( TIRTextProperty& aValue ) const;
    /*lint -restore*/
	
	/**
	*Subscription of listened P&S keys
	*/	
    void Subscribe();

		
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
	CIRPropertyObserver(
	    MIRPropertyChangeObserver& aObserver, 
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


private:  

 	/** 
 	 * Value of a subscribed category property of type int. 
 	 */
     TInt iValueInt;
 	
 	/** 
 	 * Value of a subscribed category property of type text. 
 	 */
     TIRTextProperty iValueText;
 	
    /** 
     * Handle to Publish And Subscribe component
     */ 
    RProperty iProperty;
    
    /**
     * Observer to be notified when particular Property value has changed
     */
    MIRPropertyChangeObserver& iObserver;
    
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
    RProperty::TType iPropertyType;
    
	};

#endif  // CPROPERTYOBSERVER_H

