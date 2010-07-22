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
#ifndef IRQSONGHISTORYINFO_H_
#define IRQSONGHISTORYINFO_H_

#include <QString>

/**
 * SongHistory meta data information wrapper.
 */
class IRQSongHistoryInfo
{
public:

    /**
     * Default constructor.
     */
    IMPORT_C IRQSongHistoryInfo();

    /**
    *IRQSongHistoryInfo::~IRQSongHistoryInfo()
    *standard C++ destructor
    */
    IMPORT_C ~IRQSongHistoryInfo();

    /**
    *IRQSongHistoryInfo::getArtistName()
    *Function to get artist name from a TDesC
    *@return QString& instance
    */
    IMPORT_C const QString& getArtistName() const;

    /**
    *IRQSongHistoryInfo::getSongName()
    *Function to get song name from a TDesC
    *@return QString& instance
    */
    IMPORT_C const QString& getSongName() const;

    /**
    *IRQSongHistoryInfo::getStreamUrl()
    *Function to get url name from a TDesC
    *@return QString& instance
    */
    IMPORT_C const QString& getStreamUrl() const;

    /**
    *IRQSongHistoryInfo::getChannelName()
    *Function to get channel name from a TDesC
    *@return QString& instance
    */
    IMPORT_C const QString& getChannelName() const;

    /**
    *IRQSongHistoryInfo::getChannelDesc()
    *Function to get channel Description
    *@return QString& instance
    */
    IMPORT_C const QString& getChannelDesc() const;

    /**
    *IRQSongHistoryInfo::getImageURL()
    *Function to get channel image URL
    *@return QString& instance
    */
    IMPORT_C const QString& getImageUrl() const;
    /**
    *IRQSongHistoryInfo::getGenreName()
    *Function to get channel genre name
    *@return QString& instance
    */
    IMPORT_C const QString& getGenreName() const;
    /**
    *IRQSongHistoryInfo::getCountryName()
    *Function to get channel country Name
    *@return QString& instance
    */
    IMPORT_C const QString& getCountryName() const;
    /**
    *IRQSongHistoryInfo::getLanguageName()
    *Function to get channel language name
    *@return QString& instance
    */
    IMPORT_C const QString& getLanguageName() const;

    /**
    *IRQSongHistoryInfo::getMusicStoreStatus()
    *Function to get channel MusicStoreStatus
    *@return QString& instance
    */
    IMPORT_C const QString& getMusicStoreStatus() const;

    /**
    *IRQSongHistoryInfo::getChannelType()
    *Function to get channel Type
    *@return int channeltype
    */
    IMPORT_C  int getChannelType() const;

    /**
    *IRQSongHistoryInfo::getChannelId()
    *Function to get channel Id
    *@return int channelId
    */
    IMPORT_C  int getChannelId() const;

    /**
    *IRQSongHistoryInfo::getBitrate()
    *Function to get channel bitrate
    *@return int bitrate
    */
    IMPORT_C  int getBitrate() const;

    /**
    *IRQSongHistoryInfo::setHistoryInfoL()
    *Function to set songhistory info
    */
    void setHistoryInfo(const QString& aArtistName,
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
                        int aBitrate);

    /**
    *IRQSongHistoryInfo::setArtistName()
    *Function to set artist name from a QString
    *@param QString aArtistName
    */
    void setArtistName(const QString& aArtistName);

    /**
    *IRQSongHistoryInfo::setSongName()
    *Function to set song name from a QString
    *@param QString aSongName
    */
    void setSongName (const QString& aSongName);

    /**
    *IRQSongHistoryInfo::setStreamUrl()
    *Function to set stream url from a QString
    *@param QString aStreamUrl
    */
    void setStreamUrl(const QString& aStreamUrl);

    /**
    *IRQSongHistoryInfo::setChannelName()
    *Function to set channel name from a QString
    *@param QString aChannelName
    */
    void setChannelName(const QString& aChannelName);

    /**
    *IRQSongHistoryInfo::setChannelDesc()
    *Function to set channel Description
    * @param QString aChannelDesc
    */
    void setChannelDesc(const QString& aChannelDesc);

    /**
    *IRQSongHistoryInfo::setImageURL()
    *Function to set channel image URL
    * @param QString aImageUrl
    */
    void setImageUrl(const QString& aImageUrl);
    
    /**
    *IRQSongHistoryInfo::setGenreName()
    *Function to set channel genre name
    * @param QString aGenreName
    */
    void setGenreName(const QString& aGenreName);
        
    /**
    *IRQSongHistoryInfo::setCountryName()
    *Function to set channel country name
    * @param QString aCountryName
    */
    void setCountryName(const QString& aCountryName);
    
    /**
    *IRQSongHistoryInfo::setLanguageName()
    *Function to set channel language name
    * @param QString aLanguageName
    */
    void setLanguageName(const QString& aLanguageName);

    /**
    *IRQSongHistoryInfo::setMusicStoreStatus()
    *Function to set Channel MusicStoreStatus
    *@param QString aMusicStoreStatus
    */
    void setMusicStoreStatus(const QString& aMusicStoreStatus);

    /**
    *IRQSongHistoryInfo::setChannelType()
    *Function to set channel Type
    *@param int aChannelType
    */
    void setChannelType(int aChannelType);

    /**
    *IRQSongHistoryInfo::setChannelId()
    *Function to set channel Id
    *@param int aChannelId
    */
    void setChannelId(int aChannelId);

    /**
    *IRQSongHistoryInfo::setBitrate()
    *Function to set channel bitrate
    *@param int aBitrate
    */
    void setBitrate(int aBitrate);

private:

    /**
     * The name of the artist.
     */
    QString iArtistName;

    /**
     * The name of the song.
     */
    QString iSongName;

    /**
     * The stream URL.
     */
    QString iStreamUrl;

    /**
     * Channel Name.
     */
    QString iChannelName;

    /**
     * Channel Description.
     */
    QString iChannelDescription;

    /**
     * Channel's image Url.
     */
    QString iImageUrl;
    QString iGenreName;
    QString iCountryName;
    QString iLanguageName;
    /**
     * Channel's MusicStoreStatus.
     */
    QString iMusicStoreStatus;

    /**
     * Channel Type.
     */
    int iChannelType;

    /**
     * Channel Id.
     */
    int iChannelId;

    /**
     * Channel bitrate.
     */
    int iBitrate;
};

class IRQSongInfo
{
public:
  
    
    IMPORT_C void setSongName(const QString& aName);
    IMPORT_C const QString& getSongName() const;
    
    IMPORT_C void setArtistName(const QString& aArtist);
    IMPORT_C const QString& getArtistName()const;

    IMPORT_C void setStationName(const QString& aStationName);
    IMPORT_C const QString& getStationName()const;

    IMPORT_C void setMusicshopStatus(const QString& aStatus);
    IMPORT_C const QString& getMusicshopStatus()const;
    
    IMPORT_C void setAllInfo(const QString& aName, const QString& aArtist, const QString& aStationName, const QString& aStatus);
    
private:
    
    QString iSongName;
    QString iSongArtist;
    QString iStationName;
    QString iMusicshopStatus;

};
#endif // IRQSONGHISTORYINFO_H_
