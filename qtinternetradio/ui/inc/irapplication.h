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

#include <QEvent>
#include <hbglobal.h>
#include <xqserviceprovider.h>

#include "irqevent.h"
#include "irviewdefinitions.h"

 

class IRViewManager;
class IRQIsdsClient;
class IRPlayController;     
class IRQSettings;
class IRMediaKeyObserver;
class IRLastPlayedStationInfo;
class IRQFavoritesDB;
class IRQNetworkController;
class IRQDiskSpaceWatcher;
class QLocalServer;
class IRQAdvClient;
class HbProgressDialog;
class IRQSystemEventHandler;
class XQSharableFile;
class IRPlayList;

#ifdef LOCALIZATION
class QTranslator;
#endif

class IRApplication : public XQServiceProvider
{
    Q_OBJECT
    
public:
    IRApplication(IRViewManager *aViewManager, IRQSystemEventHandler* aSystemEventHandler);    

    ~IRApplication();
    
    bool verifyNetworkConnectivity(const QString &aConnectingText = hbTrId("txt_common_info_loading"));
    
    void createLoadingDialog(const QObject *aReceiver, const char *aFunc);
    
    void closeLoadingDialog();
    
    IRViewManager* getViewManager() const;
    IRQNetworkController* getNetworkController();
    IRQIsdsClient* getIsdsClient();
    IRPlayController* getPlayController();
    IRLastPlayedStationInfo* getLastPlayedStationInfo();
    IRQFavoritesDB* getFavoritesDB();
    IRQSettings *   getSettings();
    IRMediaKeyObserver* getMediaKeyObserver();
    IRQAdvClient* getAdvClient(); 
    IRPlayList* getPlayList() const;
    
#ifdef LOCALIZATION
    /*
     * this function will take ownership of the translator
     */
    void setTranslator(QTranslator* aTranslator);
#endif
	
    //from QObject
    bool event(QEvent* e);

#ifdef _DEBUG
public:
    bool iTestPreferredBitrate;
#endif

public slots:
    void view(const QString &aFileName);
    void view(const XQSharableFile &aSharableFile);
    
signals:
    void quit();

private slots:
    //connect to signal 'networkEventNotified' from IRQNetworkController
    void networkEventNotified(IRQNetworkEvent aEvent);
    void cancelConnect();
    void loadGenre();
    void newLocalSocketConnection();
    void handleDiskSpaceLow(qint64 aCriticalLevel);
    void handleTermsConsAccepted();
    void handleCallActivated();
    void handleCallDeactivated();
    void handleHeadsetConnected();
    void handleHeadsetDisconnected();
    
private:
    void createComponents();
    void destroyComponents();
    void setupConnection();
    void setLaunchView();
    void launchStartingView(TIRViewId aViewId);
    void startSystemEventMonitor();
    void initApp();
    void setExitingView();
    
    TIRHandleResult handleConnectionEstablished();
    
    void startLocalServer();
    bool eventFilter(QObject *object, QEvent *event);
    
#ifdef _DEBUG
    void readConfiguration();
#endif

private:
    IRViewManager* iViewManager;
    
    TIRViewId   iStartingViewId;
    
    IRQNetworkController* iNetworkController;
    
    IRQIsdsClient* iIsdsClient;
    
    IRPlayController* iPlayController;
    
    IRQFavoritesDB* iFavPresets;
    
    IRQSettings      *iSettings;

    IRMediaKeyObserver *iMediaKeyObserver;

    IRLastPlayedStationInfo *iLastPlayedStationInfo;
    
    IRQAdvClient *iAdvertisementClient;   

    bool iEnableGlobalAdv;

    bool iDisconnected;

    bool iConnectingCanceled;

    QEvent::Type iInitEvent;

    QString iConnectingText;
    
    QLocalServer *iLocalServer;
    
    HbProgressDialog *iLoadingNote;
    
#ifdef LOCALIZATION
    QTranslator  *iTranslator;
#endif
    
    IRQSystemEventHandler *iSystemEventHandler;
    
    IRPlayList *iPlayList;
};

#endif
