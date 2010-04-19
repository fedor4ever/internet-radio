/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for the detail view
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 4, Fri Aug 1 16:17:00 2008 by Rohit
*  Ref:
*  Fixed bug - Once StationInfo view is opened for logo station, on listen
*  StationInfo view was shown above NowPlayingView toolbar
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

// INCLUDE FILES

#include <s32std.h>
#include <eikenv.h>
#include <eikapp.H>
#include <eikappui.H>
#include <stringloader.h>
#include <aknutils.h>                     		// AknLayoutUtils
/* no need to change to lower case */
#include <AknsBasicBackgroundControlContext.h> 	// CAknsBasicBackgroundControlContext
#include <aknsskininstance.h> 					// MAknsSkinInstance
#include <aknsdrawutils.h>                		// AknsDrawUtils
#include <eikrted.h>                      		// CEikRichTextEditor
#include <txtrich.h>                      		// CRichText
#include <internetradio.mbg>
#include <internetradio.rsg>

#include "irdebug.h"
#include "irstationinfocontainer.h"
#include "irstationinfoview.h"   		           		 
#include "irisdspreset.h"
#include "irchannelserverurl.h"
#include "irui.h"
#include "irimageconverter.h"
#include "irstationinfopicture.h"


// CONSTANT DECLARATIONS
_LIT(Kspace," ");
_LIT(KHttp,"http://");
const TInt KZero    = 0;
const TInt KOne     = 1;
const TInt KTwo    	= 2;
const TInt KBufferSize	= 128;
#define KSI_USERDEFINED 0 	// Indicates that the station is of type user defined
#define KSI_ISDS_DEFINED 1
const TInt KHttpLength 					= 7;

const TInt KLINES_COUNT 				= 50;
const TInt KTEXT_LIMIT  				= 3000;

const TInt KLOGO_WIDTH  				= 200;
const TInt KLOGO_HEIGHT  				= 200;

const TInt KRICHTEXT_ORIGIN  			= 0;
const TInt KRICHTEXT_LENGTH  			= 0;

const TInt KPARA_LINE_SPACING  			= 25;	// twips
const TInt KPARA_SPACING  				= 45;	// twips

const TInt KLEFT_MARGIN_POTRAIT     	= 20;	// twips
const TInt KRIGHT_MARGIN_POTRAIT    	= 10;	// twips

const TInt KLEFT_MARGIN_LANDSCAPE_LOGO   = 0;	// twips
const TInt KLEFT_MARGIN_LANDSCAPE_NOLOGO = 10;	// twips
const TInt KLEFT_LOGO_MARGIN_LANDSCAPE   = 20;	// twips
const TInt KRIGHT_MARGIN_LANDSCAPE  = 10;		// twips

const TInt KHEADINGTEXT_HEIGHT_DECREMENT = -5;	// twips
const TInt KNORMALTEXT_HEIGHT_DECREMENT  = 5;	// twips


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::CIRStationInfoContainer
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIRStationInfoContainer::CIRStationInfoContainer( CIRStationInfoView* aStationInfoView )
  : iBgContext( NULL ),
    iEditorLogo( NULL ),
    iEditor( NULL ),
    iBitmap(NULL), iBitmapMask(NULL),
    iStationInfoView( aStationInfoView ),
    iConverter(NULL), iError(KErrNone),
    iImageDrawn( EFalse )
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::CIRStationInfoContainer" );
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIRStationInfoContainer::ConstructL( const TRect& aRect )
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::ConstructL - Entering" );
    CreateWindowL();
    SetRect( aRect );

    // background context for mop object
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, Rect(), EFalse );

   	iEditor = CreateEditorL();
   	iEditorLogo = CreateEditorL();
	iEditorLogo->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
															CEikScrollBarFrame::EOff );
	iConverter = CIRImageConverter::NewL();
	iConverter->EnableAnimations( ETrue );
	iConverter->SetObserver( this );

    IRLOG_DEBUG( "CIRStationInfoContainer::ConstructL - Exiting" );
    }


// -----------------------------------------------------------------------------
// CIRStationInfoContainer::CreateEditorL
// Instantiates RichTextEditor
// -----------------------------------------------------------------------------
//
CEikRichTextEditor* CIRStationInfoContainer::CreateEditorL()
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::CreateEditorL - Entering" );
    CEikRichTextEditor* editor = new (ELeave) CEikRichTextEditor;
    CleanupStack::PushL( editor );

    editor->SetContainerWindowL(*this);
    editor->ConstructL(this, KLINES_COUNT, KTEXT_LIMIT, CEikEdwin::EReadOnly );

    editor->RichText()->Reset();

    // create vertical scrollbar
    editor->CreateScrollBarFrameL();
    editor->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
    												   CEikScrollBarFrame::EAuto );
	editor->ActivateL();
    CleanupStack::Pop(editor); // editor

    IRLOG_DEBUG( "CIRStationInfoContainer::CreateEditorL - Exiting" );
    return editor;
    }


// -----------------------------------------------------------------------------
// CIRStationInfoContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationInfoContainer* CIRStationInfoContainer::NewL( const TRect& aRect,
														CIRStationInfoView* aView )
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::NewL - Entering" );
    CIRStationInfoContainer* self = new ( ELeave ) CIRStationInfoContainer( aView);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );

	IRLOG_DEBUG( "CIRStationInfoContainer::NewL - Exiting" );
    return self;
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::~CIRStationInfoContainer
// Destructor.
// -----------------------------------------------------------------------------
//
CIRStationInfoContainer::~CIRStationInfoContainer()
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::~CIRStationInfoContainer - Entering" );
	if (iBgContext)
		{
	 	delete iBgContext;
	 	iBgContext = NULL;
		}
	if (iEditor)
		{
	    iEditor->RichText()->Reset();
	   	delete iEditor;
    	iEditor = NULL;
		}
	if (iEditorLogo)
		{
   	    iEditorLogo->RichText()->Reset();
	   	delete iEditorLogo;
    	iEditorLogo = NULL;
		}
	if( iBitmap )
		{
		delete iBitmap;
		iBitmap = NULL;
		}
	if( iBitmapMask )
		{
		delete iBitmapMask;
		iBitmapMask = NULL;
		}
	if( iConverter )
		{
		delete iConverter;
		iConverter = NULL;
		}
	iStationInfoView = NULL;	// not owned
	IRLOG_DEBUG( "CIRStationInfoContainer::~CIRStationInfoContainer - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::CountComponentControls
// From base class CCoeControl
// Returns number of controls contained in a compound control
// -----------------------------------------------------------------------------
//
TInt CIRStationInfoContainer::CountComponentControls() const
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::CountComponentControls" );
    return iEditor ? 2 : 0;
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::ComponentControl
// From base class CCoeControl
// Returns an indexed component of a compound control
// -----------------------------------------------------------------------------
//
CCoeControl* CIRStationInfoContainer::ComponentControl( TInt aIndex ) const
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::ComponentControl - Entering" );

    CCoeControl* ctl = NULL;

    switch(aIndex)
	    {
    	case 0:
	    	ctl = iEditor;
	    	break;
	    case 1:
	    	ctl = iEditorLogo;
	    	break;
	    default:
	    	break;
    	}
    IRLOG_DEBUG( "CIRStationInfoContainer::ComponentControl - Exiting" );
    return ctl;
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::OfferKeyEventL
// From base class CCoeControl
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CIRStationInfoContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
													  TEventCode aType )
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::OfferKeyEventL" );

	TKeyResponse res = EKeyWasNotConsumed;

    if (aType == EEventKey)
    {
        if (aKeyEvent.iCode == EKeyDownArrow)
        {
            iEditor->MoveCursorL (TCursorPosition::EFPageDown, EFalse);
            res = EKeyWasConsumed;
        }
        else if (aKeyEvent.iCode == EKeyUpArrow)
        {
            iEditor->MoveCursorL (TCursorPosition::EFPageUp, EFalse);
            res = EKeyWasConsumed;
        }
        else
        {
            res = iEditor->OfferKeyEventL(aKeyEvent, aType);
        }
    }
    return res;
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::MopSupplyObject
// From base class CCoeControl
// Retrieves an object of the same type as that encapsulated in aId.
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CIRStationInfoContainer::MopSupplyObject( TTypeUid aId )
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::MopSupplyObject - Entering" );
    if ( aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    IRLOG_DEBUG( "CIRStationInfoContainer::MopSupplyObject - Exiting" );
    return CCoeControl::MopSupplyObject( aId );
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::SizeChanged
// From base class CCoeControl
// Responds to changes to the size and position of the contents of this control.
// -----------------------------------------------------------------------------
//
void CIRStationInfoContainer::SizeChanged()
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::SizeChanged - Entering" );
    if( !iEditor )
	    {
		IRLOG_DEBUG( "CIRStationInfoContainer::SizeChanged - Exiting" );
	    return;
	    }
	if ( IsStationSupportsLogo() && !iBitmap)	// logo supported but not downloaded yet
		{
		IRLOG_DEBUG( "CIRStationInfoContainer::SizeChanged - Exiting" );
		return;
		}

	if( iClientRect != Rect() )
		{
		TRAPD( err, ReloadStationInformationL() );
		if(err != KErrNone)
			{
			IRLOG_WARNING( "CIRStationInfoContainer::SizeChanged - ReloadStationInformationL() failed" );
			}
		}
	IRLOG_DEBUG( "CIRStationInfoContainer::SizeChanged - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRStationInfoContainer::Draw
// From base class CCoeControl
// Draws the control.
// -----------------------------------------------------------------------------
//
void CIRStationInfoContainer::Draw( const TRect& /*aRect*/ ) const
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::Draw - Entering" );
	SystemGc();

    // draw background from skin
    AknsUtils::SkinInstance();				// PC Lint #529 Fix
    AknsDrawUtils::ControlContext( this );	// PC Lint #529 Fix
    IRLOG_DEBUG( "CIRStationInfoContainer::Draw - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRStationInfoContainer::StartConvertStationLogoL()
// Issues logo scaling request
// ---------------------------------------------------------------------------
//
void CIRStationInfoContainer::StartConvertStationLogoL( )
	{
	IRLOG_DEBUG( "CIRStationInfoContainer::StartConvertStationLogoL - Entering" );
	if ( IsStationSupportsLogo() )
		{
		iConverter->SetDataL( iStationInfoView->iStationPreset->GetLogoData() );
		if(iError==KErrNone)
			{
			TInt channelId = iStationInfoView->iStationPreset->GetId();
			iConverter->StartL( TSize(KLOGO_WIDTH, KLOGO_HEIGHT), channelId );
			}
		}
	IRLOG_DEBUG( "CIRStationInfoContainer::StartConvertStationLogoL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRStationInfoContainer::HandleImageConversionEventL()
// From MIRImageConverterObserver
// Invoked after image scaling is completed.
// ---------------------------------------------------------------------------
//
void CIRStationInfoContainer::HandleImageConversionEventL(
	MIRImageConverterObserver::TIRImageConversionEvent /*aEvent*/,TInt /*aId*/,TInt aError)
	{
	IRLOG_DEBUG( "CIRStationInfoContainer::HandleImageConversionEventL - Entering" );
	// Retreive scaled bitmaps
	iError=aError;
	if(iError!=KErrNone)
		{
		return;
		}

	iConverter->TransferBitmapOwnership( iBitmap, iBitmapMask );

	TRect rectParent = Rect();
	if( !iBitmap && !iBitmapMask )
		{
		IRLOG_WARNING("CIRStationInfoContainer::HandleImageConversionEventL- LogoScaling failed");
		}
	
	else if( iClientRect != rectParent )
		{
		ReloadStationInformationL();
		}	

	// Potrait view with logo
	else if( rectParent.Height() >= rectParent.Width() )
		{
		DisplayStationLogoL( iEditor, EFalse );
		iEditorLogo->DrawNow();

		TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
		TRect scrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
		iEditor->SetExtent( point1, TSize( rectParent.Width() - scrollBarRect.Width(),
							rectParent.Height() ) );

		iEditor->DrawDeferred();
		}
	// Landscape view with logo
	else
		{
		DisplayStationLogoL( iEditorLogo, ETrue );

	    TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
	    TInt	width1 = rectParent.Width()/2, height1 = rectParent.Height();
	    iEditorLogo->SetExtent( point1, TSize( width1, height1 ) );

	    iEditorLogo->DrawDeferred();
	    iEditor->DrawDeferred();
		}

	IRLOG_DEBUG( "CIRStationInfoContainer::HandleImageConversionEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationInfoContainer::ReloadStationInformationL()
// Renders the logo and text in the view as per the current resolution
// ---------------------------------------------------------------------------
//
void CIRStationInfoContainer::ReloadStationInformationL()
	{
	IRLOG_DEBUG( "CIRStationInfoContainer::ReloadStationInformationL - Entering" );

	// save current size to avoid SizeChanged multiple refreshes (optimize perf.)
	iClientRect = Rect();

	iEditor->RichText()->Reset();
    CreateRichTextDataL();
    iImageDrawn = EFalse;

	//
	// Potrait view with/without logo
	//
	TRect   rectParent = Rect();
	if( rectParent.Height() >= rectParent.Width() )
		{
		DisplayStationLogoL( iEditor, EFalse );
		iEditorLogo->DrawNow();
		iEditor->RichText()->InsertL( iEditor->RichText()->DocumentLength(),
									  CEditableText::EParagraphDelimiter);
		DisplayStationInfoL( EFalse );

	    iEditor->DrawDeferred();

		TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
		TRect scrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
		iEditor->SetExtent( point1, TSize( rectParent.Width() - scrollBarRect.Width(),
							rectParent.Height() ) );
		}

	//
	// Landscape view without logo
	//
	else if( !IsStationSupportsLogo() )
		{
		iEditorLogo->MakeVisible( EFalse );
		iEditorLogo->DrawNow();

		DisplayStationInfoL( ETrue );

		TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
		TRect scrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
		iEditor->SetExtent( point1, TSize( rectParent.Width() - scrollBarRect.Width(),
							rectParent.Height() ) );

	    iEditor->DrawDeferred();
		}

	//
	// Landscape view with logo
	//
	else
		{
		DisplayStationLogoL( iEditorLogo, ETrue );
		DisplayStationInfoL( ETrue );

	    TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
	    TInt	width1 = rectParent.Width()/2, height1 = rectParent.Height();
	    iEditorLogo->SetExtent( point1, TSize( width1, height1 ) );

	    TPoint	point2( rectParent.iTl.iX+width1, rectParent.iTl.iY );
	    TInt	width2 = rectParent.Width()/2, height2 = rectParent.Height();
        TRect scrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
        iEditor->SetExtent( point2, TSize( width2 - scrollBarRect.Width(), height2 ) );

	    iEditor->DrawDeferred();
	    iEditorLogo->DrawDeferred();
		}

    ActivateL();
	iEditor->SetCursorPosL(KZero, EFalse );

	IRLOG_DEBUG( "CIRStationInfoContainer::ReloadStationInformationL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationInfoContainer::DisplayStationLogoL
// Renders the logo in the view as per the current resolution
// ---------------------------------------------------------------------------
//
void CIRStationInfoContainer::DisplayStationLogoL( CEikRichTextEditor* aEditor,
												   TBool aIsLandscape )
	{
	IRLOG_DEBUG( "CIRStationInfoContainer::DisplayStationLogoL - Entering" );
	if( iBitmap || iBitmapMask )
		{
		//
		// Horizontally Center Align logo
		//
	    TParaFormatMask paraFormatMask;
	    CParaFormat *paraFormat = CParaFormat::NewL();
	    CleanupStack::PushL( paraFormat );

	    CRichText* richTextLogo = aEditor->RichText();
	    richTextLogo->GetParaFormatL( paraFormat, paraFormatMask, 0, 0 );
	    paraFormatMask.SetAll();
		if( aIsLandscape )
			{
			paraFormat->iLeftMarginInTwips = KLEFT_LOGO_MARGIN_LANDSCAPE;
			}
		paraFormat->iHorizontalAlignment = CParaFormat::ECenterAlign;
	    richTextLogo->ApplyParaFormatL(paraFormat, paraFormatMask, 0, 0 );
	    CleanupStack::PopAndDestroy( paraFormat );

		// Vertical center logo
		//
		if( aIsLandscape )
			{
			TCharFormat charFormatLogo;
			TCharFormatMask formatMask;
			richTextLogo->GetCharFormat( charFormatLogo, formatMask, 0, 0);
			formatMask.SetAll();

		    charFormatLogo.iFontPresentation.iPictureAlignment
		    	= TFontPresentation::EAlignCentered;

		    TInt heightPixels = aEditor->TextLayout()->BandHeight();
			charFormatLogo.iFontSpec.iHeight = heightPixels;

			richTextLogo->SetInsertCharFormatL(charFormatLogo, formatMask,
											   richTextLogo->DocumentLength());
			}

		// remove existing frame if any
		if( aIsLandscape )
			{
			iEditorLogo->MakeVisible( ETrue );
			iEditorLogo->RichText()->Reset();
			}
		else if( !iImageDrawn )
			{
			iEditorLogo->MakeVisible( EFalse );
			iEditorLogo->RichText()->Reset();
			iImageDrawn = ETrue;
			}
		else
			{
			iEditorLogo->MakeVisible( EFalse );
			iEditorLogo->RichText()->Reset();
			richTextLogo->DeleteL( KZero, KTwo );
			}

		// Display scaled station logo
		//
		CIRStationInfoPicture* picture = new(ELeave) CIRStationInfoPicture
										 ( SystemGc(), *iBitmap, *iBitmapMask );
		CleanupStack::PushL(picture);
		TPictureHeader picHeader;
		picHeader.iPicture = TSwizzle<CPicture>( picture );
		richTextLogo->InsertL( KZero, CEditableText::ELineBreak);
		richTextLogo->InsertL( KOne, picHeader);
		CleanupStack::Pop( picture ); 	// RichTextEditor owns the picture
		
		if( aIsLandscape )
			{
			richTextLogo->CancelInsertCharFormat();
			}
		}
	IRLOG_DEBUG( "CIRStationInfoContainer::DisplayStationLogoL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationInfoContainer::DisplayStationInfoL
// Reads station info from preset and renders the text in the view
// ---------------------------------------------------------------------------
//
void CIRStationInfoContainer::DisplayStationInfoL( TBool aIsLandscape )
	{
	IRLOG_DEBUG( "CIRStationInfoContainer::DisplayStationInfoL - Entering" );
	CRichText* richText = iEditor->RichText();

	// Horizontally Left Align
	//
    TParaFormatMask paraFormatMask;
    CParaFormat *paraFormat = CParaFormat::NewL();
    CleanupStack::PushL( paraFormat );

    richText->GetParaFormatL( paraFormat, paraFormatMask, KRICHTEXT_ORIGIN, KRICHTEXT_LENGTH );
    paraFormatMask.SetAll();

	paraFormat->iLineSpacingInTwips = KPARA_LINE_SPACING;
	paraFormat->iSpaceAfterInTwips = KPARA_SPACING;
	if( aIsLandscape && IsStationSupportsLogo() )		// Landscape with logo
		{
		paraFormat->iLeftMarginInTwips = KLEFT_MARGIN_LANDSCAPE_LOGO;
		paraFormat->iRightMarginInTwips = KRIGHT_MARGIN_LANDSCAPE;
		}
	else if( aIsLandscape && !IsStationSupportsLogo() )	// Landscape without logo
		{
		paraFormat->iLeftMarginInTwips = KLEFT_MARGIN_LANDSCAPE_NOLOGO;
		paraFormat->iRightMarginInTwips = KRIGHT_MARGIN_LANDSCAPE;
		}
	else // Potrait
		{
		paraFormat->iLeftMarginInTwips = KLEFT_MARGIN_POTRAIT;
		paraFormat->iRightMarginInTwips = KRIGHT_MARGIN_POTRAIT;
		}
	paraFormat->iHorizontalAlignment = CParaFormat::ELeftAlign;
	paraFormat->iKeepTogether = ETrue;
    richText->ApplyParaFormatL(paraFormat, paraFormatMask,
    						   richText->DocumentLength(), KRICHTEXT_LENGTH );
    CleanupStack::PopAndDestroy( paraFormat );

	// charFormats for bold & bigger headings
	//
	TCharFormat cfOriginal;
	TCharFormatMask formatMask;
	richText->GetCharFormat( cfOriginal, formatMask, KRICHTEXT_ORIGIN, KRICHTEXT_LENGTH);
	formatMask.SetAll();

    TCharFormat cfHeading = cfOriginal;
    cfHeading.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    cfHeading.iFontSpec.iHeight -= KHEADINGTEXT_HEIGHT_DECREMENT;

    TCharFormat cfMedium = cfOriginal;
    cfMedium.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    cfMedium.iFontSpec.iHeight -= KNORMALTEXT_HEIGHT_DECREMENT;

    TCharFormat cfNormal = cfOriginal;
    cfNormal.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
    cfNormal.iFontSpec.iHeight -= KNORMALTEXT_HEIGHT_DECREMENT;

	//
	// Read labels
	//
    HBufC* stationNameLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_NAME );
    HBufC* stationDescLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_DESCRIPTION );
    HBufC* stationGenreLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_GENRE );
    HBufC* stationLanguageLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_LANGUAGE );
    HBufC* stationCountryLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_COUNTRY );
    HBufC* stationTypeLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_TYPE );
    HBufC* stationStreamLabel = StringLoader::LoadLC( R_IRAPP_STATIONINFO_STREAM );
    HBufC* stationTypeManual = StringLoader::LoadLC( R_IRAPP_STATIONINFO_TYPEMANUAL );
    HBufC* stationTypeIsds = StringLoader::LoadLC( R_IRAPP_STATIONINFO_TYPEISDS );

	//
	// Retrieve Station Information
	//
	CIRIsdsPreset* stationPreset = iStationInfoView->iStationPreset;
	const TDesC&  stationName 		= stationPreset->GetName();
	const TDesC&  stationDescription = stationPreset->GetDescription();
	const TDesC&  stationGenre 		= stationPreset->GetGenreName();
	const TDesC&  stationLang 		= stationPreset->GetLangName();
	const TDesC&  stationCountry 	= stationPreset->GetCountryName();
	TInt 		  stationType 		= stationPreset->GetChannelType();

	// Add station name
	richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
	richText->InsertL( richText->DocumentLength(), *stationNameLabel);
	richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
	richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());

	if( stationName.Length() )
		{
		richText->InsertL( richText->DocumentLength(), stationName);
		}
	else
		{
		HBufC* stationUnnamed = StringLoader::LoadLC( R_IRAPP_STATIONINFO_UNNAMED );
		richText->InsertL( richText->DocumentLength(), *stationUnnamed);
		CleanupStack::PopAndDestroy(stationUnnamed);
		}
	richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);

	// Add station description
	if( stationDescription.Length() )
		{
		richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), *stationDescLabel);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), stationDescription);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		}

	// Add station genre
	if( stationGenre.Length() )
		{
		richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), *stationGenreLabel);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), stationGenre);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		}

	// Add station language
	if( stationLang.Length() )
		{
		richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), *stationLanguageLabel);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), stationLang);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		}

	// Add station country
	if( stationCountry.Length() )
		{
		richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), *stationCountryLabel);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), stationCountry);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		}

	// Station Type
	richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
	richText->InsertL( richText->DocumentLength(), *stationTypeLabel);
	richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);

	richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());
	if ( stationType == KSI_ISDS_DEFINED )
		{
		richText->InsertL( richText->DocumentLength(), *stationTypeIsds);
		}
	else
		{
		richText->InsertL( richText->DocumentLength(), *stationTypeManual);
		}
	richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);

	//
	// Station Bitrates and Urls
	//
	if( stationPreset->GetUrlCount() > KZero )
		{
		richText->SetInsertCharFormatL(cfHeading, formatMask, richText->DocumentLength());
		richText->InsertL( richText->DocumentLength(), *stationStreamLabel);
		richText->SetInsertCharFormatL(cfNormal, formatMask, richText->DocumentLength());
		}

	// for ISDS channels
	RArray<TInt>	channelBitrates;
	stationPreset->GetAvailableBitrates( channelBitrates );
	CleanupClosePushL( channelBitrates );

	for(TInt bitrateCounter = 0;
		stationType == KSI_ISDS_DEFINED && bitrateCounter < channelBitrates.Count();
		++bitrateCounter)
		{
		HBufC *bitrateText = StringLoader::LoadLC(
				R_IRAPP_STATIONINFO_BITRATE, channelBitrates[bitrateCounter] );
		TBuf<KBufferSize> bitRate;
		bitRate.Append(*bitrateText);
		CleanupStack::PopAndDestroy(bitrateText);
		bitRate.Append(Kspace);

		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		richText->InsertL( richText->DocumentLength(), bitRate );

		CDesCArrayFlat* pUrlsForBitrate = stationPreset->
										GetExactUrlForBitrateL( channelBitrates[bitrateCounter] );
		CleanupStack::PushL( pUrlsForBitrate );

		for(TInt urlCounter = 0; urlCounter < pUrlsForBitrate->Count(); ++urlCounter)
			{
			HBufC* url = (*pUrlsForBitrate)[urlCounter].AllocLC();

			TInt index =url->Des().Find(KHttp);
			if(index!=KErrNotFound)
				{
				url->Des().Delete(index,KHttpLength);
				}

			richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
			richText->InsertL( richText->DocumentLength(), *url );

			CleanupStack::PopAndDestroy(url);
			}
		CleanupStack::PopAndDestroy( pUrlsForBitrate );
		}
	CleanupStack::PopAndDestroy( &channelBitrates );

	// for User Defined channel
	if( stationType == KSI_USERDEFINED && stationPreset->GetUrlCount() > 0 )
		{
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);

		HBufC* url = stationPreset->GetChannelUrlAtL(0).AllocLC();

		TInt index =url->Des().Find(KHttp);
		if(index!=KErrNotFound)
			{
			url->Des().Delete(index,KHttpLength);
			}
		richText->InsertL( richText->DocumentLength(), *url );
		CleanupStack::PopAndDestroy(url);
		}

	// Cleanup String labels
    CleanupStack::PopAndDestroy(stationTypeIsds);
    CleanupStack::PopAndDestroy(stationTypeManual);
	CleanupStack::PopAndDestroy(stationStreamLabel);
    CleanupStack::PopAndDestroy(stationTypeLabel);
    CleanupStack::PopAndDestroy(stationCountryLabel);
    CleanupStack::PopAndDestroy(stationLanguageLabel);
    CleanupStack::PopAndDestroy(stationGenreLabel);
    CleanupStack::PopAndDestroy(stationDescLabel);
    CleanupStack::PopAndDestroy(stationNameLabel);

	richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);

	TRect   rectParent = Rect();
	if ( IsStationSupportsLogo() && rectParent.Height() >= rectParent.Width() )
		{
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		richText->InsertL( richText->DocumentLength(), CEditableText::ELineBreak);
		richText->InsertL( richText->DocumentLength(), CEditableText::ELineBreak);
		richText->InsertL( richText->DocumentLength(), CEditableText::ELineBreak);
		richText->InsertL( richText->DocumentLength(), CEditableText::ELineBreak);
		richText->InsertL( richText->DocumentLength(), CEditableText::ELineBreak);
		richText->InsertL( richText->DocumentLength(), CEditableText::ELineBreak);
		richText->InsertL( richText->DocumentLength(), CEditableText::EParagraphDelimiter);
		}

	richText->CancelInsertCharFormat();

    IRLOG_DEBUG( "CIRStationInfoContainer::DisplayStationInfoL - Exiting" );
	}

// ---------------------------------------------------------
// CIRStationInfoContainer::CreateRichTextDataL
// Initializes the rich text with font, color, anti-aliasing
// ---------------------------------------------------------
//
void CIRStationInfoContainer::CreateRichTextDataL()
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::CreateRichTextData - Entering" );
    TCharFormat charFormat;
    TCharFormatMask formatMask;
    iEditor->RichText()->GetCharFormat( charFormat, formatMask, 0, 0 );

	TRgb textColor;
    TInt error = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), textColor,
        									KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6 );
    if ( error != KErrNone )
        {
        textColor = KRgbBlack;
        }

    charFormat.iFontPresentation.iTextColor = textColor;
	charFormat.iFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    formatMask.SetAll();

	CCharFormatLayer* globalCharLayer = NULL;
    globalCharLayer = CCharFormatLayer::NewL( charFormat, formatMask );
    CleanupStack::PushL( globalCharLayer );

    iEditor->SetCharFormatLayer( globalCharLayer );

    CleanupStack::Pop(globalCharLayer);
    IRLOG_DEBUG( "CIRStationInfoContainer::CreateRichTextData - Exiting" );
    }


// ----------------------------------------------------------------------------------------
// CIRStationInfoContainer::HandleResourceChange
// From base class CCoeControl
// Handles a change to the control's resources like background drawing and S60 skin changes
//
// ----------------------------------------------------------------------------------------
void CIRStationInfoContainer::HandleResourceChange( TInt aType )
    {
    IRLOG_DEBUG( "CIRStationInfoContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
		TRect rect;
		TBool bRes = AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
		SetRect(rect);

        iEditor->HandleResourceChange( aType );
        iEditorLogo->HandleResourceChange( aType );

        if ( iBgContext )
            {
            iBgContext->SetRect( Rect() );
            }
        }
    else if( ( aType == KAknsMessageSkinChange ) && ( iBgContext ) )
        {
        TRAPD( ignore, iEditor->SetSkinBackgroundControlContextL( iBgContext ) );
        if( ignore )
            {
            ignore = KErrNone; //Prevent ARMV5 compilation varning
            }
		TRAPD( err, ReloadStationInformationL() );
		if(err != KErrNone)
			{
			IRLOG_WARNING( "CIRStationInfoContainer::SizeChanged - ReloadStationInformationL() failed" );
			}
        }
    IRLOG_DEBUG( "CIRStationInfoContainer::HandleResourceChange - Exiting" );
    }

// ----------------------------------------------------
// CIRStationInfoContainer::IsStationSupportsLogo
// Returns true if station logo available, else false.
// ----------------------------------------------------
//
TBool CIRStationInfoContainer::IsStationSupportsLogo()
{
	IRLOG_DEBUG( "CIRStationInfoContainer::IsStationSupportsLogo" );
	return ( iStationInfoView->iStationPreset
			 && iStationInfoView->iStationPreset->GetImgUrl().Length() );
}


// ------------------------------------------------------------
// CIRStationInfoContainer::IsISDSStation
// Returns true if station is from ISDS, false if user-defined.
// ------------------------------------------------------------
//
TBool CIRStationInfoContainer::IsISDSStation()
{
	IRLOG_DEBUG( "CIRStationInfoContainer::IsISDSStation " );
	return ( iStationInfoView->iStationPreset->GetChannelType() == CIRIsdsPreset::EIsdsPreset );
}
