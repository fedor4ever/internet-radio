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

#include "irviewmanager.h"
#include "irapplication.h"
#include "irmemorycollector.h"
#include "irqlogger.h"
#include "irqsystemeventhandler.h"

#ifdef LOCALIZATION 
#include <QTranslator> 
void initLanguage(QTranslator*& aTranslator);
#endif

bool isDiskSpaceLow(IRQSystemEventHandler* aEventHandler);
bool isSecondInstance();

int main(int argc, char* argv[])
{
    INSTALL_MESSAGE_HANDLER;
    HbApplication app(argc, argv);
    
#ifdef LOCALIZATION    
    QTranslator* translator = NULL;
    initLanguage(translator);
    Q_ASSERT( NULL != translator );
#endif
    
    QCoreApplication::setApplicationName(hbTrId("txt_irad_title_internet_radio"));
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
        HbDeviceMessageBox messageBox(hbTrId("txt_irad_info_no_space_on_c_drive_internet_radio_closed"),
                HbMessageBox::MessageTypeWarning);
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
