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
* Description:  Header for CIRStationDirectoryContainer
*
*/


#ifndef CIRSTATIONDIRECTORYCONTAINER_H_
#define CIRSTATIONDIRECTORYCONTAINER_H_

#include <aknlists.h>
#include <EIKLBX.H>
#include <aknsfld.h> 
#include <eikclb.h> 

class CIRStationDirectoryView;
class CAknDoubleStyleListBox;
/**
 * Container for Channel List View.
 */
class CIRStationDirectoryContainer : public CCoeControl , public MEikListBoxObserver    
    {
    
public:  // Methods
    
    /**
    * NewL.
    * Two-phased constructor.
    * Create a CIRStationDirectoryContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aStationsDirView The StationDirectory view's instance.
    * @return a pointer to the created instance of CIRStationDirectoryContainer.
    */
    static CIRStationDirectoryContainer* NewL( CIRStationDirectoryView& aStationsDirView,
    					 const TRect& aRect );

    /**
    * NewLC.
    * Two-phased constructor.
    * Create a CIRStationDirectoryContainer object, which will draw itself to aRect
    * @param aRect The rectangle this view will be drawn to.
    * @param aStationsDirView The StationDirectory view's instance.
    * @return a pointer to the created instance of CIRStationDirectoryContainer.
    */
    static CIRStationDirectoryContainer* NewLC( CIRStationDirectoryView& aStationsDirView,
    			 const TRect& aRect );
    /**
     * Second-phase constructor.
     * 
     * @param   aRect    Rect for Station  Direcotry Container
     */
    void ConstructL( const TRect& aRect );

    /**
     * C++ default constructor.
     */
    CIRStationDirectoryContainer(CIRStationDirectoryView& aStationsDirView );

    /**
     * Destructor.
     */
    ~CIRStationDirectoryContainer();   
    
    
    /**
      * From CCoeControl
      */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
     * From CCoeControl
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    /**
    * GetCurrentItemIndex
    * Returns the Current Selected index of the ListBox
    */
	TInt GetCurrentItemIndex() const;

    /**
     * From MEikListBoxObserver HandleListBoxEventL
     */ 
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
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
    
private: //Methods


	// from base class CCoeControl
	/**
     * From CCoeControl
     *
     * @see Draw( const TRect& aRect ) const
	 */
	void Draw( const TRect& aRect) const;

public: //Methods

    /**
     * 
     * Sets items to the listbox.
     */
    void PopulateStationListL();
    
    /**
    * CreateDiscoverStationDataL(TInt aTitle,TInt aDescription)
    * Fetches the data from stringloader that should be displayed
    * and sets it to the listbox.
    */
    void CreateDiscoverStationDataL(TInt aTitle,TInt aDescription);

private:    // Data
    
    
    /**
     * List box used to display the list of items.
     * Owned.
     */
     CDesCArray* iItemArray;
     
    /**
    * iStationDirView
    * reference of the stations directory view
    */
    CIRStationDirectoryView &iStationDirView;
    
    /**
    * iListbox
    * Data for the StationDirectory Listbox
    */
    CAknDoubleStyleListBox* iListbox;
   
};

#endif /*CIRSTATIONDIRECTORYCONTAINER_H_*/
