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
*     Accessory detection component for Internet Radio application
*
*/
/*
* ============================================================================
*  Name     : IRHeadsetEventObserver.h
*  Part of  : 
*  Origin   : Internet Radio, Nokia
*  Created  : 
*  Description:
*     Accessory detection component for Internet Radio application
*  Version  :
*  
*  ---------------------------------------------------------------------------
*  
/** @file IRHeadsetEventObserver.h */

#ifndef MIRHEADSETEVENTOBSERVER_H
#define MIRHEADSETEVENTOBSERVER_H


// CLASS DECLARATION
/**
*  
*  Observer for the Headset button state. Implementer will be notified each time the headset button
*  state has changed (button pressed).
*/
class MIRHeadsetEventObserver 
    {

    public: // New functions

		/**
		* This callback will notify of the Headset to be connected
		*/        
        virtual void HeadsetConnectedCallbackL() = 0;

		/**
		* This callback will notify of the Headset to be disconnected
		*/
		virtual void HeadsetDisconnectedCallbackL() = 0;
    };

#endif      // MIRHEADSETEVENTOBSERVER_H
            
// End of File
