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

#ifndef IRCHANNELMODEL_H
#define IRCHANNELMODEL_H

#include <QAbstractItemModel>

class HbIcon;
class IRQChannelItem;

class IrChannelModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IrChannelModel(QObject *aParent = NULL);
    ~IrChannelModel();

    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    QString imageUrl(int aRow);
    void setLogo(HbIcon *aIcon, int aIndex);

    void clearAndDestroyLogos();
    
public slots:
    void updateData(QList<IRQChannelItem*> *aPushItemsList);

signals:
    void dataAvailable();

private:
    void clearAndDestroyItems();
    
private:
    QList<IRQChannelItem*> *iChannelList;
    QMap<int, HbIcon*>     iLogos;
    HbIcon *iStationLogo;
};

#endif
