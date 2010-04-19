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


#ifndef C_IRPLSPLAYLISTITEM_H
#define C_IRPLSPLAYLISTITEM_H

#include <e32base.h>

/**
 *  Describes a single item in PLS playlist.
 *  The item consists of File, Title and Length attributes.
 */
class CIRPlsPlayListItem : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CIRPlsPlayListItem* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CIRPlsPlayListItem* NewLC();

    /**
    * Destructor.
    */
    ~CIRPlsPlayListItem();

    /**
     * Sets new File attribute. The old attribute is replaced.
     *
     * @param aFile New File attribute.
     */
    void SetFileL( const TDesC& aFile );
    
    /**
     * Sets new Title attribute. The old attribute is replaced.
     *
     * @param aTitle New Title attribute.
     */
    void SetTitleL( const TDesC& aTitle );
    
    /**
     * Sets new Length attribute. The old attribute is replaced.
     *
     * @param aLength New Length attribute.
     */
    void SetLength( TInt aLength );
    
    /**
     * Gets the File attribute.
     *
     * @return The File attribute.
     */
    const TDesC& File() const;
    
    /**
     * Gets the Title attribute.
     *
     * @return The Title attribute.
     */
    const TDesC& Title() const;
    
    /**
     * Gets the Length attribute.
     *
     * @return The Length attribute.
     */
    TInt Length() const;

private:

    /**
     * Constructor.
     */
    CIRPlsPlayListItem();

 
private: // data

    /**
     * "File" of this item.
     */
    RBuf iFile;
    
    /**
     * "Title" of this item.
     */
    RBuf iTitle;
    
    /**
     * "Length" of this item.
     */
    TInt iLength;
    };

#endif // C_IRPLSPLAYLISTITEM_H
