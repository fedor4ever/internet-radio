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


#include "irdebug.h"
#include "irnetworkbuffer.h"

// ---------------------------------------------------------------------------
// Two Phase NewL
// returns an instance CIRNetworkBuffer
// Owned by CIRNetworkBuffer
// ---------------------------------------------------------------------------
//
CIRNetworkBuffer* CIRNetworkBuffer::NewL(TUint8* aAddress,TInt aSize)
	{
	IRLOG_DEBUG( "CIRNetworkBuffer::NewL" );
	CIRNetworkBuffer* self = CIRNetworkBuffer::NewLC( aAddress, aSize );
	CleanupStack::Pop( self );
	IRLOG_DEBUG( "CIRNetworkBuffer::NewL - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// Two Phase NewLC
// Creates an instance CIRNetworkBuffer
// Owned by CIRNetworkBuffer
// ---------------------------------------------------------------------------
//
CIRNetworkBuffer* CIRNetworkBuffer::NewLC( TUint8* aAddress, TInt aSize )
	{
	IRLOG_DEBUG( "CIRNetworkBuffer::NewLC" );
	CIRNetworkBuffer* self = new (ELeave) CIRNetworkBuffer;
	CleanupStack::PushL( self );
	self->ConstructL( aAddress, aSize );
	IRLOG_DEBUG( "CIRNetworkBuffer::NewLC - Exiting." );
	return self;
	}
	
// ---------------------------------------------------------------------------
// Function : CIRNetworkBuffer
// default constructor
// ---------------------------------------------------------------------------
//
 CIRNetworkBuffer::CIRNetworkBuffer()
	{
	}

// ---------------------------------------------------------------------------
// Two Phase ConstructL
// Owned by CIRNetworkBuffer
// ---------------------------------------------------------------------------
//
void CIRNetworkBuffer::ConstructL( TUint8* aAddress,TInt aSize )
	{
	IRLOG_DEBUG3( "CIRNetworkBuffer::ConstructL - address=0x%x, size=%d", aAddress, aSize );
	//address of memory chunk
	iDataAddress = aAddress;
	//granularity of each chunk
	iSize = aSize;
	IRLOG_DEBUG( "CIRNetworkBuffer::ConstructL - Exiting." );
	return;
	}

// ---------------------------------------------------------------------------
// destructor function
// Owned by CIRNetworkBuffer
// ---------------------------------------------------------------------------
//
CIRNetworkBuffer::~CIRNetworkBuffer ()
	{
	IRLOG_DEBUG( "CIRNetworkBuffer::~CIRNetworkBuffer" );
	//No implementation
	}

