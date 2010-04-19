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


#include <aknutils.h>

#include <internetradio.rsg>
#include "irlayoututils.h"

// ---------------------------------------------------------------------------
// IRLayoutUtils::PresetBigLogoSize
// The size for the big logo is calculated from the scren size
// ---------------------------------------------------------------------------
//
TSize IRLayoutUtils::PresetBigLogoSize()
    {
    TSize screenSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize );
    TInt side = Min( screenSize.iWidth, screenSize.iHeight );
    return TSize( side, side );
    }

// ---------------------------------------------------------------------------
// IRLayoutUtils::PresetSmallLogoSizeL
// The size for the small logo is taken from the application settings
// ---------------------------------------------------------------------------
//
/*TSize IRLayoutUtils::PresetSmallLogoSizeL()
    {
    TInt width = 0;
    TInt height = 0;
    CVRRepositoryManager::GetRepositoryValueL( KVRCRUid, KVRCRPresetSmallLogoWidth, width );
    CVRRepositoryManager::GetRepositoryValueL( KVRCRUid, KVRCRPresetSmallLogoHeight, height );
    return TSize( width, height );
    }*/

// ---------------------------------------------------------------------------
// IRLayoutUtils::AdvertisementSizeL
// ---------------------------------------------------------------------------
//
TSize IRLayoutUtils::AdvertisementSizeL()
    {
    TRect parentRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, parentRect );
    
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( parentRect, R_IR_ADVERTISEMENT_SIZE );

    return TSize( layoutRect.Rect().Size() );
    }

