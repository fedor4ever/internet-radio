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
* Description:  Header for CIRStationsDirectoryView
*
*/


#ifndef C_CIRSTATIONDIRECTORYVIEW_H
#define C_CIRSTATIONDIRECTORYVIEW_H



#include "irbaseview.h"
#include "MViewsResponseAndErrorObserver.h"
#include "IRActiveNetworkObserver.h"

class CIRStationDirectoryContainer;
class MIRActiveNetworkObserver;

class CIRStationDirectoryView : public CIRBaseView ,
								public MViewsResponseAndErrorObserver,
								public MIRActiveNetworkObserver
{
public:

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
    static CIRStationDirectoryView* NewL();

    /**
     * NewLC()
     * Static constructor.
     */
	static CIRStationDirectoryView* NewLC();

	/**
	* ~CIRStationDirectoryView()
	* Destructor 
	*/
	~CIRStationDirectoryView();

	// from base class CIRBaseView    

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
	* DoCategoryRequestL()
	* Issues a request ti Isds for the Stations
	*/
	void DoCategoryRequestL();
	
	/**
	* ResponseL()
	* Activates the Category view after getting the IsdsResponse
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
	* ActivateCategoryViewL
	* Activates the Stations view
	*/
	void ActivateCategoryViewL();
	/*
	* RefreshView()
	* Refreshes the view when layout changes.
	*/
	void RefreshView();
protected:  

	// from base class CIRBaseView

    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
    			 const TDesC8& aCustomMessage );

	/**
     * From CAknView
     *
     * @see CAknView::DoDeactivate()
     */
    void DoDeactivate( );
    
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
 	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );
    
private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CIRStationDirectoryView();
    
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


private:    
    
    /** 
     * Container for this view. 
     * Owned
     */
    CIRStationDirectoryContainer* iContainer;
    
	/**
	* iLoadingCancelled
	* Data whether loading has been cancelled
	*/
	TBool iLoadingCancelled;
	
	/*
	 * iRequestIssued
	 * Checks whether the request has been issued
	 */
	TBool iRequestIssued;
	
	/*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
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

};
#endif /*C_CIRSTATIONDIRECTRORYVIEW_H_*/
