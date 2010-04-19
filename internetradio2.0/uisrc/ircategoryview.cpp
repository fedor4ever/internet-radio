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
* Description:  Category view class
*
*/

/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 9, Thu Aug 18 14:32:00 2008 by Rohit
*  Ref:
*  Fixed bug : Stations view is displaying correst results but previous genre/lang label
*  Modified ResponseL() to remove extra decrement of currentIndex.
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
*  Version: 5, Thurs July 18 12:00:00 2008 by Prabina
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
 

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "ircategoryview.h"
#include "ircategoryviewcontainer.h"
#include "irstationsview.h"
#include "irapplication.h"
#include "irisdswrapper.h"
#include "irsettings.h"
#include "irnowplayingwrapper.h"
#include "irimageconverter.h"
#include "irlogodownloadengine.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irnetworkcontroller.h"
#include "irdialoglauncher.h"

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIRCategoryView::CIRCategoryView() 
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRCategoryView::CIRCategoryView():iCallRequestPending(EFalse) 
    {
	IRLOG_DEBUG( "CIRCategoryView::CIRCategoryView" );
    iCategoryReqMade = EFalse;
	}


// ---------------------------------------------------------------------------
// void CIRCategoryView::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRCategoryView::ConstructL()
    {
	IRLOG_DEBUG( "CIRCategoryView::ConstructL - Entering" );
    CIRBaseView::ConstructL( R_IR_CATEGORY );
	SetCurrentFocus(0);
	iFilteredIndex = 0;
    iFirstTimeLaunchFlag= EFalse;
   	iRequestPending = EFalse;

	IRLOG_DEBUG( "CIRCategoryView::ConstructL - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRCategoryView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRCategoryView* CIRCategoryView::NewL()
    {
 	IRLOG_DEBUG( "CIRCategoryView::NewL" );
    CIRCategoryView* self = CIRCategoryView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRCategoryView::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRCategoryView* CIRCategoryView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRCategoryView* CIRCategoryView::NewLC()
    {
	IRLOG_DEBUG( "CIRCategoryView::NewLC - Entering" );
    CIRCategoryView* self = new( ELeave ) CIRCategoryView;
    CleanupStack::PushL( self );
    self->ConstructL();
	IRLOG_DEBUG( "CIRCategoryView::NewLC - Exiting" );
    return self;
    }

    
// ---------------------------------------------------------------------------
// CIRCategoryView::~CIRCategoryView()
// Destructor
// ---------------------------------------------------------------------------
//
CIRCategoryView::~CIRCategoryView()
    {
	IRLOG_DEBUG( "CIRCategoryView::~CIRCategoryView - Entering" );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    iFilteredIndex = 0;
	IRLOG_DEBUG( "CIRCategoryView::~CIRCategoryView - Exiting" );
    }

// ---------------------------------------------------------------------------
// TUid CIRCategoryView::Id() const
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRCategoryView::Id() const
    {
    IRLOG_DEBUG( "CIRCategoryView::Id" );
    return KIRCategoryViewId;
    }

// ---------------------------------------------------------------------------
// void CIRCategoryView::DoActivateL( )
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRCategoryView::DoActivateL( const TVwsViewId& aPrevViewId, 
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
	IRLOG_DEBUG( "CIRCategoryView::DoActivateL - Entering" );
	TInt customMessageId = aCustomMessageId.iUid;
	if(customMessageId == 1)
		{
		SetCurrentFocus(0);
		SetCurrentTopItem(0);	
		iFirstTimeLaunchFlag= ETrue;
		}
	aCustomMessageId.iUid = 0;
	
	////////////////////////////////////////////////////////////

	if(iCategoryReqMade)
		{
		if(iContainer)
			{
		    delete iContainer;
		    iContainer = NULL;
			}
	    iCategoryReqMade = EFalse;
		}
	
	////////////////////////////////////////////////////////////
	
    if ( !iContainer )
        {
 	    iContainer = CIRCategoryViewContainer::NewL( *this, ClientRect() );
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
  	SetStatusPaneTextL();
    CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    HandleStatusPaneSizeChange();
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);

    EnableObserverL(EIRSystemEventObserver);
	IRLOG_DEBUG( "CIRCategoryView::DoActivateL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRCategoryView::DoDeactivate()
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRCategoryView::DoDeactivate()
    {
	IRLOG_DEBUG( "CIRCategoryView::DoDeactivate - Entering" );
    if(&(iUi->SystemEventCollector()))
	    {
		DisableObserver(EIRSystemEventObserver);
	    }
    if ( iContainer )
        {
        if(iContainer->iCategoryListBox && iContainer->iSearchTextBox)
	        {
			iContainer->iSearchTextBox->GetSearchText(iContainer->iFindString);
			if(iContainer->iSearchTextBox && iContainer->iFindString.Length()!=0)
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
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);
        
    iFirstTimeLaunchFlag= EFalse;
	IRLOG_DEBUG( "CIRCategoryView::DoDeactivate - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRCategoryView::HandleCommandL( TInt aCommand )
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRCategoryView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRCategoryView::HandleCommandL - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	iCommand = aCommand;
	switch(aCommand)
		{
		case ESelectCmd:
			{
			if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				break;			
				}
			else if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iRequestIssued = ETrue;	
				TInt currentIndex = iContainer->GetCurrentItemIndex();
				if (currentIndex == 0 )
					{
					if (iUi->iIsdsWrapper->iCatBannerUrl && !iContainer->GetLogoNotSupported())
						{
						if(iUi->iIsdsWrapper->iCatClickThroughUrl)
							{
							if(iUi->iIsdsWrapper->iCatClickThroughUrl->Length() != 0)
								{
								iContainer->OpenUrlL(*iUi->iIsdsWrapper->iCatClickThroughUrl);
								}
							}
						else
							{
							iUi->iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL
												(R_IRAPP_ERROR_URL_UNAVAILABLE,ETrue);
							}

						}
					else
						{
						DoChannelRequestL();			
						}
					}
				else
					{
					DoChannelRequestL();		
					}
				}	

			}
		break;
		//Activate NowPlaying view.	
		case EGotoNowPlayingViewCmd:
			{ 
			appUi->SetHistory(EFalse);
			appUi->SavePrevIDL(KIRCategoryViewId, aCommand); 			
			}
		break;

	//Activate settings view.	
		case ESettingsCmd:
			{
			AppUi()->ActivateLocalViewL(KIRSettingsViewId);
			}
			break;
		case EAdvertisementUrl:
			{
			if( iUi->VerifyNetworkConnectivityL() )
				{
				break;
				}
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				if (iUi->iIsdsWrapper->iCatBannerUrl && !iContainer->GetLogoNotSupported())
					{
					if(iUi->iIsdsWrapper->iCatClickThroughUrl)
						{
						if(iUi->iIsdsWrapper->iCatClickThroughUrl->Length() != 0)
							{
							iContainer->OpenUrlL(*iUi->iIsdsWrapper->iCatClickThroughUrl);
							}
						}
					else
						{
						iUi->iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL
							(R_IRAPP_ERROR_URL_UNAVAILABLE,ETrue);
						}
					
					}
				}
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
		default:
			{
			CIRBaseView::HandleCommandL( aCommand );
			}
		}
	IRLOG_DEBUG( "CIRCategoryView::HandleCommandL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRCategoryView::SetStatusPaneTextL()
// Sets the title pane of the search results view
// ---------------------------------------------------------------------------
//
    
void CIRCategoryView:: SetStatusPaneTextL()
 	{
	IRLOG_DEBUG( "CIRCategoryView::SetStatusPaneTextL - Entering" );
	switch (iUi->iCategoryViewTitleType)
		{
		// if the browse by genre's is selected.
		case EGenreData:
			{
			SetTitleL( R_IRAPP_GENRE_STATLIST_TITLE );
			}
			break;
		// if the browse by language's is selected.	
		case ELanguageData:
			{
			SetTitleL( R_IRAPP_LANGUAGE_STATLIST_TITLE );
			}
			break;
		// if the browse by country is selected.	
		case ECountryData:
			{
			SetTitleL( R_IRAPP_COUNTRY_STATLIST_TITLE );
			}
			break;
		// if top station's is selected.	
		case ETopStationData:
			{
			SetTitleL( R_IRAPP_TOP_STATLIST_TITLE );
			}
			break;
		default:
			{
			//No implementation
			}
		}
	IRLOG_DEBUG( "CIRCategoryView::SetStatusPaneTextL - Exiting" );
	}
 	
    
// ---------------------------------------------------------------------------
// CIRCategoryView::DoChannelRequestL()
// 
// ---------------------------------------------------------------------------
//
void CIRCategoryView::DoChannelRequestL()
	{
	IRLOG_DEBUG( "CIRCategoryView::DoChannelRequestL - Entering" );
	iLoadingCancelled = EFalse;
	TInt currentIndex  = iContainer->GetCurrentItemIndex();
	
	if(iContainer->iSearchTextBox)
		iContainer->iSearchTextBox->GetSearchText(iContainer->iFindString);
	if(iContainer->iSearchTextBox && iContainer->iFindString.Length()!=0)
		{
		iFilteredIndex = currentIndex;
		iContainer->Comparestrings();
		currentIndex = iIndex;
		SetCurrentFocus(currentIndex);
		if(iContainer->GetLogoNotSupported())
			{
			currentIndex = 	currentIndex +1;
			}
		iIsdsWrapper->IsdsChannelRequestL(this,currentIndex);
		iUi->iStationsView->iResultsReqMade = ETrue;
		}
	else
		{
		if(iContainer->GetLogoNotSupported())
			{
			currentIndex = 	currentIndex +1;
			}
		iIsdsWrapper->IsdsChannelRequestL(this,currentIndex);
		iUi->iStationsView->iResultsReqMade = ETrue;
		}
	IRLOG_DEBUG( "CIRCategoryView::DoChannelRequestL - Exiting" );

	}

// ---------------------------------------------------------------------------
// void CIRCategoryView::ResponseL()
// Activates the Stations view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRCategoryView::ResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRCategoryView::ResponseL - Entering" );
	iRequestIssued = EFalse;	
	CIRUi* appUi = static_cast<	CIRUi*>(AppUi());
	if(!iLoadingCancelled )
		{
		
		iUi->iStationsViewTitleString.Copy(KNullDesC);	
		iUi->iStationsViewTitleString.Close();
		// Gets the item text if the conditions above are met.
		if(iContainer->iSearchTextBox && iContainer->iFindString.Length()!=0)
			{
			TPtrC16 currentItemText = iContainer->iItemArray->MdcaPoint(iIndex);//iContainer->GetCurrentItemIndex()

			// We have to find the position of tabs to decide the components
			// available in the item text (icon, heading and text).
			TInt tabPosition1 = currentItemText.Locate('\t');
			TInt tabPosition2 = currentItemText.Mid(tabPosition1 + 1).Locate('\t');


			TInt itemDataLength = tabPosition2 - tabPosition1;
			iUi->iStationsViewTitleString.Create(itemDataLength);
			iUi->iStationsViewTitleString.Copy(currentItemText.Mid((tabPosition1+1),itemDataLength));
			}
		else
			{
			TInt currentIndex = iContainer->GetCurrentItemIndex();

			TPtrC16 currentItemText = iContainer->iItemArray->MdcaPoint(currentIndex);

			// We have to find the position of tabs to decide the components
			// available in the item text (icon, heading and text).
			TInt tabPosition1 = currentItemText.Locate('\t');
			TInt tabPosition2 = currentItemText.Mid(tabPosition1 + 1).Locate('\t');


			TInt itemDataLength = tabPosition2 - tabPosition1;
			iUi->iStationsViewTitleString.Create(itemDataLength);
			iUi->iStationsViewTitleString.Copy(currentItemText.Mid((tabPosition1+1),itemDataLength));
			}
		    TBuf8<2> dummy;
			TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRStationsViewId);
		
		
		if(appUi->IsForeground())
			{
			AppUi()->ActivateViewL(viewId ,TUid::Uid(1),dummy);
			appUi->iActivationPending = KNullUid ;	
			}
		else
			{
			appUi->iActivationPending =KIRStationsViewId;	
			}
	
		}
		
	IRLOG_DEBUG( "CIRCategoryView::ResponseL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRCategoryView::PresetResponseL()
// Issues Request to listen to a particular channel 
// ---------------------------------------------------------------------------
//
void CIRCategoryView::PresetResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRCategoryView::PresetResponseL - Entering" );
	//No Implementation Required
	IRLOG_DEBUG( "CIRCategoryView::PresetResponseL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRCategoryView::ErrorL()
// Handles the errors from Isds 
// ---------------------------------------------------------------------------
//
void CIRCategoryView::ErrorL()
	{
	IRLOG_DEBUG( "CIRCategoryView::ErrorL - Entering" );
	if(iIsdsWrapper->iConnTimeOut)
		{
		iIsdsWrapper->iConnTimeOut = EFalse;	
		}
	IRLOG_DEBUG( "CIRCategoryView::ErrorL - Exiting" );

	}

// ---------------------------------------------------------------------------
// void CIRCategoryView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRCategoryView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRCategoryView::DynInitMenuPaneL - Entering" );
	
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_MENU_CATEGORY)
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
	IRLOG_DEBUG( "CIRCategoryView::DynInitMenuPaneL - Exiting" );
	}

// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRCategoryView::HandleStatusPaneSizeChange()
    {
	IRLOG_DEBUG( "CIRCategoryView::HandleStatusPaneSizeChange - Entering" );
    CIRBaseView::HandleStatusPaneSizeChange();

    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
	IRLOG_DEBUG( "CIRCategoryView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRCategoryView::HandleForegroundEventL( TBool aForeground )
	{
	IRLOG_DEBUG( "CIRCategoryView::HandleForegroundEventL - Entering" );

	CIRBaseView::HandleForegroundEventL( aForeground );
	if ( aForeground )
		{
		if ( iContainer )
			{
			iContainer->DrawDeferred();
			}
		}
	IRLOG_DEBUG( "CIRCategoryView::HandleForegroundEventL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRCategoryView::PresetLogoDownloadedL()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRCategoryView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRCategoryView::PresetLogoDownloadedL - Entering" );
	if(iContainer)
		{
		iContainer->PresetLogoDownloadL(aPreset);
		}
	IRLOG_DEBUG( "CIRCategoryView::PresetLogoDownloadedL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRCategoryView::PresetLogoDownloadError()
// from base class MLogoDownloadObserver
// ---------------------------------------------------------------------------
//
void CIRCategoryView::PresetLogoDownloadError(CIRIsdsPreset* )
	{
	IRLOG_DEBUG( "CIRCategoryView::PresetLogoDownloadError - Entering" );
	//call the API of stations view container
	if(iContainer)
		{
		TRAP_IGNORE(iContainer->HandleLogoErrorL())
		}
	IRLOG_DEBUG( "CIRCategoryView::PresetLogoDownloadError - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRCategoryView::LogoRequestL()
// called from the container				
// ---------------------------------------------------------------------------
//
void CIRCategoryView::AdRequestL()
	{
	IRLOG_DEBUG( "CIRCategoryView::LogoRequestL - Entering" );
	if(iContainer)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
		GetLogoDownloadEngine()->SendRequestL(iContainer->iAdvPreset,this);
		}
	IRLOG_DEBUG( "CIRCategoryView::LogoRequestL - Exiting" );
	}
// ---------------------------------------------------------
// CIRCategoryView::SetActualFilteredItemIndex ()
// Sets the actual index of the station
// ---------------------------------------------------------
//
void CIRCategoryView::SetActualFilteredItemIndex(TInt aIndex)
	{
	IRLOG_DEBUG( "CIRCategoryView::SetListBoxMarkedCount - Entering." );
	iIndex = aIndex;
	IRLOG_DEBUG( "CIRCategoryView::SetListBoxMarkedCount - Exiting." );
	}



// ---------------------------------------------------------------------------
// CIRCategoryView::GetFilteredIndex()
// Returns the filtered Index				
// ---------------------------------------------------------------------------
//
TInt CIRCategoryView::GetFilteredIndex()
	{
	IRLOG_DEBUG( "CIRCategoryView::GetFilteredIndex - Entering" );
	return iFilteredIndex;
	}
// ---------------------------------------------------------
// CIRCategoryView::GetFirstLaunchFlag ()
// returns whether is launching for the first time /coming from other view
// which is activated from this view.
// ---------------------------------------------------------
//
TBool CIRCategoryView::GetFirstLaunchFlag()
	{
	IRLOG_DEBUG( "CIRStationsView::GetFirstLaunchFlag - Entering." );
	IRLOG_DEBUG( "CIRStationsView::GetFirstLaunchFlag - Exiting." );
	return iFirstTimeLaunchFlag;
	}
// ---------------------------------------------------------
// CIRCategoryView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRCategoryView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRCategoryView::::HandleSystemEventL - Entering");
	
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
				if(!iCallRequestPending && iCommand==ESelectCmd)
					{
					iUi->iIsdsWrapper->IsdsCancelRequest();
					iCallRequestPending = ETrue;
					}
				}
			break;
			}
		case EIRCallDeactivated:
			{
			if(iCallRequestPending && iCommand==ESelectCmd)
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
	IRLOG_DEBUG( "CIRCategoryView::HandleSystemEventL - Exiting." );
	}	

// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRCategoryView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRCategoryView::NotifyActiveNetworkObserversL - Entering" );
	IRRDEBUG2("CIRCategoryView::NotifyActiveNetworkObserversL entering ", KNullDesC);

	switch(aEvent)
		{
		case ENetworkConnectionDisconnected:
			{
			iNowPlayingWrapper->HandleNetworkDisconnected();
			}
			break;
		case ENetworkConnectionEstablished:
			{
			if(iContainer)
				{
				TBool reIssueRequest = EFalse;
				if(iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iRequestIssued)
						{
						iRequestIssued=EFalse;
						reIssueRequest = ETrue;	
						}
					}
					
				if(iRequestPending || reIssueRequest )
					{
					iRequestPending=EFalse;
					TInt currentIndex = iContainer->GetCurrentItemIndex();
					if (currentIndex == 0 )
						{
						if (iUi->iIsdsWrapper->iCatBannerUrl && !iContainer->GetLogoNotSupported())
							{
							if(iUi->iIsdsWrapper->iCatClickThroughUrl)
								{
								if(iUi->iIsdsWrapper->iCatClickThroughUrl->Length() != 0)
									{
									iContainer->OpenUrlL(*iUi->iIsdsWrapper->iCatClickThroughUrl);
									}
								}
							}
						else
							{
							iRequestIssued=ETrue;
							DoChannelRequestL();			
							}
						}
					else
						{
						iRequestIssued=ETrue;
						DoChannelRequestL();		
						}
					}
				if(iUi->iNetworkController->IsHandingOverConnection())
					{
					ResetPendingRequests(EFalse);			
					}
				}
			}
		}
		
    IRRDEBUG2("CIRCategoryView::NotifyActiveNetworkObserversL exiting ", KNullDesC);
	
	IRLOG_DEBUG( "CIRCategoryView::NotifyActiveNetworkObserversL - Exiting" );
	}

// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRCategoryView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	iRequestIssued = aValue;	
	}
