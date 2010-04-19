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
* Description:  Declaration of CIRGeneralSettingsView
*
*/


#ifndef C_CIRGENERALSETTINGSVIEW_H
#define C_CIRGENERALSETTINGSVIEW_H

#include "irsettingsviewbase.h"

/**
 * Implements the general settings plugin interface.
 *
 * Loads the necessary resources and constructs the view and model of the plugin.
 */
NONSHARABLE_CLASS( CIRGeneralSettingsView ) : public CIRSettingsViewBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param   aInitParams     The ECom plugin initialization parameters.
     * @return  The newly created object.
     */
    static CIRGeneralSettingsView* NewL( TAny* aInitParams );
    
    /**
     * NewLC()
     * Static constructor.
     */
    static CIRGeneralSettingsView* NewLC();

    /**
     * Destructor.
     */
    ~CIRGeneralSettingsView();
    
    /**
     * HandleCommandL()
     * Command Handler
     */
    void HandleCommandL( TInt aCommand );
    
    /**
     * DynInitMenuPaneL()
     * Dynamically initialises a menu pane
     */
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);


private:

    /**
     * Constructor.
     */
    CIRGeneralSettingsView();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
// from base class CAknView

	/**
     * Id()
     * returns the Id of the view
     */
    TUid Id() const;
    
    
    /**
     * DoActivateL()
     * Activates the view
     */
    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
    				 const TDesC8& aCustomMessage );
    				 

// from base class CGSPluginInterface

	/** GetCaptionL()
     * Gets the caption of general settings
     */
    void GetCaptionL( TDes& aCaption ) const;
    
    /** PluginProviderCategory()
     * Getter for provider category
     */
    TInt PluginProviderCategory() const;
    
     /** CreateIconL()
     * Creates icon for the general settings
     */
    CGulIcon* CreateIconL( const TUid aIconType );
    
     /** Visible()
     * Getter for general settings view visibility status
     */
    virtual TBool Visible() const;
    
// from base class CVRSettingsViewBase

 	/** CreateContainerL()
     * Constructs new container
     */
    void CreateContainerL();

private:

    /**
     * iGeneralSettingsResourceLoader
     * The resource loader for the resource file.
     */
    RConeResourceLoader iGeneralSettingsResourceLoader;
    
    };

#endif // C_CIRGENERALSETTINGSVIEW_H 
