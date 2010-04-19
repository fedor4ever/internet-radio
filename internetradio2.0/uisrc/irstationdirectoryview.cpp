/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Station Directory View
*
*/

/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 2, Fri March 21 17:20:00 2008 by Prabina
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
*  Version: 3, Tues March 25 17:30:00 2008 by Prabina
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
*  Version: 4, Tues April 04 16:40:00 2008 by Prabina
*  Ref:
*  Added WorkAround for retaining of focus & Page retaining
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
// SYSTEM INCLUDE FILES
#include <akntitle.h>
#include <eikmenup.h>
#include <psserv.h>
#include <hlplch.h>
#include <eikenv.h>
#include <internetradio.rsg>

// INCLUDE FILES
#include "irdebug.h"
#include "irui.h"
#include "irbaseview.h"
#include "irstationdirectoryview.h"
#include "irstationdirectorycontainer.h"
#include "ircategoryview.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irviewstack.h"
#include "irisdswrapper.h"
#include "irnowplayingwrapper.h"
#include "hlplch.h"
#include "irsettings.h"
#include "irnetworkcontroller.h" 	//Added for demo of ALR //TO DO::Remove after demo
#include "iractivenetworkobserver.h"
#include "irtopstationsview.h"

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRStationDirectoryView::CIRStationDirectoryView( ):iCallRequestPending(EFalse)
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::CIRStationDirectoryView" );
	}
// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::ConstructL()
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::ConstructL - Entering" );
    CIRBaseView::ConstructL( R_IR_STATION_DIRECTORY );
	SetCurrentFocus(0);
	iRequestPending = EFalse;
	IRLOG_DEBUG( "CIRStationDirectoryView::ConstructL - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRStationDirectoryView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRStationDirectoryView* CIRStationDirectoryView::NewL()
    {
 	IRLOG_DEBUG( "CIRStationDirectoryView::NewL" );
    CIRStationDirectoryView* self = CIRStationDirectoryView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRStationDirectoryView::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRStationDirectoryView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRStationDirectoryView* CIRStationDirectoryView::NewLC()
    {
 	IRLOG_DEBUG( "CIRStationDirectoryView::NewLC" );
   	CIRStationDirectoryView* self = new( ELeave ) CIRStationDirectoryView();
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRStationDirectoryView::NewLC - Exiting" );
    return self;
    }
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRStationDirectoryView::~CIRStationDirectoryView()
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::~CIRStationDirectoryView - Entering" );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        delete iContainer;
        iContainer = NULL;
        }
	IRLOG_DEBUG( "CIRStationDirectoryView::~CIRStationDirectoryView - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRStationDirectoryView::Id() const
    {
    IRLOG_DEBUG( "CIRStationDirectoryView::Id" );
    return KIRStationDirectoryViewId;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::HandleStatusPaneSizeChange()
    {
 	IRLOG_DEBUG( "CIRStationDirectoryView::HandleStatusPaneSizeChange - Entering" );
   	CIRBaseView::HandleStatusPaneSizeChange();

    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
	IRLOG_DEBUG( "CIRStationDirectoryView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
					 const TDesC8& aCustomMessage )
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::DoActivateL - Entering" );
	
	TInt customMessageId = aCustomMessageId.iUid;
	if(customMessageId == 1)
		{
		SetCurrentFocus(0);	
		}
	aCustomMessageId.iUid = 0;
	iUi->iPresentViewID = KIRStationDirectoryViewId;
	
    if ( !iContainer )
        {
	    iContainer = CIRStationDirectoryContainer::NewL( *this, ClientRect() );
	    iContainer->SetMopParent( this );
  		AppUi()->AddToViewStackL( *this, iContainer );
        }
    else
        {
        iContainer->SetRect( ClientRect( ) );
        iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
        }

    iContainer->ActivateL( );
    iContainer->MakeVisible( ETrue );

    SetTitleL(R_IRAPP_STAT_DIR_TITLE);
    CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    HandleStatusPaneSizeChange();
    
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
    EnableObserverL(EIRSystemEventObserver);
	IRLOG_DEBUG( "CIRStationDirectoryView::DoActivateL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::DoDeactivate()
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::DoDeactivate - Entering" );
    if(&(iUi->SystemEventCollector()))
	    {
		DisableObserver(EIRSystemEventObserver);
	    }
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        delete iContainer;
        iContainer = NULL;
        }
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);
	IRLOG_DEBUG( "CIRStationDirectoryView::DoDeactivate - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::HandleCommandL( TInt aCommand )
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::HandleCommandL - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	CIRViewStack& viewStack = appUi->ViewStack();	
    switch ( aCommand )
        {
        case ESelectCmd:
	        {
			if( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			else if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				}
			else
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				iRequestIssued = ETrue;
				DoCategoryRequestL();
				}
	        }
        break;

        case EGotoNowPlayingViewCmd:
			{
			appUi->SetHistory(EFalse);
			appUi->SavePrevIDL(KIRStationDirectoryViewId, aCommand); 			
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
	       		HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(),
	       							AppUi()->AppHelpContextL());
				}
			}
		break;
		case EAknSoftkeyBack:
			{
			if((appUi->IsFirstTime()))
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
						appUi->ActivateLocalViewL(KIRMainChoiceViewID);	
						}
					}

				else if(viewStack.Count() >2 )
					{
					CIRBaseView::HandleCommandL( aCommand );	
					}
				else
					{
					viewStack.Pop();
					appUi->ActivateLocalViewL(KIRMainChoiceViewID);
					}
				}
			}
	    break;
        default:
            {
            CIRBaseView::HandleCommandL( aCommand );
            }
        break;
        }
	IRLOG_DEBUG( "CIRStationDirectoryView::HandleCommandL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::HandleForegroundEventL( TBool aForeground )
    {
	IRLOG_DEBUG( "CIRStationDirectoryView::HandleForegroundEventL - Entering" );

    CIRBaseView::HandleForegroundEventL( aForeground );
	if ( aForeground )
		{
		if ( iContainer )
			{
			iContainer->DrawDeferred();
			}
		}
	IRLOG_DEBUG( "CIRStationDirectoryView::HandleForegroundEventL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRStationDirectoryView::DoCategoryRequestL()
// 
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::DoCategoryRequestL()
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::DoCategoryRequestL - Entering" );
	iLoadingCancelled = EFalse;
	TInt currentIndex = iContainer->GetCurrentItemIndex();
	if(currentIndex >= 0)
		{
		switch (currentIndex)
			{
			// if the browse by genre's is selected.
			case EGenreData:
				{
		        iIsdsWrapper->IsdsCategoryRequestL(this,CIRIsdsClient::EGenre);
				iUi->iCategoryView->iCategoryReqMade = ETrue;		        
				}
				break;
			// if the browse by language's is selected.	
			case ELanguageData:
				{
		        iIsdsWrapper->IsdsCategoryRequestL(this,CIRIsdsClient::ELanguages);
		        iUi->iCategoryView->iCategoryReqMade = ETrue;
				}
				break;
			// if the browse by country is selected.	
			case ECountryData:
				{
		        iIsdsWrapper->IsdsCategoryRequestL(this,CIRIsdsClient::ECountries);
		        iUi->iCategoryView->iCategoryReqMade = ETrue;
				}
				break;
			// if top station's is selected.	
			case ETopStationData:
				{
		        iIsdsWrapper->IsdsCategoryRequestL(this,CIRIsdsClient::Ehotpicks);
		        iUi->iTopStationsView->iResultsReqMade = ETrue;
				}
				break;
			default:
				{
				//No implementation
				}
				break;
			}
		}
	IRLOG_DEBUG( "CIRStationDirectoryView::DoCategoryRequestL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationDirectoryView::ResponseL()
// Activates the SearchResults view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::ResponseL( CIRIsdsPreset* /*aPreset*/ )
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::ResponseL - Entering" );
	ResetPendingRequests(EFalse);
	if(!iLoadingCancelled)
		{
		TInt currentIndex = iContainer->GetCurrentItemIndex();
		if(currentIndex >= 0)
			{
			switch (currentIndex)
				{
				// if the browse by genre's is selected.
				case EGenreData:
					{
					iUi->iCategoryViewTitleType = EGenreData;
					ActivateCategoryViewL();
					}
					break;
				// if the browse by language's is selected.	
				case ELanguageData:
					{
					iUi->iCategoryViewTitleType = ELanguageData;
					ActivateCategoryViewL();
					}
					break;
				// if the browse by country is selected.	
				case ECountryData:
					{
					iUi->iCategoryViewTitleType = ECountryData;
					ActivateCategoryViewL();
					}
					break;
				// if top station's is selected.	
				case ETopStationData:
					{
					iUi->iCategoryViewTitleType = ETopStationData;
		            TBuf8<2> dummy;
		 			TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRTopStationsViewId);
					if(iUi->IsForeground())
						{
						AppUi()->ActivateViewL(viewId ,TUid::Uid(1),dummy);
						iUi->iActivationPending = KNullUid ;	
						}
					else
						{
						iUi->iActivationPending =KIRTopStationsViewId;	
						}
					}
					break;
				default:
					{
					//No implementation
					}
					break;
				}
			}
		}
	IRLOG_DEBUG( "CIRStationDirectoryView::ResponseL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRStationDirectoryView::ActivateCategoryViewL()
// Activates the Category view 
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::ActivateCategoryViewL()
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::ActivateCategoryViewL - Entering" );
	CIRUi* appUi = static_cast<	CIRUi*>(AppUi());
	TBuf8<2> dummy;
	TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRCategoryViewId);
	
	if(appUi->IsForeground())
		{
		AppUi()->ActivateViewL(viewId ,TUid::Uid(1),dummy);
		appUi->iActivationPending = KNullUid ;	
		}
	else
		{
		appUi->iActivationPending =KIRCategoryViewId;	
		}
	IRLOG_DEBUG( "CIRStationDirectoryView::ActivateCategoryViewL - Exiting" );
	
	}
// ---------------------------------------------------------------------------
// void CIRStationDirectoryView::PresetResponseL()
// Issues Request to listen to a particular channel 
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::PresetResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::PresetResponseL - Entering" );
	
		//No Implementation Required

	IRLOG_DEBUG( "CIRStationDirectoryView::PresetResponseL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRStationDirectoryView::ErrorL()
// Handles the errors from Isds 
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::ErrorL()
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::ErrorL - Entering" );
	if(iIsdsWrapper->iConnTimeOut)
		{
		iIsdsWrapper->iConnTimeOut = EFalse;	
		}
	IRLOG_DEBUG( "CIRStationDirectoryView::ErrorL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRStationDirectoryView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::DynInitMenuPaneL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_STATION_DIRECTORY_MENU)
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
	IRLOG_DEBUG( "CIRStationDirectoryView::DynInitMenuPaneL - Exiting" );
	}
// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRStationDirectoryView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::NotifyActiveNetworkObserversL - Entering" );
	IRRDEBUG2("CIRStationDirectoryView::NotifyActiveNetworkObserversL entering ", KNullDesC);

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
				if(iRequestPending)
					{
					iRequestIssued = ETrue;
					SetCurrentFocus(iContainer->GetCurrentItemIndex());
					iRequestPending=EFalse;
					DoCategoryRequestL();
					}
				
				if(iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iRequestIssued)
						{
						iRequestIssued=EFalse;
						SetCurrentFocus(iContainer->GetCurrentItemIndex());
						DoCategoryRequestL();
						}
					}
				}
			}
		}
		
	IRLOG_DEBUG( "CIRStationDirectoryView::NotifyActiveNetworkObserversL - Exiting" );
	IRRDEBUG2("CIRStationDirectoryView::NotifyActiveNetworkObserversL exiting ", KNullDesC);

	}
	
	
	
	
// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRStationDirectoryView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	iRequestIssued  = aValue;
	}
	
// ---------------------------------------------------------
// CIRStationDirectoryView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRStationDirectoryView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRStationDirectoryView::HandleSystemEventL - Entering");
	
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
	IRLOG_DEBUG( "CIRStationDirectoryView::HandleSystemEventL - Exiting." );
	}	
// ---------------------------------------------------------------------------
// void CIRStationDirectoryView::IssueSearchRequestL()
// Issues a search request to the Isds Client
// ---------------------------------------------------------------------------
//
void CIRStationDirectoryView::RefreshView()
	{
	IRLOG_DEBUG( "CIRStationDirectoryView::IssueSearchRequestL - Entering" );
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
	IRLOG_DEBUG( "CIRStationDirectoryView::IssueSearchRequestL - Exiting" );
	}
// End of file
