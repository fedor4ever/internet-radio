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
* Description:  Stations view class
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 8, Thu Aug 18 14:32:00 2008 by Rohit
*  Ref:
*  Fixed bug : Tapping StationInfo toolbar-option for last station in Station list is causing crash
*  Modified GetStationsViewActualIndex() to remove increment of currentIndex.
*
*  Version: 8, Thurs July 18 12:00:00 2008 by Prabina
*  Ref:
*  Code Review Comments Fixed
*
*  Version: 7, May 28 wed 16:34:00 2008 by Rohit
*  Fixed bug to verify network before invoking Song Information view
*
*  Version: 6, Tues March 29 10:30:00 2008 by Prabina
*  Ref:
*  Stations View Container Derived from CCoeControl
*
*  Version: 5, Tues March 25 17:30:00 2008 by Prabina
*  Ref:
*  Fixing the PCLint errors
*
*  Version: 4, Mon March 24 18:00:00 2008 by Prabina
*  Ref:
*  Added WorkAround for adding different stations to favourites list , Added Wait dialog for adding to favourites
*
*  Version: 3, Fri March 21 17:20:00 2008 by Prabina
*  Ref:
*  Added WorkAround for supporting different Resolutions , Added Wait Dialog for Loading of stations ,
*  Handled Single/Double tapping on the list itmes
*
*  Version: 2, Thurs March 20 15:50:00 2008 by Prabina
*  Ref:
*  Created new views for the Station Directory Views , Enabled the Touch toolbar functionality ,
*  Help Options enabled
*
*  </ccm_history>
* ============================================================================
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
//For Touch Toolbar
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <aknbutton.h>
#include <aknwaitdialog.h>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irstationsview.h"
#include "irstationsviewcontainer.h"
#include "irapplication.h"
#include "irisdswrapper.h"
#include "irsettings.h"
#include "irmainview.h"
#include "irnowplayingwrapper.h"
#include "irstationinfoview.h"
#include "irimageconverter.h"
#include "irlogodownloadengine.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irnetworkcontroller.h"
#include "iractivenetworkobserver.h"
#include "irdialoglauncher.h"
#include "irfilteredmodel.h"
#include "irpubsub.h"

const TInt KImagesSize = 59;
const TInt KImageReqFrom = 0;

// ---------------------------------------------------------------------------
// CIRStationsView::CIRStationsView()
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRStationsView::CIRStationsView():iCallRequestPending(EFalse)
    {
	IRLOG_DEBUG( "CIRStationsView::CIRStationsView" );
    iResultsReqMade = EFalse;
	}


// ---------------------------------------------------------------------------
// void CIRStationsView::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRStationsView::ConstructL()
    {
	IRLOG_DEBUG( "CIRStationsView::ConstructL - Entering" );
    CIRBaseView::ConstructL( R_IR_STATIONS );
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
    TRgb color;
    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
    				                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_IR_STATIONSVIEW_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_IR_STATIONSVIEW_BLACK_TOOLBAR);	
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
    
    iFilteredIndex = 0;
    iFirstTimeLaunchFlag= EFalse;
    iRequestPending = EFalse;
    iAddToFav= EFalse;
    iStationDetails = EFalse;
	IRLOG_DEBUG( "CIRStationsView::ConstructL - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRStationsView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationsView* CIRStationsView::NewL()
    {
 	IRLOG_DEBUG( "CIRStationsView::NewL" );
    CIRStationsView* self = CIRStationsView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRStationsView::NewL - Exiting" );
    return self;
    }


// ---------------------------------------------------------------------------
// CIRStationsView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRStationsView* CIRStationsView::NewLC()
    {
	IRLOG_DEBUG( "CIRStationsView::NewLC - Entering" );
    CIRStationsView* self = new( ELeave ) CIRStationsView;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRStationsView::NewLC - Exiting" );
    return self;
	}


// ---------------------------------------------------------------------------
// CIRStationsView::~CIRStationsView()
// Destructor
// ---------------------------------------------------------------------------
//
CIRStationsView::~CIRStationsView()
    {
	IRLOG_DEBUG( "CIRStationsView::~CIRStationsView - Entering" );
	if ( iContainer )
		{
		AppUi()->RemoveFromViewStack( *this, iContainer );
		delete iContainer;
		iContainer = NULL;
		}
	if ( Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	iFilteredIndex = 0;
	IRLOG_DEBUG( "CIRStationsView::~CIRStationsView - Exiting" );
    }

// ---------------------------------------------------------------------------
// TUid CIRStationsView::Id() const
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRStationsView::Id() const
    {
    IRLOG_DEBUG( "CIRStationsView::Id" );
    return KIRStationsViewId;
    }

// ---------------------------------------------------------------------------
// void CIRStationsView::DoActivateL( )
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRStationsView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
	IRLOG_DEBUG( "CIRStationsView::DoActivateL - Entering" );

	TInt customMessageId = aCustomMessageId.iUid;
	if(customMessageId == 1)
		{
		SetCurrentFocus(0);
		SetCurrentTopItem(0);
		iFirstTimeLaunchFlag= ETrue;
		}
	aCustomMessageId.iUid = 0;
	iUi->iPresentViewID = KIRStationsViewId;
	
	////////////////////////////////////////////////////////////

	if(iResultsReqMade)
		{
		if(iContainer)
			{
		    delete iContainer;
		    iContainer = NULL;
			}
	    iResultsReqMade = EFalse;
		}
	
	////////////////////////////////////////////////////////////

	
	ConstructToolbarL();
    if ( !iContainer )
        {
 	    iContainer = CIRStationsViewContainer::NewL( *this, ClientRect() );
 	    iContainer->CreateListBoxL();
 		AppUi()->AddToViewStackL( *this, iContainer);
        }
    else
        {
		// we must notify a resource change in order to update the skin context
		// This is a flaw of CEikFormattedCellListBox
	//	_LIT(KText,"")
	//	iContainer->iStationsSearchBox->SetSearchTextL  (KText)
	//	iContainer->RefreshViewBack()
		
		iContainer->SetRect(ClientRect());
        iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
        AppUi()->AddToViewStackL( *this, iContainer);
 	//	iContainer->iItemArray->Reset()
	//	iContainer->UpdateStationsViewL()
       }


    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
  	SetStatusPaneTextL();
    CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    EnableObserverL(EIRSystemEventObserver);
	HandleStatusPaneSizeChange();
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
	IRLOG_DEBUG( "CIRStationsView::DoActivateL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationsView::DoDeactivate()
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRStationsView::DoDeactivate()
    {
	IRLOG_DEBUG( "CIRStationsView::DoDeactivate - Entering" );
    if(&(iUi->SystemEventCollector()))
	    {
		DisableObserver(EIRSystemEventObserver);
	    }
    if ( iContainer )
        {
        if(iContainer->iStationsListBox && iContainer->GetSearchBoxPointer())
		    {
			iContainer->GetSearchBoxPointer()->GetSearchText(iContainer->iFindString);
			if(iContainer->GetSearchBoxPointer() && iContainer->iFindString.Length()!=0)
				{
				if(iIndex)
					{
					SetCurrentFocus(iIndex);
					}
				else
					{
					iContainer->Comparestrings();
					SetCurrentFocus(iIndex);
					}
				}
			else
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				}
			SetCurrentTopItem(iContainer->GetListBoxTopIndex());
		    }
        AppUi()->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        }
 	if ( Toolbar() )
        {
        Toolbar()->SetToolbarVisibility(EFalse, EFalse);
        Toolbar()->SetToolbarObserver( NULL );
        }
    iFirstTimeLaunchFlag= EFalse;
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);
	IRLOG_DEBUG( "CIRStationsView::DoDeactivate - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRStationsView::HandleCommandL( TInt aCommand )
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRStationsView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRStationsView::HandleCommandL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	iCommand = aCommand;

	switch(aCommand)
		{
		//Activate NowPlaying view.
		case EGotoNowPlayingViewCmd:
			{
			appUi->SetHistory(EFalse);
			appUi->SavePrevIDL(KIRStationsViewId, aCommand);
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
				iListenRequest = EFalse;
				iStationDetails = EFalse;
				break;
				}
			else
				{
				iAddToFav = ETrue;
				AddToFavouritesL(GetStationsViewActualIndex(),aCommand);
				}
			}
		break;
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
				iAddToFav = EFalse;
				iStationDetails = EFalse;
				break;			
				}
			else
				{
				//SetCurrentFocus(GetStationsViewActualIndex())
				ListenChannelL(GetStationsViewActualIndex());	
				}
			}
		break;
		//Activate settings view.
		case ESettingsCmd:
			{
			AppUi()->ActivateLocalViewL(KIRSettingsViewId);
			}
		break;
		case EHelpCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
				}
			}
		break;
		case EStationDetailsCmd:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				iStationDetails = ETrue;
				iAddToFav = EFalse;
				iListenRequest = EFalse;
				break;
				}
			else
				{
				iStationDetails = ETrue;
				TInt currentIndex = GetStationsViewActualIndex();
	    		SetCurrentFocus(currentIndex);
				DisplayStationInformationL(currentIndex, aCommand);	
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
				if((iUi->iIsdsWrapper->iClickThroughUrl->Length())!=0)
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
		default:
			{
			CIRBaseView::HandleCommandL( aCommand );
			}
		break;
		}
	IRLOG_DEBUG( "CIRStationsView::HandleCommandL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRStationsView::SetStatusPaneTextL()
// Sets the title pane of the search results view
// ---------------------------------------------------------------------------
//

void CIRStationsView:: SetStatusPaneTextL() const
 	{
	IRLOG_DEBUG( "CIRStationsView::SetStatusPaneTextL - Entering" );
	static_cast<CAknTitlePane*>( StatusPane()->ControlL( 
	TUid::Uid( EEikStatusPaneUidTitle ) ) )->SetTextL( iUi->iStationsViewTitleString );
	IRLOG_DEBUG( "CIRStationsView::SetStatusPaneTextL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationsView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRStationsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRStationsView::DynInitMenuPaneL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_MENU_STATIONS)
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
	IRLOG_DEBUG( "CIRStationsView::DynInitMenuPaneL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationsView::ListenChannelL()
// Request for listening to a channel
// ---------------------------------------------------------------------------
//

void CIRStationsView:: ListenChannelL(TInt aCurrentIndex)
 	{
	IRLOG_DEBUG( "CIRStationsView::ListenChannelL - Entering" );
 	iNowPlayingWrapper->SetView(EStations);
 	iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
 	iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
 	iNowPlayingWrapper->SetWhenUserCancelsIsdsBufferingWaitBar(EFalse);
 	iNowPlayingWrapper->CreateWaitDialogL();
	iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);

	iIsdsWrapper->IsdsListenRequestL(this,aCurrentIndex);
	IRLOG_DEBUG( "CIRStationsView::ListenChannelL - Exiting" );
	}
// ---------------------------------------------------------------------------
// Touch Toolbar
//
// ---------------------------------------------------------------------------

void CIRStationsView::ConstructToolbarL()
    {
	IRLOG_DEBUG( "CIRStationsView::ConstructToolbarL - Entering" );
    SetToolbarItems();
    
	// Update rect, as changes in toolbar visibility also affect layout.
	if(iContainer)
		{
		iContainer->SetRect( ClientRect() );
		iContainer->DrawDeferred();
		}
		
	IRLOG_DEBUG( "CIRStationsView::ConstructToolbarL - Exiting" );
    }



// --------------------------------------------------------------------------
// CIRStationsView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CIRStationsView::OfferToolbarEventL ( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRStationsView::OfferToolbarEventL - Entering" );
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRStationsView::OfferToolbarEventL - Exiting" );
	}




// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRStationsView::HandleForegroundEventL( TBool aForeground )
    {
	IRLOG_DEBUG( "CIRStationsView::HandleForegroundEventL - Entering" );
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
				CreateAndSetToolbarL(R_IR_STATIONSVIEW_WHITE_TOOLBAR);
				}
			else
				{
				CreateAndSetToolbarL(R_IR_STATIONSVIEW_BLACK_TOOLBAR);	
				}
			if( AknLayoutUtils::PenEnabled() && Toolbar() )
				{
				Toolbar()->SetToolbarObserver( this );
				}
			ConstructToolbarL();
			iContainer->DrawDeferred();
			}
		}
	IRLOG_DEBUG( "CIRStationsView::HandleForegroundEventL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRStationsView::HandleStatusPaneSizeChange()
    {
	IRLOG_DEBUG( "CIRStationsView::HandleStatusPaneSizeChange - Entering" );
    CIRBaseView::HandleStatusPaneSizeChange();

    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
       }
	IRLOG_DEBUG( "CIRStationsView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRStationsView::AddToFavouritesL()
//
// ---------------------------------------------------------------------------
//
void CIRStationsView::AddToFavouritesL(TInt aCurrentIndex,TInt aCommand)
	{
	IRLOG_DEBUG( "CIRStationsView::AddToFavouritesL - Entering" );
	iLoadingCancelled = EFalse;

	//request isds for preset data to be added to favorites
	iIsdsWrapper->IsdsPresetRequestL(this,aCurrentIndex,aCommand);

	//Display a loading Progress Bar
	IRLOG_DEBUG( "CIRStationsView::AddToFavouritesL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationsView::DisplayStationInformation()
//
// ---------------------------------------------------------------------------
//
void CIRStationsView::DisplayStationInformationL(TInt aCurrentIndex,TInt aCommand)
	{
	IRLOG_DEBUG( "CIRStationsView::DisplayStationInformationL - Entering" );

	//request isds for preset data to be added to favorites
	iIsdsWrapper->IsdsPresetRequestL(this, aCurrentIndex, aCommand, EFalse);

	//Display a loading Progress Bar
	IRLOG_DEBUG( "CIRStationsView::DisplayStationInformationL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRStationsView::ResponseL()
// Activates the Stations view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRStationsView::ResponseL( CIRIsdsPreset* /*aPreset*/ )
	{
	IRLOG_DEBUG( "CIRStationsView::ResponseL - Entering" );
	//TO DO :: The line can be removed in future written to fix
	// Code Scanner ERROR
	ResetPendingRequests(EFalse);	
	IRLOG_DEBUG( "CIRStationsView::ResponseL - Exiting" );
	
	}

// ---------------------------------------------------------------------------
// void CIRStationsView::PresetResponseL()
// Issues Request to listen to a particular channel
// ---------------------------------------------------------------------------
//
void CIRStationsView::PresetResponseL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRStationsView::PresetResponseL - Entering" );
	if ( iUi->CheckAndNotifyLowDiskSpaceL() )
		{
		return;
		}
	
	if( EStationDetailsCmd == iCommand )
		{
		ResetPendingRequests(EFalse);
		iUi->iStationInfoView->SetStationPresetL( aPreset );
		AppUi()->ActivateLocalViewL(KIRStationInfoViewId);
		}
	else
		{
		if(!iNowPlayingWrapper->GetWhenUserCancelsIsdsBufferingWaitBar())
			{
			ResetPendingRequests(EFalse);
			iNowPlayingWrapper->SetListenFromIsdsValue(ETrue);
			iNowPlayingWrapper->ListenToChannelL(aPreset);
			}
		}
	IRLOG_DEBUG( "CIRStationsView::PresetResponseL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationsView::ErrorL()
// Handles the errors from Isds
// ---------------------------------------------------------------------------
//
void CIRStationsView::ErrorL()
	{
	IRLOG_DEBUG( "CIRStationsView::ErrorL - Entering" );
	if(iIsdsWrapper->GetListenRequest())
		{
 		IRRDEBUG2("CIRNowPlayingWrapper:: DestroyWaitDialog9", KNullDesC); 
 		iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);
		iNowPlayingWrapper->DestroyWaitDialogL();
		iIsdsWrapper->SetListenRequest(EFalse);
		}
	if(iIsdsWrapper->iConnTimeOut)
		{
		iIsdsWrapper->iConnTimeOut = EFalse;
		}
	IRLOG_DEBUG( "CIRStationsView::ErrorL - Exiting" );
	}



// ---------------------------------------------------------------------------
// CIRStationsView::LogoRequestL()
// called from the container
// ---------------------------------------------------------------------------
//
void CIRStationsView::LogoRequestL()
	{
	IRLOG_DEBUG( "CIRStationsView::LogoRequestL - Entering" );
	if(iContainer)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
				GetLogoDownloadEngine()->SendRequestL(iContainer->iTempPreset,this,
									KImageReqFrom,KImagesSize,KImagesSize);
		}
	IRLOG_DEBUG( "CIRStationsView::LogoRequestL - Exiting" );
	}



// ---------------------------------------------------------------------------
// CIRStationsView::PresetLogoDownloadedL()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRStationsView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRStationsView::PresetLogoDownloadedL - Entering" );
	if(iContainer)
		{
		iContainer->PresetLogoDownloadL(aPreset);
		}
	IRLOG_DEBUG( "CIRStationsView::PresetLogoDownloadedL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRStationsView::PresetLogoDownloadError()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRStationsView::PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRStationsView::PresetLogoDownloadError" );
	//call the API of stations view container
	if(iContainer)
		{
		TRAP_IGNORE(iContainer->HandleLogoErrorL())
		}
	IRLOG_DEBUG( "CIRStationsView::PresetLogoDownloadError - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRStationsView::LogoRequestL()
// called from the container				
// ---------------------------------------------------------------------------
//
void CIRStationsView::AdRequestL()
	{
	IRLOG_DEBUG( "CIRStationsView::LogoRequestL - Entering" );
	if(iContainer)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
		GetLogoDownloadEngine()->SendRequestL(iContainer->iAdvPreset,this);
		}
	IRLOG_DEBUG( "CIRStationsView::LogoRequestL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRStationsView::GetFilteredIndex()
// Returns the filtered Index				
// ---------------------------------------------------------------------------
//
TInt CIRStationsView::GetFilteredIndex()
	{
	IRLOG_DEBUG( "CIRStationsView::GetFilteredIndex - Entering" );
	return iFilteredIndex;
	}
// ---------------------------------------------------------------------------
// CIRStationsView::SetLoadingCancelled()
// Sets iLoadingCancelled to aValue				
// ---------------------------------------------------------------------------
//
void CIRStationsView::SetLoadingCancelled(TBool aValue)
	{
	IRLOG_DEBUG( "CIRStationsView::SetLoadingCancelled - Entering" );
	iLoadingCancelled = aValue;
	IRLOG_DEBUG( "CIRStationsView::SetLoadingCancelled - Exiting" );
	}

// ---------------------------------------------------------
// CIRStationsView::SetActualFilteredItemIndex ()
// Sets the actual index of the station
// ---------------------------------------------------------
//
void CIRStationsView::SetActualFilteredItemIndex(TInt aIndex)
	{
	IRLOG_DEBUG( "CIRStationsView::SetActualFilteredItemIndex - Entering." );
	iIndex = aIndex;
	IRLOG_DEBUG( "CIRStationsView::SetActualFilteredItemIndex - Exiting." );
	}
// ---------------------------------------------------------
// CIRStationsView::GetActualFilteredItemIndex ()
// Gets the actual index of the station
// ---------------------------------------------------------
//
TInt CIRStationsView::GetActualFilteredItemIndex()
	{
	IRLOG_DEBUG( "CIRStationsView::GetActualFilteredItemIndex - Entering." );
	IRLOG_DEBUG( "CIRStationsView::GetActualFilteredItemIndex - Exiting." );
	return iIndex;
	}
// ---------------------------------------------------------------------------
// CIRStationsView::GetStationsViewActualIndex().
// Returns the actual index.
// ---------------------------------------------------------------------------
//
TInt CIRStationsView::GetStationsViewActualIndex()
    {
 	IRLOG_DEBUG( "CIRStationsViewContainer::GetStationsViewActualIndex - Entering" );
   	TInt currentIndex = iContainer->GetCurrentItemIndex();
	SetCurrentFocus(currentIndex);
	TBuf<KFindStringLength> findstring ;
   if(iContainer->GetSearchBoxPointer())
	   {
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

	IRLOG_DEBUG( "CIRStationsViewContainer::GetStationsViewActualIndex - Exiting" );
	return currentIndex;
    }
// ---------------------------------------------------------
// CIRStationsView::GetFirstLaunchFlag ()
// returns whether is launching for the first time /coming from other view
// which is activated from this view.
// ---------------------------------------------------------
//
TBool CIRStationsView::GetFirstLaunchFlag()
	{
	IRLOG_DEBUG( "CIRStationsView::GetFirstLaunchFlag - Entering." );
	IRLOG_DEBUG( "CIRStationsView::GetFirstLaunchFlag - Exiting." );
	return iFirstTimeLaunchFlag;
	}
// ---------------------------------------------------------------------------
// void CIRStationsView::SetToolbarItems()
// Setting toolbar items when text is entered
//  ---------------------------------------------------------------------------
void CIRStationsView:: SetToolbarItems()
	{
	IRLOG_DEBUG( "CIRMainView::SetToolbarItems - Entering" );
	if(Toolbar())
		{
		if(iContainer)
			{
			Toolbar()->SetToolbarVisibility(ETrue, EFalse);
			if(iContainer->iStationsListBox)
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
					Toolbar()->SetItemDimmed(EStationDetailsCmd,ETrue,ETrue);
					}
				else
					{
					if (currentIndex == 0 && !iContainer->GetLogoNotSupported())  
						{
						if (iUi->iIsdsWrapper->GetBannerUrl())
							{
							Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,ETrue,ETrue);
							Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
							Toolbar()->SetItemDimmed(EStationDetailsCmd,ETrue,ETrue);
							}
						else
							{
							Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,EFalse,ETrue);
							Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
							Toolbar()->SetItemDimmed(EStationDetailsCmd,EFalse,ETrue);
							}
						}
					else
						{
						Toolbar()->SetItemDimmed(EAddtoFavouritesCmd,EFalse,ETrue);
						Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
						Toolbar()->SetItemDimmed(EStationDetailsCmd,EFalse,ETrue);
						}
					}
				Toolbar()->DrawNow();
				}
			}
		}
	IRLOG_DEBUG( "CIRMainView::SetToolbarItems - Exiting" );
	}

// ---------------------------------------------------------
// CIRStationsView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRStationsView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRNowPlayingView::::HandleSystemEventL - Entering");
	
	CIRUi* appUi = static_cast<CIRUi*>(AppUi());
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
				if((!iCallRequestPending && iCommand==EStationDetailsCmd) ||
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
			if((iCallRequestPending && iCommand==EStationDetailsCmd) ||
			  (iCallRequestPending && iCommand==EAddtoFavouritesCmd))
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
	IRLOG_DEBUG( "CIRNowPlayingView::HandleSystemEventL - Exiting." );
	}	


// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRStationsView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRStationsView::NotifyActiveNetworkObserversL  - Entering." );
	IRRDEBUG2("CIRStationsView::NotifyActiveNetworkObserversL entering ", KNullDesC);

	switch(aEvent)
		{
		case ENetworkConnectionDisconnected:
			{
			iNowPlayingWrapper->HandleNetworkDisconnected();
			}
			break;
		case ENetworkConnectionEstablished :
			{
		
			if(iContainer)
				{
				if(iRequestPending || iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iAddToFav)
						{
						iAddToFav = ETrue;
						iListenRequest = EFalse;
						iStationDetails = EFalse;
						AddToFavouritesL(GetStationsViewActualIndex(),EAddtoFavouritesCmd);
						}
					else if (iStationDetails)
						{
						iStationDetails = ETrue;
						iAddToFav = EFalse;
						iListenRequest = EFalse;
						TInt currentIndex = GetStationsViewActualIndex();
			    		SetCurrentFocus(currentIndex);
						DisplayStationInformationL(currentIndex, EStationDetailsCmd);	
						}
					else if(iListenRequest)
						{
						iListenRequest = EFalse;
						SetCurrentFocus(GetStationsViewActualIndex());
						ListenChannelL(GetStationsViewActualIndex());	
						}
					else
						{
							
						}
					}
				}
			if(iUi->iNetworkController->IsHandingOverConnection())
			ResetPendingRequests(EFalse);				
			}
		}
		
	IRLOG_DEBUG( "CIRStationsView::NotifyActiveNetworkObserversL  - Exiting." );
	IRRDEBUG2("CIRStationsView::NotifyActiveNetworkObserversL exiting ", KNullDesC);

	}
	
	
	
// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRStationsView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	iAddToFav  = aValue;
	iListenRequest  = aValue;
	iStationDetails  = aValue;
	
	}	
