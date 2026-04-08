// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
// Monad
#include "GUI/GUIMath.h"

namespace Monad::Renderer
{
	inline constexpr DirectX::XMFLOAT4X4 VIEW{
		-1.000000f, 0.000000f, 0.000000f, 0.000000f,
		0.000000f, 1.000000f, 0.000000f, 0.000000f,
		0.000000f, 0.000000f, -1.000000f, 0.000000f,
		0.000000f, -0.000000f, 2.000000f, 1.000000f
	};

	inline constexpr DirectX::XMFLOAT4X4 SHADOW{
		-4.900360f, 0.239969f, 0.113470f, 0.104014f,
		0.000000f, 5.253244f, -0.411071f, -0.376815f,
		-0.553770f, -2.123507f, -1.004106f, -0.920430f,
		0.000001f, 0.000000f, 10.445990f, 10.575490f
	};

	constexpr DirectX::XMFLOAT4X4 ROTATE_CURSOR{ 0.267617f, -0.823639f, -0.500000f, 0.000000f,
		-0.951056f, -0.309017f, 0.000000f, 0.000000f,
		-0.154509f, 0.475528f, -0.866025f, 0.000000f,
		0.000000f, 0.000000f, 0.000000f, 1.000000f
	};

	struct Matrices final
	{
		//#define g_clockFaceLookatPtInternal (GUI::GUI_BLACK)
				//constexpr DirectX::XMFLOAT3 g_eyePtInternal(0.0f, 0.0f, 2.0f);
				//constexpr DirectX::XMVECTOR g_vUpVec{ 0.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4X4 m_matrixProjection{};

		/*g_matrixView = XMMatrixLookAtLH(XMLoadFloat3(&g_eyePtInternal), XMLoadFloat3(&g_clockFaceLookatPtInternal), g_vUpVec);*/

		/*const DirectX::XMVECTOR vLookatPt = XMLoadFloat3(&g_clockFaceLookatPtInternal);
		const DirectX::XMMATRIX SHADOW_PROJECTION_MATRIX = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(20.0f), 1.15f, 1.0f, 12.0f);
		const DirectX::XMMATRIX SHADOW_VIEW_MATRIX = DirectX::XMMatrixLookAtLH(XMLoadFloat4(&g_lightPos), vLookatPt, g_vUpVec);
		g_matrixShadowTex = XMMatrixMultiply(SHADOW_VIEW_MATRIX, SHADOW_PROJECTION_MATRIX);*/

		/*	ROTATE_CURSOR: XMMatrixMultiply(XMMatrixRotationY(XM_PI * 3.0f / 3.6f),
				XMMatrixRotationZ(XM_PIDIV2 * 1.2f));*/

				// DirectX::XMFLOAT4X4 m_matrixWallpaperScale{};

		GUI::GUIMath m_monitorProps{};
	};

#if defined _XM_NO_INTRINSICS_
	void DumpMatrix(
		const DirectX::XMMATRIX& matrix
	) noexcept;
	void DumpFloat4(
		const DirectX::XMFLOAT4& vector
	) noexcept;
#endif
}