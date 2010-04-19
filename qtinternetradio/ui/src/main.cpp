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
#include "irviewmanager.h"
#include "irapplication.h"
#include "irqdiskspacewatcher.h"
#include "irmemorycollector.h"
#include "irsymbianapplication.h"
#include "irqlogger.h"

bool isDiskSpaceLow();

int main(int argc, char* argv[])
{
    INSTALL_MESSAGE_HANDLER;
    
    QCoreApplication::setApplicationName("InternetRadioApplication");
    HbApplication app(newS60Application, argc, argv);
    if (IRSymbianApplication::getInstance()->getInstanceFlag())
    {
        return 0;
    }
    
    if(isDiskSpaceLow())
    {       
        return 0;
    }  

    IRViewManager *mainWindow = new IRViewManager;
    
    mainWindow->show();
    
    //when constructing irapp, OpenFileL() has already been called
    IRApplication *irapp = new IRApplication(mainWindow);
    
    IRMemoryCollector mc(irapp);
    
    if(!mainWindow->isExiting())
    {
        return app.exec();
    }
	return 0;
}

bool isDiskSpaceLow()
{
    IRQDiskSpaceWatcher diskSpaceWatcher;
    bool ret = diskSpaceWatcher.isBelowCriticalLevel();
    if(ret)
    {
        HbDeviceMessageBox messageBox(hbTrId("txt_irad_info_no_space_on_c_drive_internet_radio_closed"),
                HbMessageBox::MessageTypeWarning);
        messageBox.setTimeout(HbPopup::NoTimeout);
        messageBox.exec();
    }
    return ret;
}

