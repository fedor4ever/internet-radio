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
TARGET = irqsonghistory
TARGET.CAPABILITY = CAP_GENERAL_DLL

defFilePath = ..

DEPENDPATH += src ../../internetradio2.0/irsonghistory/src
LIBS += -lbafl -ledbms -lefsrv -lirsettings -lflogger

INCLUDEPATH += inc \
               ../irqmediaplayer/inc \
               ../irqcommon/inc \
               ../../internetradio2.0/irsettings/inc \
               ../../internetradio2.0/inc \
               ../../internetradio2.0/irsonghistory/inc \

MOC_DIR = moc

symbian{
  TARGET.UID3 = 0x2002FFC3
  TARGET.EPOCALLOWDLLDATA = 1
  MMP_RULES += SMPSAFE
}

# Input
HEADERS += inc/irqsonghistoryengine.h \
           inc/irqsonghistoryinfo.h

SOURCES += irqsonghistoryengine.cpp \
           irqsonghistoryinfo.cpp \
           irsonghistorydb.cpp \
           irsonghistoryinfo.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }