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
* Description:  Engine class to notify channel name/metadata changes to SongHistoryDb and UI
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
*  Version: 3, Thu Apr 14 12:00:00 2008 by Rohit
*  Ref:
*  Pc lint fixes
*
*  Version: 2, Thu Apr 10 20:00:00 2008 by Rohit
*  Ref:
*  Implemented SongHistory changes for channelwise songs
*
*  </ccm_history>
* ============================================================================
*/


#ifndef CIRSONGHISTORYENGINE_H
#define CIRSONGHISTORYENGINE_H

#include "irsonghistoryengpubsubnotifyhandler.h"
#include "irsonghistorydb.h"


class CIRSongHistoryEngPubSub;
class MSongHistoryUINotifyHandler;
class CIRMetaData;
class CIRSongHistoryInfo;
class RSongHistoryDb;
class MIRSongHistoryEngPubSubNotifyHandler;


enum TSongHistoryItemChange
{
	ENOCHANGE = 0,
	ECHANNELCHANGED,
	EMETADATACHANGED,
	EBLANKENTRIES
};

/**
* Engine component for Internet Radio SongHistory.
* This class is responsible for listening to relevant P&S keys and notifying the UI code whenever the listened keys change.
*/
NONSHARABLE_CLASS(CIRSongHistoryEngine) : public CBase,public MIRSongHistoryEngPubSubNotifyHandler
    {
    public:

    /**
    * Static constructor.
    * @param    aUiObserver   The observer to be notified of the changes in the keys.
    * @return   The newly created CIRSongHistoryEngine object.
    */
    	IMPORT_C static CIRSongHistoryEngine* NewL( MSongHistoryUINotifyHandler& aUiObserver );

    /**
    * Static constructor.
    * @param    aUiObserver   The observer to be notified of the changes in the keys.
    * @return   The newly created CIRSongHistoryEngine object.
    */

    	IMPORT_C static CIRSongHistoryEngine* NewLC( MSongHistoryUINotifyHandler& aUiObserver );

    /**
    * Destructor.
    */
 	   IMPORT_C ~CIRSongHistoryEngine();

    /**
    * Construction finalizer.
    * Must be called after CIRSongHistoryEngine::NewL.
    */
	   void ConstructL();


	//APIs to serve UI request

    /**
    * Invoked when UI request.
    * @param    aArray<CIRSongHistoryInfo>
    */
    	IMPORT_C void GetAllHistoryL( RPointerArray<CIRSongHistoryInfo>& aSongHistoryInfo );

    /**
    * Invoked when UI request.
    */
    	IMPORT_C TInt ClearAllHistory( );

    /**
    * Invoked when UI request.
    */
    	IMPORT_C TInt GetHistoryCount( );
    	
    /**
    * Called by the UI when a particular channel has been removed from the ISDS.
    */
        IMPORT_C TInt SyncHistory(TInt aChannelId);
        
    /**
    * Called by the UI when a particular channel has been changed in the ISDS.
    */
        
        IMPORT_C void UpdateSongHistoryDb( TInt aChannelId,
    	                          const TDesC& aChannelName, 
						          const TDesC& aChannelUrl,
							      const TDesC& aMusicFlag);
        
        /* called by the UI when delete a particular channel
         * */
        IMPORT_C TInt DeleteOneHistory(TInt aIndex);


  public:
  //this function made public for our internal testing. Has to be private without IMPORT_C
		/**
		* Invoked when the listened key is changed.
		* @param    aCategory   The category of the key that changed.
		* @param    aKey        The key that changed.
		* @param    aText       The new value of the key.
		*/

			IMPORT_C void HandleMetaDataReceivedL( const CIRMetaData& aMetaData );

	private:

		/**
		* Invoked when the listened key is changed.
		* @param    aCategory   The category of the key that changed.
		* @param    aKey        The key that changed.
		* @param    aText       The new value of the key.
		* Single wrapper function to be implemented in the engine class instead of two.
		*/
			void HandlePropertyChangeL( const TUid& aCategory, TUint aKey,
											   const TDesC8& aText ) ;


		/**
		* Invoked when the listened key is changed.
		* @param    aCategory   The category of the key that changed.
		* @param    aKey        The key that changed.
		* @param    aError      The error value.
		*/
			void HandlePropertyChangeErrorL( const TUid& aCategory, TUint aKey,
										 TInt aError );

		/**
		 * Invoked when data needs to be added
		 * @param	aMetaData	MetaData that needs to be added
		 * @param	aChannelName Channel name that needs to be added
		 */
		TSongHistoryItemChange DetectSongHistoryItemChangesL(const CIRMetaData& aMetaData,
					 const RBuf& aChannelName, const RBuf& aChannelUrl);

	private:

		/**
		* Constructor.
		* @param    aObserver   The observer to be notified of the changes
		* in the keys.
		*/
			CIRSongHistoryEngine( MSongHistoryUINotifyHandler & aUiObserver );


    private:

		/** An instance of songhistory database. */
		CIRSongHistoryDb* iSongHistoryDb;


        /** The observer to be notified of the database changes to the UI */
		MSongHistoryUINotifyHandler & iUiObserver;

        /** CIRSongHistoryEngPubSub objects subscribing for propery change */
        CIRSongHistoryEngPubSub *iChannelNameSub;
        
        CIRSongHistoryEngPubSub *iMetaDataSub;

    };

#endif//CIRSONGHISTORYENGINE_H
