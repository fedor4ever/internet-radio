# Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

DEFINES += SONGRECOGNITION_ENABLED
DEFINES += USER_DEFINED_DISKSPACE
DEFINES += USER_DEFINED_ISDSURL
#DEFINES += NOWPLAYING_VIEW_OPTION_B
#DEFINES += ADV_ENABLED

# ##########################################################
# Logging flag definitions
# ##########################################################

# Enable logging in release builds
# LOGGING_FLAGS += LOGGING_ENABLED

# Combines Ui and Engine logs
#  LOGGING_FLAGS += COMBINE_WITH_ENGINE_LOGGER

# Writes debug prints to file if enabled
#  LOGGING_FLAGS += TRACE_TO_FILE

DEFINES += $$LOGGING_FLAGS
