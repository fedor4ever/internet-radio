/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  view class for pls list
*
*/


#include <hlplch.h>

#include <internetradio.rsg>
#include <internetradio.mbg>

#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irplsview.h"
#include "irplscontainer.h"
#include "irui.h"
#include "irbaseview.h"
#include "irsettings.h"
#include "irnowplayingwrapper.h"
#include "irisdspreset.h"
#include <eikclbd.h>
#include <akntoolbar.h>

#include "irnetworkcontroller.h" 	
#include "iractivenetworkobserver.h"

//-----------------------------------------------------------------------------
//  CIRPlsView::NewL
//  Creates a new instance of pls view
//-----------------------------------------------------------------------------
//
CIRPlsView* CIRPlsView::NewL(const TRect& aRect)
    {
	IRLOG_DEBUG( "CIRPlsView::NewL - Entering" );
    CIRPlsView* self = CIRPlsView::NewLC(aRect);
    CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRPlsView::NewL - Exiting." );
    return self;
    }

//-----------------------------------------------------------------------------
//  CIRPlsView::NewLC
//  Creates a new instance of pls view and leaves it on the cleanup stack.
//-----------------------------------------------------------------------------
//
CIRPlsView* CIRPlsView::NewLC(const TRect& aRect)
    {
    IRLOG_DEBUG( "CIRPlsView::NewLC - Entering" );
    CIRPlsView* self = new (ELeave) CIRPlsView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
	IRLOG_DEBUG( "CIRPlsView::NewLC - Exiting." );
    return self;
    }

//----------------------------------------------------------------------------- 
//  CIRPlsView::CIRPlsView()
//  Default constructor
//-----------------------------------------------------------------------------
//
CIRPlsView::CIRPlsView():iContainer(NULL) 
    {
	IRLOG_DEBUG( "CIRPlsView::CIRPlsView" );
	SetCurrentTopItem(0);
    }

//----------------------------------------------------------------------------- 
//  CIRPlsView::~CIRPlsView()
//  Destructor
//-----------------------------------------------------------------------------
//
CIRPlsView::~CIRPlsView()
    {
	IRLOG_DEBUG( "CIRPlsView::~CIRPlsView - Entering" );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
    	}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	IRLOG_DEBUG( "CIRPlsView::~CIRPlsView - Exiting." );
    }

//-----------------------------------------------------------------------------
//  CIRPlsView::ConstructL
//  Second phase constructor
//-----------------------------------------------------------------------------
//
void CIRPlsView::ConstructL(const TRect& /*aRect*/)
	{
	IRLOG_DEBUG( "CIRPlsView::ConstructL - Entering" );
    CIRBaseView::ConstructL(R_IR_PLSVIEW);
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
    TRgb color;
    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
    				                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_NOW_PLSLIST_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_NOW_PLSLIST_BLACK_TOOLBAR);	
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
	SetCurrentFocus(0);
	iRequestPending = EFalse;
	IRLOG_DEBUG( "CIRPlsView::ConstructL - Exiting." );
	}

	
//-----------------------------------------------------------------------------
//  TUid CIRPlsView::Id()
//  This returns the view ID
//-----------------------------------------------------------------------------
//
TUid CIRPlsView::Id() const
    {
    IRLOG_DEBUG( "CIRPlsView::Id" );
    return KIRPlsViewID;
    }
    
// ----------------------------------------------------------------------------
//  This is called when a view needs to be activated. 
//  This creates container with its controls.
//  It also receives messages sent to the view.
//  aPrevViewId is passed s.t. we can return to the previous view.
//  aCustomMessageId is an ID for the message.
//  aCustomMessage is the actual message.
// ----------------------------------------------------------------------------
//
void CIRPlsView::DoActivateL(const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,
   const TDesC8& aCustomMessage)
	{
	IRLOG_DEBUG( "CIRPlsView::DoActivateL - Entering" );
	iUi->iPresentViewID = KIRPlsViewID;
    if ( !iContainer )
		{
		iContainer =CIRPlsContainer::NewL(ClientRect(),*this);
		iContainer->SetMopParent( this );
		AppUi()->AddToViewStackL( *this, iContainer );
		}
    else
       {
	    // we must notify a resource change in order to update the skin context
	    // This is a flaw of CEikFormattedCellListBox
		iContainer->SetRect(ClientRect());
    	iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
       }
	ConstructToolbarL();
    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
    SetTitleL( R_IRAPP_FEATURE_FILE_TITLE );
    CIRBaseView::DoActivateL( aPrevViewId, TUid::Uid(1), aCustomMessage );
    HandleStatusPaneSizeChange();
    iUi->iNetworkController->RegisterActiveNetworkObserverL(*this);
    IRLOG_DEBUG( "CIRPlsView::DoActivateL - Exiting" );
	}
	
// ----------------------------------------------------------------------------
// CIRPlsView::DoDeactivate()
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// ----------------------------------------------------------------------------
//
void CIRPlsView::DoDeactivate()
	{
	IRLOG_DEBUG( "CIRPlsView::DoDeactivate - Entering" );
	if ( iContainer )
		{
	    SetCurrentFocus(iContainer->GetCurrentItemIndex());
		SetCurrentTopItem(iContainer->GetListBoxTopIndex());
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
 		iContainer = NULL;
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
    iUi->iNetworkController->DeleteActiveNetworkObserver(*this);
	IRLOG_DEBUG( "CIRPlsView::DoDeactivate - Exiting." );
	}
	
// ----------------------------------------------------------------------------
//  CIRPlsView::HandleCommandL(TInt aCommand) 
//  Command handling function
// ----------------------------------------------------------------------------
//	
void CIRPlsView::HandleCommandL(TInt aCommand)
	{
	IRLOG_DEBUG( "CIRPlsView::HandleCommandL - Entering" );
	iCurrentFocus = iContainer->GetCurrentItemIndex();	
    
    // handle the corresponding command selected by the user through options.
	switch(aCommand)
		{
	
		case EListenCmd:
			{
			iUi->SetListenFromPls(ETrue);
			if( iUi->VerifyNetworkConnectivityL() )
				{
				iRequestPending = ETrue;
				break;
				}
			else
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				iContainer->ListenToStationL(); 
				}
			   
     		}
		    break;

		case ESaveCmd:
			{  
			if( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
     			iContainer->SaveL();
				}
			}	
			break;
			
		case ESaveAllCmd:
			{
			if( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iContainer->SaveAllL();
				}
		 	}
			break;
			
		case EGotoNowPlayingViewCmd:
		    {
		    SetCurrentFocus(iContainer->GetCurrentItemIndex());
			iUi->SetHistory(EFalse);
			iUi->SavePrevIDL(KIRMainChoiceViewID,aCommand); 
		    }
		    break;
		    
		case ESettingsCmd:
		    {
		    SetCurrentFocus(iContainer->GetCurrentItemIndex());
		    AppUi()->ActivateLocalViewL(KIRSettingsViewId);
		    }
		    break;
		    
	    case EHelpCmd:
			{
			if( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
	            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
	            			 AppUi()->AppHelpContextL() );
				}
			}
            break;
            
		case EGotoMainView:
			{
			_LIT8(KNull,"");
			AppUi()->ActivateLocalViewL( KIRMainChoiceViewID ,KMainViewLaunch ,KNull);
			}
	    	break;   
		default:
			{
			CIRBaseView::HandleCommandL( EAknSoftkeyExit );	
			}

		}
	IRLOG_DEBUG( "CIRPlsView::HandleCommandL - Exiting." );
	}
	
// ----------------------------------------------------------------------------
// CIRPlsView::DynInitMenuPaneL()
// Dynamically handles menu pane.
// ----------------------------------------------------------------------------
//
void CIRPlsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRPlsView::DynInitMenuPaneL - Entering" );
	// Pls view options menu  
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );  
	if(aResourceId == R_IR_MENU_PLS  )
		{
        	if(!aMenuPane) User::Leave(KErrNotFound);
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
				if(iUi->IsFirstTime())
					{
					aMenuPane->SetItemDimmed(EGotoMainView,ETrue);	
					}
				else
					{
					aMenuPane->SetItemDimmed(EGotoMainView,EFalse);	
					}
        	}
		}
	IRLOG_DEBUG( "CIRPlsView::DynInitMenuPaneL - Exiting." );
	}
	
// ---------------------------------------------------------------------------
// CIRPlsView::ConstructToolbarL()
// Coonstructs the toolbar for the view.
// ---------------------------------------------------------------------------
//    
void CIRPlsView::ConstructToolbarL()
	{
	IRLOG_DEBUG( "CIRPlsView::ConstructToolbarL - Entering." );

	TInt count = (iUi->iURLs.Count());
    if(Toolbar())
	    {
		if((count==0))
			{
			Toolbar()->SetItemDimmed(ESaveCmd,ETrue,ETrue);
			Toolbar()->SetItemDimmed(EListenCmd,ETrue,ETrue);
			}
		else
			{
			Toolbar()->SetItemDimmed(ESaveCmd,EFalse,ETrue);
			Toolbar()->SetItemDimmed(EListenCmd,EFalse,ETrue);
			}
		if((count<=2))
			{
			Toolbar()->SetItemDimmed(ESaveAllCmd,ETrue,ETrue);
			}
		else
			{
			Toolbar()->SetItemDimmed(ESaveAllCmd,EFalse,ETrue);
			}
		Toolbar()->SetToolbarVisibility( ETrue );

	    // Update rect, as changes in toolbar visibility also affect layout.
	    if(iContainer)
		    {
		    iContainer->SetRect( ClientRect() );
		    iContainer->DrawDeferred();
		    }
	    }
	IRLOG_DEBUG( "CIRPlsView::ConstructToolbarL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRPlsView::ConstructToolbarL()
// Dynamically initialises a tool bar..
// ---------------------------------------------------------------------------
//    
void CIRPlsView::DynInitToolbarL( TInt /*aResourceId*/, CAknToolbar* /*aToolbar*/ )
	{
	IRLOG_DEBUG( "CIRPlsView::DynInitToolbarL - Entering." );
	IRLOG_DEBUG( "CIRPlsView::DynInitToolbarL - Exiting." );
    }
		
// ---------------------------------------------------------------------------
// CIRMainView::OfferToolbarEventL()
// catch the selection key event and perform the operation according to it...
// ---------------------------------------------------------------------------
void CIRPlsView::OfferToolbarEventL
        ( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRPlsView::OfferToolbarEventL - Entering." );
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRPlsView::OfferToolbarEventL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRPlsView::HandleStatusPaneSizeChange()
// 
// ---------------------------------------------------------------------------
//	
void CIRPlsView::HandleStatusPaneSizeChange()
    {
    IRLOG_DEBUG( "CIRPlsView::HandleStatusPaneSizeChange - Entering." );
    // the client rect is also set here after screen orientation change
    if (iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    IRLOG_DEBUG( "CIRPlsView::HandleStatusPaneSizeChange - Exiting." );
    }
// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRPlsView::HandleForegroundEventL( TBool aForeground )
	{
	IRLOG_DEBUG( "CIRMainView::HandleForegroundEventL - Entering" );
	CIRBaseView::HandleForegroundEventL( aForeground );
	if(aForeground)
	{
	if(iContainer)
		{
		MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
	    TRgb color;
	    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
	    				                                      EAknsCIQsnTextColorsCG6 );
		if(color.Gray2()!=0)
			{
			CreateAndSetToolbarL(R_NOW_PLSLIST_WHITE_TOOLBAR);
			}
		else
			{
			CreateAndSetToolbarL(R_NOW_PLSLIST_BLACK_TOOLBAR);	
			}
        if( AknLayoutUtils::PenEnabled() && Toolbar() )
			{
			Toolbar()->SetToolbarObserver( this );
			}
		ConstructToolbarL();
	    //iUi->iNetworkController->RegisterActiveNetworkObserver(this)
	    //iListen = ETrue
	    iContainer->DrawDeferred();
        Toolbar()->DrawDeferred();
		iContainer->SecondPls();
		}
	}
	IRLOG_DEBUG( "CIRMainView::HandleForegroundEventL - Exiting." );
	}		


// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRPlsView::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRPlsView::NotifyActiveNetworkObserversL - Entering." );
	
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
				SetCurrentFocus(iContainer->GetCurrentItemIndex());
				iContainer->ListenToStationL(); 
				}
			}
		ResetPendingRequests(EFalse);
		}
	}

	IRLOG_DEBUG( "CIRPlsView::NotifyActiveNetworkObserversL - Exiting." );
	}
	
	
// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRPlsView::ResetPendingRequests(TBool aValue)
	{
	iRequestPending = aValue;
	}

	

// CIRPlsView :: SecondConstruct()
// To again initialise when Second pls file is opened.
// ---------------------------------------------------------------------------
//
void CIRPlsView::SecondConstruct()
	{
	SetCurrentFocus(0);
	SetCurrentTopItem(0);
	iRequestPending = EFalse;
	}
