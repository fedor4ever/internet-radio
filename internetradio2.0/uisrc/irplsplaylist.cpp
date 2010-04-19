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


#include <f32file.h>

#include "irplsplaylist.h"
#include "irplsplaylistitem.h"
#include "irdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIRPlsPlayList::CIRPlsPlayList
// ---------------------------------------------------------------------------
//
CIRPlsPlayList::CIRPlsPlayList()
    {
    IRLOG_DEBUG( "CIRPlsPlayList::CIRPlsPlayList" );
    }
    
// ---------------------------------------------------------------------------
// CIRPlsPlayList::ConstructL
// ---------------------------------------------------------------------------
//
void CIRPlsPlayList::ConstructL(RFile& aFileToParse)
    {
    IRLOG_DEBUG( "CIRPlsPlayList::ConstructL - Entering" );
    ParseFileL(aFileToParse);
    IRLOG_DEBUG( "CIRPlsPlayList::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::NewL
// ---------------------------------------------------------------------------
//
CIRPlsPlayList* CIRPlsPlayList::NewL(RFile& aFileToParse)
    {
    IRLOG_DEBUG( "CIRPlsPlayList::NewL - Entering" );
    CIRPlsPlayList* self = CIRPlsPlayList::NewLC(aFileToParse);
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRPlsPlayList::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::NewLC
// ---------------------------------------------------------------------------
//
CIRPlsPlayList* CIRPlsPlayList::NewLC(RFile& aFileToParse)
    {
    IRLOG_DEBUG( "CIRPlsPlayList::NewLC - Entering" );
    CIRPlsPlayList* self = new( ELeave ) CIRPlsPlayList;
    CleanupStack::PushL( self );
    self->ConstructL(aFileToParse);
    IRLOG_DEBUG( "CIRPlsPlayList::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::~CIRPlsPlayList
// ---------------------------------------------------------------------------
//
CIRPlsPlayList::~CIRPlsPlayList()
    {
    IRLOG_DEBUG( "CIRPlsPlayList::~CIRPlsPlayList - Entering" );
    iItems.ResetAndDestroy();
    iItems.Close();	
    iIndexes.Close();
    IRLOG_DEBUG( "CIRPlsPlayList::~CIRPlsPlayList - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::AddItemAt
// ---------------------------------------------------------------------------
//
void CIRPlsPlayList::AddItemAtL( TInt aIndex, CIRPlsPlayListItem* aItem )
    {
    IRLOG_DEBUG( "CIRPlsPlayList::AddItemAtL - Entering" );
    __ASSERT_DEBUG( iItems.Count() == iIndexes.Count(), User::Panic(_L("InternetRadio"), KErrCorrupt));
    
    TInt foundIndex = iIndexes.Find(aIndex);
    if (foundIndex >= 0)
        {
        iItems.Remove(foundIndex);
        //delete item;
        iIndexes.Remove(foundIndex);
        }
    else if (foundIndex != KErrNotFound)
        {
        User::LeaveIfError(foundIndex);
        }
    else
        {
        }
    iIndexes.AppendL(aIndex);
    TRAPD( err, iItems.AppendL(aItem) )
    if (err)
        {
        iIndexes.Remove(iIndexes.Count()-1);
        User::Leave(err);
        }
    IRLOG_DEBUG( "CIRPlsPlayList::AddItemAtL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::RemoveItemAt
// ---------------------------------------------------------------------------
//
void CIRPlsPlayList::RemoveItemAt( TInt aIndex )
    {
    IRLOG_DEBUG( "CIRPlsPlayList::RemoveItemAt - Entering" );
    __ASSERT_DEBUG( iItems.Count() == iIndexes.Count(), User::Panic(_L("InternetRadio"), KErrCorrupt));
    
    TInt foundIndex = iIndexes.Find(aIndex);
    if (foundIndex >= 0)
        {
        iItems.Remove(foundIndex);
        //delete item;
        iIndexes.Remove(foundIndex);
        }
    IRLOG_DEBUG( "CIRPlsPlayList::RemoveItemAt - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::ItemAt
// ---------------------------------------------------------------------------
//
CIRPlsPlayListItem* CIRPlsPlayList::ItemAt( TInt aIndex ) const
    {
    IRLOG_DEBUG( "CIRPlsPlayList::ItemAt - Entering" );
    __ASSERT_DEBUG( iItems.Count() == iIndexes.Count(), User::Panic(_L("InternetRadio"), KErrCorrupt));
    
    CIRPlsPlayListItem* retItem = NULL;
    TInt foundIndex = iIndexes.Find(aIndex);
    if (foundIndex >= 0)
        {
        retItem = iItems[foundIndex];
        }
    IRLOG_DEBUG( "CIRPlsPlayList::ItemAt - Exiting" );
    return retItem;
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::ItemAt
// ---------------------------------------------------------------------------
//
TInt CIRPlsPlayList::Count() const
    {
    IRLOG_DEBUG( "CIRPlsPlayList::Count - Entering" );
    __ASSERT_DEBUG( iItems.Count() == iIndexes.Count(), User::Panic(_L("InternetRadio"), KErrCorrupt));
    IRLOG_DEBUG( "CIRPlsPlayList::Count - Exiting" );
    
    return iItems.Count();
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::ParseFileL
// ---------------------------------------------------------------------------
//
void CIRPlsPlayList::ParseFileL(RFile& aFileToParse)
    {
    IRLOG_DEBUG( "CIRPlsPlayList::ParseFileL - Entering" );
    TBool firstLine(ETrue);
	TBool done(EFalse);
	TInt numberOfEntries(0);
	HBufC8* lineBuf = NULL;
	while ( !done )
	    {
	    TInt ret = KErrNone;
	    ReadLineL( aFileToParse, lineBuf, &ret );
	    if (ret == KErrEof)
	        {
	        // Last line is in the buffer.
	        done = ETrue;
	        }
	    CleanupStack::PushL( lineBuf );
	    TPtr8 line( lineBuf->Des() );
	    
	    if( firstLine )
	        {
	        _LIT8(KIRHeader, "[playlist]");
	        if( line.Compare( KIRHeader ) )
    	        {
    	        User::Leave(KErrCorrupt);
    	        }
	        firstLine = EFalse;
	        }
	    else
	        {
    	    _LIT8(KIRFileLine, "File");
    	    _LIT8(KIRTitleLine, "Title");
    	    _LIT8(KIRLengthLine, "Length");
    	    _LIT8(KIRNumberOfEntries, "NumberOfEntries");
    	    TPtrC8 contentType;
    	    if( line.Find( KIRFileLine ) == 0 )
    	        {
    	        contentType.Set( KIRFileLine );
    	        }
    	    else if( line.Find( KIRTitleLine ) == 0 )
    	        {
    	        contentType.Set( KIRTitleLine );
    	        }
    	    else if( line.Find( KIRLengthLine ) == 0 )
    	        {
    	        contentType.Set( KIRLengthLine );
    	        }
    	    else if( line.FindC( KIRNumberOfEntries ) == 0 )
    	        {
    	        contentType.Set( KIRNumberOfEntries );
    	        }
    	    else
    	        {
    	        contentType.Set( KNullDesC8 );
    	        }
    	    // If found parsable content.
    	    if( contentType.Length() )
    	        {
    	        // Every line must have an "equals" sign.
        	    _LIT8(KIREqualsSign, "=");
        	    TInt equalsSignPos = line.Find( KIREqualsSign );
                if (equalsSignPos == KErrNotFound)
                    {
                    // Sign was not found.
                    User::Leave(KErrCorrupt);
                    }
                // Find the content index of certain content types.
                TInt contentIndex(0);
                if ( (!contentType.Compare( KIRFileLine )) || 
                     (!contentType.Compare( KIRTitleLine )) ||
                     (!contentType.Compare( KIRLengthLine )) )
                    {
            		TLex8 indexLex( line.Mid( contentType.Length(),
            				 equalsSignPos-contentType.Length()) );
            		indexLex.Val( contentIndex );
            		if ( !contentIndex )
                        {
                        // Index couldn't be resolved.
                        User::Leave(KErrCorrupt);
                        }
                    // To convert indexes "1.." of file to "0.." of array.
                    --contentIndex;
                    }
        		TPtrC8 content( line.Mid( equalsSignPos+KIREqualsSign().Length() ) );
        		if( !contentType.Compare( KIRFileLine ) )
        		    {
        		    HBufC* convertedContent = HBufC::NewLC(content.Length());
            		convertedContent->Des().Copy(content);
        		    CIRPlsPlayListItem* item = ItemAt(contentIndex);
        		    if (item)
        		        {
        		        // Update existing item.
            		    item->SetFileL(*convertedContent);
        		        }
        		    else
        		        {
        		        // Create new item.
        		        item = CIRPlsPlayListItem::NewLC();	    
            		    item->SetFileL( *convertedContent );
            		    AddItemAtL( contentIndex, item );
            		    CleanupStack::Pop( item );
        		        }
        		    CleanupStack::PopAndDestroy( convertedContent );
        		    }
        		else if( !contentType.Compare( KIRTitleLine ) )
        		    {
        		    HBufC* convertedContent = HBufC::NewLC(content.Length());
            		convertedContent->Des().Copy(content);
        		    CIRPlsPlayListItem* item = ItemAt(contentIndex);
        		    if (item)
        		        {
        		        // Update existing item.
            		    item->SetTitleL(*convertedContent);
        		        }
        		    else
        		        {
        		        // Create new item.
        		        item = CIRPlsPlayListItem::NewLC();
            		    item->SetTitleL( *convertedContent );
            		    AddItemAtL( contentIndex, item );
            		    CleanupStack::Pop( item );
        		        }
        		    CleanupStack::PopAndDestroy( convertedContent );
        		    }
        		else if( !contentType.Compare( KIRLengthLine ) )
        		    {
        		    TInt lengthVal(0);
        		    TLex8 lengthLex( content );
        		    lengthLex.Val( lengthVal ); 
        		    CIRPlsPlayListItem* item = ItemAt(contentIndex);
        		    if (item)
        		        {
        		        // Update existing item.
            		    item->SetLength(lengthVal);
        		        }
        		    else
        		        {
        		        // Create new item.
        		        item = CIRPlsPlayListItem::NewLC();
            		    item->SetLength(lengthVal);
            		    AddItemAtL( contentIndex, item );
            		    CleanupStack::Pop( item );
        		        }
        		    }
        		else if( !contentType.CompareC( KIRNumberOfEntries ) )
        		    {
        		    TLex8 numberLex( content );
        		    numberLex.Val( numberOfEntries );
        		    }
        		else
        		    {
        		    }
    	        }
	        }
	        
	    CleanupStack::PopAndDestroy(lineBuf);
	    }

    if (Count() != numberOfEntries)
        {
        User::Leave(KErrCorrupt);
        }
    IRLOG_DEBUG( "CIRPlsPlayList::ParseFileL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRPlsPlayList::ReadLineL
// ---------------------------------------------------------------------------
//	
void CIRPlsPlayList::ReadLineL( RFile& aFile, HBufC8*& aLineBuf, TInt *aError ) const
    {
    IRLOG_DEBUG( "CIRPlsPlayList::ReadLineL - Entering" );
    *aError = KErrNone;
    TInt filePos(0);
    User::LeaveIfError( aFile.Seek( ESeekCurrent, filePos ) );
    TInt endLine( KErrNotFound );
    TInt readAmount( 0 );
    const TInt KIRBytesToRead = 64;
    // Find next line end mark index.
    while ( endLine == KErrNotFound )
        {
        // Increase read amount if end of line hasn't been yet found.
        readAmount = readAmount + KIRBytesToRead;
       	HBufC8* nextBuf = HBufC8::NewLC( readAmount );
        TPtr8 next( nextBuf->Des() );
        User::LeaveIfError( aFile.Seek( ESeekStart, filePos ) );
        User::LeaveIfError( aFile.Read( next, readAmount ) );
        // Locate nearest LF and CR.
        TInt lfEnd = next.Locate(EKeyLineFeed);
        TInt crEnd = next.Locate(EKeyEnter);
        if ((lfEnd != KErrNotFound) || (crEnd != KErrNotFound))
            {
            // Either CR or LF was found.
	        if( ((lfEnd != KErrNotFound) && (crEnd != KErrNotFound) && (lfEnd < crEnd)) || 
                (crEnd == KErrNotFound) )
                {
                // LF is nearer.
                if (lfEnd)
                    {
                    // Mark end line position as the position of LF.
                    endLine = lfEnd;
                    }
                else
                    {
                    // LF is at the begining of the line.
                    // Skip it and read file again but not increase read amount.
                    ++filePos;
                    readAmount = readAmount - KIRBytesToRead;
                    }
                }
            else
                {
                // CR is nearer.
                if (crEnd)
                    {
                    // Mark end line position as the position of CR.
                    endLine = crEnd;
                    }
                else
                    {
                    // CR is at the begining of the line.
                    // Skip it and read file again but not increase read amount.
                    ++filePos;
                    readAmount = readAmount - KIRBytesToRead;
                    }
                }
            }
        else
            {
            // No CR or LF was found.
            // Check are we in the end of the file.
            if (next.Length() < readAmount)
                {
                // Reached the end of file.
                endLine = next.Length();
                *aError = KErrEof;
                }
            }
        CleanupStack::PopAndDestroy(nextBuf);
        }
    // Read the line to buf.
    aLineBuf = HBufC8::NewLC( endLine );
    TPtr8 line( aLineBuf->Des() );
    User::LeaveIfError( aFile.Seek( ESeekStart, filePos ) );
    User::LeaveIfError( aFile.Read( line, endLine ) );
    CleanupStack::Pop( aLineBuf );
    IRLOG_DEBUG( "CIRPlsPlayList::ReadLineL - Exiting" );
    }
