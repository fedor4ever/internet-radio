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
* Description:  Disk space observer, information about low disk space.
*
*/


#ifndef IRDISKSPACEOBSERVERINTERFACE_H_
#define IRDISKSPACEOBSERVERINTERFACE_H_

#include <QtGlobal>

/**
 *  Observer of low disk space 
 *
 *  Observer which is interested low disk space situations. 
 */
class MIRDiskSpaceObserverInterface
{
public:

    /**
     * notifyLowDiskSpace Called when the observed disk's free disk space is
     * less than the critical level.
     */
    virtual void notifyLowDiskSpace(qint64 aCriticalLevel) = 0;
};

#endif // IRDiskSpaceObserverInterface_H_
