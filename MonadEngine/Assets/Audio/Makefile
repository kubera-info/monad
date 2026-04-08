##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The Wave Object Builder
MND_WAVCONV=@..\..\bin\PCMConverter.exe $? $@

include "..\NMake\Header.mk"

all: Init jingles_obj help_obj

#Jingles
Jingles\clear1.pcm: Jingles\clear1.wav
	$(MND_WAVCONV)
Jingles\clear2.pcm: Jingles\clear2.wav
	$(MND_WAVCONV)
Jingles\color1.pcm: Jingles\color1.wav
	$(MND_WAVCONV)
Jingles\color3.pcm: Jingles\color3.wav
	$(MND_WAVCONV)
Jingles\color5.pcm: Jingles\color5.wav
	$(MND_WAVCONV)
Jingles\exit1.pcm: Jingles\exit1.wav
	$(MND_WAVCONV)
Jingles\focus.pcm: Jingles\focus.wav
	$(MND_WAVCONV)
Jingles\no.pcm: Jingles\no.wav
	$(MND_WAVCONV)
Jingles\paint1.pcm: Jingles\paint1.wav
	$(MND_WAVCONV)
Jingles\paint2.pcm: Jingles\paint2.wav
	$(MND_WAVCONV)
Jingles\print1.pcm: Jingles\print1.wav
	$(MND_WAVCONV)
Jingles\vopros.pcm: Jingles\vopros.wav
	$(MND_WAVCONV)
Jingles\yes.pcm: Jingles\yes.wav
	$(MND_WAVCONV)

jingles_obj: Jingles\clear1.pcm Jingles\clear2.pcm Jingles\color1.pcm Jingles\color3.pcm Jingles\color5.pcm Jingles\exit1.pcm Jingles\focus.pcm Jingles\no.pcm Jingles\paint1.pcm Jingles\paint2.pcm Jingles\print1.pcm Jingles\vopros.pcm Jingles\yes.pcm

#help
Speech\En\help_art.pcm: Speech\En\help_art.wav
	$(MND_WAVCONV)
Speech\En\help_art_print.pcm: Speech\En\help_art_print.wav
	$(MND_WAVCONV)
Speech\En\help_menu.pcm: Speech\En\help_menu.wav
	$(MND_WAVCONV)
Speech\En-GB\help_art.pcm: Speech\En-GB\help_art.wav
	$(MND_WAVCONV)
Speech\En-GB\help_art_print.pcm: Speech\En-GB\help_art_print.wav
	$(MND_WAVCONV)
Speech\En-GB\help_menu.pcm: Speech\En-GB\help_menu.wav
	$(MND_WAVCONV)
Speech\Pl\help_art.pcm: Speech\Pl\help_art.wav
	$(MND_WAVCONV)
Speech\Pl\help_art_print.pcm: Speech\Pl\help_art_print.wav
	$(MND_WAVCONV)
Speech\Pl\help_menu.pcm: Speech\Pl\help_menu.wav
	$(MND_WAVCONV)
Speech\Ru\help_art.pcm: Speech\Ru\help_art.wav
	$(MND_WAVCONV)
Speech\Ru\help_art_print.pcm: Speech\Ru\help_art_print.wav
	$(MND_WAVCONV)
Speech\Ru\help_menu.pcm: Speech\Ru\help_menu.wav
	$(MND_WAVCONV)

help_obj: Speech\En\help_art.pcm Speech\En\help_art_print.pcm Speech\En\help_menu.pcm
help_obj: Speech\En-GB\help_art.pcm Speech\En-GB\help_art_print.pcm Speech\En-GB\help_menu.pcm
help_obj: Speech\Pl\help_art.pcm Speech\Pl\help_art_print.pcm Speech\Pl\help_menu.pcm
help_obj: Speech\Ru\help_art.pcm Speech\Ru\help_art_print.pcm Speech\Ru\help_menu.pcm

clean:
	@del /s *.pcm >nul 2>&1