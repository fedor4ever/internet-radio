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
* Description:  Container class for search results view
*
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
#include <internetradio.mbg>
#include <eikenv.h>
#include <akntoolbar.h>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irsearchresultsview.h"
#include "irsearchresultsviewcontainer.h"
#include "irapplication.h"

#include "iraap.hlp.hrh"
#include "irisdswrapper.h"
#include "irimageconverter.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irlogodownloadengine.h"
#include "irfilteredmodel.h"
#include "mirlistboxarray.h"
#include "iradvertisinglistbox.h"
#include "irlayoututils.h"


#include <aknsbasicbackgroundcontrolcontext.h> 

#include <apgcli.h>

/** Mime type when opening web browser */
_LIT8( KIRHtmlMimeType, "text/html" );
/** Browser prefix: See browser API specification */
_LIT( KBrowserPrefix, "4 " );

_LIT(KFalse,"false");



const TInt KSize = 10;
const TInt KAdvStringSize = 30;
const TInt KIconSize = 59;

#define KCONTROLSCOUNT 0
// 
const TInt KIRNoLogo = -1;
// Length of list item with known sized elements: (the number of column separators) + 3 (maximum length for icon id)
const TInt KIRListItemBaseLength = 5;
const TInt KRange = 999;
//-----------------------------------------------------------------------------
//  CIRSearchResultsViewContainer::NewL
//  Creates a new instance of CIRSearchResultsViewContainer
//-----------------------------------------------------------------------------
//
CIRSearchResultsViewContainer* CIRSearchResultsViewContainer::NewL(const TRect& aRect ,
						CIRSearchResultsView& aView)
    {
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::NewL - Entering" );
	CIRSearchResultsViewContainer* self = CIRSearchResultsViewContainer::NewLC(aRect,aView);
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::NewL - Exiting." );
    return self;
    }

//-----------------------------------------------------------------------------
//  CIRSearchResultsViewContainer::NewLC
//  Creates a new instance of CIRSearchResultsViewContainer and leaves it on the cleanup stack.
//-----------------------------------------------------------------------------
//
CIRSearchResultsViewContainer* CIRSearchResultsViewContainer::NewLC(const TRect& aRect ,
				CIRSearchResultsView& aView)
    {
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::NewLC - Entering" );
    CIRSearchResultsViewContainer* self = new (ELeave) CIRSearchResultsViewContainer(aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::NewLC - Exiting." );
    return self;
    }
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::CIRSearchResultsViewContainer()  
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRSearchResultsViewContainer::CIRSearchResultsViewContainer(
			CIRSearchResultsView &aSearchResultsView):iSearchResultsView(aSearchResultsView) 
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CIRSearchResultsViewContainer" );
	iFilteredFlag = EFalse;
	iAdRequestStatusFlag = EFalse;
	}
    
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::ConstructL( const TRect& aRect)
    {
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::ConstructL - Entering" );
    CreateWindowL();
    SetRect(aRect);
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
    												 KAknsIIDQsnBgScreen,
    												 aRect, 
    												 ETrue); 
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

	IRLOG_DEBUG( "CIRSearchResultsViewContainer::ConstructL - Exiting" ); 
    }

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::CreateListBoxL()
// Creates ListBox depending upon the presence of Advertisement
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::CreateListBoxL()
	{
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateListBoxL - Entering" );
	if (iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl())
		{
		if(iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl()->Length() != 0)
			{
			iAdPresent = ETrue;
			if(iAdvPreset)
				{
				delete iAdvPreset;
				iAdvPreset = NULL;
				}
			iAdvPreset=CIRIsdsPreset::NewL();
			iAdvPreset->SetChannelType(1);
			iAdvPreset->SetImgUrl(*iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl());
			iAdRequestStatusFlag = ETrue;
			CreateCustomListBoxL();
			iLogoDownloadError	= ETrue;
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			iSearchResultsView.AdRequestL(); //TOBEDONE:AdRequestL() in View
			}
		}  
	else
		{
		if(iSearchResultsView.iUi->iIsdsWrapper->iChannelBannerTag)
			{
			iSearchResultsView.SetCurrentFocus(0);	
			iLogoDownloadError	= ETrue;
			iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
			iAdRequestStatusFlag = EFalse;
			}
	    iSearchListBox = new (ELeave) CIRAdvertisingListBox;
		CreateCustomListBoxL();	
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateListBoxL - Exiting" ); 

	}

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::CreateCustomListBoxL()
// Creates the custom listbox and updates the list according to that
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::CreateCustomListBoxL()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateCustomListBoxL - Entering " );
	if(iAdPresent)
		{
		iSearchListBox = new (ELeave) CIRAdvertisingListBox;
		}

	iSearchListBox->SetContainerWindowL(*this);
	// Added For Dynamic Filtering
	iSearchListBox->ConstructL (this,EAknListBoxSelectionList,iFilteredModel);

	// Construct list.
	TResourceReader rr;
	iCoeEnv->CreateResourceReaderLC( rr, R_IR_SEARCHRESULTS_LIST );
	ConstructFromResourceL( rr );
	CleanupStack::PopAndDestroy(); // rr

	iItemArray = static_cast<CDesCArray*>( iSearchListBox->Model()->ItemTextArray());
	iListBoxArray->SetListArray(iItemArray);
	iFilteredModel->SetListArray(iListBoxArray); 

	if (iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl())
		{
		// Added For Dynamic Filtering
		if(iAdvString)
			{
			delete iAdvString;
			iAdvString = NULL;	
			}
		iAdvString = HBufC::NewL(KAdvStringSize);
		TPtr ptr(iAdvString->Des());
		_LIT(KDoubleTabSpace,"\t \t ");
		ptr.Copy(KDoubleTabSpace);

		iFilteredModel->SetAdvFlag(iAdPresent,*iAdvString);
		iSearchListBox->SetListItemFormat( iAdvString );
		}

	TInt resultsCount = iSearchResultsView.iIsdsWrapper->iStationsData->Count();
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

	iSearchListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iconArray );
	iSearchListBox->SetRect( Rect() );
	iSearchListBox->ActivateL();

	iSearchListBox->CreateScrollBarFrameL( ETrue );
	iSearchListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
	              CEikScrollBarFrame::EAuto);

	iSearchListBox->SetListBoxObserver( this );    

	// Set the icon array for this list. List takes ownership of the array. 
	for ( TInt i = 0; i < resultsCount; i++ )
		{
		RBuf tempImgUrlStatus;
		tempImgUrlStatus.Create(KSize);
		tempImgUrlStatus.Copy(KFalse);

		RBuf tempImgUrl;
		tempImgUrl.Create(iSearchResultsView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(i).Size());
		tempImgUrl.Copy(iSearchResultsView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(i));

		if(!(tempImgUrlStatus == tempImgUrl))
			{
			iInvalidIconIndices.InsertInOrderL(i);
			}
		iNoLogosArray.InsertInOrderL(i);
		tempImgUrlStatus.Close();
		tempImgUrl.Close();
		}

	UpdateSearchResultsViewL();
	iSearchListBox->ItemDrawer()->FormattedCellData()->SetMarqueeParams
									(KLoop, KScrollAmount, KScrollDelay, KInterval);
	iSearchListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
	if(resultsCount>1)
		{
		if(iSearchBox)
			{
			delete iSearchBox;
			iSearchBox = NULL;
			}

		// Added For Dynamic Filtering
		iSearchBox = CreateFindBoxL(iSearchListBox, iFilteredModel,
		CAknSearchField::ESearch);
		SizeChanged();
		}
	else
		{
		DisableFindBox();	
		}
	if( !iAdRequestStatusFlag)
		{
		if ( iInvalidIconIndices.Count() )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateCustomListBoxL - Exiting" ); 		
	}


// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::CreateDefaultIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CIRSearchResultsViewContainer::CreateDefaultIconL()
    {
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateDefaultIconL - Entering" ); 		
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( TSize(), EGray2 ) );
    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( mask );
    User::LeaveIfError( mask->Create( TSize(), EGray2 ) );
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2, bitmap );
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateDefaultIconL - Exiting" ); 		
    return icon;
    }
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::~CIRSearchResultsViewContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CIRSearchResultsViewContainer::~CIRSearchResultsViewContainer()
    {
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::~CIRSearchResultsViewContainer - Entering" );
    delete iConverter;
	delete iCurrentItemConverter;
	iInvalidIconIndices.Close();   
	iLogosArray.Close();
	iNoLogosArray.Close();  
    if(iSearchListBox)
    	{
    	delete iSearchListBox;
    	iSearchListBox = NULL;
    	}
     if(iSearchBox)
		{
		delete iSearchBox;
		iSearchBox = NULL;
		}
    if(iTempPreset)
		{
		delete iTempPreset;
		iTempPreset = NULL;
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

	iAdPresent = EFalse;
    if(iListBoxArray )
	    {
	    delete iListBoxArray ;
	    iListBoxArray = NULL;	
	    }
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::~CIRSearchResultsViewContainer - Exiting" );
    }

// ---------------------------------------------------------------------------
// TKeyResponse CIRSearchResultsViewContainer::OfferKeyEventL()
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRSearchResultsViewContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::OfferKeyEventL - Entering" );
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
			currentIndex = GetCurrentItemIndex();
			if (currentIndex == 0)  
				{
				if (iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl() && !iLogoDownloadError)
					{
					if(iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl()->Length() != 0)
						{
						iSearchResultsView.HandleCommandL(EAdvertisementUrl);
						}
					}
				else
					{
					iSearchResultsView.HandleCommandL(EListenCmd);	    
					}
				}
			else
				{
				iSearchResultsView.HandleCommandL(EListenCmd);	
				}
			break;
			}
		default:
			{
			if(iSearchListBox)
				{
				iSearchResultsView.SetToolbarItems();
				if ( iSearchBox )
					{
					iFilteredFlag = ETrue;
					keyResp = iSearchBox->OfferKeyEventL( aKeyEvent, aType );

					if( keyResp == EKeyWasNotConsumed )
						{
						keyResp = iSearchListBox->OfferKeyEventL( aKeyEvent, aType );
						}

					CAknFilteredTextListBoxModel* model = 
					static_cast<CAknFilteredTextListBoxModel*>( iSearchListBox->Model() );
					model->Filter()->HandleOfferkeyEventL();					
					TInt filterModelCount = iFilteredModel->NumberOfItems();	
					if(filterModelCount == 0 && 
					!(iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl()))
						{
						HBufC* noMatchesTextResource = 
						StringLoader::LoadLC(R_IRAPP_STATIONLIST_NOMATCHES);
						iSearchListBox->View()->SetListEmptyTextL(*noMatchesTextResource);
						CleanupStack::PopAndDestroy( noMatchesTextResource );
						}
					iSearchBox->GetSearchText(iFindString);
					if(iFindString.Length()!=0)
						{
						if(!iLogoDownloadError && 
						iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl() 
						&& filterModelCount > 1 )
							{
							iSearchListBox->SetCurrentItemIndex(1);
							}
						if(filterModelCount == 1 && 
						(iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl())&&
						!iLogoDownloadError)
							{
							iSearchListBox->SetCurrentItemIndex(0);
							iSearchResultsView.SetToolbarItems();
							}
						}
					}
				}
			if(	aKeyEvent.iScanCode==EStdKeyBackspace)
				{
				if(iSearchBox)
					{
					iSearchBox->GetSearchText(iFindString);
					if(iFindString.Length()==0)
						{
						iFilteredFlag = EFalse;
						}
					}
				}
			iSearchResultsView.SetToolbarItems();
			break;
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::OfferKeyEventL - Exiting" );   
	return keyResp;

	}


// ---------------------------------------------------------------------------
// From class MEikListBoxObserver.
// List box event handler.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleListBoxEventL - Entering" );
	TInt currentIndex = 0;

	switch ( aEventType )
		{
		case EEventEnterKeyPressed:
		/**  Keypress event. */
		case EEventItemDoubleClicked:
			{
			currentIndex = GetCurrentItemIndex();
			if (currentIndex == 0)  
				{
				if (iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl() && !iLogoDownloadError)
					{
					if(iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl()->Length() != 0)
						{
						iSearchResultsView.HandleCommandL(EAdvertisementUrl);
						}
					}
				else
					{
					iSearchResultsView.SetCurrentFocus(GetCurrentItemIndex());
					iSearchResultsView.HandleCommandL(EListenCmd);
					}
				}
			else
				{
				iSearchResultsView.SetCurrentFocus(GetCurrentItemIndex());
				iSearchResultsView.HandleCommandL(EListenCmd);
				}
			}

		break;
		case EEventItemClicked:
			{
			iSearchResultsView.SetCurrentFocus(GetCurrentItemIndex());
			iSearchResultsView.SetToolbarItems();
			}
		break;
		/* fall through */ // pc-lint Warning -e616  
		default:
		break;
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleListBoxEventL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::Draw()
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::Draw( const TRect& /*aRect*/ ) const
    {
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::Draw - Entering" );
    CWindowGc& gc = SystemGc();

    if(iSearchListBox)
	    {
	   	iSearchListBox->SetGC(gc);
	   	iSearchListBox->SetControlFlag(ETrue);
	    }
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    // draw background skin first. 
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	
	TRect clientRect(((CEikAppUi*)(CCoeEnv::Static()->AppUi()))->ClientRect());

	AknsDrawUtils::DrawBackground ( skin, iBgContext, this, gc, TPoint(0,0), 
			TRect(TPoint(0,0),clientRect.iBr), KAknsDrawParamDefault  );
	
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::Draw - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetHelpContext()
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::GetHelpContext( TCoeHelpContext& aContext) const
    {
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetHelpContext - Entering" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_RESULTS  );
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetHelpContext - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::UpdateSearchResultsViewL()
// updates the search results data to the search results list
// ---------------------------------------------------------------------------
//    

void CIRSearchResultsViewContainer::UpdateSearchResultsViewL()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::UpdateSearchResultsViewL - Entering" );
	
	//provides logo url in the search results stations
	TInt stationsCount = iSearchResultsView.iIsdsWrapper->iStationsData->Count();
	for(TInt i = 0; i < stationsCount; i++)
		{
		//check whether logo url exist for the current channel of the search results
		for(TInt m = 0; m < iNoLogosArray.Count(); m++)
			{
			if(i == iNoLogosArray[m])				
				{
				//logo should not be displayed in the search results for this particular station
				HBufC* itemString = FormatItemStringLC( iSearchResultsView.iIsdsWrapper->
														iStationsData->MdcaPoint(i),KIRNoLogo);
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
				HBufC* itemString = FormatItemStringLC( iSearchResultsView.iIsdsWrapper->
														iStationsData->MdcaPoint(i),i);
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
		iSearchListBox, iSearchBox,EFalse,needRefresh );
		TInt filterModelCount = iFilteredModel->NumberOfItems();	
		if(filterModelCount ==1 && iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl())
			{
			iSearchListBox->SetCurrentItemIndex(0);
			}
		else
			{
			if(filterModelCount>1 && iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl() 
			&& !iLogoDownloadError)
				{
				iSearchListBox->SetCurrentItemIndex(1);
				}
			}
		}
	else
		{
		iSearchListBox->HandleItemAdditionL();
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::UpdateSearchResultsViewL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::FormatItemStringLC
// Formatter for list item strings.
// ---------------------------------------------------------------------------
//
HBufC* CIRSearchResultsViewContainer::FormatItemStringLC( const TDesC& aFirstRow, 
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
// CIRSearchResultsViewContainer::GetCurrentItemIndex()
// returns the index of the currently selected item
// ---------------------------------------------------------------------------
//

TInt CIRSearchResultsViewContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetCurrentItemIndex" );
	return iSearchListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetListBoxTopIndex()
// returns the index of the top item
// ---------------------------------------------------------------------------
//

TInt CIRSearchResultsViewContainer::GetListBoxTopIndex() const
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetListBoxTopIndex" );
	return iSearchListBox->View()->TopItemIndex () ;
	}



//----------------------------------------------------------------------------
// CIRSearchResultsViewContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRSearchResultsViewContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CountComponentControls - Entering" );
	TInt count(KCONTROLSCOUNT);
	if(iSearchListBox)
		{
		count++;
		}
	if(iSearchBox)
		{
		count++;
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CountComponentControls - Exiting" );
	return count;
	}
//----------------------------------------------------------------------------
// CIRSearchResultsViewContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRSearchResultsViewContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::ComponentControl - Exiting" );
	switch(aIndex)
		{
		case 0:
			{
			if(iSearchListBox)
			return iSearchListBox;
			}
		case 1:
			{
			if(iSearchBox)
			return iSearchBox;	
			}
		default:
			return NULL;
		}
	}
// ---------------------------------------------------------------------------
// void CIRSearchResultsViewContainer::SizeChanged(const TRect& aRect)
// Responds to changes to the size and position of the contents of this control.
// ---------------------------------------------------------------------------
//

void CIRSearchResultsViewContainer::SizeChanged()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::SizeChanged - Entering" );
	if(iSearchResultsView.iUi->IsLandscapeOrientation())
		{
		if(iSearchListBox && iSearchBox && iLandScapeCount ==0)
			{
	   		AknFind::HandlePopupFindSizeChanged(this,iSearchListBox,iSearchBox);
	   		iLandScapeCount ++;
			}
		}
	if(iSearchListBox && iSearchBox)
		{
	    AknLayoutUtils::LayoutControl(iSearchListBox,  Rect(),
            AKN_LAYOUT_WINDOW_list_gen_pane(1));
        AknLayoutUtils::LayoutControl(iSearchBox,  Rect(),
            AKN_LAYOUT_WINDOW_find_pane);
		}
		
	else
		{
		if(iSearchListBox)
			{
			AknLayoutUtils::LayoutControl(iSearchListBox, Rect(),
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
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::SizeChanged - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRSearchResultsViewContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::HandleResourceChange(TInt aType)
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
    // application layout change request notification
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // reconstruct controls if needed
        if(iSearchListBox && iSearchBox)
	        {
	   		AknFind::HandlePopupFindSizeChanged(this,iSearchListBox,iSearchBox);
	        SizeChanged();
	        }
        }
	iSearchResultsView.SetToolbarItems();
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleResourceChange - Exiting" );
	}



// ---------------------------------------------------------------------------
// void CIRSearchResultsViewContainer::HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent aEvent, 
// from base class MIRImageConverterObserver
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::HandleImageConversionEventL( 
					MIRImageConverterObserver::TIRImageConversionEvent aEvent, 
                                                           TInt aId, 
                                                           TInt aError )
	{
	iError=aError;
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleImageConversionEventL - Entering" );
	if(aError == KErrNotSupported && iRequestForAdvertisement)
		{
		iLogoDownloadError	= ETrue;
		iFilteredModel->SetAdvFlag(EFalse,*iAdvString);
		}
	if ( !aError && aEvent == MIRImageConverterObserver::EIRImageConversionCompleted )
		{
		IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleImageConversionEventL - Entering if" );
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
	            
	            iSearchListBox->SetAdvertisementIconL( icon );
	            
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
				CArrayPtr<CGulIcon>* tempIconArray = iSearchListBox->ItemDrawer()->
				FormattedCellData()->IconArray();
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

						tempIconArray->InsertL( iIconIndex, icon );
						CleanupStack::Pop( icon );

						if(( iIconIndex+1 )<tempIconArray->Count())
							{
							icon = tempIconArray->At( iIconIndex+1 );
							tempIconArray->Delete( iIconIndex+1 );
							delete icon;
							}
			            CleanupStack::Pop( mask );
			            CleanupStack::Pop( bitmap );
						}
					iSearchListBox->DrawNow();
					}
				}
			iItemArray->Reset();
			if(iAdvString)
			{
			if(!iLogoDownloadError)
			iFilteredModel->SetItemAtZero(ETrue,*iAdvString);
			}
			UpdateSearchResultsViewL();
			if ( iInvalidIconIndices.Count() && aId == KErrUnknown )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}	
			}
		}
	else if ( !aError && aEvent == MIRImageConverterObserver::EIRFrameConversionCompleted )
		{
		IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleImageConversionEventL - Entered elseif" );
		iLogoDownloadError	= EFalse;
		iRequestForAdvertisement = EFalse;
		CGulIcon* icon = iSearchListBox->AdvertisementIcon(); 
		if ( !icon )
			{
			icon = CGulIcon::NewLC();
			iSearchListBox->SetAdvertisementIconL( icon );
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
				iSearchResultsView.SetCurrentFocus(1);	
				StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
				}
		}
	iAdPresent = EFalse;
	if(iSearchResultsView.iUi->ActiveView() == KIRSearchResultsViewID)
		{
		iSearchResultsView.SetToolbarItems();
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::HandleImageConversionEventL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::PresetLogoDownloadL()
// 
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::PresetLogoDownloadL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::PresetLogoDownloadL - Entering" );
	if (iAdPresent && iAdRequestStatusFlag)
		{
		if (iSearchResultsView.iUi->iIsdsWrapper->GetBannerUrl()->Length() != 0)
			{
			*iAdvPreset = *aPreset;
			if(iAdvPreset->GetLogoData()!=KNullDesC8 )
				{
				iFilteredModel->SetAdvFlag(iAdPresent,*iAdvString);
				iCurrentItemConverter->SetDataL(aPreset->GetLogoData());
				iCurrentItemConverter->StartL( IRLayoutUtils::AdvertisementSizeL(), KErrUnknown );
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
			iConverter->StartL( TSize(KIconSize, KIconSize), KErrUnknown );
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
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::PresetLogoDownloadL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRSearchResultsViewContainer::HandleLogoErrorL()
// called from the iSearchResultsView when the logo is not downloaded
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::HandleLogoErrorL()
	{
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


// ---------------------------------------------------------------------------
// void CIRSearchResultsViewContainer::StartConvertL( CIRImageConverter& aConverter, TInt aIndex, TInt aId )
// used to convert the images depending the index passed
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::StartConvertL( CIRImageConverter& aConverter,
				 TInt /*aIndex*/, TInt /*aId*/ )
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::StartConvertL - Entering" );
	iResult = 0;
	RBuf url;
	url.Create(iSearchResultsView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(
					iInvalidIconIndices[0]).Size());
	url.Copy(iSearchResultsView.iIsdsWrapper->iChannelImgUrl->MdcaPoint(iInvalidIconIndices[0]));

	iSearchResultsView.iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->
				GetHttpDataProvider()->GetLogoDownloadEngine()->GetCacheLogoL(url,iResult);
	
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
		iSearchResultsView.LogoRequestL();
		}
	else if(iResult == 1)//cache is useable
		{
		iError=KErrNone;
		aConverter.SetDataL(iSearchResultsView.iUi->iIsdsWrapper->GetISDSInstance()->
		GetDataProvider()->GetHttpDataProvider()->GetLogoDownloadEngine()->SendCacheLogo());
		if(iError==KErrNone)
			{
			aConverter.StartL( TSize(KIconSize, KIconSize), KErrUnknown );
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
	url.Close();
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::StartConvertL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::OpenUrlL( const TDesC& aHttpLink)
// used to connect to the advertisement url.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsViewContainer::OpenUrlL( const TDesC& aHttpLink )
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::OpenUrlL - Entering" );

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

	IRLOG_DEBUG( "CIRSearchResultsViewContainer::OpenUrlL - Exiting" );

	}
//----------------------------------------------------------------------------
// CIRSearchResultsViewContainer::CreateFindBoxL()
// creates the Find box when any character is Pressed
//----------------------------------------------------------------------------
// 
CAknSearchField* CIRSearchResultsViewContainer::CreateFindBoxL(CEikListBox* aListBox,
CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle)
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateFindBoxL - Entering" );
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
		//Filter can get by model->Filter()
		CleanupStack::Pop(findbox); // findbox
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::CreateFindBoxL - Exiting" );
	return findbox;
	}
//----------------------------------------------------------------------------
// CIRSearchResultsViewContainer::DisableFindBox()
// Deletes the findbox
//----------------------------------------------------------------------------
// 
void CIRSearchResultsViewContainer::DisableFindBox()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::DisableFindBox - Entering" );
	if (iSearchBox)
		{
		iFilteredModel->RemoveFilter();
		// Important to set the find box non-focusing before
		// deleting it, otherwise the focus changes triggered
		// by the removal of the control from stack will focus
		// the find box which is under deletion
		iSearchBox->SetNonFocusing();
		delete iSearchBox;
		iSearchBox = NULL;
		// Inform list box that find is hidden
		static_cast<CAknColumnListBoxView*>(iSearchListBox->View())->SetFindEmptyListState(EFalse);
		SizeChanged();
		}
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::DisableFindBox - Exiting" );
	}

// ---------------------------------------------------------------------------
// Function : Comparestrings
// Function called to compare the filtered string inthe Itemarray
// ---------------------------------------------------------------------------
//
void  CIRSearchResultsViewContainer::Comparestrings()
		{
		IRLOG_DEBUG( "CIRSearchResultsViewContainer::Comparestrings - Entering" );
		TInt count = iFilteredModel->NumberOfItems();
		if(count)
			{
			if((count==1) && iAdvPreset && !iLogoDownloadError)
				{
					return;
				}
			else
				{
				TPtrC16 filteredString = iFilteredModel->ItemText (iSearchResultsView.GetFilteredIndex());
				for(TInt i=0;i<iItemArray->Count();i++)
					{
					TPtrC16 matchString=iItemArray->MdcaPoint(i);
					if(filteredString==matchString)
						{
						iSearchResultsView.SetActualFilteredItemIndex(i);
						break;
						}

					}
					
				}
			}
		IRLOG_DEBUG( "CIRSearchResultsViewContainer::Comparestrings - Exiting" );
		}

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetListPointer()
// returns pointer to Listbox.
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBox* CIRSearchResultsViewContainer::GetListPointer()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetListPointer" );
	return iSearchListBox;	
	}
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetSearchBoxPointer()
// returns the search box pointer.
// ---------------------------------------------------------------------------
//
CAknSearchField* CIRSearchResultsViewContainer::GetSearchBoxPointer()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetSearchBoxPointer" );
	return 	iSearchBox;
	}

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetFilterdModelPointer()
// returns pointer to filtered model.
// ---------------------------------------------------------------------------
//
CIRFilteredModel* CIRSearchResultsViewContainer::GetFilterdModelPointer()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetListPointer" );
	return iFilteredModel;	
	}

// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetFindString()
// returns the string in the findbox
// ---------------------------------------------------------------------------
//
TDesC CIRSearchResultsViewContainer::GetFindString()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetListBoxCount " );
	return iFindString;	
	}
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer:: GetAdvPreset()
// returns the advertisement preset 
// ---------------------------------------------------------------------------
 CIRIsdsPreset* CIRSearchResultsViewContainer::GetAdvPreset()
	 {
	 IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetAdvPreset " );
	 return iAdvPreset;
	 }
 
// ---------------------------------------------------------------------------
// CIRSearchResultsViewContainer:: GetTempPreset()
// returns the temporary preset 
// ---------------------------------------------------------------------------
 CIRIsdsPreset* CIRSearchResultsViewContainer::GetTempPreset()
	 {
	 IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetTempPreset " );
	 return iTempPreset;
	 }
//----------------------------------------------------------------------------
// CIRSearchResultsViewContainer::GetLogoNotSupported() 
// returns whether the advertisement is supported by the sdk r not
//----------------------------------------------------------------------------
// 	
TBool CIRSearchResultsViewContainer::GetLogoNotSupported()
	{
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetLogoNotSupported - Entering" );
	IRLOG_DEBUG( "CIRSearchResultsViewContainer::GetLogoNotSupported - Exiting" );
	return iLogoDownloadError;
	}
