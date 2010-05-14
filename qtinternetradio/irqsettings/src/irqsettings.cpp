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
// IRQSettings::timeCorrection()
//
// ---------------------------------------------------------------------------
//
EXPORT_C QString IRQSettings::privatePath()
{
    TPath privatePath = iSettings->PrivatePath();

    return QString::fromUtf16(privatePath.Ptr(), privatePath.Length());
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
// IRQSettings::getManuallyInputtedStationUrl(QString& aUrl)
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getManuallyInputtedStationUrl(QString& aUrl)
{
    TPath stationUrl;
    TRAPD(error, stationUrl = iSettings->GetManuallyInputtedStationUrlL());
    if (KErrNone != error)
    {
        return false;
    }

    aUrl = QString::fromUtf16(stationUrl.Ptr(), stationUrl.Length());
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setManuallyInputtedStationUrl(const QString& aUrl)
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setManuallyInputtedStationUrl(const QString& aUrl)
{
    TPtrC stationUrl(reinterpret_cast<const TUint16*>(aUrl.utf16()));
    TRAP_IGNORE(iSettings->SetManuallyInputtedStationUrlL(stationUrl));
}

// ---------------------------------------------------------------------------
// IRQSettings::getManuallyInputtedStationName(QString& aName)
//
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQSettings::getManuallyInputtedStationName(QString& aName)
{
    TPath stationName;
    TRAPD(error, stationName = iSettings->GetManuallyInputtedStationNameL());
    if (KErrNone != error)
    {
        return false;
    }

    aName = QString::fromUtf16(stationName.Ptr(), stationName.Length());
    return true;
}

// ---------------------------------------------------------------------------
// IRQSettings::setManuallyInputtedStationName(const QString& aName)
//
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQSettings::setManuallyInputtedStationName(const QString& aName)
{
    TPtrC stationName(reinterpret_cast<const TUint16*>(aName.utf16()));
    TRAP_IGNORE(iSettings->SetManuallyInputtedStationNameL(stationName));
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
