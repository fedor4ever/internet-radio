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


#ifndef CCIRIMAGECONVERTER_H
#define CCIRIMAGECONVERTER_H

#include <e32base.h>

class CFbsBitmap;
class MIRImageConverterObserver;

/**
 * Image converter.
 * 
 * This class is used to convert raw image data to more Symbian-friendly CFbsBitmap format, also
 * providing scaling support at the same time.
 * 
 * Supported image types include, but are not limited to, BMP, GIF, JPEG, TIFF, PNG and SVG-T.
 * 
 * Full animation support is available for GIF and SVG-T image types.
 */
NONSHARABLE_CLASS( CIRImageConverter ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * 
     * By default, enables animations and maintains aspect ratio for all conversions.
     * 
     * @return  The created object. Ownership is transferred to the caller.
     */
    static CIRImageConverter* NewL();

    /**
     * Destructor.
     * 
     * Cancels any conversions currently in progress without notifying the observer.
     */
    ~CIRImageConverter();

    /**
     * Sets the raw image data to be converted.
     * 
     * Does not make a copy of the data supplied, so care must be taken to ensure its
     * existence until image conversion is completed.
     */
    void SetDataL( const TDesC8& aData );
    
    /**
     * Sets the image conversion observer.
     * 
     * If set, the observer is notified of all pertinent conversion events.
     * 
     * @param   aObserver           Observer to set.
     */
    void SetObserver( MIRImageConverterObserver* aObserver );
    
    /**
     * Enables or disables animations.
     * 
     * Must be used before starting the conversion. Trying to toggle the animation
     * state while the conversion is in progress will result in a panic in debug builds.
     * 
     * @param   aEnable             Flag to enable or disable animations. 
     */
    void EnableAnimations( TBool aEnable );

    /**
     * Return a flag indicating whether the current raw image data is animated.
     * 
     * @return  Flag indicating whether the current raw image data is animated.
     */
    TBool IsAnimated() const;
    
    /**
     * Enables or disables maintaining the aspect ratio when scaling.
     * 
     * Must be used before starting the conversion. Trying to toggle the animation
     * state while the conversion is in progress will result in a panic in debug builds.
     *
     * @param   aMaintain           Flag to enable or disable maintaining the aspect ratio. 
     */
    void MaintainAspectRatio( TBool aMaintain );
    
    /**
     * Starts the image conversion.
     * 
     * Must be called after SetDataL has been successfully called to initiate the
     * actual conversion. Trying to start a conversion when one is already in progress
     * will cause a leave, so calling Stop beforehand is advisable.
     * 
     * @param   aTarget             Image target size in pixels.
     * @param   aId                 Identifier used to notify the observer with. 
     */
    void StartL( const TSize& aTarget, TInt aId = KErrUnknown );

    /**
     * Stops the conversion.
     * 
     * Does nothing if no conversions are currently in progress. Notifies the observer
     * with KErrCancel if a conversion was cancelled.
     */
    void Stop();

    /**
     * Returns the converted bitmap.
     * 
     * Returns a NULL pointer when a conversion has not yet completed successfully.
     * 
     * @return  The converted bitmap. Ownership is not transferred to the caller.
     */
    const CFbsBitmap* Bitmap() const;
    
    /**
     * Returns the converted mask.
     * 
     * Returns a NULL pointer when a conversion has not yet completed successfully.
     * 
     * @return  The converted mask. Ownership is not transferred to the caller.
     */
    const CFbsBitmap* Mask() const;
    
    /**
     * Transfers ownership of the converted bitmap and mask to the caller.
     * 
     * NULL pointers are returned if a conversion has not yet completed successfully.
     * 
     * Any calls to either Bitmap or Mask methods after this will return a NULL pointer
     * until another conversion has been completed successfully.
     * 
     * @param   aBitmap             On return, contains the converted bitmap.
     *                              Ownership is tranferred to the caller.
     * @param   aBitmap             On return, contains the converted mask.
     *                              Ownership is tranferred to the caller.
     */
    void TransferBitmapOwnership( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
    
private:

    /**
     * Constructor.
     */
    CIRImageConverter();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();

private:

    /**
     * Image converter private data.
     * Owned.
     */
    struct TIRImageConverterPrivateData;
    TIRImageConverterPrivateData* iData;
    
    };

#endif // CCIRIMAGECONVERTER_H
