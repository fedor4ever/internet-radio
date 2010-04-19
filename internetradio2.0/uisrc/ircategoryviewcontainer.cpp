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
* Description:  Container class for Category view
*
*/

/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Thurs March 20 15:50:00 2008 by Prabina
*  Ref:
*  Created new views for the Station Directory Views , Enabled the Touch toolbar functionality , 
*  Help Options enabled
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Fri March 21 17:20:00 2008 by Prabina
*  Ref:
*  Added WorkAround for supporting different Resolutions , Added Wait Dialog for Loading of stations , 
*  Handled Single/Double tapping on the list itmes
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 4, Mon March 24 18:00:00 2008 by Prabina
*  Ref:
*  Added WorkAround for adding different stations to favourites list , Added Wait dialog for adding to favourites
*
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 5, Tues March 25 17:30:00 2008 by Prabina
*  Ref:
*  Fixing the PCLint errors
*
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 6, Tues March 29 10:30:00 2008 by Prabina
*  Ref:
*  Category View Container Derived from CCoeControl
*
*
*  </ccm_history>
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 7, may 7 Wed 08:35:00 2008 by Neelima
*  Removed change Resoultion function.
*  changed code in HandleResourcechange and Size changed  for handling layout changes 
*  in dynamic filtering
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 5, Thurs July 18 12:00:00 2008 by Prabina
*  Ref:
*  Code Review Comments Fixed
*
*
*  </ccm_history>
* ============================================================================
*/

#include <akniconarray.h>
#include <aknlists.h>
#include <aknview.h>
#include <barsread.h>
#include <eikenv.h>
#include <eiklabel.h>
#include <eikmenub.h>
#include <stringloader.h>
#include <internetradio.rsg>
#include <eikenv.h>
#include <apgcli.h>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "ircategoryviewcontainer.h"
#include "irapplication.h"
#include "ircategoryView.h"
#include "iraap.hlp.hrh"
#include "irisdswrapper.h"
#include "irimageconverter.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irlogodownloadengine.h"
#include <aknsbasicbackgroundcontrolcontext.h> 
#include "irfilteredmodel.h"
#include "mirlistboxarray.h"
#include "iradvertisinglistbox.h"
#include "irlayoututils.h"

/** Mime type when opening web browser */
_LIT8( KIRHtmlMimeType, "text/html" );
/** Browser prefix: See browser API specification */
_LIT( KBrowserPrefix, "4 " );

const TInt KAdvSize = 30;
const TSize KLogoSize = TSize(320,350);

#define KCONTROLSCOUNT 0

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIRCategoryViewContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRCategoryViewContainer* CIRCategoryViewContainer::NewL( CIRCategoryView& aCtaegoryView,
					 const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRCategoryViewContainer::NewL" );
    CIRCategoryViewContainer* self = CIRCategoryViewContainer::NewLC( aCtaegoryView, aRect );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRCategoryViewContainer::NewL - Exiting" );
    return self;
    }

// -----------------------------------------------------------------------------
// CIRCategoryViewContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRCategoryViewContainer* CIRCategoryViewContainer::NewLC( CIRCategoryView& aCtaegoryView,
					 const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRCategoryViewContainer::NewLC" );
    CIRCategoryViewContainer* self = new ( ELeave ) CIRCategoryViewContainer(aCtaegoryView);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    IRLOG_DEBUG( "CIRCategoryViewContainer::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::CIRCategoryViewContainer()  
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRCategoryViewContainer::CIRCategoryViewContainer(CIRCategoryView& aCtaegoryView):
                                                   iCategoryView(aCtaegoryView)
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::CIRCategoryViewContainer" );
	iFilteredFlag = EFalse;
	}
    
// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::ConstructL( const TRect& aRect)
    {
	IRLOG_DEBUG( "CIRCategoryViewContainer::ConstructL - Entering" );
    CreateWindowL();
    SetRect(aRect);
	
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen , aRect, ETrue); 

	iAdPresent = EFalse;
    iConverter = CIRImageConverter::NewL();
	iConverter->EnableAnimations( EFalse );
	iConverter->SetObserver( this );

	iCurrentItemConverter = CIRImageConverter::NewL();
	iCurrentItemConverter->SetObserver( this );
   
    // Added For Dynamic Filtering
    iFilteredModel=new(ELeave) CIRFilteredModel;
    iFilteredModel->ConstructL();

   	iListBoxArray = MIRListBoxArray::NewL();

	IRLOG_DEBUG( "CIRCategoryViewContainer::ConstructL - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::CreateListBoxL()
// Creates ListBox depending upon the presence of Advertisement
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::CreateListBoxL()
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::CreateListBoxL - Entering" );
	if (iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl)
		{
		if((iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl->Length()) != 0)
			{
			iAdPresent = ETrue;
			if(iAdvPreset)
				{
				delete iAdvPreset;
				iAdvPreset = NULL;
				}
			iAdvPreset=CIRIsdsPreset::NewL();
			iAdvPreset->SetChannelType(1);
			iAdvPreset->SetImgUrl(*iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl);
			CreateCustomListBoxL();
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			iCategoryView.SetCurrentFocus(0);	
			iLogoDownloadError	= ETrue;
			iCategoryView.AdRequestL();
			}
		else
			{
		    iCategoryListBox = new (ELeave) CIRCategoryAdvertisingListBox;
			CreateCustomListBoxL();
			iCategoryView.SetCurrentFocus(0);	
			iLogoDownloadError	= ETrue;
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			}
		}  
	else
		{
		if(iCategoryView.iUi->iIsdsWrapper->iCatBannerTag)
			{
			iCategoryView.SetCurrentFocus(0);	
			iLogoDownloadError	= ETrue;
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			}
	    iCategoryListBox = new (ELeave) CIRCategoryAdvertisingListBox;
		CreateCustomListBoxL();	
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::CreateListBoxL - Exiting" );

	}
// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::CreateCustomListBoxL()
// Destructor
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::CreateCustomListBoxL()
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::CreateCustomListBoxL - Entering" );
	if(iAdPresent)
		{
	    iCategoryListBox = new (ELeave) CIRCategoryAdvertisingListBox;
		}
	//For displaying Ads
	iCategoryListBox->SetContainerWindowL(*this);

	// Added For Dynamic Filtering
	iCategoryListBox->ConstructL (this,EAknListBoxSelectionList,iFilteredModel);

	// Construct list.
	TResourceReader rr;
	iCoeEnv->CreateResourceReaderLC( rr, R_IR_SEARCHRESULTS_LIST );
	ConstructFromResourceL( rr );
	CleanupStack::PopAndDestroy(); // rr


	iItemArray = static_cast<CDesCArray*>( iCategoryListBox->Model()->ItemTextArray());
    iListBoxArray->SetListArray(iItemArray);
    
    iFilteredModel->SetListArray(iListBoxArray); 
	if (iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl)
		{
		// Added For Dynamic Filtering
		if(iAdvString)
			{
			delete iAdvString;
			iAdvString = NULL;	
			}
		iAdvString = HBufC::NewL(KAdvSize);
		TPtr ptr(iAdvString->Des());
		_LIT(KTabFormat,"\t \t ");
		ptr.Copy(KTabFormat);

		iFilteredModel->SetAdvFlag(iAdPresent,*iAdvString);
		iCategoryListBox->SetListItemFormat( iAdvString );
		}
	iCategoryListBox->SetRect( Rect());
	iCategoryListBox->ActivateL();

	iCategoryListBox->CreateScrollBarFrameL( ETrue );
	iCategoryListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
	CEikScrollBarFrame::EAuto);
	iCategoryListBox->SetListBoxObserver( this );  
	UpdateCategoryViewL(iCategoryView.iIsdsWrapper->iCategoryData);
	iCategoryListBox->ItemDrawer()->FormattedCellData()->SetMarqueeParams
									(KLoop, KScrollAmount, KScrollDelay, KInterval);
	iCategoryListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
	IRLOG_DEBUG( "CIRCategoryViewContainer::CreateCustomListBoxL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::~CIRCategoryViewContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CIRCategoryViewContainer::~CIRCategoryViewContainer()
    {
	IRLOG_DEBUG( "CIRCategoryViewContainer::~CIRCategoryViewContainer - Entering" );
	
	delete iConverter;
	delete iCurrentItemConverter;
	
    if(iCategoryListBox)
		{
		delete iCategoryListBox;
		iCategoryListBox = NULL;
		}
	if(iSearchTextBox)
		{
		delete iSearchTextBox;
		iSearchTextBox = NULL;
		}
	if(iAdvPreset)
		{
		delete iAdvPreset;
		iAdvPreset = NULL;
		}
	if(iAdBitmap)
		{
		delete iAdBitmap;
		iAdBitmap = NULL;	
		}
	if(iBgContext)
		{
		delete iBgContext;
		iBgContext = NULL;	
		}
	if(iAdvString)
		{
		delete iAdvString;
		iAdvString = NULL;	
		}
    if(iListBoxArray )
	    {
	    delete iListBoxArray ;
	    iListBoxArray = NULL;	
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::~CIRCategoryViewContainer - Exiting" );
    }

// ---------------------------------------------------------------------------
// TKeyResponse CIRCategoryViewContainer::OfferKeyEventL()
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things 
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRCategoryViewContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                       TEventCode aType )
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::OfferKeyEventL - Entering" );
	TInt currentIndex = 0;
	TKeyResponse keyResp( EKeyWasNotConsumed );
	switch ( aKeyEvent.iCode )
		{
		case EKeyLeftArrow: // fall-through intended here
		case EKeyRightArrow:
			{
			keyResp = EKeyWasNotConsumed;
			DrawNow();
			break;
			}
		case EKeyOK:        
			{
			currentIndex = GetCurrentItemIndex();
			if (currentIndex == 0)  
				{
				if (iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl 
								&& !iLogoDownloadError)
					{
				 	if(iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl->Length() != 0)
					 	{
					 	iCategoryView.HandleCommandL(EAdvertisementUrl);
					 	}
				 	}
			 	 else
				 	{
			        iCategoryView.HandleCommandL(ESelectCmd);
			 	 	}
			 	 }
			 else
				 {
				 iCategoryView.HandleCommandL(ESelectCmd);				 	
				 }
			keyResp = EKeyWasConsumed;
			break;
			}

		default:
			{
			if(iCategoryListBox)
				{
				if ( iSearchTextBox )
					{
					iFilteredFlag = ETrue;
					keyResp = iSearchTextBox->OfferKeyEventL( aKeyEvent, aType );
					if( keyResp == EKeyWasNotConsumed )
						{
						keyResp = iCategoryListBox->OfferKeyEventL( aKeyEvent, aType );
						}
					CAknFilteredTextListBoxModel* model = 
					static_cast<CAknFilteredTextListBoxModel*>( iCategoryListBox->Model() );
					model->Filter()->HandleOfferkeyEventL();					
					TInt filterModelCount = iFilteredModel->NumberOfItems();	
					if(filterModelCount == 0 && 
					!(iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl))
						{
						HBufC* noMatchesTextResource = 
						StringLoader::LoadLC(R_IRAPP_STATIONLIST_NOMATCHES);
						iCategoryListBox->View()->SetListEmptyTextL(*noMatchesTextResource);
						CleanupStack::PopAndDestroy( noMatchesTextResource );
						}
					iSearchTextBox->GetSearchText(iFindString);
					if(iFindString.Length()!=0)
						{
						if(!iLogoDownloadError && 
						iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl
						&& filterModelCount > 1 )
							{
							iCategoryListBox->SetCurrentItemIndex(1);
							}
						else
							{
							iCategoryListBox->SetCurrentItemIndex(0);	
							}
						}
					}
				}
			if(	aKeyEvent.iScanCode==EStdKeyBackspace)
				{
				if(iSearchTextBox)
					{
					iSearchTextBox->GetSearchText(iFindString);
					if(iFindString.Length()==0)
						{
						iFilteredFlag = EFalse;
						}
					}
				}
			break;
			}
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::OfferKeyEventL - Exiting" );
	return keyResp; 
	}

// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// List box event handler.
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
	IRLOG_DEBUG( "CIRCategoryViewContainer::HandleListBoxEventL - Entering" );
	TInt currentIndex = 0;
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:/**  Keypress event. */
        case EEventItemDoubleClicked:
			{
			currentIndex = GetCurrentItemIndex();
			if (currentIndex == 0)  
				{
				 if (iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl && !iLogoDownloadError)
				 	{
				 	if(iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl->Length() != 0)
					 	{
					 	iCategoryView.HandleCommandL(EAdvertisementUrl);
					 	}
				 	}
			 	 else
				 	{
					iCategoryView.SetCurrentFocus(GetCurrentItemIndex());
					iCategoryView.HandleCommandL(ESelectCmd);
				 	}
			 	}
			else
				{
				iCategoryView.SetCurrentFocus(GetCurrentItemIndex());
				iCategoryView.HandleCommandL(ESelectCmd);
				}

			}
		break;
        default:
        break;
        }
	IRLOG_DEBUG( "CIRCategoryViewContainer::HandleListBoxEventL - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::Draw()
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::Draw( const TRect& /*aRect*/ ) const
    {
	IRLOG_DEBUG( "CIRCategoryViewContainer::Draw - Entering" );
    CWindowGc& gc = SystemGc();
    
    if(iCategoryListBox)
	    {
	   	iCategoryListBox->SetGC(gc);
	   	iCategoryListBox->SetControlFlag(ETrue);
	    }
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	
    // draw background skin first. 
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	
	TRect clientRect(((CEikAppUi*)(CCoeEnv::Static()->AppUi()))->ClientRect());
	AknsDrawUtils::DrawBackground ( skin, iBgContext, this, gc, TPoint(0,0),
	                                TRect(TPoint(0,0),clientRect.iBr), KAknsDrawParamDefault  );	

	IRLOG_DEBUG( "CIRCategoryViewContainer::Draw - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::GetHelpContext()
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
	IRLOG_DEBUG( "CIRCategoryViewContainer::GetHelpContext - Entering" );
    if( iCategoryView.iUi->iCategoryViewTitleType == CIRCategoryView::EGenreData )
        {
        aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_GENRE );
        }
    else if( iCategoryView.iUi->iCategoryViewTitleType == CIRCategoryView::ELanguageData )
        {
        aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_LANGUAGE );
        }
    else if( iCategoryView.iUi->iCategoryViewTitleType == CIRCategoryView::ECountryData )
        {
        aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_COUNTRY );
        }
    else
        {
        // NOP
        }

	IRLOG_DEBUG( "CIRCategoryViewContainer::GetHelpContext - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::UpdateSearchResultsViewL()
// updates the search results data to the search results list
// ---------------------------------------------------------------------------
//    
void CIRCategoryViewContainer::UpdateCategoryViewL(CDesCArray* aCategory)
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::UpdateCategoryViewL - Entering" );
	CDesCArray* category = aCategory; 
	TInt categoryDataCount = aCategory->Count();           
	for(TInt i = 0; i < categoryDataCount; i++)
		{
		iItemArray->AppendL(category->MdcaPoint(i));
		}
	if(categoryDataCount>1)
		{
		if(iSearchTextBox)
			{
			delete iSearchTextBox;
			iSearchTextBox = NULL;
			}

		// Added For Dynamic Filtering
		iSearchTextBox = CreateFindBoxL(iCategoryListBox, iFilteredModel,
		  CAknSearchField::ESearch);
		SizeChanged();	
		}
	else
		{
		DisableFindBox();	
		}
	if(iFilteredFlag)
		{
		TKeyEvent event;
		TEventCode type = EEventKey;
		event.iCode = EKeyDownArrow;
		TBool needRefresh( EFalse );
		AknFind::HandleFindOfferKeyEventL( event, type, this,
		iCategoryListBox, iSearchTextBox,EFalse,needRefresh );
		TInt filterModelCount = iFilteredModel->NumberOfItems();	
		if(filterModelCount ==1 && iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl)
			{
			iCategoryListBox->SetCurrentItemIndex(0);
			}
		else
			{
			if(filterModelCount>1 && iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl && !iLogoDownloadError)
			iCategoryListBox->SetCurrentItemIndex(1);
			}
		}
	else
		{
		iCategoryListBox->HandleItemAdditionL();
		}
			
	IRLOG_DEBUG( "CIRCategoryViewContainer::UpdateCategoryViewL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::GetCurrentItemIndex()
// returns the index of the currently selected item
// ---------------------------------------------------------------------------
//
TInt CIRCategoryViewContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRCategoryViewContainer::GetCurrentItemIndex" );
	return iCategoryListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::GetListBoxTopIndex()
// returns the index of the top item
// ---------------------------------------------------------------------------
//
TInt CIRCategoryViewContainer::GetListBoxTopIndex() const
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::GetListBoxTopIndex" );
	return iCategoryListBox->View()->TopItemIndex () ;
	}


//----------------------------------------------------------------------------
// CIRCategoryViewContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRCategoryViewContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::CountComponentControls - Entering" );
	IRLOG_DEBUG( "CIRCategoryViewContainer::CountComponentControls - Exiting" );
	TInt count(KCONTROLSCOUNT);
	if(iCategoryListBox)
		{
		count++;
		}
	if(iSearchTextBox)
		{
		count++;
		}
	return count;
	}
//----------------------------------------------------------------------------
// CIRCategoryViewContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRCategoryViewContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRCategoryViewContainer::ComponentControl - Exiting" );
	switch(aIndex)
		{
		case 0:
			{
			if(iCategoryListBox)
			return iCategoryListBox;
			}
		case 1:
			{
			if(iSearchTextBox)
			return iSearchTextBox;
			}
		default:
			return NULL;
		}
	}
// ---------------------------------------------------------------------------
// void CIRCategoryViewContainer::SizeChanged()
// Responds to changes to the size and position of the contents of this control.
// ---------------------------------------------------------------------------
//

void CIRCategoryViewContainer::SizeChanged()
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::SizeChanged - Entering" );
	if(iCategoryView.iUi->IsLandscapeOrientation())
		{
		if(iCategoryListBox && iSearchTextBox && iLandScapeCount ==0)
			{
	   		AknFind::HandlePopupFindSizeChanged(this,iCategoryListBox,iSearchTextBox);
	   		iLandScapeCount ++;
			}
		}
	if(iCategoryListBox && iSearchTextBox)
		{
	    AknLayoutUtils::LayoutControl(iCategoryListBox,  Rect(),
            AKN_LAYOUT_WINDOW_list_gen_pane(1));
        AknLayoutUtils::LayoutControl(iSearchTextBox,  Rect(),
            AKN_LAYOUT_WINDOW_find_pane);
		}
	else
		{
		if(iCategoryListBox)
			{
			AknLayoutUtils::LayoutControl(iCategoryListBox, Rect(),
            	AKN_LAYOUT_WINDOW_list_gen_pane(0));
			}
		}
		
	TRect clientRect(((CEikAppUi*)(CCoeEnv::Static()->AppUi()))->ClientRect());
	TRect parentRectSize(TPoint(0,0),clientRect.iBr);

	if ( iBgContext )
		{
		iBgContext->SetRect(parentRectSize);
		}
	
	DrawNow();
	IRLOG_DEBUG( "CIRCategoryViewContainer::SizeChanged - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRCategoryViewContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::HandleResourceChange(TInt aType)
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::HandleResourceChange" );	
    CCoeControl::HandleResourceChange( aType );
    // application layout change request notification
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // reconstruct controls if needed
        if(iCategoryListBox && iSearchTextBox)
	        {
		    AknFind::HandlePopupFindSizeChanged(this,iCategoryListBox,iSearchTextBox);
	        SizeChanged();
	        }
        }
	}
//----------------------------------------------------------------------------
// CIRCategoryViewContainer::CreateFindBoxL()
// creates the Find box when any character is Pressed
//----------------------------------------------------------------------------
// 
CAknSearchField* CIRCategoryViewContainer::CreateFindBoxL(CEikListBox* aListBox,
CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle)
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::CreateFindBoxL - Entering" );
	CAknSearchField* findbox = NULL;
	if (aListBox && aModel)
		{
		// Gets pointer of CAknFilteredTextListBoxModel.
		// Added For Dynamic Filtering
		 iFilteredModel = STATIC_CAST( CIRFilteredModel*, aModel );

		// Creates FindBox.
		findbox = CAknSearchField::NewL( *this, aStyle, NULL,
		KAknExListFindBoxTextLength);
		CleanupStack::PushL(findbox);
		// Creates CAknListBoxFilterItems class.
		iFilteredModel->CreateFilterL( aListBox, findbox );
		CleanupStack::Pop(findbox); // findbox
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::CreateFindBoxL - Exiting" );
	return findbox;
	}
//----------------------------------------------------------------------------
// CIRCategoryViewContainer::DisableFindBox()
// Deletes the findbox
//----------------------------------------------------------------------------
// 
void CIRCategoryViewContainer::DisableFindBox()
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::DisableFindBox - Entering" );
	if (iSearchTextBox)
		{
		iFilteredModel->RemoveFilter();
		// Important to set the find box non-focusing before
		// deleting it, otherwise the focus changes triggered
		// by the removal of the control from stack will focus
		// the find box which is under deletion
		iSearchTextBox->SetNonFocusing();
		delete iSearchTextBox;
		iSearchTextBox = NULL;
		// Inform list box that find is hidden
		static_cast<CAknColumnListBoxView*>(iCategoryListBox->View())->SetFindEmptyListState(EFalse);
		SizeChanged();
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::DisableFindBox - Exiting" );
	}


// ---------------------------------------------------------------------------
// Function : Comparestrings
// Function called to compare the filtered string inthe Itemarray
// ---------------------------------------------------------------------------
//
void  CIRCategoryViewContainer::Comparestrings()
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::Comparestrings - Entering" );
	TInt count = iFilteredModel->NumberOfItems();
	if(count)
		{
		if((count==1) && iAdvPreset &&!iLogoDownloadError )
			{
			return;
			}
		else
			{
			TPtrC16 filteredString = iFilteredModel->ItemText (iCategoryView.GetFilteredIndex());
			for(TInt i=0;i<iItemArray->Count();i++)
				{
				TPtrC16 matchString=iItemArray->MdcaPoint(i);
				if(filteredString==matchString)
					{
					iCategoryView.SetActualFilteredItemIndex(i);
					break;
					}
				}
			}
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::Comparestrings - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRCategoryViewContainer::HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent aEvent, 
// from base class MIRImageConverterObserver
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::HandleImageConversionEventL( MIRImageConverterObserver::
                                                            TIRImageConversionEvent aEvent,
                                                           TInt /*aId*/, 
                                                           TInt aError )
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::HandleImageConversionEventL - Entering" );
	iError = aError;
	if(aError == KErrNotSupported && iRequestForAdvertisement)
		{
		iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
		iLogoDownloadError	= ETrue;
		iRequestForAdvertisement = EFalse;
		}
	if ( !aError && (aEvent == MIRImageConverterObserver::EIRFrameConversionCompleted ||
					 aEvent == MIRImageConverterObserver::EIRImageConversionCompleted))
		{
		IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleImageConversionEventL - Entered elseif" );
		iLogoDownloadError	= EFalse;
		CGulIcon* icon = iCategoryListBox->AdvertisementIcon(); 
		if ( !icon )
			{
			icon = CGulIcon::NewLC();
			iCategoryListBox->SetAdvertisementIconL( icon );
			CleanupStack::Pop( icon );
			}
		icon->SetBitmap( const_cast<CFbsBitmap*>(iCurrentItemConverter->Bitmap()));
		icon->SetMask(const_cast<CFbsBitmap*>(iCurrentItemConverter->Mask() ));
		icon->SetBitmapsOwnedExternally( ETrue );
		DrawDeferred();
		}
	else
		{

		}
	if(iFilteredFlag)
		{
		TKeyEvent event;
		TEventCode type = EEventKey;
		event.iCode = EKeyDownArrow;
		TBool needRefresh( EFalse );
		AknFind::HandleFindOfferKeyEventL( event, type, this,
		iCategoryListBox, iSearchTextBox,EFalse,needRefresh );
		}
	iAdPresent = EFalse;
	IRLOG_DEBUG( "CIRCategoryViewContainer::HandleImageConversionEventL - Exiting" );	
	}

// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::PresetLogoDownloadL()
// 
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::PresetLogoDownloadL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::PresetLogoDownloadL - Entering" );
	if(iAdPresent)
		{
		if (iCategoryView.iUi->iIsdsWrapper->iCatBannerUrl)
			{
			*iAdvPreset = *aPreset;	
			if(iAdvPreset->GetLogoData()!=KNullDesC8 )
				{
				iFilteredModel->SetAdvFlag(ETrue,*iAdvString);
				iError = KErrNone;
				iCurrentItemConverter->SetDataL(aPreset->GetLogoData());
				if(iError==KErrNone)
					{
					iCurrentItemConverter->StartL( IRLayoutUtils::AdvertisementSizeL(), KErrUnknown );	//KLogoSize
					}
				iRequestForAdvertisement = ETrue;	
				}
			}
		}
	IRLOG_DEBUG( "CIRCategoryViewContainer::PresetLogoDownloadL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRCategoryViewContainer::OpenUrlL( const TDesC& aHttpLink)
// used to connect to the advertisement url.
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::OpenUrlL( const TDesC& aHttpLink )
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::OpenUrlL - Entering" );

	TDataType htmlDataType( KIRHtmlMimeType );
	TUid uid;

	RApaLsSession ls;
	User::LeaveIfError( ls.Connect() );
	CleanupClosePushL( ls );

	// Get UID of the browser.
	TInt retVal = ls.AppForDataType( htmlDataType, uid );

	if ( retVal == KErrNone && uid != KNullUid )
		{
		// We must check if the browser is already open and if it is, 
		// switch its currently opened file to the correct URL.
		TApaTaskList taskList( iEikonEnv->WsSession() );
		TApaTask task = taskList.FindApp( uid );
		if ( task.Exists() )
			{
			HBufC8* url8 = HBufC8::NewLC( aHttpLink.Length() );
			url8->Des().Copy( aHttpLink );
			task.SendMessage( TUid::Uid( 0 ), url8->Des() );
			task.BringToForeground();
			CleanupStack::PopAndDestroy( url8 );
			}
		else
			{
			TThreadId threadId;
			HBufC* newUrl = HBufC::NewLC( aHttpLink.Length() + KBrowserPrefix().Length() );
			newUrl->Des().Append( KBrowserPrefix );
			newUrl->Des().Append( aHttpLink );
			ls.StartDocument( newUrl->Des(), uid, threadId );
			CleanupStack::PopAndDestroy( newUrl );
			}
		}
	CleanupStack::PopAndDestroy( &ls );

	IRLOG_DEBUG( "CIRCategoryViewContainer::OpenUrlL - Exiting" );

	}
//----------------------------------------------------------------------------
// CIRCategoryViewContainer::GetLogoNotSupported() 
// returns whether the advertisement is supported by the sdk r not
//----------------------------------------------------------------------------
// 	
TBool CIRCategoryViewContainer::GetLogoNotSupported()
	{
	IRLOG_DEBUG( "CIRCategoryViewContainer::GetLogoNotSupported - Entering" );
	IRLOG_DEBUG( "CIRCategoryViewContainer::GetLogoNotSupported - Exiting" );
	return iLogoDownloadError;
	}
// ---------------------------------------------------------------------------
// void CIRCategoryViewContainer::HandleLogoErrorL()
// called from the iCategoryView when the logo is not downloaded
// ---------------------------------------------------------------------------
//
void CIRCategoryViewContainer::HandleLogoErrorL()
	{
	iLogoDownloadError	= ETrue;
	iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
	}
