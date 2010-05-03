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
#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QTextStream>
#include <QDir>
 

#ifdef LOCALIZATION 
#include <QTranslator>
#include "irqutility.h" 
#endif

#include "irabstractviewmanager.h"
#include "irapplication.h"

#include "irqnetworkcontroller.h"
#include "irqisdsclient.h"
#include "irplaycontroller.h"
#include "irlastplayedstationinfo.h"
#include "irqfavoritesdb.h"
#include "irqsettings.h"  
#include "irmediakeyobserver.h"
#include "ircategoryview.h"
#include "irstationsview.h"
#include "irqmusicshop.h"
#include "irqnwkinfoobserver.h"
#include "irqstatisticsreporter.h"
#include "irsymbiandocument.h"
#include "irqdiskspacewatcher.h"
#include "irqlogger.h"
/*
 * Description : constructor, initialize all data members
 * Parameters  : aViewManager : pointer to the view manager object
 * Return      : None
 */
IRApplication::IRApplication(IRAbstractViewManager* aViewManager):
#ifdef _DEBUG
                                     iTestPreferredBitrate(false),
#endif
                                     iViewManager(aViewManager),
                                     iNetworkController(NULL),    
                                     iIsdsClient(NULL),  
                                     iPlayController(NULL),  
                                     iFavPresets(NULL),                                     
                                     iSettings(NULL),
                                     iMediaKeyObserver(NULL),                                                                                                                                                                                                                     
                                     iLastPlayedStationInfo(NULL),
                                     iNwkInfoObserver(NULL),
                                     iStatisticsReporter(NULL),
                                     iMusicShop(NULL),
                                     iAdvertisementClient(NULL),
                                     iDiskSpaceWatcher(NULL),
                                     iEnableGlobalAdv(true),
                                     iDisconnected(false),
                                     iConnectingCanceled(false),
                                     iLocalServer(NULL),
                                     iConnectingNote(NULL)
                                     #ifdef LOCALIZATION
                                     ,iTranslator(NULL)
                                     #endif
{
    LOG_METHOD;
    iViewManager->setApplication(this);

#ifdef _DEBUG
    readConfiguration();
#endif

    createComponents();
    // get advertisement setting
    iSettings->getGlobalAdvFlag(iEnableGlobalAdv);
    setupConnection();
    setLaunchView();
#ifdef LOCALIZATION
    initLanguage();
#endif
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
    
    delete iConnectingNote;
    iConnectingNote = NULL;
    
#ifdef LOCALIZATION
    if( iTranslator )
    {
        qApp->removeTranslator(iTranslator);
        delete iTranslator;
        iTranslator = NULL;
    }
#endif
}

#ifdef LOCALIZATION
void IRApplication::initLanguage()
{
    iTranslator = new QTranslator();    
    QString lang = QLocale::system().name();
    LOG_FORMAT( "Current language is %s", STRING2CHAR(lang) );
    QString path = "z:/resource/qt/translations/";  //this will be changed to a micro in future
    bool ret = iTranslator->load(path + "internet_radio_10_1_" + lang);        
    qApp->installTranslator(iTranslator); 
}
#endif
/*
 * Description : When ir application is launched, set the correct view according to whether the 
 *               application is used for the first time
 * Parameters  : None
 * Return      : None
 */
void IRApplication::setLaunchView()
{
    IRSymbianDocument *document = IRSymbianDocument::getInstance();
    if (document && document->isPlsAvailable())
    {
        iViewManager->activateView(EIRView_PlsView);
    }
    else
    {
        launchStartingView();
    }
    
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    
    iInitEvent = static_cast<QEvent::Type>(QEvent::registerEventType());
    QEvent* initEvent = new QEvent(iInitEvent);
    QCoreApplication::postEvent(this, initEvent, Qt::HighEventPriority);
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

void IRApplication::createConnectingDialog()
{
    LOG_METHOD_ENTER;
    if (NULL == iConnectingNote)
    {
        iConnectingNote = new HbMessageBox(HbMessageBox::MessageTypeInformation);
        iConnectingNote->setModal(true);
        iConnectingNote->setTimeout(HbPopup::NoTimeout);        
        QAction *action = iConnectingNote->actions().at(0);
        action->setText(hbTrId("txt_common_button_cancel"));
        connect(action, SIGNAL(triggered()), this, SLOT(cancelConnect()));
    }
    
    iConnectingNote->setText(iConnectingText);
    iConnectingNote->show();
}

void IRApplication::closeConnectingDialog()
{
    LOG_METHOD_ENTER;
    if (iConnectingNote)
    {
        iConnectingNote->close();
        delete iConnectingNote;
        iConnectingNote = NULL;
    }
}

/*
 * Description : return the pointer to the view manager object
 * Parameters  : None
 * Return      : the pointer to the view manager object
 */
IRAbstractViewManager* IRApplication::getViewManager() const
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
        iIsdsClient = IRQIsdsClient::openInstance(getFavoritesDB()); 
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

IRQNwkInfoObserver * IRApplication::getNwkInfoObserver()
{
    if(NULL == iNwkInfoObserver)
    {
        iNwkInfoObserver = IRQNwkInfoObserver::openInstance();
    }
    return iNwkInfoObserver;
}

IRQStatisticsReporter * IRApplication::getStatisticsReporter()
{
    if(NULL == iStatisticsReporter)
    {
        iStatisticsReporter = IRQStatisticsReporter::openInstance(getIsdsClient());
    }
    return iStatisticsReporter;
}

IRQMusicShop * IRApplication::getMusicShop()
{
    if(NULL == iMusicShop)
    {
        iMusicShop = IRQMusicShop::openInstance();
    }
    return iMusicShop;
}

IRQAdvClient* IRApplication::getAdvClient()
{
    if(iEnableGlobalAdv && (NULL == iAdvertisementClient))
    {
        //iAdvertisementClient = IRQAdvClient::openInstance();
    }

    return iAdvertisementClient;
}
/*
 * Description : create all the application level components, including network controller,
 *               favorites db, isds client, play controller, etc
 * Parameters  : None
 * Return      : None
 */
void IRApplication::createComponents()
{
    getNetworkController();
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
    
	delete iDiskSpaceWatcher;
    iDiskSpaceWatcher = NULL;
	    
    if(iIsdsClient)
    {    
        iIsdsClient->closeInstance();
        iIsdsClient = NULL;
    }
    
    delete iFavPresets;
    iFavPresets = NULL;
    
    if(iStatisticsReporter)
    {
        iStatisticsReporter->closeInstance();
        iStatisticsReporter = NULL;
    }
    
    if(iNwkInfoObserver)
    {
        iNwkInfoObserver->closeInstance();
        iNwkInfoObserver = NULL;
    }
    
    if(iMusicShop)
    {
        iMusicShop->closeInstance();
        iMusicShop = NULL;
    }
    
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
    //connect IRApplication to IRQNetworkController
    connect(iNetworkController, SIGNAL(networkEventNotified(IRQNetworkEvent)), 
            this, SLOT(networkEventNotified(IRQNetworkEvent)));

    connect(this, SIGNAL(quit()), qApp, SLOT(quit()));
}

void IRApplication::cancelConnect()
{
    if (iConnectingCanceled)
    {
        return;
    }
    
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

// ---------------------------------------------------------------------------
//  IRApplication::startMonitoringNwkInfo
//  Start to monitor network info changes
// ---------------------------------------------------------------------------
//
void IRApplication::startMonitoringNwkInfo()
{
    getNwkInfoObserver();
    getStatisticsReporter();
    if(iNwkInfoObserver)
    {
        iNwkInfoObserver->startMonitorNwkInfo();	
        connectAndAssert(iNwkInfoObserver, SIGNAL(currentNwkChanged(const QString, const QString)), 
            iStatisticsReporter, SLOT(updateCurrentNetwork(const QString, const QString)));
        connectAndAssert(iNwkInfoObserver, SIGNAL(homeNwkChanged(const QString, const QString)),
            iStatisticsReporter, SLOT(updateHomeOperator(const QString, const QString)));   	
    }	
 
}
   
//connect to signal 'networkEventNotified' from IRQNetworkController
void IRApplication::networkEventNotified(IRQNetworkEvent aEvent)
{
    LOG_SLOT_CALLER;
    switch (aEvent)
    {
        case EIRQNetworkConnectionConnecting :
            createConnectingDialog();
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
            closeConnectingDialog();
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
                closeConnectingDialog();
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
    
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    QString fileName;
    stream>>fileName;
    IRSymbianDocument *document = IRSymbianDocument::getInstance();
    document->parseFile(fileName);
    
    if (document->isPlsAvailable())
    {
        iViewManager->activateView(EIRView_PlsView);
    }
    else
    {
        IRBaseView *topView = static_cast<IRBaseView*>(iViewManager->currentView());
        topView->updateView();
    }
    
    iViewManager->raise();
    delete socket;
}

void IRApplication::initApp()
{
    if (!iIsdsClient->isdsIsConstructSucceed())
    {
        HbMessageBox::warning("No available access points, exiting...", (QObject*)NULL, NULL);
        qApp->quit();
    }
    else
    {
        static_cast<IRBaseView*>(iViewManager->currentView())->launchAction();
    }
    
    getMediaKeyObserver();	
    startMonitoringNwkInfo();
    startMonitoringDiskSpace();
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
        closeConnectingDialog();
        IRCategoryView *categoryView = static_cast<IRCategoryView*>(getViewManager()->getView(EIRView_CategoryView, true));
        categoryView->loadCategory(IRQIsdsClient::EGenre);
        currView->setUseNetworkReason(EIR_UseNetwork_NoReason);
        return EIR_NoDefault;
    }
    
    return EIR_DoDefault;
}

void IRApplication::launchStartingView()
{
    TIRViewId viewId = EIRView_CategoryView;
    iSettings->getStartingViewId(viewId);
    TIRViewParameter viewParameter = EIRViewPara_Genre;
    iSettings->getStartingViewParameter(viewParameter);

    iViewManager->activateView(viewId);
    IRBaseView *currentView = iViewManager->getView(viewId);
        
    switch (viewId)
    {
    case EIRView_CategoryView:
    {
        IRCategoryView* categoryView = static_cast<IRCategoryView*> (currentView);
        categoryView->setViewParameter(viewParameter);

        switch (viewParameter)
        {
        case EIRViewPara_Genre:
            categoryView->setHeadingText(tr("Genre"));
            break;

        case EIRViewPara_Language:
            categoryView->setHeadingText(tr("Language"));
            break;

        case EIRViewPara_Country:
            categoryView->setHeadingText(tr("Country/Region"));
            break;

        default:
            break;
        }
    }
        break;

    case EIRView_PlayingView:
        iViewManager->pushViewById(EIRView_MainView);
        break;

    default:
        break;
    }   
}

void IRApplication::setExitingView()
{
    IRQSettings *settings = getSettings();
    bool flag = false;
    settings->getStickyViewFlag(flag);
    if (!flag)
    {
        return;
    }

    TIRViewId viewId = iViewManager->currentViewId();

    switch (viewId)
    {
      case EIRView_CategoryView:
      {
          IRCategoryView *topView = (IRCategoryView*)(iViewManager->currentView());
          TIRViewParameter parameter = topView->getViewParameter();
          settings->setStartingViewParameter(parameter);
      }
      break;
      
      case EIRView_StationsView:
      {
          IRStationsView *topView = (IRStationsView*)(iViewManager->currentView());
          TIRViewParameter parameter = topView->getViewParameter();
          if (parameter == EIRViewPara_PopularStations)
          {
              settings->setStartingViewParameter(EIRViewPara_PopularStations);
          }
          else if (parameter == EIRViewPara_CategoryStations)
          {
              IRBaseView *baseView = iViewManager->getView(EIRView_CategoryView, false);
              if (baseView)
              {
                  IRCategoryView *categoryView = (IRCategoryView*)baseView;
                  TIRViewParameter parameter = categoryView->getViewParameter();
                  settings->setStartingViewParameter(parameter);
                  settings->setStartingViewId(EIRView_CategoryView);
                  return;
              }
          }
          else if (parameter == EIRViewPara_SearchResults)
          {
                
          }
      }
      break;
      
      case EIRView_InvalidId:
      case EIRView_StationDetailsView:
      case EIRView_SettingsView:
      case EIRView_OpenWebAddressView:
      case EIRView_SearchView:
      case EIRView_SearchResultView:
      case EIRView_PlsView:    
      case EIRView_HistoryView:
      case EIRView_SongHistoryView:
      case EIRView_ViewCount:
      {
          // these views are not starting view, so do not change settings.
          return;
      }
      
      default:
          break;
    }

    settings->setStartingViewId(viewId);
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

void IRApplication::startMonitoringDiskSpace()
{
    if(NULL == iDiskSpaceWatcher)
    {
        iDiskSpaceWatcher = new IRQDiskSpaceWatcher();
    }
    
    if(NULL != iDiskSpaceWatcher)
    {
        connect(iDiskSpaceWatcher, SIGNAL(diskSpaceLow(qint64)), 
            this, SLOT(handleDiskSpaceLow(qint64)));        
        iDiskSpaceWatcher->startMonitorDrive();
    }
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
