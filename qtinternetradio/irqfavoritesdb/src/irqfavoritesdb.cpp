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
#include "irqfavoritesdb.h" 
#include "irqfavoritesdb_p.h"

IRQFavoritesDB::IRQFavoritesDB() : d_ptr(new IRQFavoritesDBPrivate(this))
{    
    d_ptr->init();
}

IRQFavoritesDB::~IRQFavoritesDB()
{
    delete d_ptr;
}

//add a preset 
//@param IRQPreset& the preset
//@return  errcode
int IRQFavoritesDB::addPreset(const IRQPreset& aPreset)
{
    return d_ptr->addPreset(aPreset);
}

//add a preset manually
//@return  errcode
//@param 
//
int IRQFavoritesDB::addPreset(const QString& aName, const QString& aURL)
{
    return d_ptr->addPreset(aName, aURL);
}

//get a preset uniq id
//@return errcode
//@param
//
int IRQFavoritesDB::getUniqId(int aNum) const
{
    return d_ptr->getUniqId(aNum);
}

//delete a preset by uniq id
//@return errcode
//@param
//
int IRQFavoritesDB::deletePreset(int aUniqId)
{
    return d_ptr->deletePreset(aUniqId);
}

//search a preset by uniqpresetId / isdspresetid
//warning: the function needs further checking
//@return errcode
//
int IRQFavoritesDB::searchPreset(int aUniqPresetId, int aIsdsPresetId)
{
    return d_ptr->searchPreset(aUniqPresetId, aIsdsPresetId);
}

//
//get the previouse preset index in the internal list
//@return the index
//
int IRQFavoritesDB::getPreviousPreset(int aIndex)
{
    return d_ptr->getPreviousPreset(aIndex);
}

//
//get the next preset index
//@return the index
//
int IRQFavoritesDB::getNextPreset(int aIndex)
{
    return d_ptr->getNextPreset(aIndex);
}
 
//replace with new preset
//@return errcode 
//
int IRQFavoritesDB::replacePreset(const IRQPreset& aNewPreset)
{
    return d_ptr->replacePreset(aNewPreset);
}

//@return errcode
//
int IRQFavoritesDB::replaceUserDefinedPreset(const IRQPreset& aNewPreset)
{
    return d_ptr->replaceUserDefinedPreset(aNewPreset);
}

//change the preset type to user defined
//@return errcode
//
int IRQFavoritesDB::makePresetUserDefined(int aChannelId, int aUserDefinedChannelId)
{
    return d_ptr->makePresetUserDefined(aChannelId, aUserDefinedChannelId);
}

//get the empty preset left count
//@return the count of empty presets left 
//
int IRQFavoritesDB::emptyPresetCount() const
{
    return d_ptr->emptyPresetCount();
}

//get the max preset count supported now
//@return errcode 
//
int IRQFavoritesDB::maxPresetCount()
{
    return d_ptr->maxPresetCount();
}

//the interface is not used currently.
//
void IRQFavoritesDB::setMoveStatus(bool aStatus)
{
    d_ptr->setMoveStatus(aStatus);  
}

//the interface is not used currently 
//
bool IRQFavoritesDB::getMoveStatus()
{
    return d_ptr->getMoveStatus();
}

//for CIRPreset is just an interface so we can wrapper it into the IRQPreset.
//the interface get the IRQPreset list. The upper layer will free all the items
//in the list and the list self
//
QList<IRQPreset*>* IRQFavoritesDB::getPresets() const
{
    return d_ptr->getPresets();
}

/*
 * Increase the played times of the preset if it's in the favorites
 */
void IRQFavoritesDB::increasePlayedTimes(const IRQPreset &aPreset)
{
    d_ptr->increasePlayedTimes(aPreset);
}




 






















