@echo off
if not [%1] == [quote] goto next1

REM -----------
REM quote setup
REM -----------
call cpp.bat
cd src\quote\quote
set vcsrpath=src\quote\quote

:next1
if not [%1] == [budget] goto next2
REM ------------
REM budget setup
REM ------------
call cpp.bat
cd src\budget
set vcsrpath=src\budget

:next2
if not [%1] == [tools] goto next3
REM -----------------
REM quote tools setup
REM -----------------
call c600.bat
cd src\quote\tools
set vcsrpath=src\quote\tools

:next3

