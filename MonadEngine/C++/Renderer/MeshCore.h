// ##########################################################################
// ### Copyright © 2016–2026 by Wlodzimierz O. Kubera. All rights reserved. ###
// ##########################################################################

#pragma once

//
// Procedural mesh generation utilities.
// This file provides tools for building meshes at runtime in C++.
//

// STD
#include <ranges>
// Monad
#include "GUI/__MonadGUI.h"
#include "Exceptions/Exceptions.h"
#include "Kernel/Templates.h"
#include "Mesh.h"

namespace Monad::Renderer::InputLayout
{
	// =========================================================================
	// Vertex definitions
	// =========================================================================

	struct VERTEXFONT final
	{
		DirectX::XMFLOAT2 m_position;
		DirectX::XMFLOAT2 m_texture;
		DirectX::XMFLOAT3 m_color;
	};

	struct VERTEXEMOJI final
	{
		DirectX::XMFLOAT2 m_position;
		DirectX::XMFLOAT2 m_texture;
	};

	struct VERTEXPANE final
	{
		constexpr VERTEXPANE(
			DirectX::XMFLOAT2 position,
			DirectX::XMFLOAT2 uv
		) noexcept :
			m_position{ position.x, position.y, GUI::ART_PANE_Z },
			m_texture{ uv }
		{}

		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT2 m_texture;
	};

	struct VERTEXPLAIN final
	{
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_normal;
	};

	// =========================================================================
	// MeshBuilder
	// =========================================================================

	/// <summary>
	/// Generic procedural mesh builder.
	/// Builds indexed geometry at runtime and produces a Mesh object.
	/// </summary>
	/// <typeparam name="V">Vertex structure</typeparam>
	/// <typeparam name="I">Index type (uint16_t or uint32_t)</typeparam>
	template<typename V, typename I = uint16_t>
		requires (std::is_same_v<I, uint16_t> || std::is_same_v<I, uint32_t>)
	struct MeshBuilder
	{
		using VERTEX_TYPE = V;
		using INDEX_TYPE = I;

	protected:
		Kernel::UnorderedMap<VERTEX_TYPE, size_t> m_verticesMap;

	public:
		static_assert(std::is_standard_layout_v<V>, "Vertex must be standard layout");
		static_assert(std::is_nothrow_copy_constructible_v<V>);
		static_assert(std::is_nothrow_copy_assignable_v<V>);
		static_assert(std::is_nothrow_move_constructible_v<V>);
		static_assert(std::is_nothrow_move_assignable_v<V>);

		/// <summary>
		/// Returns the number of triangle faces.
		/// </summary>
		INDEX_TYPE GetFacesCount() const noexcept
		{
			return static_cast<INDEX_TYPE>(m_indices.size() / 3);
		}

		size_t GetSizeOfVertices() const noexcept { return m_vertices.size(); }
		size_t GetSizeOfIndices()  const noexcept { return m_indices.size(); }

		/// <summary>
		/// Adds a triangle composed of three vertices.
		/// Duplicate vertices are deduplicated automatically.
		/// </summary>
		void AddTriangle(
			const VERTEX_TYPE& vertexA,
			const VERTEX_TYPE& vertexB,
			const VERTEX_TYPE& vertexC
		)
		{
			AddVertex(vertexA);
			AddVertex(vertexB);
			AddVertex(vertexC);
		}

		/// <summary>
		/// Adds a quad composed of two triangles.
		/// </summary>
		void AddQuad(
			const VERTEX_TYPE& vertexA,
			const VERTEX_TYPE& vertexB,
			const VERTEX_TYPE& vertexC,
			const VERTEX_TYPE& vertexD
		)
		{
			AddTriangle(vertexA, vertexB, vertexC);
			AddTriangle(vertexA, vertexC, vertexD);
		}

		/// <summary>
		/// Virtual hook for custom triangle parsing.
		/// </summary>
		virtual void OnParseTriangle(
			const VERTEX_TYPE& vertexA,
			const VERTEX_TYPE& vertexB,
			const VERTEX_TYPE& vertexC
		)
		{
			AddTriangle(vertexA, vertexB, vertexC);
		}

		/// <summary>
		/// Pre-allocates memory for vertex and index buffers.
		/// Prevents reallocations during mesh construction.
		/// </summary>
		void ReserveMemoryForMesh(
			const size_t countOfVertices,
			const size_t countOfIndices
		)
		{
			m_vertices.reserve(countOfVertices);
			m_verticesMap.reserve(countOfVertices);
			m_indices.reserve(countOfIndices);
		}

		/// <summary>
		/// Clears all buffers and releases memory.
		/// </summary>
		void ClearAndShrink() noexcept
		{
			m_vertices.ClearAndShrink();
			m_indices.ClearAndShrink();
			m_verticesMap.clear();
		}

		/// <summary>
		/// Computes a face normal for a triangle.
		/// </summary>
		static DirectX::XMVECTOR ComputeFaceNormal(
			DirectX::XMVECTOR v0,
			DirectX::XMVECTOR v1,
			DirectX::XMVECTOR v2
		)
		{
			const auto edge1 = DirectX::XMVectorSubtract(v1, v0);
			const auto edge2 = DirectX::XMVectorSubtract(v2, v0);
			return DirectX::XMVector3Normalize(
				DirectX::XMVector3Cross(edge1, edge2));
		}

		/// <summary>
		/// Computes vertex normals based on face normals.
		/// </summary>
		void ComputeNormals()
		{
			for (auto& v : m_vertices)
				v.m_normal = { 0.0f, 0.0f, 0.0f };

			for (size_t i = 0; i < GetFacesCount(); ++i)
			{
				auto& a = m_vertices[m_indices[i * 3 + 0]];
				auto& b = m_vertices[m_indices[i * 3 + 1]];
				auto& c = m_vertices[m_indices[i * 3 + 2]];

				const auto faceNormal = ComputeFaceNormal(
					DirectX::XMLoadFloat3(&a.m_position),
					DirectX::XMLoadFloat3(&b.m_position),
					DirectX::XMLoadFloat3(&c.m_position));

				DirectX::XMStoreFloat3(&a.m_normal,
					DirectX::XMVectorAdd(XMLoadFloat3(&a.m_normal), faceNormal));
				DirectX::XMStoreFloat3(&b.m_normal,
					DirectX::XMVectorAdd(XMLoadFloat3(&b.m_normal), faceNormal));
				DirectX::XMStoreFloat3(&c.m_normal,
					DirectX::XMVectorAdd(XMLoadFloat3(&c.m_normal), faceNormal));
			}

			for (auto& v : m_vertices)
				DirectX::XMStoreFloat3(
					&v.m_normal,
					DirectX::XMVector3Normalize(
						DirectX::XMLoadFloat3(&v.m_normal)));
		}

		/// <summary>
		/// Validates that vertex and index counts fit into index type.
		/// </summary>
		void ValidateSize() const
		{
			if constexpr (std::is_same_v<INDEX_TYPE, uint16_t>)
			{
				THROW_EXC_IFFALSE(
					Monad::Exceptions::OutOfRange,
					m_vertices.size() <= std::numeric_limits<INDEX_TYPE>::max(),
					L"Vertex count overflow");
				THROW_EXC_IFFALSE(
					Monad::Exceptions::OutOfRange,
					m_indices.size() <= std::numeric_limits<INDEX_TYPE>::max(),
					L"Index count overflow");
			}
		}

		/// <summary>
		/// Returns a MeshHelper structure ready for GPU upload.
		/// </summary>
		Mesh::MeshHelper GetSettings() const noexcept
		{
			ValidateSize();
			return { m_indices, m_vertices };
		}

		/// <summary>
		/// Builds and returns a GPU-ready mesh.
		/// </summary>
		Mesh Commit(bool release = true)
		{
			return { GetSettings(), release };
		}

		Kernel::VectorNoCtor<VERTEX_TYPE> m_vertices;
		Kernel::VectorNoCtor<INDEX_TYPE>  m_indices;

	protected:
		void AddVertex(const VERTEX_TYPE& vertex)
		{
			if (const auto it = m_verticesMap.find(vertex); it == m_verticesMap.end())
			{
				const size_t index = m_vertices.size();
				m_vertices.push_back(vertex);
				m_verticesMap.emplace(vertex, index);
				m_indices.push_back(static_cast<INDEX_TYPE>(index));
			}
			else
			{
				m_indices.push_back(static_cast<INDEX_TYPE>(it->second));
			}
		}
	};

	// =========================================================================
	// Specializations
	// =========================================================================

	using MeshBuilderEmoji = MeshBuilder<VERTEXEMOJI>;
	using MeshBuilderFont = MeshBuilder<VERTEXFONT>;
	using MeshBuilderPlain = MeshBuilder<VERTEXPLAIN>;

	struct MeshBuilderArtPane final : MeshBuilder<VERTEXPANE>
	{
		MeshBuilderArtPane(
			const DirectX::XMFLOAT4& paneRect,
			double start = 0.0,
			double end = 1.0
		);

		static float Lerp(
			const DirectX::XMFLOAT4& paneRect,
			float horizontalFactor
		) noexcept;
	};
}
