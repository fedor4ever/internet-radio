/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/
#include <aknutils.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdecklayout.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alfimagevisual.h>
#include <alf/alftextvisual.h>
#include <alf/alfvisual.h>
#include <alf/alftimedvalue.h>
#include <alf/alfdisplay.h>
#include <data_caging_path_literals.hrh>
#include <touchfeedback.h>


#include <internetradio.rsg>
#include <alf/alfimagevisual.h>
#include <aknview.h>
#include <aknview.h>
#include <aknvolumepopup.h>
#include <stringloader.h>
#include <e32des8.h>
#include "internetradio.mbg"
#include "irui.h"
#include "ir.hrh"
#include "ircommon.h"
#include "irdebug.h"
#include "irnowplayingviewcontainer.h"
#include "irnowplayingview.h"
#include "irnowplayingwrapper.h"
#include "irfavoritesdb.h"
#include "irstationlogocontrol.h"
#include "irstationinformationdisplay.h"
#include "irradioidle.h"

#ifdef MUL_USE_COMMON_COMPONENTS
using namespace GestureHelper;
/** The base value for the control group id needed when constructing gesture control. */
const TInt KIRFreeControlGroupIdBase = 10;
#endif
const TInt KControl1 =0;



// ---------------------------------------------------------------------------
// CIRNowPlayingViewContainer::CIRNowPlayingViewContainer()
// Default constructor
// ---------------------------------------------------------------------------
//
CIRNowPlayingViewContainer::CIRNowPlayingViewContainer(CAlfEnv& aEnv ):iAlfEnv (aEnv)
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::CIRNowPlayingViewContainer" );
    }

// ---------------------------------------------------------------------------
// CIRNowPlayingViewContainer* CIRNowPlayingViewContainer::NewL( CAlfEnv& aEnv )
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRNowPlayingViewContainer* CIRNowPlayingViewContainer::NewL( CAlfEnv& aEnv,
				CIRNowPlayingView* aInstance)
{
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::NewL" );
	CIRNowPlayingViewContainer* self = CIRNowPlayingViewContainer::NewLC(aEnv,aInstance);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::NewL - Exiting." );
	return self;

}

// ---------------------------------------------------------------------------
// CIRNowPlayingViewContainer* CIRNowPlayingViewContainer::NewL( CAlfEnv& aEnv )
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRNowPlayingViewContainer* CIRNowPlayingViewContainer::NewLC( CAlfEnv& aEnv,
			CIRNowPlayingView* aInstance)
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::NewLC - Entering" );
    CIRNowPlayingViewContainer* self = new( ELeave ) CIRNowPlayingViewContainer(aEnv);
    CleanupStack::PushL( self );
    self->ConstructL(aInstance);
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::NewLC - Exiting" );
    return self;
    }



// ---------------------------------------------------------------------------
// void CIRNowPlayingViewContainer::ConstructL( CAlfEnv& aEnv )
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::ConstructL( CIRNowPlayingView* aInstance )
	{
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::ConstructL - Entering" );
	iNowPlayingView=aInstance;
	#ifdef MUL_USE_COMMON_COMPONENTS
	    // Control group id that client must not use.
	 	const TInt freeControlGroupId  = KIRFreeControlGroupIdBase + KIRMaximumViewId.iUid;
	    CGestureControl::ConstructL( *this, iAlfEnv, iAlfEnv.PrimaryDisplay(), freeControlGroupId );
	#else
	    CAlfControl::ConstructL( iAlfEnv );
	#endif
	// Create Main View
	iLayout = CAlfAnchorLayout::AddNewL( *this );
	LayoutControl();
	iByteCounter=CAlfTextVisual::AddNewL( *this, iLayout);
	iBitrate = CAlfTextVisual::AddNewL( *this, iLayout);
	CAlfDeckLayout* deck = CAlfDeckLayout::AddNewL( *this, iLayout );
	iLogoControl = CIRStationLogoControl::NewL( Env(), KIRNowPlayingViewID.iUid, deck );
	iRadioIdle = CIRRadioIdle::NewL( iAlfEnv, KIRNowPlayingViewID.iUid, deck );
		// Update radio idle skin color
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( skin )
    	{
    	TRgb textColor; // text color when not highlighted
    	AknsUtils::GetCachedColor( skin, textColor, KAknsIIDQsnTextColors, 
    								EAknsCIQsnTextColorsCG6 );
    	TRgb highlightColor; // text color when highlighted
    	AknsUtils::GetCachedColor( skin, highlightColor, KAknsIIDQsnTextColors,
    								 EAknsCIQsnTextColorsCG10 );
    	iRadioIdle->SetPrimaryColor( textColor );
    	iRadioIdle->SetSecondaryColor( highlightColor );
    	}
	iStationInformationDisplay = CIRStationInformationDisplay::NewL( iAlfEnv, 
					KIRNowPlayingViewID.iUid,iLayout);
	if (iNowPlayingView->iUi->IsLandscapeOrientation() )
        {
        // Set display orientation.
        iStationInformationDisplay->SetDisplayOrientation( ETrue );
        }
	iAlfEnv.ControlGroup( KIRNowPlayingViewID.iUid ).AppendL( this ); // Transfers ownership. No leaving code must be run after this call.
    iFeedback = MTouchFeedback::Instance();
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::ConstructL - Exiting" );
	}



// ---------------------------------------------------------------------------
// CIRNowPlayingViewContainer::~CIRNowPlayingViewContainer()
// Destructor.
// ---------------------------------------------------------------------------
//
CIRNowPlayingViewContainer::~CIRNowPlayingViewContainer()
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::~CIRNowPlayingViewContainer - Entering" );
    iLogoControl = NULL;
	iLayout = NULL;
	iStationInformationDisplay = NULL;
	iBitrate = NULL;
	iByteCounter = NULL;
	iNowPlayingView = NULL;
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::~CIRNowPlayingViewContainer - Exiting" );
    }

// ---------------------------------------------------------------------------
// Returns a reference to the station logo control.
// ---------------------------------------------------------------------------
//
CIRStationLogoControl& CIRNowPlayingViewContainer::StationLogoControl()
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::StationLogoControl" );
    return *iLogoControl;
    }

// ---------------------------------------------------------------------------
// Returns a reference to the radio idle control.
// ---------------------------------------------------------------------------
//
CIRRadioIdle& CIRNowPlayingViewContainer::RadioIdle()
    {
    return *iRadioIdle;
    }
#ifdef MUL_USE_COMMON_COMPONENTS
// ---------------------------------------------------------------------------
// From class MGestureObserver.
// Called when user makes gestures.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::HandleGestureL( const MGestureEvent& aEvent )
    {
    IRLOG_DEBUG(" *** S60VisualRadio -- CVRNowPlayingContainer::HandleGestureL ");
    TGestureCode eventCode( aEvent.Code( MGestureEvent::EAxisBoth ) );
    switch ( eventCode )
        {
        case GestureHelper::EGestureStart:
            {
            if ( iFeedback )
                {
                // Give feedback when gesture starts.
                iFeedback->InstantFeedback( ETouchFeedbackBasic );
                }
            break;
            }
        case GestureHelper::EGestureTap:
            {
            iNowPlayingView->HandleCommandL(EIRCmdVolumeControl);
            break;
            }
        case GestureHelper::EGestureSwipeLeft:
            {
            iNowPlayingView->HandleCommandL( EIRCmdStepPrevPreset );
            break;
            }
        case GestureHelper::EGestureSwipeRight:
            {
            iNowPlayingView->HandleCommandL( EIRCmdStepNextPreset );
            break;
            }
        default:
            break;
        }
    }
#else


// ---------------------------------------------------------------------------
// From class CAlfControl.
// Called when an event occurs.
// ---------------------------------------------------------------------------
//
TBool CIRNowPlayingViewContainer::OfferEventL( const TAlfEvent& aEvent )
	{
    TBool eventHandled = EFalse;
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::OfferEventL - Entering" );
	if(aEvent.IsPointerEvent())
		{
		if ( iFeedback )
			{	
			iFeedback->InstantFeedback( ETouchFeedbackBasic );
			}
		TriggerCommandL(EIRCmdVolumeControl);
        eventHandled = ETrue;
		}
	else if(aEvent.IsKeyEvent() )
		{
		const TKeyEvent& kEvent = aEvent.KeyEvent();
		KeyHitL( kEvent.iScanCode );
		}
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::OfferEventL - Exiting" );
    return eventHandled;
	}
#endif
// ---------------------------------------------------------------------------
// Returns a reference to the station information display.
// ---------------------------------------------------------------------------
//
CIRStationInformationDisplay& CIRNowPlayingViewContainer::StationInformationDisplay()
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::StationInformationDisplay " );
    return *iStationInformationDisplay;
    }
// ---------------------------------------------------------------------------
// void CIRNowPlayingViewContainer::SetRdsTextL( const TDesC& aText )
// Set the text in the visual.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::SetBitrateL( const TDesC& aText )
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::SetBitrateL - Entering" );
    iBitrate->SetColor( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
    iBitrate->SetStyle( EAlfTextStyleSmall );
    iBitrate->SetTextL( aText );
    iBitrate->SetWrapping( CAlfTextVisual::ELineWrapManual );
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::SetBitrateL - Exiting" );
    }

// ---------------------------------------------------------------------------
// void CIRNowPlayingViewContainer::SetRdsTextL1( const TDesC& aText )
// Set the text in the visual.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::SetByteCounterL( const TDesC& aText )
	{
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::SetByteCounterL - Entering" );
	iByteCounter->SetColor( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
	iByteCounter->SetStyle( EAlfTextStyleSmall );
	iByteCounter->SetTextL( aText );
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::SetByteCounterL - Exiting" );
	}
// ---------------------------------------------------------------------------
// CIRNowPlayingViewContainer::KeyHit()
// Loads the fonts required to draw this view
// ---------------------------------------------------------------------------
//

TBool CIRNowPlayingViewContainer::KeyHitL(  TInt aScanCode 
	                  /*TVRRockerButtonHitTestType aType */)
	{
     IRLOG_DEBUG( "CIRNowPlayingViewContainer::KeyHit - Entering" );
	TBool ret = EFalse;
	switch ( aScanCode )
		{
		case EStdKeyRightArrow:
			{
			TriggerCommandL(EIRCmdStepNextPreset);
			}
		break;
		case EStdKeyLeftArrow:
			{
			TriggerCommandL(EIRCmdStepPrevPreset);
			}
		break;
		case EStdKeyDevice3:
			{
			if(iNowPlayingView->iNowPlayingWrapper->iPlaying)
				{
				TriggerCommandL(EIRCmdStop);
				}
			else
				{
				TriggerCommandL(EIRCmdPlay);
				}
			}
		break;
		case EStdKeyUpArrow:
		break;
		case EStdKeyDownArrow:
		break;
		default:
		break;
		}
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::KeyHit - Exiting" );
	return ret;
	}
// ---------------------------------------------------------------------------
// Layouts this control.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::LayoutControl()
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::LayoutControl - Entering" );
     TInt ordinal = KControl1;
	 TInt resId ;
    
    
    //Set anchor for ByteCounter
    resId = iNowPlayingView->iUi->ChooseLayoutResource( 
    									 R_IR_NOWPLAYING_VIEW_BYTECOUNTER_LAYOUT, 
                                         R_IR_NOWPLAYING_VIEW_BYTECOUNTER_LAYOUT_MIRRORED, 
                                         R_IR_NOWPLAYING_VIEW_BYTECOUNTER_LAYOUT_LANDSCAPE, 
                                 R_IR_NOWPLAYING_VIEW_BYTECOUNTER_LAYOUT_LANDSCAPE_MIRRORED );
    SetLayoutAnchor( ordinal, resId );
    ordinal++;

    //Set anchor for the BitRate
     resId = iNowPlayingView->iUi->ChooseLayoutResource( 
        								 R_IR_NOWPLAYING_VIEW_BITRATE_LAYOUT, 
                                         R_IR_NOWPLAYING_VIEW_BITRATE_LAYOUT_MIRRORED, 
                                         R_IR_NOWPLAYING_VIEW_BITRATE_LAYOUT_LANDSCAPE, 
                                 R_IR_NOWPLAYING_VIEW_BITRATE_LAYOUT_LANDSCAPE_MIRRORED );
    SetLayoutAnchor( ordinal, resId );  
     ordinal++;
                                
    // Set anchor for station logo.
    resId = iNowPlayingView->iUi->ChooseLayoutResource( 
    									 R_IR_NOWPLAYING_VIEW_LOGO_BITMAP_LAYOUT, 
                                         R_IR_NOWPLAYING_VIEW_LOGO_BITMAP_LAYOUT_MIRRORED, 
                                         R_IR_NOWPLAYING_VIEW_LOGO_BITMAP_LAYOUT_LANDSCAPE, 
                                 R_IR_NOWPLAYING_VIEW_LOGO_BITMAP_LAYOUT_LANDSCAPE_MIRRORED );
    SetLayoutAnchor( ordinal, resId );
     ordinal++;

    // Set anchor for station information.
    resId = iNowPlayingView->iUi->ChooseLayoutResource(
    									 R_IR_NOWPLAYING_VIEW_STATION_INFORMATION_LAYOUT, 
                                         R_IR_NOWPLAYING_VIEW_STATION_INFORMATION_LAYOUT_MIRRORED, 
                           R_IR_NOWPLAYING_VIEW_STATION_INFORMATION_LAYOUT_LANDSCAPE,
                           R_IR_NOWPLAYING_VIEW_STATION_INFORMATION_LAYOUT_LANDSCAPE_MIRRORED );
    SetLayoutAnchor( ordinal, resId );
    ordinal++;
    
    if ( iNowPlayingView->iUi->IsLandscapeOrientation() )
        {
        // Set anchor for RDS data on landscape.
        resId = R_IR_NOWPLAYING_VIEW_METADATA_LAYOUT_LANDSCAPE;
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            resId = R_IR_NOWPLAYING_VIEW_METADATA_LAYOUT_LANDSCAPE_MIRRORED;    
            }
        SetLayoutAnchor( ordinal, resId );
        ordinal++;
        } 
    else
	    {
        // If RDS data is on separate layout then remove it 
        iLayout->Detach( ordinal );
	    }
    if ( iStationInformationDisplay )
        {
        iStationInformationDisplay->SetDisplayOrientation( 
        			iNowPlayingView->iUi->IsLandscapeOrientation() );
        }
    iLayout->UpdateChildrenLayout();

    IRLOG_DEBUG( "CIRNowPlayingViewContainer::LayoutControl - Exiting" );
    }

// ---------------------------------------------------------------------------
// Sets layout anchor for child visual.
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::SetLayoutAnchor( TInt aOrdinal, TInt aResourceId )
    {
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::SetLayoutAnchor - Entering" );

    // Read LAFs.
    TSize size = iAlfEnv.PrimaryDisplay().VisibleArea().Size();
    TRect rect( size );
    
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( rect, aResourceId );
    
    // Set top/left anchor.
    iLayout->Attach( aOrdinal, 
                     EAlfAnchorTypeTopLeft,
                     TAlfXYMetric( TAlfMetric( layoutRect.Rect().iTl.iX ),
                     TAlfMetric( layoutRect.Rect().iTl.iY ) ),
                     EAlfAnchorAttachmentOriginTopLeft );

    // Set bottom/right anchor.
    iLayout->Attach( aOrdinal, 
                     EAlfAnchorTypeBottomRight, 
                     TAlfXYMetric( TAlfMetric( layoutRect.Rect().iBr.iX ),
                     TAlfMetric( layoutRect.Rect().iBr.iY ) ),
                     EAlfAnchorAttachmentOriginTopLeft );
    IRLOG_DEBUG( "CIRNowPlayingViewContainer::SetLayoutAnchor - Exiting" );
    }

// ---------------------------------------------------------------------------
// Triggers the command to view handling
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::TriggerCommandL( TInt iCommandId)
	{
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::TriggerCommandL - Entering" );
	TVwsViewId viewId( KNullUid, KNullUid );
	CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
	TInt err = appUi->GetActiveViewId( viewId );
	RProcess process;
	TSecureId id = process.SecureId();
	// TO be DOne GetActiveViewId doesn't always work (when switched back to VRA through RadioLauncher icon
	if( !err && viewId.iAppUid.iUid == id.iId )
		{
		CAknView* view = appUi->View( viewId.iViewUid );
		if( view )
			{
			view->ProcessCommandL( iCommandId );
			}
		}
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::TriggerCommandL - Exiting" );
	}
//Calls the Views Draw Function
// ---------------------------------------------------------------------------
//
void CIRNowPlayingViewContainer::DrawViewL( TBool aStationHasLogo)
	{
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::TriggerCommandL - Entering" );
	iNowPlayingView->DrawViewL(aStationHasLogo);
	IRLOG_DEBUG( "CIRNowPlayingViewContainer::TriggerCommandL - Entering" );
	}
