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

#include "irqisdsdatastructure.h"
#include "iruidefines.h"
#include "ircategorymodel.h"


IRCategoryModel::IRCategoryModel(QObject *aParent) : QAbstractListModel(aParent), iItems(NULL)
{
}

IRCategoryModel::~IRCategoryModel()
{
    destroyAllItems();
    delete iItems;
    iItems = NULL;
}

int IRCategoryModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    
    if (iItems)
    {
        int count = iItems->count();
        return count;
    }
    
    return 0;
}

QVariant IRCategoryModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
        return QVariant();

    if (aIndex.row() >= iItems->count())
        return QVariant();

    if (aRole == Qt::DisplayRole)
    {
        int row = aIndex.row();
        
        QString catNum;
        catNum.setNum(iItems->at(row)->size);
        QString secondaryText = catNum + QString(" Stations");

        QVariantList list;
        list.append(iItems->at(row)->catName);
        list.append(secondaryText);
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

const QString& IRCategoryModel::categoryName(const QModelIndex &aIndex) const
{
    int row = aIndex.row();
    Q_ASSERT(row >= 0 && row < iItems->count());

    return iItems->at(row)->catName;
}

void IRCategoryModel::categoryReceived(QList<IRQBrowseCategoryItem*> *aItems)
{
    destroyAllItems();
    delete iItems;
    
    iItems = aItems;

    emit dataChanged();
}

void IRCategoryModel::destroyAllItems()
{
    if (iItems)
    {
        while (!iItems->isEmpty())
        {
            IRQBrowseCategoryItem *firstItem = iItems->takeFirst();
            delete firstItem;
        }
    }
}
