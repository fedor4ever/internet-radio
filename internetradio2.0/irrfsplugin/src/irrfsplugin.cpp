/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Internet Radio RFS ECOM plugin implementation
*
*/


#include <apacmdln.h>
#include <apgcli.h>

#include "irrfsplugin.h"

// Files that are deleted upon RFS.
_LIT( KIRLastPlayedFile, "c:\\private\\2000b499\\lastPlayed.dat" );
_LIT( KIRCacheDatabaseFile, "c:\\private\\2000b499\\cacheDb.db" );
_LIT( KIRSdsResponseFile, "c:\\private\\2000b499\\iSdsResponse.xml" );
_LIT( KIRSessionFile, "c:\\private\\2000b499\\Session" );
_LIT( KIRRadioReportFile, "c:\\private\\2000b499\\RadioReport.xml" );
_LIT( KIRLogUsageFile, "c:\\private\\2000b499\\LogUsage.gz" );
_LIT( KIRSettingsFile, "c:\\private\\2000b499\\irsettings.ini" );
_LIT( KIRBackedUpSettingsFile, "c:\\private\\2000b499\\irsettings_backed_up.ini" );
_LIT( KIRSettingscreFile, "c:\\private\\10202be9\\persists\\2000B499.cre" );

//for History Database Deletion
_LIT(KSongHistoryDbFile,"songhistoryDb.db");

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRRfsPlugin* CIRRfsPlugin::NewL( TAny* aInitParams )
    {
    CIRRfsPlugin* self = new ( ELeave ) CIRRfsPlugin( aInitParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIRRfsPlugin::CIRRfsPlugin( TAny* /*aInitParams*/ )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRRfsPlugin::ConstructL()
    {
	return;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIRRfsPlugin::~CIRRfsPlugin()
    {
    // Base class handles the ECOM cleanup.
    }

// ---------------------------------------------------------------------------
// From base class CRFSPlugin
// ---------------------------------------------------------------------------
//
void CIRRfsPlugin::RestoreFactorySettingsL( const TRfsReason aType )
    {
    if ( aType == ENormalRfs )
        {
        RFs fsSession;
        User::LeaveIfError( fsSession.Connect() );

        fsSession.Delete( KIRLastPlayedFile );
        fsSession.Delete( KIRCacheDatabaseFile );
        fsSession.Delete( KIRSdsResponseFile );
        fsSession.Delete( KIRSessionFile );
        fsSession.Delete( KIRRadioReportFile );
        fsSession.Delete( KIRLogUsageFile );

        fsSession.Delete( KIRSettingsFile );
        fsSession.Delete( KIRBackedUpSettingsFile );
        fsSession.Delete( KIRSettingscreFile );
    	fsSession.Delete(KSongHistoryDbFile);
        
        fsSession.Close();
        }
    else
        {
        // RFS type not supported.
        }
    }

// ---------------------------------------------------------------------------
// From base class CRFSPlugin
// ---------------------------------------------------------------------------
//
void CIRRfsPlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    aPath.Zero();
	return;
	}

// ---------------------------------------------------------------------------
// From base class CRFSPlugin
// ---------------------------------------------------------------------------
//
void CIRRfsPlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/, TDesC& /*aCommand*/ )
    {
	return;
	}

// ======== GLOBAL FUNCTIONS ========

