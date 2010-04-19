/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CIRNowPlayingView
*
*/


#ifndef C_CIRNOWPLAYINGVIEW_H
#define C_CIRNOWPLAYINGVIEW_H

#include "internetradioconfigurationflags.hrh"
#include <coecobs.h>
#ifdef MUSICSHOP_AVAILABLE
#include <MusicStoreUidConstants.h> //For Find in Shop
#include <mpxfindinmusicshopcommon.h> //For Find in Shop
#endif
#include <mdaaudiosampleplayer.h> //For future use
#include <alf/alfenv.h>

#include <aknvolumepopup.h>
#include "irbaseview.h"
#include "irui.h"
#include "ir.hrh"
#include "pspresetobserver.h"
#include "MLogoDownloadObserver.h"
#include "irstationinformationdata.h"
#include <akntoolbarobserver.h>
#include <aknbutton.h>
#include "IRFavoritesDb.h"
#include "MViewsResponseAndErrorObserver.h"
#include <AknProgressDialog.h>

#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include "irstationlogocontrolobserver.h"

#include "IRActiveNetworkObserver.h"
#include "msyncpresetobserver.h"	// MSyncPresetObserver
//Forward Declarations
class CAlfEnv;
class CIRNowPlayingViewContainer;
class CIRIsdsPreset;
class CIRLogoDownloadEngine;
class MLogoDownloadObserver;
class CIRIsdsPreset;
class CEikMenuPane;

#ifdef __IRA_FEATURE_EQUALIZER
class CMPXViewPlugin;
class CMPXFindInMShop; //For Find in Shop
#endif

class CMdaAudioPlayerUtility;
class CStereoWidening;
class CIRSettings;
class CMPXFindInMShop; //For Find in Shop
class CAknVolumePopup;
class CAknWaitDialog;
class CIRLogoTimer;
/**
 *  NowPlayingView View
 *
 *  NowPlayingView View, the  view displayed in Internet Radio  application,when
 *  Listen is Done
 */
class CIRNowPlayingView :public CIRBaseView,public MPSPresetObserver,
						public MLogoDownloadObserver,public MAknToolbarObserver,
    					public MViewsResponseAndErrorObserver,public MCoeControlObserver,
    					public MAlfActionObserver,private MRemConCoreApiTargetObserver,
    					 public MIRLogoControlObserver,
    					 public MSyncPresetObserver


 	{
public:  // Methods

    /**
     * Function : NewL
     * Function returns an instance of CIRNowPlayingView
     * Two phase constructor
     * @param aAlfEnv Alfred environment to use
     * @return instance of CIRNowPlayingView
     */
	static CIRNowPlayingView* NewLC( CAlfEnv& aAlfEnv );

	/**
	* Two-phased constructor.
	*
	* Adds the control to the control group
	*
	* @param   aEnv    Alfred environment to use.
	* @return  The created object. Ownership is not transferred.
	*/
	static CIRNowPlayingView* NewL( CAlfEnv& aEnv);
	/**
	* Destructor.
	*/
	~CIRNowPlayingView();

	/**
     * OfferToolbarEventL()
   	 */
	void OfferToolbarEventL(TInt aCommand );

    /**
     * Id()
     * returns the id of the View
     */
	// from base class CAknView
	TUid Id() const;

	/**
     * HandleCommandL()
     */
	void HandleCommandL( TInt aCommand );

	/**
     * DynInitMenuPaneL()
     */
	// from base class MEikMenuObserver
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);


   	/**
     * HandleForegroundEventL()
  	 */
	void HandleForegroundEventL( TBool aForeground );

    /**
     * Function : PresetLogoDownloadedL
     * CallBack Function Returning the Preset With Logo
     * @param aPreset to be filled with Logo
     */
	void PresetLogoDownloadedL(CIRIsdsPreset* aPreset);

	/**
     * PresetLogoDownloadError()
     * CallBack Function Returning the Preset Without Logo
     * Called when an error/connection-fail occurs while downloading
     * @param aPreset sent without Logo
     */
	void PresetLogoDownloadError(CIRIsdsPreset* aPreset);


	/**
     * Function :PlayAndStopControlL()
     * Function Used to Play or Stop The Song
     * User Play,Stop Functionality
     */
	void PlayAndStopControlL();

	/**
     * Function :ConnectToPreset()
     * User For Connecting To Next or Prev Preset
     * For Context Navigation
     */
	void ConnectToPresetL();

	// from base class MCoeControlObserver
	/**
     * From MCoeControlObserver
     *
     * @see MCoeControlObserver::HandleControlEventL(
     *    CCoeControl* aControl,
     *    TCoeEvent aEventType )
     */
    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

// from base class MAlfActionObserver
    void HandleActionL(const TAlfActionCommand& aActionCommand);

	/**
     * Function :UpdateStationNameL()
     * User For Updating Station Name
     * @param aFadeStyle Used for setting Directionality
     */
	void UpdateStationNameL(TIRFadeStyle /*aFadeStyle*/) ;

    /**
     * Function :DisplayBitrate()
     * Used for Displaying Bitrate
     * @param aBitrate Value To show on the View
     */
	void DisplayBitrate(const TDesC& aBitrate) const;

    /**
     * Function :OnViewActivationL()
     * Used for Setting the values On View Activation
     */
	void OnViewActivationL();

    /**
     * Function :DisplaySongArtistNameL()
     * Displays the Song Name and Artist Name
     * @param  aSongName To show on the View
     * @param aArtistName To show on the View
     */
	void DisplaySongArtistNameL(const TDesC& aSongName,const TDesC& aArtistName);

    /**
     * Function :DisplayByteCounterL()
     * Used for Displaying ByteCounter
     * @param total_bytes Value To show on the View
     */
	void DisplayByteCounterL(TReal32  total_bytes);

	 /**
     * Function :SetRequestPendingWhenNoNetWork()
     * Sets the Value of Request Pendind While Checking For Network
     * @param aRequestPending Value To Set
     */
	void SetRequestPendingWhenNoNetWork(TBool aRequestPending);

	 /**
     * Function :GetRequestPending()
     * Return the Request Pending Value
     * @return Returns the Request Pending Value
     */
	TBool GetRequestPending();

	 /**
     * Function :SetRequestPendingWhenNoNetWorkForNextOrPrev()
     * Sets the Value of Request Pendind While Checking For Network
     * @param aRequestPending Value To Set
     */
	void SetRequestPendingWhenNoNetWorkForNextOrPrev(TBool aRequestPending);

	 /**
     * Function :GetRequestPendingForNextOrPreV()
     * Return the Request Pending Value
     * @return Returns the Request Pending Value
     */
	TBool GetRequestPendingForNextOrPreV();

    /**
     * Function :ConstructToolbarL()
     * Used for Constructing ToolBar
     */
	void ConstructToolbarL();

    /**
     * Function :DisplaylogoL()
     * Function User For Displaying the Logo
     */
	void DisplaylogoL() ;

    /**
     * Function :DrawViewL()
     * Function User For Layouting Controls On the View
     * @param aLogoStatus Value Used for Displaying
     * Media idle Animation
     */
	void DrawViewL(TBool aLogoStatus) ;

    /**
     * Function :StartTimerL()
     * Function User Updating Station Name When Next Is Pressed
     */
	void StartTimerL() ;
    /**
     * Function :ReFreshView()
     * Function Used For Refreshing the View
     */

	void ReFreshViewL();
	/**
     * Function :AddStationsL()
     * Function Used Used to Add the Currently Playing Channel To Fav
     */
	void AddStationsL();
	/**
     * ResponseL()
     */
	//from thr mixing class MViewsResponseAndErrorObserver
	void ResponseL( CIRIsdsPreset* aPreset = NULL );

	/**
     * PresetResponseL()
     * CallBack From IsDs when Preset data is received
     */
	void PresetResponseL(CIRIsdsPreset* aPreset);

	/**
     * ErrorL()
     */
	void ErrorL();

	/**
     * Function :GetStereoModeL()
     * Used for getting Mode of Stereo whether it is On or Off
     */

	void GetStereoModeL();

	// from base class MIRSystemEventObserver
    void HandleSystemEventL( TIRSystemEventType aEventType );

    /**
     * Function :SetVolumePopUpValue()
     * Used for setting the VolumePop up Value
     */
	void SetVolumePopUpValue(TInt aValue);

	 /**
     * Function :GetVolumePopUpValue()
     * Getting the volume Pop up Value
     */
    TInt GetVolumePopUpValue();

   	 /**
     * Function :UpdateLastplayedL()
     * Updates the LastPlayed Station
     */
    void UpdateLastplayedL();


    /**
     * LaunchFMTransmitterL()
     * Launch FM Transmitter application
     */
	void LaunchFMTransmitterL();
	/**
     * SetBitrate()
     * Sets The Bitrates Value
     */
	void SetBitrate(TInt aError) ;

	// from base class MRemConCoreApiTargetObserver

    /**
     * From MRemConCoreApiTargetObserver.
     */
    void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
        TRemConCoreApiButtonAction aButtonAct);

    /**
     * From MRemConCoreApiTargetObserver.
     */
    void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct);
// from base class MIRLogoControlObserver
    void LogoReadyForDisplayL();
    void LogoConversionFailedL();

	/**
	 * from MSyncPresetObserver
	 * SyncPresetRemovedL()
	 * called back when the preset that is synced was removed from isds
	 * @param TInt,the preset id
	 */
	void SyncPresetRemovedL(TInt aId);

	/**
	 * from MSyncPresetObserver
	 * SyncPresetChangedL()
	 * called back when the preset that is synced has been changed in the isds
	 * @param CIRIsdsPreset&,the new preset data
	 */
	void SyncPresetChangedL(CIRIsdsPreset& aPreset);

	/**
	 * from MSyncPresetObserver
	 * SyncPresetNoChangeL()
	 * called back when the preset that is synced has not been changed
	 */
	 void SyncPresetNoChangeL();

protected:

	// from base class CAknView
	 /**
     * DoActivateL()
     */
	void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
					const TDesC8& aCustomMessage );

	/**
     * DoDeactivate()
     */
	void DoDeactivate();

private: //Methods

	/**
	* By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL();

	/**
	* C++ default constructor.
	*/
	CIRNowPlayingView( CAlfEnv& aAlfEnv );

	/**
	* Updates the station logos with preset data
	*/
	void UpdateStationLogosL();

	/**
     * HandlePresetChangedL()
     * returns the instance of iStationsData
     */
	// from base class MPSPresetObserver
	void HandlePresetChangedL( TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aReason );

	/**
     * GetLogo()
     * returns the instance of iStationsData
     */
	void GetLogoL();

    /**
     * To Launch Find in Shop Option
     *
     */
    void LaunchMusicShopL();

    /**
     * To Launch Find in Shop Option
     *
     */
    void DoFindInMusicShopL( const TDesC& aSongName,
                             const TDesC& aArtistName,
                             const TDesC& aAlbumName,
                             TBool aMusicFlag );

	/**
	 *  Retrievs the string from Central Repository for the specified key and cen rep uid
	 *  Transfers the ownership of the returned HBufC object
	 *  The caller must takecare of deleting the returned object*
	 *	@param aRepositoryUid specifies the Uid of the repository to be used
	 *	@param aKey specifies the key to be used
	 *	@return HufC* pointer to the retrieved string
	 */
	HBufC* CentralRepositoryStringValueL(const TUid& aRepositoryUid,
		TUint32 aKey ) const;
    /**
     * ListenFromPls()
     * To support Context Navigation
     */
	void ListenFromPlsL();

	 /**
    * HelpContext()
    * Returns the Help Context For NowPlaying View
    */
   TCoeHelpContext HelpContext() const;

    /**
     * IsMetaData()
     * returns true if there is meta data
     */
    TBool IsMetaData();

    /**
     * MusciStoreEnabledStatus()
     * returns true if music store enabled status is "yes"
     */
    TBool MusciStoreEnabledStatus();



public:
   	/**
     * SetsameUrlFlag()
     * function sets the iSameUrl flag
     */
	void SetsameUrlFlag(TBool aSameUrl);
	 /**
     * GetsameUrlFlag()
     * function returns the iSameUrl Flag
     */
	TInt GetsameUrlFlag();

	/**
     * SetStationNameFlag()
     * function Sets the Flag to Update the Station Nmae
     */
	 void SetStationNameFlag(TInt aValue);

      /**
     * SetLastPlayedValue()
     * function Sets the Lastplayed Flag Value
     */
	 void SetLastPlayedValue(TInt aLastPlayed);

	 /**
     * SetPlayingValue()
     * function Sets the iPlaying Flag Value
     */
	 void SetPlayingValue(TInt aPlaying);

	 /**
     * HandleMediaKeysL()
     * function handles the hardware media keys
     */
	void HandleMediaKeysL(TRemConCoreApiOperationId aOperationId,
	TRemConCoreApiButtonAction aButtonAct);

   	/**
     * DeleteSteroEffect()
     * Destroys the Stereo Effect Corresponing the Player
     */
	void DeleteSteroEffect(TBool aStereo);
	/**
     * DisablePlayer()
     * Stop Player When Call is Active
     */
	void DisablePlayer();

    /**
     * Function :SetRockerIndex()
     * Sets the Current Playing Index
     * @param aRockerIndex Value used for Context Navigation
     */
	 void SetRockerIndex(TInt aRockerIndex);

	 /**
     * Function :GetRockerIndex()
     * Returns The current Playing Index
     * @return RockerIndex Value used for Context Navigation
     */
	 TInt GetRockerIndex();

    /**
     * Function :SavePreviousIndex()
     * Sets the Previous Index
     * @param aPresetCount Value used for Context Navigation
     */
	 void SavePreviousIndex(TInt aPresetCount);

	 /**
     * Function :GetRequestPending()
     * Return the Previous index Value
     * @return Returns Previous index Value
     */
	 TInt GetPreviousIndex();
	  /**
     * Indicates, has this view been activated in the life time of
     * the application.
     * @return ETrue if this view has been activated, EFalse otherwise
     */
    TBool HasBeenActivated() const;

    /**
     * Function :GetCommandId()
     * Returns the Last selected Command id
     * @return the Command id Required While network Switch
     */
    TInt  GetCommandId() ;

     /**
     * Function :IsVolumeMuted()
     * Returns whether volume is Muted
     * @return the Volume Mute State
     */
    TBool  IsVolumeMuted() ;

    /**
     * SyncPresetL()
     * Used to syncronize presets
     * @param TInt the preset id
     * @param TDesC the last modified tag for the preset
     */
    void SyncPresetL(TInt aChid,const TDesC &aModifiedTime);

      /**
     * Callback method that performs the actual deletion of connections.
     *
     * @param aSelfPtr Pointer argument given when the callback/timer is created.
     * @return KErrNone Specified in the function pointer declaration.
     */
    static TInt StaticConnectionCleanupCallback( TAny* aSelfPtr );


private:

	/**
	* The Alfred environment
	*/
	CAlfEnv& iAlfEnv;

	/**
	* iToolbar
	* Used for Touch ToolBar
    */
	CAknToolbar* iToolbar;

	/**
     * Is the start up of application ongoing.
     */
    TBool iStartUp;

    /**
    * For Find in Music Shop
    *
    */
    CMPXFindInMShop* iMPXFindInShop; //For Find in Shop

#ifdef __IRA_FEATURE_EQUALIZER
	/**
	 * Used for activating the equalizer view
     */
     CMPXViewPlugin* iMpxEqualizerViewPlugin;
#endif // __IRA_FEATURE_EQUALIZER

	/**
	* iIndex
	* Used for Context Navigation
	*/
	TInt iIndex;

	/**
	* iPreset
	* Used for storing the Current Logo Data
	*/
    CIRIsdsPreset* iCurrentPreset;

	/**
	* iPreset1
	* Used for storing the Prev Logo Data
	*/
	CIRIsdsPreset* iPrevPreset1;

	/**
	* iFadeStyle
	* Used for directionality
	*/
	TIRFadeStyle iFadeStyle;

	/**
	* iIsOn
	* Used for handling Menupane
	*/
	TBool iIsOn;

	/**
	* ipresets
	* Used for Getting the Fav Presets
	*/
	RIRPresetArray ipresets;

	/**
	* iBuffer
	* Used for storing the Station Name
	*/
	RBuf iBuffer;

	/**
	* iMenuPane
	* Used for handling the MenuPane
	*/
   	CEikMenuPane* iMenuPane;

   	/**
	* iDs
	* Used for holding the Commands
	*/
   	TIRIds iDs;


   	/**
	* iStationsData
	* Used for Referencing the Stations Data
	*/
   	CDesCArrayFlat* iStationsData;

   	/**
	* iShowToolBar
	* Used showing the Touch ToolBar
	*/
   	TBool iShowToolBar;

	/**
     * iMetadata
     * Used For storing the Metadata
     */
	RBuf iMetadata;

 	/**
     * iStereoMode
     * Used For storing status of Stereo Mode
     */
	TInt iStereoMode;


	//Instance of CurrentOperationSettings
	CIRSettings* iIRSettings;

	 /**
	 *iSameUrl
	 *Used for showing the metadata
	 */
	 TBool iSameUrl;

	/**
	* Used For Pls
	*/
	CIRIsdsPreset *plsData;

	/**
	* iPlsIndex
	* Used for Context Navigation
	*/
	TInt iPlsIndex;

	/**
	* iPresetValue
	* Used for Loading the Logos
	*/
	TInt iPresetValue;

	/**
	* iPrecount
	* Used for Loading the Logos
	*/
	TInt iPrecount;
	/**
	* iDefaultLogo
	* Used for Loading the Logos
	*/
	TBool iDefaultLogo;
	/**
	* iUpdateLastplayed
	* Used for Upadating the Lastplayed Song
	*/
	 TBool iUpdateLastplayed;
	/**
	* iUpdateStationName
	* Used for Updating the Station Name
	*/
	 TBool iUpdateStationName;

	/**
	* iRockerIndex
	* Used for Context Navigation
	*/
	TInt iRockerIndex;

   	/**
	* iPresetCount
	* Used for storing the index Value
	*/
   	TInt iPresetCount;
	/**
	* iVolumeBarValue
	* Stores the VolumeBar Value
	*/
	TInt iVolumeBarValue;

    /**
     * Flag used for When Equalizer is On.
     */
     TBool iEaualizerOn;
    /**
	* iPlaying
	* Checks Whether thr NowPlaying is Active or Not
	*/
	static TBool iPlaying;

	/**
	* iVolPopup
	* The Instance of AknVolume Popup.
	*/
	CAknVolumePopup*  iVolPopup;

    /**
     * Flag Set When Call is active.
     */
    TBool iCallActive;


	//Used for later in handling Stereo Mode
	CStereoWidening* iStereoEffect;
    /*
	 * iRequestPending
	 * Checks whether an active request is pending
	 */
	TBool iRequestPending;

	/*
	 * iNextOrPrevRequest
	 * Checks whether an active request is pending
	 */
	TBool iNextOrPrevRequest;
    /*
	 * iStationHasLogo
	 * Checks whether an active request is pending
	 */

	TBool iStationHasLogo;
	/**
     * Indexes for items in Radio Idle, needed for keeping radio idle up to date.
     */
    TInt iBitrateIndex;
    TInt iNameIndex;
    TInt iMetadataIndex;
    TInt iPSNameIndex;
    TInt iNameIndex1;
   	/*
	 * iLogoTimer
	 * Instance of Timer Class To start the Media Animation
	 */

 	CIRLogoTimer* iLogoTimer;

 	 /**
     * Indicates, has this view been activated in the life time of
     * the application.
     */
    TBool iHasBeenActivated;
   	 /**
     * Indicates, has this view been activated in the life time of
     * the application.
     */
    TInt iCommandId;
	TInt iSettingsValue;
	TBool iVolumeMute;
	TInt iLogoIndex;
	TBool iShowBitrate;

	/**
     * Timer for delayed cleanup of connections.
     * Owned.
     */
    CPeriodic* iConnectionCleanupTimer;
public:
    /**
	* The Alf container. Not owned.
	*/
	CIRNowPlayingViewContainer* iContainer;

	// TO Do: The Variable can be made private in Future
    };

#endif      // C_CIRNOWPLAYINGVIEW_H
