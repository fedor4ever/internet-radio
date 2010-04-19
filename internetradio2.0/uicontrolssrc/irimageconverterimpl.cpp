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


#include <aknlayoutfont.h>
#include <aknutils.h>
#include <bitmaptransforms.h>
#include <coemain.h>
#include <gulicon.h>
#include <ihlbitmaputil.h>
#include <imageconversion.h>
#include <svgengineinterfaceimpl.h>


#include "irimageconverterimpl.h"
#include "irimageconverterobserver.h"
#include "irdebug.h"
// Image decoder options.
const CImageDecoder::TOptions KIRImageDecoderOptions = CImageDecoder::EOptionAlwaysThread;
// Bitmap scaler quality.
const CBitmapScaler::TQualityAlgorithm KIRBitmapScalerQualityAlgorithm = 
											CBitmapScaler::EMaximumQuality;
const TInt KFour=4;
// ---------------------------------------------------------------------------
// @see CIRImageConverter::NewL()
// ---------------------------------------------------------------------------
//
CIRImageConverterImpl* CIRImageConverterImpl::NewL()
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::NewL - Entering" );
    CIRImageConverterImpl* self = new ( ELeave ) CIRImageConverterImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRImageConverterImpl::CIRImageConverterImpl()
    : CActive( CActive::EPriorityStandard ),
      iEnableAnimations( ETrue ),
      iMaintainAspectRatio( ETrue )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::ConstructL()
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::ConstructL - entering" );
    iNotifyObserverCallBack = new ( ELeave ) CAsyncCallBack( 
        TCallBack( StaticNotifyObserverCallBack, this ), CActive::EPriorityHigh );
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::~CIRImageConverter()
// ---------------------------------------------------------------------------
//
CIRImageConverterImpl::~CIRImageConverterImpl()
    {
    Cancel();

    iFrames.ResetAndDestroy();
    iFrames.Close();
    
    delete iDecoder;
    delete iScaler;
    delete iSvgEngine;
    delete iProcessedBitmap;
    delete iProcessedMask;
    delete iBitmap;
    delete iMask;
    delete iLastFrameBitmap;
    delete iLastFrameMask;
    delete iNotifyObserverCallBack;
    delete iFrameTimer;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::SetDataL( const TDesC8& aData )
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::SetDataL( const TDesC8& aData )
    {
    IRRDEBUG2( "CIRImageConverterImpl::SetDataL - Entering", KNullDesC );
    Cleanup( ETrue );
    iData.Set( aData );
    CreateDataHandlerL();
    IRRDEBUG2( "CIRImageConverterImpl::SetDataL - Exiting", KNullDesC );
   }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::SetObserver( MIRImageConverterObserver* aObserver )
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::SetObserver( MIRImageConverterObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::EnableAnimations( TBool aEnable )
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::EnableAnimations( TBool aEnable )
    {
    __ASSERT_DEBUG( iState == EIRStateIdle, User::Invariant() );
    iEnableAnimations = aEnable;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::IsAnimated() const
// ---------------------------------------------------------------------------
//
TBool CIRImageConverterImpl::IsAnimated() const
    {
    __ASSERT_DEBUG( iState != EIRStateIdle, User::Invariant() );
    return iIsAnimated;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::MaintainAspectRatio( TBool aMaintain )
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::MaintainAspectRatio( TBool aMaintain )
    {
    __ASSERT_DEBUG( iState == EIRStateIdle, User::Invariant() );
    iMaintainAspectRatio = aMaintain;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::StartL( const TSize& aTarget, TInt aId )
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::StartL( const TSize& aTarget, TInt aId )
	{
    IRRDEBUG2( "CIRImageConverterImpl::StartL - Entering", KNullDesC );
	if ( iState != EIRStateBitmapDecoderInitialized && iState != EIRStateSvgEngineInitialized )
		{
		if ( iState == EIRStateIdle )
			{
		    IRRDEBUG2("CIRImageConverterImpl::StartL, leave with KErrNotReady", KNullDesC);
			User::Leave( KErrNotReady );
			}
		else
			{
		    IRRDEBUG2("CIRImageConverterImpl::StartL, leave with KErrInUse", KNullDesC);
			User::Leave( KErrInUse );
			}
		}
	iTargetSize = aTarget;
	iId = aId;
	if ( iState == EIRStateSvgEngineInitialized )
		{
		// The SVG-T engine has been initialized with dummy 0x0 bitmaps, so updating those must be handled here.
	    IRRDEBUG2( "CIRImageConverterImpl::StartL - if begin", KNullDesC );
		CreateBitmapL( iTargetSize, iBitmap, iMask );
		iSvgEngine->SetFrameBuffer( iBitmap );
		iState = EIRStateConvertingSvg;
		iSvgEngine->Start();
		IRRDEBUG2( "CIRImageConverterImpl::StartL - if end", KNullDesC );
		}
	else
		{
	    IRRDEBUG2( "CIRImageConverterImpl::StartL - else begin", KNullDesC );
		iState = EIRStateConvertingBitmap;
		IRRDEBUG2( "CIRImageConverterImpl::StartL, iDecorder = %d", iDecoder);
		iDecoder->Convert( &iStatus, *iBitmap, *iMask );
		IRRDEBUG2( "CIRImageConverterImpl::StartL - else-exit call setactive", KNullDesC );
		SetActive();
		}
	IRRDEBUG2( "CIRImageConverterImpl::StartL - Exiting", KNullDesC );
	}

// ---------------------------------------------------------------------------
// @see CIRImageConverter::Stop()
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::Stop()
	{
    IRRDEBUG2( "CIRImageConverterImpl::Stop - Entering", KNullDesC );
	Cancel();
	if ( iSvgEngine )
	    {
	    iSvgEngine->Stop();
	    }
	TIRImageConverterState state = iState;
	Cleanup();
	if ( state != EIRStateIdle && state != EIRStateBitmapDecoderInitialized && 
		state != EIRStateSvgEngineInitialized )
	    {
	    NotifyObserver( KErrCancel );
	    }
	IRRDEBUG2( "CIRImageConverterImpl::Stop - Exiting",KNullDesC );
	}

// ---------------------------------------------------------------------------
// @see CIRImageConverter::Bitmap() const
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CIRImageConverterImpl::Bitmap() const
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::Bitmap - Entering" );
    return iProcessedBitmap;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::Mask() const
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CIRImageConverterImpl::Mask() const
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::Mask - Entering" );
    return iProcessedMask;
    }

// ---------------------------------------------------------------------------
// @see CIRImageConverter::TransferBitmapOwnership( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::TransferBitmapOwnership( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = iProcessedBitmap;
    iProcessedBitmap = NULL;
    aMask = iProcessedMask;
    iProcessedMask = NULL;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Invoked when the active object is cancelled.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::DoCancel()
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::DoCancel - Entering" );
    if ( iDecoder )
        {
        iDecoder->Cancel();
        }
    if ( iScaler )
        {
        iScaler->Cancel();
        }
	IRLOG_DEBUG( "CIRImageConverterImpl::DoCancel - Exiting" );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Invoked when the active object completes an asynchronous request.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::RunL()
	{
	IRLOG_DEBUG( "CIRImageConverterImpl::RunL - Entering" );
	if ( iStatus.Int() == KErrNone )
		{
		switch ( iState )
			{
			case EIRStateConvertingBitmap:
				{
				if ( iIsAnimated && iEnableAnimations )
					{
					CGulIcon* icon = CGulIcon::NewLC();

					icon->SetBitmapsOwnedExternally( EFalse );
					icon->SetBitmap( iBitmap );
					icon->SetMask( iMask );

					iBitmap = NULL;
					iMask = NULL;

					iFrames.AppendL( icon );
					CleanupStack::Pop( icon );

					if ( iFrames.Count() < iDecoder->FrameCount() )
						{
						CreateBitmapL( iDecoder->FrameInfo( iFrames.Count() ).iOverallSizeInPixels,
								 iBitmap, iMask );

						iDecoder->Convert( &iStatus, *iBitmap, *iMask, iFrames.Count() );
						SetActive();
						}
					else
						{
						StartBitmapAnimationL();
						}
					}
				else
					{
					iState = EIRStateScalingBitmap;
					iScaler->Scale( &iStatus, *iBitmap, iTargetSize, iMaintainAspectRatio );
					SetActive();
					}
				}
			break;
			case EIRStateScalingBitmap:
			iState = EIRStateScalingBitmapMask;
			iScaler->Scale( &iStatus, *iMask, iTargetSize, iMaintainAspectRatio );
			SetActive();
			break;
			case EIRStateScalingBitmapMask:
			NotifyObserver( KErrNone );
			break;
			default:
			break;
			}
		}
	else
		{
		NotifyObserver( iStatus.Int() );
		}
	IRLOG_DEBUG( "CIRImageConverterImpl::RunL - Exiting" );
	}

// ---------------------------------------------------------------------------
// From class CActive.
// Invoked when RunL leaves.
// ---------------------------------------------------------------------------
//
TInt CIRImageConverterImpl::RunError( TInt aError )
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::NewL - Entering" );
    NotifyObserver( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// Invoked when the bitmap has been updated by the SVG-T engine.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::UpdateScreen()
    {
    iSvgEngine->GenerateMask( iMask );
    // SVG-T engine MUST NOT be destroyed here as its internal implementation
    // relies on it existing after execution leaves this method. This means
    // that the observer notification must be asynchronous
    NotifyObserver( KErrNone, EFalse );
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// Not implemented.
// ---------------------------------------------------------------------------
//
TBool CIRImageConverterImpl::ScriptCall( const TDesC& /*aScript*/, CSvgElementImpl* 
											/*aCallerElement*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// Not implemented.
// ---------------------------------------------------------------------------
//
TInt CIRImageConverterImpl::FetchImage( const TDesC& /*aUri*/, RFs& /*aSession*/, RFile& 
																		/*aFileHandle*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// Not implemented.
// ---------------------------------------------------------------------------
//
TInt CIRImageConverterImpl::FetchFont( const TDesC& /*aUri*/, RFs& /*aSession*/, RFile& 
																		/*aFileHandle*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class MSvgRequestObserver.
// Not implemented.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::UpdatePresentation( const TInt32& /*aNoOfAnimation*/ )
    {
    }

// ---------------------------------------------------------------------------
// Performs cleanup on the converter object.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::Cleanup( TBool aThorough )
    {
    Cancel();
    if ( aThorough )
        {
        delete iProcessedBitmap;
        iProcessedBitmap = NULL;
        
        delete iProcessedMask;
        iProcessedMask = NULL;
        }
    if(iDecoder)
	    {
	    delete iDecoder;
	    iDecoder = NULL;
	    }
    if(iScaler)
	    {
	    delete iScaler;
	    iScaler = NULL;
	    }
	    
	if(iSvgEngine)
		{
	    delete iSvgEngine;
	    iSvgEngine = NULL;
		}
	if(iFrameTimer)
		{
	    delete iFrameTimer;
	    iFrameTimer = NULL;
		}
	if(iBitmap)
		{
	    delete iBitmap;
	    iBitmap = NULL;
		}
	if(iMask)
		{
	    delete iMask;
	    iMask = NULL;
		}
	if(iLastFrameBitmap)
		{
	    delete iLastFrameBitmap;
	    iLastFrameBitmap = NULL;
		}
	if(iLastFrameMask)
		{
	    delete iLastFrameMask;
	    iLastFrameMask = NULL;
		}

    iFrames.ResetAndDestroy();

    iNotifyObserverCallBack->Cancel();

    iFrameIndex = 0;
    iIsAnimated = EFalse;
    iError = KErrNone;
    iState = EIRStateIdle;
    }

// ---------------------------------------------------------------------------
// Creates the proper handler for the raw image data.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::CreateDataHandlerL()
    {
    IRRDEBUG2( "CIRImageConverterImpl::CreateDataHandlerL - Entering", KNullDesC );
    TBuf8<KMaxDataTypeLength> mimeType;
    TRAPD( err, CImageDecoder::GetMimeTypeDataL( iData, mimeType ) )

	if ( err == KErrNone ) // Image decoder can handle this MIME type.
		{

		TRAPD(err1,iDecoder = CImageDecoder::DataNewL( CCoeEnv::Static()->FsSession(), iData,
		KIRImageDecoderOptions );)
		if(err1!=KErrNone)
			{  
			if ( iObserver )
				{
			    IRRDEBUG2( "CIRImageConverterImpl::CreateDataHandlerL, err = %d", err1);
				NotifyObserver( err1 );
				IRRDEBUG2( "CIRImageConverterImpl::CreateDataHandlerL, error is notified", KNullDesC);
				}
			}
		else
			{
        iScaler = CBitmapScaler::NewL();
        iScaler->SetQualityAlgorithm( KIRBitmapScalerQualityAlgorithm );
        iScaler->UseLowMemoryAlgorithm( ETrue ); // Return value ignored on purpose; it's ok if low memory algorithm cannot be used when scaling.

        if ( iDecoder->FrameCount() > 1 )
            {
            iFrameTimer = CPeriodic::NewL( CActive::EPriorityStandard );
            iIsAnimated = ETrue;
            }

        CreateBitmapL( iDecoder->FrameInfo().iOverallSizeInPixels, iBitmap, iMask );
        }
		}
    else // Try using the SVG-T engine for decoding the image.
        {
        
        TFontSpec fontSpec = AknLayoutUtils::LayoutFontFromId( 
        						EAknLogicalFontSecondaryFont )->FontSpecInTwips();

        // The SVG-T engine requires that the bitmap is created prior to its instantiation.
        // As the dummy bitmaps created here are 0x0, StartL must take care of instantiating them to
        // proper sizes and updating the SVG-T engine's frame buffer manually before starting the conversion.
        
        CreateBitmapL( TSize( 0, 0 ), iBitmap, iMask );
        
        iSvgEngine = CSvgEngineInterfaceImpl::NewL( iBitmap, this, fontSpec );
        MSvgError* svgErr = iSvgEngine->Load( iData );
        if ( svgErr && svgErr->HasError() )
            {
            err = svgErr->SystemErrorCode();
            }
        else
            {
            err = KErrNone;
            iIsAnimated = iSvgEngine->SvgHasAnimation( iSvgEngine->SvgDocument() );
            }
        }

    if ( err )
        {
        Cleanup( ETrue );
        User::Leave( KErrNotSupported );
        }
    else
        {
        iState = iSvgEngine ? EIRStateSvgEngineInitialized : EIRStateBitmapDecoderInitialized;
        }

    IRRDEBUG2( "CIRImageConverterImpl::CreateDataHandlerL - Exiting" , KNullDesC);
   }

// ---------------------------------------------------------------------------
// Creates new bitmaps of the given size.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::CreateBitmapL( const TSize& aSize, CFbsBitmap*& aBitmap, 
										CFbsBitmap*& aMask )
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::CreateBitmapL - Entering" );
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( aSize, CCoeEnv::Static()->ScreenDevice()
    									->DisplayMode() ) );
    
    CFbsBitmap* mask = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( mask );
    User::LeaveIfError( mask->Create( aSize, EGray256 ) );
    
    delete aBitmap;
    aBitmap = bitmap;
    
    delete aMask;
    aMask = mask;
    
    CleanupStack::Pop( 2, bitmap );
	IRLOG_DEBUG( "CIRImageConverterImpl::CreateBitmapL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Creates exact copies of the supplied bitmaps.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::CreateBitmapCopyL( const CFbsBitmap& aSourceBitmap, 
                                               const CFbsBitmap& aSourceMask, 
                                               CFbsBitmap*& aBitmap, 
                                               CFbsBitmap*& aMask )
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::CreateBitmapCopyL - Entering" );
    CFbsBitmap* bitmap = IHLBitmapUtil::CopyBitmapLC( aSourceBitmap );
    CFbsBitmap* mask = IHLBitmapUtil::CopyBitmapLC( aSourceMask );
    
    delete aBitmap;
    aBitmap = bitmap;
    
    delete aMask;
    aMask = mask;

    CleanupStack::Pop( 2, bitmap );
	IRLOG_DEBUG( "CIRImageConverterImpl::CreateBitmapCopyL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Starts the bitmap animation.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::StartBitmapAnimationL()
    {
    __ASSERT_DEBUG( iFrames.Count() == iDecoder->FrameCount(), User::Invariant() );
    
    RenderBitmapAnimationFrameL( ETrue );
    }

// ---------------------------------------------------------------------------
// Renders the current bitmap animation frame.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::RenderBitmapAnimationFrameL( TBool aFirstTime )
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::RenderBitmapAnimationFrameL - Entering" );
    __ASSERT_DEBUG( iFrames.Count() && iFrames.Count() > iFrameIndex, User::Invariant() );
    
    
    const TGifImageControl* gifImageControl = static_cast<const TGifImageControl*>
    							( iDecoder->FrameData( iFrameIndex ).GetFrameData( 0 ) );
    if ( gifImageControl )
        {
        if ( aFirstTime )
            {
            // When rendering the animation for the very first time, we just make a copy of the first frame.
            CreateBitmapCopyL( *iFrames[0]->Bitmap(), *iFrames[0]->Mask(), 
            				iLastFrameBitmap, iLastFrameMask );
            }
        else
            {
            CFbsBitmapDevice* lastFrameBitmapDevice = CFbsBitmapDevice::NewL( iLastFrameBitmap );
            CleanupStack::PushL( lastFrameBitmapDevice );
            CFbsBitGc* lastFrameBitmapGc = CFbsBitGc::NewL();
            CleanupStack::PushL( lastFrameBitmapGc );
            lastFrameBitmapGc->Activate( lastFrameBitmapDevice );
            
            CFbsBitmapDevice* lastFrameMaskDevice = CFbsBitmapDevice::NewL( iLastFrameMask );
            CleanupStack::PushL( lastFrameMaskDevice );
            CFbsBitGc* lastFrameMaskGc = CFbsBitGc::NewL();
            CleanupStack::PushL( lastFrameMaskGc );
            lastFrameMaskGc->Activate( lastFrameMaskDevice );
            
            TRect rect = iDecoder->FrameInfo( iFrameIndex ).iFrameCoordsInPixels;
            

            switch ( gifImageControl->iDisposalMethod )
                {
                case TGifImageControl::ENone:
                    // The new frame is completely self-sufficient, so no information about the previous frame is required.
                    lastFrameBitmapGc->BitBlt( rect.iTl, iFrames[iFrameIndex]->Bitmap(), 
                    							TRect( TPoint(), rect.Size() ) );
                    lastFrameMaskGc->BitBlt( rect.iTl, iFrames[iFrameIndex]->Mask(),
                    						 TRect( TPoint(), rect.Size() ) );
                    break;
                case TGifImageControl::ELeaveInPlace:
                    // The new frame only contains a partial update on the image, so we have to retain the previous frame state.
                    lastFrameBitmapGc->BitBltMasked( rect.iTl, iFrames[iFrameIndex]->Bitmap(), 
                    								TRect( TPoint(), rect.Size() ),
                    								iFrames[iFrameIndex]->Mask(), EFalse );
                    break;
                case TGifImageControl::ERestoreToBackground:
                    // The new frame is restored to the background color defined in its data.
                    lastFrameBitmapGc->SetBrushColor( 
                    					iDecoder->FrameInfo( iFrameIndex ).iBackgroundColor );
                    lastFrameBitmapGc->Clear();
                    lastFrameMaskGc->SetBrushColor( KRgbBlack );
                    lastFrameMaskGc->Clear();
                    lastFrameBitmapGc->BitBlt( rect.iTl, iFrames[iFrameIndex]->Bitmap(), 
                    						TRect( TPoint(), rect.Size() ) );
                    lastFrameMaskGc->BitBlt( rect.iTl, iFrames[iFrameIndex]->Mask(),
                    						 TRect( TPoint(), rect.Size() ) );
                    break;
                case TGifImageControl::ERestoreToPrevious:
                    // The new frame is exactly the same as the previous one, so no special processing is required.
                    break;
                default:
                    break;
                }
    
            CleanupStack::PopAndDestroy( KFour, lastFrameBitmapDevice );
            }
        
        CreateBitmapCopyL( *iLastFrameBitmap, *iLastFrameMask, iBitmap, iMask );
        }
    else
        {
        User::Leave( KErrNotReady );
        }

    iState = EIRStateScalingBitmap;
    iScaler->Scale( &iStatus, *iBitmap, iTargetSize, iMaintainAspectRatio );
    SetActive();
	IRLOG_DEBUG( "CIRImageConverterImpl::RenderBitmapAnimationFrameL - Exiting" );
    }

// ---------------------------------------------------------------------------
// Notifies the observer either synchronously or asynchronously.
// ---------------------------------------------------------------------------
//
void CIRImageConverterImpl::NotifyObserver( TInt aError, TBool aSynchronous )
    {
    IRRDEBUG2( "CIRImageConverterImpl::NotifyObserver - Entering, aError = %d", aError );
    iError = aError;
    iNotifyObserverCallBack->Cancel();

    if ( aSynchronous )
        {
        StaticNotifyObserverCallBack( this );
        }
    else
        {
        iNotifyObserverCallBack->CallBack();
        }
    IRRDEBUG2( "CIRImageConverterImpl::NotifyObserver - Exiting", KNullDesC );
    }

// ---------------------------------------------------------------------------
// Call back for notifying the observer.
// ---------------------------------------------------------------------------
//
TInt CIRImageConverterImpl::StaticNotifyObserverCallBack( TAny* aSelf )
    {
	IRLOG_DEBUG( "CIRImageConverterImpl::StaticNotifyObserverCallBack - Entering" );
    CIRImageConverterImpl* self = static_cast<CIRImageConverterImpl*>( aSelf );
    if ( self )
        {
        TInt err = self->iError;
        MIRImageConverterObserver::TIRImageConversionEvent event = MIRImageConverterObserver::
        														EIRImageConversionCompleted;
        
        if ( err == KErrNone )
            {
            if ( self->iSvgEngine && self->iIsAnimated && self->iEnableAnimations )
                {
                TRAP( err, self->CreateBitmapCopyL( *self->iBitmap, *self->iMask, 
                		self->iProcessedBitmap, self->iProcessedMask ) )
                }
            else
                {
                delete self->iProcessedBitmap;
                self->iProcessedBitmap = self->iBitmap;
                self->iBitmap = NULL;
                delete self->iProcessedMask;
                self->iProcessedMask = self->iMask;
                self->iMask = NULL;
                }
            
            if ( !err && self->iIsAnimated && self->iEnableAnimations )
                {
                event = MIRImageConverterObserver::EIRFrameConversionCompleted;

                if ( self->iDecoder )
                    {
                    self->iState = EIRStateBetweenBitmapAnimationFrames;
                    
                    self->iFrameTimer->Cancel();
                    self->iFrameTimer->Start( static_cast<TInt>( self->iDecoder->
                    				FrameInfo( self->iFrameIndex ).iDelay.Int64() ), 0, 
                                              TCallBack( StaticNextFrameCallBack, self ) );

                    self->iFrameIndex++;
                    if ( self->iFrameIndex >= self->iDecoder->FrameCount() )
                        {
                        self->iFrameIndex = 0;
                        }
                    }
                }
            }

        if ( err || event == MIRImageConverterObserver::EIRImageConversionCompleted )
            {
            self->Cleanup();
            }
        
        if ( self->iObserver )
            {
            TRAP_IGNORE( self->iObserver->HandleImageConversionEventL( event, self->iId, err ) )
            }
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Call back for advancing to the next frame when bitmap animations are used.
// ---------------------------------------------------------------------------
//
TInt CIRImageConverterImpl::StaticNextFrameCallBack( TAny* aSelf )
    {
    CIRImageConverterImpl* self = static_cast<CIRImageConverterImpl*>( aSelf );
    if ( self )
        {
        self->iFrameTimer->Cancel();
        TRAPD( err, self->RenderBitmapAnimationFrameL() )
        if ( err )
            {
            self->NotifyObserver( err );
            }
        }
    return KErrNone;
    }
