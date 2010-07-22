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

#include "irqsystemeventhandler.h"
#include "irqsystemeventhandler_p.h"
#include "irdiskspaceobserver.h"
#include "iralarmobserver.h"
#include "irpropertyobserver.h"
#include "irqlogger.h"

#ifdef USER_DEFINED_DISKSPACE 
#include <QFile>
#include <QTextStream>
#include <QStringList> 
#endif

#define DEFAULT_DISKSPACE_LOW_LEVEL   (3*1024*1024)

IRQSystemEventHandlerPrivate::IRQSystemEventHandlerPrivate(IRQSystemEventHandler *aSystemEventHandler) : q_ptr(aSystemEventHandler),
                              mAlarmOn(false),mDefaultLevel(DEFAULT_DISKSPACE_LOW_LEVEL),mAlarmObserver(NULL), mDiskSpaceObserver(NULL),
                              mPropertyObserver(NULL),mErrorCode(0)
{ 
}

IRQSystemEventHandlerPrivate::~IRQSystemEventHandlerPrivate()
{
    cancel();
    delete mDiskSpaceObserver;
    delete mAlarmObserver;   
    delete mPropertyObserver;
}

bool IRQSystemEventHandlerPrivate::init()
{ 
#ifdef USER_DEFINED_DISKSPACE
    getDiskSpaceCriticalLevel(mDefaultLevel);
#endif  
    
    TRAPD(err, initializeL());      
    if( KErrNone == err )
    {
        return true;
    }
    else
    {
        mAlarmObserver = NULL;
        mDiskSpaceObserver = NULL;
        mPropertyObserver = NULL;        
    }
    
    
    return false;
}

qint64 IRQSystemEventHandlerPrivate::diskCriticalLevel() const
{
    if(NULL != mDiskSpaceObserver)
    {
        return (qint64)(mDiskSpaceObserver->CriticalLevel());
    }
    else
    {
        return -1;
    }
}


void IRQSystemEventHandlerPrivate::initializeL()
{
    mAlarmObserver = CIRAlarmObserver::NewLC(this);    
    mDiskSpaceObserver = CIRDiskSpaceObserver::NewLC(this);
    mPropertyObserver = CIRPropertyObserver::NewL(this);   
    CleanupStack::Pop(mDiskSpaceObserver);
    CleanupStack::Pop(mAlarmObserver);    
}

void IRQSystemEventHandlerPrivate::cancel()
{
    if( mAlarmObserver )
    {
        mAlarmObserver->Cancel();
    }
    
    if( mDiskSpaceObserver )
    {
        mDiskSpaceObserver->Cancel();
    }
}

void IRQSystemEventHandlerPrivate::start()
{
    if( mAlarmObserver )
    {
        mAlarmObserver->Start();        
    }
    
    if( mDiskSpaceObserver )
    {
        mDiskSpaceObserver->Start((TInt64)mDefaultLevel);            
    }       
}


bool IRQSystemEventHandlerPrivate::isBelowCriticalLevel(const qint64 aCriticalLevel)
{
    if( 0 == aCriticalLevel )
    {
        return mDiskSpaceObserver->IsBelowCriticalLevel((TInt64)mDefaultLevel );
    }
    
    return mDiskSpaceObserver->IsBelowCriticalLevel((TInt64)aCriticalLevel );
}

bool IRQSystemEventHandlerPrivate::isCallActive() const
{
    return mPropertyObserver->IsCallActive();
}

int IRQSystemEventHandlerPrivate::getErrorCode() const
{
    return mErrorCode;
}

#ifdef USER_DEFINED_DISKSPACE
void IRQSystemEventHandlerPrivate::getDiskSpaceCriticalLevel(qint64 & aLevel)
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

void IRQSystemEventHandlerPrivate::alarmStarted()
{
    if( !mAlarmOn )
    {
        emit q_ptr->alarmStarted();
        mAlarmOn = true;
    }
}

void IRQSystemEventHandlerPrivate::alarmStopped()
{
    if( mAlarmOn )
    {
        emit q_ptr->alarmStopped();
        mAlarmOn = false;
    }
}

void IRQSystemEventHandlerPrivate::notifyLowDiskSpace(qint64 aCriticalLevel)
{
    emit q_ptr->diskSpaceLowNotification(aCriticalLevel);
}

void IRQSystemEventHandlerPrivate::callIsActivated()
{
    LOG_METHOD;
    emit q_ptr->callActivated();    
}

void IRQSystemEventHandlerPrivate::callIsDeactivated()
{
    LOG_METHOD;
    emit q_ptr->callDeactivated();    
}

void IRQSystemEventHandlerPrivate::errorCallback(int aError)
{
    mErrorCode = aError;
}

void IRQSystemEventHandlerPrivate::headsetIsConnected()
{
    LOG_METHOD;    
    emit q_ptr->headsetConnected();
}

void IRQSystemEventHandlerPrivate::headsetIsDisconnected()
{
    LOG_METHOD;
    emit q_ptr->headsetDisconnected();
}

 




