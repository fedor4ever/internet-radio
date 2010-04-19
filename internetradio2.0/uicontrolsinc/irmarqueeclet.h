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


#ifndef T_TIRMARQUEECLET_H
#define T_TIRMARQUEECLET_H

#include <alf/alfeventhandler.h>

class CAlfControl;
class CAlfViewportLayout;
class CAlfTextVisual;
/**
 * Small controlet to implement marquee effects to visuals.
 * 
 * Responsible for managing the supplied viewport layout to achieve a marquee effect.
 * Provides events to the control about scroll status changes.
 */
class TIRMarqueeClet : public MAlfEventHandler
    {

public:
    
    /**
     * Supported scroll styles.
     */
    enum TIRScrollStyle
        {
        /**
         * Scroll is performed only once.
         * When scrolling is completed, the viewport is set to its original position.
         * This is the default scroll style if no other is explicitly set.
         */
        EIRScrollStyleOnce,
        /**
         * Scroll is continuous scroll.
         */
        EIRScrollStyleLoop
        };

    /**
     * Supported scroll directions.
     */
    enum TIRScrollDirection
        {
        /**
         * Scrolling is performed to left (western).
         * This is the default scroll direction if no other is explicitly set.
         */
        EIRScrollDirectionLeft,
        /**
         * Scrolling is performed to right (arabic).
         */
        EIRScrollDirectionRight
        };

public:

    /**
     * Constructor.
     */
    TIRMarqueeClet();

    /**
     * Destructor.
     */
    ~TIRMarqueeClet();

    /**
     * Sets the required information for this marquee controlet.
     * 
     * @param   aViewport       Viewport layout to use.
     * @param   aTextVisual     Text visual to use.
     */
    void Set( CAlfViewportLayout& aViewport, CAlfTextVisual& aTextVisual );

    /**
     * Sets scroll speed in pixels per second.
     * 
     * The actual scroll time is calculated dynamically by the controlet to achieve
     * smooth motion.
     * 
     * @param   aScrollSpeed    Pixels per second to scroll.
     */
    void SetScrollSpeed( TInt aScrollSpeed );

    /**
     * Sets the scroll direction.
     * 
     * @param   aScrollDirection    Scroll direction to use.
     */
    void SetScrollDirection( TIRScrollDirection aScrollDirection );

    /**
     * Sets the scroll style.
     * 
     * @param   aScrollStyle    Scroll style to use.
     */
    void SetScrollStyle( TIRScrollStyle aScrollStyle );

    /**
     * Starts scrolling.
     * 
     * Notifications of scroll start and stop events are sent to the registered control.
     * 
     * @param   aDelay      Delay in milliseconds after which the scroll is started.
     * @param   aSnooze     Snooze period in milliseconds after scroll is completed.
     *                      The use of the snooze period is dependant on the scroll style.
     *                      When the scroll style is <code>EVRScrollStyleOnce</code>, the snooze
     *                      period indicates the amount of time after scrolling is completed before
     *                      the control is notified of the scroll complete event.
     */
    void StartL( TInt aDelay = 0, TInt aSnooze = 0 );
    
// from base class MAlfEventHandler
    
    TBool OfferEventL( const TAlfEvent& aEvent );

private:
    
    /**
     * Supported scroll notification events.
     * Notification about these events are sent to the registered control when they occur.
     * Pointer to the viewport layout for which the event completed is passed as custom event data.
     */
    enum TIRScrollEvent
        {
        /**
         * Notification that is sent when the scroll starts.
         * This is sent after the delay period, if any, expires.
         */
        EIRCustomEventScrollStarted = 0x00002000,
        /**
         * Notification that is sent when the scroll has completed.
         * This is sent after scrolling is completed and after the snooze
         * period has passed.
         */
        EIRCustomEventScrollCompleted
        };

    /**
     * Viewport layout to manage.
     * Not owned.
     */
    CAlfViewportLayout* iViewport;

    /**
     * Text visual.
     * Not owned.
     */
    CAlfTextVisual* iTextVisual;

    /**
     * Text visual scroll size.
     */
    TSize iScrollSize;

    /**
     * Scroll speed in pixels per second.
     */
    TInt iScrollSpeed;

    /**
     * Scroll direction to use.
     */
    TIRScrollDirection iScrollDirection;

    /**
     * Scroll style to use.
     */
    TIRScrollStyle iScrollStyle;

    /**
     * The snooze period.
     */
    TInt iSnoozeTime;
    
    /**
     * Point of origin for the viewport.
     */
    TAlfRealPoint iOrigin;

    /**
     * The width of the original text.
     */
    TInt iOriginalTextWidth;

    };

#endif // T_TVRMARQUEECLET_H
