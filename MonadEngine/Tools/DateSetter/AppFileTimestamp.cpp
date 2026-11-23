// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include <windows.h>
#include "AppFileTimestamp.h"
#include "Files/Timestamp.h"

namespace fs = std::filesystem;

void SetAssetFileTime(const fs::path& fileName) noexcept
{
	Monad::Files::SetFileTime(fileName, {
	.wYear = 2026,
	.wMonth = 06,
	.wDay = 1,
	.wHour = 23
		}
	);
}