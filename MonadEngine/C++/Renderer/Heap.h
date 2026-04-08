// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include "d3dx12.h"
// STD
#include <memory>
// Monad
#include "__MonadRenderer.h"

namespace Monad::Renderer
{
	struct HeapCombine
	{
		HeapCombine(
			const PtrD3D12Device& d3d12Device,
			const D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapType
		);
		const CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
			const int32_t offset
		) const noexcept;
		const CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
			const int32_t offset
		) const noexcept;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() const noexcept;
		uint32_t GetDescriptorSize() const noexcept;
		operator ID3D12DescriptorHeap* () const noexcept;

	private:
		const uint32_t c_descriptorSize;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
	};
	using PtrHeapCombine = std::unique_ptr<HeapCombine>;
}