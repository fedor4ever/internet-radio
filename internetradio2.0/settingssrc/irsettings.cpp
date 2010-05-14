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
#include <centralrepository.h>

#include "irdebug.h"
#include "irsettings.h"
#include "irinternalcrkeys.h"

// Constants
_LIT( KIRSettingsResourceFileName, "irsettings.rsc" );
const TInt KSix = 6;
const TInt KFifty = 50;
const TInt KTimeSize = 60000000 ;

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
EXPORT_C TBool CIRSettings::IsFlagTermsAndConditionsL()
    {
       IRLOG_DEBUG( "CIRSettings::IsFlagTermsAndConditionsL() - Entering" );
       TInt value = 0;
       User::LeaveIfError( iRepository->Get(KIRTermsAndConditionsFlag, value) );
       IRLOG_DEBUG( "CIRSettings::IsFlagTermsAndConditionsL() - Exiting" );
       return value == 1 ? ETrue : EFalse;
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

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const  TDesC& CIRSettings::GetManuallyInputtedStationUrlL() const
    {
        IRLOG_DEBUG( "CIRSettings::GetManuallyInputtedStationUrlL() - Entering" );
        User::LeaveIfError( iRepository->Get(KIRStationUrl, (TDes&)iStationUrl) );
        IRLOG_DEBUG( "CIRSettings::GetManuallyInputtedStationUrlL() - Exiting" );
        return iStationUrl;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetManuallyInputtedStationUrlL(const TDesC& aUrl)
    {
        IRLOG_DEBUG( "CIRSettings::SetManuallyInputtedStationUrlL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRStationUrl, aUrl) );
        IRLOG_DEBUG( "CIRSettings::SetManuallyInputtedStationUrlL() - Exiting" );
    }

// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C const  TDesC& CIRSettings::GetManuallyInputtedStationNameL() const
    {
        IRLOG_DEBUG( "CIRSettings::GetManuallyInputtedStationNameL() - Entering" );
        User::LeaveIfError( iRepository->Get(KIRStationName, (TDes&)iStationName) );
        IRLOG_DEBUG( "CIRSettings::GetManuallyInputtedStationNameL() - Exiting" );
        return iStationName;
    }
    
// ---------------------------------------------------------------------------
// description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRSettings::SetManuallyInputtedStationNameL(const TDesC& aName)
    {
        IRLOG_DEBUG( "CIRSettings::SetManuallyInputtedStationNameL() - Entering" );
        User::LeaveIfError ( iRepository->Set(KIRStationName, aName) );
        IRLOG_DEBUG( "CIRSettings::SetManuallyInputtedStationNameL() - Exiting" );
    }
