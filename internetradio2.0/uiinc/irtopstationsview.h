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
* Description:  Header for CIRTopStationsView
*
*/


#ifndef CIRTOPSTATIONSVIEW_H
#define CIRTOPSTATIONSVIEW_H

#include "irbaseview.h"
#include "irstationsview.h"
#include <akntoolbarobserver.h>
#include <aknbutton.h>


class CIRStationsViewContainer;

class MIRActiveNetworkObserver;


/**
 * SearchResults View
 */
class CIRTopStationsView : public CIRStationsView

    {
    
public: //Methods

    /**
     * NewL()
     * Static constructor.
     */
    static CIRTopStationsView* NewL();
    /**
     * NewLC()
     * Static constructor.
     */
    static CIRTopStationsView* NewLC();
    
    /**
     *~CIRTopStationsView()
     * Destructor.
     */
    ~CIRTopStationsView();

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
	* DynInitMenuPaneL
	* Dynamically initialises a menu pane
	*/
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	
    void OfferToolbarEventL(TInt aCommand );
                
    /**
    * ConstructToolbarL()
    * Constructs a touch toolbar
    */         
   	void ConstructToolbarL();
   	
   	
	/**
	* ListenChannelL()
	* Issues a listen request
	*/
	void ListenChannelL(TInt aCurrentIndex);

    /**
     * From CAknView
     */
	void HandleStatusPaneSizeChange();

    /**
     * From CAknView
     *
     * @see CAknView::HandleForegroundEventL( TBool aForeground )
     */
	void HandleForegroundEventL( TBool aForeground );

	/**
	* Called when the refresh dialog is dismissed.
	* @param aButtonId The button that was used to dismiss the dialog.
	*/
	void DialogDismissedL( TInt aButtonId );

	/**
	* void CIRCategoryView::ResponseL()
	* Activates the Stations view after getting the IsdsResponse
	*/
	void ResponseL(CIRIsdsPreset* aPreset = NULL );
	
	/**
	* PresetResponseL
	* Issues a listen request
	*/
	void PresetResponseL(CIRIsdsPreset* aPreset);

	/**
	* AddToFavouritesL()
	* Issues a request for adding to favourites
	*/
	void AddToFavouritesL(TInt aCurrentIndex,TInt aCommand);
	
	/**
	* ErrorL()
	* Handles the errors from Isds
	*/
	void ErrorL();
	
	
	/**
    * PresetLogoDownloadedL()
    * CallBack Function Returning the Preset With Logo
    */
	void PresetLogoDownloadedL(CIRIsdsPreset* aPreset);
	
	/**
    * PresetLogoDownloadError()
    * CallBack Function Returning the Preset With no logo/error while downloading log
    */
	void PresetLogoDownloadError(CIRIsdsPreset* aPreset);
	/*
	* GetTopStationsActualIndex()
	* returns the actual index of the element in the list
	*/
	TInt GetTopStationsActualIndex();	
	
	/**
	* GetFilteredIndex()
	* Returns the filtered Index
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
    void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
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
     * CIRTopStationsView()
     * C++ default constructor.
     */
    CIRTopStationsView();
	
	TInt iTopStationsFilteredIndex;
	TBool iFirstTimeLaunchFlag;
	/*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	
	
    /* iListenRequest
	 * Checks whether request is for listen
	 */
	TBool iListenRequest;
  	
	
	/**
	* iAddToFav
	* Boolean value to determine if AddToFavorites was issued.
	*/
	TBool iAddToFav;
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
	 * iResultsReqMade
	 * boolean value to know whether the results(topstations/list of channels)
	 * request has made to isds or not
	 */
	TBool iResultsReqMade;
    
    };

#endif      // CIRTOPSTATIONSVIEW_H
            

