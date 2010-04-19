/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef M_IRDISKSPACEWATCHEROBSERVER_H
#define M_IRDISKSPACEWATCHEROBSERVER_H

/**
 *  Observer of low disk space 
 *
 *  Observer which is interested low disk space situations. 
 */
class MIRDiskSpaceWatcherObserver
    {

public:

    /**
     * NotifyLowDiskSpaceL Called when the observed disk's free disk space is
     * less than the critical level. Because this method does not tell which
     * drive is running out of space observer must check if it by asking from
     * <code>CVRDiskSpaceWatcher</code>
     */
    virtual void NotifyLowDiskSpaceL() = 0;

protected:

    };

#endif // M_IRDISKSPACEWATCHEROBSERVER_H
