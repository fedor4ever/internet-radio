/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CIRViewStack class
*
*/


#include <eikenv.h>
#include "irviewstack.h"
#include "irdebug.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRViewStack* CIRViewStack::NewL()
    {
    IRLOG_INFO( "CIRViewStack::NewL - Entering" );
    CIRViewStack* self = new( ELeave ) CIRViewStack( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_INFO( "CIRViewStack::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// Desctructor.
// ---------------------------------------------------------------------------
//
CIRViewStack::~CIRViewStack()
    {
    IRLOG_INFO( "CIRViewStack::~CIRViewStack - Entering" );
    iViewStack.Close();
    IRLOG_INFO( "CIRViewStack::~CIRViewStack - Exiting" );
    }

// ---------------------------------------------------------------------------
// C++ defaul desctructor.
// ---------------------------------------------------------------------------
//
CIRViewStack::CIRViewStack( )
    {
    IRLOG_INFO( "CIRViewStack::CIRViewStack" );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CIRViewStack::ConstructL()
    {
    IRLOG_INFO( "CIRViewStack::ConstructL" );
    }

// ---------------------------------------------------------------------------
// CIRViewStack::Peek()
// ---------------------------------------------------------------------------
//
TUid CIRViewStack::Peek() const
    {
    IRLOG_INFO( "CIRViewStack::ConstructL" );
    return iViewStack.Count() ? iViewStack[iViewStack.Count() - 1] : KNullUid;
    }

// ---------------------------------------------------------------------------
// CIRViewStack::PushL()
// ---------------------------------------------------------------------------
//
void CIRViewStack::PushL( TUid aView )
    {
    IRLOG_INFO( "CIRViewStack::PushL - Entering" );
    if( !iViewStack.Count() || iViewStack[iViewStack.Count()-1] != aView )
        {
        iViewStack.AppendL( aView );
        }
    IRLOG_INFO( "CIRViewStack::PushL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRViewStack::Pop()
// ---------------------------------------------------------------------------
//
TInt CIRViewStack::Count() const
    {
    IRLOG_INFO( "CIRViewStack::Count" );
    return iViewStack.Count();
    }

// ---------------------------------------------------------------------------
// CIRViewStack::Pop()
// ---------------------------------------------------------------------------
//
TUid CIRViewStack::Pop()
    {
    IRLOG_INFO( "CIRViewStack::Pop - Entering" );
    TUid ret = KNullUid;
    if( iViewStack.Count() )
        {
        ret = iViewStack[iViewStack.Count()-1];
        iViewStack.Remove( iViewStack.Count()-1 );
        }
    IRLOG_INFO( "CIRViewStack::Pop - Exiting" );
    return ret;
    }

// ---------------------------------------------------------------------------
// CIRViewStack::PopTo()
// ---------------------------------------------------------------------------
//
TUid CIRViewStack::PopTo( TUid aId )
    {
    IRLOG_INFO( "CIRViewStack::PopTo - Entering" );
    TUid ret = KNullUid;
    while ( Peek() != aId && Count() > 0 )
        {
        ret = Pop();
        }
    IRLOG_INFO( "CIRViewStack::PopTo - Exiting" );
    return ret;
    }

