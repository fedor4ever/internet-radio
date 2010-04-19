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
* Description:  Header for CIRHistoryListView
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Ref:
*  Implemented SongHistory changes for channelwise songs
*
*  </ccm_history>
* ============================================================================
*/

#ifndef IRHISTORYLISTVIEW_H
#define IRHISTORYLISTVIEW_H



#include <e32cmn.h>

//For Touch Toolbar for find in shop
#include <akntoolbarobserver.h>
#include <aknbutton.h>
#ifdef MUSICSHOP_AVAILABLE
#include <MusicStoreUidConstants.h> //For Find in Shop
#include <mpxfindinmusicshopcommon.h> //For Find in Shop
#endif
#include "irbaseview.h"
#include "IRHistoryListContainer.h"
#include "UINotifyHandler.h"
#include "irdialoglauncher.h"
#include "IRActiveNetworkObserver.h"

class CIRHistoryListContainer;
class MSongHistoryUINotifyHandler; 
class CIRSongHistoryEngine;
class CIRDialogLauncher;
class MIRActiveNetworkObserver;

class CMPXFindInMShop; //For Find in Shop

class CIRHistoryListView : public CIRBaseView,
						   public MSongHistoryUINotifyHandler,
						   public MAknToolbarObserver,
						   public MIRActiveNetworkObserver
    {
    
public: 

    /**
     * Static constructor NewLC().
     */
    static CIRHistoryListView* NewLC();
    /**
     * Static constructor NewL().
     */
    static CIRHistoryListView* NewL();

    /**
     * Destructor.
     */
    ~CIRHistoryListView();

     //MSongHistoryUINotifyHandler functions
	/**
     * NextSongReceivedL()
     * Adds the currently playing song to the history's array.
     */

	void NextSongReceivedL( const TDesC& aSongName, const TDesC& sArtistName,
					 const TDesC& aChannelName, const TDesC& aChannelUrl );
	
    /**
     * ChannelChangedL( ).
     * Moves the currently playing channel to the top.
     */
	void ChannelChangedL( const TDesC& aSongName, const TDesC& sArtistName,
					 const TDesC& aChannelName, const TDesC& aChannelUrl );
	
	/**
     * RemoveSongEntry().
     * Removes the oldest song from History array.
     */	
	TInt RemoveSongEntry( const TDesC& /*aSongName*/, const TDesC& /*aArtistName*/,
					const TDesC& /*aChannelName*/, const TDesC& /*aChannelUrl*/ );
	
	/**
     * RemoveChannelEntry().
     * Not in use now!
     */
	TInt RemoveChannelEntry( const TDesC& aSongName, const TDesC& aArtistName,
					const TDesC& aChannelName, const TDesC& aChannelUrl );
	
	/**
     * NotifyDataChangeToUIError().
     * Notification to UI of any error Conditions.
     */
  	void NotifyDataChangeToUIError(TInt /*aError*/);

	/**
     * ClearSongHistoryItemL().
     * Clears the Song History List Array
     */
	void ClearSongHistoryItemL();
	
	 /**
     * GetAllListL().
     * Getting the stored List from the History Db.
     */
	void GetAllListL();

	/**
     * Id().
     */
	 TUid Id() const;

	/**
	 * From CAknView
	 *
	 * @see CAknView::HandleCommandL( TInt aCommand )
	 */
	void HandleCommandL( TInt aCommand );
	
	/**
     * DynInitMenuPaneL()
     * For Dynamic Handling of the Menu Pane
     */
	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	
    /**
     * HandleMetaDataReceived()
     * Handling the Metadata Receving
     */
	void HandleMetaDataReceived( const CIRMetaData& aMetaData );
	
	/**
     * For Touch Toolbar
     * Handles the Dynamic changes on the toolBar
     */
	void DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar );
	/**
     * OfferToolbarEventL()
     * Handles any event on the Touch ToolBar
     */
    void OfferToolbarEventL( TInt aCommand );
    /**
     * For ConstructToolbarL();
     * Constructs the Touch ToolBar
     */
  	void ConstructToolbarL();
  	
  	/**
     * To Launch Find in Shop Option
     * LaunchMusicShopL(); 
     */
    void LaunchMusicShopL();
    
    /**
     * To Launch Find in Shop Option
     * DoFindInMusicShopL() 
     */
    void DoFindInMusicShopL( const TDesC& aSongName, 
                             const TDesC& aArtistName, 
                             const TDesC& aAlbumName );
    
	/**
	* To Launch Find in Shop Option when selected from the toolbar
	* FindInShopOption() 
	*/
    void  FindInShopOptionL();

    /**
     * From CAknView
     *
     * @see CAknView::HandleForegroundEventL( TBool aForeground )
     */
   	void HandleForegroundEventL( TBool aForeground );
  	
   	/**
   	* Get the FindInShop Instance is null or not.
   	* GetFindInShopInstance() 
   	*/
   	TBool GetFindInShopInstance();
 	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );
    
   	/**
   	* Refreshes the view on synchronisation.
   	* RefreshHistoryL() 
   	*/
    void RefreshHistoryL();
    
   	/**
   	* Gets the History's engine instance.
   	* HistoryEngineInstance() 
   	*/
    CIRSongHistoryEngine* HistoryEngineInstance() ;

protected:  

   
    /**
     * From CAknView
     *
     * @see CAknView::DoActivateL(const TVwsViewId& aPrevViewId, 
     *    TUid aCustomMessageId, 
     *    const TDesC8& aCustomMessage)
     */
    void DoActivateL( const TVwsViewId& aPrevViewId, 
                      TUid              aCustomMessageId, 
                      const TDesC8&      aCustomMessage );

	/**
     * From CAknView
     *
     * @see CAknView::DoDeactivate()
     */
	void DoDeactivate();

	/**
	 * From CAknView
	 *
	 * @see CAknView::HandleStatusPaneSizeChange()
	 */
	void HandleStatusPaneSizeChange();	
	
	
private: 

    /**
     * 2nd phase constructor
     */
    void ConstructL();

	/**
     * C++ default constructor.
     */
    CIRHistoryListView();
    

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


private:    // Data

	/** 
	 * Container for this view. 
	 */
	CIRHistoryListContainer* iContainer;
	
    /**
     * View Id of the previous view.  
     */
    TVwsViewId iPrevViewId;
    
    /**
     * RPointerArray of CIRSongHistoryInfo  
     */
    RPointerArray<CIRSongHistoryInfo> iHistoryDataArr;
    
    /**
     * Pointer to CIRSongHistoryEngine 
     */
    CIRSongHistoryEngine* iHistoryEngine;  
	
        
    /**
     * For Find in Music Shop
     *
     */
    CMPXFindInMShop* iMPXFindInShop; //For Find in Shop
    
    /*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;
	 /*
	 * iAddToFav
	 * Checks whether request is for adding to favorites
	 */
	TBool iAddToFav;
  
    /* iListenRequest
	 * Checks whether request is for listen
	 */
	TBool iListenRequest;
  
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

#endif      // IRHISTORYLISTVIEW_H
            

