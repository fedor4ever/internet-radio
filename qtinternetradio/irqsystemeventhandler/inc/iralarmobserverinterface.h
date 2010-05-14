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
#ifndef IRALARMOBSERVERINTERFACE_H
#define IRALARMOBSERVERINTERFACE_H

class MIRAlarmObserverInterface
{
public:
    virtual void alarmStarted()= 0;    
    virtual void alarmStopped() = 0;
};

#endif // IRALARMOBSERVERINTERFACE_H
