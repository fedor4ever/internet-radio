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
* Description:  Observer interface for the image converter
*
*/


#ifndef M_MIRIMAGECONVERTEROBSERVER_H
#define M_MIRIMAGECONVERTEROBSERVER_H

#include <e32def.h>

/**
 * Observer interface for the image converter.
 */
class MIRImageConverterObserver
    {

public:

    /**
     * Supported image conversion events.
     */
    enum TIRImageConversionEvent
        {
        
        /**
         * Frame has been converted successfully; this means that the converter will continue
         * processing the data and sending out call backs as more frames are completed.
         */
        EIRFrameConversionCompleted,
        
        /**
         * Image has been completely converter; no further call backs will occur.
         */
        EIRImageConversionCompleted
        
        };
    
public:

    /**
     * Call back method that is called when an image converion event occurs.
     * 
     * @param   aEvent          The event that occurred.
     * @param   aId             Identifier for the conversion.
     * @param   aError          One of the standard system error codes.
     */
    virtual void HandleImageConversionEventL( MIRImageConverterObserver::
    										TIRImageConversionEvent aEvent,
    										TInt aId, TInt aError ) = 0;
    
    };

#endif // M_MIRIMAGECONVERTEROBSERVER_H
