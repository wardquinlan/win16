@echo off
if [%1] == [quote]  goto quote
if [%1] == [budget] goto budget
if [%1] == [vcs]    goto vcs
if [%1] == [clean]  goto clean
goto error

:clean
echo cleaning %DEVELOPMENT%\BIN...
erase %DEVELOPMENT%\BIN\*.EXE
erase %DEVELOPMENT%\BIN\*.DLL
erase %DEVELOPMENT%\BIN\*.IND
erase %DEVELOPMENT%\BIN\*.BAT
echo cleaning %DEVELOPMENT%\INC...
erase %DEVELOPMENT%\INC\*.H*
echo cleaning %DEVELOPMENT%\LIB...
erase %DEVELOPMENT%\LIB\*.LIB
echo cleaning %DEVELOPMENT%\PKG...
erase %DEVELOPMENT%\PKG\*.TPL
erase %DEVELOPMENT%\PKG\*.EXE
erase %DEVELOPMENT%\PKG\*.ZIP
erase %DEVELOPMENT%\PKG\*.BAT
erase %DEVELOPMENT%\PKG\*.DLL
erase %DEVELOPMENT%\PKG\*.IND
goto exit

:vcs
if [%BLDENV%] == [C600] goto vcs1
echo wrong environment set up; exiting
goto exit
:vcs1
echo creating vcs environment...
cd %DEVELOPMENT%\SRC
set VCSRPATH=SRC
vcs getlab . Main vcs.v1.0.00
cd %DEVELOPMENT%\SRC\VCS
nmake clean
nmake install
cd %DEVELOPMENT%
goto exit

:budget
if [%BLDENV%] == [CPP] goto budget1
echo wrong environment set up; exiting
goto exit
:budget1
echo creating budget environment...
cd %DEVELOPMENT%\SRC
set VCSRPATH=SRC
vcs getlab . Main budget.v1.1.00
cd %DEVELOPMENT%\SRC\QOBJECT
nmake clean
nmake install
cd %DEVELOPMENT%\SRC\BUDGET
nmake clean
nmake install
cd %DEVELOPMENT%
goto exit

:quote
if [%BLDENV%] == [CPP] goto quote1
echo wrong environment set up; exiting
goto exit
:quote1
echo creating quote environment...
cd %DEVELOPMENT%\SRC
set VCSRPATH=SRC
vcs getlab . Main quote.v1.11.00
cd %DEVELOPMENT%\SRC\QOBJECT
nmake clean
nmake install
cd %DEVELOPMENT%\SRC\QUOTE\STATPAK
nmake clean 
nmake install
cd %DEVELOPMENT%\SRC\QUOTE\BOXCTRL
nmake clean
nmake install
cd %DEVELOPMENT%\SRC\QUOTE\INDCTR
nmake clean
nmake install
cd %DEVELOPMENT%\SRC\QUOTE\QUOTE
nmake clean
nmake install
cd %DEVELOPMENT%

REM Hack to get it to build Quote Tools too
set BLDENV=
call c600.bat
cd %DEVELOPMENT%\SRC\QUOTE\TOOLS
nmake clean
nmake clninst
nmake install
cd %DEVELOPMENT%

REM Now do the packaging
cd %DEVELOPMENT%\SRC\QUOTE\PKG
nmake clean
nmake clninst
nmake install
cd %DEVELOPMENT%
goto exit

:error
echo usage: create [budget -or- quote -or- vcs -or- clean]

:exit
set VCSRPATH=
