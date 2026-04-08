// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "ShaderMatrix.h"

// Monad
#include "Core/DXSample.h"
#include "Matrices.h"
#include "Repository/Repo.h"

using namespace DirectX;

namespace Monad::Renderer
{
	ShaderNoLight::ShaderNoLight(
		const std::string& technique,
		Mesh mesh
	) :
		ShaderGeneric(
			technique,
			mesh
		),
		m_cBufferModel{ "model"_constantBuffer }
#if defined MONAD_SHADOW
		, m_cBufferShadow{ "shadow"_constantBuffer }
#endif
	{}

	void ShaderNoLight::OnComputePerWorldFrame(
		const XMFLOAT4X4& modelSpaceMatrix
	)
	{
		__super::OnComputePerWorldFrame(modelSpaceMatrix);

#if defined MONAD_SHADOW
		XMStoreFloat4x4(
			&m_cBufferShadow.GetBuffer(),
			XMMatrixTranspose(
				XMMatrixMultiply(
					XMLoadFloat4x4(&modelSpaceMatrix),
					XMLoadFloat4x4(&SHADOW)))
		);
		m_cBufferShadow.SetMe(GetCurrentConfig());
#endif

		const auto WORLDVIEW_MATRIX = XMMatrixMultiply(
			XMLoadFloat4x4(&modelSpaceMatrix),
			XMLoadFloat4x4(&VIEW));
		auto& cbModel = m_cBufferModel.GetBuffer();
		XMStoreFloat4x4(
			&cbModel.m_model,
			XMMatrixTranspose(WORLDVIEW_MATRIX));
		XMStoreFloat4x4(
			&cbModel.m_modelViewProj,
			XMMatrixTranspose(
				XMMatrixMultiply(
					WORLDVIEW_MATRIX,
					XMLoadFloat4x4(&g_dxSample->m_matrices.m_matrixProjection))));
		m_cBufferModel.SetMe(GetCurrentConfig());
	}

#if defined MONAD_SHADOW
	void ShaderNoLight::OnComputePerShadowFrame(
		const XMFLOAT4X4& modelSpaceMatrix
	)
	{
		__super::OnComputePerShadowFrame(modelSpaceMatrix);

		XMStoreFloat4x4(
			&m_cBufferShadow.GetBuffer(),
			XMMatrixTranspose(
				XMMatrixMultiply(
					XMLoadFloat4x4(&modelSpaceMatrix),
					XMLoadFloat4x4(&SHADOW)))
		);
		m_cBufferShadow.SetMe(GetCurrentConfig());
	}
#endif

	void Shader::OnComputePerWorldFrame(
		const XMFLOAT4X4& modelSpaceMatrix
	)
	{
		__super::OnComputePerWorldFrame(modelSpaceMatrix);

		Light();
	}

	void Shader::Light() const noexcept
	{
		g_dataLightPosMatrixGeneric->SetMe(GetCurrentConfig());
	}

	Shader::Shader(
		const std::string& technique,
		Mesh mesh
	) :
		ShaderNoLight(
			technique,
			mesh
		)
	{}
}