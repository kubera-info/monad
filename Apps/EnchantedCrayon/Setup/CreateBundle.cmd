@echo off

rem ##########################################################################
rem ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
rem ##########################################################################

call "c:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

set APPVER=5.0.3.0
mkdir bundle
del bundle\*.msixbundle
copy "SetupBook\AppPackages\SetupBook_%APPVER%_ARM64_Test\*.msix" bundle\
copy "SetupBook\AppPackages\SetupBook_%APPVER%_x64_Test\*.msix" bundle\
MakeAppx.exe bundle /bv "%APPVER%" /d bundle\ /p bundle\book%APPVER%.msixbundle
del bundle\*.msix
pause