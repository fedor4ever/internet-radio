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
* Description:  Header for CIRStationsViewContainer
*
*/


#ifndef CIRSTATIONSVIEWCONTAINER_H
#define CIRSTATIONSVIEWCONTAINER_H

#include <aknlists.h>
#include <EIKLBX.H>
#include <aknsfld.h> 
#include <eikclb.h> 

#include "irimageconverterobserver.h"

class CIRStationsView;
class CIRStationsViewContainer;
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
class CIRStationsViewContainer : public CCoeControl/*public CAknDoubleStyleListBox*/ , public MEikListBoxObserver,
                            public MIRImageConverterObserver
    {
    
public:  // Methods

    
    /**
    * NewL.
    * Two-phased constructor.
    * Create a CIRStationsViewContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aView The Stations/TopStations view's instance.
    * @return a pointer to the created instance of CIRStationsViewContainer.
    */
    static CIRStationsViewContainer* NewL( CIRStationsView& aView, const TRect& aRect );

    /**
    * NewLC.
    * Two-phased constructor.
    * Create a CIRStationsViewContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aView The Stations/TopStations view's instance.
    * @return a pointer to the created instance of CIRStationsViewContainer.
    */
    static CIRStationsViewContainer* NewLC( CIRStationsView& aView, const TRect& aRect );
    /**
     * ConstructL
     * 2nd phase constructor.
     */
     void ConstructL( const TRect& aRect);
    
	/**
     * CIRStationsViewContainer()
     * C++ default constructor.
	 * @param aView View of this container.
     */
    CIRStationsViewContainer(CIRStationsView& aView);
    
    /**
     * ~CIRStationsViewContainer()
     * Destructor.
     */
    ~CIRStationsViewContainer();

	/**
     * UpdateSearchResultsViewL()
     * updates the search results data to the search results list
     */
 	void UpdateStationsViewL();
 	
	/**
	* HandleLogoErrorL()
	* called from the iStationsView when the logo is not downloaded
	*/
	void HandleLogoErrorL();

 	
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
	/*
	* CreateFindBoxL()
	* Creates the find box for dynamic filtering
	*/
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,
	CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
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
 	* PresetLogoDownloadL()
 	* Used to download the  logos depending the preset values.
 	*/
 	void PresetLogoDownloadL(CIRIsdsPreset* aPreset);	
 	/**
 	* CreateCustomListBoxL()
 	* Creates the custom listbox and updates the list according to that
 	*/
	void CreateCustomListBoxL();
 	/**
 	* CreateListBoxL()
 	* Creates ListBox depending upon the presence of Advertisement
 	*/
	void CreateListBoxL();
 	/**
 	* OpenUrlL()
 	* Used to connect to advertisement url
 	*/
	void OpenUrlL( const TDesC& aHttpLink );
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
 	/*
 	 * GetFilterdModelPointer()
	 * returns pointer to filtered model.
	 */
 	CIRFilteredModel* GetFilterdModelPointer();
 	/*
 	 * GetSearchBoxPointer()
	 * returns the search box pointer.
	 */
 	CAknSearchField* GetSearchBoxPointer();
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
    /**
    * Formats list item string with given arguments
    * 
    * @param aFirstRow text for first row
    * @param aSecondRow text for second row
    * @param aIconId Icon ID used on item
    */
    HBufC* FormatItemStringLC( const TDesC& aFirstRow,  TInt aIconId );
	/*
	* iFilteredFlag
	* This flag is set when dynamic filtering is done and
	* will be reseted when the characters in the filter box is deleted
	*/
	TBool iFilteredFlag;

private:	 
	 /**
	 * iItemArray
	 * array for the listbox model
	 */ 
	 CDesCArray* iItemArray;
	 /**
	 * iView
	 * Data for CIRStationsView view
	 */ 
	 CIRStationsView& iView;
	
	/*
	* iCurrentItemConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iCurrentItemConverter;

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
	
	
	CFbsBitmap*     iAdBitmap;
    /**
    * iAdRequestStatusFlag
    * sets when a request for advertisement is made.
    */
	TBool iAdRequestStatusFlag;
    /*
	* iConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iConverter;

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
   	/*
   	* iLandScapeCount
   	* used to call the aknfind s HandlePopupFindSizeChanged function only once
   	*/
	TInt iLandScapeCount;
	 
	/*
	* iSearchTextBox
	* Instance for CAknSearchField;
	*/
	CAknSearchField* 			iStationsSearchBox;	
public:
	 
	 /**
	 * iStationsListBox
	 * Instance of the Stations ListBox
	 */
 	CIRAdvertisingListBox* iStationsListBox;
	/**
	* Temporary preset to store url and the logo data
	*/
	CIRIsdsPreset *iTempPreset;	 
  	/*
	* iFindString
	* Used to Get the Entered Search string
	*/
	TBuf<KSearchBoxTextLength> iFindString;
	/**
	* Temporary preset to store the raw data of Advertisements
	*/
	CIRIsdsPreset*  iAdvPreset;
	TInt iError;
   };

#endif      // CIRSTATIONSVIEWCONTAINER_H
