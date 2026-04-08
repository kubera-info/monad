// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

#include <filesystem>

namespace Monad::Dev
{	
	void SetFileTime(const std::filesystem::path& fileName, const SYSTEMTIME& st) noexcept;
}