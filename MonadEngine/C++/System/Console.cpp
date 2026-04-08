// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Console.h"
// Platform
#include <Windows.h>

namespace Monad::System
{
	void MndOutputDebugString([[maybe_unused]] const wchar_t* text) noexcept
	{
		OutputDebugString(L"Monad(a) Engine: ");
		OutputDebugString(text);
		OutputDebugString(L"\n");
	}
}