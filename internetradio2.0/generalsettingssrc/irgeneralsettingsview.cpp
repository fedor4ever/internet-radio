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
* Description:  Implementation of CIRGeneralSettingsView
*
*/


#include <akntitle.h>
#include "irgeneralsettingsview.h"
#include "irgeneralsettings.hrh"
#include "ircommonsettingscontainer.h"
#include "irdebug.h"
#include <irgeneralsettings.rsg>
#include <internetradio.mbg>
#include <stringloader.h>
#include <internetradio.rsg>
#include "ir.hrh"


// General settings plugin bitmap file.

_LIT(KIRMifFileName,"InternetRadio.mif");

// General settings plugin resource file.
_LIT( KIRGeneralSettingsResourceFile, "irgeneralsettings.rsc" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRGeneralSettingsView* CIRGeneralSettingsView::NewL( TAny* /*aInitParams*/ )
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView::NewL - entering");
    CIRGeneralSettingsView* self = CIRGeneralSettingsView::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( " CIRGeneralSettingsView::NewL - exiting");
    return self;
    }
 
// ---------------------------------------------------------------------------
// CIRGeneralSettingsView::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRGeneralSettingsView* CIRGeneralSettingsView::NewLC()
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView::NewLC - Entering" );
    CIRGeneralSettingsView* self = new( ELeave ) CIRGeneralSettingsView();
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRGeneralSettingsView::NewLC - Exiting" );
    return self;
    }
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CIRGeneralSettingsView::CIRGeneralSettingsView()
    : iGeneralSettingsResourceLoader( *iCoeEnv )
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView ::CIRGeneralSettingsView");
    }
    
// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CIRGeneralSettingsView::ConstructL()
    {
	IRLOG_DEBUG( "CIRGeneralSettingsView::ConstructL - entering");
    BaseConstructL();
    LoadResourcesL( iGeneralSettingsResourceLoader, KIRGeneralSettingsResourceFile );
    IRLOG_DEBUG( "CIRGeneralSettingsView::ConstructL - Exiting");
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRGeneralSettingsView::~CIRGeneralSettingsView()
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView::~CIRGeneralSettingsView - entering");
    iGeneralSettingsResourceLoader.Close();
    if(iContainer)
    	{
    	delete iContainer;
    	iContainer = NULL;
    	}
    IRLOG_DEBUG( "CIRGeneralSettingsView::~CIRGeneralSettingsView - Exiting");
    }

// ---------------------------------------------------------------------------
// Getter for the view id
// ---------------------------------------------------------------------------
//
TUid CIRGeneralSettingsView::Id() const
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView :: Id");
    return TUid::Uid( IRGENERAL_SETTINGS_PLUGIN_DLL_UID );
    }

// ---------------------------------------------------------------------------
// View activation
// ---------------------------------------------------------------------------
//
void CIRGeneralSettingsView::DoActivateL( const TVwsViewId& aPrevViewId, 
                                          TUid aCustomMessageId, 
                                          const TDesC8& aCustomMessage )
    {
	
    IRLOG_DEBUG( " CIRGeneralSettingsView::DoActivateL - Entering" );
    
    CIRSettingsViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );

    HBufC* title = NULL;
    title = iCoeEnv->AllocReadResourceLC( R_IRAPP_GENERAL_SETTINGS_PLUGIN_TITLE );
    static_cast<CAknTitlePane*>( AppUi()->StatusPane()->ControlL(
    			 TUid::Uid( EEikStatusPaneUidTitle ) ) )->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    IRLOG_DEBUG( " CIRGeneralSettingsView::DoActivateL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Getter for the caption of general settings
// ---------------------------------------------------------------------------
//
void CIRGeneralSettingsView::GetCaptionL( TDes& aCaption ) const
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView::GetCaptionL - Entering");
    StringLoader::Load(aCaption,R_IRAPP_GENERAL_SETTINGS_PLUGIN_LISTBOX_CAPTION);
    IRLOG_DEBUG( "CIRGeneralSettingsView::GetCaptionL - Exiting");
	
	return;
    }

// ---------------------------------------------------------------------------
// Getter for provider category
// ---------------------------------------------------------------------------
//
TInt CIRGeneralSettingsView::PluginProviderCategory() const
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView::PluginProviderCategory");
    return EGSPluginProviderOEM;
    }

// ---------------------------------------------------------------------------
// Creates icon for the general settings
// ---------------------------------------------------------------------------
//
CGulIcon* CIRGeneralSettingsView::CreateIconL( const TUid aIconType )
    {
	IRLOG_DEBUG( "CIRGeneralSettingsView::CreateIconL - Entering");
    if ( aIconType == KGSIconTypeLbxItem )
	    {
	    TParse parse;
		parse.Set( KIRMifFileName, &KDC_APP_RESOURCE_DIR, NULL );
		TFileName bitmapFile = parse.FullName();
		CompleteWithAppPath( bitmapFile );

	    CIRSettingsViewBase::ResolveDriveL( iCoeEnv->FsSession(), bitmapFile );

		IRLOG_DEBUG( "CIRGeneralSettingsView::CreateIconL - exiting");
	    return AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), 
	                                      KAknsIIDDefault, 
	                                      bitmapFile, 
	                                      EMbmInternetradioQgn_prop_set_apps_ir, 
	                                      EMbmInternetradioQgn_prop_set_apps_ir_mask );
	    }
    else
	    {
	    IRLOG_DEBUG( "CIRGeneralSettingsView::CreateIconL - exiting");
	    return CGSPluginInterface::CreateIconL( aIconType );
	    }
    }

// ---------------------------------------------------------------------------
// Getter for general settings view visibility status
// ---------------------------------------------------------------------------
//
TBool CIRGeneralSettingsView::Visible() const
    {
    IRLOG_DEBUG( "CIRGeneralSettingsView::Visible");
	return ETrue;
    }

// ---------------------------------------------------------------------------
// Constructs new container
// ---------------------------------------------------------------------------
//
void CIRGeneralSettingsView::CreateContainerL()
    {
	IRLOG_DEBUG( "CIRGeneralSettingsView::CreateContainer - entering");
    if( !iContainer )
        {
        iContainer = CIRCommonSettingsContainer::NewL();
        }
    IRLOG_DEBUG( "CIRGeneralSettingsView::CreateContainer - Exiting");
    }

// ---------------------------------------------------------------------------
// HandleCommandL()
// Command handler
// ---------------------------------------------------------------------------
//
void CIRGeneralSettingsView::HandleCommandL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRGeneralSettingsView::HandleCommandL - entering");
	
	switch ( aCommand )
		{
		case EEikCmdExit:
		case EExitCmd:  //lint -fallthrough
	    case EIRExit:
		case EAknSoftkeyExit:
			{
			AppUi()->Exit();
			}
		break;
				
		default:
			{
			CIRSettingsViewBase::HandleCommandL( aCommand );
			}
		break;
		}

	IRLOG_DEBUG( "CIRGeneralSettingsView::HandleCommandL - Exiting");
	}



// ---------------------------------------------------------------------------
// void CIRGeneralSettingsView::DynInitMenuPaneL()
// Dynamically initialises a menu pane
// ---------------------------------------------------------------------------
//
void CIRGeneralSettingsView::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRGeneralSettingsView::DynInitMenuPaneL - Entering" );

	if(!aMenuPane) User::Leave(KErrNotFound);
		{
		if(aResourceId == R_IR_MENU_SETTINGS)
			{				
			// Now Playing option
			 aMenuPane->SetItemDimmed(EGotoNowPlayingViewCmd,ETrue);
			}
		}
	IRLOG_DEBUG( "CIRGeneralSettingsView::DynInitMenuPaneL - Exiting" );
	}

// ======== GLOBAL FUNCTIONS ========
