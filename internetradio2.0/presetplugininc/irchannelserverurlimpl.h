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



#ifndef IRCHANNELSERVERURLIMPL_H
#define IRCHANNELSERVERURLIMPL_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
 * This class provides the basic structure to hold the preset's url data
 *information that come from the isds server.
 *It also provides the basic getter and setter functions to 
 *manage the preset's url information.
 *has been provided 
 *   
 */
class CIRChannelServerUrlImpl:public CBase
	{
public:
	
	
	/**CIRChannelServerUrlImpl::NewL()
	*Static function
	*standard two phased constructor
	*@param void
	*@return *CIRChannelServerUrlImpl
	**/
	static  CIRChannelServerUrlImpl* NewL();
	
	
	/**CIRChannelServerUrlImpl::NewLC()
	*Static function
	*standard two phased constructor
	*@param void
	*@return *CIRChannelServerUrlImpl
	**/
	static CIRChannelServerUrlImpl* NewLC();
	
	
	/**CIRChannelServerUrlImpl::SetServerName()
	*sets server name 
	*@param TDesC
	*@return void
	**/
	void SetServerName(const TDesC& aServerName);
	
	
	/**CIRChannelServerUrlImpl::SetServerUrl()
	*sets server url 
	*@param TDesC
	*@return void
	**/
	void SetServerUrl(const TDesC& aServerUrl);
	
	
	/**CIRChannelServerUrlImpl::SetBitRate()
	*sets server bitrate 
	*@param TInt
	*@return void
	**/
	void SetBitRate(TInt aBitrate);
	
	
	/**CIRChannelServerUrlImpl::GetServerName()
	*gets server name 
	*@param void
	*@return TDesC
	**/
	TDesC& GetServerName();
	
	
	/**CIRChannelServerUrlImpl::GetServerUrl()
	*gets server url 
	*@param void
	*@return TDesC
	**/
	TDesC& GetServerUrl();
	
	
	/**CIRChannelServerUrlImpl::GetBitRate()
	*gets server bitrate
	*@param void
	*@return TInt
	**/
	TInt GetBitRate();
	
	
	/**CIRChannelServerUrlImpl::~CIRChannelServerUrl()
	*standard C++ destructor 
	*@param NA
	*@return NA
	**/	
	~CIRChannelServerUrlImpl();
	
	
	/**CIRChannelServerUrlImpl::=()
	*standard C++ copy constructor(deep copy) 
	*@param CIRChannelServerUrl
	*@return NA
	**/	
	CIRChannelServerUrlImpl& operator=(const CIRChannelServerUrlImpl& churl);
		
	
	/**CIRChannelServerUrlImpl::ExternalizeL()
	*function to externalize url data
	*@param CIRChannelServerUrl
	*@return NA
	**/
	void ExternalizeL(RWriteStream& aWriteStream);	
	
	/**CIRChannelServerUrlImpl::InternalizeL()
	*function to internalize url data
	*@param CIRChannelServerUrlImpl
	*@return NA
	**/
	void InternalizeL(RReadStream& aReadStream);
	
protected:
	
	
	/**CIRChannelServerUrlImpl::ConstructL()
	*standard second phase construction
	*@param void
	*@return void
	**/	
	void ConstructL();
	
public:
	//channel server name
	HBufC *iServerName;
	//channel server url
	HBufC *iURL;
	//bitrate supported by channel server
	TInt iBitrate;
	};
	
#endif//IRCHANNELSERVERURLIMPL_H