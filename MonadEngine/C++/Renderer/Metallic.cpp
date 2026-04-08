// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Metallic.h"

// Monad
#include "Kernel/Format.h"

using namespace DirectX;
using namespace std;

namespace Monad::Renderer
{
	ShaderMetallicGeneric::ShaderMetallicGeneric(
		const string& technique,
		Mesh mesh,
		const CB::PipeColor& metallic,
		const XMFLOAT4& dialogFrame
	) :
		Shader{
			technique,
			mesh
	},
		m_cBufferMetallic
	{
		"pipe_color"_constantBuffer,
		{
			metallic.m_diffuseColor,
			metallic.m_color,
			metallic.m_fresnelColor,
			{
				dialogFrame.x,
				dialogFrame.y * Kernel::LANDSCAPE_RATIO,
				dialogFrame.z,
				dialogFrame.w * Kernel::LANDSCAPE_RATIO
			}
		}
	}
	{}

	ShaderMetallicGeneric::ShaderMetallicGeneric(
		const string& technique,
		Mesh mesh
	) :
		Shader{
			technique,
			mesh
	},
		m_cBufferMetallic{ "pipe_color"_constantBuffer }
	{}

	ShaderMetallicGeneric::ShaderMetallicGeneric(
		const string& technique,
		Mesh mesh,
		const Renderer::CB::PipeColor& metallic
	) :
		Shader{
			technique,
			mesh
	},
		m_cBufferMetallic{ "pipe_color"_constantBuffer, metallic }
	{}

	void ShaderMetallic2::OnComputePerWorldFrame(
		const XMFLOAT4X4& modelSpaceMatrix
	)
	{
		__super::OnComputePerWorldFrame(modelSpaceMatrix);

		m_cBufferMetallic.SetMe(GetCurrentConfig());
	}

	void ShaderMetallic::OnComputePerShadowFrame(
		const XMFLOAT4X4& modelSpaceMatrix
	)
	{
		__super::OnComputePerShadowFrame(modelSpaceMatrix);

		m_cBufferMetallic.SetMe(GetCurrentConfig());
	}
}