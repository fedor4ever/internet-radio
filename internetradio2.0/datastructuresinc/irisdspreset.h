/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  iSDS preset structure.
*
*/


#ifndef IRISDSPRESET_H
#define IRISDSPRESET_H

#include <badesca.h>

class CIRChannelServerUrl;
class RReadStream;
class RWriteStream;

/**
 * This class provides the basic structure to hold the preset
 *information that come from the isds server.
 *It also provides the basic getter and setter functions to
 *manage the preset information.
 *for url selection for a particular bitrate some functionality
 *has been provided
 *copy constructor and assignment operator has been provided to
 *override the default ones
 *
 * @code
 *CIRIsdsPreset* preset = CIRIsdsPreset::NewL()
 *preset->SetXXX();
 *preset->GetXXX()
 * *preset1 = *preset2
 * @endcode
 *
 */

class CIRIsdsPreset:public CBase
	{

	public:

	/**
	*CIRIsdsPreset::NewL()
	*Static function
	*standard two phased constructor
	*@return *CIRIsdsPreset
	*/
	IMPORT_C	static CIRIsdsPreset* NewL();


	/**
	*CIRIsdsPreset::NewLC()
	*Static function
	*standard two phased constructor
	*@return *CIRIsdsPreset
	*/
	IMPORT_C	static CIRIsdsPreset* NewLC();


	/**
	*CIRIsdsPreset::~CIRIsdsPreset()
	*standard C++ destructor
	*/
	~CIRIsdsPreset();

	protected:
    /**
    *CIRIsdsPreset::CIRIsdsPreset()
    *standard C++ constructor
    */
	CIRIsdsPreset();
	
	/**
	*CIRIsdsPreset::ConstructL()
	*standard second phase constructor(Leaveable)
	*/
	void ConstructL();


	public:


	//setter functions

	/**
	*CIRIsdsPreset::SetId()
	*sets id
	*@param TDesC,supplied by xml parser
	*/
	IMPORT_C	void SetId(const TDesC &aId);

	/**
	*CIRIsdsPreset::SetId()
	*sets id
	*@param TInt,provided to manipulate the id number
	*/
	IMPORT_C	void SetId(TInt aId);

	/**
	*CIRIsdsPreset::SetName()
	*sets the preset name
	*@param TdesC aname,the name to be set
	*/
	IMPORT_C	void SetName(const TDesC &aName);

	/**
	*CIRIsdsPreset::SetLangInfo()
	*sets the preset language name and the code
	*@param TdesC,TdesC language id and name to be set
	*/
	IMPORT_C	void SetLangInfo(const TDesC &aLangId,const TDesC& aLangName);

	/**
	*CIRIsdsPreset::SetCountryInfo()
	*sets the preset country name and the code
	*@param TdesC,TdesC country id and country name to be set
	*/
	IMPORT_C void SetCountryInfo(const TDesC& aCountryId,
		const TDesC& aCountryName);

	/**
	*CIRIsdsPreset::SetLangCode()
	*sets the language code
	*@param TdesC language code
	*/
	IMPORT_C     void SetLangCode(const TDesC &aLangId);

	/**
	*CIRIsdsPreset::SetLang()
	*sets the language
	*@param TdesC language
	*/
	IMPORT_C	void SetLang(const TDesC& aLangName);

	/**
	*CIRIsdsPreset::SetCountryCode()
	*sets the language code
	*@param TdesC language code
	*/
	IMPORT_C	void SetCountryCode(const TDesC &aCountryId);

	/**
	*CIRIsdsPreset::SetCountryName()
	*sets the language name
	*@param TdesC language name
	*/
	IMPORT_C	void SetCountryName(const TDesC& aCountryName);

	/**
	*CIRIsdsPreset::SetLastModifiedTime()
	*sets the lastmodified time stamp
	*@param TdesC lastmodified time stamp
	*/
	IMPORT_C	void SetLastModifiedTime(const TDesC &aLastModified);

	/**
	*CIRIsdsPreset::SetGenreInfo()
	*sets the genreid and name
	*@param aGenreId
	*@param aGenreName
	*/
	IMPORT_C	void SetGenreInfo(const TDesC& aGenreId,const TDesC& aGenreName);

	/**
	*CIRIsdsPreset::SetImgUrl()
	*sets the image url
	*@param aImgUrl
	*/
	IMPORT_C	void SetImgUrl(const TDesC& aImgUrl);

	/**
	*CIRIsdsPreset::SetLogoDataL()
	*sets the Logo Data
	*@param aData
	*@param aSize
	*/
    IMPORT_C void SetLogoData( const TDesC8& aData );

	/**
	*CIRIsdsPreset::SetAdvertisementInUse()
	*sets the advertisementinuse url
	*@param aAdvertisementInUse
	*/
	IMPORT_C	void SetAdvertisementInUse(const TDesC& aAdvertisementInUse);

	/**
	*CIRIsdsPreset::SetDescription()
	*sets the description for the channel
	*@param aDescription
	*/
	IMPORT_C	void SetDescription(const TDesC& aDescription);

	/**
	*CIRIsdsPreset::SetAdvertisementUrl()
	*sets the advertisement url
	*@param aAdvertisementUrl
	*/
	IMPORT_C	void SetAdvertisementUrl(const TDesC& aAdvertisementUrl);

	/**
	*CIRIsdsPreset::SetUrlCount()
	*sets the count of the number of urls
	*/
	IMPORT_C	void SetUrlCount();

	/**
	*CIRIsdsPreset::SetShortDesc()
	*sets the short description
	*@param aShortDesc
	*/
	IMPORT_C	void SetShortDesc(const TDesC& aShortDesc);

	/**
	 * Sets the genre name
	 * @param aGenreName
	 */
	IMPORT_C	void SetGenreName(const TDesC& aGenreName);

	/**
	 * Sets the genre id
	 * @param aGenreId
	 */
	IMPORT_C	void SetGenreId(const TDesC& aGenreId);

	/**
	*CIRIsdsPreset::SetUrlCount()
	*sets the count of the number of urls
	*@param aCnt ,count
	*/
	IMPORT_C	void SetUrlCount(TInt aCnt);

	/**
	*CIRIsdsPreset::SetUrlL()
	*sets the channel server url
	*@param aServerName ,server name
	*@param aUrl ,server url
	*@param aBitrate .server bitrate
	*/
	IMPORT_C void SetUrlL(const TDesC &aServerName,const TDesC &aUrl,const TDesC &aBitrate);

	/**
	*CIRIsdsPreset::SetUrlL()
	*sets the channel server url
	*@param aServerName ,server name
	*@param aUrl ,server url
	*@param aBitrate ,server bitrate
	*/
	IMPORT_C void SetUrlL(const TDesC &aServerName,const TDesC &aUrl,TInt aBitrate);

	/**
	*CIRIsdsPreset::SetChannelType()
	*sets the channel type
	*@param aChannelType userdefined,isds,adhoc
	*/
	IMPORT_C	void SetChannelType(TInt aChannelType);
	
	
	/**
	*CIRIsdsPreset::SetMusicStoreStatus()
	*sets the musicStoreEnabled 
	*@param aMusicStoreStatus
	*/
	IMPORT_C    void SetMusicStoreStatus(const TDesC& aMusicStoreStatus);
	
	
	
	

	//for serialization
	/**
	*CIRIsdsPreset::ExternalizeL()
	*serializes the preset data into a stream
	*@param aWriteStream
	*/
	IMPORT_C	void ExternalizeL(RWriteStream& aWriteStream);

	/**
	*CIRIsdsPreset::InternalizeL()
	*serializes the preset data from a stream to a structure
	*@param aReadStream
	*/
	IMPORT_C 	void InternalizeL(RReadStream& aReadStream);

	//getter functions
	/**
	*CIRIsdsPreset::GetId()
	*gets the channel id
	*@return TInt
	*/
	IMPORT_C	TInt GetId() const;

	/**
	*CIRIsdsPreset::GetName()
	*gets the channel name
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetName() const;

	/**
	*CIRIsdsPreset::GetLangId()
	*gets the channel channelid
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetLangId() const;

	/**
	*CIRIsdsPreset::GetLangName()
	*gets the channel language name
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetLangName() const;

	/**
	*CIRIsdsPreset::GetCountryId()
	*gets the channel countryid
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetCountryId() const;

	/**
	*CIRIsdsPreset::GetCountryName()
	*gets the channel country name
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetCountryName() const;

	/**
	*CIRIsdsPreset::GetLastModifiedTime()
	*gets the channel last modified time stamp
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetLastModifiedTime() const;

	/**
	*CIRIsdsPreset::GetDescription()
	*gets the channel description
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetDescription() const;

	/**
	*CIRIsdsPreset::GetShortDescription()
	*gets the channel  short description
	*@return TDesC
	*/
	IMPORT_C   const TDesC& GetShortDescription() const;

    /**
    *CIRIsdsPreset::GetChannelType()
	*gets the channel  Type
	*@return TInt
	*/
	IMPORT_C	TInt GetChannelType() const;

	/**
	*CIRIsdsPreset::GetGenreId()
	*gets the channel  genreid
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetGenreId() const;

	/**
	*CIRIsdsPreset::GetGenreName()
	*gets the channel  genre name
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetGenreName() const;

	/**
	*CIRIsdsPreset::GetAdvertisementUrl()
	*gets the channel  advertisement url
	*@return TDesC
	*/
	IMPORT_C	const TDesC& GetAdvertisementUrl() const;

	/**
	 * Gets the channel advertisement in use
	 * @return TDesC
	 */
	IMPORT_C	const TDesC& GetAdvertisementInUse() const;

	/**
	 * Gets the image url
	 * @return TDesC
	 */
	IMPORT_C	const TDesC& GetImgUrl() const;

    /**
    *CIRIsdsPreset::GetUrlCount()
	*gets the channel  url count
	*@return TDesC
	*/
	IMPORT_C	TInt GetUrlCount() const;

	/*
	*CIRIsdsPreset::GetLogoData()
	*gets the Logo Data
	*@return TDesC&
	*/
    IMPORT_C const TDesC8& GetLogoData() const;

		/**
	*CIRIsdsPreset::GetMusicStoreStatus()
	*gets the musicStoreEnabled 
	*@return TDesC&
	*/
	IMPORT_C    const TDesC& GetMusicStoreStatus() const;


	/**
	*CIRIsdsPreset::SetLastModifiedTime()
	*gets the channel  url at a particular index
	*@param aindex
	*@return CIRChannelServerUrl
	*/
	IMPORT_C	CIRChannelServerUrl& GetUrl(TInt aInt);

    /**
    *CIRIsdsPreset::operator=()
	*assignment operator
	*@param CIRIsdsPreset& aPreset
	*@return CIRIsdsPreset
	*/
	IMPORT_C CIRIsdsPreset& operator=(const CIRIsdsPreset& aPreset);

	/**
	*CIRIsdsPreset::GetAvailableBitrates()
	*gets the bitrate array
	*@return RArray<TInt>
	*/
	IMPORT_C 	TInt GetAvailableBitrates(RArray<TInt>& aBitrates) const;

	/**
	*CIRIsdsPreset::GetUrlForBitrateL()
	*gets the array of bitrates for a particular bitrate
	*@param aBitrate
	*@return CDesCArrayFlat
	*/
	IMPORT_C 	CDesCArrayFlat* GetUrlForBitrateL(TInt aBitrate);

	/**
	*CIRIsdsPreset::GetExactUrlForBitrateL()
	*Urls exactly matching a given bitrate for display purpose
	*@param aBitrate
	*@return CDesCArrayFlat
	*/
	IMPORT_C 	CDesCArrayFlat* GetExactUrlForBitrateL(TInt aBitrate);

    /**
	*CIRIsdsPreset::GetChannelUrlAtL()
	*sets the lastmodified time stamp
	*returns the url at the urlarray by index
	*@param aIndex
	*@return TDesC
	*/
	IMPORT_C const TDesC& GetChannelUrlAtL(TInt aIndex) const;
    /**
	*CIRIsdsPreset::GetChannelUrlAtL()
	*sets the lastmodified time stamp
	*returns the url at the urlarray by index
	*@param aIndex
	*@return TDesC
	*/
	IMPORT_C  TInt GetChannelBitrateL(TInt aIndex);

private:

    /**
	*CIRIsdsPreset::SortInOrderL()
	*sorts the urls in bitrate order desc.
	*/
	void SortInOrderL();

public:

	//url array
	CArrayPtrFlat<CIRChannelServerUrl>*iUrlArray;

    /**type of presets(source)*/
    enum TChannelType
    {
    	EUserDefined,/**added by user*/
    	EIsdsPreset,/**fetched from isds*/
    	EAdhoc /**not in use currently*/
    };

private:
	//no of channel urls
	TInt iChannelUrlCount;
    //0 for user defined 1 for isds type.
	TInt iType;
	//isds generated preset id
	TInt iPresetId;
	//preset name
	RBuf iName;
	//preset language code
	RBuf iLanguageCode;
	//preset language
	RBuf iLanguage;
	//preset country of origin code
	RBuf iCountryCode;
	//preset country of origin
	RBuf iCountry;
	//preset last modified
	RBuf iLastModified;
	//musicStoreEnabled value
	RBuf iMusicStoreStatus;
	//not in use
	RBuf iAdvertisementUrl;
	//not in use
	RBuf iAdvertisementInUse;
	//not in use
	RBuf iImgUrl;
	//Logo Data
	RBuf8 iLogoData;
	//channel description
	RBuf iDescription;
	//channel short desription
	RBuf iShortDesc;
	//channel genre name
	RBuf iGenereName;
	//channel genre id
	RBuf iGenereId;
	//channel bitrate
	TInt iBitrate;

	};

#endif	//IRISDSPRESET_H
