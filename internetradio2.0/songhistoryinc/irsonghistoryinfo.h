/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  header for the songhistory information class
*
*/


#ifndef C_CIRSONGHISTORYINFO_H
#define C_CIRSONGHISTORYINFO_H

#include <s32mem.h>

/**
 * SongHistory meta data information wrapper.
 */
NONSHARABLE_CLASS( CIRSongHistoryInfo ) : public CBase
    {

public:

	/**
	*Function : NewL
	*Function returns an instance of CIRSongHistoryInfo
	*Two phase constructor
	*@return instance of CIRSongHistoryInfo
	*/
    	IMPORT_C static CIRSongHistoryInfo * NewL();

	/**
	*CIRSongHistoryInfo::NewLC()
	*Static function
	*standard two phased constructor
	*@return *CIRSongHistoryInfo
	*/
		IMPORT_C static CIRSongHistoryInfo* NewLC();

	/**
	*CIRSongHistoryInfo::~CIRSongHistoryInfo()
	*standard C++ destructor
	*/
		IMPORT_C ~CIRSongHistoryInfo();

	/**
	*CIRSongHistoryInfo::SetHistoryInfoL()
	*Function to set songhistory ingo from a RBuf
	*/

		IMPORT_C void SetHistoryInfo(RBuf& aSongName,
									 RBuf& aArtist, 
									 RBuf& aStreamUrl, 
									 RBuf& aChannelName,
									 TInt aChannelType , 
									 TInt aChannelId,
									 TInt aBitrate,
									 RBuf& aChannelDesc,
									 RBuf& aImageUrl,
                                     RBuf& aGenreName,
                                     RBuf& aCountryName,
                                     RBuf& aLanguageName,									 
									 RBuf& aMusicStoreStatus );

	/**
	*CIRSongHistoryInfo::SetChannelName()
	*Function to set channel name from a RBuf
	*@param RBuf aChannelName
	*/

		void SetChannelName(RBuf& aChannelName);

	/**
	*CIRSongHistoryInfo::SetArtist()
	*Function to set artist name from a RBuf
	*@param RBuf aArtist
	*/

		void SetArtist(RBuf& aArtist);

	/**
	*CIRSongHistoryInfo::SetSongName()
	*Function to set song name from a RBuf
	*@param RBuf aSongName
	*/

  		void SetSongName (RBuf& aSongName);

	/**
	*CIRSongHistoryInfo::SetStreamUrl()
	*Function to set stream url from a RBuf
	*@param RBuf aStreamUrl
	*/

		void SetStreamUrl(RBuf& aStreamUrl);

	/**
	*CIRSongHistoryInfo::GetSongInfo()
	*Function to get song name from a TDesC
	*@return RBuf& instance 
	*/

		IMPORT_C const RBuf& GetSongInfo() const;

	/**
	*CIRSongHistoryInfo::GetArtistName()
	*Function to get artist name from a TDesC
	*@return RBuf& instance
	*/

		IMPORT_C const RBuf& GetArtistInfo() const;

	/**
	*CIRSongHistoryInfo::GetStreamUrl()
	*Function to get url name from a TDesC
	*@return RBuf& instance
	*/

		 IMPORT_C const RBuf& GetStreamUrl() const;

	/**
	*CIRSongHistoryInfo::GetChannelName()
	*Function to get channel name from a TDesC
	*@return RBuf& instance
	*/

		IMPORT_C const RBuf& GetChannelName() const;
		
	/**
	*CIRSongHistoryInfo::GetChannelType()
	*Function to get channel Type
	*@return TInt channeltype
	*/
	    IMPORT_C  TInt GetChannelType() const;
	    
	/**
	*CIRSongHistoryInfo::SetChannelType()
	*Function to set channel Type
	*@param TInt aChannelImgUrl
	*/
	    void SetChannelType(TInt aChannelImgUrl);
	/**
	*CIRSongHistoryInfo::SetChannelId()
	*Function to set channel Id
	*@param TInt aChannelId
	*/
	    void SetChannelId(TInt aChannelId);
	/**
	*CIRSongHistoryInfo::GetChannelId()
	*Function to get channel Id
	*@return TInt channelId
	*/
	    IMPORT_C  TInt GetChannelId() const;
	/**
	*CIRSongHistoryInfo::SetBitrate()
	*Function to set bitrate
	*@param TInt aBitrate
	*/
        void SetBitrate(TInt aBitrate);
    /**
    *CIRSongHistoryInfo::GetBitrate()
    *Function to get bitrate
    *@return TInt aBitrate
    */
        IMPORT_C  TInt GetBitrate() const;
	/**
	*CIRSongHistoryInfo::GetChannelDesc()
	*Function to get channel Description
	*@return RBuf& instance
	*/
        IMPORT_C const RBuf& GetChannelDesc() const ;
	/**
	*CIRSongHistoryInfo::SetChannelDesc()
	*Function to set channel Description
	* @param RBuf aChannelDesc
	*/
        void SetChannelDesc(RBuf& aChannelDesc) ;
    /**
    *CIRSongHistoryInfo::GetImageUrl()
    *Function to get channel image Url
    *@return RBuf& instance
    */
        IMPORT_C const RBuf& GetImageUrl() const ;
    /**
    *CIRSongHistoryInfo::SetImageUrl()
    *Function to set channel image Url
    * @param RBuf aImageUrl
    */        
        void SetImageUrl(RBuf& aImageUrl) ;
    /**
    *IRQSongHistoryInfo::GetGenreName()
    *Function to get channel genre name
    *@return RBuf& instance
    */
        IMPORT_C const RBuf& GetGenreName() const;
    /**
    *CIRSongHistoryInfo::SetGenreName()
    *Function to set channel genre name
    * @param RBuf aGenreName
    */        
        void SetGenreName(const RBuf& aGenreName);
    /**
    *IRQSongHistoryInfo::GetCountryName()
    *Function to get channel country name
    *@return RBuf& instance
    */
        IMPORT_C const RBuf& GetCountryName() const;
    /**
    *CIRSongHistoryInfo::SetCountryName()
    *Function to set channel country name
    * @param RBuf& aCountryName
    */        
        void SetCountryName(const RBuf& aCountryName);
    /**
    *IRQSongHistoryInfo::getLanguageName()
    *Function to get channel language name
    *@return RBuf& instance
    */
        IMPORT_C const RBuf& GetLanguageName() const;
    /**
    *CIRSongHistoryInfo::SetLanguageName()
    *Function to set channel language name
    * @param RBuf aLanguageName
    */        
        void SetLanguageName(const RBuf& aLanguageName);        
	/**
	*CIRSongHistoryInfo::SetChannelMusicStatus()
	*Function to set Channel MusicStoreStatus
	*@param RBuf aChannelMusicStatus
	*/
        void SetChannelMusicStatus(RBuf& aChannelMusicStatus) ;
	/**
	*CIRSongHistoryInfo::GetChannelMusicStatus()
	*Function to get channel MusicStoreStatus
	*@return RBuf& instance
	*/
        IMPORT_C const RBuf& GetChannelMusicStatus() const ;
private:

    /**
     * Default constructor.
     */

		CIRSongHistoryInfo();


private: //data

	/**
     * The name of the artist.
     */
		RBuf iArtistInfo;

    /**
     * The name of the song.
     */
		RBuf iSongInfo;

    /**
     * The stream URL.
	 */
		RBuf iStreamUrlInfo;

    /**
     * Channel Name.
     */
		RBuf iChannelInfo;
		
    /**
     * Channel Type.
     */
		TInt iChannelType;
		
		
    /**
     * Channel Description.
     */
		RBuf iChannelDescription;
    /**
	 * Channel Description.
     */
        RBuf iImageUrl;
        RBuf iGenreName;
        RBuf iCountryName;
        RBuf iLanguageName;
    /**
     * Channel Id.
     */
		TInt iChannelId;

	/**
	 * Channel bitrate.
	 */
	TInt iBitrate;
		
    /**
     * Channel's MusicStoreStatus.
     */
		RBuf iMusicStoreStatus;

    };

#endif // C_CIRSONGHISTORYINFO_H
