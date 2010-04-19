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
* Description:  Search Results view class
*
*/




#include <akniconarray.h>
#include <akntitle.h>
#include <eikmenup.h>
#include <gulicon.h>
#include <stringloader.h>
#include <psserv.h>
#include <hlplch.h>
#include <aknnotewrappers.h>
#include <eikenv.h>
#include <internetradio.rsg>
#include <internetradio.mbg>
#include "irviewstack.h"
//For Touch Toolbar
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <aknbutton.h>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irsearchResultsview.h"
#include "irsearchresultsviewcontainer.h"
#include "irisdswrapper.h"
#include "irsettings.h"
#include "irnowplayingwrapper.h"
#include "irpubsub.h"

#include "hlplch.h"
#include "irimageconverter.h"
#include "irlogodownloadengine.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irdialoglauncher.h"
#include "irdebug.h" //  PC-Lint comments :: SPP
#include "irfilteredmodel.h"

#include "iractivenetworkobserver.h"
#include "irnetworkcontroller.h" 

const TInt KStringSize = 20;
const TInt KLogosSize = 59;
const TInt KLogosReqFrom = 0;


// ---------------------------------------------------------------------------
// CIRSearchResultsView::CIRSearchResultsView()
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRSearchResultsView::CIRSearchResultsView():iCallRequestPending(EFalse)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::CIRSearchResultsView " );
	iSearchReqMade = EFalse;
    }


// ---------------------------------------------------------------------------
// void CIRSearchResultsView::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::ConstructL()
    {
    IRLOG_DEBUG( "CIRSearchResultsView::ConstructL - Entering " );
    CIRBaseView::ConstructL( R_IR_SEARCHRESULTS );
    
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
    TRgb color;
    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
    				                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_IR_SEARCHRESULTS_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_IR_SEARCHRESULTS_BLACK_TOOLBAR);	
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
	SetCurrentFocus(0);
	iFilteredIndex = 0;
	iRequestPending = EFalse;
    iAddToFav= EFalse;
	IRLOG_DEBUG( "CIRSearchResultsView::ConstructL - Exiting " );
    }

// -----------------------------------------------------------------------------
// CIRSearchResultsView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRSearchResultsView* CIRSearchResultsView::NewL()
    {
 	IRLOG_DEBUG( "CIRSearchResultsView::NewL" );
 	CIRSearchResultsView* self = CIRSearchResultsView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRSearchResultsView::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRSearchResultsView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRSearchResultsView* CIRSearchResultsView::NewLC()
    {
    IRLOG_DEBUG( "CIRSearchResultsView::NewLC - Entering" );
    CIRSearchResultsView* self = new( ELeave ) CIRSearchResultsView();
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRSearchResultsView::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRSearchResultsView::~CIRSearchResultsView()
// Destructor
// ---------------------------------------------------------------------------
//
CIRSearchResultsView::~CIRSearchResultsView()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::~CIRSearchResultsView - Entering " );
	if (iContainer)
		{
		AppUi()->RemoveFromViewStack( *this, iContainer);
		delete iContainer;
		iContainer = NULL;
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	iFilteredIndex = 0;
	IRLOG_DEBUG( "CIRSearchResultsView::~CIRSearchResultsView - Exiting " );
	}

// ---------------------------------------------------------------------------
// TUid CIRSearchResultsView::Id() const
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRSearchResultsView::Id() const
    {
    IRLOG_DEBUG( "CIRSearchResultsView::Id" );
    return KIRSearchResultsViewID;
    }

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::DoActivateL( )
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
	IRLOG_DEBUG( "CIRSearchResultsView::DoActivateL() started." );
	if(iUi->IsForeground())
		{
		TInt customMessageId = aCustomMessageId.iUid;
		if(customMessageId == 1)
			{
			SetCurrentFocus(0);
			SetCurrentTopItem(0);
			iFirstTimeLaunchFlag= ETrue;
			}
		aCustomMessageId.iUid = 0;


	////////////////////////////////////////////////////////////

	if(iSearchReqMade)
		{
		if(iContainer)
			{
		    delete iContainer;
		    iContainer = NULL;
			}
	    iSearchReqMade = EFalse;
		}
	
	////////////////////////////////////////////////////////////
	  	SetStatusPaneTextL();
	    CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );


		ConstructToolbarL();
        iUi->iPresentViewID = KIRSearchResultsViewID;
	    if ( !iContainer )
	        {
	        iContainer = CIRSearchResultsViewContainer::NewL(ClientRect(),*this);
		    iContainer->CreateListBoxL();
	    	AppUi()->AddToViewStackL( *this, iContainer);
	        }
	    else
	        {
			// we must notify a resource change in order to update the skin context
			// This is a flaw of CEikFormattedCellListBox
			iContainer->SetRect(ClientRect());
	        iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
	        AppUi()->AddToViewStackL( *this, iContainer);
	        }


	    iContainer->ActivateL();
	    iContainer->MakeVisible( ETrue );


	    HandleStatusPaneSizeChange();
    	EnableObserverL(EIRSystemEventObserver);
   	    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
		}
		IRLOG_DEBUG( "CIRSearchResultsView::DoActivateL() exiting." );
	}

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::DoDeactivate()
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::DoDeactivate()
    {
    IRLOG_DEBUG( "CIRSearchResultsView::DoDeactivate() - Entering." );
    if(&(iUi->SystemEventCollector()))
	    {
		DisableObserver(EIRSystemEventObserver);
	    }
    if ( iContainer )
        {
        if(iContainer->GetListPointer() && iContainer->GetSearchBoxPointer())
	        {
	        TBuf<KFindStringLength> findstring = iContainer->GetFindString();
			iContainer->GetSearchBoxPointer()->GetSearchText(findstring);
			if(iContainer->GetSearchBoxPointer() && findstring.Length()!=0)
				{
				if(!iIndex)
					{
					TInt currentIndex = iContainer->GetCurrentItemIndex();
					iFilteredIndex = currentIndex;
					iContainer->Comparestrings();
					}
				SetCurrentFocus(iIndex);
				}
			else
				{
				TInt currentIndex = iContainer->GetCurrentItemIndex();
				SetCurrentFocus(currentIndex);
				}
		 SetCurrentTopItem(iContainer->GetListBoxTopIndex());
	     }
        iUi->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        }
		// Disable focus of toobar, so that it can't steal it from other views
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
     iFirstTimeLaunchFlag= EFalse;
     iUi->iNetworkController->DeleteActiveNetworkObserver(*this);
	 IRLOG_DEBUG( "CIRSearchResultsView::DoDeactivate() - Exiting." );

    }

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::HandleCommandL( TInt aCommand )
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRSearchResultsView::HandleCommandL - Entering." );
	iCommand = aCommand;
	switch(aCommand)
		{
		case EListenCmd:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			
			iUi->SetListenFromPls(EFalse);
			if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				iListenRequest = ETrue;
				break;
				}
			else
				{
				ListenChannelL(GetActualIndex());	
				}	
			break;
			}
		case ESearchCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iIsSearchAgain = ETrue;
				HBufC *SearchTitle = StringLoader::LoadLC(R_IRAPP_SEARCH_STRING);
				_LIT(KNullText,"");
				RBuf string1;
				string1.Create(KStringSize);
				string1.Append(KNullText);
				ShowQueryDialogL(*SearchTitle,string1);
				CleanupStack::PopAndDestroy(SearchTitle);
				string1.Close();
				}
			}
		break;

		//Activate NowPlaying view.
		case EGotoNowPlayingViewCmd:
			{
			iUi->SetHistory(EFalse);
			iUi->SavePrevIDL(KIRSearchResultsViewID, aCommand);
			}
		break;

		case EAddtoFavouritesCmd:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			else if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				iAddToFav = ETrue;
				break;
				}
			else
				{
				iAddToFav = ETrue;
				AddToFavouritesL(GetActualIndex(),aCommand);
				}	
			break;
			}

		//Activate settings view.
		case ESettingsCmd:
			{
			iUi->ActivateLocalViewL(KIRSettingsViewId);
			}
		break;
		case EAddStationManuallyCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iUi->SetEditStation(EFalse);
				iUi->ActivateLocalViewL(KIRAddManuallyStationViewID);
				}
			}
		break;

		case EAdvertisementUrl:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			else if( iUi->VerifyNetworkConnectivityL() )
				{
				break;
				}
				if(iUi->iIsdsWrapper->iClickThroughUrl)
					{
					if(iUi->iIsdsWrapper->iClickThroughUrl->Length() != 0)
						{
						iContainer->OpenUrlL(*iUi->iIsdsWrapper->iClickThroughUrl);
						}
					}
			else
				{
				iUi->iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL
					(R_IRAPP_ERROR_URL_UNAVAILABLE,ETrue);
				}

			}
		break;

		case EHelpCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
				}
			break;
			}
		case EAknSoftkeyBack:
        	{
 			CIRViewStack& viewStack = iUi->ViewStack();

			if((iUi->IsFirstTime()))
				{
				CIRBaseView::HandleCommandL( aCommand );
				}
			else
				{
				if(viewStack.Count() == 2 )
					{
					TUid topView = viewStack.Pop();
					if(viewStack.Peek()==KIRMainChoiceViewID)
						{
						viewStack.PushL(topView);
						CIRBaseView::HandleCommandL( aCommand );
						}
					else
						{
						viewStack.Pop();
						iUi->ActivateLocalViewL(KIRMainChoiceViewID);
						}
					}
				else
					{
					viewStack.Pop();
					iUi->ActivateLocalViewL(KIRMainChoiceViewID);
					}
				}
    		}
     	break;
		default:
			{
			CIRBaseView::HandleCommandL( aCommand );
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::HandleCommandL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRSearchResultsView::GetActualIndex().
// Returns the actual index.
// ---------------------------------------------------------------------------
//
TInt CIRSearchResultsView::GetActualIndex()
    {
    TInt currentIndex = iContainer->GetCurrentItemIndex();
	SetCurrentFocus(currentIndex);
	TBuf<KFindStringLength> findstring ;
   if(iContainer->GetSearchBoxPointer())
	   {
    	findstring = iContainer->GetFindString();
    	iContainer->GetSearchBoxPointer()->GetSearchText(findstring);
	   }
	if(iContainer->GetSearchBoxPointer() && findstring.Length()!=0)
		{
		iFilteredIndex = currentIndex;
		iContainer->Comparestrings();
		currentIndex = iIndex;
		}
	if(iContainer->GetLogoNotSupported())
		{
		currentIndex = 	currentIndex +1;
		}
	return currentIndex;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::HandleStatusPaneSizeChange()
    {
    IRLOG_DEBUG( "CIRSearchResultsView::HandleStatusPaneSizeChange - Entering" );
    
    // the client rect is also set here after screen orientation change
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
       }
    IRLOG_DEBUG( "CIRSearchResultsView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::SetStatusPaneTextL()
// Sets the title pane of the search results view
// ---------------------------------------------------------------------------
//

void CIRSearchResultsView:: SetStatusPaneTextL()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::SetStatusPaneTextL - Entering" );
	TInt number = iIsdsWrapper->iStationsData->MdcaCount();
	HBufC* stringholder = NULL;

	CDesCArray* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( array );
	array->AppendL( iUi->iSearchString); //First string

	MDesCArray* list = STATIC_CAST(MDesCArray*,array);

	CArrayFix<TInt>* presetCounts = new (ELeave) CArrayFixFlat<TInt>( 2 );
	CleanupStack::PushL( presetCounts );
	presetCounts->AppendL( number );

	if(1 == number)
		{
		stringholder = StringLoader::LoadLC( R_IRAPP_SEARCHRESULT_TITLETEXT,*list,
											 *presetCounts, iEikonEnv );
		}
	else
		{
		stringholder = StringLoader::LoadLC( R_IRAPP_SEARCHRESULTS_TITLETEXT,*list,
											 *presetCounts, iEikonEnv );
		}
	static_cast<CAknTitlePane*>(StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) )->
						SetTextL( *stringholder );
						
	CleanupStack::PopAndDestroy(stringholder);
	CleanupStack::PopAndDestroy( presetCounts );

	// Pop and delete strings array
	CleanupStack::PopAndDestroy();
	IRLOG_DEBUG( "CIRSearchResultsView::SetStatusPaneTextL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//


void CIRSearchResultsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::DynInitMenuPaneL - Entering" );
	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_MENU_SEARCHRESULTS)
			{
			// Now Playing option
			if(iNowPlayingWrapper->iPlaying)
				{
				aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,EFalse);
				}
			else
				{
				aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,ETrue);
				}
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::DynInitMenuPaneL - Exiting" );
	}



// ---------------------------------------------------------------------------
// Touch Toolbar
// Dynamically initialises a tool bar
// ---------------------------------------------------------------------------

void CIRSearchResultsView::ConstructToolbarL()
		{
		IRLOG_DEBUG( "CIRSearchResultsView::ConstructToolbarL - Entering" );
		SetToolbarItems();

		// Update rect, as changes in toolbar visibility also affect layout.
		if(iContainer)
			{
			iContainer->SetRect( ClientRect() );
			iContainer->DrawDeferred();
			}
			
		IRLOG_DEBUG( "CIRSearchResultsView::ConstructToolbarL - Exiting" );
        }


// --------------------------------------------------------------------------
// CIRSearchResultsView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CIRSearchResultsView::OfferToolbarEventL ( TInt aCommand )
  	{
  	IRLOG_DEBUG( "CIRSearchResultsView::OfferToolbarEventL - Entering" );
    HandleCommandL(aCommand);
    IRLOG_DEBUG( "CIRSearchResultsView::OfferToolbarEventL - Exiting" );
  	}

// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::HandleForegroundEventL( TBool aForeground )
    {
    IRLOG_DEBUG( "CIRSearchResultsView::HandleForegroundEventL - Entering" );
	CIRBaseView::HandleForegroundEventL( aForeground );
	if ( aForeground )
		{
		if ( iContainer )
			{
			MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
			TRgb color;
			TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
							                                      EAknsCIQsnTextColorsCG6 );
			if(color.Gray2()!=0)
				{
				CreateAndSetToolbarL(R_IR_SEARCHRESULTS_WHITE_TOOLBAR);
				}
			else
				{
				CreateAndSetToolbarL(R_IR_SEARCHRESULTS_BLACK_TOOLBAR);	
				}
			if( AknLayoutUtils::PenEnabled() && Toolbar() )
				{
				Toolbar()->SetToolbarObserver( this );
				}
			ConstructToolbarL();
			iContainer->DrawDeferred();
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::HandleForegroundEventL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRSearchResultsView::AddToFavouritesL()
// Adds the selected channel to favorites
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::AddToFavouritesL(TInt aCurrentIndex,TInt aCommand)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::AddToFavouritesL - Entering" );
	//request isds for preset data to be added to favorites
	iIsdsWrapper->IsdsPresetRequestL(this,aCurrentIndex,aCommand);
	IRLOG_DEBUG( "CIRSearchResultsView::AddToFavouritesL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRSearchResultsView::ResponseL()
// Activates the Stations view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::ResponseL(CIRIsdsPreset* /*aPreset*/)
	{
    IRLOG_DEBUG( "CIRSearchResultsView::ResponseL() started." );
    ResetPendingRequests(EFalse);
	if(iIsdsWrapper->iStationsData->Count())
		{
		if(iIsSearchAgain)
			{
			iIsSearchAgain = EFalse;
			if(iContainer)
				{
		        AppUi()->RemoveFromViewStack( *this, iContainer );
		        iContainer->MakeVisible( EFalse );
		        delete iContainer;
		        iContainer = NULL;
				}
	   		SetCurrentFocus(0);
			SetCurrentTopItem(0);
		    iContainer = CIRSearchResultsViewContainer::NewL(ClientRect(),*this);
		    iContainer->CreateListBoxL();
		    iContainer->ActivateL();
		    iContainer->MakeVisible( ETrue );
		    AppUi()->AddToViewStackL( *this, iContainer);
		    SetStatusPaneTextL();
	 		}
		}
	else
		{
		iIsdsWrapper->GetDialogLauncherInstance()->ShowInformationNoteL( R_IRAPP_RESULTS_NONE,ETrue );
		iIsdsWrapper->iError = EFalse;
		}
	IRLOG_DEBUG( "CIRSearchResultsView::ResponseL() exiting." );
	}

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::PresetResponseL()
// Called when Preset Data is received
// Used to Listen to a channel with given preset.
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::PresetResponseL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::PresetResponseL - Entering" );
	ResetPendingRequests(EFalse);
	if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
	if(!iNowPlayingWrapper->GetWhenUserCancelsIsdsBufferingWaitBar())
		{
		iNowPlayingWrapper->SetListenFromIsdsValue(ETrue);
		iNowPlayingWrapper->ListenToChannelL(aPreset);
		}
	IRLOG_DEBUG( "CIRSearchResultsView::PresetResponseL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::ErrorL()
// Handles error Conditions
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::ErrorL()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::ErrorL - Entering" );
	if(iIsdsWrapper->iDialogDismissed)
		{
		iIsdsWrapper->iDialogDismissed=EFalse;
		return;
		}
	if(iIsdsWrapper->GetListenRequest())
		{
		iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);
		iNowPlayingWrapper->DestroyWaitDialogL();
		iIsdsWrapper->SetListenRequest(EFalse);
		}

	if(iIsdsWrapper->iConnTimeOut)
		{
		iIsdsWrapper->iConnTimeOut = EFalse;
		}
	 if(iIsdsWrapper->iError)
		{
		if(iIsdsWrapper->GetDialogLauncherInstance())
			{
			iIsdsWrapper->GetDialogLauncherInstance()->ShowInformationNoteL( R_IRAPP_RESULTS_NONE,ETrue );
			iIsdsWrapper->iError = EFalse;
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::ErrorL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRSearchResultsView::ListenChannelL()
// Request for listening to a channel
// ---------------------------------------------------------------------------
//

void CIRSearchResultsView:: ListenChannelL(TInt aCurrentIndex)
 	{
	IRLOG_DEBUG( "CIRSearchResultsView::ListenChannelL - Entering" );
 	iNowPlayingWrapper->SetView(ESearch);
 	iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
 	iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
 	iNowPlayingWrapper->SetWhenUserCancelsIsdsBufferingWaitBar(EFalse);
 	iNowPlayingWrapper->CreateWaitDialogL();
	iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);

	iIsdsWrapper->IsdsListenRequestL(this,aCurrentIndex);
	IRLOG_DEBUG( "CIRSearchResultsView::ListenChannelL - Exiting" );
	}


void CIRSearchResultsView::ShowQueryDialogL(const TDesC& aTitle, TDes& aBuffer)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::ShowQueryDialogL - Entering" );
	CAknTextQueryDialog* Dialog =
	CAknTextQueryDialog::NewL(aBuffer,CAknQueryDialog::ENoTone);
	Dialog->PrepareLC(R_SEARCH_STRING_DIALOG);
	Dialog->SetPromptL(aTitle);
	//return Dialog->RunLD()
	iSearchValue = Dialog->RunLD();

	//Display a searching Progress Bar

	if(	iSearchValue)
		{
		iUi->iSearchString = aBuffer;
		if( iUi->VerifyNetworkConnectivityL() )
			{
			iRequestPending = ETrue;
			}
		else
			{
			iIsdsWrapper->IsdsSearchRequestL(this,aBuffer);
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::ShowQueryDialogL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRSearchResultsView::LogoRequestL()
// called from the container
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::LogoRequestL()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::LogoRequestL - Entering" );
	if(iContainer)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
		GetLogoDownloadEngine()->SendRequestL(iContainer->GetTempPreset(),this,
									KLogosReqFrom,KLogosSize,KLogosSize);
		}
	IRLOG_DEBUG( "CIRSearchResultsView::LogoRequestL - Exiting" );
	}



// ---------------------------------------------------------------------------
// CIRSearchResultsView::PresetLogoDownloadedL()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::PresetLogoDownloadedL - Entering" );
	if(iContainer)
		{
		iContainer->PresetLogoDownloadL(aPreset);
		}
	IRLOG_DEBUG( "CIRSearchResultsView::PresetLogoDownloadedL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRSearchResultsView::PresetLogoDownloadError()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRSearchResultsView::PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::PresetLogoDownloadError " );
	//call the API of search results view container
	if(iContainer)
		{
		TRAP_IGNORE(iContainer->HandleLogoErrorL());
		}
	IRLOG_DEBUG( "CIRSearchResultsView::PresetLogoDownloadErrorL - Exiting" );
	}


void CIRSearchResultsView::AdRequestL()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::AdRequestL - Entering" );
	if(iContainer)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
		GetLogoDownloadEngine()->SendRequestL(iContainer->GetAdvPreset(),this);
		}

	IRLOG_DEBUG( "CIRSearchResultsView::AdRequestL - Exiting" );
	}
// ---------------------------------------------------------
// CIRSearchResultsView::SetActualFilteredItemIndex ()
// Sets the listen is from favourites or not
// ---------------------------------------------------------
//
void CIRSearchResultsView::SetActualFilteredItemIndex(TInt aIndex)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::SetListBoxMarkedCount - Entering." );
	iIndex = aIndex;
	IRLOG_DEBUG( "CIRSearchResultsView::SetListBoxMarkedCount - Exiting." );
	}

// ---------------------------------------------------------
// CIRSearchResultsView::GetFilteredIndex ()
// returns the iFilteredIndex
// ---------------------------------------------------------
//
TInt CIRSearchResultsView::GetFilteredIndex()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::GetFilteredIndex - Entering." );
	IRLOG_DEBUG( "CIRSearchResultsView::GetFilteredIndex - Exiting." );
	return iFilteredIndex;
	}
// ---------------------------------------------------------
// CIRSearchResultsView::GetFirstLaunchFlag ()
// returns whether is launching for the first time /coming from other view
// which is activated from this view.
// ---------------------------------------------------------
//
TBool CIRSearchResultsView::GetFirstLaunchFlag()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::GetFirstLaunchFlag - Entering." );
	IRLOG_DEBUG( "CIRSearchResultsView::GetFirstLaunchFlag - Exiting." );
	return iFirstTimeLaunchFlag;
	}
// ---------------------------------------------------------------------------
// void CIRSearchResultsView::SetToolbarItems()
// Setting toolbar items when text is entered
//  ---------------------------------------------------------------------------
void CIRSearchResultsView:: SetToolbarItems()
	{
	IRLOG_DEBUG( "CIRSearchResultsView::SetToolbarItems - Entering" );
	if(Toolbar())
		{
		if(iContainer)
			{
			Toolbar()->SetToolbarVisibility(ETrue, EFalse);
			if(iContainer->GetSearchBoxPointer())
				{
				TInt currentIndex = iContainer->GetCurrentItemIndex();
				TBuf<KFindStringLength> findstring ;
				if(iContainer->GetSearchBoxPointer())
					{
					iContainer->GetSearchBoxPointer()->GetSearchText(findstring);
					}
				TInt filterModelCount = iContainer->GetFilterdModelPointer()->NumberOfItems();	
				if(iContainer->GetSearchBoxPointer() && findstring.Length()!=0 
				&& filterModelCount == 0)
					{
					Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,ETrue,ETrue);
					Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
					Toolbar()->SetItemDimmed(EAddStationManuallyCmd,ETrue,ETrue);
					}
				else
					{
					if (currentIndex == 0 && !iContainer->GetLogoNotSupported())  
						{
						if (iUi->iIsdsWrapper->GetBannerUrl())
							{
							Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,ETrue,ETrue);
							Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
							Toolbar()->SetItemDimmed(EAddStationManuallyCmd,ETrue,ETrue);
							}
						else
							{
							Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,EFalse,ETrue);
							Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
							Toolbar()->SetItemDimmed(EAddStationManuallyCmd,EFalse,ETrue);
							}
						}
					else
						{
						Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,EFalse,ETrue);
						Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
						Toolbar()->SetItemDimmed(EAddStationManuallyCmd,EFalse,ETrue);
						}
					}
				}
			Toolbar()->DrawDeferred();	
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::SetToolbarItems - Exiting" );
	}


// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRSearchResultsView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRSearchResultsView::NotifyActiveNetworkObserversL  - Entering." );
	IRRDEBUG2("CIRSearchResultsView::NotifyActiveNetworkObserversL entering ", KNullDesC);

	switch(aEvent)
		{
		case ENetworkConnectionDisconnected:
			{
			iNowPlayingWrapper->HandleNetworkDisconnected();
			}
			break;
		case ENetworkConnectionEstablished:
			{
			if( !iUi->CheckAndNotifyLowDiskSpaceL() && iContainer)
				{
				if(iRequestPending || iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iAddToFav)
						{
						iAddToFav = ETrue;
						AddToFavouritesL(GetActualIndex(),EAddtoFavouritesCmd);
						}
					else if(iListenRequest)
						{
						iListenRequest=EFalse;
						ListenChannelL(GetActualIndex());
						}
					else if(iSearchValue)
						{
						iSearchValue=ETrue;
						iIsdsWrapper->IsdsSearchRequestL(this,iUi->iSearchString);
						}
					else
						{
							
						}
					}
				if(iUi->iNetworkController->IsHandingOverConnection())
					{
					ResetPendingRequests(EFalse);			
					}

				}
			}
		}
   	
	IRRDEBUG2("CIRSearchResultsView::NotifyActiveNetworkObserversL exiting ", KNullDesC);
		
	IRLOG_DEBUG( "CIRSearchResultsView::NotifyActiveNetworkObserversL  - Exiting." );
	}
	
	
	
// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRSearchResultsView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	iListenRequest  = aValue;
	iAddToFav = aValue;	
	iSearchValue = aValue;	
	}
	
// ---------------------------------------------------------
// CIRSearchResultsView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRSearchResultsView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRSearchResultsView::HandleSystemEventL - Entering");
	

	switch (aEventType)
		{
		case EIRCallActivated:
			{
			if( iUi->GetNetworkControllerInstance()->IdentifyConnectionType() != EWiFi )
				{
				if(iUi->iIsdsWrapper->iIsdsWaitDialog)
					{
					TRAP_IGNORE( iUi->iIsdsWrapper->DestroyWaitDialogL() );
					}
				if((!iCallRequestPending && iCommand==ESearchCmd) ||
				  (!iCallRequestPending && iCommand==EAddtoFavouritesCmd))
					{
					iUi->iIsdsWrapper->IsdsCancelRequest();
					iCallRequestPending = ETrue;
					}
				if(iUi->iIsdsWrapper->GetListenRequest())
					{
					iNowPlayingWrapper->DestroyWaitDialogL();
					iUi->iIsdsWrapper->SetListenRequest(EFalse);
					iUi->iIsdsWrapper->IsdsCancelRequest();
					}
				}
			break;
			}
		case EIRCallDeactivated:
			{
			if(iCallRequestPending && iCommand==ESearchCmd)
				{
				iCallRequestPending = EFalse;
				if(iSearchValue)
					{	
					iIsdsWrapper->IsdsSearchRequestL(this,iUi->iSearchString);
					}
				}
			 else if(iCallRequestPending && iCommand==EAddtoFavouritesCmd)
				{
				iCallRequestPending = EFalse;
				HandleCommandL( iCommand );
				}
			break;
			}
		default:
			{
			break;
			}
		}
	IRLOG_DEBUG( "CIRSearchResultsView::HandleSystemEventL - Exiting." );
	}	
