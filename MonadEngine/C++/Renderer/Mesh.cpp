// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Mesh.h"
// Platform
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include "d3dx12.h"
// Monad
#include "Core/DXSample.h"
#include "Core/DXSampleHelper.h"
#include "Stateless.h"
#include "Repository/Repo.h"

namespace Monad
{
	using namespace Kernel;

	namespace Renderer
	{
#pragma region Mesh::MeshInterfaces
		struct Mesh::MeshInterfaces
		{
			uint32_t m_numIndices = 0;
			PtrResource
				m_vertexBufferUploadHeap,
				m_indexBufferUploadHeap,
				m_vertexBuffer,
				m_indexBuffer;
			D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
			D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};

			bool IsInitialized() const noexcept
			{
				return m_vertexBuffer;
			}

			void Render() const noexcept
			{
				if (!IsInitialized())
					return;

				g_dxSample->m_commandList.Get()->IASetVertexBuffers(
					0,
					1,
					&m_vertexBufferView
				);
				g_dxSample->m_commandList.Get()->IASetIndexBuffer(
					&m_indexBufferView
				);
				//PIXBeginEvent(g_d3d12Core->m_commandList, 0, L"Draw a mesh");
				g_dxSample->m_commandList.Get()->DrawIndexedInstanced(
					m_numIndices,
					1,
					0,
					0,
					0
				);
				//PIXEndEvent(g_d3d12Core->m_commandList);
			}

			void SetBuffers(
				const MeshHelper& meshDef,
				GraphicsCommandList1& graphicsCommandList,
				bool wasInitialized
			)
			{
				if (wasInitialized)
					graphicsCommandList.Transition({ {
						m_vertexBuffer,
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
						D3D12_RESOURCE_STATE_COPY_DEST},
						{m_indexBuffer,
						D3D12_RESOURCE_STATE_INDEX_BUFFER,
						D3D12_RESOURCE_STATE_COPY_DEST} }
						);
				else
					graphicsCommandList.Transition({ {
						m_vertexBuffer,
						D3D12_RESOURCE_STATE_COMMON,
						D3D12_RESOURCE_STATE_COPY_DEST },
						{m_indexBuffer,
						D3D12_RESOURCE_STATE_COMMON,
						D3D12_RESOURCE_STATE_COPY_DEST} }
						);

				// Copy data to the intermediate upload heap and then schedule a copy 
				// from the upload heap to the vertex buffer.
				const D3D12_SUBRESOURCE_DATA vertexData{
					meshDef.c_vertices.data(),
					static_cast<LONG_PTR>(meshDef.c_vertices.size()),
					0 // RowPitch is not used for 1D buffers
				};
				// Copy data to the intermediate upload heap and then schedule a copy 
				// from the upload heap to the index buffer.
				const D3D12_SUBRESOURCE_DATA indexData{
					meshDef.c_indices.data(),
					static_cast<LONG_PTR>(meshDef.c_indices.size()),
					0 // RowPitch is not used for 1D buffers
				};

				UpdateSubresources<1>(
					graphicsCommandList.Get(),
					m_vertexBuffer.Get(),
					m_vertexBufferUploadHeap.Get(),
					0,
					0,
					1,
					&vertexData
				);
				UpdateSubresources<1>(
					graphicsCommandList.Get(),
					m_indexBuffer.Get(),
					m_indexBufferUploadHeap.Get(),
					0,
					0,
					1,
					&indexData
				);

				assert(m_vertexBuffer);
				assert(m_indexBuffer);
				graphicsCommandList.Transition({ {
					m_vertexBuffer,
					D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER},
					{m_indexBuffer,
					D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATE_INDEX_BUFFER} }
					);
				// Initialize the vertex buffer view.
				m_vertexBufferView =
				{
					m_vertexBuffer->GetGPUVirtualAddress(),
					static_cast<uint32_t>(meshDef.c_vertices.size()),
					meshDef.c_vertexSize
				};
				// Describe the index buffer view.
				m_indexBufferView =
				{
					m_indexBuffer->GetGPUVirtualAddress(),
					static_cast<uint32_t>(meshDef.c_indices.size()),
					sizeof(uint16_t) == meshDef.c_indexSize
					? DXGI_FORMAT_R16_UINT
					: DXGI_FORMAT_R32_UINT
				};
			}

			void CreateBuffers(
				const MeshHelper& meshDef,
				GraphicsCommandList1& graphicsCommandList
			)
			{
				const auto vertexDesc = CD3DX12_RESOURCE_DESC::Buffer(meshDef.c_vertices.size());
				// Create the vertex buffer.
				THROW_EXC_D3D(g_dxSample->m_device->CreateCommittedResource(
					&HEAP_TYPE_DEFAULT,
					D3D12_HEAP_FLAG_NONE,
					&vertexDesc,
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&m_vertexBuffer)),
					L"Vertex Heap Default"
				);
				THROW_EXC_D3D(g_dxSample->m_device->CreateCommittedResource(
					&HEAP_TYPE_UPLOAD,
					D3D12_HEAP_FLAG_NONE,
					&vertexDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_vertexBufferUploadHeap)),
					L"Vertex Heap Upload"
				);

				NAME_D3D12_OBJECT(m_vertexBuffer);
				NAME_D3D12_OBJECT(m_vertexBufferUploadHeap);

				const auto indexDesc = CD3DX12_RESOURCE_DESC::Buffer(meshDef.c_indices.size());
				THROW_EXC_D3D(g_dxSample->m_device->CreateCommittedResource(
					&HEAP_TYPE_DEFAULT,
					D3D12_HEAP_FLAG_NONE,
					&indexDesc,
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&m_indexBuffer)),
					L"Indices Default"
				);

				THROW_EXC_D3D(g_dxSample->m_device->CreateCommittedResource(
					&HEAP_TYPE_UPLOAD,
					D3D12_HEAP_FLAG_NONE,
					&indexDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_indexBufferUploadHeap)),
					L"Indices Upload"
				);

				NAME_D3D12_OBJECT(m_indexBuffer);
				NAME_D3D12_OBJECT(m_indexBufferUploadHeap);

				SetBuffers(meshDef, graphicsCommandList, false);
			}

			void Reset() noexcept
			{
				m_indexBufferUploadHeap.Reset();
				m_vertexBufferUploadHeap.Reset();
			}
		};
#pragma endregion

		constexpr uint32_t HEADER_SIZE = /*3*/ 2 * sizeof uint32_t;
#pragma region Mesh
		Mesh::Mesh(
			const std::string& meshFilename
		) :
			Mesh{ Repositories::g_repositoryGeneric->m_meshes.GetMember(meshFilename) }
		{}

		Mesh::Mesh() :
			m_interfaces{ std::make_shared<MeshInterfaces>(0) }
		{}

		Mesh::Mesh(
			const MeshHelper& meshDef,
			const bool release
		) :
			m_interfaces{ std::make_shared<MeshInterfaces>(meshDef.GetIndexCount()) }
		{
			Stateless stateless;
			m_interfaces->CreateBuffers(meshDef, stateless.m_commandList);
			stateless.Commit();
			if (release)
				m_interfaces->Reset();
		}

		void Mesh::SetBuffers(
			const MeshHelper& meshDef,
			GraphicsCommandList1& graphicsCommandList,
			bool wasInitialized
		)
		{
			m_interfaces->SetBuffers(meshDef, graphicsCommandList, wasInitialized);
		}

		void Mesh::Render() const noexcept
		{
			m_interfaces->Render();
		}

		static_assert(std::is_nothrow_copy_constructible_v<Mesh>);
		static_assert(std::is_nothrow_copy_assignable_v<Mesh>);
		static_assert(std::is_nothrow_move_constructible_v<Mesh>);
		static_assert(std::is_nothrow_move_assignable_v<Mesh>);
#pragma endregion

#pragma region Mesh::MeshHelper
		Mesh::MeshHelper::MeshHelper(
			const SpanConstBytes& file
		) noexcept :
			c_indexSize{
			reinterpret_cast<const uint32_t*>(file.data())[/*2*/1]
		<= D3D12_16BIT_INDEX_STRIP_CUT_VALUE
			? sizeof uint16_t
			: sizeof uint32_t },
			c_vertexSize{ *reinterpret_cast<const uint32_t*>(file.data()) },
			c_indices{ &file[HEADER_SIZE], reinterpret_cast<const uint32_t*>(file.data())[/*2*/1] * c_indexSize },
			c_vertices{ &file[HEADER_SIZE + reinterpret_cast<const uint32_t*>(file.data())[/*2*/1] * c_indexSize], file.size() - HEADER_SIZE - c_indices.size() }
		{
			//  | uint32_t Size of vertex | uint32_t Count of indices | uint8_t* Indices | uint8_t* MeshBuilder |
		}

		inline uint32_t Mesh::MeshHelper::GetIndexCount() const noexcept
		{
			return static_cast<uint32_t>(c_indices.size()) / c_indexSize;
		}
#pragma endregion
	}
}