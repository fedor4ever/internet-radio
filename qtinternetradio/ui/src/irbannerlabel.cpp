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

#include <QPainter>

#include "irbannerlabel.h"

IrViewBannerLabel::IrViewBannerLabel(QGraphicsItem *parent) : HbTextItem(parent)
{
}


void IrViewBannerLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor color(138,128,151);
	painter->setPen(color);
	painter->setBrush(color);
	
	QRectF labelRect = rect();
	painter->drawRect(labelRect);
	HbTextItem::paint(painter, option, widget);
}

IrNowPlayingBannerLabel::IrNowPlayingBannerLabel(QGraphicsItem *parent) : HbMarqueeItem(parent)
{
}

void IrNowPlayingBannerLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color(216, 209, 84);
    painter->setPen(color);
    painter->setBrush(color);
    
    QRectF labelRect = rect();
    painter->drawRect(labelRect);
}

void IrNowPlayingBannerLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    emit pressed();
}
