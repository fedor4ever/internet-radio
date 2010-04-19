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


//supported formats
#ifndef IRMEDIAENGINEDEF_H 
#define IRMEDIAENGINEDEF_H

#include <e32def.h>

//No of volume levels required other than zero volume level
const TInt KNoVolumeLevels = 10;

//volume level corresponding to zero volume (mute volume)
const TInt KZeroVolume = 0;

//This hold the initialization information which is required for a player to statrt playing
class TConfig
	{
public:
	TInt32  iVolume; //volume level
	TInt    iPlayBufferCount; // number of playing buffers
	TUint   iPlayBufferSize; // size of single playing buffer (in bytes)
	};
	
//This structure holds the various parameter of an audio format which is available
class TAudioFormatData
	{
public:
	TUint32 iSamplingRate; //sampling rate
	TUint32 iBitRate; //bit rate
	TUint32 iVersion; //version
	TUint32 iLayer; //layer
	TUint32 iChannel; //channel
	TUint32 iPadding; //padding
	TUint32 iFrameSize; //frame size
	TUint32 iSamplesPerFrame; //samples per frame
	};
		

#endif//IRMEDIAENGINEDEF_H 

