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

#ifndef IRSONGHISTORYMODEL_H_
#define IRSONGHISTORYMODEL_H_

#include <QAbstractItemModel>

class IRQSongHistoryEngine;
class IRQSongInfo;
class IRQMetaData;

class IRSongHistoryModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit IRSongHistoryModel(QObject *aParent = NULL);
    ~IRSongHistoryModel();
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    
    IRQSongInfo* getSongHistoryInfo(int aIndex);
    void clearAllList();
    bool checkSongHistoryUpdate();   
    void clearHisotrySongDB();  
    
signals:
    void modelChanged();   

private:
    void getAllList();

private:
    QList<IRQSongInfo *> iSongHistoryList;
    IRQSongHistoryEngine *iHistoryEngine;  
};

#endif 
