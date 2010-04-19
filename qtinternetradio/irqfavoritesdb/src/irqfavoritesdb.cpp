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
#include <e32err.h>
#include "irqfavoritesdb.h" 
#include "irqenums.h"
#include "irqutility.h" 
#include "irqisdsdatastructure.h"
#include "irisdspreset.h"
#include "irpreset.h"
#include "irfavoritesdb.h"

EXPORT_C IRQFavoritesDB::IRQFavoritesDB()
{
    TRAPD(err, iIRFavoritesDb = CIRFavoritesDb::NewL());
    if( KErrNone != err)
        return;

    /* add self to the observers of the CIRFavoritesDb. When the call back function
     * is called, it means that preset is changed and we send a signal to the uper layer*/
    iIRFavoritesDb->AddObserver(*this);
}
IRQFavoritesDB::~IRQFavoritesDB()
{
    delete iIRFavoritesDb;
}

//add a preset 
//@param CIRIsdsPreset& the isds preset
//@return  errcode
EXPORT_C int IRQFavoritesDB::addPreset(const IRQPreset& aPreset)
{
    int returnCode = 0;
    TRAPD(err, addPresetL(aPreset, returnCode));
    RETURN_IF_ERR(err);

    int result = 0;
    IRQUtility::convertSError2QError(returnCode, result);
    return result;
}

//add a preset manually
//@return  errcode
//@param 
//
EXPORT_C int IRQFavoritesDB::addPreset(const QString& aName,
        const QString& aURL)
{
    
    if( 0 == aName.size() || 0 == aURL.size())
        return EIRQErrorBadParameter;
    
    TPtrC16 nameptr(reinterpret_cast<const TUint16*> (aName.utf16()));
    TPtrC16 urlptr(reinterpret_cast<const TUint16*> (aURL.utf16()));
    int returnCode = 0;
    TRAPD(err, (iIRFavoritesDb->AddPresetL(nameptr, urlptr, returnCode)));
    RETURN_IF_ERR(err);

    int result = 0;
    IRQUtility::convertSError2QError(returnCode, result);
    return result;
}

//get a preset uniq id
//@return errcode
//@param
//
EXPORT_C int IRQFavoritesDB::getUniqId(int aNum)
{
    if( aNum < 0 || aNum >= iIRFavoritesDb->iFavPresetList.Count())
        return EIRQErrorBadParameter;
    
    return iIRFavoritesDb->iFavPresetList[aNum]->Id();
}

//delete a preset by uniq id
//@return errcode
//@param
//
EXPORT_C int IRQFavoritesDB::deletePreset(int aUniqId)
{
    TRAPD( err, (iIRFavoritesDb->DeletePresetL(aUniqId)));
    RETURN_IF_ERR(err);
    return EIRQErrorNone;
}

//search a preset by uniqpresetId / isdspresetid
//warning: the function needs further checking
//@return errcode
//
EXPORT_C int IRQFavoritesDB::searchPreset(int aUniqPresetId, int aIsdsPresetId)
{
    int returnCode = 0;
    int result = 0; //for QT
    returnCode = iIRFavoritesDb->SearchPreset(aUniqPresetId, aIsdsPresetId);
    IRQUtility::convertSError2QError(returnCode, result);
    return result;
}

//
//get the previouse preset index in the internal list
//@return the index
//
EXPORT_C int IRQFavoritesDB::getPreviousPreset(int aIndex)
{
    return iIRFavoritesDb->GetPreviousPreset(aIndex);
}

//
//get the next preset index
//@return the index
//
EXPORT_C int IRQFavoritesDB::getNextPreset(int aIndex)
{
    return iIRFavoritesDb->GetNextPreset(aIndex);
}

 
//replace with new preset
//@return errcode 
//
EXPORT_C int IRQFavoritesDB::replacePreset(const IRQPreset& aNewPreset)
{
    TRAPD(err, replacePresetL(aNewPreset));
    RETURN_IF_ERR(err);
    return EIRQErrorNone;
}

//@return errcode
//
EXPORT_C int IRQFavoritesDB::replaceUserDefinedPreset(const IRQPreset& aNewPreset)
{
    TRAPD(err, replaceUserDefinedPresetL(aNewPreset));
    RETURN_IF_ERR(err);
    return EIRQErrorNone;
}

//change the preset type to user defined
//@return errcode
//
EXPORT_C int IRQFavoritesDB::makePresetUserDefined(int aChannelId,
        int aUserDefinedChannelId)
{
    TRAPD(err, (iIRFavoritesDb->MakePresetUserDefinedL(aChannelId, aUserDefinedChannelId)));
    RETURN_IF_ERR(err);
    return EIRQErrorNone;
}

//get the empty preset left count
//@return the count of empty presets left 
//
EXPORT_C int IRQFavoritesDB::emptyPresetCount() const
{
    return iIRFavoritesDb->EmptyPresetCount();
}

//get the max preset count supported now
//@return errcode 
//
EXPORT_C int IRQFavoritesDB::maxPresetCount()
{
    return iIRFavoritesDb->MaxPresetCount();
}

//the interface is not used currently.
//
EXPORT_C void IRQFavoritesDB::setMoveStatus(bool aStatus)
{
    iIRFavoritesDb->SetMoveStatus(aStatus);    
}

//the interface is not used currently 
//
EXPORT_C bool IRQFavoritesDB::getMoveStatus()
{
    return iIRFavoritesDb->GetMoveStatus();
}

//for CIRPreset is just an interface so we can wrapper it into the IRQPreset.
//the interface get the IRQPreset list. The upper layer will free all the items
//in the list and the list self
//
EXPORT_C QList<IRQPreset*>* IRQFavoritesDB::getPresets() const
{
    QList<IRQPreset*> * presetList = new QList<IRQPreset*> ();
    const RIRPresetArray& cirPresetList = iIRFavoritesDb->GetAllSortedPresets();
    int counts = cirPresetList.Count();
    for (int i = 0; i < counts; i++)
    {
        IRQPreset *irqPreset = new IRQPreset();
        IRQUtility::convertCIRPreset2IRQPreset(*(cirPresetList[i]), *irqPreset);
        presetList->append(irqPreset);
    }
    return presetList;
}

/*
 * Increase the played times of the preset if it's in the favorites
 */
EXPORT_C void IRQFavoritesDB::increasePlayedTimes(const IRQPreset &aPreset)
{
    TRAP_IGNORE(increasePlayedTimesL(aPreset));
}

EXPORT_C CIRFavoritesDb * IRQFavoritesDB::getCIRFavoriteDB() const
{
    return iIRFavoritesDb;
}

/*  None export functions */

void IRQFavoritesDB::HandlePresetChangedL(TInt aId, TUid aDataHandler,
        MPSPresetObserver::TPSReason aType)
{
    emit presetChanged(aId, aDataHandler, aType);
}

void IRQFavoritesDB::addPresetL(const IRQPreset& aPreset, int& aRetValue)
{
    CIRIsdsPreset * cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aPreset, *cirPreset);
    iIRFavoritesDb->AddPresetL(*cirPreset, aRetValue);
    CleanupStack::PopAndDestroy(cirPreset);
}

void IRQFavoritesDB::replacePresetL(const IRQPreset& aNewPreset)
{
    CIRIsdsPreset * cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aNewPreset, *cirPreset);
    iIRFavoritesDb->ReplacePresetL(*cirPreset);
    CleanupStack::PopAndDestroy(cirPreset);
}

void IRQFavoritesDB::replaceUserDefinedPresetL(const IRQPreset& aNewPreset)
{
    CIRIsdsPreset *cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aNewPreset, *cirPreset);
    iIRFavoritesDb->ReplaceUserDefinedPresetL(*cirPreset);
    CleanupStack::PopAndDestroy(cirPreset);
}

void IRQFavoritesDB::increasePlayedTimesL(const IRQPreset &aPreset)
{
    CIRIsdsPreset * cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aPreset, *cirPreset);
    iIRFavoritesDb->IncreasePlayedTimesL(*cirPreset);    
    CleanupStack::PopAndDestroy(cirPreset);
}



 






















