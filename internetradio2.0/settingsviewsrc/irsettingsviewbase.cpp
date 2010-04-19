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
* Description:  Base class for the Settings View
*
*/


#include <bldvariant.hrh>
#include <featmgr.h>
#include <hlplch.h>
#include <pathinfo.h>
#include <stringloader.h>
#include <bautils.h>
#include "ir.hrh"
#include "ircommonsettingscontainer.h"
#include "irsettingsviewbase.h"
#include "irdebug.h"
#include <internetradio.rsg>
#include <aknappui.h>
#include <akntitle.h> 




// Settings view resource file.
_LIT( KIRSettingsViewResourceFile, "apps\\internetradio.rsc" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSettingsViewBase::CIRSettingsViewBase()
    : iSettingsBaseResourceLoader( *iCoeEnv )
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::CIRSettingsViewBase " );
    }
    
// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettingsViewBase::BaseConstructL()
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::BaseConstructL - Entering " );
    FeatureManager::InitializeLibL();
    LoadResourcesL( iSettingsBaseResourceLoader, KIRSettingsViewResourceFile );
    CAknView::BaseConstructL( R_IR_SETTINGSVIEW );
    IRLOG_DEBUG( "CIRSettingsViewBase::BaseConstructL - Exiting " );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSettingsViewBase::~CIRSettingsViewBase()
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::~CIRSettingsViewBase - Entering " );
	DeleteContainer();
    FeatureManager::UnInitializeLib();
    iSettingsBaseResourceLoader.Close();
    IRLOG_DEBUG( "CIRSettingsViewBase::~CIRSettingsViewBase - Exiting " );
    }

// ---------------------------------------------------------------------------
// LoadResourcesL()
// Loads the required resources.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettingsViewBase::LoadResourcesL( RConeResourceLoader& aResourceLoader, 
                                                   const TDesC& aFileName )
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::LoadResourcesL - Entering " );
    TFileName resourceFileName( KDC_RESOURCE_FILES_DIR );
    resourceFileName.Append(aFileName);
	   
    ResolveDriveL( iCoeEnv->FsSession(), resourceFileName );

    aResourceLoader.OpenL( resourceFileName );
    IRLOG_DEBUG( "CIRSettingsViewBase::LoadResourcesL - Exiting " );
    }

// ---------------------------------------------------------------------------
// DoActivateL()
// View activation
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettingsViewBase::DoActivateL( const TVwsViewId& aPrevViewId,
				 TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::DoActivateL - Entering." );
    
    CreateContainerL();
    
    StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
    
    iContainer->SetMopParent( this );
    AppUi()->AddToViewStackL( *this, iContainer );
    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
    
    iPrevViewUid = aPrevViewId.iViewUid;
    IRLOG_DEBUG( "CIRSettingsViewBase::DoActivateL - Exiting " );
    
	}

// ---------------------------------------------------------------------------
// DoDeactivate()
// View deactivation
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettingsViewBase::DoDeactivate()
    {
	IRLOG_DEBUG( "CIRSettingsViewBase::DoDeactivate - Entering " );
    DeleteContainer();
    IRLOG_DEBUG( "CIRSettingsViewBase::DoDeactivate - Exiting " );
    }

// ---------------------------------------------------------------------------
// HandleCommandL()
// Command handler.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettingsViewBase::HandleCommandL( TInt aCommand )
    {
    IRRDEBUG2( " *** Internet Radio -- CIRSettingsViewBase::HandleCommandL(%d)", aCommand );
    switch ( aCommand )
        {
		case EIRCmdChangeSetting: //Falls through
		case EAknSoftkeyChange:
			{
			TBool calledFromMenu = ( aCommand == EIRCmdChangeSetting);
			iContainer->EditCurrentItemL( calledFromMenu );
			break;
        	}
          
        case EHelpCmd:
        	{
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
            				 AppUi()->AppHelpContextL() );
        	}
            break;
            
        //Only if invoked from settings plugin
        case EAknSoftkeyBack:
		case EIRBack:
			{
			AppUi()->ActivateLocalViewL( iPrevViewUid );	
			}
			break;
		
		case EEikCmdExit:
		case EExitCmd:  //lint -fallthrough
        case EIRExit:
		case EAknSoftkeyExit:
			{
			AppUi()->HandleCommandL(aCommand);
			}
		
		break;
		
        default:
        	AppUi()->HandleCommandL(aCommand);
        break;
        }
	    IRLOG_DEBUG( "CIRSettingsViewBase::HandleCommandL - Exiting " );
    }
 
// ---------------------------------------------------------------------------
// HandleForegroundEventL()
// Handles changes in keyboard focus when an application switches to foreground
// ---------------------------------------------------------------------------
// 
 
 EXPORT_C void CIRSettingsViewBase::HandleForegroundEventL( TBool aForeground )
	 {
	 IRLOG_DEBUG( "CIRSettingsViewBase::HandleForegroundEventL - Entering " );
	 	if(aForeground)
	 		{
	 		iContainer->UpdateStatusL();
	 		}
	 	IRLOG_DEBUG( "CIRSettingsViewBase::HandleForegroundEventL - Exiting " );
	 }
 
 
// ---------------------------------------------------------------------------
// SetTitleL()
// Sets the Title of the View
// ---------------------------------------------------------------------------
// 
 
 EXPORT_C void CIRSettingsViewBase::SetTitleL( TInt aResourceId )
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::SetTitleL - Entering " );
    HBufC* title = StringLoader::LoadLC(aResourceId);
    static_cast<CAknTitlePane*>( StatusPane()->ControlL(
    			 TUid::Uid( EEikStatusPaneUidTitle ) ) )->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    IRLOG_DEBUG( "CIRSettingsViewBase::SetTitleL - Exiting " );
    }

// ---------------------------------------------------------------------------
// DeleteContainer()
// Deletes container
// ---------------------------------------------------------------------------
//
void CIRSettingsViewBase::DeleteContainer()
    {
	IRLOG_DEBUG( "CIRSettingsViewBase::DeleteContainer - Entering " );
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
    	}
    IRLOG_DEBUG( "CIRSettingsViewBase::DeleteContainer - Exiting " );
    }
    
// ---------------------------------------------------------------------------
// Static version of ResolveDriveL.
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettingsViewBase::ResolveDriveL( const RFs& aFs, TFileName& aFileName ) 
    {
    IRRDEBUG2( "CIRSettingsViewBase::ResolveDriveL( aFileName = %S )", &aFileName );

    _LIT( resourceFileExt, ".rsc" );
    
    TParsePtrC romDrive( PathInfo::RomRootPath() );
    TParsePtrC phoneDrive( PathInfo::PhoneMemoryRootPath() );
    TParsePtrC mmcDrive( PathInfo::MemoryCardRootPath() );
    
    TParse parse;
    User::LeaveIfError( parse.Set( mmcDrive.Drive(), NULL, &aFileName ) );
    
    TBool isResourceFile = (parse.Ext() == resourceFileExt());
    
    aFileName = parse.FullName();
    
    if ( isResourceFile )
        {
        BaflUtils::NearestLanguageFile( aFs, aFileName );
        }
    
    if ( !BaflUtils::FileExists( aFs, aFileName ) )
        { 
        User::LeaveIfError( parse.Set( phoneDrive.Drive(), NULL, &aFileName ) );
        aFileName = parse.FullName();
        
        if ( isResourceFile )
            {
            BaflUtils::NearestLanguageFile( aFs, aFileName );
            }
        
        if ( !BaflUtils::FileExists( aFs, aFileName ) )
            {
            User::LeaveIfError( parse.Set( romDrive.Drive(), NULL, &aFileName ) );
            aFileName = parse.FullName();
            
            if ( isResourceFile )
                {
                BaflUtils::NearestLanguageFile( aFs, aFileName );
                }
            
            if ( !BaflUtils::FileExists( aFs, aFileName ) )
                {
                IRRDEBUG2( "CVRSettings::ResolveDriveL - File %S not found!", &aFileName );
                User::Leave( KErrNotFound );
                }
            }
        }

    IRRDEBUG2( "CIRSettingsViewBase::ResolveDriveL( aFileName = %S )", &aFileName );
    }
    
    
// ---------------------------------------------------------------------------
// GetCaptionL()
// Gets the caption
// ---------------------------------------------------------------------------
//    

EXPORT_C void CIRSettingsViewBase::GetCaptionL( TDes& aCaption ) const
    {
    IRLOG_DEBUG( "CIRSettingsViewBase::GetCaptionL - Entering " );
    aCaption.Zero();
    IRLOG_DEBUG( "CIRSettingsViewBase::GetCaptionL - Exiting " );
	return;
    }
// ======== GLOBAL FUNCTIONS ========
