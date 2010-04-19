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


#include <implementationproxy.h>

#include "irdebug.h"
#include "irrecognizer.h"

// CONSTANTS
//voilate PC Lint Warning 569: Loss of information (initialization) 
//(32 bits to 31 bits)
// Uid of the recogniser
//const TUid KUidIRRecognizer={0x2000B499}
// maximum amount of buffer space we will ever use
const TInt KMaxBufferLength=4;
// If the file name length > 4, the file extension might be valid                  
const TInt KPlsFileExtensionsMightBeValid = 4;  
const TInt KM3uFileExtensionsMightBeValid = 4;

//extension of File to be recognised.
_LIT(KPlsExtension, ".pls");
//Mime type of the .pls file
_LIT8(KPlsMimeType,"audio/x-scpls");

_LIT(KM3uExtension, ".m3u");
_LIT8(KM3uMimeType, "audio/x-mpegurl");


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// RecognizerEx::RecognizerEx()
// constructs the object
// ---------------------------------------------------------------------------
//
CApaRecognizerEx::CApaRecognizerEx():CApaDataRecognizerType(KIRRecognizerDllUid, 
	CApaDataRecognizerType::EHigh)
    {
    IRLOG_DEBUG( "CApaRecognizerEx::CApaRecognizerEx()" );
    // It supports 2 mime type
    iCountDataTypes = 2;
    IRLOG_DEBUG( "CApaRecognizerEx::CApaRecognizerEx() - Exiting." );
    }

// ---------------------------------------------------------------------------
// RecognizerEx::~RecognizerEx()
// Destroys the object
// ---------------------------------------------------------------------------
//
CApaRecognizerEx::~CApaRecognizerEx()
    {
    IRLOG_DEBUG( "CApaRecognizerEx::~CApaRecognizerEx" );
    // no implementation
    }


// ---------------------------------------------------------------------------
// RecognizerEx::CreateRecognizerL()
// Returns pointer to the new object
// ---------------------------------------------------------------------------
//
CApaDataRecognizerType* CApaRecognizerEx::CreateRecognizerL()
    {
    IRLOG_DEBUG( "CApaRecognizerEx::CreateRecognizerL" );
    return new (ELeave) CApaRecognizerEx();
    }


// ---------------------------------------------------------------------------
// RecognizerEx::PreferredBufSize()
// Returns preferred buffer size
// ---------------------------------------------------------------------------
//
TUint CApaRecognizerEx::PreferredBufSize()
    {
    IRLOG_DEBUG( "CApaRecognizerEx::PreferredBufSize" );
    return KMaxBufferLength;
    }

// ---------------------------------------------------------------------------
// RecognizerEx::SupportedDataTypeL()
// Returns supported mime type
// ---------------------------------------------------------------------------
//
TDataType CApaRecognizerEx::SupportedDataTypeL(TInt aIndex) const
    {
    IRLOG_DEBUG( "CApaRecognizerEx::SupportedDataTypeL" );
    if (0 == aIndex)
	{
        return TDataType(KPlsMimeType);
	}
    else if (1 == aIndex)
    {
        return TDataType(KM3uMimeType);
    }
    else
    {
        ASSERT(0);
        return TDataType(KNullDesC8);
    }
    }

// ---------------------------------------------------------------------------
// RecognizerEx::DoRecognizeL()
// Recognizes the file by name and/or head buffer
// ---------------------------------------------------------------------------
//
void CApaRecognizerEx::DoRecognizeL(const TDesC& aName, 
	const TDesC8& aBuffer)
    {
	IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL" );    
    // To keep code simple, we only check file name extension
    TInt len = aBuffer.Length();
    if (aName.Length()>KPlsFileExtensionsMightBeValid)
		{
		//Compare the extension of the file to be recognised with .pls
		if (aName.Right(KPlsFileExtensionsMightBeValid).CompareF(
			KPlsExtension)==0)
	    {
			iConfidence=ECertain;   // is certainly something totally new
			iDataType=TDataType(KPlsMimeType);
			IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL - Exiting (1)." );
			return;
		}
		else if (aName.Right(KM3uFileExtensionsMightBeValid).CompareF(
            KM3uExtension)==0)
		{
		    _LIT8(KHttp, "http://");
		    if (KErrNotFound != aBuffer.Find(KHttp))
		    {
		        iConfidence = ECertain;
		        iDataType = TDataType(KM3uMimeType);
		        IRLOG_DEBUG("CApaRecognizerEx::DoRecognizeL - Exiting (2).");
		    }
		    return;
		}
	    IRLOG_DEBUG( "CApaRecognizerEx::DoRecognizeL - Exiting (3)." );					
		}
    }


// -----------------------------------------------------------------------------
// ImplementationTable
// violates PC lint error :: Warning 611: Suspicious cast
// required by the ECOM framework to correctly identify the instantiation 
// method pointer to provide to a client's resolution request.
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KIRRecognizerImplementationUid, 
    	CApaRecognizerEx::CreateRecognizerL)
    };
    
// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Provides access to the implementationtable
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
	TInt& aTableCount)
    {
    IRLOG_DEBUG( "ImplementationGroupProxy" );
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    IRLOG_DEBUG( "ImplementationGroupProxy - Exiting." );
    return ImplementationTable;
    }



