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

#include "irqfavoritesdb.h"
#include "irqisdsdatastructure.h"
#include "irfavoritesmodel.h"

IRFavoritesModel::IRFavoritesModel(IRQFavoritesDB *aFavoritesDb, QObject *aParent) 
                                  : QAbstractListModel(aParent), iFavoritesDb(aFavoritesDb),
                                    iPresetsList(NULL)
{
    if (aFavoritesDb)
    {
        iPresetsList = aFavoritesDb->getPresets();
    }
    
    iStationLogo = new HbIcon("qtg_large_internet_radio");
}

IRFavoritesModel::~IRFavoritesModel()
{
    delete iStationLogo;
    clearPresetList();
    clearAndDestroyLogos();
}

IRQPreset* IRFavoritesModel::getPreset(int aIndex) const
{
    if (iPresetsList && aIndex >= 0 && aIndex < iPresetsList->count())
    {
        return iPresetsList->at(aIndex);
    }
    
    return NULL;
}

QString  IRFavoritesModel::getImgUrl(int aIndex) const
{
    if ( aIndex<0 || aIndex >=iPresetsList->count() )
    {
        return "";
    }
    return iPresetsList->at(aIndex)->imgUrl;     
}

int IRFavoritesModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    if (iPresetsList)
    {
        int count = iPresetsList->count();
        return count;
    }
    
    return 0;
}

void IRFavoritesModel::setLogo(HbIcon *aIcon, int aIndex)
{
    int elementCountNeedToAdd = aIndex + 1 - iLogos.size();
    while (elementCountNeedToAdd > 0)
    {
        iLogos.append(NULL);
        elementCountNeedToAdd--;
    }
    iLogos[aIndex] = aIcon;
    emit dataChanged(index(aIndex), index(aIndex));
}

QVariant IRFavoritesModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }

    if (aIndex.row() >= iPresetsList->count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;
        int row = aIndex.row();

        QString primaryText = iPresetsList->at(row)->name;
        list.append(primaryText);
        QString secondaryText = iPresetsList->at(row)->shortDesc;
        
        // fix bug #9888,if left descriptions as blank,
        // only one line displays in favorites view
        if (0 == secondaryText.length())
        {
            secondaryText = " ";
        }
        
        list.append(secondaryText);
        return list;
    }
	else if ( aRole == Qt::DecorationRole)
	{
	    QVariantList list;
		int row = aIndex.row();
		if(row < iLogos.size())
		{
            const HbIcon *icon = iLogos[row];
            if (icon)
            {
                list.append(*icon);
            }
            else
            {
                list.append(*iStationLogo);
            }            
		}
		else
		{
            list.append(*iStationLogo);
		}		
        return list;
    }
    return QVariant();
}

bool IRFavoritesModel::checkFavoritesUpdate()
{
    clearPresetList();
    clearAndDestroyLogos();
    iPresetsList = iFavoritesDb->getPresets();
    emit modelChanged();
    return true;
}

void IRFavoritesModel::clearAndDestroyLogos()
{
    int size = iLogos.size(); 
    for (int i = 0; i < size; i ++)
    {
        delete iLogos[i];
        iLogos[i] = NULL;
    }
    iLogos.clear();
}

void IRFavoritesModel::clearFavoriteDB()
{
    if (NULL == iPresetsList)
    {
        return;
    }
    
    int presetSize = iPresetsList->count();
    int uniqId = 0;
         
    for (int i = 0; i < presetSize; ++i)
    {
        uniqId = iFavoritesDb->getUniqId(i);
        iFavoritesDb->deletePreset(uniqId);
    }    
 
    clearPresetList();
    clearAndDestroyLogos();
    emit modelChanged();
}

void IRFavoritesModel::clearPresetList()
{
    if (NULL == iPresetsList)
    {
        return;
    }
    
    while (!iPresetsList->isEmpty())
    {
        delete iPresetsList->takeLast();
    }
    delete iPresetsList;
    iPresetsList = NULL;
}

bool IRFavoritesModel::deleteOneFavorite(int aIndex)
{
    if ( aIndex < 0 || aIndex >= iPresetsList->size())
    {
        return false;
    }
    
    IRQPreset *preset = iPresetsList->at(aIndex);        
    int ret = iFavoritesDb->deletePreset(preset->uniqID);
    if ( 0 != ret )
    {
        return false;
    }
    
    beginRemoveRows(QModelIndex(), aIndex, aIndex);
    iPresetsList->removeAt(aIndex);
    
    if (aIndex<iLogos.size())
    {
        delete iLogos[aIndex];
        iLogos[aIndex] = NULL;
    }
    iLogos.removeAt(aIndex);
    endRemoveRows();
    emit modelChanged();
    return true;    
}

