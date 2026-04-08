// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Heap.h"
// Monad
#include "Core/DXSampleHelper.h"
#include "Exceptions/Exceptions.h"

namespace Monad::Renderer
{
	HeapCombine::HeapCombine(
		const PtrD3D12Device& d3d12Device,
		const D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapType
	) :
		c_descriptorSize(d3d12Device->GetDescriptorHandleIncrementSize(
			descriptorHeapType.Type)
		)
	{
		THROW_EXC_D3D(d3d12Device->CreateDescriptorHeap(
			&descriptorHeapType,
			IID_PPV_ARGS(&m_heap)),
			L"Create Descriptor Heap"
		);
#if defined _DEBUG
		SetName(m_heap.Get(), L" Heap");
#endif
	}

	const CD3DX12_GPU_DESCRIPTOR_HANDLE HeapCombine::GetGPUDescriptorHandle(
		const int32_t offset
	) const noexcept
	{
		return {
			m_heap->GetGPUDescriptorHandleForHeapStart(),
			offset,
			c_descriptorSize
		};
	}

	const CD3DX12_CPU_DESCRIPTOR_HANDLE HeapCombine::GetCPUDescriptorHandle(const int32_t offset) const noexcept
	{
		return {
			GetCPUDescriptorHandleForHeapStart(),
			offset,
			c_descriptorSize
		};
	}

	D3D12_CPU_DESCRIPTOR_HANDLE HeapCombine::GetCPUDescriptorHandleForHeapStart() const noexcept
	{
		return m_heap->GetCPUDescriptorHandleForHeapStart();
	}

	uint32_t HeapCombine::GetDescriptorSize() const noexcept
	{
		assert(c_descriptorSize);
		return c_descriptorSize;
	}

	HeapCombine::operator ID3D12DescriptorHeap* () const noexcept
	{
		assert(m_heap);
		return m_heap.Get();
	}
}