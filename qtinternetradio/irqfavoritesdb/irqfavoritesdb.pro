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

TEMPLATE = lib

TARGET = irqfavorites
TARGET.CAPABILITY += CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(ecom)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(http)}
INCLUDEPATH += $${MW_LAYER_PUBLIC_EXPORT_PATH(http)}

INCLUDEPATH += ..\..\internetradio2.0\favoritesdbinc \
               ..\..\internetradio2.0\include \
               ..\..\internetradio2.0\settingsinc \
               ..\..\internetradio2.0\datastructuresinc \
               ..\..\internetradio2.0\presetplugininc  \
               ..\..\internetradio2.0\xmlparserinc   \
               ..\irqcommon\inc
             

               
               
LIBS += -lestor \
        -leuser \
        -lirdatastructures \
        -lirsettings \
        -lpsclient100 \
        -lirxmlparser \
        -lflogger \
        -lirfavoritesdb \
        -lirqutility

symbian{
  TARGET.UID3 = 0xecbde2d8
  TARGET.EPOCALLOWDLLDATA = 1
}


#header files list
HEADERS += inc\irqfavoritesdb.h  

#source files list
SOURCES += irqfavoritesdb.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }