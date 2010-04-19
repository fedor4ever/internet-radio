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
#ifndef IRBANNERLABEL_H
#define IRBANNERLABEL_H

#include <HbTextItem>
#include <HbMarqueeItem>

class IrViewBannerLabel : public HbTextItem    
{
    Q_OBJECT

public:
    explicit IrViewBannerLabel(QGraphicsItem *parent = 0);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class IrNowPlayingBannerLabel : public HbMarqueeItem
{    
    Q_OBJECT

public:
    IrNowPlayingBannerLabel(QGraphicsItem *parent = 0);
    
signals:
    void pressed();
    
protected:
    //from base class QGraphicsWidget
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
    //from base class QGraphicsItem
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // IRBANNERLABEL_H
