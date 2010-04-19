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

TARGET = irqutility
TARGET.CAPABILITY += CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += ../../internetradio2.0/datastructuresinc \
               ../../internetradio2.0/presetplugininc

LIBS += -lirdatastructures \
        -lfbscli \
        -lcone \
        -lws32 \
        -lapgrfx \
        -lapmime
        
symbian{
  TARGET.UID3 = 0xef3c4675
  TARGET.EPOCALLOWDLLDATA = 1
}
       
#input

#header files list
HEADERS += inc\irqisdsdatastructure.h \
           inc\irqutility.h
                       

#source files list
SOURCES += irqisdsdatastructure.cpp \
           irqutility.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }