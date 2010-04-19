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


#include <akniconutils.h>
#include <gulicon.h>

#include "irdebug.h"
#include "iriconloader.h"

//---------------------------------------------------------------------------- 
// Function:CIconFileProvider
// this function is the Default constructor
//---------------------------------------------------------------------------- 
EXPORT_C CIconFileProvider::CIconFileProvider()
	{
	IRLOG_DEBUG( "CIconFileProvider::CIconFileProvider" );
	}


//---------------------------------------------------------------------------- 
//  CIconFileProvider::NewL(..)
//  Creates a new instance of CIconFileProvider view
//  @param aRect Sets the control's extent, specifying a rectangle.
//  @return An instance of CIconFileProvider
//---------------------------------------------------------------------------- 
//
EXPORT_C CIconFileProvider* CIconFileProvider::NewL(const TDesC& aFilename)
	{
    IRLOG_DEBUG( "CIconFileProvider::NewL" );  
    CIconFileProvider* self = new (ELeave) CIconFileProvider();
    CleanupStack::PushL(self);
    self->ConstructL(aFilename);
    CleanupStack::Pop( self ); // self
    IRLOG_DEBUG( "CIconFileProvider::NewL - Exiting." );
    return self;
	}

//---------------------------------------------------------------------------- 
//  CIconFileProvider::ConstructL(..)
//  Second phase constructor
//  @param aFilename Full private path of the application
//  @return void
//---------------------------------------------------------------------------- 
//
void CIconFileProvider::ConstructL(const TDesC& aFilename)
	{
    IRLOG_DEBUG( "CIconFileProvider::ConstructL" );
	iFilename = aFilename.AllocL();
    IRLOG_DEBUG( "CIconFileProvider::ConstructL - Exiting." );
	}


//---------------------------------------------------------------------------- 
// CIconFileProvider:: ~CIconFileProvider()
// Default Destructor
//----------------------------------------------------------------------------
CIconFileProvider::	~CIconFileProvider()
	{
	IRLOG_DEBUG( "CIconFileProvider::~CIconFileProvider" );
	delete iFilename;
	}


//---------------------------------------------------------------------------- 
//  CIconFileProvider::CreateIconL(..)
//  Creates an icon.
//  The icon is created by loading the image bitmap identified by aBitmapId 
//  and the mask identified by aMaskId. It returns a pointer to the icon and 
// transfers ownership.
//---------------------------------------------------------------------------- 
//
EXPORT_C CGulIcon* CIconFileProvider::CreateIconL(TInt aBitmapId,
	TInt aMaskId) const
	{
    IRLOG_DEBUG( "CIconFileProvider::CreateIconL" );
	CGulIcon* icon = CGulIcon::NewL();
	CFbsBitmap* mask = NULL;
	CFbsBitmap* bitmap = NULL;
	AknIconUtils::CreateIconL(bitmap , mask , *iFilename, aBitmapId , aMaskId);    
	icon->SetBitmap(bitmap);
	icon->SetMask(mask);
    IRLOG_DEBUG( "CIconFileProvider::CreateIconL - Exiting." );
	return icon;
	}

//---------------------------------------------------------------------------- 
//  CIconFileProvider::CreateIconL(.)
//  Creates an icon.
//  The icon is created by loading the image bitmap identified by aBitmapId.
//  It returns a pointer to the icon and transfers ownership.
//  @param aBitmapId The bitmap ID.
//  @param aMaskId The bitmap mask ID.
//  @return A pointer to the icon
//---------------------------------------------------------------------------- 
//
EXPORT_C CFbsBitmap* CIconFileProvider::CreateIconL(TInt aBitmapId) const
	{
    IRLOG_DEBUG( "CIconFileProvider::CreateIconL" );
	if(aBitmapId!=-1)
		{
		CFbsBitmap* bitmap = NULL;
		
		//The icon is created by loading the image bitmap identified
		// by aBitmapId.
		bitmap = AknIconUtils::CreateIconL(*iFilename, aBitmapId);    
		return bitmap;
		}
    IRLOG_DEBUG( "CIconFileProvider::CreateIconL - Exiting." );
	return NULL;
	}

