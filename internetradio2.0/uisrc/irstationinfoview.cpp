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
* Description:  CIRStationInfoView class implementation
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Thu Jul 3 16:30:00 2008 by Rohit
*  Ref:
*  Codescanner fixes
*
*  Version: 2, Fri May 16 16:00:00 2008 by Rohit
*  Ref:
*  Fixed the Back button issue to go back to previous view
*
*  Version: 1, Thu May 15 14:00:00 2008 by Rohit
*  Ref:
*  Created
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

#include <internetradio.rsg>
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irbaseview.h"
#include "irstationinfoview.h"
#include "irstationinfocontainer.h"
#include "irapplication.h"
#include "irsettings.h"
#include "irviewstack.h"
#include "irisdspreset.h"
#include "irpreset.h"
#include "irisdswrapper.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irlogodownloadengine.h"

const TInt KLogoXValue = 200;
const TInt KLogoYValue = 200;

// ---------------------------------------------------------------------------
// CIRStationInfoView::CIRStationInfoView()
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CIRStationInfoView::CIRStationInfoView():iContainer(NULL)
    {
    IRLOG_DEBUG( "CIRStationInfoView::CIRStationInfoView " );
    // Nothing here
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::ConstructL
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::ConstructL(const TRect & /*aRect*/)
    {
    IRLOG_DEBUG( "CIRStationInfoView::ConstructL - Entering " );
    CIRBaseView::ConstructL(R_IR_STATIONINFO);
    iStationPreset = CIRIsdsPreset::NewL();
    IRLOG_DEBUG( "CIRStationInfoView::ConstructL - Exiting " );
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRStationInfoView* CIRStationInfoView::NewL(const TRect& aRect)
    {
    IRLOG_DEBUG("CIRStationInfoView::NewL - Entering" );
    CIRStationInfoView* self = CIRStationInfoView::NewLC(aRect);
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRStationInfoView::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//

CIRStationInfoView* CIRStationInfoView::NewLC(const TRect & aRect)
    {

    IRLOG_DEBUG( "CIRStationInfoView::NewLC - Entering" );
    CIRStationInfoView* self = new (ELeave) CIRStationInfoView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    IRLOG_DEBUG( "CIRStationInfoView::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::~CIRStationInfoView()
// Destructor
// ---------------------------------------------------------------------------
//
CIRStationInfoView::~CIRStationInfoView()
    {
    IRLOG_DEBUG( "CIRStationInfoView::~CIRStationInfoView - Entering " );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }
    if(iStationPreset)
	    {
	    delete iStationPreset;
	    iStationPreset = NULL;
	    }
    IRLOG_DEBUG( "CIRStationInfoView::~CIRStationInfoView - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::Id
// Base class CAknView function
// Returns view id
// ---------------------------------------------------------------------------
//
TUid CIRStationInfoView::Id() const
    {
    IRLOG_DEBUG( "CIRStationInfoView::Id " );
    return KIRStationInfoViewId;
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::DoActivateL
// Base class CAknView function
// Handles view activation
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    IRLOG_DEBUG( "CIRStationInfoView::DoActivateL - Entering " );

    //Create the instance of the container
    iContainer = CIRStationInfoContainer::NewL(ClientRect(), this);
    iContainer->SetMopParent(this);
    AppUi()->AddToViewStackL( *this, iContainer );
    SetTitleL( R_IRAPP_STATIONINFO_TITLE );
	CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    HandleStatusPaneSizeChange();

    // Download logo for Isds channel
	if( iStationPreset->GetChannelType() == CIRIsdsPreset::EIsdsPreset
		&& iStationPreset->GetImgUrl().Length() )
		{
		// Issue logo download request
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
			GetLogoDownloadEngine()->SendRequestL(iStationPreset, this,0,KLogoXValue,KLogoYValue);
		}

	// Populate container without logo
	if(iContainer)
		{
		iContainer->ReloadStationInformationL();	
		}

    IRLOG_DEBUG( "CIRStationInfoView::DoActivateL - Exiting " );
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::DoDeactivate
// Base class CAknView function
// Handles view deactivation
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::DoDeactivate()
    {
    IRLOG_DEBUG( "CIRStationInfoView::DoDeactivate - Entering " );
    CIRBaseView::DoDeactivate();

    CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

    if ( iContainer )
        {
        appUi->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
        delete iContainer;
        iContainer = NULL;
        }
	IRLOG_DEBUG( "CIRStationInfoView::DoDeactivate - Exiting " );
    }

// ---------------------------------------------------------------------------
// CIRStationInfoView::HandleCommandL
// Base class CAknView function
// Command handling function
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::HandleCommandL( TInt aCommand )
{
	IRLOG_DEBUG( "CIRStationInfoView::HandleCommandL - Entering " );
  	switch(aCommand)
	{
	case EAknSoftkeyClose:
	case EIRBack:
		CIRBaseView::HandleCommandL( EIRBack );
		break;

    default:
		AppUi()->HandleCommandL(aCommand);
		break;
	}
  	IRLOG_DEBUG( "CIRStationInfoView::HandleCommandL - Exiting " );
}

// ---------------------------------------------------------------------------
// CIRStationInfoView::HandleStatusPaneSizeChange
// Base class CAknView function
// Event handler for status pane size changes
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::HandleStatusPaneSizeChange()
    {
    IRLOG_DEBUG( "CIRStationInfoView::HandleStatusPaneSizeChange - Entering " );
    CIRBaseView::HandleStatusPaneSizeChange();
    // the client rect is also set here after screen orientation change
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    IRLOG_DEBUG( "CIRStationInfoView::HandleStatusPaneSizeChange - Exiting " );
    }


// ---------------------------------------------------------------------------
// CIRStationInfoView::PresetLogoDownloadedL
// From MLogoDownloadObserver
// Invoked after requested logo is downloaded
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRStationInfoView::PresetLogoDownloadedL - Entering" );

	if( !aPreset )
		{
		IRLOG_DEBUG( "CIRStationInfoView::PresetLogoDownloadedL - NULL Preset is passed" );
		User::LeaveIfNull( aPreset );
		}

	*iStationPreset = *aPreset;

	if(iContainer)
		iContainer->StartConvertStationLogoL();

	IRLOG_DEBUG( "CIRStationInfoView::PresetLogoDownloadedL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRStationInfoView::PresetLogoDownloadError
// From MLogoDownloadObserver
// Invoked if an error has occured while logo is downloading or no logo data available
// ---------------------------------------------------------------------------
//
void CIRStationInfoView::PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/ )
	{
	IRLOG_DEBUG( "CIRStationInfoView::PresetLogoDownloadError" );
	}

// -----------------------------------------------------------------------------
// CIRStationInfoView::SetStationPresetL
// Copies the given preset to member iStationPreset for display
// -----------------------------------------------------------------------------
//
void CIRStationInfoView::SetStationPresetL( CIRIsdsPreset* aPreset )
	{
	IRLOG_DEBUG( "CIRStationInfoView::SetStationPresetL - Entering " );
	if( !aPreset )
		{
		IRLOG_DEBUG( "CIRStationInfoView::PresetLogoDownloadedL - NULL Preset is passed" );
		User::LeaveIfNull( aPreset );
		}
	*iStationPreset = *aPreset;
	IRLOG_DEBUG( "CIRStationInfoView::SetStationPresetL - Exiting " );
	}

// -----------------------------------------------------------------------------
// CIRStationInfoView::SetStationPresetL
// Copies the given preset to member iStationPreset for display
// -----------------------------------------------------------------------------
//
void CIRStationInfoView::SetStationPresetL( CIRPreset* aPreset )
	{
	IRLOG_DEBUG( "CIRStationInfoView::SetStationPresetL - Entering " );
	if( !aPreset )
		{
		IRLOG_DEBUG( "CIRStationInfoView::PresetLogoDownloadedL - NULL Preset is passed" );
		User::LeaveIfNull( aPreset );
		}
	aPreset->CopyPresetData( *iStationPreset );
	IRLOG_DEBUG( "CIRStationInfoView::SetStationPresetL - Exiting " );
	}

// -----------------------------------------------------------------------------
// CIRStationInfoView::GetStationPresetL
// 
// -----------------------------------------------------------------------------
//
CIRIsdsPreset* CIRStationInfoView::GetStationPresetL( )
	{
	return iStationPreset;
	}

