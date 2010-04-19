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


#ifndef IRCTRLCOMMAND_H
#define IRCTRLCOMMAND_H

#include <e32cmn.h>

#include "ircontrolparams.h"

/**
* Class is an abstract class which provides interfaces to pass commands
* MCtrlCommand can pass a command and value to the inherited class
* FetchVolume is meant for fetching the volume
* DoAudioFadeOut is to activate Fade out fade in effect
*/
class MIRCtrlCmdObserver
    {
public:
    /**
     * Function : MCrtlCmdCommand
     * Function is pure virtual function which used to pass commands
     * @param TIRControlCommmand represents the commands,which determines 
     * what action to be taken
     * @param aValue represents any integer value if any, like buffering
     * percentage
     */
    virtual void MCtrlCommand( TIRControlCommmand aCommand, TInt aValue ) = 0;

    /**
     * Function : FetchVolume
     * Function is pure virtual function which sents Request to get the
     * current volume index
     * @return volume index
     */
    virtual TInt FetchVolume() = 0;
    
    /**
     * Function : DoAudioFadeOut
     * Function is pure virtual function, it is used to sent request start 
     * audio Fade Out
     */
    virtual void DoAudioFadeOut() = 0;
    /**
     * Function : PlayerChanged()
     * Function is pure virtual function, It notifies the Observer WhenEver
	* Player Changed
     * audio Fade Out
     */
    virtual void PlayerChanged() = 0;
    };

/**
* Class is an abstract class which provides interfaces to fill the buffer
* Request to fill the buffer is made through this interface
*/
class MIRStreamFill
    {
public:
    /**
     * Function: FilltheBuffer
     * function is pure virtual implementation of and is call to fill
     * the buffer 
     * @param reference to the buffer to be filled with data
     */
    virtual void FilltheBuffer(TDes8& aInputBuffer) = 0;
    };
    
#endif //IRCTRLCOMMAND_H

