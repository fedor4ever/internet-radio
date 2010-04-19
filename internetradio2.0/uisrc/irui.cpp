/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*  Version: 2, Tue Feb 28 18:00:00 2008 by Rohit/Kranthi
*  Ref:
*  Added Workaround in CIRUi::ConstructL() to force display of IAP list.
*  MIRCtrlCmdObserver::MCtrlCommand() function signature changed (Merged IRv1.0 Engine code changes)
*
*  </ccm_history>
* ============================================================================
*/

/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Tue Mar 19 18:00:00 2008 by Rohit
*  Ref:
*  Implemented change request to display Byte Counter in KB till 0.1 MB, and in MB threrafter
*
*  </ccm_history>
* ============================================================================
*/
#include "internetradioconfigurationflags.hrh"
#include <akniconarray.h>
#include <aknkeylock.h>
#include <aknlayout2scalabledef.h>
#include <aknlayoutfont.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <aknvolumecontrol.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <apgcli.h>
#include <apgtask.h>
#include <bautils.h>
#include <cmapplicationsettingsui.h>
#include <featmgr.h>
#include <features.hrh>
#include <hlplch.h>
#include <pstransaction.h>
#include <stringloader.h>
#include <uriutils.h>
#include <aknutils.h>
#include <internetradio.rsg>
#include <commdb.h>
#include <commsdattypesv1_1.h>
#include <data_caging_path_literals.hrh>
#include <hwrmpowerstatesdkpskeys.h>
#include <aknwaitdialog.h>
#include <ctsydomainpskeys.h>
#include <utf.h>
#include <proengfactory.h>
#include <mproengnotifyhandler.h>
/* no need to change to lower case  */
#include <MProEngNotifyHandler.h>
#include <profileenginesdkcrkeys.h>

//For IAD
#include <publicruntimeids.hrh>
#ifdef FF_IAUPDATE
#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#endif



#include "ircommon.h"
#include "irui.h"
#include "irnowplayingview.h"
#include "ir.hrh"
#include "irmainview.h"
#include "irmaincontainer.h"

#include "irautomatedbitrateselection.h"
#include "iralarmobserver.h"
//#include "irmessageobserver.h"
#include "ircommon.h"
#include "irctrlcmdobserver.h"
#include "irdataproviderconstants.h"
#include "irdebug.h"
#include "irdocument.h"
#include "irfavoritesdb.h"
#include "irhistory.h"
#include "iriconloader.h"
#include "irisdspreset.h"
#include "irmediaclient.h"
#include "irmetadata.h"
#include "irnetworkcontroller.h"
#include "irnwinfoobserver.h"
#include "irpubsub.h"
#include "irsessionlogger.h"
#include "irsettings.h"
#include "irstreamsource.h"
#include "irstreamsourceerrors.h"
#include "irui.h"
#include "isdsclientdll.h"
#include "irbrowsechannelitems.h"
#include "irbrowsecatagoryitems.h"
#include <internetradio.mbg>
#include "irhistorylistview.h"
#include "irsearchresultsview.h"
#include "irIsdsWrapper.h"
#include "irviewstack.h"
#include "irstationdirectoryview.h"
#include "irstationinformationdata.h"
#include "ircategoryview.h"
#include "irstationsview.h"
#include "irtopstationsview.h"
#include "irplsplaylist.h"
#include "irplsplaylistitem.h"
#include  "irplsview.h"
#include "irfirsttimeview.h"
#include "iraddmanuallystationview.h"
#include "irsystemeventcollector.h" //For system events
#include "irnowplayingwrapper.h"
#include "ircostwarningview.h"
#include "irstationinfoview.h"
#include "irdatatransferobserver.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irlogodownloadengine.h"
#include "iriconloader.h"
#include "irapplication.h"
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>


#ifdef FF_IAUPDATE
_LIT( KIRParamExec, "InternetRadio.exe" );
#endif

#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
#include "irbacksteppingservicewrapper.h"
#endif

const TInt KFour = 4;

//static variable declaration
TInt CIRUi::iFindInMusicStoreCount = 0;


//Added by Snigdha for HomeScreen
_LIT8( KIRCommandLineActionHandler, "-a" );
_LIT8( KIRCommandLineActionHandlerCommandStartNowPlayingView, "startnowplaying" );
const TChar KIRCommandLineOptionPrefix = '-';

// ---------------------------------------------------------------------------
// CIRUi::ConstructL(const TRect& aRect)
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CIRUi::ConstructL()
	{
	IRLOG_INFO( "CIRUi::ConstructL - Entering" );
	CIRApplication *app = static_cast<CIRApplication*>(Document()->Application());
	if(app->GetInstanceFlag())
		{
    	BaseConstructL(ENoAppResourceFile);
    	return;
		}
	BaseConstructL(EAknEnableSkin);
	UpdateLandscapeInformation();
	iProfileId=DetermineCurrentProfile();
	iSystemEventCollector = CIRSystemEventCollector::NewL();
	iSystemEventCollector->AddObserverL( this );

	iIRSettings =CIRSettings::OpenL();
    iSecondInstanceFlag = EFalse;
	UpdateSubscriberIdL();

	User::LeaveIfError(iFsSession.Connect());
	//Instantiate NowPlaying View
	iAlfEnv = CAlfEnv::NewL();
	// Switch the layout to flat so that Alf will use right layout on Now playing view
    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
	TRect layoutRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, layoutRect );
	CAlfDisplay& disp = iAlfEnv->NewDisplayL( layoutRect, CAlfEnv::ENewDisplayAsCoeControl );
	disp.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

	//Whether The Station is Editiable or not
	iIsEditStation = EFalse;
	//Index of the Preset to Edit
	iEditStationIndex = 0;

	TParse parse;
	parse.Set( KMIFFILE, &KDC_APP_RESOURCE_DIR, NULL );
	iMifFilePath = parse.FullName();
	CompleteWithAppPath( iMifFilePath );

	//Create the instance of iconprovider
	iIconProvider = CIconFileProvider::NewL(iMifFilePath);


	// Network Controller Component
	iNetworkController = CIRNetworkController::OpenL(this);
	iNetworkController->DataTransferTracker().SetObserver( this );
    iIsOffline=iNetworkController->IsOfflineMode();
	// Timer for Network indicator animation
	iNetworkIndicatorTimer = CPeriodic::NewL(CPeriodic::EPriorityStandard);

	iDialogLauncher = CIRDialogLauncher::NewL();

	iFavPresets=CIRFavoritesDb::NewL();

	iIsdsWrapper = CIRIsdsWrapper::NewL(*iIRSettings,*iFavPresets);
	iNowPlayingWrapper = CIRNowPlayingWrapper::NewL(*this,*iIRSettings,*iIsdsWrapper);
 	RAknKeyLock keylock;
    User::LeaveIfError( keylock.Connect() );
    if ( keylock.IsKeyLockEnabled() )
        {
        BringToForeground();
        }
    keylock.Close();
	// Display access points only if user has accepted Terms & Conditions
/*	if( iIRSettings->IsFlagTermsAndConditionsL() )
		{
		iNetworkController->ChooseAccessPointL();
		}
*/
	//playing is terminated by call
	iPlayTerminated = EFalse;
	//checks whether call is active or not
	iCallActive = EFalse;

	//Call observer

	iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
	GetLogoDownloadEngine()->SetFavDbInstance(iFavPresets);
	// Create Channel list view
	iLastPlayed = CIRLastPlayedStationInfo::NewL();
	iMainView = CIRMainView::NewL();
	AddViewL( iMainView );

#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
    // Create Back Stepping Service wrapper.
    iBsWrapper = CIRBackSteppingServiceWrapper::NewL( TUid::Uid( KUIDIRAPP ) );
#endif

	// Check whether phone is in low memory
	TBool bIgnore = CheckAndNotifyLowDiskSpaceL();

	iNowPlayingView = CIRNowPlayingView::NewL( *iAlfEnv );
	AddViewL( iNowPlayingView );
	//Instantiate AlarmObserver
	iAlarmObserver = CIRAlarmObserver::NewL(this);
	iAlarmObserver->Start();
//	iMessageObserver = CIRMessageObserver::NewL(this);
	if( iCallActive )
		{
		IRLOG_WARNING( "CIRUi::ConstructL - Call active during start up " );
		}

	//Create SearchResults View
	iSearchResultsView = CIRSearchResultsView::NewL();
	AddViewL( iSearchResultsView );

	//Instantiate PublishSubscribe class
	iPubSubManager = CIRPubSub::NewL();
	iPubSubManager->PublishApplicationRunningStateL(EIRPSApplicationRunning);

	// Create history list view
	iHistoryListView = CIRHistoryListView::NewL();
	AddViewL( iHistoryListView );


	// Create Station Directory View
	iStationdirView = CIRStationDirectoryView::NewL();
	AddViewL( iStationdirView );

	// Create Category View
	iCategoryView = CIRCategoryView::NewL();
	AddViewL( iCategoryView );

	// Create Stations View
	iStationsView = CIRStationsView::NewL();
	AddViewL( iStationsView );

	// Create TopStations View
	iTopStationsView = CIRTopStationsView::NewL();
	AddViewL( iTopStationsView );

	iViewStack = CIRViewStack::NewL();

	//Create First Time View
	iFirstTimeView = CIRFirstTimeView::NewL();
	AddViewL(iFirstTimeView);

	//Create Add Manually View
	iIRAddManually = CIRAddManuallyStationView::NewL( ApplicationRect() );
	AddViewL(iIRAddManually);

	// Create Settings view
	CIRSettingsView* iSettingsView = CIRSettingsView::NewL();
	AddViewL( iSettingsView );

	//Plsveiw enabled flag set to False by default.
	iPlsactivated = EFalse;

	if (!iIRSettings->IsFlagTermsAndConditionsL() ||!iIRSettings->IsFlagCostWarningL())
		{
		iCostWarningView = CIRCostWarningView::NewL(ApplicationRect());
		AddViewL( iCostWarningView );
		SetDefaultViewL(*iCostWarningView);
		}
	else
		{
		if(iStartUpInNowPlayingView)
			{
		    //iViewStack->PushL(KIRMainChoiceViewID)
			SetDefaultViewL(*iNowPlayingView);
			iStartUpInNowPlayingView = EFalse;
			}
		else
			{
			if (IsFirstTime() && (!IsFlagIfAnyFavL()))
				{
				SetDefaultViewL(*iFirstTimeView);
				}
			else
				{
				SetDefaultViewL( *iMainView );
				}
			}
		}

	// Create Station Info view
	iStationInfoView = CIRStationInfoView::NewL( ApplicationRect() );
	AddViewL( iStationInfoView );

	//call is answered
	iCallConnected = EFalse;

	iNwObserver = CIRNwInfoObserver::NewL(this);

	//getting current thread's id
	RThread thread;
	iCurrentThreadID = thread.Id();
	//Handle Background and foreground of the views.
	iCoeEnv->AddForegroundObserverL(*this);
	// Subscriber for bitrate value
	iBitrateSubscriber = CIRPropertyObserver::NewL(*this,
	KUidActiveInternetRadioApp,KIRPSBitrate,RProperty::EInt);

	// Subscriber for Simultaneous play handling
	iMultiplePlayHandler = CIRPropertyObserver::NewL(*this,
	KUidActiveInternetRadioApp,KIRPSMultiplePlay,RProperty::EInt);
	iMultiplePlayHandler->Subscribe();

	//Storing the view to be actiavted when in Background.
	iActivationPending = KNullUid;

	//Set only when going to backgroung when keypad lock.
	iBgWhenLock = EFalse;

	//For nowplaying view
///*************testing*****************************

	iSelector=CRemConInterfaceSelector::NewL();
	iTarget = CRemConCoreApiTarget::NewL( *iSelector, *this );
	iSelector->OpenTargetL();
	iIsDisconnected = EFalse;
	//****************offlineProfile***************//

	iNotifyHandler = ProEngFactory::NewNotifyHandlerL();
    User::LeaveIfError( iNotifyHandler->
       RequestProfileActivationNotificationsL( *this ));

	/**
     * Initialize feature manager, for IAD
     */
    IRRDEBUG2("CIRUi::ConstructL - Initializing FeatureManager Lib", KNullDesC);
    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;
    IRRDEBUG2("CIRUi::ConstructL - Initialed FeatureManager Lib", KNullDesC);
	IRRDEBUG2("CIRUi::ConstructL - Checking feature KFeatureIdIAUpdate", KNullDesC);
#ifdef FF_IAUPDATE
    if (FeatureManager::FeatureSupported( KFeatureIdIAUpdate))
		{
		IRRDEBUG2("CIRUi::ConstructL - KFeatureIdIAUpdate Supporting", KNullDesC);
		iUpdate = CIAUpdate::NewL(*this);
		IRRDEBUG2("CIRUi::ConstructL  - CIAUpdate NewL", KNullDesC);

       	iParameters = CIAUpdateParameters::NewL();
        IRRDEBUG2("CIRUi::ConstructL  - CIAUpdateParameters::NewL()", KNullDesC);

        // Use SIS package UID
        iParameters->SetUid( TUid::Uid( KUIDIRAPP) );
        IRRDEBUG2("CIRUi::ConstructL  - Setting Package Uid", KNullDesC);

        /**
         * We want InternetRadio.exe to be started after update is finished
         */
        iParameters->SetCommandLineExecutableL( KIRParamExec );
        IRRDEBUG2("CIRUi::ConstructL - Setting CommandLineExecutableL", KNullDesC);

        iUpdate->CheckUpdates(*iParameters );
        IRRDEBUG2("CIRUi::ConstructL  - After CheckUpdates", KNullDesC);

        }
#endif // FF_IAUPDATE


		iControlEventObserver = CIRControlEventObserver::
    	NewL( *this, KUidActiveInternetRadioApp, KIRPSControlSetPlayState ,
    	RProperty::EInt);

    	iControlEventObserver->SecondConstructL();
	IRLOG_DEBUG( "CIRUi::ConstructL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Returns pointer to the view stack.
// ---------------------------------------------------------------------------
//
CIRViewStack& CIRUi::ViewStack()
    {
    IRLOG_INFO( "CIRUi::ViewStack" );
    return *iViewStack;
    }
// ---------------------------------------------------------------------------
// CIRUi::ForwardBackCommandToBSWrapperL
// ---------------------------------------------------------------------------
//
TBool CIRUi::ForwardBackCommandToBsWrapperL()
	{
	IRLOG_DEBUG( "CIRUi::ForwardBackCommandToBsWrapperL - Entering" );
	TBool retVal( EFalse ); // By default BS Service doesn't handle back command

#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
	// Get active view UID.
	TVwsViewId activeViewId; // Contains both view UID and app UID
	GetActiveViewId( activeViewId );

	// Let Back Stepping Service handle the event.
	retVal = iBsWrapper->HandleBackCommandL( activeViewId );
#endif
	IRLOG_DEBUG( "CIRUi::ForwardBackCommandToBsWrapperL - Exiting" );
	return retVal;
	}


// ---------------------------------------------------------------------------
// CIRUi::ActiveView
// ---------------------------------------------------------------------------
//
TUid CIRUi::ActiveView() const
    {
    IRLOG_INFO( "CIRUi::ActiveView" );
    return iView ? iView->Id() : KNullUid;
    }


// ---------------------------------------------------------------------------
// CIRUi::CIRUi()
// Default constructor
// ---------------------------------------------------------------------------
//
CIRUi::CIRUi()
	{
	IRLOG_DEBUG( "CIRUi::CIRUi - Entering" );
	iIndex = 0;
	iSelect = ETrue;
	iCount = 0;
	iCurrentPresetId = 0;
	iSelectedBrowsingCategory = KErrNotFound;
	iFeatureManagerInitialized = EFalse;
	iMainViewLaunch = EFalse ;
	iDialogLauncher = NULL;
	iStartUpInNowPlayingView = EFalse;
	IRLOG_DEBUG( "CIRUi::CIRUi - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::~CIRUi()
// Destructor
// ---------------------------------------------------------------------------
//
CIRUi::~CIRUi()
	{
	IRLOG_INFO( "CIRUi::~CIRUi - Entering" );

	iStationsViewTitleString.Close();
	if(iSystemEventCollector)
		{
		iSystemEventCollector->RemoveObserver( this );
		delete iSystemEventCollector; iSystemEventCollector=NULL;
		}
	//singleton deletion
	if ( iIRSettings )
		{
		iIRSettings->Close();
		}

#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
        // Back Stepping FW is used, delete wrapper.
        delete iBsWrapper;
#endif

	delete iViewStack;
	delete iIconProvider;
	iFsSession.Close();
	delete iAlarmObserver;
//	delete iMessageObserver;
	delete iAlfEnv;
	if(iIsdsWrapper)
		{
		delete iIsdsWrapper;
		iIsdsWrapper = NULL;
		}
	iPlsLength.Close();
	if(iLastPlayed)
		{
		delete iLastPlayed;
		iLastPlayed = NULL;
		}
	if(iNowPlayingWrapper)
		{
		delete iNowPlayingWrapper;
		iNowPlayingWrapper = NULL;
		}
	delete iFavPresets;
	delete iSettingsView;
	delete iNwObserver;
	delete iPubSubManager;

	if (iNetworkController)
		{
		iNetworkController->Close();
		//iNetworkController = NULL
		}
	// Cancel the network indicator timer
	if( iNetworkIndicatorTimer )
		{
		if( iNetworkIndicatorTimer->IsActive() )
			{
			iNetworkIndicatorTimer->Cancel();
			}
		delete iNetworkIndicatorTimer;
		}
	delete iBitrateSubscriber;
	delete iMultiplePlayHandler;

	iURLs.ResetAndDestroy();
	iURLs.Close();
	iIsPresetSaved.Close();
	iPlsPresetSaved.Close();
	delete iSelector;

#ifdef FF_IAUPDATE
	delete iUpdate;
    delete iParameters;
#endif

    if ( iFeatureManagerInitialized )
    	{
        /**
         *  We can safely call UnInitializeLib as we have really intialized it
         */
        FeatureManager::UnInitializeLib();  // Decreases ref.count
        }
delete iNotifyHandler;

	if( iDialogLauncher )
		{
		delete iDialogLauncher;
		iDialogLauncher = NULL;
		}

	if(iControlEventObserver)
		{
		delete iControlEventObserver;
		iControlEventObserver = NULL;
		}

	IRLOG_DEBUG( "CIRUi::~CIRUi - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleCommandL(TInt aCommand)
// Command handling function
// ---------------------------------------------------------------------------
//
void CIRUi::HandleCommandL(TInt aCommand)
	{
	IRLOG_INFO2( "CIRUi::HandleCommandL (%d) ", aCommand );
	switch(aCommand)
		{
		case EIRDelayedPlayStation:
			{
			if ( !CheckAndNotifyLowDiskSpaceL() )
				{
				iNowPlayingWrapper->DoPlayL();
				}
			}
		break;
		// Activate the AddStationManually View
		case EAddStationManuallyCmd:
			{
			if ( !CheckAndNotifyLowDiskSpaceL() )
				{
				if ( IsForeground() || iBgWhenLock)
					{
					ActivateLocalViewL(KIRAddManuallyStationViewID);
					iActivationPending = KNullUid ;
					}
				else
					{
					iActivationPending = KIRAddManuallyStationViewID;
					}
				}
		}
		break;
		//Go to now playing view
		case EGotoNowPlayingViewCmd:
			{
			if( ( iNowPlayingView ) && ( !iNowPlayingView->HasBeenActivated() ) )
				{
	    		// For the case that NP view is force-activated,
	    		// but nothing should actually be played yet.
	    	    *iNowPlayingWrapper->iNowPlayingPreset = *iLastPlayed->iLastPlayedStation;
				}
			if(iHistory)
				{
					ActivateLocalViewL( KIRNowPlayingViewID,
					KHistoryLaunch,KIRNowPlayingViewCustomMessagePlayAfterActivation);

				}
			else if(iMainViewLaunch)
				{
				    _LIT8(KNull,"");
					ActivateLocalViewL( KIRNowPlayingViewID,KNowPlayingFrmMainView,KNull);
					iMainViewLaunch = EFalse ;

				}
			else
			{
			if ( IsForeground() || iBgWhenLock )
					{
					ActivateLocalViewL(KIRNowPlayingViewID);
					iActivationPending = KNullUid ;
					}
				else
					{
					iActivationPending = KIRNowPlayingViewID;
			}
				}
			}
			break;
		//violates PC lint  Warning 616
		case EEikCmdExit:
		case EExitCmd:  //lint -fallthrough
		case EIRExit:
		case EAknSoftkeyExit:
			{
			//This API is not there in 3.0
			//Removing all the local item definitions set using setlocalitemdefL()
			#ifndef __SERIES60_30__
			AknsUtils::SkinInstance()->RemoveLocalItemDefs();
			#endif
			//needed only for 3.2
			//part of active idle
			#ifdef __SERIES60_32__
			if ( iPubSubManager )
				{
				iPubSubManager->PublishApplicationRunningStateL
				(EIRPSApplicationClosing);
				}
			#endif
			if(iNowPlayingWrapper)
			{
			if( iNowPlayingWrapper->GetSessionStartState() )
				{
				TTerminationStatus terminationstatus = EUserTerminated;
				iNowPlayingWrapper->GetReportGeneratorInstance()->UpdateTerminatedBy(terminationstatus);
				iNowPlayingWrapper->GetReportGeneratorInstance()->SessionEndL(EFalse);
				iNowPlayingWrapper->SetSessionStartState(EFalse);
				}
				}
			Exit();
			}
		break;
		default:
			{
			//No implementation
			}
		break;
		}
	IRLOG_DEBUG( "CIRUi::HandleCommandL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRUi::IRHandleCommandL().
// Command handling function
// Saves currently selected index from the listbox
// ---------------------------------------------------------------------------
//
void CIRUi::IRHandleCommandL(TInt aIndex,TInt aCommand)
	{
	IRLOG_INFO3( "CIRUi::IRHandleCommandL (aIndex=%d, aCommand=%d)", aIndex, aCommand );
	//Save the currently selected index from the listbox
	iIndex = aIndex;
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRUi::IRHandleCommandL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::SavePrevIDL(..).
// Saves the previous View's Id
// ---------------------------------------------------------------------------
//
void CIRUi::SavePrevIDL(const TUid& aPrevID,TInt aCommand)
	{
	IRLOG_DEBUG( "CIRUi::SavePrevIDL - Entering" );
	//Save the previous view iD of NowPlaying View
	iNPPrevViewId = aPrevID;
	HandleCommandL(aCommand);
	IRLOG_DEBUG( "CIRUi::SavePrevIDL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleStatusPaneSizeChange().
// Handles resource changes, called by UI framework
// ---------------------------------------------------------------------------
//
void CIRUi::HandleStatusPaneSizeChange()
	{
	IRLOG_INFO( "CIRUi::HandleStatusPaneSizeChange" );

	}

// ---------------------------------------------------------------------------
// CIRUi::HandleKeyEventL().
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CIRUi::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/
	,TEventCode /*aType*/)
 	{
 	IRLOG_DEBUG( "CIRUi::HandleKeyEventL" );
	return EKeyWasNotConsumed;
 	}


// ---------------------------------------------------------------------------
// CIRUi::GetMifFilePath().
// Returns the full path of mif file.
// ---------------------------------------------------------------------------
//
TFileName CIRUi::GetMifFilePath() const
	{
    IRLOG_DEBUG( "CIRUi::GetMifFilePath" );
    return iMifFilePath;
	}

// ---------------------------------------------------------------------------
// CIRUi::GetPrivatePath().
// Returns the full path of mif file. .
// ---------------------------------------------------------------------------
//
TFileName CIRUi::GetPrivatePath() const
	{
	IRLOG_DEBUG( "CIRUi::GetPrivatePath" );
    return iIRSettings->PrivatePath();
	}



// ---------------------------------------------------------------------------
// CIRUi::OpenFileL().
// Open the file using Document's instance
// ---------------------------------------------------------------------------
//
void CIRUi::OpenFileL(const TDesC& aFileName)
	{
	IRLOG_DEBUG( "CIRUi::OpenFileL - Entering" );
	// File changed. Open new file with documents OpenFileL method.
	iSecondInstanceFlag=ETrue;
	CIRDocument* doc = static_cast<CIRDocument*> (Document());
	doc->OpenFileL( ETrue, aFileName, iEikonEnv->FsSession() );
	IRLOG_DEBUG( "CIRUi::OpenFileL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::IsEmbedded().
// ---------------------------------------------------------------------------
//
TBool CIRUi::IsEmbedded() const
    {
    IRLOG_DEBUG( "CIRUi::IsEmbedded" );
    return iEikonEnv->StartedAsServerApp();
    }


// ---------------------------------------------------------------------------
// CIRUi::UpdateCurrentNetwork
// Updates the current network status
// ---------------------------------------------------------------------------
//
void CIRUi::UpdateCurrentNetwork(const TDesC& aCurrentNetworkMCC
	,const TDesC& aCurrentNetworkMNC)
	{
	IRLOG_INFO3( "CIRUi::UpdateCurrentNetwork (MCC=%S, MNC=%S)", &aCurrentNetworkMCC, &aCurrentNetworkMNC );
	#ifdef __WINS__
	iCurrentNetwork.Copy(aCurrentNetworkMCC);
	iCurrentNetwork.Append(aCurrentNetworkMNC);
	iCurrentOperator = 0;
	#else
	//updates the current network MCC+MNC
	iCurrentNetwork.Copy(aCurrentNetworkMCC);
	iCurrentNetwork.Append(aCurrentNetworkMNC);
	TLex currentnetwork(iCurrentNetwork);
	TInt error = currentnetwork.Val(iCurrentOperator);
	if(error)
		{
		IRLOG_ERROR2( "CIRUi::UpdateCurrentNetwork - extracting operator value failed (error=%d)", error );
		iCurrentOperator = 0;
		}
	#endif
	iNowPlayingWrapper->GetReportGeneratorInstance()->UpdateCurrentBrowseNetwork(iCurrentOperator);
	IRLOG_DEBUG( "CIRUi::UpdateCurrentNetwork - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::UpdateHomeNetwork
// Updates the home network status
// ---------------------------------------------------------------------------
//
void CIRUi::UpdateHomeNetwork(const TDesC& aCurrentNetworkMCC,
	const TDesC& aCurrentNetworkMNC)
	{
	IRLOG_INFO3( "CIRUi::UpdateHomeNetwork (MCC=%S, MNC=%S)", &aCurrentNetworkMCC, &aCurrentNetworkMNC );
	#ifdef __WINS__
	iHomeNetwork.Copy(aCurrentNetworkMCC);
	iHomeNetwork.Append(aCurrentNetworkMNC);
	iHomeOperator = 0;
	#else
	//updates the home network MCC+MNC
	iHomeNetwork.Copy(aCurrentNetworkMCC);
	iHomeNetwork.Append(aCurrentNetworkMNC);
	TLex currentnetwork(iHomeNetwork);
	TInt error = currentnetwork.Val(iHomeOperator);
	if(error)
		{
		IRLOG_ERROR2( "CIRUi::UpdateHomeNetwork - extracting operator value failed (error=%d)", error );
		iHomeOperator = 0;
		}

	#endif
	IRLOG_DEBUG( "CIRUi::UpdateHomeNetwork - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::SetCurrentNetworkType(..)
// Sets the Current Network Type
// ---------------------------------------------------------------------------
//
void CIRUi::SetCurrentNetworkType()
	{
	IRLOG_DEBUG( "CIRUi::SetCurrentNetworkType - Entering" );
	switch(iNetworkController->IdentifyConnectionType())
		{
		case EGprs://GPRS
			{
			iConnectionType.Copy(KGprs);
			//start monitoring network for operator change
			iNwObserver->IRStartNwMonitor();
			}
		break;

		case EEdge: // Edge
			{
			iConnectionType.Copy(KEdge);
			//start monitoring network for operator change
			iNwObserver->IRStartNwMonitor();
			}
		break;
		case EWcdma:
		case ECdma2000:
			{
			iConnectionType.Copy(K3G);
			//start monitoring network for operator change
			iNwObserver->IRStartNwMonitor();
			}
		break;

		case EWiFi:
			{
			iConnectionType.Copy(KWiFi);
			//start monitoring network for operator change
			iNwObserver->IRStartNwMonitor();
			}
		break;
		}
	iNowPlayingWrapper->GetReportGeneratorInstance()->UpdateConnectionType(iConnectionType);
	IRLOG_DEBUG( "CIRUi::SetCurrentNetworkType - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::GetCurrentNetworkType(..)
// Returns the Current Network Type
// ---------------------------------------------------------------------------
//
TDesC& CIRUi::GetCurrentNetworkType()
{
IRLOG_DEBUG( "CIRUi::GetCurrentNetworkType - Entering" );
IRLOG_DEBUG( "CIRUi::GetCurrentNetworkType - Exiting." );
return iConnectionType;
}
// CIRUi::MrccatoCommand()
// This is call back function which observes hardware key events
// ---------------------------------------------------------------------------
//
void CIRUi::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
	TRemConCoreApiButtonAction aButtonAct)
	{
	IRLOG_INFO3( "CIRUi::MrccatoCommand(aOperationId=%d, aButtonAct=%d)", aOperationId, aButtonAct );

	IRLOG_DEBUG2("CIRUi::MrccatoCommand - MrccatoCommand()",
	KNullDesC );
	switch( aOperationId )
		{

		case ERemConCoreApiVolumeUp:   // -fall through
		case ERemConCoreApiVolumeDown:
			{
			if(iNowPlayingWrapper->iPlaying ||iView->Id() == KIRNowPlayingViewID )
				{
				//volume is increased or reduced
				if ( (ERemConCoreApiVolumeUp == aOperationId )|| (aOperationId == ERemConCoreApiVolumeDown))
					{

					TRAPD(err,iNowPlayingView->HandleMediaKeysL(aOperationId,aButtonAct);)
					if(err!=KErrNone)
						{
						return;
						}
					}
				}
			}
		break;
		case ERemConCoreApiStop:
			{
			if(iNowPlayingView)
				{
				iNowPlayingView->MrccatoCommand(ERemConCoreApiStop,aButtonAct);
				}
			}
		break;

		default:
		if (iNowPlayingView )
			{
			iNowPlayingView->MrccatoCommand( aOperationId, aButtonAct );
			}
		break;

		}
	IRLOG_DEBUG( "CIRUi::MrccatoCommand - Exiting." );
	}

// ---------------------------------------------------------
// CIRUi::MrccatoPlay()
// ---------------------------------------------------------
//
void CIRUi::MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, TRemConCoreApiButtonAction aButtonAct)
    {
    IRLOG_INFO3( "CIRUi::MrccatoPlay(aSpeed=%d, aButtonAct=%d)", aSpeed, aButtonAct );

    if( iView->Id() != KIRCostWarningViewId )
        {
        if(iNowPlayingView )
            {
            iNowPlayingView->MrccatoPlay( aSpeed, aButtonAct );
            }
        }

    IRLOG_DEBUG( "CIRUi::MrccatoPlay - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRUi::IsCallActive()
// Function returns whether the call is active or not
// ---------------------------------------------------------------------------
//
TBool CIRUi::IsCallActive()
	{
	IRLOG_DEBUG( "CIRUi::IsCallActive" );
	//returns whether call is active or not
	return iCallActive;
	}

//-----------------------------------------------------------------------------
// GetWrapperInstance()
// Returns the instance of the IsdsWrapper class
//-----------------------------------------------------------------------------
CIRIsdsWrapper* CIRUi::GetWrapperInstance()
	{
	IRLOG_DEBUG( "CIRUi::GetWrapperInstance" );
	return iIsdsWrapper;
	}

//-----------------------------------------------------------------------------
// GetNowPlayingWrapperInstance()
// Returns the instance of the NowPlayingWrapper class
//-----------------------------------------------------------------------------
CIRNowPlayingWrapper* CIRUi::GetNowPlayingWrapperInstance()
	{
	IRLOG_DEBUG( "CIRUi::GetNowPlayingWrapperInstance" );
	return iNowPlayingWrapper;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetSystemEventCollector()
// returns the iSystemEventCollector
// ---------------------------------------------------------------------------
//
CIRSystemEventCollector& CIRUi::SystemEventCollector()
    {
    IRLOG_DEBUG( "CIRUi::SystemEventCollector" );
    return *iSystemEventCollector;
    }

// ---------------------------------------------------------------------------
// CIRUi::IRNetworkEventL()
// network connection events
// ---------------------------------------------------------------------------
//
void CIRUi::IRNetworkEventL(TIRNetworkEvent aEvent)
	{
	IRLOG_INFO2("CIRUi::IRNetworkEventL( aEvent=%d)", aEvent );
    IRRDEBUG2("CIRUi::IRNetworkEventL - Entering(aEvent=%d)", aEvent );
	switch(aEvent)
		{
		case ENetworkConnectionConnecting:
		    {
			// This value indicates the network icon blink duration
		    const TInt KNetworkIndicatorAnimation = 500000;
			// This value indicates the network icon blink duration
            TTimeIntervalMicroSeconds32 interval(KNetworkIndicatorAnimation);
            // Cancel the network indicator timer
            iNetworkIndicatorTimer->Cancel();
            // Start the network animation timer
            iNetworkIndicatorTimer->Start(interval,interval,TCallBack(
			CIRUi::NetworkAnimationTimerExpired,this));
		    }
		    break;

		case ENetworkConnectionEstablished:
			{
            iNetworkIndicatorTimer->Cancel();
            iIsDisconnected = EFalse;
			SetCurrentNetworkType();
			iIsdsWrapper->GetISDSInstance()->IRGetIRIDL();
			iNetworkController->NotifyActiveNetworkObserversL(ENetworkConnectionEstablished);
			}
		break;

		case ENetworkConnectionDisconnected:
			{
			IRRDEBUG2("IRNetworkEventL Network DisConnected",KNullDesC);

			//network got disconnected
				iNetworkIndicatorTimer->Cancel();
				//connection status is reset in network controller
				iNetworkController->ResetConnectionStatus();
	            if ( iIsdsWrapper->GetISDSInstance() )
	                {
	                if(iIsdsWrapper->GetListenRequest()||(iNowPlayingWrapper->iSyncPreset))
				  		{
						iNowPlayingWrapper->DestroyWaitDialogL();
				  		}
	                iIsdsWrapper->DestroyWaitDialogL();
           			iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
	 			    iIsdsWrapper->GetISDSInstance()->ReleaseResources();
	                }
            	if(iNowPlayingWrapper->iPlaying && (!iNowPlayingWrapper->IsProgress()))
					{
					iNowPlayingWrapper->StopNow();
					}
				else
					{
			        if(iNowPlayingWrapper->GetConnectToServerState() && !iNowPlayingWrapper->IsProgress())
						{
						iNowPlayingWrapper->CancelNetworkRequestL();
						iNowPlayingWrapper->DestroyWaitDialogL();
						}
					//player is stopped
					if( iNowPlayingWrapper->IsProgress() )
						{
						//if in progress bar view, progress bar is cancelled
						iNowPlayingWrapper->ProgressBarCancel();
						iNowPlayingWrapper->DestroyWaitDialogL();
						}
					}

				iIsDisconnected = ETrue;
				}


		break;

		case EDisplayNetworkMessageNoConnectivity:
			{
			if(iNowPlayingView->iContainer && iNowPlayingView->GetRequestPendingForNextOrPreV())
				{
				 	iNowPlayingView->SetRockerIndex(
					iNowPlayingView->GetPreviousIndex());
				}
			//display a message on no network connectivity
			DisplayErrorNoteL(R_IRAPP_NW_NOCONN_ERROR);
			iIsDisconnected = ETrue;

			}
		break;

		case EDisplayOfflineMode:
			{
			//if offline mode message is displayed
			//DisplayErrorNoteL(R_IRAPP_INFONOTE_OFFLINE)
			}
		break;

		case EDisplayNoAccessPointsDefined:
			{
			//if access point is not defined
			DisplayErrorNoteL(R_IRAPP_NETWORKCONTROLLER_NOAP);
			}
		break;
		case EAccessPointSelectionCancelled:
			{
			// Called if the user chooses to cancel access point
			// selection
			}
		break;
		}
    IRRDEBUG2("CIRUi::IRNetworkEventL - Exiting", KNullDesC );

	IRLOG_DEBUG( "CIRUi::IRNetworkEventL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : NetworkAnimationTimerExpiredL
// Called when the buffering times out
// ---------------------------------------------------------------------------
//
TInt CIRUi::NetworkAnimationTimerExpired(TAny* /*aPtr*/)
	{
    IRLOG_INFO( "CIRUi::NetworkAnimationTimerExpired - Entering" );
    IRLOG_DEBUG( "CIRUi::NetworkAnimationTimerExpired - Exiting." );
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CIRUi::ActivateCurrentView()
// Calls Draw of the currently activated view
// ---------------------------------------------------------------------------
//
void CIRUi::ActivateCurrentView(const TUid& aUid) const
	{
	IRLOG_DEBUG( "CIRUi::ActivateCurrentView - Entering" );
	//If NowPlaying View is activated
	if(aUid == KIRNowPlayingViewID)
		{
		if(iNowPlayingView->iContainer)
			{
		    }
		}

	IRLOG_DEBUG( "CIRUi::ActivateCurrentView - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRUi::VerifyNetworkConnectivityL()
// Verifies the network connectivity
// This function can leave if displaying of info note leaves hence
// this leave can be ignored
// ---------------------------------------------------------------------------
//
TBool CIRUi::VerifyNetworkConnectivityL()
	{
	IRLOG_DEBUG( "CIRUi::VerifyNetworkConnectivityL - Entering" );

	TBool returnValue = EFalse;
	iIsDisconnected =EFalse;
	if(!(iNetworkController->GetNetworkStatus()))
		{
	IRRDEBUG2("CIRUi::VerifyNetworkConnectivityL Entering ", KNullDesC);

		if( iNetworkController->iIsConnectRequestIssued )
			{
			returnValue = ETrue;
			}
		else
			{
	        IRLOG_ERROR( "CIRUi::VerifyNetworkConnectivityL - No connection request issued." );
			//offer the user to connect to network
			ActivateCurrentView(iPresentViewID);
			//Choose the accesspoint
			iNetworkController->ChooseAccessPointL();
			returnValue = ETrue;
			}
		}
	IRRDEBUG2("CIRUi::VerifyNetworkConnectivityL Exiting ", KNullDesC);

	IRLOG_DEBUG( "CIRUi::VerifyNetworkConnectivityL - Exiting" );
	return returnValue;
	}

// ---------------------------------------------------------------------------
// CIRUi::FocusWhenSelectGot()
// Funtion is called to return the flag when a listbox item is selected
// so that the next view gets activated
// ---------------------------------------------------------------------------
//
TBool CIRUi::FocusWhenSelectGot()  const
	{
	IRLOG_DEBUG2( "CIRUi::FocusWhenSelectGot - Returning %d", iSelect );
	return(iSelect);
	}

// ---------------------------------------------------------------------------
// CIRUi::FocusWhenSelectSet()
// Funtion is called to set flag when an listbox item is selected
// so that the next view gets activated
// ---------------------------------------------------------------------------
//
void CIRUi::FocusWhenSelectSet(TBool aFocus)
	{
	IRLOG_DEBUG2( "CIRUi::FocusWhenSelectSet(aFocus=%d)", aFocus );
	iSelect=aFocus;
	IRLOG_DEBUG2( "CIRUi::FocusWhenSelectSet(aFocus=%d)" , aFocus);
	}


// ---------------------------------------------------------------------------
// CIRUi::HandlePropertyChangeL
// Derived from class MIRPropertyChangeObserver
// ---------------------------------------------------------------------------
//
void CIRUi::HandlePropertyChangeL(const TUid& /*aCategory*/, const TUint /*aKey*/, const TIRTextProperty& /*aValue*/ )
	{
	IRLOG_DEBUG( "CIRUi::HandlePropertyChangeL" );
	return;
	}

// ---------------------------------------------------------------------------
// CIRUi::HandlePropertyChangeL
// Derived from class MIRPropertyChangeObserver
// ---------------------------------------------------------------------------
//
void CIRUi::HandlePropertyChangeL(const TUid& /*aCategory*/,
								const TUint /*aKey*/, const TInt /*aValue*/ )
	{
	IRLOG_DEBUG( "CIRUi::HandlePropertyChangeL " );
	}

// ---------------------------------------------------------------------------
// CIRUi::IsLandscapeOrientation
// ---------------------------------------------------------------------------
//
TBool CIRUi::IsLandscapeOrientation() const
    {
    IRLOG_DEBUG( "CIRUi::IsLandscapeOrientation" );
    return iLandscape;
    }



// ---------------------------------------------------------------------------
// CIRUi::UpdateLandscapeInformation
// ---------------------------------------------------------------------------
//
void CIRUi::UpdateLandscapeInformation()
    {
    IRLOG_DEBUG( "CIRUi::UpdateLandscapeInformation - Entering" );
    TSize screenSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize );
    // Is it portrait or landscape, compare height and width, since we can't
    // be sure if EGraphicsOrientationNormal is portrait or landscape
    if ( screenSize.iHeight < screenSize.iWidth )
        {
        iLandscape = ETrue;
        }
    else
        {
        iLandscape = EFalse;
        }

    //Remove this hack notification when Alfred starts to notify the controls of portrait/landscape changes.
    IRLOG_DEBUG( "CIRUi::UpdateLandscapeInformation - Exiting" );
    }


    // ---------------------------------------------------------------------------
// CIRUi::ChooseLayoutResource
// ---------------------------------------------------------------------------
//
TInt CIRUi::ChooseLayoutResource( TInt aNormalPortraitRes,
        TInt aMirroredPortraitRes, TInt aNormalLandscapeRes,
        TInt aMirroredLandscapeRes ) const
    {
    IRLOG_DEBUG( "CIRUi::ChooseLayoutResource - Entering" );
    TInt resId = 0;

    if ( IsLandscapeOrientation() )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            resId = aMirroredLandscapeRes;
            }
        else
            {
            resId = aNormalLandscapeRes;
            }
        }
    else
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            resId = aMirroredPortraitRes;
            }
        else
            {
            resId = aNormalPortraitRes;
            }
        }
    IRLOG_DEBUG( "CIRUi::ChooseLayoutResource - Exiting" );
    return resId;
    }
// ---------------------------------------------------------------------------
// CIRUi::HandlePropertyChangeErrorL
// Derived from class MIRPropertyChangeObserver
// ---------------------------------------------------------------------------
//
void CIRUi::HandlePropertyChangeErrorL(const TUid& /*aCategory*/,
						const TUint /*aKey*/, TInt /*aError*/)
	{
	IRLOG_DEBUG( "CIRUi::HandlePropertyChangeErrorL" );
	//Implementation is not required
	}


// ---------------------------------------------------------------------------
//  CIRUi::sGetLockInfo()
// gets information about keypad lock.
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetLockInfo()
	{
	IRLOG_DEBUG( "CIRUi::GetLockInfo - Entering" );
	 TBool isLock(EFalse);
	 RAknKeyLock keylock;
	 TInt err = keylock.Connect();
	 if ( !err )
		{
		 //check if keypad is locked.
		 if ( keylock.IsKeyLockEnabled() )
		    {
		    isLock = ETrue;
		    }
		 else
			{
			isLock = EFalse;
			}
		keylock.Close();
		}
	 IRLOG_DEBUG( "CIRUi::GetLockInfo - Exiting" );
	return ( isLock );
	}

// ---------------------------------------------------------------------------
// UpdateSubscriberIdL()
// Updates the IMSI code and sets the Terms & Conditions flag as necessary.
// ---------------------------------------------------------------------------
//
void CIRUi::UpdateSubscriberIdL()
    {
    IRLOG_DEBUG( "CIRUi::UpdateSubscriberIdL - Entering" );
    CMDBSession* dbSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( dbSession );

    CMDBRecordSet<CCDModemBearerRecord>* recordSet = new ( ELeave )
    				CMDBRecordSet<CCDModemBearerRecord>( KCDTIdModemBearerRecord );
    CleanupStack::PushL( recordSet );

    recordSet->LoadL( *dbSession );
    if ( !recordSet->iRecords.Count() )
        {
        IRLOG_WARNING( "CIRUi::UpdateSubscriberIdL - Loading subscriber record set failed." );
        User::Leave( KErrNotFound );
        }

    TInt index = 0;
    CCDModemBearerRecord* record = NULL;
    do
        {
        record = static_cast<CCDModemBearerRecord*>( recordSet->iRecords[index++] );
        } while ( record->iTsyName.IsNull() && index < recordSet->iRecords.Count() );

    TPtrC tsyName( static_cast<const TDesC&>( record->iTsyName ) );

    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL( telServer );
    User::LeaveIfError( telServer.LoadPhoneModule( tsyName ) );

    TInt numPhones = 0;
    User::LeaveIfError( telServer.EnumeratePhones( numPhones ) );

    TInt legalPhoneIndex = KErrNotFound;
    RTelServer::TPhoneInfo phoneInfo;
    for ( TInt i = 0; i < numPhones && legalPhoneIndex == KErrNotFound; i++ )
        {
        if ( telServer.GetPhoneInfo( i, phoneInfo ) == KErrNone )
            {
            if ( phoneInfo.iNetworkType == RTelServer::ENetworkTypeMobileDigital )
                {
                legalPhoneIndex = i;
                }
            }
        }
    User::LeaveIfError( legalPhoneIndex );

    RMobilePhone phone;
    User::LeaveIfError( phone.Open( telServer, phoneInfo.iName ) );
    CleanupClosePushL( phone );

    TUint32 identityCaps;
    User::LeaveIfError( phone.GetIdentityCaps( identityCaps ) );

    if ( identityCaps & RMobilePhone::KCapsGetSubscriberId )
        {
        TRequestStatus status;
        RMobilePhone::TMobilePhoneSubscriberId subscriberId;
        phone.GetSubscriberId( status, subscriberId );
        User::WaitForRequest( status );
        if ( !status.Int() && iIRSettings->SubscriberIdL().Compare( subscriberId ) )
            {
            iIRSettings->SetSubscriberIdL( subscriberId );
            iIRSettings->ReSetFlagCostWarningL();
            iIRSettings->ReSetFlagTermsAndConditionsL();
            }
        }
    CleanupStack::PopAndDestroy( KFour, dbSession );
    IRLOG_DEBUG( "CIRUi::UpdateSubscriberIdL - Exiting." );
    }
// ---------------------------------------------------------------------------
// SelectedBrowsingCategory()
// Returns the last selected browsing category
// ---------------------------------------------------------------------------
//
TInt CIRUi::SelectedBrowsingCategory() const
    {
    IRLOG_DEBUG( "CIRUi::SelectedBrowsingCategory" );
    return iSelectedBrowsingCategory;
    }
// ---------------------------------------------------------------------------
// CancelDelayedAction()
// ---------------------------------------------------------------------------
//CIRUi::CreateWritablePrivatePath()
//Only for ROM SDK3.2
//creates a private path in c drive if it doesnot exist (for rom install)
// ---------------------------------------------------------------------------
//
void CIRUi::CreateWritablePrivatePath()
	{
	IRLOG_DEBUG( "CIRUi::CreateWritablePrivatePath - Entering." );
	TInt err;
    err = iFsSession.CreatePrivatePath(EDriveC);
    if(KErrNone != err)
    	{
    	TRAP_IGNORE(DisplayErrorNoteL(R_IRAPP_IRGENERAL_ERROR))
    	}
    IRLOG_DEBUG( "CIRUi::CreateWritablePrivatePath - Exiting." );
	}
// -----------------------------------------------------------------------------
// From class CEikAppUi.
// Handles a change to the application's resources which are shared across
// the environment.
// -----------------------------------------------------------------------------
//
void CIRUi::HandleResourceChangeL( TInt aType )
	{
	IRLOG_DEBUG( "CIRUi::HandleResourceChangeL - Entering." );
	CAknViewAppUi::HandleResourceChangeL( aType );
	UpdateLandscapeInformation();

	CEikStatusPane* statP = StatusPane();
	if(IsLandscapeOrientation())
		{
		statP->SwitchLayoutL(R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL);
		}
	else
		{
		if ( ActiveView() == KIRAddManuallyStationViewID)
			{
			statP->SwitchLayoutL( R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL);

        	}
		else
			{
			statP->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT);
			}

		}
	TriggerCommand();

	IRLOG_DEBUG( "CIRUi::HandleResourceChangeL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Triggers the command to view handling
// ---------------------------------------------------------------------------
//
void CIRUi::TriggerCommand()
    {
    IRLOG_DEBUG( "CIRUi::TriggerCommand - Entering." );
    TVwsViewId viewId( KNullUid, KNullUid );
    CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
    TInt err = appUi->GetActiveViewId( viewId );
    RProcess process;
    TSecureId id = process.SecureId();
    // GetActiveViewId doesn't always work (when switched back to VRA through RadioLauncher icon
    if( !err && viewId.iAppUid.iUid == id.iId )
        {
        CAknView* view = appUi->View( viewId.iViewUid );
        if( view )
            {
            view->HandleStatusPaneSizeChange();
            }
        }
    IRLOG_DEBUG( "CIRUi::TriggerCommand - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRUi::HandleWsEventL(..)
// From class CEikAppUi.
// Handles window server events.
// ---------------------------------------------------------------------------
//
void CIRUi::HandleWsEventL( const TWsEvent& aEvent,
                            CCoeControl* aDestination )
	{
	IRLOG_DEBUG( "CIRUi::HandleWsEventL - Entering" );


	// We don't want to shutdown on end key
	if ( aEvent.Type() != KAknUidValueEndKeyCloseEvent )
		{
		CAknViewAppUi::HandleWsEventL( aEvent, aDestination );
		}
	else
		{
		if(IsForeground())
			{
			//variable currenttask is used check whether current task is foreground
			//or background
			TApaTask currenttask( iEikonEnv->WsSession() );
			currenttask.SetWgId( iEikonEnv->RootWin().Identifier() );
			currenttask.SendToBackground();
			}
		}
	IRLOG_DEBUG( "CIRUi::HandleWsEventL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRUi::GainingForeGroundL().
// Function handles when application gains foreground focus
// ---------------------------------------------------------------------------
//
void CIRUi::GainingForeGroundL()
	{
	IRLOG_DEBUG( "CIRUi::GainingForeGroundL - Entering" );
		if(iBgWhenLock )
			{
			iBgWhenLock = EFalse;
			}

		if(iActivationPending != KNullUid)
			{
			TUid tempraryUid=iActivationPending;
		    // Reset even if ActivateLocalViewL leaves
            iActivationPending = KNullUid;
            ActivateLocalViewL( tempraryUid );
			}

	IRLOG_DEBUG( "CIRUi::GainingForeGroundL - Exiting." );
	return;
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleGainingForeground().
// This is call back function which observes application focus gaining
// foreground
// ---------------------------------------------------------------------------
//
void CIRUi::HandleGainingForeground()
	{
	// GainingForeGroundL() handles the functionality of loading the iRAPP
	// view along with the buffering control if buffering crtl is present.
	// Handled while iRAPP is put to background & then gains foreground focus.
	IRLOG_DEBUG( "CIRUi::HandleGainingForeground - Entering" );
	if( IsForeground() )
		{
		TRAP_IGNORE(GainingForeGroundL())
		}
	IRLOG_DEBUG( "CIRUi::HandleGainingForeground - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRUi::HandleLosingForeground().
// This is call back function which observes application focus losing
// foreground
// ---------------------------------------------------------------------------
//
void CIRUi::HandleLosingForeground()
	{
	IRLOG_DEBUG( "CIRUi::HandleLosingForeground - Entering" );
	LosingForeground();
	IRLOG_DEBUG( "CIRUi::HandleLosingForeground - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::LosingForeground().
// Function handles when application looses foreground focus
// ---------------------------------------------------------------------------
//
void CIRUi::LosingForeground()
	{
	if( GetLockInfo()  )
		{
		iBgWhenLock = ETrue;
		}
	IRLOG_DEBUG( "CIRUi::LosingForeground" );
	}

// ---------------------------------------------------------------------------
// CIRUi::IconProvider().
// Returns the instance of icon provider.
// ---------------------------------------------------------------------------
//
CIconFileProvider* CIRUi::IconProvider()
	{
	IRLOG_DEBUG( "CIRUi::IconProvider" );
	return iIconProvider;
	}



// ---------------------------------------------------------------------------
// CIRUi::ParseAndPrepareUrl()
// Checks for the URL whether its valid or not
// ---------------------------------------------------------------------------
//
TBool CIRUi::ParseAndPrepareUrl(const TDesC& aUrl)
	{
	IRLOG_INFO2( "CIRUi::ParseAndPrepareUrl(aUrl=%S)", &aUrl );
	_LIT8(KHttp,"http");
	_LIT8(KMms, "mms");
	TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8(iUrl8,aUrl);
	if(err != KErrNone)
		{
	    IRLOG_ERROR2("CIRUi::ParseAndPrepareUrl - Unicode to Utf8 conversion failed (err=%d)", err );
		return EFalse;
		}

    IRLOG_DEBUG2( "CIRUi::ParseAndPrepareUrl - Converted URL=%S", &iUrl8 );



	TUriParser8 uriParser;

	// Initialize the Uri Parser
	if ( uriParser.Parse( iUrl8 ) != KErrNone )
	    {
	    IRLOG_ERROR( "CIRUi::ParseAndPrepareUrl - Error while parsing URI." );
		return EFalse;
	    }

	//Extract protocol
	if( uriParser.Extract( EUriScheme ).Length() > KSmallBufSize )
		{
	    IRLOG_ERROR("CIRUi::ParseAndPrepareUrl - Extracted URI scheme component too large." );
		return EFalse;
		}
	else
		{
		TBuf8<KSmallBufSize> uriScheme;
		uriScheme.Copy( uriParser.Extract( EUriScheme ) );
		uriScheme.LowerCase();
		if ( uriScheme.Compare(KHttp) && uriScheme.Compare(KMms) )
			{
	        IRLOG_ERROR("CIRUi::ParseAndPrepareUrl - Invalid URI scheme." );
			return EFalse;
			}
		}

	// Extract the Host Name
	if ( uriParser.Extract( EUriHost ).Length() > KMaxBufSize )
		{
	    IRLOG_ERROR( "CIRUi::ParseAndPrepareUrl - Extracted Host component too large." );
		return EFalse;
		}
	if ( uriParser.Extract( EUriHost ).Length() <= 0 )
	    {
	    IRLOG_ERROR( "CIRUi::ParseAndPrepareUrl - Empty Host component." );
	    return EFalse;
	    }

	IRLOG_DEBUG( "CIRUi::ParseAndPrepareUrl - Exiting." );
	return ETrue;
	}

/// ---------------------------------------------------------------------------
// CIRUi::DisplayErrorNoteL()
// Display the error note
// ---------------------------------------------------------------------------
//
void CIRUi::DisplayErrorNoteL(TInt aResourceId)
	{
	IRLOG_DEBUG( "CIRUi::DisplayErrorNoteL - Entering" );
	HBufC* error = StringLoader::LoadLC(aResourceId);
	iIsdsWrapper->GetDialogLauncherInstance()->ShowErrorNoteL(aResourceId,ETrue);
	CleanupStack::PopAndDestroy(error);
	IRLOG_DEBUG( "CIRUi::DisplayErrorNoteL - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::DisplayErrorNoteL()
// Display the error note
// ---------------------------------------------------------------------------
//
void CIRUi::DisplayErrorNoteL(TInt aResourceId, const TDesC &  aString)
	{
	IRLOG_DEBUG( "CIRUi::DisplayErrorNoteL - Entering" );
	HBufC* error = StringLoader::LoadLC(aResourceId, aString);
	CleanupStack::PopAndDestroy(error);
	IRLOG_DEBUG( "CIRUi::DisplayErrorNoteL - Exiting" );
	}


// ---------------------------------------------------------------------------
// CIRUi::UpdateNowPlayingByteCounter()
// Retrieves current Byte Counter for displaying in Now playing view
// ---------------------------------------------------------------------------
//
void CIRUi::UpdateNowPlayingByteCounter()
	{
	IRLOG_DEBUG( "CIRUi::UpdateNowPlayingByteCounter - Entering" );
	const MIRDataTransferTracker::TIRDataTransferPckg& data = iNetworkController->
				DataTransferTracker().TransferredData();
	TReal32  total_bytes = data.iBytesSentTotal + data.iBytesReceivedTotal;
	if(iNowPlayingView->iContainer)
		{
		TRAP_IGNORE(iNowPlayingView->DisplayByteCounterL(total_bytes))
		}
	IRLOG_DEBUG( "CIRUi::UpdateNowPlayingByteCounter - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleDataTransferEventL( const MIRDataTransferTracker::TIRDataTransferPckg& aData )
// DataTransferObserver notification to receive byte counter
// ---------------------------------------------------------------------------
//
void  CIRUi::HandleDataTransferEventL( const MIRDataTransferTracker::TIRDataTransferPckg& aData )
	{
	IRLOG_DEBUG( "CIRUi::HandleDataTransferEventL - Entering" );
	TReal32 total_bytes = aData.iBytesSentTotal + aData.iBytesReceivedTotal;
	if(iNowPlayingView->iContainer)
		{
		iNowPlayingView->DisplayByteCounterL(total_bytes);
		}
	// Byte Counter Testing function (to be removed)
	IRLOG_DEBUG( "CIRUi::HandleDataTransferEventL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::IsFirstTime()
// returns which view should be launched.
// ---------------------------------------------------------------------------
//
TBool CIRUi::IsFirstTime()
	{
	IRLOG_DEBUG( "CIRUi::IsFirstTime - Entering" );
	TInt returnValue=0;
	TRAP_IGNORE(returnValue=IsFlagIfAnyFavL());
	if(iFavPresets->iFavPresetList.Count())
		{
		if(returnValue == 0)
			{
			TRAP_IGNORE(iIRSettings->SetFlagIfAnyFavL();)
			}
		}

	IRLOG_DEBUG( "CIRUi::IsFirstTime - Exiting" );

	if((iLastPlayed->iLastPlayedStation->iUrlArray->Count() == 0) &&
				(iFavPresets->iFavPresetList.Count()== 0) && (!returnValue))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
// ---------------------------------------------------------------------------
// CIRUi::SetCommandSetL()
// Dynamically changing the cba buttons.
// ---------------------------------------------------------------------------
//
void CIRUi::SetCommandSetL(TInt aResourceId)
	{
	IRLOG_DEBUG( "CIRUi::SetCommandSetL - Entering" );
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	cba->SetCommandSetL( aResourceId );
	IRLOG_DEBUG( "CIRUi::SetCommandSetL - Exiting" );
	}

//--------------------------------------------------------------------------
// CIRUi::IsForeground()
//IsForeground()	 overloaded
//---------------------------------------------------------------------------

TBool CIRUi::IsForeground()
	{
	IRLOG_DEBUG( "CIRUi::IsForeground - Entering" );
	#ifdef __WINS__
	// Stop screen saver from activating
	User::ResetInactivityTime();
	#endif
	IRLOG_DEBUG( "CIRUi::IsForeground - Exiting" );
	TBool isForeground = CAknAppUiBase::IsForeground();
	return isForeground;
	}
// ---------------------------------------------------------------------------
// CIRUi::ParsePlsDataL(..).
// Parse the PLS data
// ---------------------------------------------------------------------------
//
void CIRUi::ParsePlsDataL(CFileStore*& /*aFileStore*/, RFile& aFile)
	{
	IRLOG_INFO( "CIRUi::ParsePlsDataL - Entering" );
	CIRPlsPlayList* playList= CIRPlsPlayList::NewLC(aFile);
	if(iURLs.Count()>0)
		{
		iURLs.ResetAndDestroy();
	    iListenFrmSecondPls = EFalse;
		iPlsPresetSaved.Close();
		}

	for (TInt i=0; i<playList->Count(); i++)
		{
		CIRPlsPlayListItem* item = playList->ItemAt(i);
		if (item && item->File().Length())
			{
			if(item->Length()==-1)
				{
				HBufC8* fileBuf = HBufC8::NewLC( item->File().Length() );
				fileBuf->Des().Copy( item->File() );
				iURLs.AppendL( fileBuf );
				CleanupStack::Pop( fileBuf );
				HBufC8* titleBuf = HBufC8::NewLC( item->Title().Length() );
				titleBuf->Des().Copy( item->Title() );
				iURLs.AppendL( titleBuf );
				iPlsLength.AppendL(item->Length());
				CleanupStack::Pop( titleBuf );
				}
			}
		}
	for(TInt index=0;index < iURLs.Count()/2 ; index++ )
		{
		iPlsPresetSaved.Append(0);
		}
	CleanupStack::PopAndDestroy( playList );
	ConnectToPlsUrlL();
	IRLOG_DEBUG( "CIRUi::ParsePlsDataL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRUi::ConnectToPlsUrlL()
// Connect to PLS URL
// ---------------------------------------------------------------------------
//
void CIRUi::ConnectToPlsUrlL()
	{
	IRLOG_DEBUG( "CIRUi::ConnectToPlsUrl - Entering" );
	//Instantiate PLS View
	if(!iPlsactivated)
		{
		iPlsView = CIRPlsView::NewL(ApplicationRect());
		AddViewL(iPlsView);
		}
	else
		{
		iPlsLength.Close();
		iPlsView->SecondConstruct();
		}
	if (!iIRSettings->IsFlagTermsAndConditionsL() ||
	!iIRSettings->IsFlagCostWarningL())
		{
		iPlsactivated = ETrue;
		ActivateLocalViewL(KIRCostWarningViewId);
		}
	//Activate the Pls view
	else
		{
		if( iPresentViewID == KIRUpdateWarningViewID )
			{
			iPlsactivated = ETrue;
			}
		else
			{
			iPlsactivated = ETrue;
			ActivateLocalViewL(KIRPlsViewID);
			}
		}
	IRLOG_DEBUG( "CIRUi::ConnectToPlsUrl - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::GetPlsActiavted()
// Returns bool whether T&C or cost warning view are accepted or not
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetPlsActiavted() const
	{
	IRLOG_DEBUG( "CIRUi::GetPlsActiavted" );
	return(iPlsactivated);
	}

// ---------------------------------------------------------------------------
// CIRUi::SetPlsActiavted(..)
// Sets bool if T&C or cost warning view are not accepted
// ---------------------------------------------------------------------------
//
void CIRUi::SetPlsActiavted(TBool aPlsactivated)
	{
	IRLOG_DEBUG( "CIRUi::SetPlsActiavted - Entering" );
	iPlsactivated=aPlsactivated;
	IRLOG_DEBUG( "CIRUi::SetPlsActiavted - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRUi::DisplayInformationL
// Display the Information Note
// ----------------------------------------------------------------------------

void CIRUi::DisplayInformationL(TInt aResourceId, TInt aInt)
	{
	IRLOG_DEBUG( "CIRUi::DisplayInformationL - Entering" );
    RBuf channel;
	HBufC* displayinfo=StringLoader::LoadLC(aResourceId,aInt);
	channel.Create(*displayinfo);
	iDialogNote->ShowConfirmationNoteL( channel,ETrue );
	CleanupStack::PopAndDestroy(displayinfo);
	channel.Close();
	IRLOG_DEBUG( "CIRUi::DisplayInformationL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRUi::DisplayInformationL
// Display the Information Note
// ----------------------------------------------------------------------------

void CIRUi::DisplayInformationL(TInt aResourceId, const TDesC &  aString)
	{
	IRLOG_DEBUG( "CIRUi::DisplayInformationL - Entering" );
    RBuf channel;
	HBufC* displayinfo=StringLoader::LoadLC(aResourceId,aString);
	channel.Create(*displayinfo);
	iDialogNote->ShowConfirmationNoteL( channel,ETrue );
	CleanupStack::PopAndDestroy(displayinfo);
	channel.Close();
	IRLOG_DEBUG( "CIRUi::DisplayInformationL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRUi::GetBgWhenLockState()
// returns the iBgWhenLock lock state.
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetBgWhenLockState()
	{
	IRLOG_DEBUG( "CIRUi::GetBgWhenLockState" );
	return iBgWhenLock;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetCurrentOperatorValue()
// returns the iCurrentOperator value.
// ---------------------------------------------------------------------------
//
TInt CIRUi::GetCurrentOperatorValue()
	{
	IRLOG_DEBUG( "CIRUi::GetCurrentOperatorValue" );
	return iCurrentOperator;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetHomeOperatorValue()
// returns the vallue of iHomeOperator.
// ---------------------------------------------------------------------------
//
TInt CIRUi::GetHomeOperatorValue()
	{
	IRLOG_DEBUG( "CIRUi::GetHomeOperatorValue" );
	return iHomeOperator;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetCallActiveState()
// returns whether iCallActive variable is true/false.
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetCallActiveState()
	{
	IRLOG_DEBUG( "CIRUi::GetCallActiveState" );
	return iCallActive;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetNetworkControllerInstance()
// returns the iNetworkController .
// ---------------------------------------------------------------------------
//
CIRNetworkController* CIRUi::GetNetworkControllerInstance()
	{
	IRLOG_DEBUG( "CIRUi::GetNetworkControllerInstance" );
	return iNetworkController;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetNowPlayingViewInstance()
// returns instance od nowplaying view.
// ---------------------------------------------------------------------------
//
CIRNowPlayingView* CIRUi::GetNowPlayingViewInstance()
	{
	IRLOG_DEBUG( "CIRUi::GetNowPlayingViewInstance" );
	return iNowPlayingView;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetPubSubManagerInstance()
// returns instace of publish subscriber.
// ---------------------------------------------------------------------------
//
CIRPubSub* CIRUi::GetPubSubManagerInstance()
	{
	IRLOG_DEBUG( "CIRUi::GetPubSubManagerInstance" );
	return iPubSubManager;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetCurrentThreadIDValue()
// returns the Current thread ID.
// ---------------------------------------------------------------------------
//
TUint64 CIRUi::GetCurrentThreadIDValue()
	{
	IRLOG_DEBUG( "CIRUi::GetCurrentThreadIDValue" );
	return iCurrentThreadID;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetFavPresetsInstance()
// returns instace of Favourite preset.
// ---------------------------------------------------------------------------
//
CIRFavoritesDb* CIRUi::GetFavPresetsInstance()
	{
	IRLOG_DEBUG( "CIRUi::GetFavPresetsInstance" );
	return iFavPresets;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetNetworkType()
// returns  Network Connection Type.
// ---------------------------------------------------------------------------
//
TIRConnectionType CIRUi::GetNetworkType()
	{
	IRLOG_DEBUG( "CIRUi::GetNetworkType" );
	return iNetWorkType;
	}
// ---------------------------------------------------------------------------
// CIRUi::GetCallConnectedState()
// returns the iCallConnected state.
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetCallConnectedState()
	{
	IRLOG_DEBUG( "CIRUi::GetCallConnectedState" );
	return iCallConnected;
	}
// ---------------------------------------------------------------------------
// CIRUi::SetActivationPendingValue(TUid aUid)
// assign the Id value to iActionPending.
// ---------------------------------------------------------------------------
//
void CIRUi::SetActivationPendingValue(TUid aUid)
	{
	IRLOG_DEBUG( "CIRUi::SetActivationPendingValue - Entering" );
	iActivationPending = aUid;
	IRLOG_DEBUG( "CIRUi::SetActivationPendingValue - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRUi::GetPresentViewID()
// returns the present view ID.
// ---------------------------------------------------------------------------
//
TUid CIRUi::GetPresentViewID()
	{
	IRLOG_DEBUG( "CIRUi::GetPresentViewID" );
	return iPresentViewID;
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleCallActive(..)
// Display the battery icon depending upon the battery level
// ---------------------------------------------------------------------------
//
void  CIRUi::HandleCallActiveL(TIRSystemEventType aEventType)
{
IRLOG_DEBUG( "CIRUi::HandleCallActive - Entering" );
switch (aEventType)
	{
	case EIRCallActivated:
		{
		iCallActive=ETrue;
		if( iNowPlayingWrapper->IsProgress())
			{
			if(!(GetCurrentNetworkType().Compare(KWiFi)==0))
				{
				//if progress bar view we will go to the view from where we
				//launched progress bar
				iNowPlayingWrapper->ProgressBarCancel();

				iNowPlayingWrapper->DestroyWaitDialogL();
				iNowPlayingWrapper->SetProgress(EFalse);
				}
			//current view is progress bar view then previous view has to
			//be activated
			}
		else if(iNowPlayingWrapper->iPlaying )
			{
			//if playing
			//if it was previously playing including rebuffering
			iPlayTerminated = ETrue;
			//the player is expected to stop under all condition when a call comes
			iNowPlayingWrapper->StopPlayer();
			iNowPlayingWrapper->DoPlayingStateChanged( EFalse );
			}
		else
			{
			if ( IsForeground() )
				{
				//if application is in foreground
				iPlayTerminated=EFalse;
				iTriggeringRequired = ETrue;
				}
			else
				{
				iPlayTerminated=EFalse;
				//if application is background
				iTriggeringRequired = EFalse;
				}

			}
		}



	break;
	case EIRCallDeactivated:
		{
		iCallActive=EFalse;
		//This is check is done to tackle immediate cutting of the call
		//sometimes it is found that it will remain in progress bar it self
		//cancellation is not efficent
		if( iNowPlayingWrapper->IsProgress())
			{
			//if progress bar view we will go to the view from where we
			//launched progress bar
			iNowPlayingWrapper->ProgressBarCancel();

			iNowPlayingWrapper->DestroyWaitDialogL();
			}
		else if(!iNowPlayingWrapper->iPlaying && iPlayTerminated)
			{
			if(iNowPlayingView->iContainer)
				{
				iNowPlayingWrapper->SetLaunchNowPlayingView(ETrue);
				}
			else
				{
				iNowPlayingWrapper->SetLaunchNowPlayingView(EFalse);
				}
			iNowPlayingWrapper->DoPlayL();
			}
		else
			{
			if( iTriggeringRequired )
				{
				//bringing to fore ground
				ActivateCurrentView(iPresentViewID);
				}

			}



		}
}
IRLOG_DEBUG( "CIRUi::HandleCallActive - Exiting" );
}

// ---------------------------------------------------------------------------
// CIRUi::HandleAlarmStart()
// Handles when an alarm starts
// ---------------------------------------------------------------------------
//
void CIRUi::HandleAlarmStart()
	{
	IRLOG_DEBUG( "CIRUi::HandleAlarmStart - Entering" );
	if( iNowPlayingWrapper->IsProgress())
		{
		//if progress bar view we will go to the view from where we
		//launched progress bar
		iNowPlayingWrapper->ProgressBarCancel();

		TRAP_IGNORE(iNowPlayingWrapper->DestroyWaitDialogL());
		//current view is progress bar view then previous view has to
		//be activated
		}
	else
		{
		//if playing
		if(iNowPlayingWrapper->iPlaying )
			{


				iAlarmTerminated = ETrue;
			//if it was previously playing including rebuffering
			//iPlayCallTerminated = ETrue
			//the player is expected to stop under all condition when a call comes
			iNowPlayingWrapper->StopPlayer();
			iNowPlayingWrapper->DoPlayingStateChanged( EFalse );
			}
		}

	IRLOG_DEBUG( "CIRUi::HandleAlarmStart - Exiting." );
}

// ---------------------------------------------------------------------------
// CIRUi::HandleAlarmEnd()
// Handles when an alarm ends
// ---------------------------------------------------------------------------
//
void CIRUi::HandleAlarmEnd()
	{
	IRLOG_INFO( "CIRUi::HandleAlarmEnd - Entering" );
	if( iAlarmTerminated )
		{
		iAlarmTerminated = EFalse;

		if(!iNowPlayingWrapper->iPlaying)
			{
      IRRDEBUG2("CIRUi::HandleAlarmEnd - HandleAlarmEnd()",
			 KNullDesC );

		TRAP_IGNORE(iNowPlayingWrapper->DoPlayL();)
			}
		}
	 if( iTriggeringRequired )
		{
		//bringing to fore ground
		ActivateCurrentView(iPresentViewID);
		}
	IRLOG_DEBUG( "CIRUi::HandleAlarmEnd - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleInitialAlarmEvent()
// Handles when an alarm comes initial handling
// ---------------------------------------------------------------------------
//


void CIRUi::HandleInitialAlarmEvent()
	{
	IRLOG_DEBUG( "CIRUi::HandleInitialAlarmEvent - Entering" );
	if( iNowPlayingWrapper->IsProgress())
		{
		//if progress bar view we will go to the view from where we
		//launched progress bar
		iNowPlayingWrapper->ProgressBarCancel();

		TRAP_IGNORE(iNowPlayingWrapper->DestroyWaitDialogL());
		iNowPlayingWrapper->SetProgress(EFalse);
		//current view is progress bar view then previous view has to
		//be activated
		}
	else
		{
		//if playing
		if(iNowPlayingWrapper->iPlaying )
			{

			iAlarmTerminated = ETrue;
			//if it was previously playing including rebuffering
			//iPlayCallTerminated = ETrue
			//the player is expected to stop under all condition when a call comes
			iNowPlayingWrapper->StopPlayer();
			iNowPlayingWrapper->DoPlayingStateChanged( EFalse );
			}
		}

		if ( IsForeground() )
		{
		//if application is in foreground
		iTriggeringRequired = ETrue;
		}
	else
		{
		//if application is background
		iTriggeringRequired = EFalse;
		}
	IRLOG_DEBUG( "CIRUi::HandleInitialAlarmEvent - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRUi::HandleMessageAlertL()
// Handles when a new message comes
// ---------------------------------------------------------------------------
//
void CIRUi::HandleMessageAlertL()
	{
	IRLOG_DEBUG( "CIRUi::HandleMessageAlertL" );
	return;
	}
// ---------------------------------------------------------------------------
// CIRUi::SetPlsIndex()
// Sets the Pls Index
// ---------------------------------------------------------------------------
//
void CIRUi::SetPlsIndex(TInt aPlsIndex)
{
IRLOG_DEBUG( "CIRUi::SetPlsIndex - Entering" );
	iPlsIndex=aPlsIndex;

IRLOG_DEBUG( "CIRUi::SetPlsIndex - Exiting." );

}
// ---------------------------------------------------------------------------
// CIRUi::GetPlsIndex()
// Returns the Pls Index
// ---------------------------------------------------------------------------
//
TInt CIRUi::GetPlsIndex()
{
IRLOG_DEBUG( "CIRUi::GetPlsIndex - Entering" );
IRLOG_DEBUG( "CIRUi::GetPlsIndex - Exiting." );
	return iPlsIndex;
}
// ---------------------------------------------------------------------------
// CIRUi::SetBgLock()
//  Sets the BGround whether it is Locked or Unlocked
// ---------------------------------------------------------------------------
//
void CIRUi::SetBgLock(TInt aBgWhenLock)
{
IRLOG_DEBUG( "CIRUi::SetBgLock - Entering" );
IRLOG_DEBUG( "CIRUi::SetBgLock - Exiting." );
	iBgWhenLock=aBgWhenLock;
}
// ---------------------------------------------------------------------------
// CIRUi::GetPlsIndex()
// Returns whether the Bground is Locked or Unlocked
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetBgLock()
{
IRLOG_DEBUG( "CIRUi::GetBgLock - Entering" );
IRLOG_DEBUG( "CIRUi::GetBgLock - Exiting." );
	return iBgWhenLock;
}
// ---------------------------------------------------------------------------
// CIRUi::SetEditStation()
// 	 Sets the iIsEditStation Flag To determine whether the station
//	 can be Edited
// ---------------------------------------------------------------------------
//
void CIRUi::SetEditStation(TInt aIsEditStation)
{
IRLOG_DEBUG( "CIRUi::SetEditStation - Entering" );
IRLOG_DEBUG( "CIRUi::SetEditStation - Exiting." );
	iIsEditStation=aIsEditStation;
}
// ---------------------------------------------------------------------------
// CIRUi::GetEditStation()
// Returns whether The station can be edited Or not
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetEditStation()
{
IRLOG_DEBUG( "CIRUi::GetEditStation - Entering" );
IRLOG_DEBUG( "CIRUi::GetEditStation - Exiting." );
	return iIsEditStation;
}
// ---------------------------------------------------------------------------
// CIRUi::GetSkinColor()
// CIRUi::GetEditStation()
// Returns the Current Color
// ---------------------------------------------------------------------------
//
TRgb CIRUi::GetSkinColor()
{
	IRLOG_DEBUG( "CIRUi::GetSkinColor - Entering" );


	MAknsSkinInstance * skin=AknsUtils::SkinInstance () ;

    TRgb color;
    TInt skinColor=AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
    				 EAknsCIQsnTextColorsCG6 );
	IRLOG_DEBUG( "CIRUi::GetSkinColor - Exiting" );
    return color;
}
// ---------------------------------------------------------------------------
// CIRUi::ChooseToolBarIcons()
// Returns the ToolbarBirmap Id For The Current Theme
// ---------------------------------------------------------------------------
//
TInt  CIRUi::ChooseToolBarBitmaps(TInt aBlack,TInt aWhite)
{
IRLOG_DEBUG( "CIRUi::ChooseToolBarBitmaps - Entering" );
IRLOG_DEBUG( "CIRUi::ChooseToolBarBitmaps - Exiting" );
 	if(GetSkinColor().Gray2()==0)
    {
    return aBlack;
    }
    else
    {
     return aWhite;
    }
}

// ---------------------------------------------------------------------------
// CIRUi::ChooseToolBarMaskId()
// Returns the Toolbar mask  Id For The Current Theme
// ---------------------------------------------------------------------------
//
TInt CIRUi::ChooseToolBarMaskId(TInt aBlackMask,TInt aWhiteMask)
{

IRLOG_DEBUG( "CIRUi::ChooseToolBarMaskId - Entering" );
IRLOG_DEBUG( "CIRUi::ChooseToolBarMaskId - Exiting" );
	if(GetSkinColor().Gray2()==0)
    {
    return aBlackMask;
    }
    else
    {
    return aWhiteMask;
    }
}
// ---------------------------------------------------------------------------
// CIRUi::SetHistory()
// Set the Flag From All the Views  When Go to nowplaying Is Done
// ---------------------------------------------------------------------------
//
void CIRUi::SetHistory(TBool aHistory)
{
IRLOG_DEBUG( "CIRUi::SetHistory - Entering" );
IRLOG_DEBUG( "CIRUi::SetHistory - Exiting" );
iHistory=aHistory;
}
// ---------------------------------------------------------------------------
// CIRUi::SetHistory()
// Return the iAlarmTerminated Flag
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetAlarm()
{
IRLOG_DEBUG( "CIRUi::GetAlarm - Entering" );
IRLOG_DEBUG( "CIRUi::GetAlarm - Exiting" );
	return iAlarmTerminated;
}

// ---------------------------------------------------------------------------
// CIRUi::SetPlayStateWhileCall
// Sets This Value When Stoping the Player
// While Call is Active
// ---------------------------------------------------------------------------
//
void CIRUi::SetPlayStateWhileCall(TBool aPlayTerminated)
{
 iPlayTerminated = aPlayTerminated;
}

// ---------------------------------------------------------------------------
// CIRUi::GetPlayStateWhileCall
// Returns the Value of Play state
// While Call is Active
// ---------------------------------------------------------------------------
//
TBool CIRUi::GetPlayStateWhileCall()
{
 return iPlayTerminated ;
}


// ---------------------------------------------------------------------------
// IsFlagIfAnyFavL()
// Method to determine if any channel was added to favorites
// ---------------------------------------------------------------------------
//
 TBool CIRUi::IsFlagIfAnyFavL()
	{
	IRLOG_DEBUG( "CIRUi::IsFlagIfAnyFavL - Entering" );
	IRLOG_DEBUG( "CIRUi::IsFlagIfAnyFavL - Exiting" );
	return (iIRSettings->IsFlagIfAnyFavL());
	 }
// ---------------------------------------------------------------------------
// SetMainView()
// Method to set that the now playing view flag is from the Main view
// ---------------------------------------------------------------------------
//
void CIRUi::SetMainView(TBool val)
	{
	iMainViewLaunch = val;
	}


#ifdef FF_IAUPDATE
void CIRUi::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    IRRDEBUG2("CIRUi:: CheckUpdatesComplete - Entered", KNullDesC);
    IRRDEBUG2("CIRUi::CheckUpdatesComplete ErrorCode Received: %d", aErrorCode);
    if ( aErrorCode == KErrNone )
    	{
    	IRRDEBUG2("CIRUi::CheckUpdatesComplete AvailableUpdates: %d", aAvailableUpdates);
    	if ( aAvailableUpdates > 0 )
    	    {
    	    /**
    		 * There were some updates available. Let's ask if the
    		 * user wants to update them
    		 */
            iUpdate->UpdateQuery();
            IRRDEBUG2("CIRUi::CheckUpdatesComplete After Call to UpdateQuery", KNullDesC);
    	    }
    	else
    	    {
            /**
             * No updates available. CIAUpdate object could be deleted
             * already now, since we are not going to call anything else
             * from the API. This will also close down the IAUpdate server
             */
            IRRDEBUG2("CIRUi::CheckUpdatesComplete No Updates are available deleting handles", KNullDesC);
            delete iUpdate;
            iUpdate = NULL;
            delete iParameters;
            iParameters = NULL;
    	    }
    	}
    IRRDEBUG2("CIRUi:: CheckUpdatesComplete - Leaving", KNullDesC);
    }

void CIRUi::UpdateComplete(TInt aErrorCode, CIAUpdateResult* aResult)
    {
    IRRDEBUG2("CIRUi:: UpdateComplete - Entered", KNullDesC);
    IRRDEBUG2("CIRUi:: UpdateComplete - ErrorCode Received: %d", aErrorCode);
    if ( aErrorCode == KErrNone )
    	{
        /**
         * The update process that the user started from IAUpdate UI is
         * now completed. If the client application itself was updated in
         * the update process, this callback is never called, since the
         * client is not running anymore
         */
        TInt successCount = aResult->SuccessCount();
        IRRDEBUG2("CIRUi:: UpdateComplete - After Call to SuccessCount", KNullDesC);
        }
    /**
     * Ownership was transferred, so this must be deleted by the client
     */
    delete aResult;

    /**
     * We do not need the client-server session anymore, let's
     * delete the object to close the session
     */
    delete iUpdate;
    iUpdate = NULL;
    delete iParameters;
    iParameters = NULL;
    IRRDEBUG2("CIRUi:: UpdateComplete - Leaving", KNullDesC);
    }

void CIRUi::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    IRRDEBUG2("CIRUi:: UpdateQueryComplete - Entered", KNullDesC);
    IRRDEBUG2("CIRUi:: UpdateQueryComplete - ErrorCode Received: %d", aErrorCode);
    if ( aErrorCode == KErrNone )
        {
        IRRDEBUG2("CIRUi:: UpdateQueryComplete - aUpdateNow: %d", aUpdateNow);
    	if ( aUpdateNow )
    	    {
            /**
             * User choosed to update now, so let's launch the IAUpdate UI.
             */
            iUpdate->ShowUpdates( *iParameters );
            IRRDEBUG2("CIRUi:: UpdateQueryComplete - After Call to ShowUpdates", KNullDesC);
    	    }
        else
            {
            /**
             * The answer was 'Later'. CIAUpdate object could be deleted
             * already now, since we are not going to call anything else
             * from the API. This will also close down the IAUpdate server
             */
            delete iUpdate;
            iUpdate = NULL;
            delete iParameters;
            iParameters = NULL;
            }
        }
    IRRDEBUG2("CIRUi:: UpdateQueryComplete - Leaving", KNullDesC);
    }
#endif // FF_IAUPDATE


void CIRUi::FindInMusicStoreCounter()
	{
	IRLOG_DEBUG("CIRUi::FindInMusicStoreCounter() - Entered");
	iFindInMusicStoreCount++;
	IRLOG_DEBUG("CIRUi::FindInMusicStoreCounter() - Leaving");
	}

void CIRUi::GetFindInMusicStoreCount(TInt& aFindInMStoreCount)
	{
	IRLOG_DEBUG("CIRUi::GetFindInMusicStoreCount() - Entered");
	aFindInMStoreCount = iFindInMusicStoreCount;
	IRLOG_DEBUG("CIRUi::GetFindInMusicStoreCount() - Leaving");
	}

TBool CIRUi::IsOfflineProfile()
	{
	IRLOG_DEBUG("CIRUi::IsOfflineProfile() - Entered");
	IRLOG_DEBUG("CIRUi::IsOfflineProfile() - Leaving");
	return iIsOffline;
	}

// ---------------------------------------------------------------------------
// HandleProfileActivatedL()
// Method called whenever there is a change in the profile
// ---------------------------------------------------------------------------
//
void CIRUi::HandleProfileActivatedL(TInt aProfileId)
	{
	IRRDEBUG2("CIRUi::HandleProfileActivatedL - Entering aProfileId = %d",aProfileId );
    const TInt KProfileId=5;
	if(iProfileId==aProfileId)
		{
		return;
		}
	if(aProfileId==KProfileId)
		{
		if ( iIsdsWrapper->GetISDSInstance() )
			{
			if(iIsdsWrapper->GetListenRequest()||(iNowPlayingWrapper->iSyncPreset))
				{
				iNowPlayingWrapper->DestroyWaitDialogL();
				}
			iNowPlayingWrapper->iSyncPreset=EFalse;
			iIsdsWrapper->DestroyWaitDialogL();
			iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
			iIsdsWrapper->GetISDSInstance()->ReleaseResources();
			}
		if(iNowPlayingWrapper->iPlaying && (!iNowPlayingWrapper->IsProgress()))
			{
			iNowPlayingWrapper->StopNow();
			if(ActiveView() == KIRMainChoiceViewID)
				{
				iMainView->GetMainContainer()->UpdateAllL();
				}

			}
		else
			{
			if(iNowPlayingWrapper->GetConnectToServerState() && !iNowPlayingWrapper->IsProgress())
				{
				iNowPlayingWrapper->CancelNetworkRequestL();
				iNowPlayingWrapper->DestroyWaitDialogL();
				}
			//player is stopped
			if( iNowPlayingWrapper->IsProgress() )
				{
				//if in progress bar view, progress bar is cancelled
				iNowPlayingWrapper->ProgressBarCancel();
				iNowPlayingWrapper->DestroyWaitDialogL();
				}
			}

		//network got disconnected
		/*iNetworkIndicatorTimer->Cancel();
		//connection status is reset in network controller
		iNetworkController->ResetConnectionStatus();
		if ( iIsdsWrapper->GetISDSInstance() )
		{
		iIsdsWrapper->DestroyWaitDialogL();
		iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
		iIsdsWrapper->GetISDSInstance()->ReleaseResources();
		}
		//player is stopped
		if( iNowPlayingWrapper->IsProgress() )
		{
		//if in progress bar view, progress bar is cancelled
		iNowPlayingWrapper->ProgressBarCancel();

		iNowPlayingWrapper->DestroyWaitDialogL();
		}
		else
		{
		if(iNowPlayingWrapper->iPlaying)
		{
		iNowPlayingWrapper->StopNow();
		if(ActiveView() == KIRMainChoiceViewID)
		{
		iMainView->GetMainContainer()->UpdateAllL();
		}
		}
		}
		*/
		iProfileId=aProfileId;
		}
	else
		{
		iProfileId=aProfileId;
		}
	IRRDEBUG2("CIRUi::HandleProfileActivatedL - Exiting",KNullDesC );

}
RArray<TInt>& CIRUi::GetPlsSaved()
	{
	return iPlsPresetSaved;
	}
void CIRUi::SetListenFromPls(TBool aValue)
	{
	iListenFromPls = aValue ;
	}
TBool CIRUi::GetListenFromPls()
	{
	return iListenFromPls ;
	}
void CIRUi::SetListenFromSecondPls(TBool aValue)
	{
	iListenFrmSecondPls = aValue;
	}
TBool CIRUi::GetListenFromSecondPls()
	{
	return iListenFrmSecondPls ;
	}

// ---------------------------------------------------------------------------
// HandleProfileActivationNotificationError()
// Method called whenever there is an error while change in the profile
// ---------------------------------------------------------------------------
//
void CIRUi:: HandleProfileActivationNotificationError (TInt)
{

}


// ---------------------------------------------------------------------------
// CIRUi::CheckAndNotifyLowDiskSpaceL
// ---------------------------------------------------------------------------
//
TBool CIRUi::CheckAndNotifyLowDiskSpaceL() const
    {
	IRRDEBUG2("CIRUi::CheckAndNotifyLowDiskSpaceL - Entering", KNullDesC);
    TBool isLow = iSystemEventCollector->IsDiskSpaceBelowCriticalLevel();
    if ( isLow )
		{
		IRRDEBUG2("CIRUi::CheckAndNotifyLowDiskSpaceL - Phone LOW MEMORY detected", KNullDesC);
		iDialogLauncher->ShowInformationNoteL( R_IRAPP_MEMLO_RAM_OUT_OF_MEM, ETrue);
		}

	IRRDEBUG2("CIRUi::CheckAndNotifyLowDiskSpaceL - Exiting", KNullDesC);
    return isLow;
    }


// ---------------------------------------------------------
// CIRUi::HandleSystemEventL
// from base class MIRSystemEventObserver
// ---------------------------------------------------------
//
void CIRUi::HandleSystemEventL(TIRSystemEventType aEventType)
	{
	IRLOG_DEBUG("CIRUi::HandleSystemEventL - Entering");

	switch (aEventType)
		{
		case EIRHeadsetConnected:
            {
            StopDisplayingMenuBar();
            break;
            }
        case EIRHeadsetDisconnected:
            {
            StopDisplayingMenuBar();
            break;
            }
		case EIRLowDiskSpace:
			{
			IRRDEBUG2("CIRUi::HandleSystemEventL - EIRLowDiskSpace reported", KNullDesC);

			// Cancel pending logo requests
			iIsdsWrapper->GetISDSInstance()->GetDataProvider()->GetHttpDataProvider()->
			GetLogoDownloadEngine()->CancelTransaction();

			// Cancel pending ISDS requests & Preset request for Listen
            if ( iIsdsWrapper->GetISDSInstance() )
                {
                if(iIsdsWrapper->GetListenRequest())
			  		{
					iNowPlayingWrapper->DestroyWaitDialogL();
			  		}
                iIsdsWrapper->DestroyWaitDialogL();
       			iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
 			    iIsdsWrapper->GetISDSInstance()->ReleaseResources();
                }
        	// Stop playing
        	if(iNowPlayingWrapper->iPlaying && !iNowPlayingWrapper->IsProgress() )
				{
				iNowPlayingWrapper->StopNow();
				if(ActiveView() == KIRMainChoiceViewID)
					{
					iMainView->GetMainContainer()->UpdateAllL();
					}
				}
			// Cancel ConnectToServer requests
	        else if(iNowPlayingWrapper->GetConnectToServerState()
	        		&& !iNowPlayingWrapper->IsProgress())
				{
				iNowPlayingWrapper->CancelNetworkRequestL();
				iNowPlayingWrapper->DestroyWaitDialogL();
				}
			// Cancel Listen streaming requests
			else if( iNowPlayingWrapper->IsProgress() )
				{
				//if in progress bar view, progress bar is cancelled
				iNowPlayingWrapper->ProgressBarCancel();
				iNowPlayingWrapper->DestroyWaitDialogL();
				}

			// Show Low Memory warning
			iDialogLauncher->ShowInformationNoteL( R_IRAPP_MEMLO_RAM_OUT_OF_MEM, ETrue);
			}
		break;
		default:
			{
			}
		break;
		}
	IRLOG_DEBUG( "CIRUi::HandleSystemEventL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRUi::DetermineCurrentProfile
// Used to determine the current profile
// ---------------------------------------------------------------------------
//
TInt CIRUi::DetermineCurrentProfile() const
	{
	IRLOG_DEBUG( "CIRUi::DetermineCurrentProfile" );
	TInt value = KErrNone;

	TRAPD(err,
	CRepository* cRepositoryHandle = CRepository::NewLC(KCRUidProfileEngine );
	cRepositoryHandle->Get(KProEngActiveProfile,value);
	CleanupStack::PopAndDestroy(cRepositoryHandle);	)
	if(err != KErrNone)
		{
		value = err;
		}

	IRLOG_DEBUG( "CIRUi::DetermineCurrentProfile - Exiting." );
	return value;
	}







//ADDED BY SNIGDHA FOR HOMESCREEN




// ---------------------------------------------------------------------------
// CIRUi::ProcessActionHandlerCommandParameter
// ---------------------------------------------------------------------------
void CIRUi::ProcessActionHandlerCommandParameter( const TDesC8& aTail )
    {
   IRRDEBUG2("CIRUi::ProcessActionHandlerCommandParameter entering", KNullDesC);

    if ( aTail.Compare( KIRCommandLineActionHandlerCommandStartNowPlayingView() ) == 0 )
        {
        iStartUpInNowPlayingView = ETrue;
        }

   IRRDEBUG2("CIRUi::ProcessActionHandlerCommandParameter exiting", KNullDesC);

    }

// ---------------------------------------------------------------------------
// CIRUi::ProcessCommandTailL
// ---------------------------------------------------------------------------
void CIRUi::ProcessCommandTailL( const TDesC8& aTail )
    {

   IRRDEBUG2("CIRUi::ProcessCommandTailL entering", KNullDesC);

    TLex8 tailLex( aTail );
    TChar peek = tailLex.Peek();

    if ( peek != KIRCommandLineOptionPrefix && peek != 0) // old CLI: expect only preset ID.
        {
        }
    else if ( peek != 0 )
        {
        TPtrC8 option = tailLex.NextToken();

        // Get option
        if ( option.Length() > 0 )
            {
            tailLex.SkipSpaceAndMark();
            TPtrC8 rest = tailLex.Remainder();

            if ( option.Compare( KIRCommandLineActionHandler() )  == 0 )
                {
                ProcessActionHandlerCommandParameter( rest );
                }
            else
                {
                User::Leave( KErrArgument );
                }
            }
        }
    else
        {
        // NOP
        }

       IRRDEBUG2("CIRUi::ProcessCommandTailL exiting", KNullDesC);

    }

// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Processes shell commands.
// ---------------------------------------------------------------------------
//
TBool CIRUi::ProcessCommandParametersL( TApaCommand aCommand, TFileName&
        aDocumentName, const TDesC8& aTail )
	{
	IRRDEBUG2("CIRUi::ProcessCommandParametersL entering", KNullDesC);

	TBool ret = CAknViewAppUi::ProcessCommandParametersL( aCommand,
	aDocumentName, aTail );


	ProcessCommandTailL( aTail );
	IRRDEBUG2("CIRUi::ProcessCommandParametersL exiting", KNullDesC);

	return ret;
	}


// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Handles window server messages.
// ---------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CIRUi::HandleMessageL(
        TUint32 aClientHandleOfTargetWindowGroup, TUid aMessageUid,
        const TDesC8& aMessageParameters )
    {
    IRRDEBUG2("CIRUi::HandleMessageL Entering", KNullDesC);

    TMessageResponse resp = CAknViewAppUi::HandleMessageL(
        aClientHandleOfTargetWindowGroup, aMessageUid, aMessageParameters );

    if ( resp == EMessageNotHandled && aMessageUid == KUidActiveInternetRadioApp)
    	{
        ProcessCommandTailL( aMessageParameters );
        resp = EMessageHandled;

     	IRRDEBUG2("CIRUi::HandleMessageL iStartUpInNowPlayingView = %d", iStartUpInNowPlayingView);

        // Activate now playing view to now playing or last played channel
        if ( iStartUpInNowPlayingView )
			{
			IRRDEBUG2("CIRUi::HandleMessageL ActivateLocalViewL", KNullDesC);

			ActivateLocalViewL( KIRNowPlayingViewID );

			iStartUpInNowPlayingView = EFalse;
			}
    	}
    else
        {
        // NOP
        }
    IRRDEBUG2("CIRUi::HandleMessageL exiting", KNullDesC);

    return resp;
    }

// ---------------------------------------------------------------------------
// CVRAppUi::BringToForeground
// ---------------------------------------------------------------------------
//
void CIRUi::BringToForeground() const
    {
    IRLOG_DEBUG( " *** S60VisualRadio -- CVRAppUi::BringToForeground" );

    TApaTask task( iEikonEnv->WsSession() );
    task.SetWgId( iEikonEnv->RootWin().Identifier() );
    task.BringToForeground();
    }
// ---------------------------------------------------------------------------
// CVRAppUi::SendToBackground
// ---------------------------------------------------------------------------
//
void CIRUi::SendToBackground() const
    {
    IRLOG_DEBUG( " *** S60VisualRadio -- CVRAppUi::SendToBackground" );
    TApaTask task( iEikonEnv->WsSession() );
    task.SetWgId( iEikonEnv->RootWin().Identifier() );
    task.SendToBackground();
    }
// ---------------------------------------------------------------------------
// CIRUi::PlayerStateChangedL
// Changes the state of the player
// ---------------------------------------------------------------------------
//

void CIRUi::PlayerStateChangedL(TIRStopPlayState /*aState*/)
	{
	IRRDEBUG2("CIRUi::PlayerStateChangedL entering",KNullDesC);


	iNowPlayingWrapper->PlayAndStopL();
	IRRDEBUG2("CIRUi::PlayerStateChangedL exitng",KNullDesC);

	}


// ---------------------------------------------------------------------------
// From class MCoeViewActivationObserver.
// Notification that a view is being activated.
// ---------------------------------------------------------------------------
//
void CIRUi::HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId,
                                     const TVwsViewId& /*aViewIdToBeDeactivated*/ )
    {
    IRLOG_DEBUG( " *** S60VisualRadio -- CIRUi::HandleViewActivation" );
    if ( aNewlyActivatedViewId.iAppUid == TUid::Uid( KUIDIRAPP ) )
        {
#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
        TRAP_IGNORE( iBsWrapper->HandleViewActivationEventL( aNewlyActivatedViewId, ETrue ) )
#endif
        }
    }



