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
* Description:  
*
*/

#include <akniconarray.h>
#include <barsread.h>
#include <stringloader.h>
#include <internetradio.rsg>
#include <internetradio.mbg>
#include <aknsbasicbackgroundcontrolcontext.h> 
#include <avkon.mbg>
#include <aknsfld.h> 
#include <eikprogi.h> 


#include "irui.h"
#include "ir.hrh"
#include "irdebug.h"
#include "irmainview.h"
#include "irmaincontainer.h"
#include "iraap.hlp.hrh"
#include "irisdswrapper.h"
#include "irimageconverter.h"
#include "irfilteredmodel.h"
#include "irfavoritesdb.h"
#include "irisdspreset.h"
#include "irpreset.h"
#include "irhistory.h"
#include "irdialoglauncher.h"
#include "mirlistboxarray.h"
#include "iradvertisinglistbox.h"
#include "irnowplayingwrapper.h"
#include "irnetworkcontroller.h"


//Literals Declaration

// Format string for two row list item with logo, %S containing:
// 1. Icon ID for logo image
// 2. First row text
// 3. Second row text
_LIT( KIRFormatStringWithIcon, "%d\t%S\t%S" );
_LIT( KIRFormatStringWithoutIcon, "\t%S\t%S" );
_LIT(KAvkonBitmapFile,"z:\\resource\\apps\\avkon2.mbm");

//Constants Declaration
const TInt KProgressbarFinalValue = 100;
const TInt KProgressTimer = 500000;
const TInt KExtraLength = 30;
const TInt KRange = 999;
const TInt KIRNoLogo = -1;
// Length of list item with known sized elements: (the number of column separators) + 3 (maximum length for icon id)
const TInt KIRListItemBaseLength = 5;


#define KCONTROLSCOUNT 0


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CIRMainContainer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRMainContainer* CIRMainContainer::NewL( CIRMainView& aView, const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRMainContainer::NewL" );
    CIRMainContainer* self = CIRMainContainer::NewLC( aView, aRect );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRMainContainer::NewL - Exiting" );
    return self;
    }

// -----------------------------------------------------------------------------
// CIRMainContainer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRMainContainer* CIRMainContainer::NewLC( CIRMainView& aView, const TRect& aRect )
    {
 	IRLOG_DEBUG( "CIRMainContainer::NewLC" );
    CIRMainContainer* self = new ( ELeave ) CIRMainContainer(aView);
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    IRLOG_DEBUG( "CIRMainContainer::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRMainContainer::CIRMainContainer()  
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRMainContainer::CIRMainContainer(CIRMainView& aView) :iMainView(aView)
	{
	IRLOG_DEBUG( "CIRMainContainer::CIRMainContainer" );
	iLastPlayedChannel = EFalse;
	iProgressCount = 1;
	iDeletingCount = 1;
	iFilteredFlag = EFalse;
	iContextSensitiveFlag = EFalse;
	iMoveFocusLast = EFalse;
	IRLOG_DEBUG( "CIRMainContainer::CIRMainContainer - Exiting" );
	}
    
// ---------------------------------------------------------------------------
// CIRMainContainer::ConstructL
// ---------------------------------------------------------------------------
//
void CIRMainContainer::ConstructL( const TRect& aRect)
    {
	IRLOG_DEBUG( "CIRMainContainer::ConstructL - Entering" );
    CreateWindowL();
    SetRect(aRect);
    iImageCount = 1;
    iFinalCount = 0;
    iLogoDownloadError = EFalse;
    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen , aRect, ETrue); 
 	
   
    iConverter = CIRImageConverter::NewL();
	iConverter->EnableAnimations( EFalse );
	iConverter->SetObserver( this );

	iCurrentItemConverter = CIRImageConverter::NewL();
	iCurrentItemConverter->SetObserver( this );
    
    // Added For Dynamic Filtering
    iFilteredModel=new(ELeave) CIRFilteredModel;
    iFilteredModel->ConstructL();
   	iListBoxArray = MIRListBoxArray::NewL();
    iFavouritesListBox = new (ELeave) CIRAdvertisingListBox;

	IRLOG_DEBUG( "CIRMainContainer::ConstructL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::CreateCustomListBoxL()
// Creates the listbox
// ---------------------------------------------------------------------------
//
void CIRMainContainer::CreateCustomListBoxL()
	{
	IRLOG_DEBUG( "CIRMainContainer::CreateCustomListBoxL...Entering" );
	TRect rect;
	//For displaying Ads
	iFavouritesListBox->SetContainerWindowL(*this);
	// Added For Dynamic Filtering
	iFavouritesListBox->ConstructL (this,EAknListBoxMarkableList,iFilteredModel);
	
	 
    // Construct list.
    TResourceReader rr;
    iCoeEnv->CreateResourceReaderLC( rr, R_IR_CHANNEL_LIST );
    ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy(); // rr
    
    iListArray = static_cast<CDesCArray*>( iFavouritesListBox->Model()->ItemTextArray());
    iListBoxArray->SetListArray(iListArray);
    
    iFilteredModel->SetListArray(iListBoxArray); 
    
    CAknIconArray* iconArray = new ( ELeave ) CAknIconArray(iMainView.iUi->
    														iFavPresets->MaxPresetCount());
	CleanupStack::PushL( iconArray );
	
   // Creating mark icon and appending to iconarray.
	CFbsBitmap* markBitmap = NULL;
    CFbsBitmap* markBitmapMask = NULL;
    
    //CListItemDrawer is using this logical color as default for its marked icons
    TRgb defaultColor;
    defaultColor = iEikonEnv->Color(EColorControlText);
    

    AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(),
    			KAknsIIDQgnIndiMarkedAdd,
    			KAknsIIDQsnIconColors,
    			EAknsCIQsnIconColorsCG13,
    			markBitmap,
    			markBitmapMask,
    			KAvkonBitmapFile,
    			EMbmAvkonQgn_indi_marked_add,
    			EMbmAvkonQgn_indi_marked_add_mask,
    			defaultColor
    			);
    
    CGulIcon* markIcon = CGulIcon::NewL( markBitmap,markBitmapMask );
    // poping markBitmap, markBitmapMask
    // codescanner fix
    CleanupStack::Pop( markBitmapMask );
    CleanupStack::Pop( markBitmap ); 
    CleanupStack::PushL( markIcon );      
    iconArray->AppendL( markIcon );

    CleanupStack::Pop( markIcon );	
    
	for ( TInt i = 0; i <  iMainView.iUi->iFavPresets->MaxPresetCount() ; i++ )
		{
		CGulIcon* icon = CreateDefaultIconL();
        CleanupStack::PushL( icon );
        iconArray->AppendL( icon );
        CleanupStack::Pop( icon );
		}

	CleanupStack::Pop( iconArray  );
    iFavouritesListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iconArray );  
    iFavouritesListBox->SetRect( Rect());
    iFavouritesListBox->ActivateL();

    iFavouritesListBox->CreateScrollBarFrameL( ETrue );
    iFavouritesListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, 
                                              CEikScrollBarFrame::EAuto);
 	iFavouritesListBox->SetListBoxObserver( this );     
 	
	// Set the icon array for this list. List takes ownership of the array. 
	// Also move icon is added.
 	SetIconsL();
 	UpdateAllL();
	iFavouritesListBox->ItemDrawer()->FormattedCellData()->SetMarqueeParams
									(KLoop, KScrollAmount, KScrollDelay, KInterval);
    iFavouritesListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
	TInt favCount = iMainView.iUi->iFavPresets->iFavPresetList.Count();
	if(favCount>1)
		{
		if(iFavouritesSearchBox)
			{
			DisableFindBox();
			}
		// Added For Dynamic Filtering
		iFavouritesSearchBox = CreateFindBoxL(iFavouritesListBox, iFilteredModel,
		  CAknSearchField::ESearch);
		SizeChanged();	
		}
	else
		{
		DisableFindBox();	
		}
 	
	IRLOG_DEBUG( "CIRMainContainer::CreateCustomListBoxL - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRMainContainer::CreateDefaultIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CIRMainContainer::CreateDefaultIconL()
    {
	IRLOG_DEBUG( "CIRMainContainer::CreateDefaultIconL - Entering" );
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( TSize(), EGray2 ) );
    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( mask );
    User::LeaveIfError( mask->Create( TSize(), EGray2 ) );
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2, bitmap );
	IRLOG_DEBUG( "CIRMainContainer::CreateDefaultIconL - Exiting" );
    return icon;
    }
// ---------------------------------------------------------------------------
// CIRMainContainer::SetIconsL()
// Used to set icons to the list
// ---------------------------------------------------------------------------
//
void CIRMainContainer::SetIconsL()
	{
	// Set the icon array for this list. List takes ownership of the array. 
	// Also move icon is added.
	
		iMainView.iUi->iFavPresets->GetAllPreset();
		iMainView.SetMovingFlag(EFalse);
		

	const RIRPresetArray& presets = iMainView.iUi->iFavPresets->Presets();
	for ( TInt i = 0; i < presets.Count(); i++ )
		{
		if ( presets[i]->GetLogoData().Length() )
			{
			iInvalidIconIndices.InsertInOrderL( presets[i]->Index() );
			}
		iNoLogosArray.InsertInOrderL(presets[i]->Index());		
		}
    // Checking whether the lastplayed is present or not.
	if(iMainView.iUi->iLastPlayed->FileExists() )
		{
		// Checking whether the lastplayed is having logo data.
		//If present sends the data for conversation.
		if(iMainView.iUi->iLastPlayed->iLastPlayedStation->GetLogoData().Length())
	        {
	        iImageLast = ETrue;
	        TInt index = iMainView.iUi->iLastPlayed->iLastPlayedStation->GetIndex();
			StartConvertL( *iConverter, index, KErrUnknown );
	    	}
		// If the lastplayed is not having logo data and having the image url
		// then send a request for downloading the logo
	    else if(iMainView.iUi->iLastPlayed->iLastPlayedStation->GetImgUrl().Length())
		    {
			if((iMainView.iUi->iNetworkController->GetNetworkStatus() ))
			    {
			    SendRequestForLastPlayedLogoL();
		        }
		     else if ( iInvalidIconIndices.Count() )
				{
				StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
				}
		    }
 	
	    // If both logodata and image url is not present for lastplayed then logo
	    // request for other stations has to be made.
	    else
		    {
			if ( iInvalidIconIndices.Count() )
				{
				StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
				}
		    }
		}
	else
		{
		if ( iInvalidIconIndices.Count() )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}
		}
	}
// ---------------------------------------------------------------------------
// void CIRMainContainer::SendRequestForLastPlayedLogoL()
// 
// ---------------------------------------------------------------------------
//
void CIRMainContainer::SendRequestForLastPlayedLogoL()
	{
	IRLOG_DEBUG( "CIRMainContainer::SendRequestForLastPlayedLogoL - Entering" );
	//code to download the logo
	if(iTempPreset)
		{
		delete iTempPreset;
		iTempPreset = NULL;
		}
	iTempPreset=CIRIsdsPreset::NewL();
	iTempPreset->SetChannelType(1);
	iTempPreset->SetImgUrl(iMainView.iUi->iLastPlayed->iLastPlayedStation->GetImgUrl());
	iMainView.LogoRequestL();
	IRLOG_DEBUG( "CIRMainContainer::SendRequestForLastPlayedLogoL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRMainContainer:: GetTempPreset()
// returns the temporary preset 
// ---------------------------------------------------------------------------
 CIRIsdsPreset* CIRMainContainer::GetTempPreset()
	 {
	 IRLOG_DEBUG( "CIRMainContainer::GetTempPreset " );
	 return iTempPreset;
	 }
// ---------------------------------------------------------------------------
// CIRMainContainer::~CIRMainContainer()
// Destructor
// ---------------------------------------------------------------------------
//
CIRMainContainer::~CIRMainContainer()
    {
	IRLOG_DEBUG( "CIRMainContainer::~CIRMainContainer - Entering" );
	delete iConverter;
	delete iCurrentItemConverter;
	iInvalidIconIndices.Close();
	iLogosArray.Close();
	iNoLogosArray.Close();  
  	iMultiDeleteString.Close();
  	iFilterIndexExist.Close();
    if(iFavouritesListBox)
    	{
    	delete iFavouritesListBox;
    	iFavouritesListBox = NULL;
    	}
    if(iFavouritesSearchBox)
		{
		delete iFavouritesSearchBox;
		iFavouritesSearchBox = NULL;
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
    if(iTempPreset)
		{
		delete iTempPreset;
		iTempPreset = NULL;
		}
	if(iItem)
		{
		delete iItem;
		iItem = NULL;	
		}
	if(iLastplayed)
		{
		delete iLastplayed;
		iLastplayed = NULL;	
	 	}
    if(iProgressDialog)
    	{
    	delete iProgressDialog;
    	iProgressDialog= NULL;
    	}
    if(iProgressInfo)
    	{
    	delete iProgressInfo;
    	iProgressInfo= NULL;
    	}
    if(iListBoxArray )
	    {
	    delete iListBoxArray ;
	    iListBoxArray = NULL;	
	    }
	iImageCount = 1;
	IRLOG_DEBUG( "CIRMainContainer::~CIRMainContainer - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRMainContainer::LastPlayedDisplayL()()
// Displays the lastplayed station
// ---------------------------------------------------------------------------
//
void CIRMainContainer::LastPlayedDisplayL()
	{
	IRLOG_DEBUG( "CIRMainContainer::LastPlayedDisplayL - Entering" );
	if(iLastplayed)
		{
		delete iLastplayed;
		iLastplayed = NULL;	
		}
	if(iMainView.iNowPlayingWrapper->iPlaying)
			{
		 	iLastplayed = StringLoader::LoadL(R_IRAPP_MAIN_MENU_NOW);
			}
		else
			{
			iLastplayed = StringLoader::LoadL(R_IRAPP_MAIN_MENU_LAST);
			}

	if(iMainView.iUi->iLastPlayed->FileExists())
		{
		iLastPlayedChannel = ETrue;
		// Added For Dynamic Filtering
		if(iMainView.iUi->iLastPlayed->iLastPlayedStation->GetLogoData().Length() && !iImageLast 
			&& !iLogoDownloadError)
			{
			if(iItem)
				{
				delete iItem;
				iItem = NULL;

				}
			iItem = HBufC::NewL( KIRFormatStringWithIcon().Length() + 2 + 
			iLastplayed->Length()+iMainView.iUi->iLastPlayed->iLastPlayedStation->GetName().Length() );
			TPtr pItem( iItem->Des() );
			pItem.Format( KIRFormatStringWithIcon,iMainView.iUi->iLastPlayed->
						  iLastPlayedStation->GetIndex()+1, iLastplayed,
			  &iMainView.iUi->iLastPlayed->iLastPlayedStation->GetName());
			} 
		else
			{
			if(iItem)
				{
				delete iItem;
				iItem = NULL;
				}
			iItem = HBufC::NewL( KIRFormatStringWithoutIcon().Length() + 2 + 
			iLastplayed->Length()+iMainView.iUi->iLastPlayed->iLastPlayedStation->GetName().Length() );
			TPtr pItem( iItem->Des() );
			pItem.Format( KIRFormatStringWithoutIcon, iLastplayed,
			  		&iMainView.iUi->iLastPlayed->iLastPlayedStation->GetName());
			}
		iFavouritesListBox->SetLastPlayed(iLastPlayedChannel);
		iFilteredModel->SetAdvFlag(iLastPlayedChannel,*iItem);
		iFilteredModel->SetItemAtZero(iLastPlayedChannel,*iItem,ETrue);
		iFavouritesListBox->SetListItemFormat( iItem);

	   // Creating mark icon and appending to iconarray.
		CFbsBitmap* bitmap = NULL;
	    CFbsBitmap* bitmapMask = NULL;
	    
	    //CListItemDrawer is using this logical color as default for its marked icons
	    TRgb defaultColor;
	    defaultColor = iEikonEnv->Color(EColorControlText);
	    
	    AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(),
	    			KAknsIIDQgnIndiMarkedAdd,
	    			KAknsIIDQsnIconColors,
	    			EAknsCIQsnIconColorsCG13,
	    			bitmap,
	    			bitmapMask,
	    			KAvkonBitmapFile,
	    			EMbmInternetradioQgn_ir_transparent,
	    			EMbmInternetradioQgn_ir_transparent_mask,
	    			defaultColor
	    			);
	    
	    CGulIcon* icon = CGulIcon::NewL( bitmap,bitmapMask );
	    // poping markBitmap, markBitmapMask
	    // codescanner fix
	    CleanupStack::Pop( bitmapMask );
	    CleanupStack::Pop( bitmap ); 
	    CleanupStack::PushL( icon ); 
		iFavouritesListBox->SetAdvertisementIconL(icon,ETrue);
		CleanupStack::Pop( icon );	
		
		iListArray->AppendL( iItem->Des() );
		} 
	else
		{
		if(iAdvString)
			{
			delete iAdvString;
			iAdvString = NULL;	
			}
		_LIT(KNullFormat,"\t \t ");
		iAdvString = HBufC::NewL(KExtraLength);
		TPtr ptr(iAdvString->Des());
		ptr.Copy(KNullFormat);
		iFilteredModel->SetItemAtZero(iLastPlayedChannel,*iAdvString);
		}
	IRLOG_DEBUG( "CIRMainContainer::LastPlayedDisplayL - Exiting" );
	}
// ---------------------------------------------------------------------------
// TKeyResponse CIRMainContainer::OfferKeyEventL()
// From class CCoeControl.
// We need to catch the selection key event in order to do a couple of things
// in moving state make the movement, and in normal state open channel 
// specific popup menu. Up and down buttons are forwarded to the list.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRMainContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, TEventCode aType )
	{

	IRLOG_DEBUG( "CIRMainContainer::OfferKeyEventL - Entering" );
	TKeyResponse keyResp( EKeyWasNotConsumed );
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
			iMainView.SetCurrentFocus(iFavouritesListBox->CurrentItemIndex());
			if(!iMainView.GetMovingFlag())
				{
				if(iMainView.GetMarkedFlag()||iMainView.GetMarkedAllFlag())
					{
					ProcessPointerEventL(iFavouritesListBox->CurrentItemIndex()); //aListBox->CurrentItemIndex() not used         
					break;
					}
				else
					{
					iMainView.SetCurrentFocus(GetCurrentItemIndex());
					iMainView.HandleCommandL(EListenCmd);
					}
				}
			break;
			}
		default:
			{
			if(!iMainView.GetMovingFlag())
				{
				if((iMainView.GetListBoxMarkedCount() != GetListPointer()->Model()->NumberOfItems()))
					{
					if(iFavouritesListBox)
						{
						if ( iFavouritesSearchBox )
							{
							keyResp = iFavouritesSearchBox->OfferKeyEventL( aKeyEvent, aType );

							if( keyResp == EKeyWasNotConsumed )
								{
								keyResp = iFavouritesListBox->OfferKeyEventL( aKeyEvent, aType );
								}
	//						iFilteredFlag = ETrue
							CAknFilteredTextListBoxModel* model = 
							static_cast<CAknFilteredTextListBoxModel*>( iFavouritesListBox->Model() );
							model->Filter()->HandleOfferkeyEventL();					
							TInt filterModelCount = iFilteredModel->NumberOfItems();
							TInt favCount = iMainView.iUi->iFavPresets->iFavPresetList.Count();
							if(favCount != 0)
								{
								if(filterModelCount == 0 && !(iMainView.iUi->iLastPlayed->FileExists()))
									{
									HBufC* noMatchesTextResource = StringLoader::LoadLC(R_IRAPP_STATIONLIST_NOMATCHES);
									iFavouritesListBox->View()->SetListEmptyTextL(*noMatchesTextResource);
									CleanupStack::PopAndDestroy( noMatchesTextResource );
									}
								}
						iFavouritesSearchBox->GetSearchText(iFindString);
						if(iFindString.Length()!=0)
							{
							iFilteredFlag = ETrue;
							}
							iMainView.SetToolbarItems();
							}
						}
					}
				else
					{
					if(iFavouritesListBox)
						{
						if ( iFavouritesSearchBox )
							{
/*							TBool needRefresh( EFalse );
							// Offers the key event to find box.
							if ( AknFind::HandleFindOfferKeyEventL( aKeyEvent, aType, this,
							iFavouritesListBox, iFavouritesSearchBox,EFalse,needRefresh ) == EKeyWasConsumed )
							if(GetLastPlayedChannel()&&(iMainView.iUnMarkedArray.Count()==0))
								{
								}*/
							keyResp = iFavouritesSearchBox->OfferKeyEventL( aKeyEvent, aType );

							if( keyResp == EKeyWasNotConsumed )
								{
								keyResp = iFavouritesListBox->OfferKeyEventL( aKeyEvent, aType );
								}
							CAknFilteredTextListBoxModel* model = 
							static_cast<CAknFilteredTextListBoxModel*>( iFavouritesListBox->Model() );
							model->Filter()->HandleOfferkeyEventL();					
								
							iMainView.SetToolbarItems();
							return EKeyWasNotConsumed;
							}
						}
					}
				if(	aKeyEvent.iScanCode==EStdKeyBackspace)
					{
					if(iFavouritesSearchBox)
						{
						iFavouritesSearchBox->GetSearchText(iFindString);
						if(iFindString.Length()==0)
							{
							iFilteredFlag = EFalse;
							}
						}
					}
				}
			iMainView.SetToolbarItems();
			break;
			}
		}
	IRLOG_DEBUG( "CIRMainContainer::OfferKeyEventL - Exiting" );
	return keyResp; 
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::HandleListBoxEventL()
// From class MEikListBoxObserver.
// List box event handler.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
	IRLOG_DEBUG( "CIRMainContainer::HandleListBoxEventL - Entering" );
	switch ( aEventType )
		{
		/**  Keypress event. */
		case EEventEnterKeyPressed:
		/*  Item two-taps event*/
		case EEventItemDoubleClicked:
			{
			if(!iMainView.GetMovingFlag())
				{
				iMainView.SetCurrentFocus(iFavouritesListBox->CurrentItemIndex());
				if(iMainView.GetMarkedFlag()||iMainView.GetMarkedAllFlag())
					{
					ProcessPointerEventL(iFavouritesListBox->CurrentItemIndex()); //aListBox->CurrentItemIndex() not used         
					break;
					}
				else
					{
					iMainView.SetCurrentFocus(GetCurrentItemIndex());
					iMainView.HandleCommandL(EListenCmd);
					}
				}
			break;
			} 
		case EEventItemClicked:
			{
			iMainView.SetCurrentFocus(iFavouritesListBox->CurrentItemIndex());
			if(iMainView.GetMovingFlag())
				{
				CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
				// If the move functionalitiy is enabled and the focus is on the lasplayed then
				// only the cancel option has to be shown
				if(iMainView.GetCurrentFocus() == 0 && iMainView.iUi->iLastPlayed->FileExists())
					{
					cba->SetCommandSetL( R_IRAPP_SOFTKEY_CANCEL_CBA );
					cba->DrawDeferred();
					iMoveFocusLast = ETrue;
					}
				else
					{
					// If the move functionalitiy is enabled and if the focus is first moved to lastplayed ,
					// then after if the focus is moved to some other station in the list then
					// done and cancel as to be shown as left and right softkeys
					if(iMoveFocusLast)
						{
						cba->SetCommandSetL( R_IR_CBA_DONE_CANCEL );
						cba->DrawDeferred();
						}
					}
				
				}
			}
		break;
	default:
		break;
		}
	IRLOG_DEBUG( "CIRMainContainer::HandleListBoxEventL - Exiting" );
    }
// ----------------------------------------------------------------------------
// CIRMainContainer::ProcessPointerEventL
// Handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
//
void CIRMainContainer::ProcessPointerEventL( TInt /* aIndex */)
	{
	IRLOG_DEBUG( "CIRMainContainer::ProcessPointerEventL - Entering" );
	// Open the context sensitive menu 
	iContextSensitiveFlag = ETrue; 
	iMainView.ProcessCommandL(  EAknSoftkeyContextOptions );   
	IRLOG_DEBUG( "CIRMainContainer::ProcessPointerEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::Draw()
// From class CCoeControl.
// Draws the control.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::Draw( const TRect&/* aRect */) const
    {
	IRLOG_DEBUG( "CIRMainContainer::Draw - Entering" );

    CWindowGc& gc = SystemGc();
    if(iFavouritesListBox)
	    {
	   	iFavouritesListBox->SetGC(gc);
	   	iFavouritesListBox->SetControlFlag(ETrue);
	    }
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	
    // draw background skin first. 
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	
	TRect clientRect(((CEikAppUi*)(CCoeEnv::Static()->AppUi()))->ClientRect());

	AknsDrawUtils::DrawBackground ( skin, iBgContext, this, gc, TPoint(0,0),
					 TRect(TPoint(0,0),clientRect.iBr), KAknsDrawParamDefault  );	
	
 	IRLOG_DEBUG( "CIRMainContainer::Draw - Exiting" );
    }


// ---------------------------------------------------------------------------
// CIRMainContainer::GetHelpContext()
// From class CCoeControl.
// Opens help by context.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
	IRLOG_DEBUG( "CIRMainContainer::GetHelpContext - Entering" );
    aContext = TCoeHelpContext( KIRMCVUid, KIR_HELP_STATIONS  );
	IRLOG_DEBUG( "CIRMainContainer::GetHelpContext - Exiting" );

    }
    
// ---------------------------------------------------------------------------
// CIRMainContainer::UpdateAllL()
// updates the favourites data 
// ---------------------------------------------------------------------------
//    

void CIRMainContainer::UpdateAllL()
	{
	IRLOG_DEBUG( "CIRMainContainer::UpdateAllL() - Entering" );
	iListArray->Reset();
	
	iMainView.iUi->iFavPresets->GetAllPreset();
	iMainView.SetMovingFlag(EFalse);
		
	LastPlayedDisplayL();
	TInt favCount = iMainView.iUi->iFavPresets->iFavPresetList.Count();
	const RIRPresetArray& presets = iMainView.iUi->iFavPresets->Presets();
	for(TInt i=0;i<favCount;i++)
		{
		TUint indval;
		indval=i+1;
		RBuf stationName;
		stationName.Create(iMainView.iUi->iFavPresets->iFavPresetList[i]->
		Name().Length()+KExtraLength);
		stationName.AppendNum(indval);
		_LIT(KDot,". ");
		stationName.Append(KDot);
		stationName.Append(iMainView.iUi->iFavPresets->iFavPresetList[i]->Name()); 

		//check whether logo url exist for the current channel of the search results
		for(TInt m = 0; m < iNoLogosArray.Count(); m++)
			{
			if(m < iNoLogosArray.Count() && i<presets.Count() &&
			   presets[i]->Index() == iNoLogosArray[m])				
				{
				if(iMainView.iUi->iFavPresets->iFavPresetList[i]->GetShortDescription().Length())
					{
					//logo should not be displayed in the main view for this particular station
					//but has short description
					//element is not marked,has short description and no logo data (Format \t%S\t%S)
					HBufC* itemString = FormatItemStringLC( stationName, iMainView.iUi->iFavPresets->
					iFavPresetList[i]->GetShortDescription(), KIRNoLogo );
					iListArray->AppendL( itemString->Des() );
					CleanupStack::PopAndDestroy( itemString );
					break;
					}
				else
					{
					//logo should not be displayed in the main view for this particular station
					//but has no short description
					//element is not marked,don't have short desciption and  no logo data (Format \t%S\t)
					HBufC* itemString = FormatItemStringLC( stationName,KNullDesC , KIRNoLogo );
					iListArray->AppendL( itemString->Des() );
					CleanupStack::PopAndDestroy( itemString );
					break;							
					}
				}
			}

		for(TInt n = 0; n < iLogosArray.Count(); n++)
			{
			if(presets[i]->Index() == iLogosArray[n])
				{
				TInt index = n+1;
				if(iMainView.iUi->iLastPlayed->FileExists() && iMainView.iUi->
					iLastPlayed->iLastPlayedStation->GetLogoData().Length() && !iLogoDownloadError)
					{
					index = index +1;
					}
				
				
				if(iMainView.iUi->iFavPresets->iFavPresetList[i]->GetShortDescription().Length())
					{
					//logo should be displayed in the main view for this particular station
					//element is not marked and has logo data (Format %d\t%S\t%S)
					//has short description
					HBufC* itemString = FormatItemStringLC( stationName, iMainView.iUi->
					iFavPresets->iFavPresetList[i]->GetShortDescription(),index);
					iListArray->AppendL( itemString->Des() );
					CleanupStack::PopAndDestroy( itemString );
					break;
					}
				else
					{
					//logo should be displayed in the main view for this particular station
					//element is not marked and has logo data (Format %d\t%S)
					//has no short description
					HBufC* itemString = FormatItemStringLC( stationName,KNullDesC,index);
					iListArray->AppendL( itemString->Des() );
					CleanupStack::PopAndDestroy( itemString );
					break;								
					}
				}
			}

		stationName.Close();
		}
	if(iFilteredFlag)
		{
		CAknFilteredTextListBoxModel* model = 
		static_cast<CAknFilteredTextListBoxModel*>( iFavouritesListBox->Model() );
		model->Filter()->HandleOfferkeyEventL();					
				
		if((iMainView.GetCurrentFocus()-1)!=-1)
			{
			// Getting whether the element deleted is the last element
			// of the filtered elements.Returns true if it other than lat element 
			// and false if it last element
			TBool flag = CheckFiltered();
			if(!flag)
				{
				// If the deleted element id last element set the focus to first element 
				// of the array
				iFavouritesListBox->SetCurrentItemIndex(0);
				}
			}
		else if((iMainView.GetCurrentFocus()-1) ==-1)
			{
			iFavouritesListBox->SetCurrentItemIndex(0);	
			}
		else if(iMainView.GetCurrentFocus() == -1 && GetListBoxCount() != 0)
			{
			iFavouritesListBox->SetCurrentItemIndex(0);
			}

		}
	else
		{
		if(GetListBoxCount() == 1 && iLastPlayedChannel )
			{
			iFavouritesListBox->SetCurrentItemIndex(0);	
			}
		else if(GetListBoxCount() == 0 && !iLastPlayedChannel )
			{
			}
		else
			{
			if(iMainView.GetCurrentFocus()>= GetListBoxCount())
				{
				TInt focus ;
				if(iMainView.GetCurrentFocus()-1 >= GetListBoxCount() )
					{
					focus =0;
					}
				else
					{
					focus =	iMainView.GetCurrentFocus()-1;
					}
				iFavouritesListBox->SetCurrentItemIndex(focus);
				}
			else
				{
//				iFavouritesListBox->SetCurrentItemIndex(iMainView.GetCurrentFocus())
				}
			}
		iFavouritesListBox->HandleItemAdditionL();
		}
	if( !iFilteredFlag && favCount <=1)
	DisplayFilterListBoxL();

	EnableEmptyListBox();
//	iMainView.HandleCommandL(EUnmarkAll)
	if(favCount == 0 && !(iMainView.iUi->iLastPlayed->FileExists()))
		{
		HBufC* findStationsTextResource = StringLoader::LoadLC(R_IRAPP_FAVOURITES_LIST_EMPTY);
		iFavouritesListBox->View()->SetListEmptyTextL(*findStationsTextResource);
		CleanupStack::PopAndDestroy( findStationsTextResource );
		}
	if(iFinalCount == favCount)
		{
		if(!iProgressStart && iProgressFlag)
			{
			iMainView.iIsdsWrapper->GetDialogLauncherInstance()->
			ShowConfirmationNoteL( iMultiDeleteString,ETrue );	
			}
		else if(iProgressInfo && iProgressStart)
			{
			if ( KProgressbarFinalValue <= iProgressInfo->CurrentValue() )
				{
				iProgressDialog->ProcessFinishedL();
				iProgressDialog = NULL;
				iProgressInfo = NULL;
				DrawDeferred();	
				iMainView.iIsdsWrapper->GetDialogLauncherInstance()->
				ShowConfirmationNoteL( iMultiDeleteString,ETrue );	
				}  
			}
		iProgressStart = EFalse;
		iProgressFlag = EFalse;
		iDeletingCount = 1;
		iMainView.iMarkedIndexArray.Reset();
		iMainView.SetToolbarItems();
		iContextSensitiveFlag = EFalse; 
		iFinalCount = 0;
	    if(iMainView.GetCurrentFocus() == -1 && GetListBoxCount() != 0)
			{
			iFavouritesListBox->SetCurrentItemIndex(0);
			}
		}
	IRLOG_DEBUG( "CIRMainContainer::UpdateAllL() - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::EnableEmptyListBox
// Creating of Empty Listbox.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::EnableEmptyListBox()
	{
	IRLOG_DEBUG( "CIRMainContainer::EnableEmptyListBox() - Entering" );

	TInt filteredCount;
	filteredCount = GetFilterdModelPointer()->NumberOfItems();
	if(iLastPlayedChannel)
		{
		filteredCount = filteredCount -1;
		}
	TBuf<KFindStringLength> findString;
	if(iFavouritesSearchBox)
	iFavouritesSearchBox->GetSearchText(findString);
	IRLOG_DEBUG( "CIRMainContainer::EnableEmptyListBox() - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::FormatItemStringLC
// Formatter for list item strings.
// ---------------------------------------------------------------------------
//
HBufC* CIRMainContainer::FormatItemStringLC( const TDesC& aFirstRow, const TDesC& aSecondRow,
											 TInt aIconId )
    {
 	IRLOG_DEBUG( "CIRMainContainer::FormatItemStringLC() - Entering" );
    TInt length = KIRListItemBaseLength + aFirstRow.Length() + aSecondRow.Length();
    HBufC* itemString = HBufC::NewLC( length );
    
    TPtrC formatStringWithIcon( KIRFormatStringWithIcon );
    TPtrC formatStringWithoutIcon( KIRFormatStringWithoutIcon );

    if( aIconId >= 0 && aIconId <= KRange )
        {
        itemString->Des().Format( formatStringWithIcon, aIconId, &aFirstRow, &aSecondRow );
        }
    else 
        {
        itemString->Des().Format( formatStringWithoutIcon, &aFirstRow, &aSecondRow );
        }
    
 	IRLOG_DEBUG( "CIRMainContainer::FormatItemStringLC() - Exiting" );
    return itemString;
    }
// ---------------------------------------------------------------------------
// CIRMainContainer::GetCurrentItemIndex()
// returns the index of the currently selected item
// ---------------------------------------------------------------------------
//
TInt CIRMainContainer::GetCurrentItemIndex() const
    {
    IRLOG_DEBUG( "CIRMainContainer::GetCurrentItemIndex" );
	return iFavouritesListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CIRMainContainer::GetListBoxTopIndex()
// returns the index of the top item
// ---------------------------------------------------------------------------
//
TInt CIRMainContainer::GetListBoxTopIndex() const
	{
    IRLOG_DEBUG( "CIRMainContainer::GetListBoxTopIndex" );
	return iFavouritesListBox->View()->TopItemIndex () ;
	}
// ---------------------------------------------------------------------------
//void CIRMainContainer::HandlePresetChangedL( TInt /*aId*/, TUid /*aDataHandler*/, MPSPresetObserver::TPSReason /*aReason*/ )
// From class MPSPresetObserver.
// Handles preset content change.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::HandlePresetChangedL( TInt /*aId*/, TUid /*aDataHandler*/, MPSPresetObserver::TPSReason /*aReason*/ )
    {
    IRLOG_DEBUG( "CIRMainContainer::HandlePresetChangedL - Entering" );
    if(iMainView.GetAddToFavouratesFlag()|| iMainView.GetMovingFlag())
	    {
		iInvalidIconIndices.Reset();
		iLogosArray.Reset();
		iNoLogosArray.Reset();  
		iImageCount = 1;
	    SetIconsL();
	    }
	if(iProgressFlag)
		{
		iAftertime.HomeTime();
		iIntervalsec = iAftertime.MicroSecondsFrom(iBeforetime);
		if(iIntervalsec>=KProgressTimer)
			{
			CreateProgressBarL();	
			}
	    if(iProgressInfo)
		    {
			RBuf Feed;
			HBufC* bufferText ;
			bufferText = StringLoader::LoadLC(R_IRAPP_FAVOURITES_PROG_DELETING,iDeletingCount/*iProgressCount*/);
			Feed.Create(*bufferText);
			CleanupStack::PopAndDestroy(bufferText);
			iProgressDialog->SetTextL(Feed);

			TReal32 progressValue;
			progressValue	 = iProgressCount*iProgress;
			iProgressInfo->SetAndDraw(progressValue); 
			iProgressCount++; 
			Feed.Close();
		    }
		iMarkedElementsCount--;
//		iMainView.	iCurrentFocus =iMarkedElementsCount
		iDeletingCount++;
		TInt favCount = iMainView.iUi->iFavPresets->iFavPresetList.Count();
		if(iFinalCount == favCount)
			{
		 	UpdateAllL();
			}
		}
	else	
		{
 	  	UpdateAllL();
		}

   	IRLOG_DEBUG( "CIRMainContainer::HandlePresetChangedL - Exiting" );
    }
// ---------------------------------------------------------------------------
// void CIRMainContainer::CheckConnectionL() 
// Checks for the connection and creates  a connection to channel.
// ---------------------------------------------------------------------------
//	
void CIRMainContainer::CheckConnectionL(TInt aUrlIndex)
	{
   	IRLOG_DEBUG( "CIRMainContainer::CheckConnectionL - Entering" );
	if( iMainView.iUi->VerifyNetworkConnectivityL() )
	    {
        iMainView.SetRequestPendingStatus(ETrue) ;
        iMainView.SetFavListen(ETrue) ;
        iUrlIndex = aUrlIndex;
        }	
	else 
		{
		iMainView.DoConnectToChannelL( aUrlIndex ); 
		iUrlIndex = aUrlIndex;
		}
   	IRLOG_DEBUG( "CIRMainContainer::CheckConnectionL - Exiting" );

	}


//----------------------------------------------------------------------------
// CIRMainContainer::CountComponentControls() const
// returns the number of components in the view.
//----------------------------------------------------------------------------
//
TInt CIRMainContainer::CountComponentControls() const
	{
	IRLOG_DEBUG( "CIRMainContainer::CountComponentControls - Entering" );
	TInt count(KCONTROLSCOUNT);
	if(iFavouritesListBox)
		{
		count++;
		}
	if(iFavouritesSearchBox)
		{
		count++;
		}
	IRLOG_DEBUG( "CIRMainContainer::CountComponentControls - Exiting" );
	return count;
	}
//----------------------------------------------------------------------------
// CIRMainContainer::ComponentControl() const
// returns a pointer to the control under this view depending on the index
// passed,to the framework.
//----------------------------------------------------------------------------
// 
CCoeControl* CIRMainContainer::ComponentControl(TInt aIndex) const
	{
	IRLOG_DEBUG( "CIRMainContainer::ComponentControl - Entering" );
	IRLOG_DEBUG( "CIRMainContainer::ComponentControl - Exiting" );
	switch(aIndex)
		{
		case 0:
			{
			if(iFavouritesListBox)
			return iFavouritesListBox;
			}
		case 1:
			{
			if(iFavouritesSearchBox)
			return iFavouritesSearchBox;	
			}
		default:
			return NULL;
		}
	}
// ---------------------------------------------------------------------------
// void CIRMainContainer::SizeChanged(const TRect& aRect)
// Responds to changes to the size and position of the contents of this control.
// ---------------------------------------------------------------------------
//

void CIRMainContainer::SizeChanged()
	{
	IRLOG_DEBUG( "CIRMainContainer::SizeChanged - Entering" );
	if(iMainView.iUi->IsLandscapeOrientation())
		{
		if(iFavouritesListBox && iFavouritesSearchBox && iLandScapeCount ==0)
			{
	   		AknFind::HandlePopupFindSizeChanged(this,iFavouritesListBox,iFavouritesSearchBox);
	   		iLandScapeCount ++;
			}
		}
	if(iFavouritesListBox && iFavouritesSearchBox)
		{
	    AknLayoutUtils::LayoutControl(iFavouritesListBox,  Rect(),
            AKN_LAYOUT_WINDOW_list_gen_pane(1));
        AknLayoutUtils::LayoutControl(iFavouritesSearchBox,  Rect(),
            AKN_LAYOUT_WINDOW_find_pane);
		}
		
	else
		{
		if(iFavouritesListBox)
			{
			AknLayoutUtils::LayoutControl(iFavouritesListBox, Rect(),
            	AKN_LAYOUT_WINDOW_list_gen_pane(0));
			}
		}
		
	TRect clientRect(((CEikAppUi*)(CCoeEnv::Static()->AppUi()))->ClientRect());
	TRect parentRectSize(TPoint(0,0),clientRect.iBr);

	if ( iBgContext )
		{
		iBgContext->SetRect(parentRectSize);
		}
	IRLOG_DEBUG( "CIRMainContainer::SizeChanged - Exiting" );
	}
	
// ---------------------------------------------------------------------------
// void CIRMainContainer::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::HandleResourceChange(TInt aType)
	{
	IRLOG_DEBUG( "CIRMainContainer::HandleResourceChange - Entering" );
    CCoeControl::HandleResourceChange( aType );
    // application layout change request notification
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // reconstruct controls if needed
   		AknFind::HandlePopupFindSizeChanged(this,iFavouritesListBox,iFavouritesSearchBox);
        SizeChanged();
        }
	iMainView.SetToolbarItems();
	DrawDeferred();
	IRLOG_DEBUG( "CIRMainContainer::HandleResourceChange - Exiting" );
	}
//----------------------------------------------------------------------------
// CIRMainContainer::CreateFindBoxL()
// creates the Find box when any character is Pressed
//----------------------------------------------------------------------------
// 
CAknSearchField* CIRMainContainer::CreateFindBoxL(CEikListBox* aListBox,
CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle)
	{
	IRLOG_DEBUG( "CIRMainContainer::CreateFindBoxL - Entering" );
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

	IRLOG_DEBUG( "CIRMainContainer::CreateFindBoxL - Exiting" );
	return findbox;
	}
//----------------------------------------------------------------------------
// CIRMainContainer::DisableFindBox()
// Deletes the findbox
//----------------------------------------------------------------------------
// 
void CIRMainContainer::DisableFindBox()
	{
	IRLOG_DEBUG( "CIRMainContainer::DisableFindBox - Entering" );
	if (iFavouritesSearchBox)
		{
		iFilteredModel->RemoveFilter();
		// Important to set the find box non-focusing before
		// deleting it, otherwise the focus changes triggered
		// by the removal of the control from stack will focus
		// the find box which is under deletion
		iFavouritesSearchBox->SetNonFocusing();
		delete iFavouritesSearchBox;
		iFavouritesSearchBox = NULL;
		// Inform list box that find is hidden
		DrawDeferred();
		SizeChanged();
		}
	IRLOG_DEBUG( "CIRMainContainer::DisableFindBox - Exiting" );
	}

// ---------------------------------------------------------------------------
// Function : Comparestrings
// Function called to compare the filtered string inthe Itemarray
// ---------------------------------------------------------------------------
//
void  CIRMainContainer::Comparestrings()
	{
	IRLOG_DEBUG( "CIRMainContainer::Comparestrings - Entering" );
	if(iFilteredModel->NumberOfItems())
		{
		TPtrC16 filteredString = iFilteredModel->ItemText (iMainView.GetCurrentFocus());
		for(TInt i=0;i<iListArray->Count();i++)
			{
			TPtrC16 matchString=iListArray->MdcaPoint(i);
			if(filteredString==matchString)
				{
				iMainView.SetActualFilteredItemIndex(i);
				break;
				}

			}
		}
	IRLOG_DEBUG( "CIRMainContainer::Comparestrings - Exiting" );
	}
// ---------------------------------------------------------------------------
// Function : SetFilteredIndexArrayL
// stores the indexs of all the filtered elements
// ---------------------------------------------------------------------------
//
void  CIRMainContainer::SetFilteredIndexArrayL()
	{
	IRLOG_DEBUG( "CIRMainContainer::SetFilteredIndexArrayL - Entering" );
	iFilterIndexExist.Reset();
	TInt filterModelCount = iFilteredModel->NumberOfItems();
	TInt j=0;
	if(iMainView.iUi->iLastPlayed->FileExists())
		{
		j=1;	
		}
	if(iFilteredModel->NumberOfItems())
		{
		for( ;j<iFilteredModel->NumberOfItems();j++)
			{
			TPtrC16 filteredString = iFilteredModel->ItemText (j);
			for(TInt i=0;i<iListArray->Count();i++)
				{
				TPtrC16 matchString=iListArray->MdcaPoint(i);
				if(filteredString==matchString)
					{
					iFilterIndexExist.InsertInOrderL(i);
					}
				}
			}
		}
	IRLOG_DEBUG( "CIRMainContainer::SetFilteredIndexArrayL - Exiting" );
	}
// ---------------------------------------------------------------------------
// Function : CheckFiltered
// checks whether the deleted index exists in the filteredindex array or not
// ---------------------------------------------------------------------------
//
TBool CIRMainContainer::CheckFiltered()
	{
	IRLOG_DEBUG( "CIRMainContainer::CheckFiltered - Exiting" );
	TBool filteredIndexExist(EFalse);
	TInt currentFocus = iMainView.GetCurrentFocus();
	for(TInt i=0;i<iFilterIndexExist.Count();i++)
		{
		// checks whether the current focus index is present in the  filtered index array r not
		// and also check whether it is the last element of the array or not.
		if(currentFocus == iFilterIndexExist[i] && i!= (iFilterIndexExist.Count()-1))	
			{
			// sets the flag to true is the index is present in the array and no the last element
			filteredIndexExist = ETrue;
			return filteredIndexExist;	
			}
		}
	IRLOG_DEBUG( "CIRMainContainer::CheckFiltered - Exiting" );
	return filteredIndexExist;
	}

// ---------------------------------------------------------------------------
// void CIRMainContainer::HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent /*aEvent*/, 
// from base class MIRImageConverterObserver
// ---------------------------------------------------------------------------
//
void CIRMainContainer::HandleImageConversionEventL( MIRImageConverterObserver::
													TIRImageConversionEvent /*aEvent*/,
                                                    TInt aId, 
                                                    TInt aError )
	{
	IRLOG_DEBUG( "CIRMainContainer::HandleImageConversionEventL - Entering" );
	iError=aError;
	if(aError == KErrNotSupported && iImageLast)
		{
		iImageLast	= EFalse;
		iLogoDownloadError	= ETrue;
		}
	if(!aError)
		{
		CArrayPtr<CGulIcon>* iconArray = iFavouritesListBox->
											 ItemDrawer()->FormattedCellData()->IconArray();
		TInt imageCount;
		if(iImageLast)
			{
			imageCount = iImageCount;
			TInt index = iMainView.iUi->iLastPlayed->iLastPlayedStation->GetIndex();
			CFbsBitmap* bitmap = NULL;
			CFbsBitmap* mask = NULL;
			CGulIcon* icon = CGulIcon::NewLC();
			aId == KErrUnknown ? iConverter->TransferBitmapOwnership( bitmap, mask ) : 
			     iCurrentItemConverter->TransferBitmapOwnership( bitmap, mask );

			if ( bitmap && mask )
				{
				icon->SetBitmap( bitmap );
				icon->SetMask( mask );
				icon->SetBitmapsOwnedExternally( EFalse );

				iconArray->InsertL( imageCount, icon );
				CleanupStack::Pop( icon );
				if(  imageCount+1 < iconArray->Count())
					{
					icon = iconArray->At(imageCount+1 );
					iconArray->Delete(imageCount +1);
					delete icon;
					}
				}
			DrawDeferred();
			iImageLast = EFalse;
			}
		else
			{
			imageCount = iImageCount;
			iLogosArray.InsertInOrderL(iInvalidIconIndices[0]);
			iNoLogosArray.Remove(iNoLogosArray.FindInOrder(iInvalidIconIndices[0]));
			if ( aError == KErrNone )
				{
				CIRPreset* preset = NULL; 

				if ( aId == KErrUnknown )
					{
					if ( iInvalidIconIndices.Count() )
						{
						preset = iMainView.iUi->iFavPresets->PresetByIndex( iInvalidIconIndices[0] );
						iInvalidIconIndices.Remove( 0 );
						}
					}
				else
					{
					preset = iMainView.iUi->iFavPresets->PresetById( aId );
					}
				if ( preset )
					{
					CFbsBitmap* bitmap = NULL;
					CFbsBitmap* mask = NULL;
					CGulIcon* icon = CGulIcon::NewLC();
					aId == KErrUnknown ? iConverter->TransferBitmapOwnership( bitmap, mask ) : 
					     iCurrentItemConverter->TransferBitmapOwnership( bitmap, mask );
					if ( bitmap && mask )
						{

						icon->SetBitmap( bitmap );
						icon->SetMask( mask );
						icon->SetBitmapsOwnedExternally( EFalse );

						iconArray->InsertL( imageCount, icon );
						CleanupStack::Pop( icon );
						if(  imageCount+1 < iconArray->Count())
							{
							icon = iconArray->At(imageCount+1 );
							iconArray->Delete(imageCount +1);
							delete icon;
							}
						}
					DrawDeferred();
					}
				}
			}
		iImageCount ++;
		UpdateAllL();
		if ( iInvalidIconIndices.Count() && aId == KErrUnknown )
			{
			StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
			}	
		}
	IRLOG_DEBUG( "CIRMainContainer::HandleImageConversionEventL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRMainContainer::StartConvertL( CIRImageConverter& aConverter, TInt aIndex, TInt aId )
// used to convert the images depending the index passed
// ---------------------------------------------------------------------------
//
void CIRMainContainer::StartConvertL( CIRImageConverter& aConverter, TInt aIndex, TInt aId )
	{
	IRLOG_DEBUG( "CIRMainContainer::StartConvertL - Entering" );
	iError=KErrNone;
	if(iImageLast)
		{
		if ( iMainView.iUi->iLastPlayed->iLastPlayedStation &&
		     iMainView.iUi->iLastPlayed->iLastPlayedStation->GetLogoData().Length() )
			{
			aConverter.SetDataL( iMainView.iUi->iLastPlayed->iLastPlayedStation->GetLogoData() );
			if(iError==KErrNone)
				{
				aConverter.StartL(KBitmapSize, aId );
				}
			if(iError!=KErrNone)
				{
				if ( iInvalidIconIndices.Count() )
					{
					iImageLast = EFalse;
					iLogoDownloadError	= ETrue;
					StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
					}
				}
			}
		}
	else
		{
		CIRPreset* preset = iMainView.iUi->iFavPresets->PresetByIndex( aIndex );
		if ( preset && preset->GetLogoData().Length() )
			{
			aConverter.SetDataL( preset->GetLogoData() );
			if ( iError==KErrNone )
				{
				aConverter.StartL(KBitmapSize, aId );
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
		}
	IRLOG_DEBUG( "CIRMainContainer::StartConvertL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::PresetLogoDownloadL()
// 
// ---------------------------------------------------------------------------
//
void CIRMainContainer::PresetLogoDownloadL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRMainContainer::PresetLogoDownloadL - Entering" );
    iImageLast = ETrue;
    TInt index = iMainView.iUi->iLastPlayed->iLastPlayedStation->GetIndex();
	StartConvertL( *iConverter, index, KErrUnknown );
	IRLOG_DEBUG( "CIRMainContainer::PresetLogoDownloadL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRMainContainer::HandleLogoErrorL()
// called from the iSearchResultsView when the logo is not downloaded
// ---------------------------------------------------------------------------
//
void CIRMainContainer::HandleLogoErrorL()
	{
	iImageLast = EFalse;
	iLogoDownloadError	= ETrue;
	if ( iInvalidIconIndices.Count() )
		{
		StartConvertL( *iConverter, iInvalidIconIndices[0], KErrUnknown );
		}
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::cmdMarkL()
// implement the mark/unmark feature
// ---------------------------------------------------------------------------
//
void CIRMainContainer::cmdMarkL(TInt item,TBool mark)
	{
	IRLOG_DEBUG( "CIRMainContainer::cmdMark - Entering" );
	CTextListBoxModel* model =iFavouritesListBox->Model();
	TInt itemCount = model->NumberOfItems();
	CListBoxView* listBoxView =iFavouritesListBox->View();
	if (mark)
		{
		for (TUint i = 0; i < itemCount; i++)
		if(iLastPlayedChannel)
			{
			if (item == 0 || item == i)
			listBoxView->SelectItemL(i);
			listBoxView->DeselectItem(0);
			}
		else
			{
			if (item == -1 || item == i)
			listBoxView->SelectItemL(i);
			}
		}
	else
		{
		for (TUint i = 0; i < itemCount; i++)
		if(iLastPlayedChannel)
			{
			if (item == 0 || item == i)
			listBoxView->DeselectItem(i);
			}
		else
			{
			if (item == -1 || item == i)
			listBoxView->DeselectItem(i);
			}
		}
	for (TInt i = 0; i < itemCount; i++)
	if(iLastPlayedChannel)
		{
		if (item == 0 || item == i)
		listBoxView->SelectItemL(0);
		}
	IRLOG_DEBUG( "CIRMainContainer::cmdMark - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::GetListBoxCount()
// returns the listbox count.
// ---------------------------------------------------------------------------
//
TInt CIRMainContainer::GetListBoxCount()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetListBoxCount " );
	return iListArray->Count();	
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::GetUrlIndex()
// returns the url index.
// ---------------------------------------------------------------------------
//
TInt CIRMainContainer::GetUrlIndex()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetListBoxCount " );
	return iUrlIndex;	
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::GetListPointer()
// returns pinter to Listbox.
// ---------------------------------------------------------------------------
//
CIRAdvertisingListBox* CIRMainContainer::GetListPointer()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetListPointer" );
	return iFavouritesListBox;	
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::GetSearchBoxPointer()
// returns the search box pointer.
// ---------------------------------------------------------------------------
//
CAknSearchField* CIRMainContainer::GetSearchBoxPointer()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetSearchBoxPointer" );
	return 	iFavouritesSearchBox;
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::GetLastPlayedChannel()
// returns whether the lastplayed channel exists or not.
// ---------------------------------------------------------------------------
//
TBool CIRMainContainer::GetLastPlayedChannel()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetLastPlayedChannel" );
	return iLastPlayedChannel;	
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::GetFilterdModelPointer()
// returns pinter to filtered model.
// ---------------------------------------------------------------------------
//
CIRFilteredModel* CIRMainContainer::GetFilterdModelPointer()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetListPointer" );
	return iFilteredModel;	
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::DeleteMultiStationsL()
// To delete multiple items from favourites list.
// ---------------------------------------------------------------------------
//
void CIRMainContainer::DeleteMultiStationsL()
	{
	IRLOG_DEBUG( "CIRMainContainer::MultiDelete - Entering" );
	HBufC* textResource = NULL;
	if(iMainView.iMarkedIndexArray.Count()==1)
		{
		if(iMainView.iMarkedIndexArray[0]<= iMainView.iUi->iFavPresets->
					                 iFavPresetList.Count())
			{
			SetFilteredIndexArrayL();
			textResource = StringLoader::LoadLC( R_IRAPP_NOTE_REMOVED,iMainView.iUi->iFavPresets->
						                 iFavPresetList[iMainView.iMarkedIndexArray[0]]->Name());
			}
		}
	else
		{
		textResource = StringLoader::LoadLC( R_IRAPP_NOTE_REMOVED_MANY,
											iMainView.iMarkedIndexArray.Count());
		}
	iMultiDeleteString.Create(*textResource);

	TInt favouritesCount = iMainView.iUi->iFavPresets->iFavPresetList.Count();
	iMarkedElementsCount = iMainView.iMarkedIndexArray.Count();
	iFinalCount = favouritesCount- iMarkedElementsCount;
	iProgress = KProgressbarFinalValue/iMarkedElementsCount;
	

	TInt numOfItems = GetListBoxCount();
	iBeforetime.HomeTime();
	for(TInt j =0; j<iMainView.iMarkedIndexArray.Count();j++)
		{
		TInt presentIndex = iMainView.iMarkedIndexArray[j];
		for(TInt i=0;i<=numOfItems;i++)
			{
			if((iMainView.iMarkedIndexArray[j])==i)
				{
				TInt channeUniqId;
				channeUniqId = iMainView.iUi->iFavPresets->
							   iFavPresetList[iMainView.iMarkedIndexArray[j]]->Id();
				// delete the index from the logo's array
				RemoveFromLogoArray(iMainView.iMarkedIndexArray[j]);
				iMainView.iUi->iFavPresets->DeletePresetL(channeUniqId);
				iAftertime.HomeTime();
				iIntervalsec = iAftertime.MicroSecondsFrom(iBeforetime);
				if(iIntervalsec>=KProgressTimer)
					{
					CreateProgressBarL();	
					}
				}
			}
		iProgressFlag = ETrue;
		}
	if(iMainView.iCurrentFocus<0 ||iMainView.iCurrentFocus>GetListBoxCount())
		{
		iMainView.SetCurrentFocus(0);
		}
	cmdMarkL(0,EFalse);
	iMainView.SetMarkedAllFlag(EFalse);
	iMainView.SetMarkedFlag(EFalse);
	iMainView.iUnMarkedArray.Reset();
	iMainView.SetListBoxMarkedCount(0);
	CleanupStack::PopAndDestroy(textResource);
	cmdMarkL(-1,EFalse);
	IRLOG_DEBUG( "CIRMainContainer::MultiDelete - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::DeleteChannelL
// ---------------------------------------------------------------------------
//
void CIRMainContainer::DeleteChannelL( TInt aIndex ) 
	{
	IRLOG_DEBUG( "CIRMainContainer::DeleteChannelL - Entering" );
	SetFilteredIndexArrayL();
	RBuf channel;
	HBufC* textResource =  NULL;
	if(aIndex < iMainView.iUi->iFavPresets->iFavPresetList.Count())
	textResource = StringLoader::LoadLC( R_IRAPP_NOTE_REMOVED,iMainView.iUi->iFavPresets->
											iFavPresetList[aIndex]->Name() );
	// delete the index from the logo's array
	RemoveFromLogoArray(aIndex);
		
	channel.Create(*textResource);
	TInt channeUniqId = iMainView.iUi->iFavPresets->iFavPresetList[aIndex]->Id();
	// Removes the station from the Favourites preset.
	iMainView.iUi->iFavPresets->DeletePresetL(channeUniqId);
	iMainView.iIsdsWrapper->GetDialogLauncherInstance()->ShowConfirmationNoteL( channel,ETrue );
	CleanupStack::PopAndDestroy(textResource);
	channel.Close();
	iContextSensitiveFlag = EFalse; 
	IRLOG_DEBUG( "CIRainContainer::DeleteChannelL - Exiting" );
	}
//----------------------------------------------------------------------------
// CIRMainContainer::RemoveFromLogoArray() 
// Removes the element from logo array.
//----------------------------------------------------------------------------
// 	
void CIRMainContainer::RemoveFromLogoArray(TInt aIndex)
	{
	TBool indexFound = EFalse;
	const RIRPresetArray& presets = iMainView.iUi->iFavPresets->Presets();
	CArrayPtr<CGulIcon>* iconArray = iFavouritesListBox->
										 ItemDrawer()->FormattedCellData()->IconArray();
	// checks whether the index to deleted in the iNolOgosArray or not
	for(TInt i=0;i<iNoLogosArray.Count();i++)
		{
		if(iNoLogosArray[i]==presets[aIndex]->Index() && aIndex <presets.Count())
			{
			iNoLogosArray.Remove(i);
			indexFound = ETrue;
			return;
			}
		}
	if(!indexFound)
		{
		for(TInt i=0;i<iLogosArray.Count();i++)	
			{
			TInt index = i+1;
			if(iMainView.iUi->iLastPlayed->FileExists() && iMainView.iUi->
				iLastPlayed->iLastPlayedStation->GetLogoData().Length())
				{
				index = index +1;
				}
	// checks whether the index to be is present deleted in the iNologosArray or not
			if(iLogosArray[i] == presets[aIndex]->Index())
				{
				iLogosArray.Remove(i);
				iconArray->Delete(index);
				return;
				}
			}
		}
	
	}
	
//----------------------------------------------------------------------------
// CIRMainContainer::DisplayFilterListBoxL() 
// Displayin gof filter box depending on favourites
//----------------------------------------------------------------------------
// 	
void CIRMainContainer::DisplayFilterListBoxL()
	{
	IRLOG_DEBUG( "CIRMainContainer::DisplayFilterListBoxL - Entering" );
	TInt favCount = iMainView.iUi->iFavPresets->iFavPresetList.Count();
	if(favCount>1)
		{
		if(iFavouritesSearchBox)
			{
			DisableFindBox();
			}
		// Added For Dynamic Filtering
		iFavouritesSearchBox = CreateFindBoxL(iFavouritesListBox, iFilteredModel,
		  CAknSearchField::ESearch);
		SizeChanged();	
		}
	else
		{
		if(iFavouritesSearchBox)
			{
			DisableFindBox();
			}
		}
	IRLOG_DEBUG( "CIRMainContainer::DisplayFilterListBoxL - Exiting" );
	}


// -----------------------------------------------------------------------------
// CIRMainContainer::DialogDismissedL()
// Called when/if the dialog has been dismissed.
// iIdle must be canceled when cancel button is pressed.
// -----------------------------------------------------------------------------
//
void CIRMainContainer::DialogDismissedL( TInt aButtonId )
    {
    // Check when pressing cancel button.
	switch(aButtonId)
		{
		case EAknSoftkeyDone:   
		case EAknSoftkeyCancel:
			{   
			if(iProgressDialog)
				{	
				iProgressDialog = NULL;
				iProgressInfo = NULL;
				}
			}
		break;
		default:
		break;
		}
	return;
	}

// ---------------------------------------------------------------------------
// Function : FilteredIndexL()
// Function called when mark all is called after dynamic filtering
// ---------------------------------------------------------------------------
//
void CIRMainContainer::FilteredIndexL()
	{
	for (TInt j = 0;j<iFilteredModel->NumberOfItems();j++)
		{
		TPtrC16 filteredString = iFilteredModel->ItemText (j);
		for(TInt i=0;i<iListArray->Count();i++)
			{
			TPtrC16 matchString=iListArray->MdcaPoint(i);
			if(filteredString==matchString)
				{
				if(iLastPlayedChannel && filteredString == iListArray->MdcaPoint(0))
					{
					// If Lastplayed is present then don't insert that element into marked array.	
					}
				else
					{
					if(iLastPlayedChannel)
						{
						i = i-1;	
						}
					iMainView.iMarkedIndexArray.InsertInOrderL(i);	
					}
				break;
				}

			}
		}
	}
// ---------------------------------------------------------------------------
// Function : CreateProgressBarL()
// Function called when mark all is called after dynamic filtering
// ---------------------------------------------------------------------------
//
void CIRMainContainer::CreateProgressBarL()
	{
	if(!iProgressDialog)
		{
		iProgressStart = EFalse;
		iProgressCount = 1;
		if(iMarkedElementsCount)
			{
			iProgress = KProgressbarFinalValue/iMarkedElementsCount;
			}
		iProgressDialog = new (ELeave) CAknProgressDialog(
	           (REINTERPRET_CAST(CEikDialog**, &iProgressDialog)),
	           ETrue);
	    iProgressDialog->PrepareLC(R_IRAPP_PROGRESS_DIALOG_CANCEL);
	    iProgressInfo = iProgressDialog->GetProgressInfoL();
	    iProgressDialog->SetCallback(this);
		iProgressDialog->RunLD();
		iProgressInfo->SetFinalValue(KProgressbarFinalValue);
		iProgressStart = ETrue;
		}
	}
// ---------------------------------------------------------------------------
// CIRMainContainer::GetContextSenstiveFlag()
// returns the context senstive flag.
// ---------------------------------------------------------------------------
//
TBool CIRMainContainer::GetContextSenstiveFlag()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetContextSenstiveFlag" );
	return iContextSensitiveFlag;	
	}
// ---------------------------------------------------------
// CIRMainContainer::SetContextSenstiveFlag
// Sets the context sensitive flag
// ---------------------------------------------------------
//
void CIRMainContainer::SetContextSenstiveFlag(TBool aContextSensitiveFlag)
	{
	IRLOG_DEBUG( "CIRMainContainer::SetContextSenstiveFlag - Entering." );
	iContextSensitiveFlag = aContextSensitiveFlag;
	IRLOG_DEBUG( "CIRMainContainer::SetContextSenstiveFlag - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRMainContainer::GetFilteredFlag()
// returns the filtered flag.
// ---------------------------------------------------------------------------
//
TBool CIRMainContainer::GetFilteredFlag()
	{
	IRLOG_DEBUG( "CIRMainContainer::GetContextSenstiveFlag" );
	return iFilteredFlag;	
	}

// ---------------------------------------------------------
// CIRMainContainer::RefreshView
// Resets all the values and draws the view with new values
// ---------------------------------------------------------
//
void CIRMainContainer::RefreshViewL()
	{
	IRLOG_DEBUG( "CIRMainContainer::RefreshViewL - Entering." );
	iInvalidIconIndices.Reset();
	iLogosArray.Reset();
	iNoLogosArray.Reset();  
	iImageCount = 1;
 	iLogoDownloadError	= EFalse;
    SetIconsL();
    UpdateAllL();
	IRLOG_DEBUG( "CIRMainContainer::RefreshViewL - Exiting." );
	}
