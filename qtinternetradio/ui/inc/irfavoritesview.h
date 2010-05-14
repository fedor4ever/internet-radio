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
#ifndef IRFAVORITESVIEW_H
#define IRFAVORITESVIEW_H

#include "irabstractlistviewbase.h"
#include "irqevent.h"

class IRFavoritesModel;
class IRQPreset;

class IRFavoritesView : public IrAbstractListViewBase 
{
    Q_OBJECT

public:
    ~IRFavoritesView();
    
protected:
    IRFavoritesView(IRApplication *aApplication, TIRViewId aViewId);
    
    //from base
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
private slots:
    void clearAllFavorites();        
    void networkRequestNotified(IRQNetworkEvent aEvent);    
    void convertAnother();
    void presetLogoDownload(IRQPreset* aPreset);
    void presetLogoDownloadError();	
    void modelChanged();
    void actionClicked(HbAction *aAction);
    
private:
    //from IrAbstractListViewBase
    void setCheckedAction();
    void handleItemSelected();
    void prepareMenu();
    void startConvert(int aIndex); 
    void shareContextAction();
    void deleteContextAction();
    void renameContextAction();
    void detailsContextAction();
    
    //from base view 
    void listViewLongPressed(HbAbstractViewItem *aItem, const QPointF& aCoords);
    
    void initToolBar();
    
    void lazyInit();
    
    void normalInit();
    
private:
    
    IRFavoritesModel  *iModel;
    HbAction          *iClearFavoriteAction;
    //the following are used to support the img
    QList<int>         iIconIndexArray;
    IRQPreset         *iLogoPreset;
    
    friend class IRViewManager;
};

#endif //IRFAVORITESVIEW_H
