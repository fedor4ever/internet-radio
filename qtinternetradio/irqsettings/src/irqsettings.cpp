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
#include "irsettings.h"
#include "irqsettings.h"

// ---------------------------------------------------------------------------
// IRQSettings::openInstance()
// Static function to get a singleton instance of IRQSettings
// @return IRQSettings *
// ---------------------------------------------------------------------------
//
EXPORT_C IRQSettings* IRQSettings::openInstance()
{
    // Get singleton instance
    IRQSettings* irqsettings =
                           reinterpret_cast<IRQSettings*>(Dll::Tls());

    if (NULL == irqsettings)
    {
        TRAPD(error, irqsettings = createInstanceL());
        if (KErrNone != error)
        {
            delete irqsettings;
            irqsettings = NULL;
            Dll::SetTls(NULL);
        }
    }
    else
    {
        irqsettings->iSingletonInstances++;
    }

    return irqsettings;
}

// ---------------------------------------------------------------------------
// IRQSettings::closeInstance()
// Close a singleton instance of IRQSettings
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}

// ---------------------------------------------------------------------------
// IRQSettings::isFlagCostWarning()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::isFlagCostWarning(bool& aFlag)
{
    TRAPD(error, aFlag = (bool)iSettings->IsFlagCostWarningL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setFlagCostWarning()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setFlagCostWarning()
{
    TRAP_IGNORE(iSettings->SetFlagCostWarningL());
}

// ---------------------------------------------------------------------------
// IRQSettings::reSetFlagCostWarning()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::reSetFlagCostWarning()
{
    TRAP_IGNORE(iSettings->ReSetFlagCostWarningL());
}

// ---------------------------------------------------------------------------
// IRQSettings::isFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::isFlagTermsAndConditions(bool& aFlag)
{
    TRAPD(error, aFlag = (bool)iSettings->IsFlagTermsAndConditionsL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setFlagTermsAndConditions()
{
    TRAP_IGNORE(iSettings->SetFlagTermsAndConditionsL());
}

// ---------------------------------------------------------------------------
// IRQSettings::reSetFlagTermsAndConditions()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::reSetFlagTermsAndConditions()
{
    TRAP_IGNORE(iSettings->ReSetFlagTermsAndConditionsL());
}

// ---------------------------------------------------------------------------
// IRQSettings::setIrappVersion()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setIrappVersion(const QString& aIRAppVersion)
{
    if (aIRAppVersion.length() > KMaxVersionName)
    {
        return;
    }
    
    TPtrC irAppVersion(reinterpret_cast<const TUint16*>(aIRAppVersion.utf16()));
    TRAP_IGNORE(iSettings->SetIrappVersionL(irAppVersion));
}

// ---------------------------------------------------------------------------
// IRQSettings::getIrappVersion()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getIrappVersion(QString& aAppVersion)
{
    TVersionName irAppVersion;
    TRAPD(error, irAppVersion = iSettings->GetIrappVersionL());
    if (KErrNone != error)
    {
        return false;
    }

    aAppVersion = QString::fromUtf16(irAppVersion.Ptr(), irAppVersion.Length());
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setIRID()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setIRID(const QString& aIrid)
{
    TPtrC irid(reinterpret_cast<const TUint16*>(aIrid.utf16()));
    TRAP_IGNORE(iSettings->SetIRIDL(irid));
}

// ---------------------------------------------------------------------------
// IRQSettings::getIRID()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getIRID(QString& aIRID)
{
    TDeviceInfo irid;
    TRAPD(error, irid = iSettings->GetIRIDL());
    if (KErrNone != error)
    {
        return false;
    }

    aIRID = QString::fromUtf16(irid.Ptr(), irid.Length());
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::getISDSBaseUrl()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getISDSBaseUrl(QString& aUrl)
{
    TPath isdsBaseUrl;
    TRAPD(error, isdsBaseUrl = iSettings->GetISDSBaseUrlL());
    if (KErrNone != error)
    {
        return false;
    }

    aUrl = QString::fromUtf16(isdsBaseUrl.Ptr(), isdsBaseUrl.Length());
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setISDSBaseUrl()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setISDSBaseUrl(const QString& aIsdsUrl)
{
    TPtrC isdsUrl(reinterpret_cast<const TUint16*>(aIsdsUrl.utf16()));
    TRAP_IGNORE(iSettings->SetISDSBaseUrlL(isdsUrl));
}

// ---------------------------------------------------------------------------
// IRQSettings::getVolumeSetting()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getVolumeSetting()
{
    return (int)iSettings->GetVolumeSetting();
}

// ---------------------------------------------------------------------------
// IRQSettings::setVolumeSetting()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setVolumeSetting(int aPlayVolume)
{
    TRAP_IGNORE(iSettings->SetVolumeSettingL((TInt)aPlayVolume));
}

// ---------------------------------------------------------------------------
// IRQSettings::getManuallyAddedChId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getManuallyAddedChId()
{
    return (int)iSettings->GetManuallyAddedChId();
}

// ---------------------------------------------------------------------------
// IRQSettings::setManuallyAddedChId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setManuallyAddedChId()
{
    TRAP_IGNORE(iSettings->SetManuallyAddedChIdL());
}

// ---------------------------------------------------------------------------
// IRQSettings::isUpdateAvailable()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::isUpdateAvailable(bool& aFlag)
{
    TRAPD(error, aFlag = (bool)iSettings->IsUpdateAvailableL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setUpdateAvailable()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setUpdateAvailable()
{
    TRAP_IGNORE(iSettings->SetUpdateAvailableL());
}

// ---------------------------------------------------------------------------
// IRQSettings::reSetUpdateAvailable()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::reSetUpdateAvailable()
{
    TRAP_IGNORE(iSettings->ReSetUpdateAvailableL());
}

// ---------------------------------------------------------------------------
// IRQSettings::getLogRunningNo()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getLogRunningNo()
{
    return (int)iSettings->GetLogRunningNo();
}

// ---------------------------------------------------------------------------
// IRQSettings::setLogRunningNo()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setLogRunningNo()
{
    TRAP_IGNORE(iSettings->SetLogRunningNoL());
}

// ---------------------------------------------------------------------------
// IRQSettings::setTimeOut()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setTimeOut(int aTimeOut)
{
    TRAP_IGNORE(iSettings->SetTimeOutL((TInt)aTimeOut));
}

// ---------------------------------------------------------------------------
// IRQSettings::getTimeOut()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getTimeOut()
{
    return (int)iSettings->GetTimeOut();
}

// ---------------------------------------------------------------------------
// IRQSettings::setSilencePeriod()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setSilencePeriod(unsigned int aSilencePeriod)
{
    TRAP_IGNORE(iSettings->SetSilencePeriodL((TUint)aSilencePeriod));
}

// ---------------------------------------------------------------------------
// IRQSettings::getSilencePeriod()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getSilencePeriod(unsigned int& aPeriod)
{
    TRAPD(error, aPeriod = iSettings->GetSilencePeriodL());
    if (KErrNone != error)
    {
        return false;
    }
    return false;
}

// ---------------------------------------------------------------------------
// IRQSettings::setSilencePeriodStart()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setSilencePeriodStart(const QString& aSilencePeriodStart)
{
    if( aSilencePeriodStart.length() > KMaxLengthSilencePeriod )
    { 
        return;
    }
    
    TPtrC silencePeriodStart(reinterpret_cast<const TUint16*>(aSilencePeriodStart.utf16()));
    TRAP_IGNORE(iSettings->SetSilencePeriodStartL(silencePeriodStart));
}

// ---------------------------------------------------------------------------
// IRQSettings::getSilencePeriodStart()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getSilencePeriodStart(QString& aPeriodStart)
{
    TBuf<KMaxLengthSilencePeriod> silencePeriodStart;
    TRAPD(error, silencePeriodStart = iSettings->GetSilencePeriodStartL());
    if (KErrNone != error)
    {
        return false;
    }

    aPeriodStart = QString::fromUtf16(silencePeriodStart.Ptr(), silencePeriodStart.Length());
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::displayAccessPoint()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::displayAccessPoint(bool& aFlag)
{
    TRAPD(error, aFlag = (bool)iSettings->DisplayAccessPointL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setDisplayAccessPoint()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setDisplayAccessPoint()
{
    TRAP_IGNORE(iSettings->SetDisplayAccessPointL());
}

// ---------------------------------------------------------------------------
// IRQSettings::reSetDisplayAccessPoint()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::reSetDisplayAccessPoint()
{
    TRAP_IGNORE(iSettings->ReSetDisplayAccessPointL());
}

// ---------------------------------------------------------------------------
// IRQSettings::setBearerId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setBearerId(unsigned long aBearerId)
{
    TRAP_IGNORE(iSettings->SetBearerIdL((TUint32)aBearerId));
}

// ---------------------------------------------------------------------------
// IRQSettings::getBearerId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getBearerId(unsigned long& aBearerId)
{
    TRAPD(error, aBearerId = iSettings->GetBearerIdL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setNetworkId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setNetworkId(unsigned long aBearerId)
{
    TRAP_IGNORE(iSettings->SetNetworkIdL((TUint32)aBearerId));
}

// ---------------------------------------------------------------------------
// IRQSettings::getNetworkId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getNetworkId(unsigned long& aNetworkId)
{
    TRAPD(error, aNetworkId = iSettings->GetNetworkIdL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setApId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setApId(unsigned long aBearerId)
{
    TRAP_IGNORE(iSettings->SetApIdL((TUint32)aBearerId));
}

// ---------------------------------------------------------------------------
// IRQSettings::getApId()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getApId(unsigned long& aApId)
{
    TRAPD(error, aApId = iSettings->GetApIdL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setUserSavedApSettings()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setUserSavedApSettings(unsigned long aBearerId,
                               unsigned long aNetworkId,unsigned long aApId)
{
    TRAP_IGNORE(iSettings->SetUserSavedApSettingsL((TUint32)aBearerId,
                (TUint32)aNetworkId, (TUint32)aApId));
}

// ---------------------------------------------------------------------------
// IRQSettings::getUserSavedApSettings()
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getUserSavedApSettings(unsigned long& aBearerId,
                    unsigned long& aNetworkId,unsigned long& aApId)
{
    TRAPD(error, iSettings->GetUserSavedApSettingsL(aBearerId, aNetworkId, aApId));
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setGPRSBitrateQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setGPRSBitrateQuality(int aBitrate)
{
    TRAP_IGNORE(iSettings->SetGPRSBitrateQualityL((TInt)aBitrate));
}

// ---------------------------------------------------------------------------
// IRQSettings::getGPRSBitrateQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getGPRSBitrateQuality()
{
    return (int)iSettings->GetGPRSBitrateQuality();
}

// ---------------------------------------------------------------------------
// IRQSettings::setWiFiBitrateQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setWiFiBitrateQuality(int aBitrate)
{
    TRAP_IGNORE(iSettings->SetWiFiBitrateQualityL((TInt)aBitrate));
}

// ---------------------------------------------------------------------------
// IRQSettings::getWiFiBitrateQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getWiFiBitrateQuality()
{
    return (int)iSettings->GetWiFiBitrateQuality();
}

// ---------------------------------------------------------------------------
// IRQSettings::set3GBitrateQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::set3GBitrateQuality(int aBitrate)
{
    TRAP_IGNORE(iSettings->Set3GBitrateQualityL((TInt)aBitrate));
}

// ---------------------------------------------------------------------------
// IRQSettings::get3GBitrateQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::get3GBitrateQuality()
{
    return (int)iSettings->Get3GBitrateQuality();
}

// ---------------------------------------------------------------------------
// IRQSettings::setMaxPresetCount()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setMaxPresetCount(int aMaxCount)
{
    TRAP_IGNORE(iSettings->SetMaxPresetCountL((TInt)aMaxCount));
}

// ---------------------------------------------------------------------------
// IRQSettings::maxPresetCount()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::maxPresetCount()
{
    return (int)iSettings->MaxPresetCount();
}

// ---------------------------------------------------------------------------
// IRQSettings::setTimeCorrection()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setTimeCorrection(int aCorrection)
{
    TRAP_IGNORE(iSettings->SetTimeCorrectionL((TInt)aCorrection));
}

// ---------------------------------------------------------------------------
// IRQSettings::timeCorrection()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::timeCorrection()
{
    return (int)iSettings->TimeCorrection();
}

// ---------------------------------------------------------------------------
// IRQSettings::timeCorrection()
//
// ---------------------------------------------------------------------------
//
EXPORT_C QString IRQSettings::privatePath()
{
    TPath privatePath = iSettings->PrivatePath();

    return QString::fromUtf16(privatePath.Ptr(), privatePath.Length());
}

// ---------------------------------------------------------------------------
// IRQSettings::setStereoMode()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setStereoMode(int val)
{
    TRAP_IGNORE(iSettings->SetStereoModeL((TInt)val));
}

// ---------------------------------------------------------------------------
// IRQSettings::getStereoMode()
//
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSettings::getStereoMode()
{
    return (int)iSettings->GetStereoMode();
}

EXPORT_C void IRQSettings::setSongHistoryShow(int aShowFlag)
{
    TRAP_IGNORE(iSettings->SetSongHisotryShowL(aShowFlag));
}

EXPORT_C int IRQSettings::getSongHistoryShow()
{                     
    return iSettings->GetSongHistoryShow();
}

// ---------------------------------------------------------------------------
// IRQSettings::isFlagIfAnyFav()
// Methods to determine if any channel was added to favorites
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::isFlagIfAnyFav(bool& aFlag)
{
    TRAPD(error, aFlag = iSettings->IsFlagIfAnyFavL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setFlagIfAnyFav()
// Methods to determine if any channel was added to favorites
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setFlagIfAnyFav()
{
    TRAP_IGNORE(iSettings->SetFlagIfAnyFavL());
}

// ---------------------------------------------------------------------------
// IRQSettings::setUserDefinedSelection()
// Added for ALR/SNAP. Sets the UserDefinedSelection in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setUserDefinedSelection(unsigned long aUserDefinedSelection)
{
    TRAP_IGNORE(iSettings->SetUserDefinedSelectionL((TUint32)aUserDefinedSelection));
}

// ---------------------------------------------------------------------------
// IRQSettings::getUserDefinedSelection()
// Added for ALR/SNAP. Gets the UserDefinedSelection from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getUserDefinedSelection(TNetworkConnection& aSelection)
{
    TUint32 selection = 0;
    TRAPD(error, selection = iSettings->GetUserDefinedSelectionL());
    if (KErrNone != error)
    {
        return false;
    }

    aSelection = (TNetworkConnection)selection;
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setDestinationId()
// Added for ALR/SNAP. Sets the DestinationId in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setDestinationId(unsigned long aDestinationId)
{
    TRAP_IGNORE(iSettings->SetDestinationIdL((TUint32)aDestinationId));
}

// ---------------------------------------------------------------------------
// IRQSettings::getDestinationId()
// Added for ALR/SNAP. Gets the DestinationId from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getDestinationId(unsigned long& aDestinationId)
{
    TRAPD(error, aDestinationId = iSettings->GetDestinationIdL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::subscriberId()
// Returns the current saved subscriber identifier (IMSI code).
// @return  The saved subscriber identifier.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::subscriberId(RMobilePhone::TMobilePhoneSubscriberId& aIRMobilePhoneSubscriberId)
{
    TRAPD(error, aIRMobilePhoneSubscriberId = iSettings->SubscriberIdL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setSubscriberId()
// Sets the subscriber identifier (IMSI code).
// @param   Subscriber identifier to set.
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setSubscriberId(const RMobilePhone::TMobilePhoneSubscriberId& aSubscriberId)
{
    if( aSubscriberId.Length() > RMobilePhone::KIMSISize )
    {
        return;
    }
    
    TRAP_IGNORE(iSettings->SetSubscriberIdL(aSubscriberId));
}

// ---------------------------------------------------------------------------
// IRQSettings::setStartingViewId()
// Sets the starting view Id in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setStartingViewId(unsigned long aStartingViewId)
{
    TRAP_IGNORE(iSettings->SetStartingViewIdL((TUint32)aStartingViewId));
}

// ---------------------------------------------------------------------------
// IRQSettings::getStartingViewId()
// Gets the starting view Id from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getStartingViewId(TIRViewId& aStartingViewId)
{
    TUint32 startingViewId = 0;
    TRAPD(error, startingViewId = iSettings->GetStartingViewIdL());
    if (KErrNone != error)
    {
        return false;
    }

	aStartingViewId = (TIRViewId)startingViewId;
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setStartingViewParameter()
// Sets the starting view parameter in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setStartingViewParameter(unsigned long aParameter)
{
    TRAP_IGNORE(iSettings->SetStartingViewParameterL((TUint32)aParameter));
}

// ---------------------------------------------------------------------------
// IRQSettings::getStartingViewParameter()
// Gets the starting view parameter from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getStartingViewParameter(TIRViewParameter& aParameter)
{
    TUint32 parameter = 0;
    TRAPD(error, parameter = iSettings->GetStartingViewParameterL());
    if (KErrNone != error)
    {
        return false;
    }

    aParameter = (TIRViewParameter)parameter;
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setStickyViewFlag()
// Sets the sticky view flag in cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setStickyViewFlag(bool aFlag)
{
    TRAP_IGNORE(iSettings->SetStickyViewFlagL(aFlag));
}

// ---------------------------------------------------------------------------
// IRQSettings::getStickyViewFlag()
// Gets the sticky view flag from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getStickyViewFlag(bool& aFlag)
{
    TRAPD(error, aFlag = iSettings->GetStickyViewFlagL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}


// ---------------------------------------------------------------------------
// IRQSettings::setPreferredQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setPreferredQuality(IRQPreferredQuality aQualityValue)
{
    TInt value = 0;
    switch(aQualityValue)
    {
        case EIRQHighQuality:
            value = 1;
            break;
		default:
            break;
    }
        
    TRAP_IGNORE(iSettings->SetPreferredQualityL(value));
}

// ---------------------------------------------------------------------------
// IRQSettings::getPreferredQuality()
//
// ---------------------------------------------------------------------------
//
EXPORT_C IRQPreferredQuality IRQSettings::getPreferredQuality() const
{
    TInt value = iSettings->GetPreferredQuality();
    switch(value)
    {
        case 1:
            return EIRQHighQuality;
        default:
            return EIRQStandardQuality;
    }
}

// ---------------------------------------------------------------------------
// IRQSettings::getGlobalAdvFlag()
// Gets the global advertisement flag from cenrep
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getGlobalAdvFlag(bool& aFlag)
{
    TRAPD(error, aFlag = iSettings->GetGlobalAdvFlagL());
    if (KErrNone != error)
    {
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::IRQSettings()
// Constructor
// ---------------------------------------------------------------------------
//
IRQSettings::IRQSettings()
{
}

// ---------------------------------------------------------------------------
// IRQSettings::~IRQSettings()
// Destructor
// ---------------------------------------------------------------------------
//
IRQSettings::~IRQSettings()
{
    if (iSettings)
    {
        iSettings->Close();
    }
}

// ---------------------------------------------------------------------------
// IRQSettings::createInstanceL()
// Creates IRQSettings instance
// @return IRQSettings*
// ---------------------------------------------------------------------------
//
IRQSettings* IRQSettings::createInstanceL()
{
    IRQSettings* irqsettings = new (ELeave) IRQSettings();
    irqsettings->constructL();
    User::LeaveIfError(Dll::SetTls(irqsettings));
    irqsettings->iSingletonInstances = 1;

    return irqsettings;
}

// ---------------------------------------------------------------------------
// IRQSettings::constructL()
// Two-Phase Constructor.
// ---------------------------------------------------------------------------
//
void IRQSettings::constructL()
{
    // Get singleton instance
    iSettings = CIRSettings::OpenL();
}
