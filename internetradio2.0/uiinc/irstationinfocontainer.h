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
* Description:  Header for CIRStationInfoContainer
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Thu Jul 3 16:30:00 2008 by Rohit
*  Ref:
*  Codescanner fixes
*
*  Version: 2, Thu May 22 19:20:00 2008 by Rohit
*  Ref:
*  Implemented StationInfo landscape view. Adjusted line spacings per UI spec.
*
*  Version: 1, Wed Apr 30 16:15:00 2008 by Rohit
*  Ref:
*  Created. Implemented StationInfo potrait view
*
*  </ccm_history>
* ============================================================================
*/

#ifndef CIRSTATIONINFOCONTAINER_H
#define CIRSTATIONINFOCONTAINER_H

//  INCLUDES
#include <coecntrl.h>                       // CCoeControl
#include <txtfrmat.h>                       // TCharFormat, TCharFormatMask
#include <ImageConversion.h>

#include "IRStationInfoView.h"
#include "irimageconverterobserver.h"

// FORWARD DECLARATIONS
class CAknsBasicBackgroundControlContext;
class CEikRichTextEditor;
class CFbsBitmap;
class CIRImageConverter;

/**
 * Container for Station information View.
 */
class CIRStationInfoContainer : public CCoeControl, public MIRImageConverterObserver
{
public:  // Constructors and destructor

	static CIRStationInfoContainer* NewL( const TRect& aRect, CIRStationInfoView* aView );

	/**
	* Destructor.
	*/
	virtual ~CIRStationInfoContainer();

public: // Functions from base classes

	/**
	* From CoeControl, returns the number of control contained by this class.
	* @return Number of controls contained
	*/
	TInt CountComponentControls() const;

	/**
	* From CCoeControl, returns handle to control pointed by aIndex
	* @param aIndex Wanted control's index [0..n]
	* @return Handle to wanted control
	*/
	CCoeControl* ComponentControl( TInt aIndex ) const;

	/**
	* From CCoeControl, Handles key events
	* @param aKeyEvent The key event
	* @param aType The type of key event (key, key up or key down)
	* @return Indicates whether or not the key event was used by this control.
	*/
	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

	/**
     * MIRImageConverterObserver::HandleImageConversionEventL()
     * @param   aEvent  The event that occurred.
     * @param   aId     Identifier for the conversion.
     * @param   aError  One of the standard system error codes.
	 */
	void HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent aEvent, 
									  TInt aId, TInt aError );

	/**
	* Issues logo scaling request
	*/
	void StartConvertStationLogoL();

	/**
	* Renders the logo and text in the view as per the current resolution
	*/
	void ReloadStationInformationL();

protected: // Functions from base classes

	/**
	* From CCoeControl, used for skins
	* @param aId An encapsulated object type id
	* @return Pointer to the object provided.
	*/
	TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

	/**
	* From CoeControl
	* Called by the framework when container size changes
	*/
	void SizeChanged();

	/**
	* From CoeControl
	* Handles a change to the control's resources
	* @param aType A message UID value
	*/
	void HandleResourceChange( TInt aType );

private:   // Functions

	/**
	* C++ default constructor.
	*/
	CIRStationInfoContainer( CIRStationInfoView* aView);

	/**
	* By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL( const TRect& aRect );

	/**
	* Overridden from CCoeControl.
	* @param aRect Rectangle which needs drawing
	*/
	void Draw( const TRect &aRect ) const;

	/**
	* Initializes the rich text with font, color, anti-aliasing
	*/
    void CreateRichTextDataL();

	/**
	* Instantiates RichTextEditor
	* @return Initialized RichTextEditor
	*/
	CEikRichTextEditor* CreateEditorL();

	/**
	* Renders the logo in the view as per the current resolution
	* @param aEditor 		RichTextEditor ref
	* @param aIsLandscape	True if orientation is landscape, else False
	*/
	void DisplayStationLogoL( CEikRichTextEditor* aEditor, TBool aIsLandscape = EFalse);

	/**
	* Reads station info from preset and renders the text in the view
	* @param aIsLandscape	True if orientation is landscape, else False
	*/
	void DisplayStationInfoL( TBool aIsLandscape = EFalse );

	/**
	* @return true if station logo available, else false.
	*/
	TBool IsStationSupportsLogo();


	/**
	* @return true if station is from ISDS, false if user-defined.
	*/
	TBool IsISDSStation();

private:    // Data

	// Skin background context.
	CAknsBasicBackgroundControlContext* iBgContext;

	// RichTextEditor to show logo in landscape orientation
	CEikRichTextEditor* iEditorLogo;
	
	// RichTextEditor to show logo/text in potrait orientation, 
	// text in landscape orientation
	CEikRichTextEditor* iEditor;

    // the scaled bitmap
	CFbsBitmap* iBitmap;

    // the scaled bitmap mask
	CFbsBitmap* iBitmapMask;

	// view reference
	CIRStationInfoView *iStationInfoView;

	// API wrapper to convert and scale image
	CIRImageConverter* iConverter;

	// parent rect
	TRect	iClientRect;
	
	// Set When Any error during Image Conversion
	TInt iError;

	TBool iImageDrawn;

    };

#endif      // EVEDETAILCONTAINER_H

// End of File
