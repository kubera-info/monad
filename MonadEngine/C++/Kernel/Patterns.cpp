// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Patterns.h"
// Monad
#include "Exceptions/Exceptions.h"
#include "Kernel/Del.h"
#include "Math/Random.h"

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

	const std::string& VectorRandomString::GetRandomString() const
	{
		THROW_EXC_IFFALSE(Exceptions::NoDataFound, !empty(), L"Random String");
		return operator[](Math::g_random.GetRandomInt(size() - 1));
	}

	std::string VectorRandomString::GetRandomStringOnce()
	{
		auto ret = GetRandomString();
		erase_if(*this, [&ret](const auto& name) { return name == ret; });
		return ret;
	}
}