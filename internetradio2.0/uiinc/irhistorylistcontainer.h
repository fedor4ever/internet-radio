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
* Description:  Header for CIRHistoryListContainer
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

#ifndef IRHISTORYLISTCONTAINER_H
#define IRHISTORYLISTCONTAINER_H

#include <akntreelistobserver.h>
#include <akntreelist.h>

class CIRHistoryListView;
class CIRSongHistoryInfo;
class CAknSingleStyleTreeList;

/**
 * Container for History List View.
 */
class CIRHistoryListContainer :public CCoeControl,public MAknTreeListObserver,
					public MViewsResponseAndErrorObserver
    
    {
    
public:  // Methods

	/**
	* NewL.
	* Two-phased constructor.
	* Create a CIRHistoryListContainer object, which will draw itself to aRect.
    * @return a pointer to the created instance of CIRHistoryListContainer.
	*/
	static CIRHistoryListContainer* NewL(const TRect& aRect ,CIRHistoryListView &aView ,
					RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr);

	/**
	* NewLC.
	* Two-phased constructor.
	* Create a CIRHistoryListContainer object, which will draw itself
	* to aRect.
	* @return A pointer to the created instance of CIRHistoryListContainer.
	*/
	static CIRHistoryListContainer* NewLC(const TRect& aRect,CIRHistoryListView &aView,
					RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr);
    /**
     * 2nd phase constructor.
     */
    
    void ConstructL( const TRect& aRect, RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr );
    

	/**
     * C++ default constructor.
     *
	 * @param aView View of this container.
     */
    CIRHistoryListContainer( CIRHistoryListView& aHistoryListView );
    
    /**
     * Destructor.
     */
    ~CIRHistoryListContainer();

	/**
	* SizeChanged()
	* 
	*/
	void SizeChanged();
	 

	/**
	 * UpdateAllL()
	 * Updates all items of the list by reading data from preset data base.
	 */
	 
 	void UpdateAllL(RPointerArray<CIRSongHistoryInfo>  & aHistoryDataArr);

    /**
     * InsertSongEntryL()
     * Adds the specified list item
     * 
     */
     
     void InsertSongEntry(const TDesC & aSongName, const TDesC & aArtistName,
     			 const TDesC & aChannelName, const TDesC & aChannelUrl);
     
     /**
     * RemoveListItemL()
     * Removes the specified list item
     * 
     */
       
     void RemoveListItem(const TDesC &aSongName , const TDesC & aArtistName,
     							 const TDesC & aChannelName);

  
     
   // from base class CCoeControl

    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
	
	
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

	/**
	 * From CCoeControl
	 *
	 * @see CCoeControl::GetHelpContext( TCoeHelpContext& aContext ) const
	 */
	void GetHelpContext( TCoeHelpContext& aContext ) const;
	
	/**
	 * ClearHistoryList()	 
	 * Clears the history list
	 */
	void ClearHistoryListL();

	/**
	 * ListenUrlL()
	 * Connects to channel url and opens Now Playing view
	 */
	void ListenUrlL( TInt index );
	
	/**
	* From MAknTreeListObserver.
	* Handles tree list events.
	*/
    TBool HandleTreeListEvent( CAknTreeList& aList, TAknTreeItemID aItem,
            TEvent aEvent );
            
	/**
	 * HandleResourceChange()
	 * Handles Changes in the resources 
	 */
    void HandleResourceChange( TInt aType ); 
    
	/**
	 * GetCurrentFocussedNode()
	 * To get the focus of Node of the treeList
	 */
    TInt GetCurrentFocussedNode();
    
	/**
	* GetCurrentFocussedLeaf()
	* To get the focus of Leaf of the treeList
	*/
    TInt GetCurrentFocussedLeaf();
    
	/**
	* AddStationToFavorites()
	* For Adding the Station name to the favorites DB.
	*/
    void AddStationToFavoritesL(TInt aCommand);
    
	/**
	* DisplayErroronDBL()
	* To show error note whenever the Fovorites DB is full
	*/
    void DisplayErroronDBL();
    
	/**
	* ShowFocus()
	* To return whether a leaf or node is focused 
	*/
    TInt ShowFocus();
    
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
	* ErrorL()
	* Handles error Conditions
	*/
	void ErrorL();
	
    /**
	* IsdsOrNot()
	* Returns Whether ISDS channel or not.
	*/
    TBool IsdsOrNot();
    
    /**
	* GetButtonDimmed()
	* Returns Whether Find in Shop Button is dimmed or not.
	*/
    TBool GetButtonDimmed() ;
    
    /**
	* RemovedIsdsChannel()
	* Plays or AddtoFav the removed isds channel converting it to Userdefined.
	*/
    void RemovedIsdsChannelL() ;


private: //Methods

	 
    // from base class CCoeControl
	/**
     * From CCoeControl
     *
     * @see CCoeControl::Draw( const TRect& aRect ) const
	 */
	void Draw( const TRect& aRect ) const;
	
	
	/**
	* CountComponentControls() const
	* returns the number of components in the view.
	*/
	TInt CountComponentControls() const;
	
	/**
	* ComponentControl()
	* returns the pointer to the control
	*/
	CCoeControl* ComponentControl(TInt aIndex) const ;
	
	/**
	* Retrieves an object of the same type as that encapsulated in aId.
	*
	* @param aId An encapsulated object type ID.
	*/
	
	TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
	
	



private:    // Data

	CIRHistoryListView & iHistoryListView;
	
    /** 
	 * CAknSingleStyleTreeList object for hierarchical list
	 */
	CAknSingleStyleTreeList* iTreeListBox;
    /** 
	 * CAknsBasicBackgroundControlContext object 
	 */
	CAknsBasicBackgroundControlContext* iBgContext;
    /** 
	 * Array for storing the Node's TAknTreeItemID 
	 */
    RArray<TInt> iNodeArray;
    /** 
	 * Array for storing the Leaf's TAknTreeItemID
	 */
    RArray<TInt> iLeafArray;
    /** 
	 * Array for storing the URL Name for Listen scenario
	 */
    CDesC16ArrayFlat * iUrlArray;
    /** 
	 * Array for storing the Channel Name 
	 */
    CDesC16ArrayFlat * iChannelArray;
    /** 
	 * Array for storing the Channel Type
	 */
    RArray<TInt>  iChannelTypeArray;
    
    /** 
	 * Array for storing the Channel Description 
	 */
    CDesC16ArrayFlat * iChannelDescriptionArray;
    

    TBool iLoadingCancelled;
    
    /** 
	 * Array for storing the Channel Id
	 */
    RArray<TInt>  iChannelIdArray;
    /** 
	 * Array for storing the MusicStoreStatus of the channel
	 */
    CDesC16ArrayFlat * iMusicStoreStatus;
    
    TBool iDimmed; 
    
    /** 
	 * Used to store the value of the Id when removed from the isds.
	 */
    TInt iChannelSyncId ;
    /** 
	 * Boolean to store whether Listen request from history view.
	 */
	TBool iListenFrmHistory;
    /** 
	 * Boolean to store whether AddToFav request from history view.
	 */
    TBool iAddFrmHistory;
    /** 
	 * Used to store the value of the current index of the view removed from the isds.
	 */
    TInt iChangedIndex ;
    };

#endif      // IRHISTORYLISTCONTAINER_H
