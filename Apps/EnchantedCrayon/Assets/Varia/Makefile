##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The Assets Builder for Book
MND_MESHCONV=@$(MND_BIN_DIR)\MeshConverter.exe $? $@
MND_EMOJICONV=@pwsh.exe $(MND_RAW_BIN_DIR)\EmojiEncryption.ps1
MND_MESH_DIR=..\Meshes
MND_DEST_DIR=..\..\Res
MND_WAVCONV=@$(MND_BIN_DIR)\PCMConverter.exe $? $@
MND_WAV_DIR=..\Waves\Streams
MND_JIN_DIR=..\..\..\..\MonadEngine\Assets\Audio\Jingles
MND_SPE_DIR=..\..\..\..\MonadEngine\Assets\Audio\Speech
MND_DDS_DIR=..\..\..\..\MonadEngine\Assets\Textures\DDS
MND_EMOJI_DDS_DIR=..\..\..\..\MonadEngine\Assets\Emoji\DDS
MND_JPG_DIR=..\..\..\..\MonadEngine\Assets\Textures\D2D

include "..\NMake\Salt.mk"

all: Init
all: meshes_obj meshes jingles wallpapers flags
all: icons logo_obj logo emoji
all: satellites help streams

..\Textures\DDS\logo.dds: ..\Textures\Maps\logo.png
	..\..\..\..\MonadEngine\Bin\texconv.exe -nologo -y -gpu 0 -f BC7_UNORM_SRGB -srgb -m 2 -o "..\Textures\DDS" $?

logo_obj: ..\Textures\DDS\logo.dds

$(MND_DEST_DIR)\Auto\flag-pl.monad-tex: $(MND_DDS_DIR)\flag-pl.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\flag-ru.monad-tex: $(MND_DDS_DIR)\flag-ru.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\flag-uk.monad-tex: $(MND_DDS_DIR)\flag-uk.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\flag-us.monad-tex: $(MND_DDS_DIR)\flag-us.DDS
	$(MND_ENCRYPT)

flags: $(MND_DEST_DIR)\Auto\flag-pl.monad-tex $(MND_DEST_DIR)\Auto\flag-ru.monad-tex $(MND_DEST_DIR)\Auto\flag-uk.monad-tex $(MND_DEST_DIR)\Auto\flag-us.monad-tex

$(MND_DEST_DIR)\Auto\c.monad-tex: $(MND_DDS_DIR)\c.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\checked.monad-tex: $(MND_DDS_DIR)\checked.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\dummy.monad-tex: $(MND_DDS_DIR)\dummy.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\exit.monad-tex: $(MND_DDS_DIR)\exit.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\help.monad-tex: $(MND_DDS_DIR)\help.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\i.monad-tex: $(MND_DDS_DIR)\i.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\left.monad-tex: $(MND_DDS_DIR)\left.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\mute.monad-tex: $(MND_DDS_DIR)\mute.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\new.monad-tex: $(MND_DDS_DIR)\new.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\no.monad-tex: $(MND_DDS_DIR)\no.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\ok.monad-tex: $(MND_DDS_DIR)\ok.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\opensource.monad-tex: $(MND_DDS_DIR)\opensource.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\print.monad-tex: $(MND_DDS_DIR)\print.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\radio.monad-tex: $(MND_DDS_DIR)\radio.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\right.monad-tex: $(MND_DDS_DIR)\right.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\setup.monad-tex: $(MND_DDS_DIR)\setup.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\speaker.monad-tex: $(MND_DDS_DIR)\speaker.DDS
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\wrist.monad-tex: $(MND_DDS_DIR)\wrist.DDS
	$(MND_ENCRYPT)

icons: $(MND_DEST_DIR)\Auto\c.monad-tex $(MND_DEST_DIR)\Auto\checked.monad-tex $(MND_DEST_DIR)\Auto\dummy.monad-tex $(MND_DEST_DIR)\Auto\exit.monad-tex $(MND_DEST_DIR)\Auto\help.monad-tex $(MND_DEST_DIR)\Auto\i.monad-tex $(MND_DEST_DIR)\Auto\mute.monad-tex $(MND_DEST_DIR)\Auto\left.monad-tex $(MND_DEST_DIR)\Auto\new.monad-tex $(MND_DEST_DIR)\Auto\no.monad-tex $(MND_DEST_DIR)\Auto\ok.monad-tex $(MND_DEST_DIR)\Auto\opensource.monad-tex $(MND_DEST_DIR)\Auto\print.monad-tex $(MND_DEST_DIR)\Auto\radio.monad-tex $(MND_DEST_DIR)\Auto\right.monad-tex $(MND_DEST_DIR)\Auto\setup.monad-tex $(MND_DEST_DIR)\Auto\speaker.monad-tex $(MND_DEST_DIR)\Auto\wrist.monad-tex

$(MND_DEST_DIR)\boot2\0\wallpaper.monad-tex: $(MND_DDS_DIR)\RV_Marble_3_baseColor.dds
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\boot2\1\wallpaper.monad-tex: $(MND_DDS_DIR)\RV_Wood_3_baseColor.dds
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\boot2\2\wallpaper.monad-tex: $(MND_DDS_DIR)\RV_Wood_2_baseColor.dds
	$(MND_ENCRYPT)

wallpapers: $(MND_DEST_DIR)\boot2\0\wallpaper.monad-tex $(MND_DEST_DIR)\boot2\1\wallpaper.monad-tex $(MND_DEST_DIR)\boot2\2\wallpaper.monad-tex

$(MND_DEST_DIR)\Logo\logo.monad-tex: ..\Textures\DDS\logo.dds
	$(MND_ENCRYPT)

logo: $(MND_DEST_DIR)\Logo\logo.monad-tex

$(MND_DEST_DIR)\Sat\background.monad-lob: $(MND_JPG_DIR)\background.jpg
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Sat\background1.monad-lob: $(MND_JPG_DIR)\background1.jpg
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Sat\background2.monad-lob: $(MND_JPG_DIR)\background2.jpg
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Sat\background3.monad-lob: $(MND_JPG_DIR)\background3.jpg
	$(MND_ENCRYPT)

satellites: $(MND_DEST_DIR)\Sat\background.monad-lob $(MND_DEST_DIR)\Sat\background1.monad-lob $(MND_DEST_DIR)\Sat\background2.monad-lob $(MND_DEST_DIR)\Sat\background3.monad-lob

$(MND_DEST_DIR)\Stream\bach-menuet-dreamdancer.monad-stm: $(MND_WAV_DIR)\bach-menuet-dreamdancer.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\bach-menuet-electro.monad-stm: $(MND_WAV_DIR)\bach-menuet-electro.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\bach-menuet.monad-stm: $(MND_WAV_DIR)\bach-menuet.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot\morning.monad-stm: $(MND_WAV_DIR)\morning.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\rameau-menuet-electro.monad-stm: $(MND_WAV_DIR)\rameau-menuet-electro.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\rameau-menuet-electro_2.monad-stm: $(MND_WAV_DIR)\rameau-menuet-electro_2.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\rameau-menuet.monad-stm: $(MND_WAV_DIR)\rameau-menuet.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\raskraska.monad-stm: $(MND_WAV_DIR)\raskraska.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\schumann.monad-stm: $(MND_WAV_DIR)\schumann.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\tchaikovsky-electric.monad-stm: $(MND_WAV_DIR)\tchaikovsky-electric.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\tchaikovsky.monad-stm: $(MND_WAV_DIR)\tchaikovsky.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\tchaikovsky_2.monad-stm: $(MND_WAV_DIR)\tchaikovsky_2.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\stol-ko_zvezd.monad-stm: $(MND_WAV_DIR)\stol-ko_zvezd.mp3
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\yoga.monad-stm: $(MND_WAV_DIR)\yoga.mp3
	$(MND_ENCRYPT)

streams: $(MND_DEST_DIR)\Boot\morning.monad-stm $(MND_DEST_DIR)\Stream\bach-menuet-dreamdancer.monad-stm $(MND_DEST_DIR)\Stream\bach-menuet-electro.monad-stm $(MND_DEST_DIR)\Stream\bach-menuet.monad-stm $(MND_DEST_DIR)\Stream\rameau-menuet-electro.monad-stm $(MND_DEST_DIR)\Stream\rameau-menuet-electro_2.monad-stm $(MND_DEST_DIR)\Stream\rameau-menuet.monad-stm $(MND_DEST_DIR)\Stream\raskraska.monad-stm $(MND_DEST_DIR)\Stream\schumann.monad-stm $(MND_DEST_DIR)\Stream\tchaikovsky-electric.monad-stm $(MND_DEST_DIR)\Stream\tchaikovsky.monad-stm $(MND_DEST_DIR)\Stream\tchaikovsky_2.monad-stm $(MND_DEST_DIR)\Stream\stol-ko_zvezd.monad-stm $(MND_DEST_DIR)\Stream\yoga.monad-stm

$(MND_DEST_DIR)\Stream\clear1.monad-snd: $(MND_JIN_DIR)\clear1.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\clear2.monad-snd: $(MND_JIN_DIR)\clear2.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\color1.monad-snd: $(MND_JIN_DIR)\color1.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\color3.monad-snd: $(MND_JIN_DIR)\color3.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\color5.monad-snd: $(MND_JIN_DIR)\color5.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\exit1.monad-snd: $(MND_JIN_DIR)\exit1.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\focus.monad-snd: $(MND_JIN_DIR)\focus.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\no.monad-snd: $(MND_JIN_DIR)\no.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\paint1.monad-snd: $(MND_JIN_DIR)\paint1.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\paint2.monad-snd: $(MND_JIN_DIR)\paint2.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\print1.monad-snd: $(MND_JIN_DIR)\print1.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\vopros.monad-snd: $(MND_JIN_DIR)\vopros.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\yes.monad-snd: $(MND_JIN_DIR)\yes.pcm
	$(MND_ENCRYPT)

jingles: $(MND_DEST_DIR)\Stream\clear1.monad-snd $(MND_DEST_DIR)\Stream\clear2.monad-snd $(MND_DEST_DIR)\Stream\color1.monad-snd $(MND_DEST_DIR)\Stream\color3.monad-snd $(MND_DEST_DIR)\Stream\color5.monad-snd $(MND_DEST_DIR)\Stream\exit1.monad-snd $(MND_DEST_DIR)\Stream\focus.monad-snd $(MND_DEST_DIR)\Stream\no.monad-snd $(MND_DEST_DIR)\Stream\paint1.monad-snd $(MND_DEST_DIR)\Stream\paint2.monad-snd $(MND_DEST_DIR)\Stream\print1.monad-snd $(MND_DEST_DIR)\Stream\vopros.monad-snd $(MND_DEST_DIR)\Stream\yes.monad-snd

$(MND_DEST_DIR)\Stream\en-US\help_art.monad-snd: $(MND_SPE_DIR)\En\help_art.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\en-US\help_art_print.monad-snd: $(MND_SPE_DIR)\En\help_art_print.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\en-US\help_menu.monad-snd: $(MND_SPE_DIR)\En\help_menu.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\en-GB\help_art.monad-snd: $(MND_SPE_DIR)\En-GB\help_art.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\en-GB\help_art_print.monad-snd: $(MND_SPE_DIR)\En-GB\help_art_print.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\en-GB\help_menu.monad-snd: $(MND_SPE_DIR)\En-GB\help_menu.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\pl-PL\help_art.monad-snd: $(MND_SPE_DIR)\Pl\help_art.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\pl-PL\help_art_print.monad-snd: $(MND_SPE_DIR)\Pl\help_art_print.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\pl-PL\help_menu.monad-snd: $(MND_SPE_DIR)\Pl\help_menu.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\ru-RU\help_art.monad-snd: $(MND_SPE_DIR)\Ru\help_art.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\ru-RU\help_art_print.monad-snd: $(MND_SPE_DIR)\Ru\help_art_print.pcm
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Stream\ru-RU\help_menu.monad-snd: $(MND_SPE_DIR)\Ru\help_menu.pcm
	$(MND_ENCRYPT)

helpUS: $(MND_DEST_DIR)\Stream\en-US\help_art.monad-snd $(MND_DEST_DIR)\Stream\en-US\help_art_print.monad-snd $(MND_DEST_DIR)\Stream\en-US\help_menu.monad-snd
helpGB: $(MND_DEST_DIR)\Stream\en-GB\help_art.monad-snd $(MND_DEST_DIR)\Stream\en-GB\help_art_print.monad-snd $(MND_DEST_DIR)\Stream\en-GB\help_menu.monad-snd
helpPl: $(MND_DEST_DIR)\Stream\pl-PL\help_art.monad-snd $(MND_DEST_DIR)\Stream\pl-PL\help_art_print.monad-snd $(MND_DEST_DIR)\Stream\pl-PL\help_menu.monad-snd
helpRu: $(MND_DEST_DIR)\Stream\ru-RU\help_art.monad-snd $(MND_DEST_DIR)\Stream\ru-RU\help_art_print.monad-snd $(MND_DEST_DIR)\Stream\ru-RU\help_menu.monad-snd
help: helpUS helpGB helpPl helpRu

$(MND_MESH_DIR)\circullar_button.x.m12: $(MND_MESH_DIR)\circullar_button.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\crayon.x.m12: $(MND_MESH_DIR)\crayon.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\flag_button.x.m12: $(MND_MESH_DIR)\flag_button.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\hr.x.m12: $(MND_MESH_DIR)\hr.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\paper.x.m12: $(MND_MESH_DIR)\paper.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\push_button.x.m12: $(MND_MESH_DIR)\push_button.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\torus.x.m12: $(MND_MESH_DIR)\torus.x
	$(MND_MESHCONV)
$(MND_MESH_DIR)\pane.x.m12: $(MND_MESH_DIR)\pane.x
	$(MND_MESHCONV)

meshes_obj: $(MND_MESH_DIR)\circullar_button.x.m12 $(MND_MESH_DIR)\crayon.x.m12 $(MND_MESH_DIR)\flag_button.x.m12 $(MND_MESH_DIR)\hr.x.m12 $(MND_MESH_DIR)\paper.x.m12 $(MND_MESH_DIR)\push_button.x.m12 $(MND_MESH_DIR)\torus.x.m12 $(MND_MESH_DIR)\pane.x.m12

$(MND_DEST_DIR)\Auto\circullar_button.monad-msh: $(MND_MESH_DIR)\circullar_button.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\crayon.monad-msh: $(MND_MESH_DIR)\crayon.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\flag_button.monad-msh: $(MND_MESH_DIR)\flag_button.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\hr.monad-msh: $(MND_MESH_DIR)\hr.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\paper.monad-msh: $(MND_MESH_DIR)\paper.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\push_button.monad-msh: $(MND_MESH_DIR)\push_button.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\torus.monad-msh: $(MND_MESH_DIR)\torus.x.m12
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\pane.monad-msh: $(MND_MESH_DIR)\pane.x.m12
	$(MND_ENCRYPT)

meshes: $(MND_DEST_DIR)\Auto\circullar_button.monad-msh $(MND_DEST_DIR)\Auto\crayon.monad-msh $(MND_DEST_DIR)\Auto\flag_button.monad-msh $(MND_DEST_DIR)\Boot2\hr.monad-msh $(MND_DEST_DIR)\Auto\paper.monad-msh $(MND_DEST_DIR)\auto\push_button.monad-msh $(MND_DEST_DIR)\Boot2\torus.monad-msh $(MND_DEST_DIR)\Boot2\pane.monad-msh
	
..\..\Res\Emoji\emoji_u1f3a8.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f3a8.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f917.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f917.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f9f0.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f9f0.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u231b.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u231b.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f446_1f3fb.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f446_1f3fb.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f600.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f600.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f46c_1f3fb.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f46c_1f3fb.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f5a8.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f5a8.dds
	$(MND_ENCRYPT)
..\..\Res\Emoji\emoji_u1f6aa.monad-tex: $(MND_EMOJI_DDS_DIR)\emoji_u1f6aa.dds
	$(MND_ENCRYPT)
	
emoji: ..\..\Res\Emoji\emoji_u1f3a8.monad-tex ..\..\Res\Emoji\emoji_u1f917.monad-tex ..\..\Res\Emoji\emoji_u1f9f0.monad-tex ..\..\Res\Emoji\emoji_u231b.monad-tex
emoji: ..\..\Res\Emoji\emoji_u1f446_1f3fb.monad-tex ..\..\Res\Emoji\emoji_u1f600.monad-tex ..\..\Res\Emoji\emoji_u1f46c_1f3fb.monad-tex ..\..\Res\Emoji\emoji_u1f5a8.monad-tex ..\..\Res\Emoji\emoji_u1f6aa.monad-tex

clean:
	@del $(MND_MESH_DIR)\*.m12 >nul 2>&1
	@del $(MND_DEST_DIR)\*.$$$$$$ >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.bak >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.monad-lob >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.monad-snd >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.monad-stm >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.monad-tex >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.monad-msh >nul 2>&1