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


#ifndef IRSTREAMSOURCEERRORS_H
#define IRSTREAMSOURCEERRORS_H

#include <e32def.h>

/** 
 * Base error code for Stream source component
 */
 
const TInt KIRStreamSourceErrorBase =  { 0xA1234B };

/** 
 * Error to indicate time out has occurred
 */

const TInt KIRStreamSourceTimeOut =  { KIRStreamSourceErrorBase	+ 0x00000000 };

/** 
 * Error code for DNS lookup error
 */

const TInt KIRStreamSourceLookupError =  { KIRStreamSourceErrorBase + 0x00000001 };

/** 
 * Error code for connecting to channel server
 */

const TInt KIRStreamSourceConnectionError =  { KIRStreamSourceErrorBase	+ 0x00000002 };

/** 
 * Error code for socket write error
 */

const TInt KIRStreamSourceWriteError =  { KIRStreamSourceErrorBase + 0x00000003 };

/** 
 * Error code for socket read error 
 */
const TInt KIRStreamSourceReadError =  { KIRStreamSourceErrorBase + 0x00000004 };

/** 
 * Error code for no response from channel server
 */

const TInt KIRStreamSourceNoResponse =  { KIRStreamSourceErrorBase + 0x00000005 };

/** 
 * Error code for invalid url 
 */
const TInt KIRStreamSourceInvalidUrl =  { KIRStreamSourceErrorBase + 0x00000006 };
	
/**
 * Error code for disconnected connection
 */	
const TInt KIRStreamSourceDisconnected = { KIRStreamSourceErrorBase + 0x00000007 };

/**
 * Error code for application layer protocol errors. 
 */
const TInt KIRStreamSourceApplicationProtocolError =  { KIRStreamSourceErrorBase + 0x00000008 };

#endif // IRSTREAMSOURCEERRORS_H

