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
* Description:  Control that displays current station information
*
*/


#include <aknsconstants.h>
#include <aknutils.h>
#include <biditext.h>

#include <alf/alfcontrolgroup.h>
#include <alf/alfdecklayout.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alflayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfviewportlayout.h>


//#include "vrdebug.h"
#include "irstationinformationdata.h"
#include "irstationinformationdisplay.h"
#include "irdebug.h"

// Tag to identify a text visual with.
_LIT8( KVRTagTextVisual, "KVRTagTextVisual" );
// Tag to identify a layout visual with.
_LIT8( KVRTagLayout, "KVRTagLayout" );

// Time in milliseconds to spend animating the displays moving in and out of the screen.
const TInt KVRDisplayFadeTime = 400;

// This is context-sensitive per scrollable visual, so the actual start time depends on the additional application logic.
const TInt KVRScrollDelayTime = 1000;
// Time in milliseconds to snooze after scrolling has completed.
// This is used to halt the scrolling at the very end for a period of time, making it visually more pleasing.
const TInt KVRScrollSnoozeTime = 1000;

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRStationInformationDisplay* CIRStationInformationDisplay::NewL( CAlfEnv& aEnv, 
								TInt aControlGroupId,CAlfLayout* aParentLayout )
    {
    
    CIRStationInformationDisplay* self = new ( ELeave ) 
    						CIRStationInformationDisplay( *aParentLayout );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aControlGroupId, aParentLayout );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRStationInformationDisplay::CIRStationInformationDisplay( CAlfLayout& aParentLayout )
    : iParentAnchorLayout( aParentLayout )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::ConstructL( CAlfEnv& aEnv, TInt aControlGroupId, 
												CAlfLayout* aParentLayout )
    {
    CAlfControl::ConstructL( aEnv );

    
    // Base layout is created here so the control can be as independent from the parent layout as possible.
    iBaseLayout = CAlfDeckLayout::AddNewL( *this, aParentLayout );
    
    aEnv.ControlGroup( aControlGroupId ).AppendL( this ); // Takes ownership. No leaving code allowed after this call.
    iLandScapSetFalg=EFalse;    
    
    }

// <TUNING TEMP>
CIRStationInformationDisplay::~CIRStationInformationDisplay()
    {
    Env().CancelCustomCommands( this );
    iRdsDataText.Close();
    }


// ---------------------------------------------------------------------------
// Sets the displayed data.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::SetDisplayedDataL( const CIRStationInformationData& aData )
    {
    FadeOutDisplay( aData );
    CreateDisplayL( aData );
    FadeInDisplay( aData );
    }

// ---------------------------------------------------------------------------
// Sets the RDS data display which is used only in landscape.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::SetDisplayOrientation( TBool aLandscape )
    {
    if ( iLandscape && !aLandscape )
         {
         // Going from landscape to portrait: Remove separate display for RDS data.
        iParentAnchorLayout.Remove( iRdsDataLayout, 0 );
         }
    iLandscape = aLandscape;
    }

TIRFadeStyle  CIRStationInformationDisplay::FadeStyle()
    {
    return iFadeStyle;
    }
// ---------------------------------------------------------------------------
// Sets the RDS data.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::RdsDataReceivedL( const TDesC& aRdsData )
    {

    if ( iRdsData )
        {
        TBool rdsDataExisted = iRdsData->Text().Length() ? ETrue : EFalse;
        TAlfTimedValue opacity( iRdsData->Opacity().ValueNow() );

        if ( aRdsData.Length() )
            {
            if ( rdsDataExisted )
                {
                if( iRdsDataText != aRdsData )
                    {
                iRdsDataText.Close();
                iRdsDataText.CreateL( aRdsData );
                 // Fade out previous text.
                opacity.SetTarget( 0.0f, KVRDisplayFadeTime );
                FadeVisual( *iRdsData, opacity );
                // Displaying RDS text will be started after fade out is done.
                Env().Send( 
                	TAlfCustomEventCommand( EIRCustomEventVisualFadeOutCompleted, this ),
                	 KVRDisplayFadeTime );
                    }

                }
            else
                {
                iRdsDataText.Close();
                iRdsDataText.CreateL( aRdsData );
                iRdsData->SetTextL( iRdsDataText );
                // Start to display RDS text.
                DisplayRdsDataL();
                // Fade in.
                if ( !iLandscape )
                    {
                    // Orientation is portrait.
                    iLayout->SetRowsL( iLayout->RowCount() + 1 );
                    iLayout->Reorder( *iRdsDataLayout, iLayout->RowCount() - 1, 
                    				KVRDisplayFadeTime );
                    iRdsData->SetFlag( EAlfVisualFlagManualPosition );
                    iName->SetFlag( EAlfVisualFlagManualPosition );
                    iLayout->UpdateChildrenLayout( KVRDisplayFadeTime );
                    iRdsData->ClearFlag( EAlfVisualFlagManualPosition );
                    iName->ClearFlag( EAlfVisualFlagManualPosition );
                    }
                opacity.SetTarget( 1.0f, KVRDisplayFadeTime );
                FadeVisual( *iRdsData, opacity );
                }
            }
        else // RDS data has been lost.
            {
            }
        }
    }


// ---------------------------------------------------------------------------
// From class CAlfControl.
// Called when a visual's layout has been updated.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::VisualLayoutUpdated( CAlfVisual& aVisual )
    {
    CAlfControl::VisualLayoutUpdated( aVisual );

    // TO be done  Special handling for scrolling required while changing orientation...
    }

// ---------------------------------------------------------------------------
// From class CAlfControl.
// Event handler.
// ---------------------------------------------------------------------------
//
TBool CIRStationInformationDisplay::OfferEventL( const TAlfEvent& aEvent )
    {
    TBool eventHandled = EFalse;
    
    if ( aEvent.IsCustomEvent() )
        {

        
        switch ( aEvent.CustomParameter() )
            {
            case EIRCustomEventDisplayFadeInCompleted:
                {
                 // When a display has faded in, its position will be handled by the framework and text scrolling should start.
                reinterpret_cast<CAlfGridLayout*>( aEvent.CustomEventData() )->ClearFlag( 
                												EAlfVisualFlagManualPosition );
                // Start to display name.to be changed
                iNameMarqueeClet.StartL( KVRScrollDelayTime, KVRScrollSnoozeTime );
                // Start to display RDS data.
                DisplayRdsDataL();
                eventHandled = ETrue;
                break;
                }
            case EIRCustomEventDisplayFadeOutCompleted:
                {
                // When display has faded out old layout needs to be destroyed
                reinterpret_cast<CAlfGridLayout*>
                			( aEvent.CustomEventData() )->RemoveAndDestroyAllD();
                eventHandled = ETrue;
                break;
                }
             case EIRCustomEventVisualFadeOutCompleted:
                {
                iRdsData->SetTextL( iRdsDataText );
                if ( iRdsDataText.Length() )
                    {
                    // Start to display RDS data.
                    DisplayRdsDataL();
                    // Fade in.
                    TAlfTimedValue opacity( iRdsData->Opacity().ValueNow() );
                    opacity.SetTarget( 1.0f, KVRDisplayFadeTime );
                    FadeVisual( *iRdsData, opacity );
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

// ---------------------------------------------------------------------------
// Creates a new display that is used to show the supplied data.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::CreateDisplayL( const CIRStationInformationData& aData )
    {

    iFadeStyle = aData.FadeDirectionality();
    iLayout = CAlfGridLayout::AddNewL( *this, 1, 1, iBaseLayout );

    // A separate layout has to be created for the station's index and its name as 
    // the index has to be a separate entity from the name due to scrolling issues.
    // This layout will contain both of these texts, but only the name can scroll.
    // Index is never scrolled, and both fields are center-aligned to the display.
    iNameIndexLayout = CAlfGridLayout::AddNewL( *this, 2, 1, iLayout );
    iNameIndexLayout->SetTagL( KVRTagLayout );

    iIndex = CAlfTextVisual::AddNewL( *this, iNameIndexLayout );
    iIndex->SetTextL( aData.Index() );
    iIndex->SetTagL( KVRTagTextVisual );
    iIndex->SetOpacity( 0.0f );
    iIndex->SetColor( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
    iIndex->SetStyle( EAlfTextStyleSmall );
    iIndex->SetAlign( EAlfAlignHLocaleMirrored, EAlfAlignVCenter ); // Right-aligned normally, left-aligned in mirrored.

    CAlfViewportLayout* nameViewport = CAlfViewportLayout::AddNewL( *this, iNameIndexLayout );
    nameViewport->SetFlag( EAlfVisualFlagLayoutUpdateNotification );
    nameViewport->SetTagL( KVRTagLayout );
    
    iName = CAlfTextVisual::AddNewL( *this, nameViewport );
    iName->SetTextL( aData.Name() );
    iName->SetTagL( KVRTagTextVisual );
    iName->SetOpacity( 0.0f );
    iName->SetColor( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
    iName->SetStyle( EAlfTextStyleSmall );
    iName->SetAlign( EAlfAlignHLocale, EAlfAlignVCenter );
    iName->SetWrapping( CAlfTextVisual::ELineWrapTruncate );

    // Forces the viewport marquee to use a linear interpolation style.
    TAlfTimedPoint point( 0.0f, 0.0f );
    point.SetStyle( EAlfTimedValueStyleLinear );
    iName->SetPos( point );


    if ( AknLayoutUtils::LayoutMirrored() ) 
        {
        // In mirrored layouts the name is on the left side.

        iNameIndexLayout->Reorder( *nameViewport, 0, 0 );
        }

    // Decides the correct directionality for the scroll.
    iNameMarqueeClet.Set( *nameViewport, *iName );

// This is added due to possible scrolling issues.
    // Grid layout (iRdsDataLayout) for RDS data is added due to possible scrolling issues.
    // For some reason scrolling won’t work without this extra grid layout.
    if ( iLandscape )
        {
        // Landscape: Create own separate display for RDS data.
        iLandScapSetFalg=ETrue;
        iRdsDataLayout = CAlfGridLayout::AddNewL( *this, 1, 1, &iParentAnchorLayout );
        }
    else
        {
        // Portrait: RDS data will be displayed below the station information.
        iRdsDataLayout = CAlfGridLayout::AddNewL( *this, 1, 1, iLayout );
        }
    iRdsDataLayout->SetTagL( KVRTagLayout );

    // RDS data view port.
    CAlfViewportLayout* rdsDataViewport = CAlfViewportLayout::AddNewL( *this, iRdsDataLayout );
    rdsDataViewport->SetFlag( EAlfVisualFlagLayoutUpdateNotification );
    rdsDataViewport->SetTagL( KVRTagLayout );

    // RDS data text visual.
    iRdsDataText.Close();
    iRdsDataText.CreateL( aData.RdsPsName() );
    iRdsData = CAlfTextVisual::AddNewL( *this, rdsDataViewport );
    iRdsData->SetTextL( iRdsDataText );
    iRdsData->SetTagL( KVRTagTextVisual );
    iRdsData->SetOpacity( 0.0f );
    iRdsData->SetColor( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
    iRdsData->SetStyle( EAlfTextStyleSmall );
    iRdsData->SetWrapping( CAlfTextVisual::ELineWrapManual );
    iRdsData->SetPos( point );

    // Set RDS data.
    iRdsDataMarqueeClet.Set( *rdsDataViewport, *iRdsData );
    // RDS data scrolling style is continuous scrolling.
    iRdsDataMarqueeClet.SetScrollStyle( TIRMarqueeClet::EIRScrollStyleLoop );

    // Determining the initial row count for the grid and reordering its elements.
    TInt rowCount = 1;//1
	if ( iIndex->Text().Length() || iName->Text().Length() )
        {
        //rowCount++;
        }
    if ( iRdsData->Text().Length() && !iLandscape  )
        {
        rowCount++;
        // RDS data will always be the last element in the grid.
        iLayout->Reorder( *iRdsDataLayout, rowCount - 1, 0 );//3rd 0
        }


    iLayout->SetRowsL( rowCount );
    // Calculates the layout for station index and name by using grid weights.
    // Two cases are considered:
    //      1) Index and name fit on the screen without scrolling.
    //         In this case they are center-aligned on the screen.
    //      2) Index and name do not fit on the screen, scrolling must be used.
    //         In this case the index is aligned to either to the left or right,
    //         depending on the used locale, and the name will scroll as a separate
    //         entity.
    
    
    TInt layoutWidth = static_cast<TInt>( iNameIndexLayout->Size().ValueNow().iX );

    TInt indexWidth = iIndex->TextExtents().iWidth;
    TInt nameWidth = iName->TextExtents().iWidth;
    
    if ( layoutWidth < indexWidth + nameWidth )
        {
        nameWidth = layoutWidth - indexWidth;
        }
    else
        {
        TInt overflow = layoutWidth - indexWidth - nameWidth;

        // Overlowing part of the layout is split evenly between the index and the name.
        indexWidth += overflow / 2;
        nameWidth += overflow / 2;
        }
    
    RArray<TInt> weights;
    CleanupClosePushL( weights );
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        weights.AppendL( nameWidth );
        weights.AppendL( indexWidth );
        }
    else
        {
        weights.AppendL( indexWidth );
        weights.AppendL( nameWidth );
        }
    iNameIndexLayout->SetColumnsL( weights );
    CleanupStack::PopAndDestroy( &weights );
    iLayout->UpdateChildrenLayout();

    }

// ---------------------------------------------------------------------------
// Fades in the current display.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::FadeInDisplay( const CIRStationInformationData& aData )
    {
    if ( iLayout )
        {
        TAlfRealPoint size = iLayout->Size().ValueNow();
        TAlfRealPoint pos = iLayout->Pos().ValueNow();
        
        TAlfTimedValue opacity( 0.0f );
        opacity.SetStyle( EAlfTimedValueStyleDecelerate );

        TInt fadeTime = KVRDisplayFadeTime;
        
        TAlfTimedPoint point( pos.iX, pos.iY, KVRDisplayFadeTime );
        point.SetStyle( EAlfTimedValueStyleDecelerate );
        
        switch ( aData.FadeDirectionality() )
            {
            case EIRFadeUnknown:
                point.iX.SetValueNow( pos.iX );
                point.iY.SetValueNow( pos.iY );
                break;
            case EIRFadeLeftToRight:
                point.iX.SetValueNow( pos.iX - size.iX );
                point.iY.SetValueNow( pos.iY );
                break;
            case EIRFadeRightToLeft:
                point.iX.SetValueNow( pos.iX + size.iX );
                point.iY.SetValueNow( pos.iY );
                break;
            case EIRFadeTopToBottom:
                point.iX.SetValueNow( pos.iX );
                point.iY.SetValueNow( pos.iY - size.iY );
                break;
            case EIRFadeBottomToTop:
                point.iX.SetValueNow( pos.iX );
                point.iY.SetValueNow( pos.iY + size.iY );
                break;
            case EIRFadeNoFade:
                fadeTime = 0;
                point.iX.SetValueNow( pos.iX );
                point.iY.SetValueNow( pos.iY );
                point.SetTarget( pos, fadeTime );
                break;
            default:
                break;
            }

        opacity.SetTarget( 1.0f, fadeTime );

        iLayout->SetFlag( EAlfVisualFlagManualPosition );
        iLayout->SetPos( point );
        FadeLayout( *iLayout, opacity );

        if ( iLandscape )
            {
            // If the orientation is landscape the RDS text layout needs to be visible regardless
            // how we got in this point, be it startup, orientation change, skin change or something
            // else. This needs to be done here because RDS text may have already been available
            // and if there is not a change in RDS text then there is no notification to update
            // RDS text which leads to not showing the available RDS text
            Env().Send( TAlfCustomEventCommand( EIRCustomEventVisualFadeOutCompleted, this ), 0 );
            }
        Env().Send( TAlfCustomEventCommand( EIRCustomEventDisplayFadeInCompleted, this,
        			 reinterpret_cast<TInt>( iLayout ) ), 0 );
        }
    }

// ---------------------------------------------------------------------------
// Fades out the current display.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::FadeOutDisplay( const CIRStationInformationData& aData )
    {
    if ( iLayout )
        {
        TAlfRealPoint size = iLayout->Size().ValueNow();
        TAlfRealPoint pos = iLayout->Pos().ValueNow();

        TAlfTimedValue opacity( iLayout->Opacity().ValueNow() );
        opacity.SetStyle( EAlfTimedValueStyleAccelerate );
        opacity.SetTarget( 0.0f, KVRDisplayFadeTime );
        
        TAlfTimedPoint point( pos.iX, pos.iY );
        point.SetStyle( EAlfTimedValueStyleAccelerate );

        TAlfRealPoint target = iLayout->Pos().Target();

        TInt fadeTime = KVRDisplayFadeTime;
        
        switch ( aData.FadeDirectionality() )
            {
            case EIRFadeUnknown:
                point.SetTarget( TAlfRealPoint( target.iX, target.iY ), KVRDisplayFadeTime );
                break;
            case EIRFadeLeftToRight:
                point.SetTarget( TAlfRealPoint( target.iX + size.iX, target.iY ),
                				 KVRDisplayFadeTime );
                break;
            case EIRFadeRightToLeft:
                point.SetTarget( TAlfRealPoint( -size.iX, target.iY ), KVRDisplayFadeTime );
                break;
            case EIRFadeTopToBottom:
                point.SetTarget( TAlfRealPoint( target.iX, target.iY + size.iY ),
                								 KVRDisplayFadeTime );
                break;
            case EIRFadeBottomToTop:
                point.SetTarget( TAlfRealPoint( target.iX, target.iY - size.iY ), 
                								KVRDisplayFadeTime );
                break;
            case EIRFadeNoFade:
                fadeTime = 0;
                point.SetTarget( TAlfRealPoint( target.iX, target.iY ), fadeTime );
                opacity.SetTarget( 0.0f, fadeTime );
                break;
            default:
                break;
            }

        iLayout->SetFlag( EAlfVisualFlagManualPosition );
        iLayout->SetPos( point );
        FadeLayout( *iLayout, opacity );

        Env().CancelCustomCommands( this, EIRCustomEventDisplayFadeInCompleted );
        if ( iRdsDataLayout && iLayout != iRdsDataLayout->Layout() )
            {

            // In landscape orientation RDS data is not part of the station information data and no fading out is needed.
            // RDS data can be removed and destroyed at once.
            Env().Send( TAlfCustomEventCommand( EIRCustomEventDisplayFadeOutCompleted, this,
            			 reinterpret_cast<TInt>( iRdsDataLayout ) ), 0 );

            }
        Env().Send( TAlfCustomEventCommand( EIRCustomEventDisplayFadeOutCompleted, this,
        			 reinterpret_cast<TInt>( iLayout ) ), fadeTime );
        }

    // Saved pointers to the visuals will later on be invalid, as they will get deleted in the custom event handler.
    
    iLayout = NULL;
    iNameIndexLayout = NULL;
    iRdsDataLayout = NULL;
    iName = NULL;
    iIndex = NULL;

    iRdsData = NULL;
    }

// ---------------------------------------------------------------------------
// Fades a single visual.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::FadeVisual( CAlfVisual& aVisual,const TAlfTimedValue& aOpacity)
    {
    TAlfTimedValue opacity( aVisual.Opacity().ValueNow() );
    opacity.SetTarget( aOpacity.Target(), aOpacity.TimeToTargetinMilliSeconds() );
    
    if ( aVisual.Tag() == KVRTagTextVisual )
        {
        CAlfTextVisual& visual = static_cast<CAlfTextVisual&>( aVisual );
        if ( visual.Text().Length() )
            {
            visual.SetOpacity( opacity );
            visual.SetShadowOpacity( opacity );
            }
        }
    else
        {
        aVisual.SetOpacity( opacity );
        }
    }

// ---------------------------------------------------------------------------
// Fades all visuals contained within the supplied layout.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::FadeLayout( CAlfLayout& aLayout, const TAlfTimedValue& aOpacity)
    {
    for ( TInt i = 0; i < aLayout.Count(); i++ )
        {
        CAlfVisual& visual = aLayout.Visual( i );
        if ( visual.Tag() == KVRTagLayout )
            {
            FadeLayout( static_cast<CAlfLayout&>( visual ), aOpacity );
            }
        else
            {
            FadeVisual( visual, aOpacity );
            }
        }
        
    }



// ---------------------------------------------------------------------------
// Starts to display RDS data.
// ---------------------------------------------------------------------------
//
void CIRStationInformationDisplay::DisplayRdsDataL()
    {
    // Set default alignment.
    if ( iLandscape )
        {
        //iRdsData->SetAlign( EAlfAlignHLocale, EAlfAlignVCenter )
        iRdsData->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );

        }
    else
        {
        iRdsData->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
        }
    // Start displaying.
    iRdsDataMarqueeClet.StartL( KVRScrollDelayTime + KVRDisplayFadeTime);
    }
