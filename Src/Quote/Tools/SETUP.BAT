@echo OFF

ECHO Quote Version 1.7.1 Installation
ECHO --------------------------------

IF [%1] == [help] GOTO help
IF [%1] == [-h] GOTO help
IF [%1] == [/h] GOTO help
IF [%1] == [-?] GOTO help
IF [%1] == [/?] GOTO help

SET MEDIA_DIR=%1
SET INST_DIR=%2

IF [%MEDIA_DIR%] == [] SET MEDIA_DIR=A:
IF [%INST_DIR%] == []  SET INST_DIR=C:\QUOTE

ECHO Media Directory       : %MEDIA_DIR%
ECHO Installation Directory: %INST_DIR%
ECHO Is this correct?
CHOICE

IF ERRORLEVEL 2 goto abort

:install
IF EXIST %INST_DIR%\NUL goto direxists

MKDIR %INST_DIR%
IF EXIST %INST_DIR%\NUL goto copyfiles
ECHO Error: Could not create directory '%INST_DIR%'
goto abort

:direxists
ECHO The directory '%INST_DIR% already exists; do you want to overwrite its contents?
CHOICE
IF ERRORLEVEL 2 goto abort

:copyfiles
ECHO Copying Quote binaries...
COPY %MEDIA_DIR%\SETUP.BAT   %INST_DIR%
COPY %MEDIA_DIR%\QUOTE.EXE   %INST_DIR%
COPY %MEDIA_DIR%\QT_UPD.EXE  %INST_DIR%
COPY %MEDIA_DIR%\SLNK.EXE    %INST_DIR%
COPY %MEDIA_DIR%\STATPAK.DLL %INST_DIR%
COPY %MEDIA_DIR%\BOXCTRL.DLL %INST_DIR%
COPY %MEDIA_DIR%\RSI.IND     %INST_DIR%
COPY %MEDIA_DIR%\CCI.IND     %INST_DIR%
COPY %MEDIA_DIR%\VI.IND      %INST_DIR%
COPY %MEDIA_DIR%\SAR.IND     %INST_DIR%
COPY %MEDIA_DIR%\DMI.IND     %INST_DIR%

:updatepath
ECHO Do you want Setup to update your PATH in AUTOEXEC.BAT?
CHOICE
IF ERRORLEVEL 2 goto complete

COPY C:\AUTOEXEC.BAT C:\AUTOEXEC.QT
ECHO A backup copy of AUTOEXEC.BAT file has been created as C:\AUTOEXEC.QT
ECHO SET PATH=%%PATH%%;%INST_DIR% >> C:\AUTOEXEC.BAT
ECHO Your path has been updated; please reboot for these changes to take effect
GOTO complete

:help
ECHO Usage: SETUP MEDIR-DIR INSTALL-DIR
GOTO exit

:abort
ECHO -- Setup Aborted
GOTO exit

:complete
ECHO If you wish, you may now create a Windows Shortcut to %INST_DIR%\QUOTE.EXE
ECHO -- Setup Complete

:exit
