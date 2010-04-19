/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Visual Radio central repository keys.
*
*/


#ifndef IRINTERNALCRKEYS_H
#define IRINTERNALCRKEYS_H

#include <e32cmn.h>

/** The UID of the category we use. Must be the same as Visual Radio's UID3 (SID). */
const TUid KCRUidInternetRadio = { 0x2000B499};


const TUint32 KIRGprsBitRate = { 0x00000001};
const TUint32 KIR3GBitRate = { 0x00000002};
const TUint32 KIRWifiBitRate = { 0x00000003};
const TUint32 KIRDefaultAccessPoint = { 0x00000004 };
const TUint32 KIRUserDefinedAccessPoint = { 0x00000005 };
const TUint32 KIRUserDefinedBearerID = { 0x00000006	};
const TUint32 KIRUserDefinedNetworkID = { 0x00000007 };
const TUint32 KIRDefaultMaxPresetCount = { 0x00000008 };
const TUint32 KIRStereoMode = { 0x00000009 };
const TUint32 KIRDefaultPlayVolume = { 0x00000010 };
const TUint32 KIRCostWarningFlag = { 0x00000011 };
const TUint32 KIRTermsAndConditionsFlag = { 0x00000012 };
const TUint32 KIRUpdateAvailablity = { 0x00000013 };
const TUint32 KIRTimeOutDuration = { 0x00000014 };
const TUint32 KIRSilencePeriodDuration = { 0x00000015 };
const TUint32 KIRSilencePeriodStartingTime = { 0x00000016 };
const TUint32 KIRDefaultIsdsUrl = { 0x00000017 };
const TUint32 KIRDefaultIrId = { 0x00000018 };
const TUint32 KIRIrappVer = { 0x00000019 };
const TUint32 KIRDefaultTimeCorrection = { 0x00000020 };
const TUint32 KIRUserDefinedSubscriberId = { 0x00000021 };
const TUint32 KIRUserDefinedLogRunningNumber = { 0x00000022 };
const TUint32 KIRUserDefinedRunningChannelId = { 0x00000023 };
//added for ALR/SNAP
const TUint32 KIRUserDefinedSelection = { 0x00000024 };
const TUint32 KIRUserDefinedDestinationId = {0x00000025 };

const TUint32 KIRIfAnyFavFlag = {0x00000026 };

//added for starting view
const TUint32 KIRStartingViewId = {0x00000027};
const TUint32 KIRStartingViewParameter = {0x00000028};
const TUint32 KIRStickyViewFlag = {0x00000029};
const TUint32 KIRPreferredQuality = {0x00000030};

//added for songhistory show enable
const TUint32 KIRSongHistoryShow = {0x00000031};

//added for global advertisement
const TUint32 KIRGlobalAdvFlag = {0x00000032};
#endif 
