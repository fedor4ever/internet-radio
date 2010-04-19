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


#ifndef IRSTREAMSOURCEOBSERVER_H
#define IRSTREAMSOURCEOBSERVER_H

#include <e32def.h>

class CIRMetaData;

/**
 * This MClass is to be implemented by the observer of streamsource
 */

class MIRStreamSourceObserver
	{
public:

	/** 
	 *  MIRStreamSourceObserver::OpenComplete()
	 *  Called when connected to the channel server  
	 *  To implement the fill of buffer
	 */
	virtual void OpenComplete() = 0; 

	/** 
	 *  MIRStreamSourceObserver::ErrorConnecting()
	 *  Called when there is an error when connecting to the channel server  
	 *  @Param aErrorCode Indicates the type of error as described in
	 * 		   irstreamsourceerrors.h
	 */
	virtual void ErrorConnecting(TInt aErrorCode) = 0;

	/** 
	 *  MIRStreamSourceObserver::PlayNow()
	 *  Called to indicate to the player to start playing
	 */
	virtual void PlayNow() = 0;

	/** 
	 *  MIRStreamSourceObserver::Play()
	 *  Triggers the play after reconnecting to the channel
	 */
	virtual void Play() = 0;
	
	/** 
	 *  MIRStreamSourceObserver::StopBuffering()
	 *  Called to indicate to the player to stop buffering
	 */
	virtual void StopBuffering() = 0;

	/** 
	 *  MIRStreamSourceObserver::ConnectionEstablished(TInt aFeedValue)
	 *  Called to indicate that the connection to channel server is sucessful
	 */
	virtual void ConnectionEstablished() = 0 ;

	/** 
	 *  MIRStreamSourceObserver::UpdateProgress()
	 *  Called to provide teh buffer values
	 *  @param aFeedValue used to pump the buffering value
	 */
	virtual void UpdateProgress(TInt aFeedValue) = 0;

    /**
     * Invoked when meta data is received.
     *
     * @param   aMetaData   The meta data that was received.
     */
    virtual void HandleMetaDataReceivedL( const CIRMetaData& aMetaData ) = 0;

	// Added for ASF Player
	
	/** 
	 *  MIRStreamSourceObserver::Asf_ErrorConnecting()
	 *  Called when there is an error when connecting to the channel server  
	 *  @Param aErrorCode Indicates the type of error as described in
	 * 		   irstreamsourceerrors.h
	 */
	virtual void Asf_ErrorConnecting(TInt aErrorCode) = 0;

	/** 
	 *  MIRStreamSourceObserver::Asf_ConnectionEstablished(TInt aFeedValue)
	 *  Called to indicate that the connection to channel server is sucessful
	 */
	virtual void Asf_ConnectionEstablished() = 0 ;

	/** 
	 *  MIRStreamSourceObserver::Asf_UpdateProgress()
	 *  Called to provide teh buffer values
	 *  @param aFeedValue used to pump the buffering value
	 */
	virtual void Asf_UpdateProgress(TInt aFeedValue) = 0;

    /**
     * MIRStreamSourceObserver::Asf_HandleMetaDataReceivedL
     * Invoked when meta data is received.
     *
     * @param   aMetaData   The meta data that was received.
     */
    virtual void Asf_HandleMetaDataReceivedL( const CIRMetaData& aMetaData ) = 0;
	};

#endif // IRSTREAMSOURCEOBSERVER_H

