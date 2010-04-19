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
* Description:  ?Description
*
*/


#include <aicontentobserver.h>
#include <aiutility.h>
#include <aknutils.h>
#include <apgcli.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <e32property.h>
#include <stringloader.h>
#include <iractiveidle.rsg>
#include <features.hrh>
#include <eikenv.h> 
#include <apgtask.h>
#include "irmetadata.h"
#include "irpubsubkeys.h"
#include "ir.hrh"
#include "iractiveidle.h"
#include "iractiveidleeng.h"
#include "iractiveidlestatedetector.h"
#include "irdebug.h"
#include "irplugincontentmodel.h"



#include <eikenv.h>

const TInt KIRMinVolumeLevel = 0;
const TInt KIRMaxVolumeLevel  = 10;
const TInt KIRPublishIndex = 1; // Always 1 in this plugin
const TInt KIRActiveIdleGranularity = 3;

_LIT( KIRActiveIdleResourceFileName, "iractiveidle.rsc" ); /** Filename of the Internet Radio Active Idle resource file. */
_LIT( KIRPluginName, "iRAPP Plugin");
_LIT(KSeparatorDot,". ");
//_LIT(KSeparatorHyphen," - ")
//_LIT(KSeparatorColon," : ")
_LIT8( KIRSpace, " " );
_LIT8( KIRCommandLineActiveIdle, "-a" );
_LIT8( KIRCommandLineActiveIdleCommandStartNowPlayingView, "startnowplaying" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRActiveIdle::CIRActiveIdle()
// Constructor.
// ---------------------------------------------------------------------------
//
CIRActiveIdle::CIRActiveIdle()
    {
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::ConstructL()
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::ConstructL()
    {
    IRLOG_DEBUG( "CIRActiveIdle::ConstructL" );
    InitializeResourceLoadingL();
    iInfo.iUid = KUidIRPlugin;
    iInfo.iName.Copy( KIRPluginName );
    iContent = AiUtility::CreateContentItemArrayIteratorL( KAiIRContent );
    iResources = AiUtility::CreateContentItemArrayIteratorL( KAiIRResources );
    iEvents  = AiUtility::CreateContentItemArrayIteratorL( KAiIREvents );
    iMetaData = CIRMetaData::NewL();
    iRunningState = EIRPSApplicationUninitialized;
    IRLOG_DEBUG( "CIRActiveIdle::ConstructL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRActiveIdle* CIRActiveIdle::NewL()
    {
    IRLOG_DEBUG( "CIRActiveIdle::NewL" );
    CIRActiveIdle* self = new( ELeave ) CIRActiveIdle;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRActiveIdle::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::~CIRActiveIdle()
// Destructor
// ---------------------------------------------------------------------------
//
CIRActiveIdle::~CIRActiveIdle()
    {
    IRLOG_DEBUG( "CIRActiveIdle::~CIRActiveIdle" );
    if ( iActiveIdleResourceOffset > 0 )
        {
        CCoeEnv::Static()->DeleteResourceFile( iActiveIdleResourceOffset );
        }
    iObservers.Close();
    Release( iContent );
    Release( iResources );
    Release( iEvents );

    delete iMetaData;
    delete iIRTerminationDetector;
    delete iEngine;
    IRLOG_DEBUG( "CIRActiveIdle::~CIRActiveIdle - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::InitializeResourceLoadingL()
// Initialize resource file for loading resources.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::InitializeResourceLoadingL()
    {
    IRLOG_DEBUG( "CIRActiveIdle::InitializeResourceLoadingL" );

    CCoeEnv* coeEnv = CCoeEnv::Static();
    RFs fs( coeEnv->FsSession() );
    TFileName fileName; 
    TFileName baseResource;
    TFindFile finder( fs );
    TLanguage language( ELangNone );
    
    TParsePtrC parse( KIRActiveIdleResourceFileName );
    _LIT( resourceFileWildExt, ".r*" );

    // Make sure to find all resource files, not only .rsc files as it may be so
    // that there is only .r01, .r02, etc. files available
    fileName.Copy( parse.Name() );
    fileName.Append( resourceFileWildExt );

    // TFindFile applies search order that is from 
    // drive Y to A, then Z
    CDir* entries = NULL;
    TInt err = finder.FindWildByDir( fileName , KDC_RESOURCE_FILES_DIR, entries );
    delete entries;
    entries = NULL;
    TBool found = EFalse;
    while ( !found && err == KErrNone )
        {
        // Found file
        fileName.Zero();
        TParsePtrC foundPath( finder.File() );
        fileName.Copy( foundPath.DriveAndPath() );
        fileName.Append( KIRActiveIdleResourceFileName );
        BaflUtils::NearestLanguageFile( fs, fileName, language );
        if ( language != ELangNone && BaflUtils::FileExists( fs, fileName ) )
            {
            found = ETrue;
            iActiveIdleResourceOffset = coeEnv->AddResourceFileL( fileName );
            }
        else
            {
            if ( language == ELangNone &&
                 !baseResource.Compare( KNullDesC ) &&
                 BaflUtils::FileExists( fs, fileName ) )
                {
                baseResource.Copy( fileName );
                }
            err = finder.FindWild( entries );
            delete entries;
            entries = NULL;
            }
        }

    if ( !found && baseResource.Compare( KNullDesC ) )
        {
        // If we found *.rsc then better to use that than nothing
        if ( BaflUtils::FileExists( fs, baseResource ) )
            {
            iActiveIdleResourceOffset = coeEnv->AddResourceFileL( baseResource );
            }
        }



    IRLOG_DEBUG( "CIRActiveIdle::InitializeResourceLoadingL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::LoadResourcesL()
// Loads the required resources
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::LoadResourcesL()
    {
    IRLOG_DEBUG( "CIRActiveIdle::LoadResourcesL" );

    if ( !iIRTerminationDetector )
        {
        iIRTerminationDetector = new (ELeave) CIRActiveIdleStateDetector( *this );
        }

    if ( !iEngine )
        {
        iEngine = CIRActiveIdleEng::NewL( *this );
        iEngine->SecondConstructL();
        }
    IRLOG_DEBUG( "CIRActiveIdle::LoadResourcesL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::Resume()
// Now it is allowed to actively publish data to observers.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::Resume( TAiTransitionReason /*aReason*/ )
    {
    IRLOG_DEBUG( "CIRActiveIdle::Resume" );
    TRAP_IGNORE( LoadResourcesL() )
    IRLOG_DEBUG( "CIRActiveIdle::Resume - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::Suspend()
// Not allowed to consume CPU resources, e.g MUST stop each timer,
// cancel outstanding asynchronous operations, etc.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::Suspend( TAiTransitionReason /*aReason*/ )
    {
    IRLOG_DEBUG( "CIRActiveIdle::Suspend" );

    if ( iIRTerminationDetector )
        {
        iIRTerminationDetector->Cancel();
        }

    delete iEngine;
    iEngine = NULL;
    IRLOG_DEBUG( "CIRActiveIdle::Suspend - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::Stop
// Must free all memory and CPU resources and close all open files.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::Stop( TAiTransitionReason /*aReason*/ )
    {
    IRLOG_DEBUG( "CIRActiveIdle::Stop" );
    delete iIRTerminationDetector;
    iIRTerminationDetector = NULL;
    delete iEngine;
    iEngine = NULL;
    IRLOG_DEBUG( "CIRActiveIdle::Stop - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::SubscribeL
// Adds the content observer / subscriber to plug-in.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::SubscribeL( MAiContentObserver& aObserver )
    {
    IRLOG_DEBUG( "CIRActiveIdle::SubscribeL" );
    iObservers.AppendL( &aObserver );
    IRLOG_DEBUG( "CIRActiveIdle::SubscribeL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::ConfigureL
// Configures the plug-in.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::ConfigureL( RAiSettingsItemArray& /*aSettings*/ )
    {
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::Extension
// Returns interface extension. In S60 3.2 only event & property extensions
// are supported.
// ---------------------------------------------------------------------------
//
TAny* CIRActiveIdle::Extension( TUid aUid )
    {
    IRLOG_DEBUG( "CIRActiveIdle::Extension" );
    TAny* ret = NULL;
    if ( aUid == KExtensionUidProperty )
    	{
        ret = static_cast<MAiPropertyExtension*>( this );
    	}
    else if ( aUid == KExtensionUidEventHandler )
    	{
        ret = static_cast<MAiEventHandlerExtension*>( this );
    	}
    else
        {
        ret = NULL;
        }
    IRLOG_DEBUG( "CIRActiveIdle::Extension - Exiting." );
    return ret;
    }

// ----------------------------------------------------------------------------
// CIRActiveIdle::GetPropertyL
// Reads a plugin property.
// ----------------------------------------------------------------------------
//
TAny* CIRActiveIdle::GetPropertyL( TInt aProperty )
	{
	IRLOG_DEBUG( "CIRActiveIdle::GetPropertyL" );
    switch ( aProperty )
    	{
    	case EAiPublisherInfo:
            return &iInfo;

    	case EAiPublisherContent:
            return static_cast<MAiContentItemIterator*>( iContent );

    	case EAiPublisherResources:
            return static_cast<MAiContentItemIterator*>( iResources );

    	case EAiPublisherEvents:
            return static_cast<MAiContentItemIterator*>( iEvents );

    	default:
    	    break;
    	}
    User::Leave( KErrNotSupported );
    IRLOG_DEBUG( "CIRActiveIdle::GetPropertyL - Exiting." );
    return NULL;
	}

// ----------------------------------------------------------------------------
// CIRActiveIdle::SetPropertyL
// Sets a plugin property
// ----------------------------------------------------------------------------
//
void CIRActiveIdle::SetPropertyL( TInt aProperty, TAny* aValue )
	{
	IRLOG_DEBUG( "CIRActiveIdle::SetPropertyL" );
    if ( aValue )
    	{
        switch ( aProperty )
        	{
        	case EAiPublisherInfo:
            	{
            	const TAiPublisherInfo* info =
                    static_cast<const TAiPublisherInfo*>( aValue );
            	iInfo.iUid.iUid = info->iUid.iUid;
            	iInfo.iName.Copy( info->iName );
                break;
            	}
        	default:
                break;
        	}
    	}
    else
    	{
    	User::Leave(KErrGeneral);
    	}
    IRLOG_DEBUG( "CIRActiveIdle::SetPropertyL - Exiting." );
	}

// ----------------------------------------------------------------------------
// CIRActiveIdle::HandleEvent
// Invoked by the framework when plug-in must handle an event
// ----------------------------------------------------------------------------
//
void CIRActiveIdle::HandleEvent( TInt aEvent, const TDesC& /*aParam*/ )
	{
	IRLOG_DEBUG( "CIRActiveIdle::HandleEvent" );
	switch ( aEvent )
    	{
       	case EAiIREventVolInc:
       	    break;
       	case EAiIREventVolDec:
       	    break;
       	case EAiIREventLaunchRadio:
    	    {
    	    TRAP_IGNORE( StartApplicationL( KUidActiveInternetRadioApp,
    	    								 KNullDesC8 ) )
    	    break;
    	    }
    	default:
            break;
    	}
    IRLOG_DEBUG( "CIRActiveIdle::HandleEvent - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRActiveIdle::Clean
// Cleans all data fields from the plugin
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::Clean()
	{
	IRLOG_DEBUG( "CIRActiveIdle::Clean" );
    for ( TInt i = 0; i < iObservers.Count(); i++ )
    	{
        MAiContentObserver* observer = iObservers[i];
        observer->StartTransaction( reinterpret_cast<TInt32>( this ) );
        observer->Clean( *this, EAiIRContentVolume, KIRPublishIndex );
        observer->Clean( *this, EAiIRContentOneLineInfoCaption, KIRPublishIndex );
        observer->Clean( *this, EAiIRContentAppIcon, KIRPublishIndex );
        observer->Clean( *this, EAiIRContentPresetCaption, KIRPublishIndex );
        observer->Clean( *this, EAiIRContentArtistCaption, KIRPublishIndex );
        observer->Clean( *this, EAiIRContentTrackCaption, KIRPublishIndex );
        observer->Commit( reinterpret_cast<TInt32>( this ) );
    	}
    IRLOG_DEBUG( "CIRActiveIdle::Clean - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRActiveIdle::StartApplicationL
// Launches an application.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::StartApplicationL( const TUid& aUid, const TDesC8& aCommandLine )
	{
	IRLOG_DEBUG( "CIRActiveIdle::StartApplicationL" );
	
	TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
	TApaTask task = taskList.FindApp( KUidActiveInternetRadioApp );
	
//   _LIT8(KNull,"")	
	if ( task.Exists() )
		{
		
		TPtrC8 activeIdleCmdId( KIRCommandLineActiveIdle );
    	TPtrC8 spacePtr( KIRSpace );
    
		TPtrC8 startNowPlayingCmdvalue(
	    KIRCommandLineActiveIdleCommandStartNowPlayingView );

	    HBufC8* buf = HBufC8::NewLC(activeIdleCmdId.Length()
	    			 + spacePtr.Length() + startNowPlayingCmdvalue.Length());
	    TPtr8 tail( buf->Des() );
	    
	       
	    tail.Append( activeIdleCmdId );
	    tail.Append( spacePtr );
	    tail.Append( startNowPlayingCmdvalue );
	    task.SendMessage(KUidActiveInternetRadioApp,tail);
        task.BringToForeground();
        
        CleanupStack::PopAndDestroy( buf );
		
		}
	else
		{
		RApaLsSession apaSession;
	    CleanupClosePushL( apaSession );

	    User::LeaveIfError( apaSession.Connect() );
	    User::LeaveIfError( apaSession.GetAllApps() );

	    TApaAppInfo appInfo;
	    User::LeaveIfError( apaSession.GetAppInfo( appInfo, aUid ) );

		CApaCommandLine* startParams = CApaCommandLine::NewLC();
		startParams->SetExecutableNameL( appInfo.iFullName );
		startParams->SetCommandL( EApaCommandRun );
		startParams->SetTailEndL( aCommandLine );
		User::LeaveIfError( apaSession.StartApp( *startParams ) );
		CleanupStack::PopAndDestroy( 2, &apaSession );
		}	
	IRLOG_DEBUG( "CIRActiveIdle::StartApplicationL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRActiveIdle::HandleChannelChangeL
// Handles channel change
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::HandleChannelChangeL()
	{
	IRLOG_DEBUG( "CIRActiveIdle::HandleChannelChangeL" );
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSDataChannel,
					 iChannelName );
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSPresetIndex,
					 iPresetIndex);

	IRLOG_DEBUG3( "CIRActiveIdle::HandleChannelChangeL - iChannelName=%S, iPresetIndex=%d", &iChannelName, iPresetIndex );

	HBufC* stringholder = NULL;
	

	CDesCArray* array = new (ELeave) CDesCArrayFlat(KIRActiveIdleGranularity);
	CleanupStack::PushL( array );
	
	MDesCArray* list = STATIC_CAST(MDesCArray*,array);

	CArrayFix<TInt>* presetCounts = new (ELeave) CArrayFixFlat<TInt>( 2 );
	CleanupStack::PushL( presetCounts );
	CEikonEnv* env = CEikonEnv::Static();      
	if(iPresetIndex != 0)
		{
		 if(iMetaData->Artist() == KNullDesC && iMetaData->Song() == KNullDesC)
			 {
             array->AppendL( iChannelName );
             presetCounts->AppendL( iPresetIndex );
			 stringholder = StringLoader::LoadLC( R_IRAPP_ACTIVEIDLE_NOSTATIONDATA,*list,
													 *presetCounts ,env );
			 }
		 else
			 {
			array->AppendL( iMetaData->Artist() ); //First string
		    array->AppendL( iMetaData->Song() );
		    array->AppendL( iChannelName );
			presetCounts->AppendL( iPresetIndex );
			stringholder = StringLoader::LoadLC( R_IRAPP_ACTIVEIDLE_STATIONDATA,*list,
													 *presetCounts ,env );
			 }
			
		}
	else
		{
			array->AppendL( iMetaData->Artist() ); //First string
		    array->AppendL( iMetaData->Song() );
		    array->AppendL( iChannelName );
	        stringholder = StringLoader::LoadLC( R_IRAPP_AI_NOINDEX_STATIONDATA,*list);
		}
	

    for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
	    MAiContentObserver* observer = iObservers[i];

	    observer->StartTransaction( reinterpret_cast<TInt32>( this ) );

	    observer->Publish( *this, EAiIRContentOneLineInfoCaption, *stringholder,
	    					 KIRPublishIndex );
	    observer->Commit( reinterpret_cast<TInt32>( this ) );
		}

	CleanupStack::PopAndDestroy(stringholder);
	CleanupStack::PopAndDestroy( presetCounts );

	// Pop and delete strings array
	CleanupStack::PopAndDestroy();
	IRLOG_DEBUG( "CIRActiveIdle::HandleChannelChangeL - Exiting." );	IRLOG_DEBUG( "CIRActiveIdle::HandleChannelChangeL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRActiveIdle::HandleMetaDataReceivedL
// Handles meta data changes
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::HandleMetaDataReceivedL( const CIRMetaData& aMetaData )
    {
	IRLOG_DEBUG( "CIRActiveIdle::HandleMetaDataReceivedL" );
	iMetaData->SetL( aMetaData );
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSDataChannel,
					 iChannelName );
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSPresetIndex,
					 iPresetIndex);

	IRLOG_DEBUG3( "CIRActiveIdle::HandleChannelChangeL - iChannelName=%S, iPresetIndex=%d", &iChannelName, iPresetIndex );

	HBufC* stringholder = NULL;

	CDesCArray* array = new (ELeave) CDesCArrayFlat(KIRActiveIdleGranularity);
	CleanupStack::PushL( array );
	
	MDesCArray* list = STATIC_CAST(MDesCArray*,array);

	CArrayFix<TInt>* presetCounts = new (ELeave) CArrayFixFlat<TInt>( 2 );
	CleanupStack::PushL( presetCounts );
	CEikonEnv* env = CEikonEnv::Static();      
	if(iPresetIndex != 0)
		{
		 if(iMetaData->Artist() == KNullDesC && iMetaData->Song() == KNullDesC)
			 {
             array->AppendL( iChannelName );
             presetCounts->AppendL( iPresetIndex );
			 stringholder = StringLoader::LoadLC( R_IRAPP_ACTIVEIDLE_NOSTATIONDATA,*list,
													 *presetCounts ,env );
			 }
		 else
			 {
			array->AppendL( iMetaData->Artist() ); //First string
		    array->AppendL( iMetaData->Song() );
		    array->AppendL( iChannelName );
			presetCounts->AppendL( iPresetIndex );
			stringholder = StringLoader::LoadLC( R_IRAPP_ACTIVEIDLE_STATIONDATA,*list,
													 *presetCounts ,env );
			 }
			
		}
	else
		{
			array->AppendL( iMetaData->Artist() ); //First string
		    array->AppendL( iMetaData->Song() );
		    array->AppendL( iChannelName );
	        stringholder = StringLoader::LoadLC( R_IRAPP_AI_NOINDEX_STATIONDATA,*list);
		}
	

    for ( TInt i = 0; i < iObservers.Count(); i++ )
		{
	    MAiContentObserver* observer = iObservers[i];

	    observer->StartTransaction( reinterpret_cast<TInt32>( this ) );

	    observer->Publish( *this, EAiIRContentOneLineInfoCaption, *stringholder,
	    					 KIRPublishIndex );

	    observer->Commit( reinterpret_cast<TInt32>( this ) );
		}

	CleanupStack::PopAndDestroy(stringholder);
	CleanupStack::PopAndDestroy( presetCounts );

	// Pop and delete strings array
	CleanupStack::PopAndDestroy();
		
    IRLOG_DEBUG( "CIRActiveIdle::HandleMetaDataReceivedL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRActiveIdle::HandlePlayStopL
// Handles data change in Stop State
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::HandlePlayStopL( TIRStopPlayState aState )
	{
	IRLOG_DEBUG( "CIRActiveIdle::HandlePlayStopL" );
	RProperty::Get( KUidActiveInternetRadioApp, KIRPSDataChannel, iChannelName );
	RProperty::Get(KUidActiveInternetRadioApp, KIRPSPresetIndex, iPresetIndex);
    HBufC* oneLineInfoCaption = NULL;
	HBufC* presetCaption = NULL;

	// Checks if the channel is in favourites and if it is displays
	// the channel name with the preset index.
	if ( iPresetIndex == 0 )
        {
        oneLineInfoCaption = iChannelName.AllocLC();
        presetCaption = iChannelName.AllocLC();
        }
	else
        {
        TBuf<KMAXLENGTH> oneLineInfoBuffer;
        oneLineInfoBuffer.AppendNum(iPresetIndex);
        oneLineInfoBuffer.Append(KSeparatorDot);
	    oneLineInfoBuffer.Append(iChannelName);
	    oneLineInfoBuffer.Trim();
	    oneLineInfoCaption = oneLineInfoBuffer.AllocLC();
        TBuf<KMAXLENGTH> presetBuffer;
	    presetBuffer.AppendNum(iPresetIndex);
	    presetBuffer.Append(KSeparatorDot);
	    presetBuffer.Append(iChannelName);
	    presetBuffer.Trim();
   	    presetCaption = presetBuffer.AllocLC();
        }

	
	
    if ( aState != EIRStateUninitialized )
        {
    	for ( TInt i = 0; i < iObservers.Count(); i++ )
    		{
            MAiContentObserver* observer = iObservers[i];

            observer->StartTransaction( reinterpret_cast<TInt32>( this ) );

            observer->Publish( *this, EAiIRContentPresetCaption, *presetCaption,
            					KIRPublishIndex );

            if ( aState == EIRStatePlay )
                {
    	        observer->Publish( *this, EAiIRContentAppIcon, EAiIRResourceIRPlayingIcon,
    	        					KIRPublishIndex );
                }
            else
                {
    	        observer->Publish( *this, EAiIRContentAppIcon, EAiIRResourceIRStoppedIcon,
    	        					KIRPublishIndex );
     	        observer->Publish( *this, EAiIRContentOneLineInfoCaption, *oneLineInfoCaption,
     	        					KIRPublishIndex );
                }

            observer->Commit( reinterpret_cast<TInt32>( this ) );
    		}
        }

	CleanupStack::PopAndDestroy( presetCaption );
	CleanupStack::PopAndDestroy( oneLineInfoCaption );
	IRLOG_DEBUG( "CIRActiveIdle::HandlePlayStopL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRActiveIdle::HandleVolumeChangeL
// Handles volume change
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::HandleVolumeChangeL( const TInt aVolume )
	  {
	IRLOG_DEBUG( "CIRActiveIdle::HandleVolumeChangeL" );
	if ( aVolume >= KIRMinVolumeLevel && aVolume <= KIRMaxVolumeLevel )
	    {
        TInt resourceId( EAiIRResourceVol1 );
    	switch ( aVolume )
    	    {
    	    case EVolumeLevel0:
    	        {
    	        resourceId = EAiIRResourceVol0;
    	        break;
    	        }
    	    case EVolumeLevel1:
    	        {
    	        resourceId = EAiIRResourceVol1;
    	        break;
    	        }
    	    case EVolumeLevel2:
    	        {
    	        resourceId = EAiIRResourceVol2;
    	        break;
    	        }
    	    case EVolumeLevel3:
    	        {
    	        resourceId = EAiIRResourceVol3;
    	        break;
    	        }
    	    case EVolumeLevel4:
    	        {
    	        resourceId = EAiIRResourceVol4;
    	        break;
    	        }
    	    case EVolumeLevel5:
    	        {
    	        resourceId = EAiIRResourceVol5;
    	        break;
    	        }
    	    case EVolumeLevel6:
    	        {
    	        resourceId = EAiIRResourceVol6;
    	        break;
    	        }
    	    case EVolumeLevel7:
    	        {
    	        resourceId = EAiIRResourceVol7;
    	        break;
    	        }
    	    case EVolumeLevel8:
    	        {
    	        resourceId = EAiIRResourceVol8;
    	        break;
    	        }
    	    case EVolumeLevel9:
    	        {
    	        resourceId = EAiIRResourceVol9;
    	        break;
    	        }
    	    case EVolumeLevel10:
    	        {
    	        resourceId = EAiIRResourceVol10;
    	        break;
    	        }
    	    default:
    	        break;
    	    }
        for ( TInt i = 0; i < iObservers.Count(); i++ )
        	{
            MAiContentObserver* observer = iObservers[i];
            //Initiates content publishing transaction.
            observer->StartTransaction( reinterpret_cast<TInt32>( this ) );
            //Informs that content identified by reference resourceId must be
            //published to UI control identified by selector
     	    //EAiIRContentVolume
    		observer->Publish( *this, EAiIRContentVolume, resourceId, KIRPublishIndex );
    	    //Requests framework that content publishing transaction must be
    	    //finalized and content should be rendered to the screen.
    	    //Displays the volume level onto the Active Idle content area.
        	observer->Commit( reinterpret_cast<TInt32>( this ) );
        	}
	    }
	else
		{
		User::Leave(KErrGeneral);
		}
    IRLOG_DEBUG( "CIRActiveIdle::HandleVolumeChangeL - Exiting." );

	}

// ---------------------------------------------------------------------------
// CIRActiveIdle::HandleApplicationRunningStateChangeL
// Handles changes in Internet Radio application running state.
// ---------------------------------------------------------------------------
//
void CIRActiveIdle::HandleApplicationRunningStateChangeL(
        const TIRPSApplicationRunningState aRunningState )
    {
    IRLOG_DEBUG( "CIRActiveIdle::HandleApplicationRunningStateChangeL" );
    if ( iRunningState != aRunningState )
        {
        switch ( aRunningState )
            {
            case EIRPSApplicationUninitialized:
                {
                break;
                }
            //called when application starts
            case EIRPSApplicationRunning:
                {
                for ( TInt i = 0; i < iObservers.Count(); i++ )
                    {
                    MAiContentObserver* observer = iObservers[i];
                    //Initiates content publishing transaction.
                    observer->StartTransaction(reinterpret_cast<TInt32>(
                                                 this ) );
                    //Invoked by the plug-in to inform that content identified
                    //by reference EAiIRResourceIRStoppedIcon must be published
                    //to UI crtl identified by selector EAiIRContentAppIcon.
                    // Full Page Idle: shows IR icon.
                    // Navigation Bar Idle: updates title text and switches correct layout.
                    // App has just started, publish initially to stopped state.
                    observer->Publish( *this, EAiIRContentAppIcon, EAiIRResourceIRStoppedIcon,
                    					 KIRPublishIndex );
                    observer->Publish( *this, EAiIRContentOneLineInfoCaption,KNullDesC,
                    					KIRPublishIndex );

                    observer->Commit( reinterpret_cast<TInt32>( this ) );
                    }

                if ( iIRTerminationDetector )
                    {
                    iIRTerminationDetector->StartListeningL();
                    }
                break;
                }
            // called when the application exits
            case EIRPSApplicationClosing:
                {
                Clean();
                break;
                }
            default:
                break;
            }
        iRunningState = aRunningState;
        }
    IRLOG_DEBUG( "CIRActiveIdle::HandleApplicationRunningStateChangeL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRActiveIdle::HandleBufferingStateL
// Handles changes in Internet Radio buffering state
// ---------------------------------------------------------------------------
//

void CIRActiveIdle::HandleBufferingStateL(TIRBufferingState aState )
	{
		if (EIRStateBufferingStart == aState)
		{
		    // do nothing, just remove build warning
		}
	}
