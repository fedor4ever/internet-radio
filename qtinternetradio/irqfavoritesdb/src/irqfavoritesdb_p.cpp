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

#include "irfavoritesdb.h"
#include "irqfavoritesdb.h"
#include "irqfavoritesdb_p.h"
#include "irqenums.h"
#include "irqutility.h" 
#include "irqisdsdatastructure.h"
#include "irpreset.h"
#include "irisdspreset.h"

IRQFavoritesDBPrivate::IRQFavoritesDBPrivate(IRQFavoritesDB *aIRQFavorites) : q_ptr(aIRQFavorites),
                                                                              mIRFavoritesDb(NULL)
{
}

IRQFavoritesDBPrivate::~IRQFavoritesDBPrivate()
{
    delete mIRFavoritesDb;
    mIRFavoritesDb = NULL;
}

void IRQFavoritesDBPrivate::init()
{
    TRAPD(err, mIRFavoritesDb = CIRFavoritesDb::NewL());
    if (KErrNone != err)
    {
        return;
    }

    /* add self to the observers of the CIRFavoritesDb. When the call back function
    * is called, it means that preset is changed and we send a signal to the uper layer*/
    mIRFavoritesDb->AddObserver(*this);
}

int IRQFavoritesDBPrivate::addPreset(const IRQPreset& aPreset)
{
    int returnCode = 0;
    TRAPD(err, addPresetL(aPreset, returnCode));
    
    int result = 0;
    if (KErrNone != err)
    {
        IRQUtility::convertSError2QError(err, result);
    }
    else
    {
        IRQUtility::convertSError2QError(returnCode, result);
    }
    return result;
}

int IRQFavoritesDBPrivate::addPreset(const QString& aName, const QString& aURL)
{
    if (0 == aName.size() || 0 == aURL.size())
    {
        return EIRQErrorBadParameter;
    }
    
    TPtrC16 nameptr(reinterpret_cast<const TUint16*> (aName.utf16()));
    TPtrC16 urlptr(reinterpret_cast<const TUint16*> (aURL.utf16()));
    int returnCode = 0;
    TRAPD(err, mIRFavoritesDb->AddPresetL(nameptr, urlptr, returnCode));
    
    int result = 0;
    if (KErrNone != err)
    {
        IRQUtility::convertSError2QError(err, result);
    }
    else
    {
        IRQUtility::convertSError2QError(returnCode, result);
    }
    return result;
}

int IRQFavoritesDBPrivate::getUniqId(int aNum) const
{
    if ( aNum < 0 || aNum >= mIRFavoritesDb->iFavPresetList.Count())
    {
        return EIRQErrorBadParameter;
    }
        
    //the preset's uniqid is equal to the CIRPreset's id
    return mIRFavoritesDb->iFavPresetList[aNum]->Id();
}

int IRQFavoritesDBPrivate::deletePreset(int aUniqId)
{
    TRAPD(err, (mIRFavoritesDb->DeletePresetL(aUniqId)));
    int result = 0;
    IRQUtility::convertSError2QError(err, result);
    return result;
}

int IRQFavoritesDBPrivate::searchPreset(int aUniqPresetId, int aIsdsPresetId)
{
    int returnCode = 0;
    int result = 0; //for QT
    returnCode = mIRFavoritesDb->SearchPreset(aUniqPresetId, aIsdsPresetId);
    IRQUtility::convertSError2QError(returnCode, result);
    return result;
}

int IRQFavoritesDBPrivate::getPreviousPreset(int aIndex)
{
    return mIRFavoritesDb->GetPreviousPreset(aIndex);
}

int IRQFavoritesDBPrivate::getNextPreset(int aIndex)
{
    return mIRFavoritesDb->GetNextPreset(aIndex);
}

int IRQFavoritesDBPrivate::replacePreset(const IRQPreset& aNewPreset)
{
    TRAPD(err, replacePresetL(aNewPreset));
    int result = 0;
    IRQUtility::convertSError2QError(err, result);
    return result;
}

int IRQFavoritesDBPrivate::replaceUserDefinedPreset(const IRQPreset& aNewPreset)
{
    TRAPD(err, replaceUserDefinedPresetL(aNewPreset));
    int result = 0;
    IRQUtility::convertSError2QError(err, result);
    return result;
}

int IRQFavoritesDBPrivate::makePresetUserDefined(int aChannelId, int aUserDefinedChannelId)
{
    TRAPD(err, mIRFavoritesDb->MakePresetUserDefinedL(aChannelId, aUserDefinedChannelId));
    int result = 0;
    IRQUtility::convertSError2QError(err, result);
    return result;
}

int IRQFavoritesDBPrivate::emptyPresetCount() const
{
    return mIRFavoritesDb->EmptyPresetCount();
}

int IRQFavoritesDBPrivate::maxPresetCount()
{
    return mIRFavoritesDb->MaxPresetCount();
}

void IRQFavoritesDBPrivate::setMoveStatus(bool aStatus)
{
    mIRFavoritesDb->SetMoveStatus(aStatus);  
}

bool IRQFavoritesDBPrivate::getMoveStatus()
{
    if (mIRFavoritesDb->GetMoveStatus())
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<IRQPreset*>* IRQFavoritesDBPrivate::getPresets() const
{
    QList<IRQPreset*> * presetList = new QList<IRQPreset*> ();
    const RIRPresetArray& cirPresetList = mIRFavoritesDb->GetAllSortedPresets();
    int counts = cirPresetList.Count();
    for (int i = 0; i < counts; i++)
    {
        IRQPreset *irqPreset = new IRQPreset();
        IRQUtility::convertCIRPreset2IRQPreset(*(cirPresetList[i]), *irqPreset);
        presetList->append(irqPreset);
    }
    return presetList;
}

void IRQFavoritesDBPrivate::increasePlayedTimes(const IRQPreset &aPreset)
{
    TRAP_IGNORE(increasePlayedTimesL(aPreset));
}

/* from MPSPresetObserver */
void IRQFavoritesDBPrivate::HandlePresetChangedL(TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aType)
{
    IRQFavoritesDB::PSReason reason = IRQFavoritesDB::PSCreated;
    switch (aType)
    {
    case EPSCreated:
        reason = IRQFavoritesDB::PSCreated;
        break;
        
    case EPSDeleted:
        reason = IRQFavoritesDB::PSDeleted;
        break;
        
    case EPSModified:
        reason = IRQFavoritesDB::PSModified;
        break;
        
    default:
        Q_ASSERT(false);
        break;
    }
    
    emit q_ptr->presetChanged(aId, aDataHandler.iUid, reason);
}

void IRQFavoritesDBPrivate::addPresetL(const IRQPreset& aPreset, int& aRetValue)
{
    CIRIsdsPreset * cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aPreset, *cirPreset);
    mIRFavoritesDb->AddPresetL(*cirPreset, aRetValue);
    CleanupStack::PopAndDestroy(cirPreset);
}

void IRQFavoritesDBPrivate::replacePresetL(const IRQPreset& aNewPreset)
{
    CIRIsdsPreset * cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aNewPreset, *cirPreset);
    mIRFavoritesDb->ReplacePresetL(*cirPreset);
    CleanupStack::PopAndDestroy(cirPreset);
}

void IRQFavoritesDBPrivate::replaceUserDefinedPresetL(const IRQPreset& aNewPreset)
{
    CIRIsdsPreset *cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aNewPreset, *cirPreset);
    mIRFavoritesDb->ReplaceUserDefinedPresetL(*cirPreset);
    CleanupStack::PopAndDestroy(cirPreset);
}

void IRQFavoritesDBPrivate::increasePlayedTimesL(const IRQPreset &aPreset)
{
    CIRIsdsPreset * cirPreset = CIRIsdsPreset::NewLC();
    IRQUtility::convertIRQPreset2CIRIsdsPreset(aPreset, *cirPreset);
    mIRFavoritesDb->IncreasePlayedTimesL(*cirPreset);    
    CleanupStack::PopAndDestroy(cirPreset);
}
