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
* Description:  Implementing class will receive meta/audiodata events.
*
*/


#ifndef IRSTATIONDATAOBSERVER_H
#define IRSTATIONDATAOBSERVER_H

class CIRMetaData;

/**
 * Implementation of this interface will receive the Metadata (and someday audio data)
 * and audio data related events.
 */
class MIRStationDataObserver
	{
public: 
    /**
	 * TIRAudioEvent.
	 */ 
	enum TIRAudioEvent
		{
		EBufferFilled,
		EOpenComplete,
        EBufferPercentage		
		};    
		
public:
    /**
     * New metadata is received.
     *
     * @param aMetaData Received metadata.
     */
    virtual void MetadataReceived( const CIRMetaData& aMetaData ) = 0;

	/**
	 * Event related to that data has been received on the socket and read into a buffer.
	 * This should be replaced by simple method providing the received audio data.	 
	 */
    virtual void AudioDataEvent( const TInt aResponseCode, TInt aValue ) = 0;
	};

#endif // IRSTATIONDATAOBSERVER_H

