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

#ifndef IRNOWPLAYINGWRAPPER_H
#define IRNOWPLAYINGWRAPPER_H

#include <e32base.h>
#include <BADESCA.H>
#include <AknProgressDialog.h>

#include "irstreamsourceobserver.h"
#include "irctrlcommand.h"
#include "IRNetworkControllerObserver.h" //TIRConnectionType Defined
#include "irsessionlogger.h" //TConnectedFrom defined
#include <Etel3rdParty.h>
#include "IRActiveNetworkObserver.h"
class CIRStreamSource;
class CIRCtrlCmdObserver;
class CIRMediaClient;
class CIRReportGenerator;
class CIRSettings;
class CIRIsdsWrapper;
class CIRMetaData;
class CIRIsdsPreset;
class CIRUi;
class CAknWaitDialog;
class CIRAsfPlayer;

//CONSTANTS

//Ulr Validation
const TInt KMaxBufferSize = 256;
const TInt KMaxUrlSize = 256;
enum TSyncState
	{
	ESyncStateNull,
	ESyncReqPending,
	ESyncPsChanged,
	EPsConnectionFailed,
	EPsConnectionSuccessful
	};

//for nowplaying view

enum TIRViews
{
  ELastplayed=0,
  EFav,
  ESearch,
  EHistoryView,
  EStations,
  EBrowseByTopStations,
  EPls
};	


class CIRNowPlayingWrapper:public CBase,
						   public MIRStreamSourceObserver,
						   public MIRCtrlCmdObserver,
						   public MProgressDialogCallback,
						   public MIRActiveNetworkObserver
	{
	
public:

	/* Function : NewL
	 * function to return an instance of CIRIsdsWrapper
	 * Two phase constructor
     */
 	static CIRNowPlayingWrapper* NewL(CIRUi& aAppUi,CIRSettings& aIRSettings,
 								CIRIsdsWrapper& aIsdsWrapper);        

    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CIRIsdsWrapper object, which will draw itself
     * to aRect.
     */
    static CIRNowPlayingWrapper* NewLC(CIRUi& aAppUi,CIRSettings& aIRSettings,
    						CIRIsdsWrapper& aIsdsWrapper);        

    /**
     * ~CIRIsdsWrapper
     * Destructor.
     */
    ~CIRNowPlayingWrapper();
 
    /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CIRSearchView object.
     */
    void ConstructL();
		
	/**
     * CIRSearchView.
     * C++ default constructor.
     */
    CIRNowPlayingWrapper(CIRUi& aAppUi,CIRSettings& aIRSettings,CIRIsdsWrapper& aIsdsWrapper);  
	
	//MCLASSES FUNCTIONS
	
	/** 
	 *  MIRStreamSourceObserver::OpenComplete()
	 *  Called when connected to the channel server  
	 *  To implement the fill of buffer
	 */
	void OpenComplete(); 

	/** 
	 *  MIRStreamSourceObserver::ErrorConnecting()
	 *  Called when there is an error when connecting to the channel server  
	 *  @Param aErrorCode Indicates the type of error as described in
	 * 		   irstreamsourceerrors.h
	 */
	void ErrorConnecting(TInt aErrorCode);

	/** 
	 *  MIRStreamSourceObserver::PlayNow()
	 *  Called to indicate to the player to start playing
	 */
	void PlayNow();

	/** 
	 *  MIRStreamSourceObserver::Play()
	 *  Triggers the play after reconnecting to the channel
	 */
	void Play();
	
	/** 
	 *  MIRStreamSourceObserver::StopBuffering()
	 *  Called to indicate to the player to stop buffering
	 */
	void StopBuffering();

	/** 
	 *  MIRStreamSourceObserver::ConnectionEstablished(TInt aFeedValue)
	 *  Called to indicate that the connection to channel server is sucessful
	 */
	void ConnectionEstablished();

	/** 
	 *  MIRStreamSourceObserver::UpdateProgress()
	 *  Called to provide teh buffer values
	 *  @param aFeedValue used to pump the buffering value
	 */
	void UpdateProgress(TInt aFeedValue);

    /**
     * Function : HandleMetaDataReceivedL
     * Invoked when meta data is received.
     * @param   aMetaData   The meta data that was received.
     */
    void HandleMetaDataReceivedL( const CIRMetaData& aMetaData );


	// Callbacks for ASF Player

	/** 
	 *  MIRStreamSourceObserver::ErrorConnecting()
	 *  Called when there is an error when connecting to the channel server  
	 *  @Param aErrorCode Indicates the type of error as described in
	 * 		   irstreamsourceerrors.h
	 */
	void Asf_ErrorConnecting(TInt aErrorCode);

	/** 
	 *  MIRStreamSourceObserver::Asf_ConnectionEstablished(TInt aFeedValue)
	 *  Called to indicate that the connection to channel server is sucessful
	 */
	void Asf_ConnectionEstablished();

	/** 
	 *  MIRStreamSourceObserver::Asf_UpdateProgress()
	 *  Called to provide teh buffer values
	 *  @param aFeedValue used to pump the buffering value
	 */
	void Asf_UpdateProgress(TInt aFeedValue);

    /**
     * Function : Asf_HandleMetaDataReceivedL
     * Invoked when meta data is received.
     * @param   aMetaData   The meta data that was received.
     */
    void Asf_HandleMetaDataReceivedL( const CIRMetaData& aMetaData );

	// MIRCtrlCmdObserver functions

    /**
     * Function : MCrtlCmdCommand
     * Function is pure virtual function which used to pass commands
     * @param TIRControlCommmand represents the commands,which determines 
     * what action to be taken
     * @param aValue represents any integer value if any, like buffering
     * percentage
     */
    void MCtrlCommand( TIRControlCommmand aCommand, TInt aValue );

    /**
     * Function : FetchVolume
     * Function is pure virtual function which sents Request to get the
     * current volume index
     * @return volume index
     */
    TInt FetchVolume();
    
    /**
     * Function : DoAudioFadeOut
     * Function is pure virtual function, it is used to sent request start 
     * audio Fade Out
     */
    void DoAudioFadeOut();
     
     /**
     * Function : PlayerChanged
     * Function is pure virtual function, it is Notified Wnen Audio Player Changes
     */
    void PlayerChanged();
    
    /**
    * ConnectToChannelL()
    * This function is called to connect to a particular channel
    */
	void ConnectToChannelL(CIRIsdsPreset& aPreset);
	
	/**
	* SelectUrlUsingBitRate()
	* Selects the proper URL based on settings of quality according to current network
	*/
	void SelectUrlUsingBitRateL(CIRIsdsPreset& aPreset);
	
	/**
	* DisplayErrorNoteL()
	* Display the error note
	*/
 	void DisplayErrorNoteL(TInt aResourceId);

 	/**
 	* DoConnectToServerL()
 	* Connects to Server.
 	*/ 
	void DoConnectToServerL(const TDesC8& aUrl);
	
	/**
	* GetAvailableBitRate()
	* Selects proper bit rate from the settings
	*/
 	TInt GetAvailableBitRate(TInt aQualiyRequired,const CIRIsdsPreset& aPreset) const;
	
	/**
	* LogStartSession()
	* Starts the session log session
	*/
	void LogStartSession();
	
	/**
	* SetCurrentChannelID()
	* Function sets currently playing channel ID
	*/
	void SetCurrentChannelID();
	
	/**
	* ConnectToServerL()
	* Starts playing the selected stream
	*/
	void ConnectToServerL();

	/**
	* SetCurrentConnectionSource
	* Sets from where the current connection to the channel server is established.
	*/
	void SetCurrentConnectionSource(TConnectedFrom aConnectedFrom);

	/**
	* DoErrorConnectingL
	* Called by ErrorConnectingL when there is an error in connectiong
	*/
	void DoErrorConnectingL(TInt aErrorCode);
	
	/**
	* ProgressBarCancelL()
	* Cancels the Network request and stops the buffering on progressbar cancel
	*/
	void ProgressBarCancel();
	
	/**
	* DoPlayingStateChangedL()
	* Handles internal state change and P&S when playing is stopped or started
	*/
	void DoPlayingStateChanged( TBool aPlaying );
	
	/**
	* DoPlayL()
	* First try to reconnect to the last played channel url
	*/
	void DoPlayL();
	
	/**
	* StopNow() 
	* Function calls stop functionality
	*/
	void StopNow();
	
	/**
	* ClearMetaDataL
	* Clears the current meta data information.
	*/
	void ClearMetaDataL();
	
	//TInt GetSupportedMimeTypeL()
	/**
	* ListenToChannelL()
	* Requests for listening to a particular channel
	*/
	void ListenToChannelL(CIRIsdsPreset* aPreset);
	
	/**
	* MetaData()
	* Returns the current meta data information.
	*/
	const CIRMetaData& MetaData() const;
	
	/**
	* DialogDismissedL()
	* Called when the refresh dialog is dismissed.
	* @param aButtonId The button that was used to dismiss the dialog.
	*/
	void DialogDismissedL( TInt aButtonId );
	
	/**
	* BufferingTimerExpiredL()
	* Called when the buffering times out
	*/
	static TInt BufferingTimerExpired(TAny* aPtr);
	
	/**
	* HandlePlayError()
	* Handles play error || should called only through MCtrlCommand
	*/
	void HandlePlayError();

	/**
	* SetOverlay()
	* sets over lay enable flag
	*/
	void SetOverlay(TBool aPresent);

	/**
	* PlayAndStopL()
	* Performs play and stop functionality
	*/
	void PlayAndStopL();
	
	/**
	* LoadTerminateSession()
	* The loading cancel result in session log end
	*/
	void LoadTerminateSessionL();

    /**
    * GetMediaClientInstance()
    * Returns the Media Client Instance
    */
	CIRMediaClient* GetMediaClientInstance();
    
	/**
	* GetReportGeneratorInstance()
	* Returns the ReportGenerator Instance
	*/
	CIRReportGenerator* GetReportGeneratorInstance();
	
	/**
	* GetTerminatedLogOnCancelState()
	* Returns the iTerminatedLogOnCancel Value
	*/
	TBool GetTerminatedLogOnCancelState();
	
	/**
	* GetSessionStartState()
	* Returns the iSessionStart Value
	*/
	TBool GetSessionStartState();

	/**
	* SetSessionStartState()
	* Sets the iSessionStart Value
	*/
	void SetSessionStartState(TBool aSessionStart);
	/**
	* CancelNetworkRequestL()
	* Cancels the Network Request
	*/
	void  CancelNetworkRequestL();
	/**
	* SetVolumeL()
	* Set the Volume of the Player Via Hardware Media Keys
	*/
	void 	SetVolumeL(TInt aIndex);

	/**
	* IsProgress()
	* Returns the iProgress Value
	*/
	TBool IsProgress();

	/**
	* SetConnectToServerState
	* Sets the iConnectToServer Value
	*/
	void SetConnectToServerState(TBool aConnectToServer);

	/**
	* GetConnectToServerState
	* Returns the iConnectToServer Value
	*/
	TBool GetConnectToServerState();
		
	/**
	* GetPrevVolumeLevel
	* Returns the Prev Volume When Message is Received
	* Or Any NetWork Events
	*/
	TInt  GetPrevVolumeLevel();
		
	/**
	* SetPrevVolumeLevel
	* Sets the Volume For handling the Network Events
	*/

    void  SetPrevVolumeLevel(TInt aPrevVolume);
    	
	/**
	* SetMetadataL
	* Sets the Metadata
	*/

    void  SetMetadataL();
    	
	/**
	* PublishDataL
	* Publish the Metadata Required for History
	*/
  	void  PublishDataL();
  	
  	
	/**
	* StopPlayer
	* Stop the Player
	*/
  	void  StopPlayer();
  	
 	/**
	* SetMedadataForErrotrConditions
	* Sets the Metadata Flag During Error Conditions
	*/
  	void  SetMedadataForErrotrConditions(TBool aDisplayMetaData);
  	
  	/**
	* GetMedadataForErrotrConditions
	* Returns the Error condition to Nowplaying View
	*/
  	TBool  GetMedadataForErrotrConditions();
  	/**
	* GetMarquee
	* Returns the iMarQuee which is Set During HandleMetadataReceived
	*/
  	TBool  GetMarquee();

  	/**
	* SetMarquee
	* Sets the Marquee value required for NowPlaying view
	*/
  	void  SetMarquee(TBool aMarquee);
    /**
	* SetView
	* Set by Every View to its TIRViews when Listen is Done
	*/
  	void  SetView(TIRViews aContextViews);

	/**
	* GetView
	* Returns the View Where Listen Has Done
	*/
  	TIRViews  GetView();
  	
	/**
	* SetAudioVolume()
	* Call CPeriodic after the period is over
	* @param volume level.
	*/
	void SetAudioVolume(TInt aValue);


	/**
	* CreateWaitDialogL()
	* Called whenever Connecting to channel
	*/
	void CreateWaitDialogL();
	
	/**
	* DestroyWaitDialogL()
	* Destroys the Wait Dialog
	*/
	void DestroyWaitDialogL();
	
	/**
	* SetProgress()
	* Sets The Progress Value
	*/
	void SetProgress(TBool aProgress);

	/**
	* GetChannelConnetedInfo()
	* Returns Whether the channel connected or not
	*/
	TBool GetChannelConnetedInfo();
	
	/**
	* HandleNetworkDisconnected()
	* Function called when network got disconnected While Buffering
	*/
	void HandleNetworkDisconnected();

	/**
	* HandleNetworkEstablished()
	* Function called when network Connection Established In ALr
	*/

	void HandleNetworkEstablished();
	
	  /**
	 *  Removes whitespace characters. 
	 *  Called from HandleMetaDataReceivedL.
	 *  @param string to be cleaned.
	 */
    void RemoveWhiteSpace( TDes& aBuf );

    /**
	 * 	Checks The Call status. 
	 *  Checks the Call status Whether it is Incoming Call or OutGoing Call
	 */
	TBool CheckCallStatusL();
	//from MIRActiveNetworkObserver
	
	/**
	 * Notifies all observers whose network request is active
	 * to reissue the request  
	 * NotifyActiveNetworkObserversL()
	 */	
	 void NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent);

	 /**
	 * Notifies all observers whose network request is active
	 * to reset the pending request status  
	 * ResetPendingRequests()
	 */	
	 void ResetPendingRequests(TBool aValue);


	/**
	*TriggerCommandL.()
	*
	* triggering the Commands to the view
	*/
	void TriggerCommandL(TInt iCommandId);


	/**
	* GetSessionStartedFlag()
	* Returns the iSessionStarted Flag Value
	*/
	TBool GetSessionStartedFlag();
	/**
	* SetSessionStartedFlag()
	* Sets the iSessionStartedFlag  Value
	*/
	void SetSessionStartedFlag(TBool aSessionStartedFlag);
	/**
	* UpdateNmsLogEventsL()
	* Updates Nms Log events
	*/
	void UpdateNmsLogEventsL(const TDesC& aNmsType);
	
	 /**
     * Function :SetListenFromIsdsValue()
     * Sets when Listen from Isds is Done 
     * @param aListenFromIsds Sets when Listen from Isds is Done
     */
	 void SetListenFromIsdsValue(TInt aListenFromIsds);  
   
   	 /**
     * Function :SetListenFromIsdsValue()
     * Sets When User Cancels Buffering 
     * @param aDialogCanceled Sets When User Cancels Buffering
     */
	 void SetWhenUserCancelsBufferingWaitBar(TInt aDialogCanceled);  

   	 /**
     * Function :SetWhenUserCancelsIsdsBufferingWaitBar()
     * Sets When User Cancels Buffering WaitBar From Isds Listen
     * @param aCancelFrmIsds Sets When User Cancels Buffering
     */
	 void SetWhenUserCancelsIsdsBufferingWaitBar(TInt aCancelFrmIsds);  

	 /**
     * Function :GetWhenUserCancelsIsdsBufferingWaitBar()
     * Returns When User Cancels Isds Buffering WaitBar
     * @return Returns Cancels Isds Buffering WaitBar
     */
	 TInt GetWhenUserCancelsIsdsBufferingWaitBar();  
	/**
     * Function :SetLaunchNowPlayingView()
     * Sets the varible if NowPlaying is active
     * @param aLaunchNowplaying the variable being set
     */
	 void SetLaunchNowPlayingView(TBool aLaunchNowplaying  );
 	 
 	 /**
     * Function :GetLaunchNowPlayingView()
     * Returns the active state of NowPlaying View
     * @return Returns the Nowplayinf View active State
     */
	 TBool GetLaunchNowPlayingView(); 
	/**
     * Function :SetFavFocus()
     * Sets the foucs in Mainview
     * @param aFavIndex Focus to set in MainView
     */
	 void SetFavFocus(TInt aFavIndex  );
 	
 	 /**
     * Function :GetFavFocus()
     * Returns the Current Focus to set
     * @return Returns value to set focus  in MainView
     */
	 TInt GetFavFocus(); 

private:
   //Instance of MediaClient
	CIRMediaClient* iPlayer;


	//Instance of StreamSource
	CIRStreamSource* iNetwork;
	
	//instance of command channel
	CIRCtrlCmdObserver* iChannel;
	
	
	
	//Session Log
	CIRReportGenerator *iReportGenerator;
	
	/**
	* iIRSettings
	* Instance of CurrentOperationSettings
	*/
	CIRSettings& iIRSettings;
	
 	/**
	* iIsdsWrapper
	* Instance of ISDSWrapper
	*/
    CIRIsdsWrapper& iIsdsWrapper;
    
    /**
    * CIRUi& aAppUi
    * Instance of CIRUi
    */
    CIRUi& iAppUi;
    
    /**
     * Current meta data information.
     * Owned.
     */
    CIRMetaData* iMetaData;
    
	//index for URLs in iURLArray array
	TInt iURLCount;
	
	//holds the array of URLs
	CDesCArrayFlat* iURLArray;
	
	//multiple URL selection will be enabled only before connecting
	//to particular channel server once connected this disabled until
	//next time you try to connect to the channel this is monitored
	//by this flag
	TBool iMultipleURLSelect;
	
	TBool iConnectionRequestCancelled;
	
	//Indiacates that it is reconnecting to the channel
	TBool iReConnecting;

	//last connected channels preset id
	TInt iLastConnectedPresetId;
	
	//last connected channel url
	TBuf<KMaxBufferSize> iLastConnectedUrl;
	
	//Last played URL
	TBuf<KMaxUrlSize> iLastPlayedUrl;
	
	//last played channel's unique id
	TInt iLastConnectedPresetUniqId;
	
	//checks if session log should be terminated on loading cancel
	TBool iTerminatedLogOnCancel;
	
	// Used for checking the Audio has been fadein/fadeout
	TBool iAudioFadein;
	
	// URL
	TBuf<KMaxUrlSize> iUrl;
	
	//checks whether a session is started
	TBool iSessionStart;
	
	//this variable check whether the session should actually end or not
	//in case of multiple url select
	TBool iSessionShouldNotEnd;

	// Checks whether channel is Connected to Server or not
	TBool iConnectToServer;

	// Check the Connection
//	TSyncState iSyncState
	
	//stores the channel id of currently playing channel
	TInt iChannelID;

	//Bitrate to subscribe
    TInt iBitRateValue;
    
 	// Checks whether the channel is connected or not
	TBool iChannelConnect;

	//Holds the Network connection status
	TBool iIsConnected;

	//Connected from information is stored in iConnectedFrom
	TConnectedFrom iConnectedFrom;

	// Stores the error code 
	TInt iConnErrCode;

	// checks the Status of the Progress bar
	TBool iProgress;	

	//Flag used to test Radio Playback 
     TBool iPlayBack;
    //Unconnected URL
	TBuf<KMaxUrlSize> iUnconnectedUrl;
	
	
	/**
	* iPrevVolumeLev
	* Stores thr Prev Volume Level
	*/

	TInt iPrevVolumeLevel;
	
	/**
	* iBufferigSteamText
	* Data for checking the text of the WaitDialog
	*/
	TBool iBufferigSteamText;
	
	/**
	* iWaitDialog
	* calls the waitdialog owned by
	* CAknWaitDialog
	*/   
	CAknWaitDialog* iWaitDialog;
	
	// Used for displaying Metadata
	TBool iDisplayMetaData;
	/**
	* iMarquee
	* Instance For Confirming Whether to Display Metadata
	*/
	TBool  iMarquee;

    //User for testing Existence of Network request
	TBool iCancelNetworkRequest;
	
		//For context navigation in nowplaying view//
    TIRViews iContextViews;
    
   /**
	* iCheckValue
	* Instance Used to check While Connecting To a Channel
	*/
	TBool  iCheckValue;
	
	/**
	* iChannelConnected
	* Instance Used to check whether the current 
	* channel is connected or not
	*/
	TBool  iChannelConnected;
	
	/**
	* iPlayandStop
	* Set when Play or Stop is Done Fom Nowplaying View
	*/
	TBool  iPlayandStop;

	/**
	* iNotify
	* Set when Connection is established during ALR
	*/
	TBool iNotify;
	
	/**
	* iAsfPlayer
	* ASF Player instance for ASF streaming
	*/
	CIRAsfPlayer* iAsfPlayer;

	/**
	* iAsfStreaming
	* Indicates ASF channel streaming is on
	*/
	TBool  iAsfStreaming;
	
	/**
	* iSessionStartedFlag
	* sets when session startedL is called
	*/
	TBool iSessionStartedFlag;
	/**
	* iListenFromIsds
	* When Listen From Isds is Done 
	*/

	TBool iListenFromIsds;
	
	/**
	* iDialogCanceled
	* 	when dialog is dismissed
	*/
	TBool  iDialogCanceled;

	/**
	* iCancelFrmIsds
	* Cancel is Done from ISds
	*/
	TBool iCancelFrmIsds;
	/**
	* iLaunchNowplaying
	* Set if NowPlaying View is Active
	*/
	TBool iLaunchNowplaying;
	
	/**
	* iBufferingRequestCancelled
	* Sets this Value When User Cancels buffering
	*/

	TBool iBufferingRequestCancelled;
	
	TInt selectedbitrate ;
	
	TInt iUinqid;
	
	TInt iFavIndex;
	TInt iQualityRequired;
public:
	//Instance of CIRIsdsPreset
	//contains the preset on which the connection attempt is made
	CIRIsdsPreset* iNowPlayingPreset;
	
	//Instance of CIRIsdsPreset
	//Contains the preset which is editable
	CIRIsdsPreset* iEditStationPreset;

	//Holds the playing status
	TBool iPlaying;
	
	/**
	* iSyncPreset
	* sets the status of synchronizing the preset
	*/
	TBool iSyncPreset;
	};
	
#endif //IRNOWPLAYINGWRAPPER_H