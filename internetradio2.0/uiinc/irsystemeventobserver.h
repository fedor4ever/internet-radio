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
*     ?description_line
*     ?description_line
*     ?description_line
*
*/
/*
* ============================================================================
*  Name     : IRSystemEventObserver.h
*  Part of  : 
*  Origin   : 
*  Created  : 
*  Description:
*     ?description_line
*     ?description_line
*     ?description_line
*  Version  :
*  
*  ---------------------------------------------------------------------------
*  Version history:
* ============================================================================
*/
/** @file IRSystemEventObserver.h */

#ifndef IRSYSTEMEVENTOBSERVER_H
#define IRSYSTEMEVENTOBSERVER_H


//  INCLUDES

// DATA TYPES

/** Type of system event */
enum TIRSystemEventType
	{
	EIRHeadsetConnected,		 ///< Headset was connected
	EIRHeadsetDisconnected,		 ///< Headset was disconnected
	EIRNetworkCoverageUp,		 ///< Network coverage detected
	EIRNetworkCoverageDown,		 ///< Network coverage lost
	EIRCallActivated,			 ///< Call activated or ringing
	EIRCallDeactivated,			 ///< Call disconnected
	EIRLowDiskSpace,			 ///< Low disk space
	EIRAudioRoutingHeadset,		 ///< Audio routed through headset
	EIRAudioRoutingSpeaker,		 ///< Audio routed through speaker (IHF)
	EIRAudioResourcesAvailable,  ///< Audio resources have become available
	EIRAudioAutoResumeForbidden  ///< Audio auto resuming is forbidden
	};

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Observer for any system event state change.
*/
class MIRSystemEventObserver
    {
    public:  // Methods

		/**
		* Called when system event occurs.
		* @param aEventType Type of the event
		*/
        virtual void HandleSystemEventL(TIRSystemEventType aEventType) = 0;

    };

#endif      // IRSYSTEMEVENTOBSERVER_H   
            
// End of File
