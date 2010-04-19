/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#ifndef MIRLISTBOXARRAY_H
#define MIRLISTBOXARRAY_H

#include <bamdesca.h>



class MIRListBoxArray : public MDesCArray
    {
 public:
    /**
     * NewL()
     * Static constructor.
     */
    static MIRListBoxArray* NewL();

    /**
     * NewLC()
     * Static constructor.
     */
    static MIRListBoxArray* NewLC();
    /**
     * ConstructL()
     * 2nd phase constructor
     */
    void ConstructL();
	/*
	 *  MIRListBoxArray()
	 * default constructor
	 */
    MIRListBoxArray();
	/*
	* SetListArray()
	* sets the listarray
	*/
	void SetListArray(CDesCArray* aListArray);
	/*
	* SetItemAtZero()
	* sets the lastplayed flag and string
	*/
	void SetItemAtZero(TBool aAdFlag,HBufC& aString,TBool aMainView= EFalse);
	/*
	* MdcaCount()
	* returns the count of the listarray
	*/
	TInt MdcaCount() const ;
 	/*
	* MdcaPoint()
	* returns string at the particular index
	*/
	TPtrC MdcaPoint(TInt aIndex) const ;
	/*
	* SetFlag()
	* sets the flag whether count in the filtered model as incremented r not
	*/
	void SetFlag(TBool aFlag)  ;
	/*
	* SetIndexFlag()
	* sets the Index flag when the index is not giving the correct element
	*/
	void SetIndexFlag(TBool aFlag)  ;
	/*
	* GetIndexFlag()
	* Gets the index flag that is set.
	*/
	TBool GetIndexFlag();
 	/*
	* iListArray()
	* instance of CDesCArray
	*/
	CDesCArray* iListArray;
 	/*
	* iLastPlayed
	* saves the lastplayed status into this
	*/
	TBool iLastPlayed;
 	/*
	* iLastPlayedString
	* sets the lastplayed string into this
	*/
    HBufC* iLastPlayedString;
 	/*
	* iFlag
	* sets when the count 
	*/
 	TBool iFlag;
 	/*
	* iIndexFlag
	*  
	*/
	TBool iIndexFlag;
    };
#endif // MIRLISTBOXARRAY_H

