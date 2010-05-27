/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/
#include "irqsonghistoryinfo.h"

// ---------------------------------------------------------------------------
//IRQSongHistoryInfo::~IRQSongHistoryInfo()
//standard C++ constructor
// ---------------------------------------------------------------------------
//
EXPORT_C IRQSongHistoryInfo::IRQSongHistoryInfo()
{
    iChannelType = 0;
    iChannelId = 0;
    iBitrate = 0;
}

// ---------------------------------------------------------------------------
//IRQSongHistoryInfo::~IRQSongHistoryInfo()
//standard C++ destructor
// ---------------------------------------------------------------------------
//
EXPORT_C IRQSongHistoryInfo::~IRQSongHistoryInfo()
{

}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getArtistName
// gets Artist name
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getArtistName() const
{
    return iArtistName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getSongName
// gets the song information
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getSongName() const
{
    return iSongName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getStreamUrl
// gets StreamUrl
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getStreamUrl() const
{
    return iStreamUrl;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getChannelName
// gets Channel name
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getChannelName() const
{
    return iChannelName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getChannelDesc
// gets Channel Description
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getChannelDesc() const
{
    return iChannelDescription;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getImageUrl
// gets Channel image Url
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getImageUrl() const
{
    return iImageUrl;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getGenreName
// gets Channel genre name
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getGenreName() const 
{
  return iGenreName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getCountryName
// gets Channel country name
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getCountryName() const 
{
  return iCountryName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getLanguageName
// gets Channel language name
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getLanguageName() const 
{
  return iLanguageName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getMusicStoreStatus
// gets Channel MusicStoreStatus
// ---------------------------------------------------------------------------
//
EXPORT_C const QString& IRQSongHistoryInfo::getMusicStoreStatus() const
{
    return iMusicStoreStatus;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getChannelType
// gets Channel Type
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSongHistoryInfo::getChannelType() const
{
    return iChannelType;
}
// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getChannelId
// gets Channel Id
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSongHistoryInfo::getChannelId() const
{
    return iChannelId;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::getBitrate
// gets Channel bitrate
// ---------------------------------------------------------------------------
//
EXPORT_C int IRQSongHistoryInfo::getBitrate() const
{
    return iBitrate;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setHistoryInfo
// sets Song History Information.
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setHistoryInfo(const QString& aArtistName,
        const QString& aSongName,
        const QString& aStreamUrl,
        const QString& aChannelName,
        const QString& aChannelDesc,
        const QString& aImageUrl,
	    const QString& aGenreName,
	    const QString& aCountryName,
	    const QString& aLanguageName,
        const QString& aMusicStoreStatus, 
        int aChannelType,
        int aChannelId,
        int aBitrate)
{
    setArtistName(aArtistName);
    setSongName(aSongName);
    setStreamUrl(aStreamUrl);
    setChannelName(aChannelName);
    setChannelDesc(aChannelDesc);
    setImageUrl(aImageUrl);
    setGenreName(aGenreName);
    setCountryName(aCountryName);
    setLanguageName(aLanguageName);    
    setMusicStoreStatus(aMusicStoreStatus);
    setChannelType(aChannelType);
    setChannelId(aChannelId);
    setBitrate(aBitrate);
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setArtistName
// sets Artist name
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setArtistName(const QString& aArtistName)
{
    iArtistName = aArtistName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setSongName
// sets Song name
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setSongName (const QString& aSongName)
{
    iSongName = aSongName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setStreamUrl
// sets Stream url
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setStreamUrl(const QString& aStreamUrl)
{
    iStreamUrl = aStreamUrl;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setChannelName
// sets Channel name
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setChannelName(const QString& aChannelName)
{
    iChannelName = aChannelName;
}
// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setChannelDesc
// sets Channel description
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setChannelDesc(const QString& aChannelDesc)
{
    iChannelDescription = aChannelDesc;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setImageUrl
// sets Channel image Url
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setImageUrl(const QString& aImageUrl)
{
    iImageUrl = aImageUrl;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setGenreName
// sets Channel genre name
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setGenreName(const QString& aGenreName)
{
	iGenreName = aGenreName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setCountryName
// sets Channel country name
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setCountryName(const QString& aCountryName)
{
	iCountryName = aCountryName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setLanguageName
// sets Channel language name
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setLanguageName(const QString& aLanguageName)
{
	iLanguageName = aLanguageName;
}

// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setMusicStoreStatus
// sets Channel's MusicStoreStatus
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setMusicStoreStatus(const QString& aMusicStoreStatus)
{
    iMusicStoreStatus = aMusicStoreStatus;
}
// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setChannelType
// sets Channel type
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setChannelType(int aChannelType)
{
    iChannelType = aChannelType;
}
// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setChannelId
// sets Channel Id
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setChannelId(int aChannelId)
{
    iChannelId = aChannelId;
}
// ---------------------------------------------------------------------------
// IRQSongHistoryInfo::setBitrate
// sets Channel bitrate
// ---------------------------------------------------------------------------
//
void IRQSongHistoryInfo::setBitrate(int aBitrate)
{
    iBitrate = aBitrate;
}



EXPORT_C void IRQSongInfo::setSongName(const QString& aName)
{
    iSongName = aName;
}

EXPORT_C const QString& IRQSongInfo::getSongName() const
{
    return iSongName;
}

EXPORT_C void IRQSongInfo::setArtistName(const QString& aArtist)
{
    iSongArtist = aArtist;
}

EXPORT_C const QString& IRQSongInfo::getArtistName() const
{
    return iSongArtist;
}

EXPORT_C void IRQSongInfo::setMusicshopStatus(const QString& aStatus)
{
    iMusicshopStatus = aStatus;
}

EXPORT_C const QString& IRQSongInfo::getMusicshopStatus() const
{
    return iMusicshopStatus;
}

EXPORT_C void IRQSongInfo::setAllInfo(const QString& aName, const QString& aArtist, const QString& aStatus)
{
    iSongName =   aName;
    iSongArtist = aArtist;
    iMusicshopStatus = aStatus;    
}



