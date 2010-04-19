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
* Description:  Header for CIRSearchResultsView
*
*/


#ifndef CIRSEARCHRESULTSVIEW_H
#define CIRSEARCHRESULTSVIEW_H

#include "irbaseview.h"
#include <akntoolbarobserver.h>
#include <aknbutton.h>
#include "MViewsResponseAndErrorObserver.h"
#include "MLogoDownloadObserver.h"


class CIRSearchResultsViewContainer;
class CAknButton;
class CIRAddManuallyStationForm;
class MLogoDownloadObserver;
class MIRActiveNetworkObserver;

/**
 * SearchResults View
 */
class CIRSearchResultsView : public CIRBaseView,
							 public MAknToolbarObserver,
							 public MViewsResponseAndErrorObserver,
							 public MLogoDownloadObserver,
							 public MIRActiveNetworkObserver
    {
    
public: //Methods

	/**
     * NewLC()
     * Static constructor.
     */
    static CIRSearchResultsView* NewL();

    /**
     * NewLC()
     * Static constructor.
     */
    static CIRSearchResultsView* NewLC();
    
    /**
     *~CIRSearchResultsView()
     * Destructor.
     */
    ~CIRSearchResultsView();

    // from base class CAknView
	/**
	 * From CAknView
	 *
	 * @see CAknView::Id() const
	 */
    TUid Id() const;

	/**
	 * From CAknView
	 *
	 * @see CAknView::HandleCommandL( TInt aCommand )
	 */
	void HandleCommandL( TInt aCommand );
	
    /**
	* HandleStatusPaneSizeChange()
	* Event handler for status pane size changes.
	*/ 
	void HandleStatusPaneSizeChange();
	
	/**
	 * void SetStatusPaneTextL()
	 * Sets the title pane of the search results view
	 */
	 void SetStatusPaneTextL();
	 
	 /**
	  * DynInitMenuPaneL()
 	  * Dynamically initialises a menu pane
	  */
	void DynInitMenuPaneL(TInt /*aResourceId*/,CEikMenuPane* aMenuPane);
                                                
	//For Touch Toolbar
	
    /**
	* OfferToolbarEventL
	* gets toolbar events
	*/ 
    void OfferToolbarEventL(
                TInt aCommand );
                
    /**
	* ConstructToolbarL
	* constructs a toolbar
	*/  
   	void ConstructToolbarL();
  	
   	/**
	* HandleForegroundEventL()
	* Foreground event handling function.
	*/
   	void HandleForegroundEventL( TBool aForeground );
   	
	/**
	* DialogDismissedL()
	* Called by CAknwaitdialog when a dialog is dismissed
	*/
	
	/**
	* void ResponseL()
	* Activates the Stations view after getting the IsdsResponse
	*/
	void ResponseL( CIRIsdsPreset* aPreset = NULL );
	
	/**
	* void PresetResponseL()
	* Activates the Stations view after getting the IsdsResponse
	*/
	void PresetResponseL(CIRIsdsPreset* aPreset);
	
	/**
	* AddToFavouritesL()
	* Adds the selected channel to favorites
	*/
	void AddToFavouritesL(TInt aCurrentIndex,TInt aCommand);
	
	/**
	* ListenChannelL()
	* Request for listening to a channel
	*/
	void ListenChannelL(TInt aCurrentIndex);
	
	/**
	* ErrorL()
	* Handles error Conditions
	*/
	void ErrorL();
   /*
    * ShowQueryDialogL()
    * Implemented the function for search again
    */
	void ShowQueryDialogL(const TDesC& aTitle, TDes& aBuffer);
	/**
    * PresetLogoDownloadedL()
    * CallBack Function Returning the Preset With Logo
    */
	void PresetLogoDownloadedL(CIRIsdsPreset* aPreset);
	
	/**
    * PresetLogoDownloadError()
    * CallBack Function Returning the Preset With no Logo/Error while downloading
    */
	void PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/);
	
	/**
	* LogoRequestL()
	* Called by the container of this view
	*/
	void LogoRequestL();
	
	/**
	* AdRequestL()
	* Called by the container of this view
	*/
	void AdRequestL();
	/*
	 * GetActualIndex()
	 * returns the actual index
	 */
	TInt GetActualIndex();	
	
	/*
	 * CIRSearchResultsView::SetActualFilteredItemIndex ()
	 * Sets the listen is from favourites or not
	 */
	void SetActualFilteredItemIndex(TInt aIndex);
	
	/*
	 * CIRSearchResultsView::GetFilteredIndex ()
	 * returns the iFilteredIndex
	 */
	TInt GetFilteredIndex();
	/*
	 * GetFirstLaunchFlag()
	 * returns the whether the view is launching for the first time/
	 * coming back from that has been activated from this view
	 */
	TBool GetFirstLaunchFlag() ;
	/*
	* SetToolbarItems
	* Sets the  touch tool bar items
	*/
    void SetToolbarItems();
    //from MIRActiveNetworkObserver
	/**
	 * Notifies all observers whose network request is active
	 * to reissue the request  
	 * NotifyActiveNetworkObserversL()
	 */	
   	 void NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent);
   	 
   	 
	 /**
	 * Notifies all observers whose network request is active
	 * to reset the pending request status  
	 * ResetPendingRequests()
	 */	
	 void ResetPendingRequests(TBool aValue);
   	 
   	 
 	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );
protected:  //Methods

    // from base class CAknView
    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
        TUid /*aCustomMessageId*/, 
        const TDesC8& aCustomMessage );

	/**
     * From CAknView
     *
     * @see CAknView::DoDeactivate()
     */
	void DoDeactivate();

	
private: //Methods

    /**
     * ConstructL()
     * 2nd phase constructor
     */
    void ConstructL();
    
	/**
     * CIRSearchResultsView()
     * C++ default constructor.
     */
    CIRSearchResultsView();

private:    // Data

	/** 
	 * iContainer
	 * Container for this view. 
	 */
	CIRSearchResultsViewContainer* iContainer;

	/** 
	 * iToolbarVisible
	 * Boolean value to determine if the toolbar is visible.
	 */
    TBool iToolbarVisible;
    
	
	/**
	* iIsSearchAgain
	* to track whether the search again option is clicked or not
	*/
	TBool iIsSearchAgain;
  	

	/*
	 * Returns the actual index of the listbox
	 */
	TInt iIndex;
	
	/*
	 * Returns the filtered index of the listbox
	 */
	TInt iFilteredIndex;
	TBool iFirstTimeLaunchFlag;
	/*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	
	
	/**
	* iAddToFav
	* Boolean value to determine if AddToFavorites was issued.
	*/
	TBool iAddToFav;
	
	/**
	* iListenRequest
	* Boolean value to determine if ListenRequest was issued.
	*/
	TBool iListenRequest;
	
	/*
	 * iSearchValue
	 * Used to determine if search is issued
	 */
	TInt iSearchValue;
	/*
	 * iCallRequestPending
	 * Indicates whether delayed activation of command is required (Call Handling)
	 */
	TBool iCallRequestPending;
	/*
	 * iCommand
	 * Command to be executed (view activation) after Preset downloads
	 */
	TInt iCommand;
public:
	
	/**
	 * iSearchReqMade
	 * boolean value to know whether the search request
	 * has made to isds or not
	 */
	TBool iSearchReqMade;

	
    };

#endif      // CIRSEARCHRESULTSVIEW_H
            

