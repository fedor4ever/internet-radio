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
#include <hbglobal.h>

#include "iruidefines.h"
#include "irmainmodel.h"
#include "irsymbiandocument.h"

IRMainModel::IRMainModel(QObject *aParent) : QAbstractListModel(aParent)
{
    initModel();
}

int IRMainModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iPrimaryTexts.count();
}

QVariant IRMainModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
        return QVariant();

    if (aIndex.row() >= rowCount(QModelIndex()))
        return QVariant();
    
    if (aRole == Qt::DisplayRole)
    {
        int row = aIndex.row();
        QVariantList list;
        list.append(iPrimaryTexts.at(row));
        list.append(iSecondaryTexts.at(row));
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

void IRMainModel::initModel()
{
    iPrimaryTexts << hbTrId("txt_irad_list_recently_played_stations") 
                  << hbTrId("txt_irad_list_stations_by_country_region")
                  << hbTrId("txt_irad_list_stations_by_language")
                  << hbTrId("txt_irad_list_recently_played_songs");
    iSecondaryTexts << tr("BBC World Service, Cloud FM ...") << tr("US, UK, France, Korea, Mexico ...") 
                    << tr("English, French, German...")
                    << tr("Recently played songs");
}
void IRMainModel::checkUpdate()
{
    IRSymbianDocument *document = IRSymbianDocument::getInstance();
    QString primary = hbTrId("txt_irad_subtitle_stations_from_play_list");     
    QString secondary = tr("Stations from play list file");
    if (document->isPlsAvailable())
    {
        //if pls item is not in collections view, add
        if (iPrimaryTexts.indexOf(primary) == -1)
        {
            int number = iPrimaryTexts.count();
            beginInsertRows(QModelIndex(), number, number);
            iPrimaryTexts << primary;
            iSecondaryTexts << secondary;
            endInsertRows();
        }
    }
    else
    {
        //if pls item is in collections view, remove
        int index = iPrimaryTexts.indexOf(primary);
        if (index != -1)
        {
            beginRemoveRows(QModelIndex(), index, index);
            iPrimaryTexts.removeAt(index);
            iSecondaryTexts.removeAt(index);
            endRemoveRows();
        }
    }
}
