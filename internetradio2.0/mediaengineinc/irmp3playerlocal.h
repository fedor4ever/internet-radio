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




#ifndef IRMP3PLAYERLOCAL_H
#define IRMP3PLAYERLOCAL_H

//sampling rate of MP3 is taken from a look up table
//selection is made based on MPEG version and index bits
//the look up table is made in the form of an array of 4x4 
//and demension are define in terms of constant defined below
const TUint8 KMP3SamplingRateVersionArraySize = 4;
const TUint8 KMP3SamplingRateIndexArraySize = 4;

//bitrate rate of MP3 is taken from a look up table
//selection is made based on MPEG version and Layer and index bits
//the look up table is made in the form of an array of 2x3x16 
//and demension are define in terms of constant defined below
const TUint8 KMP3BitRateMPEGArraySize = 2;
const TUint8 KMP3BitRateLayerArraySize = 3;
const TUint8 KMP3BitRateIndexArraySize = 16;

//channel selection is based on 2 bits and is defined as array of 
//demension 4 and is defined in terms of constant.
const TUint8 KMP3ChannelArraySize = 4;


//sampling rate is given in Hzs.
//KMP3SamplingRate[MPEGVersion][bit index]
const TInt32 KMP3SamplingRate[KMP3SamplingRateVersionArraySize][KMP3SamplingRateIndexArraySize] = //! Sampling rates for MP3
	{		
		{11025,12000,8000,0},    //MPEG 2.5 unofficial   00
		{0, 0, 0, 0},            //reserved              01
		{22050,24000,16000,0},   //MPEG 2                10
		{44100,48000,32000,0}    //MPEG 1                11
	};
	
//each bit rate is given in Kbps.
// KBitRate[Version][Layer][bitselect] 
const TUint32 KBitRate[KMP3BitRateMPEGArraySize]
					  [KMP3BitRateLayerArraySize]
					  [KMP3BitRateIndexArraySize] = //! Bit rates for Mp3
	{
		{	// MPEG 1
			{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},	// Layer1
			{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,0},	// Layer2
			{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,0}	// Layer3
		},
		{	// MPEG 2, 2.5		
			{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},	// Layer1
			{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},			// Layer2
			{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}			// Layer3
		}
	};

//enumerated sampling frequency values 
//for mp3 player
enum TIRMp3SamplingFreq
	{
	EMp3SamplingFreq8000 = 8000,
	EMp3SamplingFreq11025 = 11025,
	EMp3SamplingFreq12000 = 12000,
	EMp3SamplingFreq16000 = 16000,
	EMp3SamplingFreq22050 = 22050,
	EMp3SamplingFreq24000 = 24000,
	EMp3SamplingFreq32000 = 32000,
	EMp3SamplingFreq44100 = 44100,
	EMp3SamplingFreq48000 = 48000,	
	};	

//channels 1 for stereo, and 0 for mono
// bits 6,7:
// 00 - Stereo
// 01 - Joint stereo (Stereo)
// 10 - Dual channel (Stereo)
// 11 - Single channel (Mono)
const TUint32 KChannelInfo[KMP3ChannelArraySize] =
	{1,1,0,0};

const TInt KMp3FrameHeaderSize = 5;

//zero indicates that channel is mono
const TInt KMp3ChannelMono = 0;

#endif//IRMP3PLAYERLOCAL_H

