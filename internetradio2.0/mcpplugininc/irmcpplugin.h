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
* Description:  Plugin used by Music content publisher to publish data and actions
*                to Home Screen/Matrix Menu/Any component requiring them.
*
*/


#ifndef IRMCPPLUGIN_H
#define IRMCPPLUGIN_H

#include "mcpplugin.h"
#include "mcppluginobserver.h"
#include "iractiveidleengnotifyhandler.h"

class CIRActiveIdleEng;

class CIRMetaData;
#define KMAXLENGTH 256


/**
 * IR Content Publisher plugin is publishes data and actions for 
 * Home screen and Matrix menu.
 * 
 * The plugin uses existing Active idle engine for getting information from VRA. 
 * 
 * The plugin is currently loaded by Music Content Publisher. 
 */
NONSHARABLE_CLASS( CIRMCPPlugin ) : public CMCPPlugin,
                                    public MIRActiveIdleEngNotifyHandler
    {
public:
    /**
     * Two-phase constructor. 
     * 
     * @param aObserver The plugin observer providing also the publishing API
     * @return A pointer to new instance of CIRMCPPlugin 
     */
    static CIRMCPPlugin* NewL( MMCPPluginObserver* aObserver );
    
    /**
     * Destructor.
     */
    ~CIRMCPPlugin();

// from base class CMCPPlugin
    void Deactivate();

// from base class MIRActiveIdleEngNotifyHandler    
   void HandleChannelChangeL();
    void HandleMetaDataReceivedL( const CIRMetaData& aMetaData );
    void HandleVolumeChangeL( const TInt aVolume );
    void HandlePlayStopL( TIRStopPlayState aState );
    void HandleApplicationRunningStateChangeL(  const TIRPSApplicationRunningState aRunningState );
    void HandleBufferingStateL(TIRBufferingState aState );

private:

    /**
     * Installs (publishes) a given VR Action Handler command to given destination.
     * 
     * @param aCommand The command to be installed to given destination.
     * @param aDestination The destination where the given command is installed.
     */
    void InstallIRCommandActionL( const TDesC& aCommand, TMCPTriggerDestination aDestination );

    /**
     * Activates the plugin and publishes the actions and information accordingly.
     */
    void ActivateL();
    
    
    
    /**
    * Publishes the actions and information of last played station.
	*/
	void PublishLastPlayedWidgetItemsL();
    
    
    
private:
    /**
     * C++ constructor.
     * 
     * @param aObserver The plugin observer providing also the publishing API
     */
    CIRMCPPlugin( MMCPPluginObserver* aObserver );
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();
    
    /**
     * Searches and adds the plugin's resource file to the environment.
     */
    void InitializeResourceLoadingL();

private:
    /**
     * The observer providing the publishing API.
     * Not owned.
     */
    MMCPPluginObserver* iObserver;

    /**
     * Active idle engine used for reading VRA states.
     * Owned.
     */
    CIRActiveIdleEng* iEngine;
    
    /** 
     * Last played text.
     * Owned. 
     */
    HBufC* iLastPlayedText;

    /**
     * State indicator, the plugin can be active or inactive. 
     */
    TBool iActive;
    
    /** resource offset. */
    TInt iResourceOffset;
    
    /**
     * Radio Text overrides PS name, and we have both available.
     * So we need to keep track which one is being currently published.  
     */
    TBool iShowingRDSRadioText;
    
    //Added by Snigdha
    
     /**
     * Current meta data information.
     * Owned.
     */
    CIRMetaData*  iMetaData;
    
     // Meta data info
    TBuf 	<KMAXLENGTH> iChannelName;
    TInt 	iPresetIndex;
    
    /** Internet Radio application running state */
    TIRPSApplicationRunningState        iRunningState;
    
    TInt iPlayingState;
    
      /** 
     * Last played text.
     * Owned. 
     */
    HBufC* iBufferingText;


    };

#endif /* IRMCPPLUGIN_H */
