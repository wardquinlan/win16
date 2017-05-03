@echo off
if not [%BLDENV%] == [] goto error
MODE CON RATE=32 DELAY=1
echo Setting up CPP environment...
SET PATH=%DEVELOPMENT%\BIN;D:\MSVC\BIN;%PATH%
SET TEMP=D:\TMP
SET LIB=%DEVELOPMENT%\LIB;D:\MSVC\LIB;D:\MSVC\MFC\LIB;%LIB%
SET INCLUDE=%DEVELOPMENT%\INC;D:\MSVC\INCLUDE;D:\MSVC\MFC\INCLUDE;%INCLUDE%
SET HELPFILES=D:\MSVC\HELP\*.HLP;%HELPFILES%
SET BLDENV=CPP
goto exit

:error
echo Environment already set up; exiting

:exit
