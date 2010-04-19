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
* Description:  Header for CIRCategoryView
*
*/


#ifndef CIRCATEGORYVIEW_H
#define CIRCATEGORYVIEW_H

#include "irbaseview.h"
#include "MViewsResponseAndErrorObserver.h"

#include "MLogoDownloadObserver.h"
#include "IRActiveNetworkObserver.h"


class CIRCategoryViewContainer;


/**
 * SearchResults View
 */
class CIRCategoryView : public CIRBaseView,public MViewsResponseAndErrorObserver,
						public MLogoDownloadObserver,public MIRActiveNetworkObserver
    {
    
public: //Methods

	//ENums to indicate the currently selected item in the listbox.
 	enum TCurrentSelectedItem
		{
		EGenreData=0,
		ELanguageData,
		ECountryData,
		ETopStationData
		};
    /**
     * NewL()
     * Static constructor.
     */
    static CIRCategoryView* NewL();

    /**
     * NewLC()
     * Static constructor.
     */
    static CIRCategoryView* NewLC();
    
    /**
     *~CIRCategoryView()
     * Destructor.
     */
    ~CIRCategoryView();

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
	void SetStatusPaneTextL();

	/**
	* DoChannelRequestL
	* Issues a Request to Isds for getting the Channels
	*/
	void DoChannelRequestL();
	
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
	* DynInitMenuPaneL
	* Dynamically initialises a menu pane
	*/
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);


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
	void AdRequestL();
	/*
	* SetActualFilteredItemIndex()
	* sets the original index of the filtered item
	*/
	void SetActualFilteredItemIndex(TInt aIndex);
	
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
	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );
	
	
public:
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


protected:  //Methods

    // from base class CAknView
    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/, const TDesC8& aCustomMessage );

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
     * CIRCategoryView()
     * C++ default constructor.
     */
    CIRCategoryView();


private:    // Data

	/** 
	 * iContainer
	 * Container for this view. 
	 */
	CIRCategoryViewContainer* iContainer;

 
	/**
	* iLoadingCancelled
	* Data whether loading has been cancelled
	*/
	TBool iLoadingCancelled;
	/*
	 * iIndex
	 *  
	 */
	 TInt iIndex;
	TInt iFilteredIndex;
	
	TBool iFirstTimeLaunchFlag;
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
	
		
	/*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	
	/*
	 * iRequestIssued
	 * Checks whether the request has been issued
	 */
	TBool iRequestIssued;
	
	

public:	
	/**
	 * iCategoryReqMade
	 * boolean value to know whether the category request
	 * has made to isds or not
	 */
	TBool iCategoryReqMade;
	
	 
		
    };

#endif      // CIRCATEGORYVIEW_H
            

