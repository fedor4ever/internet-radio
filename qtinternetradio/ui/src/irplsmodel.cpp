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

#include <QBrush>

#include "iruidefines.h"
#include "irplsmodel.h"
#include "irsymbiandocument.h"


IRPlsModel::IRPlsModel(QObject *aParent) : QAbstractListModel(aParent), iDocument(NULL)
{
    iDocument = IRSymbianDocument::getInstance();
}

int IRPlsModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iDocument->getNumberOfEntries();
}

QVariant IRPlsModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
        return QVariant();

    if (aIndex.row() >= rowCount(QModelIndex()))
        return QVariant();
    
    if (aRole == Qt::DisplayRole)
    {
        int row = aIndex.row();
        QVariantList list;
        IRPlsPlayListItem *item = iDocument->getEntry(row);
        list.append(item->title());
        list.append(item->file());
        return list;
    }
    else if (aRole == Qt::BackgroundRole)
    {
        if (aIndex.row() % 2 == 0)
        {
            return QBrush(KListEvenRowColor);
        }
        else
        {
            return QBrush(KListOddRowColor);
        }
    }
    
    return QVariant();
}

const QString& IRPlsModel::getFile(int aIndex) const
{
    IRPlsPlayListItem *item = iDocument->getEntry(aIndex);
    return item->file();
}

const QString& IRPlsModel::getTitle(int aIndex) const
{
    IRPlsPlayListItem *item = iDocument->getEntry(aIndex);
    return item->title();
}
