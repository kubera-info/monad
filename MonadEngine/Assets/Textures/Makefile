##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The DDS Textures Object Builder
MND_TEXCONV=@pwsh.exe ..\..\Bin\BulkTex.ps1

include "..\NMake\Header.mk"

all: Init icons_obj dummy_obj tex7_obj say

# Icons
icons_obj:
	$(MND_TEXCONV) BC4_UNORM 4 DDS Icons

# Icons
dummy_obj:
	$(MND_TEXCONV) R8_UNORM 1 DDS Dummy

tex7_obj:
	$(MND_TEXCONV) BC7_UNORM_SRGB 3 DDS Images Flags

say:
	@pwsh.exe ..\..\Bin\Say.ps1 "DDS Textures Object Builder is done."

clean:
	@del DDS\*.dds >nul 2>&1