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
* Description:  ?Description
*
*/


#ifndef IRPRESETIMPL_H
#define IRPRESETIMPL_H

#include "irpreset.h"

class CIRChannelServerUrlImpl;

/**
 * This class provides the basic structure to hold the preset
 *information that is saved in the favourites database
 *It also provides the basic getter and setter functions to
 *manage the preset information.
 *for url selection for a particular bitrate some functionality
 *has been provided
 *copy constructor and assignment operator has been provided to
 *override the default ones
 *
 * @code
 *CIRPresetImpl* preset = CIRPresetImpl::NewL()
 *preset->SetXXX();
 *preset->GetXXX()
 * *preset1 = *preset2
 * @endcode
 *
 */

class CIRPresetImpl:public CIRPreset
	{

public:

	/**CIRPresetImpl::NewL()
	*Static function
	*standard two phased constructor
	*@param void
	*@return *CIRPresetImpl
	**/
    static CIRPresetImpl* NewL(TAny* aParams);


	/**CIRPresetImpl::NewLC()
	*Static function
	*standard two phased constructor
	*@param void
	*@return *CIRPresetImpl
	**/
 	static CIRPresetImpl* NewLC(TAny* aParams);


	/**CIRPresetImpl::~CIRIsdsPreset()
	*standard C++ destructor
	*@param NA
	*@return NA
	**/
	~CIRPresetImpl();

	/**
	*CIRPresetImpl::SetId()
	*sets id
	*@param TDesC,supplied by xml parser
	*/
	void SetId(const TDesC &aId);

	/**
	*CIRPresetImpl::SetId()
	*sets id
	*@param TInt,provided to manipulate the id number
	*/
	void SetId(TInt aId);


	/**
	*CIRPresetImpl::SetLangInfo()
	*sets the preset language name and the code
	*@param TdesC,TdesC language id and name to be set
	*/
	void SetLangInfo(const TDesC &aLangId,const TDesC& aLangName);

	/**
	*CIRPresetImpl::SetCountryInfo()
	*sets the preset country name and the code
	*@param TdesC,TdesC country id and country name to be set
	*/
	void SetCountryInfo(const TDesC &aCountryId,const TDesC& aCountryName);

	/**
	*CIRPresetImpl::SetLangCode()
	*sets the language code
	*@param TdesC language code
	*/
	void SetLangCode(const TDesC &aLangId);

	/**
	*CIRPresetImpl::SetLang()
	*sets the language
	*@param TdesC language
	*/
	void SetLang(const TDesC& aLangName);

	/**
	*CIRPresetImpl::SetCountryCode()
	*sets the language code
	*@param TdesC language code
	*/
	void SetCountryCode(const TDesC &aCountryId);

	/**
	*CIRPresetImpl::SetCountryName()
	*sets the language name
	*@param TdesC language name
	*/
	void SetCountryName(const TDesC& aCountryName);

	/**
	*CIRPresetImpl::SetLastModifiedTime()
	*gets the channel  url at a particular index
	*@param TDesC,time stamp
	*/
	void SetLastModifiedTime(const TDesC &aLastModified);

	/**
	*CIRPresetImpl::SetGenreInfo()
	*sets the genreid and name
	*@param aGenreId
	*@param aGenreName
	*/
	void SetGenreInfo(const TDesC& aGenreId,const TDesC& aGenreName);

	/**
	*CIRPresetImpl::SetImgUrl()
	*sets the image url
	*@param aImgUrl
	*/
	void SetImgUrl(const TDesC& aImgUrl);

	/**
	*CIRPresetImpl::SetLogoData()
	*sets the Logo Data
	*@param aData
	*@param aSize
	*/
	void SetLogoData( const TDesC8& aData );

	/**
	*CIRPresetImpl::SetAdvertisementInUse()
	*sets the advertisementinuse url
	*@param aAdvertisementInUse
	*/
	void SetAdvertisementInUse(const TDesC& aAdvertisementInUse);

	/**
	*CIRPresetImpl::SetDescription()
	*sets the description for the channel
	*@param aDescription
	*/
	void SetDescription(const TDesC& aDescription);

	/**
	*CIRPresetImpl::SetAdvertisementUrl()
	*sets the advertisement url
	*@param aAdvertisementUrl
	*/
	void SetAdvertisementUrl(const TDesC& aAdvertisementUrl);

	/**
	*CIRPresetImpl::SetUrlCount()
	*sets the count of the number of urls
	*/
	void SetUrlCount();

	/**
	*CIRPresetImpl::SetShortDesc()
	*sets the short description
	*@param aShortDesc
	*/
	void SetShortDesc(const TDesC& aShortDesc);

	/**
	*CIRPresetImpl::SetUrlCount()
	*sets the count of the number of urls
	*@param aCnt ,count
	*/
	void SetUrlCount(TInt aCnt);

	/**
	*CIRPresetImpl::SetUrlL()
	*sets the channel server url
	*@param aServerName ,server name
	*@param aUrl ,server url
	*@param aBitrate .server bitrate
	*/
	void SetUrlL(const TDesC &aServerName,const TDesC &aUrl,
		const TDesC &aBitrate);

	/**
	*CIRPresetImpl::SetUrlL()
	*sets the channel server url
	*@param aServerName ,server name
	*@param aUrl ,server url
	*@param aBitrate ,server bitrate
	*/
	void SetUrlL(const TDesC &aServerName,const TDesC &aUrl,TInt aBitrate);

	/**
	*CIRPresetImpl::SetChannelType()
	*sets the channel type
	*@param aChannelType userdefined,isds,adhoc
	*/
	void SetChannelType(TInt aChannelType);
	
	/**
	*CIRPresetImpl::SetMusicStoreStatus()
	*sets the musicStoreEnabled 
	*@param aMusicStoreStatus
	*/
	void SetMusicStoreStatus(const TDesC& aMusicStoreStatus);

	//getter functions

	/**
	*CIRPresetImpl::GetId()
	*gets the channel id
	*@return TInt
	*/
	TInt GetId()const;
	/**
	*CIRPresetImpl::GetLangId()
	*gets the channel channelangid
	*@return TDesC
	*/
	const TDesC& GetLangId() const;

	/**
	*CIRPresetImpl::GetLangName()
	*gets the channel language name
	*@return TDesC
	*/
	const TDesC& GetLangName() const;

	/**
	*CIRPresetImpl::GetCountryId()
	*gets the channel countryid
	*@return TDesC
	*/
	const TDesC& GetCountryId() const;

	/**
	*CIRPresetImpl::GetCountryName()
	*gets the channel country name
	*@return TDesC
	*/
	const TDesC& GetCountryName() const;

	/**
	*CIRPresetImpl::GetLastModifiedTime()
	*gets the channel last modified time stamp
	*@return TDesC
	*/
	const TDesC& GetLastModifiedTime() const;

	/**
	*CIRPresetImpl::GetDescription()
	*gets the channel description
	*@return TDesC
	*/
	const TDesC& GetDescription() const;

	/**
	*CIRPresetImpl::GetShortDescription()
	*gets the channel  short description
	*@return TDesC
	*/
	const TDesC& GetShortDescription() const;

    /**
    *CIRPresetImpl::GetChannelType()
	*gets the channel  Type
	*@return TInt
	*/
	TInt GetChannelType()const;

	/**
	*CIRPresetImpl::GetGenreId()
	*gets the channel  genreid
	*@return TDesC
	*/
	const TDesC& GetGenreId() const;

	/**
	*CIRPresetImpl::GetGenreName()
	*gets the channel  genre name
	*@return TDesC
	*/
	const TDesC& GetGenreName() const;

	/**
	*CIRPresetImpl::GetAdvertisementUrl()
	*gets the channel  advertisement url
	*@return TDesC
	*/
	const TDesC& GetAdvertisementUrl() const;

	/**
	*CIRPresetImpl::GetImgUrl()
	*gets the channel  image url
	*@return TDesC
	*/
	const TDesC& GetImgUrl() const;

    /**
    *CIRPresetImpl::GetUrlCount()
	*gets the channel  url count
	*@return TDesC
	*/
	TInt GetUrlCount()const;

	/*
	*CIRPresetImpl::GetLogoData()
	*gets the Logo Data
	*@return TDesC8&
	*/
    const TDesC8& GetLogoData() const;

	/**
	*CIRPresetImpl::GetUrl()
	*gets the channel  url from zeroth position
	*@return CIRChannelServerUrlImpl&
	*/
	CIRChannelServerUrlImpl& GetUrl(TInt aInt)const;
	
	/*
	 * get the channel url at specified position
     *
     */
	const TDesC& GetChannelUrlAt(TInt aIndex) const;
	    
	/**
	*CIRPresetImpl::GetMusicStoreStatus()
	*gets the musicStoreEnabled 
	*@return TDesC&
	*/
	const TDesC& GetMusicStoreStatus()const;


    /**
    *CIRPresetImpl::operator=()
	*assignment operator
	*@param CIRIsdsPreset& aPreset
	*@return CIRIsdsPreset
	*/
	CIRPresetImpl& operator=(const CIRPresetImpl& aPreset);

	/**
 	*void CIRPresetImpl::CIRIsdsPresetToCIRPresetImpl(const CIRIsdsPreset& aIsdsPreset)
 	*used to convert the isdspresets to plugnized presets.
 	*Does a deep copy for all the member variables
 	*@param CIRIsdsPreset&,the preset to be copied from
	*/
	void CIRIsdsPresetToCIRPresetImpl(const CIRIsdsPreset& aIsdsPreset);

	/**
	*CIRPresetImpl::CopyPresetData()
	*copy data to CIRIsdsPreset
	*@param CIRIsdsPreset&,the preset to be copied into
	*/
	void CopyPresetData(CIRIsdsPreset& aIsdsPreset) const;

	/**
	*CIRPresetImpl::GetAvailableBitrates()
	*gets the bitrate array
	*@return RArray<TInt>
	*/
	RArray<TInt>& GetAvailableBitrates();

    /**
	*CIRPresetImpl::UniqId()
	*gets the unique id
	*@return TUint32
	*/
	TUint32 UniqId() ;

	/**
	*CIRPresetImpl::SetUniqId()
	*sets the unique id for the preset
	*@param aId
	*/
	void SetUniqId( TUint32 aUniqId);

    /*
     * Returns the played times of the channel
     */
    TInt GetPlayedTimes() const;
    
    /*
     * Set the played times of the channel
     */
    void SetPlayedTimes(TInt aPlayedTimes);
    
protected:

    /**CIRPresetImpl::CIRIsdsPreset()
    *standard C++ constructor
    *@param void
    *@return NA
    **/
    CIRPresetImpl();
    
	/**CIRPresetImpl::ConstructL(TAny* aParams)
	*standard second phase constructor(Leaveable)
	*@param void
	*@return void
	**/
	void ConstructL(TAny* aParams);


    //for serialization
    /**
    *CIRPresetImpl::ExternalizeL()
    *serializes the preset data into a stream
    *@param aWriteStream
    */
    void ExternalizeL(RWriteStream& aWriteStream)const;

    /**
    *CIRPresetImpl::InternalizeL()
    *serializes the preset data from a stream to a structure
    *@param aReadStream
    */
    void InternalizeL(RReadStream& aReadStream);
    
private:

	/**
	 * unique id generated for the preset saved at favourite
	 */
	TUint32 iUniqId;

	/**
	 * no of channel urls
	 */
	TInt iChannelUrlCount;

	/**
	 * 0 for user defined 1 for isds type.
	 */
	TInt iType;

	/**
	 * index in the preset table
	 */
	//TInt iIndex;not required

	/**
	 * isds generated preset id
	 */
	TInt iPresetId;

	/**
	 * preset language code
	 */
	RBuf iLanguageCode;

	/**
	 * preset language
	 */
	RBuf iLanguage;

	/**
	 * preset country of origin code
	 */
	RBuf iCountryCode;

	/**
	 * preset country of origin
	 */
	RBuf iCountry;

	/**
	 * preset last modified
	 */
	RBuf iLastModified;

	/**
	 * not in use
	 */
	RBuf iAdvertisementUrl;

	/**
	 * not in use
	 */
	RBuf iAdvertisementInUse;

	/**
	 * Logo URL
	 */
	RBuf iImgUrl;

	/**
     * Raw image data for the logo.
     * Owned.
     */
    RBuf8 iLogoData;

	/**
	 * channel description
	 */
	RBuf iDescription;

	/**
	 * channel short desription
	 */
	RBuf iShortDesc;

	/**
	 * channel genre name
	 */
	RBuf iGenereName;

	/**
	 * channel genre id
	 */
	RBuf iGenereId;
	
	/**
	 * musicStoreEnabled value
	 */
	RBuf iMusicStoreStatus;


	/**
	 * url array
	 */
	CDesCArrayFlat* iUrlForBitrate;

	/**
	 * url array
	 */
	CArrayPtrFlat<CIRChannelServerUrlImpl>* iUrlArray;

	/**
	 * bitrate array
	 */
	RArray<TInt> iBitrateArray;
	
	//added by Peter on June 23rd, 2009
	TInt iPlayedTimes;
	};

#endif	//end of IRPRESETIMPL_H
