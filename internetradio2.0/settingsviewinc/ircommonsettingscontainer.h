/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for the settings UI
*
*/


#ifndef C_CIRCOMMONSETTINGSCONTAINER_H
#define C_CIRCOMMONSETTINGSCONTAINER_H

#include <aknsettingitemlist.h>
#include "irsettings.h"
class CIRNetworkController;


//Added for ALR/SNAP
enum TUserSelection
   {
    EUserSelectionAlwaysAsk = 0,
    EUserSelectionDefaultConnection,
    EUserSelectionDestination,
    EUserSelectionConnectionMethod
    };

/**
 * The container that holds all the setting items.
 */
class CIRCommonSettingsContainer : public CAknSettingItemList
    {
public:  // Methods

    /**
     * Two-phased constructor.
     *
     * @param   aModel          The model that provides writing and reading of the settings.
     */
    IMPORT_C static CIRCommonSettingsContainer* NewL( );

    /**
     * Destructor.
     */
    IMPORT_C ~CIRCommonSettingsContainer();

    /**
     * Starts editing the item at the current index.
     */
    void EditCurrentItemL( TBool aCalledFromMenu );
    
    /**
     * GetCurrentSelection()
     * Getter function for the currently selected item in the list
     */    
    TInt GetCurrentSelection();
    
    /**
     * UpdateStatusL()
     * Fetches the new settings values from iIRSettings and updates the list
     */  
    void UpdateStatusL();
    
    /**
     * GetUserSelectionL()
     * Queries the Settings and returns the user selected IAP/SNAP
     */
    
    /**
     * SetAccessPointDetailsL()
     * Sets the details of the chosen IAP in cenrep
     */
    void SetAccessPointDetailsL(TUint aId);
    
    /**
     * SetListBoxTextL()
     * Sets the text of the Network Selection in the Settings UI
     */
    void SetListBoxTextL();

protected:

    /**
     * Constructor.
     *
     * @param   aModel      The model that provides writing and reading of the settings.
     */
    IMPORT_C CIRCommonSettingsContainer();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void BaseConstructL();

// from base class CAknSettingItemList.

	/**
     * CreateSettingItemL()
     * Creates a setting item based upon the user id aSettingId
     */
    IMPORT_C virtual CAknSettingItem* CreateSettingItemL( TInt aSettingId );
    
    /**
     * EditItemL()
     * Launches the setting page for the current item 
     */
    IMPORT_C virtual void EditItemL( TInt aIndex, TBool aCalledFromMenu );

// from base class CCoeControl

	/**
     * GetHelpContext()
     * Gets the control's help context
     */
    IMPORT_C void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    /**
     * SizeChanged()
     * Responds to changes to the size and position of the contents of this control.
     */
    IMPORT_C void SizeChanged();
    
    
    /**
     * ShowConnectionSettingsUiL()
     * Displays the connection settings ui
     */
    void ShowConnectionSettingsUiL();

private:

    /**
     * iIRSettings
     * An instance of the CIRSettings class
     */
    CIRSettings* iIRSettings;
    
    /**
     * iAccessPointIndex
     * Index of the AccessPoint setting
     */
    TInt iAccessPointIndex;
    
    /**
     * iWifiBitRateIndex
     * Index of the WifiBitRate setting
     */
    TInt iWifiBitRateIndex;
    
    /**
     * i3GBitRateIndex
     * Index of the 3GBitRate setting
     */
    TInt i3GBitRateIndex;
    
    /**
     * iGprsBitRateIndex
     * Index of the GprsBitRate setting
     */
    TInt iGprsBitRateIndex;
    
    /**
     * iNetworkController
     * An instance of the CIRNetworkController class
     */
	CIRNetworkController* iNetworkController;
	
	/** 
	 * iIapArray
	 * An array of Available IAP IDs 
	 */
	const CDesCArray* iIapArray;

	/** 
	 * iDestinationSelected
	 * A Boolean value to indicate if the selection is a Destination
	 */
	TBool iDestinationSelected;
    };

#endif // C_CIRCOMMONSETTINGSCONTAINER_H
