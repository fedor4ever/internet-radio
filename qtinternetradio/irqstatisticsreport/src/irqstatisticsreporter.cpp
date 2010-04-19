/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Qt wrapper class implementation for CIRReportGenerator
*
*/

#include "irsessionlogger.h" // CIRReportGenerator

#include "irqstatisticsreporter.h"
#include "irqnetworkcontroller.h"
#include "irqreportsender.h"

//Network Indicator
_LIT(KGPRS,"gprs");
_LIT(KWCDMA,"wcdma");
_LIT(KWLAN,"wlan");
_LIT(KCDMA2000,"cdma2000");
_LIT(KEDGE,"edge");

//Music Shop Type
_LIT(KFind,"find");
_LIT(KLaunch,"launch");

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::openInstance()
// Static function to get a singleton instance of IRQStatisticsReporter
// ---------------------------------------------------------------------------
//
EXPORT_C IRQStatisticsReporter* IRQStatisticsReporter::openInstance(IRQIsdsClient *aIsdsClient)
{
    // Get singleton instance
    IRQStatisticsReporter* reportGenerator =
                           reinterpret_cast<IRQStatisticsReporter*>(Dll::Tls());

    if (NULL == reportGenerator)
    {
        TRAPD(error, reportGenerator = createInstanceL(aIsdsClient));
        if (KErrNone != error)
        {
            delete reportGenerator;
            reportGenerator = NULL;
            Dll::SetTls(NULL);
        }
    }
    else
    {
        reportGenerator->iSingletonInstances++;
    }

    return reportGenerator;
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::closeInstance()
// Close a singleton instance of IRQStatisticsReporter
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::IRQStatisticsReporter()
// Creates IRQStatisticsReporter instance
// ---------------------------------------------------------------------------
//
IRQStatisticsReporter* IRQStatisticsReporter::createInstanceL(IRQIsdsClient *aIsdsClient)
{
    IRQStatisticsReporter* reportGenerator = new (ELeave) IRQStatisticsReporter();
    reportGenerator->constructL(aIsdsClient);
    User::LeaveIfError(Dll::SetTls(reportGenerator));
    reportGenerator->iSingletonInstances = 1;

    return reportGenerator;
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::IRQStatisticsReporter()
// Constructor.
// ---------------------------------------------------------------------------
//
IRQStatisticsReporter::IRQStatisticsReporter():iBody(NULL),
                                               iSender(NULL),
                                               iNetworkController(NULL),
                                               iCurrentNetwork(0),
                                               iHomeOperator(0),
                                               iSingletonInstances(0)
{

}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::constructL()
// Two-Phase Constructor.
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::constructL(IRQIsdsClient *aIsdsClient)
{
    iBody = CIRReportGenerator::OpenL();

    iNetworkController = IRQNetworkController::openInstance();

    iSender = new IRQReportSender(aIsdsClient);

    if(iBody && iSender)
    {
        iBody->SetDbStatusObserser(iSender);
    }

    connect(iSender, SIGNAL(reportSent()), this, SLOT(reportSent()));
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::~IRQStatisticsReporter()
// Destructor.
// ---------------------------------------------------------------------------
//
IRQStatisticsReporter::~IRQStatisticsReporter()
{
    if(iBody)
    {
        iBody->SetDbStatusObserser(NULL);
        iBody->Close();
    }

    if(iNetworkController)
    {
        iNetworkController->closeInstance();
    }

    delete iSender;
    iSender = NULL;
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::sessionStarted()
// starts a new session
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQStatisticsReporter::sessionStarted(const int aChanneldId, const IRQConnectedFrom aConnectedFrom)
{
    if(iBody)
    {
    	updateConnectedFrom(aConnectedFrom);
        iBody->UpdateChannelID(aChanneldId);
        updateConnectionType();

        TRAPD(error,iBody->SessionStartedL());
        if(error)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::markSessionStart()
// marks the start of a session
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::markSessionStart()
{
    if(iBody)
    {
        iBody->MarkSessionStart();
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::suspendSession()
// function handle session suspend
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::suspendSession()
{
    if(iBody)
    {
        iBody->HandleStopEvents(EFalse);
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::restartSession()
// function handle session restart
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::restartSession()
{
    if(iBody)
    {
        iBody->HandleStopEvents(ETrue);
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::sessionEnded()
// ends the session
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::sessionEnded(const IRQTerminatedType aTerminatedBy)
{
    if(iBody)
    {
        updateTerminatedBy(aTerminatedBy);
        TRAP_IGNORE(iBody->SessionEndL(EFalse));
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::logServerResult()
// logs server connection result
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::logServerResult(const QString& aUrl, const IRQError aServerResult)
{
    if(iBody)
    {
        TPtrC url(reinterpret_cast<const TUint16*>(aUrl.utf16()));
        switch(aServerResult)
        {
            case EIRQErrorNone:
                iBody->LogServerResult(url,EIRConnected);
                break;
            case EIRQPlayerErrorServerFull:
                iBody->LogServerResult(url,EIRFull);
                break;
            case EIRQPlayerErrorTimeOut:
                iBody->LogServerResult(url,EIRConnTimeOut);
                break;
            case EIRQPlayerErrorConnectingFailed:
                iBody->LogServerResult(url,EIRConnFailed);
                break;
            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::logNmsEvents()
// logs nmsEvents to Xml file
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::logNmsEvent(const IRQNmsType aNmsType, const int aChanneldId)
{
    TRAP_IGNORE(doLogNmsEventL(aNmsType,aChanneldId));
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::doLogNmsEventsL()
// logs nmsEvents to Xml file
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::doLogNmsEventL(const IRQNmsType aNmsType, const int aChanneldId)
{
    if(iBody)
    {
    	updateNmsType(aNmsType);
        iBody->UpdateNmsChannelID(aChanneldId);
        iBody->NmsLogStartedL();
        iBody->WriteNmsLogtoXmlL();
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::logSongRecogEvent()
// logs songRecog event to Xml file 
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::logSongRecogEvent()
{
    if(iBody)
    {
        iBody->LogSongRecog();
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::updateCurrentNetwork()
// updates the current network
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::updateCurrentNetwork(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC)
{
    QString nwkCode = aCurrentNwkMCC + aCurrentNwkMNC;
    iCurrentNetwork = nwkCode.toInt();
    iBody->UpdateCurrentNetwork(iCurrentNetwork);
    iBody->UpdateCurrentBrowseNetwork(iCurrentNetwork);
    iBody->UpdateNmsCurrentNetwork(iCurrentNetwork);    
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::updateHomeOperator()
// updates the home network
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQStatisticsReporter::updateHomeOperator(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC)
{
    QString nwkCode = aHomeNetworkMCC + aHomeNetworkMNC;
    iHomeOperator = nwkCode.toInt();
    iBody->UpdateHomeOperator(iHomeOperator);
    iBody->UpdateNmsHomeOperator(iHomeOperator);    
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::reportSent()
// should be called after report is sent to isds server
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::reportSent()
{
    if(iBody)
    {
        iBody->ReportSent();
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::updateTerminatedBy()
// updates the termination status
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::updateTerminatedBy(const IRQTerminatedType aTerminatedBy)
{
    if(iBody)
    {
        switch(aTerminatedBy)
        {
            case EIRQUserTerminated:
                iBody->UpdateTerminatedBy(EUserTerminated);
                break;
            case EIRQNoConnectionToServer:
                iBody->UpdateTerminatedBy(ENoConnectionToServer);
                break;
            case EIRQNoConnectionToNetwork:
                iBody->UpdateTerminatedBy(ENoConnectionToNetwork);
                break;
            default:
                // do NOT cover all cases
                iBody->UpdateTerminatedBy(EUserTerminated);
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::updateConnectedFrom()
// updateds connected information
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::updateConnectedFrom(const IRQConnectedFrom aConnectedFrom)
{
    if(iBody)
    {
        switch(aConnectedFrom)
        {
            case EIRQIsds:
                iBody->UpdateConnectedFrom(EIRIsds);
                break;
            case EIRQPresetAdhoc:
                iBody->UpdateConnectedFrom(EIRPresetAdhoc);
                break;
            case EIRQPresetIsds:
                iBody->UpdateConnectedFrom(EIRPresetIsds);
                break;
            case EIRQHistoryAdhoc:
                iBody->UpdateConnectedFrom(EIRHistoryAdhoc);
                break;
            case EIRQHistoryIsds:
                iBody->UpdateConnectedFrom(EIRHistoryIsds);
                break;
            case EIRQAdhocManual:
                iBody->UpdateConnectedFrom(EIRAdhocManual);
                break;
            case EIRQAdhocExternal:
                iBody->UpdateConnectedFrom(EIRAdhocExternal);
                break;                                
            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::updateConnectionType()
// updates the connection type
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::updateConnectionType()
{
    if(iBody && iNetworkController)
    {
        IRQConnectionType connectionType = iNetworkController->identifyConnectionType();
        switch(connectionType)
        {
            case EIRQGprs:
                iBody->UpdateConnectionType(KGPRS);
                break;
            case EIRQWcdma:
                iBody->UpdateConnectionType(KWCDMA);
                break;
            case EIRQCdma2000:
                iBody->UpdateConnectionType(KCDMA2000);
                break;                
            case EIRQWiFi:
                iBody->UpdateConnectionType(KWLAN);
                break;
            case EIRQEdge:
                iBody->UpdateConnectionType(KEDGE);
                break;
            default:
                // do NOT cover all cases
                iBody->UpdateConnectionType(KNullDesC);
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// IRQStatisticsReporter::updateNmsType()
// updates the nokia music shop access type
// ---------------------------------------------------------------------------
//
void IRQStatisticsReporter::updateNmsType(const IRQNmsType aNmsType)
{
    if(iBody)
    {
        switch(aNmsType)
        {
            case EIRQFind:
                iBody->UpdateNmsType(KFind);
                break;
            case EIRQLaunch:
                iBody->UpdateNmsType(KLaunch);
                break;
            default:
                break;
        }
    }
}
