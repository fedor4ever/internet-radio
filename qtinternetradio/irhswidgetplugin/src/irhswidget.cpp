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
#include <HbLabel>
#include <HbDocumentLoader>
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>
#include <HbColorScheme>
#include <HbStyleLoader>
#include <QGraphicsLinearLayout>
#include <QSettings>
#include <QPixmap>

// User includes
#include "irhswidget.h"
#include "irserviceclient.h"

// Defines
static const QString KIrHsWidgetDocml  = ":/resource/irhswidget.docml";
static const QString KIrHsWidget       = "irhswidget";
static const QString KLogoLabel        = "logoLabel";
static const QString KGoToIrLabel      = "goToIrLabel";
static const QString KControlLabel     = "controlLabel";
static const QString KFirstRowLabel    = "firstRowLabel";
static const QString KSecondRowLabel   = "secondRowLabel";
static const QString KInfoControlArea  = "info_control_area";
static const QString KCentrolArea      = "infoLayout";

static const QString KDefaultStationLogo = "qtg_large_internet_radio";
static const QString KPlayButtonIcon     = "qtg_mono_play";
static const QString KStopButtonIcon     = "qtg_mono_stop";

static const QString KFrameGraphicsName  = "qtg_fr_hswidget_normal";
static const QString KStationNameColor   = "qtc_hs_list_item_title";
static const QString KMetaDataColor      = "qtc_hs_list_item_content";

static const QString KLoadingAnimationPrefix  = "qtg_anim_loading_";
static const QString KLoadingIconName = "LoadingAnimation";

const int KIrHsWidgetLogoSize = 50; // hs widget logo size

// ======== MEMBER FUNCTIONS ========
// Constructor
IrHsWidget::IrHsWidget(QGraphicsItem* aParent, Qt::WindowFlags aFlags)
    : HbWidget(aParent, aFlags),
      mLogoLabel(NULL),
      mControlLabel(NULL),
      mGoToIrLabel(NULL),
      mStationNameLabel(NULL),
      mMetadataLabel(NULL),
      mInfoControlArea(NULL),
      mCentralArea(NULL),
      mServiceClient(NULL),
      mIrState(IrAppState::Unknown),
      mIrHsWidgetState(EUnknown)
{
    setupUi();
    initHomeSreenWidget();
    
    mServiceClient = new IrServiceClient(this);
    setupConnection();
}

// Destructor
IrHsWidget::~IrHsWidget()
{
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
}

// Called when widget is hidden from the home screen
void IrHsWidget::onHide()
{
}


void IrHsWidget::setupUi()
{
    HbDocumentLoader *uiLoader = new HbDocumentLoader();
    uiLoader->reset();

    bool loaded = false;
    uiLoader->load(KIrHsWidgetDocml, &loaded);

    if (loaded)
    {
        HbWidget *irHsWidget = qobject_cast<HbWidget *>(uiLoader->findWidget(KIrHsWidget));
        HbFrameDrawer *irHsWidgetDrawer = new HbFrameDrawer(KFrameGraphicsName, HbFrameDrawer::NinePieces);
        HbFrameItem   *irHsWidgetBgItem = new HbFrameItem(irHsWidgetDrawer, irHsWidget);
        irHsWidgetBgItem->setPreferredSize(irHsWidget->preferredSize());

        QGraphicsLinearLayout *irHsWidgetLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
        irHsWidgetLayout->addItem(irHsWidget);
        setLayout(irHsWidgetLayout);

        mGoToIrLabel      = qobject_cast<HbLabel *>(uiLoader->findWidget(KGoToIrLabel));
        mLogoLabel        = qobject_cast<HbLabel *>(uiLoader->findWidget(KLogoLabel));
        mControlLabel     = qobject_cast<HbLabel *>(uiLoader->findWidget(KControlLabel));
        mInfoControlArea  = qobject_cast<HbWidget *>(uiLoader->findWidget(KInfoControlArea));
        mCentralArea      = qobject_cast<HbWidget *>(uiLoader->findWidget(KCentrolArea));

        // make system's hs backgourd item under logo
        irHsWidgetBgItem->stackBefore(mLogoLabel);

        // set font for station name line
        mStationNameLabel = qobject_cast<HbLabel *>(uiLoader->findWidget(KFirstRowLabel));
        mStationNameLabel->setTextColor(HbColorScheme::color(KStationNameColor));

        // set font for meta data line
        mMetadataLabel = qobject_cast<HbLabel *>(uiLoader->findWidget(KSecondRowLabel));
        mMetadataLabel->setTextColor(HbColorScheme::color(KMetaDataColor));

        // Create animation.
        HbIconAnimationManager *animationManager = HbIconAnimationManager::global();
        HbIconAnimationDefinition animationDefinition;
        HbIconAnimationDefinition::AnimationFrame animationFrame;
        QList<HbIconAnimationDefinition::AnimationFrame> animationFrameList;
        
        QString animationFrameIconName;
        for (int i = 1; i < 11; i++)
        {
            animationFrame.duration = 100;
            animationFrameIconName.clear();
            animationFrameIconName.append(KLoadingAnimationPrefix);
            animationFrameIconName.append(animationFrameIconName.number(i));
            animationFrame.iconName = animationFrameIconName;
            animationFrameList.append(animationFrame);
        }
        animationDefinition.setPlayMode(HbIconAnimationDefinition::Loop);
        animationDefinition.setFrameList(animationFrameList);
        animationManager->addDefinition(KLoadingIconName, animationDefinition);

        // Construct an icon using the animation definition.
        mLoadingIcon.setIconName(KLoadingIconName);        
        
        // install event filter to widget
        mGoToIrLabel->installEventFilter(this);
        mControlLabel->installEventFilter(this);
        mLogoLabel->installEventFilter(this);
        mCentralArea->installEventFilter(this);
    }
    
    delete uiLoader;
}

void IrHsWidget::initHomeSreenWidget()
{
    if (mServiceClient->isStationPlayed())
    {
        loadStoppedLayout();
        QString stationName;
        if (mServiceClient->loadStationName(stationName))
        {
            mStationNameLabel->setPlainText(stationName);
        }
        
        if (mServiceClient->loadStationLogoFlag())
        {
            loadStationLogo();
        }
    }
    else
    {
        loadInitLayout();
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
}

bool IrHsWidget::eventFilter(QObject *aObject, QEvent *aEvent)
{
    bool eventWasConsumed = false;
    QString objectName     = aObject->objectName();
    QEvent::Type eventType = aEvent->type();
    
    if (KGoToIrLabel == objectName)
    {
        if (QEvent::GraphicsSceneMousePress == eventType)
        {
            eventWasConsumed = true;
            handleGoToIrAreaAction();
        }
    }
    else if (KLogoLabel == objectName)
    {
        if (QEvent::GraphicsSceneMousePress == eventType)
        {
            eventWasConsumed = true;
            handleLogoAreaAction();
        }
    }
    else if (KControlLabel == objectName)
    {
        if (QEvent::GraphicsSceneMousePress == eventType)
        {
            eventWasConsumed = true;
            handleControlAreaAction();
        }
    }
    else if (KCentrolArea == objectName)
    {
        if (QEvent::GraphicsSceneMousePress == eventType)
        {
            eventWasConsumed = true;
            handleCentralAreaAction();
        }
    }
        
    return eventWasConsumed;
}

// ================ handle user press event ===============
void IrHsWidget::handleLogoAreaAction()
{
    switch (mIrState)
    {
        case IrAppState::NoRunInit:
        case IrAppState::NoRunStopped:
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
    switch (mIrState)
    {
        case IrAppState::NoRunStopped:          
            mServiceClient->launchIrNowPlaying();
            loadLoadingLayout();              
            break;
                    
        case IrAppState::RunningStopped:
            mServiceClient->startPlaying();
            loadLoadingLayout();         
            break;
                    
        case IrAppState::Playing:
            mServiceClient->stopPlaying();
            loadStoppedLayout();          
            break;
            
        case IrAppState::Loading: 
            mServiceClient->cancelLoading();
            loadStoppedLayout();                     
            break;       

        default:
            break;
    }
}

void IrHsWidget::handleGoToIrAreaAction()
{
    handleLogoAreaAction();
}

void IrHsWidget::handleCentralAreaAction()
{
    handleLogoAreaAction();
}

void IrHsWidget::handleControlFailed()
{
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
    if (mStationNameLabel->plainText() != aStationName)
    {
        mStationNameLabel->setPlainText(aStationName);
    }
}

void IrHsWidget::handleStationLogoUpdated(bool aLogoAvailable)
{    
    if (aLogoAvailable)
    {
        loadStationLogo();
    }
    else
    {
        mLogoLabel->setIcon(KDefaultStationLogo);
    }
}

void IrHsWidget::handleMetaDataUpdated(const QString &aMetaData)
{
    if (mMetadataLabel->plainText() != aMetaData)
    {
        mMetadataLabel->setPlainText(aMetaData);
    }    
}


void IrHsWidget::handleIrStateUpdated(IrAppState::Type aNewState)
{
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
    mInfoControlArea->hide();
    mGoToIrLabel->show();
}

// LAF == [logo][StationInfo][Play]
void IrHsWidget::loadStoppedLayout()
{
    mGoToIrLabel->hide();
    mInfoControlArea->show();
    mMetadataLabel->setPlainText(QString(""));
    mControlLabel->setIcon(KPlayButtonIcon);
}

// LAF == [logo][StationInfo][Stop]
void IrHsWidget::loadPlayingLayout()
{
    mGoToIrLabel->hide();
    mInfoControlArea->show();
    mControlLabel->setIcon(KStopButtonIcon);      
}

// LAF == [logo][StationInfo][Loading]
void IrHsWidget::loadLoadingLayout()
{
    mGoToIrLabel->hide();
    mInfoControlArea->show();
    mMetadataLabel->setPlainText(QString(""));
    mControlLabel->setIcon(mLoadingIcon);   
}

void IrHsWidget::loadStationLogo()
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplicaton);
    if (settings.value(KIrSettingStationLogo).canConvert<QPixmap>())
    {
        QPixmap logoPixmap = settings.value(KIrSettingStationLogo).value<QPixmap>();
        QPixmap newLogoPixmap = 
             logoPixmap.scaled(QSize(KIrHsWidgetLogoSize,KIrHsWidgetLogoSize),Qt::KeepAspectRatio);
        QIcon logoQIcon(newLogoPixmap);
        HbIcon logoHbIcon(logoQIcon);            
        mLogoLabel->setIcon(logoHbIcon);
    }  
}

