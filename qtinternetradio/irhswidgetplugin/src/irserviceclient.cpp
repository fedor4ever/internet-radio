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

// System includes
#include <QSettings>
#include <QVariant>
#include <QString>
#include <hbicon.h>
#include <xqserviceglobal.h>
#include <xqservicerequest.h>


// User includes
#include "irserviceclient.h"
#include "irservicedata.h"

// Constants
// For services via Qt HighWay
static const QString KIrServiceName = "internet_radio_10_1";
static const QString KIrServiceControlInterfaceName = "com.nokia.symbian.IInternetRadioControl";
static const QString KIrServiceControlOperation     = "handleCmd(int)";
static const QString KIrServiceMonitorInterfaceName = "com.nokia.symbian.IInternetRadioMonitor";
static const QString KIrServiceMonitorOperation     = "registerNotifications()";
static const QString KIrServiceRefreshOperation     = "refreshAllData()";


// Initialize the static member variable
QMutex IrServiceClient::mMutex;
int IrServiceClient::mRef = 0;
bool IrServiceClient::mMonitoringStarted = false;    
IrServiceClient *IrServiceClient::mInstatnce = NULL;


// ==================== MEMBER FUNCTIONS ======================
// Static function to get a singleton instance of IrServiceClient
IrServiceClient* IrServiceClient::openInstance()
{
    mMutex.lock();
    if (NULL == mInstatnce)
    {
        mInstatnce = new IrServiceClient();
    }       
    if (mInstatnce != NULL)
    {
        mRef++;
    }
    mMutex.unlock();
    return mInstatnce;
}

// Close a singleton instance of IrServiceClient
void IrServiceClient::closeInstance()
{
    if (mInstatnce != NULL)
    {
        mMutex.lock();
        if (mRef > 0)
        {
            mRef--;
        }
        
        if (0 == mRef)
        {
            delete mInstatnce;
        }
        mMutex.unlock();
    }
}

// Constructor
IrServiceClient::IrServiceClient() :
    mIrAppInspector(NULL),
    mMonitorReqOngoing(false),    
    mControlReqOngoing(false),
    mMonitorRequest(NULL),
    mRefreshRequest(NULL),
    mControlRequest(NULL)
{
    mIrAppInspector = new IrAppInspector(this);
    
    QObject::connect(mIrAppInspector, SIGNAL(irRunningStatusChanged(IrAppInspector::IrRunningStatus)),
        this, SLOT(handleIrRunningStatusChanged(IrAppInspector::IrRunningStatus)));
}

// Destructor
IrServiceClient::~IrServiceClient()
{   
    delete mMonitorRequest;
    mMonitorRequest = NULL;
    
    delete mRefreshRequest;
    mRefreshRequest = NULL;
    
    delete mControlRequest;
    mControlRequest = NULL;
}

bool IrServiceClient::startMonitoringIrState()
{
    if (mMonitoringStarted)
    {
        refreshAllActiveHsWidgets();
        return true;
    }
       
    // if this is the first active hs widget
    mMonitoringStarted = true;
    bool retVal = false;
    if (mIrAppInspector->isIrRunning())
    {
        retVal = doSendRefreshRequest();
    }
    else
    {
        if (isStationPlayed())
        {
            initHsWidgetNoRunStopped();
        }
        else
        {
            notifyIrStateChanged(IrAppState::NoRunInit);
        }
    }
    
    return mIrAppInspector->startInspectingIrRunningStatus() && retVal;    
}


void IrServiceClient::initHsWidgetNoRunStopped()
{    
    QString stationName;
    if (loadStationName(stationName))
    {
        notifyStationNameUpdated(stationName);
    }
    else
    {
#ifdef SUBTITLE_STR_BY_LOCID
        notifyStationNameUpdated(hbTrId("txt_irad_info_unnamed_station"));
#else
        notifyStationNameUpdated(hbTrId("Unnamed station"));
#endif
    }
    
    notifyStationLogoUpdated(loadStationLogoFlag());
    
    notifyIrStateChanged(IrAppState::NoRunStopped);    
}

void IrServiceClient::refreshAllActiveHsWidgets()
{
    emit stationNameUpdated(mStationName);
    emit stationLogoUpdated(mStationLogoAvailable);
    emit metaDataUpdated(mMetaData);
    emit irStateChanged(mIrState);
}

/******************************************************************
****************   Monitor Service Request  **********************
*******************************************************************/
bool IrServiceClient::doSendMonitorRequest()
{
    if(mMonitorReqOngoing)
    {
        return false;
    }
    
    if (NULL == mMonitorRequest
        &&!createMonitorServiceRequest())
    {
        return false;
    }

    prepareRequestInfo(mMonitorRequest, DoNotChange);
    return mMonitorRequest->send();
}

bool IrServiceClient::createMonitorServiceRequest()
{
    bool retVal = false;

    if (NULL == mMonitorRequest)
    {
        mMonitorRequest = mApplicationManager.create(KIrServiceName,
                KIrServiceMonitorInterfaceName,
                KIrServiceMonitorOperation,
                false);

        if (mMonitorRequest)
        {
            retVal = true;

            connect(mMonitorRequest, SIGNAL(requestOk(const QVariant&)),
                this, SLOT(handleMonitorRequestOk(const QVariant&)));
            connect(mMonitorRequest, SIGNAL(requestError(int,const QString&)),
                this, SLOT(handleMonitorRequestError(int,const QString&)));

            mMonitorRequest->setEmbedded(false);
            mMonitorRequest->setSynchronous(false);
        }
    }
    else
    {
        retVal = true;
    }

    return retVal;
}

// ==================== Refresh Service Request ===========================
bool IrServiceClient::doSendRefreshRequest()
{
    if(mMonitorReqOngoing)
    {
        return false;
    }
    
    if (NULL == mRefreshRequest
        && !createRefreshServiceRequest())
    {
        return false;
    }
       
    prepareRequestInfo(mRefreshRequest, DoNotChange);        
    return mRefreshRequest->send();
}

bool IrServiceClient::createRefreshServiceRequest()
{
    bool retVal = false;

    if (NULL == mRefreshRequest)
    {
        mRefreshRequest = mApplicationManager.create(KIrServiceName,
                KIrServiceMonitorInterfaceName,
                KIrServiceRefreshOperation,
                false);

        if (mRefreshRequest)
        {
            retVal = true;

            connect(mRefreshRequest, SIGNAL(requestOk(const QVariant&)),
                this, SLOT(handleMonitorRequestOk(const QVariant&)));
            connect(mRefreshRequest, SIGNAL(requestError(int,const QString&)),
                this, SLOT(handleMonitorRequestError(int,const QString&)));
            
            mRefreshRequest->setEmbedded(false);
            mRefreshRequest->setSynchronous(false);
        }
    }
    else
    {
        retVal = true;
    }

    return retVal;
}

void IrServiceClient::handleMonitorRequestOk(const QVariant &aRetValue)
{
    mMonitorReqOngoing = false;
    
    doSendMonitorRequest();

    if (aRetValue.isValid()
         && aRetValue.canConvert<IrServiceDataList>())
    {
        IrServiceDataList dataList;
        dataList = qVariantValue<IrServiceDataList>(aRetValue);
        foreach (const IrServiceData& data, dataList)
        {
            processNotificationData(data);
        }
    }
}

void IrServiceClient::handleMonitorRequestError(int aErrCode, const QString &aErrMessage)
{
    mMonitorReqOngoing = false;
    
    Q_UNUSED(aErrCode);
    Q_UNUSED(aErrMessage);
}

/******************************************************************
****************   Control Service Request  **********************
*******************************************************************/
bool IrServiceClient::bringIrForeground()
{
    QVariant commandArgument(QVariant::Int);
    commandArgument.setValue((int)IrServiceCmd::Foreground);
    return doSendControlRequest(commandArgument, true, ToForeground);
}

bool IrServiceClient::launchIrNormally()
{
    QVariant commandArgument(QVariant::Int);
    commandArgument.setValue((int)IrServiceCmd::LaunchNormally);
    return doSendControlRequest(commandArgument, true, ToForeground);
}

bool IrServiceClient::launchIrNowPlaying()
{
    QVariant commandArgument(QVariant::Int);
    commandArgument.setValue((int)IrServiceCmd::LaunchNowPlayingView);
    return doSendControlRequest(commandArgument, false, ToBackground);
}


bool IrServiceClient::startPlaying()
{
    QVariant commandArgument(QVariant::Int);
    commandArgument.setValue((int)IrServiceCmd::Play);
    return doSendControlRequest(commandArgument, true, DoNotChange);
}


bool IrServiceClient::stopPlaying()
{
    QVariant commandArgument(QVariant::Int);
    commandArgument.setValue((int)IrServiceCmd::Stop);
    return doSendControlRequest(commandArgument, true, DoNotChange);
}

bool IrServiceClient::cancelLoading()
{
    QVariant commandArgument(QVariant::Int);
    commandArgument.setValue((int)IrServiceCmd::Cancel);
    return doSendControlRequest(commandArgument, true, DoNotChange);
}

bool IrServiceClient::doSendControlRequest(const QVariant &aArgument, bool aIsSync, IrAppVisibilty aVisibility)
{
    if(mControlReqOngoing)
    {
        return false;
    }
    
    if (NULL == mControlRequest &&
        !createControlServiceRequest())
    {
        return false;
    }

    QList<QVariant> arguments;
    arguments.append(aArgument);
    mControlRequest->setArguments(arguments);

    prepareRequestInfo(mControlRequest, aVisibility);
    mControlRequest->setSynchronous(aIsSync);    
    return mControlRequest->send();
}

bool IrServiceClient::createControlServiceRequest()
{
    bool retVal = false;

    if (NULL == mControlRequest)
    {
        mControlRequest = mApplicationManager.create(KIrServiceName,
                KIrServiceControlInterfaceName,
                KIrServiceControlOperation,
                false);

        if (mControlRequest)
        {
            retVal = true;
            
            connect(mControlRequest, SIGNAL(requestOk(const QVariant&)),
                this, SLOT(handleControlRequestOk(const QVariant&)));
            connect(mControlRequest, SIGNAL(requestError(int,const QString&)),
                this, SLOT(handleControlRequestError(int,const QString&)));
            
            mControlRequest->setEmbedded(false);
        }
    }
    else
    {
        retVal = true;
    }

    return retVal;
}

void IrServiceClient::handleControlRequestOk(const QVariant &aRetValue)
{
    mControlReqOngoing = false;    
    
    if (aRetValue.isValid()
         && aRetValue.canConvert(QVariant::Int))
    {
        if (IrServiceResult::Fail == aRetValue.toInt())
        {
            emit controlFailed();
        }
    }    
}

void IrServiceClient::handleControlRequestError(int aErrCode, const QString &aErrMessage)
{
    mControlReqOngoing = false;
    
    emit controlFailed();
    
    Q_UNUSED(aErrCode);
    Q_UNUSED(aErrMessage);    
}
/******************************************************************
*************************   Others   ******************************
*******************************************************************/
// slot connected to signals from IrAppInspector,
// used to monitor the startup / exit of IR
void IrServiceClient::handleIrRunningStatusChanged(IrAppInspector::IrRunningStatus aNewStatus)
{
    switch (aNewStatus)
    {
        case IrAppInspector::StartingUp :
            doSendRefreshRequest();
            break;
            
        case IrAppInspector::Exiting :
        {
            clearMonitorServiceRequest();
            QString stationName;
            if (loadStationName(stationName))
            {
                notifyMetaDataUpdated(QString(""));
                notifyStationNameUpdated(stationName);        
                notifyStationLogoUpdated(loadStationLogoFlag());
                notifyIrStateChanged(IrAppState::NoRunStopped);
            }
            else
            {
                notifyIrStateChanged(IrAppState::NoRunInit);
            }            
            break;
        }
            
        default:
            break;
    }
}

void IrServiceClient::clearMonitorServiceRequest()
{
    delete mMonitorRequest;
    mMonitorRequest = NULL;

    delete mRefreshRequest;
    mRefreshRequest = NULL;
}

// used to process service data sent from IR app.
void IrServiceClient::processNotificationData(const IrServiceData &aServiceData)
{
    QVariant data = aServiceData.mData;
    if (!data.isValid())
    {
        return;
    }
    switch (aServiceData.mType)
    {
        case IrServiceNotification::StationName:
            if (data.canConvert(QVariant::String))
            {
                notifyStationNameUpdated(data.toString());
            }
            break;

        case IrServiceNotification::MetaData:
            if (data.canConvert(QVariant::String))
            {
                notifyMetaDataUpdated(data.toString());
            }
            break;

        case IrServiceNotification::StationLogo:
            if (data.canConvert(QVariant::Bool))
            {
                notifyStationLogoUpdated(data.toBool());
            }            
            break;

        case IrServiceNotification::IrState:
            if (data.canConvert(QVariant::Int))
            {
                notifyIrStateChanged((IrAppState::Type)data.toInt());
            }
            break;

        default:
            break;
    }

}

// prepare the request send to IR app, including control request, refresh request, monitor request.
void IrServiceClient::prepareRequestInfo(XQAiwRequest *aRequest, IrAppVisibilty aVisibility)
{
    if (NULL == aRequest)
    {
        return;
    } 
        
    XQRequestInfo reqInfo;
    switch (aVisibility)
    {
        case ToForeground:
            reqInfo.setForeground(true);
            break;
        case ToBackground:
            reqInfo.setBackground(true);
            break;
        case DoNotChange:
            break;
        default:
            break;
    }

    aRequest->setInfo(reqInfo);
}

void IrServiceClient::notifyStationNameUpdated(const QString &aStationName)
{
    mStationName = aStationName;
    emit stationNameUpdated(mStationName);
}

void IrServiceClient::notifyStationLogoUpdated(bool aIsLogoAvailable)
{
    mStationLogoAvailable = aIsLogoAvailable;
    emit stationLogoUpdated(mStationLogoAvailable);    
}

void IrServiceClient::notifyMetaDataUpdated(const QString &aMetaData)
{
    mMetaData = aMetaData;
    emit metaDataUpdated(mMetaData);    
}

void IrServiceClient::notifyIrStateChanged(IrAppState::Type aNewState)
{
    mIrState = aNewState;
    emit irStateChanged(mIrState);    
}       
        
bool IrServiceClient::isStationPlayed()
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    QString stationName = settings.value(KIrSettingStationName,KIrDefaultStationName).toString();
    return stationName != KIrDefaultStationName;
}

bool IrServiceClient::loadStationLogoFlag()
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    return settings.value(KIrSettingStationLogoAvailable,false).toBool();
}

bool IrServiceClient::loadStationName(QString &aStationName)
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    aStationName = settings.value(KIrSettingStationName,KIrDefaultStationName).toString();
    if(aStationName != KIrDefaultStationName)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Q_IMPLEMENT_USER_METATYPE(IrServiceData)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(IrServiceDataList)

