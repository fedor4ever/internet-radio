/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*        ECOM proxy table for this plugin
*
*/


// System includes
#include <e32std.h>
#include <implementationproxy.h>

#include "irgeneralsettingsview.h"
#include "irdebug.h"
#include  "irgeneralsettings.hrh"

 

// Constants
const TImplementationProxy KGenSettingGSPluginImplementationTable[] = 
	{
	
	IMPLEMENTATION_PROXY_ENTRY( IRGENERAL_SETTINGS_PLUGIN_DLL_UID,	CIRGeneralSettingsView::NewL )
	};


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	IRLOG_DEBUG( "In ImplementationGroupProxy - Entering");
	aTableCount = sizeof(KGenSettingGSPluginImplementationTable) 
        / sizeof(TImplementationProxy);
	IRLOG_DEBUG( "In ImplementationGroupProxy - Exiting");
	return KGenSettingGSPluginImplementationTable; 
	}

