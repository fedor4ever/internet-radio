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


#include <s32strm.h>

#include "irchannelserverurlimpl.h"
#include "irdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::NewL()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
CIRChannelServerUrlImpl* CIRChannelServerUrlImpl::NewL()
	{
	IRLOG_DEBUG( "CIRChannelServerUrlImpl::NewL" );
	CIRChannelServerUrlImpl* self=CIRChannelServerUrlImpl::NewLC();
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRChannelServerUrlImpl::NewL - Exiting." );
	return self;
	}
	

// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::NewLC()
// Static function
// standard two phased constructor
// ---------------------------------------------------------------------------
//
CIRChannelServerUrlImpl* CIRChannelServerUrlImpl::NewLC()
	{
	IRLOG_DEBUG( "CIRChannelServerUrlImpl::NewLC" );
	CIRChannelServerUrlImpl* self=new(ELeave)CIRChannelServerUrlImpl;
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::NewLC - Exiting." );
	return self;
	}
	

// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::SetServerName()
// sets server name 
// ---------------------------------------------------------------------------
//
void CIRChannelServerUrlImpl::SetServerName(const TDesC& aServerName)
	{
	iServerName=aServerName.Alloc();
	}


// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::SetServerUrl()
// sets server url 
// ---------------------------------------------------------------------------
//
void CIRChannelServerUrlImpl::SetServerUrl(const TDesC& aServerUrl)
	{
	iURL=aServerUrl.Alloc();
	}


// ---------------------------------------------------------------------------	
// CIRChannelServerUrlImpl::SetBitRate()
// sets server bitrate 
// ---------------------------------------------------------------------------
//
void CIRChannelServerUrlImpl::SetBitRate(TInt aBitrate)
	{
	iBitrate=aBitrate;
	}


// ---------------------------------------------------------------------------	
// CIRChannelServerUrlImpl::GetServerName()
// gets server name 
// ---------------------------------------------------------------------------
//
TDesC& CIRChannelServerUrlImpl::GetServerName()
	{
	return *iServerName;
	}

// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::GetServerUrl()
// gets server url 
// ---------------------------------------------------------------------------
//
TDesC& CIRChannelServerUrlImpl::GetServerUrl()
	{
	return *iURL;
	}
	
// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::GetBitRate()
// gets server bitrate
// ---------------------------------------------------------------------------
//
TInt CIRChannelServerUrlImpl::GetBitRate()
	{
	return iBitrate;
	}

// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::~CIRChannelServerUrlImpl()
// destructor 
// ---------------------------------------------------------------------------
//
CIRChannelServerUrlImpl::~CIRChannelServerUrlImpl()
	{
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::~CIRChannelServerUrlImpl" );
	delete iServerName;
	delete iURL;
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::~CIRChannelServerUrlImpl - Exiting." );
	}

	
// ---------------------------------------------------------------------------	
// CIRChannelServerUrlImpl::=()
// standard C++ copy constructor(deep copy) 
// ---------------------------------------------------------------------------
//
CIRChannelServerUrlImpl& CIRChannelServerUrlImpl::operator=(
	const CIRChannelServerUrlImpl& churl)
	{
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::operator=" );
	if(&churl == this)
		{
		return *this;
		}
	iServerName=churl.iServerName;
	iURL=churl.iURL;
	iBitrate=churl.iBitrate;
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::operator= - Exiting." );
	return *this;
	}
	

// ---------------------------------------------------------------------------	
// CIRChannelServerUrlImpl::ExternalizeL()
// function to externalize url data
// ---------------------------------------------------------------------------
//
void CIRChannelServerUrlImpl::ExternalizeL(RWriteStream& aWriteStream)
	{
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::ExternalizeL" );
	//algo
	//1.check if data is present for the particular member variable
	//2.if yes get the length of the data and externalize the length and then the data
	//3.if no data is present write the length as 0.
	TInt len;
	if(iServerName)
		{
		len=iServerName->Length();
		aWriteStream.WriteInt32L(len);
		aWriteStream << *iServerName;
		}
	else
		{
		len=0;
		aWriteStream.WriteInt32L(len);
		}
		
	if(iURL)
		{
		len=iURL->Length();
		aWriteStream.WriteInt32L(len);
		aWriteStream << *iURL;
		}
	else
		{
		len=0;
		aWriteStream.WriteInt32L(len);
		}
		
	aWriteStream.WriteInt32L(iBitrate);
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::ExternalizeL - Exiting." );
	}


// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::InternalizeL()
// function to internalize url data
// ---------------------------------------------------------------------------
//
void CIRChannelServerUrlImpl::InternalizeL(RReadStream& aReadStream)
	{
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::InternalizeL" );
	//algo
	//1.get the length of the data
	//2.read that many bytes
	//3.if read length is zero. skip..
	TInt len;
	len= aReadStream.ReadInt32L();
	if(len)
		{
		iServerName=HBufC::NewL(aReadStream,len);	
		}

	len=aReadStream.ReadInt32L();
	if(len)
		{
		iURL=HBufC::NewL(aReadStream,len);	
		}
	iBitrate=aReadStream.ReadInt32L();
	IRLOG_DEBUG(  "CIRChannelServerUrlImpl::InternalizeL - Exiting." );
	}
	

// ---------------------------------------------------------------------------
// CIRChannelServerUrlImpl::ConstructL()
// standard second phase construction
// ---------------------------------------------------------------------------
//
void CIRChannelServerUrlImpl::ConstructL()
	{
	//no implementation
	return;
	}
	
