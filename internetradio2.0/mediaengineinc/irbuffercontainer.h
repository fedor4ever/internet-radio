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


#ifndef IRBUFFERCONTAINER_H
#define IRBUFFERCONTAINER_H

#include <e32base.h>

//========================================class declaration CIRBufferContainer============================================

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
 * @code
 *
 * TSglQue<CIRBufferContainer> Myque;
 * TInt f_off = _FOFF(CIRBufferContainer,iLink);
 * Myque.SetOffset(f_off);
 * TUint8* buffer;
 * TInt sizeofbuffer;
 * CIRBufferContainer* bufferholder;
 * bufferholder = CIRBufferContainer::NewL(buffer,sizeofbuffer);
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

NONSHARABLE_CLASS(CIRBufferContainer) : public CBase
	{
											//member functions
public :
	
	/**
	 * Two Phase NewL
	 * returns an instance CIRBufferContainer
	 * Owned by CIRBufferContainer
	 * @param Address of data chunk
	 * @param Granularity of data chunk
	 * @return instance CIRBufferContainer
	 */	
	static CIRBufferContainer* NewL(TUint8* aAddress,TInt aGranularity);
		
	/**
	 * Two Phase NewLC
	 * Creates an instance CIRBufferContainer
	 * Owned by CIRBufferContainer
	 * @param Address of data chunk
	 * @param Granularity of data chunk
	 * @return instance CIRBufferContainer
	 */
	static CIRBufferContainer* NewLC(TUint8* aAddress,TInt aGranularity);

	/**
	 * destructor function
	 * Owned by CIRBufferContainer
	 */ 
	~CIRBufferContainer();
	
 	/**	
	 *  Des returns the Address of the chunk;
	 *  Owned by CIRBufferContainer Class
	 *  @return address
	 */	
	 TUint8* Des() const;
	
		 
	/**	
	 *  Size returns the size of the chunk;
	 *  Owned by CIRBufferContainer Class
	 *  @return size of chunk
	 */
	 TInt Size() const;	
												
private:

	/**
	 * Function : CIRBufferContainer
	 * default constructor
	 */	
	CIRBufferContainer();
				
	/**
	 * Two Phase ConstructL
	 * Owned by CIRBufferContainer
	 * @param Address of data chunk
	 * @param Granularity of data chunk
	 */
 	void ConstructL(TUint8* aAddress,TInt aGranularity);
 		 	
												
									//function which return address and granularity

												//data members

public:
	//Que Link owned by CIRBufferContainer
	TSglQueLink iLink; 

private:
		
	//Queued buffer pointer owned by CIRBufferContainer
	TUint8* iDataAddress;
		
	//Size of the chunk
	TInt iSize;		
	};

#include "irbuffercontainer.inl"
	
#endif//IRBUFFERCONTAINER_H


