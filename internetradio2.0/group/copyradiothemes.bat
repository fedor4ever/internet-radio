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

SETLOCAL

SET THEME_DEST_C_UDEB_DIR=%EPOCROOT%epoc32\RELEASE\winscw\udeb\z\private\10207254\themes\271012080\270513751
SET THEME_DEST_C_UREL_DIR=%EPOCROOT%epoc32\RELEASE\winscw\urel\z\private\10207254\themes\271012080\270513751
SET THEME_DEST_Z_DIR=%EPOCROOT%epoc32\data\z\private\10207254\themes\271012080\270513751

echo Copying Active Idle binary themes
echo *********************************
if not exist %THEME_DEST_C_UDEB_DIR%\271063149\1.0\sources mkdir %THEME_DEST_C_UDEB_DIR%\271063149\1.0\sources
copy ..\activeidlethemes\binarythemes\activeidle\AI.* %THEME_DEST_C_UDEB_DIR%\271063149\1.0\
copy ..\activeidlethemes\themes\activeidle\*.mif %THEME_DEST_C_UDEB_DIR%\271063149\1.0\sources\
if not exist %THEME_DEST_C_UREL_DIR%\271063149\1.0\sources mkdir %THEME_DEST_C_UREL_DIR%\271063149\1.0\sources
copy ..\activeidlethemes\binarythemes\activeidle\AI.* %THEME_DEST_C_UREL_DIR%\271063149\1.0\
copy ..\activeidlethemes\themes\activeidle\*.mif %THEME_DEST_C_UREL_DIR%\271063149\1.0\sources\
if not exist %THEME_DEST_Z_DIR%\271063149\1.0\sources mkdir %THEME_DEST_Z_DIR%\271063149\1.0\sources
copy ..\activeidlethemes\binarythemes\activeidle\AI.* %THEME_DEST_Z_DIR%\271063149\1.0\
copy ..\activeidlethemes\themes\activeidle\*.mif %THEME_DEST_Z_DIR%\271063149\1.0\sources\

echo Copying Classic Idle binary themes
echo **********************************
if not exist %THEME_DEST_C_UDEB_DIR%\271063147\1.0\sources mkdir %THEME_DEST_C_UDEB_DIR%\271063147\1.0\sources
copy ..\activeidlethemes\binarythemes\classicidle\CI.* %THEME_DEST_C_UDEB_DIR%\271063147\1.0\
copy ..\activeidlethemes\themes\classicidle\*.mif %THEME_DEST_C_UDEB_DIR%\271063147\1.0\sources\
if not exist %THEME_DEST_C_UREL_DIR%\271063147\1.0\sources mkdir %THEME_DEST_C_UREL_DIR%\271063147\1.0\sources
copy ..\activeidlethemes\binarythemes\classicidle\CI.* %THEME_DEST_C_UREL_DIR%\271063147\1.0\
copy ..\activeidlethemes\themes\classicidle\*.mif %THEME_DEST_C_UREL_DIR%\271063147\1.0\sources\
if not exist %THEME_DEST_Z_DIR%\271063147\1.0\sources mkdir %THEME_DEST_Z_DIR%\271063147\1.0\sources
copy ..\activeidlethemes\binarythemes\classicidle\CI.* %THEME_DEST_Z_DIR%\271063147\1.0\
copy ..\activeidlethemes\themes\classicidle\*.mif %THEME_DEST_Z_DIR%\271063147\1.0\sources

ENDLOCAL
