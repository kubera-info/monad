// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <d2d1_3.h>
// STD
#include <filesystem>
// Monad
#include "__Monad2D.h"
#include "Kernel/Templates.h"

namespace Monad::Renderer2D
{
	struct Factory2D
	{
		Factory2D();
		void CreateWICBitmap(
			IWICBitmapDecoder* bitmapBlob,
			PtrWICBitmap& wicBitmap
		) const;
		void CreateWICBitmap(
			IStream* bitmapBlob,
			PtrWICBitmap& wicBitmap
		) const;
		/*void CreateWICBitmap(
			const std::filesystem::path& bitmapBlob,
			PtrWICBitmap& wicBitmap
		) const;*/

	private:
		const Kernel::Me<Factory2D> c_me;

	public:
		Microsoft::WRL::ComPtr<IWICImagingFactory2> m_iwicFactory;
		Microsoft::WRL::ComPtr<ID2D1Factory3> m_d2dFactory;
	} extern* g_factory2D;
}