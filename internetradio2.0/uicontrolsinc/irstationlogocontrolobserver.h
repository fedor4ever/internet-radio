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
* Description:  Observer interface for the logo control.
*
*/


#ifndef M_IRSTATIONLOGOCONTROLOBSERVER_H
#define M_IRSTATIONLOGOCONTROLOBSERVER_H

/**
 * Observer interface for the image converter.
 */
class MIRLogoControlObserver
    {

public:

    /**
     * Call back method that is called just before displaying the logo.
     */
    virtual void LogoReadyForDisplayL() = 0;

    /**
     * Call back method that is called when the logo conversion fails.
     */
    virtual void LogoConversionFailedL() = 0;
    
    };

#endif // M_IRSTATIONLOGOCONTROLOBSERVER_H
