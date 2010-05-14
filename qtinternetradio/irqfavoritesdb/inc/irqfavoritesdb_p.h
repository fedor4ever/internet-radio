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

#ifndef IRQFAVORITESDB_P_H
#define IRQFAVORITESDB_P_H

#include <QObject>
#include <pspresetobserver.h>

class CIRFavoritesDb;
class IRQPreset;
class IRQFavoritesDB;

class IRQFavoritesDBPrivate : public MPSPresetObserver
{
public:
    IRQFavoritesDBPrivate(IRQFavoritesDB *aIRQFavorites);
    
    ~IRQFavoritesDBPrivate();
    
    void init();
    
    int addPreset(const IRQPreset& aPreset);
    
    int addPreset(const QString& aName, const QString& aURL);
    
    int getUniqId(int aNum) const;
            
    int deletePreset(int aUniqId);
    
    int searchPreset(int aUniqPresetId, int aIsdsPresetId);
    
    int getPreviousPreset(int aIndex);
    
    int getNextPreset(int aIndex); 
    
    int replacePreset(const IRQPreset& aNewPreset);
    
    int replaceUserDefinedPreset(const IRQPreset& aNewPreset);
    
    int makePresetUserDefined(int aChannelId, int aUserDefinedChannelId);
    
    int emptyPresetCount() const;
    
    int maxPresetCount();
    
    void setMoveStatus(bool aStatus);
    
    bool getMoveStatus();
    
    QList<IRQPreset*>* getPresets() const;
    
    void increasePlayedTimes(const IRQPreset &aPreset);
    
private:
    /* from MPSPresetObserver */
    void HandlePresetChangedL(TInt aId, TUid aDataHandler, MPSPresetObserver::TPSReason aType);
    
    void addPresetL(const IRQPreset& aPreset, int& aRetValue);
    
    void replacePresetL(const IRQPreset& aNewPreset);
        
    void replaceUserDefinedPresetL(const IRQPreset& aNewPreset);
        
    void increasePlayedTimesL(const IRQPreset &aPreset);
    
private:
    IRQFavoritesDB * const q_ptr;
    CIRFavoritesDb *mIRFavoritesDb;
};

#endif  //IRQFAVORITESDB_P_H
