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
#include <hbinstance.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <qtextlayout.h>
#include <QGraphicsLinearLayout>

#include "irqisdsdatastructure.h"
#include "irstationdetailscontainer.h"
#include "iruidefines.h"
#include "irqenums.h"

// Const qreal
const qreal KOneListHeight = 30;
const qreal KBlankHeight = 100;

/*
 * Description : constructor
 */
IRStationDetailsContainer::IRStationDetailsContainer(QObject *aLoaderParent, QGraphicsItem *aParent) :
    HbScrollArea(aParent), iLoader(aLoaderParent)
{
    // Get the display direction
    HbMainWindow *mainWindow = hbInstance->allMainWindows().at(0);
    iPortrait = Qt::Vertical == mainWindow->orientation();

    createDetails();
}

IRStationDetailsContainer::~IRStationDetailsContainer()
{
}

/*
 * Description : update the station information.
 */
void IRStationDetailsContainer::setDetails(IRQPreset* aNowPlayingPreset, const HbIcon& aIcon)
{
    iLogo->setIcon(aIcon);
    iStationName->setText(tr("Name: ") + aNowPlayingPreset->name);
    iDescription->setText(tr("Description: ") + aNowPlayingPreset->description);
    iGenres->setText(tr("Genres: ") + aNowPlayingPreset->genreName);
    iLanguage->setText(tr("Language: ") + aNowPlayingPreset->languageName);
    iCountry->setText(tr("Country/Region: ") + aNowPlayingPreset->countryName);
    if (aNowPlayingPreset->type)
    {
        iType->setText(tr("Type: Station directory"));
    }
    else
    {
        iType->setText(tr("Type: User defined"));
    }

    if (aNowPlayingPreset->getChannelURLCount() > 1)
    {
        aNowPlayingPreset->sortURLArray();
    }

    int bitrate;
    QString bitr;
    QString url;
    QString streamItem;
    int maxIndex = aNowPlayingPreset->getChannelURLCount();

    iStreamsList->clear();

    // add all the urls to the list.
    for (int i = 0; i < maxIndex; i++)
    {
        int error = aNowPlayingPreset->getChannelBitrate(i, bitrate);
        if (EIRQErrorNone == error)
        {
            bitr.setNum(bitrate);
            aNowPlayingPreset->getChannelUrlAt(i, url);
            streamItem = bitr + "kbps: " + url;
            HbListWidgetItem *item = new HbListWidgetItem();
            item->setData(QVariant(streamItem), Qt::DisplayRole);
            iStreamsList->addItem(item);
        }
    }
    resizeContainer();
}

/*
 * Description : create the details of this widget.
 */
void IRStationDetailsContainer::createDetails()
{
    // Load the XML file
    iLoader.load(STATION_DETAILS_CONTAINER_LAYOUT_FILENAME);
    
    // Find the station information objects
    iContainer = qobject_cast<HbWidget *> (iLoader.findObject("container"));
    iLogo = qobject_cast<HbLabel *> (iLoader.findObject("logo"));
    iStationName = qobject_cast<HbTextItem *> (iLoader.findObject("stationName"));
    iDescription = qobject_cast<HbTextItem *> (iLoader.findObject("description"));
    iGenres = qobject_cast<HbTextItem *> (iLoader.findObject("genre"));
    iLanguage = qobject_cast<HbTextItem *> (iLoader.findObject("language"));
    iCountry = qobject_cast<HbTextItem *> (iLoader.findObject("country"));
    iType = qobject_cast<HbTextItem *> (iLoader.findObject("type"));
    iStreamsList = qobject_cast<HbListWidget *> (iLoader.findObject("streamList"));
    // Set the wrap method
    iStationName->setTextWrapping(Hb::TextWordWrap);
    iDescription->setTextWrapping(Hb::TextWordWrap);
    iGenres->setTextWrapping(Hb::TextWordWrap);
    iLanguage->setTextWrapping(Hb::TextWordWrap);
    iCountry->setTextWrapping(Hb::TextWordWrap);
    
    setContentWidget(iContainer);

    // setting this flag prevents the list from scrolling outside it's assigned rectangle
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

    // this sets up the scroll area to scroll in the vertical direction only
    setScrollDirections(Qt::Vertical);

//    setInertiaEnabled(false);

    setClampingStyle(HbScrollArea::StrictClamping);

    resizeWidget();
}

/*
 * Description : update the logo picture.
 */
void IRStationDetailsContainer::setLogo(const HbIcon& aIcon)
{
    iLogo->setIcon(aIcon);
}

/*
 * Description : resize the container if the direction changes.
 */
void IRStationDetailsContainer::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    bool prePortrait = iPortrait;
    HbMainWindow *mainWindow = hbInstance->allMainWindows().at(0);
    iPortrait = Qt::Vertical == mainWindow->orientation();
    if (iPortrait != prePortrait)
    {
        resizeContainer();
    }
    HbWidget::resizeEvent(event);
    if (iPortrait)
    {
        // portrait mode
        iContainer->resize(size());
    }
    else
    {
        // landscape mode
        iContainer->resize(size());
    }
}

/*
 * Description : load the layout according to the direction.
 */
void IRStationDetailsContainer::resizeContainer()
{
    if (iPortrait)
    {
        iLoader.load(STATION_DETAILS_CONTAINER_LAYOUT_FILENAME, "portrait");
    }
    else
    {
        iLoader.load(STATION_DETAILS_CONTAINER_LAYOUT_FILENAME, "landscape");
    }
    resizeWidget();
}

void IRStationDetailsContainer::resizeWidget()
{
    qreal totalHeight=0;
    totalHeight+=iLogo->preferredHeight();
    totalHeight += countWidgetHeight(iStationName);
    totalHeight += countWidgetHeight(iDescription);
    totalHeight += countWidgetHeight(iGenres);
    totalHeight += countWidgetHeight(iLanguage);
    totalHeight += countWidgetHeight(iCountry);
    totalHeight += countWidgetHeight(iType);
    qreal listHeight = KOneListHeight * iStreamsList->count();
    iContainer->setMinimumHeight(totalHeight+listHeight+KBlankHeight);
}

qreal IRStationDetailsContainer::countWidgetHeight( HbTextItem* aTextItem )
{
    QTextLayout mTextLayout;
    mTextLayout.setText(aTextItem->text());
    mTextLayout.setFont(aTextItem->font());
    mTextLayout.beginLayout();
    QFontMetricsF fontMetrics(aTextItem->font());
    qreal height = 0;
    qreal width = iDescription->preferredWidth();
    while(1)
    {
        QTextLine line = mTextLayout.createLine();
        if(!line.isValid())
        {
            break;
        }
        line.setLineWidth( width );
        height += fontMetrics.leading();
        line.setPosition(QPointF(0, height));
        height += line.height();
    }
    mTextLayout.endLayout();
    aTextItem->setPreferredHeight(height);
    return height;
}

void IRStationDetailsContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(Qt::darkGray));
    painter->setBrush(QBrush(Qt::darkGray));
    painter->drawRect(rect());
}
