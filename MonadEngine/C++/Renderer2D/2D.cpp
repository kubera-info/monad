// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "2D.h"

// Platform
#pragma comment(lib, "d2d1.lib")

// Monad
#include "Exceptions/Exceptions.h"

using namespace Microsoft::WRL;
namespace fs = std::filesystem;

namespace Monad
{
	using namespace Exceptions;

	namespace Renderer2D
	{
		Factory2D::Factory2D() :
			c_me(
				this,
				g_factory2D
			)
		{
			THROW_EXC_IFFAILED(NotInitialized, CoCreateInstance(
				CLSID_WICImagingFactory2,
				nullptr,
				CLSCTX_INPROC_SERVER,
				__uuidof(IWICImagingFactory2),
				(LPVOID*)&m_iwicFactory
			), L"WIC Factory");
#if defined _DEBUG
			constexpr D2D1_FACTORY_OPTIONS debugLevel{ .debugLevel = D2D1_DEBUG_LEVEL_INFORMATION };
#else
			constexpr D2D1_FACTORY_OPTIONS debugLevel{ .debugLevel = D2D1_DEBUG_LEVEL_NONE };
#endif
			THROW_EXC_IFFAILED(NotInitialized, D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory3), &debugLevel, &m_d2dFactory), L"D2D1CreateFactory");
		}

		void Factory2D::CreateWICBitmap(
			IWICBitmapDecoder* const bitmapBlob,
			PtrWICBitmap& wicBitmap
		) const
		{
			ComPtr<IWICBitmapFrameDecode> frame;
			THROW_EXC_IFFAILED(InvalidClass, bitmapBlob->GetFrame(0, &frame), L"WIC Get Frame");
			ComPtr<IWICFormatConverter> converter;
			THROW_EXC_IFFAILED(InvalidClass, m_iwicFactory->CreateFormatConverter(&converter), L"Create Format Converter");
			THROW_EXC_IFFAILED(InvalidClass, converter->Initialize(
				frame.Get(),                   // Input bitmap to convert
				GUID_WICPixelFormat32bppPBGRA, // Destination pixel format
				WICBitmapDitherTypeNone,       // Specified dither pattern
				nullptr,                       // Specify a particular palette
				0.0,                           // Alpha threshold
				WICBitmapPaletteTypeMedianCut  // m_palette translation type
			),
				L"Converter Initialize"
			);
			THROW_EXC_IFFAILED(InvalidClass, m_iwicFactory->CreateBitmapFromSource(
				converter.Get(),
				WICBitmapCacheOnLoad,
				wicBitmap.ReleaseAndGetAddressOf()
			),
				L"Create Bitmap From Source"
			);
		}

		void Factory2D::CreateWICBitmap(
			IStream* const bitmapBlob,
			PtrWICBitmap& wicBitmap
		) const
		{
			ComPtr<IWICBitmapDecoder> decoder;
			THROW_EXC_IFFAILED(
				InvalidClass,
				m_iwicFactory->CreateDecoderFromStream(
					bitmapBlob,
					NULL,
					WICDecodeMetadataCacheOnDemand,
					&decoder),
				L"Create Decoder From Stream"
			);
			CreateWICBitmap(
				decoder.Get(),
				wicBitmap
			);
		}

		/*void Factory2D::CreateWICBitmap(
			const fs::path& bitmapBlob,
			PtrWICBitmap& wicBitmap
		) const
		{
			ComPtr<IWICBitmapDecoder> decoder;
			THROW_EXC_IFFAILED(
				InvalidClass,
				m_iwicFactory->CreateDecoderFromFilename(
					bitmapBlob.c_str(),
					NULL,
					GENERIC_READ,
					WICDecodeMetadataCacheOnDemand,
					&decoder),
				L"CreateDecoderFromFilename"
			);
			CreateWICBitmap(
				decoder.Get(),
				wicBitmap
			);
		}*/

		Factory2D* g_factory2D = nullptr;
	}
}