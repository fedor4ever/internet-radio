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
* Description:  Header for CIRCategoryViewContainer
*
*/


#ifndef CIRCATEGORYVIEWCONTAINER_H
#define CIRCATEGORYVIEWCONTAINER_H

#include <aknlists.h>
#include <EIKLBX.H>
#include <aknsfld.h> 
#include <eikclb.h> 
#include "irimageconverterobserver.h"


class CIRCategoryView;
class CIRCategoryViewContainer;
/**
 * This classes are used for Advertisements
 */
class CIRCategoryCustomListBox;
class CIRImageConverter;
class CIRFilteredModel;
class MIRListBoxArray;
class CIRCategoryAdvertisingListBox;
/**
 * Container for Search Results View.
 */
class CIRCategoryViewContainer : public CCoeControl/*public CAknDoubleStyleListBox*/ , public MEikListBoxObserver,
                            public MIRImageConverterObserver
    
    {
    
public:  // Methods

    /**
    * NewL.
    * Two-phased constructor.
    * Create a CIRCategoryViewContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aCtaegoryView The Category view's instance.
    * @return a pointer to the created instance of CIRCategoryViewContainer.
    */
    static CIRCategoryViewContainer* NewL( CIRCategoryView& aCtaegoryView, const TRect& aRect );

    /**
    * NewLC.
    * Two-phased constructor.
    * Create a CIRCategoryViewContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aCtaegoryView The Category view's instance.
    * @return a pointer to the created instance of CIRCategoryViewContainer.
    */
    static CIRCategoryViewContainer* NewLC( CIRCategoryView& aCtaegoryView, const TRect& aRect );
    
    /**
     * ConstructL
     * 2nd phase constructor.
     */
     void ConstructL( const TRect& aRect);
    
	/**
     * CIRCategoryViewContainer()
     * C++ default constructor.
	 * @param aView View of this container.
     */
    CIRCategoryViewContainer(CIRCategoryView& aCtaegoryView);
    
    /**
     * ~CIRCategoryViewContainer()
     * Destructor.
     */
    ~CIRCategoryViewContainer();

	/**
     * UpdateSearchResultsViewL()
     * updates the search results data to the search results list
     */
 	void UpdateCategoryViewL(CDesCArray* aCategory);
 	
 	
	// from base class CCoeControl

     /**
	 * From CCoeControl
	 *
	 * @see CCoeControl::OfferKeyEventL() 
	 */    
	
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
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
     * From MEikListBoxObserver HandleListBoxEventL
     */ 
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
    
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
	void SizeChanged(/*const TRect& aRect*/);
	
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
	* Function : StartTimer
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
	* HandleLogoErrorL()
	* called from the iCategoryView when the logo is not downloaded
	*/
	void HandleLogoErrorL();
private: //Methods


	// from base class CCoeControl
	/**
     * From CCoeControl
     * @see CCoeControl::Draw( const TRect& aRect ) const
	 */
	void Draw( const TRect& aRect ) const;

private:    // Data
    /**
    * iCategoryView
    * reference of the category view
    */
    CIRCategoryView &iCategoryView;
	CFbsBitmap*     iAdBitmap;
	/*
	* iCurrentItemConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iCurrentItemConverter;
   	/*
   	* iLandScapeCount
   	* used to call the aknfind s HandlePopupFindSizeChanged function only once
   	*/
	TInt iLandScapeCount;
	TBool iFilteredFlag;
	TInt iError;
public:
    
  	 /**
	 * iItemArray
	 * array for the listbox model
	 */ 
	 CDesCArray* iItemArray;
	 /**
	 * iCategoryListBox
	 * Instance of the Category ListBox
	 */
  	CIRCategoryAdvertisingListBox* iCategoryListBox;
  	/*
	* iFindString
	* Used to Get the Entered Search string
	*/
	
	TBuf<KSearchBoxTextLength> iFindString;
	/*
	* iSearchTextBox
	* Instance for CAknSearchField;
	*/
	CAknSearchField* 			iSearchTextBox;	
	/*
	* iFilteredModel
	* Instance for CAknFilteredTextListBoxModel;
	*/
	CIRFilteredModel* iFilteredModel;

    /*
	* iConverter
	* instance of CIRImageConverter
	*/
	CIRImageConverter* iConverter;
	/**
	* Temporary preset to store the raw data of Advertisements
	*/
	CIRIsdsPreset*  iAdvPreset;
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
    };

#endif      // CIRCATEGORYVIEWCONTAINER_H
