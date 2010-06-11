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
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <xqsettingsmanager.h>
#include <xqsettingskey.h>

#include <e32err.h>
#include <apmstd.h>
#include <apgcli.h>
#include <apgtask.h>
#include <coemain.h> 

#include "irqutility.h"
#include "irchannelserverurl.h"
#include "irqisdsdatastructure.h"
#include "irisdspreset.h"
#include "irpreset.h"

_LIT8( KIRHtmlMimeType, "text/html" );
/** Browser prefix: See browser API specification */
_LIT( KBrowserPrefix, "4 " );

const long int KCenrepUidInternetRadio = { 0x2000B499 };
const unsigned long int KSongRecognitionKeyId = { 0x00000035 };

static bool openAdvLinkL(const QString& aUrl);
static TUid getSongRecognitionAppUid();
static bool launchAppByUid(const TUid aUid);
  
//
//convert the CIRIsdsPreset to the IRQPreset. 
//
EXPORT_C void IRQUtility::convertCIRIsdsPreset2IRQPrest(const CIRIsdsPreset& aCIRIsdsPreset, IRQPreset& aQIRPreset)
{
  aQIRPreset.type = aCIRIsdsPreset.GetChannelType();
  
  aQIRPreset.presetId = aCIRIsdsPreset.GetId();
  aQIRPreset.iChannelUrlCount  = aCIRIsdsPreset.GetUrlCount();
  aQIRPreset.name = QString::fromUtf16((aCIRIsdsPreset.GetName()).Ptr(), (aCIRIsdsPreset.GetName()).Length());
  
  aQIRPreset.languageCode  = QString::fromUtf16(aCIRIsdsPreset.GetLangId().Ptr(), aCIRIsdsPreset.GetLangId().Length());
  aQIRPreset.languageName  = QString::fromUtf16(aCIRIsdsPreset.GetLangName().Ptr(), aCIRIsdsPreset.GetLangName().Length());
  
  aQIRPreset.countryCode  = QString::fromUtf16(aCIRIsdsPreset.GetCountryId().Ptr(),aCIRIsdsPreset.GetCountryId().Length());
  aQIRPreset.countryName  = QString::fromUtf16(aCIRIsdsPreset.GetCountryName().Ptr(),aCIRIsdsPreset.GetCountryName().Length());
  
  aQIRPreset.lastModified  = QString::fromUtf16(aCIRIsdsPreset.GetLastModifiedTime().Ptr(), aCIRIsdsPreset.GetLastModifiedTime().Length());
  aQIRPreset.description  = QString::fromUtf16(aCIRIsdsPreset.GetDescription().Ptr(), aCIRIsdsPreset.GetDescription().Length());
  aQIRPreset.shortDesc  = QString::fromUtf16(aCIRIsdsPreset.GetShortDescription().Ptr(), aCIRIsdsPreset.GetShortDescription().Length());
  
  aQIRPreset.genreId  = QString::fromUtf16(aCIRIsdsPreset.GetGenreId().Ptr(), aCIRIsdsPreset.GetGenreId().Length());
  aQIRPreset.genreName  = QString::fromUtf16(aCIRIsdsPreset.GetGenreName().Ptr(), aCIRIsdsPreset.GetGenreName().Length());
  
  aQIRPreset.advertisementUrl  = QString::fromUtf16(aCIRIsdsPreset.GetAdvertisementUrl().Ptr(), aCIRIsdsPreset.GetAdvertisementUrl().Length());
  aQIRPreset.advertisementInUse  = QString::fromUtf16(aCIRIsdsPreset.GetAdvertisementInUse().Ptr(),aCIRIsdsPreset.GetAdvertisementInUse().Length());
  
  aQIRPreset.imgUrl  = QString::fromUtf16(aCIRIsdsPreset.GetImgUrl().Ptr(), aCIRIsdsPreset.GetImgUrl().Length());   
  aQIRPreset.logoData.Close();
  aQIRPreset.logoData.Create(aCIRIsdsPreset.GetLogoData());
  
  aQIRPreset.musicStoreStatus  = QString::fromUtf16(aCIRIsdsPreset.GetMusicStoreStatus().Ptr(), aCIRIsdsPreset.GetMusicStoreStatus().Length());
  aQIRPreset.clearChannelServerList();
  
  IRQChannelServerURL url;
  CIRChannelServerUrl *tempServerUrl = NULL;
  
  for(int i=0; i<aCIRIsdsPreset.iUrlArray->Count(); i++)
  {
      tempServerUrl = aCIRIsdsPreset.iUrlArray->At(i);
      url.bitrate = tempServerUrl->GetBitRate();
      url.serverName = QString::fromUtf16(tempServerUrl->GetServerName().Ptr(), tempServerUrl->GetServerName().Length());
      url.url = QString::fromUtf16(tempServerUrl->GetServerUrl().Ptr(), tempServerUrl->GetServerUrl().Length());
      aQIRPreset.insertChannelServer(url);
  }
  
  aQIRPreset.sortURLArray();   
}

//convert the IRQPreset to the CIRIsdsPreset 
//
EXPORT_C void IRQUtility::convertIRQPreset2CIRIsdsPreset(const IRQPreset& aQIRPreset, CIRIsdsPreset& aCIRIsdsPreset)
{
  aCIRIsdsPreset.SetId(aQIRPreset.presetId);
  aCIRIsdsPreset.SetUrlCount(aQIRPreset.iChannelUrlCount);
  aCIRIsdsPreset.SetChannelType(aQIRPreset.type);  
  
  TPtrC16 namePtr(reinterpret_cast<const TUint16*>(aQIRPreset.name.utf16()));
  aCIRIsdsPreset.SetName(namePtr);
  
  TPtrC16 languageCodePtr(reinterpret_cast<const TUint16*>(aQIRPreset.languageCode.utf16()));
  aCIRIsdsPreset.SetLangCode(languageCodePtr);
  
  TPtrC16 languageNamePtr(reinterpret_cast<const TUint16*>(aQIRPreset.languageName.utf16()));
  aCIRIsdsPreset.SetLang(languageNamePtr);
  
  TPtrC16 countryCode(reinterpret_cast<const TUint16*>(aQIRPreset.countryCode.utf16()));
  aCIRIsdsPreset.SetCountryCode(countryCode);
  
  TPtrC16 countryName(reinterpret_cast<const TUint16*>(aQIRPreset.countryName.utf16()));
  aCIRIsdsPreset.SetCountryName(countryName);
  
  TPtrC16 lastModified(reinterpret_cast<const TUint16*>(aQIRPreset.lastModified.utf16()));
  aCIRIsdsPreset.SetLastModifiedTime(lastModified);
  
  TPtrC16 description(reinterpret_cast<const TUint16*>(aQIRPreset.description.utf16()));
  aCIRIsdsPreset.SetDescription(description);
  
  TPtrC16 shortDescription(reinterpret_cast<const TUint16*>(aQIRPreset.shortDesc.utf16()));
  aCIRIsdsPreset.SetShortDesc(shortDescription);
  
  TPtrC16 genreId(reinterpret_cast<const TUint16*>(aQIRPreset.genreId.utf16()));
  aCIRIsdsPreset.SetGenreId(genreId);
  
  TPtrC16 genreName(reinterpret_cast<const TUint16*>(aQIRPreset.genreName.utf16()));
  aCIRIsdsPreset.SetGenreName(genreName);
  
  TPtrC16 advertisementUrl(reinterpret_cast<const TUint16*>(aQIRPreset.advertisementUrl.utf16()));
  aCIRIsdsPreset.SetAdvertisementUrl(advertisementUrl);
  
  TPtrC16 advertisementINUse(reinterpret_cast<const TUint16*>(aQIRPreset.advertisementInUse.utf16()));
  aCIRIsdsPreset.SetAdvertisementInUse(advertisementINUse);
  
  TPtrC16 imgUrl(reinterpret_cast<const TUint16*>(aQIRPreset.imgUrl.utf16()));
  aCIRIsdsPreset.SetImgUrl(imgUrl);
  
  TPtrC16 musicStoreStatus(reinterpret_cast<const TUint16*>(aQIRPreset.musicStoreStatus.utf16()));
  aCIRIsdsPreset.SetMusicStoreStatus(musicStoreStatus);   
  aCIRIsdsPreset.SetLogoData(aQIRPreset.logoData);

  TRAP_IGNORE(appendURLL(aQIRPreset, aCIRIsdsPreset));    
}

//convert CIRPreset to the IRQPreset 
//
EXPORT_C void IRQUtility::convertCIRPreset2IRQPreset(const CIRPreset& aCIRPreset, IRQPreset& aIRQPreset)
{
    aIRQPreset.uniqID = aCIRPreset.Id(); 
    aIRQPreset.type = aCIRPreset.GetChannelType();
    aIRQPreset.index  = aCIRPreset.Index();
    
    aIRQPreset.presetId = aCIRPreset.GetId();
    aIRQPreset.iChannelUrlCount  = aCIRPreset.GetUrlCount();
    aIRQPreset.name = QString::fromUtf16((aCIRPreset.Name()).Ptr(), (aCIRPreset.Name()).Length());
    
    aIRQPreset.languageCode  = QString::fromUtf16(aCIRPreset.GetLangId().Ptr(), aCIRPreset.GetLangId().Length());
    aIRQPreset.languageName  = QString::fromUtf16(aCIRPreset.GetLangName().Ptr(), aCIRPreset.GetLangName().Length());
    
    aIRQPreset.countryCode  = QString::fromUtf16(aCIRPreset.GetCountryId().Ptr(),aCIRPreset.GetCountryId().Length());
    aIRQPreset.countryName  = QString::fromUtf16(aCIRPreset.GetCountryName().Ptr(),aCIRPreset.GetCountryName().Length());
    
    aIRQPreset.lastModified  = QString::fromUtf16(aCIRPreset.GetLastModifiedTime().Ptr(), aCIRPreset.GetLastModifiedTime().Length());
    aIRQPreset.description  = QString::fromUtf16(aCIRPreset.GetDescription().Ptr(), aCIRPreset.GetDescription().Length());
    aIRQPreset.shortDesc  = QString::fromUtf16(aCIRPreset.GetShortDescription().Ptr(), aCIRPreset.GetShortDescription().Length());
    
    aIRQPreset.genreId  = QString::fromUtf16(aCIRPreset.GetGenreId().Ptr(), aCIRPreset.GetGenreId().Length());
    aIRQPreset.genreName  = QString::fromUtf16(aCIRPreset.GetGenreName().Ptr(), aCIRPreset.GetGenreName().Length());
    
    aIRQPreset.advertisementUrl  = QString::fromUtf16(aCIRPreset.GetAdvertisementUrl().Ptr(), aCIRPreset.GetAdvertisementUrl().Length());
    aIRQPreset.advertisementInUse  = QString::fromUtf16(aCIRPreset.GetAdvertisementInUse().Ptr(),aCIRPreset.GetAdvertisementInUse().Length());
    
    aIRQPreset.imgUrl  = QString::fromUtf16(aCIRPreset.GetImgUrl().Ptr(), aCIRPreset.GetImgUrl().Length());   
    
    aIRQPreset.musicStoreStatus  = QString::fromUtf16(aCIRPreset.GetMusicStoreStatus().Ptr(), aCIRPreset.GetMusicStoreStatus().Length());
    aIRQPreset.clearChannelServerList();
    
    IRQChannelServerURL url;
    
    for(int i=0; i<aCIRPreset.GetUrlCount(); i++)
    {
        CIRChannelServerUrl& tempServerUrl = aCIRPreset.GetUrl(i);
        url.bitrate = tempServerUrl.GetBitRate();
        url.serverName = QString::fromUtf16(tempServerUrl.GetServerName().Ptr(), tempServerUrl.GetServerName().Length());
        url.url = QString::fromUtf16(tempServerUrl.GetServerUrl().Ptr(), tempServerUrl.GetServerUrl().Length());
        aIRQPreset.insertChannelServer(url);
    }
    
    aIRQPreset.sortURLArray();   
}

//switch from the symbian error code to the IRQ error code 
//
EXPORT_C void IRQUtility::convertSError2QError(const int aSError, int& aQError)
{
  if( aSError > 0 )
  {
    aQError = aSError;
    return;
  }
  
  switch( aSError )
  {
    case KErrNone:
      aQError = EIRQErrorNone;
      break;
    case KErrNotFound:
      aQError = EIRQErrorNotFound;
      break;
    case KErrAlreadyExists:
      aQError = EIRQErrorAlreadyExist;
      break;
      
    case KErrNoMemory:
        aQError = EIRQErrorOutOfMemory;
        break;
        
    default:
      aQError = EIRQErrorGeneral;
      break;
  } 
}

/*
 * Description : check the validity of URL
 * return value: true: the URL is valid, vice versa.
 */
EXPORT_C bool IRQUtility::isValidUrl(const QString& aUrl)
{
    QString temp = aUrl.trimmed();
    if (temp.size() < 8)
    {
        return false;
    }

    if ( temp.startsWith("http://", Qt::CaseInsensitive) ||
         temp.startsWith("rtsp://", Qt::CaseInsensitive) ||
         temp.startsWith("mms://", Qt::CaseInsensitive) ||
		 temp.startsWith("mmst://", Qt::CaseInsensitive) ||
		 temp.startsWith("mmsu://", Qt::CaseInsensitive))
    {
        return true;
    }

    return false;
}

/*
 * Description : check the validity of URL
 * return value: true: the URL is valid, vice versa.
 */
EXPORT_C bool IRQUtility::openAdvLink(const QString& aUrl)
{
    bool retval = false;
    TRAP_IGNORE(retval = openAdvLinkL(aUrl));

    return retval;
}

EXPORT_C bool IRQUtility::identifySongAvailable()
{
    RApaLsSession lsSession;
    if( lsSession.Connect() != KErrNone )
    {
        return false;
    }
    
    bool ret = false;
    TUid songRecognitionAppUid = getSongRecognitionAppUid();
    TApaAppInfo appInfo;
    lsSession.GetAllApps();  

    while( KErrNone == lsSession.GetNextApp(appInfo) )
    {
        TBool appIsHidden = EFalse;
        TApaAppCapabilityBuf capability;
        if( KErrNone == lsSession.GetAppCapability(capability,appInfo.iUid) )
        {
            appIsHidden = capability().iAppIsHidden;
        }
        
        if(!appIsHidden)
        {
            if(songRecognitionAppUid == appInfo.iUid)
            {
                ret = true;
                break;
            }
        }
    }
    
    lsSession.Close();
    return ret;         
}   
    
    
EXPORT_C bool IRQUtility::openSongRecognition()
{       
    return launchAppByUid(getSongRecognitionAppUid());
}

void IRQUtility::appendURLL(const IRQPreset& aQIRPreset, CIRIsdsPreset& aCIRIsdsPreset)
{
    int count = aQIRPreset.urlArray.count();
    for( int i=0; i<count; i++)
    {
      CIRChannelServerUrl*  tempServerUrl = CIRChannelServerUrl::NewLC(); 
      TPtrC16 serverName(reinterpret_cast<const TUint16*>(aQIRPreset.urlArray[i].serverName.utf16()));
      TPtrC16 url(reinterpret_cast<const TUint16*>(aQIRPreset.urlArray[i].url.utf16()));
      tempServerUrl->SetServerName(serverName);
      tempServerUrl->SetServerUrl(url);
      tempServerUrl->SetBitRate(aQIRPreset.urlArray[i].bitrate);
      aCIRIsdsPreset.iUrlArray->AppendL(tempServerUrl);
      CleanupStack::Pop(tempServerUrl);     
    } 
}

bool openAdvLinkL(const QString& aUrl)
{
    if (aUrl.size() == 0)
    {
        return false;
    }

    bool ret = true;

    RWsSession wsSession;
    User::LeaveIfError(wsSession.Connect());
    CleanupClosePushL(wsSession);

    RApaLsSession ls;
    User::LeaveIfError(ls.Connect());
    CleanupClosePushL( ls );

    TDataType htmlDataType( KIRHtmlMimeType );
    TUid uid;
    TPtrC16 httpLink(reinterpret_cast<const TUint16*>(aUrl.utf16()));;

    // Get UID of the browser.
    TInt retVal = ls.AppForDataType( htmlDataType, uid );

    if ( retVal == KErrNone && uid != KNullUid )
    {
        // We must check if the browser is already open and if it is, 
        // switch its currently opened file to the correct URL.
        TApaTaskList taskList( wsSession );
        TApaTask task = taskList.FindApp( uid );
        if ( task.Exists() )
        {
            HBufC8* url8 = HBufC8::NewLC( httpLink.Length() );
            url8->Des().Copy( httpLink );
            task.SendMessage( TUid::Uid( 0 ), url8->Des() );
            task.BringToForeground();
            CleanupStack::PopAndDestroy( url8 );
        }
        else
        {
            TThreadId threadId;
            HBufC* newUrl = HBufC::NewLC( httpLink.Length() + KBrowserPrefix().Length() );
            newUrl->Des().Append( KBrowserPrefix );
            newUrl->Des().Append( httpLink );
            ls.StartDocument( newUrl->Des(), uid, threadId );
            CleanupStack::PopAndDestroy( newUrl );
        }
    }
    else
    {
        ret = false;
    }
    CleanupStack::PopAndDestroy( &ls );

    wsSession.Flush();    
    CleanupStack::PopAndDestroy( &wsSession );

    return ret;
}

bool launchAppByUid(const TUid aUid)
{
    RWsSession wsSession;
    if(KErrNone != wsSession.Connect())
    {
        return false;
    }
    
    bool retVal = false;
    TApaTaskList tasList(wsSession);
    TApaTask task = tasList.FindApp(aUid);

    if(task.Exists())
    {
        task.BringToForeground();
        retVal = true;
    }
    else
    {
        RApaLsSession session;
        if(KErrNone == session.Connect())
        {
            TThreadId threadId;
            TInt err = session.CreateDocument(KNullDesC, aUid, threadId);
            if(KErrNone == err)
            {
                retVal = true;
            }
            session.Close();
        }      	
    }
    
    wsSession.Flush();    
    wsSession.Close();
    return retVal;
}

TUid getSongRecognitionAppUid()
{
    // Create an XQSettingsManager object
    XQSettingsManager settingsManager;
    // Create an XQSettingsKey object for a particular central repository item.
    XQSettingsKey appUidKey(XQSettingsKey::TargetCentralRepository, 
        KCenrepUidInternetRadio, KSongRecognitionKeyId);
    
    // Read the integer value
    int uidValue = settingsManager.readItemValue(appUidKey, XQSettingsManager::TypeInt).toInt();

    return TUid::Uid(uidValue);
}


