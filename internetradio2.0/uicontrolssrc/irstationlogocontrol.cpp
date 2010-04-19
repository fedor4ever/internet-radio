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
* Description:  Control for single station logo
*
*/


#include <alf/alfcommand.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdecklayout.h>
#include <alf/alfenv.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alftexturemanager.h>
#include <alf/alftimedvalue.h>

#include "irdebug.h"

#include "irimageconverter.h"
#include "irstationlogocontrolobserver.h"
#include "irstationlogocontrol.h"
#include "irstationlogodata.h"

// Time in milliseconds to spend animating the displays moving in and out of the screen
const TInt KVRLogoFadeTime = 400;
const TInt KTwo=-2;
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CIRStationLogoControl::CIRStationLogoControl() : iCurrentId( KErrNotFound ),iError(KErrNone)
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::ConstructL( CAlfEnv& aAlfEnv, TInt aControlGroupId, 
										CAlfLayout* aParentLayout )
    {
    IRLOG_DEBUG(" *** S60VisualRadio -- CIRStationLogoControl::ConstructL()");
    CAlfControl::ConstructL( aAlfEnv );
 
    iDeck = CAlfDeckLayout::AddNewL( *this, aParentLayout );
	iFadeOut=EFalse;
    iCurrentImageConverter = CIRImageConverter::NewL();
    iCurrentImageConverter->SetObserver( this );
	iVisualSize = DisplayArea().Size();

    CAlfControlGroup& group = Env().ControlGroup( aControlGroupId );
    group.AppendL( this ); // Transfers ownership
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CIRStationLogoControl* CIRStationLogoControl::NewL( CAlfEnv& aAlfEnv, 
                                                    TInt aControlGroupId, 
                                                    CAlfLayout* aParentLayout )
    {
    CIRStationLogoControl* self = new( ELeave ) CIRStationLogoControl;
    CleanupStack::PushL( self );
    self->ConstructL( aAlfEnv, aControlGroupId, aParentLayout );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIRStationLogoControl::~CIRStationLogoControl()
    {
    iLogoControlObserver = NULL;
    delete iCurrentImageConverter;
    
    for( TInt i = 0 ; i < iLogoData.Count() ; i++ )
    {
    Env().CancelCustomCommands( iLogoData[i] );
    }
    iLogoData.ResetAndDestroy();
    iLogoData.Close();
    }

// ---------------------------------------------------------------------------
// Prepares the graphics for the logo that is to be displayed
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::PrepareLogoL( TInt aId )
    {
    IRLOG_DEBUG2(" *** S60VisualRadio -- CIRStationLogoControl::PrepareLogoL(aId=%d)", aId);
    CIRStationLogoData* logoData = LogoDataForId( aId );
    if( logoData && !logoData->Texture() )
        {
        StartConversionL( *logoData );
        }
    }

// ---------------------------------------------------------------------------
// Switches the logo that is to be displayed
// ---------------------------------------------------------------------------
//
TBool CIRStationLogoControl::SwitchToLogoL( TInt aId )
    {
    IRLOG_DEBUG2(" CIRStationLogoControl::SwitchToLogoL(aId=%d)", aId);
    // No need to fade out or load new logo.
    if ( aId != KErrNotFound )
        {
        PrepareLogoL( aId );
        }

    CIRStationLogoData* previousLogoData = LogoDataForId( iCurrentId );
    CIRStationLogoData* newData = LogoDataForId( aId );
    
    // No need to fade out or load new logo.
    if ( aId == iCurrentId && previousLogoData && previousLogoData->Visual() )
        {
        return ETrue;
        }
    // To Do 
    // Fade the current logo out.Handled in View
    /*if ( previousLogoData && previousLogoData->Visual() )
        {
        // Station logo.
        FadeOutCurrentLogo( *previousLogoData->Visual() );
        }*/
		if(iError!=KErrNone)
		{
		return EFalse;
		}
    TBool logoAdded = EFalse;
    iCurrentId = aId;
    if( newData )
        {
        Env().CancelCustomCommands( newData, 
        		CIRStationLogoData::EIRCustomEventLogoFadeOutComplete );
        if( !newData->Visual() )
            {
            iDelayedFadeIn = EIRLogoNoDelay;

            CAlfImageVisual* visual = NULL;
            if( !newData->Visual() )
                {
                visual = CAlfImageVisual::AddNewL( *this, iDeck );
                visual->SetOpacity( TAlfTimedValue( 0.0f ) );
                visual->SetScaleMode( CAlfImageVisual::EScaleFitInside );
                newData->SetVisual( visual );
                }
            else
                {
                visual = newData->Visual();
                }
 
            if( newData->Texture() )
                {
                visual->SetImage( TAlfImage( *newData->Texture() ) );
                    FadeInStationLogoL( *visual );
                }
            else
                {
                if( newData != previousLogoData )
                    {
                    iDelayedFadeIn = EIRDelayedTextureImage;
                    }
                else
                    {
                    visual->SetOpacity( TAlfTimedValue( 1.0f ) );
                    }
                }
            }
        logoAdded = ETrue;
        }

    return logoAdded;
    }



// ---------------------------------------------------------------------------
// Setter for logo data
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::SetLogosL( const RArray<TInt>& aIdArray , const CPtrC8Array& aRawData )
	{
	IRLOG_DEBUG2(" *** S60VisualRadio -- CIRStationLogoControl::SetLogosL() -- %d items", aRawData.Count());
	iFadeOut=EFalse;
	iError=KErrNone;
	DestroyPreviousLogosL(aIdArray, aRawData);
	iRet=0;
	iLogoDisplay=EFalse;
	}

void CIRStationLogoControl::UpdateLogoL( TInt aId, const TDesC8& aRawData )
    {
    IRLOG_DEBUG2(" *** S60VisualRadio -- CIRStationLogoControl::UpdateLogoL(aId=%d)", aId);
    TBool idFound = EFalse;
    for( TInt i = 0 ; i < iLogoData.Count() ; i++ )
        {
        if( aId == iLogoData[i]->Id() )
            {
            // Update existing logo
            if( aRawData == KNullDesC8 )
                {
                delete iLogoData[i];
                iLogoData.Remove( i );
                }
            else
                {
                iLogoData[i]->SetRawData( aRawData );
                }
            idFound = ETrue;
            i = iLogoData.Count();
            }
        }
    if( !idFound && aRawData != KNullDesC8 )
        {
        CIRStationLogoData* data = CIRStationLogoData::NewLC( aId, aRawData );
        iLogoData.AppendL( data );
        CleanupStack::Pop( data );
        }
    CIRStationLogoData* logoData = LogoDataForId( iCurrentId );
    if( logoData && logoData->Id() == aId )
        {
        StartConversionL( *logoData );
        }
    }

// ---------------------------------------------------------------------------
// Setter for logo fade style.
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::SetLogoFadeStyle( TIRFadeStyle aFadeStyle )
    {
    iFadeStyle = aFadeStyle;
    }

// ---------------------------------------------------------------------------
// From class CAlfControl.
// VisualLayoutUpdated
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::VisualLayoutUpdated( CAlfVisual& /*aVisual*/ )
    {
    IRLOG_DEBUG(" CIRStationLogoControl::VisualLayoutUpdated()");

        // Start the converter for current logo for animation purposes
        CIRStationLogoData* logoData = LogoDataForId( iCurrentId );
        if( logoData )
            {
            TRAP_IGNORE( StartConversionL( *logoData ) )
        }
    }

// ---------------------------------------------------------------------------
// CIRStationLogoControl::SetLogoControlObserver
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::SetLogoControlObserver( MIRLogoControlObserver& aObserver )
    {
    IRLOG_DEBUG(" CIRStationLogoControl::SetLogoControlObserver()");
    iLogoControlObserver = &aObserver;
    }

// ---------------------------------------------------------------------------
// From class MVRImageConverterObserver.
// HandleImageConversionEventL
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::HandleImageConversionEventL( MIRImageConverterObserver::
														TIRImageConversionEvent /*aEvent*/, 
                                                         TInt aId, 
                                                         TInt aError )
    {
    IRLOG_DEBUG3(" *** S60VisualRadio -- CIRStationLogoControl::HandleImageConversionEventL(aId=%d, aError=%d)", aId, aError);
    iError=aError;
    if( aError == KErrNone )
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        iCurrentImageConverter->TransferBitmapOwnership( bitmap, mask );
        CIRStationLogoData* convertedLogoData = LogoDataForId( aId );
        if( convertedLogoData )
            {
            convertedLogoData->UpdateTextureL( bitmap, mask ); // transfers ownership
            }
        else
            {
            delete bitmap;
            delete mask ;
            }
        CIRStationLogoData* currentLogoData = LogoDataForId( iCurrentId );
        if( currentLogoData && currentLogoData->Visual() && currentLogoData == convertedLogoData )
            {
             // Logo is ready for display, check that everything is OK before displaying the logo.
            iLogoControlObserver->LogoReadyForDisplayL();
            // Display the logo.
            currentLogoData->Visual()->SetImage( TAlfImage( *currentLogoData->Texture() ) );
            if( iDelayedFadeIn == EIRDelayedTextureImage )
                {
                FadeInStationLogoL( *currentLogoData->Visual() );
                }
            }
        }
    else
        {
        iLogoControlObserver->LogoConversionFailedL();
        }
    }

// ---------------------------------------------------------------------------
// Maps the logo ID to station logo data
// ---------------------------------------------------------------------------
//
CIRStationLogoData* CIRStationLogoControl::LogoDataForId( TInt aId )
    {
    CIRStationLogoData* logo = NULL;
    for( TInt i = 0 ; i < iLogoData.Count() ; i++ )
        {
        if( iLogoData[i]->Id() == aId )
            {
            logo = iLogoData[i];
            i = iLogoData.Count();
            }
        }
    return logo;
    }

// ---------------------------------------------------------------------------
// Starts the conversion for hte current logo
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::StartConversionL( CIRStationLogoData& aLogoData )
    {
    IRLOG_DEBUG(" *** S60VisualRadio -- CIRStationLogoControl::StartConversionL()");
    if( iVisualSize.iWidth && iVisualSize.iHeight )
        {
        iCurrentImageConverter->SetDataL( aLogoData.RawData() );
        if(iError==KErrNone)
	        {
	        iCurrentImageConverter->StartL( iVisualSize, aLogoData.Id() );
	        }
        }
    }

// ---------------------------------------------------------------------------
// Fades the station logo in.
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::FadeInStationLogoL( CAlfVisual& aVisual )
    {
    IRLOG_DEBUG(" *** S60VisualRadio -- CIRStationLogoControl::FadeInCurrent()");

    iDelayedFadeIn = EIRLogoNoDelay;

    // Get start point and fade time.
    TAlfTimedPoint point = FadeInStartPoint( aVisual );
    TInt fadeTime = point.iX.TimeToTargetinMilliSeconds();

    // Fade the logo in.
    TAlfTimedValue opacity( 0.0f );
    opacity.SetStyle( EAlfTimedValueStyleDecelerate );
    opacity.SetTarget( 1.0f, fadeTime );

    aVisual.SetFlag( EAlfVisualFlagManualPosition );
    aVisual.SetPos( point );
    aVisual.SetOpacity( opacity );

	return;
    }

// ---------------------------------------------------------------------------
// Fades the current logo out. If logo is not the default logo, deletes it's
// data after the fading (ansynchronously).
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::FadeOutCurrentLogo( CAlfVisual& aVisual )
    {
    IRLOG_DEBUG(" *** S60VisualRadio -- CIRStationLogoControl::FadeOutCurrentLogo");

    CIRStationLogoData* logoData = LogoDataForId( iCurrentId );

    // Get target point.
    TAlfTimedPoint point = FadeOutTargetPoint( aVisual );
    TInt fadeTime = point.iX.TimeToTargetinMilliSeconds();

    // Fade the logo out.
    TAlfTimedValue opacity( aVisual.Opacity().ValueNow() );
    opacity.SetStyle( EAlfTimedValueStyleAccelerate );
    opacity.SetTarget( 0.0f, fadeTime );

    aVisual.SetFlag( EAlfVisualFlagManualPosition );
    aVisual.SetPos( point );
    aVisual.SetOpacity( opacity );

    if( logoData ) 
        {
        // This is not default logo, send event to destroy logo data.
        IRLOG_DEBUG2(" *** S60VisualRadio -- CIRStationLogoControl::FadeOutAndDeleteCurrent() -- sending custom event to delete logo data: 0x%x", logoData);
        Env().Send( TAlfCustomEventCommand( CIRStationLogoData::EIRCustomEventLogoFadeOutComplete, 
                                            logoData, 
                                            NULL), KVRLogoFadeTime );
        }
    }

// ---------------------------------------------------------------------------
// Returns fades out target point.
// ---------------------------------------------------------------------------
//
TAlfTimedPoint CIRStationLogoControl::FadeOutTargetPoint( CAlfVisual& aVisual ) const
    {
    TAlfTimedPoint point( 0.0f, 0.0f );
    point.SetStyle( EAlfTimedValueStyleDecelerate );

    TAlfRealPoint size = aVisual.Size().ValueNow();
    TAlfRealPoint pos = aVisual.Pos().ValueNow();
    
    point = TAlfTimedPoint( pos.iX, pos.iY );

    TAlfRealPoint target = aVisual.Pos().Target();
    
    switch ( iFadeStyle )
        {
        case EIRFadeUnknown:
            point.SetTarget( TAlfRealPoint( target.iX, target.iY ), KVRLogoFadeTime );
            break;
        case EIRFadeLeftToRight:
            point.SetTarget( TAlfRealPoint( target.iX + size.iX, target.iY ), KVRLogoFadeTime );
            break;
        case EIRFadeRightToLeft:
            point.SetTarget( TAlfRealPoint( target.iX - size.iX, target.iY ), KVRLogoFadeTime );
            break;
        case EIRFadeTopToBottom:
            point.SetTarget( TAlfRealPoint( target.iX, target.iY + size.iY ), KVRLogoFadeTime );
            break;
        case EIRFadeBottomToTop:
            point.SetTarget( TAlfRealPoint( target.iX, target.iY - size.iY ), KVRLogoFadeTime );
            break;
        case EIRFadeNoFade:
            point.SetTarget( TAlfRealPoint( target.iX, target.iY ), 0 );
            break;
        default:
            break;
        }
    return point;
    }

// ---------------------------------------------------------------------------
// Returns fade in start point.
// ---------------------------------------------------------------------------
//
TAlfTimedPoint CIRStationLogoControl::FadeInStartPoint( CAlfVisual& aVisual ) const
    {
    TAlfTimedPoint point( 0.0f, 0.0f );
    point.SetStyle( EAlfTimedValueStyleDecelerate );

    TAlfRealPoint size = aVisual.Size().ValueNow();
    TAlfRealPoint pos = aVisual.Pos().ValueNow();
    
    point = TAlfTimedPoint( pos.iX, pos.iY, KVRLogoFadeTime );
    
    switch ( iFadeStyle )
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
            point.iX.SetValueNow( pos.iX );
            point.iY.SetValueNow( pos.iY );
            point.SetTarget( pos, 0 );
            break;
        default:
            break;
        }
    return point;
    }

    
// ---------------------------------------------------------------------------
// Sets the default image to the current visual
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::DestroyPreviousLogosL( const RArray<TInt>& aIdArray , 
										const CPtrC8Array& aRawData )
	{
	
	for(TInt value=0;value<iLogoData.Count();value++)
	{
	TInt Value=iLogoData[value]->Id();	
	}
	if(iCount==0)
			{
			iCurrentId=KTwo;
			iCount++;
			iLogoData.ResetAndDestroy();
			for( TInt i = 0 ; i < aRawData.Count() ; i++ )
				{
				if(aIdArray.Count())
					{
					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
					iLogoData.AppendL( data );
					CleanupStack::Pop( data );
					}
				}
			}

	else if(iCount==1)
		{
		if(aRawData.Count()==1)
			{
			iCurrentId=KTwo;
			TInt i=0;
			delete iLogoData[i];
			iLogoData.Remove( i);
			for( TInt i = 0 ; i < aRawData.Count() ; i++ )
				{
				if(aIdArray.Count())
					{
					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
					iLogoData.AppendL( data );
					CleanupStack::Pop( data );
					}	
				}
			}
		else
			{
			for( TInt i = 1 ; i < aRawData.Count() ; i++ )
				{
				if(aIdArray.Count())
					{

					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
					iLogoData.AppendL( data );
					CleanupStack::Pop( data );
					}
				}
			}
		iCount++;		
		}

	else if(iCount>1)
		{
		if(aRawData.Count()==1)
			{
			iLogoData.ResetAndDestroy();
			iLogoDisplay=ETrue;

			for( TInt i = 0 ; i < aRawData.Count() ; i++ )
				{
				if(aIdArray.Count())
					{

					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
					iLogoData.AppendL( data );
					CleanupStack::Pop( data );
					}
				}
			}
		else if(aRawData.Count()==2 && iLogoData.Count()==1)
			{
			for( TInt i = 1 ; i < aRawData.Count() ; i++ )
				{
				if(aIdArray.Count())
					{

					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
					iLogoData.AppendL( data );
					CleanupStack::Pop( data );
					}
				}
			}
		else if(iLogoData.Count()==1&&!iLogoDisplay)
			{
			for( TInt i = 1 ; i < aRawData.Count() ; i++ )
				{
				if(aIdArray.Count())
					{

					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
					iLogoData.AppendL( data );
					CleanupStack::Pop( data );
					}
				}
			iLogoDisplay=ETrue;
			}
		else if(!iLogoDisplay)
			{


			for(TInt j = 0;j<aRawData.Count();j++)
				{
				if(aIdArray.Count())
					{

					CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[j], aRawData[j] );

					for(TInt i = 0;i<iLogoData.Count();i++)
						{
						if(!(data->Id()==iLogoData[i]->Id())&&(!iRet))
							{
							if(i==1)
								{
								DefaultlogoHandlingL(aIdArray, aRawData);
								}
							else
								{
								delete iLogoData[i];
								iLogoData.Remove( i);
								for( TInt i = 1 ; i < aRawData.Count() ; i++ )
									{
									if(aIdArray.Count())
										{

										CIRStationLogoData* data = CIRStationLogoData::
											NewLC( aIdArray[i], aRawData[i] );
										iLogoData.AppendL( data );
										CleanupStack::Pop( data );
										}
									}
								iRet++;
								}

							}


						}

					CleanupStack::PopAndDestroy( data );
					}
				}
			}


		}
}
// ---------------------------------------------------------------------------
// Sets the default image to the current visual
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::DefaultlogoHandlingL( const RArray<TInt>& aIdArray , 
								const CPtrC8Array& aRawData )
	{
	  for(TInt j = 0;j<aRawData.Count();j++)
	  {
	  	TInt i=0;
	  	delete iLogoData[i];
		iLogoData.Remove( i);
		for( TInt i = 1 ; i < aRawData.Count() ; i++ )
			{
			if(aIdArray.Count())
			{
			CIRStationLogoData* data = CIRStationLogoData::NewLC( aIdArray[i], aRawData[i] );
			iLogoData.AppendL( data );
			CleanupStack::Pop( data );
			}
			}
	  }
	}

// ---------------------------------------------------------------------------
// Sets the default image to the current visual
// ---------------------------------------------------------------------------
//
void CIRStationLogoControl::FadeOutLogo(TInt /*aId*/)
	{
	CIRStationLogoData* previousLogoData = LogoDataForId( iCurrentId );
	// Fade the current logo out.
    if ( previousLogoData && previousLogoData->Visual() )
        {
        // Station logo.
        iFadeOut=ETrue;
        FadeOutCurrentLogo( *previousLogoData->Visual() );
        }
	 iCurrentId=KTwo;
	}
