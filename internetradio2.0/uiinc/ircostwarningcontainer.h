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
* Description:  Header for CIRCostWarningContainer
*
*/



#ifndef CIRCOSTWARNINGCONTAINER_H
#define CIRCOSTWARNINGCONTAINER_H

#include <aknlists.h>
#include <eikrted.h>

class CIRCostWarningView;

/**
 * Container for cost warning View.
 */
class CIRCostWarningContainer :public CCoeControl 
        
    {
    
public:  // Methods

    enum TIRTermsTextType
        {
        EIRTermsTextInvalid,
        EIRTermsTextHeading,
        EIRTermsTextNormal
        };
	/**
	* NewL.
	* Two-phased constructor.
	* Create a CIRPlsContainer object, which will draw itself to aRect.
	* @param aRect The rectangle this view will be drawn to.
    * @param aView which is a plsview's reference
    * @return a pointer to the created instance of CIRPlsContainer.
	*/
	static CIRCostWarningContainer* NewL(const TRect& aRect ,CIRCostWarningView &aView );

	/**
	* NewLC.
	* Two-phased constructor.
	* Create a CIRPlsContainer object, which will draw itself
	* to aRect.
	* @param aRect Rectangle this view will be drawn to.
	* @param aView which is a plsview's reference
	* @return A pointer to the created instance of CIRPlsContainer.
	*/
	static CIRCostWarningContainer* NewLC(const TRect& aRect,CIRCostWarningView &aView);
	
    /**
     * 2nd phase constructor.
     *
     */
    void ConstructL( const TRect& aRect );
    /**
     * C++ default constructor.
     *
	 * @param aView View of this container.
     */
    CIRCostWarningContainer(CIRCostWarningView &View);
    
    /**
     * Destructor.
     */
    ~CIRCostWarningContainer();

		
	// from base class CCoeControl

	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
     /**
	 * TandCAcceptL().
	 * For setting the terms and conditions flag
	 */
    void TandCAcceptL() const;
    
    /**
	 * CostWarningContinueL().
	 * For displaying the soft keys
	 */
    void CostWarningContinueL();
     
    /**
	 * UpdateTermsL().
	 * To update the required data whenever called
	 */
	void UpdateTermsL();
	
	/**
	 * CreateEditorL().
	 * @no param .
	 * Creates the CEikRichTextEditor object
	 */
	CEikRichTextEditor* CreateEditorL() const;
	
    
    /**
	 * SetReadOnly().
	 * @aReaOnly param as ETrue or EFalse .
	 * Making the editor as ReadOnly
	 */
    void SetReadOnly( TBool aReadOnly );
    
    /**
	 * SetDataContentL().
	 * @aText param is RichText object .
	 * Setting the RichText into the Editor
	 */
    void SetDataContentL( CRichText& aText );
    
    /**
	 * CreateRichTextDataL().
	 * @no param .
	 * Creating the CRichText object
	 */
    void CreateRichTextDataL();
    
    /**
	 * GetEditor().
	 * @no param .
	 * Getting the contents of RichText to the Editor
	 */
    CRichText* GetEditor() const;
    
    /**
	 * HandlePointerEventL().
	 * Handles pointer events.
	 */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    


private: //Methods
    
    /**
	 * ReadTermsFromFileL.
	 * @no param .
	 * Reads the text from a HTML file
	 */
    void ReadTermsFromFileL();
     
    /**
	 * LayoutTermsL
	 * @no param
	 * Inserts the text into the RichText object (Heading of Terms & Conditions). 
	 */
     void LayoutTermsL();
     
    /**
	 * GetNextTermsL
	 * @param aCounter,aTermsText,aTermsType
	 */
     TBool GetNextTermsL(TInt& aCounter, TPtrC& aTermsText, TIRTermsTextType& aTermsType);
     
    // from base class CCoeControl
	/**
     * From CCoeControl
     *
     * @see CCoeControl::Draw( const TRect& aRect ) const
	 */
	void Draw( const TRect& aRect ) const;
	
	/**
	 * CountComponentControls()
	 * @no param .
	 */
	TInt CountComponentControls() const;
	
	/**
	 * SizeChanged().
	 * @no param .
	 */
	void SizeChanged();
	
	/**
	 * ComponentControl().
	 * @aIndex as  param .
	 */
	CCoeControl* ComponentControl(TInt aIndex) const;
	
	/**
	 * HandleResourceChange().
	 * Handles Changes in the Resources
	 */
	void HandleResourceChange(TInt aType);
	
    


private:    // Data

    /**
	 * iTermsData.
	 * Data extracted from the HTML file.
	 */
	HBufC* iTermsData;
	
    /**
	 * iBackGroundControlContext.
	 * 
	 */
	CAknsBasicBackgroundControlContext* iBackGroundControlContext;
	
	// Array of fonts of terms data.
	CArrayFix<TAknLogicalFontId>* iTermsFonts;

	// Array of lines of terms data.
	CArrayFix<TPtrC>* iTermsLines;
	
    /**
	 * iTotalTextHeight.
	 * 
	 */
	TInt iTotalTextHeight;
	
    /**
	 * iMsgView.
	 * A reference to CIRCostWarningView view .
	 */
	CIRCostWarningView	&iMsgView;
	
    /**
	 * iEditor.
	 * CEikRichTextEditor object .
	 */
	CEikRichTextEditor* iEditor; 
    /**
	 * iRichText.
	 * RichText object
	 */
	CRichText* iRichText;
	
	
    };

#endif      // CIRCOSTWARNINGCONTAINER_H
