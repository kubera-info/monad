// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppWorld.h"
// Monad
#include "AppDict.h"
#include "AppSingleton.h"
#include "Renderer/Matrices.h"
#include "Time/__MonadTime.h"

using namespace DirectX;

namespace Monad
{
	using namespace GUI;
	using namespace I18N;
	using namespace Kernel;
	using namespace Renderer;
	using namespace Time;

	namespace App
	{
#pragma region CombineWorld
		/// <summary>
		/// Main rendering world responsible for managing all visible
		/// elements of the application (UI, drawing surface, dialogs).
		/// </summary>
		CombineWorld::CombineWorld() :
			c_cBufferHDR{
				"hdr"_constantBuffer,
				Renderer::HDR::_10 == g_d3d12Core->m_currentSwapChainBitDepth ? TRUE : FALSE
		},
			m_cBufferForecolor{
				"forecolor"_constantBuffer
		},
			c_cBufferLightPos{
				"light_pos"_constantBuffer,
				GetLight() }
		{
			AddRenderedObjects({
				&m_background,
				&m_torusCursor,
				&m_splashDialog });

			Register();
		}

		/// <summary>
		/// Calculates and returns the transformed light position
		/// used by the scene shaders.
		/// </summary>
		inline XMFLOAT4 CombineWorld::GetLight() const noexcept
		{
			constexpr XMFLOAT4 lightPosition{ -1.1f, 3.985f, 9.734f, 1.0f };
			XMFLOAT4 ret;

			XMStoreFloat4(
				&ret,
				XMVector4Transform(
					XMLoadFloat4(&lightPosition),
					XMLoadFloat4x4(&VIEW))
			);

			return ret;
		}

		/// <summary>
		/// Creates and initializes all runtime-dependent objects
		/// required for full application functionality.
		/// </summary>
		void CombineWorld::CreateFull()
		{
			uint32_t langID = 0;
			m_full = std::make_unique<CombineFullRequirements>(langID);
			m_full->CreateSketchVar();
			AddRenderedObject(m_full.get());
		}

		/// <summary>
		/// Applies global scaling to the world transformation matrix.
		/// </summary>
		void CombineWorld::OnNewMatrix(
			XMFLOAT4X4& matrixToAlter
		) const noexcept
		{
			const auto scale = g_d3d12Core->m_matrices.m_monitorProps.m_scale;

			XMStoreFloat4x4(
				&matrixToAlter,
				XMMatrixMultiply(
					XMMatrixScaling(scale, scale, scale),
					XMLoadFloat4x4(&matrixToAlter))
			);
		}

		/// <summary>
		/// Per-frame update callback.
		/// Updates animated shader parameters.
		/// </summary>
		bool CombineWorld::OnFrameMoveStub() noexcept
		{
			m_cBufferForecolor.GetBuffer().x =
				static_cast<float> (sin(g_fTime * 1.25f)) * .5f + .5f;

			return true;
		}

		/// <summary>
		/// Sets a new color for the currently selected pixel
		/// in the active artwork.
		/// </summary>
		void CombineWorld::SetNewColor(
			float x,
			float y
		) noexcept
		{
			m_full->SetNewColor(
				g_repository->m_currentArtProfile.m_currentArt->second
				.GetColorIndex(x, y)
			);
		}

		/// <summary>
		/// Resets all colors in the current artwork.
		/// </summary>
		void CombineWorld::ResetColors()
		{
			m_full->ResetColors();
		}

		void CombineWorld::FlushColors()
		{
			m_full->FlushColors();
		}

		/// <summary>
		/// Determines whether the world should currently be rendered.
		/// </summary>
		bool CombineWorld::IsVisible() const noexcept
		{
			return RUN_BOOT_DX < Files::g_fileManagerState;
		}
#pragma endregion

#pragma region CombineWorld::DialogSplash
		/// <summary>
		/// Splash screen displayed during application startup.
		/// </summary>
		CombineWorld::DialogSplash::DialogSplash() :
			DialogFullScreenGeneric{
				"boot"_pageNo,
				Globals::g_shortAppName,
				WAIT }
		{}
#pragma endregion

#pragma region CombineWorld::CombineFullRequirements
		/// <summary>
		/// Aggregates all UI elements, dialogs, palettes,
		/// and drawing logic used by the application.
		/// </summary>
		CombineWorld::CombineFullRequirements::CombineFullRequirements(
			uint32_t& langID
		) :
			c_me(this, g_combineFullRequirements),
			m_paletteMap(NAME_PALETTE),
			m_secondaryPaletteMap(NAME_SECONDARY_PALETTE),
			m_paletteMap1(NAME_PALETTE1),
			m_secondaryPaletteMap1(NAME_SECONDARY_PALETTE1),
			m_settingsDialog(langID)
		{
			AddRenderedObjects({
				&m_arrowCursor,
				&m_artPage,
				&m_artSelectionDialog,
				&m_printDialog,
				&m_settingsDialog,
				&m_aboutDialog,
				&m_paletteMap,
				&m_secondaryPaletteMap,
				&m_paletteMap1,
				&m_secondaryPaletteMap1,
				&m_descDialog,
				&m_openSourceDialog,
				&m_sketch,
				&m_exitAppDialog
				});
		}

		/// <summary>
		/// Sets the active artwork in all dependent components.
		/// </summary>
		void CombineWorld::CombineFullRequirements::SetArt(
			Repository::ConstArtIterator art
		)
		{
			m_artPage.m_pictureDialog.SetArt(art);
			m_artSelectionDialog.SetArt(art);
		}

		/// <summary>
		/// Initializes runtime-dependent sketch data.
		/// </summary>
		void CombineWorld::CombineFullRequirements::CreateSketchVar() noexcept
		{
			m_sketch.CreateSketchVar();
		}

		/// <summary>
		/// Resets all palette colors to their default state.
		/// </summary>
		void CombineWorld::CombineFullRequirements::ResetColors()
		{
			m_paletteMap.ResetColors();
			m_paletteMap1.ResetColors();
		}

		void CombineWorld::CombineFullRequirements::FlushColors()
		{
			for (size_t colorID = FIRST_ENABLED; colorID < ROWS_PALETTE_COUNT; ++colorID)
			{
				m_paletteMap.FlushColors();
				m_paletteMap1.FlushColors();
			}
		}

		/// <summary>
		/// Synchronizes all palette maps and returns the active palette.
		/// </summary>
		const ArrayColors&
			CombineWorld::CombineFullRequirements::SetPalette() noexcept
		{
			return
				m_secondaryPaletteMap.m_paletteContainer =
				m_paletteMap1.m_paletteContainer =
				m_secondaryPaletteMap1.m_paletteContainer =
				m_paletteMap.m_paletteContainer;
		}

		/// <summary>
		/// Applies a new color index to all active palettes.
		/// </summary>
		void CombineWorld::CombineFullRequirements::SetNewColor(
			size_t index
		) noexcept
		{
			m_paletteMap.SetNewColor(index);
			m_paletteMap1.SetNewColor(index);
		}

		CombineWorld::CombineFullRequirements* g_combineFullRequirements = nullptr;
#pragma endregion

#pragma region CombineWorld::CombineFullRequirements::DialogPrint
		/// <summary>
		/// Fullscreen dialog displayed during printing.
		/// </summary>
		CombineWorld::CombineFullRequirements::DialogPrint::DialogPrint() :
			DialogFullScreenGeneric(
				"printing"_pageNo,
				Globals::g_shortAppName,
				PRINTING)
		{}
#pragma endregion
	}
}