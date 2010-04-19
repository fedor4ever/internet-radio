/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  View class for Settings
*
*/


#include <akntitle.h>
#include "irsettingscontainer.h"
#include "irsettingsview.h"
#include "ircommon.h"
#include "irdebug.h"
#include <internetradio.rsg>
#include "irui.h"
#include "irnetworkcontroller.h"
#include <stringloader.h>
#include "irviewstack.h"
#include "ir.hrh"
#include "irnowplayingwrapper.h"


// -----------------------------------------------------------------------------
// CIRSettingsView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIRSettingsView* CIRSettingsView::NewL()
    {
 	IRLOG_DEBUG( "CIRSettingsView::NewL" );
 	CIRSettingsView* self = CIRSettingsView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRSettingsView::NewL - Exiting" );
    return self;
    }
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRSettingsView* CIRSettingsView::NewLC( )
    {
	IRLOG_DEBUG( "CIRSettingsView::NewLC - Entering" );
	CIRSettingsView* self = new ( ELeave ) CIRSettingsView();
	CleanupStack::PushL( self );
	self->ConstructL();
	IRLOG_DEBUG( "CIRSettingsView::NewLC - Exiting" );
	return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CIRSettingsView::CIRSettingsView()
    {
    IRLOG_DEBUG( "CIRSettingsView::CIRSettingsView" );
    }
    
// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CIRSettingsView::ConstructL()
    {
	IRLOG_DEBUG( "CIRSettingsView::ConstructL - Entering" );
	BaseConstructL();
	IRLOG_DEBUG( "CIRSettingsView::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRSettingsView::~CIRSettingsView()
    {
    IRLOG_DEBUG( "CIRSettingsView::~CIRSettingsView" );
    }

// ---------------------------------------------------------------------------
// Getter for the view id
// ---------------------------------------------------------------------------
//
TUid CIRSettingsView::Id() const
    {
    IRLOG_DEBUG( "CIRSettingsView::Id" );
    return KIRSettingsViewId;
    }

// ---------------------------------------------------------------------------
// View activation
// ---------------------------------------------------------------------------
//
void CIRSettingsView::DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
					 const TDesC8& aCustomMessage )
    {
	IRLOG_DEBUG( "CIRSettingsView::DoActivateL - Entering." );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	CIRSettingsViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
	if(appUi->IsLandscapeOrientation())
		{
		StatusPane()->SwitchLayoutL(R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL);
		}
	else
		{
		StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT);
		}
	SetTitleL(R_IRAPP_SETTINGS);
	if(aCustomMessageId==TUid::Uid(1))
		{
		return;
		}
	HandleViewStackL( );
	IRLOG_DEBUG( "CIRSettingsView::DoActivateL - Exiting." );
    }
    
// ---------------------------------------------------------------------------
// HandleForegroundEventL()
// Handles changes in keyboard focus when an application switches to foreground
// ---------------------------------------------------------------------------
// 
     
void CIRSettingsView::HandleForegroundEventL( TBool aForeground )
{
	IRLOG_DEBUG( "CIRSettingsView::HandleForegroundEventL - Entering." );
	CIRSettingsViewBase::HandleForegroundEventL(aForeground);
	IRLOG_DEBUG( "CIRSettingsView::HandleForegroundEventL - Exiting." );
}


// ---------------------------------------------------------------------------
// HandleViewStackL()
// Handles the view stack
// ---------------------------------------------------------------------------
// 
 
void CIRSettingsView::HandleViewStackL( )
    {
    IRLOG_DEBUG( "CIRSettingsView::HandleViewStackL - Entering." );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
    CIRViewStack& viewStack = appUi->ViewStack();
    
    if( viewStack.Peek() == Id() )
        {
        viewStack.Pop( );
        }
    else if( appUi->ActiveView() != Id() )
        {
        viewStack.PushL( appUi->ActiveView() );
        }
    else
        {
        // DONE
        }
    IRLOG_DEBUG( "CIRSettingsView::HandleViewStackL - Exiting." );
    }
    
    
// ---------------------------------------------------------------------------
// View deactivation
// ---------------------------------------------------------------------------
//
void CIRSettingsView::DoDeactivate()
    {
	IRLOG_DEBUG( "CIRSettingsView::DoDeactivate - Entering." );
	CIRSettingsViewBase::DoDeactivate();
	IRLOG_DEBUG( "CIRSettingsView::DoDeactivate - Exiting." );
    }

// ---------------------------------------------------------------------------
// System event handler
// ---------------------------------------------------------------------------
//

void CIRSettingsView::HandleSystemEventL(TIRSystemEventType aEventType)
    {
    IRLOG_DEBUG( "CIRSettingsView::HandleSystemEventL - Entering." );
	switch (aEventType)
		{
		case EIRCallActivated:
			{
			if (iContainer)
				{
				static_cast<CIRSettingsContainer*>(iContainer)->SetFaded(ETrue);
				Cba()->AddCommandSetToStackL(R_IR_SOFTKEYS_EXIT);
				Cba()->DrawDeferred();
				}
			break;
			}
		case EIRCallDeactivated:
			{
			if (iContainer)
				{
				//do not unfade the window if menu or dialog is displayed
				if(!AppUi()->IsDisplayingMenuOrDialog() && AppUi()->IsForeground()) 
					{
					static_cast<CIRSettingsContainer*>(iContainer)->SetFaded(EFalse);
					}
				Cba()->RemoveCommandFromStack(0, 0);
				Cba()->RemoveCommandFromStack(2, EAknSoftkeyExit);
				Cba()->DrawDeferred();
				}
			break;
			}
		default:
			{
			break;
			}
		}
		IRLOG_DEBUG( "CIRSettingsView::HandleSystemEventL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Command handler.
// ---------------------------------------------------------------------------
//
void CIRSettingsView::HandleCommandL( TInt aCommand )
    {
    IRLOG_DEBUG( "CIRSettingsView::HandleCommandL - Entering." );
    CIRUi* appUi = static_cast<	CIRUi*>(AppUi());

		switch ( aCommand )
			{
			case EGotoNowPlayingViewCmd:
				{
				appUi->SetHistory(EFalse);
				appUi->SavePrevIDL(KIRSettingsViewId, aCommand);
				break; 			
				}

			case EAknSoftkeyBack:
		  	case EIRBack:
		   		{
	    		CIRViewStack& viewStack = appUi->ViewStack();
	        	// Check if Back Stepping Service consumes the command.
	        	if ( !static_cast< CIRUi* >( AppUi() )->ForwardBackCommandToBsWrapperL() )
	                {
	                // Command not consumed by Back Stepping Service, handle here.
	                appUi->ActivateLocalViewL( viewStack.Peek() );
	                }
		   		}
     		break;
         	
			default:
				CIRSettingsViewBase::HandleCommandL( aCommand );
			break;
			}
		IRLOG_DEBUG( "CIRSettingsView::HandleCommandL - Exiting." );
    }

// ---------------------------------------------------------------------------
// Constructs new container
// ---------------------------------------------------------------------------
//
void CIRSettingsView::CreateContainerL()
    {
    	IRLOG_DEBUG( "CIRSettingsView::CreateContainerL - Entering." );
       	//TInt defaultAccessPoint = 0; Just for PC-Lint commets :: SPP
   		if( !iContainer )
		{
		iContainer = CIRSettingsContainer::NewL(*this);
		}
   		IRLOG_DEBUG( "CIRSettingsView::CreateContainerL - Exiting." );
    }



// ---------------------------------------------------------------------------
// void CIRSettingsView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRSettingsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRSettingsView::DynInitMenuPaneL - Entering" );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_MENU_SETTINGS)
			{				
			// Now Playing option
			if(appUi->iNowPlayingWrapper->iPlaying)
				{
				 aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,EFalse);
				}
			else
				{
				 aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,ETrue);
				}
			}
		}
	IRLOG_DEBUG( "CIRSettingsView::DynInitMenuPaneL - Exiting" );
	}
// ======== GLOBAL FUNCTIONS ========
