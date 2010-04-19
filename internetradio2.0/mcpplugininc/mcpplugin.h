/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Music Content Publisher Plugin interface definition
*
*/


#ifndef C_MCPPLUGIN_H
#define C_MCPPLUGIN_H

#include <e32base.h>

// CLASS DECLARATION
/**
 * The interface class for all MCP plugins
 * Plugins are responsible for updating data and actions to the Music Widget
 * and Matrix Menu.
 *
 * The implementator of this interface should implement Deactivate function that 
 * will be invoked when the plugin is preempted by another plugin.
 *
 * IMPORTANT NOTE: 
 * It is recommended that the plugin does not publish data while not active.
 * After calling Deactivate on the plugin it should chnage its state to
 * not active.
 *
 */
class CMCPPlugin : public CBase
    {
public:

    /** 
     * Triggers Deactivation Process.
     * Note: This should return immediatelly.
     *       Just a change of state variable is recommended.
     */
    virtual void Deactivate() = 0;
    };

#endif // C_MCPPLUGIN_H