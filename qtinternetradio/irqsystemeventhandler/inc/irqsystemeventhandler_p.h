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

#ifndef IRQSYSTEMEVENTHANDLER_P_H
#define IRQSYSTEMEVENTHANDLER_P_H
 
#include "irdiskspaceobserverinterface.h"
#include "iralarmobserverinterface.h" 

class IRQSystemEventHandler; 
class CIRAlarmObserver;
class CIRDiskSpaceObserver;

class IRQSystemEventHandlerPrivate : public MIRAlarmObserverInterface, public MIRDiskSpaceObserverInterface
{
public:
    
    IRQSystemEventHandlerPrivate(IRQSystemEventHandler *aSystemEventHandler);    
    ~IRQSystemEventHandlerPrivate();
    
    bool   init();
    bool   isBelowCriticalLevel(const qint64 aCriticalLevel = 0);
    qint64 diskCriticalLevel() const;
   
    void cancel();    
    void start();     
    
private:    
    //to initialize all the symbian components here
    void initializeL();
    //from MIRAlarmObserverInterface
    void alarmStarted();
    void alarmStopped();   
    //from MIRDisSpaceObserver
    void notifyLowDiskSpace(qint64 aCriticalLevel);
    
#ifdef USER_DEFINED_DISKSPACE
    void getDiskSpaceCriticalLevel(qint64 & aLevel);  
#endif
    
private:
    
    IRQSystemEventHandler * const q_ptr; 
    bool                          mAlarmOn;
    qint64                        mDefaultLevel;   
    
    CIRAlarmObserver*             mAlarmObserver;
    CIRDiskSpaceObserver*         mDiskSpaceObserver;     
};

#endif  //IRQSYSTEMEVENTHANDLER_P_H
