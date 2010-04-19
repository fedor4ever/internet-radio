rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@echo off

REM 
REM This is a temporary way to localise Internet Radio into SIS.
REM This script is only for Internet Radio development and testing and is dependent on hardcoded paths,
REM languages and files, so it is only for internal use.
REM 

setlocal

set _MMP=internetradio.mmp
set _MMP_ORIG=%_MMP%.saved
set _MMP_NO_LANG=%_MMP%.tmp


REM Save original internetradio.mmp and avkon.loc
move %_MMP% %_MMP_ORIG%
copy \epoc32\include\avkon.loc \epoc32\include\avkon.loc.saved


REM Copy the mmp without the LANG tag
FOR /F "tokens=1*" %%A IN (%_MMP_ORIG%) DO (
  IF NOT %%A==LANG (
    echo %%A %%B >> %_MMP_NO_LANG%
  )
)

REM Delete existing compiled resource files
IF EXIST \epoc32\data\z\system\apps\internetradio\internetradio.r??? (
  del \epoc32\data\z\system\apps\internetradio\internetradio.r???
)


REM Go through all localisation directories and build resources with the corresponding language.
REM For each language, use the corresponding localised avkon.loc
FOR /F %%i IN ('dir /AD /B ..\resources') DO (
  IF EXIST \s60\s60LocFiles\%%i\avkon_%%i.loc (
    echo.
    echo LOCALISING LANGUAGE %%i
    copy \s60\s60locfiles\%%i\avkon_%%i.loc \epoc32\include\avkon.loc
    copy %_MMP_NO_LANG% %_MMP%
    echo LANG %%i >> %_MMP%
    call abld makefile armv5
    call abld resource armv5 urel
  ) ELSE (
  IF EXIST \s60\misc\release\s60locfiles\%%i\avkon_%%i.loc ( 
    echo.
    echo LOCALISING LANGUAGE %%i
    copy \s60\misc\release\s60locfiles\%%i\avkon_%%i.loc \epoc32\include\avkon.loc
    copy %_MMP_NO_LANG% %_MMP%
    echo LANG %%i >> %_MMP%
    call abld makefile armv5
    call abld resource armv5 urel
  ) ELSE (
    echo.
    echo \s60\s60locfiles\%%i\avkon_%%i.loc doesn't exist!
    echo \s60\s60locfiles\%%i\avkon_%%i.loc doesn't exist! >> LOCALISATION_ERRORS.txt
    echo.
    REM pause
  )
  )
)


REM Delete temp files and restore originals
move \epoc32\include\avkon.loc.saved \epoc32\include\avkon.loc
move %_MMP_ORIG% %_MMP%
del %_MMP_NO_LANG%


REM build makefiles again with the original mmp
call abld makefile armv5


endlocal
