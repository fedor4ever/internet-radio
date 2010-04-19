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
* Description:  Qt wrapper class declaration for CIRReportGenerator
*
*/


#ifndef IRQSTATISTICSREPORTER_H_
#define IRQSTATISTICSREPORTER_H_

#include <QObject>
#include <QString>

#include "irqevent.h"
#include "irqenums.h"

class CIRReportGenerator;
class IRQNetworkController;
class IRQIsdsClient;
class IRQReportSender;

/*!
 * This class provides usage report related interface.
 *
 * IRQReportGenerator follows singleton pattern
 *
 */
class IRQStatisticsReporter : public QObject
{
    Q_OBJECT

public:

    /*!
     *  Get the instance of IRQStatisticsReporter
     *  @return IRQStatisticsReporter*
     */
    IMPORT_C static  IRQStatisticsReporter* openInstance(IRQIsdsClient *aIsdsClient);

    /*!
     *  Close the instance of IRQStatisticsReporter
     */
    IMPORT_C void closeInstance();

private:
    /**
     *  Creates IRQStatisticsReporter instance
     */
    static IRQStatisticsReporter* createInstanceL(IRQIsdsClient *aIsdsClient);
    
    /*!
     * Constructor.
     */
    IRQStatisticsReporter();
    
    void constructL(IRQIsdsClient *aIsdsClient);
    
    /*!
     * Destructor.
     */    
    ~IRQStatisticsReporter();
   
public:    

    /*!
     * The following methods are for Nms Event Usage Logging 
     *
     * <session starttime="R" duration="R" terminatedBy="R" connectedFrom="R" id="R">
     *    <channelid>1234</channelid>
     *    <serverResult url="R">Connected</serverResult>
      *    <connection type="R">
      *      <operator currentNetwork="R" homeOperator=""/>
      *    </connection>
      * </session>
     */  
    
    /*!
     * Function: sessionStarted
     * starts a new session
     * @return bool  return ture if session is started successfully, otherwise, return false.
     */  
    IMPORT_C bool sessionStarted(const int aChannelId, const IRQConnectedFrom aConnectedFrom);

    /*!
     * Function: markSessionStart
     * marks the start of a session
     */  
    IMPORT_C void markSessionStart();

    /*!
     * Function: suspendSession
     * function handle session suspend
     */
    IMPORT_C void suspendSession();

    /*!
     * Function: restartSession
     * function handle session restart
     */
    IMPORT_C void restartSession();
                
    /*!
     * Function: sessionEnd
     * ends the session
     */    
    IMPORT_C void sessionEnded(const IRQTerminatedType aTerminatedBy);

    /*!
     * Function: logServerResult
     * logs server connection result
     * @param aUrl URL to which connection is attempted
     * @param aServerResult response from server
     */    
    IMPORT_C void logServerResult(const QString& aUrl, const IRQError aServerResult);  

    /*!
     * The following methods are for Nms Event Usage Logging 
     *
     * <nmsLog>
     *    <nmsEvent timeStamp="R" channelId="" type="R">
     *      <operator currentNetwork="R" homeOperator=""/>
     *    </nmsEvent>
     * </nmsLog>    
     */  
    
    /*!
     * Function: logNmsEvent
     * logs nmsEvent to Xml file
     * @param aNmsType Nms type  
     * @param aChannelId Nms channel id        
     */  
    IMPORT_C void logNmsEvent(const IRQNmsType aNmsType, const int aChannelId);
    
    /*!
     * Function: logSongRecogEvent
     * logs songRecog event to Xml file 
     */  
    IMPORT_C void logSongRecogEvent();    
    
public slots:

    /*!
     * Function: updateCurrentNetwork
     * updates the current network
     */  
    IMPORT_C void updateCurrentNetwork(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC);
    
    /*!
     * Function: updateHomeOperator
     * updates the home network
     */  
    IMPORT_C void updateHomeOperator(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC);
   
private slots:

    /*!
     * Function: reportSent
     * should be called after report is sent to isds server
     */
    void reportSent();
       
private:

    /*!
     * Function: updateTerminatedBy
     * updates the termination status
     * @param aTerminatedBy terminated by information
     */      
    void updateTerminatedBy(const IRQTerminatedType aTerminatedBy);

    /*!
     * Function: updateConnectedFrom
     * updateds connected information 
     * @param aConnectedFrom connected from information
     */  
    void updateConnectedFrom(const IRQConnectedFrom aConnectedFrom);
        
    /*!
     * Function: updateConnectionType
     * updates the connection type
     */  
    void updateConnectionType();
    
    /*!
     * Function: updateNmsType
     * updates the nokia music shop access type
     */  
    void updateNmsType(const IRQNmsType aNmsType);
    
    /*!
     * Function: doLogNmsEventL
     * logs nmsEvents to Xml file
     * @param aNmsType Nms type  
     * @param aChannelId Nms channel id        
     */  
    void doLogNmsEventL(const IRQNmsType aNmsType, const int aChannelId);    
    
private:

    CIRReportGenerator *iBody;
    
    IRQReportSender *iSender;
    
    /*!
     * Used for retrieving Connection Type
     */    
    IRQNetworkController *iNetworkController;
    
    int iCurrentNetwork;
    
    int iHomeOperator;   
    
    /*!
     * Number of objects currently referring to the singleton object
     */
    int iSingletonInstances;
};

#endif /* IRQSTATISTICSREPORTER_H_ */
