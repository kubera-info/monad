// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "MeshOpt.h"

namespace Monad::Renderer
{
	namespace
	{
		template<typename I>
		OptimizedMesh<I> OptimizeMeshInternal(
			const std::span<I>& sourceIndices,
			const Kernel::SpanBlock& sourceVertices,
			const bool omitPosition3 = false
		)
		{
			// 1. Deduplication
			std::vector<uint32_t> remap(sourceVertices.c_blocks);
			const size_t new_vertex_count = meshopt_generateVertexRemap(
				remap.data(),
				sourceIndices.data(), sourceIndices.size(),
				sourceVertices.c_data, sourceVertices.c_blocks, sourceVertices.c_blockSize);

			OptimizedMesh<I> out{ {}, sourceVertices.c_blockSize };
			out.m_vertices.resize(new_vertex_count);
			out.m_indices.resize(sourceIndices.size());

			meshopt_remapIndexBuffer(
				out.m_indices.data(), sourceIndices.data(), sourceIndices.size(), remap.data());
			meshopt_remapVertexBuffer(
				out.m_vertices.data(), sourceVertices.c_data, sourceVertices.c_blocks, sourceVertices.c_blockSize, remap.data());

			// 2. Vertex cache
			meshopt_optimizeVertexCache(
				out.m_indices.data(), out.m_indices.data(), sourceIndices.size(), new_vertex_count);

			// 3. Overdraw
			if (!omitPosition3)
				meshopt_optimizeOverdraw(
					out.m_indices.data(), out.m_indices.data(), sourceIndices.size(),
					static_cast<float*> (out.m_vertices.data()), new_vertex_count, sourceVertices.c_blockSize,
					1.05f);

			// 4. Vertex fetch
			meshopt_optimizeVertexFetch(
				out.m_vertices.data(), out.m_indices.data(), sourceIndices.size(),
				out.m_vertices.data(), new_vertex_count, sourceVertices.c_blockSize);

			return out;
		}
	}

	OptimizedMesh<uint16_t> OptimizeMesh(
		const Kernel::SpanIndices16& sourceIndices,
		const Kernel::SpanBlock& sourceVertices,
		const bool omitPosition3
	)
	{
		return OptimizeMeshInternal(sourceIndices, sourceVertices, omitPosition3);
	}

	OptimizedMesh<uint32_t> OptimizeMesh(
		const Kernel::SpanIndices32& sourceIndices,
		const Kernel::SpanBlock& sourceVertices,
		const bool omitPosition3
	)
	{
		return OptimizeMeshInternal(sourceIndices, sourceVertices, omitPosition3);
	}
}