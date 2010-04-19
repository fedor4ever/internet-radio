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

#include "irqsonghistoryinfo.h"
#include "irqsonghistoryengine.h"
#include "irsonghistorymodel.h"
#include "iruidefines.h"

IRSongHistoryModel::IRSongHistoryModel(QObject *aParent) : QAbstractListModel(aParent)
{     
    iHistoryEngine = IRQSongHistoryEngine::openInstance();
    getAllList();
}

IRSongHistoryModel::~IRSongHistoryModel()
{
    while (!iSongHistoryList.isEmpty())
    {
        delete iSongHistoryList.takeFirst();
    }
    
    if (iHistoryEngine)
    {
        iHistoryEngine->closeInstance();
        iHistoryEngine = NULL;
    }
}

int IRSongHistoryModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iSongHistoryList.count();
}
 
 
QVariant IRSongHistoryModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }
    
    int row = aIndex.row();

    if (aIndex.row() >= iSongHistoryList.count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;      
        
        QString songName = iSongHistoryList.at(row)->getSongName();
        songName = songName.trimmed();
        if( "" == songName )
        {
            songName = hbTrId("txt_irad_list_unknown_song");
        }
        
        songName = QString::number(row+1) + ". " + "\" " + songName + " \"";        
        list.append(songName);
        
        QString artistName;
        artistName = iSongHistoryList.at(row)->getArtistName();
        artistName = artistName.trimmed();
        if( "" == artistName )
        {
            artistName = "< " + hbTrId("txt_irad_list_unknown_artist") + " >";
        }
        else
        {
            artistName = "< " + iSongHistoryList.at(row)->getArtistName() + " >";
        }
        list.append(artistName);
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

IRQSongInfo* IRSongHistoryModel::getSongHistoryInfo(int aIndex)
{
    if (aIndex >= 0 && aIndex < iSongHistoryList.count())
    {
        return iSongHistoryList.at(aIndex);
    }

    return NULL;
}

void IRSongHistoryModel::clearAllList()
{
    while (!iSongHistoryList.isEmpty())
    {
        IRQSongInfo *firstItem = iSongHistoryList.takeFirst();
        delete firstItem;
    }
     
    emit modelChanged();
}

bool IRSongHistoryModel::checkSongHistoryUpdate()
{     
    getAllList();    
    return true;
}
 

void IRSongHistoryModel::getAllList()
{
    iHistoryEngine->getAllSongHistory(iSongHistoryList);

    emit modelChanged();
} 

void IRSongHistoryModel::clearHisotrySongDB()
{
    while (!iSongHistoryList.isEmpty())
    {
        IRQSongInfo *firstItem = iSongHistoryList.takeFirst();
        delete firstItem;
    }
    iHistoryEngine->clearAllSongHistory();     
    emit modelChanged();
}
