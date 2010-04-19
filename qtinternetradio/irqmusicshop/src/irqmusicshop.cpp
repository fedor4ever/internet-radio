/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class implementation for visiting nokia music shop
*
*/


#include <apgtask.h> // TApaTask, TApaTaskList
#include <apgcli.h>  // RApaLsSession
#include <e32std.h>  // TThreadId

#include <w32std.h> // RWsSession
#include <e32cmn.h>
#include <e32def.h>
#ifdef USER_DEFINED_MUSICSHOP
#include <QFile>
#include <QTextStream>
#include <QStringList>
#endif

#ifdef MUSICSHOP_AVAILABLE
#include <e32property.h>
#include <centralrepository.h>
#include <mpxfindinmusicshopcommon.h> // P/S Keys, KSearchUrlMaxSize, KCategoryValueMaxSize
#include <musicstoreuidconstants.h>   // KUidMusicStore
#include <mpxfindinmusicshop.h>       // CMPXFindInMShop
#include <musicwapcenrepkeys.h>       // Central Repository : KCRUidMusicShopSettings,KMusicShopURI
#ifdef USER_DEFINED_MUSICSHOP
const TInt KMaxUrlLenth = 512;
const char* const KNewMusicShopUrl = "http://music.nokia.co.uk";
void getUrlFromConfiguration(QString & aUrl);
#endif //for USER_DEFINED_MUSICSHOP
#endif //for MUSICSHOP_AVAILABLE

#include "irqmusicshop.h"


const TUid KUidShazam = { 0x200265B3 };
bool launchAppByUid(const TUid aUid);

#ifdef MUSICSHOP_AVAILABLE
// ---------------------------------------------------------------------------
// IRQMusicShop::openInstance()
// Static function to get a singleton instance of IRQMusicShop
// ---------------------------------------------------------------------------
//
EXPORT_C IRQMusicShop* IRQMusicShop::openInstance()
{
    // Get singleton instance
    IRQMusicShop* musicShop =
                           reinterpret_cast<IRQMusicShop*>(Dll::Tls());

    if(NULL == musicShop)
    {
        TRAPD(error, musicShop = createInstanceL());
        if (KErrNone != error)
        {
            delete musicShop;
            musicShop = NULL;
            Dll::SetTls(NULL);
        }
    }
    else
    {
        musicShop->iSingletonInstances++;
    }

    return musicShop;
}

// ---------------------------------------------------------------------------
// IRQMusicShop::closeInstance()
// Close a singleton instance of IRQMusicShop
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMusicShop::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}

// ---------------------------------------------------------------------------
// IRQMusicShop::createInstanceL()
// Creates IRQMusicShop instance
// ---------------------------------------------------------------------------
//
IRQMusicShop* IRQMusicShop::createInstanceL()
{
    IRQMusicShop* musicShop = new (ELeave) IRQMusicShop();
    musicShop->constructL();
    User::LeaveIfError(Dll::SetTls(musicShop));
    musicShop->iSingletonInstances = 1;
    return musicShop;
}

// ---------------------------------------------------------------------------
// IRQMusicShop::IRQMusicShop()
// Default Constructor.
// ---------------------------------------------------------------------------
//
IRQMusicShop::IRQMusicShop():iMPXFindInShop(NULL),
                             iSingletonInstances(0)   
                             #ifdef USER_DEFINED_MUSICSHOP
                             ,iResetAlready(false)
                             #endif
{   
    _LIT_SECURITY_POLICY_C1(KReadPolicy, ECapabilityReadUserData);
    _LIT_SECURITY_POLICY_C1(KWritePolicy, ECapabilityWriteUserData);
    
    RProperty::Define(KUidMusicStore, KMShopCategoryId, RProperty::EInt, KReadPolicy, KWritePolicy);
    RProperty::Define(KUidMusicStore, KMShopCategoryName, RProperty::ELargeText, KReadPolicy, KWritePolicy);    
}

// ---------------------------------------------------------------------------
// IRQMusicShop::constructL()
// Two-Phase Constructor.
// ---------------------------------------------------------------------------
//
void IRQMusicShop::constructL()
{
    iMPXFindInShop = CMPXFindInMShop::NewL();
#ifdef USER_DEFINED_MUSICSHOP
    resetMusicShopUrlL();
#endif 
}

// ---------------------------------------------------------------------------
// IRQMusicShop::~IRQMusicShop()
// Destructor
// ---------------------------------------------------------------------------
//
IRQMusicShop::~IRQMusicShop()
{
    delete iMPXFindInShop;
    iMPXFindInShop = NULL;
#ifdef USER_DEFINED_MUSICSHOP    
    TRAP_IGNORE(restoreMusicShopUrlL());
#endif
}

// ---------------------------------------------------------------------------
// IRQMusicShop::findInMusicShop()
// Find song in music shop app.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQMusicShop::findInMusicShop(const QString& aSongName,
                                            const QString& aArtistName,
                                            const QString& aAlbumName)
{
    if(NULL == iMPXFindInShop)
    {
        return false;
    }

    TPtrC songName(reinterpret_cast<const TUint16*>(aSongName.utf16()));
    TPtrC artistName(reinterpret_cast<const TUint16*>(aArtistName.utf16()));
    TPtrC albumName(reinterpret_cast<const TUint16*>(aAlbumName.utf16()));
    
    HBufC* url = NULL;
    TRAPD(error, url = iMPXFindInShop->CreateSearchURLL(songName,
                                                artistName,
                                                albumName,
                                                KNullDesC,    // Composer - Not used
                                                KNullDesC)); // Genre - Not used
    
    bool retVal = false;
    if( (KErrNone == error) &&
        (KErrNone == RProperty::Set(KUidMusicStore,
                          KMShopCategoryId,
                          KFindInMShopKeyValid)) &&
        (KErrNone == RProperty::Set(KUidMusicStore,
                          KMShopCategoryName,
                          *url))
      )
    {
        retVal = launchMusicShop();
    }

    delete url;
    REComSession::FinalClose();
    return retVal;
}

// ---------------------------------------------------------------------------
// IRQMusicShop::launchMusicShop()
// Launch music shop app.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQMusicShop::launchMusicShop()
{
    return launchAppByUid(KUidMusicStore);
}

// ---------------------------------------------------------------------------
// IRQMusicShop::launchShazam()
// Launch shazam app.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQMusicShop::launchShazam()
{
    return launchAppByUid(KUidShazam);
}


#ifdef USER_DEFINED_MUSICSHOP
// ---------------------------------------------------------------------------
// resetMusicShopUrlL
// Reset Music Shop Url in the Central Repository
// ---------------------------------------------------------------------------
//
void IRQMusicShop::resetMusicShopUrlL()
{
    CRepository* pRepository = CRepository::NewL(KCRUidMusicShopSettings);
    CleanupStack::PushL(pRepository);

    TInt urlLenth = KMaxUrlLenth;
    HBufC* valueString = HBufC::NewLC(urlLenth);
    
    TInt error = KErrNone;
    do
    {
        TPtr ptr = valueString->Des();
        error = pRepository->Get(KMusicShopURI,ptr);
        if(error == KErrTooBig)
        {
            CleanupStack::PopAndDestroy(valueString);
            urlLenth = 2 * urlLenth;
            valueString = HBufC::NewLC(urlLenth);
        }
    } while(error == KErrTooBig);
    
    User::LeaveIfError(error);

    iMusicShopUrl =  QString::fromUtf16(valueString->Ptr(),valueString->Length());
        
    QString newMusicShopUrl(KNewMusicShopUrl);
    getUrlFromConfiguration(newMusicShopUrl);
    
    
    if(0 != iMusicShopUrl.compare(newMusicShopUrl,Qt::CaseInsensitive))
    {   
        TPtrC urlDesc(reinterpret_cast<const TUint16*>(newMusicShopUrl.utf16())); 
        User::LeaveIfError(pRepository->Set(KMusicShopURI,urlDesc));
        iResetAlready = true;
    }

    CleanupStack::PopAndDestroy(valueString);
    CleanupStack::PopAndDestroy(pRepository);
}

void IRQMusicShop::restoreMusicShopUrlL()
{
    if(iResetAlready)
    {
        CRepository* pRepository = CRepository::NewL(KCRUidMusicShopSettings);
        TPtrC16 oldMusicShopUrl(reinterpret_cast<const TUint16*>(iMusicShopUrl.utf16()));
        pRepository->Set(KMusicShopURI,oldMusicShopUrl);
        delete pRepository;
    }
}


void getUrlFromConfiguration(QString & aUrl)
{
    QFile file("C:\\data\\QTIRConfigure.txt");
    if (file.open(QIODevice::ReadOnly)) 
    {
        QTextStream stream( &file );
        QString line;
        QStringList parameter;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            parameter = line.split("=");
            if (parameter.count() == 2)
            {
                if (parameter.first() == "userDefinedMusicStoreUrl")
                {
                    aUrl = parameter.last();
                    break;
                }
            }
        }
        file.close();
    }
}
#endif

#else //for MUSICSHOP_AVAILABLE
// ---------------------------------------------------------------------------
// IRQMusicShop::openInstance()
// Static function to get a singleton instance of IRQMusicShop
// ---------------------------------------------------------------------------
//
EXPORT_C IRQMusicShop* IRQMusicShop::openInstance()
{
    // Get singleton instance
    IRQMusicShop* musicShop =
                           reinterpret_cast<IRQMusicShop*>(Dll::Tls());

    if(NULL == musicShop)
    {
        musicShop = new IRQMusicShop();
        musicShop->iSingletonInstances = 1;
    }
    else
    {
        musicShop->iSingletonInstances++;
    }

    return musicShop;
}

// ---------------------------------------------------------------------------
// IRQMusicShop::closeInstance()
// Close a singleton instance of IRQMusicShop
// ---------------------------------------------------------------------------
//
EXPORT_C void IRQMusicShop::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }
}


// ---------------------------------------------------------------------------
// IRQMusicShop::IRQMusicShop()
// Default Constructor.
// ---------------------------------------------------------------------------
//
IRQMusicShop::IRQMusicShop():iMPXFindInShop(NULL),
                             iSingletonInstances(0)   
                             #ifdef USER_DEFINED_MUSICSHOP
                             ,iResetAlready(false)
                             #endif
{   

}

// ---------------------------------------------------------------------------
// IRQMusicShop::~IRQMusicShop()
// Destructor
// ---------------------------------------------------------------------------
//
IRQMusicShop::~IRQMusicShop()
{

}

// ---------------------------------------------------------------------------
// IRQMusicShop::findInMusicShop()
// Find song in music shop app.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQMusicShop::findInMusicShop(const QString& aSongName,
                                            const QString& aArtistName,
                                            const QString& aAlbumName)
{
    Q_UNUSED(aSongName)
    Q_UNUSED(aArtistName)
    Q_UNUSED(aAlbumName)
    return false;
}

// ---------------------------------------------------------------------------
// IRQMusicShop::launchMusicShop()
// Launch music shop app.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQMusicShop::launchMusicShop()
{
	return false;
}

// ---------------------------------------------------------------------------
// IRQMusicShop::launchShazam()
// Launch shazam app.
// ---------------------------------------------------------------------------
//
EXPORT_C bool IRQMusicShop::launchShazam()
{
    return launchAppByUid(KUidShazam);
}

#endif //for MUSICSHOP_AVAILABLE

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
