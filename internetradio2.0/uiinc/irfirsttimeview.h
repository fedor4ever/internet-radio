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
* Description:  Header for CVRFindStationsView
*
*/


#ifndef C_CIRFIRSTTIMEVIEW_H_
#define C_CIRFIRSTTIMEVIEW_H_

#include "irbaseview.h"
#include "MViewsResponseAndErrorObserver.h"
#include "IRActiveNetworkObserver.h"

//#include <akntoolbarobserver.h>
#include <aknbutton.h>
class CIRFirstTimeContainer;
class CIRAddManuallyStationForm;
class CIRDialogLauncher;
class MIRActiveNetworkObserver;

class CIRFirstTimeView : public CIRBaseView, 
						 public MViewsResponseAndErrorObserver,
						 public MIRActiveNetworkObserver //, public MAknToolbarObserver
{

public:

	 /**
     * NewL()
     * Static constructor.
     */
    static CIRFirstTimeView* NewL();
    
    /**
     * NewLC()
     * Static constructor.
     */
    static CIRFirstTimeView* NewLC();
    
    /**
     * Destructor.
     */
    ~CIRFirstTimeView();

// from base class CVRBaseView    
    
    TUid Id() const;
    void HandleCommandL( TInt aCommand );
    
    /**
	  * void CIRFirstTimeView::DynInitMenuPaneL()
 	  * Dynamically initialises a menu pane
	  */
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
       
    /**
	* void CIRSearchView::ResponseL()
	* Activates the SearchResults view after getting the IsdsResponse
	*/
	void ResponseL( CIRIsdsPreset* aPreset = NULL );
	void PresetResponseL(CIRIsdsPreset* aPreset);
	void ErrorL();
    
    void ShowQueryDialogL(const TDesC& aTitle, TDes& aBuffer);
	
	/* void CIRFirstTimeView::IssueSearchRequestL()
	 * Issues a search request to the Isds Client
	*/
	void IssueSearchRequestL();
 	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );
	/*
	* RefreshView()
	* Refreshes the view when layout changes.
	*/
	void RefreshView();
protected:  

// from base class CVRBaseView

    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
    				 const TDesC8& aCustomMessage );
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
    
private:

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ default constructor.
     */
    CIRFirstTimeView();
    
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
     */
    CIRFirstTimeContainer* iContainer;
    
    /**
     * Are we first time in this view for this session
     */
    TBool iFirstTime;
    /*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	
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

};

#endif /*C_CIRFirstTimeView_H_*/
