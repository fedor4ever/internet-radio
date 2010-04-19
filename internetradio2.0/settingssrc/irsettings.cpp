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
#include <barsc.h>
#include <bautils.h>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <irsettings.rsg>
#include <pathinfo.h>
#include <s32file.h>
#include <StringLoader.h>

#include "irdebug.h"
#include "irsettings.h"
#include "irinternalcrkeys.h"

// Constants
_LIT( KIRSettingsResourceFileName, "irsettings.rsc" );
const TInt KSix = 6;
const TInt KFifty = 50;
const TInt KTimeSize = 60000000 ;
const TInt KOne = 1;

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSettings* CIRSettings::OpenL()
    {
    IRLOG_DEBUG( "CIRSettings::OpenL() - Entering" );
    CIRSettings* self = static_cast<CIRSettings*>( Dll::Tls() );

    if ( self )
        {
        User::LeaveIfError( self->Open() );
        }
    else
        {
        self = new ( ELeave ) CIRSettings;
        CleanupClosePushL( *self );
        self->ConstructL();
        CleanupStack::Pop( self );
        }
    IRLOG_DEBUG( "CIRSettings::OpenL() - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
CIRSettings::CIRSettings()
    : iResourceFileOffset( KErrNotFound )
    {
    IRLOG_DEBUG( "CIRSettings::CIRSettings" );
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
void CIRSettings::ConstructL()
    {
    IRLOG_DEBUG( "CIRSettings::ConstructL() - Enter" );

	iRepository = CRepository::NewL(KCRUidInternetRadio);
		
    User::LeaveIfError( iFs.Connect() );

    TParse parse;

    // Parses out the resource file name and path to it.

    TFileName resourceFileNameAndPath;
    Dll::FileName( resourceFileNameAndPath );

    parse.Set( KIRSettingsResourceFileName, &KDC_RESOURCE_FILES_DIR, &resourceFileNameAndPath );
    resourceFileNameAndPath = parse.FullName();

    BaflUtils::NearestLanguageFile( iFs, resourceFileNameAndPath );

    IRLOG_DEBUG2("CIRSettings::ConstructL - resourceFileNameAndPath = %S", &resourceFileNameAndPath );
    
    iResourceFileOffset = CCoeEnv::Static()->AddResourceFileL( resourceFileNameAndPath );

    // Parses out the private path of the running process.
	
    User::LeaveIfError( iFs.PrivatePath( iPrivatePath ) );
    
    TParsePtrC phoneMemoryRootPath( PathInfo::PhoneMemoryRootPath() );
    TPtrC drive( phoneMemoryRootPath.Drive() );
    User::LeaveIfError( parse.Set( iPrivatePath, &drive, NULL ) );
    iPrivatePath = parse.FullName();

    BaflUtils::EnsurePathExistsL( iFs, parse.FullName() );

    IRLOG_DEBUG2("CIRSettings::ConstructL - iPrivatePath = %S", &iPrivatePath );
    
    User::LeaveIfError( Dll::SetTls( this ) );
    
    IRLOG_DEBUG( "CIRSettings::ConstructL() - Exit" );
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
CIRSettings::~CIRSettings()
    {
    IRLOG_DEBUG( "CIRSettings::~CIRSettings() - Entering" );
	delete iRepository;
    iFs.Close();

    if ( iResourceFileOffset != KErrNotFound )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    
    Dll::FreeTls();
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::IsFlagCostWarningL()
    {
      IRLOG_DEBUG( "CIRSettings::IsFlagCostWarningL() - Entering" );
      TBool value = ETrue;
      User::LeaveIfError( iRepository->Get(KIRCostWarningFlag, value) );
      IRLOG_DEBUG( "CIRSettings::IsFlagCostWarningL() - Exiting" );
      return value; 
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetFlagCostWarningL()
    {
      IRLOG_DEBUG( "CIRSettings::SetFlagCostWarningL() - Entering" );
      User::LeaveIfError ( iRepository->Set(KIRCostWarningFlag, 1));
      IRLOG_DEBUG( "CIRSettings::SetFlagCostWarningL() - Exiting" );
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::ReSetFlagCostWarningL()
    {
    	IRLOG_DEBUG( "CIRSettings::ReSetFlagCostWarningL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRCostWarningFlag, 0));
        IRLOG_DEBUG( "CIRSettings::ReSetFlagCostWarningL() - Exiting" );
     }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::IsFlagTermsAndConditionsL()
    {
       IRLOG_DEBUG( "CIRSettings::IsFlagTermsAndConditionsL() - Entering" );
       TBool value = ETrue;
       User::LeaveIfError( iRepository->Get(KIRTermsAndConditionsFlag, value) );
       IRLOG_DEBUG( "CIRSettings::IsFlagTermsAndConditionsL() - Exiting" );
       return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetFlagTermsAndConditionsL()
    {
       IRLOG_DEBUG( "CIRSettings::SetFlagTermsAndConditionsL() - Entering" );
       User::LeaveIfError ( iRepository->Set(KIRTermsAndConditionsFlag, 1));
       IRLOG_DEBUG( "CIRSettings::SetFlagTermsAndConditionsL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::ReSetFlagTermsAndConditionsL()
    {
    	IRLOG_DEBUG( "CIRSettings::ReSetFlagTermsAndConditionsL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRTermsAndConditionsFlag, 0));
        IRLOG_DEBUG( "CIRSettings::ReSetFlagTermsAndConditionsL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetIrappVersionL(const TDesC& aIRAppVersion)
    {
    	 IRLOG_DEBUG( "CIRSettings::SetIrappVersionL() - Entering" );
         User::LeaveIfError ( iRepository->Set(KIRIrappVer, aIRAppVersion) );
         IRLOG_DEBUG( "CIRSettings::SetIrappVersionL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRSettings::GetIrappVersionL() const
    {
    		IRLOG_DEBUG( "CIRSettings::GetIrappVersionL() - Entering" );
    		User::LeaveIfError( iRepository->Get(KIRIrappVer, (TDes&)iIrappVersion) );
    		IRLOG_DEBUG( "CIRSettings::GetIrappVersionL() - Exiting" );
    		return iIrappVersion;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetIRIDL(const TDesC& aIrid)
    {
    		IRLOG_DEBUG( "CIRSettings::SetIRIDL() - Entering" );
            User::LeaveIfError ( iRepository->Set(KIRDefaultIrId, aIrid) );
            IRLOG_DEBUG( "CIRSettings::SetIRIDL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRSettings::GetIRIDL() const
    {
    		IRLOG_DEBUG( "CIRSettings::GetIRIDL() - Entering" );
    		User::LeaveIfError( iRepository->Get(KIRDefaultIrId, (TDes&)iIRID) );
    		IRLOG_DEBUG( "CIRSettings::GetIRIDL() - Exiting" );
    		return iIRID;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const  TDesC& CIRSettings::GetISDSBaseUrlL() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetISDSBaseUrlL() - Entering" );
    	User::LeaveIfError( iRepository->Get(KIRDefaultIsdsUrl, (TDes&)iISDSBaseUrl) );
    	IRLOG_DEBUG( "CIRSettings::GetISDSBaseUrlL() - Exiting" );
       	return iISDSBaseUrl;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetISDSBaseUrlL(const TDesC& aIsdsUrl)
    {
    	IRLOG_DEBUG( "CIRSettings::SetISDSBaseUrlL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRDefaultIsdsUrl, aIsdsUrl) );
        IRLOG_DEBUG( "CIRSettings::SetISDSBaseUrlL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetVolumeSetting() const
    {
    IRLOG_DEBUG( "CIRSettings::GetVolumeSetting() - Entering" );
    TInt value = KSix;
    TInt err = iRepository->Get(KIRDefaultPlayVolume, value);
    if(err)
	    {
        IRLOG_DEBUG( "CIRSettings::GetVolumeSetting() - Error" );
	    }
 
    IRLOG_DEBUG( "CIRSettings::GetVolumeSetting() - Exiting" );
    return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetVolumeSettingL(TInt aPlayVolume)
    {
      IRLOG_DEBUG( "CIRSettings::SetVolumeSettingL() - Entering" );
      User::LeaveIfError ( iRepository->Set(KIRDefaultPlayVolume, aPlayVolume));
      IRLOG_DEBUG( "CIRSettings::SetVolumeSettingL() - Exiting" );
    }
    

    
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetManuallyAddedChId() const
    {
    IRLOG_DEBUG( "CIRSettings::GetManuallyAddedChId() - Entering" );
    TInt value = KOne;
    TInt err = iRepository->Get(KIRUserDefinedRunningChannelId, value);
    if(err)
	    {
        IRLOG_DEBUG( "CIRSettings::GetManuallyAddedChId() - Error" );
	    }
    IRLOG_DEBUG( "CIRSettings::GetManuallyAddedChId() - Exiting" );
    return value;
    
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetManuallyAddedChIdL()
    {
    IRLOG_DEBUG( "CIRSettings::SetManuallyAddedChIdL() - Entering" );
    iRunningChId++;
    User::LeaveIfError ( iRepository->Set(KIRUserDefinedRunningChannelId, iRunningChId));
    IRLOG_DEBUG( "CIRSettings::SetManuallyAddedChIdL() - Exiting" );
    }
    


// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::IsUpdateAvailableL()
    {
    	IRLOG_DEBUG( "CIRSettings::IsUpdateAvailableL() - Entering" );
       TBool value = ETrue;
       User::LeaveIfError( iRepository->Get(KIRUpdateAvailablity, value) );
       IRLOG_DEBUG( "CIRSettings::IsUpdateAvailableL() - Exiting" );
       return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetUpdateAvailableL()
    {
    	IRLOG_DEBUG( "CIRSettings::SetUpdateAvailableL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRUpdateAvailablity, 1));
        IRLOG_DEBUG( "CIRSettings::SetUpdateAvailableL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::ReSetUpdateAvailableL()
    {
    	IRLOG_DEBUG( "CIRSettings::ReSetUpdateAvailableL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRUpdateAvailablity, 0));
        IRLOG_DEBUG( "CIRSettings::ReSetUpdateAvailableL() - Exiting" );
    }
    
 
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetLogRunningNo() const
    {
		IRLOG_DEBUG( "CIRSettings::GetLogRunningNo() - Entering" );
        TInt value = 1;
        TInt err = iRepository->Get(KIRUserDefinedLogRunningNumber, value);
        if(err)
	        {
    	    IRLOG_DEBUG( "CIRSettings::GetLogRunningNo() - Error" );
    	    }
    	IRLOG_DEBUG( "CIRSettings::GetLogRunningNo() - Exiting" );
    	return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetLogRunningNoL()
    {
    IRLOG_DEBUG( "CIRSettings::SetLogRunningNoL() - Entering" );
    TInt value = GetLogRunningNo();
    value++;
    User::LeaveIfError ( iRepository->Set(KIRUserDefinedLogRunningNumber,value));
    IRLOG_DEBUG( "CIRSettings::SetLogRunningNoL() - Exiting" );
    }
    
  
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetTimeOutL(TInt aTimeOut)
    {
       IRLOG_DEBUG( "CIRSettings::SetTimeOutL() - Entering" );
       User::LeaveIfError ( iRepository->Set(KIRTimeOutDuration, aTimeOut) );
       IRLOG_DEBUG( "CIRSettings::SetTimeOutL() - Exiting" );
       }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetTimeOut() const
    {
    IRLOG_DEBUG( "CIRSettings::GetTimeOut() - Entering" );
    TInt value = KTimeSize;
    TInt err = iRepository->Get(KIRTimeOutDuration, value) ;
    if(err)
	    {
        IRLOG_DEBUG( "CIRSettings::GetTimeOut() - Error" );
	    }

    IRLOG_DEBUG( "CIRSettings::GetTimeOut() - Exiting" );
    return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetSilencePeriodL(TUint aSilencePeriod)
    {
    	IRLOG_DEBUG( "CIRSettings::SetSilencePeriodL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRSilencePeriodDuration, (TInt)aSilencePeriod) );
        IRLOG_DEBUG( "CIRSettings::SetSilencePeriodL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CIRSettings::GetSilencePeriodL() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetSilencePeriodL() - Entering" );
    	TInt value =0;
    	User::LeaveIfError( iRepository->Get(KIRSilencePeriodDuration, value) );
    	IRLOG_DEBUG( "CIRSettings::GetSilencePeriodL() - Exiting" );
    	return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetSilencePeriodStartL(const TDesC& aSilencePeriodStart)
    {
    	IRLOG_DEBUG( "CIRSettings::SetSilencePeriodStartL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRSilencePeriodStartingTime, aSilencePeriodStart) );
        IRLOG_DEBUG( "CIRSettings::SetSilencePeriodStartL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRSettings::GetSilencePeriodStartL() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetSilencePeriodStartL() - Entering" );
    	User::LeaveIfError( iRepository->Get(KIRSilencePeriodStartingTime,
    					  (TDes&)iSilencePeriodStartTime) );
    	IRLOG_DEBUG( "CIRSettings::GetSilencePeriodStartL() - Exiting" );
        return iSilencePeriodStartTime;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::DisplayAccessPointL() const
    {
    	IRLOG_DEBUG( "CIRSettings::DisplayAccessPointL() - Entering" );
      	TBool value = ETrue;
        User::LeaveIfError( iRepository->Get(KIRDefaultAccessPoint, value) );
        IRLOG_DEBUG( "CIRSettings::DisplayAccessPointL() - Exiting" );
    	return value; 
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetDisplayAccessPointL()
    {
    IRLOG_DEBUG( "CIRSettings::SetDisplayAccessPointL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRDefaultAccessPoint, 1) );
    IRLOG_DEBUG( "CIRSettings::SetDisplayAccessPointL() - Exiting" );
    }
    
EXPORT_C void CIRSettings::SetStereoModeL(TInt val)
{
	IRLOG_DEBUG( "CIRSettings::SetStereoModeL() - Entering" );
	User::LeaveIfError ( iRepository->Set(KIRStereoMode, val));
	IRLOG_DEBUG( "CIRSettings::SetStereoModeL() - Exiting" );
	
}

EXPORT_C TInt CIRSettings::GetStereoMode()
{
		IRLOG_DEBUG( "CIRSettings::GetStereoMode() - Entering" );
	    TInt value = 0;
	    TInt err = iRepository->Get(KIRStereoMode, value);
	    if(err)
		    {
	        IRLOG_DEBUG( "CIRSettings::GetStereoMode() - Error" );
		    }
	    IRLOG_DEBUG( "CIRSettings::GetStereoMode() - Exiting" );
    	return value;
}

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::ReSetDisplayAccessPointL()
    {
    IRLOG_DEBUG( "CIRSettings::ReSetDisplayAccessPointL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRDefaultAccessPoint, 0) );
    IRLOG_DEBUG( "CIRSettings::ReSetDisplayAccessPointL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetBearerIdL(TUint32 aBearerId)
    {
    IRLOG_DEBUG( "CIRSettings::SetBearerIdL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRUserDefinedBearerID, (TInt)aBearerId));
    IRLOG_DEBUG( "CIRSettings::SetBearerIdL() - Exiting" );        
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetBearerIdL() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetBearerIdL() - Entering" );
    	TInt value = 0;
	    User::LeaveIfError( iRepository->Get(KIRUserDefinedBearerID, value) );
	    IRLOG_DEBUG( "CIRSettings::GetBearerIdL() - Exiting" );
    	return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetNetworkIdL(TUint32 aBearerId)
    {
    IRLOG_DEBUG( "CIRSettings::SetNetworkIdL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRUserDefinedNetworkID, (TInt)aBearerId) );
    IRLOG_DEBUG( "CIRSettings::SetNetworkIdL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetNetworkIdL() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetNetworkIdL() - Entering" );
    	TInt value = 0;
	    User::LeaveIfError( iRepository->Get(KIRUserDefinedNetworkID, value) );
	    IRLOG_DEBUG( "CIRSettings::GetNetworkIdL() - Exiting" );
    	return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetApIdL(TUint32 aBearerId)
    {
      IRLOG_DEBUG( "CIRSettings::SetApIdL() - Entering" );
      User::LeaveIfError ( iRepository->Set(KIRUserDefinedAccessPoint , (TInt)aBearerId) );
      IRLOG_DEBUG( "CIRSettings::SetApIdL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetApIdL() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetApIdL() - Entering" );
    	TInt value = 0;
	  	User::LeaveIfError( iRepository->Get(KIRUserDefinedAccessPoint, value) );
	  	IRLOG_DEBUG( "CIRSettings::GetApIdL() - Exiting" );
    	return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetUserSavedApSettingsL( TUint32 aBearerId,TUint32 aNetworkId,
													TUint32 aApId)
    {
    	IRLOG_DEBUG( "CIRSettings::SetUserSavedApSettingsL() - Entering" );
    	SetBearerIdL( aBearerId );
    	SetNetworkIdL( aNetworkId );
    	SetApIdL( aApId );
    	IRLOG_DEBUG( "CIRSettings::SetUserSavedApSettingsL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::GetUserSavedApSettingsL( TUint32& aBearerId,TUint32& aNetworkId,
												    TUint32& aApId) const
    {
      IRLOG_DEBUG( "CIRSettings::GetUserSavedApSettingsL() - Entering" );
      TInt value = 0;
      User::LeaveIfError( iRepository->Get(KIRUserDefinedAccessPoint, value) );
      aApId = value;
      
      value =0;
      User::LeaveIfError( iRepository->Get(KIRUserDefinedNetworkID, value) );
      aNetworkId = value;
      
      value =0;
      User::LeaveIfError( iRepository->Get(KIRUserDefinedBearerID, value) );
      aBearerId = value; 
      IRLOG_DEBUG( "CIRSettings::GetUserSavedApSettingsL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetGPRSBitrateQualityL(TInt aBitrate)
    {
    IRLOG_DEBUG( "CIRSettings::SetGPRSBitrateQualityL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRGprsBitRate, aBitrate) );
    IRLOG_DEBUG( "CIRSettings::SetGPRSBitrateQualityL() - Exiting" );
    }
   
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetGPRSBitrateQuality() const
    {
    IRLOG_DEBUG( "CIRSettings::GetGPRSBitrateQuality() - Entering" );
    TInt value =0;
    TInt err = iRepository->Get(KIRGprsBitRate, value);
    if(err)
	    {
   	    IRLOG_DEBUG( "CIRSettings::GetGPRSBitrateQuality() - Error" );
	    }
   	IRLOG_DEBUG( "CIRSettings::GetGPRSBitrateQuality() - Exiting" );
    return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetWiFiBitrateQualityL(TInt aBitrate)
    {
    IRLOG_DEBUG( "CIRSettings::SetWiFiBitrateQualityL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRWifiBitRate, aBitrate) );
    IRLOG_DEBUG( "CIRSettings::SetWiFiBitrateQualityL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetWiFiBitrateQuality() const
    {
    IRLOG_DEBUG( "CIRSettings::GetWiFiBitrateQuality() - Entering" );
    TInt value =2;
    TInt err = iRepository->Get(KIRWifiBitRate, value);
    if(err)
	    {
        IRLOG_DEBUG( "CIRSettings::GetWiFiBitrateQuality() - Error" );
	    }

    IRLOG_DEBUG( "CIRSettings::GetWiFiBitrateQuality() - Exiting" );
    return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::Set3GBitrateQualityL(TInt aBitrate)
    {
    IRLOG_DEBUG( "CIRSettings::Set3GBitrateQualityL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIR3GBitRate, aBitrate) );
    IRLOG_DEBUG( "CIRSettings::Set3GBitrateQualityL() - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::Get3GBitrateQuality() const
    {
    	IRLOG_DEBUG( "CIRSettings::Get3GBitrateQuality() - Entering" );
    	TInt value = 1;
    	TInt err = iRepository->Get(KIR3GBitRate, value);
    	if(err)
	    	{
	        IRLOG_DEBUG( "CIRSettings::Get3GBitrateQuality() - Error" );
	    	}

	    IRLOG_DEBUG( "CIRSettings::Get3GBitrateQuality() - Exiting" );
    	return value;
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetPreferredQualityL(TInt aQualityValue)
    {
    IRLOG_DEBUG( "CIRSettings::SetPreferredQualityL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRPreferredQuality, aQualityValue) );
    IRLOG_DEBUG( "CIRSettings::SetPreferredQualityL() - Exiting" );
    }

    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::GetPreferredQuality() const
    {
    	IRLOG_DEBUG( "CIRSettings::GetPreferredQuality() - Entering" );
    	TInt value = 0;
    	TInt err = iRepository->Get(KIRPreferredQuality, value);
    	if(err)
	    	{
	        IRLOG_DEBUG( "CIRSettings::GetPreferredQuality() - Error" );
	    	}

	    IRLOG_DEBUG( "CIRSettings::GetPreferredQuality() - Exiting" );
    	return value;
    }

EXPORT_C TInt CIRSettings::GetSongHistoryShow() const
{
    IRLOG_DEBUG( "CIRSettings::GetSongHistoryShow() - Entering" );
    TInt value = 0;
    TInt err = iRepository->Get(KIRSongHistoryShow, value);
    if(err)
        {
        IRLOG_DEBUG( "CIRSettings::GetSongHistoryShow() - Error" );
        }

    IRLOG_DEBUG( "CIRSettings::GetSongHistoryShow() - Exiting" );
    return value;
}

EXPORT_C void CIRSettings::SetSongHisotryShowL(TInt aShowFlag)
{
    IRLOG_DEBUG( "CIRSettings::SetSongHisotryShow() - Entering" );
    User::LeaveIfError(iRepository->Set(KIRSongHistoryShow, aShowFlag));
    IRLOG_DEBUG( "CIRSettings::SetSongHisotryShow() - Exiting" );
}
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetMaxPresetCountL(TInt aMaxCount)
    {
    IRLOG_DEBUG( "CIRSettings::SetMaxPresetCountL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRDefaultMaxPresetCount, aMaxCount) );
    IRLOG_DEBUG( "CIRSettings::SetMaxPresetCountL() - Exiting" );    
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::MaxPresetCount()
    {
    	IRLOG_DEBUG( "CIRSettings::MaxPresetCount() - Entering" );
    	TInt value = KFifty;
    	TInt err = iRepository->Get(KIRDefaultMaxPresetCount, value) ;
    	if(err)
	    	{
	        IRLOG_DEBUG( "CIRSettings::MaxPresetCount() - Error" );
	    	}

	    IRLOG_DEBUG( "CIRSettings::MaxPresetCount() - Exiting" );
    	return value;
    }
    
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetTimeCorrectionL(TInt aCorrection)
    {
    	IRLOG_DEBUG( "CIRSettings::SetTimeCorrectionL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRDefaultTimeCorrection, aCorrection) );
        IRLOG_DEBUG( "CIRSettings::SetTimeCorrectionL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSettings::TimeCorrection() const
    {
    	IRLOG_DEBUG( "CIRSettings::TimeCorrection() - Entering" );
        TInt value = 0;
        TInt err = iRepository->Get(KIRDefaultTimeCorrection, value) ;
        if(err)
	        {
	        IRLOG_DEBUG( "CIRSettings::TimeCorrection() - Error" );
	        }

	    IRLOG_DEBUG( "CIRSettings::TimeCorrection() - Exiting" );
    	return value;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIRSettings::PrivatePath() const
    {
    IRLOG_DEBUG( "CIRSettings::PrivatePath" );
    return iPrivatePath;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const RMobilePhone::TMobilePhoneSubscriberId& CIRSettings::SubscriberIdL() const
    {
    	IRLOG_DEBUG( "CIRSettings::SubscriberIdL() - Entering" );
        User::LeaveIfError( iRepository->Get(KIRUserDefinedSubscriberId, (TDes&)iSubscriberId) );
	    IRLOG_DEBUG( "CIRSettings::SubscriberIdL() - Exiting" );
        return iSubscriberId;
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetSubscriberIdL( const RMobilePhone::TMobilePhoneSubscriberId& 
											 aSubscriberId )
    {
       IRLOG_DEBUG( "CIRSettings::SetSubscriberIdL() - Entering" );
       User::LeaveIfError ( iRepository->Set(KIRUserDefinedSubscriberId, (TDes&)aSubscriberId) );
       IRLOG_DEBUG( "CIRSettings::SetSubscriberIdL() - Exiting" );
    }


//Added for ALR/SNAP

    
// ---------------------------------------------------------------------------
// SetUserDefinedSelectionL()
// Sets the UserDefinedSelection in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetUserDefinedSelectionL(TUint32 aUserDefinedSelection)
    {
    IRLOG_DEBUG( "CIRSettings::SetUserDefinedSelectionL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRUserDefinedSelection, (TInt)aUserDefinedSelection) );
    IRLOG_DEBUG( "CIRSettings::SetUserDefinedSelectionL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// GetUserDefinedSelectionL()
// Gets the UserDefinedSelection from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetUserDefinedSelectionL() const
    {
    IRLOG_DEBUG( "CIRSettings::GetUserDefinedSelectionL() - Entering" );
	TInt value = 0;
    User::LeaveIfError( iRepository->Get(KIRUserDefinedSelection, value) );
    IRLOG_DEBUG( "CIRSettings::GetUserDefinedSelectionL() - Exiting" );
	return value;
    }

// ---------------------------------------------------------------------------
// SetDestinationIdL()
// Sets the DestinationId in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetDestinationIdL(TUint32 aDestinationId)
    {
    IRLOG_DEBUG( "CIRSettings::SetDestinationIdL() - Entering" );
	User::LeaveIfError ( iRepository->Set(KIRUserDefinedDestinationId, (TInt)aDestinationId) );
	IRLOG_DEBUG( "CIRSettings::SetDestinationIdL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// GetDestinationIdL()
// Gets the DestinationId from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetDestinationIdL() const
    {
    IRLOG_DEBUG( "CIRSettings::GetDestinationIdL() - Entering" );
    TInt value = 0;
    User::LeaveIfError( iRepository->Get(KIRUserDefinedDestinationId, value) );
    IRLOG_DEBUG( "CIRSettings::GetDestinationIdL() - Exiting" );
	return value;
    } 
    
// ---------------------------------------------------------------------------
// IsFlagIfAnyFavL()
// Method to determine if any channel was added to favorites
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::IsFlagIfAnyFavL()
    {
	  IRLOG_DEBUG( "CIRSettings::IsFlagIfAnyFavL() - Entering" );
	  TBool value = EFalse;
	  User::LeaveIfError( iRepository->Get(KIRIfAnyFavFlag, value) );
	  IRLOG_DEBUG( "CIRSettings::IsFlagIfAnyFavL() - Exiting" );
	  return value;
    }
    
// ---------------------------------------------------------------------------
// SetFlagIfAnyFavL()
//  Method to set boolean value when any channel was added to favorites
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetFlagIfAnyFavL()
    {
      IRLOG_DEBUG( "CIRSettings::SetFlagIfAnyFavL() - Entering" );
      User::LeaveIfError ( iRepository->Set(KIRIfAnyFavFlag, 1));
      IRLOG_DEBUG( "CIRSettings::SetFlagIfAnyFavL() - Exiting" );
    }

// ---------------------------------------------------------------------------
// SetStartingViewIdL()
// Sets the starting view Id in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetStartingViewIdL(TUint32 aStartingViewId)
    {
    IRLOG_DEBUG( "CIRSettings::SetStartingViewIdL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRStartingViewId, (TInt)aStartingViewId) );
    IRLOG_DEBUG( "CIRSettings::SetStartingViewIdL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// GetStartingViewIdL()
// Gets the starting view Id from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetStartingViewIdL() const
    {
    IRLOG_DEBUG( "CIRSettings::GetStartingViewIdL() - Entering" );
    TInt value = 0;
    User::LeaveIfError( iRepository->Get(KIRStartingViewId, value) );
    IRLOG_DEBUG( "CIRSettings::GetStartingViewIdL() - Exiting" );
    return value;
    } 

// ---------------------------------------------------------------------------
// SetStartingViewParameterL()
// Sets the starting view parameter in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetStartingViewParameterL(TUint32 aParameter)
    {
    IRLOG_DEBUG( "CIRSettings::SetStartingViewParameterL() - Entering" );
    User::LeaveIfError ( iRepository->Set(KIRStartingViewParameter, (TInt)aParameter) );
    IRLOG_DEBUG( "CIRSettings::SetStartingViewParameterL() - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// GetStartingViewParameterL()
// Gets the starting view parameter from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CIRSettings::GetStartingViewParameterL() const
    {
    IRLOG_DEBUG( "CIRSettings::GetStartingViewParameterL() - Entering" );
    TInt value = 0;
    User::LeaveIfError( iRepository->Get(KIRStartingViewParameter, value) );
    IRLOG_DEBUG( "CIRSettings::GetStartingViewParameterL() - Exiting" );
    return value;
    } 

// ---------------------------------------------------------------------------
// SetStickyViewFlagL()
// Sets the sticky view flag in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetStickyViewFlagL(TBool aFlag)
    {
      IRLOG_DEBUG( "CIRSettings::SetStickyViewFlagL() - Entering" );
      User::LeaveIfError ( iRepository->Set(KIRStickyViewFlag, aFlag));
      IRLOG_DEBUG( "CIRSettings::SetStickyViewFlagL() - Exiting" );
    }

// ---------------------------------------------------------------------------
// GetStickyViewFlagL()
// Gets the sticky view flag from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::GetStickyViewFlagL()
    {
	  IRLOG_DEBUG( "CIRSettings::GetStickyViewFlagL() - Entering" );
	  TBool value = EFalse;
	  User::LeaveIfError( iRepository->Get(KIRStickyViewFlag, value) );
	  IRLOG_DEBUG( "CIRSettings::GetStickyViewFlagL() - Exiting" );
	  return value;
    }

// ---------------------------------------------------------------------------
// GetGlobalAdvFlagL()
// Gets the global advertisement flag from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CIRSettings::GetGlobalAdvFlagL()
{
  IRLOG_DEBUG( "CIRSettings::GetGlobalAdvFlagL() - Entering" );
  TBool value = ETrue;
  User::LeaveIfError( iRepository->Get(KIRGlobalAdvFlag, value) );
  IRLOG_DEBUG( "CIRSettings::GetGlobalAdvFlagL() - Exiting" );
  return value;
}
