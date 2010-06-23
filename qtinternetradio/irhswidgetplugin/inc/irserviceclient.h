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
* Description: internet radio hs widget service client
*
*/

#ifndef IRSERVICECLIENT_H_
#define IRSERVICECLIENT_H_

// System includes
#include <QObject>
#include <xqappmgr.h>

// User includes
#include "irservicedef.h"
#include "irappinspector.h"

// Forward declarations
class XQAiwRequest;
class IrServiceData;
class QString;
class QVariant;

class IrServiceClient : public QObject
{
Q_OBJECT

public:
    explicit IrServiceClient(QObject *aParent = 0);
    virtual ~IrServiceClient();

public:
    bool startMonitoringIrState();
    void stopMonitoringIrState();

    bool isStationPlayed();
    bool loadStationName(QString &aStationName);
    bool loadStationLogoFlag();    
    
    bool bringIrForeground();
    bool launchIrNormally();
    bool launchIrNowPlaying();

    bool startPlaying();
    bool stopPlaying();
    bool cancelLoading();

signals:
    void stationNameUpdated(const QString &aStationName);
    void stationLogoUpdated(bool aIsLogoAvailable);
    void metaDataUpdated(const QString &aMetaData);
    void irStateChanged(IrAppState::Type aNewState);
    
    void controlFailed();

private slots:
    // used for return value from Monitor Service via Qt HighWay
    void handleMonitorRequestOk(const QVariant &aRetValue);
    void handleMonitorRequestError(int aErrCode, const QString &aErrMessage);

    // used for return value from Control Service via Qt HighWay
    void handleControlRequestOk(const QVariant &aRetValue);
    void handleControlRequestError(int aErrCode, const QString &aErrMessage);
    
    // used for signals from IrAppInspector
    void handleIrRunningStatusChanged(IrAppInspector::IrRunningStatus aNewStatus);

private:
    bool createControlServiceRequest();
    bool createMonitorServiceRequest();
    bool createRefreshServiceRequest();

    enum IrAppVisibilty
    {
        DoNotChange,
        ToForeground,
        ToBackground
    };
    
    bool doSendMonitorRequest();
    bool doSendRefreshRequest();
    bool doSendControlRequest(const QVariant &aArgument, bool aIsSync, IrAppVisibilty aVisibility);
    void prepareRequestInfo(XQAiwRequest *aRequest, IrAppVisibilty aVisibility);
    
    void processNotificationData(const IrServiceData &aServiceData);
    
    void initHsWidgetNoRunStopped();
    void resetHsWidgetExitStopped();
    
private:    
    IrAppInspector   *mIrAppInspector;
    
    bool  mMonitorReqOngoing;
    bool  mControlReqOngoing;
    
    XQApplicationManager mApplicationManager;
    XQAiwRequest        *mMonitorRequest;
    XQAiwRequest        *mRefreshRequest;
    XQAiwRequest        *mControlRequest;
    
    Q_DISABLE_COPY(IrServiceClient)    
};

#endif /* IRSERVICECLIENT_H_ */
