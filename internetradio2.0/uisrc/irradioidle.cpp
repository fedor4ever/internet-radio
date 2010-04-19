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
/*
* ============================================================================
*  Name        : radioidle.cpp
*  Part of     :
*  Description :
*  Version     :
*
* ==============================================================================
*/

// INCLUDE FILES

#include <eikenv.h>
#include <stringloader.h>

#include <alf/alfcontrolgroup.h>
#include <alf/alfenv.h>
#include <alf/alftextvisual.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimagebrush.h>
#include <alf/alftexturemanager.h>
#include <alf/alfdisplay.h>
#include <alf/alfviewportlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alfutil.h>
#include <alf/alfTextStyle.h>
#include <aknsitemid.h>

#include <e32math.h>
#include <data_caging_path_literals.hrh>
#include "irradioidle.h"
#include "irdebug.h"
// CONSTANTS

// bitmap file for the background of the display
_LIT8( KRadioIdleAnchorTag, "radioIdleAnchor" );
_LIT8( KRadioIdleFlowLayoutTag, "radioIdleFlowLayout" );
_LIT8( KRadioIdleBackgroundTextTag, "radioIdleBackgroundText" );
_LIT8( KRadioIdleItemTag, "radioIdleItem" );

const TInt KSizeOfRadioIdleItemBigFont = 400; // background text
const TInt KSizeOfRadioIdleItemMediumFont = 60;
const TInt KSizeOfRadioIdleItemSmallFont = 30;

const TInt KRadioIdleFadeOutDurationTime = 400;
const TInt KRadioIdleFadeInDurationTime = 0; 

const TInt  KRadioIdleBackgroundTextScrollInterval = 12000;
const TReal32 KRadioIdleBackgroundTextOpacity = 0.2f;
const TInt  KRadioIdleBackgroundTextSideToSideTransitionDurationTime = 25000;

const TInt KMaxNumberOfRadioIdleItems = 5;

const TReal32 KMaxRadioIdleItemsOverlap = 0.2f;
const TInt KMaxRadioIdleItemScrollDurationTime = 35000;
const TInt KMinRadioIdleItemScrollDurationTime = 25000;

const TInt KModeChangingFactor = 3;

const TReal32 KMaxOpacity = 0.7f;
const TReal32 KMinOpacity = 0.3f;

const TInt KMaxRadioIdleItemColorDeviation = 30; // must be <= 255
const TRgb KRadioIdleItemDefaultColor(128,128,128);
const TRgb KRadioIdleItemBackgroundColor(255, 255, 255);
const TReal32 KRadioIdleItemBackgroundOpacity = 0.5f;
const TReal32 KRadioIdleBackgroundOpacity = 0.4f;

const TInt KRadioIdleItemArrayGranularity = 4;
const TInt Kone=1;
const TInt KTwoFiftyFive=255;
const TInt KZero=0;
const TInt KTen=10;
const TInt KThousand=1000;
//const TInt KThree=3

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CIRRadioIdle::NewL
// Two-phase constructor of CIRRadioIdle
// ---------------------------------------------------------------------------
//
CIRRadioIdle* CIRRadioIdle::NewL( CAlfEnv& aEnv, TInt aControlGroupId, CAlfLayout* aParentLayout )
	{
	CIRRadioIdle* self = new (ELeave) CIRRadioIdle();
	CleanupStack::PushL(self);
	self->ConstructL( aEnv, aControlGroupId, aParentLayout );
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::CIRRadioIdle
// Default constructor
// ----------------------------------------------------------------------------
//
CIRRadioIdle::CIRRadioIdle()
:   iOrientation ( EOrientationNone ),
	iFadeStyle( EFadeStyleNone ),
	iRadioIdleState( EStopped ),
	iRadioIdleMode( EBackgroundTextAndRadioIdleItems ),
	iRadioIdleRequestedMode( EBackgroundTextAndRadioIdleItems ),
    iRadioIdlePrimaryColor(KRadioIdleItemDefaultColor),
    iRadioIdleSecondaryColor( KRadioIdleItemDefaultColor )
	{
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::ConstructL( CAlfEnv& aEnv, TInt aControlGroupId, CAlfLayout* aParentLayout )
	{
	CAlfControl::ConstructL( aEnv );

	iParentLayout = aParentLayout;

    // Create largest font for the background text of radio idle
   	CAlfTextStyleManager& manager = Env().TextStyleManager();
	iRadioIdleItemBackgroundTextStyleId = manager.CreatePlatformTextStyleL();
	CAlfTextStyle* radioIdleItemStyle = manager.TextStyle( iRadioIdleItemBackgroundTextStyleId );
	radioIdleItemStyle->SetTextSizeInPixels( KSizeOfRadioIdleItemBigFont, ETrue );

	 // Create text styles for other radio idle items
    RArray<TInt> fontSizeArray;
    CleanupClosePushL( fontSizeArray );
    User::LeaveIfError( fontSizeArray.Append( KSizeOfRadioIdleItemMediumFont ) );
    User::LeaveIfError( fontSizeArray.Append( KSizeOfRadioIdleItemSmallFont ) );
    fontSizeArray.Sort();

	TInt numberOfTextStyles = fontSizeArray.Count();
	for( TInt i = 0; i < numberOfTextStyles; i++ )
		{
		TInt radioIdleItemTextStyleId = manager.CreatePlatformTextStyleL();
		radioIdleItemStyle = manager.TextStyle( radioIdleItemTextStyleId );
		radioIdleItemStyle->SetTextSizeInPixels( fontSizeArray[i], ETrue );
		iTextStyleIdArray.Append( radioIdleItemTextStyleId );
		}
	CleanupStack::PopAndDestroy( &fontSizeArray );

	aEnv.ControlGroup( aControlGroupId ).AppendL( this ); // Takes ownership.
			// No leaving code allowed after this call.
  	}

// ---------------------------------------------------------------------------
// ~CIRRadioIdle::~CIRRadioIdle
// Destructor
// ---------------------------------------------------------------------------
//
CIRRadioIdle::~CIRRadioIdle()
	{
	Env().CancelCustomCommands( this );
	if( iRadioIdleItemArray )
		{
		if( iRadioIdleItemArray->Count() > KZero )
			{
			iRadioIdleItemArray->Reset();
			}
		delete iRadioIdleItemArray;
		}
	CAlfTextStyleManager& manager = Env().TextStyleManager();
	for( TInt i=0; i < iTextStyleIdArray.Count(); i++ )
		{
		manager.DeleteTextStyle( iTextStyleIdArray[i] );
		}
	iTextStyleIdArray.Close();
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::AddRadioIdleL
// Creates the radio idle with passed number of radio idle items
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::AddRadioIdleL( const TInt aNumberOfRadioIdleItems )
	{

	CAlfAnchorLayout* radioIdleAnchor = static_cast<CAlfAnchorLayout*>
								 ( FindTag( KRadioIdleAnchorTag ) );
	if ( !radioIdleAnchor )
		{
		radioIdleAnchor = CAlfAnchorLayout::AddNewL( *this, iParentLayout );
		radioIdleAnchor->SetTagL( KRadioIdleAnchorTag );
		}

	if( !iViewport )
		{
		iViewport = CAlfViewportLayout::AddNewL( *this, radioIdleAnchor );
		iViewport->SetViewportPos( TAlfRealPoint( .0f, .0f ), KZero );
		iViewport->SetViewportSize( TAlfRealSize( 1.0f, 1.0f ), KZero );
		}

	CreateBacgroundTextVisualL();
	// Create the flow layout for the other radio idle items
	CAlfFlowLayout* radioIdleFlowLayout = static_cast<CAlfFlowLayout*> 
		( iViewport->FindTag( KRadioIdleFlowLayoutTag ) );
	if( !radioIdleFlowLayout )
		{
		radioIdleFlowLayout = CAlfFlowLayout::AddNewL( *this, iViewport );
		radioIdleFlowLayout->SetTagL( KRadioIdleFlowLayoutTag );
		radioIdleFlowLayout->SetFlowDirection( CAlfFlowLayout::EFlowVertical );
		radioIdleFlowLayout->SetMode( CAlfFlowLayout::EModeCenterPerpendicular );
		}

	InitializeRadioIdleItemsL( aNumberOfRadioIdleItems );
	AnimateRadioIdleItems();
	UpdateLayout();
	RadioIdleFadeIn();
	}

// ----------------------------------------------------------------------------
// CRadioIdle::CreateBacgroundTextVisualL
// Creates, initializes and animates the radio idle background text visual
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::CreateBacgroundTextVisualL()
	{
	HBufC* itemText = GetRandomRadioIdleContentL();
	CleanupStack::PushL( itemText );
    CAlfTextVisual* backgroundText = static_cast<CAlfTextVisual*> 
    	( iViewport->FindTag( KRadioIdleBackgroundTextTag ) );
	if( !backgroundText && itemText )
		{
		backgroundText = AddRadioIdleItemL(iViewport, *itemText,KRadioIdleBackgroundTextTag, EFalse,
                            KRadioIdleItemBackgroundColor,
                            KRadioIdleItemBackgroundOpacity );

		SetRadioIdleItemSettingsL( backgroundText, *itemText, iRadioIdleItemBackgroundTextStyleId );
        backgroundText->EnableBrushesL();
	    backgroundText->EnableShadow( EFalse );
	    backgroundText->SetFlag(  EAlfVisualFlagManualLayout  );
		SetScrollingToBackgroundText( backgroundText,
                           KRadioIdleBackgroundTextSideToSideTransitionDurationTime,
                           ERadioIdleBackgroundTextScrollEnd );
		backgroundText->SetOpacity( TAlfTimedValue( KRadioIdleBackgroundTextOpacity ) );
		}
	else if( itemText )
		{
		SetRadioIdleItemSettingsL( backgroundText, *itemText,
			 iRadioIdleItemBackgroundTextStyleId );
        SetScrollingToBackgroundText( backgroundText, 
        	KRadioIdleBackgroundTextSideToSideTransitionDurationTime, 
        	ERadioIdleBackgroundTextScrollEnd );
        backgroundText->SetOpacity( TAlfTimedValue( KRadioIdleBackgroundTextOpacity ) );
        }
	else
		{
		}	
	CleanupStack::PopAndDestroy( itemText );
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::InitializeRadioIdleItemsL
// Initializes the radio idle items
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::InitializeRadioIdleItemsL( const TInt aNumberOfRadioIdleItems )
	{

	CAlfFlowLayout* radioIdleFlowLayout = static_cast<CAlfFlowLayout*> 
						( iViewport->FindTag( KRadioIdleFlowLayoutTag ) );
	if( radioIdleFlowLayout )
		{
		// Create the radio idle items
		for( TInt i = 0; i < aNumberOfRadioIdleItems; i++ )
			{
			HBufC* itemText = GetRandomRadioIdleContentL();
			if( itemText )
				{
				CleanupStack::PushL( itemText );
				TInt randFontStyleIx = AlfUtil::RandomInt( KZero,
				  iTextStyleIdArray.Count() - Kone );
			    CAlfTextVisual* radioIdleItem = NULL;
			    if( radioIdleFlowLayout->Count() < aNumberOfRadioIdleItems )
					{
                    radioIdleItem  = AddRadioIdleItemL( radioIdleFlowLayout,
                    	 *itemText, KRadioIdleItemTag,EFalse, 
                    	 KRadioIdleItemBackgroundColor, KRadioIdleItemBackgroundOpacity );
					}
			    else
			    	{
			    	/*lint -save -e826 (Info -- Suspicious pointer-to-pointer conversion (area too small)*/
			    	radioIdleItem = static_cast<CAlfTextVisual*> 
			    			( GetPointer( radioIdleFlowLayout->Visual( i ) ) );
			    	/*lint -restore*/
			    	radioIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
					radioIdleItem->EnableTransformationL( ETrue );
					CAlfTransformation* radioIdleItemTransformation =
									 &( radioIdleItem->Transformation() );
					radioIdleItemTransformation->LoadIdentity();
			    	}
			    SetRadioIdleItemSettingsL( radioIdleItem, *itemText, 
			    		iTextStyleIdArray[ randFontStyleIx ] );
			 	CleanupStack::PopAndDestroy( itemText );
				}
			}
		}


	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::AnimateRadioIdleItems
// Sets defined animation to the created radio idle items
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::AnimateRadioIdleItems()
	{

	CAlfFlowLayout* radioIdleFlowLayout = static_cast<CAlfFlowLayout*> 
				( iViewport->FindTag( KRadioIdleFlowLayoutTag ) );
	if( radioIdleFlowLayout )
		{
		for( TInt i = 0; i < radioIdleFlowLayout->Count(); i++ )
			{
	    	/*lint -save -e826 (Info -- Suspicious pointer-to-pointer conversion (area too small)*/
		    CAlfTextVisual* radioIdleItem = static_cast<CAlfTextVisual*> 
		    	( GetPointer( radioIdleFlowLayout->Visual( i ) ) );
		    /*lint -restore*/
			 SetRadioIdleItemAnimation( radioIdleItem );
			}
		}

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::AddRadioIdleItemL
// Creates the radio idle item to the passed radio idle view port.
// ----------------------------------------------------------------------------
//
CAlfTextVisual* CIRRadioIdle::AddRadioIdleItemL( CAlfLayout* aRadioIdle, 
					const TDesC& aRadioIdleItemText, 
					const TDesC8& aTag, TBool aRadioIdleItemHasBackground,
					const TRgb& aBackgroundColor,
					const TReal32 aBackgroundOpacity )
	{

	CAlfTextVisual* radioIdleItem = CAlfTextVisual::AddNewL( *this, aRadioIdle );
	// Set text to transparent during initialization
	radioIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
    radioIdleItem->EnableBrushesL();
    radioIdleItem->SetTextL( aRadioIdleItemText );
    radioIdleItem->SetTagL( aTag );
    radioIdleItem->EnableShadow( EFalse );
    radioIdleItem->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
    radioIdleItem->SetSize( radioIdleItem->TextExtents() );

    if ( aRadioIdleItemHasBackground )
	    {
		CAlfGradientBrush* gradientBrush = CAlfGradientBrush::NewLC( Env() );
		gradientBrush->SetColor( aBackgroundColor, aBackgroundOpacity );
		radioIdleItem->Brushes()->AppendL( gradientBrush, EAlfHasOwnership );
		CleanupStack::Pop( gradientBrush );
	    }
	return radioIdleItem;

	}
// ----------------------------------------------------------------------------
// CIRRadioIdle::DeviateColorComponent
// Randomly modifies the value of a color component.
// ----------------------------------------------------------------------------
//
TInt CIRRadioIdle::DeviateColorComponent( TInt aBaseValue )
    {
    TInt deviatedValue = aBaseValue
                       + AlfUtil::RandomInt( 0, KMaxRadioIdleItemColorDeviation )
                       - AlfUtil::RandomInt( 0, KMaxRadioIdleItemColorDeviation );

    if( deviatedValue < 0)
        {
        deviatedValue = 0;
        }
    else if( deviatedValue > KTwoFiftyFive )
        {
        deviatedValue = KTwoFiftyFive;
        }
    
    return deviatedValue;
    }

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetRadioIdleItemSettings
// Sets color, opacity and scaling settings to the radio idle item
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetRadioIdleItemSettingsL( CAlfTextVisual* aRadioIdleItem, 
											const TDesC& aRadioIdleItemText,
											  const TInt aTextStyleId )
	{
	// Set text of the radio idle item
	aRadioIdleItem->SetTextL( aRadioIdleItemText );
	// Set text to transparent during initialization
	aRadioIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
	aRadioIdleItem->EnableTransformationL( ETrue );
	CAlfTransformation* radioIdleItemTransformation = &( aRadioIdleItem->Transformation() );
	radioIdleItemTransformation->LoadIdentity();
    
    // Select base color for text

    TRgb radioIdleItemColor( iRadioIdlePrimaryColor );
    if( AlfUtil::RandomInt(0, 1) == 0 )
        {
        radioIdleItemColor = iRadioIdleSecondaryColor;
        }
    
    // Modify the color
    radioIdleItemColor.SetRed( DeviateColorComponent( radioIdleItemColor.Red() ) );
    radioIdleItemColor.SetGreen( DeviateColorComponent( radioIdleItemColor.Green() ) );
    radioIdleItemColor.SetBlue( DeviateColorComponent( radioIdleItemColor.Blue() ) );

	// Set text color of the radio idle item
	aRadioIdleItem->SetColor( radioIdleItemColor );
	// Set text style of the radio idle	item
	aRadioIdleItem->SetTextStyle( aTextStyleId );

	TSize itemSize( 0, 0 );
	if ( aRadioIdleItem->TextExtents() != itemSize )
		{
		aRadioIdleItem->SetSize( aRadioIdleItem->TextExtents() );
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetRadioIdleItemAnimation
// Sets translating animations to the radio idle item
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetRadioIdleItemAnimation( CAlfTextVisual* aRadioIdleItem )
	{

	CAlfFlowLayout* radioIdleFlowLayout = static_cast<CAlfFlowLayout*> 
										( aRadioIdleItem->Layout() );
	TInt radioIdleItemCount = radioIdleFlowLayout->Count();
	TInt radioIdleItemIndex = radioIdleFlowLayout->FindVisual( aRadioIdleItem );

	TReal32 radioIdleItemWidth = aRadioIdleItem->DisplayRect().Width();
	TReal32 radioIdleItemHeight = aRadioIdleItem->DisplayRect().Height();
	TReal32 radioIdleWidth = radioIdleFlowLayout->Size().Target().iX;

	TInt randomTranslateDuration = AlfUtil::RandomInt( KMinRadioIdleItemScrollDurationTime,
									 KMaxRadioIdleItemScrollDurationTime );

	// Calculate y position of the radio idle item
	TReal32 randomMeanDeviationY = 0;
	if(radioIdleItemIndex != 0 && radioIdleItemIndex != radioIdleItemCount - Kone )
		{
		randomMeanDeviationY = AlfUtil::RandomReal( 0.0f,  radioIdleItemHeight * 
															KMaxRadioIdleItemsOverlap );
		TInt deviationDirection = AlfUtil::RandomInt( KZero,  Kone );
		if( deviationDirection )
		 	{
		 	randomMeanDeviationY = -randomMeanDeviationY;
		 	}
		}

	TInt scrollDirection = AlfUtil::RandomInt( KZero,  Kone );
	TAlfTimedValue targetPositionX;
	if( scrollDirection )
		{
		targetPositionX.SetValueNow( -radioIdleWidth/2 - radioIdleItemWidth/2 );
		targetPositionX.SetTarget( radioIdleWidth/2 + radioIdleItemWidth/2,
								 randomTranslateDuration );
		}
	else
		{
		targetPositionX.SetValueNow( radioIdleWidth/2 + radioIdleItemWidth/2  );
		targetPositionX.SetTarget( -radioIdleWidth/2 - radioIdleItemWidth/2,
													 randomTranslateDuration );
		}
	Translate( aRadioIdleItem, targetPositionX, TAlfTimedValue( randomMeanDeviationY ) );
	if( radioIdleItemIndex >= KZero )
		{
		Env().Send( TAlfCustomEventCommand( ERadioIdleItemTransformationEnd,
											this,
											radioIdleItemIndex ),
											randomTranslateDuration );
		}
	// Set text opacity of the radio idle item
	TReal32 randomOpacity = AlfUtil::RandomReal( KMinOpacity, KMaxOpacity );
	aRadioIdleItem->SetOpacity( TAlfTimedValue( randomOpacity ) );
	radioIdleFlowLayout->UpdateChildrenLayout( KMinRadioIdleItemScrollDurationTime );


	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::RadioIdleFadeIn
// Fades in radio idle
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::RadioIdleFadeIn( TBool aNow )
	{

	if ( iViewport )
		{
		if( aNow )
			{
			FadeIn( iViewport, KZero );
			}
		else
			{
			FadeIn( iViewport, KRadioIdleFadeInDurationTime );
			}
		}

	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::RadioIdleFadeOut
// Fades out radio idle
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::RadioIdleFadeOut( TBool aNow )
	{
	

	if ( iViewport )
		{
		iFading = ETrue;
		if( aNow )
			{
			FadeOut( iViewport, KZero );
			iFading = EFalse;
			}
		else if ( iFadeStyle == EFadeStyleNone )
			{
			FadeOut( iViewport, KRadioIdleFadeOutDurationTime );
	        Env().Send( TAlfCustomEventCommand( ERadioIdleFadeOutCompleted, this,
												reinterpret_cast<TInt>( iViewport ) ),
												KRadioIdleFadeOutDurationTime );
	        iViewport = NULL;
			}
		else
			{
		    TAlfTimedPoint point( 0.0f, 0.0f );
		    point.SetStyle( EAlfTimedValueStyleDecelerate );

		    TAlfRealPoint size = iViewport->Size().ValueNow();
		    TAlfRealPoint pos = iViewport->Pos().ValueNow();

		    point = TAlfTimedPoint( pos.iX, pos.iY );

		    TAlfRealPoint target = iViewport->Pos().Target();

		    switch ( iFadeStyle )
		        {
		        case EFadeRight: // Fade left to right
		            point.SetTarget( TAlfRealPoint( target.iX + size.iX, target.iY ),
		            						 KRadioIdleFadeOutDurationTime );
		            break;
		        case EFadeLeft: // Fade right to left
		            point.SetTarget( TAlfRealPoint( target.iX - size.iX, target.iY ),
		            			 KRadioIdleFadeOutDurationTime );
		            break;
		        default:
		            break;
		        }

		    // Fade the radio idle out.
		    TAlfTimedValue opacity( iViewport->Opacity().ValueNow() );
		    opacity.SetStyle( EAlfTimedValueStyleAccelerate );
		    opacity.SetTarget( 0.0f, KRadioIdleFadeOutDurationTime );

		    iViewport->SetFlag( EAlfVisualFlagManualPosition );
		    iViewport->SetPos( point );
		    iViewport->SetOpacity( opacity );

		    iFadeStyle = EFadeStyleNone;

	        Env().Send( TAlfCustomEventCommand( ERadioIdleFadeOutCompleted, this,
												reinterpret_cast<TInt>( iViewport ) ),
												KRadioIdleFadeOutDurationTime );
	        iViewport = NULL;
	        ResetRadioIdleContent();
			}
		}

	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::StartRadioIdle
// Launches radio idle.
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::StartRadioIdleL()
    {
    if( iRadioIdleState == EStopped || iRadioIdleState == ESuspended )
	    {
		Env().CancelCustomCommands( this );
	    AddRadioIdleL( KMaxNumberOfRadioIdleItems );
	    }
    iRadioIdleState = ERunning;
    iDelayedStartup = EFalse;
    
    }

// ---------------------------------------------------------------------------
// CIRRadioIdle::SuspendRadioIdle
// Suspends radio idle and starts it again after startup delay
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::SuspendRadioIdle()
    {
    Env().CancelCustomCommands( this );
    if( iRadioIdleState == ERunning )
	    {
	    RadioIdleFadeOut( ETrue );
	    }
    iRadioIdleState = ESuspended;
    iFading = EFalse;
    }

// ---------------------------------------------------------------------------
// CIRRadioIdle::ShutdownRadioIdle
// Stops radio idle. Radio idle is set to stopped state.
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::ShutdownRadioIdle()
    {

    Env().CancelCustomCommands( this );
    if( iRadioIdleState == ERunning )
	    {
	    RadioIdleFadeOut();
	    }
    iRadioIdleState = EStopped;
    iFading = EFalse;
    }

// ----------------------------------------------------------------------------
// CIRRadioIdle::AddRadioIdleContentL
// Adds radio idle item text to be used in the radio idle
// ----------------------------------------------------------------------------
//
TInt CIRRadioIdle::AddRadioIdleContentL( const TDesC& aRadioIdleItemText )
	{
	if( !iRadioIdleItemArray )
		{
		// Array for radio idle items
    	iRadioIdleItemArray = new( ELeave ) CDesCArrayFlat( KRadioIdleItemArrayGranularity );
		}
	iRadioIdleItemArray->AppendL( aRadioIdleItemText );
	return iRadioIdleItemArray->Count() - Kone;

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::UpdateRadioIdleContent
// Updates radio idle item text in specified index.
// Note that the index of a particular radio idle item text is defined by
// the order in which it has been added by AddRadioIdleContentL.
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::UpdateRadioIdleContent( const TDesC& aRadioIdleItemText, TInt aIndex )
	{

	if( aIndex >= 0 && aIndex < iRadioIdleItemArray->MdcaCount() )
		{
		TPtrC itemContent( iRadioIdleItemArray->MdcaPoint( aIndex ) );
		if ( itemContent.Length() )
			{ 
			iRadioIdleItemArray->Delete( aIndex );
			}
		
		TRAP_IGNORE( iRadioIdleItemArray->InsertL( aIndex, aRadioIdleItemText ) )
		}

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::GetRandomRadioIdleContentL
// Retrieves text from randomly chousen radio idle item
// ----------------------------------------------------------------------------
//
HBufC* CIRRadioIdle::GetRandomRadioIdleContentL()
	{

	if( !iRadioIdleItemArray )
		{
		return NULL;
		}
	TInt count = iRadioIdleItemArray->MdcaCount();
	if( count > KZero )
    	{
    	// Try to find a non-empty radio idle item from the array
		TInt radioIdleItemIndex;
    	for ( TInt i = 0; i < KTen*count; i++ )
    		{
    		radioIdleItemIndex = AlfUtil::RandomInt( KZero, count - Kone );
    		TPtrC itemContent( iRadioIdleItemArray->MdcaPoint( radioIdleItemIndex ) );
    		// Check if there really is some content
    		if ( itemContent.Length() )
    			{
    			return itemContent.AllocL();
    			}
    		}
    	}
   	return NULL;

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::ResetRadioIdleContent
// Removes all radio idle item texts
// ----------------------------------------------------------------------------
//
void  CIRRadioIdle::ResetRadioIdleContent()
	{
	if( iRadioIdleItemArray )
		{
		if( iRadioIdleItemArray->Count() > KZero )
			{
			iRadioIdleItemArray->Reset();
			iRadioIdleItemArray->Compress();
			}
		}

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::RadioIdleContentCount
// Returns number of radio idle content items
// ----------------------------------------------------------------------------
//
TInt CIRRadioIdle::RadioIdleContentCount() const
	{

	if( iRadioIdleItemArray )
		{
		return iRadioIdleItemArray->Count();
		}
	else
		{
		return KZero;
		}

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetPrimaryColor
// Sets primary color for the radio idle
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetPrimaryColor( TRgb aColor )
	{
	iRadioIdlePrimaryColor = aColor;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetSecondaryColor
// Sets secondary color for the radio idle
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetSecondaryColor( TRgb aColor )
	{
	iRadioIdleSecondaryColor = aColor;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetBackgroundColor
// Sets the background color of the radio idle
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetBackgroundColor( TRgb aColor )
	{
	iRadioIdleBackgroundColor = aColor;
	if( iRadioIdleBackgroundGradientBrush )
		{
		iRadioIdleBackgroundGradientBrush->SetColor( aColor, KRadioIdleBackgroundOpacity );
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::Translate
// Translates the visual object with passed arguments
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::Translate( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, 
															const TAlfTimedValue& aY )
	{
	if( aTextVisual )
		{
		TRAPD( err, aTextVisual->EnableTransformationL( ETrue ) );
		if( err == KErrNone )
			{
			CAlfTransformation* visualTransformation = &( aTextVisual->Transformation() );
			TAlfTimedValue x(aX);
			x.SetStyle( EAlfTimedValueStyleLinear );

			TAlfTimedValue y(aY);
			y.SetStyle( EAlfTimedValueStyleLinear );

			visualTransformation->Translate( x, y );
			}
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::Scale
// Scales the visual object with passed arguments
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::Scale( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, 
												const TAlfTimedValue& aY )
	{
	if( aTextVisual )
		{
		TRAPD( err, aTextVisual->EnableTransformationL( ETrue ) );
		if( err == KErrNone )
			{
			CAlfTransformation* visualTransformation = &( aTextVisual->Transformation() );
			visualTransformation->Scale( aX, aY );
			}
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::LoadTextVisualIdentity
// Removes all transformations of the visual object such as the scaling and translating.
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::LoadTextVisualIdentity( CAlfTextVisual* aTextVisual )
	{
	if( aTextVisual )
		{
		TRAPD( err, aTextVisual->EnableTransformationL( ETrue ) );
		if( err == KErrNone )
			{
			CAlfTransformation* visualTransformation = &( aTextVisual->Transformation() );
			visualTransformation->LoadIdentity();
			}
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::UpdateLayout
// Calculates display layout
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::UpdateLayout()
	{
	CAlfVisual* visual = FindTag( KRadioIdleAnchorTag );
	if( visual )
		{
		CAlfAnchorLayout* radioIdleAnchor = static_cast<CAlfAnchorLayout*>( visual );
		radioIdleAnchor->UpdateChildrenLayout( );
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::Refresh
// Starts radio idle again without reseting radio idle items.
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::RefreshL()
	{
	if ( iRadioIdleState == ERunning )
		{
		RadioIdleFadeOut();
		}
	iRadioIdleState = ESuspended;
    iFading = EFalse;
	HandleCommandL( EStartup );
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetIdleFadeStyle
// Sets fade style for radio idle
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetIdleFadeStyle( TRadioIdleFadeStyle aFadeStyle )
	{
	iFadeStyle = aFadeStyle;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetModeChangeRequest
// Sets request for mode change
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetModeChangeRequestL( TRadioIdleMode aRequestedMode )
	{
	iRadioIdleRequestedMode = aRequestedMode;
	if( iRadioIdleRequestedMode == EBackgroundTextAndRadioIdleItems &&
		Mode() == EOnlyBackgroundText )
		{
		InitializeRadioIdleItemsL( KMaxNumberOfRadioIdleItems );
		AnimateRadioIdleItems();
		SetMode( EBackgroundTextAndRadioIdleItems );
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetMode
// Sets the radio idle mode
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetMode( TRadioIdleMode aMode )
	{
	iRadioIdleMode = aMode;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::Mode
// Retrieves the radio idle mode
// ----------------------------------------------------------------------------
//
CIRRadioIdle::TRadioIdleMode CIRRadioIdle::Mode()
	{
	return iRadioIdleMode;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::State
// Retrieves the radio idle state
// ----------------------------------------------------------------------------
//
CIRRadioIdle::TRadioIdleState CIRRadioIdle::State()
	{
	return iRadioIdleState;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::Orientation
// Return orientation of the display
// ----------------------------------------------------------------------------
//
CIRRadioIdle::TOrientation CIRRadioIdle::Orientation()
	{
	return iOrientation;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::Orientation
// Set orientation of the display
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetOrientation( TOrientation aOrientation )
	{
	iOrientation = aOrientation;
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::OfferEventL
// From CAlfControl, takes care of alfred event handling.
// ----------------------------------------------------------------------------
//
TBool CIRRadioIdle::OfferEventL( const TAlfEvent& aEvent )
	{

	TBool isHandled = EFalse;
	if( aEvent.IsCustomEvent() )
		{
		switch( aEvent.CustomParameter() )
			{
			case ERadioIdleItemTransformationEnd:
				{
				HandleRadioIdleItemTransformationEndEventL( aEvent );
				isHandled = ETrue;
				}
				break;
			case ERadioIdleBackgroundTextScrollEnd:
				{
				HandleBackgroundTextScrollEndEventL( aEvent );
				isHandled = ETrue;
				}
				break;
			case ERadioIdleChangeMode:
				{
				if( Mode() == EBackgroundTextAndRadioIdleItems )
					{
					SetModeChangeRequestL( EOnlyBackgroundText );
					}
				else
					{
					SetModeChangeRequestL( EBackgroundTextAndRadioIdleItems );
					}
				isHandled = ETrue;
				}
				break;
			case ERadioIdleFadeOutCompleted:
				{
				reinterpret_cast<CAlfViewportLayout*>( aEvent.CustomEventData() )
														->RemoveAndDestroyAllD();
				iFading = EFalse;
				
				// Check if there was a startup command during fading out
				if ( iDelayedStartup )
					{
					StartRadioIdleL();
					}
				isHandled = ETrue;
				}
				break;
			default:
			break;
			}
		}
	return isHandled;

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::HandleRadioIdleItemTransformationEndEventL
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::HandleRadioIdleItemTransformationEndEventL( const TAlfEvent& aEvent )
	{

	iItemsTransformationEndEventCounter++;
	if ( iRadioIdleRequestedMode == EOnlyBackgroundText || Mode() == EOnlyBackgroundText )
		{
		CAlfFlowLayout* radioIdleFlowLayout = static_cast<CAlfFlowLayout*> 
						( iViewport->FindTag( KRadioIdleFlowLayoutTag ) );
		if ( radioIdleFlowLayout )
			{
			TInt radioIdleItemIndex = aEvent.CustomEventData();
	    	/*lint -save -e826 (Info -- Suspicious pointer-to-pointer conversion (area too small)*/
			CAlfTextVisual* radioIdleItem = static_cast<CAlfTextVisual*> ( GetPointer( radioIdleFlowLayout->
													Visual( radioIdleItemIndex ) ) );
			/*lint -restore*/
			radioIdleItem->SetOpacity( TAlfTimedValue( .0f ) );

			if( iItemsTransformationEndEventCounter == radioIdleFlowLayout->Count() )
				{
				SetMode( EOnlyBackgroundText );
				iItemsTransformationEndEventCounter = 0;
				Env().Send( TAlfCustomEventCommand( ERadioIdleBackgroundTextScrollEnd, this ), KZero );
				}
			}
		}
	else
		{
		CAlfFlowLayout* radioIdleFlowLayout = static_cast<CAlfFlowLayout*> ( iViewport->
												FindTag( KRadioIdleFlowLayoutTag ) );
		if ( radioIdleFlowLayout )
			{
			HBufC* itemText = GetRandomRadioIdleContentL();
			if( itemText )
				{
				CleanupStack::PushL( itemText );
				TInt radioIdleItemIndex = aEvent.CustomEventData();
		    	/*lint -save -e826 (Info -- Suspicious pointer-to-pointer conversion (area too small)*/
				CAlfTextVisual* radioIdleItem = static_cast<CAlfTextVisual*> ( GetPointer(
											 radioIdleFlowLayout->Visual( radioIdleItemIndex ) ) );
				/*lint -restore*/
				if ( iTextStyleIdArray.Count() > KZero )
					{
			    	// First hide the radio idle item
					radioIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
					radioIdleItem->EnableTransformationL( ETrue );
					CAlfTransformation* radioIdleItemTransformation = 
											&( radioIdleItem->Transformation() );
					radioIdleItemTransformation->LoadIdentity();

					TInt randFontStyleIx = AlfUtil::RandomInt( KZero,  iTextStyleIdArray.Count() - Kone );
					SetRadioIdleItemSettingsL( radioIdleItem, *itemText,
										  iTextStyleIdArray[ randFontStyleIx ] );
					SetRadioIdleItemAnimation( radioIdleItem );

					if ( iRadioIdleRequestedMode != EOnlyBackgroundText )
						{
						if( iItemsTransformationEndEventCounter == radioIdleFlowLayout->Count() )
							{
							Env().Send( TAlfCustomEventCommand( ERadioIdleChangeMode, this ),
									KMaxRadioIdleItemScrollDurationTime * KModeChangingFactor );
							iItemsTransformationEndEventCounter = 0;
							}
						}
					}
				CleanupStack::PopAndDestroy( itemText );
				}
			}
		}

	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::HandleBackgroundTextScrollEndEventL
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::HandleBackgroundTextScrollEndEventL( const TAlfEvent& /*aEvent*/ )
	{
	if( iViewport )
		{
		CAlfTextVisual* backgroundText = static_cast<CAlfTextVisual*> ( iViewport->
										FindTag( KRadioIdleBackgroundTextTag ) );
		if( backgroundText )
			{
			backgroundText->SetOpacity( TAlfTimedValue( .0f ) );

			if ( iRadioIdleRequestedMode == EOnlyBackgroundText && Mode() == 
											 EBackgroundTextAndRadioIdleItems )
				{
				}
			else
				{
				if( iRadioIdleRequestedMode == EOnlyBackgroundText &&
						Mode() == EOnlyBackgroundText )
					{
					Env().CancelCustomCommands( this );
					Env().Send( TAlfCustomEventCommand( ERadioIdleChangeMode, this ),
							KRadioIdleBackgroundTextSideToSideTransitionDurationTime );
					}
				backgroundText->EnableTransformationL( ETrue );
				CAlfTransformation* radioIdleItemTransformation = 
										&( backgroundText->Transformation() );
				radioIdleItemTransformation->LoadIdentity();
				HBufC* itemText = GetRandomRadioIdleContentL();
				if( itemText )
					{
					CleanupStack::PushL( itemText );
					SetRadioIdleItemSettingsL( backgroundText, *itemText,
									 iRadioIdleItemBackgroundTextStyleId );
					CleanupStack::PopAndDestroy( itemText );
					}
				backgroundText->SetOpacity( TAlfTimedValue( KRadioIdleBackgroundTextOpacity ) );
				SetScrollingToBackgroundText( backgroundText,
								   KRadioIdleBackgroundTextSideToSideTransitionDurationTime,
								   ERadioIdleBackgroundTextScrollEnd );
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::HandleCommandL
// From MRadioIdleCommandObserver, takes care of radio idle command handling.
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::HandleCommandL( TRadioIdleCommand aCommand )
    {
    switch ( aCommand )
	    {
		case ESuspend:
			SuspendRadioIdle();
			break;
		case EStartup:
			{
			// Check if radio idle can be started right away
			if ( !iFading )
				{
				StartRadioIdleL();
				}
			else
				{
				iDelayedStartup = ETrue;
				}
			}	
			break;
		case EShutdown:
			ShutdownRadioIdle();
			break;
		case ERefresh:
			RefreshL();
			break;
		case EResetContent:
			ResetRadioIdleContent();
			break;
		default:
			break;
		}
    }

// ----------------------------------------------------------------------------
// CIRRadioIdle::SetScrollingToBackgroundText
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::SetScrollingToBackgroundText( CAlfTextVisual* aVisual, TInt aTransitionTime,
						 TRadioIdleCustomEvent aEventAfterScrollingEnd )
	{
	CAlfViewportLayout* radioIdle = static_cast<CAlfViewportLayout*> ( aVisual->Layout() );
	TReal32 radioIdleItemWidth = aVisual->DisplayRect().Width();
	TReal32 radioIdleWidth = radioIdle->Size().Target().iX;

	TAlfTimedValue targetPositionX;
	if( iBackgroundTextScrollDirection == EScrollRightToLeft  )
	 	{
		targetPositionX.SetValueNow(  -radioIdleItemWidth );
		targetPositionX.SetTarget( radioIdleWidth, aTransitionTime );
		iBackgroundTextScrollDirection = EScrollLeftToRight;
	 	}
	else
	 	{
		targetPositionX.SetValueNow( radioIdleWidth );
		targetPositionX.SetTarget( -radioIdleItemWidth, aTransitionTime );
		iBackgroundTextScrollDirection = EScrollRightToLeft;
	 	}
	Translate( aVisual, targetPositionX, TAlfTimedValue( -30.0f ) ); 
	Env().Send( TAlfCustomEventCommand( aEventAfterScrollingEnd, this ),
			    aTransitionTime + KRadioIdleBackgroundTextScrollInterval );
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::ScrollToLeft
// Sets and starts scrolling animation to CAlfTextVisual.
// The visual object is scrolled from current position to
// left side of the display
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::ScrollToLeft( CAlfTextVisual* aVisual ) const
	{
	if( aVisual )
		{
		TReal32 visualWidth = aVisual->DisplayRect().Width();
		TInt textWidth = aVisual->TextExtents().iWidth;
		TReal32 scrollValue = visualWidth;
		if( textWidth > visualWidth )
			{
			scrollValue += textWidth - visualWidth;
			}
		TAlfTimedPoint timedPosition = aVisual->Pos();
		TAlfRealPoint positionZero( 0.0f, timedPosition.iY.ValueNow() );
		TAlfRealPoint positionLeft( - ( scrollValue ), timedPosition.iY.ValueNow() );
		aVisual->SetPos( positionZero );
		aVisual->SetPos( positionLeft, KThousand );
		}
	}

// ----------------------------------------------------------------------------
// CIRRadioIdle::ScrollToRight
// Sets and starts scrolling animation to CAlfTextVisual.
// The visual object is scrolled from current position to
// right side of the display
// ----------------------------------------------------------------------------
//
void CIRRadioIdle::ScrollToRight( CAlfTextVisual* aVisual ) const
	{
	TReal32 visualWidth = aVisual->DisplayRect().Width();
	TInt textWidth = aVisual->TextExtents().iWidth;
	TReal32 scrollValue = visualWidth;
	if( textWidth > visualWidth )
		{
		scrollValue += textWidth - visualWidth;
		}
	TAlfTimedPoint timedPosition = aVisual->Pos();
	TAlfRealPoint positionZero( 0.0f, timedPosition.iY.ValueNow() );
	TAlfRealPoint positionRight( scrollValue, timedPosition.iY.ValueNow() );
	aVisual->SetPos( positionZero );
	aVisual->SetPos( positionRight, KThousand );
	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::FadeIn
// Sets fade-in animation to the CAlfVisual.
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::FadeIn( CAlfVisual* aVisual, TInt aTime, TReal32 aOpacity ) const
	{
	TAlfTimedValue opacity;
	opacity.SetValueNow( 0.0f ); // immediate change
	opacity.SetTarget( aOpacity, aTime ); // and smooth target
	aVisual->SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::FadeOut
// Sets fade-out animation to the CAlfVisual.
// ---------------------------------------------------------------------------
//
void CIRRadioIdle::FadeOut( CAlfVisual* aVisual, TInt aTime, TReal32 aOpacity ) const
	{
	TAlfTimedValue opacity;
	opacity.SetTarget( aOpacity, aTime );
	aVisual->SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CIRRadioIdle::GetPointer
// Converts reference to a AlfVisual to pointer.
// ---------------------------------------------------------------------------
//
CAlfVisual* CIRRadioIdle::GetPointer( CAlfVisual& aRef )
	{
	return &aRef;
	}

//  End of File

