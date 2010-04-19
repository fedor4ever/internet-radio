/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Image converter
*
*/


#include "irimageconverter.h"
#include "irimageconverterimpl.h"

/**
 * Image converter private data.
 */ 
struct CIRImageConverter::TIRImageConverterPrivateData
    {
    
    /**
     * Image converter implementation.
     * Not owned.
     */
    CIRImageConverterImpl* iConverterImpl;
    
    };

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRImageConverter* CIRImageConverter::NewL()
    {
    CIRImageConverter* self = new ( ELeave ) CIRImageConverter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRImageConverter::CIRImageConverter()
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::ConstructL()
    {
    iData = new ( ELeave ) TIRImageConverterPrivateData;
    
    iData->iConverterImpl = CIRImageConverterImpl::NewL();
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIRImageConverter::~CIRImageConverter()
    {
    if ( iData )
        {
        delete iData->iConverterImpl;
        }
    delete iData;
    }

// ---------------------------------------------------------------------------
// Sets the raw image data to be converted.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::SetDataL( const TDesC8& aData )
    {
    iData->iConverterImpl->SetDataL( aData );
    }

// ---------------------------------------------------------------------------
// Sets the image conversion observer.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::SetObserver( MIRImageConverterObserver* aObserver )
    {
    iData->iConverterImpl->SetObserver( aObserver );
    }

// ---------------------------------------------------------------------------
// Enables or disables animations.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::EnableAnimations( TBool aEnable )
    {
    iData->iConverterImpl->EnableAnimations( aEnable );
    }

// ---------------------------------------------------------------------------
// Return a flag indicating whether the current raw image data is animated.
// ---------------------------------------------------------------------------
//
TBool CIRImageConverter::IsAnimated() const
    {
    return iData->iConverterImpl->IsAnimated();
    }

// ---------------------------------------------------------------------------
// Enables or disables maintaining the aspect ratio when scaling.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::MaintainAspectRatio( TBool aMaintain )
    {
    iData->iConverterImpl->MaintainAspectRatio( aMaintain );
    }

// ---------------------------------------------------------------------------
// Starts the image conversion.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::StartL( const TSize& aTarget, TInt aId )
    {
    iData->iConverterImpl->StartL( aTarget, aId );
    }

// ---------------------------------------------------------------------------
// Stops the conversion.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::Stop()
    {
    iData->iConverterImpl->Stop();
    }

// ---------------------------------------------------------------------------
// Returns the converted bitmap.
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CIRImageConverter::Bitmap() const
    {
    return iData->iConverterImpl->Bitmap();
    }

// ---------------------------------------------------------------------------
// Returns the converted mask.
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CIRImageConverter::Mask() const
    {
    return iData->iConverterImpl->Mask();
    }

// ---------------------------------------------------------------------------
// Transfers ownership of the converted bitmap and mask to the caller.
// ---------------------------------------------------------------------------
//
void CIRImageConverter::TransferBitmapOwnership( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    iData->iConverterImpl->TransferBitmapOwnership( aBitmap, aMask );
    }
