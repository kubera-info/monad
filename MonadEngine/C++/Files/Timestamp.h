// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#include <filesystem>

namespace Monad::Files
{	
	void SetFileTime(const std::filesystem::path& fileName, const SYSTEMTIME& st);
}