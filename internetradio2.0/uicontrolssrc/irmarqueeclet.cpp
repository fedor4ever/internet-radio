/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Controlet to implement marquee functionality for visuals
*
*/


#include <alf/alfcontrol.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alfviewportlayout.h>
#include <alf/alftimedvalue.h>
#include <alf/alftextvisual.h>

#include "irmarqueeclet.h"
 
// Default scroll style.
const TIRMarqueeClet::TIRScrollStyle KVRDefaultScrollStyle = TIRMarqueeClet::EIRScrollStyleOnce;
// Default scroll direction.
const TIRMarqueeClet::TIRScrollDirection KVRDefaultScrollDirection = 
										TIRMarqueeClet::EIRScrollDirectionLeft;
// Default scroll speed in pixels per second.
const TInt KVRDefaultScrollSpeed = 30;
//Default Scroll Time
const TInt KVRScrollTime=1000;
// Spaces needed when creating continuously scrolling text. 
_LIT( KVRSpace, "     " );

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
TIRMarqueeClet::TIRMarqueeClet() :
    iViewport( NULL ), iTextVisual( NULL ), iScrollSize(), 
    iScrollSpeed( KVRDefaultScrollSpeed ), iScrollDirection( KVRDefaultScrollDirection ), 
    iScrollStyle( KVRDefaultScrollStyle ), iSnoozeTime( 0 ), iOrigin(), iOriginalTextWidth( 0 )
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
TIRMarqueeClet::~TIRMarqueeClet()
    {
    if ( iTextVisual )
        {
        iTextVisual->Owner().Env().CancelCustomCommands( this );
        }
    }

// ---------------------------------------------------------------------------
// Sets the required information for this marquee controlet.
// ---------------------------------------------------------------------------
//
void TIRMarqueeClet::Set( CAlfViewportLayout& aViewport, CAlfTextVisual& aTextVisual )
    {
    aTextVisual.Owner().Env().CancelCustomCommands( this );

    iViewport = &aViewport;
    iTextVisual = &aTextVisual;
    }

// ---------------------------------------------------------------------------
// Sets scroll speed in pixels per second.
// ---------------------------------------------------------------------------
//
void TIRMarqueeClet::SetScrollSpeed( TInt aScrollSpeed )
    {
    iScrollSpeed = aScrollSpeed;
    }

// ---------------------------------------------------------------------------
// Sets the scroll direction.
// ---------------------------------------------------------------------------
//
void TIRMarqueeClet::SetScrollDirection( TIRScrollDirection aScrollDirection )
    {
    iScrollDirection = aScrollDirection; 
    }

// ---------------------------------------------------------------------------
// Sets the scroll style.
// ---------------------------------------------------------------------------
//
void TIRMarqueeClet::SetScrollStyle( TIRScrollStyle aScrollStyle )
    {
    iScrollStyle = aScrollStyle;
    }

// ---------------------------------------------------------------------------
// Starts scrolling.
// ---------------------------------------------------------------------------
//
void TIRMarqueeClet::StartL( TInt aDelay, TInt aSnooze )
    {
    ASSERT( iViewport );
    ASSERT( iTextVisual );

    iViewport->SetViewportSize( TAlfRealPoint( 1.0f, 1.0f ), 0 );
    iViewport->SetVirtualSize( TAlfRealPoint( 1.0f, 1.0f ), 0 );
    iViewport->SetViewportPos( TAlfRealPoint( 0.0f, 0.0f ), 0 );

    iTextVisual->Owner().Env().CancelCustomCommands( this );

    iSnoozeTime = aSnooze;

    // Get original text width in pixels.
    iOriginalTextWidth = iTextVisual->TextExtents().iWidth;
    TInt targetWidth = iViewport->Size().Target().AsSize().iWidth;

    // Check if loop scrolling and if the text does not fit to the viewport size.
    if ( iScrollStyle == TIRMarqueeClet::EIRScrollStyleLoop && iOriginalTextWidth > targetWidth )
        {
        // Create continuously scrolling text (text + 5 spaces + text).
        RBuf buffer;
        CleanupClosePushL( buffer );
        buffer.CreateL( 2 * iOriginalTextWidth + KVRSpace().Length() );
        buffer.Append( iTextVisual->Text() );
        buffer.Append( KVRSpace );
        buffer.Append( iTextVisual->Text() );
        iTextVisual->SetTextL( buffer );
        CleanupStack::PopAndDestroy(&buffer);// buffer
        }
    
    // Text size to display.
    iScrollSize = iTextVisual->TextExtents();

    // Check if text must be scrolled.
    if ( iScrollSize.iWidth > targetWidth )
        {
        // Set line wrapping to truncate: if text does not fit to the viewport width, the text is
        // truncated with ellipsis (...).
        iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
        // Decides the correct directionality for the scroll.
        TBidiText::TDirectionality directionality = TBidiText::
        					TextDirectionality( iTextVisual->Text() );
        if ( directionality == TBidiText::ELeftToRight )
            {
            SetScrollDirection( TIRMarqueeClet::EIRScrollDirectionLeft ); 
            }
        else
            {
            SetScrollDirection( TIRMarqueeClet::EIRScrollDirectionRight ); 
            }
        // Align horizontally.
        iTextVisual->SetAlign( EAlfAlignHLocale, EAlfAlignVCenter );
        // Start scrolling.
        iTextVisual->Owner().Env().Send( TAlfCustomEventCommand( 
            TIRMarqueeClet::EIRCustomEventScrollStarted, this ), aDelay );
        }
    }

// ---------------------------------------------------------------------------
// From class CAlfControl.
// Event handler.
// ---------------------------------------------------------------------------
//
TBool TIRMarqueeClet::OfferEventL( const TAlfEvent& aEvent )
    {
    TBool eventHandled = EFalse;
    
    if ( aEvent.IsCustomEvent() && iViewport )
        {
        switch ( aEvent.CustomParameter() )
            {
            case TIRMarqueeClet::EIRCustomEventScrollCompleted:
                {
                // Viewport is reset.
                iViewport->SetVirtualSize( TAlfRealPoint( 1.0f, 1.0f ), 0 );
                iViewport->SetViewportPos( TAlfRealPoint( 0.0f, 0.0f ), 0 );
                // Set line wrapping to truncate.
                iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
                eventHandled = ETrue;
                break;
                }
            case TIRMarqueeClet::EIRCustomEventScrollStarted:
                {
                 // Visible scroll space width.
                TReal32 visibleWidth = iViewport->Size().Target().iX;
 
                // One scroll width.
                TReal32 overflowWidth( 0.0f );
                switch ( iScrollStyle )
                    {
                    case TIRMarqueeClet::EIRScrollStyleOnce:
                        overflowWidth = iScrollSize.iWidth - visibleWidth;
                        break;
                    case TIRMarqueeClet::EIRScrollStyleLoop:
                        // In EIRScrollStyleLoop mode, the scrolling width is 
                        // the width of the original text + the trailing spaces.
                        overflowWidth = iScrollSize.iWidth - iOriginalTextWidth;
                        break;
                    default:
                        break;
                    }

                // Width ratio.
                TReal32 widthRatio = overflowWidth / visibleWidth;
                // Target position.
                TAlfRealPoint target( iViewport->Pos().Target() );
                // Coordinate space used for scrolling.
                TAlfRealPoint virt( widthRatio + 1.0f, 1.0f );
                // Start position.
                iOrigin = iViewport->Pos().ValueNow();
                // Scrolling time in milliseconds.
                TInt scrollTime = KVRScrollTime *
                				 static_cast<TInt>( overflowWidth / iScrollSpeed );
                
                switch ( iScrollDirection )
                    {
                    case TIRMarqueeClet::EIRScrollDirectionLeft:
                        iOrigin.iX = 0.0f;
                        target.iX = widthRatio;
                        break;
                    case TIRMarqueeClet::EIRScrollDirectionRight:
                        iOrigin.iX = widthRatio;
                        target.iX = 0.0f;
                        break;
                    default:
                        break;
                    }

                // Set virtual size for scrolling.
                iViewport->SetVirtualSize( virt, 0 );
                // Set viewport to start position.
                iViewport->SetViewportPos( iOrigin, 0 );
                // Start scrolling to target position.
                iViewport->SetViewportPos( target, scrollTime );

                // Set line wrapping to manual.
                iTextVisual->SetWrapping( CAlfTextVisual::ELineWrapManual );

                switch ( iScrollStyle )
                    {
                    case TIRMarqueeClet::EIRScrollStyleOnce:
                        // Send EIRCustomEventScrollCompleted after scrolling is ended.
                        iTextVisual->Owner().Env().Send( TAlfCustomEventCommand(
                            TIRMarqueeClet::EIRCustomEventScrollCompleted, this ), 
                            scrollTime + iSnoozeTime );
                        break;
                    case TIRMarqueeClet::EIRScrollStyleLoop:
                        // Start loop scrolling immediately after previous scrolling has ended.
                        iTextVisual->Owner().Env().Send( TAlfCustomEventCommand(
                            TIRMarqueeClet::EIRCustomEventScrollStarted, this ), scrollTime );
                        break;
                    default:
                        break;
                    }

                eventHandled = ETrue;
                break;
                }
             default:
                break;
            }
        }


    return eventHandled;
    }

// End of file
