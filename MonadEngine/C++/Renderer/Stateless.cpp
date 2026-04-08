// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "Stateless.h"
// Monad
#include "Core/DXSample.h"
#include "Core/DXSampleHelper.h"
#include "Exceptions/Exceptions.h"

namespace Monad::Renderer
{
	namespace
	{
		inline void ValidateSample()
		{
			THROW_EXC_IFFALSE(
				Exceptions::ThreeDFailed,
				g_dxSample && g_dxSample->m_device,
				L"Device");
		}
	}

	Stateless::Stateless()
	{
		ValidateSample();

		THROW_EXC_D3D(g_dxSample->m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)), L"Less Create Alloc");
		NAME_D3D12_OBJECT(m_commandAllocator);
		THROW_EXC_D3D(g_dxSample->m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)), L"Less Create List");
		NAME_D3D12_OBJECT(m_commandList);
	}

	void Stateless::Commit()
	{
		ValidateSample();

		// Close the resource creation command list and execute it to begin the vertex buffer copy into
		// the default heap.
		THROW_EXC_D3D(m_commandList.Get()->Close(), L"Command List Close");
		ID3D12CommandList* ppCommandLists[]{ m_commandList.Get() };
		g_dxSample->m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		UINT64 fenceValue = 0;
		THROW_EXC_D3D(g_dxSample->m_device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_commandList.m_fence)), L"Less Fence");
		NAME_D3D12_OBJECT(m_commandList.m_fence);
		fenceValue++;

		// Schedule a Signal command in the queue.
		THROW_EXC_D3D(g_dxSample->m_commandQueue->Signal(
			m_commandList.m_fence.Get(),
			fenceValue),
			L"Command Queue Signal"
		);
		m_commandList.WaitForCompletionEvent(fenceValue);

		// Wait until the fence has been processed.
		if (m_commandList.m_fence->GetCompletedValue() < fenceValue)
		{
			THROW_EXC_D3D(g_dxSample->m_commandQueue->Wait(
				m_commandList.m_fence.Get(),
				fenceValue),
				L"Command Queue Wait"
			);
		}
	}
}