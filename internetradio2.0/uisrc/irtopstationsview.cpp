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
* Description:  TopStations view class
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
*  Version: 7, Thurs July 18 12:00:00 2008 by Prabina
*  Ref:
*  Code Review Comments Fixed
*
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
#include <aknwaitdialog.h> 
//For Touch Toolbar
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <aknbutton.h>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irtopstationsview.h"
#include "irstationsviewcontainer.h"
#include "irapplication.h"
#include "irisdswrapper.h"
#include "irsettings.h"
#include "irnowplayingwrapper.h"
#include "irfilteredmodel.h"
#include "irpubsub.h"

#include "irimageconverter.h"

#include "irnetworkcontroller.h"
#include "iractivenetworkobserver.h"

// ---------------------------------------------------------------------------
// CIRTopStationsView::CIRTopStationsView() 
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRTopStationsView::CIRTopStationsView():iCallRequestPending(EFalse) 
    {
	IRLOG_DEBUG( "CIRTopStationsView::CIRTopStationsView" );
    iResultsReqMade = EFalse;
	}


// ---------------------------------------------------------------------------
// void CIRTopStationsView::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::ConstructL()
    {
	IRLOG_DEBUG( "CIRTopStationsView::ConstructL - Entering" );
    CIRStationsView::ConstructL();
    
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
    TRgb color;
    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
    				                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_IR_TOPSTATIONS_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_IR_TOPSTATIONS_BLACK_TOOLBAR);	
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
    
    iTopStationsFilteredIndex = 0;
    iFirstTimeLaunchFlag = EFalse;
	IRLOG_DEBUG( "CIRTopStationsView::ConstructL - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRTopStationsView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRTopStationsView* CIRTopStationsView::NewL()
    {
 	IRLOG_DEBUG( "CIRTopStationsView::NewL" );
    CIRTopStationsView* self = CIRTopStationsView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRTopStationsView::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRTopStationsView* CIRTopStationsView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRTopStationsView* CIRTopStationsView::NewLC()
    {
	IRLOG_DEBUG( "CIRTopStationsView::NewLC - Entering" );
    CIRTopStationsView* self = new( ELeave ) CIRTopStationsView;
    CleanupStack::PushL( self );
    self->ConstructL();
	IRLOG_DEBUG( "CIRTopStationsView::NewLC - Exiting" );
    return self;
    }

    
// ---------------------------------------------------------------------------
// CIRTopStationsView::~CIRTopStationsView()
// Destructor
// ---------------------------------------------------------------------------
//
CIRTopStationsView::~CIRTopStationsView()
    {
    IRLOG_DEBUG( "CIRTopStationsView::~CIRTopStationsView " );
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
	iTopStationsFilteredIndex = 0;
	//calls base class destructor
	IRLOG_DEBUG( "CIRTopStationsView::~CIRTopStationsView - Exiting" );
	}

// ---------------------------------------------------------------------------
// TUid CIRTopStationsView::Id() const
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRTopStationsView::Id() const
    {
    IRLOG_DEBUG( "CIRTopStationsView::Id" );
    return KIRTopStationsViewId;
    }

// ---------------------------------------------------------------------------
// void CIRTopStationsView::DoActivateL( )
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
			 const TDesC8& aCustomMessage )
    {
	IRLOG_DEBUG( "CIRTopStationsView::DoActivateL - Entering" );
    
	TInt customMessageId = aCustomMessageId.iUid;
	if(customMessageId == 1)
		{
		SetCurrentFocus(0);	
		SetCurrentTopItem(0);	
		iFirstTimeLaunchFlag = ETrue;
		}
	aCustomMessageId.iUid = 0;
	iUi->iPresentViewID = KIRTopStationsViewId;		
	
	
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
  	SetTitleL( R_IRAPP_STAT_DIR_TOP );
    CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
	
	
	
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
		iContainer->SetRect(ClientRect());
        iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
        AppUi()->AddToViewStackL( *this, iContainer);
        }


    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
	HandleStatusPaneSizeChange();
    EnableObserverL(EIRSystemEventObserver);
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);

	IRLOG_DEBUG( "CIRTopStationsView::DoActivateL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRTopStationsView::DoDeactivate()
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::DoDeactivate()
    {
    IRLOG_DEBUG( "CIRTopStationsView::DoDeactivate - Entering" );
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
				if(GetActualFilteredItemIndex())
					{
					SetCurrentFocus(GetActualFilteredItemIndex());
					}
				else
					{
					iContainer->Comparestrings();
					SetCurrentFocus(GetActualFilteredItemIndex());
					}
				}
			else
				{
				TInt currentIndex = iContainer->GetCurrentItemIndex();
				SetCurrentFocus(currentIndex);
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
	iFirstTimeLaunchFlag = EFalse;
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);

	IRLOG_DEBUG( "CIRTopStationsView::DoDeactivate - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRTopStationsView::HandleCommandL( TInt aCommand )
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRTopStationsView::HandleCommandL  - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	iCommand = aCommand;

	switch(aCommand)
		{
		//Activate NowPlaying view.	
		case EGotoNowPlayingViewCmd:
			{
			appUi->SetHistory(EFalse);
			appUi->SavePrevIDL(KIRTopStationsViewId, aCommand); 			
			}
		break;
		case EAddStationManuallyCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				appUi->SetEditStation(EFalse); 
				SetCurrentFocus(GetTopStationsActualIndex());
			    AppUi()->ActivateLocalViewL(KIRAddManuallyStationViewID);
				}
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
				AddToFavouritesL(GetTopStationsActualIndex(),aCommand);
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
				break;
				}
			else
				{
				ListenChannelL(GetTopStationsActualIndex());
				}
			}
		break;
		default:
			{
			CIRStationsView::HandleCommandL( aCommand );
			}
		break;
		}
	IRLOG_DEBUG( "CIRTopStationsView::HandleCommandL - Exiting" );
	}
 	
// ---------------------------------------------------------------------------
// void CIRTopStationsView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
	
    
void CIRTopStationsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRTopStationsView::DynInitMenuPaneL - Entering" );
	CIRStationsView::DynInitMenuPaneL(aResourceId,aMenuPane);
	IRLOG_DEBUG( "CIRTopStationsView::DynInitMenuPaneL - Exiting" );
	}

// ---------------------------------------------------------------------------
// Touch Toolbar
//
// ---------------------------------------------------------------------------

void CIRTopStationsView::ConstructToolbarL()
	{
	IRLOG_DEBUG( "CIRTopStationsView::ConstructToolbarL - Entering" );

    SetToolbarItems();
    
	// Update rect, as changes in toolbar visibility also affect layout.
	if(iContainer)
		{
		iContainer->SetRect( ClientRect() );
		iContainer->DrawDeferred();
		}
	IRLOG_DEBUG( "CIRTopStationsView::ConstructToolbarL - Exiting" );

	}

// ---------------------------------------------------------------------------
// void CIRTopStationsView::ListenChannelL()
// Request for listening to a channel
// ---------------------------------------------------------------------------
//
    
void CIRTopStationsView:: ListenChannelL(TInt aCurrentIndex)
 	{
	IRLOG_DEBUG( "CIRTopStationsView::ListenChannelL - Entering" );
	iNowPlayingWrapper->SetView(EBrowseByTopStations);
	iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
 	iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
 	iNowPlayingWrapper->SetWhenUserCancelsIsdsBufferingWaitBar(EFalse);
 	iNowPlayingWrapper->CreateWaitDialogL();
 	iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);
	iIsdsWrapper->IsdsListenRequestL(this,aCurrentIndex);
	IRLOG_DEBUG( "CIRTopStationsView::ListenChannelL - Exiting" );
	}
// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::HandleStatusPaneSizeChange()
    {
	IRLOG_DEBUG( "CIRTopStationsView::HandleStatusPaneSizeChange - Entering" );
    CIRBaseView::HandleStatusPaneSizeChange();

    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
	IRLOG_DEBUG( "CIRTopStationsView::HandleStatusPaneSizeChange - Exiting" );
    }
// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::HandleForegroundEventL( TBool aForeground )
    {
	IRLOG_DEBUG( "CIRTopStationsView::HandleForegroundEventL - Entering" );
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
				CreateAndSetToolbarL(R_IR_TOPSTATIONS_WHITE_TOOLBAR);
				}
			else
				{
				CreateAndSetToolbarL(R_IR_TOPSTATIONS_BLACK_TOOLBAR);	
				}
			if( AknLayoutUtils::PenEnabled() && Toolbar() )
				{
				Toolbar()->SetToolbarObserver( this );
				}

			ConstructToolbarL();
			iContainer->DrawDeferred();
			}
		}
	IRLOG_DEBUG( "CIRTopStationsView::HandleForegroundEventL - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRTopStationsView::AddToFavouritesL()
// 
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::AddToFavouritesL(TInt aCurrentIndex,TInt aCommand)
	{
	IRLOG_DEBUG( "CIRTopStationsView::AddToFavouritesL - Entering" );
	SetLoadingCancelled(EFalse);

	//request isds for preset data to be added to favorites
	iIsdsWrapper->IsdsPresetRequestL(this,aCurrentIndex,aCommand);
	
	IRLOG_DEBUG( "CIRTopStationsView::AddToFavouritesL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRTopStationsView::ResponseL()
// Activates the Stations view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::ResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRTopStationsView::ResponseL - Entering" );
	//TO DO :: The line can be removed in future written to fix
	// Code Scanner ERROR
	ResetPendingRequests(EFalse);
	IRLOG_DEBUG( "CIRTopStationsView::ResponseL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRTopStationsView::PresetResponseL()
// Issues Request to listen to a particular channel 
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::PresetResponseL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRTopStationsView::PresetResponseL - Entering" );
	if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
		{
		ResetPendingRequests(EFalse);
		if(!iNowPlayingWrapper->GetWhenUserCancelsIsdsBufferingWaitBar())
			{
			iNowPlayingWrapper->SetListenFromIsdsValue(ETrue);
			iNowPlayingWrapper->ListenToChannelL(aPreset);
			}
		}
	IRLOG_DEBUG( "CIRTopStationsView::PresetResponseL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRTopStationsView::ErrorL()
// Handles the errors from Isds 
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::ErrorL()
	{
	IRLOG_DEBUG( "CIRTopStationsView::ErrorL - Entering" );
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
	IRLOG_DEBUG( "CIRTopStationsView::ErrorL - Exiting" );
	}


// --------------------------------------------------------------------------
// CIRTopStationsView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CIRTopStationsView::OfferToolbarEventL ( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRTopStationsView::OfferToolbarEventL - Entering" );
    HandleCommandL(aCommand);
    IRLOG_DEBUG( "CIRTopStationsView::OfferToolbarEventL - Exiting" );
   	}
	

// ---------------------------------------------------------------------------
// CIRTopStationsView::PresetLogoDownloadedL()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRTopStationsView::PresetLogoDownloadedL - Entering" );
	if(iContainer)
		{
		iContainer->PresetLogoDownloadL(aPreset);
		}
	IRLOG_DEBUG( "CIRTopStationsView::PresetLogoDownloadedL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRTopStationsView::PresetLogoDownloadError()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRTopStationsView::PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRTopStationsView::PresetLogoDownloadError ---Entering" );
	//call the API of stations view container
	if(iContainer)
		{
		TRAP_IGNORE(iContainer->HandleLogoErrorL())
		}
	IRLOG_DEBUG( "CIRTopStationsView::PresetLogoDownloadError --- Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRTopStationsView::GetFilteredIndex()
// Returns the filtered Index				
// ---------------------------------------------------------------------------
//
TInt CIRTopStationsView::GetFilteredIndex()
	{
	IRLOG_DEBUG( "CIRTopStationsView::GetFilteredIndex - Entering" );
	return iTopStationsFilteredIndex;
	}

// ---------------------------------------------------------------------------
// CIRTopStationsView::GetTopStationsActualIndex().
// Returns the actual index.
// ---------------------------------------------------------------------------
//
TInt CIRTopStationsView::GetTopStationsActualIndex()
    {
 	IRLOG_DEBUG( "CIRTopStationsView::GetTopStationsActualIndex - Entering" );
   	TInt currentIndex = iContainer->GetCurrentItemIndex();
	SetCurrentFocus(currentIndex);
	TBuf<KFindStringLength> findstring ;
   if(iContainer->GetSearchBoxPointer())
	   {
    	iContainer->GetSearchBoxPointer()->GetSearchText(findstring);
	   }
	if(iContainer->GetSearchBoxPointer() && findstring.Length()!=0)
		{
		iTopStationsFilteredIndex = currentIndex;
		iContainer->Comparestrings();
		currentIndex = GetActualFilteredItemIndex();
		}
	if(iContainer->GetLogoNotSupported())
		{
		currentIndex = 	currentIndex +1;
		}

	IRLOG_DEBUG( "CIRTopStationsView::GetTopStationsActualIndex - Exiting" );
	return currentIndex;
    }
// ---------------------------------------------------------
// CIRTopStationsView::GetFirstLaunchFlag ()
// returns whether is launching for the first time /coming from other view
// which is activated from this view.
// ---------------------------------------------------------
//
TBool CIRTopStationsView::GetFirstLaunchFlag()
	{
	IRLOG_DEBUG( "CIRTopStationsView::GetFirstLaunchFlag - Entering." );
	IRLOG_DEBUG( "CIRTopStationsView::GetFirstLaunchFlag - Exiting." );
	return iFirstTimeLaunchFlag;
	}
// ---------------------------------------------------------------------------
// void CIRTopStationsView::SetToolbarItems()
// Setting toolbar items when text is entered
//  ---------------------------------------------------------------------------
void CIRTopStationsView:: SetToolbarItems()
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
					Toolbar()->SetItemDimmed(EAddStationManuallyCmd,ETrue,ETrue);
					}
				else
					{
					if (currentIndex == 0  && !iContainer->GetLogoNotSupported())  
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
				Toolbar()->DrawNow();
				}
			}
		}
	IRLOG_DEBUG( "CIRMainView::SetToolbarItems - Exiting" );
	}
// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRTopStationsView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRTopStationsView::NotifyActiveNetworkObserversL  - Entering." );
	IRRDEBUG2("CIRTopStationsView::NotifyActiveNetworkObserversL entering ", KNullDesC);

	switch(aEvent)
		{
		case ENetworkConnectionDisconnected:
			{
			iNowPlayingWrapper->HandleNetworkDisconnected();
			}
			break;
		case ENetworkConnectionEstablished:
			{
		
			if( !iUi->CheckAndNotifyLowDiskSpaceL() && iContainer )
				{
				if(iRequestPending || iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iAddToFav)
						{
						iAddToFav=ETrue;
						AddToFavouritesL(GetTopStationsActualIndex(),EAddtoFavouritesCmd);
						}
					else if(iListenRequest)
						{
						iListenRequest=EFalse;
						ListenChannelL(GetTopStationsActualIndex());
						}
					else
						{
							
						}
					}
				}
			if(iUi->iNetworkController->IsHandingOverConnection())
				{
				ResetPendingRequests(EFalse);				
				}
			}
		}
   	
	IRRDEBUG2("CIRTopStationsView::NotifyActiveNetworkObserversL exiting ", KNullDesC);
	
	IRLOG_DEBUG( "CIRTopStationsView::NotifyActiveNetworkObserversL  - Exiting." );
	}
	
	
	
// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRTopStationsView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	iAddToFav  = aValue;
	iListenRequest  = aValue;
	}	
	
// ---------------------------------------------------------
// CIRTopStationsView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRTopStationsView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRTopStationsView::HandleSystemEventL - Entering");
	
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
				if(!iCallRequestPending && iCommand==EAddtoFavouritesCmd)
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
			if(iCallRequestPending && iCommand==EAddtoFavouritesCmd)
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
	IRLOG_DEBUG( "CIRTopStationsView::HandleSystemEventL - Exiting." );
	}
