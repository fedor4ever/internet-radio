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

#include <HbLabel>
#include <hbaction.h>
#include "irapplication.h"
#include "irstationdetailsview.h"
#include "iruidefines.h"
#include "irqmetadata.h"
#include "irqisdsdatastructure.h"
#include "irqenums.h"
#include "irplaycontroller.h"
#include "irviewmanager.h"
#include "irqsonghistoryinfo.h"
IRStationDetailsView::IRStationDetailsView(IRApplication* aApplication, TIRViewId aViewId) : IRBaseView(aApplication, aViewId),
                                    iScrollArea(NULL),iContainer(NULL),
                                    iStationName(NULL),iDescription(NULL),
                                    iGenres(NULL),iLanguage(NULL),
                                    iCountry(NULL),iBitRate(NULL),
                                    iNowPlayingUrl(NULL)
{
    //this view won't be starting view, don't need lazy init
    IRBaseView::lazyInit();
    setInitCompleted(true);
    
    createDetails();
    connect( getViewManager(), SIGNAL( orientationChanged(Qt::Orientation) ),
             this, SLOT( handleOrientationChanged(Qt::Orientation) ) );
    handleOrientationChanged(getViewManager()->orientation());                 
}

IRStationDetailsView::~IRStationDetailsView()
{
}

/*
 * Description : update the station information.
 */
void IRStationDetailsView::setDetails(IRQSongHistoryInfo *aCurChannelInfo)
{
    iStationName->setPlainText(aCurChannelInfo->getChannelName());
    iGenres->setPlainText(QString(""));
    iLanguage->setPlainText(QString(""));
    iCountry->setPlainText(QString(""));
    iBitRate->setPlainText(hbTrId("txt_irad_setlabel_bit_rate") + QString(" : ") + QString::number(0) + QString("kbps"));
    iDescription->setPlainText(aCurChannelInfo->getChannelDesc());
    iNowPlayingUrl->setPlainText(aCurChannelInfo->getStreamUrl());
}

/*
 * Description : update the station information.
 */
void IRStationDetailsView::setDetails()
{
    IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
    iStationName->setPlainText(nowPlayingPreset->name);
    iGenres->setPlainText(nowPlayingPreset->genreName);
    iLanguage->setPlainText(nowPlayingPreset->languageName);
    iCountry->setPlainText(nowPlayingPreset->countryName);
    iBitRate->setPlainText(hbTrId("txt_irad_setlabel_bit_rate") + QString(" : ") + QString::number(iPlayController->getNowPlayingBitRate()) + QString("kbps"));
    iDescription->setPlainText(nowPlayingPreset->description);
    iNowPlayingUrl->setPlainText(iPlayController->getNowPlayingUrl());
}

/*
 * Description : create the details of this widget.
 */
void IRStationDetailsView::createDetails()
{
    setObjectName(STATION_DETAILS_VIEW_OBJECT_NAME);
    QObjectList roots;
    roots.append(this);
    iLoader.setObjectTree(roots);
    iLoader.load(STATION_DETAILS_VIEW_LAYOUT_FILENAME);
    
    HbAction *exitAction = qobject_cast<HbAction *> (iLoader.findObject(EXIT_ACTION));
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit())); 
    
    // Find the station information objects
    iScrollArea = qobject_cast<HbScrollArea *>(iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_SCROLL_AREA));
    iContainer = qobject_cast<HbWidget *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_SCROLLAREA_CONTENTS));
    iStationName = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_STATION_NAME));
    iGenres = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_GENRE));
    iLanguage = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_LANGUAGE));
    iCountry = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_COUNTRY));
    iBitRate = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_BITE_RATE));
    iDescription = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_DESCRIPTION));    
    iNowPlayingUrl = qobject_cast<HbLabel *> (iLoader.findObject(STATION_DETAILS_VIEW_OBJECT_STATION_URL));
}


/*
 * Description : load the layout according to the direction.
 */
void IRStationDetailsView::handleOrientationChanged(Qt::Orientation aOrientation)
{
    if (aOrientation == Qt::Vertical)
    {
        iLoader.load(STATION_DETAILS_VIEW_LAYOUT_FILENAME, "portrait");
    }
    else
    {
        iLoader.load(STATION_DETAILS_VIEW_LAYOUT_FILENAME, "landscape");
    }
}

