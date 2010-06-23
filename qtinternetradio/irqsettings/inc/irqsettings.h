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

    IMPORT_C bool isFlagTermsAndConditions(bool& aFlag);
    IMPORT_C void setFlagTermsAndConditions();
    IMPORT_C void reSetFlagTermsAndConditions();

    IMPORT_C bool getIRID(QString& aIRID);
    IMPORT_C int getVolumeSetting();
    IMPORT_C void setVolumeSetting(int aPlayVolume);
    IMPORT_C void setTimeOut(int aTimeOut);
    IMPORT_C int getTimeOut();
    IMPORT_C void setMaxPresetCount(int aMaxCount);
    IMPORT_C int maxPresetCount();
    IMPORT_C QString privatePath();
    IMPORT_C int getSongHistoryShow();
    IMPORT_C void setSongHistoryShow(int aShowFlag);

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

    IMPORT_C bool getManuallyInputtedStationUrl(QString& aUrl);
    IMPORT_C void setManuallyInputtedStationUrl(const QString& aUrl);
    IMPORT_C bool getManuallyInputtedStationName(QString& aName);
    IMPORT_C void setManuallyInputtedStationName(const QString& aName);
private:

    /**
     *  Default constructor must be private
	 */
    IRQSettings();

    ~IRQSettings();

    /**
     *  Creates IRQSettings instance
     */
    static IRQSettings* createInstance();
	static void doCreateInstanceL(IRQSettings * aQsettings);
    
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
