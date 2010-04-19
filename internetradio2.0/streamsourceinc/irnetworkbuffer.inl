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
* Description:  Network buffer
*
*/



#ifndef IR_BUFFERCONTAINER_INL
#define IR_BUFFERCONTAINER_INL

// -----------------------------------------------------------------------------	
//  Des returns the Address of the chunk;
//  Owned by CIRNetworkBuffer Class
// -----------------------------------------------------------------------------
//
 inline TUint8* CIRNetworkBuffer::Des() const
	{
	return iDataAddress;	
	} 
	 
// -----------------------------------------------------------------------------	
//  Size returns the size of the chunk;
//  Owned by CIRNetworkBuffer Class
// -----------------------------------------------------------------------------
//
 inline TInt CIRNetworkBuffer::Size() const
	{
	return iSize;	
	}
	
#endif //IR_BUFFERCONTAINER_INL
