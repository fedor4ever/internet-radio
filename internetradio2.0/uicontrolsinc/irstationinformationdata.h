/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Station information data container
*
*/


#ifndef C_CIRSTATIONINFORMATIONDATA_H
#define C_CIRSTATIONINFORMATIONDATA_H

#include <e32base.h>

/** Possible directionalities of the fade. */
enum TIRFadeStyle
    {
    EIRFadeUnknown,         /**< Unknown directionality. This means that the data fades in or out with no position changes. */
    EIRFadeLeftToRight,     /**< Position changes from left to right. */
    EIRFadeRightToLeft,     /**< Position changes from right to left. */
    EIRFadeTopToBottom,     /**< Position changes from top to bottom. */
    EIRFadeBottomToTop,     /**< Position changes from bottom to top. */
    EIRFadeNoFade           /**< No fading is performed. */
    };

/** Possible display styles. */
enum TIRDisplayStyle
    {
    EIRDisplaySingleLine,   /**< Single line display. */
    EIRDisplayDoubleLine,   /**< Double line display. */
    EIRDisplayTripleLine,   /**< Triple line display. */
    EIRDisplayTuning,       /**< Tuning display. */
    EIRDisplayNoDisplay     /**< No display used. */
    };

/**
 * Station information data container class.
 *
 * Holds all relevant "cacheable" station information data that is used to display the transitions between them
 * in the station information display.
 *
 * @since   S60 v3.2
 */
class CIRStationInformationData : public CBase 
    {

public:

    /**
     * Static two-phased constructor.
     *
     * @since   S60 v3.2
     * @param   aCoeEnv     Control environment to use.
     * @return  The created object. Ownership is transferred.
     */
    static CIRStationInformationData* NewL( CCoeEnv& aCoeEnv );

    /**
     * Destructor.
     *
     * @since   S60 v3.2
     */
    ~CIRStationInformationData();

    

    /**
     * Sets the name.
     *
     * @since   S60 v3.2
     * @param   Name to set.
     */
    void SetNameL( const TDesC& aName );

    /**
     * Sets the RDS PS name.
     *
     * @since   S60 v3.2
     * @param   RDS PS name to set.
     */
    void SetRdsPsNameL( const TDesC& aRdsPsName );

    /**
     * Sets the index.
     *
     * Formats the index to a descriptor.
     *
     * @since   S60 v3.2
     * @param   aIndex  Index to set.
     */
    void SetIndexL( TInt aIndex );

    

    /**
     * Sets the visual service enabled or disabled.
     *
     * @since   S60 v3.2
     * @param   aServiceEnabled     <code>ETrue</code> if service is enabled, <code>EFalse</code> otherwise.
     */
    void SetServiceEnabled( TBool aServiceEnabled );

    /**
     * Sets the display style in use.
     *
     * @since   S60 v3.2
     * @param   aDisplayStyle   Display style to use.
     */
    void SetDisplayStyle( TIRDisplayStyle aDisplayStyle );

    /**
     * Sets the fade directionality.
     *
     * @since   S60 v3.2
     * @param   aFadeDirectionality     Directionality of the fade.
     */
    void SetFadeDirectionality( TIRFadeStyle aFadeDirectionality );

    /**
     * Returns the frequency as a descriptor.
     *
     * @since   S60 v3.2
     * @return  Frequency as a descriptor.
     */
    const TDesC& Frequency() const;

    /**
     * Returns the name.
     *
     * @since   S60 v3.2
     * @return  Name of the data.
     */
    const TDesC& Name() const;

    /**
     * Returns the RDS PS name.
     *
     * @since   S60 v3.2
     * @return  RDS PS name of the data.
     */
    const TDesC& RdsPsName() const;

    /**
     * Returns the index as a descriptor.
     *
     * @since   S60 v3.2
     * @return  Index as a descriptor.
     */
    const TDesC& Index() const;

    /**
     * Returns the location.
     *
     * @since   S60 v3.2
     * @return  Location of the data.
     */
    const TDesC& Location() const;

    /**
     * Returns whether or not visual service is enabled.
     *
     * @since   S60 v3.2
     * @return  <code>ETrue</code> if visual service is enabled, <code>EFalse</code> otherwise.
     */
    TBool ServiceEnabled() const;

    /**
     * Returns the display style.
     *
     * @since   S60 v3.2
     * @return  Display style.
     */
    TIRDisplayStyle DisplayStyle() const;

    /**
     * Returns the fade directionality.
     *
     * @since   S60 v3.2
     * @return  Fade directionality.
     */
    TIRFadeStyle FadeDirectionality() const;

private:

    /**
     * Constructor.
     *
     * @since   S60 v3.2
     * @param   aCoeEnv     Control environment to use.
     */
    CIRStationInformationData( CCoeEnv& aCoeEnv );

    /**
     * Second-phase constructor.
     *
     * @since   S60 v3.2
     */
    void ConstructL();

private:

    /**
     * Frequency.
     * Own.
     */
    RBuf iFrequency;

    /**
     * Name.
     * Own.
     */
    RBuf iName;

    /**
     * RDS PS name.
     * Own.
     */
    RBuf iRdsPsName;

    /**
     * Index.
     * Own.
     */
    RBuf iIndex;

    /**
     * Location.
     * Own.
     */
    RBuf iLocation;
    
    /**
     * <code>ETrue</code> if visual service is enabled, <code>EFalse</code> otherwise.
     */
    TBool iServiceEnabled;

    /**
     * Display style.
     */
    TIRDisplayStyle iDisplayStyle;

    /**
     * Directionality of the fade.
     */
    TIRFadeStyle iFadeDirectionality;    

    /**
     * Control environment.
     */


    };

#endif // C_CIRSTATIONINFORMATIONDATA_H

