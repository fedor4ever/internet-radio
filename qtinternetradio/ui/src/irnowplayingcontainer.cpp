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
#include <HbTextItem>
#include <HbLabel>
#include <hbeffect.h>
#include <hbinstance.h>
#include <QGraphicsSceneEvent>
#include <hbmarqueeitem.h>
#include <QTimer>

#include "irnowplayingcontainer.h"
#include "iruidefines.h"


// Const strings
const char* NOW_PLAYING_CONTAINER_OBJECT_NAME = "ex-IRNowPlayingContainer"; // object name in the XML

/*
 * Description : constructor
 */
IRNowPlayingContainer::IRNowPlayingContainer(QObject *aLoaderParent, QGraphicsItem *aParent) :
    HbWidget(aParent), iLoader(aLoaderParent), iStationName(NULL),
    iCategory(NULL),
    iSongName(NULL),
    iAdvImage(NULL),
    iLogo(NULL)
{
    // Create widget hierarchy
    setObjectName(NOW_PLAYING_CONTAINER_OBJECT_NAME);

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append(this); // IRNowPlayingContainer is referred in the XML document
    iLoader.setObjectTree(roots);

    // Load the XML file
    iLoader.load(NOW_PLAYING_CONTAINER_LAYOUT_FILENAME);

    // Find the HbLabel objects
    iStationName = qobject_cast<HbTextItem *> (iLoader.findObject("stationName"));
    iStationName->setMaximumLines(2);
    iCategory = qobject_cast<HbTextItem *> (iLoader.findObject("category"));
    iCategory->setMaximumLines(2);
    iSongName = qobject_cast<HbMarqueeItem *> (iLoader.findObject("artistNsongName"));
    iAdvImage = qobject_cast<HbLabel *> (iLoader.findObject("advertisementImage"));
    iLogo = qobject_cast<HbLabel *> (iLoader.findObject("stationIcon"));

    // Set the wrap method
    iStationName->setTextWrapping(Hb::TextWordWrap);
    iCategory->setTextWrapping(Hb::TextWordWrap);
    iSongName->setLoopCount(-1);

    // Get the display direction
    constructContainer();
}

IRNowPlayingContainer::~IRNowPlayingContainer()
{
}

/*
 * Description : construct the container widget.
 */
void IRNowPlayingContainer::constructContainer()
{
    // Set the text font. It may be deleted in the future to use the default
    // font.
    QFont playingFont;
    playingFont.setPointSizeF(7.0);
    playingFont.setBold(true);
    iStationName->setFont(playingFont);
    playingFont.setPointSizeF(6.0);
    playingFont.setBold(false);
    iCategory->setFont(playingFont);
    iSongName->setFont(playingFont);

    HbMainWindow *mainWindow = hbInstance->allMainWindows().at(0);
    resizeContainer(mainWindow->orientation());
}

/*
 * Description : set the station name text to the station label
 * Parameters  : aStationName : station's name
 */
void IRNowPlayingContainer::setStationName(const QString &aStationName)
{
    iStationName->setText(aStationName);
}

/*
 * Description : set the category text to the category label
 * Parameters  : aCategory : station's category
 */
void IRNowPlayingContainer::setCategory(const QString &aCategory)
{
    iCategory->setText(aCategory);
}

/*
 * Description : set the song name text to the song name label
 * Parameters  : aSongName : song's name
 */
void IRNowPlayingContainer::setSongName(const QString &aSongName)
{
    iSongName->setText(aSongName);
    if ( !iSongName->isAnimating() )
    {
        QTimer::singleShot(100, iSongName, SLOT(startAnimation()));
    }
}

/*
 * Description : set the logo picture to the logo label
 * Parameters  : aLogo : logo picture
 */
void IRNowPlayingContainer::setLogo(const HbIcon& aLogo)
{
    iLogo->setIcon(aLogo);
    HbEffect::start(iLogo, "nowplaying", "show");
}

/*
 * Description : set the advertisment picture
 * Parameters  : aAdvImage : advertisement picture
 */
void IRNowPlayingContainer::setAdvImage(const HbIcon& aAdvImage)
{
    iAdvImage->setIcon(aAdvImage);
    HbEffect::start(iAdvImage, "nowplaying", "show");
}

/*
 * Description : get the mouse press event and emit a signal.
 */
void IRNowPlayingContainer::mousePressEvent(QGraphicsSceneMouseEvent *aEvent)
{
    QRectF advRect = iAdvImage->geometry();
    QPointF pos = aEvent->pos();
    
    if (advRect.contains(pos))
    {
        emit advertisementPressed();
    }
    else
    {
        emit volumePressed();   
    }
}

/*
 * Description : resize the container if the direction changes.
 */
void IRNowPlayingContainer::orientationChanged(Qt::Orientation aOrientation)
{
    resizeContainer(aOrientation);
}

/*
 * Description : load the layout according to the direction.
 */
void IRNowPlayingContainer::resizeContainer(Qt::Orientation aOrientation)
{
    if (aOrientation == Qt::Vertical)
    {
        iLoader.load(NOW_PLAYING_CONTAINER_LAYOUT_FILENAME, "portrait");
    }
    else
    {
        iLoader.load(NOW_PLAYING_CONTAINER_LAYOUT_FILENAME, "landscape");
    }
}

void IRNowPlayingContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::darkGray));
    painter->setBrush(QBrush(Qt::darkGray));
    painter->drawRect(rect());
}
