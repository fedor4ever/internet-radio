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
*  Version: 25, Tue Mar 19 18:00:00 2008 by Rohit
*  Ref:
*  Implemented change request to display Byte Counter in KB till 0.1 MB, and in MB threrafter
*
*  Version: 24, Tue Feb 28 18:00:00 2008 by Rohit/Kranthi
*  Ref:
*  MIRCtrlCmdObserver::MCtrlCommand() function signature changed (Merged IRv1.0 Engine code changes)
*
*  </ccm_history>
* ============================================================================
*/

#ifndef IRUI_H
#define IRUI_H

#include <aknsitemdef.h>
#include <aknviewappui.h>
#include <remconcoreapitargetobserver.h>
#include <aknindicatorcontainer.h>
#include "internetradioconfigurationflags.hrh"

#include <MProEngProfileActivationObserver.h>
//For IAD
#ifdef FF_IAUPDATE
#include <iaupdateobserver.h>
#endif

//starting adding of ui code
#include "irbat.h"
#include "irctrlcommand.h"
#include "irnetworkcontrollerobserver.h"
#include "irpropertyobserver.h"
#include "irsessionparams.h"
#include "irstreamsourceobserver.h"
#include "misdsresponseobserver.h"
#include "IRdatatransferobserver.h"
#include "irfirsttimeview.h"
#include "irsettingsview.h"
#include "irsystemeventobserver.h" //For system events
#include "irpubsubkeys.h"
#include "IRControlEventObserver.h"

class CIRSettingsView;
class CIRBaseView;
class CIRCtrlCmdObserver;
class CIRDiscoverStationsView;
class CIRFavoritesDb;
class CIRMediaClient;
class CIRNetworkController;
class CIRNowPlayingView;
class CIRNwInfoObserver;
class CIROTAUpdate;
class CIRPubSub;
class CIRReportGenerator;
class CIRSettings;
class CIRStreamSource;
class CAlfEnv;
class CIRAlarmObserver;
class CIRMessageObserver;
class CIRBrowseChannelItems;
class CIRBrowseCatagoryItems;
class CIRLastPlayedStationInfo;
class CIconFileProvider;
class CIRFirstTimeView;
class CIRAddManuallyStationView;
class CIRStationDirectoryView;
class CIRCategoryView;
class CIRSearchResultsView;
class CIRStationsView;
class CIRTopStationsView;
class CIRPlsView;
class CIRCostWarningView;
class CIRMainView;
class CIRDialogLauncher;
class CAknWaitDialog;
class CIRIsdsWrapper;
class CIRNowPlayingWrapper;
class CIRViewStack;
class CIRSystemEventCollector; //For system events
class CIRStationInfoView;
class CIRHistoryListView;
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;
class MProEngNotifyHandler;
//For IAD
class CIAUpdate;
class CIAUpdateParameters;
class CIRControlEventObserver;

#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
class CIRBackSteppingServiceWrapper;
#endif // __IRA_FEATURE_BACKSTEPPING_SERVICE


//Network Indicator
_LIT(KGprs,"Gprs");
_LIT(K3G,"3G");
_LIT(KWiFi,"WiFi");
_LIT(KEdge,"Edge");

//CONSTANTS DECLARATION

//Resolution
const TInt KIRLowResolution = 176;
const TInt KIRQVGAResolution = 240;
const TInt KIRPotraitResolution = 320;
//Ulr Validation
const TInt KMaxBufSize = 256;
const TInt KSmallBufSize = 32;
const TInt KSearchStringChar = 100;
const TInt KLoop = 3;
const TInt KScrollAmount =20;
const TInt KScrollDelay = 1000000;
const TInt KInterval = 200000;

//charging Animation time
const TInt KChargingAnimTimer = 500000;

//For help context
#define KUIDIRAPP 0x2000B499
const TUid KIRMCVUid = {KUIDIRAPP};

//add for now can change later
enum TIROverLayText
    {
    KIROverLayTextNotDefined = 0,
    KIROverLayTextBuffering,
    KIROverLayTextLoading,
    KIROverLayTextSearching,
    KIROverLayTextConnecting
    };


 enum TIRKeyboardKeys
{
   EIRKeyboardKey0 = 0x30,
   EIRKeyboardKey1 = 0x31,
   EIRKeyboardKey2 = 0x32,
   EIRKeyboardKey3 = 0x33,
   EIRKeyboardKey4 = 0x34,
   EIRKeyboardKey5 = 0x35,
   EIRKeyboardKey6 = 0x36,
   EIRKeyboardKey7 = 0x37,
   EIRKeyboardKey8 = 0x38,
   EIRKeyboardKey9 = 0x39
};


const TInt KSearchBoxTextLength = 100;
const TInt KFindStringLength = 10;


//========================================class declaration CIRUi============================================




/**
 * This class Creates CIRUi.
 * Controls the  entire Ui.
 * It creates the instance's of all the views.
 * Instantiates NetworkController,IsdsClient,IsdsPreset,MediaClient,FavouritesDb,IRPubSub
 *
 * @code
 * Controls the user commands
 * HandleCommandL()
 *
 * Handle window server events
 * HandleWsEventL()
 *
 * Display the battery value
 * DisplayBatteryValueL()
 *
 * Redraw the currently activated view
 * ActivateCurrentView()
 *
 * Update the current network status
 * UpdateCurrentNetwork()
 *
 * Connect to server
 * iNetwork->ConnectToServerL(iUrl)
 *
 * Call back called when category data recieved
 * IsdsCatogoryDataReceivedL()
 *
 * Call play
 * iPlayer->Play()
 *
 * @endcode
 *
 */
class MAknEditingStateIndicator
	{
public:

	enum TAknEditingState
		{
		EStateNone,
		ET9Upper,
		ET9Lower,
		ET9Shifted,
		ENumeric,
		EMultitapUpper,
		EMultitapLower,
		EMultitapShifted,
		};

public:
	virtual void SetState(TAknEditingState aState) = 0;
	virtual CAknIndicatorContainer *IndicatorContainer() = 0;
	};

class CIRUi :public CAknViewAppUi, public MIRNetworkController,
			 public MCoeForegroundObserver,public MIRPropertyChangeObserver,
			 public MIRDataTransferObserver,
#ifdef FF_IAUPDATE
 			 public MIAUpdateObserver,
#endif
 			 public MRemConCoreApiTargetObserver,public MProEngProfileActivationObserver,
 			 public MIRSystemEventObserver,
 			 public MCoeViewActivationObserver
	{
public:
	/**
	* ConstructL
	* 2nd phase constructor.
	* Perform the second phase construction of a
	* CIRUi object.
	*/
	void ConstructL();

	/**
	* CIRCategoryView.
	* C++ default constructor.
	*/
	CIRUi();

	/**
	* ~CIRCategoryView
	* Destructor.
	*/
	~CIRUi();

	/**
	* GetWrapperInstance()
	* Returns the instance of the IsdsWrapper object
	*/

	CIRIsdsWrapper* GetWrapperInstance();
	/**
	* HandleCommandL()
	* Handles the user input commands.
	* @param input command
	*/

	/**
	* GetNowPlayingWrapperInstance()
	* Returns the instance of the CIRNowPlayingWrapper object
	*/
	CIRNowPlayingWrapper* GetNowPlayingWrapperInstance();
	/**
	 * From CAknView
	 *
	 * @see CAknView::HandleCommandL( TInt aCommand )
	 */
	void HandleCommandL(TInt aCommand);

	/**
	* IRHandleCommandL()
	* Saves the index of the selected item from the listbox.
	* @param currently selected item index.
	* @param user command
	*/
	void IRHandleCommandL(TInt aIndex,TInt aCommand);

	/**
	* SavePrevIDL()
	* Saves the previousID of the view.
	* @param previousID of the view.
	* @param user command.
	*/
	void SavePrevIDL(const TUid& aPrevID,TInt aCommand);

	/**
	* HandleStatusPaneSizeChange()
	* Called by framework whenever the resource is changed.
	*/
	void HandleStatusPaneSizeChange();

	/**
	* HandleWsEventL()
	* Called by framework on an window server event.
	* @param window server event
	*/
	void HandleWsEventL( const TWsEvent& aEvent,CCoeControl* aDestination );

	/**
	* HandleKeyEventL()
	* Called by framework on an window server event.
	* @param window server event
	*/
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	/**
	* IconProvider()
	* @return the instance of icon provider..
	*/
	CIconFileProvider* IconProvider();

	/**
	* GetMifFilePath().
	* @return the full path of mif file.
	*/
	TFileName GetMifFilePath() const;

	/**
	* GetPrivatePath().
	* Returns the full path of the file.
	* @return filepath
	*/
	TFileName GetPrivatePath()const;
	/**
	* ActivateCurrentView().
	* Redraws the currently active view.
	* @param Uid of the currently active view.
	*/
	void ActivateCurrentView(const TUid& aUid) const;

	/**
	* VerifyNetworkConnectivityL()
	* verify whether the network is connected or not
	* This function can leave if displaying of info note leaves hence
	* this leave can be ignored
	* @param    aObserver     The delayed action observer to receive the event when network connection is established.
	* @return the bool that informs the network connectivity status.
	*/
	TBool VerifyNetworkConnectivityL();

	/**
	* HandleGainingForeground()
	* This is call back function which observes application focus gaining
	* foreground
	*/
	void HandleGainingForeground();

	/**
	* HandleLosingForeground()
	* Function handles when application looses foreground focus
	*/
	void HandleLosingForeground();
	/**
	 * LosingForeground
	 * Function handles when application looses foreground focus
	 */
	void LosingForeground();


	/**
	* ViewStack()
	* Returns pointer to the view stack.
	* @return   Pointer to view stack.
	*/
	CIRViewStack& ViewStack();

	/**
	* ForwardBackCommandToBsWrapperL()
	* Forwards back command to Back Stepping Service wrapper.
	* @return Returns ETrue if Back Stepping Service consumes the back command, otherwise returns EFalse.
	*/

	TBool ForwardBackCommandToBsWrapperL();

	/**
	* ActiveView()
	* Function returns the UID of the Current Active view
	**/
	TUid ActiveView() const;

	/**
	* FocusWhenSelectGot()
	* Funtion is called to get the flag when an listbox item is selected
	* so that the next view gets activated
	**/
	TBool FocusWhenSelectGot() const;

	/**
	* FocusWhenSelectSet()
	* Funtion is called to set flag when an listbox item is selected
	* so that the next view gets activated
	**/
	void FocusWhenSelectSet(TBool aFocus);

	/**
	* OpenFileL()
	* Open the file using Document's instance
	* @param filename to be opened
	*/
	void OpenFileL( const TDesC& aFileName );


	/**
	* GainingForeGroundL()
	* Function handles when application gains foreground focus
	*/
	void GainingForeGroundL();
	/**
	* MrccatoCommand()
	* This is called to decrease the volume
	* @param aOperationId Operation Id
	* @param aButtonAct Button Action
	*/
	void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
	TRemConCoreApiButtonAction aButtonAct);

	/**
     * From MRemConCoreApiTargetObserver.
     */
    void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct);

	/**
	* SetVolume()
	* Call CPeriodic after the period is over
	* @param volume level.
	*/
	void SetVolume(TInt aValue);

	 /**
     * Returns a reference to System event collector
     */
    CIRSystemEventCollector& SystemEventCollector();

	/**
	* IRNetworkEventL()
	*
	*/
	virtual void IRNetworkEventL(TIRNetworkEvent aEvent);

	/**
	* UpdateCurrentNetwork()
	* Updates the CurrentNetwork
	* @param MCC
	* @param MNC
	*/
	void UpdateCurrentNetwork(const TDesC& aCurrentNetworkMCC,
	const TDesC& aCurrentNetworkMNC);

	/**
	* UpdateHomeNetwork()
	* Updates the home
	* @param MCC
	* @param MNC
	*/
	void UpdateHomeNetwork(const TDesC& aCurrentNetworkMCC,
	const TDesC& aCurrentNetworkMNC);


	/**
	* ParseAndPrepareUrl()
	* Checks for the URL whether its valid or not
	* @param URL
	*/
	TBool ParseAndPrepareUrl(const TDesC& aUrl);

	/**
	* SetCurrentNetworkType()
	* Sets Current Network Being Used
	*/
	void SetCurrentNetworkType();

	/**
	* GetCurrentNetworkType()
	* Returns Current Network Being Used
	*/
	TDesC& GetCurrentNetworkType();

	/**
	* SetPlayStateWhileCall()
	* Sets This Value When Stoping the Player
	* While Call is Active
	*/
	void  SetPlayStateWhileCall(TBool aPlayTerminated);
	/**
	* GetPlayStateWhileCall()()
	* Returns the Value of Play state
	* While Call is Active
	*/
	TBool  GetPlayStateWhileCall();

	/**
	* IsEmbedded()
	*/
	TBool IsEmbedded() const;

	/**
	* IsCallActive()
	* @return the state of call
	**/
	TBool IsCallActive();

	/**
	* HandlePropertyChangeL
	* Derived from MIRPropertyChangeObserver
	*/
	virtual	void HandlePropertyChangeL(const TUid& aCategory,
	const TUint aKey, const TInt aValue );

	/**
	* HandlePropertyChangeL
	* Derived from MIRPropertyChangeObserver
	*/
	virtual	void HandlePropertyChangeL(const TUid& aCategory,
	const TUint aKey, const TIRTextProperty& aValue );

	/**
	* HandlePropertyChangeErrorL
	* Derived from MIRPropertyChangeObserver
	*/
	virtual	void HandlePropertyChangeErrorL(const TUid& aCategory,
	const TUint aKey, TInt aError);

	/**
	* CurrentViewContainer
	* Returns the pointer of the active view container
	*/
	CIRBaseView* CurrentViewContainer();

	/**
	* IsOverLayActive()
	*Returns the status of the loading or buffering
	*/
	TBool IsOverLayActive() const;

	/**
	* The last selected browsing category
	*
	* @return The selected item as TCurrentSelectedItem, of KErrNotFound
	*/
	TInt SelectedBrowsingCategory() const;
	/**
	 * Checks if current screen orientation is landscape
	 *
	 * @return ETrue if orientation is landscape, otherwise EFalse
	 */
	TBool IsLandscapeOrientation() const;
	/**
	* Cancels the pending delayed action.
	*/
	void CancelDelayedAction();

	/**
	* DisplayNetworkIconL(TBool aNetworkFlag,CGulIcon* aNetworkBitmap;TIRConnectionType aConnectionType, TInt aBitmapId, TInt aMaskId=-1, const TDesC& aDisplayConnectionType)
	* Display the Connection Icon
	*/
	void DisplayNetworkIconL(CGulIcon** aNetworkBitmap, TIRConnectionType aConnectionType,
			 TInt aBitmapId, TInt aMaskId, const TDesC& aDisplayConnectionType);

	/**
	* FlashNetworkIConL(TBool aNetworkFlag,CGulIcon* aNetworkBitmap;TIRConnectionType aConnectionType, TInt aBitmapId, TInt aMaskId=-1, const TDesC& aDisplayConnectionType)
	* Flash the Network Icon
	*/
	void FlashNetworkIConL(CGulIcon** aNetworkBitmap, TIRConnectionType aConnectionType,
				 TInt aBitmapId, TInt aMaskId);

	/**
	* SetCommandSetL(TInt aResourceId)
	* Function is called to set the CBA buttons Dynamically
	*/
	void SetCommandSetL(TInt aResourceId);

	/* IsFirstTime()
	* Determines whether this view is launched first time or not.
	*/
	TBool IsFirstTime();

	/**
	* DisplayErrorNoteL()
	* Display the error note
	* @param string id from the resource
	**/
	void DisplayErrorNoteL(TInt aResourceId);

	/**
	* DisplayErrorNoteL()
	* Display the error note
	* @param string id from the resource
	**/
	void DisplayErrorNoteL(TInt aResourceId, const TDesC &  aString);

	/**
	* DisplayInformationL(const TDesC& aResourceDisp)
	* Display the information note.
	*/
	void DisplayInformationL(TInt aResourceId, TInt aInt);

	/**
	* DisplayInformationL(const TDesC& aResourceDisp)
	* Display the information note.
	*/
	void DisplayInformationL(TInt aResourceId, const TDesC &  aString = KNullDesC);

	/**
	* Retrieves current Byte Counter for displaying in Now playing view
	*/
	void UpdateNowPlayingByteCounter();

	/**
	* Triggers the view according to layout change.
	*/
	void TriggerCommand();
	/**
	* IsForeground()
	* Function To Check Whether Application is in Foreground.
	*/
	TBool IsForeground();
	/**
	* ParsePlsDataL()
	* Parse the PLS data
	* @param file to be parsed
	*/
	void ParsePlsDataL(CFileStore*& /*aFileStore*/, RFile& aFile);
	/**
	* ConnectToPlsUrlL()
	* Connects to PLS URL
	*/
	void ConnectToPlsUrlL();
	/**
	* GetPlsActiavted()
	* @return bool whether T&C or cost warning view are accepted or not
	*/
	TBool GetPlsActiavted() const;
	/**
	* SetPlsActiavted()
	* Sets bool if T&C or cost warning view are not accepted
	* @param bool to be set
	*/
	void SetPlsActiavted(TBool aPlsactivated);
	/**
	* GetBgWhenLockState()
	* returns the iBgWhenLock lock state
	*/
	TBool GetBgWhenLockState();
	/**
	* GetCurrentOperatorValue()
	* returns the iCurrentOperator value
	*/
	TInt GetCurrentOperatorValue();
	/**
	* GetHomeOperatorValue()
	* returns the vallue of iHomeOperator
	*/
	TInt GetHomeOperatorValue();
	/**
	* GetCallActiveState()
	* returns whether iCallActive variable is true/false
	*/
	TBool GetCallActiveState();
	/**
	* GetNetworkControllerInstance()
	* returns the iNetworkController
	*/
	CIRNetworkController* GetNetworkControllerInstance();
	/**
	* GetNowPlayingViewInstance()
	* returns instance od nowplaying view
	*/
	CIRNowPlayingView* GetNowPlayingViewInstance();
	/**
	* GetPubSubManagerInstance()
	* returns instace of publish subscriber
	*/
	CIRPubSub* GetPubSubManagerInstance();
	/**
	* GetCurrentThreadIDValue()
	* returns the Current thread ID
	*/
	TUint64 GetCurrentThreadIDValue();
	/**
	* GetFavPresetsInstance()
	* returns instace of Favourite preset
	*/
	CIRFavoritesDb* GetFavPresetsInstance();
	/**
	* GetNetworkType()
	* returns  Network Connection Type
	*/
	TIRConnectionType GetNetworkType();
	/**
	* GetCallConnectedState()
	* returns the iCallConnected state
	*/
	TBool GetCallConnectedState();
	/**
	* SetActivationPendingValue()
	*  assign the Id value to iActionPending
	*/
	void SetActivationPendingValue(TUid aUid);
	/**
	* GetPresentViewID()
	* returns the present view ID
	*/
	TUid GetPresentViewID();

	/**
	* HandleAlarmStart()
	* Handles when an alarm comes
	*/
	void HandleAlarmStart();

	/**
	* HandleAlarmEnd()
	* Handles when an alarm ends
	*/
	void HandleAlarmEnd();

	/**
	* HandleInitialAlarmEvent()
	* Handles initial event when alarm comes
	*/
	void HandleInitialAlarmEvent();

	/**
	* HandleMessageAlertL()
	* Handles when a new message comes
	*/
	void HandleMessageAlertL();

	/**
	* GetPlsIndex()
	* Returns the Pls Index
	*/
	TInt GetPlsIndex();

	/**
	* SetPlsIndex()
	* Sets the Pls Index
	*/
	void SetPlsIndex(TInt aPlsIndex);

	/**
	* GetBgLock()
	* Returns whether the Bground is Locked
	*/
	TBool GetBgLock();

	/**
	* SetBgLock()
	* Sets the BGround whether it is Locked or Unlocked
	*/
	void SetBgLock(TBool aBgWhenLock);

	/**
	* GetEditStation()
	* Returns whether The station can be edited Or not
	*/
	TBool GetEditStation();

	/**
	* SetEditStation()
	* Sets the iIsEditStation Flag To determine whether the station
	* can be Edited
	*/
	void SetEditStation(TBool aIsEditStation);

	/**
	* IsOfflineProfile()
	* Function returns True if the Current Profile is Offline
	*/
	TBool IsOfflineProfile();

	// Abstract method for handling profile activation event.
	void  HandleProfileActivatedL (TInt aProfileId);
	void  HandleProfileActivationNotificationError (TInt) ;

	/**
	 * Checks if free disk space is under pre-set limit. Also
	 * shows information note telling user to delete some data.
	 * Some operations should check the disk space with this
	 * function before proceeding and possibly cancel the process.
	 *
	 * @return ETrue if disk space is under limit, EFalse otherwise.
	 */
	TBool CheckAndNotifyLowDiskSpaceL() const;

	/* MIRSystemEventObserver::HandleSystemEventL()
 	 * Handles the system events
 	 */
	 void HandleSystemEventL(TIRSystemEventType aEventType);

	 /*
	  * Changes the state of the player
	  */
	 void PlayerStateChangedL(TIRStopPlayState aState);

	 /*
	  * from base class MCoeViewActivationObserver
	  */
    void HandleViewActivation(const TVwsViewId& aNewlyActivatedViewId,
        					  const TVwsViewId& aViewIdToBeDeactivated );

private:

	/**
	* NetworkAnimationTimerExpiredL
	* Called when the buffering times out
	**/
	static TInt NetworkAnimationTimerExpired(TAny* aPtr);

	/**
	* GetLockInfo()
	* To get the information about phone lock
	* @param TBool Return true if the phone is Locked.
	**/
	TBool GetLockInfo();

	/**
	* Updates the subscriber identifier (IMSI code) and sets the Terms & Conditions display as necessary.
	*/
	void UpdateSubscriberIdL();
	/**
	* HandleDataTransferEventL()
	* CallBack which Returns the ByteCounter
	**/

	void HandleDataTransferEventL( const MIRDataTransferTracker::TIRDataTransferPckg& aData );

	/**
	* ResetVolumeTimer()
	* Set the volume bar timer for displaying volume bar
	* @param time interval for which volume bar is to be displayed
	*/
	void ResetVolumeTimer( TInt aVolumeTimer );

	/**
	* IsVolumeBarPressed()
	* checks whether volume bar is pressed
	* @return ETrue whether volume bar key is pressed
	*/
	TBool IsVolumeBarPressed();

	/**
	* IsVolumeUp()
	* checks whether volume bar is pressed
	* @return ETrue if volume is increased and EFalse if volume is decreased
	*/
	TBool IsVolumeUp();




	/**
	* Updates the landscape data. This is done separately
	* and not in IsLandscapeOrientation method as
	* IsLandscapeOrientation MUST NOT do a window server flush
	*/
	void UpdateLandscapeInformation();

	/**
	*CIRUi::CreateWritablePrivatePath()
	*Only for ROM SDK3.2
	*creates a private path in c drive if it doesnot exist (for rom install)
	*/
	void CreateWritablePrivatePath();

	// from base class MIAUpdateObserve
#ifdef FF_IAUPDATE
    void CheckUpdatesComplete(TInt aErrorCode, TInt aAvailableUpdates);
    void UpdateComplete(TInt aErrorCode, CIAUpdateResult* aResult);
    void UpdateQueryComplete(TInt aErrorCode, TBool aUpdateNow);
#endif


public:
	/*
	* HandleResourceChangeL( TInt aType )
	* Handles the changes in the resolution
	*/
	void HandleResourceChangeL( TInt aType );


// Handles call functionalities when the call is active state or inactive state
	void HandleCallActiveL(TIRSystemEventType aEventType);

 TInt ChooseLayoutResource( TInt aNormalPortraitRes,
        TInt aMirroredPortraitRes, TInt aNormalLandscapeRes,
        TInt aMirroredLandscapeRes ) const;
	/*
	* GetSkinColor
	* Returns the Current Color
	*/
	TRgb GetSkinColor();
	/*
	* ChooseToolBarIcons
	* Returns the ToolbarBirmap Id For The Current Theme
	*/
	TInt  ChooseToolBarBitmaps(TInt aBlack,TInt aWhite);
	/*
	* ChooseToolBarMaskId
	* Returns the ToolbarBirmap Id For The Current Theme
	*/
	TInt  ChooseToolBarMaskId(TInt aBlackMask,TInt aWhiteMask);

	/*
	* SetHistory
	* Set the Flag From All the Views  When Go to nowplaying Is Done
	*/
	void  SetHistory(TBool aHistory);


	/*
	* GetAlarm
	* Return the iAlarmTerminated Flag
	*/
	TBool GetAlarm();

	//Methods to determine if any channel was added to favorites
	TBool IsFlagIfAnyFavL();

    void SetMainView(TBool val);

    /**
	 * FindInMusicStoreCounter()
	 * Counter of how many times a user has started
	 * "FindInMusicStore"
	 */
	void FindInMusicStoreCounter();

	/**
	 * GetFindInMusicStoreCount()
	 * Gets the value of how many times a user has started
	 * "FindInMusicStore"
	 */
	void GetFindInMusicStoreCount(TInt& aFindInMStoreCount);

	RArray<TInt>& GetPlsSaved() ;

	void SetListenFromPls(TBool aValue);

    TBool GetListenFromPls();

   	/**
	 *  Used to determine the current profile
	 *  @return TInt describing the profile value
	 */
	TInt DetermineCurrentProfile() const;

	void SetListenFromSecondPls(TBool aValue);
	TBool GetListenFromSecondPls();

	/**
	 * Brings VRA task to foreground.
	 */
	void BringToForeground() const;

	/**
	 * Sends VRA task to background.
	 */
	void SendToBackground() const;


private:		//Added by Snigdha for HomeScreen


	 /**
     * Handles action handler command line parameter.
     *
     * @param   aTail   The tail of the command line; command id is expected.
     */
    void ProcessActionHandlerCommandParameter( const TDesC8& aTail );

    /**
     * Parses the command line options and calls option specific methods. Supports existing and extended
     * command line interfaces.
     *
     * in old CLI, the expected format of aTail is "N", where N is a preset ID.
     *
     * in new CLI, the expected format is "-<option> <value>", where option can be "p", "f" or "a".
     *             for option p (preset) <value> is integer presenting the preset ID
     *             for option f (frequency) <value> is integer presenting frequency.
     *             for option a (action handler command) <value> is id of the command
     *                           - commands can be found from vractionhandlerdefs.h.
     *
     * @param   aTail   The tail of the command line.
     */
    void ProcessCommandTailL( const TDesC8& aTail );


    MCoeMessageObserver::TMessageResponse HandleMessageL(
	    TUint32 aClientHandleOfTargetWindowGroup,
	    TUid aMessageUid,
	    const TDesC8& aMessageParameters );

    TBool ProcessCommandParametersL(
        TApaCommand aCommand,
        TFileName& aDocumentName, const TDesC8& aTail );

public:



	//Array of URL's
	// TO DO: This varible can be made as private in Future
	/*
	* iURLs
	* Instance of RPointerArray
	*/
	RPointerArray<HBufC8> iURLs;
	/*
	* iNowPlayingView
	* Instance of NowPlaying View
	*/
	CIRNowPlayingView* iNowPlayingView;
	/*
	* iFirstTimeView
	* Instance of First Time View
	*/
	CIRFirstTimeView* iFirstTimeView;
	/*
	* iIRAddManually
	* Instance of Add Manually View
	*/
	CIRAddManuallyStationView* iIRAddManually;
	/*
	* iIRAddManually
	* Instance of NetworkController
	*/
	CIRNetworkController* iNetworkController;
	/*
	* iIconProvider
	* Instance of Icon File Provider
	*/
	CIconFileProvider* iIconProvider;
	/*
	* iFsSession
	* Instance of filesession
	*/
	RFs iFsSession;
	/*
	* iFavPresets
	* Instance of FavoritesDb
	*/
	CIRFavoritesDb *iFavPresets;
	/*
	* iIRSettings
	* Instance of CurrentOperationSettings
	*/
	CIRSettings* iIRSettings;
	/*
	* iSettingsView
	* Instance of Settings View
	*/
	CIRSettingsView* iSettingsView;

	//Currently selected listbox item index
	TInt iIndex;


	//Now Playing View's previous ID
	TUid iNPPrevViewId;


	//Presently activated View's ID
	TUid iPresentViewID;

	/*
	* iLastPlayed
	* Instance of Last played station Information
	*/
	CIRLastPlayedStationInfo *iLastPlayed;

	/*
	* iStationdirView
	* Instance of Station Directory View
	*/
	CIRStationDirectoryView *iStationdirView;
	/*
	* iCategoryView
	* Instance of Category View
	*/
	CIRCategoryView *iCategoryView;

	/*
	* iSearchResultsView
	* Instance of SearchResults View
	*/
	CIRSearchResultsView* iSearchResultsView;

	/*
	* iStationsView
	* Instance of  Stations View
	*/
	CIRStationsView *iStationsView;
	/*
	* iTopStationsView
	* Instance of TopStations View
	*/
	CIRTopStationsView *iTopStationsView;

	/**
	* 	Store Veiw to be activated when in Background.
	*/
	TUid iActivationPending ;

	/*
	* iUpdateApp
	* Instance of CIRAppUpdate
	*/
	TBuf<KSearchBoxTextLength> iSearchString;

	// Holds the data for the Stations view Title
	RBuf iStationsViewTitleString;

	// Holds the data for the Category view Title type
	TInt iCategoryViewTitleType;

	/**
	* To Hold the Index of the Station to Edit
	*/
	TInt iEditStationIndex;
	/*
	* iPlsView
	* Instance of Play List view
	*/
	CIRPlsView* iPlsView;
	/*
	* iDialogNote
	* Instance of Dialog Launcher class
	*/
	CIRDialogLauncher * iDialogNote;




private:

#ifdef FF_IAUPDATE
    /**
     * IAD client object.
     * Owned.
     */
    CIAUpdate* iUpdate;

    /**
     * Parameters for IAD update.
     * Owned.
     */
    CIAUpdateParameters* iParameters;
#endif // FF_IAUPDATE



   TBool iFeatureManagerInitialized;



	/*
	* iSelector
	* Instance of CRemConInterfaceSelector For
	* Handling Media Keys
	*/

	CRemConInterfaceSelector* iSelector;
	/*
	* iTarget
	* Instance of CRemConCoreApiTarget For
	* Handling Media Keys
	*/

	CRemConCoreApiTarget* iTarget;

	//holds type of current network
	TIRConnectionType iNetWorkType;

	//checks whether call is connected or not
	TBool iCallConnected;

	//current operator
	TInt iCurrentOperator;

	//home operator
	TInt iHomeOperator;

	//network observer for finding home and current operator
	CIRNwInfoObserver* iNwObserver;

	/**
	* System event collector
	* For handling system events related to call, network and headset
	*/
	CIRSystemEventCollector* iSystemEventCollector;


	// Used for url checking
	TBuf8<KMaxBufSize> iUrl8;

	/*
	* iIsPresetSaved
	* array used to check whether the preset is saved in Favorites from PLS
	*/
	RArray<TInt> iIsPresetSaved;

	RArray<TInt> iPlsPresetSaved;

	//Checks whether PLS view is active or not
	TBool iPlsactivated;

	// for cheking if the view is activated through the select
	TBool iSelect;


	//indicates that play has to be triggered after alarm
	TBool iTriggeringRequired;

	//determines overlay text
	TIROverLayText iOverlayVariable;


	TUid iBGPresentViewId;

	/*
	* iCntChaReqForSaving
	*
	*/
	TInt iCntChaReqForSaving;

	//for call observing
	TBool iCallActive;

	//For adding songs to history to handle adding to history
	TInt iCount;

	// Current preset id
	TInt iCurrentPresetId;

    TBool iListenFrmSecondPls;


	// The selected browsing category
	TInt iSelectedBrowsingCategory;

	TInt iRequestedPresets;

	//Skinning API needs the file name to be stored in a TBuf
	TBuf<KMaxBufSize> iMbmFileName;

	//set if a sync request is made from ui
	TBool iOtaRequest;


	//stores current thread's id
	TUint64 iCurrentThreadID;


	TBool iAlarmTerminated;


		//current network
	TBuf<10> iCurrentNetwork;

	//home network
	TBuf<10> iHomeNetwork;

	//Holds the type of network connected
	TBuf<5> iConnectionType;

	/*
	* iPlsLength
	* Instance of RArray
	*/
	RArray< TInt > iPlsLength;

	/*
	* iValue
	*value to store the return value from confirmation Notes
	*/
	TInt iValue;

	/*
	* iAlfEnv
	* Instance of CAlfEnv
	*/
	CAlfEnv* iAlfEnv;

  	/*
	* iMifFilePath
	* path for the Mif File
	*/
	TFileName iMifFilePath;

	//Checks whether application is call terminated
	TBool iPlayTerminated;

	/*
	* iLandscape
	* Instance to return the value of landscape
	*/
	TBool iLandscape;

		//Instance of IRPubSub
	CIRPubSub* iPubSubManager;

	/*
	* iMultiStationSave
	* return the bool value for multi station saved
	*/
	TBool iMultiStationSave;

	/**
	* subscriber for bitrate
	*/
	CIRPropertyObserver* iBitrateSubscriber;

	/**
	* subscriber for multiple play handling
	*/
	CIRPropertyObserver* iMultiplePlayHandler;
	/**
	* Timer to animate the network indicatior icon
	*/
	CPeriodic* iNetworkIndicatorTimer;

		//Gets the Pls Index
	TInt iPlsIndex;

	//check if going to background due to keypad lock
	TBool iBgWhenLock;

	/**
	* Boolean varaible to store whether the station is
	* editable or not
	*/
    TBool iIsEditStation;

	/**
	* Boolean varaible which is used for Switching Between Npv and History
	* View
	*/
    TBool iHistory;


    TBool iSecondInstanceFlag;

    TBool iMainViewLaunch;


    static TInt iFindInMusicStoreCount;
    /**
	* Boolean varaible which is used for Checking The Current profile
	*/
	TBool iIsOffline;

	/*
	* iNotifyHandler
	* Instance of Notify Handler Used for Registering Profile
	* Changed
	*/
    MProEngNotifyHandler* iNotifyHandler;
    TBool iListenFromPls ;

	CIRDialogLauncher*  iDialogLauncher;

	/*
	* iProfileId
	* Instance Which strores the Profile Id
	*/
    TInt iProfileId;


    /**
     * Is application started or focused to Now playing view.
     */
    TBool iStartUpInNowPlayingView;

#ifdef __IRA_FEATURE_BACKSTEPPING_SERVICE
    /**
     * Backstepping Service wrapper.
     * Own.
     */
    CIRBackSteppingServiceWrapper* iBsWrapper;
#endif

	CIRControlEventObserver * iControlEventObserver; 
public:

	/**
	* iIsdsWrapper
	* Instance of the IsdsWrapper class
	*/
	CIRIsdsWrapper *iIsdsWrapper;
	/**
	* iNowPlayingWrapper
	* Instance of the NowPlayingWrapper class
	*/
	CIRNowPlayingWrapper *iNowPlayingWrapper;
	/*
	* iHistoryListView
	* Instance of History view
	*/
	CIRHistoryListView* iHistoryListView;
	/**
	* View stack holds all previous views, not the current view.
	*/
	CIRViewStack* iViewStack;
	/*
	* iUpdateApp
	* Instance of Cost warning view
	*/
	CIRCostWarningView* iCostWarningView;
	/*
	* iMainView
	* Instance of Main View
	*/
	CIRMainView* iMainView;
	/*
	* iAlarmObserver
	* Instance of Alarm observer
	*/
	CIRAlarmObserver* iAlarmObserver;
	/*
	* iMessageObserver
	* Instance of Message observer
	*/
//	CIRMessageObserver* iMessageObserver;

	/**
	* iStationInfoView
	* Instance of the StationInfoView class
	*/
	CIRStationInfoView *iStationInfoView;

	/**
	 * iIsDisconnected
	 * checks if the network status is disconnected
	 */
	 TBool iIsDisconnected;



	};


#endif // IRUI_H


