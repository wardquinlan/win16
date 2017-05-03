@echo off
echo Enter the disk in drive a:
pause
format /u a:
erase VCS16.ZIP
pkzip -sqt+171 -r -p VCS16.ZIP %VCSROOT%\*.*
copy VCS16.ZIP a:

