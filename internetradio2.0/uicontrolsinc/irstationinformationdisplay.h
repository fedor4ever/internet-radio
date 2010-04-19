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


#ifndef C_CIRSTATIONINFORMATIONDISPLAY_H
#define C_CIRSTATIONINFORMATIONDISPLAY_H

#include <alf/alfcontrol.h>

#include "irmarqueeclet.h"
#include "irstationinformationdata.h"

class CAlfLayout;
class CAlfDeckLayout;
class CAlfGridLayout;
class CIRStationInformationData;

/**
 * Control that displays current station information.
 * 
 * Responsible for handling fade effects and transitions for the station information.
 */
class CIRStationInformationDisplay : public CAlfControl
    {

public:

    /**
     * Two-phased constructor.
     * 
     * The object's ownership is implicitly transferred to the supplied control group.
     * 
     * @param   aEnv                Alfred environment to use.
     * @param   aControlGroupId     Control group into which this control is added.
     * @param   aParentLayout       Parent layout to use.
     *
     * @return  The constructed object. Does not transfer ownership to the caller.
     */
    static CIRStationInformationDisplay* NewL( CAlfEnv& aEnv, TInt aControlGroupId,
    											CAlfLayout* aParentLayout = NULL );

    // <TUNING TEMP>
    ~CIRStationInformationDisplay();
   
    // </TUNING TEMP>
    
    /**
     * Sets the displayed data.
     * 
     * This causes an animation to take place that is determined by the supplied data's transition argument.
     * 
     * @param   aData   The data to set.
     */
    void SetDisplayedDataL( const CIRStationInformationData& aData );

    /**
     * Sets the RDS PS name.
     * 
     * Does not do anything if there is no data being displayed currently.
     *
     * @param   aRdsPsName      RDS PS name to set.
     */
    void RdsDataReceivedL( const TDesC& aRdsData );
    
    /**
     * Called when orienattion of the display has changed.
     * 
     * @param   aRdsDataDisplay   RDS data display.
     */
    void SetDisplayOrientation( TBool aLandscape ); 
    
    /**
     * Returns current fade style.
     * 
     * @return Fade style.
     */
    TIRFadeStyle FadeStyle(); 
 
// from base class CAlfControl

    void VisualLayoutUpdated( CAlfVisual& aVisual );
    TBool OfferEventL( const TAlfEvent& aEvent );
    
private:

    /**
     * Supported custom events.
     */
    enum TIRDisplayCustomEvents
        {
        EIRCustomEventDisplayFadeInCompleted,     /**< Event that is executed when a display has faded in. */
        EIRCustomEventDisplayFadeOutCompleted,    /**< Event that is executed when a display has faded out. */
        EIRCustomEventVisualFadeOutCompleted   /**< Event that is executed when a display has faded out. */
        };
    
private:

    /**
     * Constructor.
     */
    CIRStationInformationDisplay( CAlfLayout& aParentLayout );

    /**
     * Second-phase constructor.
     * 
     * @param   aEnv                Alfred environment to use.
     * @param   aControlGroupId     Control group into which this control is added.
     * @param   aParentLayout       Parent layout to use.
     */
    void ConstructL( CAlfEnv& aEnv, TInt aControlGroupId,CAlfLayout* aParentLayout );

    /**
     * Creates a new display that is used to show the supplied data.
     * 
     * @param   aData       Station information data to display.
     */
    void CreateDisplayL( const CIRStationInformationData& aData );

    /**
     * Fades out the current display.
     */
    void FadeOutDisplay( const CIRStationInformationData& aData );

    /**
     * Fades in the current display.
     */
    void FadeInDisplay( const CIRStationInformationData& aData );
    
    /**
     * Fades a single visual.
     * 
     * If the supplied visual is a text visual, its shadow opacity is also modified.
     * 
     * @param   aVisual     Visual to fade.
     * @param   aOpacity    Target opacity.
     */
    void FadeVisual( CAlfVisual& aVisual, const TAlfTimedValue& aOpacity );

    /**
     * Fades all visuals contained within the supplied layout.
     * 
     * If the layout contains other layouts, also their child visuals will be modified.
     * 
     * @param   aLayout     Layout whose visuals to fade.
     * @param   aOpacity    Target opacity.
     */
    void FadeLayout( CAlfLayout& aLayout, const TAlfTimedValue& aOpacity );

    /**
     * Starts to display RDS data.
     */
    void DisplayRdsDataL();
private:
    /**
     * Parent anchor layout.
     * Needed when creating own separate display for RDS data.
     */
    CAlfLayout& iParentAnchorLayout;

    /**
     * Base layout for the visuals.
     * Not owned.
     */
    CAlfDeckLayout* iBaseLayout;

    /**
     * Layout that is currently being displayed.
     * Contains all the visuals necessary to display any kind of station information data.
     * Not owned.
     */
    CAlfGridLayout* iLayout;

    /**
     * Layout for the name and index number.
     * Not owned.
     */
    CAlfGridLayout* iNameIndexLayout;

    /**
     * Layout for the RDS data.
     * Not owned.
     */
    CAlfGridLayout* iRdsDataLayout;

    /**
     * Text visual for the station name.
     * Not owned.
     */
    CAlfTextVisual* iName;

    /**
     * Text visual for the index number.
     * Not owned.
     */
    CAlfTextVisual* iIndex;

    /**
     * Text visual for the RDS PS name.
     * Not owned.
     */
    CAlfTextVisual* iRdsData;

    /**
     * Marquee controlet for the name.
     */
    TIRMarqueeClet iNameMarqueeClet;
    
    /**
     * Marquee controlet for the PS name (and Radio Text).
     */
    TIRMarqueeClet iRdsDataMarqueeClet;
    
    
    /**
     * RDS data text.
     * Owned.
     */
    RBuf iRdsDataText;

    /**
     * ETrue if orientation is landscape, otherwise EFalse.
     */
    TBool iLandscape;

    /**
     * Style of the logo fading.
     */
    TIRFadeStyle iFadeStyle;

    TBool iLandScapSetFalg;
    // </TUNING TEMP>
    
    };

#endif // C_CIRSTATIONINFORMATIONDISPLAY_H
