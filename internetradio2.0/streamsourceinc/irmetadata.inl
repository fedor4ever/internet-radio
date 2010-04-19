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
* Description:  Meta data information wrapper
*
*/


// ---------------------------------------------------------------------------
// Default two-phased constructor.
// ---------------------------------------------------------------------------
//
inline CIRMetaData* CIRMetaData::NewL()
    {
    CIRMetaData* self = new ( ELeave ) CIRMetaData;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor for direct internalization.
// ---------------------------------------------------------------------------
//
inline CIRMetaData* CIRMetaData::NewL( const TDesC8& aData )
    {
    CIRMetaData* self = new ( ELeave ) CIRMetaData;
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
inline CIRMetaData::CIRMetaData()
    {
    }
    
// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// Second-phase constructor from binary data.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::ConstructL( const TDesC8& aData )
    {
    RDesReadStream stream( aData );
    CleanupClosePushL( stream );
    InternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
inline CIRMetaData::~CIRMetaData()
    {
    iArtist.Close();
    iSong.Close();
    iStreamUrl.Close();
    }

// ---------------------------------------------------------------------------
// Copies values from the supplied meta data object.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::SetL( const CIRMetaData& aMetaData )
    {
    SetArtistL( aMetaData.Artist() );
    SetSongL( aMetaData.Song() );
    SetStreamUrlL( aMetaData.StreamUrl() );
    }

// ---------------------------------------------------------------------------
// Sets the artist name.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::SetArtistL( const TDesC& aArtist )
    {
    iArtist.Close();
    iArtist.CreateL( aArtist );
    }
    
// ---------------------------------------------------------------------------
// Sets the song name.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::SetSongL( const TDesC& aSong )
    {
    iSong.Close();
    iSong.CreateL( aSong );
    }
    
// ---------------------------------------------------------------------------
// Sets the stream URL.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::SetStreamUrlL( const TDesC& aStreamUrl )
    {
    iStreamUrl.Close();
    iStreamUrl.CreateL( aStreamUrl );
    }

// ---------------------------------------------------------------------------
// Returns the artist name.
// ---------------------------------------------------------------------------
//
inline const TDesC& CIRMetaData::Artist() const
    {
    return iArtist;
    }
    
// ---------------------------------------------------------------------------
// Returns the song name.
// ---------------------------------------------------------------------------
//
inline const TDesC& CIRMetaData::Song() const
    {
    return iSong;
    }
    
// ---------------------------------------------------------------------------
// Returns the stream URL.
// ---------------------------------------------------------------------------
//
inline const TDesC& CIRMetaData::StreamUrl() const
    {
    return iStreamUrl;
    }
    
// ---------------------------------------------------------------------------
// Internalizes the object from the supplied stream.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::InternalizeL( RReadStream& aStream )
    {
    // The order of internalization must not be changed, nor can any of these
    // be removed. Futher data fields may be added.

    iArtist.Close();
    iArtist.CreateL( aStream, KMaxTInt );

    iSong.Close();
    iSong.CreateL( aStream, KMaxTInt );

    iStreamUrl.Close();
    iStreamUrl.CreateL( aStream, KMaxTInt );
    }
    
// ---------------------------------------------------------------------------
// Externalizes the object to the supplied stream.
// ---------------------------------------------------------------------------
//
inline void CIRMetaData::ExternalizeL( RWriteStream& aStream ) const
    {
    // The order of externalization must not be changed, nor can any of these
    // be removed. Futher data fields may be added.

    aStream << iArtist;
    aStream << iSong;
    aStream << iStreamUrl;
    
    aStream.CommitL();
    }

// ---------------------------------------------------------------------------
// Equality comparison operator.
// ---------------------------------------------------------------------------
//
TBool CIRMetaData::operator==( const CIRMetaData& aMetaData ) const
    {
    TBool ret = EFalse;

    if ( iArtist == aMetaData.Artist() && iSong == aMetaData.Song() && iStreamUrl == aMetaData.StreamUrl() )
        {
        ret = ETrue;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Inequality comparison operator.
// ---------------------------------------------------------------------------
//
TBool CIRMetaData::operator!=( const CIRMetaData& aMetaData ) const
    {
    return !(*this == aMetaData);
    }
