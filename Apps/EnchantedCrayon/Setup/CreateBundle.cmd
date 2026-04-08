@echo off

rem ############################################################################
rem ### Copyright © 2016-2025 by Włodzimierz O. Kubera. All rights reserved. ###
rem ############################################################################

call "c:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

set APPVER=5.0.0.0
mkdir bundle
del bundle\*.msixbundle
copy "SetupBook\AppPackages\SetupBook_%APPVER%_ARM64_Test\*.msix" bundle\
copy "SetupBook\AppPackages\SetupBook_%APPVER%_x64_Test\*.msix" bundle\
MakeAppx.exe bundle /bv "%APPVER%" /d bundle\ /p bundle\book%APPVER%.msixbundle
del bundle\*.msix
pause