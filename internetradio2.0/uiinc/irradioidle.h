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
*  Name               : vrradioidle.h
*  Part of            : S60VisualRadio / UI
*  Description        :
*  Version    		  :
*
* ==============================================================================
*/

#ifndef IRRADIOIDLE_H_
#define IRRADIOIDLE_H_

// INCLUDES

#include <AknUtils.h>
#include <alf/alfenv.h>
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>

// FORWARD DECLARATIONS

class CAlfGradientBrush;
class CAlfViewportLayout;
class CAlfEnv;

// CONSTANTS

/** Commands for radio idle. */
enum TRadioIdleCommand
	{
	ESuspend = 0x4A,
	EStartup,
	EShutdown,
	ERefresh,
	EResetContent
	};

// CLASS DEFINITIONS

class MIRRadioIdleCommandObserver
	{
	public:
		/**
		* Called when change to the state of radio idle is wanted
		*/
		virtual void HandleCommandL( TRadioIdleCommand aCommand ) = 0;
	};

/**
* CVRRadioIdle class
*
* Class launches animated information in the defined subregion of the display after startup delay.
* Launching and radio idle running can be suspended by giving ESuspend command via
* MVRRadioIdleCommandObserver interface.  When suspended,
* background bitmap will replace the animated information and startup delay is reset to zero.
* The animation will start again after the delay.
*/
NONSHARABLE_CLASS(CIRRadioIdle) : public CAlfControl, public MIRRadioIdleCommandObserver
	{
public:		// Constructors and destructor

		/** Display orientations. */
		enum TOrientation
			{
			EPortrait = 0x4A,
			ELandscape,
			EOrientationNone
			};

		/**
	    * Epoc two-phased constructor.
	    * @param aAlfEnv The Alfred environment.
	    * @param aControlGroupId The control group that this control is to be added to.
	    * @param aParentLayout The parent layout for the visuals.
	    */
		static CIRRadioIdle* NewL( CAlfEnv& aEnv, TInt aControlGroupId, CAlfLayout* aParentLayout );
		/**
        * Destructor
        */
		virtual ~CIRRadioIdle();

public: 	// Functions from base classes
		/**
 	    * From CAlfControl, takes care of alfred event handling.
 	    */
		TBool OfferEventL( const TAlfEvent& aEvent );

public:		// New functions

		/** States of radio idle. */
		enum TRadioIdleState
			{
			ERunning = 0xA0,
			ESuspended,
			EStopped
			};

		/** Radio idle modes. */
		enum TRadioIdleMode
			{
			EOnlyBackgroundText = 0xB0,
			EBackgroundTextAndRadioIdleItems
			};

		/** Fade styles. */
		enum TRadioIdleFadeStyle
			{
			EFadeLeft = 0xC0,
			EFadeRight,
			EFadeStyleNone
			};

		/**
 	    * Adds radio idle item text to be used in the radio idle
 	    * @param aRadioIdleItemText Text of the radio idle item
 	    * @return Index of the added item
 	    */
		TInt AddRadioIdleContentL( const TDesC& aRadioIdleItemText );
		/**
 	    * Updates radio idle item text in specified index
 	    * @param aRadioIdleItemText Text of the radio idle item
 	    * @param aIndex Index of the radio idle item to be updated
 	    */
		void UpdateRadioIdleContent( const TDesC& aRadioIdleItemText, TInt aIndex );
		/**
	    * Removes all radio idle item texts
		*/
		void ResetRadioIdleContent();
		/**
	    * Returns number of radio idle content items
	    * @return content count
		*/
		TInt RadioIdleContentCount() const;
		/**
		 * Sets the fade style for radio idle.
		 * @param aFadeStyle  Fade style
		 */
		void SetIdleFadeStyle( TRadioIdleFadeStyle aFadeStyle );
		/**
	    * Sets the mode request for mode change. The mode
	    * will be changed as soon as possible.
	    * @param aRequestedMode Requested mode
		*/
		void SetModeChangeRequestL( TRadioIdleMode aRequestedMode );
		/**
	    * Retrieves the radio idle mode
		*/
		TRadioIdleMode Mode();
		/**
		 * Retrieves the radio idle state
		 */
		TRadioIdleState State();
		/**
 	    * Sets primary color for the radio idle
 	    * @param aColor The color that is used as primary color of radio idle
 	    */
		void SetPrimaryColor( TRgb aColor );
		/**
 	    * Sets secondary color for the radio idle
 	    * @param aColor The color that is used as secondary color of radio idle
 	    */
		void SetSecondaryColor( TRgb aColor );
		/**
 	    * Sets the background color of the radio idle
 	    * @param aColor The color that is used as the background color of the radio idle
 	    */
		void SetBackgroundColor( TRgb aColor );
		/**
 	    * Return orientation of the display
 	    */
		TOrientation Orientation();
		/**
 	    * Set orientation of the display
 	    * @param aOrientation The display orientation
 	    */
		void SetOrientation( TOrientation aOrientation );
		/**
		* Calculates display layout
	    */
		void UpdateLayout();
		/**
 	    * From MIRRadioIdleCommandObserver, takes care of radio idle command handling.
 	    * @param aCommand Radio idle command
 	    */
        void HandleCommandL( TRadioIdleCommand aCommand );
     
private:
		/**
		* C++ default constructor
		*/
		CIRRadioIdle();
		/**
	    * EPOC second phase constructor.
	    * @param aEnv  Reference to the Hitchcock environment instance.
	    * @param aControlGroupId  Id of the control group to which radio idle belongs.
	    * @param aParentLayout  Pointer to parent layout.
	    */
		void ConstructL( CAlfEnv& aEnv, TInt aControlGroupId, CAlfLayout* aParentLayout );

private:	// New functions

		/** Scroll directions for the radio idle item. */
		enum TRadioIdleItemScrollDirection
			{
			EScrollLeftToRight = 0xA0,
			EScrollRightToLeft,
			EScrollNone
			};

		/** Custom events for the radio idle. */
		enum TRadioIdleCustomEvent
			{
			ERadioIdleItemTransformationEnd = 0x5B,
			ERadioIdleBackgroundTextScrollEnd,
			EAfterStartupDelay,
			ERadioIdleChangeMode,
			ERadioIdleEventNone,
			ERadioIdleFadeOutCompleted
			};

		/**
 	    * Suspends radio idle and starts it again after startup delay
 	    */
		void SuspendRadioIdle();
		/**
 	    * Stops radio idle. Radio idle is set to stopped state.
 	    */
		void ShutdownRadioIdle();
		/**
 	    * Launches radio idle immediately.
 	    * The background bitmap fades out and at the same time radio idle fades in
 	    */
		void StartRadioIdleL();
		/**
		 * Starts radio idle again without reseting radio idle items.
		 */
		void RefreshL();
		/**
 	    * Creates radio idle with passed number of the radio idle items
 	    * @param aNumberOfRadioIdleItems Number of the radio idle items
 	    */
		void AddRadioIdleL( const TInt aNumberOfRadioIdleItems );
		/**
 	    * Creates the radio idle item to the passed radio idle view port.
 	    * @param aRadioIdle The view port of the radio idle, ownership now transferred
 	    * @param aRadioIdleItemText Text of the radio idle item
 	    * @param aRadioIdleItemHasBackground TBool to indicate background visibility
 	    * @param aBackgroundColor The background color of the radio idle item
 	    * @param aBackgroundOpacity The background opacity of the radio idle item
 	    */
		CAlfTextVisual* AddRadioIdleItemL( CAlfLayout* aRadioIdle,
										   const TDesC& aRadioIdleItemText,
										   const TDesC8& aTag,
									       TBool aRadioIdleItemHasBackground = EFalse,
										   const TRgb& aBackgroundColor = KRgbWhite,
										   const TReal32 aBackgroundOpacity = 0.5f );
		/**
 	    * Creates, initializes and animates the radio idle background text visual
 	    */
		void CreateBacgroundTextVisualL();
		/**
 	    * Initializes the radio idle items
 	    * @param aNumberOfRadioIdleItems Number of the radio idle items
 	    */
		void InitializeRadioIdleItemsL( const TInt aNumberOfRadioIdleItems );
		/**
 	    * Sets defined animation to the created radio idle items
 	    */
		void AnimateRadioIdleItems();
		/**
 	    * Retrieves text from randomly chosen radio idle item.
 	    * Transfers ownership.
 	    */
		HBufC* GetRandomRadioIdleContentL();
		/**
 	    * Sets color, opacity and scaling settings to the radio idle item
 	    * @param aRadioIdleItem The radio idle item, ownership not transferred
 	    * @param aRadioIdleItemText Text of the radio idle item
 	    * @param aTextStyleId Text style for text of the radio idle item
 	    */
		void SetRadioIdleItemSettingsL( CAlfTextVisual* aRadioIdleItem, const TDesC& aRadioIdleItemText,
										const TInt aTextStyleId );
		/**
 	    * Sets translating animations to the radio idle item
 	    * @param aRadioIdleItem The radio idle item, ownership not transferred
 	    * @param aRandomSeed
 	    */
		void SetRadioIdleItemAnimation( CAlfTextVisual* aRadioIdleItem );

		/**
	    * Sets the radio idle mode
		*/
		void SetMode( TRadioIdleMode aMode );
		/**
	    * Fades in radio idle
	    * @param aNow If ETrue shows radio idle immediately otherwise
	    * fades with defined fading duration
		*/
		void RadioIdleFadeIn( TBool aNow = EFalse );
		/**
	    * Fades out radio idle
	    * @param aNow If ETrue hides radio idle immediately otherwise
	    * fades with defined fading duration
		*/
		void RadioIdleFadeOut( TBool aNow = EFalse );
		/**
 	    * Sets and starts scrolling animation to CAlfTextVisual.
 	    * The visual object is scrolled from current position to
 	    * right side of the display
 	    * @param aVisual a pointer to the visual object, ownership not transferred
 	    */
		void ScrollToRight( CAlfTextVisual* aVisual ) const;
		/**
 	    * Sets and starts scrolling animation to CAlfTextVisual.
 	    * The visual object is scrolled from current position to
 	    * left side of the display
 	    * @param aVisual a pointer to the visual object, ownership not transferred
 	    */
		void ScrollToLeft( CAlfTextVisual* aVisual ) const;
		/**
	    * todo
 	    * @param aVisual a pointer to the visual object, ownership not transferred
 	    * @param aTransitionTime Duration for reaching the target.
 	    * @param aEventAfterScrollingEnd todo custom even that will be sent when transition time is end
 	    */
		void SetScrollingToBackgroundText( CAlfTextVisual* aVisual,
							    TInt aTransitionTime,
							    TRadioIdleCustomEvent aEventAfterScrollingEnd = ERadioIdleEventNone ) ;
		/**
	    * Sets fade-in animation to the CAlfVisual.
	    * @param aVisual a pointer to the visual object, ownership not transferred
	    * @param aTime Time duration after the visual object has been faded in. Fading duration in milliseconds
	    * @param aOpacity Target opacity value
	    */
		void FadeIn( CAlfVisual* aVisual, TInt aFadingTime, TReal32 aOpacity = 1.0f ) const;
		/**
	    * Sets fade-out animation to the CAlfVisual.
	    * @param aVisual a pointer to the visual object, ownership not transferred
	    * @param aTime Time duration after the visual object has been faded out. Fading duration in milliseconds
	    * @param aOpacity Target opacity value
	    */
		void FadeOut( CAlfVisual* aVisual, TInt aFadingTime, TReal32 aOpacity = 0.0f ) const;
		/**
 	    * Translates the visual object with passed arguments
 	    * @param aVisual a pointer to the visual object, ownership not transferred
 	    * @param aX Translation value in the horizontal direction
 	    * @param aY Translation value in the vertical direction
 	    */
		void Translate( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, 
						const TAlfTimedValue& aY );
		/**
 	    * Scales the visual object with passed arguments
 	    * @param aVisual a pointer to the visual object, ownership not transferred
 	    * @param aX Scaling value in the horizontal direction
 	    * @param aY Scaling value in the vertical direction
 	    */
		void Scale( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, const TAlfTimedValue& aY );
		/**
 	    * Removes all transformations of the visual object such as the scaling and translating.
 	    * @param aVisual a pointer to the visual object, ownership not transferred
 	    */
		void LoadTextVisualIdentity( CAlfTextVisual* aTextVisual );
	    /**
	     * Converts CAlfVisual reference to pointer
	     */
	    CAlfVisual* GetPointer( CAlfVisual& aRef );	    
        /**
         * Callback for radio idle item transformation end.
         */
		void HandleRadioIdleItemTransformationEndEventL( const TAlfEvent& aEvent );
		/**
		 * Callback for background text transformation end.
		 */
        void HandleBackgroundTextScrollEndEventL( const TAlfEvent& aEvent );
		/**
		 * Randomly modifies a component of a radio idle text color.
		 */
        TInt DeviateColorComponent( TInt aBaseValue );
        
private:	// Data

		/** Display orientation */
	    TOrientation iOrientation;

	    /** Visual of radio idle content. Not owned. */
	    CAlfViewportLayout* iViewport;
	    /** Parent layout. Not owned. */
	    CAlfLayout* iParentLayout;
	    /** Layout for radio idle */
	    TAknLayoutRect iRadioIdleLayout;

	    /** Fade style for radio idle */
	    TRadioIdleFadeStyle iFadeStyle;
	    /** States of radio idle */
		TRadioIdleState iRadioIdleState;
		/** Keeps track of the current radio idle mode */
		TRadioIdleMode  iRadioIdleMode;
		/** Keeps track of the requested mode */
		TRadioIdleMode  iRadioIdleRequestedMode;
	    /** Counts the items that have finished their transformations for mode change */
		TInt iItemsTransformationEndEventCounter;

	    /** Color to be used for the radio idle items */
	    TRgb iRadioIdlePrimaryColor;
	    /** Color to be used for the radio idle items */
	    TRgb iRadioIdleSecondaryColor;
	    /** Scroll direction for the radio idle item */
	    TRadioIdleItemScrollDirection iScrollDirection;

		/** Array for the strings of the radio idle items. Owned. */
        CDesCArrayFlat* iRadioIdleItemArray;
		/** Array for the text styles of the radio idle items */
        RArray<TInt> iTextStyleIdArray;

        /** Background of radio idle. Not owned.*/
		CAlfGradientBrush* iRadioIdleBackgroundGradientBrush;
        /** Text style id of the radio idle background text */
        TInt iRadioIdleItemBackgroundTextStyleId;
        /** Color that is used as the background color of radio idle */
	    TRgb iRadioIdleBackgroundColor;
	    /** Scroll direction for the radio idle background text */
	    TRadioIdleItemScrollDirection iBackgroundTextScrollDirection;
	    /** Indicates whether fading is ongoing */
	    TBool iFading;
	    /** Indicates whether there was a startup try when fading out radio idle */
	    TBool iDelayedStartup;

	};

#endif /*VRRADIOIDLE_H_*/
