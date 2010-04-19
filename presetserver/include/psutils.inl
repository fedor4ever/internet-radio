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
* Description:  Common utility methods.
*
*/


#if defined __SERIES60_30__ || defined __SERIES60_31__
_LIT( KPSDatabaseFileName, "\\data\\pspresets.db" );    // File name of the preset database.
#else
_LIT( KPSDatabaseFileName, "pspresets.db" );            // File name of the preset database.
#endif

// ---------------------------------------------------------------------------
// Gets the full file name and path to the preset database file.
// ---------------------------------------------------------------------------
//
inline void PSUtils::GetDatabaseFullNameL( TFileName& aFullName )
    {
    PSDEBUG( "PSUtils::GetDatabaseFullNameL( TFileName& ) - Enter" );

    TParsePtrC phoneMemoryRootPath( PathInfo::PhoneMemoryRootPath() );
    TPtrC drive( phoneMemoryRootPath.Drive() );

    TParse parse;

#if defined __SERIES60_30__ || defined __SERIES60_31__
    parse.Set( KPSDatabaseFileName, &drive, NULL );
#else
    // The database file resides in the server's private directory from 3.2 onwards.
    TFileName privatePath;
    
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    
    User::LeaveIfError( fsSession.PrivatePath( privatePath ) );
    parse.Set( privatePath, &KPSDatabaseFileName, &drive );

    CleanupStack::PopAndDestroy( &fsSession );
#endif

    aFullName = parse.FullName();

    PSDEBUG2( "PSUtils::GetDatabaseFullNameL( aFullName = %S ) - Exit", &aFullName );
    }
