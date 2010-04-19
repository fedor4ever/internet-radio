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
* Description:  
*
*/


#include <bautils.h>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <e32cmn.h>
#include <implementationproxy.h>
#include <liwvariant.h>
#include <stringloader.h>
#include <irmcpplugin.rsg>

#include <e32property.h>

#include "iractionhandler.hrh"
#include "iractionhandlerdefs.h"
#include "iractiveidleeng.h"
#include "iractiveidleengnotifyhandler.h"
#include "irdebug.h"
#include "irmcpplugin.h"
#include "irmcpplugin.hrh"



#include "irmetadata.h"

const TInt KIRMinVolumeLevel = 0;
const TInt KIRMaxVolumeLevel  = 10;

_LIT( KIRMCPPluginResourceFileName, "irmcpplugin.rsc" );

//Temp images from Music widget, svg via handle to rgba bitmap will be supported soon.
_LIT( KIMGPlay, "z:\\Resource\\apps\\qgn_prop_image_tb_play2_dimmed2.png" );
_LIT( KIMGStop, "z:\\Resource\\apps\\qgn_prop_image_tb_pause_dimmed2.png" );

_LIT8( KIRPluginUid, "plugin_id" );
_LIT8( KIRData, "data" );
_LIT( KEmpty, "" );
// ---------------------------------------------------------
// CIRMCPPlugin::NewL
// ---------------------------------------------------------
//
CIRMCPPlugin* CIRMCPPlugin::NewL( MMCPPluginObserver* aObserver )
    {
    CIRMCPPlugin* self = new ( ELeave ) CIRMCPPlugin( aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CIRMCPPlugin::~CIRMCPPlugin
// ---------------------------------------------------------
//
CIRMCPPlugin::~CIRMCPPlugin()
    {
    if ( iResourceOffset > 0 )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }

    delete iMetaData;

    delete iEngine;
    delete iLastPlayedText;
    }

// ---------------------------------------------------------
// CIRMCPPlugin::CIRMCPPlugin
// ---------------------------------------------------------
//
CIRMCPPlugin::CIRMCPPlugin( MMCPPluginObserver* aObserver )
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------
// CIRMCPPlugin::ConstructL
// ---------------------------------------------------------
//
void CIRMCPPlugin::ConstructL()
    {
    InitializeResourceLoadingL();

    iRunningState = EIRPSApplicationUninitialized;

    iLastPlayedText = StringLoader::LoadL( R_IR_MCP_LASTPLAYED);
    iBufferingText = StringLoader::LoadL( R_IR_MCP_BUFFERING);

    iEngine = CIRActiveIdleEng::NewL( *this );
    iEngine->SecondConstructL();
    
    iMetaData = CIRMetaData::NewL();
	HandleApplicationRunningStateChangeL(iRunningState);

    }


// ---------------------------------------------------------------------------
// CIRMCPPlugin::InitializeResourceLoadingL
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::InitializeResourceLoadingL()
    {
    CCoeEnv* coeEnv = CCoeEnv::Static();
    RFs fs( coeEnv->FsSession() );
    TFileName fileName; 
    TFileName baseResource;
    TFindFile finder( fs );
    TLanguage language( ELangNone );
    
    TParsePtrC parse( KIRMCPPluginResourceFileName );
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
        fileName.Append( KIRMCPPluginResourceFileName );
        BaflUtils::NearestLanguageFile( fs, fileName, language );
        if ( language != ELangNone && BaflUtils::FileExists( fs, fileName ) )
            {
            found = ETrue;
            iResourceOffset = coeEnv->AddResourceFileL( fileName );
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
            iResourceOffset = coeEnv->AddResourceFileL( baseResource );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIRMCPPlugin::Deactivate
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::Deactivate()
    {
    IRRDEBUG2("InternetRadio -- CIRMCPPlugin::Deactivate Entering", KNullDesC);

    iActive = EFalse;
    }

// ---------------------------------------------------------------------------
// CIRMCPPlugin::ActivateL
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::ActivateL()
    {
    IRRDEBUG2("CIRMCPPlugin::ActivateL Entering", KNullDesC);

    iActive = ETrue;
    iObserver->BecameActiveL(this);
    
    HandleChannelChangeL();
    
    HandleMetaDataReceivedL(*iMetaData);
  
  	RProperty::Get( KUidActiveInternetRadioApp,KIRPSPlayStop,
					 iPlayingState );	 

	HandlePlayStopL(static_cast<TIRStopPlayState>(iPlayingState));
	
/*	IRRDEBUG2("ActivateL iRunningState = %d",iRunningState);
    InstallIRCommandActionL( KIRCommandValueStartNowPlaying(), EMusicWidgetTrigger1 );
    iObserver->PublishImageL( this, EMusicWidgetImage1, KIMGPlay ); */ 	
	
    InstallIRCommandActionL( KIRCommandValueStartNowPlaying(), EMusicWidgetTrigger2 );            

    }




// ---------------------------------------------------------------------------
// CIRMCPPlugin::PublishLastPlayedWidgetItemsL
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::PublishLastPlayedWidgetItemsL()
    {
    if ( iActive )
    	{
   	    IRRDEBUG2("CIRMCPPlugin::PublishLastPlayedWidgetItemsL Entering", KNullDesC);

        InstallIRCommandActionL( KIRCommandValueStartNowPlaying(), EMusicWidgetTrigger1 );
        iObserver->PublishImageL( this, EMusicWidgetImage1, KIMGPlay );
        }
	
    HandleChannelChangeL();
    IRRDEBUG2("CIRMCPPlugin::PublishLastPlayedWidgetItemsL Exitings", KNullDesC);
       
    }



// ---------------------------------------------------------------------------
// CIRMCPPlugin::HandleChannelChangeL
// Handles channel change
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::HandleChannelChangeL()
	{
	
    IRRDEBUG2("CIRMCPPlugin::HandleChannelChangeL Entering", KNullDesC);

	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSDataChannel,
				 	 iChannelName );
				 	 
	IRRDEBUG2("HandleChannelChange iRunningState = %d",iRunningState);

	 if ( iActive )
        {
        switch(iRunningState)
	        {
	        case EIRPSApplicationRunning:
		        {
		        iObserver->PublishTextL( this, EMusicWidgetText1, iChannelName);
		        IRRDEBUG2("CIRMCPPlugin::HandleChannelChangeL iChannelName published", KNullDesC);
							       	
				break;
		        }
		     
	        case  EIRPSApplicationUninitialized:
	        case  EIRPSApplicationClosing:
		       {
	     	   IRRDEBUG2("CIRMCPPlugin::HandleChannelChangeL EIRPSApplicationUninitialized", KNullDesC);
		       iObserver->PublishTextL( this, EMusicWidgetText1, *iLastPlayedText );
	           iObserver->PublishTextL( this, EMusicWidgetText2, iChannelName );
	      			
					break;
		       }
		       
		    default:
		    	break;
	        }
        }	

    IRRDEBUG2("CIRMCPPlugin::HandleChannelChangeL Exiting", KNullDesC);

	}


// ---------------------------------------------------------------------------
// CIRMCPPlugin::HandleMetaDataReceivedL
// Handles meta data changes
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::HandleMetaDataReceivedL( const CIRMetaData& aMetaData )
    {
    IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL Entering", KNullDesC);

    iMetaData->SetL( aMetaData );
    
    if ( iActive )
        {
        switch(iRunningState)
	        {

	        case EIRPSApplicationRunning:
		        {
		        IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL EIRPSApplicationRunning", KNullDesC);
				if(iMetaData->Song().Length())
					{
					iObserver->PublishTextL( this, EMusicWidgetText2, iMetaData->Song());
		            IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL Song published", KNullDesC);
					}
			/*	else
					{
					iObserver->PublishTextL( this, EMusicWidgetText2, KEmpty)
					}*/
										
				if(iMetaData->Artist().Length())
					{
					iObserver->PublishTextL( this, EMusicWidgetText3, iMetaData->Artist());
			        IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL Artist published", KNullDesC);
					} 
			/*	else
					{
					iObserver->PublishTextL( this, EMusicWidgetText3, KEmpty)
					}*/	
									       	
				break;
		        }
		     
	        case  EIRPSApplicationUninitialized:
	        case  EIRPSApplicationClosing:
		       {
	     	   IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL EIRPSApplicationUninitialized/Closing", KNullDesC);
	           if(iMetaData->Song().Length())
					{
					iObserver->PublishTextL( this, EMusicWidgetText3, iMetaData->Song());
		            IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL LastSong published", KNullDesC);
					}
			   else
					{
					iObserver->PublishTextL( this, EMusicWidgetText3, KEmpty);
					}	
						
					break;
		       }
		       
		    default:
		    	break;
	        }
        }	   
        	
    IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL exiting", KNullDesC);

	}

// ---------------------------------------------------------------------------
// CIRMCPPlugin::HandlePlayStopL
// Handles data change in Stop State
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::HandlePlayStopL( TIRStopPlayState aState )
	{
	
    IRRDEBUG2("CIRMCPPlugin::HandlePlayStopL Entering", KNullDesC);

	if ( aState == EIRStatePlay )
		{
	    IRRDEBUG2("CIRMCPPlugin::HandlePlayStopL unmute/play", KNullDesC);

	    InstallIRCommandActionL( KIRCommandValueStop(), EMusicWidgetTrigger1 );

        iObserver->PublishImageL( this, EMusicWidgetImage1, KIMGStop );
		}
	else 			//if (aState == EIRStateStop)
		{
		
	    IRRDEBUG2("CIRMCPPlugin::HandlePlayStopL mute/stop", KNullDesC);

	    InstallIRCommandActionL( KIRCommandValuePlay(), EMusicWidgetTrigger1 );

        iObserver->PublishImageL( this, EMusicWidgetImage1, KIMGPlay );
		}
		
		

    IRRDEBUG2("InternetRadio -- CIRMCPPlugin::HandlePlayStopL Exiting", KNullDesC);

	}

// ---------------------------------------------------------------------------
// CIRMCPPlugin::HandleVolumeChangeL
// Handles volume change
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::HandleVolumeChangeL( const TInt aVolume )
	  {
      if ( aVolume >= KIRMinVolumeLevel && aVolume <= KIRMaxVolumeLevel )
	      {
	      	
	      }
	  else
		  {
		  User::Leave(KErrGeneral);
	 	  }		      
 	  }


// ---------------------------------------------------------------------------
// CIRMCPPlugin::HandleApplicationRunningStateChangeL
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::HandleApplicationRunningStateChangeL( TIRPSApplicationRunningState 
															aRunningState )
	{
	IRRDEBUG2("CIRMCPPlugin::HandleApplicationRunningStateChangeL Entering aRunningState= %d", aRunningState);

	if ( iRunningState != aRunningState )
		{
		iRunningState = aRunningState;

		switch ( iRunningState )
			{
			case EIRPSApplicationUninitialized:
			case EIRPSApplicationRunning:
			ActivateL();
			break;
			case EIRPSApplicationClosing:
			if ( iActive )
				{
				IRRDEBUG2("HandleApplicationRunningStateChangeL EIRPSApplicationClosing iActive", KNullDesC);

				InstallIRCommandActionL( KIRCommandValueStartNowPlaying(), EMusicWidgetTrigger1 );
				IRRDEBUG2("HandleApplicationRunningStateChangeL StartNowPlaying", KNullDesC);

				iObserver->PublishImageL( this, EMusicWidgetImage1, KIMGPlay );
				IRRDEBUG2("HandleApplicationRunningStateChangeL PublishImageL", KNullDesC);

 				iObserver->PublishTextL( this, EMusicWidgetText1, *iLastPlayedText );
	            iObserver->PublishTextL( this, EMusicWidgetText2, iChannelName );
	      	

				if(iMetaData->Song().Length())
					{
					iObserver->PublishTextL( this, EMusicWidgetText3, iMetaData->Song());
					IRRDEBUG2("CIRMCPPlugin::HandleMetaDataReceivedL Song published", KNullDesC);
					}
				else
					{
					iObserver->PublishTextL( this, EMusicWidgetText3, KEmpty);
					}

				}
			iActive = EFalse;
			break;
			default:
			break;

			}

		}
	IRRDEBUG2("CIRMCPPlugin::HandleApplicationRunningStateChangeL Exiting", KNullDesC);

	}


// ---------------------------------------------------------------------------
// CIRMCPPlugin::InstallIRCommandActionL
// ---------------------------------------------------------------------------
//
void CIRMCPPlugin::InstallIRCommandActionL( const TDesC& aCommand, 
											TMCPTriggerDestination aDestination )
    {
    IRRDEBUG2("CIRMCPPlugin::InstallIRCommandActionL Entering", KNullDesC);

    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();

    mapTrigger->InsertL( KIRPluginUid, TLiwVariant( 
	TUid::Uid( KIRActionHandlerImplementationUid ) ) );
	
    mapData->InsertL( KIRCommand, TLiwVariant( aCommand ) );
    
    mapTrigger->InsertL( KIRData, TLiwVariant( mapData ) );
    
    if ( iActive )
        {
        IRRDEBUG2("InstallIRCommandActionL iActive", KNullDesC);
        
        iObserver->PublishActionL( this, aDestination, mapTrigger );
        IRRDEBUG2("InstallIRCommandActionL PublishActionL", KNullDesC);
        
        }
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    
    IRRDEBUG2("CIRMCPPlugin::InstallIRCommandActionL Exiting", KNullDesC);

    }



// ---------------------------------------------------------------------------
// CIRActiveIdle::HandleBufferingStateL
// Handles changes in Internet Radio buffering state
// ---------------------------------------------------------------------------
//

void CIRMCPPlugin::HandleBufferingStateL(TIRBufferingState aState )
	{
	IRRDEBUG2("CIRMCPPlugin::HandleBufferingStateL Entering", KNullDesC);
	
	if(iActive)
		{
		switch (aState)	
			{
			case EIRStateBufferingStop:
				{
				IRRDEBUG2("CIRMCPPlugin::HandleBufferingStateL 0", KNullDesC);

				HandleChannelChangeL();
    
    			HandleMetaDataReceivedL(*iMetaData);
  
  				RProperty::Get( KUidActiveInternetRadioApp,KIRPSPlayStop,
					 iPlayingState );	 

				HandlePlayStopL(static_cast<TIRStopPlayState>(iPlayingState));
	
	
				}
			break;
			case EIRStateBufferingStart:
				{
				
 				IRRDEBUG2("CIRMCPPlugin::HandleBufferingStateL 1", KNullDesC);

				iObserver->PublishTextL( this, EMusicWidgetText1, KEmpty);
				iObserver->PublishTextL( this, EMusicWidgetText2,KEmpty);
				iObserver->PublishTextL( this, EMusicWidgetText3,*iBufferingText);
	
				}
				break;
				
			case  EIRStateBufferingError:
				{
				iObserver->PublishTextL( this, EMusicWidgetText1, KEmpty);
				iObserver->PublishTextL( this, EMusicWidgetText2,KEmpty);
				iObserver->PublishTextL( this, EMusicWidgetText3,KEmpty);
	
				}
			break;
			}
		}

	IRRDEBUG2("CIRMCPPlugin::HandleBufferingStateL Exiting", KNullDesC);
	
	}

    
/** Provides a key-value pair table, that is used to identify the correct construction function for the requested interface. */
const TImplementationProxy KIRMCPPluginImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KIRMCPPluginImplementationUid, CIRMCPPlugin::NewL )
    };

const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount );

// ---------------------------------------------------------------------------
// Returns the implementations provided by this ECOM plugin DLL.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    IRRDEBUG2("CIRMCPPlugin::ImplementationGroupProxy", KNullDesC);

    aTableCount = sizeof( KIRMCPPluginImplementationTable ) / sizeof( TImplementationProxy );
    return KIRMCPPluginImplementationTable;
    }

