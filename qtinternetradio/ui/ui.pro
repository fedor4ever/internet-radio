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
               ..\irqmusicshop\inc \
               ..\irqnwkinfoobserver\inc \
               ..\irqstatisticsreport\inc \
               ..\irqfavoritesdb\inc \
               ..\irqnetworkcontroller\inc \
               ..\irqsonghistory\inc \
               ..\irqsettings\inc    \
               ..\irqstationexporter\inc \
               ..\irqdiskspacewatcher\inc \	 
               ..\irqlogger\inc \                      
               ..\..\internetradio2.0\uiinc     #needed by irsystemeventobserver.h

CONFIG += hb
QT += sql
 
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
        -lirqmusicshop \
        -lirqnwkinfoobserver \
        -lirqstatisticsreport \
        -lirqdiskspacewatcher \	
        -lirqlogger \
        -lirqstationexporter
	
symbian{
  TARGET.UID3 = 0xE28364bb
  TARGET.EPOCHEAPSIZE = 0x020000 0x2000000
  TARGET.VID = VID_DEFAULT
  ICON = resources/irqradio.svg
  datatype_list = "datatype_list={DATATYPE{priority=EDataTypePriorityHigh;type=\"audio/x-scpls\";}," \
                                 "DATATYPE{priority=EDataTypePriorityHigh;type=\"audio/x-mpegurl\";}" \
                  "};"
  RSS_RULES += datatype_list 
}

#input
#header files list
HEADERS += inc\irapplication.h \
           inc\irabstractviewmanager.h \ 
           inc\irviewmanager.h \
           inc\irbaseview.h \
           inc\irmainview.h \
           inc\irmainmodel.h \
           inc\ircategoryview.h \      
           inc\irstationsview.h \  
           inc\irnowplayingview.h \
           inc\irnowplayingcontainer.h \
           inc\irplaycontroller.h \
           inc\ircategorymodel.h \
           inc\irchannelmodel.h \
           inc\irabstractlistviewbase.h \
           inc\irbannerlabel.h \
           inc\irfavoritesview.h \
           inc\irfavoritesmodel.h \
           inc\irhistoryview.h \
           inc\irsonghistoryview.h \
           inc\irsettingsview.h \
           inc\irlastplayedstationinfo.h \
           inc\irstationdetailscontainer.h \
           inc\iropenwebaddressview.h \
           inc\irhistorymodel.h \
           inc\irsonghistorymodel.h \
           inc\irsearchcriteriadb.h \
           inc\irsearchchannelsview.h \
           inc\irdocumentloader.h \
           inc\ircombobox.h \
           inc\irlineeditor.h \
           inc\irmediakeyobserver.h \
           inc\irsymbianapplication.h \
           inc\irsymbiandocument.h \
           inc\irsymbianappui.h \
           inc\irplsmodel.h \
           inc\irplsview.h \
           inc\irmemorycollector.h
           
#source files list
SOURCES += main.cpp  \
           irapplication.cpp \
           irabstractviewmanager.cpp \
           irviewmanager.cpp \
           irbaseview.cpp \
           irmainview.cpp \
           irmainmodel.cpp \
           ircategoryview.cpp \
           irstationsview.cpp \
           irnowplayingview.cpp \
           irnowplayingcontainer.cpp \
           irplaycontroller.cpp \
           ircategorymodel.cpp \
           irchannelmodel.cpp \
           irabstractlistviewbase.cpp \
           irbannerlabel.cpp \
           irfavoritesview.cpp \
           irfavoritesmodel.cpp \
           irhistoryview.cpp \
           irsonghistoryview.cpp \
           irsettingsview.cpp \
           irlastplayedstationinfo.cpp \
           irstationdetailscontainer.cpp \
           iropenwebaddressview.cpp \
           irhistorymodel.cpp \
           irsonghistorymodel.cpp \
           irsearchcriteriadb.cpp \
           irsearchchannelsview.cpp \
           irdocumentloader.cpp \
           ircombobox.cpp \
           irlineeditor.cpp \
           irmediakeyobserver.cpp \
           irsymbianapplication.cpp \
           irsymbiandocument.cpp \
           irsymbianappui.cpp \
           irplsmodel.cpp \
           irplsview.cpp \
           irmemorycollector.cpp
          	   
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
 
 
 
 