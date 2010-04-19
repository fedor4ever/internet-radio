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
* Description:  
*
*/


#ifndef C_CIRSETTINGSCONTAINER_H
#define C_CIRSETTINGSCONTAINER_H

//#include "vrcommonsettingscontainer.h"
#include "ircommonsettingscontainer.h"

class CIRSettingsView;

// Need to be changed.

/**
 * The container that holds all the setting items.
 */
NONSHARABLE_CLASS( CIRSettingsContainer ) : public CIRCommonSettingsContainer
    {
public:  // Methods

    /**
     * Two-phased constructor.
     *
     * @param   aModel          The model that provides writing and reading of the settings.
     */
    static CIRSettingsContainer* NewL(CIRSettingsView& aSettingsView);
    
    /**
     * Two-phased constructor.
     *
     * @param   aModel          The model that provides writing and reading of the settings.
     */
    static CIRSettingsContainer* NewLC(CIRSettingsView& aSettingsView);

    /**
     * Destructor.
     */
    ~CIRSettingsContainer();

    /**
    * Fades/unfades this container.
    *
    * @param    aFaded          ETrue if container is to be faded, otherwise EFalse
    */
    void SetFaded(TBool aFaded) const;

private:

    /**
     * Constructor.
     *
     * @param   aModel          The model that provides writing and reading of the settings.
     */
    CIRSettingsContainer(CIRSettingsView& aSettingsView);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

// from base class CAknSettingItemList.
    void EditItemL( TInt aIndex, TBool aCalledFromMenu );
    CAknSettingItem* CreateSettingItemL( TInt aSettingId );

// from base class CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private:
	CIRSettingsView& iSettingsView;
    };

#endif // C_CIRSETTINGSCONTAINER_H
