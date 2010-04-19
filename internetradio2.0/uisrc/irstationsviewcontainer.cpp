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
* Description:  Container class for Stations View
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
*  Stations View Container Derived from CCoeControl
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
* changed code in HandleResourcechange and Size changed  for handling layout changes 
* in dynamic filtering
* ============================================================================
*/
/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 8, Thurs July 18 12:00:00 2008 by Prabina
*  Ref:
*  Code Review Comments Fixed
*
*
*  </ccm_history>
* ============================================================================
*/

#include <AknIconArray.h>
#include <aknlists.h>
#include <aknview.h>
#include <barsread.h>
#include <eikenv.h>
#include <eiklabel.h>
#include <eikmenub.h>
#include <stringloader.h>
#include <internetradio.rsg>
#include <internetradio.mbg>
#include <eikenv.h>
#include <apgcli.h>
#include <akntoolbar.h>


#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irstationsview.h"
#include "irstationsviewcontainer.h"
#include "irapplication.h"
#include "ircategoryview.h"
#include "iraap.hlp.hrh"
#include "irisdswrapper.h"
#include "irimageconverter.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irlogodownloadengine.h"
/* no need to change to lower case  */
#include <AknsBasicBackgroundControlContext.h> 
#include "irfilteredmodel.h"
#include "mirlistboxarray.h"
#include "iradvertisinglistbox.h"
#include "irlayoututils.h"
//For Ads
#include <akniconutils.h> 
#include <aknsfld.h> 
/** Mime type when opening web browser */
_LIT8( KIRHtmlMimeType, "text/html" );
/** Browser prefix: See browser API specification */
_LIT( KBrowserPrefix, "4 " );
_LIT(KFalse,"false");

const TInt KAdvSize = 30;
const TSize KLogoSize = TSize(320,350);
const TInt KSize = 10;
#define KCONTROLSCOUNT 0
 
const TInt KIRNoLogo = -1;
// Length of list item with known sized elements: (the number of column separators) + 3 (maximum length for icon id)
const TInt KIRListItemBaseLength = 5;
const TInt KRange = 999;
// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIRStationsViewContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationsViewContainer* CIRStationsViewContainer::NewL( CIRStationsView& aView,
					 const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRStationsViewContainer::NewL" );
    CIRStationsViewContainer* self = CIRStationsViewContainer::NewLC( aView, aRect );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRStationsViewContainer::NewL - Exiting" );
    return self;
    }

// -----------------------------------------------------------------------------
// CIRStationsViewContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationsViewContainer* CIRStationsViewContainer::NewLC( CIRStationsView& aView,
					 const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRStationsViewContainer::NewLC" );
    CIRStationsViewContainer* self = new ( ELeave ) CIRStationsViewContainer(aView);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    IRLOG_DEBUG( "CIRStationsViewContainer::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRStationsViewContainer::CIRStationsViewContainer()  
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRStationsViewContainer::CIRStationsViewContainer(CIRStationsView& aView) :iView(aView)
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::CIRStationsViewContainer" );
	iFilteredFlag = EFalse;
	iAdRequestStatusFlag = EFalse;
	}
    
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::ConstructL( const TRect& aRect)
    {
	IRLOG_DEBUG( "CIRStationsViewContainer::ConstructL - Entering" );
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
    
	
	IRLOG_DEBUG( "CIRStationsViewContainer::ConstructL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::CreateListBoxL()
// Creates ListBox depending upon the presence of Advertisement
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::CreateListBoxL()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateListBoxL - Entering" );
	if (iView.iUi->iIsdsWrapper->GetBannerUrl())
		{
		if((iView.iUi->iIsdsWrapper->GetBannerUrl()->Length())!=0)
			{
			iAdPresent = ETrue;
			if(iAdvPreset)
				{
				delete iAdvPreset;
				iAdvPreset = NULL;
				}
			iAdvPreset=CIRIsdsPreset::NewL();
			iAdvPreset->SetChannelType(1);
			iAdvPreset->SetImgUrl(*iView.iUi->iIsdsWrapper->GetBannerUrl());
			iAdRequestStatusFlag = ETrue;
			CreateCustomListBoxL();
			iLogoDownloadError	= ETrue;
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			iView.AdRequestL();
			}
		}  
	else
		{
		if(iView.iUi->iIsdsWrapper->iChannelBannerTag)
			{
			iView.SetCurrentFocus(0);	
			iLogoDownloadError	= ETrue;
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			iAdRequestStatusFlag = EFalse;
			}
	    iStationsListBox = new (ELeave) CIRAdvertisingListBox;
		CreateCustomListBoxL();	
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateListBoxL - Exiting" );

	}
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::~CIRStationsViewContainer()
// Destructor
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::CreateCustomListBoxL()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateCustomListBoxL - Entering" );
	iLandScapeCount = 0;
	if(iAdPresent)
		{
		iStationsListBox = new (ELeave) CIRAdvertisingListBox;
		}
	//For displaying Ads
	iStationsListBox->SetContainerWindowL(*this);
	// Added For Dynamic Filtering
	iStationsListBox->ConstructL (this,EAknListBoxSelectionList,iFilteredModel);


	// Construct list.
	TResourceReader rr;
	iCoeEnv->CreateResourceReaderLC( rr, R_IR_SEARCHRESULTS_LIST );
	ConstructFromResourceL( rr );
	CleanupStack::PopAndDestroy(); // rr

	iItemArray = static_cast<CDesCArray*>( iStationsListBox->Model()->ItemTextArray());
	iListBoxArray->SetListArray(iItemArray);
	iFilteredModel->SetListArray(iListBoxArray); 
	
	if (iView.iUi->iIsdsWrapper->GetBannerUrl())
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
		iStationsListBox->SetListItemFormat( iAdvString );
		}

	TInt resultsCount = iView.iIsdsWrapper->iStationsData->Count();

	CAknIconArray* iconArray = new ( ELeave ) CAknIconArray( resultsCount );
	CleanupStack::PushL( iconArray );

	for ( TInt i = 0; i < resultsCount; i++ )
		{
		CGulIcon* icon = CreateDefaultIconL();
        CleanupStack::PushL( icon );
        iconArray->AppendL( icon );
	    CleanupStack::Pop( icon );
		}

	CleanupStack::Pop( iconArray  );
	iStationsListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iconArray );  

	iStationsListBox->SetRect( Rect());
	iStationsListBox->ActivateL();

	iStationsListBox->CreateScrollBarFrameL( ETrue );
	iStationsListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
	CEikScrollBarFrame::EAuto);
	iStationsListBox->SetListBoxObserver( this );     


	// Set the icon array for this list. List takes ownership of the array. 
	for ( TInt i = 0; i < resultsCount; i++ )
		{
		RBuf tempImgUrlStatus;
		tempImgUrlStatus.Create(KSize);
		tempImgUrlStatus.Copy(KFalse);

		RBuf tempImgUrl;
		tempImgUrl.Create(iView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(i).Size());
		tempImgUrl.Copy(iView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(i));

		if(!(tempImgUrlStatus == tempImgUrl))
			{
			iInvalidIconIndices.InsertInOrderL(i);
			}
		iNoLogosArray.InsertInOrderL(i);
		tempImgUrlStatus.Close();
		tempImgUrl.Close();
		}

	UpdateStationsViewL();
	iStationsListBox->ItemDrawer()->FormattedCellData()->SetMarqueeParams
							(KLoop, KScrollAmount, KScrollDelay, KInterval);
	iStationsListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

	if(resultsCount>1)
		{
		if(iStationsSearchBox)
			{
			delete iStationsSearchBox;
			iStationsSearchBox = NULL;
			}

		// Added For Dynamic Filtering
		iStationsSearchBox = CreateFindBoxL(iStationsListBox, iFilteredModel,
		CAknSearchField::ESearch);
		SizeChanged();	
		}
	else
		{
		DisableFindBox();	
		}
	if((!iAdRequestStatusFlag))
		{
		if ( iInvalidIconIndices.Count() )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateCustomListBoxL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::CreateDefaultIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CIRStationsViewContainer::CreateDefaultIconL()
    {
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateDefaultIconL - Entering" );
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( TSize(), EGray2 ) );
    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( mask );
    User::LeaveIfError( mask->Create( TSize(), EGray2 ) );
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2, bitmap );
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateDefaultIconL - Exiting" );
    return icon;
    }
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::~CIRStationsViewContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CIRStationsViewContainer::~CIRStationsViewContainer()
    {
	IRLOG_DEBUG( "CIRStationsViewContainer::~CIRStationsViewContainer - Entering" );
	delete iConverter;
	delete iCurrentItemConverter;
	iLogosArray.Close();
	iNoLogosArray.Close();  
	iInvalidIconIndices.Close();
    if(iStationsListBox)
    	{
    	delete iStationsListBox;
    	iStationsListBox = NULL;
    	}
    if(iStationsSearchBox)
		{
		delete iStationsSearchBox;
		iStationsSearchBox = NULL;
		}
	if(iTempPreset)
		{
		delete iTempPreset;
		iTempPreset =NULL;
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
	IRLOG_DEBUG( "CIRStationsViewContainer::~CIRStationsViewContainer - Exiting" );
    }

// ---------------------------------------------------------------------------
// TKeyResponse CIRStationsViewContainer::OfferKeyEventL()
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRStationsViewContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::OfferKeyEventL - Entering" );
	TKeyResponse keyResp( EKeyWasNotConsumed );
	TInt currentIndex = 0;
	switch ( aKeyEvent.iCode )
		{
		case EKeyLeftArrow: // fall-through intended here
		case EKeyRightArrow:
		case EKeyIncVolume:
		case EKeyDecVolume:
			{
			keyResp = EKeyWasNotConsumed;
			break;
			}
		case EKeyOK:        
			{
			if( iView.iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			currentIndex = GetCurrentItemIndex();
			if (currentIndex == 0)  
				{
				if (iView.iUi->iIsdsWrapper->GetBannerUrl() && !iLogoDownloadError)
					{
					if(iView.iUi->iIsdsWrapper->GetBannerUrl()->Length() != 0)
						{
						iView.HandleCommandL(EAdvertisementUrl);
						}
					}
				else
					{
					iView.SetCurrentFocus(GetCurrentItemIndex());
					iView.HandleCommandL(EListenCmd);
					}
				}
			else
				{
				iView.SetCurrentFocus(GetCurrentItemIndex());
				iView.HandleCommandL(EListenCmd);
				}
			break;
			}
		default:
			{
			if(iStationsListBox)
				{
				iView.SetToolbarItems();
				if ( iStationsSearchBox )
					{
					iFilteredFlag = ETrue;
					keyResp = iStationsSearchBox->OfferKeyEventL( aKeyEvent, aType );
					if( keyResp == EKeyWasNotConsumed )
						{
						keyResp = iStationsListBox->OfferKeyEventL( aKeyEvent, aType );
						}
					CAknFilteredTextListBoxModel* model = 
					static_cast<CAknFilteredTextListBoxModel*>( iStationsListBox->Model() );
					model->Filter()->HandleOfferkeyEventL();					
					TInt filterModelCount = iFilteredModel->NumberOfItems();	
					if(filterModelCount == 0 && 
					!(iView.iUi->iIsdsWrapper->GetBannerUrl()))
						{
						HBufC* noMatchesTextResource = 
						StringLoader::LoadLC(R_IRAPP_STATIONLIST_NOMATCHES);
						iStationsListBox->View()->SetListEmptyTextL(*noMatchesTextResource);
						CleanupStack::PopAndDestroy( noMatchesTextResource );
						}
					iStationsSearchBox->GetSearchText(iFindString);
					if(iFindString.Length()!=0)
						{
						if(!iLogoDownloadError && 
						iView.iUi->iIsdsWrapper->GetBannerUrl() && 
						filterModelCount > 1 )
							{
							iStationsListBox->SetCurrentItemIndex(1);
							}
						else
							{
							iStationsListBox->SetCurrentItemIndex(0);
							}
						}
					}
				}
			if(	aKeyEvent.iScanCode==EStdKeyBackspace)
				{
				if(iStationsSearchBox)
					{
					iStationsSearchBox->GetSearchText(iFindString);
					if(iFindString.Length()==0)
						{
						iFilteredFlag = EFalse;
						}
					}
				}

			iView.SetToolbarItems();
			break;
			}
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::OfferKeyEventL - Exiting" );
	return keyResp; 
	}

// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// List box event handler.
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleListBoxEventL - Entering" );
	TInt currentIndex = 0;
	switch ( aEventType )
		{
		case EEventEnterKeyPressed:/**  Keypress event. */
		case EEventItemDoubleClicked:
			{
			if( iView.iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			currentIndex = GetCurrentItemIndex();
			if (currentIndex == 0)  
				{
				if (iView.iUi->iIsdsWrapper->GetBannerUrl() && !iLogoDownloadError)
					{
					if(iView.iUi->iIsdsWrapper->GetBannerUrl()->Length() != 0)
						{
						iView.HandleCommandL(EAdvertisementUrl);
						}
					}
				else
					{
					iView.SetCurrentFocus(GetCurrentItemIndex());
					iView.HandleCommandL(EListenCmd);
					}
				}
			else
				{
				iView.SetCurrentFocus(GetCurrentItemIndex());
				iView.HandleCommandL(EListenCmd);
				}
			}
		break;
		case EEventItemClicked:
			{
			iView.SetCurrentFocus(GetCurrentItemIndex());
			iView.SetToolbarItems();
			}
		break;
		default:
		break;
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleListBoxEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationsViewContainer::Draw()
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::Draw( const TRect& /*aRect*/ ) const
    {
	IRLOG_DEBUG( "CIRStationsViewContainer::Draw - Entering" );
    CWindowGc& gc = SystemGc();

    if(iStationsListBox)
	    {
	   	iStationsListBox->SetGC(gc);
	   	iStationsListBox->SetControlFlag(ETrue);
	    }
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	
    // draw background skin first. 
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	
	TRect clientRect(((CEikAppUi*)(CCoeEnv::Static()->AppUi()))->ClientRect());

	AknsDrawUtils::DrawBackground ( skin, iBgContext, this, gc, TPoint(0,0),
					 TRect(TPoint(0,0),clientRect.iBr), KAknsDrawParamDefault  );	
	
 	IRLOG_DEBUG( "CIRStationsViewContainer::Draw - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRStationsViewContainer::GetHelpContext()
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
	IRLOG_DEBUG( "CIRStationsViewContainer::GetHelpContext - Entering" );
	if( iView.iUi->iCategoryViewTitleType == CIRCategoryView::EGenreData )
		{
		aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_GENRE_STATIONS );
		}
	else if( iView.iUi->iCategoryViewTitleType == CIRCategoryView::ELanguageData )
		{
		aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_LANGUAGE_STATIONS );
		}
	else if( iView.iUi->iCategoryViewTitleType == CIRCategoryView::ECountryData )
		{
		aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_COUNTRY_STATIONS );
		}
	else 
		{
		aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_TOP );
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::GetHelpContext - Exiting" );

    }
    
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::UpdateSearchResultsViewL()
// updates the search results data to the search results list
// ---------------------------------------------------------------------------
//    

void CIRStationsViewContainer::UpdateStationsViewL()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::UpdateStationsViewL - Entering" );

	//provides logo url in the search results stations
	TInt stationsCount = iView.iIsdsWrapper->iStationsData->Count();
	for(TInt i = 0; i < stationsCount; i++)
		{
		//check whether logo url exist for the current channel of the search results
		for(TInt m = 0; m < iNoLogosArray.Count(); m++)
			{
			if(i == iNoLogosArray[m])				
				{
				//logo should not be displayed in the search results for this particular station
				HBufC* itemString = FormatItemStringLC( iView.iIsdsWrapper->iStationsData->
														MdcaPoint(i),KIRNoLogo);
				iItemArray->AppendL( itemString->Des() );
				CleanupStack::PopAndDestroy( itemString );
				
				break;
				}
			}
			
		for(TInt n = 0; n < iLogosArray.Count(); n++)
			{
			if(i == iLogosArray[n])
				{
				//logo should be displayed in the search results for this particular station
				HBufC* itemString = FormatItemStringLC( iView.iIsdsWrapper->iStationsData->
														MdcaPoint(i),i);
				iItemArray->AppendL( itemString->Des() );
				CleanupStack::PopAndDestroy( itemString );
				break;
				}
			}
		}
	if(iFilteredFlag)
		{
		TKeyEvent event;
		TEventCode type = EEventKey;
		event.iCode = EKeyDownArrow;
		TBool needRefresh( EFalse );
		AknFind::HandleFindOfferKeyEventL( event, type, this,
		iStationsListBox, iStationsSearchBox,EFalse,needRefresh );
		TInt filterModelCount = iFilteredModel->NumberOfItems();	
		if(filterModelCount ==1 && iView.iUi->iIsdsWrapper->GetBannerUrl())
			{
			iStationsListBox->SetCurrentItemIndex(0);
			}
		else
			{
			if(filterModelCount>1 && iView.iUi->iIsdsWrapper->GetBannerUrl() && !iLogoDownloadError)
			iStationsListBox->SetCurrentItemIndex(1);
			}
		}
	else
		{
		iStationsListBox->HandleItemAdditionL();
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::UpdateStationsViewL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::FormatItemStringLC
// Formatter for list item strings.
// ---------------------------------------------------------------------------
//
HBufC* CIRStationsViewContainer::FormatItemStringLC( const TDesC& aFirstRow, 
											 TInt aIconId )
    {
 	_LIT(KItemFormatStringWithLogo, "%d%S"); 
	_LIT(KItemFormatStringWithOutLogo, "%S");   

    TInt length = KIRListItemBaseLength + aFirstRow.Length() ;
    HBufC* itemString = HBufC::NewLC( length );
    
    TPtrC formatStringWithIcon( KItemFormatStringWithLogo );
    TPtrC formatStringWithoutIcon( KItemFormatStringWithOutLogo );

    if( aIconId >= 0 && aIconId <= KRange )
        {
        itemString->Des().Format( KItemFormatStringWithLogo, aIconId, &aFirstRow);
        }
    else 
        {
        itemString->Des().Format( KItemFormatStringWithOutLogo, &aFirstRow);
        }
    
    return itemString;
    }

// ---------------------------------------------------------------------------
// CIRStationsViewContainer::GetCurrentItemIndex()
// returns the index of the currently selected item
// ---------------------------------------------------------------------------
//

TInt CIRStationsViewContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRStationsViewContainer::GetCurrentItemIndex" );
	return iStationsListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CIRStationsViewContainer::GetListBoxTopIndex()
// returns the index of the top item
// ---------------------------------------------------------------------------
//

TInt CIRStationsViewContainer::GetListBoxTopIndex() const
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::GetListBoxTopIndex" );
	return iStationsListBox->View()->TopItemIndex () ;
	}
// ---------------------------------------------------------------------------
// CIRStationsViewContainer::GetSearchBoxPointer()
// returns the search box pointer.
// ---------------------------------------------------------------------------
//
CAknSearchField* CIRStationsViewContainer::GetSearchBoxPointer()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::GetSearchBoxPointer" );
	return 	iStationsSearchBox;
	}

// ---------------------------------------------------------------------------
// CIRStationsViewContainer::GetFilterdModelPointer()
// returns pointer to filtered model.
// ---------------------------------------------------------------------------
//
CIRFilteredModel* CIRStationsViewContainer::GetFilterdModelPointer()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::GetListPointer" );
	return iFilteredModel;	
	}
//----------------------------------------------------------------------------
// CIRStationsViewContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRStationsViewContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::CountComponentControls - Entering" );
	TInt count(KCONTROLSCOUNT);
	IRLOG_DEBUG( "CIRStationsViewContainer::CountComponentControls - Exiting" );
	if(iStationsListBox)
		{
		count++;
		}
	if(iStationsSearchBox)
		{
		count++;
		}
	return count;
	}
//----------------------------------------------------------------------------
// CIRStationsViewContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRStationsViewContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRStationsViewContainer::ComponentControl - Exiting" );
	switch(aIndex)
		{
		case 0:
			{
			if(iStationsListBox)
			return iStationsListBox;
			}
		case 1:
			{
			if(iStationsSearchBox)
			return iStationsSearchBox;
			}
		default:
			return NULL;
		}
	}
// ---------------------------------------------------------------------------
// void CIRStationsViewContainer::SizeChanged(const TRect& aRect)
// Responds to changes to the size and position of the contents of this control.
// ---------------------------------------------------------------------------
//

void CIRStationsViewContainer::SizeChanged()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::SizeChanged - Entering" );
	if(iView.iUi->IsLandscapeOrientation())
		{
		if(iStationsListBox && iStationsSearchBox && iLandScapeCount ==0)
			{
	   		AknFind::HandlePopupFindSizeChanged(this,iStationsListBox,iStationsSearchBox);
	   		iLandScapeCount ++;
			}
		}
	if(iStationsListBox && iStationsSearchBox)
		{
	    AknLayoutUtils::LayoutControl(iStationsListBox,  Rect(),
            AKN_LAYOUT_WINDOW_list_gen_pane(1));
        AknLayoutUtils::LayoutControl(iStationsSearchBox,  Rect(),
            AKN_LAYOUT_WINDOW_find_pane);
		}
	else
		{
		if(iStationsListBox)
			{
			AknLayoutUtils::LayoutControl(iStationsListBox, Rect(),
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
	IRLOG_DEBUG( "CIRStationsViewContainer::SizeChanged - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationsViewContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::HandleResourceChange(TInt aType)
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
    // application layout change request notification
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // reconstruct controls if needed
        if(iStationsListBox && iStationsSearchBox )
	        {
	   		AknFind::HandlePopupFindSizeChanged(this,iStationsListBox,iStationsSearchBox);
	        SizeChanged();
	        }
        }
	iView.SetToolbarItems();
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleResourceChange - Exiting" );
	}
//----------------------------------------------------------------------------
// CIRStationsViewContainer::CreateFindBoxL()
// creates the Find box when any character is Pressed
//----------------------------------------------------------------------------
// 
CAknSearchField* CIRStationsViewContainer::CreateFindBoxL(CEikListBox* aListBox,
CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle)
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateFindBoxL - Entering" );
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
	IRLOG_DEBUG( "CIRStationsViewContainer::CreateFindBoxL - Exiting" );
	return findbox;
	}
//----------------------------------------------------------------------------
// CIRStationsViewContainer::DisableFindBox()
// Deletes the findbox
//----------------------------------------------------------------------------
// 
void CIRStationsViewContainer::DisableFindBox()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::DisableFindBox - Entering" );
	if (iStationsSearchBox)
		{
		iFilteredModel->RemoveFilter();
		// Important to set the find box non-focusing before
		// deleting it, otherwise the focus changes triggered
		// by the removal of the control from stack will focus
		// the find box which is under deletion
		iStationsSearchBox->SetNonFocusing();
		delete iStationsSearchBox;
		iStationsSearchBox = NULL;
		// Inform list box that find is hidden
		static_cast<CAknColumnListBoxView*>(iStationsListBox->View())->SetFindEmptyListState(EFalse);
		SizeChanged();
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::DisableFindBox - Exiting" );
	}

// ---------------------------------------------------------------------------
// Function : Comparestrings
// Function called to compare the filtered string inthe Itemarray
// ---------------------------------------------------------------------------
//
void  CIRStationsViewContainer::Comparestrings()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::Comparestrings - Entering" );
	TInt count = iFilteredModel->NumberOfItems();	
	if(count)
		{
		if((count==1) && iAdvPreset && !iLogoDownloadError)
			{
			return;
			}
		else
			{
			TPtrC16 filteredString = iFilteredModel->ItemText (iView.GetFilteredIndex());
			for(TInt i=0;i<iItemArray->Count();i++)
				{
				TPtrC16 matchString=iItemArray->MdcaPoint(i);
				if(filteredString==matchString)
					{
					iView.SetActualFilteredItemIndex(i);
					break;
					}
				}
			}
		}
	IRLOG_DEBUG( "CIRStationsViewContainer::Comparestrings - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRStationsViewContainer::HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent /*aEvent*/, 
// from base class MIRImageConverterObserver
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::HandleImageConversionEventL( 
						MIRImageConverterObserver::TIRImageConversionEvent aEvent, 
                                                           TInt aId, 
                                                           TInt aError )
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleImageConversionEventL - Entering" );
	iError=aError;
	if(aError == KErrNotSupported && iRequestForAdvertisement)
		{
		iLogoDownloadError	= ETrue;
		iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
		iRequestForAdvertisement = EFalse;
		}
	if ( !aError && aEvent == MIRImageConverterObserver::EIRImageConversionCompleted )
		{
		IRLOG_DEBUG( "CIRStationsViewContainer::HandleImageConversionEventL - Entering if" );
		if(iRequestForAdvertisement)
			{
	        CFbsBitmap* bitmap = NULL;
	        CFbsBitmap* mask = NULL;
	        
	        iCurrentItemConverter->TransferBitmapOwnership( bitmap, mask );
	        if ( bitmap && mask )
		        {
          	    CleanupStack::PushL( bitmap );
            	CleanupStack::PushL( mask );
		      	CGulIcon* icon = CGulIcon::NewLC();
                
                icon->SetBitmap( bitmap );
                icon->SetMask( mask );
                icon->SetBitmapsOwnedExternally( ETrue );
                
                iStationsListBox->SetAdvertisementIconL( icon );
                
                CleanupStack::Pop( icon );
                icon->SetBitmapsOwnedExternally( EFalse );
                CleanupStack::Pop( mask );
                CleanupStack::Pop( bitmap );
		        }
			iRequestForAdvertisement = EFalse;
			iLogoDownloadError	= EFalse;
			}
		else
			{
			if ( iInvalidIconIndices.Count() )
				{
				iIconIndex = iInvalidIconIndices[0];		
				iLogosArray.InsertInOrderL(iInvalidIconIndices[0]);
				if(iNoLogosArray.Count())
					{
					iNoLogosArray.Remove(iNoLogosArray.FindInOrder(iIconIndex));
					}
				CArrayPtr<CGulIcon>* iconArray = iStationsListBox->ItemDrawer()
						->FormattedCellData()->IconArray();
				if(aError == KErrNone)
					{
					if(aId == KErrUnknown)
						{
						if ( iInvalidIconIndices.Count() )
							{
							iInvalidIconIndices.Remove( 0 );
							}					
						}

					CFbsBitmap* bitmap = NULL;
					CFbsBitmap* mask = NULL;
					aId == KErrUnknown ? iConverter->TransferBitmapOwnership( bitmap, mask ) : 
					iCurrentItemConverter->TransferBitmapOwnership( bitmap, mask );
					if ( bitmap && mask )
						{
			      	    CleanupStack::PushL( bitmap );
			        	CleanupStack::PushL( mask );
						CGulIcon* icon = CGulIcon::NewLC();
						icon->SetBitmap( bitmap );
						icon->SetMask( mask );
						icon->SetBitmapsOwnedExternally( EFalse );

						iconArray->InsertL( iIconIndex, icon );
						CleanupStack::Pop( icon );
						if(iIconIndex+1 < iconArray->Count())
							{
							icon = iconArray->At( iIconIndex+1 );
							iconArray->Delete( iIconIndex+1 );
							delete icon;
							}
						}
	                CleanupStack::Pop( mask );
	                CleanupStack::Pop( bitmap );
					iStationsListBox->DrawNow();
					}
				}
			iItemArray->Reset();
			if(iAdvString )
				{
				if(!iLogoDownloadError)
				iFilteredModel->SetItemAtZero(ETrue,*iAdvString);
				}

			UpdateStationsViewL();
			if ( iInvalidIconIndices.Count() && aId == KErrUnknown )
				{
				StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
				}
			}

		}
	else if ( !aError && aEvent == MIRImageConverterObserver::EIRFrameConversionCompleted )
		{
		iRequestForAdvertisement = EFalse;
		iLogoDownloadError	= EFalse;
		CGulIcon* icon = iStationsListBox->AdvertisementIcon(); 
		if ( !icon )
			{
			icon = CGulIcon::NewLC();
			iStationsListBox->SetAdvertisementIconL( icon );
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
	if(iAdPresent)
		{
		if ( iInvalidIconIndices.Count() )
			{
			iRequestForAdvertisement = EFalse;
			iView.SetCurrentFocus(1);	
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}

		}
	iAdPresent = EFalse;
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleImageConversionEventL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRStationsViewContainer::HandleLogoErrorL()
// called from the iStationsView when the logo is not downloaded
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::HandleLogoErrorL()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleLogoErrorL - Entering" );
	if(iAdRequestStatusFlag)
		{
		iLogoDownloadError	= ETrue;
		iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
		iAdRequestStatusFlag = EFalse;
		iAdPresent = EFalse;
		if ( iInvalidIconIndices.Count() )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}
		}
	else
		{
		if ( iInvalidIconIndices.Count()  )
			{
			iInvalidIconIndices.Remove( 0 );
			}	
		if ( iInvalidIconIndices.Count() )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}
		}	
	IRLOG_DEBUG( "CIRStationsViewContainer::HandleLogoErrorL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRStationsViewContainer::StartConvertL( CIRImageConverter& aConverter, TInt aIndex, TInt aId )
// used to convert the images depending the index passed
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::StartConvertL( CIRImageConverter& aConverter, TInt aIndex, TInt /*aId*/ )
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::StartConvertL - Entering" );
	TInt channelsCount = iView.iIsdsWrapper->iStationsData->Count();
	iResult = 0;
	RBuf url;
	url.Create(iView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(aIndex).Size());
	url.Copy(iView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(aIndex));

	iView.iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
			GetLogoDownloadEngine()->GetCacheLogoL(url,iResult);

	if((iResult == 0) || (iResult == -1))//not available in the cache or cache is stale/not useable
		{
		//code to download the logo
		if(iTempPreset)
			{
			delete iTempPreset;
			iTempPreset = NULL;
			}
		iTempPreset=CIRIsdsPreset::NewL();
		iTempPreset->SetChannelType(1);
		iTempPreset->SetImgUrl(url);		
		iView.LogoRequestL();
		}
	else if(iResult == 1)//cache is useable
		{
		iError=KErrNone;
		aConverter.SetDataL(iView.iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->
					GetHttpDataProvider()->GetLogoDownloadEngine()->SendCacheLogo());
					
		if(iError==KErrNone)
			{
			iConverter->StartL( KBitmapSize, KErrUnknown );	
			}
			else
			{
		if( iError!=KErrNone)
			{
			if ( iInvalidIconIndices.Count() )
				{
				iInvalidIconIndices.Remove( 0 );
				}	
			if ( iInvalidIconIndices.Count() )
				{
				StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
				}
			}
			}
		}
	url.Close();
	IRLOG_DEBUG( "CIRStationsViewContainer::StartConvertL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRStationsViewContainer::PresetLogoDownloadL()
// 
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::PresetLogoDownloadL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::PresetLogoDownloadL - Entering" );
	if(iAdPresent && iAdRequestStatusFlag)
		{
		if (iView.iUi->iIsdsWrapper->GetBannerUrl())
			{
			*iAdvPreset = *aPreset;	
			if(iAdvPreset->GetLogoData()!=KNullDesC8 )
				{
				iFilteredModel->SetAdvFlag(ETrue,*iAdvString);
				iError=KErrNone;
				iCurrentItemConverter->SetDataL(aPreset->GetLogoData());
				if(iError==KErrNone)
					{
					iCurrentItemConverter->StartL( IRLayoutUtils::AdvertisementSizeL(), KErrUnknown );//KLogoSize
					}
				iRequestForAdvertisement = ETrue;	
				iAdRequestStatusFlag = EFalse;
				}
			}
		return ;
		}

	if(aPreset->GetLogoData()!=KNullDesC8)
		{
		iRequestForAdvertisement = EFalse;
		iError=KErrNone;	
		iConverter->SetDataL(aPreset->GetLogoData());
		if(iError==KErrNone)
			{
			iConverter->StartL( KBitmapSize, KErrUnknown );	
			}
		if(iError!=KErrNone)
			{
			if ( iInvalidIconIndices.Count() )
				{
				iInvalidIconIndices.Remove( 0 );
				}	
			if ( iInvalidIconIndices.Count() )
				{
				StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
				}
			}
		}

	IRLOG_DEBUG( "CIRStationsViewContainer::PresetLogoDownloadL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationsViewContainer::OpenUrlL( const TDesC& aHttpLink)
// used to connect to the advertisement url.
// ---------------------------------------------------------------------------
//
void CIRStationsViewContainer::OpenUrlL( const TDesC& aHttpLink )
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::OpenUrlL - Entering" );

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

	IRLOG_DEBUG( "CIRStationsViewContainer::OpenUrlL - Exiting" );

	}
//----------------------------------------------------------------------------
// CIRStationsViewContainer::GetLogoNotSupported() 
// returns whether the advertisement is supported by the sdk r not
//----------------------------------------------------------------------------
// 	
TBool CIRStationsViewContainer::GetLogoNotSupported()
	{
	IRLOG_DEBUG( "CIRStationsViewContainer::GetLogoNotSupported - Entering" );
	IRLOG_DEBUG( "CIRStationsViewContainer::GetLogoNotSupported - Exiting" );
	return iLogoDownloadError;
	}
