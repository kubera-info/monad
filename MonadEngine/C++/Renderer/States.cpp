// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "States.h"

namespace Monad::Renderer
{

	Samplers::Samplers() noexcept :
		m_samplersDescs{ {
	{
	.Filter = D3D12_FILTER_ANISOTROPIC,
	.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS,
	.MaxAnisotropy = 1u << 4u,
	.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
	.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
	.MinLOD = 0.0f,
	.MaxLOD = D3D12_FLOAT32_MAX,
	.ShaderRegister = 0,
	.RegisterSpace = 0,
	.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
	},
	{
	.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
	.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS,
	.MaxAnisotropy = D3D12_MIN_MAXANISOTROPY,
	.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
	.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
	.MinLOD = 0.0f,
	.MaxLOD = D3D12_FLOAT32_MAX,
	.ShaderRegister = 1,
	.RegisterSpace = 0,
	.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
	},
	{
	.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
	.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS,
	.MaxAnisotropy = D3D12_MIN_MAXANISOTROPY,
	.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
	.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
	.MinLOD = 0.0f,
	.MaxLOD = D3D12_FLOAT32_MAX,
	.ShaderRegister = 2,
	.RegisterSpace = 0,
	.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
	} } }
	{}

	const D3D12_STATIC_SAMPLER_DESC* Samplers::GetAllSamplersDesc() const noexcept
	{
		return m_samplersDescs.data();
	}

	void Samplers::SetAnisotropy(
		uint32_t newMaxAnisotropy
	) noexcept
	{
		m_samplersDescs[0].MaxAnisotropy = 2u << newMaxAnisotropy;
	}

	Samplers g_samplers;
}