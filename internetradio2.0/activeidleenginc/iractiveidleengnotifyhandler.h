/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRACTIVEIDLEENGNOTIFYHANDLER_H
#define IRACTIVEIDLEENGNOTIFYHANDLER_H

#include "irpubsubkeys.h"

class CIRMetaData;

/**
* Observer interface for CIRActiveIdleEng.
* The observer is notified about the changes in relevant keys through this interface.
*/
NONSHARABLE_CLASS(MIRActiveIdleEngNotifyHandler)
    {

public:
        
    /**
    * Invoked when a new channel has been tuned into.
    */
    virtual void HandleChannelChangeL() = 0;
    
    /**
    * Invoked when a new channel has been tuned into.
    * @param    aVolume       The index of the volume level.
    */
    virtual void HandleVolumeChangeL( TInt aVolume ) = 0;
    
    /**
    * Invoked when a new channel has been tuned into.
    */
    virtual void HandlePlayStopL( TIRStopPlayState aState ) = 0;
    
   /**
    * Invoked when the application's running state has changed.
    * @param    aRunningState   The new running state.
    */
    virtual void HandleApplicationRunningStateChangeL( 
    			TIRPSApplicationRunningState aRunningState ) = 0;

    /**
     * Invoked when meta data is received.
     *
     * @param   aMetaData       The meta data that was received.
     */
    virtual void HandleMetaDataReceivedL( const CIRMetaData& aMetaData ) = 0;

     /**
     * Invoked when buffering starts or stops 
     *
     * @param   aValue  The state of buffering
     */
    virtual void HandleBufferingStateL(TIRBufferingState aState) = 0;

    };

#endif//IRACTIVEIDLEENGNOTIFYHANDLER_H
           
