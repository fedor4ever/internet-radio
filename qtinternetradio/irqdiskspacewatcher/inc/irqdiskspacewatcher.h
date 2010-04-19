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
* Description:  Disk space watcher, information about low disk space.
*
*/


#ifndef IRQDISKSPACEWATCHER_H_
#define IRQDISKSPACEWATCHER_H_

#include <QObject>
#include <QtGlobal>

#include "irqdiskspaceobserverinterface.h"  // MIRQDiskSpaceObserver

class IRQDiskSpaceObserver;

class IRQDiskSpaceWatcher : public QObject, 
                            public MIRQDiskSpaceObserver
{
    Q_OBJECT
    
public:
    /*!
     * Constructor.
     */
    IMPORT_C IRQDiskSpaceWatcher();
    
    /*!
     * Destructor.
     */    
    IMPORT_C ~IRQDiskSpaceWatcher();

public:   
    /*!
     *  start monitoring disk space change
     *  @param aCriticalLevel critical level which is been observed.
     *                        can NOT less than 0
     */
    IMPORT_C bool startMonitorDrive(const qint64 aCriticalLevel);
                                    
    IMPORT_C bool startMonitorDrive();
                                    
    /*!
     *  stop monitoring disk space change
     */    
    IMPORT_C void stopMonitorDrive();

    /**
     * @return critical level which is been observed.
     *         if -1 is returned, indicates watcher can NOT work
     */
    IMPORT_C qint64 criticalLevel() const;
    
                                           
    /**
     * Returns whether or disk defined by parameter 
     * contains less than the critical level free disk space.
     *
     * @param aCriticalLevel critical level which is been observed.
     * @return true if there is less free disk space than the critical 
     * level, otherwise false.
     */
    IMPORT_C bool isBelowCriticalLevel(const qint64 aCriticalLevel );
    IMPORT_C bool isBelowCriticalLevel();    
public:      
    /*!
     * From MIRQDiskSpaceObserver.
     */  
    void notifyLowDiskSpace(const qint64 aCriticalLevel);

signals:
    void diskSpaceLow(qint64 aCriticalLevel); 

private:
    IRQDiskSpaceObserver*  iDiskSpaceObserver;
    qint64  iDefaultLevel;
};

#endif // IRQDISKSPACEWATCHER_H_
