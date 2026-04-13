// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <DirectXMath.h>
#include "../../../Modules/meshoptimizer/src/meshoptimizer.h"
// Monad
#include "Kernel/BytesVector.h"

namespace Monad::Renderer
{
	template<typename I>
	struct OptimizedMesh
	{
		Kernel::VectorNoCtor<I> m_indices;
		Kernel::VectorBlock m_vertices;
		};

	OptimizedMesh<uint16_t> OptimizeMesh(
		const Kernel::SpanIndices16& sourceIndices, 
		const Kernel::SpanBlock& sourceVertices,
		const bool omitPosition3 = false
	);

	OptimizedMesh<uint32_t> OptimizeMesh(
		const Kernel::SpanIndices32& sourceIndices,
		const Kernel::SpanBlock& sourceVertices,
		const bool omitPosition3 = false
	);
}