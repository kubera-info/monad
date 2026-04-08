// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Shader0.h"
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
			m_mesh{ mesh }
		{
			const auto& thisTechnique = g_dxSample->m_techniques.find(technique);
			THROW_EXC_ONEND(
				g_dxSample->m_techniques,
				thisTechnique,
				L"Technique"
			);
			m_shadersWorld = g_dxSample->m_fxCollection.GetPipeline(
				thisTechnique->second.m_worldState);
#if defined MONAD_SHADOW
			if (thisTechnique->second.m_shadowState)
				m_shadersShadow.emplace(
					g_dxSample->m_fxCollection.GetPipeline(
						thisTechnique->second.m_shadowState.value()));
#endif
		}

		void ShaderGeneric::OnFrameRender(const XMFLOAT4X4& modelSpaceMatrix)
		{
			switch (g_stageOfRendering)
			{
				using enum STAGE_MODES;
			case ALPHA_BLENDING:
			case WORLD:
				if ((ALPHA_BLENDING == g_stageOfRendering) != m_shadersWorld->m_alphaBlending)
					return;
				if (IsSystem())
					return;
				m_shadersWorld->SetMe();
				OnComputePerWorldFrame(modelSpaceMatrix);
				g_dataHDRGeneric->SetMe(m_shadersWorld);
#if MONAD_SHADOW
				g_dxSample->m_shadows.GetDsTexture().SetMe(
					m_shadersWorld,
					T0_SHADOW_MAP
				);
#endif
				break;
#if MONAD_SHADOW
			case SHADOW:
				if (!m_shadersShadow)
					return;
				m_shadersShadow.value()->SetMe();
				OnComputePerShadowFrame(modelSpaceMatrix);
				break;
#endif
			default:
				assert(CUSTOM == g_stageOfRendering);
				if (!IsSystem())
					return;
				m_shadersWorld->SetMe();
				OnComputePerWorldFrame(modelSpaceMatrix);
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

		ShaderConfigGeneric* ShaderGeneric::GetCurrentConfig() const noexcept
		{
#if defined MONAD_SHADOW
			switch (g_stageOfRendering)
			{
			case STAGE_MODES::SHADOW:
				return m_shadersShadow.value();
			default:
				return m_shadersWorld;
			}
#else
			return m_shadersWorld;
#endif			
		}
	}
}