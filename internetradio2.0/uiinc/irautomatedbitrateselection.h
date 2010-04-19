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


#ifndef IRAUTOMATEDBITRATESELECTION_H
#define IRAUTOMATEDBITRATESELECTION_H

#include <e32def.h>

//high quality selection default for WLAN network
const TInt KHighQuality = 2;

//high quality selection default for 3G network
const TInt KMediumQuality = 1;

//high quality selection default for GPRS network
const TInt KLowQuality = 0;

//limit for high bit rate greater than or equals 128kbps
const TInt KHighBitRateLimit = 128;

//limit for medium bit rate between 128kbps and 48kps
const TInt KMediumBitRateLimit = 48;

#endif //IRAUTOMATEDBITRATESELECTION_H

