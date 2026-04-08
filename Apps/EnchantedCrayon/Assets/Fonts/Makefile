##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The Fonts Builder for Books
MND_FONT_DIR=..\..\..\..\MonadEngine\Assets\Fonts\Release

include ..\NMake\Salt.mk

all: Init
all: ..\..\Res\Font\century_gothic.monad-fnt
#all: ..\..\Res\Font\courier_new.monad-fnt
all: ..\..\Res\Font\times.monad-fnt

..\..\Res\Font\century_gothic.monad-fnt: $(MND_FONT_DIR)\century_gothic.Output
	$(MND_ENCRYPT)
..\..\Res\Font\courier_new.monad-fnt: $(MND_FONT_DIR)\courier_new.Output
	$(MND_ENCRYPT)
..\..\Res\Font\times.monad-fnt: $(MND_FONT_DIR)\times.Output
	$(MND_ENCRYPT)

clean:
	@del /q /s ..\..\Res\*.monad-fnt >nul 2>&1