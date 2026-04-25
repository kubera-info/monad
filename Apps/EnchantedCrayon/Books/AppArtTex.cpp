// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppArtTex.h"
// STD
#include <mdspan>
// Monad
#include "Pages/Page.h"
using namespace std;

namespace Monad
{
	using namespace Files;
	using namespace Kernel;

	namespace App
	{
		using namespace Pages;

		namespace
		{
#pragma region DDS
			/// <summary>
			/// A DDS file's specific struct
			/// </summary>
			struct DDS_PIXELFORMAT
			{
				uint32_t    size;
				uint32_t    flags;
				uint32_t    fourCC;
				uint32_t    RGBBitCount;
				uint32_t    RBitMask;
				uint32_t    GBitMask;
				uint32_t    BBitMask;
				uint32_t    ABitMask;
			};

			/// <summary>
			/// A DDS file's specific struct
			/// </summary>
			struct DDS_HEADER
			{
				uint32_t        size;
				uint32_t        flags;
				uint32_t        height;
				uint32_t        width;
				uint32_t        pitchOrLinearSize;
				uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
				uint32_t        mipMapCount;
				uint32_t        reserved1[11];
				DDS_PIXELFORMAT ddspf;
				uint32_t        caps;
				uint32_t        caps2;
				uint32_t        caps3;
				uint32_t        caps4;
				uint32_t        reserved2;
			};

			/// <summary>
			/// A DDS file's specific struct in DX10 and newer
			/// </summary>
			struct DDS_HEADER_DXT10
			{
				DXGI_FORMAT     dxgiFormat;
				uint32_t        resourceDimension;
				uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
				uint32_t        arraySize;
				uint32_t        miscFlags2;
			};

			/// <summary>
			/// A full DDS header
			/// </summary>
			struct DDS
			{
				DWORD					dwMagic;
				DDS_HEADER				header;
				DDS_HEADER_DXT10		header10;
			};
#pragma endregion
		}

#pragma region TaskAndMemoryLookup
		TaskAndMemoryLookup::TaskAndMemoryLookup(
			VectorBytesArchive&& file
		) noexcept :
			ITask{ false },
			c_archiveBuffer{ move(file) }
		{
			assert(!c_archiveBuffer.empty());
		}

		void TaskAndMemoryLookup::Run()
		{
			const ScopeCounter refCounter;
			c_archiveBuffer.Decompress(m_bufferCPU);
		}

		size_t TaskAndMemoryLookup::GetColorIndex(
			float x,
			float y
		) const noexcept
		{
			const size_t
				xInd = static_cast<size_t>(static_cast<float>(GetWidth() - 1) * x),
				yInd = static_cast<size_t>(static_cast<float>(GetHeight() - 1) * y);
			/// <summary>
			/// Normalization for shader
			/// </summary>
			constexpr uint32_t shiftRight = 6u;
			return mdspan(
				reinterpret_cast<const uint16_t*>(&m_bufferCPU[sizeof DDS]),
				GetHeight(),
				GetWidth())[yInd, xInd] >> shiftRight;
		}

		uint32_t TaskAndMemoryLookup::GetWidth() const noexcept
		{
			return reinterpret_cast<const DDS* const>(m_bufferCPU.data())->header.width;
		}

		uint32_t TaskAndMemoryLookup::GetHeight() const noexcept
		{
			return reinterpret_cast<const DDS* const>(m_bufferCPU.data())->header.height;
		}

		const VectorBytes& TaskAndMemoryLookup::GetCPUBuffer() const noexcept
		{
			assert(!m_bufferCPU.empty());
			return m_bufferCPU;
		}

		void TaskAndMemoryLookup::ClearAndShrink() noexcept
		{
			m_bufferCPU.ClearAndShrink();
		}
#pragma endregion
	}
}