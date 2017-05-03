@echo off
if not [%BLDENV%] == [] goto error
MODE CON RATE=32 DELAY=1
echo Setting up C600 environment...
SET PATH=D:\C600\BIN;D:\C600\BINB;%PATH%
SET TEMP=D:\TMP
SET LIB=%DEVELOPMENT%\LIB;D:\C600\LIB;%LIB%
SET INCLUDE=%DEVELOPMENT%\INC;D:\C600\INCLUDE;%INCLUDE%
SET INIT=D:\C600\INIT;%INIT%
SET HELPFILES=D:\C600\HELP\*.HLP;%HELPFILES%
SET BLDENV=C600
goto exit

:error
echo Environment already set up; exiting

:exit
