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
#ifndef IRQSETTINGS_H_
#define IRQSETTINGS_H_

#include <QString>
#include <etelmm.h>

#include "irqenums.h"
#include "irviewdefinitions.h"

enum TNetworkConnection
{
    EIRAlwaysAsk,
    EIRDefaultConnection,
    EIRDestination,
    EIRConnectionMethod
};

class CIRSettings;

/**
 * This class provides the interface to IR settings component
 *
 * IRQSettings follows a singleton pattern
 * hence the destructor is private and to destroy it we need to use an API
 *
 */

class IRQSettings
{
public:

    /**
     *  Get the instance of IRQSettings
     *  @return IRQSettings*
     */
    IMPORT_C static IRQSettings* openInstance();
    IMPORT_C void closeInstance();

    IMPORT_C bool isFlagCostWarning(bool& aFlag);
    IMPORT_C void setFlagCostWarning();
    IMPORT_C void reSetFlagCostWarning();
    IMPORT_C bool isFlagTermsAndConditions(bool& aFlag);
    IMPORT_C void setFlagTermsAndConditions();
    IMPORT_C void reSetFlagTermsAndConditions();
    IMPORT_C void setIrappVersion(const QString& aIRAppVersion);
    IMPORT_C bool getIrappVersion(QString& aIrappVersion);

    IMPORT_C void setIRID(const QString& aIrid);
    IMPORT_C bool getIRID(QString& aIRID);
    IMPORT_C bool getISDSBaseUrl(QString& aISDSBaseUrl);
    IMPORT_C void setISDSBaseUrl(const QString& aIsdsUrl);
    IMPORT_C int getVolumeSetting();
    IMPORT_C void setVolumeSetting(int aPlayVolume);
    IMPORT_C int getManuallyAddedChId();
    IMPORT_C void setManuallyAddedChId();
    IMPORT_C bool isUpdateAvailable(bool& aFlag);
    IMPORT_C void setUpdateAvailable();
    IMPORT_C void reSetUpdateAvailable();
    IMPORT_C int getLogRunningNo();
    IMPORT_C void setLogRunningNo();
    IMPORT_C void setTimeOut(int aTimeOut);
    IMPORT_C int getTimeOut();
    IMPORT_C void setSilencePeriod(unsigned int aSilencePeriod);
    IMPORT_C bool getSilencePeriod(unsigned int& aSilencePeriod);
    IMPORT_C void setSilencePeriodStart(const QString& aSilencePeriodStart);
    IMPORT_C bool getSilencePeriodStart(QString& aSilencePeriodStart);
    IMPORT_C bool displayAccessPoint(bool& aFlag);
    IMPORT_C void setDisplayAccessPoint();
    IMPORT_C void reSetDisplayAccessPoint();
    IMPORT_C void setBearerId(unsigned long aBearerId);
    IMPORT_C bool getBearerId(unsigned long& aBearerId);
    IMPORT_C void setNetworkId(unsigned long aBearerId);
    IMPORT_C bool getNetworkId(unsigned long& aNetworkId);
    IMPORT_C void setApId(unsigned long aBearerId);
    IMPORT_C bool getApId(unsigned long& aApId);
    IMPORT_C void setUserSavedApSettings( unsigned long aBearerId,unsigned long aNetworkId,unsigned long aApId);
    IMPORT_C bool getUserSavedApSettings( unsigned long& aBearerId,
                    unsigned long& aNetworkId,unsigned long& aApId);
    IMPORT_C void setGPRSBitrateQuality(int aBitrate);
    IMPORT_C int getGPRSBitrateQuality();
    IMPORT_C void setWiFiBitrateQuality(int aBitrate);
    IMPORT_C int getWiFiBitrateQuality();
    IMPORT_C void set3GBitrateQuality(int aBitrate);
    IMPORT_C int get3GBitrateQuality();
    IMPORT_C void setMaxPresetCount(int aMaxCount);
    IMPORT_C int maxPresetCount();
    IMPORT_C void setTimeCorrection(int aCorrection);
    IMPORT_C int timeCorrection();
    IMPORT_C QString privatePath();
    IMPORT_C void setStereoMode(int val);
    IMPORT_C int getStereoMode();
    IMPORT_C int getSongHistoryShow();
    IMPORT_C void setSongHistoryShow(int aShowFlag);


    //Methods to determine if any channel was added to favorites
    IMPORT_C bool isFlagIfAnyFav(bool& aFlag);
    IMPORT_C void setFlagIfAnyFav();

    //Added for ALR/SNAP
    /**
     * setUserDefinedSelection()
     * Sets the UserDefinedSelection in cenrep
     */
    IMPORT_C void setUserDefinedSelection(unsigned long aUserDefinedSelection);

    /**
     * getUserDefinedSelection()
     * Gets the UserDefinedSelection from cenrep
     */
    IMPORT_C bool getUserDefinedSelection(TNetworkConnection& aSelection);

    /**
     * setDestinationId()
     * Sets the DestinationId in cenrep
     */
    IMPORT_C void setDestinationId(unsigned long aDestinationId);

    /**
     * getDestinationId()
     * Gets the DestinationId from cenrep
     */
    IMPORT_C bool getDestinationId(unsigned long& aDestinationId);

    /**
     * Returns the current saved subscriber identifier (IMSI code).
     *
     * @return  The saved subscriber identifier.
     */
    IMPORT_C bool subscriberId(RMobilePhone::TMobilePhoneSubscriberId& aIRMobilePhoneSubscriberId);

    /**
     * Sets the subscriber identifier (IMSI code).
     *
     * @param   Subscriber identifier to set.
     */
    IMPORT_C void setSubscriberId( const RMobilePhone::TMobilePhoneSubscriberId& aSubscriberId );
    
    /**
     * setStartingViewId()
     * Sets the starting view Id in cenrep
     */
    IMPORT_C void setStartingViewId(unsigned long aStartingViewId);

    /**
     * getStartingViewId()
     * Gets the starting view Id from cenrep
     */
    IMPORT_C bool getStartingViewId(TIRViewId& aStartingViewId);

    /**
     * setStartingViewParameter()
     * Sets the starting view parameter in cenrep
     */
    IMPORT_C void setStartingViewParameter(unsigned long aParameter);

    /**
     * getStartingViewParameter()
     * Gets the starting view parameter from cenrep
     */
    IMPORT_C bool getStartingViewParameter(TIRViewParameter& aParameter);
    
    /**
     * setStickyViewFlag()
     * Sets the sticky view flag in cenrep
     */
    IMPORT_C void setStickyViewFlag(bool aFlag);

    /**
     * getStickyViewFlag()
     * Gets the sticky view flag from cenrep
     */
    IMPORT_C bool getStickyViewFlag(bool& aFlag);
    
    /**
     * setPreferredQuality()
     * Sets the Preferred Quality in cenrep
     */
         
    IMPORT_C void setPreferredQuality(IRQPreferredQuality aQualityValue);

    /**
     * getPreferredQuality()
     * Gets the Preferred Quality from cenrep
     */    
    IMPORT_C IRQPreferredQuality getPreferredQuality() const;      

    /**
     * getGlobalAdvFlag()
     * Gets the global adv flag from cenrep. True: enable NAE adv
     */
    IMPORT_C bool getGlobalAdvFlag(bool& aFlag);

private:

    /**
     *  Default constructor must be private
	 */
    IRQSettings();

    ~IRQSettings();

    /**
     *  Creates IRQSettings instance
     */
    static IRQSettings* createInstanceL();
    
    void constructL();

private:

    /**
     *  IRQSettings singleton instance
     */
    CIRSettings* iSettings;

    /**
     * Number of objects currently referring to the singleton object IRQSettings
     */
    int iSingletonInstances;
};

#endif /* IRQSETTINGS_H_ */
