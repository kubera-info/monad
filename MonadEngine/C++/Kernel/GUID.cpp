// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#include "stdafx.h"
#include "Patterns.h"
//
#include <array>

namespace Monad::Kernel
{
	std::wstring GetGUID()
	{
		std::array<wchar_t, 40> wszGuid{ 0 };
		GUID guid{};
		CoCreateGuid(&guid);
		// Convert the GUID to a string
		StringFromGUID2(guid, wszGuid.data(), static_cast<int>(wszGuid.size()));
		return std::wstring(wszGuid.data());
	}

}