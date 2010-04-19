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
#ifndef IRHISTORYMODEL_H_
#define IRHISTORYMODEL_H_

#include <QAbstractItemModel>

class IRQSongHistoryEngine;
class IRQSongHistoryInfo;
class HbIcon;

class IRHistoryModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit IRHistoryModel(QObject *aParent = NULL);
    ~IRHistoryModel();
    
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    
    IRQSongHistoryInfo* getHistoryInfo(int aIndex);
    void clearAllList();
    bool checkHistoryUpdate();
    void clearAndDestroyLogos();
    QString getImageUrl(int aRow) const;
    void setLogo(HbIcon *aIcon, int aIndex);
    bool deleteOneItem(int aIndex);
    
signals:
    void modelChanged();   

private:
    void getAllList();

private:
    QList<IRQSongHistoryInfo *> iHistoryList;
    IRQSongHistoryEngine *iHistoryEngine;
    QMap<int, HbIcon*>     iLogos;
    HbIcon *iStationLogo;
};

#endif /* IRHISTORYMODEL_H_ */
