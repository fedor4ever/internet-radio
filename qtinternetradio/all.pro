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

TEMPLATE = subdirs
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             "rom/qtinternetradio.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(qtinternetradio.iby)" 

 
exists($${OS_LAYER_PLATFORM_EXPORT_PATH(qt/translations/internet_radio_10_1*.ts)}) {
       BLD_INF_RULES.prj_exports += "rom/qtinternetradio_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(qtinternetradio_resources.iby)"  
       message( "LOALIZATION IS TURNED ON" )       
}                         
                           

 

SUBDIRS = irqlogger \
          irqcommon \
          irqsettings \
          irqfavoritesdb \
          irqisdsclient \
          irqmediaplayer \
          irqnetworkcontroller \
          irqsonghistory \
          irqmusicshop \
          irqnwkinfoobserver \
          irqstatisticsreport \
          irqdiskspacewatcher \
          irqstationexporter \
          ui 
          
SYMBIAN_PLATFORMS = WINSCW ARMV5
