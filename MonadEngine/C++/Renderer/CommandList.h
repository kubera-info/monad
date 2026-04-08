// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#pragma once

// Platform
#include <wrl.h>
// Monad
#include "System/__MonadSystem.h"
#include "__MonadRenderer.h"
#include "Targets.h"

namespace Monad::Renderer
{
	struct GraphicsCommandList1
	{
		struct ResourceBarrier : D3D12_RESOURCE_BARRIER
		{
			ResourceBarrier(
				ID3D12Resource* resource,
				D3D12_RESOURCE_STATES stateBefore,
				D3D12_RESOURCE_STATES stateAfter
			) noexcept;
			ResourceBarrier(
				const PtrResource& resource,
				D3D12_RESOURCE_STATES stateBefore,
				D3D12_RESOURCE_STATES stateAfter
			) noexcept;
		};
		using InitializerListResourceBarrier = std::initializer_list<ResourceBarrier>;

		struct ScopedBarrier final
		{
			ScopedBarrier(
				GraphicsCommandList1& commandList,
				InitializerListResourceBarrier resourceBarriers
			) noexcept;
			ScopedBarrier(ScopedBarrier&&) = default;
			ScopedBarrier& operator=(ScopedBarrier&&) = default;
			ScopedBarrier(ScopedBarrier const&) = delete;
			ScopedBarrier& operator=(ScopedBarrier const&) = delete;
			~ScopedBarrier();

			GraphicsCommandList1& m_commandList;
			std::vector<ResourceBarrier> m_resourceBarriers;
		};

		void Transition(
			InitializerListResourceBarrier resourceBarriers
		) const;
		[[nodiscard]]
		ScopedBarrier ScopedTransition(
			InitializerListResourceBarrier resourceBarriers
		) noexcept;
		ID3D12GraphicsCommandList1* Get() const noexcept;
		ID3D12GraphicsCommandList1** operator&() noexcept;

		void WaitForCompletionEvent(
			uint64_t value
		);
		void SetMe(
			ID3D12PipelineState* pipelineState,
			ID3D12RootSignature* rootSignature
		) noexcept;

		void ResolveSubresource(
			ID3D12Resource* destinationSource,
			RenderTargetDepthStencilGeneric& SourceRenderTarget
		);

	private:
		struct SmartHandleFence final : System::SmartHandle
		{
			SmartHandleFence();
			OPER_DEL_NO_DEF_CTOR(SmartHandleFence);
		} const c_fenceEvent;

	public:
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList1> m_commandList;
	};
}