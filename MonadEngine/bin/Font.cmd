@echo off
set MND_PATH=Release\%1
set MYTMP=%MND_PATH%.CmpXML
set FONTDDS=%MND_PATH%.dds
set FONTOUT=%MND_PATH%.Output
echo Compacting Font: %1
PwSh.exe "..\..\bin\XMLFontPack.ps1" %1
copy /b %MYTMP% + %FONTDDS% %FONTOUT%
del /q %MYTMP%