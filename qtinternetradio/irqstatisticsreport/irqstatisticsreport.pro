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
# Description:  Project definition file for internet radio statisticsreport

TEMPLATE = lib

TARGET = irqstatisticsreport
TARGET.CAPABILITY = CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH += src

INCLUDEPATH += inc \
               ..\irqcommon\inc \
               ..\..\internetradio2.0\sessionloginc \
               ..\irqisdsclient\inc \
               ..\irqnetworkcontroller\inc \
               ..\irqsettings\inc

symbian{
  TARGET.UID3 = 0xe3801560
  TARGET.EPOCALLOWDLLDATA = 1
}

LIBS += -lirsessionlog \
        -lirqnetworkcontroller \
        -lirqisdsclient \
        -lirqsettings

# Input
HEADERS += inc\irqstatisticsreporter.h \
           inc\irqreportsender.h
           
SOURCES += irqstatisticsreporter.cpp \
           irqreportsender.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }