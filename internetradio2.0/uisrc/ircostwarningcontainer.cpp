/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for cost warning view
*
*/


#include <txtrich.h>
/* don't need to change it to lower case */
#include <AknsBasicBackgroundControlContext.h> // CAknsBasicBackgroundControlContext
#include <eikedwin.h>
#include <utf.h>
#include <stringloader.h>


#include <internetradio.rsg>
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "ircostwarningview.h"
#include "ircostwarningcontainer.h"
#include "irapplication.h"
#include "irnetworkcontroller.h"
#include "irsettings.h"


const TInt KNoOfLines = 50;
const TInt KNoOfCharacters = 3000;
const TInt KScrollbar = 15;
//-----------------------------------------------------------------------------
//  CIRCostWarningContainer::NewL
//  Creates a new instance of CIRCostWarningContainer
//-----------------------------------------------------------------------------
//
CIRCostWarningContainer* CIRCostWarningContainer::NewL(const TRect& aRect ,
					CIRCostWarningView& aView)
    {
	IRLOG_DEBUG( "CIRCostWarningContainer::NewL - Entering" );
    CIRCostWarningContainer* self = CIRCostWarningContainer::NewLC(aRect,aView);
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRCostWarningContainer::NewL - Exiting." );
    return self;
    }

//-----------------------------------------------------------------------------
//  CIRCostWarningContainer::NewLC
//  Creates a new instance of CIRCostWarningContainer and leaves it on the cleanup stack.
//-----------------------------------------------------------------------------
//
CIRCostWarningContainer* CIRCostWarningContainer::NewLC(const TRect& aRect ,
						CIRCostWarningView& aView)
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::NewLC - Entering" );
    CIRCostWarningContainer* self = new (ELeave) CIRCostWarningContainer(aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
	IRLOG_DEBUG( "CIRCostWarningContainer::NewLC - Exiting." );
    return self;
    }
// ---------------------------------------------------------
// C++ default constructor.
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------
//

CIRCostWarningContainer::CIRCostWarningContainer( CIRCostWarningView& aMsgView ):
                                                  iMsgView(aMsgView)
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::CIRCostWarningContainer - Entering" );
    IRLOG_DEBUG( "CIRCostWarningContainer::CIRCostWarningContainer - Exiting" );
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::ConstructL()
// Two Phase constructor
// ---------------------------------------------------------
//
void CIRCostWarningContainer::ConstructL( const TRect& aRect )
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::ConstructL - Entering" );
    CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
    CreateWindowL();
    iEditor = CreateEditorL();
    SetReadOnly(ETrue);
    
    SetRect(aRect);
    iEditor->SetRect(aRect);

    // for Skin Support, Create background control context:
    iBackGroundControlContext = CAknsBasicBackgroundControlContext::NewL(
                    KAknsIIDQsnBgAreaMain,  // Default mainpane skinning
                    Rect(),                 // Layout to the entire client rect
                    EFalse );
   
    if ( iBackGroundControlContext )
        {
        iEditor->SetSkinBackgroundControlContextL( iBackGroundControlContext );
        }
	if (!ui->iIRSettings->IsFlagCostWarningL())
		{
        ui->SetCommandSetL(R_IR_CBA_CONTINUE_EXIT);
		UpdateTermsL(); 
		}
    else
	    {   
	    ui->SetCommandSetL(R_IR_CBA_ACCEPT_DECLINE);                
		UpdateTermsL();   	
	    }
   	iEditor->SetCursorPosL(0, EFalse );

	IRLOG_DEBUG( "CIRCostWarningContainer::ConstructL - Exiting" );
    
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::~CIRCostWarningContainer()
// Destructor
// ---------------------------------------------------------
//
CIRCostWarningContainer::~CIRCostWarningContainer()
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::~CIRCostWarningContainer - Entering" );
    delete iBackGroundControlContext;
    delete iEditor;
    if (iRichText)
	    {
	    delete iRichText;
	    iRichText = NULL;	
	    }
    delete iTermsData;
    IRLOG_DEBUG( "CIRCostWarningContainer::~CIRCostWarningContainer - Exiting" );
    }


//----------------------------------------------------------------------------
// CIRCostWarningContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
//
TInt CIRCostWarningContainer::CountComponentControls() const
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::CountComponentControls - Entering" );
    return iEditor ? 1 : 0;
    }


//----------------------------------------------------------------------------
// CIRCostWarningContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRCostWarningContainer::ComponentControl( TInt aIndex ) const
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::ComponentControl - Entering" );

    CCoeControl* ctrl = NULL;

    switch(aIndex)
	    {
    	case 0:
	    	ctrl = iEditor;
	    	break;
	    default:
	    	break;
    	}
    IRLOG_DEBUG( "CIRCostWarningContainer::ComponentControl - Exiting" );
    return ctrl;
    }


// ---------------------------------------------------------------------------
// TKeyResponse CIRCostWarningContainer::OfferKeyEventL( )
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things 
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRCostWarningContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType )
    {    
    
    IRLOG_DEBUG( "CIRCostWarningContainer::OfferKeyEventL - Entering" );   	
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
    IRLOG_DEBUG( "CIRCostWarningContainer::OfferKeyEventL - Exiting" );
    return res;
    }


// ---------------------------------------------------------
// CIRCostWarningContainer::UpdateTermsL
// To update the required data whenever called
// 
// ---------------------------------------------------------
//
void CIRCostWarningContainer::UpdateTermsL()
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::UpdateTermsL - Entering" );
    CreateRichTextDataL();
    iEditor->DrawNow();
    ActivateL();		
    IRLOG_DEBUG( "CIRCostWarningContainer::UpdateTermsL - Exiting" );
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::SizeChanged
// ---------------------------------------------------------
//
void CIRCostWarningContainer::SizeChanged()
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::SizeChanged - Entering" );
    if ( iEditor ) 
        {
		TRect   rectParent = Rect();
		TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
		TRect scrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
		iEditor->SetExtent( point1, TSize( rectParent.Width() - scrollBarRect.Width(),
		    					rectParent.Height() - KScrollbar  ) );
        iEditor->DrawNow();
        }
        
    IRLOG_DEBUG( "CIRCostWarningContainer::SizeChanged - Exiting" );
    }


// ---------------------------------------------------------
// CIRCostWarningContainer::CreateEditorL
// Creates the CEikRichTextEditor object
// 
// ---------------------------------------------------------
//
CEikRichTextEditor* CIRCostWarningContainer::CreateEditorL() const
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::CreateEditorL - Entering" );
    CEikRichTextEditor* editor = new (ELeave) CEikRichTextEditor;
    CleanupStack::PushL( editor );
    editor->SetContainerWindowL(*this);
    TInt edwinflags =  CEikEdwin::EReadOnly | CEikEdwin::EEdwinAlternativeWrapping ;
    editor->ConstructL(this, KNoOfLines,KNoOfCharacters,edwinflags );
    
    // create vertical scrollbar
    editor->CreateScrollBarFrameL();
    editor->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
    												   CEikScrollBarFrame::EAuto );
	editor->ActivateL();
    CleanupStack::Pop(editor); // editor
    IRLOG_DEBUG( "CIRCostWarningContainer::CreateEditorL - Exiting" );
    return editor;
    }

// ---------------------------------------------------------------------------
// void CIRCostWarningContainer::Draw( const TRect& aRect ) const
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRCostWarningContainer::Draw( const TRect& aRect ) const
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::Draw - Entering" );
    CWindowGc& gc = SystemGc();
    
    // for clearing gap between editor and the view rect
    if ( IsReadyToDraw() )
        {
        DrawUtils::ClearBetweenRects( gc, aRect, iEditor->Rect() );
        if ( iBackGroundControlContext )
            {//Draw the skin background
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            AknsDrawUtils::Background( skin, iBackGroundControlContext, this, gc, aRect );
            }
        }
    IRLOG_DEBUG( "CIRCostWarningContainer::Draw - Exiting" );
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::SetReadOnly
// Making the editor as ReadOnly
// 
// ---------------------------------------------------------
//
void CIRCostWarningContainer::SetReadOnly( TBool aReadOnly )
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::SetReadOnly - Entering" );
    iEditor->SetReadOnly(aReadOnly);
    IRLOG_DEBUG( "CIRCostWarningContainer::SetReadOnly - Exiting" );
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::SetDataContentL
// Setting the RichText into the Editor
// 
// ---------------------------------------------------------
//
void CIRCostWarningContainer::SetDataContentL( CRichText& aText )
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::CIRCostWarningContainer - Entering" );
    iEditor->SetDocumentContentL(aText);
   	TRect   rectParent = Rect();
    TPoint	point1( rectParent.iTl.iX, rectParent.iTl.iY );
    TRect scrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
	iEditor->SetExtent( point1, TSize( rectParent.Width() - scrollBarRect.Width(),
	        					rectParent.Height() - KScrollbar  ) );
    IRLOG_DEBUG( "CIRCostWarningContainer::SetDataContentL - Exiting" );  
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::CreateRichTextDataL
// Creating the CRichText object
// 
// ---------------------------------------------------------
//
void CIRCostWarningContainer::CreateRichTextDataL()
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::CreateRichTextDataL - Entering" );
    CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
    CParaFormatLayer* globalParaLayer = CParaFormatLayer::NewL();
    CleanupStack::PushL(globalParaLayer);
    
                   
    CCharFormatLayer* globalCharLayer = NULL;   
    TRgb textColor;
    
    TCharFormat charFormat;
    TCharFormatMask formatMask;
        
    iEditor->RichText()->GetCharFormat( charFormat, formatMask, 0, 0 );
    MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
    TInt error = AknsUtils::GetCachedColor(
        skin,
        textColor,
        KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG6 );
     
	if( error != KErrNone )
		{
		textColor = KRgbBlack;
		}
		
	charFormat.iFontPresentation.iTextColor = textColor;	
	charFormat.iFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    formatMask.SetAll();

    globalCharLayer = CCharFormatLayer::NewL( charFormat, formatMask );
    CleanupStack::PushL( globalCharLayer );
     
    iRichText = CRichText::NewL( globalParaLayer, globalCharLayer );
    CParaFormat *para=CParaFormat::NewL();
    CleanupStack::PushL( para );
    TParaFormatMask paraMask;
    paraMask.SetAttrib(EAttSpaceAfter);	
    
    
    if (!ui->iIRSettings->IsFlagCostWarningL())
	    {

		TCharFormat charFormat;
		TCharFormatMask charFormatMask;
		TPtrC costHeading;
		HBufC *CWHeading = StringLoader::LoadLC(R_IRAPP_COST_WARNING_HEADING);
		costHeading.Set(*CWHeading);
		charFormatMask.SetAttrib(EAttFontStrokeWeight);
		charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
		iRichText->SetInsertCharFormatL(charFormat,charFormatMask,iRichText->DocumentLength());
		iRichText->InsertL(iRichText->DocumentLength(),costHeading);
		iRichText->InsertL(iRichText->DocumentLength(),CEditableText::ELineBreak );
	    iRichText->InsertL(iRichText->DocumentLength(), CEditableText::ELineBreak );
		iRichText->CancelInsertCharFormat();
		CleanupStack::PopAndDestroy(CWHeading);	
        
		TPtrC costText;
		HBufC *CWInfo = StringLoader::LoadLC(R_IRAPP_COST_WARNING_INFO);
		costText.Set(*CWInfo);
		charFormatMask.SetAttrib(EAttFontStrokeWeight);
		charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
		iRichText->SetInsertCharFormatL(charFormat,charFormatMask,iRichText->DocumentLength());
		iRichText->InsertL(iRichText->DocumentLength(),costText);
		iRichText->CancelInsertCharFormat();
		CleanupStack::PopAndDestroy(CWInfo);		
	    }
    else
	    {
	    ReadTermsFromFileL();
	    LayoutTermsL();
	    }
  
    
    SetDataContentL( *iRichText );
    
    iEditor->SetCharFormatLayer( globalCharLayer );
    iEditor->SetCursorPosL(0, EFalse );
     
    CleanupStack::PopAndDestroy(para);  // para
    CleanupStack::Pop(globalCharLayer);            // globalCharLayer
    CleanupStack::PopAndDestroy(globalParaLayer);  // globalParaLayer
    IRLOG_DEBUG( "CIRCostWarningContainer::CreateRichTextDataL - Exiting" );
    }

// ---------------------------------------------------------
// CIRCostWarningContainer::GetEditor
// Getting the contents of RichText to the Editor
// 
// ---------------------------------------------------------
CRichText* CIRCostWarningContainer::GetEditor() const
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::GetEditor" );
    return iEditor->RichText();
    }

// ----------------------------------------------------
// CIRCostWarningContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across
// the environment.
// ----------------------------------------------------
void CIRCostWarningContainer::HandleResourceChange( TInt aType )
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
	if( aType == KEikDynamicLayoutVariantSwitch )
		{
		TRect rect;
		TBool bRes = AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane  , rect);
		SetRect(rect);

        iEditor->HandleResourceChange( aType );

		if ( iBackGroundControlContext )
			{
			iBackGroundControlContext->SetRect( Rect() );
			}
		}
	else if( ( aType == KAknsMessageSkinChange ) && ( iBackGroundControlContext ) )
			{
			TRAPD( ignore, iEditor->SetSkinBackgroundControlContextL( iBackGroundControlContext ) );
            if(ignore)
				{
				return;	
				}
			if (iRichText)
				{
				delete iRichText;
				iRichText=NULL;	
				}
			TRAPD(err ,UpdateTermsL());
			if(err)
				{
				return;	
				}
			TRAPD( error ,iEditor->SetCursorPosL(0, EFalse ));
			if(error)
				{
				return;
				}

	        } 
	IRLOG_DEBUG( "CIRCostWarningContainer::HandleResourceChange - Exiting" );			
    }
	
    
// ----------------------------------------------------
// CIRCostWarningContainer::ReadTermsFromFileL()
// Reads the text from a HTML file
//
// ----------------------------------------------------
void CIRCostWarningContainer::ReadTermsFromFileL()
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::ReadTermsFromFileL - Entering" );
    RFs& fs = static_cast<CEikonEnv*>(iCoeEnv)->FsSession();
	TFileName privatePath;
	User::LeaveIfError(fs.PrivatePath(privatePath));
	HBufC* termsFileName = StringLoader::LoadL(R_IRAPP_TERMS_FILE);
    TParse parse;
    parse.Set(*termsFileName, &privatePath, NULL);
    delete termsFileName;
    TFileName fileName = parse.FullName();
    CompleteWithAppPath(fileName);
    RFile fileHandle;
    User::LeaveIfError(fileHandle.Open(fs, fileName, EFileRead));
    CleanupClosePushL(fileHandle);
    TInt fileSize;
    User::LeaveIfError( fileHandle.Size( fileSize ) );
    HBufC8* termsData = HBufC8::NewLC( fileSize );
    TPtr8 pData8( termsData->Des() );
    User::LeaveIfError( fileHandle.Read( pData8 ) );
    delete iTermsData;
    iTermsData = NULL;
    iTermsData = CnvUtfConverter::ConvertToUnicodeFromUtf8L( pData8 );
    CleanupStack::PopAndDestroy( termsData );
    CleanupStack::PopAndDestroy( &fileHandle );
    IRLOG_DEBUG( "CIRCostWarningContainer::ReadTermsFromFileL - Exiting" );
    }

// ----------------------------------------------------
// CIRCostWarningContainer::LayoutTermsL()
// Inserts the text into the RichText object (Heading of Terms & Conditions).
//
// ----------------------------------------------------
void CIRCostWarningContainer::LayoutTermsL()
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::LayoutTermsL - Entering" );
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    
    TInt dataCounter(0);
    iTotalTextHeight = 0;
    TIRTermsTextType type(EIRTermsTextInvalid);
    TPtrC text;
    
	charFormatMask.SetAttrib(EAttFontStrokeWeight);
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	iRichText->SetInsertCharFormatL(charFormat,charFormatMask,iRichText->DocumentLength());
	
	HBufC *CWHeading = StringLoader::LoadLC(R_IRAPP_TERMS_CONDITIONS_HEADING);
	TPtrC termsHeading;

	termsHeading.Set( *CWHeading );

	iRichText->InsertL(iRichText->DocumentLength(),termsHeading);
	iRichText->InsertL(iRichText->DocumentLength(), CEditableText::ELineBreak );
	
	iRichText->CancelInsertCharFormat();
	CleanupStack::PopAndDestroy(CWHeading);
	
    while (GetNextTermsL(dataCounter, text, type)) 
		{
		if( type != EIRTermsTextHeading && type != EIRTermsTextNormal)
			{
			User::Leave(KErrCorrupt);
			}

		}
    IRLOG_DEBUG( "CIRCostWarningContainer::LayoutTermsL - Exiting" );
    
    }
    
// ---------------------------------------------------------------------------
// GetNextTermsL()
// Inserts the text into the RichText object (Body of Terms & Conditions). 
// ---------------------------------------------------------------------------
//
TBool CIRCostWarningContainer::GetNextTermsL(TInt& aCounter,
                                             TPtrC& aTermsText, TIRTermsTextType& aTermsType)
    {
    IRLOG_DEBUG( "CIRCostWarningContainer::GetNextTermsL - Entering" );
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    TBool textFound(EFalse);
    _LIT(KIRHeadingStartTag, "<h1>");
    _LIT(KIRHeadingEndTag, "</h1>");
    _LIT(KIRNormalStartTag, "<p>");
    _LIT(KIRNormalEndTag, "</p>");
    TPtr data = iTermsData->Des();
    
    TInt heading = data.Mid(aCounter).Find(KIRHeadingStartTag);
    TInt normal = data.Mid(aCounter).Find(KIRNormalStartTag);
    
    
    if ((heading != KErrNotFound) || (normal != KErrNotFound))
        {
        if( ((heading != KErrNotFound) && (normal != KErrNotFound) && (heading<normal)) || 
            (normal == KErrNotFound) )
            {
            aTermsType = EIRTermsTextHeading;
            aCounter = aCounter + heading + KIRHeadingStartTag().Length();
            }
        else
            {
            aTermsType = EIRTermsTextNormal;
            aCounter = aCounter + normal + KIRNormalStartTag().Length();
            }        
        if( aTermsType == EIRTermsTextNormal )
            {
            aTermsText.Set(data.Mid(aCounter, data.Mid(aCounter).Find(KIRNormalEndTag)));
            aCounter = aCounter + aTermsText.Length() + KIRNormalEndTag().Length();
            charFormatMask.SetAttrib(EAttFontStrokeWeight);
            charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
            
            iRichText->SetInsertCharFormatL(charFormat,charFormatMask,iRichText->DocumentLength());
            iRichText->InsertL(iRichText->DocumentLength(),aTermsText);
            iRichText->InsertL(iRichText->DocumentLength(), CEditableText::ELineBreak );
            iRichText->CancelInsertCharFormat();
            }
        else
            {
            aTermsText.Set(data.Mid(aCounter, data.Mid(aCounter).Find(KIRHeadingEndTag)));
            aCounter = aCounter + aTermsText.Length() + KIRHeadingEndTag().Length();
            charFormatMask.SetAttrib(EAttFontStrokeWeight);
            charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
            iRichText->SetInsertCharFormatL(charFormat,charFormatMask,iRichText->DocumentLength());
            iRichText->InsertL(iRichText->DocumentLength(),aTermsText);
            iRichText->InsertL(iRichText->DocumentLength(), CEditableText::ELineBreak );
            iRichText->CancelInsertCharFormat();
            }
        textFound = ETrue;
        }
    IRLOG_DEBUG( "CIRCostWarningContainer::GetNextTermsL - Exiting" );
    return textFound;
    
    
    }

	
// ---------------------------------------------------------------------------------
// 
// CIRCostWarningContainer::TandCAcceptL() 
// For setting the terms and conditions flag
// ---------------------------------------------------------------------------------
void CIRCostWarningContainer::TandCAcceptL() const
{
IRLOG_DEBUG( "CIRCostWarningContainer::TandCAcceptL - Entering" );
    CIRUi* ui = static_cast<CIRUi*>( iCoeEnv->AppUi() );
	//set the t&c flag,so that it doesnot come up the next time
	ui->iIRSettings->SetFlagTermsAndConditionsL();
//	ui->iNetworkController->ChooseAccessPointL()
    if(ui->GetPlsActiavted())
		{
		ui->ActivateLocalViewL(KIRPlsViewID);
		}
    else 
	    {
	    if(ui->IsFirstTime())
		    {
		    ui->ActivateLocalViewL(KIRFirstTimeViewId);	
		    }
		else
			{
			ui->ActivateLocalViewL(KIRMainChoiceViewID);	
			}    
	    }
    	
    IRLOG_DEBUG( "CIRCostWarningContainer::TandCAcceptL - Exiting" );
}

// ---------------------------------------------------------------------------
// 
// CIRCostWarningContainer::CostWarningContinueL() 
// For displaying the soft keys
// ---------------------------------------------------------------------------
void CIRCostWarningContainer::CostWarningContinueL() 
{   
IRLOG_DEBUG( "CIRCostWarningContainer::CostWarningContinueL - Entering" );
    CIRUi* ui = static_cast<CIRUi*>( iCoeEnv->AppUi() );
    ui->iIRSettings->SetFlagCostWarningL();
    if (iRichText)
    {
    delete iRichText;
    iRichText=NULL;	
    }
	UpdateTermsL();
	iEditor->SetCursorPosL(0,EFalse ); 
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL( R_IR_CBA_ACCEPT_DECLINE );
    cba->DrawDeferred();
    DrawNow();  		
    IRLOG_DEBUG( "CIRCostWarningContainer::CostWarningContinueL - Exiting" );
}

// ---------------------------------------------------------------------------
// CIRCostWarningContainer::HandlePointerEventL()
// Handles pointer events.
// ---------------------------------------------------------------------------
void CIRCostWarningContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
	{
	IRLOG_DEBUG( "CIRCostWarningContainer::HandlePointerEventL - Entering" );
	if(iEditor)
		{
		CCoeControl::HandlePointerEventL( aPointerEvent );
		IRLOG_DEBUG( "CIRCostWarningContainer::HandlePointerEventL - Exiting" );
		}
	}
