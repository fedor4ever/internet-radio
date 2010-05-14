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

const TUint32 KIRDefaultMaxPresetCount = { 0x00000008 };
const TUint32 KIRDefaultPlayVolume = { 0x00000010 };
const TUint32 KIRTermsAndConditionsFlag = { 0x00000012 };
const TUint32 KIRUpdateAvailablity = { 0x00000013 };
const TUint32 KIRTimeOutDuration = { 0x00000014 };
const TUint32 KIRSilencePeriodDuration = { 0x00000015 };
const TUint32 KIRSilencePeriodStartingTime = { 0x00000016 };
const TUint32 KIRDefaultIsdsUrl = { 0x00000017 };
const TUint32 KIRDefaultIrId = { 0x00000018 };
const TUint32 KIRIrappVer = { 0x00000019 };
const TUint32 KIRDefaultTimeCorrection = { 0x00000020 };
const TUint32 KIRUserDefinedLogRunningNumber = { 0x00000022 };

//added for starting view
const TUint32 KIRStartingViewId = {0x00000027};

const TUint32 KIRPreferredQuality = {0x00000030};

//added for songhistory show enable
const TUint32 KIRSongHistoryShow = {0x00000031};

//added for global advertisement
const TUint32 KIRGlobalAdvFlag = {0x00000032};

//added for Go to station view
const TUint32 KIRStationUrl = { 0x00000033 };
const TUint32 KIRStationName = { 0x00000034 };
#endif 
