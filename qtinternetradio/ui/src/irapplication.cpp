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

#include <hbaction.h>
#include <qcoreapplication.h>
#include <hbdevicemessagebox.h>
#include <hbprogressdialog.h>
#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include <xqserviceutil.h>
#include <hbiconitem.h>
#include <hbiconanimator.h>
#include <hbiconanimationmanager.h>

#ifdef LOCALIZATION 
#include <QTranslator>
#include "irqutility.h" 
#endif

#include "irviewmanager.h"
#include "irapplication.h"

#include "irqnetworkcontroller.h"
#include "irqisdsclient.h"
#include "irplaycontroller.h"
#include "irlastplayedstationinfo.h"
#include "irqfavoritesdb.h"
#include "irqsettings.h"  
#include "irmediakeyobserver.h"
#include "ircategoryview.h"
#include "irqlogger.h"
#include "iruidefines.h"
#include "irqsystemeventhandler.h"
#include "irabstractlistviewbase.h"
#include "irfileviewservice.h"
/*
 * Description : constructor, initialize all data members
 * Parameters  : aViewManager : pointer to the view manager object
 * Return      : None
 */
IRApplication::IRApplication(IRViewManager* aViewManager, IRQSystemEventHandler* aSystemEventHandler):
#ifdef _DEBUG
                                     iTestPreferredBitrate(false),
#endif
                                     iViewManager(aViewManager),
                                     iStartingViewId(EIRView_CategoryView),
                                     iNetworkController(NULL),    
                                     iIsdsClient(NULL),  
                                     iPlayController(NULL),  
                                     iFavPresets(NULL),                                     
                                     iSettings(NULL),
                                     iMediaKeyObserver(NULL),                                                                                                                                                                                                                     
                                     iLastPlayedStationInfo(NULL),
                                     iAdvertisementClient(NULL),                                    
                                     iEnableGlobalAdv(true),
                                     iDisconnected(false),
                                     iConnectingCanceled(false),
                                     iLocalServer(NULL),
                                     iLoadingNote(NULL),
                                     #ifdef LOCALIZATION
                                     iTranslator(NULL),
                                     #endif
                                     iSystemEventHandler(aSystemEventHandler),
                                     iLoadingAnimation(NULL),
                                     iFileViewService(NULL)
                                     
{
    LOG_METHOD;
    
    iViewManager->setApplication(this);
    iInitEvent = static_cast<QEvent::Type>(QEvent::registerEventType());

#ifdef _DEBUG
    readConfiguration();
#endif

    createComponents();
    // get advertisement setting
    iSettings->getGlobalAdvFlag(iEnableGlobalAdv);
    setupConnection();
    setLaunchView();
    
    QString name = XQServiceUtil::interfaceName();
    if (name == QString("com.nokia.symbian.IFileView"))
    {
        iFileViewService = new IRFileViewService(this);
    }
} 

/*
 * Description : destructor, destroy all data members
 * Parameters  : None
 * Return      : None
 */
IRApplication::~IRApplication()
{
    LOG_METHOD;
    setExitingView();
    destroyComponents();
    
    if (iLocalServer)
    {
        iLocalServer->close();
    }
    
    delete iLastPlayedStationInfo;
    iLastPlayedStationInfo = NULL;
    
    delete iLoadingNote;
    iLoadingNote = NULL;
    
    delete iSystemEventHandler;
    
#ifdef LOCALIZATION
    if( iTranslator )
    {
        qApp->removeTranslator(iTranslator);
        delete iTranslator;
        iTranslator = NULL;
    }
#endif
    
    delete iFileViewService;
    iFileViewService = NULL;
}

 
/*
 * Description : When ir application is launched, set the correct view according to whether the 
 *               application is used for the first time
 * Parameters  : None
 * Return      : None
 */
void IRApplication::setLaunchView()
{
    if (!XQServiceUtil::isService())
    {
        //normal launch, launch starting view
        TIRViewId viewId = EIRView_CategoryView;
        iSettings->getStartingViewId(viewId);
        if (EIRView_PlayingView == viewId)
        {
            //handle error case
            if (NULL == getLastPlayedStationInfo()->getLastPlayedStation())
            {
                viewId = EIRView_CategoryView;
            }
        }
        
        launchStartingView(viewId);
    }
}

/*
 * Description : verify if the network connection has been established
 * Parameters  : None
 * Return      : true : network connection has been established
 *               false : network connection has not been established
 */
bool IRApplication::verifyNetworkConnectivity(const QString &aConnectingText)
{
    Q_ASSERT(iNetworkController);
    
    bool ret = true;
    
    if (!iNetworkController->getNetworkStatus())
    {
        ret = false;
        if (!iNetworkController->isConnectRequestIssued())
        {
            iConnectingText = aConnectingText;
            iNetworkController->chooseAccessPoint();
        }
    }
    
    return ret;
}

void IRApplication::startLoadingAnimation(const QPointF& aPos)
{
    if( NULL == iLoadingAnimation )
    {
        getLoadingAnimation();
    }
    
    if (iLoadingAnimation)
    {
        iLoadingAnimation->setPos(aPos);
        iLoadingAnimation->show();
        iLoadingAnimation->animator().startAnimation();
    }    
}

void IRApplication::startLoadingAnimation(const QObject *aReceiver, const char *aFunc)
{
    LOG_METHOD_ENTER;
    
    //for downloading logos in stations view, favorites view and history view, network connection
    //is initiated by low layer, we don't show any dialog
    if (!iNetworkController->getNetworkStatus())
    {
        IRBaseView *currentView = static_cast<IRBaseView*>(iViewManager->currentView());
        if (currentView && EIR_UseNetwork_NoReason == currentView->getUseNetworkReason())
        {
            return;
        }
    }
    
    //if in search, we could not show the dialog
    TIRViewId curViewID = static_cast<IRBaseView*>(iViewManager->currentView())->id();
    if ( EIRView_SearchView == curViewID )
    {
        return;        
    }
    
    if (NULL == iLoadingNote)
    {
        iLoadingNote = new HbProgressDialog(HbProgressDialog::WaitDialog);
        iLoadingNote->setModal(true);
        iLoadingNote->setTimeout(HbPopup::NoTimeout);
        QAction *action = iLoadingNote->actions().at(0);
        action->setText(hbTrId("txt_common_button_cancel"));
    }
    
    iLoadingNote->disconnect(SIGNAL(cancelled()));
    connect(iLoadingNote, SIGNAL(cancelled()), aReceiver, aFunc);

    if (iLoadingNote->isVisible())
    {
        return;
    }
    
    //if iConnectingText == "", network connection is initiated by lower layer (eg. downloading logos)
    if ("" != iConnectingText)
    {
        iLoadingNote->setText(iConnectingText);
        iConnectingText = "";
    }
    else
    {
        iLoadingNote->setText(hbTrId("txt_common_info_loading"));
    }
    
    iLoadingNote->show();
}

void IRApplication::stopLoadingAnimation()
{
    LOG_METHOD_ENTER;
    if (iLoadingNote)
    {
        iLoadingNote->close();
    }
    
    if( iLoadingAnimation )
    {
        iLoadingAnimation->animator().stopAnimation();
        iLoadingAnimation->hide();         
    }
}

/*
 * Description : return the pointer to the view manager object
 * Parameters  : None
 * Return      : the pointer to the view manager object
 */
IRViewManager* IRApplication::getViewManager() const
{
    return iViewManager;
}

/* 
 * Description : return the pointer to the network controller object
 * Parameters  : None
 * Return      : the pointer to the network controller object
 */
IRQNetworkController* IRApplication::getNetworkController()
{
    if(NULL == iNetworkController)
    {
        iNetworkController = IRQNetworkController::openInstance(); 
        connect(iNetworkController, SIGNAL(networkEventNotified(IRQNetworkEvent)),
               this, SLOT(networkEventNotified(IRQNetworkEvent)));
    } 
    return iNetworkController;
}

/*
 * Description : return the pointer to the isds client object
 * Parameters  : None
 * Return      : the pointer to the isds client object
 */
IRQIsdsClient* IRApplication::getIsdsClient()
{
    if(NULL == iIsdsClient)
    {
        iIsdsClient = IRQIsdsClient::openInstance(); 
    }
    return iIsdsClient;
}

/*
 * Description : return the pointer to the play controller object
 * Parameters  : None
 * Return      : the pointer to the play controller object
 */
IRPlayController* IRApplication::getPlayController()
{
    if(NULL == iPlayController)
    {
        iPlayController = new IRPlayController(this);
    }
    return iPlayController;
}

IRLastPlayedStationInfo* IRApplication::getLastPlayedStationInfo()
{
    if(NULL == iLastPlayedStationInfo)
    {
        iLastPlayedStationInfo = new IRLastPlayedStationInfo();
    }
    return iLastPlayedStationInfo;
}

IRQFavoritesDB* IRApplication::getFavoritesDB()
{
    if(NULL == iFavPresets)
    {
        iFavPresets = new IRQFavoritesDB();
    }
    return iFavPresets;
}

IRQSettings * IRApplication::getSettings()
{
    if(NULL == iSettings)
    {
        iSettings = IRQSettings::openInstance();
    }   
    return iSettings;
}

IRMediaKeyObserver* IRApplication::getMediaKeyObserver()
{
    if(NULL == iMediaKeyObserver)
    {
    iMediaKeyObserver = new IRMediaKeyObserver(this);
    }
    return iMediaKeyObserver;
}


IRQAdvClient* IRApplication::getAdvClient()
{
    if(iEnableGlobalAdv && (NULL == iAdvertisementClient))
    {
        //iAdvertisementClient = IRQAdvClient::openInstance();
    }

    return iAdvertisementClient;
}

IRPlayList* IRApplication::getPlayList() const
{
    if (NULL == iFileViewService)
    {
        return NULL;
    }
    
    return iFileViewService->getPlayList();
}

#ifdef LOCALIZATION
/*
* this function will take ownership of the translator
*/
void IRApplication::setTranslator(QTranslator* aTranslator)
{
    iTranslator = aTranslator;        
}
#endif

/*
 * Description : create all the application level components, including network controller,
 *               favorites db, isds client, play controller, etc
 * Parameters  : None
 * Return      : None
 */
void IRApplication::createComponents()
{
    getSettings();
}

/*
 * Descriiption : destroy all the application level components 
 * Parameters   : None
 * Return       : None
 */
void IRApplication::destroyComponents()
{	
    delete iPlayController;
    iPlayController = NULL;
    
    delete iSystemEventHandler;
    iSystemEventHandler = NULL;
	    
    if(iIsdsClient)
    {    
        iIsdsClient->closeInstance();
        iIsdsClient = NULL;
    }
    
    delete iFavPresets;
    iFavPresets = NULL;
    
    if(iNetworkController)
    {             
        iNetworkController->closeInstance();
        iNetworkController = NULL;
    }
    
    if(iSettings)
    {    
        iSettings->closeInstance();
        iSettings = NULL;
    }
    
    delete iMediaKeyObserver;
    iMediaKeyObserver = NULL;
}

void IRApplication::setupConnection()
{
    connect(this, SIGNAL(quit()), qApp, SLOT(quit()));
}

void IRApplication::cancelConnect()
{
    if (iConnectingCanceled)
    {
        return;
    }
    
    stopLoadingAnimation();
    iConnectingCanceled = true;
    if (iNetworkController->getNetworkStatus())
    {
        getIsdsClient()->isdsCancelRequest();
    }
    else
    {
        iNetworkController->cancelConnecting();
    }
    
    iNetworkController->notifyActiveNetworkObservers(EIRQConnectingCancelled);
}

   
//connect to signal 'networkEventNotified' from IRQNetworkController
void IRApplication::networkEventNotified(IRQNetworkEvent aEvent)
{
    LOG_SLOT_CALLER;
    switch (aEvent)
    {
        case EIRQNetworkConnectionConnecting :
            startLoadingAnimation(this, SLOT(cancelConnect()));
            iConnectingCanceled = false;
            break;
            
        case EIRQNetworkConnectionEstablished :
            iDisconnected = false;
            if (EIR_DoDefault == handleConnectionEstablished())
            {
                iNetworkController->notifyActiveNetworkObservers(aEvent);               
            }
            
            if (EIRQNoConnectionToNetwork == getPlayController()->getStopReason() &&
                iNetworkController->isHandlingOverConnection())
            {
                getPlayController()->resume();
            }
            break;
            
        case EIRQAccessPointSelectionCancelled : 
            cancelConnect();
            break;
            
        case EIRQNetworkConnectionDisconnected:
            {
                iDisconnected = true;
                iNetworkController->resetConnectionStatus();
                getPlayController()->stop(EIRQNoConnectionToNetwork);
                iNetworkController->notifyActiveNetworkObservers(aEvent);
            }
            break;
            
        case EIRQDisplayNetworkMessageNoConnectivity:
            {
                stopLoadingAnimation();
                HbMessageBox::warning(hbTrId("txt_irad_info_no_network_connectiion"), (QObject*)NULL, NULL);
                if (!iDisconnected)
                {
                    /* the handling is up to each view */
                    iNetworkController->notifyActiveNetworkObservers(EIRQDisplayNetworkMessageNoConnectivity);
                }                
            }
            break;
            
        default:
            break;
    }    
     
}

void IRApplication::loadGenre()
{
    LOG_SLOT_CALLER;
    bool hasCache = iIsdsClient->isdsIsCategoryCached(IRQIsdsClient::EGenre);

    if (!hasCache)
    {
        IRBaseView *currView = static_cast<IRBaseView*>(iViewManager->currentView());
        Q_ASSERT(currView);
        currView->setUseNetworkReason(EIR_UseNetwork_LoadCategory);
        if (false == verifyNetworkConnectivity())
        {
            return;
        }
        currView->setUseNetworkReason(EIR_UseNetwork_NoReason);
    }
    
    IRCategoryView *categoryView = static_cast<IRCategoryView*>(getViewManager()->getView(EIRView_CategoryView, true));
    categoryView->loadCategory(IRQIsdsClient::EGenre);
}

void IRApplication::newLocalSocketConnection()
{
    QLocalSocket *socket = iLocalServer->nextPendingConnection();
    if (!socket)
    {
        return;
    }
    delete socket;
    
    iViewManager->raise();
}

void IRApplication::getLoadingAnimation()
{
    HbIconAnimationManager::global()->addDefinitionFile("qtg_anim_loading.axml");
    iLoadingAnimation = new HbIconItem("qtg_anim_loading");
    iLoadingAnimation->hide();
    QGraphicsScene *targetScene = getViewManager()->scene();
    QGraphicsScene *oldScene = iLoadingAnimation->scene();

    if (targetScene != oldScene)
    {
        if (oldScene)
        {
            oldScene->removeItem(iLoadingAnimation);
        }
        targetScene->addItem(iLoadingAnimation); // takes ownership
    }

}

void IRApplication::initApp()
{
    getNetworkController();    
    
    IRBaseView *view = static_cast<IRBaseView*> (iViewManager->currentView());
    if (view)
    {
        view->lazyInit();
        view->launchAction();
    }
    getMediaKeyObserver();	
    startSystemEventMonitor();
    startLocalServer();
}

bool IRApplication::event(QEvent* e)
{
    if(e->type() == iInitEvent)
    {
        initApp();
        return true;        
    }
    
    return QObject::event(e);
}

TIRHandleResult IRApplication::handleConnectionEstablished()
{
    IRBaseView *currView = static_cast<IRBaseView*>(iViewManager->currentView());
    Q_ASSERT(currView);
    
    if (EIR_UseNetwork_LoadCategory == currView->getUseNetworkReason())
    {
        IRCategoryView *categoryView = static_cast<IRCategoryView*>(getViewManager()->getView(EIRView_CategoryView, true));
        categoryView->loadCategory(IRQIsdsClient::EGenre);
        currView->setUseNetworkReason(EIR_UseNetwork_NoReason);
        return EIR_NoDefault;
    }
    
    return EIR_DoDefault;
}

void IRApplication::handleTermsConsAccepted()
{
    iSettings->reSetFlagTermsAndConditions();
    IRBaseView * termsConsView = iViewManager->getView(EIRView_TermsConsView);
    iViewManager->removeView(termsConsView);
    termsConsView->deleteLater();
    
    iViewManager->activateView(iStartingViewId); 
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QEvent* initEvent = new QEvent(iInitEvent);
    QCoreApplication::postEvent(this, initEvent, Qt::HighEventPriority); 	 
}

void IRApplication::launchStartingView(TIRViewId aViewId)
{
    bool isFirstTimeUsage = false;
    iSettings->isFlagTermsAndConditions(isFirstTimeUsage);
    iStartingViewId = aViewId;
    if(isFirstTimeUsage)
    {
        iViewManager->activateView(EIRView_TermsConsView);  
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);    
    }
    else
    {
        iViewManager->activateView(iStartingViewId);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        QEvent* initEvent = new QEvent(iInitEvent);
        QCoreApplication::postEvent(this, initEvent, Qt::HighEventPriority);         
    }	
}

void IRApplication::setExitingView()
{
    if (XQServiceUtil::isService())
    {
        return;
    }    
    TIRViewId viewId = iViewManager->getExitingView();
    if(EIRView_InvalidId != viewId)
    {
        getSettings()->setStartingViewId(viewId);
    }
}

void IRApplication::startLocalServer()
{
    QString serverName = QCoreApplication::applicationName();
    iLocalServer = new QLocalServer(this);
    connect(iLocalServer, SIGNAL(newConnection()),
            this, SLOT(newLocalSocketConnection()));
    
    if (!iLocalServer->listen(serverName)) 
    {
        QAbstractSocket::SocketError error = iLocalServer->serverError();
        QString fullServerName;
        fullServerName = QDir::cleanPath(QDir::tempPath());
        fullServerName += QLatin1Char('/') + serverName;
        bool fileExists = QFile::exists(fullServerName);
        
        if (error == QAbstractSocket::AddressInUseError && fileExists) 
        {
            QFile::remove(fullServerName);
            bool ret = iLocalServer->listen(serverName);
            //following lines aim to fix coverity errors
            if (!ret)
            {
                Q_ASSERT(false);
            }
        }
    }
}

bool IRApplication::eventFilter(QObject *object, QEvent *event)
{
    bool eventWasConsumed = false;

    if (object->objectName() == ABSTRACT_LIST_VIEW_BASE_OBJECT_PLAYINGBANNER)
    {
        if (NULL == iViewManager->currentView())
        {
            return false;
        }
        
        if( (EIRView_PlayingView == static_cast<IRBaseView*>(iViewManager->currentView())->id()) \
        	||(EIRView_SearchView == static_cast<IRBaseView*>(iViewManager->currentView())->id()) )
        {
            return false;
        }    
	
        if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            eventWasConsumed = true;
            Q_ASSERT(iPlayController->isPlaying());
            IrAbstractListViewBase* listview = static_cast<IrAbstractListViewBase*>(iViewManager->currentView());
            listview->setPlayingBannerTextColor("qtc_multimedia_trans_pressed");
        }
        else if(event->type() == QEvent::GraphicsSceneMouseRelease)
        {
            eventWasConsumed = true;
            Q_ASSERT(iPlayController->isPlaying());
            IrAbstractListViewBase* listview = static_cast<IrAbstractListViewBase*>(iViewManager->currentView());
            listview->setPlayingBannerTextColor("qtc_multimedia_trans_normal");
            iViewManager->activateView(EIRView_PlayingView);
        }
    }
    return eventWasConsumed;
}

void IRApplication::startSystemEventMonitor()
{     
    connect(iSystemEventHandler, SIGNAL(diskSpaceLowNotification(qint64)), 
        this, SLOT(handleDiskSpaceLow(qint64)));    
    connect(iSystemEventHandler, SIGNAL(callActivated()), this, SLOT(handleCallActivated()));
    connect(iSystemEventHandler, SIGNAL(callDeactivated()), this, SLOT(handleCallDeactivated()));
    iSystemEventHandler->start();
}
    
void IRApplication::handleDiskSpaceLow(qint64 aCriticalLevel)
{
    Q_UNUSED(aCriticalLevel);
    HbDeviceMessageBox messageBox(hbTrId("txt_irad_info_no_space_on_c_drive_internet_radio_closed"),
            HbMessageBox::MessageTypeWarning);
    messageBox.setTimeout(HbPopup::NoTimeout);
    messageBox.exec();
    qApp->quit();    
}

void IRApplication::handleCallActivated()
{
    LOG_METHOD;
    //for the buffering state needs more attention, we firstly
    //don't handle it, improve in future.
    if( iPlayController->isPlaying() /*|| iPlayController->isBuffering()*/)
    {
        iPlayController->stop(EIRQCallIsActivated);
        stopLoadingAnimation();
    }
	
	//for we don't cancel the loading when call is activated, 
	//so, here , we don't add the handling for it. 
}

void IRApplication::handleCallDeactivated()
{
    LOG_METHOD;
    if( iPlayController->getStopReason() == EIRQCallIsActivated )
    {
        iPlayController->resume();
    }
}

void IRApplication::handleHeadsetConnected()
{
    LOG_METHOD;
}

void IRApplication::handleHeadsetDisconnected()
{
    LOG_METHOD;
}

#ifdef _DEBUG
void IRApplication::readConfiguration()
{
    QFile file("C:\\data\\QTIRConfigure.txt");
    if (file.open(QIODevice::ReadOnly)) 
    {
        QTextStream stream( &file );
        QString line;
        QStringList parameter;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            parameter = line.split("=");
            if (parameter.count() == 2)
            {
                if (parameter.first() == "testPreferredBitrate"
                    && parameter.last() == "true")
                {
                    iTestPreferredBitrate = true;
                }
            }
        }
    }
    file.close();
}
#endif
