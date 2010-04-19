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


#ifndef IRAACPLAYERLOCAL_H
#define IRAACPLAYERLOCAL_H

#include <e32def.h>

//AAC sampling rate is based on bit index
//sampling rate are taken from an array of 1x16 demension
//and size of the array is defined below
const TUint8 KAACSamplingRateArraySize = 16;

//AAC sampling rate selection array
const TUint KAacSamplingRate[KAACSamplingRateArraySize] =
	{
	96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,0,0,0,0
	};

//enumerated sampling frequency values 
//for aac/aacp player	
enum TIRAACSamplingFreq
	{
	EAACSamplingFreq8000 = 8000,
	EAACSamplingFreq11025 = 11025,
	EAACSamplingFreq12000 = 12000,
	EAACSamplingFreq16000 = 16000,
	EAACSamplingFreq22050 = 22050,
	EAACSamplingFreq24000 = 24000,
	EAACSamplingFreq32000 = 32000,
	EAACSamplingFreq44100 = 44100,
	EAACSamplingFreq48000 = 48000,
	EAACSamplingFreq64000 = 64000,
	EAACSamplingFreq88200 = 88200,
	EAACSamplingFreq96000 = 96000,	
	};	

//zero indicates that channel is mono
const TInt KAACChannelMono = 0;

const TInt KAACHeaderSize = 4;

#endif//IRAACPLAYERLOCAL_H





