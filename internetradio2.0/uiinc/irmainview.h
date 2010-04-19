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
* Description:  Header for CIRMainView
*
*/


#ifndef CIRMAINVIEW_H
#define CIRMAINVIEW_H


#include "irbaseview.h"
#include <akntoolbarobserver.h>
#include <aknbutton.h>
#include <pspresetobserver.h>

#include "irpreset.h"
#include "MViewsResponseAndErrorObserver.h"
#include "IRActiveNetworkObserver.h"
#include "msyncpresetobserver.h"	// MSyncPresetObserver
#include "MLogoDownloadObserver.h"

class CIRMainContainer;
class CAknWaitDialog;
class MLogoDownloadObserver;


/**
 * SearchResults View
 */
class CIRMainView : public CIRBaseView,
						   public MAknToolbarObserver,
	   				       public MPSPresetObserver,
	   				       public MIRActiveNetworkObserver,
	   				       public MSyncPresetObserver,
					       public MLogoDownloadObserver,
					       public MViewsResponseAndErrorObserver

    {
    
public: //Methods
    /**
     * NewL()
     * Static constructor.
     */
    static CIRMainView* NewL();

    /**
     * NewLC()
     * Static constructor.
     */
    static CIRMainView* NewLC();
    
    /**
     *~CIRMainView()
     * Destructor.
     */
    ~CIRMainView();
    /**
     * ConstructL()
     * 2nd phase constructor
     */
    void ConstructL();

	/**
     * CIRMainView()
     * C++ default constructor.
     */
    CIRMainView();

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

	/*
	* OfferToolbarEventL
	* 
	*/
	void OfferToolbarEventL(TInt aCommand );
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

	/**
	 * HandlePresetChangedL
	 * whenever their is any change in preset
	 * the function will get invoked.
	 */
	 void HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason );
	 /**
	 * ConnectToLastPlayedStationL()
	 * Connects to the last played station.
	 */
    void ConnectToLastPlayedStationL();
	/*
	* GetMainContainer()
	* returns pointer to main container
	*/
	CIRMainContainer* GetMainContainer();
	/**
	 * SetUrlFromPresetL.
	 */
    void SetUrlFromPresetL(TInt aUrlIndex);
	/**
	 * DoConnectToChannelL
	 * Connects to the  station .
	 */
    void DoConnectToChannelL( TInt aUrlIndex );
	/**
	 * SyncPresetL.
	 */
    void SyncPresetL(TInt aChid,const TDesC &aModifiedTime);
	/**
	* SetListBoxMarkedCount()
	* sets the marked item count in the listbox
	*/ 
	void SetListBoxMarkedCount(TInt aMarkedItemCount);
	/**
	* SetMarkedFlag()
	* sets the marked flag
	*/ 
	void SetMarkedFlag(TInt aMarkedFlag);
	/**
	* SetMarkedAllFlag()
	* sets the marked All flag value.
	*/ 
	void SetMarkedAllFlag(TInt aMarkedAllFlag);
	/*
	* SetRequestPendingStatus()
	* Sets the request pending state
	*/
	void SetRequestPendingStatus(TBool aRequestPending);
	/*
	* SetFavListen()
	* Sets the listen is from favourites or not
	*/
	void SetFavListen(TBool aFavListen);
	/*
	* SetActualFilteredItemIndex()
	* sets the original index of the filtered item
	*/
	void SetActualFilteredItemIndex(TInt aFilteredIndex);
 	/**
	* SetMovingFlag()
	* Sets the view is in move mode .
	*/ 
    void SetMovingFlag(TBool aValue);
	/**
	* GetMarkedIndexArray()
	* returns the array of elements marked  in the listbox
	*/ 
	RArray<TInt> GetMarkedIndexArray();
	/**
	* GetListBoxMarkedCount()
	* returns the number of elements marked  in the listbox
	*/ 
	TInt GetListBoxMarkedCount();
	/**
	* GetMarkedFlag()
	* returns the iMarkedflag value
	*/ 
	TInt GetMarkedFlag();
	/**
	* GetMarkedAllFlag()
	* returns the iMarkedAllflag value
	*/ 
	TInt GetMarkedAllFlag();
	/**
	* GetCurrentIndex()
	* returns the index of the station.
	*/ 
	TInt GetCurrentIndex();
	/**
	* GetLastPlayedStatus()
	* returns the iLastplayed value
	*/ 
	TBool GetLastPlayedStatus();
	/**
	* GetAddToFavouratesFlag()
	* returns whether any station is added to favourates or not.
	*/ 
  	TBool GetAddToFavouratesFlag();
	/*
	* GetRequestPendingStatus()
	* returns whether an active request is pending
	*/
	TBool GetRequestPendingStatus();
	/*
	* GetFavListen()
	* returns /Checks whether request is listen to favorites.
	*/
	TBool GetFavListen();
    /**
	* GetAddDimmingStatus()
	* returns the dimming status value for Enable/Disabling Add to favourites option
	*/ 
    TBool GetAddDimmingStatus();
   
    /**
	* GetEditDimmingStatus()
	* returns the dimming status value for Enable/Disabling Edit option
	*/ 
  	TBool GetEditDimmingStatus();
 	/**
	* GetMovingFlag()
	* Gets the view is in move mode or not.
	*/ 
    TBool GetMovingFlag();
    
	/**
	 * from MSyncPresetObserver
	 * SyncPresetRemovedL()
	 * called back when the preset that is synced was removed from isds
	 * @param TInt,the preset id
	 */
	void SyncPresetRemovedL(TInt aId);

	/**
	 * from MSyncPresetObserver	
	 * SyncPresetChangedL()
	 * called back when the preset that is synced has been changed in the isds
	 * @param CIRIsdsPreset&,the new preset data
	 */
	void SyncPresetChangedL(CIRIsdsPreset& aPreset);
	
	/**
	 * from MSyncPresetObserver	
	 * SyncPresetNoChangeL()
	 * called back when the preset that is synced has not been changed
	 */
	 void SyncPresetNoChangeL();
	 
	/**
	 * ConnectToSelectedStationL()
	 * Used to synchronize the LastPlayed preset before listen
	 */
	 void ConnectToSelectedStationL();
	 
	/**
	 * AddStationtoFavoritesL()
	 * Called when user selects AddtoStations cmd from the MainView options
	 */ 
	 void AddStationtoFavoritesL();
    
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
	* ResponseL()
	* From the class MViewsResponseAndErrorObserver
	*/
	void ResponseL(CIRIsdsPreset* aPreset = NULL );

	/**
	* PresetResponseL()
	* From the class MViewsResponseAndErrorObserver
	*/
	void PresetResponseL(CIRIsdsPreset* aPreset);

	/**
	* ErrorL()
	* From the class MViewsResponseAndErrorObserver
	*/
	void ErrorL();
	
	
	
	
    
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

  	
private:
	/** 
	* iContainer
	* Container for this view. 
	*/
	CIRMainContainer* iContainer;
	/** 
	* iMarkedFlag
	* returns bool value depending on mark/unmark state. 
	*/    
	TBool iMarkedFlag;
	/** 
	* iMarkedAllFlag
	* returns bool value depending on whether all the elements are marked/unmarked state. 
	*/    
	TBool iMarkedAllFlag;
	/*
	* iAddToFavouratesFlag
	* returns whether any station is added to favourates or not.
	*/
	TBool iAddToFavouratesFlag;
	
 	/*
	* iIndexExisting
	*/
	TBool iIndexExisting;
	/*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	/*
	 * iFavListen
	 * Checks whether request is listen to favorites 
	 */
	TBool iFavListen;
	/*
	 * iStationDetailsCmd
	 *  
	 */
    TBool iStationDetailsCmd;
	
	/** 
	* iMoveMarkedFlag
	* boolean flag to set the moving state 
	*/
	TBool iMoveMarkedFlag;
	/**
	* iMarkedItemCount
	* Data to count the no.of items marked
	* Used to Handle multiselection
	*/
	TInt iMarkedItemCount; 
	/*
	 * iFilteredIndex
	 * the index that is obtained after the dynamic filtering of a station
	 */
	TInt iFilteredIndex;
	/** 
	* iMovedItemIndex
	* flag to get the marked item 
	*/
    TInt iMovedItemIndex;
	/** 
	* iIndexTo
	* Index of moved place 
	*/
    TInt iIndexTo;
	/** 
	* iIndexFrom
	* Index of moving place
	*/
    TInt iIndexFrom;
	/** 
	* iChannelIdTo
	* Id of the moved channel
	*/
    TInt iChannelIdTo;
	/** 
	* iChannelIdFrom
	* Id of the moving channel
	*/
    TInt iChannelIdFrom;
    
     /* iListenRequest
	 * Checks whether request is for listen
	 */
	TBool iListenRequest;
	
	/* iAddtoStationsCmd
	* Checks whether request is for AddtoStations
	*/
	TBool iAddtoStationsCmd;
  

public:
	/*
	* iInvalidIconIndices
	* instance of RArray
	*/
	RArray<TInt> iMarkedIndexArray;
	/*
	* iInvalidIconIndices
	* instance of RArray
	*/
	RArray<TInt> iUnMarkedArray;
	/*
	* iEditStation
	* instance for getting the Edit station index
	*/
	TBool iEditStation;
	
	/*
	* iSyncReq
	* to know the status whether the sync request is completed or not
	*/	
    TBool iSyncReq;
    
    
    
   };

#endif      // CIRMAINVIEW_H
            

            

