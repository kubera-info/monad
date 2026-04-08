##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_TITLE=The DDS Textures Object Builder
MND_TEXCONV=@pwsh.exe ..\..\Bin\BulkTex.ps1

include "..\NMake\Header.mk"

all: Init tex7_obj say

tex7_obj:
	$(MND_TEXCONV) BC7_UNORM_SRGB 3 DDS ..\..\..\Modules\noto-emoji\png\512

say:
	@pwsh.exe ..\..\Bin\Say.ps1 "Emoji Textures Object Builder is done."

clean:
	@del DDS\*.dds >nul 2>&1