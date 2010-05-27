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

TARGET = irqisdsclient
TARGET.CAPABILITY += CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(libc)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(ecom)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(http)}
INCLUDEPATH += $${MW_LAYER_PUBLIC_EXPORT_PATH(http)}       
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(mmf/common)}
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(xml)}

INCLUDEPATH += ..\..\internetradio2.0\settingsinc \
               ..\..\internetradio2.0\datastructuresinc \
               ..\..\internetradio2.0\favoritesdbinc \
               ..\..\internetradio2.0\networkcontrollerinc \
               ..\..\internetradio2.0\isdsclientinc  \
               ..\..\internetradio2.0\resources \
               ..\..\internetradio2.0\include  \
               ..\..\internetradio2.0\dataproviderinc \
               ..\..\internetradio2.0\cachemgmtinc \
               ..\..\internetradio2.0\xmlparserinc \
               ..\..\internetradio2.0\sessionloginc \
               ..\..\internetradio2.0\settingsinc \
               ..\irqcommon\inc \
               ..\irqfavoritesdb\inc \
               ..\irqlogger\inc
               
LIBS += -lirdatastructures \
        -lirfavoritesdb \
        -lirqfavorites \
        -lirnetworkcontroller \
        -lirsettings \
        -lisdsclientinterface \
        -lirdataprovider \
        -lflogger \
        -lirqlogger \
        -lirqutility       
  

symbian{
  TARGET.UID3 = 0xec6ecb67
  TARGET.EPOCALLOWDLLDATA = 1
}

#input

#header files list
HEADERS += inc\irqisdsclient.h \
           inc\irqisdsclientimpl.h

#source files list
SOURCES += irqisdsclient.cpp \
           irqisdsclientimpl.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

include(../common.pri)

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }