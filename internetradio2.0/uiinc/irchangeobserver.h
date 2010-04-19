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


#ifndef IRCHANGEOBSERVER_H
#define IRCHANGEOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/**
 * MIRChangeObserver
 * Abstract interface for handling property change events.
 */
class MIRChangeObserver
	{
	public:
	/**
	 * This is a callback function which is called when a property value of type int is changed.
	 * @param aCategory UID of Publish And Subscribe category
	 * @param aKey subkey to specify the category event; to be used with Publish And Subscribe
	 * @param aValue the new value
	 */
	 virtual void HandleChangeL(const TUid& aCategory, const TUint aKey, 
	 	const TInt aValue) = 0;
	};


// CLASS DECLARATION

/**
 * CIRObserver Observer class that observes changes of Property values and propogates them further.
 * The class defines a handle to a property, a single data value representing
 * an item of state information.
 *
 * @Code
 *
 * Updates an int value reference in correspondence 
 * with the current Property value
 * CIRObserver::GetValue(TInt& aValue)
 *
 * @endcode
 */
class CIRObserver:public CActive
	{
public:
	/**
     * Two-phased constructor.
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with Publish And Subscribe
     * @param aPropertyType property type to observe
     */
	static CIRObserver* NewL(MIRChangeObserver& aObserver,const TUid& aCategory, 
		const TUint aKey,const RProperty::TType aPropertyType);
		
	/**
     * Destructor.
     */	
	~CIRObserver();
	
	/**
     * Updates an int value reference in correspondence with the current Property value
     * @param aValue handle to a value which will be updated
     */
	void GetValue(TInt& aValue) const;
	
protected:  // Functions from base classes

    /**
     * From CActive
     * Handles an active object’s request completion event
     */
    void RunL();

    /**
     * From CActive, 
     * Cancels and outstanding request
     */
    void DoCancel();

private:
	/**
     * C++ default constructor.
     * @param aObserver a reference to the observer interface implementer
     * @param aCategory UID of Publish And Subscribe category
     * @param aKey subkey to specify the category event; to be used with Publish And Subscribe
     * @param aPropertyType property type to observe
     **/
	CIRObserver(MIRChangeObserver& aObserver, const TUid& aCategory, 
		const TUint aKey, const RProperty::TType aPropertyType);
		
	/**
     * default Symbian 2nd phase constructor.
     */
	void ConstructL();
	

private:

	//Value of a subscribed category property of type int.
 	TInt                        iValueInt;
	
	// handle to Publish And Subscribe component
    RProperty                   iProperty;

	MIRChangeObserver&  iObserver;
    // UID of Publish And Subscribe category
    TUid                        iCategory;
    // subkey to be used with Publish And Subscribe
    TUint                       iKey;
    //Type of the observed property.
    RProperty::TType            iPropertyType;

	};

#endif // IRCHANGEOBSERVER_H