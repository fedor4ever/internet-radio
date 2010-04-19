/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer interface for delayed actions
*
*/


#ifndef M_MIRDELAYEDACTIONOBSERVER_H
#define M_MIRDELAYEDACTIONOBSERVER_H

/**
 * Observer interface for delayed actions.
 *
 * Delayed actions are used to process user actions that were done prior to 
 * network connection being up as soon as the connection is established.
 */
class MIRDelayedActionObserver
    {

public:    
    
    /**
     * Called when the network connection is up and ready to be used.
     */
    virtual void ExecuteDelayedActionL() = 0;

    };

#endif // M_MIRDELAYEDACTIONOBSERVER_H
