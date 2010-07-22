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
* Description: Internet Radio App Insecptor
*
*/

// System includes
#include <QVariant>
#include <xqsettingsmanager.h>
#include <xqpublishandsubscribeutils.h>

// User includes
#include "irappinspector.h"
#include "irservicedef.h"

// Constructor
IrAppInspector::IrAppInspector(QObject *aParent) :
    QObject(aParent),
    mSettingsManager(new XQSettingsManager(this)),
    mIrRunningStatusKey(XQSettingsKey::TargetPublishAndSubscribe,
                        KInternetRadioPSUid,
                        KInternetRadioStartupKey)   
{   
    QObject::connect(mSettingsManager, SIGNAL(itemDeleted(XQSettingsKey)),
        this, SLOT(handleIrExit(XQSettingsKey)));
    QObject::connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(handleIrRunningStatusChanged(XQSettingsKey, QVariant)));
}

// Destructor 
IrAppInspector::~IrAppInspector()
{
    stopInspectingIrRunningStatus();
}


/*!
 Handling of deletion of listened P&S key.
 \param aKey Deleted key.
 */
void IrAppInspector::handleIrExit(const XQSettingsKey &aKey)
{
    if (KInternetRadioPSUid == aKey.uid()
         && KInternetRadioStartupKey == aKey.key())
    {
        emit irRunningStatusChanged(Exiting);
        
        // since the P&S key has been deleted, should re-start the inspecting
        startInspectingIrRunningStatus();
    }
}


void IrAppInspector::handleIrRunningStatusChanged(const XQSettingsKey &aKey,
                                                 const QVariant &aValue)
{
    if (KInternetRadioPSUid == aKey.uid()
         && KInternetRadioStartupKey == aKey.key())
    {
        if (aValue.isValid())
        {
            if (aValue.canConvert(QVariant::Int))
            {
                emit irRunningStatusChanged(StartingUp);
            }
        }
    }
}

bool IrAppInspector::startInspectingIrRunningStatus()
{
    return mSettingsManager->startMonitoring(mIrRunningStatusKey);
}

void IrAppInspector::stopInspectingIrRunningStatus()
{
    mSettingsManager->stopMonitoring(mIrRunningStatusKey);
}

bool IrAppInspector::isIrRunning()
{
    QVariant value = mSettingsManager->readItemValue(mIrRunningStatusKey);
    if (value.isValid())
    {
        if (value.canConvert(QVariant::Int))
        {
            return true;
        }
    }
    return false;  
}

