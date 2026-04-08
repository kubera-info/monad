// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "__MonadRenderer.h"

namespace Monad::Renderer
{
	uint32_t g_frameIndex = 0;

	VectorBytesBytecode::VectorBytesBytecode(
		VectorBytes&& Source
	) noexcept :
		VectorBytes{ move(Source) }
	{}

	VectorBytesBytecode::operator const D3D12_SHADER_BYTECODE() const noexcept
	{
		return
		{
			data(),
			size()
		};
	}

	static_assert(static_cast<size_t>(Registers::CONSTANT_BUFFER::B13) + 1 == D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	static_assert(static_cast<size_t>(Registers::INPUT_RESOURCE::T127) + 1 == D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	static_assert(static_cast<size_t>(InputLayout::SEMANTIC_INDEX::SI31) + 1 == D3D12_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT);
}