// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// STD
#include <memory>
// Monad
#include "Kernel/Del.h"
#include "CommandList.h"
#include "__MonadRenderer.h"

struct DXSample;

namespace Monad::Renderer
{
	static_assert(sizeof DXGI_FORMAT == sizeof uint32_t);
	/// <summary>
	/// GPU mesh container.
	/// Holds vertex and index buffers together with their metadata.
	/// </summary>
	struct Mesh final
	{
		/// <summary>
		/// A mediator, gets various parameters, return vertices and indices
		/// </summary>
		struct MeshHelper final
		{
			friend Mesh;
			/// <summary>
			/// Constructs mesh data from a binary file.
			/// </summary>
			MeshHelper(
				const Kernel::SpanConstBytes& file
			) noexcept;

			/// <summary>
			/// Constructs mesh data from index and vertex containers.
			/// </summary>
			template<typename I, typename V>
			inline MeshHelper(
				const I& indices,
				const V& vertices
			) noexcept :
				c_indexSize{ sizeof I::value_type },
				c_vertexSize{ sizeof V::value_type },
				c_indices(
					reinterpret_cast<const uint8_t*>(indices.data()),
					indices.size() * sizeof(I::value_type)),
				c_vertices(
					reinterpret_cast<const uint8_t*>(vertices.data()),
					vertices.size() * sizeof(V::value_type))
			{
				static_assert(
					std::is_same_v<typename I::value_type, uint16_t>
					|| std::is_same_v<typename I::value_type, uint32_t>,
					"Indices must be uint16_t or uint32_t");
			}
			OPER_DEL(MeshHelper);

		private:
			inline uint32_t GetIndexCount() const noexcept;

			const uint32_t c_indexSize, c_vertexSize;
			const Kernel::SpanConstBytes c_indices, c_vertices;
		};

		Mesh();
		Mesh(
			const MeshHelper& meshDef,
			const bool release = true
		);
		Mesh(
			const std::string& meshFilename
		);
		~Mesh() = default;
		OPER_DEF_NO_DEF_CTOR(Mesh);

		void SetBuffers(
			const MeshHelper& meshDef,
			GraphicsCommandList1& graphicsCommandList,
			bool wasInitialized = true
		);
		void Render() const noexcept;

	private:
		struct MeshInterfaces;
		// This shared ptr costs a small fortune,
		// however derives consistent data on all instances.
		std::shared_ptr<MeshInterfaces> m_interfaces;
	};
}