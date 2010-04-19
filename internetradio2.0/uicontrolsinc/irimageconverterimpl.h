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
* Description:  Internal image converter implementation
*
*/


#ifndef C_IRIMAGECONVERTERIMPL_H
#define C_IRIMAGECONVERTERIMPL_H

#include <f32file.h>
#include <svgrequestobserver.h>

class CBitmapScaler;
class CFbsBitmap;
class CGulIcon;
class CImageDecoder;
class CSvgEngineInterfaceImpl;
class MIRImageConverterObserver;

/**
 * Internal image converter implementation.
 * 
 * This class and some of its methods are document in more detail in irimageconverter.h,
 * as indicated by the @see tags.
 * 
 * @see CIRImageConverter
 */
NONSHARABLE_CLASS( CIRImageConverterImpl ) : public CActive, public MSvgRequestObserver
    {
    
public:

    /**
     * @see CIRImageConverter::NewL() 
     */
    static CIRImageConverterImpl* NewL();
    
    /**
     * @see CIRImageConverter::~CIRImageConverter()
     */
    ~CIRImageConverterImpl();

    /**
     * @see CIRImageConverter::SetDataL( const TDesC8& aData )
     */
    void SetDataL( const TDesC8& aData );
    
    /**
     * @see CIRImageConverter::SetObserver( MIRImageConverterObserver* aObserver )
     */
    void SetObserver( MIRImageConverterObserver* aObserver );
    
    /**
     * @see CIRImageConverter::EnableAnimations( TBool aEnable )
     */
    void EnableAnimations( TBool aEnable );
    
    /**
     * @see CIRImageConverter::IsAnimated() const
     */
    TBool IsAnimated() const;
    
    /**
     * @see CIRImageConverter::MaintainAspectRatio( TBool aMaintain )
     */
    void MaintainAspectRatio( TBool aMaintain );
    
    /**
     * @see CIRImageConverter::StartL( const TSize& aTarget, TInt aId )
     */
    void StartL( const TSize& aTarget, TInt aId );

    /**
     * @see CIRImageConverter::Stop()
     */
    void Stop();
    
    /**
     * @see CIRImageConverter::Bitmap() const
     */
    const CFbsBitmap* Bitmap() const;

    /**
     * @see CIRImageConverter::Mask() const
     */
    const CFbsBitmap* Mask() const;

    /**
     * @see CIRImageConverter::TransferBitmapOwnership( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
     */
    void TransferBitmapOwnership( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
    
// from base class CActive
    
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );
    
// from base class MSvgRequestObserver
    
    void UpdateScreen();
    TBool ScriptCall( const TDesC& aScript, CSvgElementImpl* aCallerElement );
    TInt FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );
    TInt FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );
    void UpdatePresentation( const TInt32& aNoOfAnimation );

private:

    /**
     * Constructor.
     */
    CIRImageConverterImpl();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Performs cleanup on the converter object.
     * 
     * Thorough cleanup must not be used if the converter should remain usable without having
     * to call SetDataL afterwards.
     * 
     * Should thorough cleanup be specified, everything, including the ICL image decoder and 
     * the SVG-T engine are destroyed.
     * 
     * @param   aThorough           Flag to indicate if thorough cleanup should be used.
     */
    void Cleanup( TBool aThorough = EFalse );

    /**
     * Creates the proper handler for the raw image data.
     * 
     * The handler must be created whenever the raw image data is changed, even if
     * the same type of handler would be used.
     */
    void CreateDataHandlerL();
    
    /**
     * Creates new bitmaps of the given size.
     * 
     * Performs deletion on the supplied bitmaps before assigning the created bitmaps 
     * on them, so calling this with bitmaps that already exists is acceptable.
     * 
     * Should creation of either of the new bitmaps fail, the supplied bitmaps are not modified.
     * 
     * @param   aSize               Size in pixels that the bitmaps are created to.
     * @param   aBitmap             On return, contains the created bitmap. 
     *                              Ownership is transferred to the caller.
     * @param   aMask               On return, contains the created mask. 
     *                              Ownership is transferred to the caller.
     */ 
    void CreateBitmapL( const TSize& aSize, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
    
    /**
     * Creates exact copies of the supplied bitmaps.
     * 
     * Performs deletion on the supplied bitmaps before assigning the created bitmaps 
     * on them, so calling this with bitmaps that already exists is acceptable.
     * 
     * Should creation of either of the new bitmaps fail, the supplied bitmaps are not modified.
     *
     * @param   aSourceBitmap       The bitmap to copy data from.
     * @param   aSourceMask         The mask to copy data from.
     * @param   aBitmap             On return, contains the copied bitmap. 
     *                              Ownership is transferred to the caller.
     * @param   aMask               On return, contains the copied mask. 
     *                              Ownership is transferred to the caller.
     */
    void CreateBitmapCopyL( const CFbsBitmap& aSourceBitmap, const CFbsBitmap& aSourceMask, 
    						CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
    
    /**
     * Starts the bitmap animation.
     * 
     * Requires that all the bitmap animation frames are prerendered successfully.
     */
    void StartBitmapAnimationL();
    
    /**
     * Renders the current bitmap animation frame.
     * 
     * @param   aFirstTime          Flag to indicate if this is the very first frame of the animation
     *                              to be rendered, ever.
     */
    void RenderBitmapAnimationFrameL( TBool aFirstTime = EFalse );
    
    /**
     * Notifies the observer either synchronously or asynchronously.
     * 
     * @param   aError              Error code to notify the observer with.
     * @param   aSynchronous        Flag to indicate if the notification should be synchronous or asynchronous.
     */
    void NotifyObserver( TInt aError, TBool aSynchronous = ETrue );
    
    /**
     * Call back for notifying the observer.
     * 
     * @param   aSelf               The object that made the request.
     * @return  KErrNone.
     */
    static TInt StaticNotifyObserverCallBack( TAny* aSelf );

    /**
     * Call back for advancing to the next frame when bitmap animations are used.
     * 
     * @param   aSelf               The object that made the request.
     * @return  KErrNone.
     */
    static TInt StaticNextFrameCallBack( TAny* aSelf );
    
private:

    /**
     * Possible converter internal states.
     */
    enum TIRImageConverterState
        {

        /**
         * Converter is idle; it cannot be used until SetDataL has been successfully called on it.
         */
        EIRStateIdle,
        
        /**
         * Converter has been initialized and it is using the SVG-T engine for rendering the raw image data. 
         */
        EIRStateSvgEngineInitialized,
        
        /**
         * Converter has been initialized and it is using ICL for rendering the raw image data. 
         */
        EIRStateBitmapDecoderInitialized,
        
        /**
         * Converter is currently converting the raw image data with the SVG-T engine.
         */
        EIRStateConvertingSvg,
        
        /**
         * Converter is currently converting the raw image data with ICL.
         */
        EIRStateConvertingBitmap,
        
        /**
         * Converter is currently scaling the decoded bitmap.
         */
        EIRStateScalingBitmap,
        
        /**
         * Converter is currently scaling the decoded bitmap's mask.
         */
        EIRStateScalingBitmapMask,
        
        /**
         * Converter is currently waiting for the next bitmap animation frame to be rendered.
         */
        EIRStateBetweenBitmapAnimationFrames
        
        };
    
private:

    /**
     * ICL image decoder that is used to convert all bitmap raw image data formats.
     * Owned.
     */
    CImageDecoder* iDecoder;
    
    /**
     * Scaler that is used to scale the converter bitmap and its mask when using the ICL image decoder.
     * Owned.
     */
    CBitmapScaler* iScaler;
    
    /**
     * SVG-T engine that is used to convert SVG image data.
     * Owned. 
     */
    CSvgEngineInterfaceImpl* iSvgEngine;

    /**
     * The actual resulting bitmap from the last successful conversion.
     * 
     * Note that the user of the class may take ownership of this object via the
     * TransferBitmapOwnership method.
     * 
     * Owned.
     */
    CFbsBitmap* iProcessedBitmap;
    
    /**
     * The actual resulting mask from the last successful conversion.
     * 
     * Note that the user of the class may take ownership of this object via the
     * TransferBitmapOwnership method.
     * 
     * Owned.
     */
    CFbsBitmap* iProcessedMask;

    /**
     * Bitmap that is being currently worked on.
     * Owned.
     */
    CFbsBitmap* iBitmap;
    
    /**
     * Mask that is being currently worked on.
     * Owned.
     */
    CFbsBitmap* iMask;

    /**
     * Bitmap of the last frame's state when using bitmap animations.
     * Owned.
     */
    CFbsBitmap* iLastFrameBitmap;
    
    /**
     * Mask of the last frame's state when using bitmap animations.
     * Owned.
     */
    CFbsBitmap* iLastFrameMask;
    
    /**
     * Original converted bitmap animation frames.
     * 
     * These are the, in most cases partial, bitmap animation frames that 
     * have been converted via ICL. These are not of the required target size,
     * but rather the size that has been defined for the frame in the bitmap
     * animation data. The bitmaps in this array must not be scaled. 
     * 
     * Owned.
     */
    RPointerArray<CGulIcon> iFrames;
    
    /**
     * Asynchronous call back to notify the observer.
     * Owned. 
     */
    CAsyncCallBack* iNotifyObserverCallBack;
    
    /**
     * Periodic timer used to display bitmap animations.
     * Owned. 
     */
    CPeriodic* iFrameTimer;
    
    /**
     * Observer that is notified of conversion events.
     * Not owned.
     */
    MIRImageConverterObserver* iObserver;
    
    /**
     * Flag used to indicate whether animations are enabled.
     */
    TBool iEnableAnimations;
    
    /**
     * Flag to indicate whether aspect ratio should be maintained when scaling.
     */
    TBool iMaintainAspectRatio;
    
    /**
     * Descriptor pointer to the raw image data supplied by the user.
     */
    TPtrC8 iData;
    
    /**
     * Conversion identifier that is used when notifying the observer.
     */
    TInt iId;
    
    /**
     * Target size of the converted bitmap in pixels.
     */
    TSize iTargetSize;

    /**
     * Flag to indicate whether the raw image data is animated.
     */
    TBool iIsAnimated;
    
    /**
     * Current frame index when processing bitmap animations.
     */
    TInt iFrameIndex;
    
    /**
     * Error code to notify the observer with.
     */
    TInt iError;
    
    /**
     * Current internal image converter state.
     */
    TIRImageConverterState iState;
    
    };

#endif // C_IRIMAGECONVERTERIMPL_H
