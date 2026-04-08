##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=Arts for Book

MND_TRANSLATECONV=$(MND_BIN_DIR)\net10.0\Translator.exe $? $@
MND_DDS_DIR=..\Textures\DDS
MND_ART_DIR=..\..\Res\Art

include "..\NMake\Salt.mk"

all: Init arts_obj arts

# Arts
$(MND_DDS_DIR)\indyk.png.dds: indyk.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\kaczka.png.dds: kaczka.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\kon.png.dds: kon.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\kot.png.dds: kot.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\koza.png.dds: koza.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\krolik.png.dds: krolik.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\krowa.png.dds: krowa.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\kura.png.dds: kura.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\motyl.png.dds: motyl.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\papuga.png.dds: papuga.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\pies.png.dds: pies.png
	$(MND_TRANSLATECONV)
$(MND_DDS_DIR)\rybka.png.dds: rybka.png
	$(MND_TRANSLATECONV)

arts_obj: $(MND_DDS_DIR)\indyk.png.dds $(MND_DDS_DIR)\kaczka.png.dds $(MND_DDS_DIR)\kon.png.dds $(MND_DDS_DIR)\kot.png.dds $(MND_DDS_DIR)\koza.png.dds $(MND_DDS_DIR)\krolik.png.dds $(MND_DDS_DIR)\krowa.png.dds $(MND_DDS_DIR)\kura.png.dds $(MND_DDS_DIR)\motyl.png.dds $(MND_DDS_DIR)\papuga.png.dds $(MND_DDS_DIR)\pies.png.dds $(MND_DDS_DIR)\rybka.png.dds

# Arts
$(MND_ART_DIR)\indyk.monad-art: $(MND_DDS_DIR)\indyk.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\kaczka.monad-art: $(MND_DDS_DIR)\kaczka.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\kon.monad-art: $(MND_DDS_DIR)\kon.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\kot.monad-art: $(MND_DDS_DIR)\kot.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\koza.monad-art: $(MND_DDS_DIR)\koza.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\krolik.monad-art: $(MND_DDS_DIR)\krolik.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\krowa.monad-art: $(MND_DDS_DIR)\krowa.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\kura.monad-art: $(MND_DDS_DIR)\kura.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\motyl.monad-art: $(MND_DDS_DIR)\motyl.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\papuga.monad-art: $(MND_DDS_DIR)\papuga.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\pies.monad-art: $(MND_DDS_DIR)\pies.png.dds
	$(MND_ENCRYPT)
$(MND_ART_DIR)\rybka.monad-art: $(MND_DDS_DIR)\rybka.png.dds
	$(MND_ENCRYPT)

arts: $(MND_ART_DIR)\indyk.monad-art $(MND_ART_DIR)\kaczka.monad-art $(MND_ART_DIR)\kon.monad-art $(MND_ART_DIR)\kot.monad-art $(MND_ART_DIR)\koza.monad-art $(MND_ART_DIR)\krolik.monad-art $(MND_ART_DIR)\krowa.monad-art $(MND_ART_DIR)\kura.monad-art $(MND_ART_DIR)\motyl.monad-art $(MND_ART_DIR)\papuga.monad-art $(MND_ART_DIR)\pies.monad-art $(MND_ART_DIR)\rybka.monad-art

clean:
	@del /s $(MND_DDS_DIR)\*.png.dds >nul 2>&1
	@del /s $(MND_ART_DIR)\*.monad-art >nul 2>&1