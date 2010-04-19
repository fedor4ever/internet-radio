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
* Description:  Implementation of streaming and playback on ASF channel url
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 1, Fri Sep 19 13:00:00 2008 by Rohit
*  Ref:
*  Created
*
*  </ccm_history>
* ============================================================================
*/

#ifndef IRASFPLAYER_H
#define IRASFPLAYER_H

//#include "ircontrolparams.h"
//#include "irmediaenginedef.h"
//#include "irmediaengineinterface.h"
#include <mdaaudiosampleplayer.h>	// MMdaAudioPlayerCallback

// Forward declarations
class MIRCtrlCmdObserver;
class MIRStreamSourceObserver;
class CIRMetaData;

//==============================class declaration CIRAsfPlayer==============================

/*
MIRCtrlCmdObserver functions invoked by AsfPlayer
	FetchVolume

MIRStreamSourceObserver functions invoked by AsfPlayer
	Asf_ErrorConnecting
	Asf_ConnectionEstablished
	Asf_UpdateProgress
	Asf_HandleMetaDataReceivedL
*/


/**
 * This class is the implementation of Asf format streaming and playback
 * 
 * @code
 * @endcode
 */

NONSHARABLE_CLASS(CIRAsfPlayer) : public MMdaAudioPlayerCallback,
								  public MAudioLoadingObserver 
	{
public:
									//Member Functions
	/**
	 * Function : NewL
	 * function returns an instance of CIRAsfPlayer
	 * Two phase constructor
	 * @return instance of CIRAsfPlayer
	 */
	IMPORT_C static CIRAsfPlayer* NewL(MIRStreamSourceObserver& aStreamObserver, 
									   MIRCtrlCmdObserver& aCtrlCmdObserver);

	/**
	 * Function : NewLC
	 * function creates an instance of CIRAsfPlayer
	 * Two phase constructor
	 * @return instance of CIRAsfPlayer
	 */
	IMPORT_C static CIRAsfPlayer* NewLC(MIRStreamSourceObserver& aStreamObserver, 
										MIRCtrlCmdObserver& aCtrlCmdObserver);

	/**
	 * Function : ~CIRAsfPlayer
	 * Default Destructor
	 */
	virtual ~CIRAsfPlayer();

//Function which control the play

	/**
	 * Function : Play
	 * function to which intiate the player
	 */
	IMPORT_C void PlayL(const TDesC& aUri);

	/**
	 * Function : Play
	 * function to which intiate the player
	 */
	IMPORT_C void Play();
	
	/**
	 * Function : Stop
	 * function to which stop the player
	 */
	IMPORT_C void Stop();   	     

//Function to control the volume

	IMPORT_C void SetVolume(TInt aVolume);

	IMPORT_C TInt Volume() const;

	IMPORT_C TInt MaxVolume() const;

// Helper functions
	IMPORT_C static TBool DetectAsfChannel(const TDesC& aUri);

private:
	/**
	 * Function: CIRAsfPlayer
	 * This function is the default constructor
	 */
	CIRAsfPlayer(MIRStreamSourceObserver& aStreamObserver, MIRCtrlCmdObserver& aCtrlCmdObserver);

	/**
	 * Function: ConstructL
	 * Two phase constructor is used to intialize data members
	 */
	void ConstructL();

	/**
	 * Function: ReadMetadataL
	 * Reads the meta data from the CMdaAudioPlayerUtility
	 */
	void ReadMetadataL();
	
//Call back functions
public:	

	// MMdaAudioPlayerCallback members
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);

	// MAudioLoadingObserver
	void MaloLoadingStarted();
	void MaloLoadingComplete();

private:	
	//Enum for Play State
	enum TState
		{
		ENotReady,    //Not ready to play
		EReadyToPlay, //Ready to play
		EPlaying      //Playing
		};

	//For implement Player functionalities

    /**
     * CMdaAudioPlayerUtility instance to stream Asf stream
     * Owned
     */
    CMdaAudioPlayerUtility* iMdaAudioPlayer;

    /**
     * Current meta data information.
     * Owned
     */
    CIRMetaData* iMetaData;
	
    /**
     * URI of the station server
     * Owned
     */
	RBuf	iStationUri;

	// Observer for streaming and playback events

	/**
	 * MIRCtrlCmdObserver reference to get volume setting
	 */
    MIRCtrlCmdObserver& iCtrlCmdObserver;

	/**
	 * Reference of the Stream source observer
	 */
    MIRStreamSourceObserver& iStreamObserver;

	// Player state	
	TState iState; 		
	};  

#endif//IRASFPLAYER_H


