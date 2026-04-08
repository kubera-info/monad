// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "AppSketch.h"
// Platform
#include <DirectXTex.h>
// Monad
#include "AppConstantBuffers.h"
#include "Files/__MonadFile.h"
#include "Files/Temporary.h"
#include "AppSingleton.h"
#include "Kernel/__MonadKernel.h"
#include "Printer/Texture.h"
#include "Repository/Repo.h"

using namespace std;

namespace Monad
{
	using namespace Kernel;
	using namespace Print;
	using namespace Renderer;

	namespace App
	{
		namespace
		{
			const Renderer::FormatAndClearRenderTarget g_renderTargetPrinterViewDesc{ DXGI_FORMAT_R8G8B8A8_UNORM, Renderer::FLOAT_4_ONES, D3D12_RTV_DIMENSION_TEXTURE2D };
		}

		ShaderSketch::ShaderSketch() :
			m_renderTargetID(
				g_dxSample->m_rtCurrentID++
			)
		{}

		void ShaderSketch::OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		)
		{
			__super::OnComputePerWorldFrame(modelSpaceMatrix);

			if (PRINT_STATUSES::START == m_printOnce)
			{
				const auto& art = g_repository->m_currentArtProfile.m_currentArt;

				GetCurrentTexture() = g_repository->GetArtTexture(art);
				g_repository->m_shaderResources
					.GetMember(
						NAME_PALETTE).
					SetMe(
						GetCurrentConfig(),
						T3_PALETTE);

				__super::OnComputePerWorldFrame(modelSpaceMatrix);

				g_singleton->m_d3dSingleton->m_screenshotRT.CreateRenderTarget(
#if defined _DEBUG
					L"Screenshot",
#endif
					{
						static_cast<int32_t>(m_renderTargetID),
						g_renderTargetPrinterViewDesc,
						m_texID
					},
					art->second.GetWidth(),
					art->second.GetHeight());
				if (!m_texID)
					m_texID = make_optional(
						g_singleton->
						m_d3dSingleton->
						m_screenshotRT.GetRtTexture().GetOnHeapID());
				g_singleton->m_d3dSingleton->m_screenshotRT.SetMe();

				m_printOnce = PRINT_STATUSES::PRINT_ONCE;
			}
		}

		void ShaderSketch::OnPrint()
		{
			if (PRINT_STATUSES::PRINT_ONCE == m_printOnce && !Exceptions::InAnyCloseReason())
			{
				THROW_EXC_IFFALSE(Exceptions::ThreeDFailed, g_singleton->m_d3dSingleton->m_screenshotRT.GetRtResource(), L"3D Print Init");
				g_singleton->InsertTask(
					new TaskTexturePrinter(
						g_d3d12Core->m_screenshotRT.GetRtResource()));
				m_printOnce = PRINT_STATUSES::READY;
			}
		}

		void ShaderSketch::ScreenshotRT() noexcept
		{
			if (PRINT_STATUSES::READY == m_printOnce)
				g_singleton->m_d3dSingleton->m_screenshotRT.reset();
		}
	}
}