# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  

#include <platform_paths.hrh>
include(../common.pri)

TEMPLATE = app
TARGET = internet_radio_10_1
TARGET.CAPABILITY += NetworkServices  AllFiles  ReadDeviceData WriteDeviceData ReadUserData WriteUserData

MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(http)}
INCLUDEPATH += $${MW_LAYER_PUBLIC_EXPORT_PATH(http)}

INCLUDEPATH += inc \
               ..\irqisdsclient\inc \
               ..\irqmediaplayer\inc \
               ..\irqcommon\inc \
               ..\irqstatisticsreporter\inc \
               ..\irqfavoritesdb\inc \
               ..\irqnetworkcontroller\inc \
               ..\irqsonghistory\inc \
               ..\irqsettings\inc    \                
               ..\irqlogger\inc \                      
               ..\irqsystemeventhandler\inc

CONFIG += hb service
QT += sql

SERVICE.FILE = service_conf.xml

exists($${OS_LAYER_PLATFORM_EXPORT_PATH(qt/translations/internet_radio_10_1*.ts)}) {           
       TRANSLATIONS = internet_radio_10_1.ts
       message( "LOALIZATION IS TURNED ON" )   
       DEFINES += LOCALIZATION     
}


LIBS += -lirqnetworkcontroller \         
        -lirqisdsclient \
        -lirqmediaplayer \
        -lirqutility \
        -lirqfavorites \
        -lirqsonghistory \
        -lirqsettings \	    
        -lremconcoreapi \
        -lremconinterfacebase \
        -lirqstatisticsreporter \         
        -lirqlogger \
        -lshareui \
        -lirqsystemeventhandler \
	-lxqservice \
        -lxqserviceutil
        	
symbian{
  TARGET.UID3 = 0xE28364bb
  TARGET.EPOCHEAPSIZE = 0x020000 0x2000000
  TARGET.VID = VID_DEFAULT
  SKINICON = qtg_large_internet_radio
  datatype_list = "datatype_list={DATATYPE{priority=EDataTypePriorityHigh;type=\"audio/x-scpls\";}};"
  RSS_RULES += datatype_list 
}

#input
#header files list
HEADERS += inc\irapplication.h \
           inc\irviewmanager.h \
           inc\irbaseview.h \
           inc\irmainview.h \
           inc\irmainmodel.h \
           inc\ircategoryview.h \      
           inc\irstationsview.h \  
           inc\irnowplayingview.h \
           inc\irplaycontroller.h \
           inc\ircategorymodel.h \
           inc\irchannelmodel.h \
           inc\irabstractlistviewbase.h \
           inc\irfavoritesview.h \
           inc\irfavoritesmodel.h \
           inc\irhistoryview.h \
           inc\irsonghistoryview.h \
           inc\irsettingsview.h \
           inc\irlastplayedstationinfo.h \
           inc\iropenwebaddressview.h \
           inc\irhistorymodel.h \
           inc\irsonghistorymodel.h \            
           inc\irsearchchannelsview.h \
           inc\irdocumentloader.h \           
           inc\irmediakeyobserver.h \
           inc\irplsmodel.h \
           inc\irplsview.h \
           inc\irmemorycollector.h \
           inc\irstationshare.h \
           inc\irstationdetailsview.h \
           inc\irtermsconsview.h \
           inc\irplaylist.h \
           inc\irsearchresultdb.h \
           inc\irfileviewservice.h
           
#source files list
SOURCES += main.cpp  \
           irapplication.cpp \
           irviewmanager.cpp \
           irbaseview.cpp \
           irmainview.cpp \
           irmainmodel.cpp \
           ircategoryview.cpp \
           irstationsview.cpp \
           irnowplayingview.cpp \
           irplaycontroller.cpp \
           ircategorymodel.cpp \
           irchannelmodel.cpp \
           irabstractlistviewbase.cpp \
           irfavoritesview.cpp \
           irfavoritesmodel.cpp \
           irhistoryview.cpp \
           irsonghistoryview.cpp \
           irsettingsview.cpp \
           irlastplayedstationinfo.cpp \
           iropenwebaddressview.cpp \
           irhistorymodel.cpp \
           irsonghistorymodel.cpp \                   
           irsearchchannelsview.cpp \
           irdocumentloader.cpp \            
           irmediakeyobserver.cpp \
           irplsmodel.cpp \
           irplsview.cpp \
           irmemorycollector.cpp \
           irstationshare.cpp \
           irstationdetailsview.cpp \
           irtermsconsview.cpp \
           irplaylist.cpp \
           irsearchresultdb.cpp \
           irfileviewservice.cpp
          	   
#resource
RESOURCES += resources/irui.qrc

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }
 
 
 
 