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
* Description:  Header for CIRFindStationsContainer
*
*/


#ifndef C_CIRFIRSTTIMECONTAINER_H_
#define C_CIRFIRSTTIMECONTAINER_H_

#include <e32base.h>
#include <eiklbo.h>
#include <aknlists.h>


class CIRFirstTimeView;
class CAknDoubleStyleListBox;


/**
 * Container for First Time View.
 */
class CIRFirstTimeContainer :  public CCoeControl, public MEikListBoxObserver
    {
    
public:  // Methods
    
    /**
     * Second-phase constructor.
     * 
     * @param   aRect    Rect for Find Station Container
     */
    void ConstructL( const TRect& aRect );

    /**
     * C++ default constructor.
     *
     * @param aSetupStationsLaterItem Is Setup Station Later item in the list.
     */
    CIRFirstTimeContainer(CIRFirstTimeView& aFirstTimeView);

    /**
     * Destructor.
     */
    ~CIRFirstTimeContainer( );
    
     /**
      * From CCoeControl
      */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    /**
     * From CCoeControl
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
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
    * GetCurrentItemIndex
    * Returns the Current Selected index of the ListBox
    */
	TInt GetCurrentItemIndex() const;
  
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
     * PopulateFirstTimeListL
     * Sets items to the listbox.
     */
    void PopulateFindStationListL();
    void CreateFindStationDataL(TInt aTitle,TInt aDescription);

private:    // Data
	/**
	* List box used to display the list of items.
	* Owned.
	*/
	CDesCArray* iItemArray;

	/**
	* iFirstTimeView
	* Instance of FirstTimeView
	*/
	CIRFirstTimeView& iFirstTimeView;
	/**
	* iListbox
	* Data for the StationDirectory Listbox
	*/
	CAknDoubleStyleListBox* iListbox;
    
};

#endif /*C_CIRFirstTimeContainer_H_*/
