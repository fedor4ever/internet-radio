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
* Description:  Base view from which other views are derived (with a few
*                exceptions).
*
*/


#include <akntitle.h>
#include <stringloader.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <internetradio.rsg>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irapplication.h"
#include "IRIsdsWrapper.h"
#include "irviewstack.h"
#include "irsystemeventcollector.h" //For system events
#include "irnowplayingwrapper.h"
#include "irdialoglauncher.h"  
#include "irisdspreset.h"

// ---------------------------------------------------------
// CIRBaseView::CIRBaseView
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CIRBaseView::CIRBaseView()
    {
    IRLOG_DEBUG( "CIRBaseView::CIRBaseView" );
    }

// ---------------------------------------------------------
// CIRBaseView::~CIRBaseView
// Destructor
// ---------------------------------------------------------
//
CIRBaseView::~CIRBaseView()
    {
    IRLOG_DEBUG( "CIRBaseView::~CIRBaseView - Entering" );
	IRLOG_DEBUG( "CIRBaseView::~CIRBaseView - Exiting" );
    }

// ---------------------------------------------------------
// CIRBaseView::ConstructL
// EPOC default constructor can leave.
// ---------------------------------------------------------
//
void CIRBaseView::ConstructL( TInt aResourceId )
    {
    IRLOG_DEBUG( "CIRBaseView::ConstructL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
    BaseConstructL( aResourceId );
    iUi = REINTERPRET_CAST(CIRUi*,iEikonEnv->EikAppUi());
	//get the instance of the Wrapper class from AppUi
    iIsdsWrapper = appUi->GetWrapperInstance();
    iNowPlayingWrapper = appUi->GetNowPlayingWrapperInstance();
    IRLOG_DEBUG( "CIRBaseView::ConstructL - Exiting" );
 	}

// ---------------------------------------------------------
// CIRBaseView::EnableObserverL
// (other items were commented in a header).
// ---------------------------------------------------------
//
 void CIRBaseView::EnableObserverL( TInt aObservers )
	{
	IRLOG_DEBUG( "CIRBaseView::EnableObserverL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	if (aObservers & EIRSystemEventObserver )
		{
		appUi->SystemEventCollector().AddObserverL( this );
		iObserverEnabled |= EIRSystemEventObserver;
		}
	IRLOG_DEBUG( "CIRBaseView::EnableObserverL - Exiting" );
	}

// ---------------------------------------------------------
// CIRBaseView::DisableObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIRBaseView::DisableObserver( TInt aObservers )
    {
    IRLOG_DEBUG( "CIRBaseView::DisableObserver - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
    if (aObservers & EIRSystemEventObserver )
        {
        appUi->SystemEventCollector().RemoveObserver( this );
        iObserverEnabled &= ~EIRSystemEventObserver;
        }
    IRLOG_DEBUG( "CIRBaseView::DisableObserver - Exiting" );
    }
// ---------------------------------------------------------
// CIRBaseView::SetTitleL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIRBaseView::SetTitleL( TInt aResourceId )
    {
    IRLOG_DEBUG( "CIRBaseView::SetTitleL - Entering" );
    HBufC* title = StringLoader::LoadLC(aResourceId);
    static_cast<CAknTitlePane*>( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) )->
    							 SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    IRLOG_DEBUG( "CIRBaseView::SetTitleL - Exiting" );
    }

// ---------------------------------------------------------
// CIRBaseView::SetTitleL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIRBaseView::SetTitleL( const TDesC& aTitleText )
    {
    IRLOG_DEBUG( "CIRBaseView::SetTitleL - Entering" );
    static_cast<CAknTitlePane*>( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) )->
    							 SetTextL( aTitleText );
    IRLOG_DEBUG( "CIRBaseView::SetTitleL - Exiting" );
    }

// ---------------------------------------------------------
// CIRBaseView::DoActivateL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIRBaseView::DoActivateL( const TVwsViewId& /*aPrevViewId*/, TUid aCustomMessageId, const TDesC8& /*aCustomMessage*/ )
	{
	IRLOG_DEBUG( "CIRBaseView::DoActivateL - Entering" );
	CEikStatusPane* statP = StatusPane();
	if(iUi->IsLandscapeOrientation())
		{
		statP->SwitchLayoutL(R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL);
		}
	else
		{
		statP->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT);
		}
	if(aCustomMessageId==TUid::Uid(1))
		{
		return;
		}
	HandleViewStackL( aCustomMessageId);
	IRLOG_DEBUG( "CIRBaseView::DoActivateL - Exiting" );    
	}

// ---------------------------------------------------------
// CIRBaseView::DoDeactivate
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIRBaseView::DoDeactivate()
    {
    IRLOG_DEBUG( "CIRBaseView::DoDeactivate" );
    // Nothing here at the moment
    }


// ---------------------------------------------------------
// CIRBaseView::HandleCommandL
// ---------------------------------------------------------
//
void CIRBaseView::HandleCommandL( TInt aCommand )
    {
    IRLOG_DEBUG( "CIRBaseView::HandleCommandL - Exiting" );   
     	CIRUi* appUi = static_cast<	CIRUi*>(AppUi());
    CIRViewStack& viewStack = appUi->ViewStack();
    switch ( aCommand )
        {
       	case EIRBack:
        case EAknSoftkeyBack:
        	{
            // Check if Back Stepping Service consumes the command.
            if ( !static_cast< CIRUi* >( AppUi() )->ForwardBackCommandToBsWrapperL() )
                {
                // Command not consumed by Back Stepping Service, handle here.
                appUi->ActivateLocalViewL( viewStack.Peek() );
                }
        	}
         	break;
        default:
            appUi->HandleCommandL(aCommand);
            break;
        }
    IRLOG_DEBUG( "CIRBaseView::HandleCommandL - Exiting" );   
    }

// ---------------------------------------------------------
// CIRBaseView::HandleForegroundEventL
// ---------------------------------------------------------
//
void CIRBaseView::HandleForegroundEventL( TBool aForeground )
    {
    IRLOG_DEBUG( "CIRBaseView::HandleForegroundEventL - Entering" );   
    CAknView::HandleForegroundEventL(aForeground);
    IRLOG_DEBUG( "CIRBaseView::HandleForegroundEventL - Exiting" );
    }


// ---------------------------------------------------------
// From class MIRSystemEventObserver.
// CIRBaseView::HandleSystemEventL
// ---------------------------------------------------------
//
void CIRBaseView::HandleSystemEventL(TIRSystemEventType /*aEventType*/)
	{
	IRLOG_DEBUG( "CIRBaseView::HandleSystemEventL" );
	}

// ---------------------------------------------------------
// From class MIRSystemEventObserver.
// CIRBaseView::HandleSystemEventL
// ---------------------------------------------------------
//
void CIRBaseView::HandleViewStackL(TUid aCustomMessageId )
    {
    IRLOG_DEBUG( "CIRBaseView::HandleViewStackL - Entering" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
    CIRViewStack& viewStack = appUi->ViewStack();
    
    if( viewStack.Peek() == Id() && !(aCustomMessageId==KHistoryLaunch ) 
                                 && !(aCustomMessageId==KMainViewLaunch)
                                 && !(aCustomMessageId==KNowPlayingLaunch)
                                 && !(aCustomMessageId==KNowPlayingFrmMainView))
        {
	    viewStack.Pop();
        }
    else if( appUi->ActiveView() != Id() )
        {
        viewStack.PushL( appUi->ActiveView() );
        }
    else
        {
        // DONE
        }
    IRLOG_DEBUG( "CIRBaseView::HandleViewStackL - Exiting" );
    }

// ---------------------------------------------------------
// CIRBaseView::GetCurrentFocus
// ---------------------------------------------------------
//
TInt CIRBaseView::GetCurrentFocus()
	{
	IRLOG_DEBUG( "CIRBaseView::GetCurrentFocus" );
	return iCurrentFocus;
	}

// ---------------------------------------------------------
// CIRBaseView::GetCurrentTopItemInstance
// ---------------------------------------------------------
//
TInt CIRBaseView::GetCurrentTopItem()
	{
	IRLOG_DEBUG( "CIRBaseView::GetCurrentTopItem" );
	return iTopItemIndex;
	}

// ---------------------------------------------------------
// CIRBaseView::SetCurrentFocus
// ---------------------------------------------------------
//
void CIRBaseView::SetCurrentFocus(TInt aCurrentFocus)
	{
	IRLOG_DEBUG( "CIRBaseView::SetCurrentFocus - Entering" );
	iCurrentFocus = aCurrentFocus;
	IRLOG_DEBUG( "CIRBaseView::SetCurrentFocus - Exiting" );
	}

// ---------------------------------------------------------
// CIRBaseView::SetCurrentTopItemInstance
// ---------------------------------------------------------
//
void CIRBaseView::SetCurrentTopItem(TInt aTopItemIndex)
	{
	IRLOG_DEBUG( "CIRBaseView::SetCurrentTopItem - Entering" );
	iTopItemIndex = aTopItemIndex;
	IRLOG_DEBUG( "CIRBaseView::SetCurrentTopItem - Exiting" );
	}

// End of file
