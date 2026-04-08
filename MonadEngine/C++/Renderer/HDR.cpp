// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "HDR.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Monad::Renderer::HDR
{
#pragma region PowerFloat4
	PowerFloat4::PowerFloat4(
		const XMFLOAT4& float4Value
	) noexcept :
		c_float4Value{ float4Value }
	{}

	PowerFloat4::PowerFloat4(
		const XMFLOAT3& float3Value
	) noexcept :
		PowerFloat4({
			float3Value.x,
			float3Value.y,
			float3Value.z,
			1.0f }
			)
	{}

	XMFLOAT4 PowerFloat4::GetFloatSRGBA() const noexcept
	{
		XMFLOAT4 ret;
		XMStoreFloat4(&ret,
			XMColorSRGBToRGB(
				XMLoadFloat4(&c_float4Value)));
		return ret;
	}

	XMFLOAT3 PowerFloat4::GetFloatSRGB() const noexcept
	{
		const auto in = GetFloatSRGBA();
		return
		{
			in.x,
			in.y,
			in.z
		};
	}

	XMHALF4 PowerFloat4::GetHalfRGBA() const noexcept
	{
		return
		{
			XMConvertFloatToHalf(c_float4Value.x),
			XMConvertFloatToHalf(c_float4Value.y),
			XMConvertFloatToHalf(c_float4Value.z),
			XMConvertFloatToHalf(c_float4Value.w)
		};
	}
}