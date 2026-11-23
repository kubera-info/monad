@echo off
echo *************************
echo        submodules
echo *************************
cd ..\..
git submodule update --recursive --remote
echo -------------------------
pause