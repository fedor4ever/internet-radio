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


#ifndef IRICONLOADER_H
#define IRICONLOADER_H

#include <e32base.h>

class CFbsBitmap;
class CGulIcon;

/**
 *  This class Creates CIconFileProvider.
 *
*  Since the AknIconUtils cannot access the private directory of application,
*  mix-in class MAknIconFileProvider is implemented to provide handle of the 
*  icon files (ScalableScreenDrawingIcons.mbm in this case) to AknIconUtils.
 *  Creates an icon.All the view will use this API to create the icons for the view.
 *  
 * @code
 * 
 * The icon is created by loading the image bitmap identified by aBitmapId 
 * and the mask identified by aMaskId.
 * AknIconUtils::CreateIconL(bitmap , mask , *iFilename, aBitmapId , aMaskId);    .
 *
 * Creates an icon.
 * The icon is created by loading the image bitmap identified by aBitmapId.
 * It returns a pointer to the icon and transfers ownership.
 * AknIconUtils::CreateIconL(*iFilename, aBitmapId);
 * 
 * @endcode
 *
*/
class CIconFileProvider : public CBase
{	
public:
	/**
	* CIconFileProvider().
	* C++ default constructor.
	*/
	IMPORT_C CIconFileProvider();

	/**
	* NewL.
	* Two-phased constructor.
	* Create a CIconFileProvider object, which will draw itself to 
	* aRect.
	* @param aFilename the path where the icon should be loaded.
	* @return a pointer to the created instance of CIconFileProvider.
	*/
	IMPORT_C static CIconFileProvider* NewL(const TDesC& aFilename);

	/**
	* ConstructL
	* 2nd phase constructor.
	* Perform the second phase construction of a
	* CIconFileProvider object.
	* @param aFilename the path where the icon should be loaded.
	*/
	void ConstructL( const TDesC& aFilename);

	/**
	* ~CIconFileProvider
	* Destructor.
	*/
	~CIconFileProvider();

	/**
	* CreateIconL().
	* Creates the icons for the listbox.
	* @param ID for the bitmap for which icon should be created.
	* @param ID for the bitmap mask for which icon should be created.
	*/
	IMPORT_C CGulIcon* CreateIconL(TInt aBitmapId,TInt aMaskId) const;

	/**
	* CreateIconL().
	* Creates the icons for the listbox.
	* @param ID for the bitmap for which icon should be created.
	*/
	IMPORT_C CFbsBitmap* CreateIconL(TInt aBitmapId) const;
private:
	// Filename where the icon should be created.
    HBufC* iFilename;
};

#endif//IRICONLOADER_H