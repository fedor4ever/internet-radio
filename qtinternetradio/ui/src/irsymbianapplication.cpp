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

#include <coemain.h>
#include <apgwgnam.h>

#include "irsymbianapplication.h"
#include "irsymbiandocument.h"

IRSymbianApplication * IRSymbianApplication::appInstance = NULL;

CApaApplication *newS60Application()
{
    return IRSymbianApplication::getInstance();
}

IRSymbianApplication * IRSymbianApplication::getInstance()
{
    if (NULL == appInstance)
    {
        appInstance = new IRSymbianApplication;
    }
    
    return appInstance;
}

bool IRSymbianApplication::getInstanceFlag() const
{
    return iSecondInstance;
}

IRSymbianApplication::IRSymbianApplication() : iSecondInstance(false)
{
    
}

CApaDocument * IRSymbianApplication::CreateDocumentL()
{
    return IRSymbianDocument::getInstance();
}

void IRSymbianApplication::PreDocConstructL()
{
    RWsSession& ws = iCoeEnv->WsSession();
    const TInt myWgId = iCoeEnv->RootWin().Identifier();
    TInt wgId = 0;
    TUid uid(AppDllUid());
    iSecondInstance = false;
    
    // Look for another instance of this app
    while (wgId >= 0)
    {
        if (wgId && wgId != myWgId)
        {
            iSecondInstance = true;
        }
        CApaWindowGroupName::FindByAppUid(uid, ws, wgId);
    }
    
    if (!iSecondInstance)
    {
        CAknApplication::PreDocConstructL(); 
    }
}

