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

#include <stringloader.h> 
#include "irdebug.h"
#include "mirlistboxarray.h"




// ---------------------------------------------------------------------------
// void MIRListBoxArray::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void MIRListBoxArray::ConstructL()
    {
	IRLOG_DEBUG( "MIRListBoxArray::ConstructL - Entering" );
	iFlag = EFalse;
	IRLOG_DEBUG( "MIRListBoxArray::ConstructL - Exiting" );
   	}
// -----------------------------------------------------------------------------
// MIRListBoxArray::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MIRListBoxArray* MIRListBoxArray::NewL()
    {
 	IRLOG_DEBUG( "MIRListBoxArray::NewL" );
    MIRListBoxArray* self = MIRListBoxArray::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "MIRListBoxArray::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// MIRListBoxArray* MIRListBoxArray::NewLC()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
MIRListBoxArray* MIRListBoxArray::NewLC()
    {
    IRLOG_DEBUG( "MIRListBoxArray::NewLC - Entering" );
    MIRListBoxArray* self = new( ELeave ) MIRListBoxArray;
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "MIRListBoxArray::NewL - Exiting." );
    return self;
    }


// ---------------------------------------------------------------------------
// MIRListBoxArray::MIRListBoxArray()
// Destructor
// ---------------------------------------------------------------------------
//
/*MIRListBoxArray::~MIRListBoxArray()
	{
	}
*/
// ---------------------------------------------------------------------------
// MIRListBoxArray::MIRListBoxArray()
// Destructor
// ---------------------------------------------------------------------------
//
MIRListBoxArray::MIRListBoxArray()
	{
	}
    
// ----------------------------------------------------------
// TInt  MIRListBoxArray::MdcaCount() 
// returns the number of items the list box has. 
// ----------------------------------------------------------	
 TInt MIRListBoxArray::MdcaCount() const
    {
    IRLOG_DEBUG( "MIRListBoxArray::MdcaCount - Entering." );
	TInt count = iListArray->Count() ;
    IRLOG_DEBUG( "MIRListBoxArray::MdcaCount - Exiting." );
	return count;
    }  
// ----------------------------------------------------------
// TPtrC MIRListBoxArray::MdcaPoint(TInt aIndex) const
// Returns the text of the specified item. 
// ----------------------------------------------------------	
TPtrC MIRListBoxArray::MdcaPoint(TInt aIndex) const 
    {
    IRLOG_DEBUG( "MIRListBoxArray::MdcaPoint - Entering." );
    IRLOG_DEBUG( "MIRListBoxArray::MdcaPoint - Exiting." );
	return iListArray->MdcaPoint (aIndex);
    }
 // ----------------------------------------------------------
// void MIRListBoxArray::SetListArray() 
// sets the Listarray 
// ----------------------------------------------------------	
void MIRListBoxArray::SetListArray(CDesCArray* aListArray)
	{
    IRLOG_DEBUG( "MIRListBoxArray::SetListArray - Entering." );
    iListArray = aListArray;
    IRLOG_DEBUG( "MIRListBoxArray::SetListArray - Exiting." );
	}

// ----------------------------------------------------------
// void MIRListBoxArray::SetItemAtZero() 
// sets the item at the zeroth index. 
// ----------------------------------------------------------	
void MIRListBoxArray::SetItemAtZero(TBool aAdFlag,HBufC& aString,TBool aMainView)
	{
    IRLOG_DEBUG( "MIRListBoxArray::SetItemAtZero - Entering." );
	iLastPlayedString = &aString;
	iLastPlayed = aAdFlag;
	if(iLastPlayed && !aMainView)
	TRAP_IGNORE(iListArray->InsertL(0,*iLastPlayedString));
    IRLOG_DEBUG( "MIRListBoxArray::SetItemAtZero - Exiting." );
	}
// ----------------------------------------------------------
// TInt MIRListBoxArray::SetFlag() 
// Returns the flag when the count in the filtered model is incremented. 
// ----------------------------------------------------------	
void MIRListBoxArray::SetFlag(TBool aFlag)  
    {
    IRLOG_DEBUG( "MIRListBoxArray::SetFlag - Entering." );
	iFlag = aFlag;
    IRLOG_DEBUG( "MIRListBoxArray::SetFlag - Exiting." );
    }
  
// ----------------------------------------------------------
// void MIRListBoxArray::SetIndexFlag() 
// Returns the index flag. 
// ----------------------------------------------------------	
void MIRListBoxArray::SetIndexFlag(TBool aFlag)  
    {
    IRLOG_DEBUG( "MIRListBoxArray::SetFlag - Entering." );
	iIndexFlag = aFlag;
    IRLOG_DEBUG( "MIRListBoxArray::SetFlag - Exiting." );
    }

// ----------------------------------------------------------
// TBool MIRListBoxArray::GetIndexFlag() 
// Returns the index flag 
// ----------------------------------------------------------	
TBool MIRListBoxArray::GetIndexFlag()  
    {
    IRLOG_DEBUG( "MIRListBoxArray::SetFlag - Entering." );
    IRLOG_DEBUG( "MIRListBoxArray::SetFlag - Exiting." );
	return iIndexFlag ;
    }

 /*// ----------------------------------------------------------
// void MIRListBoxArray::SetListArray() 
// sets the item at the zeroth index. 
// ----------------------------------------------------------	
MDesCArray* MIRListBoxArray::GetListArray()
	{
    IRLOG_DEBUG( "MIRListBoxArray::GetListArray - Entering." );
    IRLOG_DEBUG( "MIRListBoxArray::GetListArray - Exiting." );
    return iListArray;
	}
// ----------------------------------------------------------
// TInt MIRListBoxArray::GetActualCount() 
// Returns the text of the specified item. 
// ----------------------------------------------------------	
TInt MIRListBoxArray::GetActualCount()  
    {
    IRLOG_DEBUG( "MIRListBoxArray::GetActualCount - Entering." );
    IRLOG_DEBUG( "MIRListBoxArray::GetActualCount - Exiting." );
 	return iListArray->Count();
   }
// ----------------------------------------------------------
// TInt MIRListBoxArray::GetActualCount() 
// Returns the text of the specified item. 
// ----------------------------------------------------------	
void MIRListBoxArray::SetActualCount(TInt aCount)  
    {
    IRLOG_DEBUG( "MIRListBoxArray::SetActualCount - Entering." );
	iCount = aCount;
    IRLOG_DEBUG( "MIRListBoxArray::SetActualCount - Exiting." );
    }*/
