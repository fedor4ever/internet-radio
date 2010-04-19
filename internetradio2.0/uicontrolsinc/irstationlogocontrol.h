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


#ifndef C_CIRSTATIONLOGOCONTROL_H
#define C_CIRSTATIONLOGOCONTROL_H

#include <alf/alfcontrol.h>

#include "irstationinformationdata.h"
#include "irimageconverterobserver.h"

class CAlfImageVisual;
class CIRImageConverter;
class CIRStationLogoData;
class MIRLogoControlObserver;

/**
 * Station Logo Control
 *
 * @since   S60 v3.2
 */
class CIRStationLogoControl : public CAlfControl, 
                              public MIRImageConverterObserver
    {
public:

    /**
     * Two-phased constructor.
     * Adds the control to the control group and to layout hierarchy.
     *
     * @param aAlfEnv The Alfred environment.
     * @param aControlGroupId The control group that this control is to be added to.
     * @param aParentLayout The parent layout for the visuals.
     */
    static CIRStationLogoControl* NewL( CAlfEnv& aAlfEnv, TInt aControlGroupId, 
    									CAlfLayout* aParentLayout = NULL );

    /**
     * Destructor.
     */
    ~CIRStationLogoControl();
    
    /**
     * Prepares the logo graphics for specified ID.
     * 
     * @param aId The identifier for the logo.
     */
    void PrepareLogoL( TInt aId );
    
    /**
     * Switches to specified logo.
     * 
     * @param aId   The identifier for the logo.
     */
    TBool SwitchToLogoL( TInt aId );
    
    /**
     * Sets the raw data for all the logos.
     * 
     * @param aIdArray Identifiers to be used for the logos.
     * @param aRawData The raw graphics data for each logo.
     */
    void SetLogosL( const RArray<TInt>& aIdArray, const CPtrC8Array& aRawData );
    
    /**
     * Updates one logo.
     * 
     * @param   aId         The ID of the logo to be updated. 
     *                      If such ID is not found, it is added.
     * @param   aRawData    The raw data of the logo.
     *                      If the data is a KNullDesC8, it is removed.
     */
    void UpdateLogoL( TInt aId, const TDesC8& aRawData );
    
    
    /**
     * Sets logo fade style.
     * 
     * @param   aFadeStyle  The style of the logo fading. 
     */
    void SetLogoFadeStyle( TIRFadeStyle aFadeStyle );
    /**
     * Sets the observer for this control.
     * 
     * @param   aObserver  The observer for this control. 
     */
    void SetLogoControlObserver( MIRLogoControlObserver& aObserver );

    // from CAlfControl
    void VisualLayoutUpdated( CAlfVisual& aVisual );


    // from MVRImageConverterObserver
    void HandleImageConversionEventL( MIRImageConverterObserver::TIRImageConversionEvent aEvent, 
                                      TInt aId, 
                                      TInt aError );
     void FadeOutLogo(TInt aId);                               

private: // Methods

    /**
     * The types of delayed showing of logo.
     */
    enum TIRLogoFadeDelayType
        {
        EIRLogoNoDelay,
        EIRDelayedTextureImage,
        EIRDelayedDefaultImage
        };

    /**
     * Default constructor.
     */
    CIRStationLogoControl();

    /**
     * Second phase constructor.
     *
     * @param aAlfEnv The Alfred environmennt.
     * @param aControlGroupId The control group ID for the controls.
     * @param aParentLayout The parent layout for the visuals.
     */
    void ConstructL( CAlfEnv& aAlfEnv, TInt aControlGroupId, CAlfLayout* aParentLayout );

    /**
     * Provides the station logo data for specified id.
     * 
     * @param aId The identifier for the logo.
     */
    CIRStationLogoData* LogoDataForId( TInt aId );

    /**
     * Starts to convert the graphics for specified logo
     * 
     * @param aLogoData The logo data to be converted
     */
    void StartConversionL( CIRStationLogoData& aLogoData );

    /**
     * Fades the station logo in.
     * 
     * @param aVisual The logo to be faded in.
     */
    void FadeInStationLogoL( CAlfVisual& aVisual );
    
    /**
     * Fades the current logo out. If logo is not the default logo, deletes it's data
     * after the fading (ansynchronously).
     * 
     * @param aVisual The logo to be faded out.
     */
    void FadeOutCurrentLogo( CAlfVisual& aVisual );

    /**
     * Returns target point for logo fade out.
     * 
     * @param aVisual The visual to be faded out.
     * @return Target point.
     */
    TAlfTimedPoint FadeOutTargetPoint( CAlfVisual& aVisual ) const;

    /**
     * Returns start point for logo fade in.
     * 
     * @param aVisual The visual to be faded in.
     * @return Start point.
     */
    TAlfTimedPoint FadeInStartPoint( CAlfVisual& aVisual ) const;


    /**
     * Updates iLogodata Array.
     */
     void DestroyPreviousLogosL( const RArray<TInt>& aIdArray, const CPtrC8Array& aRawData );
     /**
     * Updates iLogodata Array.
     */
     void DefaultlogoHandlingL( const RArray<TInt>& aIdArray, const CPtrC8Array& aRawData );


private: // Members

    /**
     * The image converter.
     * Owned.
     */
    CIRImageConverter* iCurrentImageConverter;

    /**
     * The logo data for all stations.
     * Owned.
     */
    RPointerArray<CIRStationLogoData> iLogoData;

    /**
     * The parent layout for the logos.
     * Not owned.
     */
    CAlfDeckLayout* iDeck;

    /**
     * The size that is used for image conversions
     */
    TSize iVisualSize;
    
    /**
     * The mif bitmap id for default logo
     */
    TInt iBitmapId;
    
    /**
     * The mif mask id for default logo
     */
    TInt iMaskId;
    
    /**
     * The ID for the current logo. KErrNotFound for the default logo
     */
    TInt iCurrentId;
    
    /**
     * The fading identification for situations when the graphics aren't ready
     */
    TIRLogoFadeDelayType iDelayedFadeIn;

    /**
     * Style of the logo fading.
     */
    TIRFadeStyle iFadeStyle;
    /**
     * Used to store logo.
     */
    TInt iCount;
    /**
     * Used for logo storage.
     */
     TInt iRet;
     /**
     * Used for logo storage.
     */
     TInt iLogoDisplay;
    /**
     * Used for Handling Error Conditions
     * During Logo Conversion.
     */
     TInt iError;
    /**
     * Used for Handling Logo FadeOut
     */
     TBool iFadeOut;
     
    MIRLogoControlObserver* iLogoControlObserver;
    };

#endif // C_CIRSTATIONLOGOCONTROL_H