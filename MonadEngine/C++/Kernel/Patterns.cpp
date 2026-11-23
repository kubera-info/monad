// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Patterns.h"

namespace Monad::Kernel
{
	uint32_t GetOptimalMapSize(
		const VectorResPair& resolutions,
		const uint32_t dimensionSize,
		const uint32_t defaultSize
	) noexcept
	{
		if (const auto optimalValue = std::ranges::find_if(resolutions,
			[=](const auto& searchedSize) noexcept
			{ return searchedSize.first <= dimensionSize; });
			resolutions.cend() != optimalValue)
			return optimalValue->second;

		return defaultSize;
	}
}