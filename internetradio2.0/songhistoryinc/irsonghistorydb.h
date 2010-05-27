/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Song History database used for stroing song history entries.
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

#ifndef CIRSONGHISTORYDB_H
#define CIRSONGHISTORYDB_H

#include <d32dbms.h>
#include "irsettings.h"

class CIRSongHistoryInfo;
class CIRSettings;
 

// Maximum no of entries to be stored in the history list.
const TInt KMaxNoChannelHistoryDbEntries = 30;
const TInt KMaxNoSongHistoryDbEntries = 100;

// Maximum length of Dbms field string
const TInt KIRDbMaxStrLen = 255;

const TInt KIRDbMaxChType = 2;



NONSHARABLE_CLASS(CIRSongHistoryDb) : public RDbNamedDatabase
	{
//member functions
public:

	/**
	* Function : NewL()
	* Two phased construction
	* @return instance of CIRSongHistoryDb
	*/
		static CIRSongHistoryDb* NewL();

	/**
	* Function : NewLC()
	* Two phased construction
	* @return instance of CIRSongHistoryDb
	*/
		static CIRSongHistoryDb* NewLC();

	/**
	* Function : ~CIRSongHistoryDb()
	* default destructor
	*/
		~CIRSongHistoryDb();



    /**
    * Function : AddToSongHistoryDbL()
    * adds the song history entry into data base
    * @param various channel information 
    */
    TBool AddToSongHistoryDbL( const TDesC& aSongName,
                                const TDesC& aArtistName, 
                                const TDesC& aChannelName, 
                                const TDesC& aChannelUrl,
                                RBuf& aDelSongName, 
                                RBuf& aDelArtistname, 
                                RBuf& aDelChannelName, 
                                RBuf& aDelChannelUrl,
                                TInt  aChannelType,
                                TInt aChannelId,
                                TInt aBitrate,
                                const TDesC& aChannelDesc,
                                const TDesC& aImageUrl,
                                const TDesC& aGenreName,
                                const TDesC& aCountryName,
                                const TDesC& aLanguageName,
                                const TDesC& aMusicFlag);
	 /**
	 * Function : AddToSongHistoryDb2L()
	 * adds the song history entry into data base
	 * @param various song information 
	 */
	 TBool AddToSongHistoryDb2L( const TDesC& aSongName,
	                             const TDesC& aArtistName, 	                                
	                             const TDesC& aMusicFlag);


	/**
	* CIRSongHistoryDb::GetAllSongHistoryListL()
	* gets all the song history entries  into an array
	* @param aSongHistoryEntries is the array which is to be filled by the SongHistoryDb. 
	*/

		void GetAllSongHistoryListL(RPointerArray<CIRSongHistoryInfo>& aSongHistoryEntries);
    /**
	* CIRSongHistoryDb::GetAllSongHistoryList2L()
	* gets all the song history entries  into an array
	* @param aSongEntries is the array which is to be filled by the SongHistoryDb. 
	*/
		void GetAllSongHistoryList2L(RPointerArray<CIRSongHistoryInfo>& aSongEntries);
	/**
	* Function : ClearAllSongHistoryFromDb
	* delete all the song entries from the database.
	*/

		TInt ClearAllSongHistoryFromDb();
		
	/**
	* Function : ClearAllSongHistoryFromDb2
	* delete all the song entries from the database.
	*/
		TInt ClearAllSongHistoryFromDb2();

	/**
	* Function : CountSongHistoryDb()
	* count the database entries
	*/

		TInt CountSongHistoryDb();
		
	/**
	* Function : CountSongHistoryDb2()
	* count the database entries
	*/
		TInt CountSongHistoryDb2();

	/**
	* Function : GetLastHistoryItemL()
	* Gives the last added entry in the database
	* @param Channel data to be filled .
	*/
	void GetLastHistoryItemL(RBuf& aSongName, RBuf& aArtistName,
					 RBuf& aChannelName, RBuf& aChannelUrl);

	/**
	* Function : GetChannelSongsCount()
	* Returns SongsCount for given channel
	*/
	TUint GetChannelSongsCountL( const RBuf& aChannelName, const RBuf& aChannelUrl );
	
	/**
	* Function : GetChannelSongsCount()
	* Updates the song history DB when there is a channel removed in the isds.
	*/
    void SyncSongHistoryDbL(TInt aChannelId) ;
    
	/**
	* Function : GetIdPresentInDb()
	* Gets whether the particular channel ID is present in history DB or not. 
	*/
    TBool GetIdPresentInDbL(TInt aChannelId) ;
    
	/**
	* Function : UpdateSongHistoryDb()
	* Updates the song history DB when there is a channel change in the isds.
	*/
    TInt UpdateSongHistoryDbL( TInt aChannelId,
    	                      const TDesC& aChannelName, 
						      const TDesC& aChannelUrl,
						      const TDesC& aImageUrl,
                              const TDesC& aGenreName,
                              const TDesC& aCountryName,
                              const TDesC& aLanguageName,						      
							  const TDesC& aMusicFlag);

    /*
     * search if a record already exists in database. If yes, delete it.
     */
    void SearchAndDeleteRecordL(const TDesC& aChannelName, const TDesC& aChannelUrl,
                                TInt aChannelType);
    
    /*
     * delete a recorder by the index, for station history database
     * */
    TInt DeleteOneHistory(TInt aIndex);
    
    /*
     * delete a recorder by the index, for songhistory database
     * */
    TInt DeleteOneSongHistory(TInt aIndex);

private:

	/**
	* Function : ConstructL()
	* two phase construction
	*/
    void ConstructL();

    /**
	* Function : CloseSongHistoryDb()
	* Closes the database
	*/
	void CloseSongHistoryDb();

	/**
	* Function : CreateSongHistoryTablesL()
	* creates songhistory table with three columns
	* log data
	* ---------------------------------------------------------------------------
	* SongHistoryTable
	*---------------------------
	*| SongName | ArtistName | ChannelName | ChannelUrl | ChannelType | ChannelId    | Bitrate      | ChannelDesc | ImageUrl   | GenreName  | CountryName | LanguageName | MusicStatus
	*---------------------------
	*|EDbColText| EDbColText | EDbColText  | EDbColText | EDbColUint8 | EDbColUint16 | EDbColUint16 | EDbColText  | EDbColText | EDbColText | EDbColText  | EDbColText   | EDbColText 
	*----------------------------------------------------------------------------
	*/
	void CreateSongHistoryTablesL();
	
    /**
	* Function : CreateSongHistoryTables2L()
	* creates songhistory table with three columns
	* log data
	* ---------------------------------------------------------------------------
	* SongHistoryTable
	*---------------------------
	*| SongName | ArtistName | MusicStatusFlag
	*---------------------------
	*|EDbColText| EDbColText | EDbColText |
	*----------------------------------------------------------------------------
	*/
	void CreateSongHistoryTables2L();

   /**
	* Function : CreateDbCondition()
	* Creates  the dbms file conditionally(only if not yet created)
	* @param aSongHistoryDbFile the Database file name
	*/
	void CreateDbCondition(const TFileName& aSongHistoryDbFile);


	/**
	* Function : OpenSongHistoryDbL()
	* @param aSongHistoryDbFile the Database file name
	* opening the data base
	*/
	void OpenSongHistoryDbL(const TFileName& aSongHistoryDbFile);

	/**
	* Function : CreateSongHistoryDb()
	* creates the dbms files
	* @param aSongHistoryDbFile database filename
	*/
	TInt CreateSongHistoryDb(const TFileName& aSongHistoryDbFile);
	
	/**
	 * to wrapper the delete leaving
	 */
	TInt DeleteOneHistoryL(TInt aIndex);

    /**
     * to wrapper the delete leaving
     */
    TInt DeleteOneSongHistoryL(TInt aIndex);

	/**
	* Data-structure to hold unique channel info
	*/
	class CIRChannelInfo : public CBase
	{
	public:
		~CIRChannelInfo()
		{
			iChannelUrl.Close();
			iChannelName.Close();
		}
		RBuf iChannelUrl;
		RBuf iChannelName;
		TInt iChannelType;
	};
	


	/**
	* Comparer function to compare two CIRChannelInfo's
	*/
	static TBool CompareChannelInfos(const CIRChannelInfo& aFirst, const CIRChannelInfo& aSecond);
	 
//data members

private:

	//	File session

    RFs iFsSession;


     //	Table which stores History information

    RDbTable iSongHistoryTable;

    // only save the song info for the song history view.
	RDbTable iSongHistoryTable2;

    };

#endif  //CIRSONGHISTORYDB_H








