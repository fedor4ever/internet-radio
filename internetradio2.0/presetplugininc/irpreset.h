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


#ifndef IRPRESET_H
#define IRPRESET_H

#include <badesca.h>
#include <pspresetinterface.h>

class CIRIsdsPreset;
class CIRChannelServerUrl;

const TUid KIRPreset = { 0x10009DC1 };

/**
 *preset abstract class to expose general apis to the app
 *it is the base class for IRPresetImpl,which is a plugin
 *the plugin implements all the pure virtual functions
*/
class CIRPreset:public CPSPresetInterface
	{

public:

	/**
	*CIRPreset::SetId()
	*sets id
	*@param TDesC,supplied by xml parser
	*/
	virtual void SetId(const TDesC &aId)=0;

	/**
	*CIRPreset::SetId()
	*sets id
	*@param TInt,provided to manipulate the id number
	*/
	virtual void SetId(TInt aId)=0;


	/**
	*CIRPreset::SetLangInfo()
	*sets the preset language name and the code
	*@param TdesC,TdesC language id and name to be set
	*/
	virtual void SetLangInfo(const TDesC &aLangId,const TDesC& aLangName)=0;

	/**
	*CIRPreset::SetCountryInfo()
	*sets the preset country name and the code
	*@param TdesC,TdesC country id and country name to be set
	*/
	virtual void SetCountryInfo(const TDesC &aCountryId,
		const TDesC& aCountryName)=0;

	/**
	*CIRPreset::SetLangCode()
	*sets the language code
	*@param TdesC language code
	*/
	virtual void SetLangCode(const TDesC &aLangId)=0;

	/**
	*CIRPreset::SetLang()
	*sets the language
	*@param TdesC language
	*/
	virtual void SetLang(const TDesC& aLangName)=0;

	/**
	*CIRPreset::SetCountryCode()
	*sets the language code
	*@param TdesC language code
	*/
	virtual void SetCountryCode(const TDesC &aCountryId)=0;

	/**
	*CIRPreset::SetCountryName()
	*sets the language name
	*@param TdesC language name
	*/
	virtual void SetCountryName(const TDesC& aCountryName)=0;

	/**
	*CIRPreset::SetLastModifiedTime()
	*gets the channel  url at a particular index
	*@param TDesC,time stamp
	*/
	virtual void SetLastModifiedTime(const TDesC &aLastModified)=0;

	/**
	*CIRPreset::SetGenreInfo()
	*sets the genreid and name
	*@param aGenreId
	*@param aGenreName
	*/
	virtual void SetGenreInfo(const TDesC& aGenreId,
		const TDesC& aGenreName)=0;

	/**
	*CIRPreset::SetImgUrl()
	*sets the image url
	*@param aImgUrl
	*/
	virtual void SetImgUrl(const TDesC& aImgUrl)=0;

	/**
	*CIRPreset::SetAdvertisementInUse()
	*sets the advertisementinuse url
	*@param aAdvertisementInUse
	*/
	virtual void SetAdvertisementInUse(const TDesC& aAdvertisementInUse)=0;

	/**
	*CIRPreset::SetDescription()
	*sets the description for the channel
	*@param aDescription
	*/
	virtual void SetDescription(const TDesC& aDescription)=0;

	/**
	*CIRPreset::SetAdvertisementUrl()
	*sets the advertisement url
	*@param aAdvertisementUrl
	*/
	virtual void SetAdvertisementUrl(const TDesC& aAdvertisementUrl)=0;

	/**
	*CIRPreset::SetUrlCount()
	*sets the count of the number of urls
	*/
	virtual void SetUrlCount()=0;

	/**
	*CIRPreset::SetShortDesc()
	*sets the short description
	*@param aShortDesc
	*/
	virtual void SetShortDesc(const TDesC& aShortDesc)=0;


	/**
	*CIRPreset::SetMusicStoreStatus()
	*sets the musicStoreEnabled flag
	*@param aMusicStoreStatus
	*/
	virtual void SetMusicStoreStatus(const TDesC& aMusicStoreStatus)=0;


	/**
	*CIRPreset::SetUrlCount()
	*sets the count of the number of urls
	*@param aCnt ,count
	*/
	virtual void SetUrlCount(TInt aCnt)=0;

	/**
	*CIRPreset::SetIndex()
	*sets the index for the preset.used only for presets in the fav list
	*@param aInt
	*/
	//not required
	//virtual void SetIndex(TInt aInt)=0

	/**
	*CIRPreset::SetUrlL()
	*sets the channel server url
	*@param aServerName ,server name
	*@param aUrl ,server url
	*@param aBitrate .server bitrate
	*/
	virtual void SetUrlL(const TDesC &aServerName,const TDesC &aUrl,
		const TDesC &aBitrate)=0;

	/**
	*CIRPreset::SetUrlL()
	*sets the channel server url
	*@param aServerName ,server name
	*@param aUrl ,server url
	*@param aBitrate ,server bitrate
	*/
	virtual void SetUrlL(const TDesC &aServerName,const TDesC &aUrl,
		TInt aBitrate)=0;

	/**
	*CIRPreset::SetChannelType()
	*sets the channel type
	*@param aChannelType userdefined,isds,adhoc
	*/
	virtual void SetChannelType(TInt aChannelType)=0;

	/**
	*CIRPreset::GetId()
	*gets the channel id
	*@return TInt
	*/
	virtual TInt GetId()const=0;

	/**
	*CIRPreset::GetLangId()
	*gets the channel channelangid
	*@return TDesC
	*/
	virtual const TDesC& GetLangId() const=0;

	/**
	*CIRPreset::GetLangName()
	*gets the channel language name
	*@return TDesC
	*/
	virtual const TDesC& GetLangName() const=0;

	/**
	*CIRPreset::GetCountryId()
	*gets the channel countryid
	*@return TDesC
	*/
	virtual const TDesC& GetCountryId() const=0;

	/**
	*CIRPreset::GetCountryName()
	*gets the channel country name
	*@return TDesC
	*/
	virtual const TDesC& GetCountryName() const=0;

	/**
	*CIRPreset::GetLastModifiedTime()
	*gets the channel last modified time stamp
	*@return TDesC
	*/
	virtual const TDesC& GetLastModifiedTime() const=0;

	/**
	*CIRPreset::GetDescription()
	*gets the channel description
	*@return TDesC
	*/
	virtual const TDesC& GetDescription() const=0;

	/**
	*CIRPreset::GetShortDescription()
	*gets the channel  short description
	*@return TDesC
	*/
	virtual const TDesC& GetShortDescription() const=0;


	/**
	*CIRPreset::GetMusicStoreStatus()
	*gets the channel  musicStoreEnabled flag
	*@return TDesC
	*/
	virtual const TDesC& GetMusicStoreStatus() const=0;


    /**
    *CIRPreset::GetChannelType()
	*gets the channel  Type
	*@return TInt
	*/
	virtual TInt GetChannelType()const=0;

	/**
	*CIRPreset::GetGenreId()
	*gets the channel  genreid
	*@return TDesC
	*/
	virtual const TDesC& GetGenreId() const=0;

	/**
	*CIRPreset::GetGenreName()
	*gets the channel  genre name
	*@return TDesC
	*/
	virtual const TDesC& GetGenreName() const=0;

	/**
	*CIRPreset::GetAdvertisementUrl()
	*gets the channel  advertisement url
	*@return TDesC
	*/
	virtual const TDesC& GetAdvertisementUrl() const=0;

    /**
    *CIRPreset::GetAdvertisementInUse()
    *gets the channel  advertisement In Use
    *@return TDesC
    */
    virtual const TDesC& GetAdvertisementInUse() const = 0;
	
	/**
	*CIRPreset::GetImgUrl()
	*gets the channel  image url
	*@return TDesC
	*/
	virtual const TDesC& GetImgUrl()const=0;

    /**
    *CIRPreset::GetUrlCount()
	*gets the channel  url count
	*@return TDesC
	*/
	virtual TInt GetUrlCount()const=0;

	virtual CIRChannelServerUrl& GetUrl(TInt aIndex) const = 0;
	
	/*
	 * get the channel url at specified position
	 *
	 */
	virtual const TDesC& GetChannelUrlAt(TInt aIndex) const = 0;
	
	/**
	*CIRPreset::GetAvailableBitrates()
	*gets the bitrate array
	*@return RArray<TInt>
	*/
    virtual RArray<TInt>& GetAvailableBitrates()=0;

	/**
 	*void CIRPresetImpl::CIRIsdsPresetToCIRPresetImpl(const CIRIsdsPreset& aIsdsPreset)
 	*used to convert the isdspresets to plugnized presets.
 	*Does a deep copy for all the member variables
 	*@param CIRIsdsPreset&,the preset to be copied from
	*/
    virtual void CIRIsdsPresetToCIRPresetImpl(const CIRIsdsPreset& aIsdsPreset)=0;

	/**
	*CIRPreset::CopyPresetData()
	*copy data to CIRIsdsPreset
	*@param CIRIsdsPreset&,the preset to be copied into
	*/
	virtual void CopyPresetData(CIRIsdsPreset& aIsdsPreset) const =0;

	 /**
     * Sets the logo data for this preset for a specific size.
     *
     * @param   aData           Raw image data to set.
     * @param   aSize           Size of the logo in pixels.
     */
    virtual void SetLogoData( const TDesC8& aData ) = 0;

    /**
     * Returns the logo data for this preset.
     *
     * If the returned descriptor's length is zero, then it means
     * that the preset does not have a logo.
     */
    virtual  const TDesC8& GetLogoData() const = 0;

    /*
     * Returns the played times of the channel
     */
    virtual TInt GetPlayedTimes() const = 0;
    
    /*
     * Set the played times of the channel
     */
    virtual void SetPlayedTimes(TInt aPlayedTimes) = 0;
    
    virtual TBool GetRenamed() const = 0;
    
    virtual void SetRenamed() = 0;
	};

#endif	//end of IRPRESET_H
