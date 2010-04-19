/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header for MIRAudioRoutingObserver. Interface for observing 
*                audio routing events
*
*/


#ifndef MIRAUDIOROUTINGOBSERVER_H
#define MIRAUDIOROUTINGOBSERVER_H

#include "vraudioutils.hrh"

/**
 *  Interface for observing audio routing events.
 *
 *  Implementer will be notified when audio routing changes
 *  between IHF and headset.
 *
 */
class MIRAudioRoutingObserver
    {
    
public:  // Methods

	/**
	 * Notifies of a requested audio routing change. Changes requested
	 * by other applications do not generate a notification.
	 *
	 * @param aOutputSource Current audio output source
	 */
    virtual void AudioRoutingChangedL(
        TVROutputDestination aOutputDestination) = 0;

    };

#endif      // MIRAUDIOROUTINGOBSERVER_H
