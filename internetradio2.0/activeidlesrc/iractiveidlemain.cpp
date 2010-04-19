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

#include "iractiveidle.h"

/** Provides a key-value pair table, that is used to identify the correct construction function for the requested interface. */
const TImplementationProxy KIRActiveIdleImplementationTable[] =
	{
    /*lint -save -e611 (Warning -- Suspicious cast)*/
    IMPLEMENTATION_PROXY_ENTRY( 0x2000B4A7, CIRActiveIdle::NewL )
    /*lint -restore*/
	};

const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount );

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------------------------
// Returns the implementations provided by this ECOM plugin DLL.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( KIRActiveIdleImplementationTable ) / 
							sizeof( TImplementationProxy );
	return KIRActiveIdleImplementationTable;
	}

// ================= MEMBER FUNCTIONS =======================
