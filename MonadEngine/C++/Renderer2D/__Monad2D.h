// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <wincodec.h>
#include <wrl.h>

namespace Monad::Renderer2D
{
	using PtrWICBitmap = Microsoft::WRL::ComPtr<IWICBitmap>;
}