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
* Description:  Wrapper class for Isds functions
*
*/

#include "irisdswrapper.h"
#include "isdsclientdll.h"
#include "irdebug.h"
#include "irsettings.h"
#include "misdsresponseobserver.h"
#include "mviewsresponseanderrorobserver.h"
#include "irbrowsecatagoryitems.h"
#include "irbrowsechannelitems.h"
#include "irisdspreset.h"
#include "irfavoritesdb.h"
#include "irdialoglauncher.h"
#include <internetradio.rsg>
#include <stringloader.h>
#include "irdataproviderconstants.h"
#include "ir.hrh"
#include "msyncpresetobserver.h"
#include <aknwaitdialog.h>


_LIT(KFalse,"false");
const TInt KStationsDataMaxLength = 5;
const TInt KCategoryDataMaxLength = 5;
const TInt KBufSize = 100;
const TInt KSize = 10;


// ---------------------------------------------------------------------------
// CIRIsdsWrapper* CIRIsdsWrapper::NewL()
// function to return an instance of CIRIsdsWrapper 
// Two phase constructor
// ---------------------------------------------------------------------------
//
CIRIsdsWrapper* CIRIsdsWrapper::NewL(CIRSettings& aIRSettings,CIRFavoritesDb& aFavPresets)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::NewL - Entering" );
	CIRIsdsWrapper* self = CIRIsdsWrapper::NewLC(aIRSettings,aFavPresets);
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRIsdsWrapper::NewL - Exiting" );
	return self;
	}
        
// ---------------------------------------------------------------------------
// CIRIsdsWrapper* CIRIsdsWrapper::NewLC()
// Two-phased constructor.
// Create a CIRIsdsWrapper object, which will draw itself
// to aRect.
// ---------------------------------------------------------------------------
//
CIRIsdsWrapper* CIRIsdsWrapper::NewLC(CIRSettings& aIRSettings,CIRFavoritesDb& aFavPresets)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::NewLC - Entering" );
	CIRIsdsWrapper* self = new (ELeave) CIRIsdsWrapper(aIRSettings,aFavPresets);
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRIsdsWrapper::NewLC - Exiting." );
	return self;
	}

// ---------------------------------------------------------------------------
// CIRIsdsWrapper:: ~CIRIsdsWrapper()
// Destructor..
// ---------------------------------------------------------------------------
//
CIRIsdsWrapper:: ~CIRIsdsWrapper()
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::~CIRIsdsWrapper - Entering" );
	if( iISDSClient )
	{
	delete iISDSClient;
	}
	delete iStationsData;
	delete iDiaologLauncher;
	delete iCategoryData;
	delete iChannelImgUrl;

	iChannelIdArray.Close();
	if(iBannerUrl) 
		{
		delete iBannerUrl;
		}
	iBannerUrl = NULL;


    if (iClickThroughUrl)
		{
		delete iClickThroughUrl;
		}
    iClickThroughUrl = NULL;
    
    if(iCatBannerUrl) 
		{
		delete iCatBannerUrl;
		}
	iCatBannerUrl = NULL;


    if (iCatClickThroughUrl)
		{
		delete iCatClickThroughUrl;
		}
    iCatClickThroughUrl = NULL;
	if(iIsdsWaitDialog)
    	{
    	delete iIsdsWaitDialog;
    	iIsdsWaitDialog= NULL;
    	}
	IRLOG_DEBUG( "CIRIsdsWrapper::~CIRIsdsWrapper - Exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRIsdsWrapper::ConstructL()
// Constructs the IsdsClient object
// ---------------------------------------------------------------------------
//
 
void CIRIsdsWrapper::ConstructL()
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::ConstructL - Entering" );

	// ISDS INTERFACE 
	_LIT(KServerAddress, "http://idirectory.xgns.net/isds");
	//iISDSClient = CIRIsdsClient::NewL(*this , iIRSettings.GetISDSBaseUrlL());
	iISDSClient = CIRIsdsClient::NewL(*this , KServerAddress);
	iCategoryData = new (ELeave) CDesCArrayFlat(KStationsDataMaxLength);

	iStationsData = new (ELeave) CDesCArrayFlat(KStationsDataMaxLength);
	iChannelImgUrl = new (ELeave) CDesCArrayFlat(KStationsDataMaxLength);
	//Dialog Launcher instance
	iDiaologLauncher = CIRDialogLauncher::NewL();
	iReqAddToFav = EFalse;
	iError = EFalse;
	iConnTimeOut = EFalse;
	iBannerUrl = NULL;
	iClickThroughUrl = NULL;
	iCatBannerUrl = NULL;
	iCatClickThroughUrl = NULL;
	iIsdsWaitDialog = NULL;
	iViewObserver = NULL;
	IRLOG_DEBUG( "CIRIsdsWrapper::ConstructL - Exiting" );
	}
		
// ---------------------------------------------------------------------------
// void CIRIsdsWrapper::CIRIsdsWrapper()
// Default Constructor
// ---------------------------------------------------------------------------
//
CIRIsdsWrapper::CIRIsdsWrapper(CIRSettings& aIRSettings,CIRFavoritesDb& aFavPresets):
				iIRSettings(aIRSettings),iFavPresets(aFavPresets)
	{
   	IRLOG_DEBUG( "CIRIsdsWrapper::CIRIsdsWrapper" );
	//No implementation
   }
// ---------------------------------------------------------------------------
// CIRIsdsClient* CIRIsdsWrapper::GetISDSInstance()
// Returns the ISDS Instance
// ---------------------------------------------------------------------------
//
CIRIsdsClient* CIRIsdsWrapper::GetISDSInstance()
   {
   IRLOG_DEBUG( "CIRIsdsWrapper::GetISDSInstance" );
   return iISDSClient;
   }
// ---------------------------------------------------------------------------
// CIRIsdsClient* CIRIsdsWrapper::GetISDSInstance()
// Returns the ISDS Instance
// ---------------------------------------------------------------------------
//
CIRDialogLauncher* CIRIsdsWrapper::GetDialogLauncherInstance()
   {
   IRLOG_DEBUG( "CIRIsdsWrapper::GetDialogLauncherInstance" );
   return iDiaologLauncher;
   }
// ---------------------------------------------------------------------------
// void CIRIsdsWrapper::IsdsSearchRequestL()
// Issues a search request to the Isds Client
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsSearchRequestL(MViewsResponseAndErrorObserver* aViewObserver,
										const TDesC& aIsdsSearchString)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsSearchRequestL started." );
	iViewObserver = aViewObserver;	
	iSSearchRequest = ETrue;
	iListenRequest=EFalse;
	iWaitDialogWhileSearch=ETrue;
	CreateWaitDialogL();
	iISDSClient->IRISDSSearchL(aIsdsSearchString);
	iWaitDialogWhileSearch=EFalse;
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsSearchRequestL exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRIsdsWrapper::IsdsCategoryRequestL()
// Issues a Category request to the Isds Client
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsCategoryRequestL(MViewsResponseAndErrorObserver* aViewObserver,
										  CIRIsdsClient::TIRIsdsclientInterfaceIDs aIDType)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCategoryRequestL started." );
	iViewObserver = aViewObserver;	
	iListenRequest=EFalse;
	CreateWaitDialogL();
	iISDSClient->IRIsdsClientIntefaceL(aIDType);
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCategoryRequestL exiting" );
	}
// ---------------------------------------------------------------------------
// void CIRIsdsWrapper::IsdsChannelRequestL()
// Issues a Channel request to the Isds Client
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsChannelRequestL(MViewsResponseAndErrorObserver* aViewObserver,TInt aIndex)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsChannelRequestL started." );
	iViewObserver = aViewObserver;	
	iListenRequest=EFalse;
	CreateWaitDialogL();
	iISDSClient->IRIsdsClientIntefaceL(aIndex,CIRIsdsClient::ECatagory);
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsChannelRequestL exiting" );
	}

// ---------------------------------------------------------------------------
// void CIRIsdsWrapper::IsdsListenRequestL()
// Issues a Listen request to the Isds Client
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsListenRequestL(MViewsResponseAndErrorObserver* aViewObserver,
										TInt aCurrentIndex,TBool aHistoryBool)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsListenRequestL started." );
	iViewObserver = aViewObserver;
	iListenRequest=ETrue;
	iHistoryBool = aHistoryBool ;
	if(aHistoryBool)
		{
		iISDSClient->IRIsdsClientIntefaceL(aCurrentIndex,CIRIsdsClient::EChannels,ETrue);
		}
	else
		{
		iCurrentPlayingIndex = aCurrentIndex;
		iISDSClient->IRIsdsClientIntefaceL(aCurrentIndex,CIRIsdsClient::EChannels);
		}
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsListenRequestL exiting" );
	}



// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsErrorL()
// Used to Indiacte error in retrieving data from Isds
// @param aErrCode- contains error code
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsErrorL(TInt aErrorCode)
    {
    IRLOG_DEBUG( "CIRIsdsWrapper::IsdsErrorL started." );
    IRRDEBUG2("CIRISDSWRAPPER::ISDSERRORL",KNullDesC);
    DestroyWaitDialogL();
    
    
    iReqAddToFav = EFalse;

    
	if( aErrorCode == KNotFound)
		{
		IRRDEBUG2("CIRISDSWRAPPER::ISDSERRORL--NOTFOUND",KNullDesC);
		if(iViewObserver)
			{
			if(iSSearchRequest)	
				{
				iError = ETrue;
				iViewObserver->ErrorL();
				iSSearchRequest = EFalse;		
				}
			else 
				{
				if(iHistoryBool)
					{
					iKNotFound = ETrue ;
					iViewObserver->ErrorL();
					iHistoryBool = EFalse ;
					iKNotFound = EFalse ;
					}
				else if(iSyncReqFromMainView)
					{
					iSyncReqFromMainView = EFalse;
					iViewObserver->ErrorL();						
					}
				else
					{
					iViewObserver->ErrorL();
					iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ERROR_NOTINISDS,ETrue);	
					}
				}
			}
		}
		
	else if( aErrorCode == KNotModified )
		{
		IRRDEBUG2("CIRISDSWRAPPER::ISDSERRORL--NOTMODIFIED",KNullDesC);
		iViewObserver->ErrorL();
		//iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ERROR_NOTINISDS,ETrue)
		}
	else if	( aErrorCode == KErrCouldNotConnect )
        {
        //VerifyNetworkConnectivityL()
        //iDiaologLauncher->ShowErrorNoteL(R_IRAPP_NW_NOCONN_ERROR,ETrue)
        }
	else if	( aErrorCode == KDndTimedOut  )
        {
  		IRRDEBUG2("IsdsErrorL :DndTimedOut ",KNullDesC);
  		if(iViewObserver)
			{
			iViewObserver->ErrorL();
       		iDiaologLauncher->ShowErrorNoteL(R_IRAPP_NW_NOCONN_ERROR,ETrue);
			}
        }
    else if ( aErrorCode == KServiceUnavailable )
	    {
		if(iSyncReqFromMainView)
			{
			iSyncReqFromMainView = EFalse;
			}
		if( iViewObserver )
			{
			iViewObserver->ErrorL();
			iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ERROR_ISDS_UNAVAILABLE,ETrue);	
			}
	    }
    else if ( aErrorCode == KErrCorrupt )
        {
        IRRDEBUG2("CIRISDSWRAPPER::ISDSERRORL--KERRCORRUPT",KNullDesC);
        iDiaologLauncher->ShowErrorNoteL( R_IRAPP_ERROR_ADDRESS,ETrue );
        }
    else if (aErrorCode == KDataProviderTimeout)
		{
		IRRDEBUG2("CIRISDSWRAPPER::ISDSERRORL--KDATAPROVIDERTIMEOUT",KNullDesC);
		if(iViewObserver)
			{
			iConnTimeOut= ETrue;
			iViewObserver->ErrorL();	

			iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ERROR_CONNECTION,ETrue);
			}
		}
		
	else
		{
		IRRDEBUG2("CIRISDSWRAPPER::ISDSERRORL--LASTELSE aErrorCode =%d",aErrorCode);
		if(iViewObserver)
			{
			iViewObserver->ErrorL();
			iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ERROR_OPERATION,ETrue);
			}
		}
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsErrorL exiting" );
    }
    
// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsCatogoryDataReceivedL()
// Called when catogory information is recieved from ISDS
// @param aParsedStructure - contains catogory info
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::IsdsCatogoryDataReceivedL(CArrayPtrFlat<CIRBrowseCatagoryItems> 
											   & aParsedStructure) 
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCatogoryDataReceivedL started." );
	iError = EFalse;
	iConnTimeOut = EFalse;
	iCatBannerTag = EFalse;
	DestroyWaitDialogL();

	if (iCategoryData)
		{
		iCategoryData->Reset();
		delete iCategoryData;
		iCategoryData = NULL;
		}
	if(iCatBannerUrl) 
		delete iCatBannerUrl;
	
	iCatBannerUrl = NULL;
	
	if (iCatClickThroughUrl)
		delete iCatClickThroughUrl;
	
	iCatClickThroughUrl = NULL;
	//Set the catogory array with categoryname and description
	iCategoryData = new (ELeave) CDesCArrayFlat(KCategoryDataMaxLength);
	for(TInt i=0;i<aParsedStructure.Count();i++)
		{
		_LIT(KTabSpace,"\t");
			if(aParsedStructure[i]->iCatBannerUrl )		
				{
				iCatBannerTag = ETrue;
				if(aParsedStructure[i]->iCatBannerUrl->Length() !=0)
					{
					iCatBannerUrl = HBufC::NewL(aParsedStructure[i]->
					             iCatBannerUrl->Des().Length());
					iCatBannerUrl->Des().Append(aParsedStructure[i]->
					                        iCatBannerUrl->Des()); 
					}
				        
				if(aParsedStructure[i]->iCatClickThroughUrl)        
					{
					if(aParsedStructure[i]->iCatClickThroughUrl->Length() != 0)	
						{
						iCatClickThroughUrl = HBufC::NewL(aParsedStructure[i]->
						                   iCatClickThroughUrl->Des().Length());
						iCatClickThroughUrl->Des().Append(aParsedStructure[i]->
												iCatClickThroughUrl->Des());
						}
					}
							
				} 
			if(aParsedStructure[i]->iCatName)
				{
				HBufC* textResource = HBufC::NewLC(aParsedStructure[i]->iCatName->Des().Length() + KBufSize);
				textResource->Des().Append(KTabSpace);
				textResource->Des().Append(aParsedStructure[i]->iCatName->Des());
				textResource->Des().Append(KTabSpace);

				HBufC* expandedText = StringLoader::LoadLC(R_IRAPP_GENRE_STATLIST_TITLE_EXPAND,
														   aParsedStructure[i]->iSize);
				textResource->Des().Append(*expandedText);												
				iCategoryData->AppendL(*textResource);
				CleanupStack::PopAndDestroy(expandedText);
				CleanupStack::PopAndDestroy(textResource);
				}
		}
		
	
	iViewObserver->ResponseL();
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCatogoryDataReceivedL exiting" );			
	}
// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsChannelDataReceivedL()
// Called when channel information is recieved from ISDS
// @param aParsedStructure - contains Channel info
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::IsdsChannelDataReceivedL(CArrayPtrFlat<CIRBrowseChannelItems> 
											  & aParsedStructure)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsChannelDataReceivedL started." );
	iError = EFalse;
	iConnTimeOut = EFalse;
	iChannelBannerTag = EFalse;
	iStationsData->Reset();
	iChannelImgUrl->Reset();
    DestroyWaitDialogL();
	iChannelIdArray.Reset();

	if(iBannerUrl) 
	delete iBannerUrl;
	iBannerUrl = NULL;

	if (iClickThroughUrl)
	delete iClickThroughUrl;
	iClickThroughUrl = NULL;
	
	for(TInt i=0;i<aParsedStructure.Count();i++)
		{
		_LIT(KTabSpace,"\t");
		if(aParsedStructure[i]->iBannerUrl )		
			{
			iChannelBannerTag = ETrue;
			if(aParsedStructure[i]->iBannerUrl->Length() != 0)
				{
				iBannerUrl = HBufC::NewL(aParsedStructure[i]->
									iBannerUrl->Des().Length());
				iBannerUrl->Des().Append(aParsedStructure[i]->
											iBannerUrl->Des()); 
				}

			if(aParsedStructure[i]->iClickThroughUrl)
				{
				if(aParsedStructure[i]->iClickThroughUrl->Length() != 0)
					{
					iClickThroughUrl = HBufC::NewL(aParsedStructure[i]->
					iClickThroughUrl->Des().Length());
					iClickThroughUrl->Des().Append(aParsedStructure[i]->
					iClickThroughUrl->Des());
					}
				}
			} 
		
		if(aParsedStructure[i]->iChannelName)
			{
			TInt len = aParsedStructure[i]->iChannelName->Des().Length();
			if(aParsedStructure[i]->iShortDescription)
				{
				len = len + aParsedStructure[i]->iShortDescription->Des().Length();
				}
			HBufC* textResource = HBufC::NewLC(len + KSize);
			textResource->Des().Append(KTabSpace);
			textResource->Des().Append(aParsedStructure[i]->iChannelName->Des());
			textResource->Des().Append(KTabSpace);
			if(aParsedStructure[i]->iShortDescription)
			textResource->Des().Append(aParsedStructure[i]->iShortDescription->Des());
			iStationsData->AppendL(*textResource);
			CleanupStack::PopAndDestroy(textResource);

			if(aParsedStructure[i]->iImgUrl.Length())
				{
				iChannelImgUrl->AppendL(aParsedStructure[i]->iImgUrl);
				}
			else
				{
				RBuf urlStatus;
				urlStatus.Create(KSize);
				urlStatus.Copy(KFalse);
				iChannelImgUrl->AppendL(urlStatus);
				urlStatus.Close();
				}

			//the current channel list that is being displayed
			//in the stations view
			//this array contains all that channel ids 
			iChannelIdArray.Append(aParsedStructure[i]->iChannelID);
			}

		}
	iViewObserver->ResponseL();	
	
	if(iSSearchRequest)	
		{
		iSSearchRequest = EFalse;
		}			
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsChannelDataReceivedL exiting" );
	}
// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsPresetDataReceivedL()
// Called when preset information is recieved from ISDS
// @param aParsedStructure - contains Preset info
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsPresetDataReceivedL(CArrayPtrFlat<CIRIsdsPreset> & aParsedStructure)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetDataReceivedL started." );
	iError = EFalse;
	iConnTimeOut = EFalse;
	iListenRequest=EFalse;
	CIRIsdsPreset* data = aParsedStructure[0];
	data->SetChannelType(1);
	DestroyWaitDialogL();
	if(iReqAddToFav)
		{
		TInt err;
						
		iFavPresets.AddPresetL(*data,err,iReqForLogo);
		
		iViewObserver->ResponseL(data);
		iReqAddToFav=EFalse;
		
		
		//If favourites is full then display an error message
		if(err==KErrNoMemory)
			{
			iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
			return;			
			}
			
		else
			{
			if((err==KErrAlreadyExists )&&
			 ( iFavPresets.iFavPresetList.Count() == KPresetsArrayMaxLength) )
				{
				iDiaologLauncher->ShowErrorNoteL(R_IRAPP_ADDFAVOURITES_ERROR,ETrue);
				return;			
				}
			else
				{
				RBuf channel;
				HBufC* textResource = StringLoader::LoadLC( R_IRAPP_STATIONS_POPUP_SAVED,data->GetName() );//CurrentItemIndex()-1
				channel.Create(*textResource);
				iDiaologLauncher->ShowConfirmationNoteL( channel,ETrue );
				CleanupStack::PopAndDestroy(textResource);
				channel.Close();
				iIRSettings.SetFlagIfAnyFavL();
				return;
				}
			}
		}
		
	iViewObserver->PresetResponseL(data);
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetDataReceivedL exiting" );
	}
// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsIRIDRecieved()
// Called to provide IRID
// @param aIRID  - contains IRID recived from Isds
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsIRIDRecieved(const TDesC& aIRID)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsIRIDRecieved" );
	TRAP_IGNORE( iIRSettings.SetIRIDL(aIRID) )
	}
// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsOtaInfoRecieved()
// called back when OTA info is recieved and Ui needs to be intimated
// @param CIROTAUpdate&,the ota information 
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsOtaInfoRecieved( CIROTAUpdate &/*aOtaData*/)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsOtaInfoRecieved" );	
	}

// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsPresetRemovedL()
// called back when the preset that is synced was removed from isds
// @param TInt,the preset id
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsPresetRemovedL(TInt aId)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetRemovedL - Entering" );
	iFavPresets.MakePresetUserDefinedL(aId,0);
	iSyncPresetObserver->SyncPresetRemovedL(aId);
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetRemovedL - Exiting" );
	}

// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsPresetChangedL()
// called back when the preset that is synced has been changed in the isds
// @param CIRIsdsPreset&,the new preset data
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsPresetChangedL(CIRIsdsPreset& aPreset)
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetChangedL - Entering" );
	iFavPresets.ReplacePresetL(aPreset);
	iSyncPresetObserver->SyncPresetChangedL(aPreset);
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetChangedL - Exiting" );		
	}
	
// ---------------------------------------------------------------------------
// MIsdsResponseObserver::IsdsPresetNoChangeL()
// called back when the preset that is synced has not been changed
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsPresetNoChangeL()
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetNoChangeL - Entering" );
	iSyncPresetObserver->SyncPresetNoChangeL();
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetNoChangeL - Exiting" );
	}
	  
	  
// ---------------------------------------------------------------------------
// IsdsPresetRequestL()
// called by all the views to get preset data
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::IsdsPresetRequestL(MViewsResponseAndErrorObserver* aViewObserver,TInt aIndex,
										TInt aCommand,TInt aReqForLogo,TInt aHistoryBool)
{
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetRequestL started." );
	iViewObserver = aViewObserver;	
	iListenRequest=EFalse;
	CreateWaitDialogL();

	if(EAddtoFavouritesCmd == aCommand )
	iReqAddToFav=ETrue;
	iHistoryBool = aHistoryBool ;
	iReqForLogo = aReqForLogo;
	if(aHistoryBool)
		{
		iISDSClient->IRIsdsClientIntefaceL(aIndex,CIRIsdsClient::EChannels,ETrue);
		}
	else
		{
		iISDSClient->IRIsdsClientIntefaceL(aIndex,CIRIsdsClient::EChannels);
		}
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsPresetRequestL exiting" );
}

// ---------------------------------------------------------------------------
// IsdsCancelRequest()
// called by all the views to cancel the UI request
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::IsdsCancelRequest()
{	
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCancelRequest started." );
	iISDSClient->IRISDSCancelRequest();
    IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCancelRequest exiting." );
}


// ---------------------------------------------------------------------------
// IsdsCancelRequest()
// called by all the views to cancel the UI request
// ---------------------------------------------------------------------------
//

CDesCArrayFlat* CIRIsdsWrapper::GetStationsData()
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::GetStationsData" );
	return iStationsData;
	}

// ---------------------------------------------------------------------------
// IsdsCancelRequest()
// called by all the views to cancel the UI request
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::CreateWaitDialogL()
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::CreateWaitDialogL - Entering" );
	iDialogDismissed=EFalse;
	RBuf title;
	HBufC* titleText;
	if(iWaitDialogWhileSearch)
		{
	 	titleText = StringLoader::LoadLC(R_IRAPP_SEARCHING_PROGRESS);
		}
 	else if(iListenRequest)
		{
		titleText = StringLoader::LoadLC(R_IRAPP_LOADING_BUFFERING);
		}
	else 
		{
	 	titleText = StringLoader::LoadLC(R_IRAPP_LOADING_LOADING);
		}
	title.CreateL(*titleText );
	CleanupStack::PopAndDestroy( titleText );
	
	if(!iIsdsWaitDialog)	
		{
		iIsdsWaitDialog =  new ( ELeave ) CAknWaitDialog( NULL,ETrue);
		iIsdsWaitDialog->SetCallback(this);
		iIsdsWaitDialog->SetTextL(title);
		iIsdsWaitDialog->ExecuteLD(R_IRAPP_PROGRESS_DIALOG);
		}
	title.Close();
	IRLOG_DEBUG( "CIRIsdsWrapper::IsdsCancelRequest - exiting." );
	}
// -----------------------------------------------------------------------------
// CIRIsdsWrapper::DialogDismissedL()
// Called by CAknwaitdialog 
// -----------------------------------------------------------------------------
// 
void CIRIsdsWrapper::DialogDismissedL( TInt aButtonId )
	{
	IRLOG_DEBUG( "CIRIsdsWrapper::DialogDismissedL - Entering" );
	if(aButtonId == EIRCancel)
		{
		iIsdsWaitDialog= NULL;
		iDialogDismissed=ETrue;
		iReqAddToFav=EFalse;
		iViewObserver->ErrorL();
		iDialogDismissed=EFalse;
		IsdsCancelRequest();
		}
	IRLOG_DEBUG( "CIRIsdsWrapper::DialogDismissedL - Exiting" );
	}    
// -----------------------------------------------------------------------------
// CIRIsdsWrapper::GetBannerUrl()
// Returns the BannerUrl
// -----------------------------------------------------------------------------
// 
HBufC* CIRIsdsWrapper::GetBannerUrl()
{	
IRLOG_DEBUG( "CIRIsdsWrapper::GetBannerUrl - Entering" );
IRLOG_DEBUG( "CIRIsdsWrapper::GetBannerUrl - Exiting" );
	if(iBannerUrl)
	{
		return iBannerUrl;
	}
	else
	{
		return NULL;
	}
}

// ---------------------------------------------------------------------------
// DestroyWaitDialogL()
// Destroys The WaitDialog
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::DestroyWaitDialogL()
	{
	
	IRLOG_DEBUG( "CIRIsdsWrapper::DestroyWaitDialogL - Entering" );

		if(iIsdsWaitDialog)
		{
		iIsdsWaitDialog->ProcessFinishedL();
		iIsdsWaitDialog=NULL;
		}
	
	IRLOG_DEBUG( "CIRIsdsWrapper::DestroyWaitDialogL - exiting." );
	}
	
// ---------------------------------------------------------------------------
// SetCurrentPlayingIndex()
// Sets the Current Playing Index
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::SetCurrentPlayingIndex(TBool aCurrentPlayingIndex  )
	{
	
	IRLOG_DEBUG( "CIRIsdsWrapper::SetCurrentPlayingIndex - Entering" );
	iCurrentPlayingIndex=aCurrentPlayingIndex;
	IRLOG_DEBUG( "CIRIsdsWrapper::SetCurrentPlayingIndex - exiting" );
	}
// ---------------------------------------------------------------------------
// GetCurrentPlayingIndex()
// Returns The current Playing Index
// ---------------------------------------------------------------------------
//

TBool CIRIsdsWrapper::GetCurrentPlayingIndex( )
	{
	
	IRLOG_DEBUG( "CIRIsdsWrapper::GetCurrentPlayingIndex - Entering" );
	IRLOG_DEBUG( "CIRIsdsWrapper::GetCurrentPlayingIndex - exiting" );
	return iCurrentPlayingIndex;
	}


// ---------------------------------------------------------------------------
// SetListenRequest()
// Sets The ListenRequest if Listen is Done
// ---------------------------------------------------------------------------
//

void CIRIsdsWrapper::SetListenRequest(TBool aListenRequest)
	{
	
	IRLOG_DEBUG( "CIRIsdsWrapper::SetListenRequest - Entering" );
	iListenRequest=aListenRequest;
	IRLOG_DEBUG( "CIRIsdsWrapper::SetListenRequest - exiting" );
	}

// ---------------------------------------------------------------------------
// GetListenRequest()
// Returns the Listen Request Value
// ---------------------------------------------------------------------------
//

TBool CIRIsdsWrapper::GetListenRequest( )
	{
	
	IRLOG_DEBUG( "CIRIsdsWrapper::GetListenRequest - Entering" );
	IRLOG_DEBUG( "CIRIsdsWrapper::GetListenRequest - exiting" );
	return iListenRequest;
	}
// ---------------------------------------------------------------------------
// ActiveWaitDialog( )
// Returns Whether the Isds WaitDialog is Active or Not
// ---------------------------------------------------------------------------
//

TBool CIRIsdsWrapper::ActiveWaitDialog( )
	{
	
	IRLOG_DEBUG( "CIRIsdsWrapper::ActiveWaitDialog- Entering" );
	IRLOG_DEBUG( "CIRIsdsWrapper::ActiveWaitDialog- exiting" );
	if(iIsdsWaitDialog)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// ---------------------------------------------------------------------------
// CIRIsdsWrapper::SetSyncPresetObserver()
// Notifies the sync preset call back to the observer
// @param MSyncPresetObserver, observer for the preset synchronization
// ---------------------------------------------------------------------------
//
void CIRIsdsWrapper::SetSyncPresetObserver(MSyncPresetObserver* aSyncPresetObserver)
	{
	IRLOG_DEBUG("CIRIsdsWrapper::SetSyncPresetObserver - Entering");
	iSyncPresetObserver = aSyncPresetObserver;
	IRLOG_DEBUG("CIRIsdsWrapper::SetSyncPresetObserver -- Exiting");
	}
	

void CIRIsdsWrapper::SetSyncPresetObserver(MSyncPresetObserver* aSyncPresetObserver,
									MViewsResponseAndErrorObserver* aViewObserver)
	{
	IRLOG_DEBUG("CIRIsdsWrapper::SetSyncPresetObserver 2 - Entering");
	iViewObserver = aViewObserver;
	SetSyncPresetObserver(aSyncPresetObserver);
	IRLOG_DEBUG("CIRIsdsWrapper::SetSyncPresetObserver 2 - Exiting");
	}
	

// ---------------------------------------------------------------------------
// CIRIsdsWrapper::GetErrorCode()
// Getting the errorcode
// ---------------------------------------------------------------------------
//
TInt CIRIsdsWrapper::GetErrorCode()
	{
	return iKNotFound ;	
	}
	
// ------------------------------------------------------------------------------
// CIRIsdsWrapper::SetSyncReqFromMainview()
// called before the syncrequest is made form MAINVIEW
// once the reponse receives saying "preset removed",
// error note "item no longer available" should not be displayed
// @param aValue, to know whether the syncrequest is made form mainview or not
// ------------------------------------------------------------------------------
void CIRIsdsWrapper::SetSyncReqFromMainView(TBool aValue)
	{
	iSyncReqFromMainView = aValue;		
	}


	
