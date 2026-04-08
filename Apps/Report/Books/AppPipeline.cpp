// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Renderer/Pipeline.h"
// Monad
#include "Fonts/__MonadFont.h"
#include "AppConstantBuffers.h"
#include "AppSingleton.h"
#include "Renderer/States.h"

namespace Monad
{
	using namespace Fonts;
	using namespace Renderer;

	namespace App
	{
#pragma region ShaderConfig

		// ---------------------------------------------------------------------
		// Frequently used constant buffer bindings
		// ---------------------------------------------------------------------

#define B2_SHADOW   { D3D12_SHADER_VISIBILITY_VERTEX, Registers::CONSTANT_BUFFER::B2 }
#define B1_LIGHT_POS { D3D12_SHADER_VISIBILITY_VERTEX, Registers::CONSTANT_BUFFER::B1 }
#define B3_MODEL    { D3D12_SHADER_VISIBILITY_VERTEX, Registers::CONSTANT_BUFFER::B3 }

		namespace
		{
			/// <summary>
			/// Helper sets used to group shaders that share the same
			/// root signature layout.
			/// </summary>
			SetIfThis
				g_metallic{ "metallic_hr"_fx, "metallic_border"_fx };
		}

		// ---------------------------------------------------------------------
		// Creates a graphics pipeline description based on shader identifier.
		// This function defines:
		//  - root signature layout
		//  - constant buffers
		//  - textures
		//  - blending, depth and raster states
		// ---------------------------------------------------------------------
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC
			ShaderConfig::CreateGraphicsPipelineStateDesc(
				const std::string& id
			)
		{
			SetIfThis::const_iterator idFromMany;

			// -----------------------------------------------------------------
			// Shader-specific resource bindings
			// -----------------------------------------------------------------

			if ("font"_fx == id)
			{
				insert(id,
					{
						T0_SHADOW_MAP,
						T1_FONT,
						B0_HDR,
						B2_SHADOW,
						B3_MODEL,
						{ D3D12_SHADER_VISIBILITY_VERTEX, Registers::CONSTANT_BUFFER::B5 }
					});
			}
			else if ("emoji"_fx == id)
			{
				insert(id,
					{
						T0_SHADOW_MAP,
						T1_FONT,
						B0_HDR,
						B2_SHADOW,
						B3_MODEL
					});
			}
			else if (idFromMany = g_metallic.IfClass(id);
				idFromMany != g_metallic.cend())
			{
				insert(*idFromMany,
					{
						T0_SHADOW_MAP,
						B0_HDR,
						B2_SHADOW,
						B3_MODEL,
						{ D3D12_SHADER_VISIBILITY_ALL, Registers::CONSTANT_BUFFER::B4 },
						B1_LIGHT_POS
					});
			}
			else if ("iconic"_fx == id)
			{
				insert(id,
					{
						T0_SHADOW_MAP,
						T2_ART,
						B0_HDR,
						B2_SHADOW,
						B3_MODEL,
						{ D3D12_SHADER_VISIBILITY_ALL, Registers::CONSTANT_BUFFER::B5 },
						B1_LIGHT_POS
					});
			}
			else
			{
				std::unreachable();
			}

			// -----------------------------------------------------------------
			// Final pipeline assembly
			// -----------------------------------------------------------------

			const auto& vsil =
				Repositories::g_repositoryGeneric->m_vs.GetMember(m_vs);
			const auto& ps =
				Repositories::g_repositoryGeneric->m_ps.GetMember(m_ps);

			const bool isShadow = m_vs.starts_with("shadow"_vertexShader);
			const bool isBlank = ("blank_tex"_fx == id);

			return {
				.pRootSignature = m_parametersDef->m_rootSignature.Get(),
				.VS = static_cast<const D3D12_SHADER_BYTECODE>(vsil.m_vs),
				.PS = static_cast<const D3D12_SHADER_BYTECODE>(ps),
				.BlendState = m_alphaBlending ? BLEND_DESC_ALPHA : BLEND_DESC_OPAQUE,
				.SampleMask = UINT_MAX,
				.RasterizerState = RASTERIZER_DESC,
				.DepthStencilState = isBlank ? DEPTH_STENCIL_DESC_NULL : DEPTH_STENCIL_DESC,
				.InputLayout = g_d3d12Core->c_inputLayouts.GetInputLayoutDesc(vsil.c_il),
				.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
				.NumRenderTargets = isShadow ? 0u : 1u,
				.RTVFormats = {
					isShadow
						? DXGI_FORMAT_UNKNOWN
						: (isBlank
							? DXGI_FORMAT_R8G8B8A8_UNORM
							: g_d3d12Core->GetCurrentHDRConfig().m_hdrFormat)
				},
				.DSVFormat = isBlank ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_D32_FLOAT,
				.SampleDesc = { isShadow || isBlank ? 1u : g_d3d12Core->c_sampleCount, 0u }
			};
		}

#pragma endregion
	}
}