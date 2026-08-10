##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The Shaders Builder for Book
MND_VSICONV=$(MND_BIN_DIR)\EncryptVS.cmd $(MND_SALT) $? $@

MND_SRC_DIR=..\..\HLSL\Object
MND_DEST_DIR=..\..\Res

include "..\NMake\Salt.mk"

all: Init vsi ps ps_low ps_medium ps_high

#Vertex Shaders
$(MND_DEST_DIR)\Auto\blank_tex.monad-vsi: $(MND_SRC_DIR)\blank_tex.vso
	$(MND_VSICONV) image
$(MND_DEST_DIR)\Auto\color_button.monad-vsi: $(MND_SRC_DIR)\color_button.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Boot2\crayon.monad-vsi: $(MND_SRC_DIR)\crayon.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Boot2\emoji.monad-vsi: $(MND_SRC_DIR)\emoji.vso
	$(MND_VSICONV) emoji
$(MND_DEST_DIR)\Boot2\font.monad-vsi: $(MND_SRC_DIR)\font.vso
	$(MND_VSICONV) font
$(MND_DEST_DIR)\Auto\iconic.monad-vsi: $(MND_SRC_DIR)\iconic.vso
	$(MND_VSICONV) tex
$(MND_DEST_DIR)\Boot2\metal_border.monad-vsi: $(MND_SRC_DIR)\metal_border.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Boot2\metal_hr.monad-vsi: $(MND_SRC_DIR)\metal_hr.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Auto\paper.monad-vsi: $(MND_SRC_DIR)\paper.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Auto\raw_tex.monad-vsi: $(MND_SRC_DIR)\raw_tex.vso
	$(MND_VSICONV) image
$(MND_DEST_DIR)\Boot2\shadow.monad-vsi: $(MND_SRC_DIR)\shadow.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Boot2\shadow_border.monad-vsi: $(MND_SRC_DIR)\shadow_border.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Boot2\shadow_emoji.monad-vsi: $(MND_SRC_DIR)\shadow_emoji.vso
	$(MND_VSICONV) emoji
$(MND_DEST_DIR)\Boot2\shadow_font.monad-vsi: $(MND_SRC_DIR)\shadow_font.vso
	$(MND_VSICONV) font
$(MND_DEST_DIR)\Boot2\shadow_hr.monad-vsi: $(MND_SRC_DIR)\shadow_hr.vso
	$(MND_VSICONV) plain
$(MND_DEST_DIR)\Auto\shadow_tex.monad-vsi: $(MND_SRC_DIR)\shadow_tex.vso
	$(MND_VSICONV) tex
$(MND_DEST_DIR)\Boot2\wallpaper.monad-vsi: $(MND_SRC_DIR)\wallpaper.vso
	$(MND_VSICONV) image

vsi: $(MND_DEST_DIR)\Auto\blank_tex.monad-vsi $(MND_DEST_DIR)\Auto\color_button.monad-vsi $(MND_DEST_DIR)\Boot2\crayon.monad-vsi $(MND_DEST_DIR)\Boot2\emoji.monad-vsi $(MND_DEST_DIR)\Boot2\font.monad-vsi $(MND_DEST_DIR)\Auto\iconic.monad-vsi $(MND_DEST_DIR)\Boot2\metal_border.monad-vsi $(MND_DEST_DIR)\Boot2\metal_hr.monad-vsi $(MND_DEST_DIR)\Auto\paper.monad-vsi $(MND_DEST_DIR)\Auto\raw_tex.monad-vsi $(MND_DEST_DIR)\Boot2\shadow.monad-vsi $(MND_DEST_DIR)\Boot2\shadow_border.monad-vsi $(MND_DEST_DIR)\Boot2\shadow_emoji.monad-vsi $(MND_DEST_DIR)\Boot2\shadow_font.monad-vsi $(MND_DEST_DIR)\Boot2\shadow_hr.monad-vsi $(MND_DEST_DIR)\Auto\shadow_tex.monad-vsi $(MND_DEST_DIR)\Boot2\wallpaper.monad-vsi

# Pixel Shaders
$(MND_DEST_DIR)\Auto\blank_tex.monad-pxs: $(MND_SRC_DIR)\blank_tex.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\shadow.monad-pxs: $(MND_SRC_DIR)\shadow.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\shadow_emoji.monad-pxs: $(MND_SRC_DIR)\shadow_emoji.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\shadow_font.monad-pxs: $(MND_SRC_DIR)\shadow_font.pso
	$(MND_ENCRYPT)

ps: $(MND_DEST_DIR)\Auto\blank_tex.monad-pxs $(MND_DEST_DIR)\Boot2\shadow.monad-pxs $(MND_DEST_DIR)\Boot2\shadow_emoji.monad-pxs $(MND_DEST_DIR)\Boot2\shadow_font.monad-pxs

# Low
$(MND_DEST_DIR)\Auto\Lo\color_button.monad-pxs: $(MND_SRC_DIR)\Low\color_button.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Lo\crayon.monad-pxs: $(MND_SRC_DIR)\Low\crayon.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Lo\emoji.monad-pxs: $(MND_SRC_DIR)\Low\emoji.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Lo\flag.monad-pxs: $(MND_SRC_DIR)\Low\flag.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Lo\font.monad-pxs: $(MND_SRC_DIR)\Low\font.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Lo\iconic.monad-pxs: $(MND_SRC_DIR)\Low\iconic.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Lo\logo.monad-pxs: $(MND_SRC_DIR)\Low\logo.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Lo\metal.monad-pxs: $(MND_SRC_DIR)\Low\metal.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Lo\paper.monad-pxs: $(MND_SRC_DIR)\Low\paper.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Lo\raw_tex.monad-pxs: $(MND_SRC_DIR)\Low\raw_tex.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Lo\wood_tex.monad-pxs: $(MND_SRC_DIR)\Low\wood_tex.pso
	$(MND_ENCRYPT)

ps_low: $(MND_DEST_DIR)\Auto\Lo\color_button.monad-pxs $(MND_DEST_DIR)\Boot2\Lo\crayon.monad-pxs $(MND_DEST_DIR)\Boot2\Lo\emoji.monad-pxs $(MND_DEST_DIR)\Auto\Lo\flag.monad-pxs $(MND_DEST_DIR)\Boot2\Lo\font.monad-pxs $(MND_DEST_DIR)\Auto\Lo\iconic.monad-pxs $(MND_DEST_DIR)\Auto\Lo\logo.monad-pxs $(MND_DEST_DIR)\Boot2\Lo\metal.monad-pxs $(MND_DEST_DIR)\Auto\Lo\paper.monad-pxs $(MND_DEST_DIR)\Auto\Lo\raw_tex.monad-pxs $(MND_DEST_DIR)\Boot2\Lo\wood_tex.monad-pxs

# Medium
$(MND_DEST_DIR)\Auto\Med\color_button.monad-pxs: $(MND_SRC_DIR)\Medium\color_button.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Med\crayon.monad-pxs: $(MND_SRC_DIR)\Medium\crayon.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Med\emoji.monad-pxs: $(MND_SRC_DIR)\Medium\emoji.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Med\flag.monad-pxs: $(MND_SRC_DIR)\Medium\flag.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Med\font.monad-pxs: $(MND_SRC_DIR)\Medium\font.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Med\iconic.monad-pxs: $(MND_SRC_DIR)\Medium\iconic.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Med\logo.monad-pxs: $(MND_SRC_DIR)\Medium\logo.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Med\metal.monad-pxs: $(MND_SRC_DIR)\Medium\metal.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Med\paper.monad-pxs: $(MND_SRC_DIR)\Medium\paper.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Med\raw_tex.monad-pxs: $(MND_SRC_DIR)\Medium\raw_tex.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Med\wood_tex.monad-pxs: $(MND_SRC_DIR)\Medium\wood_tex.pso
	$(MND_ENCRYPT)

ps_medium: $(MND_DEST_DIR)\Auto\Med\color_button.monad-pxs $(MND_DEST_DIR)\Boot2\Med\crayon.monad-pxs $(MND_DEST_DIR)\Boot2\Med\emoji.monad-pxs $(MND_DEST_DIR)\Auto\Med\flag.monad-pxs $(MND_DEST_DIR)\Boot2\Med\font.monad-pxs $(MND_DEST_DIR)\Auto\Med\iconic.monad-pxs $(MND_DEST_DIR)\Auto\Med\logo.monad-pxs $(MND_DEST_DIR)\Boot2\Med\metal.monad-pxs $(MND_DEST_DIR)\Auto\Med\paper.monad-pxs $(MND_DEST_DIR)\Auto\Med\raw_tex.monad-pxs $(MND_DEST_DIR)\Boot2\Med\wood_tex.monad-pxs

# High
$(MND_DEST_DIR)\Auto\Hi\color_button.monad-pxs: $(MND_SRC_DIR)\High\color_button.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Hi\crayon.monad-pxs: $(MND_SRC_DIR)\High\crayon.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Hi\emoji.monad-pxs: $(MND_SRC_DIR)\High\emoji.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Hi\flag.monad-pxs: $(MND_SRC_DIR)\High\flag.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Hi\font.monad-pxs: $(MND_SRC_DIR)\High\font.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Hi\iconic.monad-pxs: $(MND_SRC_DIR)\High\iconic.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Hi\logo.monad-pxs: $(MND_SRC_DIR)\High\logo.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Hi\metal.monad-pxs: $(MND_SRC_DIR)\High\metal.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Hi\paper.monad-pxs: $(MND_SRC_DIR)\High\paper.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Auto\Hi\raw_tex.monad-pxs: $(MND_SRC_DIR)\High\raw_tex.pso
	$(MND_ENCRYPT)
$(MND_DEST_DIR)\Boot2\Hi\wood_tex.monad-pxs: $(MND_SRC_DIR)\High\wood_tex.pso
	$(MND_ENCRYPT)

ps_high: $(MND_DEST_DIR)\Auto\Hi\color_button.monad-pxs $(MND_DEST_DIR)\Boot2\Hi\crayon.monad-pxs $(MND_DEST_DIR)\Boot2\Hi\emoji.monad-pxs $(MND_DEST_DIR)\Auto\Hi\flag.monad-pxs $(MND_DEST_DIR)\Boot2\Hi\font.monad-pxs $(MND_DEST_DIR)\Auto\Hi\iconic.monad-pxs $(MND_DEST_DIR)\Auto\Hi\logo.monad-pxs $(MND_DEST_DIR)\Boot2\Hi\metal.monad-pxs $(MND_DEST_DIR)\Auto\Hi\paper.monad-pxs $(MND_DEST_DIR)\Auto\Hi\raw_tex.monad-pxs $(MND_DEST_DIR)\Boot2\Hi\wood_tex.monad-pxs

clean:
	@del /s $(MND_DEST_DIR)\*.monad-pxs >nul 2>&1
	@del /s $(MND_DEST_DIR)\*.monad-vsi >nul 2>&1