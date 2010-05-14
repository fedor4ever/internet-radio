/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  a new wrapper class in QT 
 *
*/

#include "irqisdsclientimpl.h"   
 
//Static function
//to get an instance of the IRQIsdsClient
//@return IRQIsdsClient *
EXPORT_C IRQIsdsClient *IRQIsdsClient::openInstance()
{
    IRQIsdsClient* irqisdsclient =
            reinterpret_cast<IRQIsdsClient*> (Dll::Tls());

    if (NULL == irqisdsclient)
    {
        irqisdsclient = new IRQIsdsClient();
        int result = 0;
        result = Dll::SetTls(irqisdsclient); 
        
        if( KErrNone != result )
        {
            delete irqisdsclient;
            return NULL;
        }
        
        irqisdsclient->iSingletonInstances = 1;
    }
    else
        irqisdsclient->iSingletonInstances++;

    return irqisdsclient;
}

//
//close the instance.
//@return void
EXPORT_C void IRQIsdsClient::closeInstance()
{
    iSingletonInstances--;

    if (0 == iSingletonInstances)
    {
        Dll::SetTls(NULL);
        delete this;
    }

    return;
}

//Issue a search request to the isds server
//@param QString, the requested search string
//
EXPORT_C void IRQIsdsClient::isdsSearchRequest(const QString& aIsdsSearchString)
{
    iImpl->isdsSearchRequestImpl(aIsdsSearchString);     
}

//Send the category request by the category type
// 
EXPORT_C void IRQIsdsClient::isdsCategoryRequest(
        IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType, bool& aCache)
{ 
    iImpl->isdsCategoryRequestImpl(aIDType, aCache);
}

EXPORT_C bool IRQIsdsClient::isdsIsCategoryCached(IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType)
{
    return iImpl->isdsIsCategoryCachedImpl(aIDType);
}

EXPORT_C bool IRQIsdsClient::isdsIsChannelCached(int aIndex)
{
    return iImpl->isdsIsChannelCachedImpl(aIndex);
}
//Send the channels request by the channel index in the specify category
//
EXPORT_C void IRQIsdsClient::isdsChannelRequest(int aIndex, bool& aCache)
{
    
    if( 0 > aIndex )
        return;
    
    /* if the isds has the banner, the index can't be 0 , or it will crash*/
    if( isdsIsCategoryBanner() && 0 == aIndex )
        return;
    
    iImpl->isdsChannelRequestImpl(aIndex, aCache);
}

//issue a listen request to the isds client
//@param int,bool, the current index of channel, the history tag  
//
EXPORT_C void IRQIsdsClient::isdsListenRequest(int aCurrentIndex,
        bool aHistoryBool)
{ 
    if (0 > aCurrentIndex)
        return;

    /* if the isds has the banner, the index can't be 0 , or it will crash*/
    if (isdsIsChannelBanner() && 0 == aCurrentIndex)
        return;
    
    iImpl->isdsListenRequestImpl(aCurrentIndex,aHistoryBool); 
}

//to syncronize presets
//@param int,QString, the preset id and the last modified tag for the preset   
//
EXPORT_C int IRQIsdsClient::isdsSyncPreset(int aPresetId,
        const QString& aIfModifySince, IRQFavoritesDB *aFavPresets)
{     
    TInt result = 0;
    result = iImpl->isdsSyncPresetImpl(aPresetId, aIfModifySince, aFavPresets);
    return result;
} 

//Cacel the request sent by the UI.
//@param None
//
EXPORT_C void IRQIsdsClient::isdsCancelRequest()
{
    iImpl->isdsCancelRequestImpl();  
}

 

//to see wether category view has a banner.
//@param None
//
EXPORT_C bool IRQIsdsClient::isdsIsCategoryBanner()
{
    return iImpl->isdsIsCategoryBannerImpl();
}

//
//to see wether channel view has a banner.
//@param None
EXPORT_C bool IRQIsdsClient::isdsIsChannelBanner()
{
    return iImpl->isdsIsChannelBannerImpl();
}

//the api is called from the UI(nowplaying view) to download logo.
//@param None
//
EXPORT_C void IRQIsdsClient::isdsLogoDownSendRequest(IRQPreset* aPreset,
        int aNPVReq, int aXValue, int aYValue)
{ 
    iImpl->isdsLogoDownSendRequestImpl(aPreset, aNPVReq, aXValue, aYValue);
}

EXPORT_C bool IRQIsdsClient::isdsIsLogoCached(IRQPreset* aPreset, int aXValue, int aYValue)
{
    return iImpl->isdsIsLogoCachedImpl(aPreset, aXValue, aYValue);
}
//
//the api is called to cancel the current transaction
//@param None
//
EXPORT_C void IRQIsdsClient::isdsLogoDownCancelTransaction()
{
    iImpl->isdsLogoDownCancelTransactionImpl();
}

//
//To know the status of downloading logo
EXPORT_C bool IRQIsdsClient::isdsLogoDownIsRunning() const
{
    return iImpl->isdsLogoDownIsRunningImpl();
}

//takes the url as a parameter and returns the logo data which is in cache
//this API is called form the search results for to display logo on the view
//@param QString: the url of the img, int: the status for getting
//
EXPORT_C void IRQIsdsClient::isdsLogoDownCheckCacheLogo(const QString& aURL,
        int& aStatus)
{ 
    iImpl->isdsLogoDownCheckCacheLogoImpl(aURL, aStatus);
}

//get the cache logo from the logodown engine. The "send" is the point from a logodown engine
//@param None
//
EXPORT_C TDesC8& IRQIsdsClient::isdsLogoDownSendCacheLogo()
{
    return iImpl->isdsLogoDownSendCacheLogoImpl();
}

EXPORT_C void IRQIsdsClient::isdsPostLog(const QString& aFileName)
{
    iImpl->isdsPostLogImpl(aFileName);
}

EXPORT_C void IRQIsdsClient::isdsGetIRID()
{
    iImpl->isdsGetIRIDImpl();
}

EXPORT_C void IRQIsdsClient::isdsGetBrowseBanner(QString& aBannerUrl, QString& aClickThroughUrl)
{
    iImpl->isdsGetBrowseBannerImpl(aBannerUrl, aClickThroughUrl);
}

EXPORT_C void IRQIsdsClient::isdsMultSearch(QString aGenreID, QString aCountryID, QString aLanguageID, QString aSearchText)
{
    iImpl->isdsMultSearchImpl(aGenreID, aCountryID, aLanguageID, aSearchText);
}

EXPORT_C bool IRQIsdsClient::isdsIsConstructSucceed() const
{
    return iImpl->isdsIsConstructSucceed();
}

/************************ private functions **************************/
//the c++ default destruction function
// private
IRQIsdsClient::~IRQIsdsClient()
{    
    delete iImpl;
}


IRQIsdsClient::IRQIsdsClient()
{    
    iImpl = new IRQIsdsClientImpl(); 
    Q_ASSERT(iImpl);
    
    connect(iImpl, SIGNAL(categoryItemsChangedImpl(QList<IRQBrowseCategoryItem *> *)),
                this, SIGNAL(categoryItemsChanged(QList<IRQBrowseCategoryItem *> *)));
    
    connect(iImpl, SIGNAL(channelItemsChangedImpl(QList<IRQChannelItem *> *)),
                    this, SIGNAL(channelItemsChanged(QList<IRQChannelItem *> *)));
    
    connect(iImpl, SIGNAL(operationExceptionImpl(IRQError)),
            this, SIGNAL(operationException(IRQError)));
    
    
    connect(iImpl, SIGNAL(presetResponseImpl(IRQPreset *)),
            this, SIGNAL(presetResponse(IRQPreset *)));
   
    
    connect(iImpl, SIGNAL(syncPresetResultImpl(IRQSycPresetStatus, IRQPreset*)),
                this, SIGNAL(syncPresetResult(IRQSycPresetStatus,  IRQPreset*)));
    
    
    connect(iImpl, SIGNAL(presetLogoDownloadedImpl(IRQPreset*)),
                    this, SIGNAL(presetLogoDownloaded(IRQPreset*)));
   
    
    connect(iImpl, SIGNAL(presetLogoDownloadErrorImpl()),
                        this, SIGNAL(presetLogoDownloadError()));     
   
    
    connect(iImpl, SIGNAL(iridReceivedImpl(QString)), 
                        this, SIGNAL(iridReceived(QString)));
    
}

 
 
