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
#include "irfilteredmodel.h"
#include <stringloader.h> 
#include "irdebug.h"


// ----------------------------------------------------------
// TInt  CIRFilteredModel::NumberOfItems() 
// returns the number of items the list box has. 
// ----------------------------------------------------------	
TInt CIRFilteredModel::NumberOfItems() const
    {
    IRLOG_DEBUG( "CIRFilteredModel::NumberOfItems - Entering." );
	TBool flag = EFalse;
	TInt count = CAknFilteredTextListBoxModel::NumberOfItems();

	TInt arrayCount = 0;
	if(iListBoxArray)
	arrayCount =  MatchableTextArray()->MdcaCount();
	

	TInt actualIndex = 0;
	if(CAknFilteredTextListBoxModel::Filter ()) 
		{
		TInt filteredCount = CAknFilteredTextListBoxModel::Filter ()->FilteredNumberOfItems ();
		if(filteredCount>0)
		actualIndex = CAknFilteredTextListBoxModel::Filter ()->FilteredItemIndex(0);
		// if the actual index of first filtered element is greater than zero and lastplayed
		// is present (or) if the lastplayed is present and the number of filtered elements is zero
		// increment the count and the flag to true.
		if(( actualIndex != 0 && iAdFlag)||(filteredCount == 0 && iAdFlag ))
			{
			count++;
			flag = ETrue;
			iListBoxArray->SetFlag(flag);
			}
		}
	if(count==arrayCount && !flag)
		{
		if(iListBoxArray)
			{
			iListBoxArray->SetFlag(EFalse);
			iListBoxArray->SetIndexFlag(EFalse);
			}
		}
    IRLOG_DEBUG( "CIRFilteredModel::NumberOfItems - Exiting." );
	return count;
    }
// ----------------------------------------------------------
// void CIRFilteredModel::SetItemAtZero() 
// sets the item at the zeroth index. 
// ----------------------------------------------------------	
void CIRFilteredModel::SetItemAtZero(TBool aAdFlag,HBufC& aAdvString,TBool aMainView)
	{
    IRLOG_DEBUG( "CIRFilteredModel::SetItemAtZero - Entering." );
	iLastPlayedString = &aAdvString;
	iAdFlag = aAdFlag;
	iListBoxArray->SetItemAtZero(iAdFlag,*iLastPlayedString,aMainView);
    IRLOG_DEBUG( "CIRFilteredModel::SetItemAtZero - Exiting." );
	}
// ----------------------------------------------------------
// void CIRFilteredModel::SetItemAtZero() 
// sets the item at the zeroth index. 
// ----------------------------------------------------------	
void CIRFilteredModel::SetAdvFlag(TBool aAdFlag,HBufC& aAdvString)
	{
    IRLOG_DEBUG( "CIRFilteredModel::SetItemAtZero - Entering." );
	iLastPlayedString = &aAdvString;
	iAdFlag = aAdFlag;
    IRLOG_DEBUG( "CIRFilteredModel::SetItemAtZero - Exiting." );
	}
// ----------------------------------------------------------
// TPtrC CIRFilteredModel::ItemText(TInt aItemIndex) const
// Returns the text of the specified item. 
// ----------------------------------------------------------	
TPtrC CIRFilteredModel::ItemText(TInt aItemIndex) const
    {
    IRLOG_DEBUG( "CIRFilteredModel::ItemText - Entering." );
	// if lastplayed is not present in the filtered items/elements
	if(iListBoxArray->iFlag )
		{
		if(aItemIndex == 0)
			{
			TPtr ptr(iLastPlayedString->Des());
			IRLOG_DEBUG( "CIRFilteredModel::ItemText - Exiting." );
			return ptr;
			}
		else
			{
			TInt actualIndex = 0;
		//	TInt filteredCount 
			if(CAknFilteredTextListBoxModel::Filter ()) 
				{
				actualIndex = CAknFilteredTextListBoxModel::Filter ()->FilteredItemIndex(aItemIndex-1);//aItemIndex-1
				TPtrC16 advString = CAknFilteredTextListBoxModel::MatchableTextArray()->MdcaPoint(actualIndex);
				TPtrC16 lastplayedstring = CAknFilteredTextListBoxModel::MatchableTextArray()->MdcaPoint(0);
				if(lastplayedstring ==advString)
					{
					iListBoxArray->SetIndexFlag(ETrue);
					}
				if(iListBoxArray->GetIndexFlag())
					{
					actualIndex = CAknFilteredTextListBoxModel::Filter ()->FilteredItemIndex(aItemIndex);						
					}
				else
					{
					actualIndex = CAknFilteredTextListBoxModel::Filter ()->FilteredItemIndex(aItemIndex-1);						
					}
			//	filteredCount = CAknFilteredTextListBoxModel::Filter ()->FilteredNumberOfItems ()
				}
			IRLOG_DEBUG( "CIRFilteredModel::ItemText - Exiting." );
			return iListBoxArray->MdcaPoint(actualIndex);
			}
	    }
	//If the filtering is not done.
	else
		{
		TInt actualIndex;
		//TInt filteredCount 
		if(CAknFilteredTextListBoxModel::Filter ()) 
			{
			actualIndex = CAknFilteredTextListBoxModel::Filter ()->FilteredItemIndex(aItemIndex);
		//	filteredCount = CAknFilteredTextListBoxModel::Filter ()->FilteredNumberOfItems ()
			}
		else
		actualIndex = aItemIndex;
		IRLOG_DEBUG( "CIRFilteredModel::ItemText - Exiting." );
		return iListBoxArray->MdcaPoint(actualIndex);
		//	return CAknFilteredTextListBoxModel::ItemText (aItemIndex)
		}


    }
// ----------------------------------------------------------
// void CIRFilteredModel::SetItemAtZero() 
// sets the item at the zeroth index. 
// ----------------------------------------------------------	
void CIRFilteredModel::SetListArray( MIRListBoxArray* aListArray)
	{
    IRLOG_DEBUG( "CIRFilteredModel::SetListArray - Entering." );
    iListBoxArray = aListArray;	
    IRLOG_DEBUG( "CIRFilteredModel::SetListArray - Exiting." );
	}
// ----------------------------------------------------------
// TPtrC CIRFilteredModel::MdcaPoint() const
// Returns the text of the specified item. 
// ----------------------------------------------------------	
const MDesCArray* CIRFilteredModel::MatchableTextArray  ()  const
	{
	return (iListBoxArray);
	}
