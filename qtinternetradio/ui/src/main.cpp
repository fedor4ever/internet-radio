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

#include <hbapplication.h>
#include <hbdevicemessagebox.h>
#include <QLocalSocket>
#include <hbsplashscreen.h>
#include <QSettings>

#include "irviewmanager.h"
#include "irapplication.h"
#include "irmemorycollector.h"
#include "irqlogger.h"
#include "irqsystemeventhandler.h"
#include "irqsettings.h"
#include "irservicedef.h"

#ifdef LOCALIZATION 
#include <QTranslator> 
void initLanguage(QTranslator*& aTranslator);
#endif

bool isDiskSpaceLow(IRQSystemEventHandler* aEventHandler);
bool isSecondInstance();
void setSplashScreen();
bool isSplashNowplaying();

int main(int argc, char* argv[])
{
    installLogDir();
    
    INSTALL_MESSAGE_HANDLER;
    //the following is for splash screen, this must be placed 
    //before the creating of HbApplication
    setSplashScreen(); 
    
    HbApplication app(argc, argv);
    
#ifdef LOCALIZATION    
    QTranslator* translator = NULL;
    initLanguage(translator);
    Q_ASSERT( NULL != translator );
#endif
    
#ifdef SUBTITLE_STR_BY_LOCID
    QCoreApplication::setApplicationName(hbTrId("txt_irad_title_internet_radio"));
#else
    QCoreApplication::setApplicationName(hbTrId("Internet radio"));    
#endif
    if (isSecondInstance())
    {
        return 0;
    }
    
    //for the note should be translated too     
    IRQSystemEventHandler *systemEventHandler = new IRQSystemEventHandler();
    if( isDiskSpaceLow(systemEventHandler) )
    {       
#ifdef LOCALIZATION   
        qApp->removeTranslator(translator);
        delete translator;
#endif
        delete systemEventHandler;
        return 0;
    }

    IRViewManager *mainWindow = new IRViewManager;    
    mainWindow->show();
    
    //here, we transfer the ownership of the event handler to the irapp
    IRApplication *irapp = new IRApplication(mainWindow, systemEventHandler);
    
#ifdef LOCALIZATION
    //this function will transfer the ownership of translator to irapp
    irapp->setTranslator(translator);
#endif
    
    IRMemoryCollector mc(irapp);
    
    if(!mainWindow->isExiting())
    {
        return app.exec();
    }
	return 0;
}

bool isDiskSpaceLow(IRQSystemEventHandler* aEventHandler)
{     
    bool ret = aEventHandler->isBelowCriticalLevel();
    if(ret)
    {
#ifdef SUBTITLE_STR_BY_LOCID
        HbDeviceMessageBox messageBox(hbTrId("txt_irad_info_insufficient_disk_space"),
                HbMessageBox::MessageTypeWarning);
#else
        HbDeviceMessageBox messageBox(hbTrId("Insufficient disk space"),
                HbMessageBox::MessageTypeWarning);        
#endif
        messageBox.setTimeout(HbPopup::NoTimeout);
        messageBox.exec();
    }
    return ret;
}

#ifdef LOCALIZATION 
void initLanguage(QTranslator*& aTranslator)
{
    aTranslator = new QTranslator();    
    QString lang = QLocale::system().name();
    LOG_FORMAT( "Current language is %s", STRING2CHAR(lang) );
    QString path = "z:/resource/qt/translations/";  //this will be changed to a micro in future
    bool ret = aTranslator->load(path + "internet_radio_10_1_" + lang);        
    qApp->installTranslator(aTranslator); 
}
#endif

bool isSecondInstance()
{
    QLocalSocket socket;
    QString serverName = QCoreApplication::applicationName();
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void setSplashScreen()
{     
    if( isSplashNowplaying() )
    {
        HbSplashScreen::setScreenId("nowplaying_screen");
    }
    else
    {
        HbSplashScreen::setScreenId("normal_screen");
    }    
}

bool isSplashNowplaying()
{
    QSettings settings(KIrSettingOrganization, KIrSettingApplication);
    return settings.value(KIrSettingSplashNowplaying,false).toBool();
}
 
