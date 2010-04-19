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
#ifndef C_CIRSETTINGS_H
#define C_CIRSETTINGS_H

#include <etelmm.h>
#include <f32file.h>
#include <centralrepository.h>

// CLEANUP REQUIRED

const TInt KMaxLengthIAPID = 256;
const TInt KMaxLengthIRAppVersion = 50;
const TInt KMaxLengthIrId = 32;
const TInt KMaxLengthSilencePeriod = 32;
const TInt KMaxLengthUrl = 500;
const TInt KMaxLengthFileName = 500;

// END CLEANUP

NONSHARABLE_CLASS( CIRSettings ) : public CObject
    {

public:

    IMPORT_C static CIRSettings* OpenL();
    ~CIRSettings();

// CLEANUP REQUIRED

    IMPORT_C TBool IsFlagCostWarningL();
    IMPORT_C void SetFlagCostWarningL();
    IMPORT_C void ReSetFlagCostWarningL();
    IMPORT_C TBool IsFlagTermsAndConditionsL();
    IMPORT_C void SetFlagTermsAndConditionsL();
    IMPORT_C void ReSetFlagTermsAndConditionsL();
    IMPORT_C void SetIrappVersionL(const TDesC& aIRAppVersion);
    IMPORT_C const TDesC& GetIrappVersionL() const;
        
    IMPORT_C void SetIRIDL(const TDesC& aIrid);
    IMPORT_C const TDesC& GetIRIDL() const;
    IMPORT_C const  TDesC& GetISDSBaseUrlL() const;
    IMPORT_C void SetISDSBaseUrlL(const TDesC& aIsdsUrl);
    IMPORT_C TInt GetVolumeSetting() const;
    IMPORT_C void SetVolumeSettingL(TInt aPlayVolume);
    IMPORT_C TInt GetManuallyAddedChId() const;
    IMPORT_C void SetManuallyAddedChIdL();
    IMPORT_C TBool IsUpdateAvailableL();
    IMPORT_C void SetUpdateAvailableL();
	IMPORT_C void ReSetUpdateAvailableL();
    IMPORT_C TInt GetLogRunningNo() const;
    IMPORT_C void SetLogRunningNoL();
    IMPORT_C void SetTimeOutL(TInt aTimeOut);
    IMPORT_C TInt GetTimeOut() const;
    IMPORT_C void SetSilencePeriodL(TUint aSilencePeriod);
    IMPORT_C TUint GetSilencePeriodL() const;
    IMPORT_C void SetSilencePeriodStartL(const TDesC& aSilencePeriodStart);
    IMPORT_C const TDesC& GetSilencePeriodStartL() const;
    IMPORT_C TBool DisplayAccessPointL() const;
    IMPORT_C void SetDisplayAccessPointL();
    IMPORT_C void ReSetDisplayAccessPointL();
    IMPORT_C void SetBearerIdL(TUint32 aBearerId);
    IMPORT_C TUint32 GetBearerIdL() const;
    IMPORT_C void SetNetworkIdL(TUint32 aBearerId);
    IMPORT_C TUint32 GetNetworkIdL() const;
    IMPORT_C void SetApIdL(TUint32 aBearerId);
    IMPORT_C TUint32 GetApIdL() const;
    IMPORT_C void SetUserSavedApSettingsL( TUint32 aBearerId,TUint32 aNetworkId,TUint32 aApId);
    IMPORT_C void GetUserSavedApSettingsL( TUint32& aBearerId,
    				TUint32& aNetworkId,TUint32& aApId) const;
    IMPORT_C void SetGPRSBitrateQualityL(TInt aBitrate);
    IMPORT_C TInt GetGPRSBitrateQuality() const;
    IMPORT_C void SetWiFiBitrateQualityL(TInt aBitrate);
    IMPORT_C TInt GetWiFiBitrateQuality() const;
    IMPORT_C void Set3GBitrateQualityL(TInt aBitrate);
    IMPORT_C TInt Get3GBitrateQuality() const;
    IMPORT_C void SetPreferredQualityL(TInt aQualityValue);
    IMPORT_C TInt GetPreferredQuality() const; 
    IMPORT_C void SetSongHisotryShowL(TInt aShowFlag);
    IMPORT_C TInt GetSongHistoryShow() const;
    IMPORT_C void SetMaxPresetCountL(TInt aMaxCount);
    IMPORT_C TInt MaxPresetCount();
    IMPORT_C void SetTimeCorrectionL(TInt aCorrection);
    IMPORT_C TInt TimeCorrection() const;
    IMPORT_C const TDesC& PrivatePath() const;
    IMPORT_C void SetStereoModeL(TInt val);
    IMPORT_C TInt GetStereoMode();


	//Methods to determine if any channel was added to favorites
	IMPORT_C TBool IsFlagIfAnyFavL();
    IMPORT_C void SetFlagIfAnyFavL();
    
    //Added for ALR/SNAP
    /**
     * SetUserDefinedSelectionL()
     * Sets the UserDefinedSelection in cenrep
     */
    IMPORT_C void SetUserDefinedSelectionL(TUint32 aUserDefinedSelection);
    
    /**
     * GetUserDefinedSelectionL()
     * Gets the UserDefinedSelection from cenrep
     */
    IMPORT_C TUint32 GetUserDefinedSelectionL() const;
    
    /**
     * SetDestinationIdL()
     * Sets the DestinationId in cenrep
     */
    IMPORT_C void SetDestinationIdL(TUint32 aDestinationId);
    
    /**
     * GetDestinationIdL()
     * Gets the DestinationId from cenrep
     */
    IMPORT_C TUint32 GetDestinationIdL() const;


// END CLEANUP

    /**
     * Returns the current saved subscriber identifier (IMSI code).
     *
     * @return  The saved subscriber identifier.
     */
    IMPORT_C const RMobilePhone::TMobilePhoneSubscriberId& SubscriberIdL() const;
    
    /**
     * Sets the subscriber identifier (IMSI code).
     *
     * @param   Subscriber identifier to set.
     */
    IMPORT_C void SetSubscriberIdL( const RMobilePhone::TMobilePhoneSubscriberId& aSubscriberId );
    
	// Methods for starting view
    /**
     * SetStartingViewIdL()
     * Sets the starting view Id in cenrep
     */
    IMPORT_C void SetStartingViewIdL(TUint32 aStartingViewId);
   
    /**
     * GetStartingViewIdL()
     * Gets the starting view Id from cenrep
     */
    IMPORT_C TUint32 GetStartingViewIdL() const;
   
    /**
     * SetStartingViewParameterL()
     * Sets the starting view parameter in cenrep
     */
    IMPORT_C void SetStartingViewParameterL(TUint32 aParameter);
  
    /**
     * GetStartingViewParameterL()
     * Gets the starting view parameter from cenrep
     */
    IMPORT_C TUint32 GetStartingViewParameterL() const;
	
	/**
     * SetStickyViewFlagL()
     * Sets the sticky view flag in cenrep
     */
    IMPORT_C void SetStickyViewFlagL(TBool aFlag);
	
	/**
     * GetStickyViewFlagL()
     * Gets the sticky view flag from cenrep
     */
	IMPORT_C TBool GetStickyViewFlagL();

	/**
	 * GetGlobalAdvFlagL()
	 * Gets the global advertisement flag from cenrep
	 */
	IMPORT_C TBool GetGlobalAdvFlagL();

private:

    CIRSettings();
    void ConstructL();

private:

	CRepository *iRepository;
    RFs iFs;

// CLEANUP REQUIRED

 	//key KAccessPoints
      TInt iAccessPoints;//!number of accesspoints available 
	//key KRunningChId
      TInt iRunningChId; //! Channelid for isds channels 
    // zero once it reaches the value.
    // to arrange the preset data,
	//key KIrappVersion
      TBuf<KMaxLengthIRAppVersion> iIrappVersion;//! irapp version number, 
    //key KIRID
      TBuf<KMaxLengthIrId> iIRID;//! ir-id supplied by isds. cannot change
    // key KSilencePeriodStartTime
      TBuf<KMaxLengthSilencePeriod> iSilencePeriodStartTime; //!Universal timestamp for the time of decline of an Update
    //key KISDSBaseUrl
      TBuf<KMaxLengthUrl> iISDSBaseUrl;//!the base url of isds
    TBuf<KMaxLengthFileName> iPrivatePath;
    
// END CLEANUP

    /** The current subscriber identifier (IMSI code). */
    RMobilePhone::TMobilePhoneSubscriberId iSubscriberId;

    TInt iResourceFileOffset;

    TFileName iSettingsFileName;
    TFileName iBackedUpSettingsFileName;

    };
    
#endif // C_CIRSETTINGS_H
