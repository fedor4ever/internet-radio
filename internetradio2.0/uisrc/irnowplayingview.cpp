/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 18, Tue Mar 19 18:00:00 2008 by Rohit
*  Ref:
*  Implemented change request to display Byte Counter in KB till 0.1 MB, and in MB threrafter
*
*  </ccm_history>
* ============================================================================
*/

// INCLUDE FILES


#include <aknviewappui.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alfroster.h>
#include <eikmenup.h>
#include <eiklabel.h>
#include <avkon.rsg>
#include <aknnotewrappers.h>
#include <internetradio.rsg>
#include <data_caging_paths_strings.hrh>
#include <mdaaudiosampleplayer.h>
#include <stereowideningbase.h>
#include <f32file.h> // for File operations
#include <s32file.h> // for File operations
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <aknbutton.h>
#include <eikcolib.h>
/* no need to change to lower case */
#include <AknsConstants.h>
#include <hlplch.h>
#ifdef MUSICSHOP_AVAILABLE
#include <mpxfindinmusicshop.h> //For Find in Shop
#include <mpxmusicplayerviewplugin.hrh>
/* no need to change to lower case */
#include <MusicWapCenRepKeys.h>
#endif
#include <apgcli.h> //For Find in Shop
#include <e32des8.h>
#include <stringloader.h>
#include <internetradio.mbg>
#include <aknwaitdialog.h>
#include <math.h> 				// floor()

#include <mpxviewplugin.h>
#include <mpxviewplugin.hrh>


#include <featmgr.h>//Fmtransmitter
#include <features.hrh>


#include "iraap.hlp.hrh"

#include "irpubsubkeys.h"
#include "irisdspreset.h"
#include "irfavoritesdb.h"
#include "irisdswrapper.h"
#include "irisdspreset.h"
#include "mlogodownloadobserver.h"
#include "irlogodownloadengine.h"
#include "irbaseview.h"
#include "irnowplayingviewcontainer.h"
#include "irnowplayingview.h"
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irhistory.h"
#include "irpreset.h"
#include "irmetadata.h"
#include "irsettings.h"
#include "irstationlogocontrol.h"
#include "irstationlogodata.h"
#include "pspresetobserver.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irstationinformationdata.h"
#include "irstationinformationdisplay.h"
#include "irradioidle.h"
#include "irdialoglauncher.h"
#include "irmediaclient.h"
#include "irnowplayingwrapper.h"
#include "irstationinfoview.h"
#include "irpubsub.h"


#include "irsystemeventcollector.h"
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include "irnetworkcontroller.h"
#include "msyncpresetobserver.h"	// MSyncPresetObserver

_LIT( KIRMifFileName, "\\resource\\apps\\InternetRadio.mif" );

const TInt KIRLogoArrayGranularity = 10; // Granularity for logo arrays
const TInt KPresetNumbering = 4;//format 1-20.
const TInt KOne =1;
const TInt KSize =360;
const TInt KMinVolume=0;
const TInt KMaxVolumeRange=10;
const TInt KMaxVolume=10;
const TInt KWaitTimeInMicroSeconds = 100000;
const TInt KMusicStoreURLSize = 512;

// Constants for byte counter display
const TInt KIRByteCounterDisplayThreshold1 = 10485760;	// 10 MB
const TInt KIRByteCounterDisplayThreshold2 = 104857600;	// 100 MB
const TInt KIRMBtoBytesConversion		   = 1048576;	// 1 MB

//Literal Declaration
_LIT(KConst1,"    ");
//_LIT(KHiFn,"-")
const TInt KTwo = 2;
const TInt KChannelNameSize = 200;
const TInt KTen=10;
const TInt KHundred=100;
const TUid KFmTxAppUid  = { 0x10282BEF };
const TInt KDefault=-1;
const TInt KDefaultValue=-2;
//Static Variable Declaration
TBool CIRNowPlayingView::iPlaying;

#ifdef __IRA_FEATURE_EQUALIZER
const TUint32 KMPXEqualizerViewImplementationId = 0x101FFC77;
#endif

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CIRNowPlayingView::CIRNowPlayingView( CAlfEnv& aAlfEnv )
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CIRNowPlayingView::CIRNowPlayingView( CAlfEnv& aAlfEnv ):
	iAlfEnv( aAlfEnv ), iToolbar(NULL), iStartUp(ETrue)
	,iMPXFindInShop(NULL)
#ifdef __IRA_FEATURE_EQUALIZER
	, iMpxEqualizerViewPlugin(NULL)
#endif // __IRA_FEATURE_EQUALIZER
	{
	IRLOG_DEBUG( "CIRNowPlayingView::CIRNowPlayingView" );
	}
// ---------------------------------------------------------
// CIRNowPlayingView::ConstructL( )
// EPOC default constructor can leave.
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
void CIRNowPlayingView::ConstructL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::ConstructL - Entering" );
	CIRBaseView::ConstructL(R_IR_NOWPLAYING_VIEW );
	EnableObserverL(EIRSystemEventObserver);
	iIRSettings = CIRSettings::OpenL();
	iIndex=0;
	iRockerIndex=0;
	iAlfEnv.AddActionObserverL( this );
	iDefaultLogo=EFalse;

	//For Find in Shop
#ifdef __IRA_FEATURE_EQUALIZER
    if (FeatureManager::FeatureSupported( KFeatureIdEqualizer))
		{
#ifdef MUSICSHOP_AVAILABLE
		TRAPD(error, iMPXFindInShop = CMPXFindInMShop::NewL());
		if (error != KErrNotFound && error != KErrNone)
			{
			User::Leave(error);
			}
#endif
		TRAPD(eqError, iMpxEqualizerViewPlugin = CMPXViewPlugin::NewL(
					   TUid::Uid(KMPXEqualizerViewImplementationId), NULL));
		if (eqError != KErrNotFound && eqError != KErrNone)
			{
			User::Leave(eqError);
			}
		}
#endif // __IRA_FEATURE_EQUALIZER

    iVolPopup = CAknVolumePopup::NewL(NULL, ETrue);
    iVolPopup->SetObserver(this);
    iVolPopup->SetRange(KMinVolume,KMaxVolumeRange);
 	iVolPopup->SetValue(2);

    iUpdateLastplayed=ETrue;
    iSameUrl=EFalse;
    iEaualizerOn=EFalse;
    iCallActive=EFalse;

    iRequestPending = EFalse;
    iStationHasLogo=EFalse;
    iNextOrPrevRequest=EFalse;
    iVolumeMute=EFalse;
    // TO Do: need to check
    iNowPlayingWrapper->SetFavFocus(-1);
	IRLOG_DEBUG( "CIRNowPlayingView::ConstructL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingView* CIRNowPlayingView::NewL( CAlfEnv& aEnv )
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRNowPlayingView* CIRNowPlayingView::NewL( CAlfEnv& aEnv)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::NewL" );
	CIRNowPlayingView* self = CIRNowPlayingView::NewLC(aEnv);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRNowPlayingView::NewL - Exiting." );
	return self;
	}

// ---------------------------------------------------------
// CIRNowPlayingView::ConstructL( )
// Two-phased constructor.
// ---------------------------------------------------------
CIRNowPlayingView* CIRNowPlayingView::NewLC( CAlfEnv& aAlfEnv )
	{
	IRLOG_DEBUG( "CIRNowPlayingView::NewLC - Entering" );
	CIRNowPlayingView* self = new (ELeave) CIRNowPlayingView( aAlfEnv );
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRNowPlayingView::NewLC - Exiting" );
	return self;
	}
// ---------------------------------------------------------
// CIRNowPlayingView::ConstructL( )
// Destructor
// ---------------------------------------------------------
CIRNowPlayingView::~CIRNowPlayingView()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::~CIRNowPlayingView - Entering" );
	CAlfEnv* alfEnv = CAlfEnv::Static();
	if( alfEnv )
		{
		alfEnv->PrimaryDisplay().Roster().Hide( alfEnv->ControlGroup( KIRNowPlayingViewID.iUid ) );
		alfEnv->DeleteControlGroup( KIRNowPlayingViewID.iUid );
		iContainer = NULL;
		}
    if( alfEnv )
        {
        iAlfEnv.RemoveActionObserver( this );
        }
	if ( iToolbar )
		{
		iToolbar->SetToolbarVisibility(EFalse, EFalse);
		iToolbar->SetToolbarObserver( NULL );
		}
	if(iConnectionCleanupTimer)
	{
	iConnectionCleanupTimer->Cancel();
	}
	if(iConnectionCleanupTimer)
	{
	delete iConnectionCleanupTimer;
	iConnectionCleanupTimer=NULL;
	}
	iMetadata.Close();
	iBuffer.Close();

#ifdef __IRA_FEATURE_EQUALIZER
	if (iMpxEqualizerViewPlugin)
		{
		delete iMpxEqualizerViewPlugin;
		iMpxEqualizerViewPlugin = NULL;
		}
#endif // __IRA_FEATURE_EQUALIZER

#ifdef MUSICSHOP_AVAILABLE
	if (iMPXFindInShop)
		{
		delete iMPXFindInShop;
		iMPXFindInShop = NULL;
		}
#endif

	if(plsData)
		{
		delete plsData;

		}
    if(iIRSettings)
		{
		iIRSettings->Close();
		iIRSettings = NULL;
		}
    if(iVolPopup)
	    {
	    delete iVolPopup;
	    iVolPopup=NULL;
	    }
    if(iPrevPreset1)
	    {
	    delete iPrevPreset1;
	    iPrevPreset1=NULL;
	    }
    if(iCurrentPreset)
	    {
	    delete iCurrentPreset;
	    iCurrentPreset=NULL;
	    }
	IRLOG_DEBUG( "CIRNowPlayingView::~CIRNowPlayingView - Exiting " );
	}

// ---------------------------------------------------------
// CIRNowPlayingView::Id
// ---------------------------------------------------------
TUid CIRNowPlayingView::Id() const
	{
	IRLOG_DEBUG( "CIRNowPlayingView::Id" );
	return KIRNowPlayingViewID;
	}

// ---------------------------------------------------------
// CIRNowPlayingView::DoActivateL
// ---------------------------------------------------------

void CIRNowPlayingView::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
								    const TDesC8& aCustomMessage)
	{
	//IRLOG_DEBUG( "CIRNowPlayingView::DoActivateL - Entering" )
	iFadeStyle=EIRFadeNoFade;
	CIRBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    iAlfEnv.PrimaryDisplay().SetVisibleArea( ClientRect() );
	iUpdateLastplayed=ETrue;
	iUpdateStationName=ETrue;
	iStationHasLogo=EFalse;
	iShowBitrate=ETrue;

	if(iNowPlayingWrapper->GetView()==ESearch||iNowPlayingWrapper->GetView()==EStations||
	   iNowPlayingWrapper->GetView()==EBrowseByTopStations )
		{
		iStationsData=iIsdsWrapper->GetStationsData();
		if(iStationsData->Count()==1)
			{
			iShowToolBar=EFalse;
			}
		else
			{
			iShowToolBar=ETrue;
			}
		}
	else if(iNowPlayingWrapper->GetView()==EFav)
		{
		TInt presetCount=iUi->iFavPresets->iFavPresetList.Count();
		if(presetCount==1||presetCount==0)
			{
			iShowToolBar=EFalse;
			}
		else
			{
			iShowToolBar=ETrue;
			}
		if(presetCount >= 1)
			{
			//fovorities list is not empty
			TInt index = iUi->iFavPresets->SearchPreset(iNowPlayingWrapper->iNowPlayingPreset->
						   GetId(),iNowPlayingWrapper->
						   iNowPlayingPreset->UniqId());
			if (index != KErrNotFound )
				{
				iNowPlayingWrapper->SetFavFocus(index+1) ;
				}
			else
				{
				iNowPlayingWrapper->SetFavFocus(-1) ;
				}

			}
		}
	else if(iNowPlayingWrapper->GetView()==ELastplayed||
			iNowPlayingWrapper->GetView()==EHistoryView)
		{
		iShowToolBar=EFalse;
		}
	  else if(iNowPlayingWrapper->GetView()==EPls)
		 {

          TInt plslist=(iUi->iURLs.Count()) / KTwo ;
		  if(plslist==1 || plslist==0 || !iUi->GetListenFromSecondPls())
			{
			iShowToolBar=EFalse;
			}
		else
			{
			iShowToolBar=ETrue;
			}
		 }
  	    // TO Do: need to check
	if(iNowPlayingWrapper->GetView()!=EFav)
		{
		iNowPlayingWrapper->SetFavFocus(-1) ;
		}
	ConstructToolbarL();
	CIRUi* ui = reinterpret_cast<CIRUi*>( AppUi() );
	ui->iPresentViewID = KIRNowPlayingViewID;
	SetTitleL( R_IRAPP_NOWPLAYING_TITLE );
	if( !iContainer )
		{
		iAlfEnv.NewControlGroupL( KIRNowPlayingViewID.iUid );
		iContainer = CIRNowPlayingViewContainer::NewL( iAlfEnv,this );
		}
		iUi->iFavPresets->AddObserver( *this );
		iPrevPreset1=CIRIsdsPreset::NewL();
		iCurrentPreset=CIRIsdsPreset::NewL();
		iUpdateStationName=ETrue;
		iContainer->StationLogoControl().SetLogoControlObserver( *this );
		OnViewActivationL();
		GetLogoL();
		iAlfEnv.PrimaryDisplay().Roster().ShowL( iAlfEnv.ControlGroup( KIRNowPlayingViewID.iUid ));
		Cba()->AddCommandSetToStackL( R_AVKON_SOFTKEYS_OPTIONS_BACK);
		iFadeStyle=EIRFadeNoFade;
	if(iCallActive)
		{
		ConstructToolbarL();
		iFadeStyle=EIRFadeUnknown;
		DrawViewL(iStationHasLogo);
		DisplaylogoL();
		}

	iIsOn = ETrue;
	iHasBeenActivated = ETrue;
	IRLOG_DEBUG( "CIRNowPlayingView::DoActivateL - Exiting" );
	}

// ---------------------------------------------------------
// CIRNowPlayingView::DoDeactivate
// ---------------------------------------------------------
//
void CIRNowPlayingView::DoDeactivate()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DoDeactivate Entering" );
	CAlfEnv* alfEnv = CAlfEnv::Static();
	if( alfEnv )
		{
		alfEnv->PrimaryDisplay().Roster().Hide( alfEnv->ControlGroup( KIRNowPlayingViewID.iUid ) );
		TRAP_IGNORE( iContainer->RadioIdle().HandleCommandL( EShutdown ) )
		alfEnv->DeleteControlGroup( KIRNowPlayingViewID.iUid );
		iContainer = NULL;
		}
	 iUi->iFavPresets->RemoveObserver( *this );
   	if ( iToolbar )
		{
		iToolbar->SetToolbarVisibility(EFalse, EFalse);
		iToolbar->SetToolbarObserver( NULL );
		}
	if(iConnectionCleanupTimer)
		{
		iConnectionCleanupTimer->Cancel();
		}
	if(iConnectionCleanupTimer)
		{
		delete iConnectionCleanupTimer;
		iConnectionCleanupTimer=NULL;
		}
	iMetadata.Close();
	iBuffer.Close();
   	iPrecount=0;
	iPresetValue=0;
	if(iPrevPreset1)
	    {
	    delete iPrevPreset1;
	    iPrevPreset1=NULL;
	    }
    if(iCurrentPreset)
	    {
	    delete iCurrentPreset;
	    iCurrentPreset=NULL;
	    }

	if(iNowPlayingWrapper->iPlaying)
		{
		 iUpdateLastplayed=ETrue;
		}
	else
		{
		iUpdateLastplayed=EFalse;
		}
	iShowToolBar=EFalse;
	iStartUp=ETrue;
	iRockerIndex=0;
	iIndex=0;
	if(iPlaying)
		{
		iNowPlayingWrapper->SetMedadataForErrotrConditions(EFalse);
		}
	else
		{
		iNowPlayingWrapper->SetMedadataForErrotrConditions(ETrue);
		}
	iNowPlayingWrapper->SetMarquee(EFalse);
	IRLOG_DEBUG( "CIRNowPlayingView::DoDeactivate Exiting" );
	}

// ---------------------------------------------------------
// CIRNowPlayingView::HandleCommandL
// ---------------------------------------------------------
//
void CIRNowPlayingView::HandleCommandL(TInt aCommand)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::HandleCommandL Activating" );
	switch (aCommand)
		{
		case EStationDetailsCmd:
			{
			if( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				iUi->iStationInfoView->SetStationPresetL( iNowPlayingWrapper->iNowPlayingPreset );
				AppUi()->ActivateLocalViewL(KIRStationInfoViewId);
				}
			break;
			}
		case EAddtoStationsCmd:
			{
			if( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				AddStationsL();
				}
			}
		break;
		case EIRCmdStepNextPreset:
			{
			if( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			iCommandId=EIRCmdStepNextPreset;
			iPresetCount=iRockerIndex;
			iRockerIndex++;

			if(iNowPlayingWrapper->GetView()==EFav)
				{
				iNowPlayingWrapper->SetView(EFav);

				if( iRockerIndex == (iUi->iFavPresets->iFavPresetList.Count()) )
					{
					iRockerIndex = 0;
					}
				iFadeStyle=EIRFadeRightToLeft;
				if( iUi->VerifyNetworkConnectivityL() )
					{
					 iNextOrPrevRequest=ETrue;
					}
					else
					{
					ConnectToPresetL();
					//StartTimerL()
					}
				break;
				}
			else if(iNowPlayingWrapper->GetView()==ESearch||
					iNowPlayingWrapper->GetView()==EStations ||
					iNowPlayingWrapper->GetView()==EBrowseByTopStations )
				{
				//iNowPlayingWrapper->SetView(ESearch)
				iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
				iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
				iFadeStyle=EIRFadeRightToLeft;
				if(iIsdsWrapper->GetBannerUrl())
				{
				 if(iRockerIndex>iStationsData->Count())
				 {
				 iRockerIndex = 1;
				 }
				 if( iUi->VerifyNetworkConnectivityL() )
					{
					 iNextOrPrevRequest=ETrue;
					}
					else
					{
					ConnectToPresetL();
					}
				break;
				}
				else
				{
				if(iRockerIndex>=iStationsData->Count())
					{
					iRockerIndex = 0;
					}
				if( iUi->VerifyNetworkConnectivityL() )
					{
					 iNextOrPrevRequest=ETrue;
					}
				else
					{
					ConnectToPresetL();
					}
				break;
				}
				}
			else if(iNowPlayingWrapper->GetView()==EPls)
				{
				iNowPlayingWrapper->SetView(EPls);
                TInt plslist=(iUi->iURLs.Count()) / KTwo ;
				if(iRockerIndex==plslist)
					{
					iRockerIndex = 0;

					}
				iFadeStyle=EIRFadeRightToLeft;
				if( iUi->VerifyNetworkConnectivityL() )
					{
					 iNextOrPrevRequest=ETrue;
					}
					else
					{
					ConnectToPresetL();
					StartTimerL();
					}
				}
				break;
			}
		case EIRCmdStepPrevPreset:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}
			iCommandId=EIRCmdStepPrevPreset;
			iPresetCount=iRockerIndex;
			iRockerIndex--;
			if(iNowPlayingWrapper->GetView()==EFav)
				{
				iNowPlayingWrapper->SetView(EFav);
				if(iRockerIndex < 0)
					{
					iRockerIndex = (iUi->iFavPresets->iFavPresetList.Count()) - 1;
					}
				iFadeStyle=EIRFadeLeftToRight;
				if( iUi->VerifyNetworkConnectivityL() )
					{
					 iNextOrPrevRequest=ETrue;
					}
					else
					{
					ConnectToPresetL();
					//StartTimerL()
					}
				}
			else if(iNowPlayingWrapper->GetView()==ESearch||
					iNowPlayingWrapper->GetView()==EStations ||
					iNowPlayingWrapper->GetView()==EBrowseByTopStations )
				{
				//iNowPlayingWrapper->SetView(ESearch)
				iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
				iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
				iFadeStyle=EIRFadeLeftToRight;
				if(iIsdsWrapper->GetBannerUrl())
					{
					 if(iRockerIndex==0)
						 {
						 iRockerIndex=iStationsData->Count();
						 }
					 if( iUi->VerifyNetworkConnectivityL() )
						{
						iNextOrPrevRequest=ETrue;
						}
					else
						{
						ConnectToPresetL();
						}
					break;
					}
				else
				{
				if(iRockerIndex< 0)
					{
					iRockerIndex =(iStationsData->Count())-1 ;
					}
				if( iUi->VerifyNetworkConnectivityL() )
					{
					iNextOrPrevRequest=ETrue;
					}
				else
					{
					ConnectToPresetL();
					}
				break;
				}
				}

			else if(iNowPlayingWrapper->GetView()==EPls)
				{
				iNowPlayingWrapper->SetView(EPls);
				if(iRockerIndex< 0)
					{
                    TInt plslist=(iUi->iURLs.Count()) / KTwo ;
					iRockerIndex =plslist-1;
					}
				iFadeStyle=EIRFadeLeftToRight;
				if( iUi->VerifyNetworkConnectivityL() )
					{
					 iNextOrPrevRequest=ETrue;
					}
					else
					{
					ConnectToPresetL();
					StartTimerL();
					}
				}
			break;
			}
		case EIRCmdPlay:
		case EPlayCmd:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

			if( iContainer )
				{
				if( iUi->VerifyNetworkConnectivityL() )
					{
					iRequestPending = ETrue;
					break;
					}
				else
					{
					PlayAndStopControlL();
					}
				}
			}
		break;
		case EIRCmdStop:
		case EStopCmd:
			{
			if( iContainer )
				{
				iShowBitrate=ETrue;
				PlayAndStopControlL();
				}
			}
		break;
		case EFmRecOrPhone:
			{
			if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				LaunchFMTransmitterL();
				}
			}
		break;

		case EGotoMusicStore:
		case EFindInShop:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

		    _LIT(KLaunchStr, "launch");
		    _LIT(KFindStr, "find");

		    if(aCommand == EGotoMusicStore)
			    {
				DoFindInMusicShopL(iNowPlayingWrapper->MetaData().Song(),
			                     iNowPlayingWrapper->MetaData().Artist(),
			                     KNullDesC,EFalse);
				iNowPlayingWrapper->UpdateNmsLogEventsL(KLaunchStr);
			    }
			else if(aCommand == EFindInShop)
				{
				DoFindInMusicShopL(iNowPlayingWrapper->MetaData().Song(),
			                     iNowPlayingWrapper->MetaData().Artist(),
			                     KNullDesC,ETrue);
				/**
				 * Increment the FindInMusicStore count by one
				 */
				iUi->FindInMusicStoreCounter();







				iNowPlayingWrapper->UpdateNmsLogEventsL(KFindStr);
				}
			}
		break;

		case  EStereoMode:
			{
			iIsOn = ETrue;
			break;
			}
		case EOnCmd:
			{
			iIsOn=ETrue;
			iDs=EOnCmd;
			iStereoMode = 1;
			iIRSettings->SetStereoModeL(iStereoMode);
			GetStereoModeL();
			DynInitMenuPaneL(R_NOW_SUBMENU,iMenuPane);
			break;
			}
		case EOffCmd:
			{
			iIsOn=ETrue;
			iDs=EOffCmd;
			iStereoMode = 0;
			iIRSettings->SetStereoModeL(iStereoMode);
			GetStereoModeL();
			DynInitMenuPaneL(R_NOW_SUBMENU,iMenuPane);
			break;
			}
		case EEqualizer:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

			iToolbar->SetToolbarVisibility(EFalse, EFalse);
			iEaualizerOn=ETrue;

#ifdef __IRA_FEATURE_EQUALIZER
			if (iMpxEqualizerViewPlugin)
				{
				iMpxEqualizerViewPlugin->ActivateViewL();
				}
#endif

		    iEaualizerOn=EFalse;
			iToolbar->SetToolbarVisibility(ETrue, EFalse);
			}
		break;
		case EHistory:
			{
			_LIT8(KNull,"");
			AppUi()->ActivateLocalViewL( KIRHistoryListViewId ,KNowPlayingLaunch,KNull);
			break;
			}
		case ESettingsCmd:
			{
			AppUi()->ActivateLocalViewL(KIRSettingsViewId);
			break;
			}
		case EHelpCmd:
			{
			if ( iUi->CheckAndNotifyLowDiskSpaceL() )
				{
				break;
				}

			TCoeHelpContext context = HelpContext();
            // Launch help app with help context of the view.
            CArrayFix<TCoeHelpContext>* array = new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
            array->AppendL( context );
            // Launch help application with help context of this view.
            // It must be verified that the ownership of the help context array is transfered.
            // Remember to verify also CIRSettingsViewBase help application launch.
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), array );
			break;
			}
		case EIRCmdVolumeControl:
			{
				if(iVolPopup)
					{
						if(iVolumeMute)
							{
							iVolumeBarValue=0;
							}
						else
							{
							iVolumeBarValue=iUi->iIRSettings->GetVolumeSetting();
							}
					iVolPopup->SetValue(2);
					if(iVolumeMute)
						{
						TInt index = iNowPlayingWrapper->GetMediaClientInstance()->MaxVolume()/KMaxVolume;
						TInt volume = index*iVolumeBarValue;
						iNowPlayingWrapper->GetMediaClientInstance()->SetVolume(volume);
						}
					else
						{
						iNowPlayingWrapper->SetVolumeL(iVolumeBarValue);
						}
					if(!iVolPopup->IsFocused())
					iVolPopup->ShowVolumePopupL();
					}

			}
		break;
		case EGotoMainView:
		{
		_LIT8(KNull,"");
		AppUi()->ActivateLocalViewL( KIRMainChoiceViewID,KMainViewLaunch , KNull);
		break;
		}
		case EAknSoftkeyBack:
			{
			if((iUi->GetPlsSaved().Count()) &&
			   (iUi->GetListenFromPls()) &&
			    iUi->GetListenFromSecondPls())
				{
				if(iUi->GetPlsSaved()[iRockerIndex]==0)
					{
					RBuf sumvalue;
					HBufC* textResource = StringLoader::LoadLC(R_IRAPP_NOTE_POPUP_SAVE );
					sumvalue.Create(*textResource);
					CIRDialogLauncher* queryNote = CIRDialogLauncher::NewL();
					CleanupStack::PushL( queryNote );
					TInt dlgValue;
					queryNote->ShowQueryDialogDeletionL(sumvalue,dlgValue);
					if(dlgValue)
						{
						AddStationsL();
						}
					CleanupStack::PopAndDestroy( queryNote );
					CleanupStack::PopAndDestroy( textResource );
					sumvalue.Close();
					}
				}
			CIRBaseView::HandleCommandL(aCommand);
			}
		default:
			{
			CIRBaseView::HandleCommandL(aCommand);
			break;
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::HandleCommandL Exiting" );
	}

// ---------------------------------------------------------
// CIRNowPlayingView::UpdateStationNameL()
// This function updates station name
// ---------------------------------------------------------
//
void CIRNowPlayingView::UpdateStationNameL(TIRFadeStyle /*aFadeStyle*/)
	{

	IRLOG_DEBUG( "CIRNowPlayingView::UpdateStationNameL Activating" );


	if(iContainer)
		{
		CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
	 if(iUpdateLastplayed)
	 {

		if(iNowPlayingWrapper->iNowPlayingPreset)
		{
		// store the last played station
		*ui->iLastPlayed->iLastPlayedStation = *iNowPlayingWrapper->iNowPlayingPreset;
		TRAPD(err,ui->iLastPlayed->CommitLastPlayedChannelL());
		TInt error=KErrNone;
		if(err != KErrNone)
			{
			error = ui->iLastPlayed->RemoveLastPlayedFile();
			User::Leave(error);
			}
		}
	 }

	 if(iUpdateStationName)
	 {
		// Check whether the currently playing station is from fav db to enable
		// switching/browsing b/w channels that are there in fav db &
		// enable the left & right rocker keys accordingly
		if(iNowPlayingWrapper->GetView()==EFav)
			{
			if(ui->iFavPresets->iFavPresetList.Count() >= 1)
				{
				//fovorities list is not empty
				TInt index = ui->iFavPresets->SearchPreset(iNowPlayingWrapper->iNowPlayingPreset->
														   GetId(),iNowPlayingWrapper->
														   iNowPlayingPreset->UniqId());
				if (index != KErrNotFound )
					{
					//if there are contents in favorites
					iIndex = index;
					iRockerIndex = iIndex;
					//assigning the preset
					ui->iFavPresets->iFavPresetList[iIndex]->CopyPresetData(*iNowPlayingWrapper->
																			 iNowPlayingPreset);
					iBuffer.Close();
					iBuffer.Create(iNowPlayingWrapper->iNowPlayingPreset->GetName().Length()+
								   KPresetNumbering);
					iBuffer.Append(iNowPlayingWrapper->iNowPlayingPreset->GetName());
					}
				else
					{
					iIndex = KErrNotFound;
					iBuffer.Close();
					iBuffer.Create(iNowPlayingWrapper->iNowPlayingPreset->GetName().Length()+
								   KPresetNumbering);
					iBuffer.Append(iNowPlayingWrapper->iNowPlayingPreset->GetName());
					}
				}
			}
		else if(iNowPlayingWrapper->GetView()==ESearch||iNowPlayingWrapper->GetView()==EStations ||
				iNowPlayingWrapper->GetView()==EBrowseByTopStations )
			{
			iRockerIndex = (iIsdsWrapper->GetCurrentPlayingIndex());
			iBuffer.Close();
			iBuffer.Create(iNowPlayingWrapper->iNowPlayingPreset->GetName().Length()+
						   KPresetNumbering);
			iBuffer.Append(iNowPlayingWrapper->iNowPlayingPreset->GetName());
			}
		else if(iNowPlayingWrapper->GetView()==ELastplayed||
				iNowPlayingWrapper->GetView()==EHistoryView ||iNowPlayingWrapper->GetView()==EPls)
			{
			 if(iNowPlayingWrapper->GetView()==ELastplayed)
				{
				TInt index = ui->iFavPresets->SearchPreset(iNowPlayingWrapper->
				iNowPlayingPreset->GetId(),iNowPlayingWrapper->iNowPlayingPreset->UniqId());
				if (index != KErrNotFound )
				{
						iIndex=index;
						iRockerIndex = iIndex;

				}
				else if(iNowPlayingWrapper->iNowPlayingPreset->GetChannelType()==0)
					{
					iIndex=1;
					iRockerIndex = iIndex;
					}
				else
					{
					iIndex=1;
					iRockerIndex = iIndex;
					}
				}
			else if(iNowPlayingWrapper->GetView()==EPls)
				{
				iIndex=	iUi->GetPlsIndex();
				iRockerIndex = iIndex;
				}
			else if(iNowPlayingWrapper->GetView()==EHistoryView)
				{
				iIndex=1;
				iRockerIndex = iIndex;
				}

			iBuffer.Close();
			iBuffer.Create(iNowPlayingWrapper->iNowPlayingPreset->GetName().Length()+
						   KPresetNumbering);
			iBuffer.Append(iNowPlayingWrapper->iNowPlayingPreset->GetName());
			}
	 }
		}

	IRLOG_DEBUG( "CIRNowPlayingView::UpdateStationNameL Exiting" );
	}

// ---------------------------------------------------------
// CIRNowPlayingView::UpdateStationLogosL
// ---------------------------------------------------------
void CIRNowPlayingView::UpdateStationLogosL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::UpdateStationLogosL Activating" );
	TBool upLoadLogo=EFalse;
	if( iContainer )
		{
		RArray<TInt> idArray( KIRLogoArrayGranularity );
		CleanupClosePushL( idArray );
		CPtrC8Array* dataArray = new (ELeave) CPtrC8Array( KIRLogoArrayGranularity );
		CleanupStack::PushL( dataArray );
		if(iNowPlayingWrapper->GetView()==EFav||iNowPlayingWrapper->GetView()==ELastplayed
			||iNowPlayingWrapper->GetView()==EHistoryView||
		   	iNowPlayingWrapper->GetView()==EPls )
			{
			if(iPresetValue==0||iNowPlayingWrapper->GetView()==ELastplayed)
				{
					if(iCurrentPreset->GetLogoData() != KNullDesC8 )
						{
						dataArray->AppendL(iCurrentPreset->GetLogoData() );
						idArray.AppendL( iRockerIndex);
						iContainer->StationLogoControl().SetLogosL( idArray, *dataArray );
						iPresetValue++;
						}

				}
			else
				{
				if(iPrevPreset1->GetLogoData() != KNullDesC8 && !iDefaultLogo)
					{
					dataArray->AppendL(iPrevPreset1->GetLogoData() );
					idArray.AppendL( iPresetCount );
					}
				if(iCurrentPreset->GetLogoData() != KNullDesC8 )
					{
					 	 upLoadLogo=ETrue;
					dataArray->AppendL(iCurrentPreset->GetLogoData() );
					idArray.AppendL(iRockerIndex);
					}
				if(upLoadLogo)
				iContainer->StationLogoControl().SetLogosL( idArray, *dataArray );
				iDefaultLogo=EFalse;
				}
			}
		if(iNowPlayingWrapper->GetView()==ESearch||iNowPlayingWrapper->GetView()==EStations
			 ||iNowPlayingWrapper->GetView()==EBrowseByTopStations )
			{
			if(iPresetValue==0)
				{
				if(iCurrentPreset->GetLogoData() != KNullDesC8 )
					{
					dataArray->AppendL(iCurrentPreset->GetLogoData() );
					idArray.AppendL(iLogoIndex);
					iContainer->StationLogoControl().SetLogosL( idArray, *dataArray );
					iPresetValue++;
					}
				}
			else
				{
				if(iPrevPreset1->GetLogoData() != KNullDesC8 && !iDefaultLogo)
					{
					 upLoadLogo=ETrue;
					dataArray->AppendL(iPrevPreset1->GetLogoData() );
					idArray.AppendL(iPresetCount);
					}
				if(iCurrentPreset->GetLogoData() != KNullDesC8 )
					{
					 upLoadLogo=ETrue;
					dataArray->AppendL(iCurrentPreset->GetLogoData() );
					idArray.AppendL(iLogoIndex );
					}
				iContainer->StationLogoControl().SetLogosL( idArray, *dataArray );
				iDefaultLogo=EFalse;
				}
			}
		CleanupStack::PopAndDestroy( dataArray );
		CleanupStack::PopAndDestroy( &idArray );
		}
	IRLOG_DEBUG( "CIRNowPlayingView::UpdateStationLogosL Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingView::DisplayBitrate
// Displays the song name on to the screen
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DisplayBitrate(const TDesC& aBitrate) const
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DisplaySongNameL - Entering" );
	if( iContainer )
		{
		TRAP_IGNORE(iContainer->SetBitrateL( aBitrate));
		if(iNowPlayingWrapper->iPlaying || !iShowBitrate)
			{
			if(iBitrateIndex)
				{
				if ( iContainer->RadioIdle().State() == CIRRadioIdle::ERunning )
					{
					iContainer->RadioIdle().UpdateRadioIdleContent( aBitrate,
					iBitrateIndex );
					}

				}
			}
			else
			{
			if(iBitrateIndex)
				{
				if ( iContainer->RadioIdle().State() == CIRRadioIdle::ERunning )
					{
					iContainer->RadioIdle().UpdateRadioIdleContent( iBuffer,
					iBitrateIndex );
					}

				}
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::DisplaySongNameL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingView::DisplaySongArtistNameL
// Displays the song name on to the screen
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DisplaySongArtistNameL(const TDesC& aSongName,const TDesC& aArtistName)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DisplayArtistNameL - Entering " );
	if(aSongName==KNullDesC &&	aArtistName==KNullDesC )
		{
		iNowPlayingWrapper->SetMedadataForErrotrConditions(ETrue);
		DisplaySongArtistNameL(KConst1,KConst1);
		return;
		}
	if(aSongName!=KNullDesC && (!iNowPlayingWrapper->GetMedadataForErrotrConditions())
	&&	aArtistName!=KNullDesC)
		{
		CDesCArray* strings = new ( ELeave ) CDesCArrayFlat( 2 );
		CleanupStack::PushL( strings );
		if(iNowPlayingWrapper->GetMarquee())
			{
			strings->AppendL(  aArtistName);
			strings->AppendL( aSongName );
			}
		else
			{
			strings->AppendL( aSongName );
			strings->AppendL(  aArtistName);
			}
		iNowPlayingWrapper->SetMedadataForErrotrConditions(EFalse);
		HBufC* text = StringLoader::LoadLC( R_IR_NOW_ARTIST_SONG, *strings );
		iMetadata.Close();
		iMetadata.Create(text->Length());
		iMetadata.Copy(*text);
		CleanupStack::PopAndDestroy( 2, strings );
		}
	else
		{
		iMetadata.Close();
		iMetadata.Create(aSongName.Length()+aArtistName.Length()+KTwo);
		if(aSongName!=KNullDesC)
			{
			iMetadata.Append(aSongName);
			}
		if(aArtistName!=KNullDesC)
			{
			iMetadata.Append(aArtistName);
			}
		}
	if(iNowPlayingWrapper->GetMarquee())
		{
		TRAP_IGNORE( iContainer->StationInformationDisplay().RdsDataReceivedL( iMetadata ) );
		if ( iContainer->RadioIdle().State() == CIRRadioIdle::ERunning )
			{
			iContainer->RadioIdle().UpdateRadioIdleContent( iMetadata,
			iMetadataIndex );
			}
		}

	IRLOG_DEBUG( "CIRNowPlayingView::DisplayArtistNameL - Exiting" );
}

// ---------------------------------------------------------------------------
// CIRNowPlayingView::DisplayByteCounter
// Formats byte counter bytes for display
// For example, Byte counter is indicated in the following formats:
//   0.00 to 9.99 MB
//   10.0 to 99.9 MB
//   100 MB and more
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DisplayByteCounterL(TReal32  total_bytes)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DisplayByteCounter Activating" );

	TReal32 totalMB = total_bytes / KIRMBtoBytesConversion;

	// required for byte counter in deciaml format
	CArrayFix<TInt>* byteCtr_nums = new (ELeave) CArrayFixFlat<TInt>( KTwo );
	CleanupStack::PushL( byteCtr_nums );

	HBufC* text = NULL;
	if( total_bytes < KIRByteCounterDisplayThreshold1 )
		{
		TInt wholeMB = floor(totalMB);
		TInt fractionMB = (totalMB - wholeMB) * KHundred;
		byteCtr_nums->AppendL( wholeMB );
		byteCtr_nums->AppendL( fractionMB );
		text = StringLoader::LoadLC( R_IRAPP_NOWPLAYING_BYTECOUNTER_DECI, *byteCtr_nums );

		if( fractionMB < KTen )
			{
			RBuf buff;
			buff.CreateL( *text, text->Length()+1 );
			TInt insertPos = KTwo;
			while(wholeMB >= KTen)
				{
				++insertPos;
				wholeMB /= KTen;
				}
			_LIT( KZero, "0" );
			buff.Insert( insertPos, KZero );
			CleanupStack::PopAndDestroy( text );
			text = HBufC::NewLC( buff.Length() );
			*text = buff;
			buff.Close();
			}
		}
	else if( total_bytes >= KIRByteCounterDisplayThreshold1 &&
			  total_bytes < KIRByteCounterDisplayThreshold2 )
		{
		TInt wholeMB = floor(totalMB);
		TInt fractionMB = (totalMB - wholeMB) * KTen;
		byteCtr_nums->AppendL( wholeMB );
		byteCtr_nums->AppendL( fractionMB );
		text = StringLoader::LoadLC( R_IRAPP_NOWPLAYING_BYTECOUNTER_DECI, *byteCtr_nums );
		}
	else
		{
		text = StringLoader::LoadLC( R_IRAPP_NOWPLAYING_BYTECOUNTER, totalMB );
		}

	if( iContainer )
		{
		iContainer->SetByteCounterL( *text );
		}
	CleanupStack::PopAndDestroy( text );
	CleanupStack::PopAndDestroy( byteCtr_nums );

	IRLOG_DEBUG( "CIRNowPlayingView::DisplayByteCounter - Exiting" );
	}


//
// ---------------------------------------------------------------------------
// CIRNowPlayingView::OnViewActivationL
// Displays the song name on to the screen
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::OnViewActivationL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::OnViewActivationL - Entering" );
	UpdateStationNameL(iFadeStyle);
	if(iNowPlayingWrapper->MetaData().Artist().Length()
		&& iNowPlayingWrapper->MetaData().Song().Length() )
		{
		iNowPlayingWrapper->SetMedadataForErrotrConditions(EFalse);
		}
    DisplaySongArtistNameL(iNowPlayingWrapper->MetaData().Artist(),
    					   iNowPlayingWrapper->MetaData().Song());
	IRLOG_DEBUG( "CIRNowPlayingView::OnViewActivationL - Exiting." );
	}

 // -----------------------------------------------------------------------------
// ConnectToPresetL()
// Function called to connect to any channel after the timer expires
// ------------------------------------------------------------------------------
//
void CIRNowPlayingView::ConnectToPresetL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::ConnectToPresetL - Entering" );
	iSameUrl=EFalse;
	if(iStereoEffect)
		{
		if(iStereoEffect->IsEnabled())
			{
			iStereoEffect->DisableL();
			}
		}
	if(iNowPlayingWrapper->GetView()==EFav)
		{
			iNowPlayingWrapper->SetConnectToServerState(ETrue);
			iIndex=iRockerIndex;
			//connected from saved preset
			iNowPlayingWrapper->SetCurrentConnectionSource(EIRPresetIsds);
			if(iUi->iFavPresets->iFavPresetList.Count() >= 1)
				{
				if(iUi->iFavPresets->iFavPresetList[iRockerIndex]->
					GetChannelType() == CIRIsdsPreset::EIsdsPreset)
					{
					// Sync Favourite Isds preset
					SyncPresetL(iUi->iFavPresets->iFavPresetList[iRockerIndex]->GetId(),
					iUi->iFavPresets->iFavPresetList[iRockerIndex]->GetLastModifiedTime());
					}
				else
					{
					// TO Do: need to check

					iNowPlayingWrapper->SetFavFocus(iRockerIndex+1);
					// Connect to Favourite User-defined preset
					iUi->iFavPresets->iFavPresetList[iRockerIndex]->
						CopyPresetData(*iNowPlayingWrapper->iNowPlayingPreset);
					iNowPlayingWrapper->ConnectToChannelL(*iNowPlayingWrapper->iNowPlayingPreset);
					StartTimerL();
					}
				}
			}
	else  if(iNowPlayingWrapper->GetView()==ESearch|| iNowPlayingWrapper->GetView()==EStations ||
	iNowPlayingWrapper->GetView()==EBrowseByTopStations )
		{
	       if(iRockerIndex >=0 && iRockerIndex <=iStationsData->Count())
	       {

		 	iNowPlayingWrapper->SetWhenUserCancelsBufferingWaitBar(EFalse);
		 	iNowPlayingWrapper->SetListenFromIsdsValue(EFalse);
		    iNowPlayingWrapper->CreateWaitDialogL();
			iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);

			iIsdsWrapper->IsdsListenRequestL(this,iRockerIndex);
	       }
		}
	else if(iNowPlayingWrapper->GetView()==EPls)
		{
		iIndex=iRockerIndex;
		ListenFromPlsL();
		}
	else
		{
		// if browsing to same channel that we were and
		// playback were stopped, need to try to restart it
		if( !iNowPlayingWrapper->iPlaying )
			{
			TRAP_IGNORE( PlayAndStopControlL() );
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::DisplayByteCounter Exiting" );
}


// ---------------------------------------------------------------------------
// CIRNowPlayingView::PlayAndStopControlL()
// Used to stop & play the station
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::PlayAndStopControlL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::StopAndPlayControlL - Entering" );
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
	iNowPlayingWrapper->PlayAndStopL();
	IRLOG_DEBUG( "CIRNowPlayingView::StopAndPlayControlL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingView::DynInitMenuPaneL
// Dynamically handles menu pane.
// From class MEikMenuObserver.
// Itialiases the items on the menu.
// ---------------------------------------------------------------------------
void CIRNowPlayingView::DynInitMenuPaneL(TInt aResourceId,
		CEikMenuPane* aMenuPane)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DynInitMenuPaneL - Entering " );
	iMenuPane=aMenuPane;
	if(aResourceId == R_IR_MENU_NOWPLAYING )
		{
		if(!iContainer)
			{
 			aMenuPane->SetItemDimmed(EGotoMainView,ETrue);
			aMenuPane->SetItemDimmed(EStationDetailsCmd,ETrue);
			aMenuPane->SetItemDimmed(EFmRecOrPhone,ETrue);
			aMenuPane->SetItemDimmed(EAddtoStationsCmd,ETrue);
			aMenuPane->SetItemDimmed(EStereoMode,ETrue);
			aMenuPane->SetItemDimmed(EEqualizer,ETrue);
			aMenuPane->SetItemDimmed(EHistory,ETrue);
			aMenuPane->SetItemDimmed(EFindInShop,ETrue);
            aMenuPane->SetItemDimmed(EGotoMusicStore, ETrue);
			aMenuPane->SetItemDimmed(ESettingsCmd,ETrue);
			aMenuPane->SetItemDimmed(EHelpCmd,ETrue);
			aMenuPane->SetItemDimmed(EExitCmd,ETrue);
			}
		else
			{

			 if(iUi->GetPlsActiavted() && iUi->GetListenFromPls())
				{
				aMenuPane->SetItemDimmed(EGotoMainView,EFalse);
				}
			else
				{
				 aMenuPane->SetItemDimmed(EGotoMainView,ETrue);
				}
			#ifdef FF_FMTX

				aMenuPane->SetItemDimmed(EFmRecOrPhone,EFalse);
			#else

				aMenuPane->SetItemDimmed(EFmRecOrPhone,ETrue);
		     #endif

#ifdef __IRA_FEATURE_EQUALIZER
			/**
			 * Activate "FindInShop" or "Goto MusicStore" only if iMpxFindInShop
			 * object is succcessfully created. Show "FindInShop" option for
			 * isds presets which has MetaData and MusicStoreEnalbed status as true.
			 * Otherwise show "Goto MusicStore".
			 */
			if (iMPXFindInShop)
				{
				if (iNowPlayingWrapper->iNowPlayingPreset->
				             GetChannelType() == CIRIsdsPreset::EIsdsPreset)
					{
					if (IsMetaData() && MusciStoreEnabledStatus() )
						{
						aMenuPane->SetItemDimmed(EFindInShop,EFalse);
						aMenuPane->SetItemDimmed(EGotoMusicStore, ETrue);
						}
					else
						{
					    aMenuPane->SetItemDimmed(EFindInShop,ETrue);
					    aMenuPane->SetItemDimmed(EGotoMusicStore, EFalse);
						}
					}
				else //preset is UserDefined
					{
					aMenuPane->SetItemDimmed(EFindInShop,ETrue);
					aMenuPane->SetItemDimmed(EGotoMusicStore, EFalse);
					}
				}
			else
#endif // __IRA_FEATURE_EQUALIZER
				{
				aMenuPane->SetItemDimmed(EFindInShop,ETrue);
				aMenuPane->SetItemDimmed(EGotoMusicStore, ETrue);
				}

#ifdef __IRA_FEATURE_EQUALIZER
			if (iMpxEqualizerViewPlugin)
				{
				aMenuPane->SetItemDimmed(EEqualizer,EFalse);
				}
			else
#endif // __IRA_FEATURE_EQUALIZER
				{
				aMenuPane->SetItemDimmed(EEqualizer,ETrue);
				}
			}
		}
	if(aResourceId==R_NOW_SUBMENU)
		{
		if(iDs==EOnCmd)
			{
			aMenuPane->SetItemButtonState( EOnCmd,
			iIsOn?
			EEikMenuItemSymbolOn:
			EEikMenuItemSymbolIndeterminate );
			}
		if(iDs==EOffCmd)
			{
			aMenuPane->SetItemButtonState( EOffCmd,
			iIsOn?
			EEikMenuItemSymbolOn:
			EEikMenuItemSymbolIndeterminate );
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::DynInitMenuPaneL - Exiting." );
	}

// ---------------------------------------------------------
// CIRNowPlayingView::HandlePresetChangedL
// ---------------------------------------------------------
void CIRNowPlayingView::HandlePresetChangedL( TInt /*aId*/, TUid /*aDataHandler*/,
											  MPSPresetObserver::TPSReason /*aReason*/ )
	{
	IRLOG_DEBUG( "CIRNowPlayingView::HandlePresetChangedL - Entering." );
	if(iContainer)
	{
	if(iNowPlayingWrapper->GetView()==EFav)
		{
		TInt presetCount=iUi->iFavPresets->iFavPresetList.Count();
		if(presetCount==1||presetCount==0)
			{
			iShowToolBar=EFalse;
			}
		else
			{
			iShowToolBar=ETrue;
			}
		}
	ConstructToolbarL();
}

	IRLOG_DEBUG( "CIRNowPlayingView::HandlePresetChangedL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingView::AddStationsL()
// Saves the currently playing station into fav DB
// ---------------------------------------------------------------------------
void CIRNowPlayingView::AddStationsL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::AddStationsL - Entering" );
	CIRUi* ui = reinterpret_cast<CIRUi*>( iCoeEnv->AppUi() );
	TInt returnVal;
	ui->iFavPresets->AddPresetL(*(iNowPlayingWrapper->iNowPlayingPreset),returnVal);
	if(returnVal==KErrNoMemory)
		{
		iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
		return;
		}
	else
		{
		if(iUi->iFavPresets->iFavPresetList.Count() == KPresetsArrayMaxLength)
			{
			iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL
					(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
			return;
			}
		else
			{
			RBuf channel;
			HBufC* textResource = StringLoader::LoadLC(R_IRAPP_STATIONS_POPUP_SAVED,
													   iNowPlayingWrapper->iNowPlayingPreset->
													   GetName());
			channel.Create(*textResource);
			iIsdsWrapper->GetDialogLauncherInstance()->ShowConfirmationNoteL( channel,ETrue );
			CleanupStack::PopAndDestroy(textResource);
			channel.Close();
			}
		if(iUi->GetListenFromPls() && iUi->GetPlsSaved().Count())
			{
			iUi->GetPlsSaved()[iRockerIndex]=1;
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::AddStationsL - Exiting." );
	}

// CIRNowPlayingView::PresetLogoDownloadedL()
//----------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::PresetLogoDownloadedL - Entering" );
	TInt Iindex=KDefaultValue;
	TInt LogoDownloadedIndex=KDefault;
	/*if(iNowPlayingWrapper->iPlaying)
		{
		Iindex=aPreset->GetId();
		LogoDownloadedIndex=iNowPlayingWrapper->iNowPlayingPreset->GetId();
		if(Iindex==LogoDownloadedIndex && aPreset->GetLogoData().Length())
			{
			iNowPlayingWrapper->iNowPlayingPreset->SetLogoData(aPreset->GetLogoData());
			UpdateLastplayedL();
			}
		}*/
	if(iContainer)
	{
	if(iNowPlayingWrapper->GetView()==EFav)
		{
		Iindex = iUi->iFavPresets->SearchPreset(iNowPlayingWrapper->iNowPlayingPreset->
				   GetId(),iNowPlayingWrapper->
				   iNowPlayingPreset->UniqId());
		LogoDownloadedIndex= iUi->iFavPresets->SearchPreset(aPreset->GetId()
					   ,aPreset->UniqId());
		}
	else if(iNowPlayingWrapper->GetView()==EStations
	   || iNowPlayingWrapper->GetView()==EBrowseByTopStations
	   ||iNowPlayingWrapper->GetView()==ESearch)
		{
		Iindex=aPreset->GetId();
		LogoDownloadedIndex=iNowPlayingWrapper->iNowPlayingPreset->GetId();
		}
	else
		{
		Iindex=0;
		LogoDownloadedIndex=0;
		}
	if(LogoDownloadedIndex==Iindex)
	{
		iStationHasLogo=ETrue;
		if(iPrecount==0||iNowPlayingWrapper->GetView()==ELastplayed)
			{
			iDefaultLogo=EFalse;
			if(aPreset->GetLogoData() != KNullDesC8 )
				{
				*iCurrentPreset=*aPreset;
				iPrecount++;
				}
			}
	else
		{
		if(!iDefaultLogo)
			{
			*iPrevPreset1=*iCurrentPreset;
			}
		*iCurrentPreset=*aPreset;
		}

	if ( iUi->CheckAndNotifyLowDiskSpaceL() )
		{
		return;
		}

	if(iNowPlayingWrapper->GetView()==EHistoryView||iNowPlayingWrapper->GetView()==ESearch
	   || iNowPlayingWrapper->GetView()==EFav||iNowPlayingWrapper->GetView()==EStations
	   || iNowPlayingWrapper->GetView()==EBrowseByTopStations
	   ||iNowPlayingWrapper->GetView()==ELastplayed||iNowPlayingWrapper->GetView()==EPls)
			{
			UpdateStationLogosL();
			DisplaylogoL();
			}
	}
	IRLOG_DEBUG( "CIRNowPlayingView::PresetLogoDownloadedL - Exiting" );
	}
	}


// CIRNowPlayingView::PresetLogoDownloadError()
//----------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::PresetLogoDownloadError(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::PresetLogoDownloadErrorL");
	//TO Do:implementation need to done for handling Media animation
	iStationHasLogo=EFalse;
	//TRAP_IGNORE(DrawViewL(iStationHasLogo))
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingView::ConstructToolbarL
// Simulate user pressing play immediately when came into this view.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::ConstructToolbarL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::ConstructToolbarL - Entering" );
	TInt bitmapId;
	TInt maskId;
	if ( AknLayoutUtils::PenEnabled() )
		{
		iToolbar = this->Toolbar();
		if ( iToolbar )
			{
			iToolbar->SetToolbarObserver( this );
			if(iToolbar)
				{

				bitmapId=iUi->ChooseToolBarBitmaps(EMbmInternetradioQgn_indi_tb_previous,
				EMbmInternetradioQgn_indi_tb_previous_white);

				maskId=iUi->ChooseToolBarMaskId(EMbmInternetradioQgn_indi_tb_previous_mask,
				EMbmInternetradioQgn_indi_tb_previous_white_mask);

				// There are no presets or the only preset is active
				HBufC* previousToolTipText =
				CCoeEnv::Static()->AllocReadResourceLC(R_IR_NOW_TOOLTIP_PREVIOUS);
				CAknButton*  	 button1 = CAknButton::NewL(KIRMifFileName,bitmapId,
				maskId,0,0,bitmapId,maskId,bitmapId,maskId,KNullDesC ,
				*previousToolTipText,0,0,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone);

				CleanupStack::PopAndDestroy(previousToolTipText);
				CleanupStack::PushL(button1);
				iToolbar->AddItemL( button1, EAknCtButton, EIRCmdStepPrevPreset, 0, 0 );
				CleanupStack::Pop(button1);

				if(!iNowPlayingWrapper->iPlaying)
					{
					bitmapId=iUi->ChooseToolBarBitmaps(EMbmInternetradioQgn_indi_tb_play,
					EMbmInternetradioQgn_indi_tb_play_white);

					maskId=iUi->ChooseToolBarMaskId(EMbmInternetradioQgn_indi_tb_play_mask,
					EMbmInternetradioQgn_indi_tb_play_white_mask);

					HBufC* playToolTipText = CCoeEnv::Static()->AllocReadResourceLC( R_IR_NOW_TOOLTIP_PLAY);
					CAknButton*    button2 = CAknButton::NewL(KIRMifFileName,bitmapId,
					maskId,0,0,bitmapId,maskId,bitmapId,maskId,KNullDesC ,
					*playToolTipText,0,0,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone);

					CleanupStack::PopAndDestroy(playToolTipText);
					CleanupStack::PushL(button2);
					iToolbar->AddItemL(button2, EAknCtButton, EIRCmdPlay, 0, 1 );
					CleanupStack::Pop(button2);
					}
				else
					{

					bitmapId=iUi->ChooseToolBarBitmaps(EMbmInternetradioQgn_indi_tb_stop,
					EMbmInternetradioQgn_indi_tb_stop_white);

					maskId=iUi->ChooseToolBarMaskId(EMbmInternetradioQgn_indi_tb_stop_mask,
					EMbmInternetradioQgn_indi_tb_stop_white_mask);

					HBufC* stopToolTipText = CCoeEnv::Static()->AllocReadResourceLC( R_IR_NOW_TOOLTIP_STOP);
					CAknButton*    button2 = CAknButton::NewL(KIRMifFileName,bitmapId,
					maskId,0,0,bitmapId,maskId,bitmapId,maskId,KNullDesC
					,*stopToolTipText,0,0,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone);

					CleanupStack::PopAndDestroy(stopToolTipText);
					CleanupStack::PushL(button2);
					iToolbar->AddItemL(button2, EAknCtButton, EIRCmdStop, 0, 1 );
					CleanupStack::Pop(button2);
					}


				bitmapId=iUi->ChooseToolBarBitmaps(EMbmInternetradioQgn_indi_tb_next,
				EMbmInternetradioQgn_indi_tb_next_white);

				maskId=iUi->ChooseToolBarMaskId(EMbmInternetradioQgn_indi_tb_next_mask,
				EMbmInternetradioQgn_indi_tb_next_white_mask);

				HBufC* nextToolTipText1 = CCoeEnv::Static()->AllocReadResourceLC(R_IR_NOW_TOOLTIP_NEXT);
				CAknButton*   button3 = CAknButton::NewL(KIRMifFileName,bitmapId,
				maskId,	0,0,bitmapId,maskId,bitmapId,maskId,KNullDesC ,
				*nextToolTipText1,0,0,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone,KAknsIIDNone);

				CleanupStack::PopAndDestroy(nextToolTipText1);
				CleanupStack::PushL(button3);
				iToolbar->AddItemL(button3, EAknCtButton, EIRCmdStepNextPreset, 0, 2 );
				CleanupStack::Pop(button3);

				// Take toolbar control event handling from touch pane.
				// Events need to be forwarded to touch pane manually.
				iToolbar->SetToolbarVisibility(ETrue, EFalse);
				}
			if(!iShowToolBar)
				{
				iToolbar->SetItemDimmed(EIRCmdStepNextPreset,ETrue,ETrue);
				iToolbar->SetItemDimmed(EIRCmdStepPrevPreset,ETrue,ETrue);
				}
			if(iUi->IsCallActive())
				{
				if(!iNowPlayingWrapper->iPlaying)
					{
					iToolbar->SetItemDimmed(EIRCmdPlay,ETrue,ETrue);
					}
				else
					{
					iToolbar->SetItemDimmed(EIRCmdStop,ETrue,ETrue);
					}
				}

			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::ConstructToolbarL - Exiting" );
	}


// --------------------------------------------------------------------------
// CIRNowPlayingView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CIRNowPlayingView::OfferToolbarEventL( TInt aCommand )
	{
	IRLOG_DEBUG( "CIRNowPlayingView::OfferToolbarEventL - Entering" );
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRNowPlayingView::OfferToolbarEventL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingView::Displaylogo
// Simulate user pressing play immediately when came into this view.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DisplaylogoL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DisplaylogoL - Entering" );
	if(iContainer)
		{
		iStationHasLogo=ETrue;
		if(iNowPlayingWrapper->GetView()==EFav||iNowPlayingWrapper->GetView()==ESearch
		||iNowPlayingWrapper->GetView()==EStations
		|| iNowPlayingWrapper->GetView()==EBrowseByTopStations
		|| iNowPlayingWrapper->GetView()==EPls)
			{
			iStationHasLogo=iContainer->StationLogoControl().SwitchToLogoL( iLogoIndex );
			}
	else if(iNowPlayingWrapper->GetView()==ELastplayed
	||iNowPlayingWrapper->GetView()==EHistoryView)
		{
		iStationHasLogo=iContainer->StationLogoControl().SwitchToLogoL( iIndex );
		}

		}
	IRLOG_DEBUG( "CIRNowPlayingView::DisplaylogoL - Exiting" );
	}

// ---------------------------------------------------------------------------
// From MIRLogoControlObserver.
// CIRNowPlayingView::LogoReadyForDisplayL
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::LogoReadyForDisplayL()
    {
    IRLOG_DEBUG( " CIRNowPlayingView::LogoReadyForDisplayL " );
    if ( iContainer->RadioIdle().State() != CIRRadioIdle::EStopped )
        {
        iContainer->RadioIdle().HandleCommandL( EShutdown );
        }
    }

// ---------------------------------------------------------------------------
// From MIRLogoControlObserver.
// CIRNowPlayingView::LogoConversionFailedL
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::LogoConversionFailedL()
    {
    IRLOG_DEBUG( "CIRNowPlayingView::LogoConversionFailedL " );
    if ( iContainer->RadioIdle().State() == CIRRadioIdle::EStopped )
        {
        iContainer->RadioIdle().HandleCommandL( EStartup );
        }
    }

// ---------------------------------------------------------------------------
// CIRNowPlayingView::GetLogo
// Simulate user pressing play immediately when came into this view.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::GetLogoL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::GetLogoL - Entering" );
	if(iContainer)
	{
	iLogoIndex=iRockerIndex;
	if(iNowPlayingWrapper->iNowPlayingPreset->GetImgUrl().Length() != 0)
		{
		iUi->iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
		GetLogoDownloadEngine()->SendRequestL(iNowPlayingWrapper->iNowPlayingPreset,
						this,KTwo,KSize,KSize);
		}

	else
		{
		iStationHasLogo=EFalse;
		iDefaultLogo=ETrue;
		}

	}
	IRLOG_DEBUG( "CIRNowPlayingView::GetLogoL - Exiting" );

	}
// ---------------------------------------------------------
// CIRNowPlayingView::HandleForegroundEventL
// ---------------------------------------------------------
//
void CIRNowPlayingView::HandleForegroundEventL(TBool aForeground)
	{
	IRLOG_DEBUG2(" *** S60InternetRadio -- CIRNowPlayingView::HandleForegroundEventL(%d)",aForeground);

	CIRBaseView::HandleForegroundEventL(aForeground);

    if (aForeground)
        {
		if (iStartUp)
			{
			CIRUi* appUi = static_cast<CIRUi*>( AppUi() );

			iStartUp = EFalse;
			if(appUi->SystemEventCollector().IsNetworkCoverage())
				{
				HandleSystemEventL(EIRNetworkCoverageUp);
				}
			else
			{
			HandleSystemEventL(EIRNetworkCoverageDown);
			}
			if(!iEaualizerOn)
				{
				if(iContainer && !iCallActive)
					{
					iFadeStyle=EIRFadeUnknown;
			delete iConnectionCleanupTimer;
			iConnectionCleanupTimer = NULL;
        TRAPD( err, iConnectionCleanupTimer = CPeriodic::NewL( CActive::EPriorityStandard ) )
        if ( err == KErrNone )
            {
            iConnectionCleanupTimer->Start( KWaitTimeInMicroSeconds, 0,
            			 TCallBack( StaticConnectionCleanupCallback, this ) );
            }
					}
				}
			}
		else
		{
		if(!iEaualizerOn)
			{
			if(	iContainer && !iCallActive)
				{
				if ( iContainer )
	                {
	                iContainer->LayoutControl();
	                }
				iFadeStyle=EIRFadeNoFade;
				DrawViewL(iStationHasLogo);

				}

			}
		}
		}
	else
		{
		if(iContainer)
			{
			CAlfEnv* env = CAlfEnv::Static();
			if ( env )
				{
				iContainer->RadioIdle().HandleCommandL( EShutdown );
				}
			}

		/*if(iContainer)
			{
			if(iVolPopup->IsVisible())
				{
				iVolPopup->CloseVolumePopup();
				}
			}*/
		}
}

// ---------------------------------------------------------
// CIRNowPlayingView::Draw
// ---------------------------------------------------------
//
void CIRNowPlayingView::DrawViewL( TBool aLogoStatus )
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DrawViewL - Entering" );
	if(iContainer)
	{
	if(iBuffer!=KNullDesC)
		{
			CIRStationInformationData* data = CIRStationInformationData::NewL( *iCoeEnv );
			CleanupStack::PushL( data );
			CIRStationInformationDisplay& display = iContainer->StationInformationDisplay();
			data->SetFadeDirectionality( iFadeStyle );
			data->SetNameL(iBuffer);
			if(iMetadata!=KNullDesC)
				{
				data->SetRdsPsNameL(iMetadata);
				}
			if(iNowPlayingWrapper->GetView()==EFav)
			{
			if(iIndex>=0)
			data->SetIndexL(iIndex);
			}
		RBuf bitrate;
		bitrate.CleanupClosePushL();
	if(iShowBitrate || iNowPlayingWrapper->iPlaying)
		{
		TInt bitRate(0);
		RProperty::Get ( KUidActiveInternetRadioApp, KIRPSBitrate, bitRate );
		HBufC* BitRate = StringLoader::LoadLC(R_IRAPP_NOWPLAYING_BITRATE, bitRate);
		bitrate.Create(BitRate->Length());
		bitrate.Copy(*BitRate);
		CleanupStack::PopAndDestroy(BitRate);
		DisplayBitrate(bitrate);
		}
	///displaying the byte counter///////
	iUi->UpdateNowPlayingByteCounter();
	iContainer->StationLogoControl().SetLogoFadeStyle( iFadeStyle );
		if( iFadeStyle == EIRFadeNoFade
		&& iContainer->RadioIdle().State() != CIRRadioIdle::EStopped )
			{
			if(iNowPlayingWrapper->iPlaying)
				{
				iContainer->RadioIdle().UpdateRadioIdleContent( bitrate,iBitrateIndex  );
				}
			else
				{
				iContainer->RadioIdle().UpdateRadioIdleContent( data->Name(),iBitrateIndex );
				}
			iContainer->RadioIdle().UpdateRadioIdleContent( data->Name(),	iNameIndex  );
			iContainer->RadioIdle().UpdateRadioIdleContent( data->Name(),iNameIndex1 );
			iContainer->RadioIdle().UpdateRadioIdleContent( data->RdsPsName(),iMetadataIndex  ); // Rds
			if(iNowPlayingWrapper->iNowPlayingPreset->GetGenreName().Length())
				{
				iContainer->RadioIdle().UpdateRadioIdleContent(
				iNowPlayingWrapper->iNowPlayingPreset->GetGenreName(),iPSNameIndex );
				}
			else
				{
				iContainer->RadioIdle().UpdateRadioIdleContent( data->Name(),iPSNameIndex); // PS name, updated later
				}
			}
		else
			{
			switch ( iFadeStyle )
				{
				case EIRFadeLeftToRight:
				iContainer->RadioIdle().SetIdleFadeStyle( CIRRadioIdle::EFadeRight );
				break;
				case EIRFadeRightToLeft:
				iContainer->RadioIdle().SetIdleFadeStyle( CIRRadioIdle::EFadeLeft );
				break;
				case EIRFadeNoFade:
				iContainer->RadioIdle().SetIdleFadeStyle( CIRRadioIdle::EFadeStyleNone );
				break;
				default:
				break;
				}
			if(!aLogoStatus)
				{
				iContainer->StationLogoControl().FadeOutLogo(iRockerIndex);
				}
			if ( iContainer->RadioIdle().State() != CIRRadioIdle::EStopped )
				{
				iContainer->RadioIdle().HandleCommandL( EShutdown );
				}
			// Tuned to new station which has no logo, set data and start radio idle.
			/*lint -save -e744 (Info -- Boolean within 'left side of && within if' always evaluates to True)*/
			iContainer->RadioIdle().HandleCommandL( EResetContent );
			if ( !aLogoStatus )
			/*lint -restore*/
				{

				if(iNowPlayingWrapper->iPlaying)
					{
					iBitrateIndex = iContainer->RadioIdle().AddRadioIdleContentL( bitrate );
					}
				else
					{
					iBitrateIndex=iContainer->RadioIdle().AddRadioIdleContentL( data->Name() );
					}
				iNameIndex = iContainer->RadioIdle().AddRadioIdleContentL( data->Name() );
				iNameIndex1=iContainer->RadioIdle().AddRadioIdleContentL( data->Name() );
				iMetadataIndex = iContainer->RadioIdle().AddRadioIdleContentL( data->RdsPsName() ); // Rds
				if(iNowPlayingWrapper->iNowPlayingPreset->GetGenreName().Length())
					{
					iPSNameIndex = iContainer->RadioIdle().AddRadioIdleContentL(
					iNowPlayingWrapper->iNowPlayingPreset->GetGenreName());
					}
				else
					{
					iPSNameIndex = iContainer->RadioIdle().AddRadioIdleContentL( data->Name()); // PS name, updated later
					}
				}

			}
       if ( !aLogoStatus && iContainer->RadioIdle().State() == CIRRadioIdle::EStopped )
	       {
	       iContainer->RadioIdle().HandleCommandL( EStartup );
	       }
       else if(aLogoStatus)
	       {
	       DisplaylogoL();
	       }
		display.SetDisplayedDataL( *data );
		CleanupStack::PopAndDestroy(&bitrate);
		CleanupStack::PopAndDestroy( data );
		iStationHasLogo=aLogoStatus;
		}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::DrawViewL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::StartTimerL()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::StartTimerL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::StartTimerL - Entering" );
	iBuffer.Close();
	iBuffer.Create(iNowPlayingWrapper->iNowPlayingPreset->GetName().Length()+ KPresetNumbering);
	iBuffer.Append(iNowPlayingWrapper->iNowPlayingPreset->GetName());
	if(!iSameUrl)
		{
		iNowPlayingWrapper->SetMarquee(EFalse);
		iNowPlayingWrapper->SetMedadataForErrotrConditions(ETrue);
		DisplaySongArtistNameL(KConst1,KConst1);
		//iNowPlayingWrapper->SetMetadataL()
		}
	iSameUrl=EFalse;
	iStationHasLogo=EFalse;

	DrawViewL(iStationHasLogo);
	GetLogoL();
	IRLOG_DEBUG( "CIRNowPlayingView::StartTimerL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::ReFreshView()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::ReFreshViewL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::ReFreshViewL - Entering" );
	if ( iContainer )
		{
        DrawViewL(iStationHasLogo);
		}
	IRLOG_DEBUG( "CIRNowPlayingView::ReFreshView - Exiting." );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::ResponseL()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::ResponseL(CIRIsdsPreset* /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::ResponseL " );
	//TO DO :: The line can be removed in future written to fix
	// Code Scanner ERROR
		return;
	//no implementation
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::PresetResponseL()
//Callback To Receive ISDS Preset
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::PresetResponseL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::PresetResponseL - Entering" );

	if ( !iUi->CheckAndNotifyLowDiskSpaceL() )
		{
		// TO Do
		// Check the condition in If
		//!iNowPlayingWrapper->GetWhenUserCancelsIsdsBufferingWaitBar()
			iNowPlayingWrapper->SetListenFromIsdsValue(ETrue);
			iNowPlayingWrapper->ListenToChannelL(aPreset);
			StartTimerL();
		}
	IRLOG_DEBUG( "CIRNowPlayingView::PresetResponseL - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetsameUrlFlag()
//For displaing the metadata
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::SetsameUrlFlag(TBool aSameUrl)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::SetsameUrlFlag - Entering" );
	iSameUrl=aSameUrl;
	IRLOG_DEBUG( "CIRNowPlayingView::SetsameUrlFlag - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetsameUrlFlag()
//For displaing the metadata
// ---------------------------------------------------------------------------
//
TInt CIRNowPlayingView::GetsameUrlFlag()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::SetsameUrlFlag - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingView::SetsameUrlFlag - Exiting" );
	return iSameUrl;
	}

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::ErrorL()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::ErrorL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::ErrorL - Entering" );
	if(iIsdsWrapper->GetListenRequest())
		{
		iRockerIndex=iPresetCount;
		iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);
		iNowPlayingWrapper->DestroyWaitDialogL();
		iIsdsWrapper->SetListenRequest(EFalse);
		}
	if(iIsdsWrapper->iConnTimeOut)
		{
		iIsdsWrapper->iConnTimeOut = EFalse;
		}
	if(iIsdsWrapper->iDialogDismissed)
		{
		iIsdsWrapper->iDialogDismissed=EFalse;
		iRockerIndex=iPresetCount;
		}
	if(iUi->iNowPlayingWrapper->iSyncPreset)
		{
		iRockerIndex=iPresetCount;
		iNowPlayingWrapper->DestroyWaitDialogL();
		iUi->iNowPlayingWrapper->iSyncPreset=EFalse;
		}
	//TO DO :: The line can be removed in future nad written to fix
	// Code Scanner ERROR
      IRLOG_DEBUG( "CIRNowPlayingView::ErrorL - Exiting" );
      return;
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::GetStereoModeL()
// Used Getting the status of Stereo Mode
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::GetStereoModeL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::GetStereoModeL - Entering" );
	iStereoMode=iIRSettings->GetStereoMode();
	if(iStereoMode)
		{
		iDs=EOnCmd;

		//Used for later in implementing Stereo mode Functionality

		if(!iStereoEffect)  // If stereo widening is ON and not constructed
				{
				TUint stereoLevel = KHundred;
				// cmdUtil ownership passed into new object
				TRAPD(error,iStereoEffect = CStereoWidening::NewL(
				*(iNowPlayingWrapper->GetMediaClientInstance()->GetPlayerInstance()),EFalse, stereoLevel));
				if(error)
					{
					iStereoEffect = NULL;
					User::Leave(KErrNotSupported);
					}
				}
		if(iNowPlayingWrapper->iPlaying)
			{
			iStereoEffect->EnableL();
			TUint8 level = KHundred;
			iStereoEffect->SetStereoWideningLevelL( level );
			iStereoEffect->ApplyL();
			}
		}
	else
		{
		iDs=EOffCmd;

		//For future use
		//Used for later in implementing Stereo mode Functionality

		if (iStereoEffect)   // If audio effects was not on, then no need to disable
			{
			if(iNowPlayingWrapper->iPlaying)
				{
				iStereoEffect->DisableL();
				}
			}

		}

	IRLOG_DEBUG( "CIRNowPlayingView::GetStereoModeL - Exiting" );
}

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::LaunchMusicShopL()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::LaunchMusicShopL()
	{
#ifdef MUSICSHOP_AVAILABLE
	  IRLOG_DEBUG( "Entered CIRNowPlayingView::LaunchMusicShopL - Entering" );

	  TApaTaskList tasList( iCoeEnv->WsSession() );
	  TApaTask task = tasList.FindApp( KUidMusicStore );

	  if ( task.Exists() )
		  {
		  task.BringToForeground();
		  }
	  else
		{
		RApaLsSession session;
		if ( KErrNone == session.Connect() )
			{
			CleanupClosePushL( session );
			TThreadId threadId;
			session.CreateDocument( KNullDesC, KUidMusicStore, threadId );
			CleanupStack::PopAndDestroy(&session);
			}
		}
	  IRLOG_DEBUG( "Leaving CIRNowPlayingView::LaunchMusicShopL - Exiting" );
#endif
	}

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::DoFindInMusicShopL()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DoFindInMusicShopL( const TDesC& aSongName,
                                            const TDesC& aArtistName,
                                            const TDesC& aAlbumName,
                                            TBool aMusicFlag )
	{
	IRLOG_DEBUG( "Entered CIRNowPlayingView::DoFindInMusicShopL - Entering" );
#ifdef MUSICSHOP_AVAILABLE
	HBufC* url = NULL;
#ifdef __IRA_FEATURE_EQUALIZER
	if(aMusicFlag && iMPXFindInShop)
		{
		//which means, create url for to FindInShop operation
		url = iMPXFindInShop->CreateSearchURLL( aSongName,
	                                  aArtistName,
	                                  aAlbumName,
	                                  KNullDesC,     // Composer - Not used
	                                  KNullDesC );   // Genre - Not used
		CleanupStack::PushL(url);   // the pointer will be poped
		}
	else
#endif // __IRA_FEATURE_EQUALIZER
		{
		//which means, crate url for to GotoMusicStore operation
		url = CentralRepositoryStringValueL(KCRUidMusicShopSettings,KMusicShopURI);
		CleanupStack::PushL(url);   // the pointer will be poped
		}

	RProperty::Set( KUidMusicStore,
	          KMShopCategoryId,
	          KFindInMShopKeyValid );  // Set Key to Valid

	RProperty::Set( KUidMusicStore,
	          KMShopCategoryName,
	          *url );
	LaunchMusicShopL();
	if(url)
		{
		CleanupStack::PopAndDestroy(url);
		}

	REComSession::FinalClose();
#else
    if(aSongName.Length() > 0 || aArtistName.Length() > 0 || aAlbumName.Length() > 0
        || aMusicFlag)
    {
        // do nothing, just remove build warnings.
    }
#endif
	IRLOG_DEBUG( "Leaving CIRNowPlayingView::DoFindInMusicShopL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingView::CentralRepositoryStringValueL
// Retrievs the string from Central Repository for the specified key and cen rep uid
// Transfers the ownership of the returned HBufC object
// The caller must takecare of deleting the returned object*
// ---------------------------------------------------------------------------
//
HBufC* CIRNowPlayingView::CentralRepositoryStringValueL(
	const TUid& aRepositoryUid, TUint32 aKey ) const
	{
	IRLOG_DEBUG( "CIRNetworkController::CentralRepositoryStringValueL- Entering" );
	CRepository* pRepository = CRepository::NewL( aRepositoryUid );
	CleanupStack::PushL( pRepository );

	TInt length = KMusicStoreURLSize;
	HBufC* valueString = HBufC::NewLC( length );
	TInt error( KErrTooBig );
	do
		{
		TPtr ptr = valueString->Des();
		error = pRepository->Get( aKey, ptr );
		if ( error == KErrTooBig )
			{
			CleanupStack::PopAndDestroy( valueString );
			length = KTwo * length;
			valueString = HBufC::NewLC( length );
			}
		} while ( error == KErrTooBig );

	if ( error )
		{
		TPtr ptr = valueString->Des();
		ptr.Copy( KNullDesC() );
		}

	CleanupStack::Pop( valueString );
	CleanupStack::PopAndDestroy( pRepository );
	IRLOG_DEBUG( "CIRNetworkController::CentralRepositoryStringValueL - Exiting." );
	return valueString;
	}



// ---------------------------------------------------------------------------
// void CIRNowPlayingView::ListenFromPls()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::ListenFromPlsL()
	{
	IRLOG_DEBUG( " CIRNowPlayingView::ListenFromPlsL - Entering" );
	TInt ListBoxIndex =iRockerIndex;
	TInt UrlIndex = (ListBoxIndex * KTwo);

	CIRIsdsPreset* preset = CIRIsdsPreset::NewL();
	CleanupStack::PushL( preset );

	//copy channel name
	TBuf<KChannelNameSize> channelName;
	if(UrlIndex>=0 && UrlIndex <iUi->iURLs.Count())
		{
		channelName.Copy(*( iUi->iURLs[UrlIndex+1]));
		}
	preset->SetName(channelName);
	preset->SetChannelType(0);
	channelName.Zero();
	//copy channel url
	channelName.Copy(*( iUi->iURLs[UrlIndex] ) );
	_LIT(KServerName,"NA");

	//set station information to NA
	preset->SetUrlL(KServerName,channelName,0);
	preset->SetUrlCount();


		// Set the channel as last played and currently playing
*iNowPlayingWrapper->iNowPlayingPreset = *preset;
	CleanupStack::PopAndDestroy( preset );

	//connecting to server
	iNowPlayingWrapper->SetCurrentConnectionSource(EIRAdhocExternal);
	iNowPlayingWrapper->ConnectToChannelL(*iNowPlayingWrapper->iNowPlayingPreset);


	IRLOG_DEBUG( "Leaving CIRNowPlayingView::ListenFromPlsL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::HandleControlEventL()
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView ::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
	{
	IRLOG_DEBUG( " CIRNowPlayingView::HandleControlEventL - Entering" );
	if( AknLayoutUtils::PenEnabled() )
		{
		if(aEventType==EEventStateChanged)
			{
			if(aControl==iVolPopup)
				{
				iSettingsValue=iIRSettings->GetVolumeSetting();
				iVolumeBarValue=iVolPopup->Value();
				if(iVolumeBarValue<iSettingsValue)
					{
						TInt diff=iSettingsValue-iVolumeBarValue;
						if(diff>1 && (iVolumeBarValue==0 ))
							{
							TInt index = iNowPlayingWrapper->GetMediaClientInstance()->MaxVolume()/KMaxVolume;
							TInt volume = index*iVolumeBarValue;
							iVolumeMute=ETrue;
							iNowPlayingWrapper->GetMediaClientInstance()->SetVolume(volume);
							}
						else
							{
							if(iVolumeMute)
								{
								iVolPopup->SetValue(2);
								iVolumeBarValue=iVolPopup->Value();
								}
							iVolumeMute=EFalse;
							iNowPlayingWrapper->SetVolumeL(iVolumeBarValue);
							}
					}
				else
					{
					if(iVolumeMute)
						{
						iVolPopup->SetValue(2);
						iVolumeBarValue=iVolPopup->Value();
						}
					iNowPlayingWrapper->SetVolumeL(iVolumeBarValue);
					iVolumeMute=EFalse;
					}
				}
			}
		}
	IRLOG_DEBUG( "Leaving CIRNowPlayingView::HandleControlEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetLastPlayedValue()
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::SetLastPlayedValue(TInt aLastPlayed)
	{
	IRLOG_DEBUG( " CIRNowPlayingView::SetLastPlayedValue - Entering" );
	iUpdateLastplayed=aLastPlayed;
	IRLOG_DEBUG( " CIRNowPlayingView::SetLastPlayedValue - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetStationNameFlag()
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::SetStationNameFlag(TInt aUpdateStationName)
	{
	IRLOG_DEBUG( " CIRNowPlayingView::SetStationNameFlag - Entering" );
	iUpdateStationName=aUpdateStationName;
	IRLOG_DEBUG( "Leaving CIRNowPlayingView::SetStationNameFlag - Exiting" );
	}


// ---------------------------------------------------------
// CIRNowPlayingView::HandleSystemEventL
// ---------------------------------------------------------
//
void CIRNowPlayingView::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG(" CIRNowPlayingView::::HandleSystemEventL - Entering");

	CIRUi* appUi = static_cast<CIRUi*>(AppUi());
	switch (aEventType)
		{
		case EIRNetworkCoverageUp:
			{
			break;
			}
		case EIRNetworkCoverageDown:
			{
			break;
			}
		case EIRCallActivated:
			{
			iCallActive=ETrue;
			if( appUi->ActiveView() == KIRNowPlayingViewID )
				{
				ConstructToolbarL();
				}
				iUi->HandleCallActiveL(aEventType);

			break;
			}
		case EIRCallDeactivated:
			{
				iCallActive=EFalse;
				iUi->HandleCallActiveL(aEventType);
			break;
			}
		default:
			{
			break;
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::HandleSystemEventL - Exiting." );
	return;
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetPlayingValue()
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::SetPlayingValue(TInt aPlaying)
	{
	IRLOG_DEBUG( " CIRNowPlayingView::SetLastPlayedValue - Entering" );
	iPlaying=aPlaying;
	IRLOG_DEBUG( " CIRNowPlayingView::SetLastPlayedValue - Exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::HandleMediaKeys()
// function handles the hardware media keys
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::HandleMediaKeysL(TRemConCoreApiOperationId aOperationId,
				TRemConCoreApiButtonAction aButtonAct)
	{
	switch(aOperationId)
		{
		case ERemConCoreApiVolumeUp:
			{
			TInt volume=iIRSettings->GetVolumeSetting();
			if(volume==KMaxVolume)
				{
				if(!iVolPopup->IsFocused())
				iVolPopup->ShowVolumePopupL();
				}
			else if(volume==KMinVolume && !iVolumeMute)
				{
				iVolPopup->SetValue(KMinVolume+(KMaxVolume/KMaxVolumeRange));
				if(!iVolPopup->IsFocused())
				iVolPopup->ShowVolumePopupL();
				iNowPlayingWrapper->SetVolumeL(iVolPopup->Value());
				}
			else
				{
				switch(aButtonAct)
					{
					case ERemConCoreApiButtonPress:
					case ERemConCoreApiButtonClick:
						{
						iVolumeMute=EFalse;
						iVolPopup->SetValue(2);
						if(!iVolPopup->IsFocused())
						iVolPopup->ShowVolumePopupL();
						iNowPlayingWrapper->SetVolumeL(iVolPopup->Value());
						}
					break;
					case ERemConCoreApiButtonRelease:
						{

						}
					break;
					}
				}


			}
		break;
		case ERemConCoreApiVolumeDown:
			{
			TInt volume=iIRSettings->GetVolumeSetting();

			if(volume==KMinVolume && !iVolumeMute)
				{
				if(!iVolPopup->IsFocused())
				iVolPopup->ShowVolumePopupL();
				}
			else if(volume==KMaxVolume)
				{
				iVolPopup->SetValue(KMaxVolume-(KMaxVolume/KMaxVolumeRange));
				if(!iVolPopup->IsFocused())
				iVolPopup->ShowVolumePopupL();
				iNowPlayingWrapper->SetVolumeL(iVolPopup->Value());
				}
			else
				{
				switch(aButtonAct)
					{
					case ERemConCoreApiButtonPress:
					case ERemConCoreApiButtonClick:
						{
						iVolumeMute=EFalse;
						TInt volume=iIRSettings->GetVolumeSetting();
                        TInt value =volume - (KMaxVolume/KMaxVolumeRange);
							if(value==0)
								{
								iVolumeMute=EFalse;
								}
							iVolPopup->SetValue(2);
							if(!iVolPopup->IsFocused())
							iVolPopup->ShowVolumePopupL();
							iNowPlayingWrapper->SetVolumeL(iVolPopup->Value());
						}
					break;
					case ERemConCoreApiButtonRelease:
						{

						}
					break;
					}
				}


			}
			break;
		}
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetVolumePopUpValue()
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::SetVolumePopUpValue(TInt aValue)
	{
	IRLOG_DEBUG( " CIRNowPlayingView::SetVolumePopUpValue - Entering" );

	  iVolPopup->SetValue(aValue);
	IRLOG_DEBUG( " CIRNowPlayingView::SetVolumePopUpValue - Exiting" );
	}

	// ---------------------------------------------------------------------------
// void CIRNowPlayingView::GetVolumePopUpValue()
// ---------------------------------------------------------------------------
//

TInt CIRNowPlayingView::GetVolumePopUpValue()
	{
	IRLOG_DEBUG( " CIRNowPlayingView::GetVolumePopUpValue - Entering" );

	IRLOG_DEBUG( " CIRNowPlayingView::GetVolumePopUpValue - Exiting" );
     return iVolPopup->Value();
	}

// ---------------------------------------------------------------------------
// From class CBaseView.
// Returns help context for this view.

// ---------------------------------------------------------------------------
//
TCoeHelpContext CIRNowPlayingView::HelpContext() const
    {
    return TCoeHelpContext( KIRMCVUid, KIR_HELP_NOW );
    }


// ---------------------------------------------------------------------------
// void CIRNowPlayingView::LaunchFMTransmitterL()
// Launch FM Transmitter application
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::LaunchFMTransmitterL()
    {
    IRLOG_DEBUG( "CIRNowPlayingView::LaunchFMTransmitterL - Entering" );

    if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
        {
        TApaTaskList tasList( iCoeEnv->WsSession() );
        TApaTask task = tasList.FindApp( KFmTxAppUid );
        if ( task.Exists() )
            {
            task.BringToForeground();
            }
        else
            {
            RProcess process;
            TApaAppInfo appInfo;
            RApaLsSession session;
            if ( KErrNone == session.Connect() )
                {
                CleanupClosePushL( session );
                TThreadId threadId;
                session.CreateDocument( KNullDesC, KFmTxAppUid, threadId );
                CleanupStack::PopAndDestroy(&session);
                }
            }
        }
	IRLOG_DEBUG( "CIRNowPlayingView::LaunchFMTransmitterL - Exiting" );
    }
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::DeleteSteroEffect()
// Destroys the Stereo Effect Corresponing the Player
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DeleteSteroEffect(TBool /*aStereo*/)
{
IRLOG_DEBUG( "CIRNowPlayingView::DeleteSteroEffect - Entering" );

	if(iStereoEffect)
	{
		delete iStereoEffect;
		iStereoEffect=NULL;

	}
IRLOG_DEBUG( "CIRNowPlayingView::DeleteSteroEffect - Exiting" );

}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::DisableVolumePopup()
// Disbale the volumepopup when it is Active
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::DisablePlayer()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::DisablePlayer - Entering" );
	if(iUi->IsCallActive() )
		{
		if(iUi->GetCurrentNetworkType().Compare(KWiFi)==0)
			{
			iNowPlayingWrapper->StopPlayer();
			iNowPlayingWrapper->DoPlayingStateChanged(EFalse);
			iUi->SetPlayStateWhileCall(ETrue);
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingView::DisablePlayer - Exiting" );
	}

// ---------------------------------------------------------
// From class MAlfActionObserver.
// CIRNowPlayingView::HandleActionL
// ---------------------------------------------------------
//
void CIRNowPlayingView::HandleActionL( const TAlfActionCommand& aActionCommand )
    {
	IRLOG_DEBUG( "CIRNowPlayingView::HandleActionL - Entering" );

    if ( aActionCommand.Id() == KAlfActionIdDeviceLayoutChanged )
        {
        if ( iContainer )
            {
            iAlfEnv.PrimaryDisplay().SetVisibleArea( ClientRect() );
            iContainer->LayoutControl();
           	iFadeStyle=EIRFadeNoFade;
          /*delete iConnectionCleanupTimer;
		iConnectionCleanupTimer = NULL;
        TRAPD( err, iConnectionCleanupTimer = CPeriodic::NewL( CActive::EPriorityStandard ) )
        if ( err == KErrNone )
            {
            iConnectionCleanupTimer->Start( KWaitTimeInMicroSeconds, 0,
            			 TCallBack( StaticConnectionCleanupCallback, this ) );
            }*/
           	if(iStationHasLogo)
	           	{
	           	DisplaylogoL();
	           	}
			DrawViewL(iStationHasLogo);
            }
        }
    else if ( aActionCommand.Id() == KAlfActionIdSkinChanged )
    	{
    	 if ( iContainer )
    		{
    		// Update radio idle skin color
    	    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    	    if ( skin )
    	    	{
    	    	TRgb textColor; // text color when not highlighted
    	    	AknsUtils::GetCachedColor( skin, textColor, KAknsIIDQsnTextColors,
    	    								 EAknsCIQsnTextColorsCG6 );
    	    	TRgb highlightColor; // text color when highlighted
    	    	AknsUtils::GetCachedColor( skin, highlightColor, KAknsIIDQsnTextColors,
    	    				 EAknsCIQsnTextColorsCG10 );
    	    	iContainer->RadioIdle().SetPrimaryColor( textColor );
    	    	iContainer->RadioIdle().SetSecondaryColor( highlightColor );
	           	iFadeStyle=EIRFadeNoFade;
	           	ConstructToolbarL();
	           	if(iStationHasLogo)
	           		{
	           		DisplaylogoL();
	           		}
				DrawViewL(iStationHasLogo);
    	    	}
    		}

    	}
    else
    	{
    	}
    }
 // ---------------------------------------------------------
// void CIRNowPlayingView::SetBitrate()
// Sets The Bitrates Value
// ---------------------------------------------------------
//
void CIRNowPlayingView::SetBitrate(TInt aError)
{
IRLOG_DEBUG( "CIRNowPlayingView::SetBitrate - Entering" );

	if(iContainer)
	{
   	TInt bitRate(0);
	if(!aError)
		{
		iShowBitrate=ETrue;
		RProperty::Get ( KUidActiveInternetRadioApp, KIRPSBitrate, bitRate );
		}
	else
		{
		iShowBitrate=EFalse;
		}
	RBuf bitrate;
	HBufC* BitRate=NULL;
	TRAPD(err,BitRate= StringLoader::LoadL(R_IRAPP_NOWPLAYING_BITRATE, bitRate);)
	if(err==KErrNone)
		{
		bitrate.Create(BitRate->Length());
		bitrate.Copy(*BitRate);
		DisplayBitrate(bitrate);
		bitrate.Close();
		}
	delete BitRate;
	}

IRLOG_DEBUG( "CIRNowPlayingView::SetBitrate - Exiting " );
}

// ---------------------------------------------------------
// CIRNowPlayingView::IsMetaData()
// returns true if there is meta data
// ---------------------------------------------------------
TBool CIRNowPlayingView::IsMetaData()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::IsMetaData - Entered " );
	TBool retVal = ETrue;

	if( iNowPlayingWrapper->MetaData().Song().Length() <= KOne &&
	    iNowPlayingWrapper->MetaData().Artist().Length() <= KOne )
		{
		retVal = EFalse;
		}
	IRLOG_DEBUG( "CIRNowPlayingView::IsMetaData - Leaving " );
	return retVal;
	}

// ---------------------------------------------------------
// CIRNowPlayingView::MusciStoreEnabledStatus()
// returns true if music store enabled status is "yes"
// ---------------------------------------------------------
TBool CIRNowPlayingView::MusciStoreEnabledStatus()
    {
	IRLOG_DEBUG( "CIRNowPlayingView::MusciStoreEnabledStatus - Entered " );
	_LIT(KMusicStoreEnabled, "yes");
	TBool retVal = EFalse;

	if (iNowPlayingWrapper->iNowPlayingPreset->
		      GetMusicStoreStatus().CompareF(KMusicStoreEnabled)==0 )
		{
		retVal = ETrue;
		}
	IRLOG_DEBUG( "CIRNowPlayingView::MusciStoreEnabledStatus - Leaving " );
	return retVal;
    }

// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetLogoStatus()
// Sets the Value of Request Pendind While Checking For Network
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::SetRequestPendingWhenNoNetWork(TBool aRequestPending)
{
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Entering" );
iRequestPending=aRequestPending;
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Exiting" );
}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetLogoStatus()
// Return the Request Pending Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingView::GetRequestPending()
{
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Entering" );
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Exiting" );
return iRequestPending;
}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetLogoStatus()
// Sets the Value of Request Pendind While Checking For Network
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::SetRequestPendingWhenNoNetWorkForNextOrPrev(TBool aRequestPending)
{
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Entering" );
iNextOrPrevRequest=aRequestPending;
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Exiting" );
}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::SetLogoStatus()
// Return the Request Pending Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingView::GetRequestPendingForNextOrPreV()
{
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Entering" );
IRLOG_DEBUG( "CIRLogoTimer::SetLogoStatus - Exiting" );
return iNextOrPrevRequest;
}
// ---------------------------------------------------------------------------
// SetRockerIndex()
// Sets the Current Playing Index
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::SetRockerIndex(TInt aRockerIndex  )
	{

	IRLOG_DEBUG( "CIRNowPlayingView::SetRockerIndex - Entering" );
	iRockerIndex=aRockerIndex;
	IRLOG_DEBUG( "CIRNowPlayingView::SetRockerIndex - exiting" );
	}
// ---------------------------------------------------------------------------
// GetRockerIndex()
// Returns The current Playing Index
// ---------------------------------------------------------------------------
//

TInt CIRNowPlayingView::GetRockerIndex( )
	{

	IRLOG_DEBUG( "CIRNowPlayingView::GetRockerIndex - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingView::GetRockerIndex - exiting" );
	return iRockerIndex;
	}

// ---------------------------------------------------------------------------
// SavePreviousIndex()
// Sets the Current Playing Index
// ---------------------------------------------------------------------------
//

void CIRNowPlayingView::SavePreviousIndex(TInt aPresetCount  )
	{

	IRLOG_DEBUG( "CIRNowPlayingView::SavePreviousIndex - Entering" );
	iPresetCount=aPresetCount;
	IRLOG_DEBUG( "CIRNowPlayingView::SavePreviousIndex - exiting" );
	}
// ---------------------------------------------------------------------------
// GetPreviousIndex()
// Returns The Previous  Index
// ---------------------------------------------------------------------------
//

TInt CIRNowPlayingView::GetPreviousIndex( )
	{

	IRLOG_DEBUG( "CIRNowPlayingView::GetPreviousIndexex - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingView::GetPreviousIndex - exiting" );
	return iPresetCount;
	}
// ---------------------------------------------------------------------------
// IsVolumeMuted()
// Returns whether volume is Muted
// ---------------------------------------------------------------------------
//

TBool CIRNowPlayingView::IsVolumeMuted( )
	{
	IRLOG_DEBUG( "CIRNowPlayingView::IsVolumeMuted - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingView::IsVolumeMuted - exiting" );
	return iVolumeMute;
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingView::MrccatoCommand()
// This is call back function which observes hardware key events
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
    TRemConCoreApiButtonAction aButtonAct)
    {
    IRLOG_INFO3( "CIRNowPlayingView::MrccatoCommand(aOperationId=%d, aButtonAct=%d)", aOperationId, aButtonAct );


    switch( aOperationId )
        {
        case ERemConCoreApiPausePlayFunction:
            if( iNowPlayingWrapper->iPlaying )
                {
                MrccatoCommand( ERemConCoreApiPause, aButtonAct );
                }
            else
                {
                // Note that ERemConCoreApiPlaybackSpeedForward is just any value,
                // it's not used by MrccatoPlay implementation in this class.
                MrccatoPlay( ERemConCoreApiPlaybackSpeedForward, aButtonAct );
                }
            break;
        case ERemConCoreApiPause:               // -fall through
        case ERemConCoreApiStop:
            if( aButtonAct == ERemConCoreApiButtonClick )
                {
                if( iNowPlayingWrapper->iPlaying)
                    {
                        TRAP_IGNORE( PlayAndStopControlL() )
                        }
                }

            break;
        case ERemConCoreApiForward:
            if( aButtonAct == ERemConCoreApiButtonClick )
                {
                if(iContainer)
	                {
	                TRAP_IGNORE(HandleCommandL(EIRCmdStepNextPreset);)
	                }
                }
            break;
        case ERemConCoreApiBackward:
            if( aButtonAct == ERemConCoreApiButtonClick )
                {
                if(iContainer)
	                {
	               TRAP_IGNORE(HandleCommandL(EIRCmdStepPrevPreset);)
	                }
                }

            break;
        default:
            break;
        }

    IRLOG_DEBUG( "CIRNowPlayingView::MrccatoCommand - Exiting." );
    }

// ---------------------------------------------------------
// CIRNowPlayingView::MrccatoPlay()
// ---------------------------------------------------------
//
void CIRNowPlayingView::MrccatoPlay(TRemConCoreApiPlaybackSpeed IRDEBUGVAR(aSpeed),
				 TRemConCoreApiButtonAction aButtonAct)
    {
    IRLOG_INFO3( "CIRNowPlayingView::MrccatoPlay(aSpeed=%d, aButtonAct=%d)", aSpeed, aButtonAct );


    if( aButtonAct == ERemConCoreApiButtonClick )
        {
        if( !iNowPlayingWrapper->iPlaying )
            {
             if(iContainer)
	             {
				 TRAP_IGNORE(PlayAndStopControlL();)
	             }
            }
        }

    IRLOG_DEBUG( "CIRNowPlayingView::MrccatoPlay - Exiting." );
    }

// -----------------------------------------------------------------------------
// CIRNowPlayingView::HasBeenActivated()
// ------------------------------------------------------------------------------
//
TBool CIRNowPlayingView::HasBeenActivated() const
	{
	return iHasBeenActivated;
	}

// CIRNowPlayingView::GetCommandId()
// ------------------------------------------------------------------------------
//
TInt CIRNowPlayingView::GetCommandId()
{

return iCommandId;
}
// ---------------------------------------------------------------------------
// void CIRNowPlayingView::UpdateLastplayed()
// To Update LastPlayed
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::UpdateLastplayedL()
{
	if(iNowPlayingWrapper->iNowPlayingPreset)
		{
		// store the last played station
		*iUi->iLastPlayed->iLastPlayedStation = *iNowPlayingWrapper->iNowPlayingPreset;
		TRAPD(err,iUi->iLastPlayed->CommitLastPlayedChannelL());
		TInt error=KErrNone;
		if(err != KErrNone)
			{
			error = iUi->iLastPlayed->RemoveLastPlayedFile();
			User::Leave(error);
			}
		}
}
// ---------------------------------------------------------------------------
// CIRNowPlayingView::SyncPresetL()
// ---------------------------------------------------------------------------
//
void CIRNowPlayingView::SyncPresetL(TInt aChid,const TDesC &aModifiedTime)
    {
    //sync state set to pending
	iUi->iNowPlayingWrapper->CreateWaitDialogL();
	iUi->GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);

	iUi->iNowPlayingWrapper->iSyncPreset = ETrue;
	iIsdsWrapper->SetSyncPresetObserver( this,this );
    iUi->iIsdsWrapper->GetISDSInstance()->SyncPresetL(aChid,aModifiedTime);
    }

// ------------------------------------------------------------------------
// from MSyncPresetObserver
// SyncPresetRemovedL()
// called back when the preset that is synced was removed from isds
// @param TInt,the preset id
// -----------------------------------------------------------------------
void CIRNowPlayingView::SyncPresetRemovedL(TInt /*aId*/)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::SyncPresetRemovedL - Entering." );
	// TO Do: need to check
	iNowPlayingWrapper->SetFavFocus(iRockerIndex+1) ;
	iUi->iNowPlayingWrapper->iSyncPreset = EFalse;

	// Connect to Favourite User-defined preset
	if(iUi->iFavPresets->iFavPresetList.Count() >= 1)
		{
		iUi->iFavPresets->iFavPresetList[iRockerIndex]->
			CopyPresetData(*iNowPlayingWrapper->iNowPlayingPreset);
		iNowPlayingWrapper->ConnectToChannelL(*iNowPlayingWrapper->iNowPlayingPreset);
		StartTimerL();
		}
	IRLOG_DEBUG( "CIRNowPlayingView::SyncPresetRemovedL - Entering." );
	}

// -----------------------------------------------------------------------
// from MSyncPresetObserver
// SyncPresetChangedL()
// called back when the preset that is synced has been changed in the isds
// @param CIRIsdsPreset&,the new preset data
// ----------------------------------------------------------------------
void CIRNowPlayingView::SyncPresetChangedL(CIRIsdsPreset& /*aPreset*/)
	{
	IRLOG_DEBUG( "CIRNowPlayingView::SyncPresetChangedL - Entering." );
	iUi->iNowPlayingWrapper->iSyncPreset = EFalse;
	// TO Do: need to check
	iNowPlayingWrapper->SetFavFocus(iRockerIndex+1);
	// Connect to Favourite User-defined preset
	if(iUi->iFavPresets->iFavPresetList.Count() >= 1)
		{
		iUi->iFavPresets->iFavPresetList[iRockerIndex]->
			CopyPresetData(*iNowPlayingWrapper->iNowPlayingPreset);
		iNowPlayingWrapper->ConnectToChannelL(*iNowPlayingWrapper->iNowPlayingPreset);
		StartTimerL();
		}

	IRLOG_DEBUG( "CIRNowPlayingView::SyncPresetChangedL - Entering." );
	}

// -----------------------------------------------------------------------
// from MSyncPresetObserver
// SyncPresetNoChangeL()
// called back when the preset that is synced has not been changed
// -----------------------------------------------------------------------
void CIRNowPlayingView::SyncPresetNoChangeL()
	{
	IRLOG_DEBUG( "CIRNowPlayingView::SyncPresetNoChangeL - Entering." );
	iUi->iNowPlayingWrapper->iSyncPreset = EFalse;
	// TO Do: need to check
	iNowPlayingWrapper->SetFavFocus(iRockerIndex+1);

	if(iUi->iFavPresets->iFavPresetList.Count() >= 1)
		{
		iUi->iFavPresets->iFavPresetList[iRockerIndex]->
			CopyPresetData(*iNowPlayingWrapper->iNowPlayingPreset);
		iNowPlayingWrapper->ConnectToChannelL(*iNowPlayingWrapper->iNowPlayingPreset);
		StartTimerL();
		}

	IRLOG_DEBUG( "CIRNowPlayingView::SyncPresetNoChangeL - Entering." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingView::StaticConnectionCleanupCallback
// ---------------------------------------------------------------------------
//
TInt CIRNowPlayingView::StaticConnectionCleanupCallback( TAny* aSelfPtr )
	{
	CIRNowPlayingView* self = reinterpret_cast<CIRNowPlayingView*>( aSelfPtr );

	 if(self)
		 {
		delete self->iConnectionCleanupTimer;
		self->iConnectionCleanupTimer = NULL;
		TRAP_IGNORE(self->DrawViewL(self->iStationHasLogo);)
		 }
		return KErrNone;
	}
