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
#include <hbpushbutton.h>
#include <hbtoolbar.h>
#include <hbaction.h> 
#include <QFont>
#include <QActionGroup> 
#include <QSizePolicy> 
#include <QStringList>
#include <QStringListModel> 
#include <QTimer>

#include "irqisdsdatastructure.h"
#include "irsearchchannelsview.h" 
#include "irabstractviewmanager.h" 
#include "irstationsview.h" 
#include "irqnetworkcontroller.h" 
#include "irbannerlabel.h"  
#include "irqmetadata.h"
#include "irplaycontroller.h"
#include "irqisdsclient.h"
#include "irapplication.h" 
#include "irsearchcriteriadb.h"
#include "ircombobox.h"
#include "iruidefines.h" 

const QString KInitialString = "Input key string";  

IRSearchChannelsView::IRSearchChannelsView(IRApplication* aApplication,
        TIRViewId aViewId): IRBaseView(aApplication, aViewId),iCollectionsAction(NULL),iFavoritesAction(NULL),
        iGenresAction(NULL),iSearchAction(NULL),iSearchButton(NULL),iSearchText(NULL),iSearchIconLabel(NULL),
        iSearchCombobox(NULL),iFirstTime(true),iBannerLabel(NULL),iDB(NULL)

{
    // Create widget hierarchy
    setObjectName( SEARCH_CHANNELS_VIEW_OBJECT_VIEW );

    // List existing root elements - this allows us to refer to objects in the XML 
    // which are created outside the document.
    QObjectList roots;
    roots.append( this );
    iLoader.setObjectTree( roots );

    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME);
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME,SEARCH_CHANNELS_VIEW_NO_PLAYINGBANNER_SECTION); 

    iSearchCombobox = qobject_cast<IrComboBox *> (iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_SEARCHKEY_LABEL));       
    iSearchButton = qobject_cast<HbPushButton *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_SEARCHKEY_BUTTON));    
    iBannerLabel = qobject_cast<IrNowPlayingBannerLabel *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_PLAYING_BANNER));
    
    initContentWidget();
    initToolBar();    
    initMenu();        
    createDB();   
}

IRSearchChannelsView::~IRSearchChannelsView()
{
    delete iDB;
    iDB = NULL;
}

 
void IRSearchChannelsView::helpAction()
{
    popupNote(tr("Not ready"), HbMessageBox::MessageTypeInformation);
}
 


TIRHandleResult IRSearchChannelsView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aReason);
    TIRHandleResult ret = EIR_DoDefault;
    QString str = "";
    
    switch (aCommand)
    {
    case EIR_ViewCommand_ACTIVATED:
        updateView();
        iBannerLabel->startAnimation();
        str = iSearchCombobox->currentText();        
        str = str.trimmed();
        if( str == KInitialString || "" == str)
            iSearchButton->setEnabled(false);
        else
            iSearchButton->setEnabled(true);        
             
        iSearchAction->setChecked(true);
        break;

    case EIR_ViewCommand_DEACTIVATE:              
        iBannerLabel->stopAnimation();
        break;
    }

    return ret;
}

 
void IRSearchChannelsView::initContentWidget()
{
    iBannerLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary)); 

    iSearchButton->setEnabled(false);    
    iSearchCombobox->setEditable(true);
    iSearchCombobox->setEditText(KInitialString);
    
    connect(iSearchButton, SIGNAL(clicked(bool)),this, SLOT(startSearch(bool)));
    connect(iSearchCombobox, SIGNAL(editTextChanged(const QString&)), this, SLOT(searchBoxTextChanged(const QString&)));   
    connect(iNetworkController, SIGNAL(networkRequestNotified(IRQNetworkEvent)), this, SLOT(networkRequestNotified(IRQNetworkEvent)));     
    connect(iSearchCombobox, SIGNAL(pressed()), this, SLOT(comboboxClicked()));    
    connect(iPlayController, SIGNAL(metaDataAvailable(IRQMetaData*)), this,
            SLOT(metaDataAvailable(IRQMetaData*)));
    connect(iPlayController, SIGNAL(playingStopped()), this,
            SLOT(removeBanner()));
    connect(iBannerLabel, SIGNAL(pressed()), this, SLOT(gotoNowPlaying()));
}

void IRSearchChannelsView::initMenu()
{          
    
    HbAction *settings = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_SETTING_ACTION));
    HbAction *openWebAdressAction = qobject_cast<HbAction*>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_OPEN_WEB_ADDRESS_ACTION));
    HbAction *helpAction = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_HELP_ACTION));
    HbAction *exitAction = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_EXIT_ACTION));    
     
    connect(settings, SIGNAL(triggered()), this, SLOT(launchSettingsView()));
    connect(openWebAdressAction, SIGNAL(triggered()), this, SLOT(openWebAddress()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(helpAction()));
    connect(exitAction, SIGNAL(triggered()), iApplication, SIGNAL(quit()));
}


void IRSearchChannelsView::initToolBar()
{
    HbToolBar *viewToolBar = qobject_cast<HbToolBar *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_TOOLBAR));
    
    QActionGroup *actionGroup = new QActionGroup(this);

    iCollectionsAction = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_COLLECTION_ACTION));
    iCollectionsAction->setActionGroup(actionGroup);

    iFavoritesAction = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_FAVORITES_ACTION));
    iFavoritesAction->setActionGroup(actionGroup);

    iGenresAction = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_GENRES_ACTION));
    iGenresAction->setActionGroup(actionGroup);

    iSearchAction = qobject_cast<HbAction *>(iLoader.findObject(SEARCH_CHANNELS_VIEW_OBJECT_SEARCH_ACTION));
    iSearchAction->setActionGroup(actionGroup);
    
    connect(iCollectionsAction, SIGNAL(triggered()), this, SLOT(collectionsActionClicked()));
    connect(iFavoritesAction, SIGNAL(triggered()), this, SLOT(favoritesActionClicked()));
    connect(iGenresAction, SIGNAL(triggered()), iApplication, SLOT(loadGenre()));    

    iSearchAction->setChecked(true);
}

void IRSearchChannelsView::collectionsActionClicked()
{
    getViewManager()->activateView(EIRView_MainView);
}

void IRSearchChannelsView::favoritesActionClicked()
{
    getViewManager()->activateView(EIRView_FavoritesView);
}

void IRSearchChannelsView::startSearch(bool aBool)
{
    Q_UNUSED(aBool);
    setUseNetworkReason(EIR_UseNetwork_StartSearch);
    if (false == iApplication->verifyNetworkConnectivity())
    {
        return;
    }
    setUseNetworkReason(EIR_UseNetwork_NoReason);
    
    QString str = iSearchCombobox->currentText();   
    str = str.trimmed();
    
    if( "" == str )
    {         
        return;
    }

    bool exists = iDB->isAlreadyExist(str);
    if (!exists)
    {
        iDB->addOneCriteria(str);
    }

    IRStationsView *searchResultView =
            static_cast<IRStationsView*> (getViewManager()->getView(
                    EIRView_SearchResultView, true));

    searchResultView->loadSearchResult(str);     
} 
 
 
void IRSearchChannelsView::searchBoxTextChanged(const QString &aString)
{
    QString str = aString.trimmed();
    if( "" != str )
    {
        iSearchButton->setEnabled(true);
    }   
    else  
    {
        iSearchButton->setEnabled(false);
    }
}  

void IRSearchChannelsView::cancelRequest()
{
    iIsdsClient->isdsCancelRequest();
} 

void IRSearchChannelsView::networkRequestNotified(IRQNetworkEvent aEvent)
{        
    if (getViewManager()->currentView() != this)
    {       
        return;
    }
    
    switch (aEvent)
    {
    case EIRQNetworkConnectionEstablished:
        iApplication->closeConnectingDialog();
        if(EIR_UseNetwork_StartSearch == getUseNetworkReason())
        {
            QString str = iSearchCombobox->currentText();
            str = str.trimmed();

            if ("" == str)
            {
                return;
            }
            
            bool exists = iDB->isAlreadyExist(str);
            if (!exists)
            {             
                iDB->addOneCriteria(str);
            }

            IRStationsView *searchResultView = static_cast<IRStationsView*> (getViewManager()->getView(
                            EIRView_SearchResultView, true));

            searchResultView->loadSearchResult(str);
        }
        
        setUseNetworkReason(EIR_UseNetwork_NoReason);
        break;

    default:
        iSearchAction->setChecked(true);
        break;
    }   
} 

void IRSearchChannelsView::comboboxClicked()
{        
    if( iFirstTime )
    {
        iSearchCombobox->clearEditText();
        iFirstTime = false;        
    }     
}


void IRSearchChannelsView::launchSettingsView()
{
    getViewManager()->activateView(EIRView_SettingsView);
}

void IRSearchChannelsView::openWebAddress()
{
    getViewManager()->activateView(EIRView_OpenWebAddressView);
}

void IRSearchChannelsView::gotoNowPlaying()
{
    Q_ASSERT(iPlayController->isPlaying());
    
    getViewManager()->activateView(EIRView_PlayingView);
}

void IRSearchChannelsView::addBanner(const QString &aText)
{
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME, SEARCH_CHANNELS_VIEW_WITH_PLAYINGBANNER_SECTION);
    iBannerLabel->setText(aText);
    iBannerLabel->setLoopCount(-1);
}

void IRSearchChannelsView::metaDataAvailable(IRQMetaData* aMetaData)
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

void IRSearchChannelsView::updateView()
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
}

void IRSearchChannelsView::removeBanner()
{
    iBannerLabel->stopAnimation();
    iLoader.load(SEARCH_CHANNELS_VIEW_LAYOUT_FILENAME,SEARCH_CHANNELS_VIEW_NO_PLAYINGBANNER_SECTION); 
}

void IRSearchChannelsView::createDB()
{
    iDB = new IRSearchCriteriaDB();
    QStringList strList;
    iDB->getAllCriteria(strList);
    QStringListModel* comboboxListModel = new QStringListModel(iSearchCombobox);
    iSearchCombobox->setModel(comboboxListModel);
    comboboxListModel->setStringList(strList);
    connect(iDB, SIGNAL(dataAdded()),this, SLOT(dbDataChanged()));   
}

void IRSearchChannelsView::dbDataChanged()
{         
    QStringList strList;
    bool ret = iDB->getAllCriteria(strList);
     
    if (ret)
    {         
        (static_cast<QStringListModel*> (iSearchCombobox->model()))->setStringList(
                strList);
    }    
}

