// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppWorld.h"
// Monad
#include "AppSingleton.h"
#include "Renderer/Matrices.h"
#include "Time/__MonadTime.h"
#include "System/Parameter.h"
#include "AppParameter.h"

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
			m_full = std::make_unique<CombineFullRequirements>();
			AddRenderedObject(m_full.get());
			Pages::SelectPage("quick"_pageNo);
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
				sin(static_cast<float> (g_fTime) * 1.25f) * .5f + .5f;

			return true;
		}

		/// <summary>
		/// Determines whether the world should currently be rendered.
		/// </summary>
		bool CombineWorld::IsVisible() const noexcept
		{
			return RUN_BOOT_DX < Files::g_fileManagerState;
		}
#pragma endregion

#pragma region CombineWorld::CombineFullRequirements
		/// <summary>
		/// Aggregates all UI elements, dialogs, palettes,
		/// and drawing logic used by the application.
		/// </summary>
		CombineWorld::CombineFullRequirements::CombineFullRequirements() :
			c_me(this, g_combineFullRequirements)
		{
			if (g_parameters->GetParameter(Monad::System::PAR_NAME_TYPE) == Monad::System::PAR_VAL_INFO)
				m_dlg = std::make_unique<DialogInfo>();
			else
				m_dlg = std::make_unique<DialogReport>();
			AddRenderedObject(m_dlg.get());
		}

		CombineWorld::CombineFullRequirements* g_combineFullRequirements = nullptr;
#pragma endregion
	}
}