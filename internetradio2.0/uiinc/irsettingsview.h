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
* Description:  View class for Settings
*
*/


#ifndef C_CIRSETTINGSVIEW_H
#define C_CIRSETTINGSVIEW_H

// Need to be changed.

#include "irsettingsviewbase.h"
#include "irsystemeventobserver.h"
#include "irbaseview.h"


/**
 * Implementation of the settings view
 * CVRSettingsViewBase needs to be changed to CIRSettingsViewBase
 */

NONSHARABLE_CLASS( CIRSettingsView ) : public CIRSettingsViewBase,  public MIRSystemEventObserver

    {
public:

	 /**
     * NewL()
     * Static constructor.
     */
    static CIRSettingsView* NewL();
    
    /**
     * Two-phased constructor.
     * @param aShowRegionItem ETrue if region setting item is to be shown, otherwise EFalse
     * @return  The newly created object.
     */
    static CIRSettingsView* NewLC();
    
    /**
     * Destructor.
     */
    ~CIRSettingsView();

    // from base class CAknView
    
    /* HandleCommandL()
	 * Command Handler
	 */
    void HandleCommandL( TInt aCommand );
    
    /* HandleForegroundEventL()
	 *  Handles changes in keyboard focus when an application switches to foreground
	 */
    void HandleForegroundEventL( TBool aForeground );
    
    /* DynInitMenuPaneL()
 	 * Dynamically initialises a menu pane
 	 */
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

private:

    /**
     * Constructor.
     */
    CIRSettingsView();

    /**
     * Second-phase constructor.
     *
     * @param aShowRegionItem ETrue if region setting item is to be shown
     */
    void ConstructL();

// from base class CIRSettingsViewBase

	/* CreateContainerL()
 	 * Creates a new container
 	 */
    void CreateContainerL();
    
// from base class CAknView

	/* Id()
 	 * Returns the Id of the view
 	 */
    TUid Id() const;
    
    /* DoActivateL()
 	 * Activates the view
 	 */
    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
    			 const TDesC8& aCustomMessage );
    			 
     /* DoDeactivate()
 	  * Deactivates the view
 	  */
    void DoDeactivate();

// from base class MIRSystemEventObserver

	/* HandleSystemEventL()
 	 * Handles the system events
 	 */
	 void HandleSystemEventL(TIRSystemEventType aEventType);
	 
	 /* HandleViewStackL()
 	  * handles the view stack
 	  */
	 void HandleViewStackL( );

    };

#endif // C_CIRSETTINGSVIEW_H 
