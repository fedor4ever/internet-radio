/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utilities for layouting purposes
*
*/


#ifndef IRLAYOUTUTILS_H
#define IRLAYOUTUTILS_H

#include <e32cmn.h>

/**
 *  Utilities for layouting purposes
 */
class IRLayoutUtils
    {
public:

    /**
     * Returns the preferred size for the big logo stored for the preset
     * 
     * @return The size
     */
    static TSize PresetBigLogoSize();
    
    /**
     * Returns the preferred size for the small logo stored for the preset
     * 
     * @return The size
     */
   // static TSize PresetSmallLogoSizeL()
    
    /**
     * Returns the preferred size for the advertisement in SDS view.
     * 
     * @return The size
     */
    static TSize AdvertisementSizeL();
    };


#endif // IRLAYOUTUTILS_H
