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

SET THEME_INSTALL_DIR1=%EPOCROOT%epoc32\winscw\C\Data\themeinstalltemp\activeidle
SET THEME_INSTALL_DIR3=%EPOCROOT%epoc32\winscw\C\Data\themeinstalltemp\classicidle

SET THEME_INSTALLED_DIR=%EPOCROOT%epoc32\winscw\c\private\10207254\themes\271012080\270513751

SET THEME_DEST_C_UDEB_DIR=%EPOCROOT%epoc32\RELEASE\winscw\udeb\z\private\10207254\themes\271012080\270513751
SET THEME_DEST_C_UREL_DIR=%EPOCROOT%epoc32\RELEASE\winscw\urel\z\private\10207254\themes\271012080\270513751
SET THEME_DEST_Z_DIR=%EPOCROOT%epoc32\data\z\private\10207254\themes\271012080\270513751

if not exist %THEME_INSTALL_DIR1% mkdir %THEME_INSTALL_DIR1%
if not exist %THEME_INSTALL_DIR3% mkdir %THEME_INSTALL_DIR3%
if not exist %THEME_INSTALL_DIR1%\Loc mkdir %THEME_INSTALL_DIR1%\loc
if not exist %THEME_INSTALL_DIR3%\Loc mkdir %THEME_INSTALL_DIR3%\loc

echo Copying Active Idle theme files
echo *******************************
copy ..\activeidlethemes\themes\activeidle\activeidle_dat.dat %THEME_INSTALL_DIR1%
copy ..\activeidlethemes\themes\activeidle\*.dtd %THEME_INSTALL_DIR1%
copy ..\activeidlethemes\themes\activeidle\*.xml %THEME_INSTALL_DIR1%
copy ..\activeidlethemes\themes\activeidle\*.css %THEME_INSTALL_DIR1%
copy ..\activeidlethemes\themes\activeidle\*.mif %THEME_INSTALL_DIR1%
copy ..\activeidlethemes\themes\activeidle\*.svg %THEME_INSTALL_DIR1%

echo Copying Active Idle localized theme files
echo *****************************************
copy ..\activeidlethemes\themes\activeidle\activeidle_loc.dat %THEME_INSTALL_DIR1%\loc\
copy ..\activeidlethemes\themes\activeidle\*.xml %THEME_INSTALL_DIR1%\loc
copy ..\activeidlethemes\themes\activeidle\*.css %THEME_INSTALL_DIR1%\loc

echo Copying Classic Idle theme files
echo ********************************
copy ..\activeidlethemes\themes\classicidle\classicidle_dat.dat %THEME_INSTALL_DIR3%
copy ..\activeidlethemes\themes\classicidle\*.dtd %THEME_INSTALL_DIR3%
copy ..\activeidlethemes\themes\classicidle\*.xml %THEME_INSTALL_DIR3%
copy ..\activeidlethemes\themes\classicidle\*.css %THEME_INSTALL_DIR3%
copy ..\activeidlethemes\themes\classicidle\*.mif %THEME_INSTALL_DIR3%
copy ..\activeidlethemes\themes\classicidle\*.svg %THEME_INSTALL_DIR3%

echo Copying Classic Idle localized theme files
echo ******************************************
copy ..\activeidlethemes\themes\classicidle\classicidle_loc.dat %THEME_INSTALL_DIR3%\loc\
copy ..\activeidlethemes\themes\classicidle\*.xml %THEME_INSTALL_DIR3%\loc
copy ..\activeidlethemes\themes\classicidle\*.css %THEME_INSTALL_DIR3%\loc

if not exist %EPOCROOT%epoc32\winscw\c\logs\themeinstallercons mkdir %EPOCROOT%epoc32\winscw\c\Logs\themeinstallercons

echo Installing Active Idle theme files
echo **********************************
%EPOCROOT%epoc32\release\winscw\udeb\xnthemeinstallercons.exe C:\data\themeinstalltemp\activeidle\
type %EPOCROOT%epoc32\winscw\c\logs\themeinstallercons\themeinstallercons.log

xcopy /s /i /c %THEME_INSTALLED_DIR% %THEME_DEST_C_UDEB_DIR%
xcopy /s /i /c %THEME_INSTALLED_DIR% %THEME_DEST_C_UREL_DIR%
xcopy /s /i /c %THEME_INSTALLED_DIR% %THEME_DEST_Z_DIR%

echo Installing Classic Idle theme files
echo ***********************************
%EPOCROOT%epoc32\release\winscw\udeb\xnthemeinstallercons.exe C:\data\themeinstalltemp\classicidle\
type %EPOCROOT%epoc32\winscw\c\Logs\themeinstallercons\themeinstallercons.log

xcopy /s /i /c %THEME_INSTALLED_DIR% %THEME_DEST_C_UDEB_DIR%
xcopy /s /i /c %THEME_INSTALLED_DIR% %THEME_DEST_C_UREL_DIR%
xcopy /s /i /c %THEME_INSTALLED_DIR% %THEME_DEST_Z_DIR%

echo Copying Active Idle binary themes
echo *********************************
copy %THEME_DEST_Z_DIR%\271063149\1.0\AI.*  ..\activeidlethemes\binarythemes\activeidle\

echo Copying Classic Idle binary themes
echo **********************************
copy %THEME_DEST_Z_DIR%\271063147\1.0\CI.*  ..\activeidlethemes\binarythemes\classicidle\

ENDLOCAL
