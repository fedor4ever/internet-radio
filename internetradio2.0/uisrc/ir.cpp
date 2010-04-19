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


#include <eikstart.h>

#include "irapplication.h"
#include "irdebug.h"

// ---------------------------------------------------------------------------
// Create an application, and return a pointer to the framework
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    IRLOG_DEBUG( "CApaApplication::NewApplication" );
    return new CIRApplication;
    }

// ---------------------------------------------------------------------------
// The application entry-point
// voilates PC lint error 714-- not refrenced.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    IRLOG_DEBUG( "CApaApplication::E32Main" );
    return EikStart::RunApplication( NewApplication );
    }


