/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/

#ifndef IRPLUGINCONTENTMODEL_H
#define IRPLUGINCONTENTMODEL_H

#include <aicontentmodel.h>

// Plug-in Implementation UID.
const TInt KImplUidIRPlugin = 0x2000B4A7; 
const TUid KUidIRPlugin = { KImplUidIRPlugin };

// ================================= CONTENT ===================================

/**
 * Content Ids
 */
enum TAiIRContentIds
    {
    EAiIRContentVolume,
    EAiIRContentOneLineInfoCaption, // Full page idle
    EAiIRContentAppIcon,
    EAiIRContentPresetCaption,
    EAiIRContentArtistCaption,
    EAiIRContentTrackCaption
    };

/**
 * Content that the plugin will publish
 */
const TAiContentItem KAiIRContent[] =
    {
    { EAiIRContentVolume,             	L"Volume",       KAiContentTypeText },
    { EAiIRContentOneLineInfoCaption,   L"OneLineInfo",  KAiContentTypeText },
    { EAiIRContentAppIcon,              L"AppIcon",       KAiContentTypeBitmap },
    { EAiIRContentPresetCaption,        L"Preset",       KAiContentTypeText },
    { EAiIRContentArtistCaption,        L"Artist",       KAiContentTypeText },
    { EAiIRContentTrackCaption,   		L"Track",        KAiContentTypeText }
    };


// ================================ RESOURCES ==================================

/**
 * Resource Ids
 */
enum TAiIRPluginResourceIds
    {
    EAiIRResourceVol0,
    EAiIRResourceVol1,
    EAiIRResourceVol2,
    EAiIRResourceVol3,
    EAiIRResourceVol4,
    EAiIRResourceVol5,
    EAiIRResourceVol6,
    EAiIRResourceVol7,
    EAiIRResourceVol8,
    EAiIRResourceVol9,
    EAiIRResourceVol10,
    EAiIRResourceIRPlayingIcon,
    EAiIRResourceIRStoppedIcon
    };

/**
 * Resources that the plugin will publish
 */
const TAiContentItem KAiIRResources[] =
    {
    //UI resources that represents volume levels
    { EAiIRResourceVol0,            L"Vol0",            KAiContentTypeText },
    { EAiIRResourceVol1,            L"Vol1",            KAiContentTypeText },
    { EAiIRResourceVol2,            L"Vol2",            KAiContentTypeText },
    { EAiIRResourceVol3,            L"Vol3",            KAiContentTypeText },
    { EAiIRResourceVol4,            L"Vol4",            KAiContentTypeText },
    { EAiIRResourceVol5,            L"Vol5",            KAiContentTypeText },
    { EAiIRResourceVol6,            L"Vol6",            KAiContentTypeText },
    { EAiIRResourceVol7,            L"Vol7",            KAiContentTypeText },
    { EAiIRResourceVol8,            L"Vol8",            KAiContentTypeText },
    { EAiIRResourceVol9,            L"Vol9",            KAiContentTypeText },
    { EAiIRResourceVol10,           L"Vol10",           KAiContentTypeText },
    { EAiIRResourceIRPlayingIcon,   L"IRPlaying",       KAiContentTypeBitmap },
    { EAiIRResourceIRStoppedIcon,   L"IRStopped",       KAiContentTypeBitmap }
    };


// ============================ SERVICES (Events) ==============================

/**
 * Event Ids.
 */
enum TAiIREventIds
    {
    EAiIREventVolInc,
    EAiIREventVolDec,
    EAiIREventLaunchRadio
    };

/**
 * Services that the plugin can perform
 */
const TAiContentItem KAiIREvents[] =
    {
    { EAiIREventVolInc,      L"VolInc", "" },
    { EAiIREventVolDec,      L"VolDec", "" },
    { EAiIREventLaunchRadio, L"LaunchRadio", "" }
    };


#endif // IRPLUGINCONTENTMODEL_H
