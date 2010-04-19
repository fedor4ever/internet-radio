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
#ifndef IRAPPLICATION_H
#define IRAPPLICATION_H

#include <QObject>
#include <QEvent>
#include <hbglobal.h>
#include "irqevent.h"
#include "irviewdefinitions.h"

 

class IRAbstractViewManager;
class IRQIsdsClient;
class IRPlayController;     
class IRQSettings;
class IRMediaKeyObserver;
class IRLastPlayedStationInfo;
class IRQFavoritesDB;
class IRQNetworkController;
class IRQNwkInfoObserver;
class IRQStatisticsReporter;
class IRQMusicShop;
class IRQDiskSpaceWatcher;
class QLocalServer;
class IRQAdvClient;
class HbMessageBox;

#ifdef LOCALIZATION
class QTranslator;
#endif

class IRApplication : public QObject
{
    Q_OBJECT
    
public:
    explicit IRApplication(IRAbstractViewManager *aViewManager);
    ~IRApplication();
    
    bool verifyNetworkConnectivity(const QString &aConnectingText = hbTrId("Connecting to server..."));
    
    void createConnectingDialog();
    
    void closeConnectingDialog();
    
    IRAbstractViewManager* getViewManager() const;
    IRQNetworkController* getNetworkController();
    IRQIsdsClient* getIsdsClient();
    IRPlayController* getPlayController();
    IRLastPlayedStationInfo* getLastPlayedStationInfo();
    IRQFavoritesDB* getFavoritesDB();
    IRQSettings *   getSettings();
    IRMediaKeyObserver* getMediaKeyObserver();
    IRQNwkInfoObserver* getNwkInfoObserver();
    IRQStatisticsReporter* getStatisticsReporter();
    IRQMusicShop* getMusicShop();
    IRQAdvClient* getAdvClient();
	
    //from QObject
    bool event(QEvent* e);

#ifdef _DEBUG
public:
    bool iTestPreferredBitrate;
#endif

signals:
    void quit();

private slots:
    //connect to signal 'networkEventNotified' from IRQNetworkController
    void networkEventNotified(IRQNetworkEvent aEvent);
    void cancelConnect();
    void loadGenre();
    void newLocalSocketConnection();
    void handleDiskSpaceLow(qint64 aCriticalLevel);
    
private:
    void createComponents();
    void destroyComponents();
    void setupConnection();
    void setLaunchView();
    void launchStartingView();
    void startMonitoringNwkInfo();
    void startMonitoringDiskSpace();
    void initApp();
    void setExitingView();
    
    TIRHandleResult handleConnectionEstablished();
    
    void startLocalServer();
    
#ifdef LOCALIZATION
    void initLanguage();
#endif
    
#ifdef _DEBUG
    void readConfiguration();
#endif

private:
    IRAbstractViewManager* iViewManager;
    
    IRQNetworkController* iNetworkController;
    
    IRQIsdsClient* iIsdsClient;
    
    IRPlayController* iPlayController;
    
    IRQFavoritesDB* iFavPresets;
    
    IRQSettings      *iSettings;

    IRMediaKeyObserver *iMediaKeyObserver;

    IRLastPlayedStationInfo *iLastPlayedStationInfo;

    IRQNwkInfoObserver *iNwkInfoObserver;

    IRQStatisticsReporter *iStatisticsReporter;
    
    IRQMusicShop *iMusicShop;

    IRQAdvClient *iAdvertisementClient;

    IRQDiskSpaceWatcher *iDiskSpaceWatcher;

    bool iEnableGlobalAdv;

    bool iDisconnected;

    bool iConnectingCanceled;

    QEvent::Type iInitEvent;

    QString iConnectingText;
    
    QLocalServer *iLocalServer;
    
    HbMessageBox *iConnectingNote;
    
#ifdef LOCALIZATION
    QTranslator  *iTranslator;
#endif
};

#endif
