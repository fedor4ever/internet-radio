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


#include "irbuffercontainer.h"
#include "irdebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Two Phase NewL
// returns an instance CIRBufferContainer
// Owned by CIRBufferContainer
// ---------------------------------------------------------------------------
//
CIRBufferContainer* CIRBufferContainer::NewL
	(TUint8* aAddress,TInt aGranularity)
	{
	IRLOG_DEBUG( "CIRBufferContainer::NewL" );
	CIRBufferContainer *self = 
		CIRBufferContainer::NewLC(aAddress,aGranularity);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRBufferContainer::NewL - Exiting." );
	return self;	
	}

// ---------------------------------------------------------------------------
// Two Phase NewLC
// Creates an instance CIRBufferContainer
// Owned by CIRBufferContainer
// ---------------------------------------------------------------------------
//
CIRBufferContainer* CIRBufferContainer::NewLC
	(TUint8* aAddress,TInt aGranularity)
	{
	IRLOG_DEBUG( "CIRBufferContainer::NewLC" );
	CIRBufferContainer *self = new(ELeave) CIRBufferContainer;
	CleanupStack::PushL(self);
	self->ConstructL(aAddress,aGranularity);
	IRLOG_DEBUG( "CIRBufferContainer::NewLC - Exiting." );
	return self;
	}
	
// ---------------------------------------------------------------------------
// destructor function
// Owned by CIRBufferContainer
// ---------------------------------------------------------------------------
//
CIRBufferContainer::~CIRBufferContainer ()
	{
	IRLOG_DEBUG( "CIRBufferContainer::~CIRBufferContainer" );
	//No implementation
	}	
	
// ---------------------------------------------------------------------------
// Function : CIRBufferContainer
// default constructor
// ---------------------------------------------------------------------------
//
 CIRBufferContainer::CIRBufferContainer()
	{
	IRLOG_DEBUG( "CIRBufferContainer::CIRBufferContainer" );
	//No implementation	
	}

// ---------------------------------------------------------------------------
// Two Phase ConstructL
// Owned by CIRBufferContainer
// ---------------------------------------------------------------------------
//
void CIRBufferContainer::ConstructL(TUint8* aAddress,TInt aGranularity)
	{
	IRLOG_DEBUG( "CIRBufferContainer::ConstructL" );
	//address of memory chunk
	iDataAddress = aAddress;
	//granularity of each chunk
	iSize = aGranularity;
	IRLOG_DEBUG( "CIRBufferContainer::ConstructL - Exiting." );
	}


