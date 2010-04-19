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
#include <QPixmap>
#include <e32err.h>
#include <fbs.h>
#include <apmstd.h>
#include <apgcli.h>
#include <apgtask.h>
#include <coemain.h> 

#ifdef _DEBUG
#include <QFile>
#include <QTextStream>
#endif

#include "irqutility.h"
#include "irchannelserverurl.h"
#include "irqisdsdatastructure.h"
#include "irisdspreset.h"
#include "irpreset.h"

_LIT8( KIRHtmlMimeType, "text/html" );
/** Browser prefix: See browser API specification */
_LIT( KBrowserPrefix, "4 " );

//
//convert the CIRIsdsPreset to the IRQPreset. 
//
EXPORT_C void IRQUtility::convertCIRIsdsPreset2IRQPrest(const CIRIsdsPreset& aCIRIsdsPreset, IRQPreset& aQIRPreset)
{
  aQIRPreset.uniqID = aCIRIsdsPreset.UniqId(); 
  aQIRPreset.type = aCIRIsdsPreset.GetChannelType();
  aQIRPreset.index  = aCIRIsdsPreset.GetIndex();
  
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
  aCIRIsdsPreset.SetId(aQIRPreset.uniqID);
  aCIRIsdsPreset.SetIndex(aQIRPreset.index);
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
  CIRIsdsPreset * cirIsdsPreset = NULL;  
  TRAPD(err, cirIsdsPreset = CIRIsdsPreset::NewL());
  if( KErrNone != err)
    return;
   
  aCIRPreset.CopyPresetData(*cirIsdsPreset);  
  convertCIRIsdsPreset2IRQPrest(*cirIsdsPreset, aIRQPreset);
  delete cirIsdsPreset;  
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
    if (aUrl.size() < 8)
    {
        return false;
    }

    QString prefix7 = aUrl.left(7);
    QString prefix6 = aUrl.left(6);

    if (0 == prefix7.compare("http://", Qt::CaseInsensitive) ||
        0 == prefix7.compare("rtsp://", Qt::CaseInsensitive) ||
        0 == prefix6.compare("mms://", Qt::CaseInsensitive))
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

EXPORT_C void IRQUtility::writeLog2File(const QString& aString, bool aDeleteOldFile)
{       
#ifdef _DEBUG 
    QFile file("C:\\data\\QTIRLog.txt");
    
    if (file.exists() && aDeleteOldFile)
    {
        bool ret = file.remove();
        if (!ret)
        {
            return;
        }
    }

    bool ret = file.open(QIODevice::ReadWrite);    
    if( !ret )
    {
        return;
    }
    
    QTextStream stream(&file);
    stream << aString;
    stream << "\r\n";    

    stream.flush();
    file.close();
   
#else
    Q_UNUSED(aString);
    Q_UNUSED(aDeleteOldFile);
#endif

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

bool IRQUtility::openAdvLinkL(const QString& aUrl)
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


