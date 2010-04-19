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
* Description:  ?Description
*
*/


#ifndef IRMEDIAENGINEINTERFACE_H
#define IRMEDIAENGINEINTERFACE_H

#include <ecom.h>
#include <mdaaudiooutputstream.h>

class CIRCtrlCmdObserver;
class TConfig;

// UID of this interface
const TUid KMediaEngineInterfaceUid = {0x200087E8};

//========================================class declaration CIRMediaEngineInterface============================================

/**
 *
 * This is interface class for media engine, This has pure virtual function 
 * which the player implementation in the media engine should have. The default 
 * data typically can be mime type like audio/mpeg etc. 
 * It has virtual implementation like
 * Play() for play implementation
 * Stop() for stop implemtation 
 * SetVolume(TInt aVolume) for setting implementation
 * Volume() for setting the volume
 * MaxVolume() for setting the maximum volume
 * BufferFilled() is call back to intimate buffer is filled
 * Intialize(TConfig& ,TUint8* ,CIRCtrlCmdObserver* )
 * TConfig is structure the congfiguration data if required is supposed to pass
 * by this way, TUint8* refers to buffer and CIRCtrlCmdObserver* is instance to
 * CIRCtrlCmdObserver defined in IRCommandChannel.lib
 * and also functions like StopPlayerBuffering() and StartPlayerBuffering() to 
 * start and stop buffering.
 *
 */


class CIRMediaEngineInterface : public CBase
      {
												//Member Function
public:

	/**
	 * Function : NewL
	 * function to creates an instance of CIRMediaEngineInterface
	 * Two phase constructor
	 * @param matching string
	 */	
	static CIRMediaEngineInterface* NewL(const TDesC8& aMatchString);

	/**
	 * Function : ~CIRMediaEngineInterface
	 * Default Destructor
	 */
	~CIRMediaEngineInterface();

	// Pure interface method

	/**
	 * Function : Play
	 * function to which intiate the player
	 */
	virtual void Play() = 0;

	/**
	 * Function : Stop
	 * function to which stop the player
	 */
	virtual void Stop() = 0; 
	
	/**
	 * Function : SetVolume
	 * function to set the volume 
	 * @param integer level of volume is the input
	 */		
	virtual void SetVolume(TInt aVolume) = 0;

	/**
	 * Function : Volume
	 * function to returns the volume, integer level of voume is the Output
	 * @return integer value is the Output
	 */
	virtual TInt Volume() const = 0;
	 
	/**
	 * Function : MaxVolume
	 * function to returns the maximum volume 
	 * @return integer value is the Output 
	 */
	virtual	TInt MaxVolume() const = 0;

	/**
	 * Function: Intialize
	 * Set the codec type and sampling rate channel of stream
	 * This is set to initial settings which is required to start the player
	 * @param configuration infomation and instance of stream source
	 * @param instance of data buffer, whose size is defined in IRMediaEngineBuffer.h
	 * @param instance of the channel through with information is sent to UI
	 */			
	virtual void Intialize(TConfig& aConfig,TUint8* aInitParams,CIRCtrlCmdObserver* aChannel) = 0;
						
	/**
	 * Function: BufferFilled
	 * Function which is called when network gets the buffer filled with data
	 */
	virtual void BufferFilled() = 0;
	
	/**
	 * Function: StopPlayerBuffering
	 * Function is used to stop bufferring 
	 */
	virtual void StopPlayerBuffering() = 0;
	
	/**
	 * Function: GetAudioPlayer()
	 * Function is used to Return the Current Audio Player 
	 */
	virtual CMdaAudioOutputStream* GetAudioPlayer() = 0;
									//Data Members
protected:
	//Default c'tor
	inline CIRMediaEngineInterface();

protected:

    /* Size of a play buffer. */
    TInt iPlayBufferSize;

    /* Total size of input buffer(s). */
    TInt iInputBufferSize;

    /* Offset address of the buffers. */
    TInt iBufferOffset;

    /* The level of current volume */
    TInt iCurrentVolume;
 
private:
	// Unique instance identifier key
	TUid iDtor_ID_Key;
	};

#include "irmediaengineinterface.inl"

#endif//IRMEDIAENGINEINTERFACE_H

