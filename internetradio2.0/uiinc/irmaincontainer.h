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
* Description:  Header for CIRMainContainer
*
*/


#ifndef CIRMAINCONTAINER_H
#define CIRMAINCONTAINER_H

#include <aknlists.h>
#include <EIKLBX.H>
#include <aknsfld.h> 
#include <eikclb.h> 
#include <aknprogressdialog.h>

#include "irimageconverterobserver.h"

class CIRMainView;
class CIRMainContainer;
class CAknIconArray;
class CIRImageConverter;
/**
 * This classes are used for Advertisements
 */
class CIRFilteredModel;
class MIRListBoxArray;
class CIRAdvertisingListBox;
 /**
 * Container for Search Results View.
 */
class CIRMainContainer : public CCoeControl,
						 public MEikListBoxObserver,
                         public MIRImageConverterObserver,
    					 MProgressDialogCallback
    {
    
public:  // Methods

    
    /**
    * NewL.
    * Two-phased constructor.
    * Create a CIRMainContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aView The MainView's instance.
    * @return a pointer to the created instance of CIRMainContainer.
    */
    static CIRMainContainer* NewL( CIRMainView& aView, const TRect& aRect );

    /**
    * NewLC.
    * Two-phased constructor.
    * Create a CIRMainContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aView The MainView's instance.
    * @return a pointer to the created instance of CIRMainContainer.
    */
    static CIRMainContainer* NewLC( CIRMainView& aView, const TRect& aRect );
    /**
     * ConstructL
     * 2nd phase constructor.
     */
     void ConstructL( const TRect& aRect);
    
	/**
     * CIRMainContainer()
     * C++ default constructor.
	 * @param aView View of this container.
     */
    CIRMainContainer(CIRMainView& aView);
    
    /**
     * ~CIRMainContainer()
     * Destructor.
     */
    ~CIRMainContainer();

	
	// from base class CCoeControl

     /**
	 * From CCoeControl
	 * @see CCoeControl::OfferKeyEventL() 
	 */    
	
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
    /**
     * From MEikListBoxObserver HandleListBoxEventL
     */ 
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
	/**
	 * From CCoeControl
	 *
	 * @see CCoeControl::GetHelpContext( TCoeHelpContext& aContext ) const
	 */
	void GetHelpContext( TCoeHelpContext& aContext ) const;
	

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
	* SizeChanged()
	* Responds to changes to the size and position of the contents of this control.
	*/
	void SizeChanged();
	
	/**
	* HandleResourceChange()
	* Handles a change to the control's resources.
	*/
	void HandleResourceChange(TInt aType);
	/*
	* CreateFindBoxL()
	* Creates the find box for dynamic filtering
	*/
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,
				CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
	/*
	* GetListPointer()
	* returns List box pointer
	*/
	CIRAdvertisingListBox* GetListPointer();
	/*
	* GetFilterdModelPointer()
	* returns filtered model pointer
	*/
	CIRFilteredModel* GetFilterdModelPointer();
	/*
	* GetSearchBoxPointer()
	* returns Find  box pointer
	*/
	CAknSearchField* GetSearchBoxPointer();
    /**
    * Formats list item string with given arguments
    * 
    * @param aFirstRow text for first row
    * @param aSecondRow text for second row
    * @param aIconId Icon ID used on item
    */
    HBufC* FormatItemStringLC( const TDesC& aFirstRow, const TDesC& aSecondRow, TInt aIconId );
	/**
     * UpdateAllL()
     * updates the search results data to the search results list
     */
 	void UpdateAllL();
	/*
	* DisableFindBox()
	* disable the listbox when foxus is not present
	*/
	void DisableFindBox();
	
	/**
	* Function : Comparestrings
	* Function called to get the actual index of the station
	*/
 	void  Comparestrings();
 	
 	/**
 	* HandleImageConversionEventL()
 	* from base class MIRImageConverterObserver
 	*/
	void HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent aEvent,
						 TInt aId, TInt aError );
 	/**
 	* CreateCustomListBoxL()
 	* Creates the custom listbox and updates the list according to that
 	*/
	void CreateCustomListBoxL();
	/**
	* StartConvertL()
	* used to convert the images depending the index passed
	*/
	void LastPlayedDisplayL();
	/**
	* CIRSearchResultsViewContainer::cmdMarkL()
	* implement the mark/unmark feature
	*/
	void cmdMarkL(TInt item,TBool mark);
	/**
	* DeleteMultiStationsL()
	* Deletes multiple elements from preset
	*/ 
	void DeleteMultiStationsL();
	/**
	* DeleteChannelL()
	* used to convert the images depending the index passed
	*/
	void DeleteChannelL( TInt aIndex ) ;
	// from base class MIRPresetChangesObserver
	/**
	* From MPSPresetObserver
	* 
	* @see HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason )
	*/
	void HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason );
	/**
	* StartConvertL()
	* used to convert the images depending the index passed
	*/
	void CheckConnectionL(TInt aUrlIndex);

	/**
	* DisplayFilterListBoxL()
	* Displayin gof filter box depending on favourites
	*/
	void DisplayFilterListBoxL();
    /**
    * DialogDismissedL
    * Called when/if the dialog has been dismissed.
    * @param aButtonId
    */
    void DialogDismissedL( TInt aButtonId );
	/**
	* Function : FilteredIndexL
	* Function called when mark all is called after dynamic filtering
	*/
	void FilteredIndexL();
	/*
	* CreateProgressBarL()
	* creates a progress bar.
	*/
	void CreateProgressBarL();
	/*
	* SetIconsL()
	* Set Icons to the List
	*/
	void SetIconsL();
	/*
	* RemoveFromLogoArray()
	* Removes the station from the logoarray
	*/
	void RemoveFromLogoArray(TInt aIndex);
	/*
	* SetContextSenstiveFlag()
	* Set context senstive flag when the element is marked and
	* single clicked and resets when all the elements are unmarked
	*/
	void SetContextSenstiveFlag(TBool aContextSensitiveFlag);
	/*
	* EnableEmptyListBox()
	* creates the empty listbox
	*/
	void EnableEmptyListBox();
  	/**
	* GetListBoxCount()
	* returns the number of elements in the listbox
	*/ 
  	TInt GetListBoxCount();
	/**
	 * GetCurrentItemIndex()
	 * returns the index of the currently selected item
	 */ 
	TInt GetCurrentItemIndex() const;
	
	/**
	 * GetListBoxTopIndex()
	 * returns the index of the top item
	 */ 
	TInt GetListBoxTopIndex() const;
   	/**
	* GetUrlIndex()
	* returns the number of elements in the listbox
	*/ 
  	TInt GetUrlIndex();
  	/**
	* GetLastPlayedChannel()
	* returns whether the last played station exists/not.
	*/ 
	TBool GetLastPlayedChannel();
	/*
	* GetContextSenstiveFlag()
	* returns the context sensitive flag 
	*/
	TBool GetContextSenstiveFlag();
	/*
	* GetFilteredFlag()
	* returns the filtered flag 
	*/
	TBool GetFilteredFlag();
    /**
     * Creates default (empty) icon
     * 
     * @return  Default icon
     */
    CGulIcon* CreateDefaultIconL();
	void SendRequestForLastPlayedLogoL();
 	/*
  	 * CIRIsdsPreset* GetTempPreset()
  	 * returns the temporary preset 
  	 */
  	CIRIsdsPreset* GetTempPreset(); 
	/**
	* HandleLogoErrorL()
	* called from the iSearchResultsView when the logo is not downloaded
	*/
	void HandleLogoErrorL();
   	/**
 	* PresetLogoDownloadL()
 	* Used to download the  logos depending the preset values.
 	*/
 	void PresetLogoDownloadL(CIRIsdsPreset* aPreset);
    /**
 	* RefreshViewL()
 	* Resets all the values and draws the view with new values.
 	*/
	void RefreshViewL();
    /**
 	* SetFilteredIndexArrayL()
 	* stores the indexs of all the filtered elements
 	*/
	void  SetFilteredIndexArrayL();
    /**
 	* CheckFiltered()
 	* checks whether the deleted index exists in the filteredindex array or not
 	*/
	TBool CheckFiltered();
	
private: //Methods


	// from base class CCoeControl
	/**
     * From CCoeControl
     *
     * @see CCoeControl::Draw( const TRect& aRect ) const
	 */
	void Draw( const TRect& aRect ) const;
	/**
	* StartConvertL()
	* used to convert the images depending the index passed
	* 
	*/
	void StartConvertL( CIRImageConverter& aConverter, TInt aIndex, TInt aId );	
	
	void ProcessPointerEventL( TInt /* aIndex */);

private:    // Data
	 /**
	 * iItemArray
	 * array for the listbox model
	 */ 
	 CDesCArray* iListArray;
	 
	 /**
	 * iMainView
	 * Data for CIRStationsView view
	 */ 
	 CIRMainView& iMainView;
    /*
	* iConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iConverter;
	
	/*
	* iCurrentItemConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iCurrentItemConverter;
    /*
    * iBgContext
    * Instance of CAknsBasicBackgroundControlContext
    */
    CAknsBasicBackgroundControlContext* iBgContext;
	/**
	* iFavouritesListBox
	* Instance of the Stations ListBox
	*/
	CIRAdvertisingListBox* iFavouritesListBox;
	/*
	* iSearchTextBox
	* Instance for CAknSearchField;
	*/
	CAknSearchField* 			iFavouritesSearchBox;	
    /**
    * iLabel
    * instance for CAknDoubleNumberStyleListBox
    */
    CAknDoubleLargeStyleListBox* iEmptyListbox;
     /**
    * iProgressDialog
    * Not owned by CAknExNoteContainer object.
    */
    CAknProgressDialog*             iProgressDialog;

    /**
    * iProgressInfo
    * Not owned by CAknExNoteContainer object.
    */
    CEikProgressInfo*               iProgressInfo;
 	/*
	* iFilteredModel
	* Instance for CAknFilteredTextListBoxModel
	*/ 
	CIRFilteredModel* iFilteredModel;
    /*
    * iListBoxArray
    * Instance of MIRListBoxArray
    */
   	MIRListBoxArray* iListBoxArray;
	/**
	* Temporary preset to store url and the logo data
	*/
	CIRIsdsPreset *iTempPreset;
   /*
    * iItem
    * sets the itemfor filtered zeroth index
    */
	HBufC *iItem;
    /*
    * iLastplayed
    * sets the Lastplayed string
    */
	HBufC *iLastplayed ;
    /*
    * iAdvString
    * sets the advertisement string
    */
    HBufC* iAdvString;
	/*
	* iInvalidIconIndices
	* instance of RArray
	*/
	RArray<TInt> iInvalidIconIndices;
	/*
	* iLogosArray
	* instance of RArray
	*/
	RArray<TInt> iLogosArray;
	/*
	* iNoLogosArray
	* instance of RArray
	*/
	RArray<TInt> iNoLogosArray;    
	/*
	* iInvalidIconIndices
	* instance of RArray
	*/
	RArray<TInt> iFilterIndexExist;
 	/*
	* iFindString
	* Used to Get the Entered Search string
	*/
	TBuf<KSearchBoxTextLength> iFindString;
    /**
	* iMultiDeleteString
	* contains the data that how many elements are deleted after mark
	*/   
	RBuf iMultiDeleteString;	
    /**
	* iFinalCount
	* contains the final count to present after the deletion of marked items
	*/   
	TInt iFinalCount;
	/*
	* iUrlIndex
	* contains the indx of the url
	*/
   	TInt iUrlIndex;
    /**
	* iProgressCount
	* contains the number of counts the progress has incremented
	*/   
	TInt iProgressCount;
 	/*
	* iDeletingCount
	* contains the number of elements to be deleted
	*/
    TInt iDeletingCount;
	/*
	* iImageCount
	* have the position of image that has to be placed in iconarray
	*/
    TInt iImageCount;
    /*
   	* iLandScapeCount
   	* used to call the aknfind s HandlePopupFindSizeChanged function only once
   	*/
	TInt iLandScapeCount;
   /**
	* iAftertime
	* contains the time after the deletion of station
	*/   
	TTime iAftertime;
    /**
	* iBeforetime
	* contains the time before the deletion of stations started
	*/   
	TTime iBeforetime;
    /**
	* iIntervalsec
	* contians the difference of time taken to delete the elements 
	* i.e., (iAftertime-iBeforetime)
	*/   
	TTimeIntervalMicroSeconds iIntervalsec;
    /**
	* markedCount
	* conatians the cunt of marked elements
	*/   
	TReal32 iMarkedElementsCount;
   /**
	* iProgress
	* contains the amount of progress should be done inorder to update the progress dialog
	*/   
	TReal32 iProgress;
	/*
	* iFilteredFlag
	* This flag is set when dynamic filtering is done and
	* will be reseted when the characters in the filter box is deleted
	*/
	TBool iFilteredFlag;
	/*
	* iContextSensitiveFlag
	* sets when any station is marked and the element is single clicked
	*/
    TBool iContextSensitiveFlag;
	/*
	* iImageLast
	* sets when lastplayed has logo data
	* and resets when the logo is dowmloaded
	*/
    TBool iImageLast;
	/*
	* iLastPlayedChannel
	*  variable to set the lastplayed station
	*/
	TBool iLastPlayedChannel;
    /*
    *
    * Flag for progress dialog status
    */
    TBool iProgressFlag;
	/*
	* iProgressStart
	* this will be set when the progressdialog is created
	*/
	TBool iProgressStart;
	/*
	* iMoveFocusLast
	* this will be set when the Move is enabled and the focus is on the lastplayed
	*/
	TBool iMoveFocusLast;
   	/*
   	* iLogoDownloadError
   	* sets when the  advertisment future is not supported
   	*/
	TBool iLogoDownloadError;
	TInt iError;
	};

#endif      // CIRMAINCONTAINER_H
