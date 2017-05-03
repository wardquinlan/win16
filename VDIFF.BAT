@echo off

set VDIFF_NAME=VDIFF.NEW

if [%2] == [] goto error0
if not [%3] == [] goto error0

if exist %VDIFF_NAME% goto error1

ren %1 %VDIFF_NAME%
vcs getfile %1 %2
diff %1 %VDIFF_NAME% > VDIFF.OUT
vcs hdfile %1
ren %VDIFF_NAME% %1
goto exit

:error0
echo vdiff: usage: vdiff filename version
goto exit
goto exit

:error1
echo vdiff: '%VDIFF_NAME%' already exists; exiting...
goto exit

:exit
set VDIFF_NAME=
