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


#ifndef IRCOMMON_H
#define IRCOMMON_H

#include <e32base.h>
namespace
    {

    // ---------------------------------------------------------------------------
    // Performs ResetAndDestroy() and Close() on the supplied object.
    // ---------------------------------------------------------------------------
    //
    template <class T>
    inline void DoCleanupResetAndDestroyClose( TAny* aSelf )
        {
        T* self = static_cast<T*>( aSelf );
        if ( self )
            {
            self->ResetAndDestroy();
            self->Close();
            }
        }

    // ---------------------------------------------------------------------------
    // Pushes an object onto the cleanup stack.
    //
    // Use this only for objects that you want to have ResetAndDestroy() and
    // Close() called upon their destruction.
    // ---------------------------------------------------------------------------
    //
    template <class T>
    inline void CleanupResetAndDestroyClosePushL( T& aObject )
        {
        CleanupStack::PushL( TCleanupItem( DoCleanupResetAndDestroyClose<T>, &aObject ) );
        }

    }

#define KUIDIRAPP 0x2000B499

//violates PC Lint Warning 569: Loss of information
//(initialization) (32 bits to 31 bits)

/*
* Application Uid
*/
static const TUid KUidInternetRadioApp = {KUIDIRAPP};

/** Maximum UID for the views. If some view starts to use this id, this */
/** value must be changed.                                              */ 
const TUid KIRMaximumViewId = { 0x00000010 };

/*
* View IDs
*/
static const TUid KIRTandCViewID = {1};
static const TUid KIRMainChoiceViewID = {2};
static const TUid KIRDiscoverStationsViewID = {3};
static const TUid KIRBrowseByCategoryViewID = {4};
static const TUid KIRStationsViewID = {5};
static const TUid KIRNowPlayingViewID = {6};
static const TUid KIRStationInformationViewID = {7};
static const TUid KIRSavedStationsViewID = {8};
static const TUid KIRSearchViewID = {9};
static const TUid KIRSearchResultsViewID = {10};
static const TUid KIRAddManuallyStationViewID = {11};
static const TUid KIRProgressBarViewID = {12};
static const TUid KIRUpdateWarningViewID = {15};
static const TUid KIRPlsViewID = {17};
static const TUid KIRSettingsViewId = {18};
static const TUid KIRSettingsPopupViewID = {19};
static const TUid KIRAccessPointView = {20};
static const TUid KIRGPRSBitRateView = {21};
static const TUid KIR3GBitRateView = {22};
static const TUid KIRWIFIBitRateView = {23};
static const TUid KIRStationDirectoryViewId = {30};
static const TUid KIRCostWarningViewId={26};
static const TUid KIRCategoryViewId={27};
static const TUid KIRStationsViewId={28};
static const TUid KIRTopStationsViewId={29};
/** UID of the Channel List View */
const TUid KVRChannelListViewId = { 0x00000004 };
const TUid KIRHistoryListViewId = { 24 };
const TUid KIRFirstTimeViewId =   { 0x00000005 };
static const TUid KIRStationInfoViewId = {31};

static const TUid KIRNowPlayingViewCustomMessageId = {106};
static const TUid KHistoryLaunch = {100};
static const TUid KMainViewLaunch = {104};
static const TUid KNowPlayingLaunch = {107};
static const TUid KNowPlayingFrmMainView = {108};


 
_LIT8(KIRNowPlayingViewCustomMessagePlayAfterActivation, "PlayAfterActivation");

/*
* Mif Files
*/
_LIT(KMIFFILE,"InternetRadio.mif");
_LIT(KEDWINSKINFILE,"InternetRadioEdwinSkin.mif");

/*
* File path
*/
_LIT(KTAB,"\t");

/*
* Application name
*/
_LIT(KAPPNAME,"Internet Radio");

/*
* SVG name
*/
_LIT(KSVGLOADING,"qgn_ir_loading.svg");
_LIT(KBUFFERING,"qgn_ir_buffering_anim.svg");

//Different Bitrate ranges
enum TIRBitRateQuality
    {
	EIRStandardQuality,
	EIRHighQuality,
	EIRBestQuality
	};

#endif // IRCOMMON_H