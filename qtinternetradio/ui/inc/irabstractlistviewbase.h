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
#ifndef IRABSTRACTLISTVIEWBASE_H
#define IRABSTRACTLISTVIEWBASE_H


#include <hbeffect.h>
#include "irbaseview.h"
#include "irqenums.h"

class HbListView;
class IrNowPlayingBannerLabel;
class IrViewBannerLabel;
class HbAction;
class HbLabel;
class IRQMetaData;
class HbAbstractViewItem;

class IrAbstractListViewBase : public IRBaseView
{
	Q_OBJECT
public:
	void setHeadingText(const QString &aText);
	QString getHeadingText() const;
    virtual void resetCurrentItem();
	
    ~IrAbstractListViewBase();
    
protected:    
    IrAbstractListViewBase(IRApplication *aApplication, TIRViewId aViewId);
    virtual void storeCurrentItem();
    virtual void handleItemSelected();
    
   
    //we call the functin when we handle the handleitemselect.
    //The return value shows wether we need to connect to the
    //network, true means we connect and false means do not    
	//eg. click song name, Music Shop will be open. IR needn't connect netork
    virtual void itemAboutToBeSelected(bool &aNeedNetwork);
    
    virtual void setCheckedAction();
    
    virtual void updateView();

    //from base
    TIRHandleResult handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason);
    
private slots:
    void clickItem(const QModelIndex &aIndex);

	void collectionsActionClicked();
	
	void favoritesActionClicked();
	
	void searchActionClicked();

	virtual void prepareMenu();
	
	void metaDataAvailable(IRQMetaData*);
	
	void removeBanner();
	
	void notReady();

    void gotoNowPlaying();
    
    void launchSettingsView();
    
	void openWebAddress();
	
	virtual void listViewLongPressed(HbAbstractViewItem *aItem, const QPointF &aCoords);
	
	void selectEffectComplete1(HbEffect::EffectStatus aStatus);
	
	void selectEffectComplete2(HbEffect::EffectStatus aStatus);
private:
    void initMenu();
    void initToolBar();
    void initContentWidget();
    void initEffects();
    void initScrollBar();
    void addBanner(const QString &aText);
    void clickAfterEffects();
    
    
    
protected:
	HbListView  *iListView;
	IrNowPlayingBannerLabel *iBannerLabel;
	IrViewBannerLabel *iHeadingLabel;
	HbAction      *iOpenWebAddressAction;
	HbAction      *iCollectionsAction;
	HbAction      *iFavoritesAction;
	HbAction      *iGenresAction;
	HbAction      *iSearchAction;
	QTimer        *iConvertTimer;
	bool          iEffectOnGoing;
	QModelIndex   iChosenIndex;
};

#endif // IRABSTRACTLISTVIEWBASE_H
