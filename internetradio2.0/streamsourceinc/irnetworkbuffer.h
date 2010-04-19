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


#ifndef IRNETWORKBUFFER_H
#define IRNETWORKBUFFER_H

#include <e32base.h>

//========================================class declaration CIRNetworkBuffer============================================

/**
 * This class is wrapper class which can hold an address of a memory chunk
 * and size (granularity) of memory chunk.
 * This is implemented so as to use with queue like TSglQue
 * iLink is public member and is of a member of type TSglQueLink
 * The memory address is of TUint8* type and address is of type TInt
 * you can assign the address and granularity only when you create an instance
 * and retrive address and size using Des() and GetSize() respectively
 * typical usage is as follows
 * Note: This class DOESN'T take any responsibility of removing memory chunk
 * associated to it when you delete the chunk, since it is not creating the memory
 * chunk it doesn't do deallocation it has to taken care from where you create
 * the memory chunk
 *
 * Since it may require changes we are not using irbuffercontainer.cpp present in IRMediaEngine
 * however the logic is borrowed from IRMediaEngine
 * @code
 *
 * TSglQue<CIRNetworkBuffer> Myque;
 * TInt f_off = _FOFF(CIRNetworkBuffer,iLink);
 * Myque.SetOffset(f_off);
 * TUint8* buffer;
 * TInt sizeofbuffer;
 * CIRNetworkBuffer* bufferholder;
 * bufferholder = CIRNetworkBuffer::NewL(buffer,sizeofbuffer);
 * Myque.AddLast(*bufferholder);
 * TUint8* ptr = bufferholder->Des();
 * TInt size = bufferholder->GetSize();
 * bufferholder = Myque.First();
 * Myque.Remove(*bufferholder);
 * delete bufferholder; //deleting bufferholder doesn't deletes memory allocated
 *			            //to buffer is not getting deleted we have delete it explicitly
 * 
 * delete buffer;
 *
 * @endcode
 *   
 */

NONSHARABLE_CLASS( CIRNetworkBuffer ) : public CBase
	{
											//member functions
public :
	
	/**
	 * Two Phase NewL
	 * returns an instance CIRNetworkBuffer
	 * Owned by CIRNetworkBuffer
	 * @param aAddress Address of data chunk
	 * @param aSize Size of data chunk
	 * @return instance CIRNetworkBuffer
	 */	
	static CIRNetworkBuffer* NewL( TUint8* aAddress,TInt aSize );
		
	/**
	 * Two Phase NewLC
	 * Creates an instance CIRNetworkBuffer
	 * Owned by CIRNetworkBuffer
	 * @param aAddress Address of data chunk
	 * @param aSize Size of data chunk
	 * @return instance CIRNetworkBuffer
	 */
	static CIRNetworkBuffer* NewLC( TUint8* aAddress,TInt aSize );

	/**
	 * destructor function
	 * Owned by CIRNetworkBuffer
	 */ 
	~CIRNetworkBuffer();
	
 	/**	
	 *  Des returns the Address of the chunk;
	 *  Owned by CIRNetworkBuffer Class
	 *  @return address
	 */	
	 TUint8* Des() const;
	
		 
	/**	
	 *  Size returns the size of the chunk;
	 *  Owned by CIRNetworkBuffer Class
	 *  @return size of chunk
	 */
	 TInt Size() const;	

private:

	/**
	 * Function : CIRNetworkBuffer
	 * default constructor
	 */	
	CIRNetworkBuffer();
				
	/**
	 * Two Phase ConstructL
	 * Owned by CIRNetworkBuffer
	 * @param aAddress Address of data chunk
	 * @param aSize Size of data chunk
	 */
 	void ConstructL( TUint8* aAddress,TInt aSize );

public:
	/**
	 * Queue Link owned by CIRNetworkBuffer
	 */
	TSglQueLink iLink; 

private:
		
	/**
	 * Queued buffer pointer owned by CIRNetworkBuffer
	 */
	 TUint8* iDataAddress;
		
	/**
	 * Size of the chunk
	 */
	 TInt iSize;		
	};

#include "irnetworkbuffer.inl"
	
#endif //IRNETWORKBUFFER_H


