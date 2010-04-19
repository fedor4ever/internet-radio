/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CIRBaseView
*
*/


#ifndef C_CIRBASEVIEW_H
#define C_CIRBASEVIEW_H

#include <aknview.h>

#include "irsystemeventobserver.h"


const TInt KIRIconSubCellIndex = 3;
const TInt KAlphanumericStart = 48; //Scan code of '0' key on keypad.
const TInt KAlphanumericEnd = 59;   //Scan code of 'w' key on keypad.
const TInt KAphaUpperCaseStart = 65;
const TInt KAphaUpperCaseEnd = 91;
const TInt KAphaLowerCaseStart = 97;
const TInt KAphaLowerCaseEnd = 122;
const TInt KAknExListFindBoxTextLength = 10;

const TInt KFont = 200;
const TSize KBitmapSize = TSize(59, 59);

class CIRIsdsWrapper;
class CIRNowPlayingWrapper;
class CIRUi;
class CIRAddManuallyStationForm;
class CIRDialogLauncher;
/**
 * Base View
 *
 * Base View, which is used to hold common functionality across different views.
 * At the moment contains only some event handling and some view activation related functionality.
 */
class CIRBaseView :public CAknView,
    public MIRSystemEventObserver
    {
public:

 	/**
     * Available observers to be used in derived classes.
     */
    enum TIRObservers
        {
        EIRNone = 0x00000000,
        EIRSystemEventObserver = 0x00000002
        };

    /**
     * Destructor.
     */
    ~CIRBaseView();

    /**
     * Set the title of the current view. This is a utility method for more easy
     * title setting. SetTitleL has two overloaded methods: one for resource
     * based strings and one for descriptor based strings.
     *
     * @param aResourceid A resource id pointing to a text
     */
    void SetTitleL( TInt aResourceId );
    /**
     * @param aTitleText Descriptor which holds the title text
     */
    void SetTitleL( const TDesC& aTitleText );

    /**
     * Takes care of pushing and poping views for view backtracking
     */
    void HandleViewStackL(TUid aCustomMessageId);

	// from base class CAknView
    void HandleCommandL( TInt aCommand );
    void HandleForegroundEventL( TBool aForeground );
	// from base class MEikMenuObserver

   
    /**
    * GetCurrentFocus
    * Returns Data for the current focus in the Listbox
    */
    TInt GetCurrentFocus();
    
    /**
    * GetCurrentTopItemInstance
    * Returns Data for the top item index in the Listbox
    */
   
    TInt GetCurrentTopItem();
    
    /**
    * SetCurrentFocus
    * Sets Data for the current focus in the Listbox
    */
    void SetCurrentFocus(TInt aCurrentFocus);
    
    /**
    * SetCurrentTopItemInstance
    * Sets Data for the top item index in the Listbox
    */
    void SetCurrentTopItem(TInt aTopItemIndex);
    
	//From the base class MIRSystemEventObserver    
    void HandleSystemEventL(TIRSystemEventType aEventType);


protected:

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aResourceId A resource id for BaseContructL
     */
    void ConstructL( TInt aResourceId );

    /**
     * C++ default constructor.
     */
    CIRBaseView();

    /**
     * Enable event observing.
     *
     * @param aObservers List of observers to be enabled
     */
     void EnableObserverL( TInt aObservers );

    /**
     * Disable event observing.
     *
     * @param aObservers List of observers to be disabled
     */
    void DisableObserver( TInt aObservers );


	// from base class CAknView
    void DoActivateL( const TVwsViewId& aPrevViewId,
                      TUid aCustomMessageId,
                      const TDesC8& aCustomMessage );
    void DoDeactivate();

private:

    /**
     * Saves information about which observers are currently enabled/disabled.
     */
    TInt iObserverEnabled;

public:    //data
    
    CIRUi* iUi;

    /**
     * iIsdsWrapper
     * instance of the IsdsWrapper class
     * To be used by all the views
     */
	CIRIsdsWrapper *iIsdsWrapper;
    /**
     * iNowPlayingWrapper
     * instance of the CIRNowPlayingWrapper class
     * To be used by all the views
     */
	CIRNowPlayingWrapper *iNowPlayingWrapper;
	
	/**
	* iCurrentFocus
	* Data for the current focus in the Listbox
	*/
	TInt iCurrentFocus;
	
	/**
	* iTopItemIndex
	* Data for the top item index in the Listbox
	*/
	TInt iTopItemIndex;
    };

#endif /* C_CIRBASEVIEW_H */
