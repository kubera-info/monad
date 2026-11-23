@echo off

rem ############################################################################
rem ### Copyright © 2016-2024 by Włodzimierz O. Kubera. All rights reserved. ###
rem ############################################################################

set SALT=%1
set IPATH=%2.combine
"..\..\..\..\MonadEngine\bin\VSI.exe" %4 %2 %IPATH%
"..\..\..\..\MonadEngine\bin\Encryption.exe" %SALT% %IPATH% %3
del %IPATH%