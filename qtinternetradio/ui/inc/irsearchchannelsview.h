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
#ifndef IRSEARCHVIEWC_H
#define IRSEARCHVIEWC_H

 
#include "irqevent.h"
#include "irbaseview.h" 
 
 
class HbPushButton; 
class HbAction;   
class HbLabel;  
class IrComboBox; 
class IrNowPlayingBannerLabel;
class IRQMetaData;
class IRSearchCriteriaDB;


class IRSearchChannelsView : public IRBaseView
{
    Q_OBJECT
    
public:
    
    ~IRSearchChannelsView();
    //from base class IRBaseView
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
protected:
    IRSearchChannelsView(IRApplication* aApplication, TIRViewId aViewId);
    
private:
    
    //move from irabstractlistbaseview
    void updateView();
    void initMenu();
    void initToolBar();    
    void initContentWidget();    
    void addBanner(const QString &aText);
    void createDB();
    
    
private slots:
    
    void collectionsActionClicked();
    void favoritesActionClicked();    
 
    void startSearch(bool aBool);     
    void helpAction();        
    void searchBoxTextChanged(const QString &aString);    
    void cancelRequest();
    void networkRequestNotified(IRQNetworkEvent aEvent);  
    void comboboxClicked();
    void launchSettingsView();
    void openWebAddress();
    void gotoNowPlaying();
    void metaDataAvailable(IRQMetaData* aMetaData);
    void removeBanner();    
    void dbDataChanged();
    
private: // members    
    
    HbAction            *iCollectionsAction;
    HbAction            *iFavoritesAction;
    HbAction            *iGenresAction;
    HbAction            *iSearchAction;             
    HbPushButton        *iSearchButton;       
    HbLabel             *iSearchText;
    HbLabel             *iSearchIconLabel;
    IrComboBox          *iSearchCombobox;
    bool                 iFirstTime;
    IrNowPlayingBannerLabel *iBannerLabel;
    IRSearchCriteriaDB   *iDB;

    friend class IRViewManager;
};
#endif
