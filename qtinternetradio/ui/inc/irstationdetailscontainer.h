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
#ifndef IRSTATIONDETAILSCONTAINER_H_
#define IRSTATIONDETAILSCONTAINER_H_

#include <hbscrollarea.h>
#include "irdocumentloader.h"

class HbLabel;
class HbTextItem;
class HbListWidget;

/**
 * This class is a widget of now playing view. It displays all the
 * information of the channel server.
 */

class IRStationDetailsContainer : public HbScrollArea
{
public:
    explicit IRStationDetailsContainer(QObject *aLoaderParent, QGraphicsItem *aParent = 0);
    ~IRStationDetailsContainer();

    // Methods to set station information
    void setDetails(IRQPreset *aNowPlayingPreset, const HbIcon& aIcon);
    void setLogo(const HbIcon& aIcon);

protected:
    // Inherited method to respond to direction change
    void resizeEvent(QGraphicsSceneResizeEvent *aEvent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void createDetails();
    void resizeContainer();
    void resizeWidget();
    qreal countWidgetHeight(HbTextItem* aTextItem);

private:
    // This loader is for XML layout.
    IRDocumentLoader iLoader;

    // In portrait mode or landscape mode
    bool iPortrait;

    HbWidget* iContainer;

    // Station information
    HbLabel *iLogo;
    HbTextItem *iStationName;
    HbTextItem *iDescription;
    HbTextItem *iGenres;
    HbTextItem *iLanguage;
    HbTextItem *iCountry;
    HbTextItem *iType;
    HbListWidget *iStreamsList;
};

#endif /* IRSTATIONDETAILSCONTAINER_H_ */
