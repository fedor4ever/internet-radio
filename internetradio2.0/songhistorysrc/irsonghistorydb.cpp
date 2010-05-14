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
* Description:  Implementation of the songhistory database
*
*/


/* ---------------------------------------------------------------------------
*  Version history:
*  Template version:
*  <ccm_history>
*
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

#include <bautils.h>
#include <badesca.h>    // CDesCArrayFlat
#include <s32file.h>    // CFileStore & CPermanentFileStore
#include <bautils.h>    // file helpers
#include <eikenv.h>

#include "irsonghistorydb.h"
#include "irsonghistoryinfo.h"
#include "irdebug.h"
#include "irsettings.h"


 
_LIT( KSongHistoryDBTable,              "ChannelHistoryTable"  );
_LIT( KSongHistoryDBTable2,             "SongHistoryTable"  );
_LIT( KSongHistoryDBSongNameColumn,     "SongName"	   );
_LIT( KSongHistoryDBArtistNameColumn,   "ArtistName"   );
_LIT( KSongHistoryDBChannelNameColumn,  "ChannelName"  );
_LIT( KSongHistoryDBChannelUrlColumn,   "ChannelUrl"  );
_LIT( KSongHistoryDBChannelTypeColumn,   "ChannelType"  );
_LIT( KSongHistoryDBChannelIdColumn,   "ChannelId"  );
_LIT( KSongHistoryDBBitrateColumn,   "Bitrate"  );
_LIT( KSongHistoryDBChannelDescColumn,   "ChannelDescription"  );
_LIT( KSongHistoryDBImageUrlColumn,   "ImageUrl"  );
_LIT( KSongHistoryDBMusicStatusColumn,   "MusicStoreStatus"  );

_LIT(KSongHistoryDbFile,"songhistoryDb.db");
_LIT(KNo , "No") ;

const TInt KMaxSize = 255;

const TInt KMaxColumnLength = 255;
TBool CIRSongHistoryDb::CompareChannelInfos(const CIRChannelInfo& aFirst,
						 const CIRChannelInfo& aSecond)
{
   return ( aFirst.iChannelName == aSecond.iChannelName && 
   					aFirst.iChannelUrl == aSecond.iChannelUrl &&
   					aFirst.iChannelType == aSecond.iChannelType);
}

 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Function : NewL()
// two phased construction
// ---------------------------------------------------------------------------
//
CIRSongHistoryDb* CIRSongHistoryDb::NewL()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::NewL" );
	CIRSongHistoryDb* self=CIRSongHistoryDb::NewLC();
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRSongHistoryDb::NewL - Exiting." );
	return self;
}


// ---------------------------------------------------------------------------
// Function : NewLC()
// Two phased construction
// ---------------------------------------------------------------------------
//
CIRSongHistoryDb* CIRSongHistoryDb::NewLC()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::NewLC" );
	CIRSongHistoryDb *self=new(ELeave)CIRSongHistoryDb;
	CleanupStack::PushL(self);
	self->ConstructL();
	IRLOG_DEBUG( "CIRSongHistoryDb::NewLC - Exiting." );
	return self;
}


// ---------------------------------------------------------------------------
// CIRSongHistoryDb::~CIRSongHistoryDb()
// default destructor
// ---------------------------------------------------------------------------
//
CIRSongHistoryDb::~CIRSongHistoryDb()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::~CIRSongHistoryDb" );

	CloseSongHistoryDb();
	iFsSession.Close();
	IRLOG_DEBUG( "CIRSongHistoryDb::~CIRSongHistoryDb - Exiting." );
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::CreateDbCondition()
// Creates both the dbms files conditionally(only if not yet created)
// calls CreateSongHistoryDb()
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::CreateDbCondition(const TFileName& aSongHistoryDbFile)
{
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateDbCondition" );


	if( !BaflUtils::FileExists(iFsSession, aSongHistoryDbFile) )
	{
		TInt error = CreateSongHistoryDb(aSongHistoryDbFile);
		if( error )
		{
			IRLOG_DEBUG( "CIRSongHistoryDb::CreateDbCondition - Exiting (1)." );
			
		}
	}
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateDbCondition - Exiting (2)." );
	
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb:AddToSongHistoryDbL()
// adds the session log entry into data base
// Returns ETure if songHistory count exceeds 30, else EFalse
// ---------------------------------------------------------------------------
//
TBool CIRSongHistoryDb::AddToSongHistoryDbL(const TDesC& aSongName,
											const TDesC& aArtistName, 
											const TDesC& aChannelName, 
											const TDesC& aChannelUrl,
											RBuf& aDelSongName, 
											RBuf& aDelArtistName, 
											RBuf& aDelChannelName, 
											RBuf& aDelChannelUrl,
											TInt  aChannelType,
											TInt aChannelId,
											TInt aBitrate,
											const TDesC& aChannelDesc,
											const TDesC& aImageUrl,
											const TDesC& aMusicFlag
											)
{
	IRLOG_DEBUG( "CIRSongHistoryDb::AddSongHistoryStartL" );

	TBool bRetval = EFalse;
	TInt error= 0;

	iSongHistoryTable.Reset();

	//if song history entries are  greater than or equal to 30
	if( iSongHistoryTable.CountL() >= KMaxNoChannelHistoryDbEntries )
	{
		//first row is selected
		iSongHistoryTable.FirstL();

		//the current row is selected
		iSongHistoryTable.GetL();

		//delete that entry
		CDbColSet* columns = iSongHistoryTable.ColSetL();
		TInt songColumn = columns->ColNo( KSongHistoryDBSongNameColumn );
		TInt artistColumn = columns->ColNo( KSongHistoryDBArtistNameColumn );
		TInt channelColumn = columns->ColNo( KSongHistoryDBChannelNameColumn );
		TInt channelUrlColumn = columns->ColNo( KSongHistoryDBChannelUrlColumn );

		delete columns;
		columns = NULL;

        aDelSongName.Create(iSongHistoryTable.ColDes( songColumn ));
        aDelArtistName.Create(iSongHistoryTable.ColDes( artistColumn ));
        aDelChannelName.Create(iSongHistoryTable.ColDes( channelColumn ));
        aDelChannelUrl.Create(iSongHistoryTable.ColDes( channelUrlColumn ));
        
        iSongHistoryTable.DeleteL();
        Compact();

        bRetval = ETrue;
	}

    CDbColSet* columns = iSongHistoryTable.ColSetL();
    CleanupStack::PushL(columns);

	const TPtrC &songName = aSongName.Left(KIRDbMaxStrLen);
	const TPtrC &artistName = aArtistName.Left(KIRDbMaxStrLen);
	const TPtrC &channelName = aChannelName.Left(KIRDbMaxStrLen);
	const TPtrC &channelUrl = aChannelUrl.Left(KIRDbMaxStrLen);
    const TPtrC &channelDesc = aChannelDesc.Left(KIRDbMaxStrLen);
    const TPtrC &imageUrl = aImageUrl.Left(KIRDbMaxStrLen);
	
	TInt songLen = aSongName.Length();
	TInt artistLen = aArtistName.Length();
	if(  songLen + artistLen > KMaxSize )
	{
		IRLOG_DEBUG3( "CIRSongHistoryDb::AddSongHistoryStartL - LONG METADATA %d, %d", songLen, artistLen );
	}
	
	TRAP(error,//trap start

  		iSongHistoryTable.InsertL();
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBSongNameColumn ), songName);
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBArtistNameColumn ), artistName);
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBChannelNameColumn ), channelName);
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBChannelUrlColumn ), channelUrl);
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBChannelTypeColumn ), aChannelType);
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBChannelIdColumn ), aChannelId);
		iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBBitrateColumn ), aBitrate);
        iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBChannelDescColumn ), channelDesc);
        iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBImageUrlColumn ), imageUrl);
        iSongHistoryTable.SetColL(columns->ColNo( KSongHistoryDBMusicStatusColumn ), aMusicFlag);
		iSongHistoryTable.PutL();
		);

 	CleanupStack::PopAndDestroy(columns); 
 	if( error!=KErrNone )
 	{
	 	User::LeaveIfError(error);
 	}
    
 	iSongHistoryTable.Reset();
 	IRLOG_DEBUG( "CIRSongHistoryDb::AddSongHistoryStartL - Exiting." );

	return bRetval;
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb:AddToSongHistoryDb2L()
// adds the history song into the db
// Returns ETure if songHistory count exceeds 100, else EFalse
// ---------------------------------------------------------------------------
 
TBool CIRSongHistoryDb::AddToSongHistoryDb2L( const TDesC& aSongName,
                                    const TDesC& aArtistName,                                   
                                    const TDesC& aMusicFlag)
{
    IRLOG_DEBUG( "CIRSongHistoryDb::AddToSongHistoryDb2L" );

    TBool bRetval = EFalse;
    TInt error= 0;
    TInt songCompareFlag = 0;
    TInt artistCompareFlag = 0;    
     

    iSongHistoryTable2.Reset();    
    
    CDbColSet* columns = iSongHistoryTable2.ColSetL();
    CleanupStack::PushL(columns);
    
    /* the following is to judge wether the song is already in the db */    
    if ( iSongHistoryTable2.CountL() >= 1 )
    {
        TInt songNameColumn = columns->ColNo( KSongHistoryDBSongNameColumn );
        TInt artistNameColumn = columns->ColNo( KSongHistoryDBArtistNameColumn );     

        for ( iSongHistoryTable2.LastL(); iSongHistoryTable2.AtRow(); iSongHistoryTable2.PreviousL() )
        {
            iSongHistoryTable2.GetL();
            RBuf songName;
            RBuf artistName;
            
            songName.Create(iSongHistoryTable2.ColDes( songNameColumn ));
            songName.CleanupClosePushL();
            
            artistName.Create(iSongHistoryTable2.ColDes( artistNameColumn ));
            artistName.CleanupClosePushL();    
            
            songCompareFlag = (songName).Compare(aSongName);
            artistCompareFlag = (artistName).Compare(aArtistName);
            
            CleanupStack::PopAndDestroy(&artistName);       
            CleanupStack::PopAndDestroy(&songName);
            
            if( 0 == songCompareFlag && 0 == artistCompareFlag )
            {
                CleanupStack::PopAndDestroy(columns); 
                iSongHistoryTable2.Reset();
                return bRetval;
            }
        }        
    }
	/* end  */
    
    //if song history entries are  greater than or equal to 100
    if( iSongHistoryTable2.CountL() >= KMaxNoSongHistoryDbEntries )
    {
        //first row is selected
        iSongHistoryTable2.FirstL();
        //the current row is selected
        iSongHistoryTable2.GetL();       
        //delete the current row
        iSongHistoryTable2.DeleteL();
        Compact();     
        bRetval = ETrue;   
    }
    
    const TPtrC &songName = aSongName.Left(KIRDbMaxStrLen);
    const TPtrC &artistName = aArtistName.Left(KIRDbMaxStrLen);
    const TPtrC &musicFlag = aMusicFlag.Left(KIRDbMaxStrLen);
     
    
    TInt songLen = aSongName.Length();
    TInt artistLen = aArtistName.Length();
    if(  songLen + artistLen > KMaxSize )
    {
        IRLOG_DEBUG3( "CIRSongHistoryDb::AddSongHistoryStartL - LONG METADATA %d, %d", songLen, artistLen );
    }
    
    TRAP(error,//trap start

        iSongHistoryTable2.InsertL();
        iSongHistoryTable2.SetColL(columns->ColNo( KSongHistoryDBSongNameColumn ), songName);
        iSongHistoryTable2.SetColL(columns->ColNo( KSongHistoryDBArtistNameColumn ), artistName);        
        iSongHistoryTable2.SetColL(columns->ColNo( KSongHistoryDBMusicStatusColumn ), musicFlag);
        iSongHistoryTable2.PutL();
        );

    CleanupStack::PopAndDestroy(columns); 
    if( error!=KErrNone )
    {
        User::LeaveIfError(error);
    }
    
    iSongHistoryTable2.Reset();
    IRLOG_DEBUG( "CIRSongHistoryDb::AddSongHistoryStartL - Exiting." );

    return bRetval;  
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::GetAllSongHistoryListL()
//  gets all the song history entries  into an array
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::GetAllSongHistoryListL(RPointerArray<CIRSongHistoryInfo>& aHistoryDataArr)
{
 	IRLOG_DEBUG( "CIRSongHistoryDb::GetAllSongHistoryListL" ); 	 

    if(iSongHistoryTable.CountL() < 1)
    {
    	return;
    }

    CDbColSet* columns = iSongHistoryTable.ColSetL();
    TInt songColumn = columns->ColNo( KSongHistoryDBSongNameColumn );
    TInt artistColumn = columns->ColNo( KSongHistoryDBArtistNameColumn );
    TInt channelColumn = columns->ColNo( KSongHistoryDBChannelNameColumn );
    TInt channelUrlColumn = columns->ColNo( KSongHistoryDBChannelUrlColumn );
    TInt channelTypeColumn = columns->ColNo( KSongHistoryDBChannelTypeColumn );
    TInt channelIdColumn = columns->ColNo( KSongHistoryDBChannelIdColumn );
    TInt bitrateColumn = columns->ColNo( KSongHistoryDBBitrateColumn );
    TInt channelDescColumn = columns->ColNo( KSongHistoryDBChannelDescColumn );
    TInt imageUrlColumn = columns->ColNo( KSongHistoryDBImageUrlColumn );
    TInt musicStatusColumn = columns->ColNo( KSongHistoryDBMusicStatusColumn );

    delete columns;
    columns = NULL; 	 
    
 	TInt song = 0;
 	
 	
    for ( iSongHistoryTable.LastL(); iSongHistoryTable.AtRow(); iSongHistoryTable.PreviousL() ) 	 
    {
        iSongHistoryTable.GetL();
        
        RBuf songName;
        RBuf artistName;
        RBuf channelName;
        RBuf channelUrl;
        TInt channelType;
        TInt channelId;
        TInt bitrate;
        RBuf channelDesc;
        RBuf imageUrl;
        RBuf channelMusicStatus;
        
        songName.CreateL(iSongHistoryTable.ColDes( songColumn ));
        songName.CleanupClosePushL();
        
        artistName.CreateL(iSongHistoryTable.ColDes( artistColumn ));
        artistName.CleanupClosePushL();
        
        channelName.CreateL(iSongHistoryTable.ColDes( channelColumn ));
        channelName.CleanupClosePushL();
        
        channelUrl.CreateL(iSongHistoryTable.ColDes( channelUrlColumn ));
        channelUrl.CleanupClosePushL();
        
        channelType=iSongHistoryTable.ColUint8( channelTypeColumn );
        
        channelId=iSongHistoryTable.ColUint16( channelIdColumn );
        
        bitrate=iSongHistoryTable.ColUint16( bitrateColumn );
        
        channelDesc.CreateL(iSongHistoryTable.ColDes( channelDescColumn ));
        channelDesc.CleanupClosePushL();

        imageUrl.CreateL(iSongHistoryTable.ColDes( imageUrlColumn ));
        imageUrl.CleanupClosePushL();

        channelMusicStatus.CreateL(iSongHistoryTable.ColDes( musicStatusColumn ));
        channelMusicStatus.CleanupClosePushL();
 
        aHistoryDataArr[song]->SetHistoryInfo(songName, artistName, channelUrl,
                         channelName,channelType,channelId,bitrate,channelDesc ,
                         imageUrl, channelMusicStatus);
        ++song;
 
        
        CleanupStack::PopAndDestroy(&channelMusicStatus);
        CleanupStack::PopAndDestroy(&imageUrl);
        CleanupStack::PopAndDestroy(&channelDesc);
        CleanupStack::PopAndDestroy(&channelUrl);
        CleanupStack::PopAndDestroy(&channelName);
        CleanupStack::PopAndDestroy(&artistName);
        CleanupStack::PopAndDestroy(&songName);
    }        
 

	IRLOG_DEBUG( "CIRSongHistoryDb::GetAllSongHistoryListL - Exiting." );
}


// ---------------------------------------------------------------------------
// CIRSongHistoryDb::GetAllSongHistoryList2L()
//  gets all the song history entries  into an array
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::GetAllSongHistoryList2L(RPointerArray<CIRSongHistoryInfo>& aSongEntries)
{
    IRLOG_DEBUG( "CIRSongHistoryDb::GetAllSongHistoryList2L" );    

    if(iSongHistoryTable2.CountL() < 1)
    {
        return;
    }

    CDbColSet* columns = iSongHistoryTable2.ColSetL();
    TInt songColumn = columns->ColNo( KSongHistoryDBSongNameColumn );
    TInt artistColumn = columns->ColNo( KSongHistoryDBArtistNameColumn );     
    TInt musicStatusColumn = columns->ColNo( KSongHistoryDBMusicStatusColumn );

    delete columns;
    columns = NULL;
    
    TInt song = 0;
    
    for ( iSongHistoryTable2.LastL(); iSongHistoryTable2.AtRow(); iSongHistoryTable2.PreviousL() )
    {
        iSongHistoryTable2.GetL();
         
        RBuf songName;
        RBuf artistInfo;
        RBuf musicStatus;
        
        songName.Create(iSongHistoryTable2.ColDes( songColumn ));
        songName.CleanupClosePushL();
        
        artistInfo.Create(iSongHistoryTable2.ColDes( artistColumn ));
        artistInfo.CleanupClosePushL();
        
        musicStatus.Create(iSongHistoryTable2.ColDes(musicStatusColumn));
        musicStatus.CleanupClosePushL();
        
        aSongEntries[song]->SetSongName(songName);
        aSongEntries[song]->SetArtist(artistInfo);
        aSongEntries[song]->SetChannelMusicStatus(musicStatus);
        ++song;         
        
        CleanupStack::PopAndDestroy(&musicStatus);
        CleanupStack::PopAndDestroy(&artistInfo);
        CleanupStack::PopAndDestroy(&songName);        
    }
     
    IRLOG_DEBUG( "CIRSongHistoryDb::GetAllSongHistoryListL - Exiting." );
}

// ---------------------------------------------------------------------------
// Function : ClearAllSongHistoryFromDb
// delete all the entries from the database.
// ---------------------------------------------------------------------------
//
TInt CIRSongHistoryDb::ClearAllSongHistoryFromDb(/*RPointerArray<CIRSongHistoryInfo>& aSongHistoryInfoArr*/)
{

	TInt error = KErrNone;

	IRLOG_DEBUG( "CIRSongHistoryDb::ClearAllSongHistoryFromDb - Entering" );
	TRAP(error, iSongHistoryTable.FirstL());
	if(error != KErrNone)
	{
		return error;
	}

    while (iSongHistoryTable.AtRow())
	{
		TRAPD(error,
		iSongHistoryTable.GetL();
		iSongHistoryTable.DeleteL();
		iSongHistoryTable.NextL(););
		if(error)
		{
		    Compact();
		    return error;
		}
	}
	
    Compact();
    IRLOG_DEBUG( "CIRSongHistoryDb::ClearAllSongHistoryFromDb - Exiting." );
	return KErrNone;

}

TInt CIRSongHistoryDb::DeleteOneHistory(TInt aIndex)
{
    IRLOG_DEBUG( "CIRSongHistoryDb::DeleteOneHistory - Entering" );
    
    TInt retValue = 0;
    TRAPD(error,(retValue = DeleteOneHistoryL(aIndex)));
    
    if( KErrNone != error )
    {
        return error;
    }
    
    if( KErrNone != retValue)
    {
        return retValue;
    }    
    IRLOG_DEBUG( "CIRSongHistoryDb::DeleteOneHistory - exiting" );    
    return KErrNone;
}

TInt CIRSongHistoryDb::DeleteOneHistoryL(TInt aIndex)
{
    IRLOG_DEBUG( "CIRSongHistoryDb::DeleteOneHistoryL - Entering" );
    TInt totalCount = iSongHistoryTable.CountL();     
    if (aIndex < 0 || aIndex >= totalCount)
    {
        return KErrArgument;
    }
	
    iSongHistoryTable.LastL();
    TInt tempIndex = 0;     
    while (tempIndex < aIndex)
    {
        TRAPD(error,iSongHistoryTable.PreviousL());
        if (KErrNone != error)
        {
            Compact();
            return error;
        }
        tempIndex++;
    }
    
    iSongHistoryTable.GetL();
    iSongHistoryTable.DeleteL();
    IRLOG_DEBUG( "CIRSongHistoryDb::DeleteOneHistoryL - exiting" );   
    return KErrNone;    
}

// ---------------------------------------------------------------------------
// Function : ClearAllSongHistoryFromDb2
// delete all the entries from the database.
// ---------------------------------------------------------------------------
//
TInt CIRSongHistoryDb::ClearAllSongHistoryFromDb2()
{

    TInt error = KErrNone;

    IRLOG_DEBUG( "CIRSongHistoryDb::ClearAllSongHistoryFromDb2 - Entering" );
    TRAP(error, iSongHistoryTable2.FirstL());
    if(error != KErrNone)
    {
        return error;
    }

    while (iSongHistoryTable2.AtRow())
    {
        TRAPD(error,
        iSongHistoryTable2.GetL();
        iSongHistoryTable2.DeleteL();
        iSongHistoryTable2.NextL(););
        if(error)
        {
            Compact();
            return error;
        }

    }
    Compact();
    IRLOG_DEBUG( "CIRSongHistoryDb::ClearAllSongHistoryFromDb - Exiting." );
    return KErrNone;

}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::ConstructL()
// Standard 2nd phase construction
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::ConstructL()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::ConstructL" );
	User::LeaveIfError(iFsSession.Connect());

	CIRSettings* settings = CIRSettings::OpenL();

	TFileName songHistoryDbFile = settings->PrivatePath();
	songHistoryDbFile.Append(KSongHistoryDbFile);

	CreateDbCondition(songHistoryDbFile);
	OpenSongHistoryDbL(songHistoryDbFile);

	settings->Close();

	IRLOG_DEBUG( "CIRSongHistoryDb::ConstructL- Exiting." );
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::CloseSongHistoryDb()
// Closes the database
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::CloseSongHistoryDb()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::CloseSongHistoryDb" );
	iSongHistoryTable.Close();
	iSongHistoryTable2.Close();
	Close();
	IRLOG_DEBUG( "CIRSongHistoryDb::CloseSongHistoryDb - Exiting." );
}


/**
* Function : CreateSongHistoryTablesL()
* creates songhistory table with three columns
* log data
* ---------------------------------------------------------------------------
* SongHistoryTable
*---------------------------
*| SongName | ArtistName | ChannelName | ChannelUrl | ChannelType | ChannelId | Bitrate | ChannelDesc | ImageUrl 
*---------------------------
*|EDbColText| EDbColText | EDbColText | EDbColText | EDbColUint8 | EDbColUint16 | EDbColUint16 | EDbColText | EDbColText
*----------------------------------------------------------------------------
*/
void CIRSongHistoryDb::CreateSongHistoryTablesL()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateSongHistoryTablesL" );
    CDbColSet* columns = CDbColSet::NewLC();

    TRAPD( error,
    columns->AddL( TDbCol( KSongHistoryDBSongNameColumn, EDbColText, KMaxColumnLength ) );
    columns->AddL( TDbCol( KSongHistoryDBArtistNameColumn, EDbColText, KMaxColumnLength) );
    columns->AddL( TDbCol( KSongHistoryDBChannelNameColumn, EDbColText, KMaxColumnLength ) );
    columns->AddL( TDbCol( KSongHistoryDBChannelUrlColumn, EDbColText, KMaxColumnLength ) );
    columns->AddL( TDbCol( KSongHistoryDBChannelTypeColumn, EDbColUint8 ) );
    columns->AddL( TDbCol( KSongHistoryDBChannelIdColumn, EDbColUint16 ) );
    columns->AddL( TDbCol( KSongHistoryDBBitrateColumn, EDbColUint16 ) );
    columns->AddL( TDbCol( KSongHistoryDBChannelDescColumn, EDbColText, KMaxColumnLength ) );
    columns->AddL( TDbCol( KSongHistoryDBImageUrlColumn, EDbColText, KMaxColumnLength ) );
    columns->AddL( TDbCol( KSongHistoryDBMusicStatusColumn, EDbColText, KMaxColumnLength ) );
    );
	User::LeaveIfError( error );

    User::LeaveIfError( CreateTable( KSongHistoryDBTable, *columns ) );

    CleanupStack::PopAndDestroy( columns );
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateSongHistoryTablesL - Exiting." );
}

/**
* Function : CreateSongHistoryTables2L()
* creates songhistory table with three columns
* log data
* ---------------------------------------------------------------------------
* SongHistoryTable
*---------------------------
*| SongName | ArtistName | MusicStatusFlag 
*---------------------------
*|EDbColText| EDbColText | EDbColText 
*----------------------------------------------------------------------------
*/
void CIRSongHistoryDb::CreateSongHistoryTables2L()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateSongHistoryTablesL" );
    CDbColSet* columns = CDbColSet::NewLC();

    TRAPD( error,
    columns->AddL( TDbCol( KSongHistoryDBSongNameColumn, EDbColText, KMaxColumnLength ) );
    columns->AddL( TDbCol( KSongHistoryDBArtistNameColumn, EDbColText, KMaxColumnLength) );     
    columns->AddL( TDbCol( KSongHistoryDBMusicStatusColumn, EDbColText, KMaxColumnLength ) );
    );
	User::LeaveIfError( error );

    User::LeaveIfError( CreateTable( KSongHistoryDBTable2, *columns ) );

    CleanupStack::PopAndDestroy( columns );
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateSongHistoryTablesL - Exiting." );
}



// ---------------------------------------------------------------------------
// CIRSongHistoryDb::OpenSongHistoryDbL()
// opening the data base
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::OpenSongHistoryDbL(const TFileName& aSongHistoryDbFile)
{
	IRLOG_DEBUG( "CIRSongHistoryDb::OpenSongHistoryDbL" );
	CloseSongHistoryDb(); //Ensure that the database is closed before trying to open it.

	 
    TInt error  = KErrNone;
 
	error = Open(iFsSession,aSongHistoryDbFile);
	if( error!=KErrNone )
	{
		//if database is failed to open then
		//function leaves
		IRLOG_ERROR2( "CIRSongHistoryDb::OpenSongHistoryDbL - Opening session database failed (%d)", error );
		User::LeaveIfError(error);
	}
	if( IsDamaged() || !InTransaction() )
	{
		//if data base is damaged then
		//it tried to recover
		//if recovery is not possible function leaves
		error = Recover();
		if ( KErrNone == error)
			{
			//if recovered data base is compacted
			error = Compact();
			}
	    User::LeaveIfError(error);
	}

	//open the table
	error = iSongHistoryTable.Open(*this, KSongHistoryDBTable, iSongHistoryTable.EUpdatable);
	if( error )
	{
		User::LeaveIfError(error);
	}

	error = iSongHistoryTable2.Open(*this, KSongHistoryDBTable2, iSongHistoryTable2.EUpdatable);
	if( error )
	{
	    iSongHistoryTable.Close();
		User::LeaveIfError(error);
	} 

	IRLOG_DEBUG( "CIRSongHistoryDb::OpenSongHistoryDbL - Exiting." );
}


// ---------------------------------------------------------------------------
// CIRSongHistoryDb::CreateSongHistoryDb()
// @database filename
// ---------------------------------------------------------------------------
//
TInt CIRSongHistoryDb::CreateSongHistoryDb(const TFileName& aSongHistoryDbFile )
{
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateSongHistoryDb" );


	TInt error = Replace(iFsSession,aSongHistoryDbFile);
	if ( error != KErrNone )
    {
	    IRLOG_ERROR2( "CIRSongHistoryDb::CreateSongHistoryDb - Creating history database failed (%d)", error );
	    return error;
    }

	TRAPD(err1,CreateSongHistoryTablesL());
	if(err1)
	{
	    return err1;
	}

	//if error, no handling 
	TRAPD(err2, CreateSongHistoryTables2L());
	if( err2 )
	{
	    return err2;	  
	}	
	IRLOG_DEBUG( "CIRSongHistoryDb::CreateSongHistoryDb - Exiting." );
	return KErrNone;
}


// ---------------------------------------------------------------------------
// CIRSongHistoryDb::CountSongHistoryDb()
// count the database entries
// ---------------------------------------------------------------------------
//

TInt CIRSongHistoryDb::CountSongHistoryDb()
{
	IRLOG_DEBUG( "CIRSongHistoryDb::CountSongHistoryDb" );
	TInt historyCount = 0;
	iSongHistoryTable.Reset();
	TRAPD(error,historyCount= iSongHistoryTable.CountL());
	if(error)
	{
		
	}
	IRLOG_DEBUG( "CIRSongHistoryDb::CountSongHistoryDb - Exiting." );
 	return historyCount;

}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::CountSongHistoryDb2()
// count the database entries
// ---------------------------------------------------------------------------
//
TInt CIRSongHistoryDb::CountSongHistoryDb2()
{
    IRLOG_DEBUG( "CIRSongHistoryDb::CountSongHistoryDb" );
    TInt historyCount = 0;
    iSongHistoryTable2.Reset();
    TRAPD(error,historyCount= iSongHistoryTable2.CountL());
    if(error)
    {
        historyCount = -1;
    }
    IRLOG_DEBUG( "CIRSongHistoryDb::CountSongHistoryDb - Exiting." );
    return historyCount;
}
// ---------------------------------------------------------------------------
// CIRSongHistoryDb::GetLastHistoryItemL()
// Gets the last song history item
// ---------------------------------------------------------------------------
//
void CIRSongHistoryDb::GetLastHistoryItemL(RBuf& aSongName, RBuf& aArtistName,
				 RBuf& aChannelName, RBuf& aChannelUrl)
{
	if( iSongHistoryTable.IsEmptyL() || iSongHistoryTable.LastL() == EFalse)
	{
		return;
	}

	iSongHistoryTable.GetL();

	CDbColSet* columns = iSongHistoryTable.ColSetL();
	TInt songColumn = columns->ColNo( KSongHistoryDBSongNameColumn );
	TInt artistColumn = columns->ColNo( KSongHistoryDBArtistNameColumn );
	TInt channelColumn = columns->ColNo( KSongHistoryDBChannelNameColumn );
	TInt channelUrlColumn = columns->ColNo( KSongHistoryDBChannelUrlColumn );

	delete columns;
    columns = NULL;

    aSongName.Create(iSongHistoryTable.ColDes( songColumn ));
    aArtistName.Create(iSongHistoryTable.ColDes( artistColumn ));
    aChannelName.Create(iSongHistoryTable.ColDes( channelColumn ));
    aChannelUrl.Create(iSongHistoryTable.ColDes( channelUrlColumn ));


    iSongHistoryTable.Reset();
}
// ---------------------------------------------------------------------------
// CIRSongHistoryDb::GetChannelSongsCountL()
// Gets the channel's song count.
// ---------------------------------------------------------------------------
//

TUint CIRSongHistoryDb::GetChannelSongsCountL( const RBuf& aChannelName, const RBuf& aChannelUrl )
{
	_LIT( strQuery, "Select SongName from SongHistoryTable where ChannelName='%S' AND ChannelUrl='%S'");

	TBuf<KMaxSize>  bufQuery;
	bufQuery.Format( strQuery, &aChannelName, &aChannelUrl );

	TDbQuery	querySearchChannel( bufQuery );
	RDbView		dbView;

	TInt error = dbView.Prepare( *this, querySearchChannel );
	User::LeaveIfError(error);

	error = dbView.Evaluate();	// 0, evaluation is complete 
	User::LeaveIfError(error);

	TInt cRows = dbView.CountL();

	dbView.Close();

	return cRows;
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::SyncSongHistoryDb()
// Synchronises the History DB with removed channel entry in the ISDS
// ---------------------------------------------------------------------------
//

void CIRSongHistoryDb::SyncSongHistoryDbL(TInt aChannelId)
{
 	IRLOG_DEBUG( "CIRSongHistoryDb::SyncSongHistoryDbL" );


    if(iSongHistoryTable.CountL() < 1)
    {
    	return;
    }

    CDbColSet* columns = iSongHistoryTable.ColSetL();
    TInt songColumn = columns->ColNo( KSongHistoryDBSongNameColumn );
    TInt artistColumn = columns->ColNo( KSongHistoryDBArtistNameColumn );
    TInt channelColumn = columns->ColNo( KSongHistoryDBChannelNameColumn );
    TInt channelUrlColumn = columns->ColNo( KSongHistoryDBChannelUrlColumn );
    TInt channelTypeColumn = columns->ColNo( KSongHistoryDBChannelTypeColumn );
    TInt channelIdColumn = columns->ColNo( KSongHistoryDBChannelIdColumn );
    TInt bitrateColumn = columns->ColNo( KSongHistoryDBBitrateColumn );
    TInt channelDescColumn = columns->ColNo( KSongHistoryDBChannelDescColumn );
    TInt imageUrlColumn = columns->ColNo( KSongHistoryDBImageUrlColumn );
    TInt musicStatusColumn = columns->ColNo( KSongHistoryDBMusicStatusColumn );

    delete columns;
    columns = NULL;

	// Find out the channel to be changed 
		for ( iSongHistoryTable.LastL(); iSongHistoryTable.AtRow(); iSongHistoryTable.PreviousL() )
		{
			iSongHistoryTable.GetL();
			// Extracting the values from the database.
				TInt channelId;
				RBuf imageUrl;
                RBuf channelMusicStatus;
                
				channelId=iSongHistoryTable.ColUint16( channelIdColumn );
				imageUrl.Create(iSongHistoryTable.ColDes( imageUrlColumn ));
				imageUrl.CleanupClosePushL();
				channelMusicStatus.Create(iSongHistoryTable.ColDes( musicStatusColumn ));
                channelMusicStatus.CleanupClosePushL();

                
                if(aChannelId == channelId)
	                {
	                iSongHistoryTable.UpdateL();
		            iSongHistoryTable.SetColL(channelTypeColumn, 0);
		            iSongHistoryTable.SetColL(channelIdColumn, 0);
		            iSongHistoryTable.SetColL(imageUrlColumn, KNo);
		            iSongHistoryTable.SetColL(musicStatusColumn, KNo);
		            iSongHistoryTable.PutL();
	                }
                CleanupStack::PopAndDestroy(&channelMusicStatus);
                CleanupStack::PopAndDestroy(&imageUrl);
		}
	iSongHistoryTable.Reset();
		
	IRLOG_DEBUG( "CIRSongHistoryDb::SyncSongHistoryDbL - Exiting." );
}

// ---------------------------------------------------------------------------
// CIRSongHistoryDb::GetIdPresentInDb()
// Gets whether that particular channel id is present in Db or not.
// ---------------------------------------------------------------------------
//

TBool CIRSongHistoryDb::GetIdPresentInDbL(TInt aChannelId)
{
 	IRLOG_DEBUG( "CIRSongHistoryDb::GetIdPresentInDbL" );

 	TBool value = EFalse;

    if(iSongHistoryTable.CountL() < 1)
    {
    	return value;
    }

    CDbColSet* columns = iSongHistoryTable.ColSetL();
    TInt channelIdColumn = columns->ColNo( KSongHistoryDBChannelIdColumn );

    delete columns;
    columns = NULL;

	// Find out the channel to be changed 
		for ( iSongHistoryTable.LastL(); iSongHistoryTable.AtRow(); iSongHistoryTable.PreviousL() )
		{
			iSongHistoryTable.GetL();
				TInt channelId;
				
				channelId=iSongHistoryTable.ColUint16( channelIdColumn );
                if(aChannelId == channelId)
	                {
	                value = ETrue;
	                }

		}
		
	iSongHistoryTable.Reset();
	
	IRLOG_DEBUG( "CIRSongHistoryDb::GetIdPresentInDbL - Exiting." );
	return value ;
}



// ---------------------------------------------------------------------------
// CIRSongHistoryDb::UpdateSongHistoryDb()
// Updates the song history DB when there is a channel change in the isds.
// ---------------------------------------------------------------------------
//

TInt CIRSongHistoryDb::UpdateSongHistoryDbL(	TInt aChannelId,
											const TDesC& aChannelName, 
											const TDesC& aChannelUrl,
											const TDesC& aImageUrl,
											const TDesC& aMusicFlag)
{
 	IRLOG_DEBUG( "CIRSongHistoryDb::UpdateSongHistoryDbL" );

 	TBool channelChanged = EFalse ;
 	TInt compFlagName ;
 	TInt compFlagUrl ;
 	TInt compFlagImageUrl ;
 	TInt compFlagMusic ;
 	TInt compUrlChange ;

    if(iSongHistoryTable.CountL() < 1)
    {
    	return EFalse;
    }

    CDbColSet* columns = iSongHistoryTable.ColSetL();
    TInt channelColumn = columns->ColNo( KSongHistoryDBChannelNameColumn );
    TInt channelUrlColumn = columns->ColNo( KSongHistoryDBChannelUrlColumn );
    TInt channelTypeColumn = columns->ColNo( KSongHistoryDBChannelTypeColumn );
    TInt channelIdColumn = columns->ColNo( KSongHistoryDBChannelIdColumn );
    TInt channelDescColumn = columns->ColNo( KSongHistoryDBChannelDescColumn );
    TInt imageUrlColumn = columns->ColNo( KSongHistoryDBImageUrlColumn );
    TInt musicStatusColumn = columns->ColNo( KSongHistoryDBMusicStatusColumn );

    delete columns;
    columns = NULL;

	// Find out the channel to be changed 
	
		for ( iSongHistoryTable.LastL(); iSongHistoryTable.AtRow(); iSongHistoryTable.PreviousL() )
		{
			iSongHistoryTable.GetL();
			// Extracting the values from the database.
				RBuf channelName;
				RBuf channelUrl;
				RBuf imageUrl;
				TInt channelId;
                RBuf channelMusicStatus;
				channelName.Create(iSongHistoryTable.ColDes( channelColumn ));
				channelName.CleanupClosePushL();
				
				channelUrl.Create(iSongHistoryTable.ColDes( channelUrlColumn ));
				channelUrl.CleanupClosePushL();
				
				imageUrl.Create(iSongHistoryTable.ColDes( imageUrlColumn ));
				imageUrl.CleanupClosePushL();

				channelId=iSongHistoryTable.ColUint16( channelIdColumn );
                
                channelMusicStatus.Create(iSongHistoryTable.ColDes( musicStatusColumn ));
                channelMusicStatus.CleanupClosePushL();
                
                if(aChannelId == channelId)
	                {
					compFlagName =  (channelName).Compare(aChannelName);
	                compFlagUrl =   (channelUrl).Compare(aChannelUrl);
	                compFlagImageUrl = (imageUrl).Compare(aImageUrl);
	                compFlagMusic = (channelMusicStatus).Compare(aMusicFlag);
	                if(compFlagName)
		                {
	                    iSongHistoryTable.UpdateL();
		                iSongHistoryTable.SetColL(channelColumn, aChannelName);
		                iSongHistoryTable.PutL();
		                channelChanged = ETrue ;
		                }
		            compUrlChange = (aChannelUrl).Compare(KNo);
		                
	                if(compFlagUrl && compUrlChange)
		                {
		                iSongHistoryTable.UpdateL();
		                iSongHistoryTable.SetColL(channelUrlColumn, aChannelUrl);
		                iSongHistoryTable.PutL();
		                channelChanged = ETrue ;
		                }
	                if(compFlagImageUrl)
	                    {
	                    iSongHistoryTable.UpdateL();
	                    iSongHistoryTable.SetColL(imageUrlColumn, aImageUrl);
	                    iSongHistoryTable.PutL();
	                    channelChanged = ETrue ;
	                    }
	                if(compFlagMusic)
		                {
		                iSongHistoryTable.UpdateL();
		                iSongHistoryTable.SetColL(musicStatusColumn, aMusicFlag);
		                iSongHistoryTable.PutL();
		                channelChanged = ETrue ;
		                }
	                
	                }
                CleanupStack::PopAndDestroy(&channelMusicStatus);
                CleanupStack::PopAndDestroy(&imageUrl);
				CleanupStack::PopAndDestroy(&channelUrl);
				CleanupStack::PopAndDestroy(&channelName);

		}
	iSongHistoryTable.Reset();
	IRLOG_DEBUG( "CIRSongHistoryDb::UpdateSongHistoryDbL - Exiting." );
	return channelChanged;
}

void CIRSongHistoryDb::SearchAndDeleteRecordL(const TDesC& aChannelName, const TDesC& aChannelUrl,
                                              TInt aChannelType)
{
    CDbColSet* columns = iSongHistoryTable.ColSetL();
    TInt channelNameColumn = columns->ColNo( KSongHistoryDBChannelNameColumn );
    TInt channelUrlColumn = columns->ColNo( KSongHistoryDBChannelUrlColumn );
    TInt channelTypeColumn = columns->ColNo( KSongHistoryDBChannelTypeColumn );
    
    delete columns;
    columns = NULL;
    
    for (iSongHistoryTable.LastL(); iSongHistoryTable.AtRow(); iSongHistoryTable.PreviousL())
    {
        iSongHistoryTable.GetL();
        TPtrC channelName = iSongHistoryTable.ColDes(channelNameColumn);
        TPtrC channelUrl  = iSongHistoryTable.ColDes(channelUrlColumn);
        TInt channelType = iSongHistoryTable.ColInt(channelTypeColumn);
        
        if (aChannelName == channelName && aChannelUrl == channelUrl && aChannelType == channelType)
        {
            iSongHistoryTable.DeleteL();  
            return;
        }
    }
}
