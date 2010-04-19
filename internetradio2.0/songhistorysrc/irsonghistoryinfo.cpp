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
// CIRSongHistoryInfo::SetArtist
// Sets Artist name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetArtist(RBuf& aArtist)
 {
 	iArtistInfo.Create(aArtist);
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetSongName
// Sets Song name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetSongName (RBuf& aSongName)
 {
 	iSongInfo.Create(aSongName);
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetStreamUrl
// Sets Stream url name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetStreamUrl(RBuf& aStreamUrl)
 {
 	iStreamUrlInfo.Create(aStreamUrl);
 }
 
  // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelName
// Sets Channel name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelName(RBuf& aChannelName)
 {
 	iChannelInfo.Create(aChannelName);
 }
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelDesc
// Sets Channel name
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelDesc(RBuf& aChannelDesc)
 {
 	iChannelDescription.Create(aChannelDesc);
 }
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetImageUrl
// Sets Channel image Url
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetImageUrl(RBuf& aImageUrl)
 {
    iImageUrl.Create(aImageUrl);
 }
 // ---------------------------------------------------------------------------
// CIRSongHistoryInfo::SetChannelMusicStatus
// Sets Channel's MusicStoreStatus
// ---------------------------------------------------------------------------
//
 void CIRSongHistoryInfo::SetChannelMusicStatus(RBuf& aChannelMusicStatus)
 {
 	iMusicStoreStatus.Create(aChannelMusicStatus);
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
 EXPORT_C void CIRSongHistoryInfo::SetHistoryInfo(RBuf& aSongName,
												  RBuf& aArtist, 
												  RBuf& aStreamUrl, 
												  RBuf& aChannelName ,
												  TInt aChannelType, 
												  TInt aChannelId,
												  TInt aBitrate,
												  RBuf& aChannelDesc,
												  RBuf& aImageUrl,
												  RBuf& aMusicStoreStatus
												   )
 {
 	SetSongName(aSongName);
 	SetArtist(aArtist);
 	SetStreamUrl(aStreamUrl);
 	SetChannelName(aChannelName);
    SetChannelType(aChannelType);
    SetChannelId(aChannelId);
    SetBitrate(aBitrate);
    SetChannelDesc(aChannelDesc);
    SetImageUrl(aImageUrl);
    SetChannelMusicStatus(aMusicStoreStatus);
	
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
