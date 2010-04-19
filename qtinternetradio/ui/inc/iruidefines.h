/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/
#ifndef IRUIDEFINES_H_
#define IRUIDEFINES_H_

const QColor KListOddRowColor(211, 211, 211, 127);
const QColor KListEvenRowColor(255, 250, 250, 127);
const QColor KListMusicshopDisableColor(200, 0, 0, 127);
const QColor KListMusicshopEnableColor(0,200,0,127);

// Docml File
static const char* ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME     = ":/layout/abstractlistviewbase.docml";
static const char* STATION_DETAILS_CONTAINER_LAYOUT_FILENAME   = ":/layout/stationdetailscontainer.docml";
static const char* NOW_PLAYING_VIEW_LAYOUT_FILENAME            = ":/layout/nowplayingview.docml";
static const char* NOW_PLAYING_CONTAINER_LAYOUT_FILENAME       = ":/layout/nowplayingcontainer.docml";
static const char* OPEN_WEB_ADDRESS_VIEW_LAYOUT_FILENAME       = ":/layout/openwebaddressview.docml";
static const char* SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME        = ":/layout/searchchannelsview.docml";

// IrAbstractListViewBase -- Object Name
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_VIEW                        = "ext-IrAbstractListViewBase";
// menu action
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_OPEN_WEB_ADDRESS_ACTION     = "openWebAddressAction";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_SETTING_ACTION              = "settings";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_HELP_ACTION                 = "help";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_EXIT_ACTION                 = "exit";
// tool bar action
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_TOOLBAR              = "toolbar";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_COLLECTION_ACTION    = "collections";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_FAVORITE_ACTION      = "favorites";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_GENRE_ACTION         = "genres";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_SEARCH_ACTION        = "search";
// widget
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_HEADINGTEXTLABEL     = "headingText";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_PLAYINGBANNER        = "playingBanner";
static const char* ABSTRACT_LIST_VIEW_BASE_OBJECT_LISTVIEW             = "listView";

// IrAbstractListViewBase -- Section Name
static const char* ABSTRACT_LIST_VIEW_BASE_NO_PLAYINGBANNER_SECTION    = "section_no_playingbanner";
static const char* ABSTRACT_LIST_VIEW_BASE_WITH_PLAYINGBANNER_SECTION  = "section_with_playingbanner";

// IRSearchChannelsView -- Object Name
static const char* SEARCH_CHANNELS_VIEW_OBJECT_VIEW                        = "ext-IRSearchChannelsView";
// menu action
static const char* SEARCH_CHANNELS_VIEW_OBJECT_SETTING_ACTION              = "settings";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_OPEN_WEB_ADDRESS_ACTION     = "openWebAddressAction";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_HELP_ACTION                 = "help";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_EXIT_ACTION                 = "exit";
// tool bar action
static const char* SEARCH_CHANNELS_VIEW_OBJECT_TOOLBAR                     = "toolbar";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_COLLECTION_ACTION           = "collections";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_FAVORITES_ACTION            = "favorites";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_GENRES_ACTION               = "genres";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_SEARCH_ACTION               = "search";
// widget
static const char* SEARCH_CHANNELS_VIEW_OBJECT_SEARCHKEY_LABEL             = "searchKey";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_SEARCHKEY_BUTTON            = "searchButton";
static const char* SEARCH_CHANNELS_VIEW_OBJECT_PLAYING_BANNER              = "playingBanner";

// IRSearchChannelsView -- Section Name
static const char* SEARCH_CHANNELS_VIEW_NO_PLAYINGBANNER_SECTION      = "section_no_playingbanner";
static const char* SEARCH_CHANNELS_VIEW_WITH_PLAYINGBANNER_SECTION    = "section_with_playingbanner";


#endif /* IRUIDEFINES_H_ */
