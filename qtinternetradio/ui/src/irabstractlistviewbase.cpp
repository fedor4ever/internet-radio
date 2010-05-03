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

#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hblistview.h>
#include <hbabstractviewitem.h>
#include <hbscrollbar.h>
#include <QTimer>

#include "irabstractviewmanager.h"
#include "irabstractlistviewbase.h"
#include "irbannerlabel.h"
#include "irplaycontroller.h"
#include "irapplication.h"
#include "irqmetadata.h"
#include "irqisdsdatastructure.h"
#include "irqisdsclient.h"
#include "irqenums.h"
#include "iruidefines.h"
 

IrAbstractListViewBase::IrAbstractListViewBase(IRApplication *aApplication, TIRViewId aViewId)
    : IRBaseView(aApplication, aViewId),
	  iListView(NULL),
      iBannerLabel(NULL),
      iHeadingLabel(NULL),
      iCollectionsAction(NULL), iFavoritesAction(NULL),
      iGenresAction(NULL), iSearchAction(NULL),iConvertTimer(NULL), iEffectOnGoing(false)
{
    // Create widget hierarchy
    setObjectName( ABSTRACT_LIST_VIEW_BASE_OBJECT_VIEW );

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this ); // abstractlistviewbase is referred in the XML document
    iLoader.setObjectTree( roots );
    
    // Load the XML file
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME);
        
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_NO_PLAYINGBANNER_SECTION);
    initEffects();
    initMenu();
    initContentWidget();
    initScrollBar();
    initToolBar();

    connect(iListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(clickItem(const QModelIndex&)));
    connect(iListView, SIGNAL(longPressed(HbAbstractViewItem *,const QPointF&)), this, SLOT(listViewLongPressed(HbAbstractViewItem *,const QPointF&)));     
    connect(iBannerLabel, SIGNAL(pressed()), this, SLOT(gotoNowPlaying()));
    connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this, SLOT(metaDataAvailable(IRQMetaData*)));
    connect(iPlayController, SIGNAL(playingStopped()), this, SLOT(removeBanner()));
}

IrAbstractListViewBase::~IrAbstractListViewBase()
{
    HbEffect::remove(QString("viewItem"), QString(":/effect/effects_item_selecte.fxml"), QString("chosen") );
    HbEffect::remove(QString("viewItem"), QString(":/effect/effects_item_select_end.fxml"),QString("chosenEnd"));
}

void IrAbstractListViewBase::initMenu()
{
    HbMenu *viewMenu = menu();
    connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(prepareMenu()));

    iOpenWebAddressAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_OPEN_WEB_ADDRESS_ACTION));
    connect(iOpenWebAddressAction, SIGNAL(triggered()), this, SLOT(openWebAddress()));

    QObject *settings = iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_SETTING_ACTION);
    connect(settings, SIGNAL(triggered()), this, SLOT(launchSettingsView()));
    
    QObject *helpAction = iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_HELP_ACTION);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(notReady()));
    
    QObject *exitAction = iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_EXIT_ACTION);
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));
}

void IrAbstractListViewBase::initToolBar()
{
    HbToolBar *viewToolBar = qobject_cast<HbToolBar *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_TOOLBAR));
    QActionGroup *actionGroup = new QActionGroup(viewToolBar);
    iCollectionsAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_COLLECTION_ACTION));
	iCollectionsAction->setActionGroup(actionGroup);
	iFavoritesAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_FAVORITE_ACTION));
	iFavoritesAction->setActionGroup(actionGroup);
	iGenresAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_GENRE_ACTION));
	iGenresAction->setActionGroup(actionGroup);	 
	iSearchAction = qobject_cast<HbAction *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_SEARCH_ACTION));
	iSearchAction->setActionGroup(actionGroup);

	connect(iCollectionsAction, SIGNAL(triggered()), this, SLOT(collectionsActionClicked()));
	connect(iFavoritesAction, SIGNAL(triggered()), this, SLOT(favoritesActionClicked()));
	connect(iSearchAction, SIGNAL(triggered()), this, SLOT(searchActionClicked()));
	connect(iGenresAction, SIGNAL(triggered()), iApplication, SLOT(loadGenre()));

	setCheckedAction();
}

void IrAbstractListViewBase::initContentWidget()
{
    iHeadingLabel = qobject_cast<IrViewBannerLabel *>(iLoader.findWidget(ABSTRACT_LIST_VIEW_BASE_OBJECT_HEADINGTEXTLABEL));
    QFont font;
    font.setBold(true);
    iHeadingLabel->setFont(font);
    
    iBannerLabel = qobject_cast<IrNowPlayingBannerLabel *>(iLoader.findWidget(ABSTRACT_LIST_VIEW_BASE_OBJECT_PLAYINGBANNER)); 
    iBannerLabel->setFont(font);

    iListView = qobject_cast<HbListView *>(iLoader.findObject(ABSTRACT_LIST_VIEW_BASE_OBJECT_LISTVIEW));
}

void IrAbstractListViewBase::initScrollBar()
{
    HbScrollBar *scrollbar = iListView->verticalScrollBar();
    scrollbar->setVisible(true);
    scrollbar->setInteractive(true);
    iListView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
}

void IrAbstractListViewBase::setCheckedAction()
{       
    //default implementation
    QList<QAction*> actionList = iCollectionsAction->actionGroup()->actions();
    for (QList<QAction*>::const_iterator it = actionList.begin(); it != actionList.end(); ++it)
    {
        (*it)->setChecked(false);
    }
}

void IrAbstractListViewBase::setHeadingText(const QString &aText)
{
    if (iHeadingLabel)
    {
        iHeadingLabel->setText(aText);
    }
}

QString IrAbstractListViewBase::getHeadingText() const
{
    if (iHeadingLabel)
    {
        return iHeadingLabel->text();
    }
    return ("");
}

void IrAbstractListViewBase::resetCurrentItem()
{
    
}

void IrAbstractListViewBase::storeCurrentItem()
{
}

/*
 * handle the signal of one item being selected.
 * Default implementation is doing nothing.
 */
void IrAbstractListViewBase::handleItemSelected()
{
}

/*
 * we call the functin when we handle the handleitemselect.
 * The return value shows wether we need to connect to the
 * network, true means we connect and false means do not
 */
void IrAbstractListViewBase::itemAboutToBeSelected(bool &aNeedNetwork)
{    
    aNeedNetwork =  true;
}

TIRHandleResult IrAbstractListViewBase::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = EIR_DoDefault;

    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:
        updateView();
        iBannerLabel->startAnimation();
        break;

    case EIR_ViewCommand_DEACTIVATE:
        storeCurrentItem();
        iBannerLabel->stopAnimation();
        ret = EIR_NoDefault;
        break;

    default:
        break;
    }

    return ret;
}

/*
 * When a view is shown, update the view.
 */
void IrAbstractListViewBase::updateView()
{
    if (iPlayController->isPlaying())
    {
        IRQMetaData *metaData = iPlayController->getMetaData();
        if (metaData && "" != metaData->getArtistSongName().trimmed() &&
            "-" != metaData->getArtistSongName().trimmed())
        {
            //set the banner text as song name
            addBanner(metaData->getArtistSongName());
        }
        else
        {
            //set the banner text as station name
            IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
            Q_ASSERT(nowPlayingPreset);
            addBanner(nowPlayingPreset->name);
        }
    }
    else
    {
        removeBanner();
    }
    
    setCheckedAction();
}

void IrAbstractListViewBase::collectionsActionClicked()
{
    getViewManager()->activateView(EIRView_MainView);
}

void IrAbstractListViewBase::favoritesActionClicked()
{
    getViewManager()->activateView(EIRView_FavoritesView);
}

void IrAbstractListViewBase::searchActionClicked()
{
    getViewManager()->activateView(EIRView_SearchView);
}

void IrAbstractListViewBase::openWebAddress()
{
    getViewManager()->activateView(EIRView_OpenWebAddressView);
}

/*
 * dynamically set the text of menu item or remove menu item.
 * Concrete classes can override this function if needed
 */
void IrAbstractListViewBase::prepareMenu()
{
}

void IrAbstractListViewBase::addBanner(const QString &aText)
{
    iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_WITH_PLAYINGBANNER_SECTION);
    iBannerLabel->setText(aText);
    iBannerLabel->setLoopCount(-1);

}

void IrAbstractListViewBase::launchSettingsView()
{
    getViewManager()->activateView(EIRView_SettingsView);
}

void IrAbstractListViewBase::metaDataAvailable(IRQMetaData* aMetaData)
{
    if (!aMetaData)
    {
        return;
    }
    
    const QString artistSongName = aMetaData->getArtistSongName();
    if ("" != artistSongName.trimmed() && "-" != artistSongName.trimmed())
    {
        addBanner(aMetaData->getArtistSongName());
    }
    else
    {
        //set the banner text as station name
        IRQPreset *nowPlayingPreset = iPlayController->getNowPlayingPreset();
        Q_ASSERT(nowPlayingPreset);
        addBanner(nowPlayingPreset->name);
    }
}

void IrAbstractListViewBase::removeBanner()
{
    if (getViewManager()->currentView() == this)
    {
        iBannerLabel->stopAnimation();
        iLoader.load(ABSTRACT_LIST_VIEW_BASE_LAYOUT_FILENAME, ABSTRACT_LIST_VIEW_BASE_NO_PLAYINGBANNER_SECTION);
    }
}

void IrAbstractListViewBase::notReady()
{
	popupNote(tr("Not ready"), HbMessageBox::MessageTypeInformation );
}

void IrAbstractListViewBase::gotoNowPlaying()
{
    Q_ASSERT(iPlayController->isPlaying());
    
    getViewManager()->activateView(EIRView_PlayingView);
}

void IrAbstractListViewBase::listViewLongPressed(HbAbstractViewItem *aItem, const QPointF &aCoords)
{    
    Q_UNUSED(aItem);
    Q_UNUSED(aCoords);    
}

void IrAbstractListViewBase::clickItem(const QModelIndex &aIndex)
{   
    if (!iEffectOnGoing)
    {        
        HbAbstractViewItem *listViewItem = iListView->itemByIndex(aIndex);
        iEffectOnGoing = true;         
        iChosenIndex = aIndex;
        HbEffect::start(listViewItem, "viewItem", "chosen",this, "selectEffectComplete1");
    } 
}

void IrAbstractListViewBase::initEffects()
{
    HbEffect::add(QString("viewItem"), QString(":/effect/effects_item_select.fxml"),
        QString("chosen") );
    HbEffect::add(QString("viewItem"), QString(":/effect/effects_item_select_end.fxml"),QString("chosenEnd"));
}

void IrAbstractListViewBase::selectEffectComplete1(HbEffect::EffectStatus aStatus )
{
    Q_UNUSED(aStatus);
    HbAbstractViewItem *listViewItem = iListView->itemByIndex(iChosenIndex);     
    HbEffect::start(listViewItem, QString("viewItem"), QString("chosenEnd"),this, "selectEffectComplete2");
}

void IrAbstractListViewBase::selectEffectComplete2(HbEffect::EffectStatus aStatus )
{
    Q_UNUSED(aStatus);
    iEffectOnGoing = false;        
    clickAfterEffects();
}

void IrAbstractListViewBase::clickAfterEffects()
{        
    bool needNetwork;
    itemAboutToBeSelected(needNetwork);     
    if (needNetwork)
    {
        setUseNetworkReason(EIR_UseNetwork_SelectItem);
        if (false == iApplication->verifyNetworkConnectivity())
        {
            return;
        }
        setUseNetworkReason(EIR_UseNetwork_NoReason);
    }

    handleItemSelected();          
}
 
