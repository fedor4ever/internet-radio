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


#ifndef IRCONTROLPARAMS_H
#define IRCONTROLPARAMS_H

#include <e32def.h>

//This value defines amount of time between each step in audio fade in effect
const TInt KVolumeTime = 500000;  //defines Audio Fade In time

//This value defines amount of time between each step in audio fade out effect
const TInt KDownVolumeTime = 200000; //defines the Audio Fade Out Time

/**
 * Base for IR custom errors delivered via Command Channel
 */
const TInt KIRCtrlCmdErrorBase =  { 0xB1234B };

/**
 * IR specific error telling that creationg of player failed.
 */
const TInt KIRCtrlCmdPlayerNotCreated =  { KIRCtrlCmdErrorBase + 0x00000000 };

/**
 * IR specific general error sent by the player to report about its errors.
 */
const TInt KIRCtrlCmdGeneralPlayerError = { KIRCtrlCmdErrorBase + 0x00000001 };

/**
 * TIRControlCommmand is enum which defines various errors and condition of
 *  the player
 */
enum TIRControlCommmand
    {
    //indicates that buffer is start to fill
    EBufferFillStart = 0x0F, 
    // indicates that buffer is filled
    EBufferFillStop,
    //indicates that buffer is filling
    EBufferFilling,
    // audio fade out    
    EBufferFadeOut,
    //audio fade in 
    EBufferFadeIn,
    //player stopped playing due some external conditions 
    EStoppedPlaying,
    //ready to start audio fade in 
    EBufferFadeInReady, 
    //indicates that an error has occured
    EError, 
    //indicates that player changed
    EPlayerChanged,
   //makes stereo mode on
   	EStereo, 
   	//When Player Stars Playing
   	EPlayingState,
    };

#endif //IRCONTROLPARAMS_H

