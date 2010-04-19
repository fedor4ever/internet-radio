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
* Description:  class declaration for monitoring network info change
*
*/


#ifndef IRQNWKINFOOBSERVER_H_
#define IRQNWKINFOOBSERVER_H_

#include <QObject>
#include <QString>

#include "irqnwkobserverinterface.h"  // MIRQNwkInfoUpdate

class IRQNwkInfoObserverImpl;

class IRQNwkInfoObserver : public QObject, 
                           public MIRQNwkInfoUpdate
{
    Q_OBJECT
    
public:
    /*!
     *  Get the instance of IRQNwkInfoObserver
     *  @return IRQNwkInfoObserver*
     */
    IMPORT_C static  IRQNwkInfoObserver* openInstance();

    /*!
     *  Close the instance of IRQNwkInfoObserver
     */
    IMPORT_C void closeInstance();

public:   
    /*!
     *  start monitoring network info change
     */
    IMPORT_C void startMonitorNwkInfo();

    /*!
     *  stop monitoring network info change
     */    
    IMPORT_C void stopMonitorNwkInfo();

signals:

    void currentNwkChanged(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC);
    
    void homeNwkChanged(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC);  
    
private:

    /*!
     * Constructor.
     */
    IRQNwkInfoObserver();
    
    /*!
     * Destructor.
     */    
    ~IRQNwkInfoObserver();
    
    /**
     *  Creates IRQNwkInfoObserver instance
     */
    static IRQNwkInfoObserver* createInstanceL();
    
    void constructL();    
    
public:      
    /*!
     * From MIRQNwkInfoUpdate.
     */  
    void updateCurrentNwkInfo(const QString &aCurrentNwkMCC, const QString &aCurrentNwkMNC);
    
    void updateHomeNwkInfo(const QString &aHomeNetworkMCC, const QString &aHomeNetworkMNC);  
    
private:

    IRQNwkInfoObserverImpl *iBody;
    
    int iSingletonInstances;    
};

#endif // IRQNWKINFOOBSERVER_H_
