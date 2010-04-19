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
* Description:  Header for CIRPlsContainer
*
*/


#ifndef CIRPLSCONTAINER_H
#define CIRPLSCONTAINER_H

#include <aknlists.h>
#include <eikclb.h>
#include <eikclbd.h>

class CIRPlsView;
class CIRIsdsPreset;
class CIRDialogLauncher;
class CEikColumnListBox ;

class CIRPlsContainer : public CCoeControl,public MEikListBoxObserver
					
    
    {
    
public:  // Methods

	/**
	* NewL.
	* Two-phased constructor.
	* Create a CIRPlsContainer object, which will draw itself to aRect.
	* @param aRect The rectangle this view will be drawn to.
    * @param aView which is a plsview's reference
    * @return a pointer to the created instance of CIRPlsContainer.
	*/
	static CIRPlsContainer* NewL(const TRect& aRect ,CIRPlsView &aView );

	/**
	* NewLC.
	* Two-phased constructor.
	* Create a CIRPlsContainer object, which will draw itself
	* to aRect.
	* @param aRect Rectangle this view will be drawn to.
	* @param aView which is a plsview's reference
	* @return A pointer to the created instance of CIRPlsContainer.
	*/
	static CIRPlsContainer* NewLC(const TRect& aRect,CIRPlsView &aView);
	/**
	* ~CIRPlsViewContainer
	* Destructor.
	*/
	~CIRPlsContainer();
	
    
	/*
	* 2nd phase constructor.
	*/
	void ConstructL( const TRect& aRect );
	
	/**
	* CIRPlsContainer()
	* Default Constructor.
	*/
	
	CIRPlsContainer(CIRPlsView& aView);
	
    /**
	* GetCurrentItemIndex()
	* To get the current Item index.
	*/
	TInt GetCurrentItemIndex() const;
	
    /**
	* ListenToStationL()
	* Function to listening to a station.
	*/
	void ListenToStationL();
	
    /**
	* CreatePresetL()
	* Creating a preset object
	*/
	CIRIsdsPreset* CreatePresetL();
	
    /**
	* CreatePresetListL()
	* Creating a Preset List
	*/
	void CreatePresetListL();
	
    /**
	* SaveL()
	* Save a channel name to favorites DB.
	*/
	void SaveL();
	
	/**
	* SaveAllL()
	* Save  channel names to favorites DB.
	*/
	void SaveAllL();
    
	/**
	* Updates all items of the list by reading data from preset data base.
	*/
	void UpdateAllL() ;

    /**
	* OfferKeyEventL() from CCoeControl
	* 
	*/
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
	
	/**
	* HandlePointerEventL
	* Handles all the Pointer events
	*/
	void HandlePointerEventL( const TPointerEvent& aPointerEvent );
	

	/**
	* From CCoeControl
	*
	* @see CCoeControl::GetHelpContext( TCoeHelpContext& aContext ) const
	*/
	void GetHelpContext( TCoeHelpContext& aContext ) const;
	
    /**
	* HandleListBoxEventL()
	* Handles any event on the List Box.
	*/
	void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
	
    /**
	* GetCount()
	* Gets the count of the List.
	*/
	TInt GetCount() const;
	
    /**
	* GetListBoxTopIndex()
	* Gets the TopItemIndex.
	*/
	TInt GetListBoxTopIndex() const;
    
    /**
	* SecondPls()
	*/
	void SecondPls();
	
    /**
	* Handles a change to the control's resources.
	*/
    void HandleResourceChange(TInt aType);



private: //Methods

	// from base class CCoeControl
	/**
	* From CCoeControl
	*
	* @see CCoeControl::Draw( const TRect& aRect ) const
	*/
	void Draw( const TRect& aRect ) const;
	
    /**
	* DisplayErroronDBL()
	* Displays error if favorites db is full.
	*/ 
	void DisplayErroronDBL();
	
    /**
	* DisplayErrorOnNoItemsL()
	* Displays error if no streaming links
	*/
	void DisplayErrorOnNoItemsL();
	
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
	* Called by framework when the view size is changed.
	*/
    void SizeChanged();
    



private:    // Data

	/** 
	* Parent view of this container. 
	*/
	CIRPlsView&	iPlsView;

	// bool to check if all stations saved	
	TBool iSavedAllUrl;    

	/**
	* This array holds the list of preset objects to be saved
	*/    
	RPointerArray<CIRIsdsPreset> iPresetList;

	/**
	* Indicates if the preset list is present or not
	*/
	TBool iPresetListPresent;
    
	/*
	* iIsPresetSaved
	* array used to check whether the preset is saved
	*/
	RArray<TInt> iIsPresetSaved;
	
	/*
	* iPlsListBox
	* CAknSingleStyleListBox listbox for the PLS list.
	*/
	CEikColumnListBox *iPlsListBox;
	/*
	* iListArray
	* Pls List 
	*/
	CDesCArray* iListArray;   

	
	};

#endif      // CIRPlsContainer_H
