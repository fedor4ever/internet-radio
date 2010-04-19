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
* Description:  Base Alfred control for the now playing view
*
*/


#ifndef C_CIRNOWPLAYINGVIEWCONTAINER_H
#define C_CIRNOWPLAYINGVIEWCONTAINER_H

#include "internetradioconfigurationflags.hrh"

#ifdef MUL_USE_COMMON_COMPONENTS

#include <gesturecontrol.h>
#include <gestureobserver.h>

#else

#include <alf/alfcontrol.h>

#endif

class CAlfEnv;	
class CAlfAnchorLayout;
class CAlfDeckLayout;
class CAlfImageVisual;
class CAlfTextVisual;
class TAlfImage;
class CIRStationInformationDisplay;
class CIRStationLogoControl;
class CIRNowPlayingView;
class MTouchFeedback;
class CIRRadioIdle;

#ifdef MUL_USE_COMMON_COMPONENTS
using namespace GestureHelper;
#endif

/**
 * Now playing container
 *
 * @since   S60 v3.2
 */
#ifdef MUL_USE_COMMON_COMPONENTS
class CIRNowPlayingViewContainer : public CGestureControl, public MGestureObserver
#else
class CIRNowPlayingViewContainer : public CAlfControl
#endif
    {
	public:
	/**
	* Two-phased constructor.
	*
	* Adds the control to the control group
	*
	* @param   aEnv    Alfred environment to use.
	* @return  The created object. Ownership is not transferred.
	*/
	static CIRNowPlayingViewContainer* NewL( CAlfEnv& aEnv,CIRNowPlayingView* instance );

   	/* Two-phased constructor.
	*
	* Adds the control to the control group
	*
	* @param   aEnv    Alfred environment to use.
	* @return  The created object. Ownership is not transferred.
	*/
	static CIRNowPlayingViewContainer* NewLC( CAlfEnv& aEnv,CIRNowPlayingView* aInstance );

	/**
	* Destructor.
	*/
	~CIRNowPlayingViewContainer();

	/**
	* Returns a reference to the station logo control.
	*
	* @return  Reference to the station logo control.
	*/
	CIRStationLogoControl& StationLogoControl();
	
	/**
     * Returns a reference to the radio idle control.
     * 
     * @return  Reference to the radio idle control.
     */ 
    CIRRadioIdle& RadioIdle();
	/**
	* Set the text in the visual.
	*
	* @param   aText   is the Bitrate Text
	*/
	void SetBitrateL( const TDesC& aText );
	
	/**
	* Set the text in the visual.
	*
	* @param   aText   RDS text
	*/

	void SetByteCounterL( const TDesC& aText );
	
	/**
	* StationInformationDisplay()
	* reyurns the Instance of the StationInformationDisplay
	*/
	
	CIRStationInformationDisplay& StationInformationDisplay();
	
	/**
	* Layouts this control.
	*
	* TODO: Move this method to be private when CAlfControl is notified
	* of portrait/landscape events.
	*/
	void LayoutControl();

	
	void SetLayoutAnchor( TInt aOrdinal, TInt aResourceId );
	/**
     * Draw()
     * Function used to Draw the View
     */
	void DrawViewL(TBool aLogoStatus) ;
	// from base class MGestureObserver

	#ifdef MUL_USE_COMMON_COMPONENTS
	// from base class MGestureObserver
	    void HandleGestureL( const MGestureEvent& aEvent );
	#else
	// from base class CAlfControl
	    TBool OfferEventL( const TAlfEvent& aEvent );
	#endif

private: // Methods

	/**
	* Constructor.
	*/
	CIRNowPlayingViewContainer(CAlfEnv& aEnv);

	/**
	* Second-phase constructor.
	*
	* @param   aEnv    Alfred environment to use.
	*/
	void ConstructL(CIRNowPlayingView* instance);

	/**
	*TriggerCommandL.()
	*
	* triggering the Commands to the view
	*/
	void TriggerCommandL(TInt iCommandId);
    
    /**
	* KeyHitL()
	*
	* used for handling Key Events
	*/
	TBool KeyHitL( TInt aScanCode);
    /**
     * Touch feedback.
     * Not owned.
     */
    MTouchFeedback* iFeedback;

private: // Members
    
    /**
     * The Alfred environment
     */
    CAlfEnv& iAlfEnv;
	
	/**
	* Logo control.
	* Not owned.
	*/
	CIRStationLogoControl* iLogoControl;
	
	/**
	* The base layout to which all controls are layouted.
	* Not owned.
	*/
	CAlfAnchorLayout* iLayout;

	/**
	* Station information display.
	* Not owned.
	*/
	CIRStationInformationDisplay* iStationInformationDisplay;

	/**
	* RDS text visual.
	* Not owned.
	*/
	CAlfTextVisual* iBitrate;

	/**
	* RDS text visual.
	* Not owned.
	*/
	
	CAlfTextVisual* iByteCounter;


	/**
	* iNowPlayingView1
	* Used for Handling KeyEvents
	*/
	CIRNowPlayingView* iNowPlayingView;
	
	/**
     * Radio Idle.
     * Owned.
     */
    CIRRadioIdle* iRadioIdle;
    };


#endif // C_CIRNOWPLAYINGVIEWCONTAINER_H
