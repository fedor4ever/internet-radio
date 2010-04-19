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
* Description:  Describes a single item in PLS playlist.
*
*/


#include "irplsplaylistitem.h"
#include "irdebug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::CIRPlsPlayListItem
// ---------------------------------------------------------------------------
//
CIRPlsPlayListItem::CIRPlsPlayListItem()
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::CIRPlsPlayListItem" );
    }
    


// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::NewL
// ---------------------------------------------------------------------------
//
CIRPlsPlayListItem* CIRPlsPlayListItem::NewL()
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::NewL - Entering" );
    CIRPlsPlayListItem* self = CIRPlsPlayListItem::NewLC();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRPlsPlayListItem::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::NewLC
// ---------------------------------------------------------------------------
//
CIRPlsPlayListItem* CIRPlsPlayListItem::NewLC()
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::NewLC - Entering" );
    CIRPlsPlayListItem* self = new( ELeave ) CIRPlsPlayListItem;
    CleanupStack::PushL( self );
    IRLOG_DEBUG( "CIRPlsPlayListItem::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::~CIRPlsPlayListItem
// ---------------------------------------------------------------------------
//
CIRPlsPlayListItem::~CIRPlsPlayListItem()
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::~CIRPlsPlayListItem - Entering" );
    iFile.Close();
    iTitle.Close();
    IRLOG_DEBUG( "CIRPlsPlayListItem::~CIRPlsPlayListItem - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::SetFileL
// ---------------------------------------------------------------------------
//
void CIRPlsPlayListItem::SetFileL( const TDesC& aFile )
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::SetFileL - Entering" );
    iFile.Close();
    iFile.CreateL( aFile );
    IRLOG_DEBUG( "CIRPlsPlayListItem::SetFileL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::SetTitleL
// ---------------------------------------------------------------------------
//
void CIRPlsPlayListItem::SetTitleL( const TDesC& aTitle )
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::SetTitleL - Entering" );
    iTitle.Close();
    iTitle.CreateL( aTitle );
    IRLOG_DEBUG( "CIRPlsPlayListItem::SetTitleL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::SetLength
// ---------------------------------------------------------------------------
//
void CIRPlsPlayListItem::SetLength( TInt aLength )
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::SetLength - Entering" );
    iLength = aLength;
    IRLOG_DEBUG( "CIRPlsPlayListItem::SetLength - Exiting" );
    }
    
// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::File
// ---------------------------------------------------------------------------
//
const TDesC& CIRPlsPlayListItem::File() const
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::File" );
    return iFile;
    }
    
// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::Title
// ---------------------------------------------------------------------------
//
const TDesC& CIRPlsPlayListItem::Title() const
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::Title" );
    return iTitle;
    }
    
// ---------------------------------------------------------------------------
// CIRPlsPlayListItem::Length
// ---------------------------------------------------------------------------
//
TInt CIRPlsPlayListItem::Length() const
    {
    IRLOG_DEBUG( "CIRPlsPlayListItem::Length" );
    return iLength;
    }
