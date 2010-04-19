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





#ifndef IRISDSWRAPPER_H
#define IRISDSWRAPPER_H

#include <e32base.h>
#include <BADESCA.H>
#include "misdsresponseobserver.h"
#include "isdsclientdll.h"
#include <AknProgressDialog.h>

const TInt KPresetsArrayMaxLength = 50;


class CIRIsdsClient;
class CIRBrowseCatagoryItems;
class CIRBrowseChannelItems;
class CIRSettings;
class CIRFavoritesDb;
class CIRIsdsPreset;
class CIRDialogLauncher;
class CAknWaitDialog;

class MViewsResponseAndErrorObserver;
class MSyncPresetObserver;

class CIRIsdsWrapper:public CBase,public MIsdsResponseObserver,public MProgressDialogCallback
{

public:
	/* Function : NewL
	 * function to return an instance of CIRIsdsWrapper
	 * Two phase constructor
     */
 	static CIRIsdsWrapper* NewL(CIRSettings& aIRSettings,CIRFavoritesDb& aFavPresets);        
    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CIRIsdsWrapper object, which will draw itself
     * to aRect.
     */
    static CIRIsdsWrapper* NewLC(CIRSettings& aIRSettings,CIRFavoritesDb& aFavPresets);        
    /**
     * ~CIRIsdsWrapper
     * Destructor.
     */
    ~CIRIsdsWrapper();
 
   /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CIRSearchView object.
     */
    void ConstructL();
		
	/**
     * CIRSearchView.
     * C++ default constructor.
     */
    CIRIsdsWrapper(CIRSettings& aIRSettings,CIRFavoritesDb& aFavPresets);  
	/**
	* IsdsSearchRequestL
	* Issues a search request to the Isds Client
	*/
	void IsdsSearchRequestL(MViewsResponseAndErrorObserver* aViewObserver,
							const TDesC& aIsdsSearchString);

	/**
	* IsdsCategoryRequestL
	* Issues a category request to the Isds Client
	*/
	void IsdsCategoryRequestL(MViewsResponseAndErrorObserver* aViewObserver,
							CIRIsdsClient::TIRIsdsclientInterfaceIDs aIDType);

	/**
	* IsdsChannelRequestL
	* Issues a channel request to the Isds Client
	*/
	void IsdsChannelRequestL(MViewsResponseAndErrorObserver* aViewObserver,TInt aIndex);

	/**
	* IsdsListenRequestL
	* Issues a Listen request to the Isds Client
	*/
	void IsdsListenRequestL(MViewsResponseAndErrorObserver* aViewObserver,
					TInt aCurrentIndex,TBool aHistoryBool=EFalse);
	
    /** 
     * MIsdsResponseObserver::IsdsErrorL()
	 * Used to Indiacte error in retrieving data from Isds
	 * @param aErrCode- contains error code
	 */
    void IsdsErrorL(TInt aErrCode);
    
	/**
	 * MIsdsResponseObserver::IsdsCatogoryDataReceivedL()
	 * Called when catogory information is recieved from ISDS
	 * @param aParsedStructure - contains catogory info
	 */
	void IsdsCatogoryDataReceivedL(
		CArrayPtrFlat<CIRBrowseCatagoryItems> & aParsedStructure) ;
    
	/**
	 * MIsdsResponseObserver::IsdsChannelDataReceivedL()
	 * Called when channel information is recieved from ISDS
	 * @param aParsedStructure - contains Channel info
	 */
	void IsdsChannelDataReceivedL(
		CArrayPtrFlat<CIRBrowseChannelItems> & aParsedStructure);
    
	/**
	 * MIsdsResponseObserver::IsdsPresetDataReceivedL()
	 * Called when preset information is recieved from ISDS
	 * @param aParsedStructure - contains Preset info
	 */
	void IsdsPresetDataReceivedL(
		CArrayPtrFlat<CIRIsdsPreset> & aParsedStructure);
    
	/**
	 * MIsdsResponseObserver::IsdsIRIDRecieved()
	 * Called to provide IRID
	 * @param aIRID  - contains IRID recived from Isds
	 */
	void IsdsIRIDRecieved(const TDesC& /*aIRID*/);

	/**
	 * MIsdsResponseObserver::IsdsOtaInfoRecieved()
	 * called back when OTA info is recieved and Ui needs to be intimated
	 * @param CIROTAUpdate&,the ota information 
	 */
	void IsdsOtaInfoRecieved( CIROTAUpdate &/*aOtaData*/);

	/**
	 * MIsdsResponseObserver::IsdsPresetRemovedL()
	 * called back when the preset that is synced was removed from isds
	 * @param TInt,the preset id
	 */
	void IsdsPresetRemovedL(TInt /*aId*/);

	/**
	 * MIsdsResponseObserver::IsdsPresetChangedL()
	 * called back when the preset that is synced has been changed in the isds
	 * @param CIRIsdsPreset&,the new preset data
	 */
	void IsdsPresetChangedL(CIRIsdsPreset& /*aPreset*/);
	
	/**
	 * MIsdsResponseObserver::IsdsPresetNoChangeL()
	 * called back when the preset that is synced has not been changed
	 */
	 void IsdsPresetNoChangeL();	
	/**
	 * IsdsPresetRequestL()
	 * called by all the views to get preset data
	 */
	void IsdsPresetRequestL(MViewsResponseAndErrorObserver* aViewObserver,TInt aIndex,
				TInt aCommand,TInt aReqForLogo = ETrue,TInt aHistoryBool=EFalse);
	

	/**
	* CIRIsdsWrapper::GetISDSInstance()
	* Returns the ISDS Instance
	*/
	CIRIsdsClient* GetISDSInstance();
	
	/**
	* CIRIsdsWrapper::IsdsCancelRequest()
	* Cancels the Isds request made by UI
	*/
	void IsdsCancelRequest();
	
	/**
	* CIRIsdsWrapper::GetDialogLauncherInstance()
	* Returns the Dialog Launcher Instance
	*/
    CIRDialogLauncher* GetDialogLauncherInstance();
    
    /**
     * GetStationsData()
     * returns the instance of iStationsData
     */
    CDesCArrayFlat* GetStationsData();
    void CreateWaitDialogL();
    	/**
	* Called when the refresh dialog is dismissed.
	* @param aButtonId The button that was used to dismiss the dialog.
	*/
	void DialogDismissedL( TInt aButtonId );
	
	
	/**
	* CIRIsdsWrapper::GetBannerUrl()
	* Returns the BannerUrl
	*/
	HBufC* GetBannerUrl();

	/**
	* DestroyWaitDialogL()
	* Destroys the Wait Dialog
	*/
	void DestroyWaitDialogL();
	
	/**
	* CIRIsdsWrapper::SetCurrentPlayingIndex()
	* Sets the Current Playing Index
	*/
	void SetCurrentPlayingIndex(TBool aCurrentPlayingIndex);

	/**
	* CIRIsdsWrapper::GetCurrentPlayingIndex()
	* Returns The current Playing Index
	*/
	TBool GetCurrentPlayingIndex();

	/**
	* CIRIsdsWrapper::SetListenRequest()
	* Sets The ListenRequest if Listen is Done
	*/
	void  SetListenRequest(TBool aListenRequest);

	/**
	* CIRIsdsWrapper::GetListenRequest()
	* Returns the Listen Request Value
	*/
	TBool GetListenRequest();
	/**
	* CIRIsdsWrapper::ActiveWaitDialog();
	* Returns checks Whether tHe IsdS waitDialog is Active or Not
	*/
	
	TBool  ActiveWaitDialog();
	
	/**
	* CIRIsdsWrapper::SetSyncPresetObserver()
	* Notifies the sync preset call back to the observer
	* @param MSyncPresetObserver, observer for the synchronization
	*/
	void SetSyncPresetObserver(MSyncPresetObserver* aSyncPresetObserver);
	
	void SetSyncPresetObserver(MSyncPresetObserver* aSyncPresetObserver,
					MViewsResponseAndErrorObserver* aViewObserver);
	
	
    TInt GetErrorCode();
    
    /**
    * CIRIsdsWrapper::SetSyncReqFromMainView()
    * called before the syncrequest is made form MAINVIEW
    * once the reponse receives saying "preset removed",
    * error note "item no longer available" should not be displayed
	* @param aValue, to know whether the syncrequest is made form mainview or not
	*/
	void SetSyncReqFromMainView(TBool aValue);
	
public: 	//data

	/**
	* iStationsData
	* Data for stationa data
	*/
	CDesCArrayFlat* iStationsData;
	/**
	* iCategoryData
	* Data for category data
	*/
	CDesCArrayFlat* iCategoryData;
	
	/**
	* iCategoryData
	* Array which contains the data for imgUrl of each channel
	*/
	CDesCArrayFlat* iChannelImgUrl;
	
	
	/**
	* iChannelID
	* stores the channel id of currently playing channel
	*/
	TInt iChannelID;
	
	/**
	* iChannelIdArray
	* for station id comparision
	*/
	RArray<TInt> iChannelIdArray;
	
	
	/**
	* iIRSettings
	* Instance of CurrentOperationSettings
	*/
	CIRSettings& iIRSettings;
	
	/**
	* iISDSClient
	* ISDS INTERFACE
	*/
	CIRIsdsClient* iISDSClient;
	
	/**
	* iViewObserver
	* data of MViewsResponseAndErrorObserver class
	*/
	MViewsResponseAndErrorObserver *iViewObserver;	
	
	/**
	* iFavPresets
	* Instance of favoritesDb class
	*/
	CIRFavoritesDb& iFavPresets;
	
	/**
	* iReqAddToFav
	* Boolean value to determine whether add to favorites is requested
	*/
	TBool iReqAddToFav;
	
	/**
	* iReqForLogo
	* Boolean value to determine whether logo is requested
	*/
	TBool iReqForLogo;
	
	/**
	* iSSearchRequest;
	* Checks if the Isds request is for search
	*/
	TBool iSSearchRequest;
	
	/**
	* iError
	* Checks if there is any Isds error.
	*/
	TBool iError;
	TBool iConnTimeOut;
	TBool iDialogDismissed;
	
	/**
	* iDiaologLauncher
	* instance of CIRDiaologLauncher
	*/
	CIRDialogLauncher* iDiaologLauncher;
	
	 
private:
	 /**
	  * iBannerUrl
	  * Banner Url of the Advertisement for Channels
	  */
	  HBufC* iBannerUrl;
	  
	  /**
	  * iWaitDialogWhileSearch
	  * Set When request For Search
	  */
	  TBool iWaitDialogWhileSearch;
	 
	 /**
	 *iCurrentPlayingIndex
	 * index of currently playing item
	 */
	 TInt iCurrentPlayingIndex;

	  /**
	  * iListenRequest
	  * Set When request For Listen
	  */
	  TBool iListenRequest;
	  
	  TBool iHistoryBool ;
	  /**
	  * iSyncPresetObserver
	  * observer for the prest synchronization
	  */
	  MSyncPresetObserver* iSyncPresetObserver;
	  TBool iKNotFound;
	 
public:
	  
	  /**
	  * iCliokThroughUrl
	  * ClickThroughUrl of the Advertisement for Channels
	  */
	  HBufC* iClickThroughUrl;
	 /**
	  * iBannerUrl
	  * Banner Url of the Advertisement for Channels
	  */
	  HBufC* iCatBannerUrl;
	  
	  /**
	  * iCliokThroughUrl
	  * ClickThroughUrl of the Advertisement for Channels
	  */
	  HBufC* iCatClickThroughUrl;
    /**
	* iWaitDialog
	* calls the waitdialog owned by
	* CAknWaitDialog
	*/   
	CAknWaitDialog* iIsdsWaitDialog;
	TBool iCatBannerTag;
	TBool iChannelBannerTag;
	
	/**
	* iSyncReqFromMainView
	* used to know whether the sync request is made from mainview or not
	*/
	TBool iSyncReqFromMainView;

	 
};

#endif //IRISDSWRAPPER_H