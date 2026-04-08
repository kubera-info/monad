// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
#include <string>
// Monad
#include "Safety/SafetyManager.h"

namespace Monad::Files
{
	void SaveStringAs(
		Safety::SafetyState& cipher,
		const std::filesystem::path& filename,
		const std::string& utf8Buffer
	);
}