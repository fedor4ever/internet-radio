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
* Description:  Channel list view class
*
*/


#include <akniconarray.h>
#include <akntitle.h>
#include <eikmenup.h>
#include <gulicon.h>
#include <stringloader.h>
#include <hlplch.h>
#include <aknappui.h>

//For Touch Toolbar
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <aknbutton.h>
#include <internetradio.mbg>
#include <internetradio.rsg>

#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irsettings.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irapplication.h"
#include "iraddmanuallystationview.h"
#include "iraddmanuallystationcontainer.h"
#include "irviewstack.h"
#include "irmainview.h"
#include "irnowplayingwrapper.h"


const TInt KWaitTimeInMicroSeconds = 100000;

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRAddManuallyStationView::CIRAddManuallyStationView() 
    {
    IRLOG_DEBUG( "CIRAddManuallyStationView::CIRAddManuallyStationView " );
    }


// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRAddManuallyStationView::ConstructL(const TRect& /*aRect*/)
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::ConstructL - Begin " );
    CIRBaseView::ConstructL(R_IR_ADDURL);
	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
	TRgb color;
	TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
				                                      EAknsCIQsnTextColorsCG6 );
	if(color.Gray2()!=0)
		{
		CreateAndSetToolbarL(R_IR_ADDMANUALLY_WHITE_TOOLBAR);
		}
	else
		{
		CreateAndSetToolbarL(R_IR_ADDMANUALLY_BLACK_TOOLBAR);	
		}
	if( AknLayoutUtils::PenEnabled() && Toolbar() )
		{
		Toolbar()->SetToolbarObserver( this );
		}
	
    IRLOG_DEBUG( "CIRAddManuallyStationView::ConstructL - Exiting " );
    }


CIRAddManuallyStationView* CIRAddManuallyStationView::NewL(const TRect& aRect)
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::NewL - Begin " );
	CIRAddManuallyStationView* self = CIRAddManuallyStationView::NewLC(aRect);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRAddManuallyStationView::NewL - Exiting " );
	return self;
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRAddManuallyStationView* CIRAddManuallyStationView::NewLC(const TRect& aRect)
    {
    IRLOG_DEBUG( "CIRAddManuallyStationView::NewLC - Begin " );
    CIRAddManuallyStationView* self = new( ELeave ) CIRAddManuallyStationView;
    CleanupStack::PushL( self );
    self->ConstructL(aRect);
    IRLOG_DEBUG( "CIRAddManuallyStationView::NewLC - Exiting " );
    return self;
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRAddManuallyStationView::~CIRAddManuallyStationView()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::~CIRAddManuallyStationView - Begin " );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	if ( Toolbar() )
		{
		Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	if(iAddStationsTimer)
		{
		iAddStationsTimer->StopTimer();
		delete iAddStationsTimer;
		iAddStationsTimer = NULL;
		}
	CActiveScheduler::Delete(iAddStationsTimer);
	IRLOG_DEBUG( "CIRAddManuallyStationView::~CIRAddManuallyStationView - Exiting " );
	}

// ---------------------------------------------------------------------------
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRAddManuallyStationView::Id() const
    {
    IRLOG_DEBUG( "CIRAddManuallyStationView::Id " );
    return KIRAddManuallyStationViewID;
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRAddManuallyStationView::DoActivateL( const TVwsViewId& aPrevViewId, 
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::DoActivateL - Begin " ); 
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	appUi->iPresentViewID = KIRAddManuallyStationViewID;
 	ConstructToolbarL(); 
	Toolbar()->SetToolbarVisibility(ETrue, EFalse);
	//save the previous view id
	iPrevViewId = aPrevViewId;
	
//	SetAddStationsTitleL()
	

	if ( !iContainer )
		{
		iContainer =CIRAddManuallyStationContainer::NewL(this);
		iContainer->ConstructL(R_FORM_MENUBAR);
		}
	else
		{
		iContainer->SetRect(ClientRect());
		iContainer->HandleResourceChange(KEikDynamicLayoutVariantSwitch);			
		}
	
	iAddStationsTimer = CIRAddStationsTimer::NewL(*iContainer);
	CActiveScheduler::Add(iAddStationsTimer);

    iAddStationsTimer->ExecuteL();
 	CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
	IRLOG_DEBUG( "CIRAddManuallyStationView::DoActivateL - Exiting " );
	}


// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRAddManuallyStationView::DoDeactivate()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::DoDeactivate - Begin " );
	if ( iContainer )
		{
		if(iUi->ActiveView() == KIRAddManuallyStationViewID)
			{
			delete iContainer;
			}
		iContainer = NULL;
		}
	if ( Toolbar() )
		{
	    Toolbar()->SetToolbarVisibility(EFalse, EFalse);
		Toolbar()->SetToolbarObserver( NULL );
		}
	if(iAddStationsTimer)
		{
		iAddStationsTimer->StopTimer();
		delete iAddStationsTimer;
		iAddStationsTimer = NULL;
		}
	IRLOG_DEBUG( "CIRAddManuallyStationView::DoDeactivate - Exiting " );
	}


// ---------------------------------------------------------------------------
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRAddManuallyStationView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::HandleCommandL - Begin " );
	CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
	CIRViewStack& viewStack = appUi->ViewStack();

	switch ( aCommand )
	    {
	   	   
	    case EGotoNowPlayingViewCmd:
	   		{
	   		appUi->SetHistory(EFalse);
	  		appUi->SavePrevIDL(KIRAddManuallyStationViewID, aCommand); 
	        break;
	        }
	    case EHelpCmd:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
	        	HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
	        	AppUi()->AppHelpContextL() );
				}
	        break;
			}
		case EAknSoftkeyBack:
	    	{
			if((appUi->IsFirstTime()))
				{
				CIRBaseView::HandleCommandL( aCommand );	                
				}
			else
				{
				TInt count = (viewStack.Count());
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
	        break;
	        }
	    }
	IRLOG_DEBUG( "CIRAddManuallyStationView::HandleCommandL - Exiting " );
	}




// ---------------------------------------------------------------------------
//  CIRAddManuallyStationView::ConstructToolbarL 
//  To Create Touch Toolbar
// ---------------------------------------------------------------------------

void CIRAddManuallyStationView::ConstructToolbarL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::ConstructToolbarL - Begin " );
	
	if(Toolbar())
		{
	    Toolbar()->SetItemDimmed(EButton1,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton3,EFalse,ETrue);	
	    
	    if (iIsTextEntered )
			{
			Toolbar()->SetItemDimmed(EButton2,EFalse,ETrue);
			}
		else
			{
			if (iUi->GetEditStation())
				{
				if (iContainer)
					{
					if (!iContainer->iIsEnable)
						{
						Toolbar()->SetItemDimmed(EButton2,ETrue,ETrue);							
						}
					else
						{
						Toolbar()->SetItemDimmed(EButton2,EFalse,ETrue);				
						}
					}
				}
			else 
				{
				Toolbar()->SetItemDimmed(EButton2,ETrue,ETrue);
				}
			}
	    	   
		Toolbar()->SetToolbarVisibility( ETrue );
		}
	IRLOG_DEBUG( "CIRAddManuallyStationView::ConstructToolbarL - Exiting " );
	}

// ---------------------------------------------------------------------------
//  CIRAddManuallyStationView::DynInitToolbarL 
// ---------------------------------------------------------------------------

void CIRAddManuallyStationView::DynInitToolbarL( TInt /*aResourceId*/, CAknToolbar* /*aToolbar*/ )
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::DynInitToolbarL - Begin " );
	/*if ( !aToolbar ) User::Leave(KErrNotFound);
		{

		}*/
	IRLOG_DEBUG( "CIRAddManuallyStationView::DynInitToolbarL - Exiting " );
	}
	
// --------------------------------------------------------------------------
// CIRAddManuallyStationView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CIRAddManuallyStationView::OfferToolbarEventL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::OfferToolbarEventL - Begin " );
	switch(aCommand)
		{
		
		case EButton1:
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
	         	iContainer->CopyDataL();
				}
            break;
      
        case EButton2:
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
	         	iContainer->SaveDataL();
				}
            break;
        case EButton3:
        	iContainer->PasteDataL();
        	break;
	
        default:
            break;
	        
		}
	IRLOG_DEBUG( "CIRAddManuallyStationView::OfferToolbarEventL - Exiting " );
	}

// --------------------------------------------------------------------------
// CIRAddManuallyStationView::SetToolbarItems
// To dim/undim the touch toolbar buttons
// --------------------------------------------------------------------------

void CIRAddManuallyStationView::SetToolbarItems()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::SetToolbarItems - Begin " );
	CIRUi* appUi = static_cast<CIRUi*>( iCoeEnv->AppUi( ) );
  	  
	if(iIsTextEntered )
	    {
	    Toolbar()->SetItemDimmed(EButton1,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton2,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton3,EFalse,ETrue);	
	
	    }
	else if (appUi->GetEditStation() && iIsTextEntered)
		{
	    Toolbar()->SetItemDimmed(EButton1,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton2,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton3,EFalse,ETrue) ;	
		}
		
	else if(iIsCopied)
	    {
	    Toolbar()->SetItemDimmed(EButton1,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton2,ETrue,ETrue);
	    Toolbar()->SetItemDimmed(EButton3,EFalse,ETrue)	;
	    }
	else
	  {
	    Toolbar()->SetItemDimmed(EButton1,EFalse,ETrue);
	    Toolbar()->SetItemDimmed(EButton2,ETrue,ETrue);
	    Toolbar()->SetItemDimmed(EButton3,EFalse,ETrue);	
	  }
	Toolbar()->DrawNow();
	iIsCopied=EFalse; 
	IRLOG_DEBUG( "CIRAddManuallyStationView::SetToolbarItems - Exiting " );  
	}

// --------------------------------------------------------------------------
// CIRAddManuallyStationView::SetAddStationsTitleL
// Sets the Title
// --------------------------------------------------------------------------
void CIRAddManuallyStationView::SetAddStationsTitleL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::SetAddStationsTitleL - Begin " );
	if (!iUi->GetEditStation())
		{
		SetTitleL(R_IRAPP_ADDMANUALLY_TITLE);
		}
	else
		{
		SetTitleL(R_IRAPP_EDITSTATION_TITLE);	
		}
	IRLOG_DEBUG( "CIRAddManuallyStationView::SetAddStationsTitleL - Exiting " );
	}


// --------------------------------------------------------------------------
// CIRAddStationTimer::CIRAddStationTimer
// --------------------------------------------------------------------------

CIRAddStationsTimer::CIRAddStationsTimer(CIRAddManuallyStationContainer& aContainer): 
					 CActive(CActive::EPriorityStandard),iContainer(aContainer)
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::CIRAddStationsTimer - Begin " );
	iCounter=0;
	IRLOG_DEBUG( "CIRAddManuallyStationView::CIRAddStationsTimer - Exiting " );
	}

// --------------------------------------------------------------------------
// CIRAddStationTimer::~CIRAddStationTimer
// --------------------------------------------------------------------------
CIRAddStationsTimer::~CIRAddStationsTimer()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::~CIRAddStationsTimer - Begin " );
	// Make sure we're cancelled
	Cancel();
	iTimer.Close(); // Close the ASP
	IRLOG_DEBUG( "CIRAddManuallyStationView::~CIRAddStationsTimer - Exiting " );
	}

// --------------------------------------------------------------------------
// CIRAddStationTimer::NewL
// --------------------------------------------------------------------------
CIRAddStationsTimer* CIRAddStationsTimer::NewL(CIRAddManuallyStationContainer& aContainer)
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::NewL - Begin " );
	CIRAddStationsTimer* self = new (ELeave) CIRAddStationsTimer(aContainer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRAddManuallyStationView::NewL - Exiting " );
	return self;
	}   

// --------------------------------------------------------------------------
// CIRAddStationTimer::ConstructL
// --------------------------------------------------------------------------
void CIRAddStationsTimer::ConstructL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::ConstructL - Begin " );
	iTimer.CreateLocal();
	IRLOG_DEBUG( "CIRAddManuallyStationView::ConstructL - Exiting " );
	return;

	} 

// --------------------------------------------------------------------------
// CIRAddStationTimer::IssueRequest
// --------------------------------------------------------------------------
void CIRAddStationsTimer::IssueRequest()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::IssueRequest - Begin " );
	// Request another wait
	iStatus = KRequestPending;
	if(!IsActive())
		{
		iTimer.After( iStatus, KWaitTimeInMicroSeconds);
		SetActive();
		}
	IRLOG_DEBUG( "CIRAddManuallyStationView::IssueRequest - Exiting " );
	} 

// --------------------------------------------------------------------------
// CIRAddStationTimer::DoCancel
// --------------------------------------------------------------------------
void CIRAddStationsTimer::DoCancel()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::DoCancel - Begin " );
	iTimer.Cancel(); // Close the ASP
	IRLOG_DEBUG( "CIRAddManuallyStationView::DoCancel - Exiting " );
	}

// --------------------------------------------------------------------------
// CIRAddStationTimer::RunL
// --------------------------------------------------------------------------
void CIRAddStationsTimer::RunL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::RunL - Begin " );
	iContainer.ExecuteLD( R_ADD_STATION_FORM_DIALOG );
	IRLOG_DEBUG( "CIRAddManuallyStationView::RunL - Exiting " );
	}
   
// --------------------------------------------------------------------------
// CIRAddStationTimer::ExecuteL
// --------------------------------------------------------------------------
void CIRAddStationsTimer::ExecuteL()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::ExecuteL - Begin " );
	IssueRequest();
	IRLOG_DEBUG( "CIRAddManuallyStationView::ExecuteL - Exiting " );
	return;
	}

// --------------------------------------------------------------------------
// CIRAddStationTimer::StopTimer
// --------------------------------------------------------------------------
void CIRAddStationsTimer::StopTimer()
	{
	IRLOG_DEBUG( "CIRAddManuallyStationView::StopTimer - Begin " );
	iTimer.Cancel();
	IRLOG_DEBUG( "CIRAddManuallyStationView::StopTimer - Exiting " );
	}	
	
// ---------------------------------------------------------------------------
// From class CAknView.
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CIRAddManuallyStationView::HandleForegroundEventL( TBool aForeground )
    {
	IRLOG_DEBUG( "CIRAddManuallyStationView::HandleForegroundEventL - Entering" );
	CIRBaseView::HandleForegroundEventL( aForeground );

	if ( aForeground )
		{
		if(iContainer)
			{
			MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;
			TRgb color;
			TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
						                                      EAknsCIQsnTextColorsCG6 );
			if(color.Gray2()!=0)
				{
				CreateAndSetToolbarL(R_IR_ADDMANUALLY_WHITE_TOOLBAR);
				}
			else
				{
				CreateAndSetToolbarL(R_IR_ADDMANUALLY_BLACK_TOOLBAR);	
				}
			if( AknLayoutUtils::PenEnabled() && Toolbar() )
				{
				Toolbar()->SetToolbarObserver( this );
				}
			ConstructToolbarL();
			}
		}
	IRLOG_DEBUG( "CIRAddManuallyStationView::HandleForegroundEventL" );
    }
	


void CIRAddManuallyStationView::RefreshView()
	{
	if (iContainer)
		{
		iContainer->SetRect(ClientRect());		
		}
	}
