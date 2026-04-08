#pragma once

// Platform
#include <DirectXPackedVector.h>
// STD
#include <array>
// Monad
#include "__MonadRenderer.h"

namespace Monad::Renderer::HDR
{
	/// <summary>
	/// Supported swap-chain bit depths.
	/// </summary>
	enum SWAP_CHAIN_BIT_DEPTH : size_t
	{
		_8,
		_10,
		// _16,
		SWAP_CHAIN_BIT_DEPTH_COUNT
	};

	/// <summary>
	/// Display chromaticity coordinates used for HDR metadata.
	/// </summary>
	struct DisplayChromaticities
	{
		float
			RedX,
			RedY,
			GreenX,
			GreenY,
			BlueX,
			BlueY,
			WhiteX,
			WhiteY;
	};

	struct PowerFloat4 final
	{
		PowerFloat4(
			const DirectX::XMFLOAT4& float4Value
		) noexcept;
		PowerFloat4(
			const DirectX::XMFLOAT3& float3Value
		) noexcept;
		DirectX::XMFLOAT3 GetFloatSRGB() const noexcept;
		DirectX::XMFLOAT4 GetFloatSRGBA() const noexcept;
		DirectX::PackedVector::XMHALF4 GetHalfRGBA() const noexcept;

	private:
		const DirectX::XMFLOAT4 c_float4Value;
	};

	/// <summary>
	/// HDR configuration for a swap chain.
	/// </summary>
	struct HDRProfile
	{
		const DXGI_COLOR_SPACE_TYPE m_currentSwapChainColorSpace;
		const DXGI_FORMAT m_hdrFormat;
	};

	/// <summary>
	/// Supported HDR output configurations.
	/// </summary>
	constexpr auto g_HDRConfig = std::to_array<HDRProfile>(
		{
			{
				DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709,
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
			},
			{
				DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020,
				DXGI_FORMAT_R10G10B10A2_UNORM
			}/*,
			{
				DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709,
				DXGI_FORMAT_R16G16B16A16_FLOAT
			}*/
		});

	/// <summary>
	/// Converts a linear RGB color to sRGB color space.
	/// </summary>
	inline DirectX::XMFLOAT4 GetSRGB(
		const DirectX::XMFLOAT3& createColor
	) noexcept
	{
		return PowerFloat4{ {
			MND_MAKE_FLOAT0(createColor.x),
			MND_MAKE_FLOAT0(createColor.y),
			MND_MAKE_FLOAT0(createColor.z),
			1.0f } }.GetFloatSRGBA();
	}
}