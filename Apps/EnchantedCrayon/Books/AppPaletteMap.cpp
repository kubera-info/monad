// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppPaletteMap.h"
// Monad
#include "AppSingleton.h"
#include "Core/DXSampleHelper.h"
#include "Renderer/HDR.h"
#include "Repository/Repo.h"
#include "Time/__MonadTime.h"

using namespace std;

namespace Monad
{
	using namespace GUI;
	using namespace Kernel;
	using namespace Renderer;
	using namespace Time;

	namespace App
	{
		namespace
		{
			/// <summary>
			/// Temporary buffer converting palette colors
			/// into GPU-friendly half-float format.
			/// Represents one palette line uploaded to the GPU.
			/// </summary>
			struct ArrayTexelsConverter
				: array<DirectX::PackedVector::XMHALF4, ROWS_PALETTE_COUNT>
			{
				inline ArrayTexelsConverter(
					ArrayColors& paletteContainer
				) noexcept
				{
					for (size_t x = 0; x < size(); ++x)
					{
						operator[](x) =
							HDR::PowerFloat4{
								paletteContainer[x].GetCurrentColor()
						}.GetHalfRGBA();
					}
				}

				/// <summary>
				/// Total buffer size in bytes.
				/// </summary>
				static constexpr size_t PALETTE_WIDTH =
					ROWS_PALETTE_COUNT * sizeof(value_type);
			};

			/// <summary>
			/// Generates a smooth oscillating scalar value used
			/// for subtle animation of palette colors.
			/// </summary>
			/// <param name="speed">Speed multiplier of oscillation.</param>
			/// <returns>Animated scalar in range ~[0.0–0.25]</returns>
			float GetColorScalar(const double speed) noexcept
			{
				return static_cast<float>(
					sin(g_fTime * speed) * 0.125 + 0.125
					);
			}
		}

		// ---------------------------------------------------------------------
		// PaletteMap
		// ---------------------------------------------------------------------
		// Represents GPU-backed color palette used by the coloring system.
		// Colors are uploaded as a 1D texture and updated dynamically.
		// ---------------------------------------------------------------------

		PaletteMap::PaletteMap(const string& palette) :
			m_tex{
				ROWS_PALETTE_COUNT,
				DXGI_FORMAT_R16G16B16A16_FLOAT,
				D3D12_RESOURCE_FLAG_NONE
		}
		{
			// Register texture in global shader repository
			g_repository->m_shaderResources.Insert(
				palette,
				m_tex
			);

			// Create upload buffer for palette texture
			const auto buf =
				CD3DX12_RESOURCE_DESC::Buffer(
					ArrayTexelsConverter::PALETTE_WIDTH
				);

			THROW_EXC_D3D(
				g_d3d12Core->m_device->CreateCommittedResource(
					&Renderer::HEAP_TYPE_UPLOAD,
					D3D12_HEAP_FLAG_NONE,
					&buf,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_textureUploadHeap)
				),
				L"Commit Palette Tex"
			);

			NAME_D3D12_OBJECT(m_textureUploadHeap);
		}

		// Updates palette texture every frame
		void PaletteMap::OnFrameRender(
			const DirectX::XMFLOAT4X4&
		)
		{
			// Animate first palette entry slightly
			m_paletteContainer[0].AlterColor({
				GetColorScalar(2.0),
				GetColorScalar(1.0),
				GetColorScalar(0.5)
				});

			// Upload palette to GPU
			m_tex.UpdateTexture1D(
				ArrayTexelsConverter{ m_paletteContainer },
				m_textureUploadHeap
			);
		}

		// Marks this resource as system-level (always active)
		bool PaletteMap::IsSystem() const noexcept
		{
			return true;
		}

		// Applies a new color to the selected palette index
		void PaletteMap::SetNewColor(
			size_t index
		) noexcept
		{
			if (index > 0)
				m_paletteContainer[index]
				.SetNewColor(GetCrayonColor());
		}

		// Resets all palette colors to default values
		void PaletteMap::ResetColors()
		{
			for (size_t index = FIRST_ENABLED;
				index < m_paletteContainer.size();
				++index)
			{
				m_paletteContainer[index].ResetColor();
			}
		}
		void PaletteMap::FlushColors()
		{
			for (size_t index = FIRST_ENABLED;
				index < m_paletteContainer.size();
				++index)
			{
				m_paletteContainer[index].FlushColor();
			}
		}
	}
}