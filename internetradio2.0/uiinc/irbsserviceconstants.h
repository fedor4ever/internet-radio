/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for Back Stepping Service Provider
 *
*/


#ifndef IR_BS_SERVICE_CONSTANTS_H
#define IR_BS_SERVICE_CONSTANTS_H

// BS Service implementation UID
const TInt KBSServiceImplUid( 0x2000F840);

// from LIW
_LIT8( KIRGenericParamServiceCmdIDStr, "cmd" );

// BSS Service and Interface ID
_LIT8( KIRBSServiceID, "Service.BackStepping" );
_LIT8( KIRBSInterface, "IBackStepping" );

// BSS commands
_LIT8( KIRBSCmdInitialize, "Initialize" );
_LIT8( KIRBSCmdForwardActivationEvent, "ForwardActivationEvent" );
_LIT8( KIRBSCmdHandleBackCommand, "HandleBackCommand" );

// BSS in param names
_LIT8( KIRBSInParamAppUid, "AppUid" );
_LIT8( KIRBSInParamState, "State" );
_LIT8( KIRBSInParamEnter, "Enter" );
_LIT8( KIRBSInParamCheckOnly, "CheckOnly" );

// BSS out param names
_LIT8( KIRBSOutParamStatusInfo, "StatusInfo" );

#endif // IR_BS_SERVICE_CONSTANTS_H
