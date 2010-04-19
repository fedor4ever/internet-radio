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
#ifndef IRNOWPLAYINGCONTAINER_H
#define IRNOWPLAYINGCONTAINER_H

#include <hbwidget.h>
#include "irdocumentloader.h"

class HbTextItem;
class HbLabel;
class HbMarqueeItem;

/**
 * This class is a widget of now playing view. It displays the
 * main information of the channel server.
 */

class IRNowPlayingContainer : public HbWidget
{
    Q_OBJECT

public:
    explicit IRNowPlayingContainer(QObject *aLoaderParent, QGraphicsItem *aParent = 0);
    ~IRNowPlayingContainer();

    // Methods to set station information
    void setStationName(const QString &aStationName);
    void setCategory(const QString &aCategory);
    void setSongName(const QString &aSongName);
    void setLogo(const HbIcon& aLogo);
    void setAdvImage(const HbIcon& aAdvImage);

signals:
    // This signal will cause the popup of volume slider control
    void volumePressed(void);
    // This signal will cause a browser open a link to the adv.
    void advertisementPressed(void);

protected:
    // Inherited methods
    void mousePressEvent(QGraphicsSceneMouseEvent *aEvent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void orientationChanged(Qt::Orientation aOrientation);

private:
    void constructContainer();
    void resizeContainer(Qt::Orientation aOrientation);

private:
    // This loader is for XML layout.
    IRDocumentLoader iLoader;

    // Station information
    HbTextItem *iStationName;
    HbTextItem *iCategory;
    HbMarqueeItem *iSongName;
    HbLabel *iAdvImage;
    HbLabel *iLogo;
};

#endif   //IRNOWPLAYINGCONTAINER_H
