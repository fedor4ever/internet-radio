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
* Description:  Header for CIRStationsView
*
*/


#ifndef CIRSTATIONSVIEW_H
#define CIRSTATIONSVIEW_H

#include "irbaseview.h"
#include <akntoolbarobserver.h>
#include <aknbutton.h>
#include "MViewsResponseAndErrorObserver.h"
#include "MLogoDownloadObserver.h"

class CIRStationsViewContainer;
class MLogoDownloadObserver;
class MIRActiveNetworkObserver;

/**
 * SearchResults View
 */
class CIRStationsView : public CIRBaseView,
						public MAknToolbarObserver,
						public MViewsResponseAndErrorObserver,
						public MLogoDownloadObserver,
						public MIRActiveNetworkObserver
    {
    
public: //Methods

    /**
     * NewL()
     * Static constructor.
     */
    static CIRStationsView* NewL();
    /**
     * NewLC()
     * Static constructor.
     */
    static CIRStationsView* NewLC();
    
    /**
     *~CIRStationsView()
     * Destructor.
     */
    ~CIRStationsView();

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
	 * void SetStatusPaneTextL()
	 * Sets the title pane of the search results view
	 */
	void SetStatusPaneTextL() const;
	
	/**
	* DynInitMenuPaneL
	* Dynamically initialises a menu pane
	*/
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	
	/**
	* ListenChannelL()
	* Issues a listen request
	*/
	virtual void ListenChannelL(TInt aCurrentIndex);
	
    void OfferToolbarEventL(TInt aCommand );
    
    /**
    * ConstructToolbarL()
    * Constructs a touch toolbar
    */         
   	virtual void ConstructToolbarL();
   	
   	
    /**
     * From CAknView
     *
     * @see CAknView::HandleForegroundEventL( TBool aForeground )
     */
   	void HandleForegroundEventL( TBool aForeground );

    /**
     * From CAknView
     */
	void HandleStatusPaneSizeChange();


	/**
	* void CIRCategoryView::ResponseL()
	* Activates the Stations view after getting the IsdsResponse
	*/
	void ResponseL( CIRIsdsPreset* aPreset = NULL );

	/**
	* ErrorL()
	* Handles the errors from Isds
	*/
	void ErrorL();
	
	/**
	* PresetResponseL
	* Issues a listen request
	*/
	void PresetResponseL(CIRIsdsPreset* aPreset);

	/**
	* AddToFavouritesL()
	* Issues a request for adding to favourites
	*/
	virtual void AddToFavouritesL(TInt aCurrentIndex,TInt aCommand);

	/**
	* DisplayStationInformationL()
	* Issues a request for Station Info view
	*/
	virtual void DisplayStationInformationL(TInt aCurrentIndex,TInt aCommand);
	
	/**
    * PresetLogoDownloadedL()
    * CallBack Function Returning the Preset With Logo
    */
	void PresetLogoDownloadedL(CIRIsdsPreset* aPreset);
	
	/**
    * PresetLogoDownloadError()
    * CallBack Function Returning the Preset With no Logo/error while downloading logo
    */
	void PresetLogoDownloadError(CIRIsdsPreset* aPreset);
	
	/**
	* LogoRequestL()
	* Called by the container of this view
	*/
	void LogoRequestL();
	/**
	* LogoRequestL()
	* Called by the container of this view
	*/
	void AdRequestL();
	

	/**
	* GetFilteredIndex()
	* Returns the filtered Index
	*/
	virtual	TInt GetFilteredIndex();
	
	/**
	* SetLoadingCancelled
	* Sets iLoadingCancelled to aValue
	*/
	void SetLoadingCancelled(TBool aValue);
	
	/**
	* SetActualFilteredItemIndex()
	* sets the original index of the filtered item
	*/
	void SetActualFilteredItemIndex(TInt aIndex);
	
	/**
	* GetActualFilteredItemIndex
	* Gets the actual index of the station
	*/
	TInt GetActualFilteredItemIndex();
	/*
	 * GetStationsViewActualIndex()
	 * returns the actual index
	 */
	TInt GetStationsViewActualIndex();	
	/*
	 * GetFirstLaunchFlag()
	 * returns the whether the view is launching for the first time/
	 * coming back from that has been activated from this view
	 */
	virtual TBool GetFirstLaunchFlag() ;
	/*
	* SetToolbarItems
	* Sets the  touch tool bar items
	*/
   virtual void SetToolbarItems();
   
	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );
    
    //from MIRActiveNetworkObserver
	/**
	 * Notifies all observers whose network request is active
	 * to reissue the request  
	 * NotifyActiveNetworkObserversL()
	 */	
   	 virtual void NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent);
   	 
	 /**
	 * Notifies all observers whose network request is active
	 * to reset the pending request status  
	 * ResetPendingRequests()
	 */	
	 void ResetPendingRequests(TBool aValue);
protected:  //Methods

    // from base class CAknView
    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
    			 const TDesC8& aCustomMessage );

	/**
     * From CAknView
     *
     * @see CAknView::DoDeactivate()
     */
	void DoDeactivate();

	
public: //Methods

    /**
     * ConstructL()
     * 2nd phase constructor
     */
    void ConstructL();

	/**
     * CIRStationsView()
     * C++ default constructor.
     */
    CIRStationsView();
    
private:
	
   /*
	* iLoadingCancelled
	* Data whether loading has been cancelled
	*/
	TBool iLoadingCancelled;

	/*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	
    /* iListenRequest
	 * Checks whether request is for listen
	 */
	TBool iListenRequest;
  
	/*
	 * iCallRequestPending
	 * Indicates whether delayed activation of command is required (Call Handling)
	 */
	TBool iCallRequestPending;
	TBool iToolbarVisible;
	TBool iCommandState;

	TInt iIndex;
	TInt iFilteredIndex;

	/*
	 * iCommand
	 * Command to be executed (view activation) after Preset downloads
	 */
	TInt iCommand;

	TBool iFirstTimeLaunchFlag;
	
	/**
	* iAddToFav
	* Boolean value to determine if AddToFavorites was issued.
	*/
	TBool iAddToFav;
	
	
	/**
	* iStationDetails
	* Boolean value to determine if StationDetails request was issued.
	*/
	TBool iStationDetails;

public:
	/** 
	 * iContainer
	 * Container for this view. 
	 */
	CIRStationsViewContainer* iContainer;

	
	
	/**
	 * iResultsReqMade
	 * boolean value to know whether the results(topstations/list of channels)
	 * request has made to isds or not
	 */
	TBool iResultsReqMade;
    };

#endif      // CIRSTATIONSVIEW_H
            

