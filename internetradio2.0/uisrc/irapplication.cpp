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


#include "irapplication.h"
#include "irdebug.h"
#include "irdocument.h"
#include "irdebug.h"

#include <eikenv.h>
#include <apgwgnam.h>
// UID for the application, this should correspond to the uid defined in the mmp file
// voilate PC Lint 569 -Loss of information (initialization)
// (32 bits to 31 bits)
static const TUid KUidIR = {0x2000B499};

// ---------------------------------------------------------------------------
// Function: CreateDocumentL()
// returns an instance of CIRDocument
// ---------------------------------------------------------------------------
//
CApaDocument* CIRApplication::CreateDocumentL()
    {  
    IRLOG_INFO( "CIRApplication::CreateDocumentL" );
    return CIRDocument::NewL(*this);
    }

// ---------------------------------------------------------------------------
// Function: AppDllUid()
// Return the UID for the IR application
// ---------------------------------------------------------------------------
// 
TUid CIRApplication::AppDllUid() const
    {
    IRLOG_INFO( "CIRApplication::AppDllUid" );
    return KUidIR;
    }
// ---------------------------------------------------------------------------
// Function: PreDocConstructL()
// Return the UID for the IR application
// ---------------------------------------------------------------------------
// 
void CIRApplication::PreDocConstructL()
	{
	// Check that this app is started as stand-alone
	RWsSession& ws = static_cast<CEikonEnv*>(iCoeEnv)->WsSession();
	const TInt myWgId = static_cast<CEikonEnv*>(iCoeEnv)->RootWin().Identifier();
	TInt wgId = 0;
	TUid uid(AppDllUid());
	// Look for another instance of this app
	while (wgId >= 0)
		{
		if (wgId && wgId != myWgId)
			{
			iSecondInstance =ETrue;
			}
		CApaWindowGroupName::FindByAppUid(uid, ws, wgId);
		}
	if(!iSecondInstance)
	CAknApplication::PreDocConstructL(); 
	}
// ---------------------------------------------------------------------------
// Function: GetInstanceFlag()
// Return the Boolean for second instance
// ---------------------------------------------------------------------------
// 
TBool CIRApplication::GetInstanceFlag()
	{
	IRLOG_INFO( "CIRApplication::GetInstanceFlag" );
	return iSecondInstance;
	}
