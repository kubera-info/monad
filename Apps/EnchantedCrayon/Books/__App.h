// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Core/__MonadCore.h"

namespace Monad::App
{
	/// <summary>
	/// Count of maximum resource views
	/// </summary>
	namespace SRV
	{
		constexpr uint32_t
			CACHED_PALETTE_VECTORS_COUNT = 4;
	}
	constexpr uint32_t
		CBV_SRV_COUNT = SRV::CACHED_PALETTE_VECTORS_COUNT + SRV_GHOST;
}

#define APP_VER L"5.0.0"