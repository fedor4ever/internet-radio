/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Find Station View
*
*/


// INCLUDE FILES

#include <aknnotewrappers.h>
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <aknbutton.h>
#include <internetradio.rsg>
#include "irfirsttimeview.h"
#include <internetradio.mbg>
#include "irfirsttimecontainer.h"
#include "ircommon.h"
/* don't need to change to lower case  */
#include <AknQueryDialog.h> 
#include "irisdswrapper.h"

  
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irsettings.h"
#include <stringloader.h>
#include "irnowplayingwrapper.h"


#include "hlplch.h"
#include "irsettings.h"
#include "irdialoglauncher.h"
#include "irnetworkcontroller.h" 	
#include "iractivenetworkobserver.h"
#include "irsearchresultsview.h"



const TInt KStringSize = 20;

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRFirstTimeView::CIRFirstTimeView():iCallRequestPending(EFalse)
    {
    IRLOG_DEBUG( "CIRFirstTimeView::CIRFirstTimeView" );
    // Nothing here
    }


// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::ConstructL()
    {
    IRLOG_DEBUG( "CIRFirstTimeView::ConstructL  - Entering" );
    CIRBaseView::ConstructL( R_IR_FIRSTTIME_VIEW );
    iRequestPending = EFalse;
    IRLOG_DEBUG( "CIRFirstTimeView::ConstructL - Exiting" );
    }

// -----------------------------------------------------------------------------
// CIRFirstTimeView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRFirstTimeView* CIRFirstTimeView::NewL()
    {
 	IRLOG_DEBUG( "CIRFirstTimeView::NewL" );
 	CIRFirstTimeView* self = CIRFirstTimeView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRFirstTimeView::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRFirstTimeView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRFirstTimeView* CIRFirstTimeView::NewLC()
    {
    IRLOG_DEBUG( "CIRFirstTimeView::NewLC - Entering" );
    CIRFirstTimeView* self = new( ELeave ) CIRFirstTimeView();
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRFirstTimeView::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRFirstTimeView::~CIRFirstTimeView()
    {
    IRLOG_DEBUG( "CIRFirstTimeView::~CIRFirstTimeView - Entering" );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    IRLOG_DEBUG( "CIRFirstTimeView::~CIRFirstTimeView - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRFirstTimeView::Id() const
    {
    IRLOG_DEBUG( "CIRFirstTimeView::Id" );
    return KIRFirstTimeViewId;
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::HandleStatusPaneSizeChange()
    {
    IRLOG_DEBUG( "CIRFirstTimeView::HandleStatusPaneSizeChange - Entering" );
    CIRBaseView::HandleStatusPaneSizeChange();
    
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    IRLOG_DEBUG( "CIRFirstTimeView::HandleStatusPaneSizeChange - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::DoActivateL( const TVwsViewId& aPrevViewId,
                                    TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    IRLOG_DEBUG( "CIRFirstTimeView::DoActivateL - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
    
    TInt customMessageId = aCustomMessageId.iUid;
	if(customMessageId == 1)
		{
		SetCurrentFocus(0);	
		}
	aCustomMessageId.iUid = 0;
	
    iFirstTime = appUi->IsFirstTime();
    appUi->iPresentViewID = KIRFirstTimeViewId;

    if ( !iContainer )
        {
        iContainer = new( ELeave ) CIRFirstTimeContainer(*this);
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
		AppUi()->AddToViewStackL( *this, iContainer );
       }
    else
        {
        // we must notify a resource change in order to update the skin context
        // This is a flaw of CEikFormattedCellListBox
        iContainer->SetRect( ClientRect() );
        iContainer->HandleResourceChange( KEikDynamicLayoutVariantSwitch );
        }

    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
    SetTitleL( R_IRAPP_FIRSTTIME_TITLE );
               

    if ( iFirstTime &&  !(iUi->IsFlagIfAnyFavL()))
        {
        SetTitleL( R_IRAPP_FIRSTTIME_TITLE);
		CIRBaseView::DoActivateL( aPrevViewId, TUid::Uid(1), aCustomMessage );
        }
    else
        {
        SetTitleL( R_IRAPP_FINDSTATIONS_TITLE );
		CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
        }
    HandleStatusPaneSizeChange(); 
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
    EnableObserverL(EIRSystemEventObserver);
    IRLOG_DEBUG( "CIRFirstTimeView::DoActivateL - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::DoDeactivate()
    {
    IRLOG_DEBUG( "CIRFirstTimeView::DoDeactivate - Entering" );
    iFirstTime = EFalse;
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
    IRLOG_DEBUG( "CIRFirstTimeView::DoDeactivate - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::HandleCommandL( TInt aCommand )
    {
    IRLOG_DEBUG( "CIRFirstTimeView::HandleCommandL - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi( ) );
    SetCurrentFocus(iContainer->GetCurrentItemIndex());
	iCommand = aCommand;

    switch ( aCommand )
        {
        case EFirstTimeSelectCmd:
        {
        
        switch(iContainer->GetCurrentItemIndex( ) )
                {
                case 0:
	                {
				    TBuf8<2> dummy;
					TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRStationDirectoryViewId);
					AppUi()->ActivateViewL(viewId ,TUid::Uid(1),dummy);
	                }
                    break;
                case 1:
	                {
					if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
						{
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
                case 2:
                    {
					if( !iUi->CheckAndNotifyLowDiskSpaceL() )
						{
		                 appUi->SetEditStation(EFalse);
		                 appUi->ActivateLocalViewL(KIRAddManuallyStationViewID);
						}
	                }
                    break;
                default:
                    break;
                }
        	
        }
        break;
        
        case EFirstTimeSettingsCmd:
        {
        AppUi()->ActivateLocalViewL(KIRSettingsViewId);
        }
        break;
            
        case EHelpCmd:
        {
		if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
			{
	        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                             	 AppUi()->AppHelpContextL() );
			}
        }                    
        break;
            
        case EGotoNowPlayingViewCmd:
		{  
		appUi->SetHistory(EFalse);
		AppUi()->ActivateLocalViewL(KIRNowPlayingViewID);
		}
       	break;
        default:
        {
        	if(iFirstTime)
				{
				CIRBaseView::HandleCommandL( EAknSoftkeyExit );		
				}
        	else
				{
				CIRBaseView::HandleCommandL( aCommand );	
				}
           break;
        }
      }
    IRLOG_DEBUG( "CIRFirstTimeView::HandleCommandL - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::HandleForegroundEventL( TBool aForeground )
    {
    IRLOG_DEBUG( "CIRFirstTimeView::HandleForegroundEventL - Entering" );
    CIRBaseView::HandleForegroundEventL( aForeground );
    IRLOG_DEBUG( "CIRFirstTimeView::HandleForegroundEventL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// void CIRFirstTimeView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
	
    
void CIRFirstTimeView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRFirstTimeView::DynInitMenuPaneL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_FIRSTTIME_MENU)
			{				
		/*
			// Update Application option
			if(appUi->iIRSettings->IsUpdateAvailable())
				{
				 aMenuPane->SetItemDimmed(EUpdateApplicationCmd,EFalse);
				}
			else
				{
				 aMenuPane->SetItemDimmed(EUpdateApplicationCmd,ETrue);
				}
		*/

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
	IRLOG_DEBUG( "CIRFirstTimeView::DynInitMenuPaneL - Exiting" );
	}


void CIRFirstTimeView::ShowQueryDialogL(const TDesC& aTitle, TDes& aBuffer)
	{
	IRLOG_DEBUG( "CIRFirstTimeView::ShowQueryDialogL - Entering" );
	CAknTextQueryDialog* Dialog =
	CAknTextQueryDialog::NewL(aBuffer,CAknQueryDialog::ENoTone);
	Dialog->PrepareLC(R_SEARCH_STRING_DIALOG);
	Dialog->SetPromptL(aTitle); 
	iSearchValue = Dialog->RunLD();

	if(	iSearchValue)
		{
		iUi->iSearchString = aBuffer;
		
		if( iUi->VerifyNetworkConnectivityL() )
			{
			iRequestPending = ETrue;
			}
		else
			{
			IssueSearchRequestL();
			}
		
		
		}
	IRLOG_DEBUG( "CIRFirstTimeView::ShowQueryDialogL - Exiting" );
	}


// ---------------------------------------------------------------------------
// void CIRFirstTimeView::ResponseL()
// Activates the SearchResults view after getting the IsdsResponse
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::ResponseL(CIRIsdsPreset* /*aPreset*/ )
	{
	IRLOG_DEBUG( "CIRFirstTimeView::ResponseL() started." );
	

	if(iIsdsWrapper->iStationsData->Count())
	{
		CIRUi* appUi = static_cast<	CIRUi*>(AppUi());
		TBuf8<2> dummy;
		TVwsViewId viewId(TUid::Uid(KUIDIRAPP),KIRSearchResultsViewID);
		if(appUi->IsForeground())
			{
			AppUi()->ActivateViewL(viewId ,TUid::Uid(1),dummy);
			appUi->iActivationPending = KNullUid ;	
			}
		else
			{
			appUi->iActivationPending =KIRSearchResultsViewID;	
			}
	}
	else
	{
		iIsdsWrapper->GetDialogLauncherInstance()->ShowInformationNoteL( R_IRAPP_RESULTS_NONE,ETrue );
	    iIsdsWrapper->iError = EFalse;

	}
	
	IRLOG_DEBUG( "CIRFirstTimeView::ResponseL() exiting." );		
	}


void CIRFirstTimeView::PresetResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRFirstTimeView::PresetResponseL() started." );
	IRLOG_DEBUG( "CIRFirstTimeView::PresetResponseL() exiting." );
	return;
	}

void CIRFirstTimeView::ErrorL()
	{
	IRLOG_DEBUG( "CIRFirstTimeView::ErrorL - Entering" );
	if(iIsdsWrapper->iDialogDismissed)
		{
		iIsdsWrapper->iDialogDismissed=EFalse;
	    return;
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

	IRLOG_DEBUG( "CIRFirstTimeView::ErrorL - Exiting" );
	}

// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRFirstTimeView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRFirstTimeView::NotifyActiveNetworkObserversL - Entering" );
	IRRDEBUG2("CIRFirstTimeView::NotifyActiveNetworkObserversL entering ", KNullDesC);

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
				if(iRequestPending || iUi->iNetworkController->IsHandingOverConnection())
					{
					if(iSearchValue)
						{	
						IssueSearchRequestL();
						}
					}
				}
				
			ResetPendingRequests(EFalse);				
			}
		}

	IRRDEBUG2("CIRFirstTimeView::NotifyActiveNetworkObserversL exiting ", KNullDesC);
	
	IRLOG_DEBUG( "CIRFirstTimeView::NotifyActiveNetworkObserversL - Exiting" );
	}



// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRFirstTimeView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
//	iSearchValue = aValue
	}


// ---------------------------------------------------------------------------
// void CIRFirstTimeView::IssueSearchRequestL()
// Issues a search request to the Isds Client
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::IssueSearchRequestL()
	{
	IRLOG_DEBUG( "CIRFirstTimeView::IssueSearchRequestL - Entering" );
	//Display a searching Progress Bar
	
	iIsdsWrapper->IsdsSearchRequestL(this,iUi->iSearchString);
	iUi->iSearchResultsView->iSearchReqMade = ETrue;
	IRLOG_DEBUG( "CIRFirstTimeView::IssueSearchRequestL - Exiting" );
	}
// ---------------------------------------------------------
// CIRFirstTimeView::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRFirstTimeView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRFirstTimeView::HandleSystemEventL - Entering");
	
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
				if(!iCallRequestPending && iCommand==EFirstTimeSelectCmd &&
					(iContainer->GetCurrentItemIndex() == 1))
					{
					iUi->iIsdsWrapper->IsdsCancelRequest();
					iCallRequestPending = ETrue;
					}
				}
			break;
			}
		case EIRCallDeactivated:
			{
			if(iCallRequestPending && iCommand==ESelectCmd &&
				(iContainer->GetCurrentItemIndex() == 1))
				{
				iCallRequestPending = EFalse;
				if(iSearchValue)
					{	
					IssueSearchRequestL();
					}
				}
			break;
			}
		default:
			{
			break;
			}
		}
	IRLOG_DEBUG( "CIRFirstTimeView::HandleSystemEventL - Exiting." );
	}	
// ---------------------------------------------------------------------------
// void CIRFirstTimeView::RefreshView()
// Refreshes the view when layout changes.
// ---------------------------------------------------------------------------
//
void CIRFirstTimeView::RefreshView()
	{
	IRLOG_DEBUG( "CIRFirstTimeView::IssueSearchRequestL - Entering" );
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
	IRLOG_DEBUG( "CIRFirstTimeView::IssueSearchRequestL - Exiting" );
	}
// End of file
