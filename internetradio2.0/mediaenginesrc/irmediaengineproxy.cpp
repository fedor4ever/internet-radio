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


#include <implementationproxy.h>

#include "iraacplayer.h"
#include "irmp3player.h"

// Map the interface UIDs to implementation factory functions
// violates PC lint error :: Warning 611: Suspicious cast
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x200087E9,CIRMP3Player::NewL),	
	IMPLEMENTATION_PROXY_ENTRY(0x200087EA,CIRAACPlayer::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x200087EB,CIRAACPlayer::NewL),
	};

// Exported proxy for instantiation method resolution
// violates PC lint error 714-- not refrenced.  
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
	(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

