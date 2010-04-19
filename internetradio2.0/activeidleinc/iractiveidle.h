/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The content publisher for Internet Radio Active Idle plugin
*
*/


#ifndef IRACTIVEIDLE_H
#define IRACTIVEIDLE_H

#include <aicontentpublisher.h>
#include <aieventhandlerextension.h>
#include <aipropertyextension.h>

#include "iractiveidleengnotifyhandler.h"

class CIRActiveIdleEng;
class CIRActiveIdleStateDetector;
class CIRMetaData;
class MAiContentItemIterator;
class MAiContentObserver;

#define KMAXLENGTH 256

/**
 *  Internet Radio Active Idle plugin
 *
 *  @lib iractiveidle.lib
 */
class CIRActiveIdle : public CAiContentPublisher, 
                      public MAiPropertyExtension,
                      public MAiEventHandlerExtension,
                      public MIRActiveIdleEngNotifyHandler
    {
public:

    /**
     * Two-phased constructor.
     */
    static CIRActiveIdle* NewL();

    /**
     * Destructor.
     */
    /*lint -save -e1509 (Warning -- base class destructor for class 'MAiPropertyExtension' is not virtual -- Effective C++ #14)
                        (Warning -- base class destructor for class 'MAiEventHandlerExtension' is not virtual -- Effective C++ #14)*/
    virtual ~CIRActiveIdle();
    /*lint -restore*/

	// from base class CAiContentPublisher
	/**
     *void Resume( TAiTransitionReason aReason )
     *To actively publish data to observers. 
     *@param aReason Defines set of reasons for plug-in state change.
     */
    void Resume( TAiTransitionReason aReason );
    
    /**
     *void Suspend( TAiTransitionReason aReason )
     *Cancels outstanding asynchronous operations
     *@param aReason Defines set of reasons for plug-in state change.
     */
    void Suspend( TAiTransitionReason aReason );
    
    /**
     *void Stop( TAiTransitionReason aReason )
     *Frees all memory and CPU resources and close all open files.
     *@param aReason Defines set of reasons for plug-in state change.
     */
    void Stop( TAiTransitionReason aReason );
    
    /**
     *void SubscribeL( MAiContentObserver& aObserver )
     *Adds the content observer / subscriber to plug-in.
     *@param aObserver The observer to be notified of the changes in the keys.
     */
    void SubscribeL( MAiContentObserver& aObserver );
    
    /**
     *void ConfigureL( RAiSettingsItemArray& aSettings )
     *Configures the plug-in.
     *@param aSettings setting items defined in the UI definition.
     *This plugin takes ownership of the MAiPluginSettings objects 
     *in the array.
     */
    void ConfigureL( RAiSettingsItemArray& aSettings );
    
    /**
     *TAny* Extension( TUid aUid )
     *Returns interface extension.
     *@param aUid 
     *@return Returns pointer to extension type
     */
    TAny* Extension( TUid aUid );

	// from base class MAiPropertyExtension
	/**
     *TAny* GetPropertyL( TInt aProperty )
     *Reads a plugin property.
     *@param aProperty identification of property.
     *@return Pointer to property value
     */
    TAny* GetPropertyL( TInt aProperty );
    
    /**
     *void SetPropertyL( TInt aProperty, TAny* aValue )
     *Sets a plugin property
     *@param aProperty  identification of property.
     *@param aValue contains pointer to property value.
     */
    void SetPropertyL( TInt aProperty, TAny* aValue );

	// from base class MAiEventHandlerExtension
	/**
     *void HandleEvent( TInt aEvent, const TDesC& aParam )
     *Sets a plugin property
     *@param aEvent unique identifier of event from plug-in content model.
     *@param aParam Parameters associated with event
     */
    void HandleEvent( TInt aEvent, const TDesC& aParam );

// from base class MIRActiveIdleEngNotifyHandler

    void HandleChannelChangeL();
    void HandleMetaDataReceivedL( const CIRMetaData& aMetaData );
    void HandleVolumeChangeL( const TInt aVolume );
    void HandlePlayStopL( TIRStopPlayState aState );
    void HandleApplicationRunningStateChangeL(  const TIRPSApplicationRunningState aRunningState );
    void HandleBufferingStateL(TIRBufferingState aState );
    
   private:

    /**
     * Constructor.
     */
    CIRActiveIdle();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Initialize resource file for loading resources.
     */
    void InitializeResourceLoadingL();

    /**
     * Loads the required resources.
     */
    void LoadResourcesL();

    /**
     * Cleans all data fields from plugin.
     */
    void Clean();

    /**
     * Starts an application.
     * @param aUid The uid value of the application
     * @param aCommandLine The command line (if any) to be passed to the process
     */
    void StartApplicationL( const TUid& aUid, const TDesC8& aCommandLine );

private: // data

    /** IR's Active Idle engine. */
    CIRActiveIdleEng*                   iEngine;

    /** 
     * Detects Internet Radio process termination
     * Owned.
     */
    CIRActiveIdleStateDetector*         iIRTerminationDetector;
    
    /**
     * Current meta data information.
     * Owned.
     */
    CIRMetaData*                        iMetaData;
     
    /** Observers that are informed about changes state or new content available. */
    RPointerArray<MAiContentObserver>   iObservers;
    /** */
    TAiPublisherInfo                    iInfo;
    /** */
    MAiContentItemIterator*             iContent;
    /** */
    MAiContentItemIterator*             iResources;
    /** */
    MAiContentItemIterator*             iEvents;
    
    /** Active Idle's resource offset. */
    TInt                                iActiveIdleResourceOffset;
    /** Internet Radio application running state */
    TIRPSApplicationRunningState        iRunningState;
    
 	// Meta data info
    TBuf 	<KMAXLENGTH> iChannelName;
    TInt 	iPresetIndex;
    
    //Application volume level
    enum TIRPSVolumeLevel
    	{
    	EVolumeLevel0 = 0,
	    EVolumeLevel1,      
	    EVolumeLevel2,   
	    EVolumeLevel3,   
	    EVolumeLevel4,   
	    EVolumeLevel5,   
	    EVolumeLevel6,   
	    EVolumeLevel7,   
	    EVolumeLevel8,   
	    EVolumeLevel9,   
	    EVolumeLevel10                    
	    };
    
    TInt                                iLastSetVolume;
    };

#endif // IRACTIVEIDLE_H
