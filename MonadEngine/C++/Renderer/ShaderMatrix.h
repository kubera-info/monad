// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "Kernel/Templates.h"
#include "Shader0.h"
#include "Renderer/ConstantBuffers.h"

namespace Monad::Renderer
{
	struct ShaderNoLight : ShaderGeneric
	{
	protected:
		ShaderNoLight(
			const TechniqueName& technique,
			Mesh mesh
		);
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;
#if defined MONAD_SHADOW
		void OnComputePerShadowFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;
#endif

	protected:
		CB::ModelInstance m_cBufferModel;
#if defined MONAD_SHADOW
		CB::ShadowInstance m_cBufferShadow;
#endif
	};

	struct Shader : ShaderNoLight
	{
	protected:
		Shader(
			const TechniqueName& technique,
			Mesh mesh
		);
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;
		void Light() const noexcept;
	};
}