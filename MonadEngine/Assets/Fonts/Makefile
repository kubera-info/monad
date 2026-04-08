##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The Fonts Object Builder
MND_FONTCONV=@..\..\bin\Font.cmd
MND_DDSCONV=@..\..\bin\texconv.exe -nologo -y -gpu 0 -f BC4_UNORM -m 5 -o Release $?

include "..\NMake\Header.mk"

all: Init 
all: Release\century_gothic.dds Release\century_gothic.Output 
all: Release\courier_new.dds Release\courier_new.Output
all: Release\times.dds Release\times.Output

# Source files are prepared with BMFont tool.
# Theirs configs are in this solution.
# The font must be built from four PNG files:
# ------------------------
# | Nornal | Bold        |
# ------------------------
# | Italic | Bold Italic |
# ------------------------
# Each file must have the same dimensions: 8192x8192 pixels.
# Result file will have the resolution: 16384x16384 pixels.
# It must be done manually in an image editor.

Release\century_gothic.dds: Res\century_gothic.png
	$(MND_DDSCONV)
Release\courier_new.dds: Res\courier_new.png
	$(MND_DDSCONV)
Release\times.dds: Res\times.png
	$(MND_DDSCONV)

Release\century_gothic.Output: Release\century_gothic.dds Res\century_gothic.fnt Res\century_gothic_bold.fnt Res\century_gothic_italic.fnt Res\century_gothic_bold_italic.fnt
	$(MND_FONTCONV) century_gothic
Release\courier_new.Output: Release\courier_new.dds Res\courier_new.fnt Res\courier_new_bold.fnt Res\courier_new_italic.fnt Res\courier_new_bold_italic.fnt
	$(MND_FONTCONV) courier_new
Release\times.Output: Release\times.dds Res\times.fnt Res\times_bold.fnt Res\times_italic.fnt Res\times_bold_italic.fnt
	$(MND_FONTCONV) times

clean:
	@del Release\*.dds >nul 2>&1
	@del Release\*.Output >nul 2>&1