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
* Description:  Describes a PLS playlist.
*
*/


#ifndef C_IRPLSPLAYLIST_H
#define C_IRPLSPLAYLIST_H

#include <e32base.h>

class CIRPlsPlayListItem;
class RFile;

/**
 *  Describes a PLS playlist.
 */
class CIRPlsPlayList : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aFileToParse Open file that is parsed to content of this object.
     */
    static CIRPlsPlayList* NewL(RFile& aFileToParse);
    
    /**
     * Two-phased constructor.
     *
     * @param aFileToParse Open file that is parsed to content of this object.
     */
    static CIRPlsPlayList* NewLC(RFile& aFileToParse);

    /**
    * Destructor.
    */
    ~CIRPlsPlayList();

    /**
     * Adds new item to playlist to the specified index.
     * Takes ownership of aItem.
     * If there exists an item at specified index, it's replaced.
     *
     * @param aIndex Index to add item at.
     * @param aItem The item to be added.
     */
    void AddItemAtL( TInt aIndex, CIRPlsPlayListItem* aItem );
    
    /**
     * Removes item at specified index of this playlist.
     * No-op, if no item was found with the index.
     *
     * @param aIndex Index to removed item at.
     */
    void RemoveItemAt( TInt aIndex );
    
    /**
     * Returns item at specified index in this playlist.
     * NULL is returned if no item found with the index.
     * Ownership of item is not transferred.
     *
     * @param aIndex Index to return item from.
     * @return The item at aIndex, or NULL.
     */
    CIRPlsPlayListItem* ItemAt( TInt aIndex ) const;
    
    /**
     * Returns the number of items in this playlist.
     *
     * @return The number of items.
     */
    TInt Count() const;

private:

    /**
     * Constructor.
     */
    CIRPlsPlayList();

    /**
     * Constructor.
     *
     * @param aFileToParse Open file that is parsed to content of this object.
     */
    void ConstructL(RFile& aFileToParse);
    
    /**
     * Parses a file content into this object.
     *
     * @param aFileToParse Open file that is parsed to content of this object.
     */
    void ParseFileL(RFile& aFileToParse);
    
    /**
	 * Reads a single line of specified file at cursor position. 
	 * The file must be open. Changes the position of cursor, 
	 * so that this enables reading the whole file with sequential 
	 * calls to this function.
	 * Fills the given buffer with content of line. The buffer must
	 * must be NULL when calling this function.
	 * When the last line is read and buffer filled, return value
	 * is KErrEof.
	 * Empty lines are skipped, only lines with content are returned!
	 *
	 * @param aFile The file to read from.
	 * @param aLineBuf The buffer to be filled.
	 * @return KErrNone or KErrEof
	 */
    void ReadLineL( RFile& aFile, HBufC8*& aLineBuf, TInt *aError ) const;

private: // data

    /**
     * Array of items in this list.
     */
    RPointerArray<CIRPlsPlayListItem> iItems;
    
    /**
     * Array of item indexes in this list.
     */
    RArray<TInt> iIndexes;
    };

#endif // C_IRPLSPLAYLIST_H
