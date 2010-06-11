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
#include <hbglobal.h>

#include "irqsonghistoryinfo.h"
#include "irqsonghistoryengine.h"
#include "irsonghistorymodel.h"

IRSongHistoryModel::IRSongHistoryModel(QObject *aParent) : QAbstractListModel(aParent)
{     
    mHistoryEngine = IRQSongHistoryEngine::openInstance();
    getAllList();
}

IRSongHistoryModel::~IRSongHistoryModel()
{
    while (!mSongHistoryList.isEmpty())
    {
        delete mSongHistoryList.takeFirst();
    }
    
    if (mHistoryEngine)
    {
        mHistoryEngine->closeInstance();
        mHistoryEngine = NULL;
    }
}

int IRSongHistoryModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return mSongHistoryList.count();
}
 
 
QVariant IRSongHistoryModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }
    
    int row = aIndex.row();

    if (aIndex.row() >= mSongHistoryList.count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;      
             
        QString artistName = mSongHistoryList.at(row)->getArtistName().trimmed();
        if( "" == artistName )
        {
            artistName = hbTrId("txt_irad_list_unknown_artist");
        }
      
        QString songName = mSongHistoryList.at(row)->getSongName().trimmed();
        if( "" == songName )
        {
            songName = hbTrId("txt_irad_list_unknown_song");
        }
        
        QString stationName = mSongHistoryList.at(row)->getStationName().trimmed();       
        if (Qt::Vertical == mOrientation)
        {
            list.append(artistName);
            list.append(songName);
        }
        else
        {
            list.append(artistName + " - " + songName);
            list.append(stationName);
        }
        return list;
    }

    return QVariant();
}

IRQSongInfo* IRSongHistoryModel::getSongHistoryInfo(int aIndex)
{
    if (aIndex >= 0 && aIndex < mSongHistoryList.count())
    {
        return mSongHistoryList.at(aIndex);
    }

    return NULL;
}

void IRSongHistoryModel::clearList()
{
    while (!mSongHistoryList.isEmpty())
    {
        IRQSongInfo *firstItem = mSongHistoryList.takeFirst();
        delete firstItem;
    }

    mHistoryEngine->clearAllSongHistory();

    emit modelChanged();
}

bool IRSongHistoryModel::checkSongHistoryUpdate()
{     
    getAllList();    
    return true;
}
 

void IRSongHistoryModel::getAllList()
{
    mHistoryEngine->getAllSongHistory(mSongHistoryList);

    emit modelChanged();
}

void IRSongHistoryModel::setOrientation(Qt::Orientation aOrientation)
{
    mOrientation = aOrientation;
}

bool IRSongHistoryModel::deleteOneItem(int aIndex)
{
    bool ret = mHistoryEngine->deleteOneSongHistoryItem(aIndex);

    if( !ret )
    {
        return false;                
    }
    
    beginRemoveRows(QModelIndex(), aIndex, aIndex);
    mSongHistoryList.removeAt(aIndex);
    endRemoveRows(); 
    
    emit modelChanged();    
    return true;
}
