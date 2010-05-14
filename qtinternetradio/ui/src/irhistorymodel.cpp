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

#include <hbicon.h>

#include "irqsonghistoryinfo.h"
#include "irqsonghistoryengine.h"
#include "irhistorymodel.h"

IRHistoryModel::IRHistoryModel(QObject *aParent) : QAbstractListModel(aParent)
{
    iStationLogo = new HbIcon("qtg_large_internet_radio");

    iHistoryEngine = IRQSongHistoryEngine::openInstance();
    getAllList();
}

IRHistoryModel::~IRHistoryModel()
{
    delete iStationLogo;
    iStationLogo = NULL;

    clearAndDestroyLogos();

    while (!iHistoryList.isEmpty())
    {
        delete iHistoryList.takeFirst();
    }
    
    if (iHistoryEngine)
    {
        iHistoryEngine->closeInstance();
        iHistoryEngine = NULL;
    }
}

int IRHistoryModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iHistoryList.count();
}

QString IRHistoryModel::getImageUrl(int aRow) const
{
    return iHistoryList.at(aRow)->getImageUrl();
}

void IRHistoryModel::setLogo(HbIcon *aIcon, int aIndex)
{
    iLogos[aIndex] = aIcon;
    emit dataChanged(index(aIndex), index(aIndex));
}

QVariant IRHistoryModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }

    if (aIndex.row() >= iHistoryList.count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;
        
        int row = aIndex.row();
        QString primaryText = iHistoryList.at(row)->getChannelName();
        list.append(primaryText);
        // fix bug #9888,if left descriptions as blank, only one line appears
        QString tempDes = iHistoryList.at(row)->getChannelDesc();
        if (0 == tempDes.length())
        {
            tempDes = " ";
        }
        list.append(tempDes);
        
        return list;
    }
    else if (aRole == Qt::DecorationRole)
    {
        QVariantList list;
        int row = aIndex.row();
        const HbIcon *icon = iLogos.value(row);
        if (icon)
        {
            list.append(*icon);
        }
        else
        {
            list.append(*iStationLogo);
        }
        
        return list;
    }

    return QVariant();
}

IRQSongHistoryInfo* IRHistoryModel::getHistoryInfo(int aIndex)
{
    if (aIndex >= 0 && aIndex < iHistoryList.count())
    {
        return iHistoryList.at(aIndex);
    }

    return NULL;
}

void IRHistoryModel::clearAllList()
{
    while (!iHistoryList.isEmpty())
    {
        IRQSongHistoryInfo *firstItem = iHistoryList.takeFirst();
        delete firstItem;
    }
    iHistoryEngine->clearAllHistory();
    clearAndDestroyLogos();
    emit modelChanged();
}

bool IRHistoryModel::checkHistoryUpdate()
{
    getAllList();    
    return true;
}

void IRHistoryModel::clearAndDestroyLogos()
{
    for (QMap<int, HbIcon*>::iterator it = iLogos.begin(); it != iLogos.end(); ++it)
    {
        delete it.value();
    }
    
    iLogos.clear();
}

void IRHistoryModel::getAllList()
{
    iHistoryEngine->getAllHistory(iHistoryList);

    emit modelChanged();
}

bool IRHistoryModel::deleteOneItem(int aIndex)
{
    bool ret = iHistoryEngine->deleteOneItem(aIndex);
     
    if( !ret )
    {
        return false;                
    }
    
    beginRemoveRows(QModelIndex(), aIndex, aIndex);
    iHistoryList.removeAt(aIndex);
    endRemoveRows(); 
    
    emit modelChanged();    
    return true;
}
