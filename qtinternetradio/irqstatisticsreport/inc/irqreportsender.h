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
* Description:  class used for sending report to isds server
*
*/


#ifndef IRQREPORTSENDER_H
#define IRQREPORTSENDER_H

#include <QObject>

#include "irdbstatusobserver.h"

class IRQSettings;
class IRQIsdsClient;
class IRQNetworkController;

class IRQReportSender : public QObject,
                        public MIRLogDbStatusObserver
{
    Q_OBJECT

public:
    /*!
     * Constructor.
     */
    explicit IRQReportSender(IRQIsdsClient *aIsdsClient);

    /*!
     * Destructor.
     */
    ~IRQReportSender();
    
private:

    /*!
     * From MIRLogDbStatusObserver.
     */       
    void LogDbNeedFlush();
    
    /*!
     * Function: sendReport
     * send the statistics report to isds server.
     */    
    void sendReport();
    
signals:
    void reportSent();
    
private:
    /*!
     * Used for sending out report
     */     
    IRQIsdsClient *iIsdsClient;  
     
    /*!
     * Used for getting the private path of the log file
     */       
    IRQSettings *iSettingManager;
    
    /*!
     * Used for querying the connection status
     */    
    IRQNetworkController *iNetworkController;       
};

#endif // IRQREPORTSENDER_H
