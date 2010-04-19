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
* Description:  Base class for the Settings View
*
*/


#ifndef C_CIRSETTINGSVIEWBASE_H
#define C_CIRSETTINGSVIEWBASE_H

#include <coneresloader.h>
#include <gsplugininterface.h>


class CIRCommonSettingsContainer;

/**
 * Implements the common settings view for internet Radio.
 * Inherited from the general settings plugin interface so that 
 * common behaviour can be shared between General Settings View and 
 * internet Radio's own settings view.
 */

  class CIRSettingsViewBase : public CGSPluginInterface
    {

public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CIRSettingsViewBase();
    
    /**
     * ResolveDriveL()
     * Static version of ResolveDriveL.
     */
    IMPORT_C void ResolveDriveL( const RFs& aFs, TFileName& aFileName );
        
     /**
     * SetTitleL()
     * Sets the Title of the View
     */

    IMPORT_C void SetTitleL( TInt aResourceId );
    
        
     /**
     * HandleForegroundEventL()
     * Handles changes in keyboard focus when an application switches to 
     * foreground
     */

    IMPORT_C void HandleForegroundEventL( TBool aForeground );

protected:

    /**
     * Constructor.
     */
    IMPORT_C CIRSettingsViewBase();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void BaseConstructL();

    /**
     * Loads resource file with given resource loader
     *
     * @param aResourceLoader A reference to resource loader
     * @param aFileName The resource file to be opened, no path included
     */
    IMPORT_C void LoadResourcesL( RConeResourceLoader& aResourceLoader, const TDesC& aFileName );
    
// from base class CAknView

    
     /**
     * DoActivateL()
     * View activation
     */

    IMPORT_C virtual void DoActivateL( const TVwsViewId& aPrevViewId,
    		 TUid aCustomMessageId, const TDesC8& aCustomMessage );
    		 
    		     
     /**
     * DoDeactivate()
     * View Deactivation
     */

    IMPORT_C virtual void DoDeactivate();
    
        
     /**
     * HandleCommandL()
     * Command Handler
     */

    IMPORT_C virtual void HandleCommandL( TInt aCommand );
    
    // from base class CGSPluginInterface
    
        
     /**
     * GetCaptionL()
     * Gets the caption
     */

    IMPORT_C virtual void GetCaptionL( TDes& aCaption ) const;

   
private:

    /**
     * DeleteContainer()
     * Deletes the container and removes it from the view stack.
     */
    void DeleteContainer();

    /**
     * CreateContainerL()
     * Constructs new container
     */
    virtual void CreateContainerL() = 0;
    
    

protected:

    
     /**
     * iContainer
     * Static version of ResolveDriveL.
     */

	CIRCommonSettingsContainer* iContainer;
    
private:

   
    /** 
     * iSettingsBaseResourceLoader
     * The resource loader for the resource file.
     */
    RConeResourceLoader iSettingsBaseResourceLoader;

	/** 
	 * iPrevViewUid
	 * The UID of the view that activated this one. 
	 */
    TUid iPrevViewUid;
    };

#endif // C_CIRSETTINGSVIEWBASE_H 
