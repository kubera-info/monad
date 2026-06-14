// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Shader0.h"
// STD
#include <ranges>
// Monad
#include "Repository/Repo.h"
#include "Core/DXSample.h"

using namespace DirectX;

namespace Monad
{
	using namespace Kernel;

	namespace Renderer
	{
		HDRGeneric* g_dataHDRGeneric = nullptr;

		HDRGeneric::HDRGeneric(
			const std::string& counter,
			const BUFFER_TYPE& cbType
		) noexcept :
			CB::HDRInstance(
				counter,
				cbType
			),
			c_me(
				this,
				g_dataHDRGeneric
			)
		{}

		DirectX::XMFLOAT4X4* g_dataMorphingMatrix = nullptr;

		MorphingMatrix::MorphingMatrix() noexcept :
			c_me(
				this,
				g_dataMorphingMatrix
			)
		{
			XMStoreFloat4x4(
				this,
				XMMatrixIdentity()
			);
		}

		LightPosGeneric* g_dataLightPosMatrixGeneric = nullptr;

		LightPosGeneric::LightPosGeneric(
			const std::string& counter,
			DirectX::XMFLOAT4 lightPosition) noexcept :
			CB::LightPosInstance(
				counter,
				lightPosition
			),
			c_me(
				this,
				g_dataLightPosMatrixGeneric
			)
		{}

		STAGE_MODES g_stageOfRendering;
		uint32_t g_texDepthHeight = 1024, g_texDepthWidth = 2048;

		ShaderGeneric::ShaderGeneric(
			const std::string& technique,
			Mesh mesh
		) :
			m_mesh{ mesh },
			m_shaderConfig
			{
				std::ranges::to<MapShaders>(
					std::views::transform
					(
						g_dxSample->m_techniques.find(technique)->second,
						[](const auto& art)
						{
							return std::make_pair(art.first, g_dxSample->m_fxCollection.GetPipeline(art.second));
						}
					)
				)
			}
		{
#if defined _DEBUG
			const auto& thisTechnique = g_dxSample->m_techniques.find(technique);
			THROW_EXC_ONEND(
				g_dxSample->m_techniques,
				thisTechnique,
				L"Technique"
			);
#endif
		}

		void ShaderGeneric::OnFrameRender(const XMFLOAT4X4& modelSpaceMatrix)
		{
			switch (g_stageOfRendering)
			{
				using enum STAGE_MODES;
			case STAGE_MODE_ALPHA_BLENDING:
			case STAGE_MODE_WORLD:
				if (!m_shaderConfig.contains(STAGE_MODE_WORLD) && !m_shaderConfig.contains(STAGE_MODE_ALPHA_BLENDING))
					return;
				if ((STAGE_MODE_ALPHA_BLENDING == g_stageOfRendering) != m_shaderConfig.at(STAGE_MODE_WORLD)->m_alphaBlending)
					return;
				m_shaderConfig.at(STAGE_MODE_WORLD)->SetMe();
				OnComputePerWorldFrame(modelSpaceMatrix);
				g_dataHDRGeneric->SetMe(m_shaderConfig.at(STAGE_MODE_WORLD));
#if MONAD_SHADOW
				g_dxSample->m_shadows.GetDsTexture().SetMe(
					m_shaderConfig.at(STAGE_MODE_WORLD),
					T0_SHADOW_MAP
				);
#endif
				break;
#if MONAD_SHADOW
			case STAGE_MODE_SHADOW:
				if (!m_shaderConfig.contains(STAGE_MODE_SHADOW))
					return;
				m_shaderConfig.at(STAGE_MODE_SHADOW)->SetMe();
				OnComputePerShadowFrame(modelSpaceMatrix);
				break;
#endif
			case STAGE_MODE_CUSTOM:
				if (!m_shaderConfig.contains(STAGE_MODE_CUSTOM))
					return;
				m_shaderConfig.at(STAGE_MODE_CUSTOM)->SetMe();
				OnComputePerSystemFrame(modelSpaceMatrix);
				break;
			}
			m_mesh.Render();
		}

		void ShaderGeneric::OnComputePerShadowFrame(
			const XMFLOAT4X4&
		)
		{}

		void ShaderGeneric::OnComputePerWorldFrame(
			const XMFLOAT4X4&
		)
		{}

		void ShaderGeneric::OnComputePerSystemFrame(
			const DirectX::XMFLOAT4X4&
		)
		{}

		ShaderConfigGeneric* ShaderGeneric::GetCurrentConfig() const noexcept
		{
#if defined MONAD_SHADOW
			switch (g_stageOfRendering)
			{
			case STAGE_MODES::STAGE_MODE_SHADOW:
				return m_shaderConfig.at(STAGE_MODE_SHADOW);
			case STAGE_MODES::STAGE_MODE_CUSTOM:
				return m_shaderConfig.at(STAGE_MODE_CUSTOM);
			default:
				return m_shaderConfig.at(STAGE_MODE_WORLD);
			}
#else
			return m_shaderConfig.at(STAGE_MODE_WORLD);
#endif			
		}
	}
}