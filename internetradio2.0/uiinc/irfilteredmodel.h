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

#ifndef __IRFILTEREDMODEL_H__
#define __IRFILTEREDMODEL_H__

#include <aknutils.h> 
#include <bamdesca.h>
#include "mirlistboxarray.h"



class CIRFilteredModel : public CAknFilteredTextListBoxModel
    {
 public:
    /**
    * Implementation of @c CTextListBoxModel::NumberOfItems()
    * @return The number of items the list box has.
    */
    TInt NumberOfItems() const;
    /**
    * Returns the text of the specified item.
    * @param aItemIndex Index to the item.
    * @return The text of item in given index.
    */
    TPtrC ItemText(TInt aItemIndex) const;
    /**
    * SetItemAtZero()
  	* sets the lastplayed flag and string
    */
	void SetItemAtZero(TBool aAdFlag,HBufC& aAdvString,TBool aMainView= EFalse);
    /**
    * SetAdvFlag()
  	* sets the advertisment flag and string
    */
	void SetAdvFlag(TBool aAdFlag,HBufC& aAdvString);
	/*
	* SetListArray()
	* sets the listarray
	*/
 	 void SetListArray( MIRListBoxArray* aListArray);
 	/*
	* iLastPlayedString
	* sets the lastplayed string into this
	*/
    HBufC* iLastPlayedString;
 	/*
	* iAdFlag
	* saves the lastplayed/advertisement status into this
	*/
   TBool iAdFlag;
    /**
    * iListBoxArray
    * Instance of MIRListBoxArray
    */
  	MIRListBoxArray* iListBoxArray;
    /**
    * MatchableTextArray ()
    * Implementation of  CTextListBoxModel::MatchableTextArray()
    * return Pointer to the text array.
    */
	const  MDesCArray* MatchableTextArray  () const ;
    };
#endif // __IRFILTEREDMODEL_H__
