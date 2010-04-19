/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listbox implementation supporting advertisement element. 
*
*/


#ifndef IRADVERTISINGLISTBOX_H
#define IRADVERTISINGLISTBOX_H

#include <aknlists.h>
#include <eikfrlb.h>
 


class CIRAdvertisingListBox;
class CIRCategoryAdvertisingListBox;

//class CIRFilteredModel;
/**
 * CIRAdvertisingListBoxData
 * 
 * Extends the functionality of CFormattedCellListBoxData by providing placeholder
 * for advertisement bitmaps.
 * 
 */
class CIRAdvertisingListBoxData : public CFormattedCellListBoxData
    {
public:
    /**
     * Static constructor.
     */
    static CIRAdvertisingListBoxData* NewL();
    
    /**
     * Sets the current advertisement icon. Deletes the previous if it exists.
     * 
     * @param aIcon The current advertisement icon to be displayed. Takes ownership.
     */
    void SetAdvertisementIcon( CGulIcon* aIcon );
    
    /**
     * Gets the current advertisement icon.
     * 
     * @return Current advertisement icon. Ownership is not transferred.
     */
    CGulIcon* AdvertisementIcon();
    
	/*
	* DrawEmptyListL
	* Draws the text on the listbox
	*/
	void DrawEmptyListL(CWindowGc &aGc,TPtrC aText,TRect aParentRect) const;
	/*
	* DrawEmptyListDrawUpToTwoLinesL
	* Draws the text on the listbox
	*/
	void DrawEmptyListDrawUpToTwoLinesL(CWindowGc &aGc,
										  TPtrC aText,
                                          TAknLayoutText &aLine1,
                                          TAknLayoutText &aLine2,
                                          TInt aLine1length,
                                          TInt aLine2length,
                                          const CFont* aFont,
                                          TRgb aColor,
                                          TDes& aBuffer,
                                          TRect aParentRect) const;

	/*
	* DrawEmptyListDrawMoreThanTwoLinesL
	* Draws the text on the listbox
	*/
	void DrawEmptyListDrawMoreThanTwoLinesL( CWindowGc &aGc,
												  TRect &aParentRect,
                                                  TPtrC aText,
                                                  TRgb aColor,
                                                  TDes& buffer ) const;
	/**
	* SetControlFlag
	* sets the iControlFlag value
	*/
	void SetControlFlag(TBool aControlFlag);

	/**
	* GetControlFlag
	* gets the iControlFlag value
	*/
	TBool GetControlFlag();
private:
    /**
     * C++ default constructor
     */
    CIRAdvertisingListBoxData();
    
    /**
     * Destructor.
     */
    ~CIRAdvertisingListBoxData();    
    
    /**
     * The advertisement icon. 
     * Owned.
     */
    CGulIcon* iIcon;
    
	/**
	* iControlFlag
	* holds whether controls have been created
	*/
    TBool iControlFlag;

    };


/**
 * CIRAdvertisingListBoxDrawer
 * 
 * Draws the advertisement icon owned by the CIRAdvertisingListBoxData.
 * 
 */
class CIRAdvertisingListBoxDrawer : public CFormattedCellListBoxItemDrawer
    {
public:
    
    /**
     * C++ constructor.
     * 
     * @param aTextListBoxModel The model od the associated listbox
     * @param aFont The font to be used in drawing.
     * @param aFormattedCellData The listbox data to be drawn.
     */ 
    CIRAdvertisingListBoxDrawer( MTextListBoxModel* aTextListBoxModel,
                                 const CFont* aFont,
                                 CIRAdvertisingListBoxData* aFormattedCellData,
                                 CIRAdvertisingListBox* aListBox =NULL,
                      			 CIRCategoryAdvertisingListBox* aCatListBox =NULL );

private:
    // From CTextListItemDrawer

    /**
     * Draws the advertisement and then lets CTextListItemDrawer to draw the 
     * actual list items.
     * 
     * @see CTextListItemDrawer::DrawActualItem
     */
    void DrawActualItem( TInt aItemIndex, 
                         const TRect& aActualItemRect, 
                         TBool aItemIsCurrent, 
                         TBool aViewIsEmphasized, 
                         TBool aViewIsDimmed,
                         TBool aItemIsSelected ) const;
private:                         
                         
    /*
    * iListBox
    * instance of CCustomListBox
    */
   CIRAdvertisingListBox* iListBox;
   
   /**
    * iCatListBox
    * instance of CIRCategoryCustomListBox
    */
   CIRCategoryAdvertisingListBox* iCatListBox;
                     
    };

/**
 * CIRAdvertisingListBox
 * 
 * Listbox displaying a special advertisement item at the top of the list.
 * Provides accessor methods for getting and setting the advertisement icon, 
 * to expose neat interface towards callers.
 * 
 * Currently supports displaying of only one advertisement.
 */
class CIRAdvertisingListBox : public CAknDoubleLargeStyleListBox
    {
public:
    /**
     * C++ default constructor.
     */ 
    CIRAdvertisingListBox();
    
    /**
     * Destructor
     */
    ~CIRAdvertisingListBox();

    /**
     * Sets the format used in the super class to represent a list item.
     * 
     * @param aItemFormat The list item format
     */
    void SetListItemFormat( HBufC* aItemFormat);
    
    /**
     * Sets the advertisement for the listbox.
     * 
     * If there is no advertisement element present in the listbox, 
     * inserts it as a new element on top of the list. Otherwise, only updates
     * the advertisement icon.
     * 
     * @param aIcon The advertisement icon. Ownership is transferred. 
     */
    void SetAdvertisementIconL( CGulIcon* aIcon,TBool aMainView = EFalse );
    
    /**
     * Clears (deletes) the current advertisement icon. 
     */
    void ClearAdvertisementIcon();
    
    /**
     * Gets the current advertisement icon.
     * 
     * @return The current advertisement icon. Ownership is not transferred.
     */
    CGulIcon* AdvertisementIcon();        
     /*
    * ConstructL()
    * constructs the listbox
    */
	void ConstructL(const CCoeControl* aParent, 
					TInt aFlags,CAknFilteredTextListBoxModel* aFilteredmodel);
       
	/**
	* SetLastPlayed()
	* Sets the status of lastplayed i.e., whether lastplayed is present r not 
	*/
	void SetLastPlayed(TBool aLastPlayed);
	/**
	* GetLastPlayed
	* returns the status of lastplayed i.e., whether lastplayed is present r not
	*/
	TBool GetLastPlayed() const;
	/**
	* GetFavouritesCount
	* returns the number of stations present in the favourites preset.
	*/
	TInt  GetFavouritesCount() const;
	/**
	* GetActiveView
	* returns whether the mainview is active r not.
	*/
	TBool GetActiveView() const;
	/**
	* GetFilteredFlag()
	* returns  the filtered flag 
	*/
	TBool GetFilteredFlag() const;
	/*
	* Draw
	*/
	void Draw(const TRect& aRect) const ;

	/*
	* SetGC
	* Sets the Graphic Context
	*/
	void SetGC(CWindowGc& aGc);
	/*
	* SetControlFlag
	* Tests whether all controls are created
	*/
	void SetControlFlag(TBool aControlFlag);

private:
    // From CAknDoubleLargeStyleListBox

    /**
     * @see CAknDoubleLargeStyleListBox::CreateItemDrawerL
     */
    void CreateItemDrawerL();

    /**
     * The format of the list item.
     * Owned. 
     */
    HBufC* iItemFormat;
    /*
    * iLastPlayed
    * 
    */
	TBool iLastPlayed;
    /**
     * iNoMatchesTextResource
     * Data for the resource string 
     */
	HBufC* iNoMatchesTextResource;
	
    /**
     * iFindStationsTextResource
     * Data for the resource string 
     */
	HBufC* iFindStationsTextResource;
    /**
     * iGc
     * Data for the Graphics Context 
     */
	CWindowGc* iGc;
    };

//  Advertisement Listbox for Category view
class CIRCategoryAdvertisingListBox : public CAknDoubleStyleListBox
    {
public:
    /**
     * C++ default constructor.
     */ 
    CIRCategoryAdvertisingListBox();
    
    /**
     * Destructor
     */
    ~CIRCategoryAdvertisingListBox();

    /**
     * Sets the format used in the super class to represent a list item.
     * 
     * @param aItemFormat The list item format
     */
    void SetListItemFormat(  HBufC* aItemFormat );
    
    /**
     * Sets the advertisement for the listbox.
     * 
     * If there is no advertisement element present in the listbox, 
     * inserts it as a new element on top of the list. Otherwise, only updates
     * the advertisement icon.
     * 
     * @param aIcon The advertisement icon. Ownership is transferred. 
     */
    void SetAdvertisementIconL( CGulIcon* aIcon );
    
    /**
     * Clears (deletes) the current advertisement icon. 
     */
    void ClearAdvertisementIcon();
    
    /**
     * Gets the current advertisement icon.
     * 
     * @return The current advertisement icon. Ownership is not transferred.
     */
    CGulIcon* AdvertisementIcon();        
     /*
    * ConstructL()
    * constructs the listbox
    */
	void ConstructL(const CCoeControl* aParent, 
					TInt aFlags,CAknFilteredTextListBoxModel* aFilteredmodel);
       
	/*
	* Draw
	*/
	void Draw(const TRect& aRect) const ;
	/*
	* SetGC
	* Sets the Graphic Context
	*/
	void SetGC(CWindowGc& aGc);
	/*
	* SetControlFlag
	* Tests whether all controls are created
	*/
	void SetControlFlag(TBool aControlFlag);

private:
    // From CAknDoubleStyleListBox

    /**
     * @see CAknDoubleStyleListBox::CreateItemDrawerL
     */
    void CreateItemDrawerL();

    /**
     * The format of the list item.
     * Owned. 
     */
    HBufC* iItemFormat;
    /**
     * iTextResource
     * Data for the resource string 
     */
	HBufC* iTextResource;
    /**
     * iGc
     * Data for the Graphics Context 
     */
	CWindowGc* iGc;

    };
#endif /*IRADVERTISINGLISTBOX_H*/
