// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

// STD
#include <filesystem>
#include <vector>
#include <string>

namespace Monad::Kernel
{
	/// <summary>
	/// Vector of range-to-value mappings.
	/// Each entry maps a range threshold to a destination value.
	/// </summary>
	using VectorResPair = std::vector<std::pair<uint32_t, uint32_t>>;

	/// <summary>
	/// Selects an optimal value based on the given dimension size.
	/// </summary>
	/// <param name="resolutions">List of size-to-value mappings.</param>
	/// <param name="dimensionSize">Input value to be matched against ranges.</param>
	/// <param name="defaultValue">Value used if no range matches.</param>
	/// <returns>Selected value based on the range mapping.</returns>
	uint32_t GetOptimalMapSize(
		const VectorResPair& resolutions,
		const uint32_t dimensionSize,
		const uint32_t defaultValue
	) noexcept;

	using VectorPath = std::vector<std::filesystem::path>;
	using VectorWString = std::vector<std::wstring>;
	using VectorString = std::vector<std::string>;

	/// <summary>
	/// Container for random string selection.
	/// </summary>
	struct VectorRandomString : VectorString
	{
		using VectorString::VectorString;

		/// <summary>
		/// Returns a randomly selected string from the vector.
		/// </summary>
		const std::string& GetRandomString() const;

		/// <summary>
		/// Returns a randomly selected string and removes it from the vector.
		/// </summary>
		std::string GetRandomStringOnce();
	};
}
