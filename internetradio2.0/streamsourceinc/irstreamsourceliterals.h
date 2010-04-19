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


#ifndef IRSTREAMSOURCELITERALS_H
#define IRSTREAMSOURCELITERALS_H

#include <e32def.h>

const TInt KByteSpecifier = 1024;



_LIT8( KIcyRequest, "GET %S HTTP/1.0\r\nHost: %S\r\nUser-Agent: %S\r\nAccept: */*\r\nIcy-MetaData:1\r\nConnection: close\r\n\r\n" );


// Constants for specifying to the server to send meta info or not
// Icy protocol specific http headers

_LIT(KTcpProtocol,"tcp");
_LIT8(KIcyName,"icy-name:");
_LIT8(KIcyGenre,"icy-genre:");
_LIT8(KIcyBitrate, "icy-br:");
_LIT8(KIcyMetaint,"icy-metaint:");
// RFC 2616 specifies this format
_LIT8(KContentType,"Content-Type:");
// This format is obtained from the channel server
_LIT8(KContentTypeInvalid,"content-type:");
_LIT8(KCarReturn,"\xD\xA");
_LIT8(KHeaderEnd,"\xD\xA\xD\xA");

// Supported content type is only of mime type audio
_LIT8(KValidContentType,"audio/*");

// Response validation
_LIT8(KOkResponse,"200 OK");

#endif //IRSTREAMSOURCELITERALS_H

