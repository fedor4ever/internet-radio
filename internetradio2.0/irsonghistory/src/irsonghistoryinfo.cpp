/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class for the songhistory information.
*
*/

#include "irsonghistoryinfo.h"

// ---------------------------------------------------------------------------
// Function : NewL
// Function returns an instance of CIRSongHistoryInfo
// Two phase constructor
// ---------------------------------------------------------------------------
//
	EXPORT_C CIRSongHistoryInfo * CIRSongHistoryInfo::NewL()
		{
			CIRSongHistoryInfo* self =  CIRSongHistoryInfo :: NewLC();
			CleanupStack::Pop( self );
			return self;
		}

// ---------------------------------------------------------------------------
//CIRLastPlayedSongs::NewLC()
//Static function
//standard two phased constructor
// ---------------------------------------------------------------------------
//
	EXPORT_C CIRSongHistoryInfo * CIRSongHistoryInfo::NewLC()
		{
			CIRSongHistoryInfo* self = new( ELeave ) CIRSongHistoryInfo();
			CleanupStack::PushL( self );
			return self;
		}
// ---------------------------------------------------------------------------
//CIRSongHistoryInfo::~CIRSongHistoryInfo()
//standard C++ destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CIRSongHistoryInfo::~CIRSongHistoryInfo()
	{
		iArtistInfo.Close();

		iSongInfo.Close();

		iStreamUrlInfo.Close();

		iChannelInfo.Close();
		
		iChannelDescription.Close();

        iImageUrl.Close();
        iGenreName.Close();
        iCountryName.Close();
        iLanguageName.Close();
		
		iMusicStoreStatus.Close();
		
	}

// ---------------------------------------------------------------------------
//CIRSongHistoryInfo::~CIRSongHistoryInfo()
//standard C++ constructor
// ---------------------------------------------------------------------------
//
CIRSongHistoryInfo::CIRSongHistoryInfo()
	 {
		
	 }


// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetArtistL
// Sets Artist name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetArtistL(RBuf& aArtist)
 {
 	iArtistInfo.CreateL(aArtist);
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetSongNameL
// Sets Song name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetSongNameL (RBuf& aSongName)
 {
 	iSongInfo.CreateL(aSongName);
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetStreamUrlL
// Sets Stream url name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetStreamUrlL(RBuf& aStreamUrl)
 {
 	iStreamUrlInfo.CreateL(aStreamUrl);
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelNameL
// Sets Channel name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelNameL(RBuf& aChannelName)
 {
 	iChannelInfo.CreateL(aChannelName);
 }
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelDescL
// Sets Channel name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelDescL(RBuf& aChannelDesc)
 {
 	iChannelDescription.CreateL(aChannelDesc);
 }
// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetImageUrlL
// Sets Channel image Url
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetImageUrlL(RBuf& aImageUrl)
 {
    iImageUrl.CreateL(aImageUrl);
 }
 
 // ---------------------------------------------------------------------------
 // CIRSongHistoryInfo::SetGenreNameL
 // Set channel genre name
 // ---------------------------------------------------------------------------
 //
void CIRSongHistoryInfo::SetGenreNameL(const RBuf& aGenreName)
{
    iGenreName.CreateL(aGenreName);
}

// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetCountryNameL
// Set channel country name
// ---------------------------------------------------------------------------
//
void CIRSongHistoryInfo::SetCountryNameL(const RBuf& aCountryName)
{
    iCountryName.CreateL(aCountryName);
}

// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetLanguageNameL
// Set channel language name
// ---------------------------------------------------------------------------
//
void CIRSongHistoryInfo::SetLanguageNameL(const RBuf& aLanguageName)
{
    iLanguageName.CreateL(aLanguageName);
}
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelMusicStatusL
// Sets Channel's MusicStoreStatus
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelMusicStatusL(RBuf& aChannelMusicStatus)
 {
 	iMusicStoreStatus.CreateL(aChannelMusicStatus);
 }
// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelType
// Sets Channel TYpe
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelType(TInt aChannelType)
 {
 	iChannelType=aChannelType;
 }
// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelId
// Sets Channel Id
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelId(TInt aChannelId)
 {
 	iChannelId=aChannelId;
 }

// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetBitrate
// Sets bitrate
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetBitrate(TInt aBitrate)
 {
    iBitrate=aBitrate;
 }
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetHistoryInfo
// Sets Song History Information.
// ---------------------------------------------------------------------------
//

 EXPORT_C void CIRSongHistoryInfo::SetHistoryInfoL(RBuf& aSongName,
												  RBuf& aArtist, 
												  RBuf& aStreamUrl, 
												  RBuf& aChannelName ,
												  TInt aChannelType, 
												  TInt aChannelId,
												  TInt aBitrate,
												  RBuf& aChannelDesc,
                                                  RBuf& aImageUrl,
                                                  RBuf& aGenreName,
                                                  RBuf& aCountryName,
                                                  RBuf& aLanguageName,
												  RBuf& aMusicStoreStatus
												   )
 {
 	SetSongNameL(aSongName);
 	SetArtistL(aArtist);
 	SetStreamUrlL(aStreamUrl);
 	SetChannelNameL(aChannelName);
    SetChannelType(aChannelType);
    SetChannelId(aChannelId);
    SetBitrate(aBitrate);
    SetChannelDescL(aChannelDesc);
    SetImageUrlL(aImageUrl);
    SetGenreNameL(aGenreName);
    SetCountryNameL(aCountryName);
    SetLanguageNameL(aLanguageName);
    SetChannelMusicStatusL(aMusicStoreStatus);
	
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetSongInfo
// Gets the song information
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetSongInfo() const
 {
 	return iSongInfo;
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetArtistInfo
// Gets Artist name
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetArtistInfo() const
 {
 	return iArtistInfo;
 }
 
// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetStreamUrl
// Gets StreamUrl name
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetStreamUrl() const
 {
 	return iStreamUrlInfo;
 }
 
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetChannelName
// Gets Channel name
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetChannelName() const
 {
 	return iChannelInfo;
 }
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetChannelDesc
// Gets Channel name
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetChannelDesc() const 
 {
 	return iChannelDescription;
 }

// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetImageUrl
// Gets Channel image Url
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetImageUrl() const 
 {
    return iImageUrl;
 }
 // ---------------------------------------------------------------------------
 // CIRSongHistoryInfo::GetGenreName
 // Get channel genre name
 // ---------------------------------------------------------------------------
 //
 
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetGenreName() const 
 {
    return iGenreName;
 }
 
// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetCountryName
// Get channel country name
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetCountryName() const 
 {
    return iCountryName;
 }
 
// ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetLanguageName
// Get channel language name
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetLanguageName() const 
 {
    return iLanguageName;
 }

  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetChannelType
// Gets Channel Type
// ---------------------------------------------------------------------------
//
 EXPORT_C TInt CIRSongHistoryInfo::GetChannelType() const
 {
 	return iChannelType;
 }
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetChannelId
// Gets Channel Id
// ---------------------------------------------------------------------------
//
 EXPORT_C TInt CIRSongHistoryInfo::GetChannelId() const
 {
 	return iChannelId;
 }
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetBitrate
// Gets bitrate
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CIRSongHistoryInfo::GetBitrate() const
{
   return iBitrate;
}
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::GetChannelMusicStatus
// Gets Channel MusicStoreStatus
// ---------------------------------------------------------------------------
//
 EXPORT_C const RBuf& CIRSongHistoryInfo::GetChannelMusicStatus() const 
 {
 	return iMusicStoreStatus;
 }
