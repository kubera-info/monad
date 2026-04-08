// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <string>
#include <utility>

namespace Monad::Kernel
{
	template<typename P>
		requires (!std::is_pointer_v<P>&& std::is_standard_layout_v<P>)
	inline auto operator<=>(
		const P& left,
		const P& right
		) noexcept
	{
		switch (memcmp(&left, &right, sizeof(P)))
		{
		case 0: return std::strong_ordering::equivalent;
		case -1: return std::strong_ordering::less;
		case 1: return std::strong_ordering::greater;
		default: std::unreachable();
		}
	}

	extern const std::string GHOST;
}