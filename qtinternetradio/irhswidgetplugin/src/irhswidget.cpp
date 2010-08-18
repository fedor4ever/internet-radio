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
* Description:  Internet Radio home screen widget
*
*/

// System includes
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <QGraphicsLinearLayout>
#include <QTimer>

// User includes
#include "irhswidget.h"
#include "irserviceclient.h"
#include "irhswidgettitlerow.h"
#include "irhswidgetmetadatarow.h"
#include "irqlogger.h"

// Constants
static const int KIrHsWidgetContentsMargin = 0;
static const QString KIrHsWidgetBackgroundImage = "qtg_fr_hswidget_normal";

static const int KLaunchLatency = 100; // ms, used for launch nowplyaing view later

// ======== MEMBER FUNCTIONS ========
// Constructor
IrHsWidget::IrHsWidget(QGraphicsItem* aParent, Qt::WindowFlags aFlags)
    : HbWidget(aParent, aFlags),
      mUserActionEnabled(false),
      mRowLayout(NULL),
      mTitleRow(NULL),
      mMetaDataRow(NULL),
      mServiceClient(NULL),
      mIrState(IrAppState::Unknown),
      mIrHsWidgetState(EUnknown)
{
    setupUi();
    initHomeSreenWidget();
    
    mServiceClient = IrServiceClient::openInstance();
    setupConnection();
}

// Destructor
IrHsWidget::~IrHsWidget()
{
    if (mServiceClient)
    {
        mServiceClient->closeInstance();
    } 
    
    if (mRowLayout->count() == 1)
    {
        delete mMetaDataRow;
        mMetaDataRow = NULL;
    }
}

// Initializes the widget.
// called by kqti fw when widget is added to home screen
void IrHsWidget::onInitialize()
{    
    mServiceClient->startMonitoringIrState();    
}

// Uninitializes the widget.
// called by kqti fw when widget is removed from home screen
void IrHsWidget::onUninitialize()
{
}

// Called when widget is shown in the home screen
void IrHsWidget::onShow()
{ 
    if (mRowLayout->count() == 2)
    {
        mMetaDataRow->startMetaDataMarquee();
    }   
}

// Called when widget is hidden from the home screen
void IrHsWidget::onHide()
{
    if (mRowLayout->count() == 2)
    {
        mMetaDataRow->stopMetaDataMarquee();
    }  
}


void IrHsWidget::setupUi()
{
    setContentsMargins( KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin,
            KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin);
    
    //Setup layout
    mRowLayout = new QGraphicsLinearLayout(Qt::Vertical);

    mRowLayout->setContentsMargins(KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin,
            KIrHsWidgetContentsMargin, KIrHsWidgetContentsMargin);
    mRowLayout->setSpacing(KIrHsWidgetContentsMargin);
    setLayout(mRowLayout);
   
   //background
   HbFrameDrawer * backgroundFrameDrawer = new HbFrameDrawer(KIrHsWidgetBackgroundImage, HbFrameDrawer::NinePieces);
   HbFrameItem* backgroundLayoutItem = new HbFrameItem(backgroundFrameDrawer); 
   setBackgroundItem( backgroundLayoutItem );
}

void IrHsWidget::initHomeSreenWidget()
{
    mTitleRow = new IrHsWidgetTitleRow(); 
    mMetaDataRow = new IrHsWidgetMetaDataRow();
    
    mRowLayout->addItem(mTitleRow);
    resizeHomeScreenWidget();
                
    if (mServiceClient->isStationPlayed())
    {
        loadStoppedLayout();
        QString stationName;
        if (mServiceClient->loadStationName(stationName))
        {
            mTitleRow->setStationName(stationName);
        }
        
        if (mServiceClient->loadStationLogoFlag())
        {
            mTitleRow->loadStationLogo();
        }
    }
    else
    {
        loadInitLayout();
    }
}

void IrHsWidget::resizeHomeScreenWidget()
{
    //resize here so homescreen will place widget correctly on screen
    setPreferredSize( mRowLayout->preferredSize() );
    if (parentWidget())
    {
        //to place widget properly after adding to homescreen
        parentWidget()->resize(preferredSize()); 
	}
}
    
void IrHsWidget::setupConnection()
{
    // signal - slot for service event
    QObject::connect(mServiceClient, SIGNAL(stationNameUpdated(QString)),
        this, SLOT(handleStationNameUpdated(QString)));
    QObject::connect(mServiceClient, SIGNAL(stationLogoUpdated(bool)),
        this, SLOT(handleStationLogoUpdated(bool)));
    QObject::connect(mServiceClient, SIGNAL(metaDataUpdated(QString)),
        this, SLOT(handleMetaDataUpdated(QString)));
    QObject::connect(mServiceClient, SIGNAL(irStateChanged(IrAppState::Type)),
        this, SLOT(handleIrStateUpdated(IrAppState::Type)));
    QObject::connect(mServiceClient, SIGNAL(controlFailed()),
        this, SLOT(handleControlFailed()));  
        
    enableUserAction();          
}

void IrHsWidget::launchNowplayingView()
{
    mServiceClient->launchIrNowPlaying();  
}

// ================ handle user press event ===============
void IrHsWidget::handleCommonAreaAction()
{
    switch (mIrState)
    {
        case IrAppState::NoRunInit:
        case IrAppState::NoRunStopped:
            disableUserAction();
            mServiceClient->launchIrNormally();
            break;
            
        case IrAppState::RunningInit:
        case IrAppState::RunningStopped:
        case IrAppState::Playing:
        case IrAppState::Loading:
            mServiceClient->bringIrForeground();
            break;

        default:
            break;
    }
}

void IrHsWidget::handleControlAreaAction()
{
    LOG("IrHsWidget::handleControlAreaAction()");
    //here, for we will control the application by homescreen, 
    //to avoid repeat actions, we disable all control event here
    //and enable them when state changed or failed.      
    disableUserAction();
    
    switch (mIrState)
    {
        case IrAppState::NoRunStopped:
            loadLoadingLayout();
            QTimer::singleShot(KLaunchLatency, this, SLOT(launchNowplayingView()));
            break;
                    
        case IrAppState::RunningStopped:
            mServiceClient->startPlaying();        
            break;
                    
        case IrAppState::Playing:
            mServiceClient->stopPlaying();         
            break;
            
        case IrAppState::Loading: 
            mServiceClient->cancelLoading();                  
            break;       

        default:
            break;
    }
}


void IrHsWidget::handleControlFailed()
{ 
    LOG("handleControlFailed()");
    enableUserAction();
    switch (mIrHsWidgetState)
    {
        case EInit:     // LAF == [logo][go to interneat radio]
            loadInitLayout();
            break;

        case EPlaying:  // LAF == [logo][StationInfo][Play]
            loadPlayingLayout();
            break;

        case EStopped:  // LAF == [logo][StationInfo][Stop]
            loadStoppedLayout();
            break;

        case ELoading:  // LAF == [logo][StationInfo][Loading]
            loadLoadingLayout();
            break;

        default:
            break;
    }    
}


// ======== for service notification ========
void IrHsWidget::handleStationNameUpdated(const QString &aStationName)
{
    mTitleRow->setStationName(aStationName);
}

void IrHsWidget::handleStationLogoUpdated(bool aLogoAvailable)
{    
    if (aLogoAvailable)
    {
        mTitleRow->loadStationLogo();
    }
    else
    {
        mTitleRow->setDefaultLogo();
    }
}

void IrHsWidget::handleMetaDataUpdated(const QString &aMetaData)
{
    mMetaDataRow->setMetaData(aMetaData); 
}


void IrHsWidget::handleIrStateUpdated(IrAppState::Type aNewState)
{
    LOG("IrHsWidget::handleIrStateUpdated()");
    enableUserAction();
    if (aNewState == mIrState)
    {
        return;
    }
    
    mIrState = aNewState;
    switch (mIrState)
    {
        case IrAppState::NoRunInit:
        case IrAppState::RunningInit:   // LAF == [logo][go to interneat radio]
            handleHsWidgetStateChange(EInit);
            break;

        case IrAppState::NoRunStopped:
        case IrAppState::RunningStopped: // LAF == [logo][StationInfo][Play]
            handleHsWidgetStateChange(EStopped);
            break;

        case IrAppState::Playing:        // LAF == [logo][StationInfo][Stop]
            handleHsWidgetStateChange(EPlaying);
            break;

        case IrAppState::Loading:        // LAF == [logo][StationInfo][Loading]
            handleHsWidgetStateChange(ELoading);
            break;

        default:
            break;
    }
}

void IrHsWidget::handleHsWidgetStateChange(IrHsWidgetState aNewState)
{
    if (aNewState == mIrHsWidgetState)
    {
        return;
    }
    
    mIrHsWidgetState = aNewState;
    switch (mIrHsWidgetState)
    {
        case EInit:     // LAF == [logo][go to interneat radio]
            loadInitLayout();
            break;

        case EPlaying:  // LAF == [logo][StationInfo][Play]
            loadPlayingLayout();
            break;

        case EStopped:  // LAF == [logo][StationInfo][Stop]
            loadStoppedLayout();
            break;

        case ELoading:  // LAF == [logo][StationInfo][Loading]
            loadLoadingLayout();
            break;

        default:
            break;
    }
}
// LAF == [logo][go to interneat radio]
void IrHsWidget::loadInitLayout()
{
    if (mRowLayout->count() == 2)
    {
        mRowLayout->removeItem(mMetaDataRow);
        resizeHomeScreenWidget();      
    }
    
    mTitleRow->setDefaultTitle();        
}

// LAF == [logo][StationInfo][Play]
void IrHsWidget::loadStoppedLayout()
{
    if (mRowLayout->count() == 1)
    {
        mRowLayout->addItem(mMetaDataRow);
        resizeHomeScreenWidget();      
    }
    
    mMetaDataRow->setPlayIcon();
}

// LAF == [logo][StationInfo][Stop]
void IrHsWidget::loadPlayingLayout()
{
    if (mRowLayout->count() == 1)
    {
        mRowLayout->addItem(mMetaDataRow);
        resizeHomeScreenWidget();      
    }
    
    mMetaDataRow->setStopIcon();     
}

// LAF == [logo][StationInfo][Loading]
void IrHsWidget::loadLoadingLayout()
{
    if (mRowLayout->count() == 1)
    {
        mRowLayout->addItem(mMetaDataRow);
        resizeHomeScreenWidget();      
    }
    
    mMetaDataRow->setLoadingIcon(); 
}



void IrHsWidget::enableUserAction()
{
    if (!mUserActionEnabled)
    {
        mUserActionEnabled = true;
        // signal - slot for user click action
        QObject::connect(mTitleRow, SIGNAL(titleRowClicked()),
            this, SLOT(handleCommonAreaAction()));
        QObject::connect(mMetaDataRow, SIGNAL(metaDataAreaClicked()),
            this, SLOT(handleCommonAreaAction()));     
        QObject::connect(mMetaDataRow, SIGNAL(controlAreaClicked()),
            this, SLOT(handleControlAreaAction()));      
    }
}

void IrHsWidget::disableUserAction()
{
    if (mUserActionEnabled)
    {    
        mUserActionEnabled = false;
        // signal - slot for user click action
        QObject::disconnect(mTitleRow, SIGNAL(titleRowClicked()),
            this, SLOT(handleCommonAreaAction()));
        QObject::disconnect(mMetaDataRow, SIGNAL(metaDataAreaClicked()),
            this, SLOT(handleCommonAreaAction()));     
        QObject::disconnect(mMetaDataRow, SIGNAL(controlAreaClicked()),
            this, SLOT(handleControlAreaAction()));  
    }
}

