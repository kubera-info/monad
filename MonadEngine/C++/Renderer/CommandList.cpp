// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

// Me
#include "stdafx.h"
#include "CommandList.h"
// Platform
#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include "d3dx12.h"
// Monad
#include "Exceptions/Exceptions.h"

namespace Monad::Renderer
{
	GraphicsCommandList1::ResourceBarrier::ResourceBarrier(
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter
	) noexcept :
		D3D12_RESOURCE_BARRIER(CD3DX12_RESOURCE_BARRIER::Transition(
			resource,
			stateBefore,
			stateAfter)
		)
	{
		assert(resource);
		assert(stateBefore != stateAfter);
	}

	GraphicsCommandList1::ResourceBarrier::ResourceBarrier(
		const PtrResource& resource,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter
	) noexcept :
		ResourceBarrier(
			resource.Get(),
			stateBefore,
			stateAfter
		)
	{}

	GraphicsCommandList1::ScopedBarrier::ScopedBarrier(
		GraphicsCommandList1& commandList,
		InitializerListResourceBarrier resourceBarriers
	) noexcept :
		m_commandList(commandList),
		m_resourceBarriers(resourceBarriers)
	{}

	GraphicsCommandList1::ScopedBarrier::~ScopedBarrier()
	{
		// reverse barrier inputs and outputs
		for (auto& b : m_resourceBarriers)
			std::swap(b.Transition.StateAfter, b.Transition.StateBefore);

		// Set barriers
		m_commandList.m_commandList->ResourceBarrier(static_cast<uint32_t>(m_resourceBarriers.size()), m_resourceBarriers.data());
	}

	GraphicsCommandList1::ScopedBarrier GraphicsCommandList1::ScopedTransition(
		InitializerListResourceBarrier resourceBarriers) noexcept
	{
		Transition(resourceBarriers);
		return ScopedBarrier(*this, resourceBarriers);
	}

	void GraphicsCommandList1::Transition(
		InitializerListResourceBarrier resourceBarriers
	) const
	{
		assert(m_commandList);
		assert(resourceBarriers.begin() != resourceBarriers.end());
		m_commandList->ResourceBarrier(
			static_cast<uint32_t>(resourceBarriers.size()),
			resourceBarriers.begin()
		);
	}

	GraphicsCommandList1::SmartHandleFence::SmartHandleFence() :
		SmartHandle{
		CreateEvent(
			nullptr,
			FALSE,
			FALSE,
			nullptr)
	}
	{
		if (m_handle == nullptr)
			THROW_EXC_GETLASTERROR(Exceptions::InvalidClass, L"Less Fence Ev");
	}

	void GraphicsCommandList1::WaitForCompletionEvent(
		uint64_t value
	)
	{
		assert(m_fence);
		assert(c_fenceEvent);
		THROW_EXC_D3D(m_fence->SetEventOnCompletion(
			value,
			c_fenceEvent),
			L"Set Event On Completion"
		);
		WaitForSingleObjectEx(
			c_fenceEvent,
			INFINITE,
			FALSE
		);
	}

	void GraphicsCommandList1::SetMe(
		ID3D12PipelineState* pipelineState,
		ID3D12RootSignature* rootSignature
	) noexcept
	{
		assert(pipelineState);
		assert(rootSignature);
		assert(m_commandList);
		m_commandList->SetPipelineState(pipelineState);
		m_commandList->SetGraphicsRootSignature(rootSignature);
	}

	ID3D12GraphicsCommandList1* GraphicsCommandList1::Get() const noexcept
	{
		return m_commandList.Get();
	}

	ID3D12GraphicsCommandList1** GraphicsCommandList1::operator&() noexcept
	{
		return &m_commandList;
	}

	void GraphicsCommandList1::ResolveSubresource(
		ID3D12Resource* destinationSource,
		RenderTargetDepthStencilGeneric& sourceRenderTarget
	)
	{
		m_commandList->ResolveSubresource(
			destinationSource,
			0,
			sourceRenderTarget.GetRtResource(),
			0,
			sourceRenderTarget.GetRtClearValue().Format
		);
	}
}