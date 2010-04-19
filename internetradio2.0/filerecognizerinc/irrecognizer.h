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


#ifndef IRRECOGNIZER_H
#define IRRECOGNIZER_H

#include <apmrec.h>

const TInt KIRRecognizerImplementationUid = 0x2000B4DE;
const TUid KIRRecognizerDllUid = {0x2000B4A9};
/**
 * Recognizer plug-in for .Pls files.
 * 
 * @code
 * Uses Methods from the base class CApaDataRecognizerType
 * to recognise the .pls files  
 * @endcode
 *
*/
class CApaRecognizerEx: public CApaDataRecognizerType
    {
public:
    /**
    * Default constructor
    */
    CApaRecognizerEx();

    /**
    * Destructor.
    */
    virtual ~CApaRecognizerEx();

    /**
    * CreateRecognizerL()
    * From CApaDataRecognizerType CreateRecognizerL.
    * creates and returns recognizer
    */
	static CApaDataRecognizerType* CreateRecognizerL();

    /**
    * PreferredBufSize()
    * From CApaDataRecognizerType PreferredBufSize.
    * returns preferred buffer size
    */
	TUint PreferredBufSize();

    /**
    * SupportedDataTypeL(SupportedDataTypeL)
    * From CApaDataRecognizerType SupportedDataTypeL.
    * @param aIndex is index that identifies the data type
    * returns supported mime type
    */
	TDataType SupportedDataTypeL(TInt aIndex) const;

private:
        /**
	 * DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	 * From CApaDataRecognizerType DoRecognizeL.
	 * recognises the file by name and/or buffer
	 * @param aName is file name containing the data to be recognized
	 * @param aBuffer is buffer containing data to be recognized
	 */
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
    };

#endif //IRRECOGNIZER_H
