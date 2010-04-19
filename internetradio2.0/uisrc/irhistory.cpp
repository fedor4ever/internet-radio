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
* Description:  ?Description
*
*/


#include <bautils.h>
#include <s32file.h>

#include "irdebug.h"
#include "irhistory.h"
#include "irisdspreset.h"
#include "irui.h"

//constant declaration
_LIT(KLastPlayedChannel,"lastPlayed.dat");

// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::NewL
// 
// ---------------------------------------------------------------------------
//
CIRLastPlayedStationInfo* CIRLastPlayedStationInfo::NewL()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::NewL - Entering" );
	CIRLastPlayedStationInfo *self=CIRLastPlayedStationInfo::NewLC();
	CleanupStack::Pop(self);
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::NewL - Exiting." );
	return self;
	}
	
// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::NewLC
// 
// ---------------------------------------------------------------------------
//
CIRLastPlayedStationInfo* CIRLastPlayedStationInfo::NewLC()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::NewLC - Entering" );
	CIRLastPlayedStationInfo *self=new(ELeave)CIRLastPlayedStationInfo;
	CleanupStack::PushL(self);
	self->ConstructL();
    IRLOG_DEBUG( "CIRLastPlayedSongInfo::NewLC - Exiting." );	
	return self;
	}
	
// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::ConstructL
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CIRLastPlayedStationInfo::ConstructL()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::ConstructL - Entering" );
	iLastPlayedStation=CIRIsdsPreset::NewL();
	ui = REINTERPRET_CAST(CIRUi*,CEikonEnv::Static()->EikAppUi());
	TFileName pathLastPlayed=ui->GetPrivatePath();
	pathLastPlayed.Append(KLastPlayedChannel);
	
    RFs& fs = CCoeEnv::Static()->FsSession();	
	TBool res;
	res= BaflUtils::FileExists( fs, pathLastPlayed );
	if(res)
		{
        TRAPD( err, RetriveLastPlayedChannelL() );
        if ( err != KErrNone )
            {
            IRLOG_ERROR2( "CIRLastPlayedSongInfo::ConstructL - Retrieving last played channel failed, err=%d", err );            
            delete iLastPlayedStation;
            iLastPlayedStation = NULL;
            iLastPlayedStation = CIRIsdsPreset::NewL();
            User::LeaveIfError( BaflUtils::DeleteFile( fs, pathLastPlayed ) );            
            }
		}	
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::ConstructL - Exiting." );		
	}
	
// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::~CIRLastPlayedStationInfo
// standard c++ destructor
// ---------------------------------------------------------------------------
//	
CIRLastPlayedStationInfo::~CIRLastPlayedStationInfo()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::~CIRLastPlayedStationInfo - Entering" );
	if(iLastPlayedStation)
		{
		delete iLastPlayedStation;
		iLastPlayedStation = NULL;
		}
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::~CIRLastPlayedStationInfo - Exiting." );	
	}

// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::RetriveLastPlayedChannelL
// Internalizes the preset data from the last played dat file
// ---------------------------------------------------------------------------
//	
void CIRLastPlayedStationInfo::RetriveLastPlayedChannelL()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::RetriveLastPlayedChannelL - Entering" );
	TFileName filename;
	filename=ui->GetPrivatePath();
	filename.Append(KLastPlayedChannel);
	TParse	filestorename;
	ui->iFsSession.Parse(filename,filestorename);
	CFileStore* store = CDirectFileStore::OpenLC(ui->iFsSession,filestorename.
		FullName(),EFileRead);
		
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
   	iLastPlayedStation->InternalizeL(instream);
	CleanupStack::PopAndDestroy(&instream);
	CleanupStack::PopAndDestroy(store);
    IRLOG_DEBUG( "CIRLastPlayedSongInfo::RetriveLastPlayedChannelL - Exiting." );	
	}

// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::CommitLastPlayedChannelL
// Externalizes the preset data into last played dat
// ---------------------------------------------------------------------------
//	
void CIRLastPlayedStationInfo::CommitLastPlayedChannelL()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::CommitLastPlayedChannelL - Entering" );
	TFileName filename;
	filename=ui->GetPrivatePath();
	filename.Append(KLastPlayedChannel);
	TParse	filestorename;
	ui->iFsSession.Parse(filename,filestorename);
	CFileStore* store = CDirectFileStore::ReplaceLC(ui->iFsSession,
		filestorename.FullName(),EFileWrite);
		
	// Must say what kind of file store.
	store->SetTypeL(KDirectFileStoreLayoutUid);
    // Construct the output stream.
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	iLastPlayedStation->ExternalizeL(outstream);
	outstream.CommitL();
	// Cleanup the stream object
	CleanupStack::PopAndDestroy(&outstream);
	// Set this stream id as the root
	store->SetRootL(id);
	// Commit changes to the store
	store->CommitL();
	CleanupStack::PopAndDestroy(store);	
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::CommitLastPlayedChannelL - Exiting." );	
	}
	
// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::FileExists
// checks if the last played dat file exists
// ---------------------------------------------------------------------------
//
TBool CIRLastPlayedStationInfo::FileExists()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::FileExists - Entering" );
	TFileName filename; 
	filename=ui->GetPrivatePath();
	filename.Append(KLastPlayedChannel);
	TBool result=BaflUtils::FileExists(ui->iFsSession,filename);
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::FileExists - Exiting." );
	return result;
	}
	
// ---------------------------------------------------------------------------
// CIRLastPlayedStationInfo::RemoveLastPlayedFile
// removes the last played dat file if it gets corrupted
// ---------------------------------------------------------------------------
//
TInt CIRLastPlayedStationInfo::RemoveLastPlayedFile()
	{
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::RemoveLastPlayedFile - Entering" );
	TInt ret = KErrNone;
	if(FileExists())
		{
		CFileMan *fileHandle = NULL;
		TRAPD(err , fileHandle=CFileMan::NewL(ui->iFsSession));	
		if(err)
			{
			ret = err ;
			}
		TFileName filename; 
		filename=ui->GetPrivatePath();
    		filename.Append(KLastPlayedChannel);
    		ret = fileHandle->Delete(filename);
		delete fileHandle;
		}
	IRLOG_DEBUG( "CIRLastPlayedSongInfo::RemoveLastPlayedFile - Exiting." );	
	return ret;
	
	}

