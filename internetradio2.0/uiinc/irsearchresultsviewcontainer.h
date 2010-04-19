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
* Description:  Header for CIRSearchResultsViewContainer
*
*/


#ifndef CIRSEARCHRESULTSVIEWCONTAINER_H
#define CIRSEARCHRESULTSVIEWCONTAINER_H

#include <aknlists.h>
#include "irimageconverterobserver.h"
#include <aknsfld.h> 

class CIRSearchResultsView;
class CAknIconArray;
class CIRImageConverter;

/**
 * This classes are used for Advertisements
 */
class CCustomListBox;
class CIRFilteredModel;
class MIRListBoxArray;
class CIRAdvertisingListBox;


/**
 * Container for Search Results View.
 */
class CIRSearchResultsViewContainer : public CCoeControl,public MEikListBoxObserver,
                            public MIRImageConverterObserver
    {
    
public:  // Methods

	/**
	* NewL.
	* Two-phased constructor.
	* @param aRect The rectangle this view will be drawn to.
    * @param aView which is a plsview's reference
    * @return a pointer to the created instance of CIRSearchResultsViewContainer.
	*/
	static CIRSearchResultsViewContainer* NewL(const TRect& aRect ,CIRSearchResultsView &aView );

	/**
	* NewLC.
	* Two-phased constructor.
	* to aRect.
	* @param aRect Rectangle this view will be drawn to.
	* @param aView which is a plsview's reference
	* @return A pointer to the created instance of CIRSearchResultsViewContainer.
	*/
	static CIRSearchResultsViewContainer* NewLC(const TRect& aRect,CIRSearchResultsView &aView);
	
    
    /**
     * ConstructL
     * 2nd phase constructor.
     */
     void ConstructL( const TRect& aRect);
    
	/**
     * CIRSearchResultsViewContainer()
     * C++ default constructor.
	 * @param aView View of this container.
     */
    CIRSearchResultsViewContainer(CIRSearchResultsView &aSearchResultsView);
    
    /**
     * ~CIRSearchResultsViewContainer()
     * Destructor.
     */
    ~CIRSearchResultsViewContainer();

	/**
     * UpdateSearchResultsViewL()
     * updates the search results data to the search results list
     */
 	void UpdateSearchResultsViewL();
 	
 	
	// from base class CCoeControl

     /**
	 * From CCoeControl
	 *
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
	
	/**
 	* HandleImageConversionEventL()
 	* from base class MIRImageConverterObserver
 	*/
	void HandleImageConversionEventL( 
		MIRImageConverterObserver::TIRImageConversionEvent /*aEvent*/, TInt aId, TInt aError );
	
	/**
	* HandleLogoErrorL()
	* called from the iSearchResultsView when the logo is not downloaded
	*/
	void HandleLogoErrorL();

	
    void UpdateSearchResultsL();
    
    /**
 	* CreateCustomListBoxL()
 	* Creates the custom listbox and updates the list according to that
 	*/
	void CreateCustomListBoxL();
	
 	/**
 	* OpenUrlL()
 	* Used to connect to advertisement url
 	*/
	void OpenUrlL( const TDesC& aHttpLink );
	/*
	* CreateFindBoxL()
	* Creates the find box for dynamic filtering
	*/
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,
	CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
	/*
	* DisableFindBox()
	* disable the listbox when focus is not present
	*/
	void DisableFindBox();
	
	/**
	* Function : Comparestrings
	* Function called to get the actual index of the station
	*/
 	void  Comparestrings();
 	
 	/*
 	 * CIRSearchResultsViewContainer::GetFilterdModelPointer()
	 * returns pointer to filtered model.
	 */
 	CIRFilteredModel* GetFilterdModelPointer();
 	/*
 	 * CIRSearchResultsViewContainer::GetFindString()
	 * returns the string in the findbox
	 */
 	TDesC GetFindString();
 	
 	/*
 	 * CIRSearchResultsViewContainer::GetListPointer()
	 * returns pointer to Listbox.
	 */
  	CIRAdvertisingListBox* GetListPointer();
 	/*
 	 * CIRSearchResultsViewContainer::GetSearchBoxPointer()
	 * returns the search box pointer.
	 */
 	CAknSearchField* GetSearchBoxPointer();

 	/*
 	 * CIRIsdsPreset* GetAdPreset()
 	 * returns the advertisement preset 
 	 */
 	CIRIsdsPreset* GetAdvPreset();
 	/*
  	 * CIRIsdsPreset* GetTempPreset()
  	 * returns the temporary preset 
  	 */
  	CIRIsdsPreset* GetTempPreset(); 
	/**
 	* CreateListBoxL()
 	* Creates ListBox depending upon the presence of Advertisement
 	*/
	void CreateListBoxL();	
    /**
    * Formats list item string with given arguments
    * 
    * @param aFirstRow text for first row
    * @param aSecondRow text for second row
    * @param aIconId Icon ID used on item
    */
    HBufC* FormatItemStringLC( const TDesC& aFirstRow,  TInt aIconId );
	/**
	* GetLogoNotSupported()
	* returns whether the advertisement is supported by the sdk r not
	*/
	TBool GetLogoNotSupported();
    /**
     * Creates default (empty) icon
     * 
     * @return  Default icon
     */
    CGulIcon* CreateDefaultIconL();
   	/**
 	* PresetLogoDownloadL()
 	* Used to download the  logos depending the preset values.
 	*/
 	void PresetLogoDownloadL(CIRIsdsPreset* aPreset);	

private: //Methods


	// from base class CCoeControl
	/**
     * From CCoeControl
     *
     * @see CCoeControl::Draw( const TRect& aRect ) const
	 */
	void Draw( const TRect& /*aRect*/ ) const;

	/**
	* StartConvertL()
	* used to convert the images depending the index passed
	* 
	*/
	void StartConvertL( CIRImageConverter& aConverter, TInt /*aIndex*/, TInt /*aId*/ );	




private:    // Data

	/**
	 * iChannelNameArray
	 * array to hold the name of channels
	 */ 
	 CDesCArray* iChannelNameArray;
	 
	 /**
	 * iChannelInfoArray
	 * array to hold the description of channels
	 */ 
	 CDesCArray* iChannelInfoArray;
	 /**
	 * iItemArray
	 * array for the listbox model
	 */ 
	 CDesCArray* iItemArray;
	 
	 /**
	 * iSearchResultsView
	 * reference of the SearchResultsView 
	 */ 
	 CIRSearchResultsView &iSearchResultsView;

	/**
    * boolean for forcibly getting the logo from cache
    */
	TBool   iForceGet;
	
	/**
    * response from the cache for the availability of logo(CacheNotValid,NotCached,CacheUseable)
    */
	TInt 	iResult;
	
	/**
	* to keep track of the icon array index
	*/
	TInt iIconIndex;
	
	/**
	* to keep track of the logo url index(for temporary)
	*/
	TInt iLogoIndex;
   	/*
   	* iLandScapeCount
   	* used to call the aknfind s HandlePopupFindSizeChanged function only once
   	*/
	TInt iLandScapeCount;
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
	* iCurrentItemConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iCurrentItemConverter;
	
	/*
	 * iAdBitmap
	 * instance of CFbsBitmap to store the advertisement
	 */
	CFbsBitmap*     iAdBitmap;

private:
	
	 /**
	 * iStationsListBox
	 * Instance of the SearchResults ListBox
	 */
	  CIRAdvertisingListBox* iSearchListBox;

	/*
	* iConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iConverter;

	/**
	* Temporary preset to store url and the logo data
	*/
	CIRIsdsPreset *iTempPreset;
	
	/**
	* Temporary preset to store the raw data of Advertisements
	*/
	CIRIsdsPreset*  iAdvPreset;
	/*
	* iFilteredModel
	* Instance for CIRFilteredModel
	*/ 
	CIRFilteredModel* iFilteredModel;
	
    /*
    * iAdPresent
    * returns whether the advetisment is present or not
    */
    TBool iAdPresent;
    /*
    * iBgContext
    * Instance of CAknsBasicBackgroundControlContext
    */
    CAknsBasicBackgroundControlContext* iBgContext;
    /*
	* iSearchTextBox
	* Instance for CAknSearchField;
	*/
	CAknSearchField* 			iSearchBox;	
  	/*
	* iFindString
	* Used to Get the Entered Search string
	*/
	TBuf<KSearchBoxTextLength> iFindString;
    /*
    * iAdvString
    * sets the advertisement string
    */
    HBufC* iAdvString;
    /*
    * iListBoxArray
    * Instance of MIRListBoxArray
    */
   	MIRListBoxArray* iListBoxArray;
   	/*
   	* iRequestForAdvertisement
   	* sets when the request for advertisment conversation is made
   	*/
	TBool iRequestForAdvertisement;
   	/*
   	* iLogoDownloadError
   	* sets when the  advertisment future is not supported
   	*/
	TBool iLogoDownloadError;
    /**
    * iAdRequestStatusFlag
    * sets when a request for advertisement is made.
    */
	TBool iAdRequestStatusFlag;
	/*
	* iFilteredFlag
	* This flag is set when dynamic filtering is done and
	* will be reseted when the characters in the filter box is deleted
	*/
	TBool iFilteredFlag;
	
	TInt iError;

    };

#endif      // CIRSEARCHRESULTSVIEWCONTAINER_H

