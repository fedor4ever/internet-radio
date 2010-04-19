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
* Description:  Manages MCP plugins, and content publishing.
*
*/


#ifndef M_MCPPLUGINOBSERVER_H
#define M_MCPPLUGINOBSERVER_H

class CLiwDefaultMap;
class CMCPPlugin;


/**
 * Music Widget Image Destinations.
 * 9.1 Music Widget has two panels, The left panel contains an image:
 * TMCPImageDestination:EMWImage1, the right panel contains three text
 * rows: TMCPTextDestination:EMWText1, TMCPTextDestination:EMWText2, 
 * TMCPTextDestination:EMWText3. Actions can be installed into this
 * two panels to the triggers: TMCPTriggerDestination:EMWTrigger1, 
 * TMCPTriggerDestination:EMWTrigger2.
 */
enum TMCPImageDestination
    {
     EMusicWidgetImage1 = 1000//do not change
    };

/**
 * Music Widget Text Destinations.
 * 9.1 Music Widget has two panels, The left panel contains an image:
 * TMCPImageDestination:EMWImage1, the right panel contains three text
 * rows: TMCPTextDestination:EMWText1, TMCPTextDestination:EMWText2, 
 * TMCPTextDestination:EMWText3. Actions can be installed into this
 * two panels to the triggers: TMCPTriggerDestination:EMWTrigger1, 
 * TMCPTriggerDestination:EMWTrigger2.
 */
enum TMCPTextDestination
    {
     EMusicWidgetText1 = 2000//do not change
     ,EMusicWidgetText2
     ,EMusicWidgetText3
    };

/**
 * Music Widget Action Destinations.
 * 9.1 Music Widget has two panels, The left panel contains an image:
 * TMCPImageDestination:EMWImage1, the right panel contains three text
 * rows: TMCPTextDestination:EMWText1, TMCPTextDestination:EMWText2, 
 * TMCPTextDestination:EMWText3. Actions can be installed into this
 * two panels to the triggers: TMCPTriggerDestination:EMWTrigger1, 
 * TMCPTriggerDestination:EMWTrigger2.
 */
enum TMCPTriggerDestination
    {
     EMusicWidgetTrigger1 = 3000//do not change
     ,EMusicWidgetTrigger2
    };

/**
 *  Observer interface, to be implemented by the Music Content Publisher.
 *  This interface is to be used to inform the music harves of changes on the
 *  plugin that need to be propagated to the Music Widget UI. 
 *  @since S60 5.0
 */
class MMCPPluginObserver
    {
public:
    
    /**
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aImagePath Source path of the Image.
     */
    virtual void PublishImageL(CMCPPlugin* aPlugin,
            TMCPImageDestination aDestination, const TDesC& aImagePath) = 0;

    /**
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aBitmapHandle Bitmap handle to the image.
     */
    virtual void PublishImageL(CMCPPlugin* aPlugin, 
            TMCPImageDestination aDestination, TInt aBitmapHandle) = 0;

    /**
     * Called when a text needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the text is to be placed.
     * @param aText Descriptor containing the text.
     */
    virtual void PublishTextL( CMCPPlugin* aPlugin, 
            TMCPTextDestination aDestination, const TDesC& aText ) = 0;

    /**
     * Called when an action needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Destination trigger that should launch the action.
     * @param aTrigerMap LIW Map containint the action that can be interpreted
     * by the specified action handler plugin.
     */
    virtual void PublishActionL( CMCPPlugin* aPlugin, 
            TMCPTriggerDestination aDestination, 
            CLiwDefaultMap* aTrigerMap ) = 0;
    
    /**
     * Called when a plugin becomes active.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     */
    virtual void BecameActiveL( CMCPPlugin* aPlugin ) = 0;

    };

#endif /*MCPPLUGINOBSERVER_H*/
