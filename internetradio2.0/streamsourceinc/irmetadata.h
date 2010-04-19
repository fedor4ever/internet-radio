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
* Description:  Meta data wrapper class
*
*/


#ifndef C_CIRMETADATA_H
#define C_CIRMETADATA_H

#include <s32mem.h>

/**
 * Station meta data information wrapper.
 *
 * Supports all ICY protocol specified meta data information and provides
 * internalization and externalization for passing the meta data via binary
 * APIs, such as Publish & Subscribe.
 */
NONSHARABLE_CLASS( CIRMetaData ) : public CBase
    {

public:

    /**
     * Default two-phased constructor.
     *
     * Initializes the object to default values.
     *
     * @return  The created object. Ownership is transferred.
     */
    inline static CIRMetaData* NewL();

    /**
     * Two-phased constructor for direct internalization.
     *
     * Initializes the object by internalizing it from the supplied data.
     *
     * @param   aData   Binary data to be used for internalization.
     * @return  The created object. Ownership is transferred.
     */
    inline static CIRMetaData* NewL( const TDesC8& aData );

    /**
     * Destructor.
     *
     * Frees all allocated resources.
     */
    inline ~CIRMetaData();

    /**
     * Copies values from the supplied meta data object.
     *
     * Resulting object is identical to the supplied one after the call
     * has been successfully completed.
     *
     * @param   aMetaData   Meta data to copy information from.
     */
    inline void SetL( const CIRMetaData& aMetaData );

    /**
     * Sets the artist name.
     *
     * @param   aArtist     The artist name to set.
     */
    inline void SetArtistL( const TDesC& aArtist );

    /**
     * Sets the song name.
     *
     * @param   aSong       The song name to set.
     */
    inline void SetSongL( const TDesC& aSong );

    /**
     * Sets the stream URL.
     *
     * @param   aStreamUrl  The stream URL to set.
     */
    inline void SetStreamUrlL( const TDesC& aStreamUrl );

    /**
     * Returns the artist name.
     *
     * @return  The artist name.
     */
    inline const TDesC& Artist() const;

    /**
     * Returns the song name.
     *
     * @return  The song name.
     */
    inline const TDesC& Song() const;

    /**
     * Returns the stream URL.
     *
     * @return  The stream URL.
     */
    inline const TDesC& StreamUrl() const;

    /**
     * Internalizes the object from the supplied stream.
     *
     * @param   aStream     Stream to internalize the object from.
     */
    inline void InternalizeL( RReadStream& aStream );

    /**
     * Externalizes the object to the supplied stream.
     *
     * @param   aStream     Stream to externalize the object to.
     */
    inline void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Equality comparison operator.
     *
     * Compares two meta data objects and returns whether or not they are identical.
     *
     * @param   aMetaData   Meta data to be used for comparison.
     * @return  <code>ETrue</code> if the objects are identical, <code>EFalse</code> otherwise.
     */
    inline TBool operator==( const CIRMetaData& aMetaData ) const;

    /**
     * Inequality comparison operator.
     *
     * Compares two meta data objects and returns whether or not they are not identical.
     *
     * @param   aMetaData   Meta data to be used for comparison.
     * @return  <code>ETrue</code> if the objects are not identical, <code>EFalse</code> otherwise.
     */
    inline TBool operator!=( const CIRMetaData& aMetaData ) const;

private:

    /**
     * Default constructor.
     */
    inline CIRMetaData();

    /**
     * Second-phase constructor.
     */
    inline void ConstructL();

    /**
     * Second-phase constructor from binary data.
     *
     * @param   aData   Binary data to internalize the object from.
     */
    inline void ConstructL( const TDesC8& aData );

private:

    /**
     * The name of the artist.
     * Owned.
     */
    RBuf iArtist;

    /**
     * The name of the song.
     * Owned.
     */
    RBuf iSong;

    /**
     * The stream URL.
     * Owned.
     */
    RBuf iStreamUrl;

    };

#include "irmetadata.inl"

#endif // C_CIRMETADATA_H
