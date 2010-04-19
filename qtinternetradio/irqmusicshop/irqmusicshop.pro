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

TARGET = irqmusicshop
TARGET.CAPABILITY = CAP_GENERAL_DLL

MOC_DIR = moc
DEPENDPATH += src

INCLUDEPATH += inc 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

symbian{
  TARGET.UID3 = 0xe2480ba4
  TARGET.EPOCALLOWDLLDATA = 1
}

# DEFINES += MUSICSHOP_AVAILABLE

LIBS += -lapgrfx \
        -lws32 \
        -lcentralrepository \
        -lecom

# Input
HEADERS += inc\irqmusicshop.h
SOURCES += irqmusicshop.cpp

include(../common.pri)

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }