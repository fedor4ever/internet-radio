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
* Description:  Contains logic for watching certain disk's space
*
*/

#include <e32base.h>
#ifdef USER_DEFINED_DISKSPACE
#include <QFile>
#include <QTextStream>
#include <QStringList>
#endif
#include "irqdiskspacewatcher.h"
#include "irqdiskspaceobserver.h"

const TInt KIR_DEFAULT_CRITICAL_LEVEL = 3*1024*1024;    // 3MB

#ifdef USER_DEFINED_DISKSPACE
void getDiskSpaceCriticalLevel(qint64 & aLevel);
#endif

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C IRQDiskSpaceWatcher::IRQDiskSpaceWatcher()
       : iDiskSpaceObserver(NULL),
         iDefaultLevel(KIR_DEFAULT_CRITICAL_LEVEL) 
{
#ifdef USER_DEFINED_DISKSPACE
    getDiskSpaceCriticalLevel(iDefaultLevel);
#endif    
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C IRQDiskSpaceWatcher::~IRQDiskSpaceWatcher()
{
    delete iDiskSpaceObserver;
    iDiskSpaceObserver = NULL;
}


// ---------------------------------------------------------------------------
// IRQDiskSpaceWatcher::criticalLevel()
// return critical level which is been observed.
// ---------------------------------------------------------------------------
//
EXPORT_C qint64 IRQDiskSpaceWatcher::criticalLevel() const
{
    if(NULL != iDiskSpaceObserver)
    {
        return (qint64)(iDiskSpaceObserver->criticalLevel());
    }
    else
    {
        return -1;
    }
}


// ---------------------------------------------------------------------------
// IRQDiskSpaceWatcher::startMonitorDrive()
// start monitoring disk space change
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQDiskSpaceWatcher::startMonitorDrive(const qint64 aCriticalLevel)
{
    if(aCriticalLevel < 0)
    {
        return false;
    }
    
    if(NULL == iDiskSpaceObserver)
    {
        TRAPD(err, iDiskSpaceObserver = IRQDiskSpaceObserver::NewL(this))
        if(KErrNone != err)
        {
            return false;
        }
    }
    
    stopMonitorDrive();
    iDiskSpaceObserver->start((TInt64)aCriticalLevel);
    return true;
}

EXPORT_C bool IRQDiskSpaceWatcher::startMonitorDrive()
{
    return startMonitorDrive(iDefaultLevel);
}
// ---------------------------------------------------------------------------
// IRQDiskSpaceWatcher::stopMonitorDrive()
// stop monitoring disk space change
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQDiskSpaceWatcher::stopMonitorDrive()
{
    if(NULL != iDiskSpaceObserver)
    {
        iDiskSpaceObserver->Cancel();
    }
}

// ---------------------------------------------------------------------------
// IRQDiskSpaceWatcher::isBelowCriticalLevel()
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQDiskSpaceWatcher::isBelowCriticalLevel( const qint64 aCriticalLevel )
{
    if(NULL == iDiskSpaceObserver)
    {
        TRAPD(err, iDiskSpaceObserver = IRQDiskSpaceObserver::NewL(this))
        if(KErrNone != err)
        {
            return false;
        }
    }
    
    return iDiskSpaceObserver->isBelowCriticalLevel((TInt64)aCriticalLevel );
}                               

EXPORT_C bool IRQDiskSpaceWatcher::isBelowCriticalLevel()
{
    return isBelowCriticalLevel(iDefaultLevel);
}

// ---------------------------------------------------------------------------
// From MIRQDiskSpaceObserver.
// ---------------------------------------------------------------------------
//
void IRQDiskSpaceWatcher::notifyLowDiskSpace(const qint64 aCriticalLevel)
{
    emit diskSpaceLow(aCriticalLevel);
}

#ifdef USER_DEFINED_DISKSPACE
void getDiskSpaceCriticalLevel(qint64 & aLevel)
{
    QFile file("C:\\data\\QTIRConfigure.txt");
    if(file.open(QIODevice::ReadOnly)) 
    {
        QTextStream stream( &file );
        QString line;
        QStringList parameter;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            parameter = line.split("=");
            if (parameter.count() == 2)
            {
                if (parameter.first() == "diskSpaceCriticalLevel")
                {
                    QString level = parameter.last();
                    aLevel = level.toLongLong();
                    break;
                }
            }
        }
    }
    file.close();
}
#endif
