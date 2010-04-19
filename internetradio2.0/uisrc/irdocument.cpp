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


#include <aknglobalnote.h>
#include <apgwgnam.h>
#include <hal.h>
#include <internetradio.rsg>
#include <stringloader.h>
#include <coeaui.h>

#include "ir.hrh"
#include "irdebug.h"
#include "irdocument.h"
#include "irui.h"
#include "irpubsubkeys.h"


// At least 3 MB of free RAM is needed to run the application.
const TInt KIRMinimumRAMNeeded = 3145728;
// 5 seconds inverval for memory check timer.
const TTimeIntervalMicroSeconds32 KIRMemCheckTimerInterval = 5000000;

const TInt KBufMaxSize = 255;

_LIT( KIRPlsFile, "c:\\private\\2000b499\\tempplsfile.pls" );
// ----------------------------------------------------------------------------
// CIRDocument::NewL(CEikApplication& aApp).
// returns an instance of CIRDocument.
// ----------------------------------------------------------------------------
//      
CIRDocument* CIRDocument::NewL(CEikApplication& aApp)
    {
    IRLOG_INFO( "CIRDocument::NewL - Entering" );    
    CIRDocument* self = new (ELeave) CIRDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRDocument::NewL - Exiting" );    
    return self;
    }

// ----------------------------------------------------------------------------
// CIRDocument::ConstructL().
// ----------------------------------------------------------------------------
//  
void CIRDocument::ConstructL()
    {
    IRLOG_DEBUG( "CIRDocument::ConstructL - Entering" );  
	iMemCheckTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iMemCheckTimer->Start(0, KIRMemCheckTimerInterval, 
        TCallBack( CIRDocument::MemCheckTimerCallback, this ) );
    IRLOG_DEBUG( "CIRDocument::ConstructL - Exiting" );
    }    


// ----------------------------------------------------------------------------
// CIRDocument::CIRDocument(CEikApplication& aApp).
// ----------------------------------------------------------------------------
//
CIRDocument::CIRDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    IRLOG_DEBUG( "CIRDocument::CIRDocument" );
    }

CIRDocument::~CIRDocument()
    {
    IRLOG_DEBUG( "CIRDocument::~CIRDocument - Entering" );
    delete iMemCheckTimer;
    IRLOG_DEBUG( "CIRDocument::~CIRDocument - Exiting" );
    }
    
// ----------------------------------------------------------------------------
// CIRDocument::CreateAppUiL().
// Create the application user interface, and return a pointer to it,the 
// framework takes ownership of this object 
// ----------------------------------------------------------------------------
// 
CEikAppUi* CIRDocument::CreateAppUiL()
    {
    IRLOG_DEBUG( "CIRDocument::CreateAppUiL - Entering" );
    iAppUi = new (ELeave) CIRUi;
    IRLOG_DEBUG( "CIRDocument::CreateAppUiL - Exiting" );
    return iAppUi;
    }

TInt CIRDocument::MemCheckTimerCallback(TAny* /*aSelf*/)
    {
    IRLOG_DEBUG("CIRDocument::MemCheckTimerCallback(TAny*) - Entering");
    TInt val(0);
    TInt err = HAL::Get(HALData::EMemoryRAMFree, val);
    if ( err || (val < KIRMinimumRAMNeeded) )
        {
        TRAP_IGNORE(
            HBufC* errorText = StringLoader::LoadLC( R_IRAPP_MEMLO_RAM_OUT_OF_MEM );
            CAknGlobalNote* note = CAknGlobalNote::NewLC();
            note->ShowNoteL( EAknGlobalErrorNote, *errorText );
            CleanupStack::PopAndDestroy( note );
            CleanupStack::PopAndDestroy( errorText ) )
        User::Exit( KErrNoMemory );
        }
    IRLOG_DEBUG("CIRDocument::MemCheckTimerCallback(TAny*) - Exiting");
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIRDocument::OpenFileL( TBool aDoOpen, const TDesC& aFilename, RFs& aFs )
// ----------------------------------------------------------------------------
//
CFileStore* CIRDocument::OpenFileL( TBool /*aDoOpen*/, const TDesC& aFilename, RFs& /*aFs*/ )
    {
    IRLOG_DEBUG("CIRDocument::OpenFileL(TBool, const TDesC&, RFs&)");
    _LIT(KPlsExtension, ".pls");
    
    CIRUi *appui = static_cast<CIRUi *> (iAppUi);

	TParse parse;
	parse.Set(aFilename,NULL,NULL);
	TPtrC ext=parse.Ext(); // extract the extension from the filename  
	CEikonEnv* env = CEikonEnv::Static();      
	RFs& fs = env->FsSession();
	RFile plsFile;
	TInt status=plsFile.Open(fs, aFilename, EFileShareReadersOnly);
	if(status)
		{
	    OpenFileL( ETrue, aFilename, fs );
		return NULL;	
		}
	CleanupClosePushL(plsFile);
	CFileStore* fileStore;
	fileStore = NULL; //So the other OpenFileL version is not called

	if (ext.CompareF(KPlsExtension)==0) 
		{
		appui->ParsePlsDataL(fileStore,plsFile);	
		}
	CleanupStack::PopAndDestroy( &plsFile );
	//Deleting the local copy of the pls file
	fs.Delete( KIRPlsFile );	
	return NULL;
    }

// ----------------------------------------------------------------------------
// CIRDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
// ----------------------------------------------------------------------------
//

void CIRDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
    {
	IRLOG_DEBUG("CIRDocument::OpenFileL(CFileStore*&, RFile&) - Entering");
	CIRUi *appui = static_cast<CIRUi *> (iAppUi);
	aFileStore = NULL;  //So the other OpenFileL version is not called
	
	TBuf16<KBufMaxSize> aName;
	aFile.FullName(aName);

	CEikonEnv* env = CEikonEnv::Static();
	TApaTaskList taskList( env->WsSession() );
	// Find handler application by its UID
	RWsSession& ws = env->WsSession();
	const TInt myWgId = env->RootWin().Identifier();
	TInt wgId = 0;
	TUid uid(KUidActiveInternetRadioApp);
	// Look for another instance of this app
	while (wgId >= 0)
		{
		if (wgId && wgId != myWgId)
			{
			TApaTask FirstInstance(ws);
			TApaTask SecondInstance(ws);
			FirstInstance.SetWgId(wgId);
			SecondInstance.SetWgId(myWgId);
			SecondInstance.EndTask();  
			
		    //To copy the contents of .PLS file into local PLS file 
		    
		    TInt fileSize;
		    User::LeaveIfError( aFile.Size( fileSize ) );
			HBufC8* plsData = HBufC8::NewLC( fileSize );
			TPtr8 pData8( plsData->Des() );
			User::LeaveIfError( aFile.Read( pData8 ) );
			RFile plsFile;
			RFs& fs = env->FsSession();
			TInt ret=plsFile.Open(fs, KIRPlsFile, EFileShareReadersOrWriters);
			if(ret==KErrNotFound)
				{
				TInt status=plsFile.Create(fs, KIRPlsFile, EFileShareReadersOrWriters);
				}
            plsFile.Write(pData8);
			TBuf16<KBufMaxSize> plsName;
			plsFile.FullName(plsName);
            
            CleanupStack::PopAndDestroy( plsData );
            
			User::LeaveIfError( FirstInstance.SwitchOpenFile( plsName ) );
			return;
			}
		CApaWindowGroupName::FindByAppUid(uid, ws, wgId);
		}

	_LIT(KPlsExtension, ".pls");
	TParse parse;
	parse.Set(aName,NULL,NULL);
	TPtrC ext=parse.Ext(); // extract the extension from the filename
	if (ext.CompareF(KPlsExtension)==0) 
		{
		appui->ParsePlsDataL(aFileStore,aFile);	
		}
	IRLOG_DEBUG("CIRDocument::OpenFileL - Exiting."); 
    }

// ---------------------------------------------------------------------------
// From CEikDocument; Hide the application from the tasklist
// ---------------------------------------------------------------------------
//    
void CIRDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
    {
    IRLOG_INFO("CIRDocument::UpdateTaskNameL - Hiding InternetRadio from grid - Entering");
    CAknDocument::UpdateTaskNameL( aWgName );
#ifndef __DISABLE_RADIO_LAUNCHER
    
    aWgName->SetHidden( EFalse );
    IRLOG_DEBUG("CIRDocument::UpdateTaskNameL - Exiting."); 
#endif
    }
