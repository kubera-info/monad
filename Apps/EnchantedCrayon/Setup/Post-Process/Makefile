##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=Prepare files for the installers of Coloring Book

include "..\..\..\..\MonadEngine\Assets\NMake\Header.mk"

MND_BIN_DIR=..\..\..\..\MonadEngine\bin

all: Init clean data_setter

data_setter:
	@echo Preparing timestamped monads
	@xcopy /s/y/i "..\..\Res\*.monad-???" "..\Res"

clean:
	@del /s "..\Res\*.monad-???" >nul 2>&1