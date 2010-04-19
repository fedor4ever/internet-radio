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


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 19, Fri Sep 19 13:00:00 2008 by Rohit
*  Ref:
*  Added ASF detection and Integrated CIRAsfPlayer in Now Playing scenario
*
*  </ccm_history>
* ============================================================================
*/

#include <internetradio.rsg>
#include <stringloader.h>
#include <aknwaitdialog.h> 

#include "irnowplayingwrapper.h"
#include "irmainview.h"
#include "irdebug.h"
#include "irstreamsource.h"
#include "irmediaclient.h"
#include "irctrlcmdobserver.h"
#include "irsessionlogger.h"
#include "irsettings.h"
#include "irisdswrapper.h"
#include "irmetadata.h"
#include "irisdspreset.h"
#include "irautomatedbitrateselection.h" // KHighQuality defined
#include "irdialoglauncher.h"
#include "ircommon.h"
#include "irui.h"
#include "irpubsub.h"
#include "irstreamsourceerrors.h"//KIRStreamSourceTimeOut defined
#include "irnowplayingview.h"
#include "ir.hrh"
#include "irnetworkcontroller.h"
#include "irhistory.h"
#include "irplsview.h"
#include "irnetworkcontrollerobserver.h"
#include "irasfplayer.h"                // CIRAsfPlayer				
#include <mmfcontrollerpluginresolver.h> //to get the supported MIME types
#include "irmainview.h"
#include "irmaincontainer.h"
#include "iractivenetworkobserver.h"
#include "irreportsettings.h"

const TInt KTemporaryUrlMaxLength = 256;
const TInt KUrlbufMaxLength = 256;
//No of volume level is 10 + 1 level 1 level is mute
const TInt KNOVOLUMELEVELS =10;


_LIT(KConst," ");
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper* CIRNowPlayingWrapper::NewL()
// function to return an instance of CIRNowPlayingWrapper 
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRNowPlayingWrapper* CIRNowPlayingWrapper::NewL(CIRUi& aAppUi,CIRSettings& aIRSettings,
					CIRIsdsWrapper& aIsdsWrapper)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::NewL - Entering" );
	CIRNowPlayingWrapper* self = CIRNowPlayingWrapper::NewLC(aAppUi,aIRSettings,aIsdsWrapper);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRNowPlayingWrapper::NewL - Exiting." );
	return self;
	}
        
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper* CIRNowPlayingWrapper::NewLC()
// Two-phased constructor.
// Create a CIRNowPlayingWrapper object, which will draw itself
// to aRect.
// ---------------------------------------------------------------------------
//
CIRNowPlayingWrapper* CIRNowPlayingWrapper::NewLC(CIRUi& aAppUi,CIRSettings& aIRSettings,
			CIRIsdsWrapper& aIsdsWrapper)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::NewLC - Entering" );
	CIRNowPlayingWrapper* self = new (ELeave) CIRNowPlayingWrapper(aAppUi,aIRSettings,aIsdsWrapper);
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRNowPlayingWrapper::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::~CIRNowPlayingWrapper()
// Destructor.
// ---------------------------------------------------------------------------
//
CIRNowPlayingWrapper:: ~CIRNowPlayingWrapper()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::~CIRNowPlayingWrapper - Entering" );
	if(iNetwork)
		{
		delete iNetwork;
		iNetwork = NULL;	
		}
	iAppUi.GetNowPlayingViewInstance()->DeleteSteroEffect(EFalse);
	if(iPlayer)
		{
		delete iPlayer;
		iPlayer = NULL;
		}
	if(iChannel)
		{
		delete iChannel;
		iChannel = NULL;	
		}
	if( iReportGenerator )
		{
	    iReportGenerator->Close();
		}
	if(iMetaData)
		{
		delete iMetaData;
		iMetaData = NULL;	
		}
	if(iNowPlayingPreset)
		{
		delete iNowPlayingPreset;
		iNowPlayingPreset = NULL;	
		}
	if(iEditStationPreset)
		{
		delete iEditStationPreset;
		iEditStationPreset = NULL;	
		}
	if(iWaitDialog)
		{
		delete iWaitDialog;
		iWaitDialog = NULL;	
		}
	if(iURLArray)
		{
		delete iURLArray;
		iURLArray = NULL;
		}
    iAppUi.iNetworkController->DeleteActiveNetworkObserver(*this);

	if( iAsfPlayer )
		{
		delete iAsfPlayer;
		iAsfPlayer = NULL;
		iAsfStreaming = EFalse;
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::~CIRNowPlayingWrapper - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRNowPlayingWrapper::ConstructL()
// Constructs the 
// ---------------------------------------------------------------------------
//
 
void CIRNowPlayingWrapper::ConstructL()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ConstructL - Entering" );
	//Instantiate StreamSource
	iNetwork = CIRStreamSource::NewL(*this);

	//command channel object creation it is through this component
	//media engine interacts with UI
	iChannel = CIRCtrlCmdObserver::NewL(this,iNetwork);

	//media cient object creation
	iPlayer = CIRMediaClient::NewL(iChannel);
	
	//Session Log
	iReportGenerator = CIRReportGenerator::OpenL();
	
    iMetaData = CIRMetaData::NewL();

	//Instantiate ISDS Preset
	iNowPlayingPreset=CIRIsdsPreset::NewL();
	
	iEditStationPreset = CIRIsdsPreset::NewL();
	
	//currently not playing
	iPlaying = EFalse;
	
	//session log should not be terminated on loading cancel
	iTerminatedLogOnCancel = EFalse;
	
	// when skipping channels used for audio fadein fade out
	iAudioFadein = EFalse;
	
	// currently set to false since no session created
	iSessionShouldNotEnd = EFalse;

    // set when connect to channel server
	iConnectToServer = EFalse;
    
	// TO DO: need to remove this variable
	// set when stop is done from nowplayimng view
	iPlayandStop = EFalse;
	
	// Checks whether network connection has done
	iIsConnected = EFalse;

	// Waitdialog required for for showing buffering
	iWaitDialog = NULL;
	
	// The multiple url's of a channel are stored
	iURLArray = NULL;
	
	
	//Used For Now Playing View
	
	iDisplayMetaData=EFalse;

	iMarquee = EFalse;
	
	// The variable is set when player is not supported
    iPlayBack=ETrue;
    
    // The variable is set when called for cancel network request
    iCancelNetworkRequest=EFalse;
    
    // The variable is set when buffering waitdialog is deleted
    iDialogCanceled=EFalse;

    // Registering for network controller to get the notification
    // when network is lost or network connection is made
    iAppUi.iNetworkController->RegisterActiveNetworkObserverL(*this);
    
    // Required when Switching to network
    iNotify=EFalse;

    // Asf streaming
    iAsfPlayer = CIRAsfPlayer::NewL( *this, *this );
    
    // Valuse set when a session is stared
    iSessionStartedFlag = EFalse;
    
    // This variable is set when listen from Isds is done
    iListenFromIsds=EFalse;
    
    // The variable checks whether the nowplaying view is active
    iLaunchNowplaying=EFalse;
    
    // The variable is required when user does buffering watdialog
    iBufferingRequestCancelled=EFalse;
    
    iPlaying=EFalse;
    iQualityRequired=-1;
    IRLOG_DEBUG( "CIRNowPlayingWrapper::ConstructL - Exiting" );
	}
		
// ---------------------------------------------------------------------------
// void CIRNowPlayingWrapper::CIRNowPlayingWrapper()
// Default Constructor
// ---------------------------------------------------------------------------
//
CIRNowPlayingWrapper::CIRNowPlayingWrapper(CIRUi& aAppUi,CIRSettings& aIRSettings,
			CIRIsdsWrapper& aIsdsWrapper):iIRSettings(aIRSettings),
              iIsdsWrapper(aIsdsWrapper),iAppUi(aAppUi),
              iAsfPlayer(NULL), iAsfStreaming(EFalse)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::CIRNowPlayingWrapper - Entering" );
	 iSessionStart=EFalse;
	 iChannelConnect=EFalse;
	 IRLOG_DEBUG( "CIRNowPlayingWrapper::NewLC - Exiting" );
	}

// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::OpenComplete()
// Called when connected to the channel server To implement the fill of buffer
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::OpenComplete()
	{
	//once a buffer is completed this api is called as
	//callback
	IRLOG_DEBUG("CIRNowPlayingWrapper::OpenComplete - Entering" );

	/**** ASF streaming changes : begin ****/
	if(!iAsfStreaming)
		{
		iPlayer->BufferFilled();
		}
	/**** ASF streaming changes : begin ****/

    IRLOG_DEBUG("CIRNowPlayingWrapper::OpenComplete - Exiting." );
	}
// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::ErrorConnecting()
// Called when there is an error when connecting to the channel server  @Param aErrorCode Indicates the type of error as described in irstreamsourceerrors.h---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ErrorConnecting(TInt aErrorCode)
	{
	IRLOG_DEBUG2( "CIRNowPlayingWrapper::ErrorConnecting(aErrorCode=%d)", aErrorCode );
	iChannelConnect = EFalse;
	// this method fails only if display of info note fails which can be
	// ignored so we use TRAP_IGNORE
		
	TRAP_IGNORE(DoErrorConnectingL(aErrorCode))

	IRLOG_DEBUG( "CIRNowPlayingWrapper::ErrorConnecting - Exiting." );
	}
// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::PlayNow()
// Called to indicate to the player to start playing
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::PlayNow()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::PlayNow - Entering" );

	DoPlayingStateChanged( ETrue );

	//sets the configuration data
	TConfig config;
	//by default takes the volume from data base
	//that is when the player is not currently playing
	config.iVolume = iIRSettings.GetVolumeSetting();
	//if current playing we have to do audio fade in.
	//So we set the volume as zero
	if(iAudioFadein)
		{
		config.iVolume = 0;
		}
	TRAPD(err,iPlayer->StartNewPlayerL(config,iNetwork->ContentTypeL()))
	if( err )
		{
		IRLOG_ERROR2( "CIRNowPlayingWrapper::PlayNow - Starting new player failed (err=%d)", err );
		//if error occurs in occurs, displaying
		TRAP_IGNORE(
			DisplayErrorNoteL(R_IRAPP_IRGENERAL_ERROR);
			)
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::PlayNow - Exiting." );
	}
// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::Play()
// Triggers the play after reconnecting to the channel
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::Play()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::Play - Entering" );
	//play is called
	iUrl.Copy(iLastPlayedUrl);
	TInt err = iPlayer->Play();

	if( err )
		{
		IRLOG_ERROR2( "CIRNowPlayingWrapper::Play - Play failed (err=%d), starting new player.", err );
		TConfig config;
		config.iVolume = iIRSettings.GetVolumeSetting();
		TRAPD(err2,iPlayer->StartNewPlayerL(config,iNetwork->ContentTypeL()))
		if( err2 )
			{
			IRLOG_ERROR2( "CIRNowPlayingWrapper::Play - Starting new player failed (err=%d)", err2 );
			//if error occurs in occurs, displaying
			TRAP_IGNORE(
				DisplayErrorNoteL(R_IRAPP_IRGENERAL_ERROR);
				)
			}
		}
	IRLOG_DEBUG("CIRNowPlayingWrapper::Play - Exiting." );
	}
// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::StopBuffering()
// Called to indicate to the player to stop buffering
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::StopBuffering()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::StopBuffering - Entering" );

	/**** ASF streaming changes : begin ****/
	if(!iAsfStreaming)
		iPlayer->StopMediaBuffering();
	/**** ASF streaming changes : end ****/

	IRLOG_INFO( "CIRNowPlayingWrapper::StopBuffering - Exiting. " );
	}
// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::ConnectionEstablished(TInt aFeedValue)
// Called to indicate that the connection to channel server is sucessful
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ConnectionEstablished()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::ConnectionEstablished - Entering." );
	// Reset channel connection request
	TInt bitRate;
	//To Do:Commented code for accp channel
    /*if( iReConnecting )
		{
		 iProgress=EFalse;
		}
	// The control only for the first initial buffering
	// not while reconnecting to same channel i.e, while play & stop.
		// TO Do: here check is dine only for aacp player
		// All content types should be checked
		
		_LIT(KAudiContent,"aacp");//aacp
		RBuf content;
		TRAP_IGNORE(content.Create(iNetwork->ContentTypeL().Length());)
		TRAP_IGNORE(content.Copy(iNetwork->ContentTypeL());)
		TInt MimeType1= content.Find(KAudiContent);
		if(MimeType1!=KErrNotFound)
			{
			TInt returnValue=0;
			TRAP_IGNORE(returnValue=GetSupportedMimeTypeL());
			if(returnValue==KErrNotFound)
				{
				iCancelNetworkRequest = ETrue;
				iURLCount++;
				if(iURLCount>=iURLArray->MdcaCount())	
					{
					MCtrlCommand(EError,KErrNotSupported);
					content.Close();
					return;
					}
				iNetwork->CancelRequest();
				iDialogCanceled=EFalse;
				DoPlayingStateChanged( EFalse );
				if(iURLArray->MdcaCount())
					{
					//TRAP_IGNORE(CreateWaitDialogL())
					TBuf8<KUrlbufMaxLength> urlbuf;
					urlbuf.Copy(iURLArray->MdcaPoint(iURLCount));
					//connecting to a new channel so multiple URL selection enabled
					//that if current URL doesn't connect we will try to connect to
					//next URL in iURLArray
					iMultipleURLSelect = ETrue;
					iUnconnectedUrl.Copy(urlbuf);
					TRAPD(error,DoConnectToServerL(urlbuf));
					if( error )
						{
						IRLOG_ERROR2( "CIRNowPlayingWrapper::ConnectToChannel - DoConnectToServer failed (error=%d)", error );
						TRAP_IGNORE( DisplayErrorNoteL(R_IRAPP_ERROR_CONNECTION) )
						}
					content.Close();
					return;
					}
				}
			}
		content.Close();*/
	 // here since the connection got established the value set to True
	iChannelConnected=ETrue;
	iLastConnectedPresetId = iNowPlayingPreset->GetId();
	iLastConnectedPresetUniqId = iNowPlayingPreset->UniqId();
	iChannelConnect = EFalse;
	//connecting to a new channel so multiple URL selection disabled
	//if we are able to connect to a particular URL we have to
	//disable trying to connect to next URLs since error conditions after this
	//are to handled differently
	iMultipleURLSelect = EFalse;
	if ( iLastConnectedUrl != iUrl )
		{
		IRLOG_INFO( "CIRNowPlayingWrapper::ConnectionEstablished - Station URL changed, clearing history list and current meta data information" );
		TRAP_IGNORE( ClearMetaDataL() )
		}

	iLastConnectedUrl.Copy(iUrl);
    iReportGenerator->MarkSessionStart();
    if( iSessionStart )
        {
        // Next url to it is connected is logged.
        iReportGenerator->LogServerResult( iUrl, EIRConnected );
        }

	//set the sync state as EPsConnectionSuccessful on a successful connection
	if( !iReConnecting )
		{
		iProgress = ETrue;
		//session end is marked
		//iReportGenerator->MarkSessionStart()
		TRAP_IGNORE(
		TFileName pathtopost=iAppUi.GetPrivatePath();
		pathtopost.Append(KGZipLogFileName);
		//if this api leaves it is ignored
		//we need not do any action if post fails
		//the application should be left unaffected
		if(!iNotify)
		iIsdsWrapper.GetISDSInstance()->IRISDSPostL(pathtopost);
		)
		//starts audio fade out for the previous channel
		/**** ASF streaming changes : begin ****/
		if(!iAsfStreaming)
		iChannel->DoAudioFadeOut();
		/**** ASF streaming changes : end ****/

		/*if(iSessionStart)
			{
			//next url to it is connected is logged
			iReportGenerator->LogServerResult(iUrl,EIRConnected);
			}*/
		}
	else
		{
		// Value set to false becoz the the user is trying 
		 // to connect to same channel so need to update progress
		iProgress = EFalse;
		}
	// This part of code is needed In some cases 
	// When channel is playing and In intial buffer content the bitrate value is Zero
	// So the bitate vaule is taken from the Preset Information and Subscibing 
	iPlayBack=ETrue;
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSBitrate, bitRate );
	if(bitRate==0)
		{

		TRAP_IGNORE(bitRate= iNowPlayingPreset->GetChannelBitrateL(iURLCount);)
		RProperty::Set ( KUidActiveInternetRadioApp, KIRPSBitrate, bitRate );
		}
	if((iAppUi.GetNowPlayingViewInstance()->iContainer))
		{
		iAppUi.GetNowPlayingViewInstance()->SetBitrate(EFalse);
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ConnectionEstablished - Exiting." );
	}

// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::UpdateProgress()
// Called to provide teh buffer values @param aFeedValue used to pump the buffering value
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::UpdateProgress(TInt aFeedValue)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::UpdateProgress - Entering" );
	if(iProgress)
		{
		if(!iIsdsWrapper.ActiveWaitDialog())	
			{
			RBuf Feed;
			HBufC* percent=NULL;
			TRAP_IGNORE(percent= StringLoader::LoadL(R_IRAPP_LOADING_BUFFERING_PERCENTAGE, aFeedValue);)
			Feed.Create(percent->Length());
			Feed.Copy(*percent);
			delete percent;
			if(!iWaitDialog)	
				{
				iWaitDialog =  new CAknWaitDialog( NULL,ETrue);
				if(!iWaitDialog)
					{
					return;
					}
				else
					{
					iWaitDialog->SetCallback(this);
					TRAP_IGNORE(iWaitDialog->ExecuteLD(R_IRAPP_PROGRESS_DIALOG);)
					}
				}
			if(iWaitDialog)	
				{
				TRAPD(err,iWaitDialog->SetTextL(Feed);)
				if(err!=KErrNone)
					{
					return;
					}
				}
			iBufferigSteamText=ETrue;
			Feed.Close();
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::UpdateProgress - Exiting." );
	}
// ---------------------------------------------------------------------------
// MCrtlCmdCommand::HandleMetaDataReceivedL()
// Invoked when meta data is received. @param  aMetaData The meta data that was received.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::HandleMetaDataReceivedL( const CIRMetaData& aMetaData )
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::HandleMetaDataReceivedL  - Entering" );
	TBuf<KMAXMETABUFLENGTH> artist = aMetaData.Artist();
	TBuf<KMAXMETABUFLENGTH> song = aMetaData.Song();
	RemoveWhiteSpace( artist );
	RemoveWhiteSpace( song );
	iMetaData->SetArtistL( artist );
	iMetaData->SetSongL( song );

	if(iMetaData->Artist().Length() || iMetaData->Song().Length())
		{
		if ((iAppUi.GetNowPlayingViewInstance()->iContainer ))
			{
			iMarquee=ETrue;	
			iDisplayMetaData=EFalse;
			iAppUi.GetNowPlayingViewInstance()->DisplaySongArtistNameL( iMetaData->Song(),
			iMetaData->Artist() );
			}
	// the history should get updated only when 100 persent buffering is done
		if(!iWaitDialog||iAppUi.iIsdsWrapper->GetListenRequest())
			{
			PublishDataL();
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::HandleMetaDataReceivedL - Exiting." );
	}
// ---------------------------------------------------------------------------
// MCrtlCmdCommand::MCtrlCommand()
// Function is pure virtual function which used to pass commands @param TIRControlCommmand represents the commands,which determines what action to be taken
// @param aValue represents any integer value if any, like buffering percentage
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::MCtrlCommand( TIRControlCommmand aCommand, TInt aValue )
	{
	IRLOG_INFO3( "CIRNowPlayingWrapper::MCtrlCommand (aCommand=%d, aValue=%d)", aCommand, aValue );
	switch (aCommand)
		{
		case EBufferFillStart:
			{
			//TO Do:: This is needed for future Use

			break;
			}

		case EBufferFillStop:
			{
			TRAP_IGNORE(DestroyWaitDialogL());
			DoPlayingStateChanged( ETrue );
			iBufferingRequestCancelled=EFalse;
			iDialogCanceled=EFalse;
			iPlayBack=ETrue;
			if(iNowPlayingPreset->GetChannelType())
				{
				iUinqid=iNowPlayingPreset->UniqId();
				}
			if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
				{
				TRAP_IGNORE(iAppUi.iMainView->GetMainContainer()->UpdateAllL();)
				}
			// publish the play status
			RProperty::Set( KUidActiveInternetRadioApp,KIRPSMultiplePlay,
			iAppUi.GetCurrentThreadIDValue());
			if( iSessionStart )
				{
				iReportGenerator->HandleStopEvents(ETrue);
				}
			if( iProgress )
				{
				//since we are able to connect to the URL the url is current streaming URL
				//implemented for the progress bar view
				iLastPlayedUrl.Copy(iLastConnectedUrl);
				//To publish the metadata
				TRAP_IGNORE(PublishDataL();)
				if(!iLaunchNowplaying && iNotify)
					{	
					IRRDEBUG2("CIRNowPlayingWrapper::EBufferFillStop - iNotify", KNullDesC );
					iProgress = EFalse;
					iLaunchNowplaying=EFalse;
					break;
					}
				iNotify=EFalse;
				//Activate NowPlayingView
				if(!iAppUi.GetNowPlayingViewInstance()->iContainer)
				{
				if(iContextViews==EHistoryView)
					{
					if(iAppUi.IsForeground()|| iAppUi.GetBgLock())
						{
						TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
						KHistoryLaunch,KIRNowPlayingViewCustomMessagePlayAfterActivation));
						iAppUi.SetActivationPendingValue(KNullUid);	
						}
					else
						{
						iAppUi.SetActivationPendingValue(KIRNowPlayingViewID);
						}
					}
				else if(iContextViews==EFav||iContextViews==ELastplayed)
					{
					_LIT8(KNull,"");
					if(iAppUi.IsForeground()|| iAppUi.GetBgLock())
						{
						TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
						KNowPlayingFrmMainView,KNull);)
						iAppUi.iActivationPending = KNullUid ;	
						}
					else
						{
						iAppUi.SetActivationPendingValue(KIRNowPlayingViewID);	
						}
					}
				else
					{
					if ( iAppUi.IsForeground()|| iAppUi.GetBgLock())
						{
						iAppUi.SetActivationPendingValue(KNullUid);
						TRAP_IGNORE(iAppUi.ActivateLocalViewL(KIRNowPlayingViewID))
						}
					else
						{
						iAppUi.SetActivationPendingValue(KIRNowPlayingViewID);
						}
					}
				}
				iProgress = EFalse;
				}
			else
				{
				IRRDEBUG2("CIRNowPlayingWrapper::DestroyWaitDialogLiProgress Entering", KNullDesC); 
				TRAP_IGNORE(DestroyWaitDialogL());
				TRAP_IGNORE(PublishDataL();)
				iAppUi.GetNowPlayingViewInstance()->SetsameUrlFlag(ETrue);
				DoPlayingStateChanged( ETrue );
				if(iAppUi.GetPlayStateWhileCall())
					{
					iAppUi.SetPlayStateWhileCall(EFalse);
					break;
					}
				if(iPlayandStop)
					{
					iPlayandStop=EFalse;
					if(!iAppUi.GetNowPlayingViewInstance()->iContainer)
						{
						if(iContextViews==EHistoryView)
							{
							TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
							KHistoryLaunch,KIRNowPlayingViewCustomMessagePlayAfterActivation);)
							}
						else if(iContextViews==EFav||iContextViews==ELastplayed)
							{
							_LIT8(KNull,"");
							TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
							KNowPlayingFrmMainView,KNull);)
							}
						else
							{
							if ( iAppUi.IsForeground() || iAppUi.GetBgLock() )	
								{
								TRAP_IGNORE(iAppUi.ActivateLocalViewL(KIRNowPlayingViewID))
								}
							}
						}
					}
				else if(iAppUi.IsCallActive()||iAppUi.GetAlarm())
					{
					if(!iAppUi.GetNowPlayingViewInstance()->iContainer)
						{
						if(iContextViews==EHistoryView)
							{
							TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
							KHistoryLaunch,KIRNowPlayingViewCustomMessagePlayAfterActivation);)
							}
						else if(iContextViews==EFav)
							{
							_LIT8(KNull,"");
							TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
							KNowPlayingFrmMainView,KNull);)
							}
						else
							{
							if ( iAppUi.IsForeground() || iAppUi.GetBgLock() )	
								{
								TRAP_IGNORE(iAppUi.ActivateLocalViewL(KIRNowPlayingViewID))
								}
							}
						}
					}
				}
			iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStop);
			IRRDEBUG2("CIRNowPlayingWrapper::EBufferFillStop Exit", KNullDesC); 

			break;
			}

		case EBufferFilling:
			{
			//no implementation
			break;
			}

		case EBufferFadeOut:
			{
			//for fade out volume is updated
			SetAudioVolume(aValue);
			break;
			}

		case EBufferFadeIn:
			{
			//for fade in volume is updated
			SetAudioVolume(aValue);
			break;
			}

		case EStoppedPlaying:
			{

			if(iPlaying)
				{
				iAudioFadein=ETrue;
				}

			if( iSessionStart )
				{
				iReportGenerator->HandleStopEvents(EFalse);
				}
			if ( !iProgress || aValue != KErrDied )
				{
				if( !iAppUi.GetAlarm() && !iAppUi.IsCallActive() && aValue != KErrDied )
					{
					iNetwork->CancelRequest();
					}
				}

			TBool returnValue = EFalse;
			#ifndef __WINS__
			TRAP_IGNORE( ( returnValue = CheckCallStatusL() ) )
			#endif
			if(!returnValue)
				{
				DoPlayingStateChanged( EFalse );
				}
			if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
				{
				TRAP_IGNORE(iAppUi.iMainView->GetMainContainer()->UpdateAllL();)
				}

			break;
			}

		case EBufferFadeInReady:
			{
			//start audio fade in
			if(iAudioFadein && (!iAppUi.GetNowPlayingViewInstance()->IsVolumeMuted()))
				{
				TIRControlCommmand command = EBufferFadeIn;
				TInt volume=0;
				volume = iIRSettings.GetVolumeSetting();
				iChannel->AudioFadeInOut(command,volume);
				iAudioFadein = EFalse;
				}
			break;
			}
		case EStereo:
			{
			if(iAppUi.GetNowPlayingViewInstance())
				{
				iAppUi.GetNowPlayingViewInstance()->DeleteSteroEffect(ETrue);
				}
			break;
			}
		case EPlayingState:
			{
			if(iAppUi.GetNowPlayingViewInstance())
				{
				iAppUi.GetNowPlayingViewInstance()->DisablePlayer();
				}	
			}
		break;
		case EError:
			{
			if( ( KErrNotSupported == aValue ) || ( KErrNotFound == aValue )
					||(KIRCtrlCmdPlayerNotCreated==aValue) )
				{
				//iURLCount++
				//Handling Radio Playback Failed	
				if(iURLArray->MdcaCount()>0)
					{
					TRAP_IGNORE(DestroyWaitDialogL());
					DoPlayingStateChanged( EFalse );
					if( iSessionStart )
						{
						iReportGenerator->LogServerResult(iUrl,EIRConnFailed);
						iReportGenerator->HandleStopEvents(EFalse);
						//update current operator
						iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
						iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
						//update home operator
						iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
						iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
						
						TTerminationStatus terminationstatus = EUserTerminated;
						//and the cause of error
						iReportGenerator->UpdateTerminatedBy(terminationstatus);
						//session ends due to error
						TRAP_IGNORE(iReportGenerator->SessionEndL(EFalse);)
						iSessionStart=EFalse;	
						iSessionShouldNotEnd = EFalse;//check
						}
					iPlayBack=EFalse;
					if(!iProgress)
					{
					iNetwork->CancelRequest();
						if(iAppUi.GetNowPlayingViewInstance()->iContainer)
						{
						iLastPlayedUrl.Copy(iUnconnectedUrl);
						iLastConnectedUrl.Copy(iUnconnectedUrl);
						iLastPlayedUrl.Copy(iUnconnectedUrl);
						}
					else
						{
						iUrl.Delete(0,iUrl.Length());
				        iLastConnectedUrl.Zero();
				        iLastPlayedUrl.Zero();
						}
					}
					else
						{
						//reset the view to a proper view during the error conditions
						HandlePlayError();
						}
						if((iAppUi.GetNowPlayingViewInstance()->iContainer))
							{
							iAppUi.GetNowPlayingViewInstance()->SetBitrate(EFalse);
							}
					//if error occurs in player an error note is displayed
					TRAP_IGNORE(
					DisplayErrorNoteL(R_IRAPP_PLAY_FAIL_ERROR);
					)
					}
				}
			}
		break;

		default:
			{
			break;
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::MCtrlCommand - Exiting.");
	}
// ---------------------------------------------------------------------------
// MCrtlCmdCommand::FetchVolume()
// Function is pure virtual function which sents Request to get the current volume index
// @return volume index
// ---------------------------------------------------------------------------
//
TInt CIRNowPlayingWrapper::FetchVolume()
    {
	//gets the volume from the settings
	IRLOG_DEBUG( "CIRNowPlayingWrapper::FetchVolume" );
	TInt volume=0;
	volume=iIRSettings.GetVolumeSetting();
	return volume ;
	}
// ---------------------------------------------------------------------------
// MCrtlCmdCommand::DoAudioFadeOut()
// Function is pure virtual function, it is used to sent request start audio Fade Out
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::DoAudioFadeOut()
    {
	//triggers audio fade out
	IRLOG_DEBUG2( "CIRNowPlayingWrapper::DoAudioFadeOut - iAudioFadein=%d", iAudioFadein );
	if(iAudioFadein)
		{
		TIRControlCommmand command = EBufferFadeOut;
		TInt volume=0;
		volume= iIRSettings.GetVolumeSetting();
		iChannel->AudioFadeInOut(command,volume);
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DoAudioFadeOut - Exiting." );
	}
// ---------------------------------------------------------------------------
// MCrtlCmdCommand::PlayerChanged()
// Function is pure virtual function which sents Request to get the current volume index
// @return volume index
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::PlayerChanged()
    {
     if(iAppUi.GetNowPlayingViewInstance())
     {

     TRAP_IGNORE(iAppUi.GetNowPlayingViewInstance()->GetStereoModeL();)
     }
    }
	
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::ConnectToChannel()
// This function is called to connect to a particular channel
// This function will obtain the multiple urls into an array
// connects to the first url in the array
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ConnectToChannelL(CIRIsdsPreset& aPreset)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ConnectToChannel - Entering" );
	 iCheckValue=EFalse;
	 iDialogCanceled=EFalse;
	 iPlayandStop=ETrue;
	 iSyncPreset=EFalse;
	 if(iQualityRequired!=-1 && iURLArray->MdcaCount()!=1 && !iPlaying  )
	 {
	 TInt qualityrequired;
	 switch(iAppUi.iNetworkController->IdentifyConnectionType())
		{
		case EGprs: // Falls through.
		case EEdge:
			{
			//if GPRS network get the selection
			qualityrequired = iIRSettings.GetGPRSBitrateQuality();
			}
			break;
		case EWcdma:
		case ECdma2000:
			{
			//if 3G network get the selection
			qualityrequired = iIRSettings.Get3GBitrateQuality();
			}
			break;

		case EWiFi:
			{
			//if WiFi network get the selection
			qualityrequired = iIRSettings.GetWiFiBitrateQuality();
			}
			break;
		default:
			{
			//if WiFi network get the selection
			qualityrequired = iIRSettings.GetWiFiBitrateQuality();
			}
			break;
		}
	if(iQualityRequired!=qualityrequired)
		{
		iLastConnectedUrl.Zero();	
		}
	 }
	//getting the URLs from the preset according to the preset
	SelectUrlUsingBitRateL(aPreset);
	TBuf8<KUrlbufMaxLength> urlbuf;
			TBuf<KMaxUrlSize> CheckLastConnected;
	iURLCount = 0;
	if ( iURLArray->MdcaCount() > 0 )
	    {
       	urlbuf.Copy(iURLArray->MdcaPoint(iURLCount));
	    //connecting to a new channel so multiple URL selection enabled
	    //that if current URL doesn't connect we will try to connect to
	    //next URL in iURLArray
	    iMultipleURLSelect = ETrue;
	    iUnconnectedUrl.Copy(urlbuf);
	    for(TInt i=0;i<iURLArray->MdcaCount() && !iCheckValue;i++)
	    {
					CheckLastConnected.Copy(iURLArray->MdcaPoint(i));
			if(iLastConnectedUrl.Compare(CheckLastConnected)==0)	
	     {
				if(iPlaying||(iUinqid==iNowPlayingPreset->UniqId())||(iURLArray->MdcaCount()==1))
					{
					iCheckValue=ETrue;
					}
				if(iPlaying)
					{
					DestroyWaitDialogL();	
					}
				}
			}
		if(iNotify)
			{
			iDialogCanceled=EFalse;
			if(!iIsdsWrapper.ActiveWaitDialog())	
			CreateWaitDialogL();
			iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);

			if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
				{
				iAppUi.iMainView->GetMainContainer()->UpdateAllL();	
				}
			iReConnecting=EFalse;
             TRAPD(error,DoConnectToServerL(urlbuf));
				if( error )
					{
					IRLOG_ERROR2( "CIRNowPlayingWrapper::ConnectToChannel - DoConnectToServer failed (error=%d)", error );
					TRAP_IGNORE( DisplayErrorNoteL(R_IRAPP_ERROR_CONNECTION) )
					}
			}
		else
			{
		if((iAppUi.GetNowPlayingViewInstance()->iContainer))
			{
			iLaunchNowplaying=ETrue;
			}
		if(!iCheckValue)	
		{
		iLastConnectedUrl.Zero();
		CheckLastConnected.Zero();	
		if((iAppUi.GetNowPlayingViewInstance()->iContainer))
			{
			iAppUi.GetNowPlayingViewInstance()->SetsameUrlFlag(EFalse);
			}
		if(iPlaying && !iProgress)
		{
		#ifndef __WINS__
			StopBuffering();
			/**** ASF streaming changes : begin ****/
			if(!iAsfStreaming)
				{
				iPlayer->Stop();
				}
			else
				{
				iAsfStreaming = EFalse;
				iAsfPlayer->Stop();
				}
				if(iSessionStart)
				{
				//update current operator
				iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
				iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
				//update home operator
				iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
				iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
				
				TTerminationStatus terminationstatus = EUserTerminated;
				//and the cause of error
				iReportGenerator->UpdateTerminatedBy(terminationstatus);
				//session ends due to error
				iReportGenerator->SessionEndL(EFalse);
				iSessionStart=EFalse;
				}

			/**** ASF streaming changes : end ****/
		#endif //__WINS__
		}
		if(iContextViews==EFav||iContextViews==EPls||
				iContextViews==EHistoryView||iContextViews==ELastplayed)
			{
			CreateWaitDialogL();
			iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);
			}
		if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
			{
			iAppUi.iMainView->GetMainContainer()->UpdateAllL();	
			}	
		DoConnectToServerL(urlbuf);
		}
		else if(iCheckValue)
			{
						urlbuf.Copy(CheckLastConnected);
						iUnconnectedUrl.Copy(CheckLastConnected);
				if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
					{
					iAppUi.iMainView->GetMainContainer()->UpdateAllL();	
					}	

			 TRAPD(error,DoConnectToServerL(urlbuf));
			 if( error )
			    {
			    IRLOG_ERROR2( "CIRNowPlayingWrapper::ConnectToChannel - DoConnectToServer failed (error=%d)", error );
			    TRAP_IGNORE( DisplayErrorNoteL(R_IRAPP_ERROR_CONNECTION) )
	    		}	
			}
	    }
		}
		if(iContextViews==EFav)
		{
		TInt passId=iAppUi.GetFavPresetsInstance()->SearchPreset( iNowPlayingPreset->GetId(),
			iNowPlayingPreset->UniqId());
		// Checks for the preset index of the channel, if preset index exists
		// in favourites list, publishes the preset index.
		if( passId != KErrNotFound)
			{
			TInt presetIndex = passId+1;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
				presetIndex );
			}
		else
			{
			// If preset not present in favourites list, publishes the index
			// as 0. This is set to 0 here to indicate the active idle
			// component that the index with 0 is a non favourite channel.
			TInt presetIndex  = 0;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
				presetIndex );
			}


		}
		else
		{
			TInt presetIndex  = 0;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
				presetIndex );
		}

	IRLOG_DEBUG( "CIRNowPlayingWrapper::ConnectToChannel - Exiting." );
	}

// -------------------------------------------------------------------------
// CIRNowPlayingWrapper::SelectUrlUsingBitRateL()
// Selects the proper URL based on settings of quality
// according to current network
// current network type is stored in iNetwork. gets the URLs in iURLArray
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SelectUrlUsingBitRateL(CIRIsdsPreset& aPreset)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SelectUrlUsingBitRate - Entering" );
#ifdef __WINS__
	if(iURLArray)
		{
		delete iURLArray;
		iURLArray = NULL;
		}
	aPreset.SetUrlCount(); // remove build warning
	_LIT(KWinUrl,"http://172.28.205.137:8000");
    iURLArray = new (ELeave) CDesCArrayFlat(1);
    iURLArray->AppendL(KWinUrl);
#else	
	//by default we are setting quality as KHighQuality
	TInt qualityrequired = KHighQuality;
	switch(iAppUi.iNetworkController->IdentifyConnectionType())
		{
		case EGprs: // Falls through.
		case EEdge:
			{
			//if GPRS network get the selection
			qualityrequired = iIRSettings.GetGPRSBitrateQuality();
			}
			break;
		case EWcdma:
		case ECdma2000:
			{
			//if 3G network get the selection
			qualityrequired = iIRSettings.Get3GBitrateQuality();
			}
			break;

		case EWiFi:
			{
			//if WiFi network get the selection
			qualityrequired = iIRSettings.GetWiFiBitrateQuality();
			}
			break;
		default:
			{
			//if WiFi network get the selection
			qualityrequired = iIRSettings.GetWiFiBitrateQuality();
			}
			break;
		}
	iQualityRequired=qualityrequired;
	 selectedbitrate = 0;
	//depending on required quality we are selecting bit rate
	selectedbitrate = GetAvailableBitRate(qualityrequired,aPreset);

	IRLOG_INFO3( "CIRNowPlayingWrapper::SelectUrlUsingBitRate - selectedbitrate=%d, qualityrequired=%d",
	             selectedbitrate, qualityrequired );

	//getting all the available URLs
	if(iURLArray)
		{
		delete iURLArray;
		iURLArray = NULL;
		}
	TRAP_IGNORE(iURLArray = aPreset.GetUrlForBitrateL(selectedbitrate))
#endif

	IRLOG_DEBUG( "CIRNowPlayingWrapper::SelectUrlUsingBitRate - Exiting." );
	return;
	}

/// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::DisplayErrorNoteL()
// Display the error note
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::DisplayErrorNoteL(TInt aResourceId)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DisplayErrorNoteL - Entering" );
	HBufC* error = StringLoader::LoadLC(aResourceId);
	iIsdsWrapper.GetDialogLauncherInstance()->ShowErrorNoteL(aResourceId,ETrue);
	CleanupStack::PopAndDestroy(error);
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DisplayErrorNoteL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::DoConnectToServerL()
// Connects to Server.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::DoConnectToServerL(const TDesC8& aUrl)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DoConnectToServerL - Entering" );
    //Compress the heap memory
	iListenFromIsds=ETrue;
	RHeap& heapHandle = User::Heap();
	heapHandle.Compress();iListenFromIsds=ETrue;
	//This variable is used for checking whether request for each URl is done
	iConnectionRequestCancelled = EFalse;
	TBuf<KTemporaryUrlMaxLength> temporaryUrl;
	//This variable set while reconnecting to the same channel
	iReConnecting = EFalse;
	temporaryUrl.Copy(aUrl);
	if( (iLastConnectedUrl.Compare(temporaryUrl) == 0) &&
		(iLastPlayedUrl.Compare(temporaryUrl) == 0) )
		{
		 TInt test=iNowPlayingPreset->UniqId();
		if( iNowPlayingPreset->UniqId() != iLastConnectedPresetUniqId && iPlaying)
			{
			//if channel's unique is not equal to last connected channels unique id but
			//their urls match the preset information has to be updated
			//and channel name has to be changed
			iLastConnectedPresetId = iNowPlayingPreset->GetId();
			iLastConnectedPresetUniqId = iNowPlayingPreset->UniqId();
			iAppUi.GetNowPlayingViewInstance()->SetsameUrlFlag(ETrue);
			PublishDataL();
		if(!iAppUi.GetNowPlayingViewInstance()->iContainer)
		{
			if ( iAppUi.IsForeground() || iAppUi.GetBgWhenLockState() )
				{
				iAppUi.SetActivationPendingValue(KNullUid);
				if(iContextViews==EHistoryView)
				{
				iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
				KHistoryLaunch,KIRNowPlayingViewCustomMessagePlayAfterActivation);
				}
				else if(iContextViews==EFav)
				{
				_LIT8(KNull,"");
				TRAP_IGNORE(iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
				KNowPlayingFrmMainView,KNull);)
				}

				else
					{
					TRAP_IGNORE(iAppUi.ActivateLocalViewL(KIRNowPlayingViewID))
					}
				}
			else
				{
				iAppUi.SetActivationPendingValue(KIRNowPlayingViewID);
				}
		}
			}
			//If radioplay back is occured and if the user Tries to connect
			//to the same connected channel 
	  else	if(iPlayBack && iPlaying)
			{
			PublishDataL();
			if( KIRNowPlayingViewID == iAppUi.GetPresentViewID() )
				{
				//redraw now playing view if already activated
				iAppUi.GetNowPlayingViewInstance()->SetsameUrlFlag(ETrue);
				}
			else
				{
				//last played url is same as current url no need
				//to reconnect loads the now playing view
				if ( iAppUi.IsForeground()  || iAppUi.GetBgWhenLockState() )
					{
					//if in fore ground view is loaded
					iAppUi.ActivateLocalViewL( KIRNowPlayingViewID,
					KIRNowPlayingViewCustomMessageId,
					KIRNowPlayingViewCustomMessagePlayAfterActivation );
					iAppUi.SetActivationPendingValue(KNullUid) ;
					}
				else
					{
					iAppUi.SetActivationPendingValue(KIRNowPlayingViewID) ;
					}
				}
			}
	else if((iURLArray->MdcaCount()==1)||(iUinqid==iNowPlayingPreset->UniqId()))
		{
		DoPlayL();
		}
	
		}
	else
		{
		//If the next channel is getting connected then the Session which is made
		//should get terminated	
		if(iPlaying)
		{
    //audio fade in has to be done		
		iAudioFadein = ETrue;
		iPlaying=EFalse;
		}
		iTerminatedLogOnCancel=ETrue;						
		if( iSessionStart )
			{
			/*if( !iSessionShouldNotEnd )
				{
				//posting the previous session after ending it
				TTerminationStatus terminationstatus = EUserTerminated;
				//ending the previous session
				iReportGenerator->UpdateTerminatedBy(terminationstatus);
				iSessionStart = EFalse;
				iSessionShouldNotEnd = EFalse;
				//session end is called
				TRAPD(error,iReportGenerator->SessionEndL(ETrue));
				if( error )
					{
					IRLOG_ERROR2( "CIRNowPlayingWrapper::DoConnectToServerL - Report generation (Session End) failed (error=%d).",
					      error );
					}
				}*/
			iSessionShouldNotEnd = EFalse;
			}
		if(!iSessionStart)
			{
			LogStartSession();
			}
		iUrl.Copy(aUrl);
		//connection to new server url is attempted
		ConnectToServerL();
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DoConnectToServerL - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetAvailableBitRate()
// Selects proper bit rate from the settings
// it can be low quality (<48), medium quality (<128), high quality(>=128)
// nearest quality bitrate is selected if none is available the lowest one,
// which is the first one in preset array is returned
// ---------------------------------------------------------------------------
//
TInt CIRNowPlayingWrapper::GetAvailableBitRate(TInt aQualiyRequired,
			const CIRIsdsPreset& aPreset) const
	{
	IRLOG_DEBUG2( "CIRNowPlayingWrapper::GetAvailableBitRate(aQualityRequired )", aQualiyRequired );
	TInt qualityrequired = aQualiyRequired;

	//checking the quality is between KLowQuality and KHighQuality
	if( KLowQuality > qualityrequired )
		{
		qualityrequired = KLowQuality;
		}
	if( KHighQuality < qualityrequired )
		{
		qualityrequired = KHighQuality;
		}

	//getting all available bit rates from preset
	RArray<TInt> arrayofbitrates;
	aPreset.GetAvailableBitrates(arrayofbitrates);

	//finding the number of bit rates available
	TInt noofbitrateentries =  arrayofbitrates.Count();
	TInt bitrate = 0;
	//bit rate is not found
	TBool bitratefound = EFalse;
	//selecting the bitrate since index is one less than count we taken
	//noofbitrateentries - 1
	while(((noofbitrateentries - 1) >= 0) && (EFalse == bitratefound))
		{
		//proper bit rate is not found
		bitratefound = EFalse;
		//get the bit rate from the array for comparison
		bitrate = arrayofbitrates[noofbitrateentries - 1];
		if( KLowQuality == qualityrequired )
			{
			//if required quality is low we will select all the bit rate
			// less than Medium quality level (<48kbps)
			if( KMediumBitRateLimit > bitrate )
				{
				bitratefound = ETrue;
				}
			}
		else if( KMediumQuality == qualityrequired )
			{
			//if required quality is medium we will select all the bit
			//rate less than high quality level (<48kbps)
			if( KHighBitRateLimit > bitrate )
				{
				bitratefound = ETrue;
				}
			}
		else
			{
			//if required quality is high we will allow current bit rate
			bitratefound = ETrue;
			}
		//if we take next element if noofbitrateentries is greater than zero
		//and bitratefound = EFalse
		noofbitrateentries--;
		}
	//if no bitrate is available we will sent the first bitrate from entry

	arrayofbitrates.Close();
	IRLOG_DEBUG2( "CIRNowPlayingWrapper::GetAvailableBitRate - Returning %d", bitrate );
	return bitrate;
	}


// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::LogStartSession()
// Starts the session log session.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::LogStartSession()
    {
	IRLOG_DEBUG( "CIRNowPlayingWrapper::LogStartSession - Entering" );
	iReportGenerator->UpdateConnectedFrom(iConnectedFrom);
	//setting the channel id
	SetCurrentChannelID();
	iReportGenerator->UpdateChannelID(iChannelID);
	//update current operator
	iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
	iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
	//update home operator
	iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
	iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
	if(iSessionStartedFlag)
		{
		iSessionStart = ETrue;
		}
	else
		{
	//if session ended new session starts
	TRAPD(error,iReportGenerator->SessionStartedL());
	if( error )
    	{
		iSessionStart = EFalse;
		}
	else
		{
		iSessionStart = ETrue;
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::LogStartSession - Exiting" );
    }

// ---------------------------------------------------------------------------
// Function : SetCurrentChannelID
// Function sets currently playing channel ID
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetCurrentChannelID()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetCurrentChannelID - Entering" );
	//get current channel id
	iChannelID = iNowPlayingPreset->GetId();
	IRLOG_INFO2( "CIRNowPlayingWrapper::SetCurrentChannelID - ID now %d", iChannelID );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetCurrentChannelID - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::ConnectToServerL()
// Starts playing the selected stream
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ConnectToServerL()
	{
	//connection to a particular channel server url is
	//attempted
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ConnectToServerL - Entering" );
	iChannelConnect = ETrue;
	iConnectToServer=ETrue;
	RProperty::Get ( KUidActiveInternetRadioApp, KIRPSBitrate,
				 iBitRateValue);

	// ASF streaming check
	iAsfStreaming = EFalse;
#ifdef __IRA_FEATURE_WMA_ENABLED
	if( CIRAsfPlayer::DetectAsfChannel(iUrl) )
		{
		iAsfStreaming = ETrue;
		iAsfPlayer->PlayL(iUrl);
		}
#endif		
	if( !iAsfStreaming )
		{
		iNetwork->ConnectToServerL(iUrl);
		}

	iIsConnected = ETrue;
	//iFromPlsFile = EFalse
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ConnectToServerL - Exiting." );
	}

// ---------------------------------------------------------------------------
// Function : SetCurrentConnectionSource
// Function sets from where the current connection to the channel server
// is established. It may be from
// isds or saved preset
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetCurrentConnectionSource(TConnectedFrom aConnectedFrom)
	{
	//0 means isds
	//1 means saved preset
	IRLOG_DEBUG2( "CIRNowPlayingWrapper::SetCurrentConnectionSource(aConnectedFrom=%d)", aConnectedFrom );
	iConnectedFrom = aConnectedFrom;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetCurrentConnectionSource - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::DoErrorConnectingL()
// Called by CIRStreamSource when there is an error in connectiong
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::DoErrorConnectingL(TInt aErrorCode)
	{
	IRLOG_ERROR3("CIRNowPlayingWrapper::DoErrorConnectingL(aErrorCode=%d) - iUrl=%S", aErrorCode, &iUrl );
	//Called by CIRStreamSource when there is an error in connectiong
	//to channel server.
	iChannelConnected=EFalse;
	if(iSessionStart)
		{
		if( KIRStreamSourceTimeOut == aErrorCode )
			{
			//error URLs is logged
			iReportGenerator->LogServerResult(iUrl,EIRConnTimeOut);
			}
		else if( KIRStreamSourceNoResponse == aErrorCode )
			{
			//error URLs is logged
			iReportGenerator->LogServerResult(iUrl,EIRFull);
			}
		else
			{
			//error URLs is logged
			iReportGenerator->LogServerResult(iUrl,EIRConnFailed);
			}
		}

	//URL is deleted
	iUrl.Zero();
	iURLCount++;
	    //since each preset has multiple urls trying to connect to the next url
	if( iMultipleURLSelect )
		{
		//if error before connection to channel server
		//multiple urlselection is enabled
		if( (iURLCount) < iURLArray->MdcaCount() && !iConnectionRequestCancelled )
			{
			//take the subsequent URLs and play in case of failure
			TBuf8<KUrlbufMaxLength> urlbuf;
			urlbuf.Copy(iURLArray->MdcaPoint(iURLCount));
			iUnconnectedUrl.Copy(urlbuf);
			iMultipleURLSelect = ETrue;
			iSessionShouldNotEnd = ETrue;
			//connect to server is called
			DoConnectToServerL(urlbuf);
			}
		else
			{
				//If all the possible urls fails to connect it displays
				//Unable to connect
				iTerminatedLogOnCancel=EFalse;						
			if(iAppUi.GetNowPlayingViewInstance()->iContainer)
        		{
        		iDisplayMetaData=ETrue;
        		iAppUi.GetNowPlayingViewInstance()->DisplaySongArtistNameL( KConst,KConst);
				iLastPlayedUrl.Copy(iUnconnectedUrl);
				iLastConnectedUrl.Copy(iUnconnectedUrl);
				iLastConnectedPresetId=0;
        		}
        	else
	        	{
		        iUrl.Delete(0,iUrl.Length());
		        iLastConnectedUrl.Zero();
		        iLastPlayedUrl.Zero();
	        	}
        	ClearMetaDataL();
			DestroyWaitDialogL();
			iPlayBack=ETrue;
			if((iAppUi.GetNowPlayingViewInstance()->iContainer))
				{
				iAppUi.GetNowPlayingViewInstance()->SetBitrate(ETrue);
				}
			iDialogCanceled=EFalse;
            iConnectToServer=EFalse;
            iNotify=EFalse;
            iListenFromIsds=EFalse;
            if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
					{
					iAppUi.iMainView->GetMainContainer()->UpdateAllL();	
					}
			iAppUi.SetPlayStateWhileCall(EFalse);
			iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);
				DisplayErrorNoteL(aErrorCode == 
					KIRStreamSourceInvalidUrl ? R_IRAPP_ERROR_ADDRESS : R_IRAPP_ERROR_GENERAL);
				iConnErrCode = 0;
				//Since channel is not able to connect Ending the Session
				if(iSessionStart)
					{
					//update current operator
					iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
					iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
					//update home operator
					iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
					iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());

					TTerminationStatus terminationstatus = ENoConnectionToServer;
					//and the cause of error
					iReportGenerator->UpdateTerminatedBy(terminationstatus);
					//session ends due to error
					iReportGenerator->SessionEndL(EFalse);
	    			iSessionStart=EFalse;	
					iSessionShouldNotEnd = EFalse;//check
					}
				DoPlayingStateChanged( EFalse );
				}
		}
	else
		{
		 //if the error is Connection Timed out
		if( KIRStreamSourceTimeOut == aErrorCode )
			{
			//if the response is no response from server
			if( iProgress )
				{
				//progress bar view is cancelled
				ProgressBarCancel();
				}
			DestroyWaitDialogL();
			iConnectToServer=EFalse;
			iNotify=EFalse;
			iListenFromIsds=EFalse;
			iDialogCanceled=EFalse;
			
			iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);

			//error note to be displayed
			DisplayErrorNoteL(R_IRAPP_ERROR_CONNECTION);//player is expected not to play
			ClearMetaDataL();
			iLastPlayedUrl.Copy(iUnconnectedUrl);
			iLastConnectedUrl.Copy(iUnconnectedUrl);
	        iLastConnectedPresetId=0;
	        iPlayBack=ETrue;
			DoPlayingStateChanged( EFalse );
			iAppUi.SetPlayStateWhileCall(EFalse);
			}
		else
			{
			StopNow();
			//if error due connection at any point after connecting to channel server
			if(iAppUi.GetNowPlayingViewInstance()->iContainer)
        		{
        		iDisplayMetaData=ETrue;
        		iAppUi.GetNowPlayingViewInstance()->DisplaySongArtistNameL( KConst,KConst);
				iLastPlayedUrl.Copy(iUnconnectedUrl);
				iLastConnectedUrl.Copy(iUnconnectedUrl);
				iLastConnectedPresetId=0;
        		}
        	else
	        	{
		        iUrl.Delete(0,iUrl.Length());
		        iLastConnectedUrl.Zero();
		        iLastPlayedUrl.Zero();
	        	}
        	 ClearMetaDataL();
        	iTerminatedLogOnCancel=EFalse;						
        	iDialogCanceled=EFalse;
        	DestroyWaitDialogL();
        	DoPlayingStateChanged(EFalse);
        	if((iAppUi.GetNowPlayingViewInstance()->iContainer))
				{
				iAppUi.GetNowPlayingViewInstance()->SetBitrate(ETrue);
				}
        	iConnectToServer=EFalse;
        	iNotify=EFalse;
        	iListenFromIsds=EFalse;
        	iPlayBack=ETrue;
        	if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
					{
					iAppUi.iMainView->GetMainContainer()->UpdateAllL();	
					}
					
			iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingError);
					
        	if(!iAppUi.iNetworkController->IsHandingOverConnection())
        	{	
        	
        		if (aErrorCode != KIRStreamSourceDisconnected)
				{
				DisplayErrorNoteL(aErrorCode == KIRStreamSourceInvalidUrl ?
								 R_IRAPP_ERROR_ADDRESS : R_IRAPP_ERROR_GENERAL);
				}
			}	
			}
		 	if(iSessionStart)
				{
				//update current operator
				iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
				iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
				//update home operator
				iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
				iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());

				TTerminationStatus terminationstatus = ENoConnectionToServer;
				//and the cause of error
				iReportGenerator->UpdateTerminatedBy(terminationstatus);
				//session ends due to error
				iReportGenerator->SessionEndL(EFalse);
				iSessionStart=EFalse;	
				iSessionShouldNotEnd = EFalse;//check
				}
		}	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DoErrorConnectingL - Exiting.");
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::ProgressBarCancel()
// Cancels the Network request and stops the buffering on progressbar cancel.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ProgressBarCancel()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ProgressBarCancel - Entering");

	RProperty::Set( KUidActiveInternetRadioApp,
		KIRPSBitrate, iBitRateValue );
	//Cancel the Network request
	
	/**** ASF streaming changes : begin ****/
	//Cancel the Network request
	if(!iAsfStreaming)
		iNetwork->CancelRequest();
	/**** ASF streaming changes : end ****/
	
	iLastConnectedPresetId=0;
	iLastConnectedPresetUniqId = 0;
    DoPlayingStateChanged( EFalse );
	iAudioFadein = EFalse;
	iProgress = EFalse;
	if(iAppUi.GetNowPlayingViewInstance()->iContainer)
		{
		iLastPlayedUrl.Copy(iUnconnectedUrl);
		iLastConnectedUrl.Copy(iUnconnectedUrl);
		}
	else
		{
		iUrl.Delete(0,iUrl.Length());
        iLastConnectedUrl.Zero();
        iLastPlayedUrl.Zero();
		}		
	//stops buffering from media client, media engine is not stopped
	
	//stops buffering from media client, media engine is not stopped
	/**** ASF streaming changes : begin ****/
	if(!iAsfStreaming)
		iPlayer->StopInitialBuffering();
	/**** ASF streaming changes : end ****/

	if(!iCancelNetworkRequest)
	{
	if( iSessionStart )
		{
		//update current operator
		iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
		iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
		//update home operator
		iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
		iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
		
		TTerminationStatus terminationstatus = EUserTerminated;
		//ending the previous session
		iReportGenerator->UpdateTerminatedBy(terminationstatus);
		iSessionStart = EFalse;
		iSessionShouldNotEnd = EFalse;
		//session is ended
		TRAP_IGNORE( iReportGenerator->SessionEndL(EFalse) )
		}
	}
	iCancelNetworkRequest=EFalse;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ProgressBarCancel - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::DoPlayingStateChangedL()
// Handles internal state change and P&S when playing is stopped or started.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::DoPlayingStateChanged( TBool aPlaying )
    {
    IRLOG_INFO2( "CIRNowPlayingWrapper::DoPlayingStateChanged - playing state=%d", aPlaying );
    if ( aPlaying )
        {
		iPlaying = ETrue;
		iAppUi.GetNowPlayingViewInstance()->SetPlayingValue (iPlaying);
        iAppUi.GetPubSubManagerInstance()->PublishChannel( iNowPlayingPreset->GetName() );
    	iAppUi.GetPubSubManagerInstance()->PublishPlayStop( EIRStatePlay );
        }
    else
        {
		iPlaying = EFalse;
		iAppUi.GetNowPlayingViewInstance()->SetPlayingValue (iPlaying);
    	iAppUi.GetPubSubManagerInstance()->PublishPlayStop( EIRStateStop );
        }
	if(iAppUi.GetNowPlayingViewInstance()->iContainer)
		{
		TRAP_IGNORE(iAppUi.GetNowPlayingViewInstance()->ConstructToolbarL();)
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DoPlayingStateChangedL - Exiting" );
    }
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::DoPlay()
// First try to reconnect to the last played channel url
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::DoPlayL()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::DoPlayL - Entering" );
	CreateWaitDialogL();
	iAppUi.GetPubSubManagerInstance()->PublishBufferingState(EIRStateBufferingStart);

	//reconnecting to the channel
	if( iAppUi.GetCallConnectedState() )
		{
	    IRLOG_DEBUG( "CIRNowPlayingWrapper::DoPlayL - Call ongoing, Exiting." );
		return;
		}
	// connection exits	hence proceed with the connection
	// to channel
	else
		{
    	//load the loading screen
    	iMultipleURLSelect = EFalse;
    	iReConnecting = ETrue;
    	iChannelConnect = ETrue;
    	iDialogCanceled=EFalse;
    	iUrl.Copy(iLastPlayedUrl);
	if((iAppUi.GetNowPlayingViewInstance()->iContainer))
			{
			iLaunchNowplaying=ETrue;
			}	
		/**** ASF streaming changes : begin ****/
		TInt error = KErrNone;
		if(!iAsfStreaming)
			{
			TRAP(error,iNetwork->ReconnectL(iLastPlayedUrl));
			}
		else
			{
			iAsfPlayer->PlayL(iLastPlayedUrl);
			}
		/**** ASF streaming changes : end ****/

    	if( error )
    		{
    		IRLOG_ERROR2( "CIRNowPlayingWrapper::DoPlayL - Reconnect failed (error=%d)", error );
    		TRAP_IGNORE(DisplayErrorNoteL(R_IRAPP_ERROR_CONNECTION))
    		DestroyWaitDialogL();
    		}
		if(!iSessionStart)
			{
            LogStartSession();
			}

		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DoPlayL - Exiting." );

	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::StopNow()
// Function calls stop functionality.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::StopNow()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::StopNow - Entering" );
	if(iIsConnected)
		{
		//player is stopped
		if(iPlaying)
			{

			/**** ASF streaming changes : begin ****/
			if(!iAsfStreaming)
				{
				iPlayer->Stop();
				}
			else

				{
				iAsfPlayer->Stop();
				iAsfStreaming = EFalse;
				}
			/**** ASF streaming changes : end ****/

			DoPlayingStateChanged( EFalse );
			if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
				{
				TRAP_IGNORE(iAppUi.iMainView->GetMainContainer()->UpdateAllL();)
				}
			iPlayandStop=EFalse;
			if(iAppUi.GetNowPlayingViewInstance()->iContainer)
				{
				iPlayandStop=ETrue;
				}
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::StopNow - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::ClearMetaDataL()
// Clears the current meta data information.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ClearMetaDataL()
    {
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ClearMetaDataL - Entering" );
    CIRMetaData* metaData = CIRMetaData::NewL();
    CleanupStack::PushL( metaData );
    // Handles clearing the information with strings read from resources
    HandleMetaDataReceivedL( *metaData ); 
    CleanupStack::PopAndDestroy( metaData );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ClearMetaDataL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::ListenToChannel()
// Requests for listening to a particular channel
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::ListenToChannelL(CIRIsdsPreset* aPreset)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ListenToChannel - Entering" );
	*iNowPlayingPreset = *aPreset;
	
	//replace the preset with favorites preset (if exists in favorites)
	iAppUi.GetFavPresetsInstance()->ReplacePresetL(*iNowPlayingPreset);

	//channel is connected from isds
	SetCurrentConnectionSource(EIRIsds);
	//connecting to a particular channel
	ConnectToChannelL(*iNowPlayingPreset);
	IRLOG_DEBUG( "CIRNowPlayingWrapper::ListenToChannel - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::MetaData()
// Returns the current meta data information.
// ---------------------------------------------------------------------------
//
const CIRMetaData& CIRNowPlayingWrapper::MetaData() const
    {
    IRLOG_DEBUG( "CIRNowPlayingWrapper::MetaData " );
    return *iMetaData;
    }

// -----------------------------------------------------------------------------
// CIRNowPlayingWrapper::DialogDismissedL()
// Called by CAknwaitdialog 
// -----------------------------------------------------------------------------
// 
void CIRNowPlayingWrapper::DialogDismissedL( TInt aButtonId )
	{
	IRLOG_DEBUG( "CIRSearchView::DialogDismissedL() started." );
	
		if(aButtonId == EIRCancel)
			{
			if(iSyncPreset)
				{
				iAppUi.iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();	
				iSyncPreset=EFalse;
				if((iAppUi.iNowPlayingView->iContainer))
				{
			iAppUi.iNowPlayingView->SetRockerIndex(
						iAppUi.iNowPlayingView->GetPreviousIndex());
				}
				iWaitDialog= NULL; 
				return;
				}
			if(!iDialogCanceled)
				{
				if(iContextViews==ESearch||iContextViews==EBrowseByTopStations
					||iContextViews==EStations||iContextViews==EHistoryView)
					{
					iAppUi.iIsdsWrapper->SetListenRequest(EFalse);
					if(iNowPlayingPreset->GetChannelType())
						{
						if(!iListenFromIsds)
							{
							if ( iAppUi.iIsdsWrapper->GetISDSInstance() )
								{
								//iListenFromIsds=EFalse
								iCancelFrmIsds=ETrue;
								iAppUi.iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
								}
							if((iAppUi.iNowPlayingView->iContainer))
								{
							iAppUi.iIsdsWrapper->SetCurrentPlayingIndex(
							iAppUi.iNowPlayingView->GetPreviousIndex());
					iAppUi.iNowPlayingView->SetRockerIndex(
								iAppUi.iNowPlayingView->GetPreviousIndex());
								}
							iWaitDialog= NULL;
							return;
							}

						}
					}
		if(iBufferigSteamText)
			{
			// Dialog will delete itself
			iWaitDialog= NULL;   
			ProgressBarCancel();
			iUrl.Delete(0,iUrl.Length());
	        iLastConnectedUrl.Zero();
	        iLastPlayedUrl.Zero();
	        iUinqid=0;
			}
		else
			{
			// Dialog will delete itself
			iWaitDialog= NULL;
			if(!(iAppUi.GetNowPlayingViewInstance()->GetsameUrlFlag()))
 
			CancelNetworkRequestL();
			DoPlayingStateChanged(EFalse);
			iAppUi.GetNowPlayingViewInstance()->SetsameUrlFlag(EFalse);
			iUrl.Delete(0,iUrl.Length());
	        iLastConnectedUrl.Zero();
	        iLastPlayedUrl.Zero();
	       	iUinqid=0;
			}
		ClearMetaDataL();
		if( iSessionStart )
			{
			//update current operator
			iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
			iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
			//update home operator
			iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
			iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
			
			TTerminationStatus terminationstatus = EUserTerminated;
			//ending the previous session
			iReportGenerator->UpdateTerminatedBy(terminationstatus);            
			iSessionStart = EFalse;
			iSessionShouldNotEnd = EFalse;
			//session is ended
			TRAP_IGNORE( iReportGenerator->SessionEndL(EFalse) )					
			}
		}
	}

	IRLOG_DEBUG( "CIRSearchView::DialogDismissedL() exiting." );
	}    

// ---------------------------------------------------------------------------
// Function : HandlePlayError()
// Handles play error || should called only through MCtrlCommand
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::HandlePlayError()
	{
	IRLOG_ERROR( "CIRNowPlayingWrapper::HandlePlayError - Entering" );
	//if progress bar is present cancel progress bar
	if( iProgress )
		{
		ProgressBarCancel();
		TRAP_IGNORE(DestroyWaitDialogL());
		}	
	//buffering overlay if present is removed

	IRLOG_DEBUG( "CIRNowPlayingWrapper::HandlePlayError - Exiting." );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::PlayAndStopL()
// Performs play and stop functionality
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::PlayAndStopL()
	{
	IRLOG_INFO( "CIRNowPlayingWrapper::PlayAndStopL - Entering" );

	if( !iPlaying )
		{
		//if currently status is not playing
		//play request is issued
		DoPlayL();

		// Active Idle
		TInt passId=iAppUi.GetFavPresetsInstance()->SearchPreset( iNowPlayingPreset->GetId(),
			iNowPlayingPreset->UniqId());
		// Checks for the preset index of the channel, if preset index exists
		// in favourites list, publishes the preset index.
		if( passId != KErrNotFound)
			{
			TInt presetIndex = passId+1;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
				presetIndex );
			}
		else
			{
			// If preset not present in favourites list, publishes the index
			// as 0. This is set to 0 here to indicate the active idle
			// component that the index with 0 is a non favourite channel.
			TInt presetIndex  = 0;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
				presetIndex );
			}
		}
	else
		{
		//if status is playing stop is called
		if(iIsConnected)
			{
			iPlayandStop=ETrue;
			iConnectToServer=EFalse;
			/**** ASF streaming changes : begin ****/
			if(!iAsfStreaming)
				iPlayer->Stop();
			/**** ASF streaming changes : end ****/

			if( iSessionStart )
		        {
				//update current operator
				iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
				iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
				//update home operator
				iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
				iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
					        
		        // End the session
		        iReportGenerator->UpdateTerminatedBy( EUserTerminated );
		        iSessionStart = EFalse;
		        iSessionShouldNotEnd = EFalse;
		        iReportGenerator->SessionEndL( EFalse );
		        }
            DoPlayingStateChanged( EFalse );
			}
		// Active Idle
		TInt passId=iAppUi.GetFavPresetsInstance()->SearchPreset( iNowPlayingPreset->GetId(),
			iNowPlayingPreset->UniqId());
		// Checks for the preset index of the channel, if preset index exists
		// in favourites list, publishes the preset index.
		if( passId != KErrNotFound)
			{
			TInt presetId = passId+1;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
			presetId );
			}
		else
			{
			// If preset not present in favourites list, publishes the index
			// as 0. This is set to 0 here to indicate the active idle
			// component that the index with 0 is a non favourite channel.
			TInt presetId = 0;
			RProperty::Set( KUidActiveInternetRadioApp, KIRPSPresetIndex,
				presetId );
			}
		}
		
	IRLOG_DEBUG( "CIRNowPlayingWrapper::PlayAndStopL - Exiting." );
	}
	
// ---------------------------------------------------------------------------
// LoadTerminateSession()
// The loading cancel result in session log end
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::LoadTerminateSessionL()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::LoadTerminateSession - Entering." );
	//checks whether session log can be terminated
	if( iTerminatedLogOnCancel )
		{
		//loading cancel is cancelling session log
		//session log should not be cancelled
		iTerminatedLogOnCancel = EFalse;
		//checks whether session log exists or not
		if(iSessionStart)
			{
			//logging server results connection is not yet made
			//and attempt is failed
			iReportGenerator->LogServerResult(iUrl,EIRConnFailed);
			//update current operator
			iReportGenerator->UpdateCurrentNetwork(iAppUi.GetCurrentOperatorValue());
			iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
			//update home operator
			iReportGenerator->UpdateHomeOperator(iAppUi.GetHomeOperatorValue());
			iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
			
			//user terminated
			iReportGenerator->UpdateTerminatedBy(EUserTerminated);
			//session ends due to error
			iReportGenerator->SessionEndL(EFalse);
			
			TBool validate = iReportGenerator->ReStorePreviousSession();
			//restoring previous session
			if (validate)
				{
				iSessionStart = ETrue; 					
				}
			else
				{
				iSessionStart=EFalse;		
				}
			iSessionShouldNotEnd = EFalse;
			
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::LoadTerminateSession - Exiting." );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetMediaClientInstance()
// Returns the Media Client Instance
// ---------------------------------------------------------------------------
//
CIRMediaClient* CIRNowPlayingWrapper::GetMediaClientInstance()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetMediaClientInstance " );
	return iPlayer;	
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetMediaClientInstance()
// Returns the ReportGenerator Instance
// ---------------------------------------------------------------------------
//
CIRReportGenerator* CIRNowPlayingWrapper::GetReportGeneratorInstance()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetReportGeneratorInstance " );
	return iReportGenerator;	
	}

	
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetTerminatedLogOnCancelState()
// Returns the iTerminatedLogOnCancel Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::GetTerminatedLogOnCancelState()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetTerminatedLogOnCancelState " );
	return iTerminatedLogOnCancel;	
	}
	
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetSessionStartState()
// Returns the iSessionStart Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::GetSessionStartState()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetSessionStartState " );
	return iSessionStart;
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetSessionStartedFlag()
// Returns the iSessionStarted Flag Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::GetSessionStartedFlag()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetSessionStartedFlag " );
	return iSessionStartedFlag;
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetSessionStartedFlag()
// Sets the iSessionStartedFlag  Value
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetSessionStartedFlag(TBool aSessionStartedFlag)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetSessionStartedFlag - Entering " );
	iSessionStartedFlag = aSessionStartedFlag;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetSessionStartedFlag - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetSessionStartState()
// Sets the iSessionStart Value
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetSessionStartState(TBool aSessionStart)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetSessionStartState - Entering " );
	iSessionStart = aSessionStart;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetSessionStartState - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::IsProgress()
// Returns the iProgress Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::IsProgress()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::IsProgress " );
	return iProgress;
	}


// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetConnectToServerState()
// Sets the iConnectToServer Value
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetConnectToServerState(TBool aConnectToServer)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetConnectToServerState - Entering" );
	iConnectToServer = aConnectToServer;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetConnectToServerState - Exiting" );
	}
	
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetConnectToServerState()
// Returns the iConnectToServer Value
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::GetConnectToServerState()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetConnectToServerState - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetConnectToServerState - Exiting" );
	return iConnectToServer ;
	}
// CIRNowPlayingWrapper::CancelNetworkRequestL()
// Sets the iConnectToServer Value
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::CancelNetworkRequestL()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::CanecelNetworkRequest - Entering" );
	if(iChannelConnect)
		{
		/**** ASF streaming changes : begin ****/
		if(!iAsfStreaming)
			iNetwork->CancelRequest();
		/**** ASF streaming changes : end ****/
		if(iAppUi.GetNowPlayingViewInstance()->iContainer)
			{
			iLastPlayedUrl.Copy(iUnconnectedUrl);
			iLastConnectedUrl.Copy(iUnconnectedUrl);
			}
		else
			{
			iUrl.Delete(0,iUrl.Length());
	        iLastConnectedUrl.Zero();
			}		

		LoadTerminateSessionL();
		}
	else
		{
		iCancelNetworkRequest = ETrue;
		ProgressBarCancel();	
		}
	IRLOG_DEBUG( "CIRNowPlayingWrapper::CanecelNetworkRequest - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetVolumeL()
// Sets the Volume to Player
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetVolumeL(TInt aIndex)
{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetVolume - Entering" );

	/**** ASF streaming changes : begin ****/
	if(!iAsfStreaming && iPlayer)
		{
		TInt index = iPlayer->MaxVolume()/KNOVOLUMELEVELS;
		TInt volume = index*aIndex;
		iPlayer->SetVolume(volume);	
		}
	else if(iAsfStreaming && iAsfPlayer)
		{
		TInt index = iAsfPlayer->MaxVolume()/KNOVOLUMELEVELS;
		TInt volume = index*aIndex;
		iAsfPlayer->SetVolume(volume);	
		}
	/**** ASF streaming changes : end ****/

	iIRSettings.SetVolumeSettingL(aIndex);
    iAppUi.GetPubSubManagerInstance()->PublishVolume( iIRSettings.GetVolumeSetting() );
    IRLOG_DEBUG( "CIRNowPlayingWrapper::SetVolume - Exiting" );
}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetPrevVolumeLevel()
// Sets the Volume to Player
// ---------------------------------------------------------------------------
//
TInt  CIRNowPlayingWrapper::GetPrevVolumeLevel()
{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetPrevVolumeLevel" );
	return iPrevVolumeLevel; 
}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetPrevVolumeLevel()
// Sets the Volume to Player
// ---------------------------------------------------------------------------
//
void   CIRNowPlayingWrapper::SetPrevVolumeLevel(TInt aPrevVolume)
{
	 IRLOG_DEBUG( "CIRNowPlayingWrapper::SetPrevVolumeLevel - Entering" );
	 iPrevVolumeLevel = aPrevVolume;
	 IRLOG_DEBUG( "CIRNowPlayingWrapper::SetPrevVolumeLevel - Exiting" );
}
 // ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetPrevVolumeLevel()
// Sets the Volume to Player
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper:: SetMetadataL()
{
	 IRLOG_DEBUG( "CIRNowPlayingWrapper::SetMetadata() - Entering" );
	iMetaData->SetArtistL(KConst);
	iMetaData->SetSongL( KConst );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetMetadata() - Exiting" );

}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::PublishDataL()
// Publishes the Metadata
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper:: PublishDataL()
{	 
	IRLOG_DEBUG( "CIRNowPlayingWrapper::PublishDataL() - Entering" );

		iAppUi.GetPubSubManagerInstance()->PublishChannel( iNowPlayingPreset->GetName() );
		iAppUi.GetPubSubManagerInstance()->PublishMetaDataL( *iMetaData );
		iAppUi.GetPubSubManagerInstance()->PublishChannelType(
						iNowPlayingPreset->GetChannelType());
		iAppUi.GetPubSubManagerInstance()->PublishChannelId(iNowPlayingPreset->GetId());
	    iAppUi.GetPubSubManagerInstance()->PublishChannelDesc(
	    				iNowPlayingPreset->GetShortDescription());
	    if(iNowPlayingPreset->GetChannelType()==1)
		    {
	    	iAppUi.GetPubSubManagerInstance()->PublishChannelMusicFlag(
	    	            iNowPlayingPreset->GetMusicStoreStatus());
		    }
		 if(iAppUi.GetNowPlayingViewInstance())
			 {
			 iAppUi.GetNowPlayingViewInstance()->UpdateLastplayedL();
			 }
	IRLOG_DEBUG( "CIRNowPlayingWrapper::PublishDataL() - Exiting" );

}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::StopPlayerL()
// Stops the Player
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::StopPlayer()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::StopPlayerL() - Entering" );

	/**** ASF streaming changes : begin ****/
	if(!iAsfStreaming && iPlayer)
		{
		iPlayer->Stop();
		DoPlayingStateChanged(EFalse);
		iPlayandStop=EFalse;
		}
	else if(iAsfStreaming && iAsfPlayer)
		{
		iAsfPlayer->Stop();
		iAsfStreaming = EFalse;
		DoPlayingStateChanged(EFalse);
		}
		if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
		{
		TRAP_IGNORE(iAppUi.iMainView->GetMainContainer()->UpdateAllL();)	
		}	
	/**** ASF streaming changes : end ****/

	IRLOG_DEBUG( "CIRNowPlayingWrapper::StopPlayerL() - Exiting" );
	}
	
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetMedadataForErrotrConditions()
// Sets the iDisplayMetaData Flag When Handling the Context Navigation
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::SetMedadataForErrotrConditions(TBool aDisplayMetaData)
{
IRLOG_DEBUG( "CIRNowPlayingWrapper::SetMedadataForErrotrConditions() - Entering" );

	iDisplayMetaData=aDisplayMetaData;
 IRLOG_DEBUG( "CIRNowPlayingWrapper::SetMedadataForErrotrConditions() - Exiting" );

}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetMedadataForErrotrConditions()
// Returns the iDisplayMetaData Flag When Handling the Context Navigation
// ---------------------------------------------------------------------------
//

TBool CIRNowPlayingWrapper::GetMedadataForErrotrConditions()
{
IRLOG_DEBUG( "CIRNowPlayingWrapper::GetMedadataForErrotrConditions() - Entering" );
IRLOG_DEBUG( "CIRNowPlayingWrapper::GetMedadataForErrotrConditions() - Exiting" );
	return iDisplayMetaData;
}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetMarquee()
// Returns the iMarQuee which is Set During HandleMetadataReceived
// ---------------------------------------------------------------------------
//

TBool CIRNowPlayingWrapper::GetMarquee()
{
IRLOG_DEBUG( "CIRNowPlayingWrapper::GetMarquee() - Entering" );
IRLOG_DEBUG( "CIRNowPlayingWrapper::GetMarquee() - Exiting" );
	return iMarquee;
}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetMarquee()
// Sets the Marquee value required for NowPlaying view
// ---------------------------------------------------------------------------
//
 void CIRNowPlayingWrapper::SetMarquee(TBool aMarquee)
{
IRLOG_DEBUG( "CIRNowPlayingWrapper::SetMarquee() - Entering" );
	
iMarquee=aMarquee;

IRLOG_DEBUG( "CIRNowPlayingWrapper::SetMarquee() - Exiting" );
	
}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::SetView()
// Set by Every View to its TIRViews when Listen is Done
// ---------------------------------------------------------------------------
//
 	
void CIRNowPlayingWrapper:: SetView(TIRViews aContextViews)
{
IRLOG_DEBUG( "CIRNowPlayingWrapper::SetView() - Entering" );

	iContextViews=aContextViews;
IRLOG_DEBUG( "CIRNowPlayingWrapper::SetView() - Exiting" );

}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::GetView()
// Returns the View Where Listen Has Done
// ---------------------------------------------------------------------------
//
TIRViews CIRNowPlayingWrapper:: GetView()
{
IRLOG_DEBUG( "CIRNowPlayingWrapper::GetView() - Entering" );
IRLOG_DEBUG( "CIRNowPlayingWrapper::GetView() - Exiting" );
return iContextViews;
}
// ---------------------------------------------------------------------------
// CIRUi::SetAudioVolume()
// function set the volume
// ---------------------------------------------------------------------------
//		
void CIRNowPlayingWrapper::SetAudioVolume(TInt aValue)
	{
	IRLOG_INFO2( "CIRNowPlayingWrapper::SetAudioVolume(aValue=%d)", aValue );	

	/**** ASF streaming changes : begin ****/
	if(!iAsfStreaming)
		{
		TInt index = iPlayer->MaxVolume()/KNOVOLUMELEVELS;	
		TInt volume = aValue * index;
		//volume level between zero volume and maximum volume  of
		//device is taken, if volume is beyound these limit it is not set
		if( (0 <= volume) && (iPlayer->MaxVolume() >= volume) )
			{
			iPlayer->SetVolume(volume);
			}		
		}
	else
		{
		TInt index = iAsfPlayer->MaxVolume()/KNOVOLUMELEVELS;	
		TInt volume = aValue * index;
		//volume level between zero volume and maximum volume  of
		//device is taken, if volume is beyound these limit it is not set
		if( (0 <= volume) && (iAsfPlayer->MaxVolume() >= volume) )
			{
			iAsfPlayer->SetVolume(volume);
			}		
		}
	/**** ASF streaming changes : end ****/
		

	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetAudioVolume - Exiting." );
	}
// ---------------------------------------------------------------------------
// CreateWaitDialogL()
// Creates the waitDialog Required While Connecting To a Channel
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::CreateWaitDialogL()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::CreateWaitDialogL - Entering" );
	    
		HBufC* bufferText  = StringLoader::LoadLC(R_IRAPP_LOADING_BUFFERING);
	if(!iWaitDialog)	
		{
		iWaitDialog =  new ( ELeave ) CAknWaitDialog( NULL,ETrue);
		iWaitDialog->SetCallback(this);
		iWaitDialog->ExecuteLD(R_IRAPP_PROGRESS_DIALOG);
		iWaitDialog->SetTextL(*bufferText);
		iBufferigSteamText=EFalse;
		}
	CleanupStack::PopAndDestroy(bufferText);

	IRLOG_DEBUG( "CIRNowPlayingWrapper::CreateWaitDialogL - exiting." );
	}
// ---------------------------------------------------------------------------
// DestroyWaitDialogL()
// Destroys The WaitDialog
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::DestroyWaitDialogL()
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DestroyWaitDialogL - Entering" );

		if(iWaitDialog)
		{
		iWaitDialog->ProcessFinishedL();
		iWaitDialog=NULL;
		iDialogCanceled=ETrue;
		iBufferingRequestCancelled=ETrue;
		}
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::DestroyWaitDialogL - exiting." );
	}
	
// ---------------------------------------------------------------------------
// SetProgress()
// Sets the Progress Value
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::SetProgress(TBool aProgress)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetProgress - Entering" );
	iProgress=aProgress;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetProgress - exiting." );
	}
// ---------------------------------------------------------------------------
// GetChannelConnetedInfo()
// Returns Whether the channel connected or not
// ---------------------------------------------------------------------------
//

TBool CIRNowPlayingWrapper::GetChannelConnetedInfo()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetChannelConnetedInfo - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetChannelConnetedInfo - exiting." );
	return iChannelConnected;
	}
// ---------------------------------------------------------------------------
// HandleNetworkDisconnected()
// Function called when network got disconnected While Buffering
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::HandleNetworkDisconnected()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::HandleNetworkDisconnected - Entering" );

	iAppUi.iNetworkController->ResetConnectionStatus();
	if(!iAppUi.iIsDisconnected)
	{
	if(iAppUi.iMainView->iSyncReq)
		{
		iAppUi.iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
		TRAP_IGNORE(DestroyWaitDialogL();)
		}
	if(iConnectToServer && !iProgress)
		{
		TRAP_IGNORE(DestroyWaitDialogL();)
		TRAP_IGNORE(CancelNetworkRequestL();)	
		}
	if(iProgress)
		{
		//if in progress bar view, progress bar is cancelled
		TRAP_IGNORE(DestroyWaitDialogL();)
		ProgressBarCancel();
		}
	if ( iAppUi.iIsdsWrapper->GetISDSInstance() )
	    {
	    if(iAppUi.iIsdsWrapper->GetListenRequest())
		  {
	    TRAP_IGNORE(DestroyWaitDialogL();)
		  }
	    TRAP_IGNORE(iAppUi.iIsdsWrapper->DestroyWaitDialogL();)
	    iAppUi.iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
	    iAppUi.iIsdsWrapper->GetISDSInstance()->ReleaseResources();
	    }
	    }
	IRLOG_DEBUG( "CIRNowPlayingWrapper::HandleNetworkDisconnected - exiting." );
	}
// ---------------------------------------------------------------------------
// HandleNetworkEstablished()
// Function called when network Connection Established In ALr
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::HandleNetworkEstablished()
	{
	 if(!iPlaying)
	 {
	  iLastConnectedUrl.Zero();
	 	iLastPlayedUrl.Zero();
	if((iAppUi.GetNowPlayingViewInstance()->iContainer))
	{
	 if(iSyncPreset)
		 {
		 	iAppUi.iNowPlayingView->SetRockerIndex(
			iAppUi.iNowPlayingView->GetPreviousIndex());
			TRAP_IGNORE(TriggerCommandL(iAppUi.GetNowPlayingViewInstance()->GetCommandId());)	
            return;
		 }
	}
	  //iLastPlayedUrl.Zero()
	if(iAppUi.iIsdsWrapper->GetListenRequest())
		  {
IRRDEBUG2("CIRNowPlayingWrapper:: HandleNetworkEstablished1", KNullDesC); 

		  if(!(iAppUi.GetNowPlayingViewInstance()->iContainer))
			  {
			  if(!iBufferingRequestCancelled)
				  {
				  iLaunchNowplaying=EFalse;	
				  }
			  else
				  {
				  iLaunchNowplaying=ETrue;	
				  }
			  }
		  if(iAppUi.GetNowPlayingViewInstance()->iContainer)
			  {
			iAppUi.iNowPlayingView->SetRockerIndex(
			iAppUi.iNowPlayingView->GetPreviousIndex());
			 TRAP_IGNORE(TriggerCommandL(iAppUi.GetNowPlayingViewInstance()->GetCommandId());)	
			  }
		  else
			  {
			  TRAP_IGNORE(TriggerCommandL(EListenCmd);)	
			  }
		  }
	  else if(iConnectToServer && !iPlaying)
		  {

		  if(!(iAppUi.GetNowPlayingViewInstance()->iContainer))
			  {
			  if(!iBufferingRequestCancelled)
				  {
				  iLaunchNowplaying=EFalse;	
				  }
			  else
				  {
				  iLaunchNowplaying=ETrue;	
				  }
			  }
		  else
			  {
			  iLaunchNowplaying=ETrue;		
			  }
		  iNotify=ETrue;
		  iQualityRequired=-1;
		  TRAP_IGNORE(ConnectToChannelL(*iNowPlayingPreset);)
		  }
	 	  
	 }
	
	}

// ---------------------------------------------------------------------------
// Triggers the command to view handling
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::TriggerCommandL( TInt iCommandId)
	{
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::TriggerCommandL - Entering" );
	TVwsViewId viewId( KNullUid, KNullUid );
	CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
	TInt err = appUi->GetActiveViewId( viewId );
	RProcess process;
	TSecureId id = process.SecureId();
	// TO be DOne GetActiveViewId doesn't always work (when switched back to VRA through RadioLauncher icon
	if( !err && viewId.iAppUid.iUid == id.iId )
		{
		CAknView* view = appUi->View( viewId.iViewUid );
		if( view )
			{
			view->ProcessCommandL( iCommandId );
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::TriggerCommandL - Exiting" );
	}

// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::RemoveWhiteSpace( TDes& aBuf )
// Called from HandleMetaDataReceivedL.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::RemoveWhiteSpace( TDes& aBuf )
	{
	_LIT(KNewLine, "\n");
	_LIT(KCharLine, "\r");
	_LIT(KTab, "\t");
	_LIT(KSpace, " ");
	
	TInt index = aBuf.Find(KNewLine);
	while (index != KErrNotFound)
		{
		aBuf.Replace(index, 1, KSpace);
		index = aBuf.Find(KNewLine);
		}
		
	index = aBuf.Find(KCharLine);
	while (index != KErrNotFound)
		{
		aBuf.Replace(index, 1, KSpace);
		index = aBuf.Find(KCharLine);
		}
	
	index = aBuf.Find(KTab);
	while (index != KErrNotFound)
		{
		aBuf.Replace(index, 1, KSpace);
		index = aBuf.Find(KTab);
		}

	aBuf.TrimAll();
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingWrapper::CheckCallStatusL
// Checks the Call status Whether it is Incoming Call or OutGoing Call
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::CheckCallStatusL()
{
CTelephony* telephony = CTelephony::NewLC();

CTelephony::TCallInfoV1 callInfoV1;
CTelephony::TCallInfoV1Pckg callInfoV1Pckg( callInfoV1 );

CTelephony::TCallSelectionV1 callSelectionV1;
CTelephony::TCallSelectionV1Pckg callSelectionV1Pckg( callSelectionV1 );

CTelephony::TRemotePartyInfoV1 remotePartyInfoV1;
CTelephony::TRemotePartyInfoV1Pckg remotePartyInfoV1Pckg( remotePartyInfoV1  );

callSelectionV1.iLine = CTelephony::EVoiceLine;
callSelectionV1.iSelect =CTelephony::EInProgressCall;

telephony->GetCallInfo( callSelectionV1Pckg, callInfoV1Pckg, remotePartyInfoV1Pckg );
CTelephony::TCallDirection iDirection1;
iDirection1=CTelephony::EMobileOriginated;

if(remotePartyInfoV1.iDirection==iDirection1)
{
 iPlaying=ETrue;
 CleanupStack::PopAndDestroy( telephony );	
 return ETrue;
 	
}
CleanupStack::PopAndDestroy( telephony );
IRLOG_DEBUG( "CIRNowPlayingWrapper::NotifyActiveNetworkObserversL - Entering" );
return EFalse;
}

// ---------------------------------------------------------------------------
//Notifies all observers whose network request is active
// to reissue the request  
//NotifyActiveNetworkObserversL()
// ---------------------------------------------------------------------------

void CIRNowPlayingWrapper::NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::NotifyActiveNetworkObserversL - Entering" );

	switch(aEvent)
		{
		case ENetworkConnectionDisconnected:
			{
			if(!iAppUi.iIsDisconnected)
				{
				if(iAppUi.GetNowPlayingViewInstance()->iContainer)
					{
					if(iSyncPreset)
					{
					if((iAppUi.iNowPlayingView->iContainer))
						{
					iAppUi.iNowPlayingView->SetRockerIndex(
								iAppUi.iNowPlayingView->GetPreviousIndex());
						}
						
					}
					IRRDEBUG2("CIRNowPlayingWrapper::ENetworkConnectionDisconnected1 - Entering ", KNullDesC); 
					if(iAppUi.iIsdsWrapper->GetListenRequest()||iSyncPreset)
						{	
						TRAP_IGNORE(DestroyWaitDialogL();)
						iAppUi.iIsdsWrapper->GetISDSInstance()->IRISDSCancelRequest();
						}
					if(iPlaying && !iProgress)
						{

						StopNow();
						if(iAppUi.ActiveView() == KIRMainChoiceViewID)								 
							{
							iAppUi.iMainView->GetMainContainer()->UpdateAllL();	
							}
						}	
					else
						{
						if(iConnectToServer && !iProgress)
							{

							CancelNetworkRequestL();
							DestroyWaitDialogL();
							}
						if(iProgress)
							{
							//if in progress bar view, progress bar is cancelled
							DestroyWaitDialogL();
							ProgressBarCancel();
							}
						}
					}

				}
			}
			break;
			
		case ENetworkConnectionEstablished:
			{
			if(!iAppUi.IsCallActive())
				{
				if(iAppUi.iNetworkController->IsHandingOverConnection())
					{
					iAppUi.GetNowPlayingViewInstance()->SetRequestPendingWhenNoNetWork(EFalse);
					iAppUi.GetNowPlayingViewInstance()->SetRequestPendingWhenNoNetWorkForNextOrPrev(EFalse);	
					if(!iPlaying)
					{

					HandleNetworkEstablished();
					}
					}
			else if(iAppUi.GetNowPlayingViewInstance()->GetRequestPending())
					{
					PlayAndStopL();	
					}
			else if(iAppUi.GetNowPlayingViewInstance()->GetRequestPendingForNextOrPreV())
					{
					if(iContextViews==EFav||iContextViews==EPls)
						{
						iAppUi.GetNowPlayingViewInstance()->ConnectToPresetL();
						iAppUi.GetNowPlayingViewInstance()->StartTimerL();
						}
					else
						{
						iAppUi.GetNowPlayingViewInstance()->ConnectToPresetL();
						}
					}
				}
			ResetPendingRequests(EFalse);				
			}
		break;
		default:
		break;			
		}

	IRLOG_DEBUG( "CIRNowPlayingWrapper::NotifyActiveNetworkObserversL - Exiting" );
}



// -----------------------------------------------------------------------------
// Notified by network controller when user cancels network connection, to reset 
// the pending requests  
// ResetPendingRequests()
// -----------------------------------------------------------------------------
void CIRNowPlayingWrapper::ResetPendingRequests(TBool aValue)
	{
	iAppUi.GetNowPlayingViewInstance()->SetRequestPendingWhenNoNetWork(aValue);
	iAppUi.GetNowPlayingViewInstance()->SetRequestPendingWhenNoNetWorkForNextOrPrev(aValue);	
	}

/**** Added for ASF streaming - begin *****/

// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::Asf_ErrorConnecting()
// Called when there is an error when connecting to the channel server  @Param aErrorCode Indicates the type of error as described in irstreamsourceerrors.h---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::Asf_ErrorConnecting(TInt aErrorCode)
	{
	IRLOG_DEBUG2( "CIRNowPlayingWrapper::Asf_ErrorConnecting(aErrorCode=%d)", aErrorCode );
	ErrorConnecting( aErrorCode );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::Asf_ErrorConnecting - Exiting." );
	}

// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::Asf_ConnectionEstablished(TInt aFeedValue)
// Called to indicate that the connection to channel server is sucessful
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::Asf_ConnectionEstablished()
	{
    IRLOG_INFO( "CIRNowPlayingWrapper::Asf_ConnectionEstablished - Entering." );
	ConnectionEstablished();
	IRLOG_DEBUG( "CIRNowPlayingWrapper::Asf_ConnectionEstablished - Exiting." );
	}

// ---------------------------------------------------------------------------
// MIRStreamSourceObserver::Asf_UpdateProgress()
// Called to provide teh buffer values @param aFeedValue used to pump the buffering value
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::Asf_UpdateProgress(TInt aFeedValue)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::Asf_UpdateProgress - Entering" );
	UpdateProgress(aFeedValue);
	IRLOG_DEBUG( "CIRNowPlayingWrapper::Asf_UpdateProgress - Exiting." );
	}

// ---------------------------------------------------------------------------
// MCrtlCmdCommand::Asf_HandleMetaDataReceivedL()
// Invoked when meta data is received. @param  aMetaData The meta data that was received.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingWrapper::Asf_HandleMetaDataReceivedL( const CIRMetaData& aMetaData )
    {
	IRLOG_DEBUG( "CIRNowPlayingWrapper::Asf_HandleMetaDataReceivedL  - Entering" );
	HandleMetaDataReceivedL( aMetaData );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::Asf_HandleMetaDataReceivedL - Exiting." );
    }

/**** Added for ASF streaming - end *****/

// ---------------------------------------------------------------------------
//	 UpdateNmsLogEventsL()
//	 Updates Nms Log events
// ---------------------------------------------------------------------------	
	
void CIRNowPlayingWrapper::UpdateNmsLogEventsL(const TDesC& aNmsType)
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::UpdateNmsLogEventsL - Entering" );

	iReportGenerator->UpdateNmsCurrentNetwork(iAppUi.GetCurrentOperatorValue());
	iReportGenerator->UpdateNmsHomeOperator(iAppUi.GetHomeOperatorValue());
	iReportGenerator->UpdateNmsChannelID(iNowPlayingPreset->GetId());
	iReportGenerator->UpdateNmsType(aNmsType);
	iReportGenerator->SessionStartedL();
	iReportGenerator->WriteNmsLogtoXmlL();

	IRLOG_DEBUG( "CIRNowPlayingWrapper::UpdateNmsLogEventsL - exiting." );
	}

/* This is reuired Only Accp or Acc type check
// ---------------------------------------------------------------------------
// GetSupportedMimeTypeL()
// Function Evaluates the Suppoeted Mime Types
// ---------------------------------------------------------------------------
//

TInt CIRNowPlayingWrapper::GetSupportedMimeTypeL()
	{
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetSupportedMimeTypeL - Entering" );
//	_LIT(KAudiContent,"aacp")//aacp
    TInt findContent=KErrNotFound;
	TInt i,ii,j;
    CMMFControllerPluginSelectionParameters *cs=CMMFControllerPluginSelectionParameters::NewLC();
    CMMFFormatSelectionParameters * fs = CMMFFormatSelectionParameters::NewLC();
    cs->SetRequiredPlayFormatSupportL(*fs);
    cs->SetRequiredRecordFormatSupportL(*fs);
    
    RMMFControllerImplInfoArray controllers;
    CleanupResetAndDestroyPushL(controllers);
    cs->ListImplementationsL(controllers);
    TBuf<KSize>z1;
    z1.Copy(iNetwork->ContentTypeL());
    TBool checkMime=EFalse;
    TInt contrCount = controllers.Count();
    for(i=0;i<controllers.Count();i++)
        {
        //supported play formats
        const RMMFFormatImplInfoArray &pf=controllers[i]->PlayFormats();
		TInt pfCount = pf.Count();
        for(ii=0;ii<pf.Count();ii++)
            {
            TBuf<KSize> z;
            //file extensions
            const CDesC8Array &fe=pf[ii]->SupportedFileExtensions();
            //MIME types
            const CDesC8Array &mt=pf[ii]->SupportedMimeTypes();
            for(j=0;j<mt.Count();j++)
                {
                z.Copy(mt[j]);
                if(z==z1)
	                {
	                checkMime=ETrue;
	                }
                }
            }
        }
    CleanupStack::PopAndDestroy(&controllers);//controllers
    CleanupStack::PopAndDestroy(fs);
    CleanupStack::PopAndDestroy(cs);

     if(checkMime)
	     {
	      findContent=KErrNone;	
	     }
     else
	     {
	      findContent=KErrNotFound;	
	     }
   	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetSupportedMimeTypeL - exiting" );
   	return findContent;
	}
	
	*/
// ---------------------------------------------------------------------------
// SavePreviousIndex()
// Sets when Listen from Isds is Done
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::SetListenFromIsdsValue(TInt aListenFromIsds  )
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetListenFromIsdsValue - Entering" );
	iListenFromIsds=aListenFromIsds;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetListenFromIsdsValue - exiting" );
	}
// ---------------------------------------------------------------------------
// SetWhenUserCancelsBufferingWaitBar()
// Sets When User Cancels Buffering
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::SetWhenUserCancelsBufferingWaitBar(TInt aDialogCanceled  )
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetWhenUserCancelsBuffering - Entering" );
	iDialogCanceled=aDialogCanceled;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetWhenUserCancelsBuffering - exiting" );
	}
// ---------------------------------------------------------------------------
// SetWhenUserCancelsIsdsBufferingWaitBar()
// Sets When User Cancels Buffering WaitBar From Isds Listen
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::SetWhenUserCancelsIsdsBufferingWaitBar(TInt aCancelFrmIsds  )
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetWhenUserCancelsIsdsBufferingWaitBar - Entering" );
	iCancelFrmIsds=aCancelFrmIsds;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetWhenUserCancelsIsdsBufferingWaitBar - exiting" );
	}
// ---------------------------------------------------------------------------
// GetPreviousIndex()
// Returns When User Cancels Isds Buffering WaitBar
// ---------------------------------------------------------------------------
//

TBool CIRNowPlayingWrapper::GetWhenUserCancelsIsdsBufferingWaitBar( )
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetWhenUserCancelsIsdsBufferingWaitBar - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetWhenUserCancelsIsdsBufferingWaitBar - exiting" );
	return iCancelFrmIsds;
	}
// ---------------------------------------------------------------------------
// SetLaunchNowPlayingView()
// Sets the varible if NowPlaying is active
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::SetLaunchNowPlayingView(TBool aLaunchNowplaying  )
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetLaunchNowPlayingView - Entering" );
	iLaunchNowplaying=aLaunchNowplaying;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetLaunchNowPlayingView - exiting" );
	}
// ---------------------------------------------------------------------------
// GetLaunchNowPlayingView()
// Returns the Nowplaying Vies Active State
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingWrapper::GetLaunchNowPlayingView()
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetLaunchNowPlayingView - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetLaunchNowPlayingView - exiting" );
	return iLaunchNowplaying;
	}
// ---------------------------------------------------------------------------
// SetFavFocus()
// Sets the foucs in Mainview
// ---------------------------------------------------------------------------
//

void CIRNowPlayingWrapper::SetFavFocus(TInt  aFavIndex  )
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetFavFocus - Entering" );
	iFavIndex=aFavIndex;
	IRLOG_DEBUG( "CIRNowPlayingWrapper::SetFavFocus - exiting" );
	}
// ---------------------------------------------------------------------------
// GetFavFocus()
// Returns the Current Focus to set
// ---------------------------------------------------------------------------
//
TInt CIRNowPlayingWrapper::GetFavFocus()
	{
	
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetFavFocus - Entering" );
	IRLOG_DEBUG( "CIRNowPlayingWrapper::GetFavFocus - exiting" );
	return iFavIndex;
	}
