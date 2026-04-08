// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>

namespace Monad::Files
{
	/// <summary>
	/// Max. asset's size (to avoid too big allocations)
	/// </summary>
	constexpr uint32_t ZIP_BOMB_RANGE_SIZE = 256ul * 256ul * 256ul * 64ul;

	extern const std::filesystem::path PROFILE_FILENAME, WILDCARD, WILDCARD_ANY;

	/// <summary>
	/// Check if it is Monad(a) Engine's file extension
	/// </summary>
	/// <param name="fileExt">A file extension with a preceding dot</param>
	/// <returns>Is it Monad(a) File?</returns>
	bool IsMonadExt(
		const std::wstring& fileExt
	);
}