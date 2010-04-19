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
* Description:  view class for cost warning view
*
*/




#include <akntitle.h>
#include <stringloader.h>
#include <internetradio.rsg>


#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "ircostwarningview.h"
#include "ircostwarningcontainer.h"
#include "irapplication.h"
#include "irnetworkcontroller.h"
#include "irsettings.h"


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRCostWarningView::CIRCostWarningView():iContainer(NULL) 
    {
    IRLOG_DEBUG("CIRCostWarningView::CIRCostWarningView" );
    // Nothing here 
    }


// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRCostWarningView::ConstructL(const TRect & /*aRect*/)
    {
    IRLOG_DEBUG("CIRCostWarningView::ConstructL - Entering" );
    BaseConstructL(R_IR_TANDCVIEW);
    IRLOG_DEBUG("CIRCostWarningView::ConstructL - Exiting" );
    }
// ---------------------------------------------------------------------------
// Two-phased constructor.
// CIRCostWarningView::NewL
// ---------------------------------------------------------------------------
//
CIRCostWarningView* CIRCostWarningView::NewL(const TRect& aRect)
    {
    IRLOG_DEBUG("CIRCostWarningView::NewL - Entering" );
    CIRCostWarningView* self = CIRCostWarningView::NewLC(aRect);
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRCostWarningView::NewL - Exiting." );
    return self;
    }
// ---------------------------------------------------------------------------
// Two-phased constructor.
// CIRCostWarningView::NewLC
// ---------------------------------------------------------------------------
//
CIRCostWarningView* CIRCostWarningView::NewLC(const TRect & aRect)
    {
       
    IRLOG_DEBUG( "CIRCostWarningView::NewLC - Entering" );
    CIRCostWarningView* self = new (ELeave) CIRCostWarningView();
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    IRLOG_DEBUG( "CIRCostWarningView::NewLC - Exiting." );
    return self;
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRCostWarningView::~CIRCostWarningView()
    {
    IRLOG_DEBUG( "CIRCostWarningView::~CIRCostWarningView - Entering" );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }
    IRLOG_DEBUG( "CIRCostWarningView::~CIRCostWarningView - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CIRCostWarningView::Id() const
    {
    IRLOG_DEBUG( "CIRCostWarningView::Id" );
    return KIRCostWarningViewId;
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view activation.
// ---------------------------------------------------------------------------
//
void CIRCostWarningView::DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    IRLOG_DEBUG( "CIRCostWarningView::DoActivateL - Entering" );
    //Create the instance of the container
    CIRUi* ui = reinterpret_cast<CIRUi*>( AppUi() );
	ui->iPresentViewID = KIRCostWarningViewId;
    iContainer = CIRCostWarningContainer::NewL(ClientRect(),*this);
    HBufC* stringholder = StringLoader::LoadLC( R_IRAPP_COST_WARNING_TITLE );
    static_cast<CAknTitlePane*>( StatusPane()->
                                 ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) )->
                                 SetTextL( *stringholder );
    CleanupStack::PopAndDestroy(stringholder);
    iContainer->SetMopParent(this);
    AppUi()->AddToStackL( *this, iContainer ); 
    IRLOG_DEBUG( "CIRCostWarningView::DoActivateL - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Handles view deactivation.
// ---------------------------------------------------------------------------
//
void CIRCostWarningView::DoDeactivate()
    {
    IRLOG_DEBUG( "CIRCostWarningView::DoDeactivate - Exiting" );
    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );
   
    if ( iContainer )
        {
        appUi->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        delete iContainer;
        iContainer = NULL;
        }
    IRLOG_DEBUG( "CIRCostWarningView::DoDeactivate - Exiting" );
    }


// ---------------------------------------------------------------------------
// From class CAknView.
// Command handling function.
// ---------------------------------------------------------------------------
//
void CIRCostWarningView::HandleCommandL( TInt aCommand )
{
IRLOG_DEBUG( "CIRCostWarningView::HandleCommandL - Entering" );
  	switch(aCommand)
	{
	case EIRAccept:
     	{   
	    iContainer->TandCAcceptL();
        break;        
	    }
    case EIRContinue:
		{
		iContainer->CostWarningContinueL();
		break;		
		}
    default: 
		{
		AppUi()->HandleCommandL(aCommand);
		break;
		}
	}
  	IRLOG_DEBUG( "CIRCostWarningView::HandleCommandL - Exiting" );
}


// ---------------------------------------------------------------------------
// From class CAknView.
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CIRCostWarningView::HandleStatusPaneSizeChange()
    {
    IRLOG_DEBUG( "CIRCostWarningView::HandleStatusPaneSizeChange - Entering" );
    // the client rect is also set here after screen orientation change
    CAknView::HandleStatusPaneSizeChange();
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    IRLOG_DEBUG( "CIRCostWarningView::HandleStatusPaneSizeChange - Exiting" );
    }

