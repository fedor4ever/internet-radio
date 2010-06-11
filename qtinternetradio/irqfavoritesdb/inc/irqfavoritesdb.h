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
#ifndef IRQFAVORITESDB_H
#define IRQFAVORITESDB_H

#include <QObject>

#include "irqfavoritesdbexporter.h"

class IRQPreset;
class IRQFavoritesDBPrivate;

class IRQFAVORITES_DLL_EXPORT IRQFavoritesDB : public QObject
{
Q_OBJECT
public:
    enum PSReason
    {
        PSCreated,
        PSDeleted,
        PSModified
    };
    
public:

    IRQFavoritesDB();

    ~IRQFavoritesDB();

    /* add a preset 
     * @param IRQPreset& the isds preset,
     * @return  errcode
     */
    int addPreset(const IRQPreset& aPreset);

    /* add a preset manually
     * @return  errcode
     * @param 
     */
    int addPreset(const QString& aName, const QString& aURL);

    /* get a preset uniq id
     * @return 
     * @param
     */
    int getUniqId(int aNum) const;
    
    /* delete a preset by uniq id
     * @return errcode
     * @param
     */
    int deletePreset(int aUniqId);
    
    /* search a preset by uniqpresetId / isdspresetid
     * warning: the function needs further checking
     *  @return errcode
     */
    int searchPreset(int aUniqPresetId, int aIsdsPresetId);

    /* get the previouse preset index in the internal list
     * @return the index
     */
    int getPreviousPreset(int aIndex);

    /* get the next preset index
     * @return the index
     */
    int getNextPreset(int aIndex); 

    /* replace with new preset
     * @return errcode 
     */
    int replacePreset(const IRQPreset& aNewPreset);

    /* @return errcode
     */
    int replaceUserDefinedPreset(const IRQPreset& aNewPreset);

    /* change the preset type to user defined
     * @return errcode
     */
    int makePresetUserDefined(int aChannelId, int aUserDefinedChannelId);

    /* get the empty preset left count
     * @return the count of empty presets left 
     */
    int emptyPresetCount() const;

    /* get the max preset count supported now
     * @return errcode 
     */
    int maxPresetCount();

    /* the interface is not used currently.
     **/
    void setMoveStatus(bool aStatus);

    /* the interface is not used currently 
     */
    bool getMoveStatus();

    /* for CIRPreset is just an interface so we can wrapper it into the IRQPreset.
     * the interface get the IRQPreset list. The upper layer will free all the items
     * in the list and the list self*/
    QList<IRQPreset*>* getPresets() const;

    /*
     * Increase the played times of the preset if it's in the favorites
     */
    void increasePlayedTimes(const IRQPreset &aPreset);
    
    int renamePreset(const IRQPreset &aPreset, const QString &aNewName);
    
signals:
    /*
     * the signal will triggered when we receive the message that preset is changed
     */
    void presetChanged(int aId, int aDataHandler, PSReason aReason);

private:
    IRQFavoritesDBPrivate * const d_ptr;
    
    Q_DISABLE_COPY(IRQFavoritesDB)
    
    friend class IRQFavoritesDBPrivate;
};

#endif
