// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <d3d12.h>
// STD
#include <array>

namespace Monad::Renderer
{
#pragma region Quality ID
	enum RENDERER_QUALITIES : DWORD
	{
		RENDERER_QUALITY_LO,
		RENDERER_QUALITY_MED,
		RENDERER_QUALITY_HI,
		RENDERER_QUALITY_SIZE
	};
#pragma endregion

#pragma region Blend State
	constexpr D3D12_BLEND_DESC
		BLEND_DESC_OPAQUE
	{
	.AlphaToCoverageEnable = FALSE,
	.IndependentBlendEnable = FALSE,
	.RenderTarget = { {
	.BlendEnable = FALSE,
	.LogicOpEnable = FALSE,
	.SrcBlend = D3D12_BLEND_ONE,
	.DestBlend = D3D12_BLEND_ZERO,
	.BlendOp = D3D12_BLEND_OP_ADD,
	.SrcBlendAlpha = D3D12_BLEND_ONE,
	.DestBlendAlpha = D3D12_BLEND_ZERO,
	.BlendOpAlpha = D3D12_BLEND_OP_ADD,
	.LogicOp = D3D12_LOGIC_OP_NOOP,
	.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	} }
	},
		BLEND_DESC_ALPHA
	{
	.AlphaToCoverageEnable = FALSE,
	.IndependentBlendEnable = FALSE,
	.RenderTarget = { {
	.BlendEnable = TRUE,
	.LogicOpEnable = FALSE,
	.SrcBlend = D3D12_BLEND_SRC_ALPHA,
	.DestBlend = D3D12_BLEND_INV_SRC_ALPHA,
	.BlendOp = D3D12_BLEND_OP_ADD,
	.SrcBlendAlpha = D3D12_BLEND_ONE,
	.DestBlendAlpha = D3D12_BLEND_ZERO,
	.BlendOpAlpha = D3D12_BLEND_OP_ADD,
	.LogicOp = D3D12_LOGIC_OP_CLEAR,
	.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	} }
	};
#pragma endregion

#pragma region Rasterizer State
	constexpr D3D12_RASTERIZER_DESC RASTERIZER_DESC
	{
	.FillMode = D3D12_FILL_MODE_SOLID,
	.CullMode = D3D12_CULL_MODE_BACK,
	.FrontCounterClockwise = FALSE,
	.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
	.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
	.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
	.DepthClipEnable = TRUE,
	.MultisampleEnable = TRUE,
	.AntialiasedLineEnable = TRUE,
	.ForcedSampleCount = 0,
	.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
#pragma endregion

#pragma region Depth Stencil
	constexpr D3D12_DEPTH_STENCIL_DESC
		DEPTH_STENCIL_DESC_NULL{},
		DEPTH_STENCIL_DESC
	{
	.DepthEnable = TRUE,
	.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
	.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
	.StencilEnable = FALSE,
	.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
	.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
	.FrontFace = {.StencilFailOp = D3D12_STENCIL_OP_KEEP,
	.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
	.StencilPassOp = D3D12_STENCIL_OP_KEEP,
	.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS },
	.BackFace = {.StencilFailOp = D3D12_STENCIL_OP_KEEP,
	.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
	.StencilPassOp = D3D12_STENCIL_OP_KEEP,
	.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS } };
#pragma endregion

#pragma region Samplers
	struct Samplers
	{
		Samplers() noexcept;
		void SetAnisotropy(
			uint32_t newMaxAnisotropy
		) noexcept;
		consteval uint32_t GetAllSamplersCount() const noexcept
		{
			return static_cast<const uint32_t>(m_samplersDescs.size());
		}
		const D3D12_STATIC_SAMPLER_DESC* GetAllSamplersDesc() const noexcept;

	private:
		std::array<D3D12_STATIC_SAMPLER_DESC, 3> m_samplersDescs;
	};
	extern Samplers g_samplers;
#pragma endregion

#define B0_HDR { D3D12_SHADER_VISIBILITY_PIXEL, Registers::CONSTANT_BUFFER::B0 }
}