@echo off
set SALT=%1
set SRC=%2
set OUTPUT=%3
set OUTTMP=%SRC%.$$$
echo Converting PCM: %SRC%
PCMConverter.exe %SRC% %OUTTMP%
Encryption.exe %SALT% %OUTTMP% %OUTPUT%
del %OUTTMP%