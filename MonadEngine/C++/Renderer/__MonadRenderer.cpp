// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadRenderer.h"

// Monad
#include "Core/DXSample.h"

namespace Monad::Renderer
{
	uint32_t g_frameIndex = 0;

	VectorBytesBytecode::VectorBytesBytecode(
		VectorBytes&& Source
	) noexcept :
		VectorBytes{ move(Source) }
	{
	}

	VectorBytesBytecode::operator D3D12_SHADER_BYTECODE() const noexcept
	{
		return
		{
			data(),
			size()
		};
	}

	ConfigVS::ConfigVS(VectorBytesBytecode vs, std::string il) :
		m_vs{ vs },
		c_il{ il }
	{
	}

	ConfigVS::operator D3D12_SHADER_BYTECODE() const noexcept
	{
		return m_vs;
	}

	ConfigVS::operator D3D12_INPUT_LAYOUT_DESC() const
	{
		return g_dxSample->c_inputLayouts.GetInputLayoutDesc(c_il);
	}

	static_assert(static_cast<size_t>(Registers::CONSTANT_BUFFER::B13) + 1 == D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	static_assert(static_cast<size_t>(Registers::INPUT_RESOURCE::T127) + 1 == D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	static_assert(static_cast<size_t>(InputLayout::SEMANTIC_INDEX::SI31) + 1 == D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT);
}

TechniqueName operator""_technique(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

FXName operator""_fx(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

VertexShaderName operator""_vertexShader(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

PixelShaderName operator""_pixelShader(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

ConstantBufferName operator""_constantBuffer(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

ShaderResourceName operator""_shaderResource(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}

MeshName operator""_mesh(const char* literal, std::size_t sizeOfString)
{
	return { literal, sizeOfString };
}
