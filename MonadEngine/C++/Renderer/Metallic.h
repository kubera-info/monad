// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Monad
#include "ShaderMatrix.h"
#include "ConstantBuffers.h"

namespace Monad::Renderer
{
	/// <summary>
	/// Base class for metallic-surface shaders.
	/// Provides shared functionality for shaders rendering metallic materials.

	/// </summary>
	interface ShaderMetallicGeneric abstract : Shader
	{
		using Shader::Shader;
		ShaderMetallicGeneric(
			const std::string& technique,
			Mesh mesh,
			const CB::PipeColor& metallic,
			const DirectX::XMFLOAT4& dialogFrame
		);

		ShaderMetallicGeneric(
			const std::string& technique,
			Mesh mesh
		);

		ShaderMetallicGeneric(
			const std::string& technique,
			Mesh mesh,
			const CB::PipeColor& metallic
		);

	protected:
		CB::PipeColorInstance m_cBufferMetallic;
	};

	/// <summary>
	/// A shader for metallic surface (cursors)
	/// </summary>
	struct ShaderMetallic2 : ShaderMetallicGeneric
	{
		using ShaderMetallicGeneric::ShaderMetallicGeneric;

	protected:
		void OnComputePerWorldFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) override;
	};

	/// <summary>
	/// A shader for metallic surface (Dialog's borders and horizontal rule)
	/// </summary>
	struct ShaderMetallic : ShaderMetallic2
	{
		using ShaderMetallic2::ShaderMetallic2;

	private:
		void OnComputePerShadowFrame(
			const DirectX::XMFLOAT4X4& modelSpaceMatrix
		) final;
	};
}