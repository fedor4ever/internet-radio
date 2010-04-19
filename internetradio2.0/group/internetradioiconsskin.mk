#
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
#
#=====================================================================
# Name : InternetRadioIcons.mk
# Part of : InternetRadio
#
# Description:
#
#
#=====================================================================
#
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif
# -------------------------------------------------------------------
# TODO: Configure these.
# -------------------------------------------------------------------

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\internetradio.mif
HEADERFILENAME=$(HEADERDIR)\internetradio.mbg

do_nothing : 
	@rem do_nothing
		
MAKMAKE : do_nothing

BLD : do_nothing
		
CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# -------------------------------------------------------------------
# TODO: Configure these.
#
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included
# automatically by MifConv if the mask depth is defined.
# -------------------------------------------------------------------
RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) /Ficonlist.txt

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
