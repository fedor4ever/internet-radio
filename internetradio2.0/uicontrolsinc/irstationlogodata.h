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
* Description:  Data for single station logo
*
*/


#ifndef C_CIRSTATIONLOGODATA_H
#define C_CIRSTATIONLOGODATA_H

#include <alf/alfbitmapprovider.h>
#include <alf/alfeventhandler.h>
#include <e32base.h>

class CAlfTexture;
class CFbsBitmap;
class CIRImageConverter;

/**
 * Station Logo Data
 *
 * @since   S60 v3.2
 */
class CIRStationLogoData : public CBase, 
                           public MAlfBitmapProvider,
                           public MAlfEventHandler
    {
public:

    /**
     * Logo related custom events
     */
    enum TIRLogoCustomEvents
        {
        EIRCustomEventLogoFadeOutComplete
        };
    
    /**
     * Two-phased constructor.
     * Adds the control to the control group and to layout hierarchy
     *
     * @param aId The IID for this logo
     * @param aRawData The graphics raw data for this logo
     */
    static CIRStationLogoData* NewLC( TInt aId, const TDesC8& aRawData );

    /**
     * Destructor.
     */
    ~CIRStationLogoData();

    /**
     * Sets the raw data for the graphics
     * 
     * @param aRawData reference to raw data
     */
    void SetRawData( const TDesC8& aRawData );

    /**
     * Provides the raw data for the graphics
     * 
     * @return reference to raw data
     */
    const TDesC8& RawData() const;
    
    /**
     * Updates the texture according to the bitmaps provided. 
     * Ownership of the bitmaps is transfered always, even if this method leaves.
     * So the bitmap an mask must not be in cleanup stack
     * 
     * @param aBitmap The bitmap to be used
     * @param aMask The mask to be used
     */
    void UpdateTextureL( CFbsBitmap* aBitmap, CFbsBitmap* aMask );

    /**
     * Destroys the texture and possible bitmaps
     */
    void DestroyTexture();

    /**
     * Sets the visual for this logo data.
     * 
     * @param aVisual The visual to be set. Transfers ownership
     */
    void SetVisual( CAlfImageVisual* aVisual );

    /**
     * Provides the image visual for this logo
     * 
     * @return Pointer to CAlfImageVisual, or NULL
     */
    CAlfImageVisual* Visual();

    /**
     * Destroys the visual for the logo
     */
    void DestroyVisual();

    /**
     * Provides the texture for this logo
     * 
     * @return The reference to texture
     */
    CAlfTexture* Texture();

    /**
     * Provides the ID for this logo
     * 
     * @return The ID
     */
    TInt Id() const ;

    // from MAlfBitmapProvider
    void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);

    // from MAlfEventHandler
    TBool OfferEventL( const TAlfEvent& aEvent );

private: // Methods

    /**
     * Default constructor
     *
     * @param aId The IID for this logo
     * @param aRawData The graphics raw data for this logo
     */
    CIRStationLogoData( TInt aId, const TDesC8& aRawData );

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Members

    /**
     * The logo as a bitmap.
     * Owned.
     */
    CFbsBitmap* iBitmap;

    /**
     * The logo bitmap mask.
     * Owned.
     */
    CFbsBitmap* iMask;

    /**
     * The logo represented as a texture.
     * Owned.
     */
    CAlfTexture* iTexture;

    /**
     * The visual for the logo
     * Owned.
     */
    CAlfImageVisual* iVisual;

    /**
     * The raw data of graphics.
     */
    TPtrC8 iData;
    
    /**
     * The ID that is used to identify the logo
     */
    TInt iId;
    
    /**
     * The texture ID provided by the texture manager
     */
    TInt iTextureId;
    };

#endif // C_CIRSTATIONLOGODATA_H
